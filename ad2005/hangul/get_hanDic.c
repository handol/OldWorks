
#include <stdio.h>
#include <ctype.h>
#include "hanDic_hash.h"
#include "str_util.h"
#include "str_conv.h"
#include "mem_mng.h"
#include "CLog.h"

#ifdef AD_SHM
#define ALLOC SALLOC
#define strALLOC str_SHMalloc
#define strALLOC_n str_SHMalloc_n
#else

#define ALLOC GALLOC
#define strALLOC str_GALLOC
#define strALLOC_n str_GALLOC_n
#endif

typedef unsigned char byte;

#ifdef LOG
#include "CLog.h"
extern	CLog *gLog;
#define	PRN		gLog->LogPRN
#else
#define	PRN		printf
#endif

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

/*
byte *skip_non_hangul(byte *line)
{
	while(*line && (*line & 0x80)==0 ) line++;
	return line;
}

char *skip_nonspace(char *line)
{
	while(*line && ! ISSPACE(*line)) line++;
	return line;
}
*/

CHanDicHash *hdicHash=0;
int	hdicSize=0;
int init_hdic_hash()
{
	hdicHash = new CHanDicHash();
	hdicSize = hdicHash->init_hash_table(150000);
	return hdicSize;
}

// 한 라인에 한글외 문자가 있으면 return 0
int get_hangul_only(byte *line)
{
	int	twobyte=0;
	while (*line) {
		if (*line=='\r' || *line=='\n') {
			*line = 0;
			return 1;
		}
		if ((*line) & 0x80) {
			twobyte = *line << 8;
			line++;
			twobyte |= *line;
			
			if (twobyte < 0xB0A1 || twobyte > 0xC8FE) {
				*line = 0;
				return 0;
			}
		}
		
		line++;
	}
	return 1;	
}

int take_hangul_only(byte *line, byte *hword)
{
	int	twobyte=0;
	int	n_hanchars=0;
	while (*line) {
		if (*line=='\r' || *line=='\n') {
			break;
		}
		if ((*line) & 0x80) {
			//twobyte = *line << 8;
			*hword++ = *line++;
			//twobyte |= *line;
			n_hanchars++;
					/*
			if (twobyte < 0xB0A1 || twobyte > 0xC8FE) {
				*line = 0;
				return 0;
			}
					*/
		}
		else
			line++;
	}

	*hword = 0;
	return n_hanchars;	
}


int take_ONLY_hangul(byte *line, byte *hword)
{
	int	twobyte=0;
	int	n_hanchars=0;
	while (*line & 0x80) {
		#ifdef HANGUL_ONLY
		if (twobyte==0) 
			twobyte = *line << 8;
		else {
			twobyte |= *line;
			if (twobyte < 0xB0A1 || twobyte > 0xC8FE) {
				n_hanchars--;
				*(hword-1) = 0;
				break;
			}
			twobyte = 0;			
		}
		#endif

		n_hanchars++;
		*hword++ = *line++;
	}

	*hword = 0;
	return n_hanchars;	
}

#define MAX_ELEMENT	(10)
int no_wordElement; // 한 어절이 분리된 총 갯수 
char wordElement[MAX_ELEMENT][64]; // 형태소 분석된 단어 (어간, 조사, 어미 등 ) 
char wordPoS[MAX_ELEMENT][5]; // 품사 

/*----- 예시 
513000070	여러분	여러분/np
513000080	안녕하십니까?	안녕/nc+하/xsm+시/ep+ㅂ니까/ef+?/s
513000090	KBS	KBS/f
513000100	9시	9/nn+시/nb
513000110	뉴스입니다.	뉴스/nc+이/co+ㅂ니다/ef+./s

============================
3.2 품사 분류 테이블
계층1	계층2	계층3
1. s(기호)		
2. f(외국어)		
3. n(명사)	3.1 nc(자립명사)	
	3.2 nb(의존명사)	
4. np(대명사)		
5. nn(수사)		
6. pv(동사)		
7. pa(형용사)		
8. px(보조용언)		
9. co(지정사)		
10. ma(부사)	10.1 mag(일반부사)	
	10.2 maj(접속부사)	
11. mm(관형사)		
12. ii(감탄사)		
13. x(접사)	13.1 xp(접두사)	
	13.2 xs(접미사)	13.2.1 xsn(명사 파생 접미사)
		13.2.2 xsv(동사 파생 접미사)
		13.2.3 xsm(형용사 파생 접미사)
14. j(조사)	14.1 jc(격조사)	
	14.2 jx(보조사)	
	14.3 jj(접속조사)	
	14.4 jm(속격조사)	
15. ep(선어말어미)		
16. e(어말어미)	16.1 ef(종결어미)	
	16.2 ec(연결어미)	
	16.3 et(전성어미)	16.3.1 etn(명사형어미)
		16.3.2 etm(관형형어미)
================================
------*/
int parse_ETRI_line(char *line)
{
	no_wordElement=0;
	while (*line && *line != '\n') {
		line += copy_to_char(wordElement[no_wordElement], line, 40, '/');
		line++;
		line += copy_alphabet(wordPoS[no_wordElement], line, 4);
		if ( !isalnum(wordElement[no_wordElement][0]) &&
			wordPoS[no_wordElement][0] != 's' &&
			wordPoS[no_wordElement][0] != 'f')
			no_wordElement++;
		
		if (*line!='+') break;
		else line++;
	}
	return no_wordElement;
}

int read_ETRI_hdic_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128];
	//char hanword[64];
	byte *ptr;
	int ch, i;	
	HanDicType *handicData, tmpdicData;
	int zero_len=0;
	int new_handicwords=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	while (fgets(buf, 128, fp) != NULL) {
		ptr = (byte*)skip_space(buf);
		if (*ptr == ';') continue; // 화일 헤더 부분 
		
		ptr = (byte*)skip_nonspace((char*)ptr); // 첫 필드의 숫자를 skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // 한글이 아닌 문자로 시작하면 처리 안함 
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];
		
		if (ch  < 0xB0A1 || ch > 0xC8FE) continue;

		ptr = (byte*)skip_nonspace((char*)ptr); // 원 어절을 skip
		ptr = (byte*)skip_space((char*)ptr);

		parse_ETRI_line((char*)ptr);

		tmpdicData.id = tmpdicData.freq = 0;
		
		for(i=0; i<no_wordElement; i++) {
			/*
			if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
				nohangul++;
				continue;
			}		
			handicData = hdicHash->hash_search(hanword);
			*/

			if (wordPoS[i][0] == 'j' // 조사 
				|| wordPoS[i][0] == 'e' // 어미 
			  )
			  continue;

		
			tmpdicData.word = wordElement[i];
			strcpy(tmpdicData.PoS, wordPoS[i]);
			
			handicData = hdicHash->hash_search_data(&tmpdicData);
			if (handicData==0) {
				handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
				handicData->word = strALLOC(wordElement[i]);
				strcpy(handicData->PoS, wordPoS[i]);
				hdicHash->hash_insert(handicData);
				new_handicwords++;
			}
			
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif

			cnt++;
		}
		
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);

	return new_handicwords;
}


// 강승식 교수 (HAM) 자료 중 wordList
int read_HAM_hdic_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	byte *ptr;
	int ch;	
	HanDicType *handicData;
	int	new_handicwords=0;
	int zero_len=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	while (fgets(buf, 128, fp) != NULL) {
		ptr = skip_non_hangul((byte*)buf);
		if (*ptr==0 || *ptr=='#') continue;
		
		if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}
		cnt++;

	
		handicData = hdicHash->hash_search(hanword);
		if (handicData==0) {
			handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
			handicData->word = strALLOC(hanword);
			hdicHash->hash_insert(handicData);
			new_handicwords++;
		}
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);


	return new_handicwords;
}

// 강승식 교수 (HAM) 자료 중 빈도수 자료
int read_FREQ_words_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	byte *ptr;
	int ch;	
	HanDicType *handicData;
	int zero_len=0;
	int freq_rank=0;
	int new_handicwords=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);

	while (fgets(buf, 128, fp) != NULL) {
		ptr = skip_non_hangul((byte*)buf);
		if (*ptr==0 || *ptr=='#') continue;
		
		if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}
		cnt++;

	
		handicData = hdicHash->hash_search(hanword);
		if (handicData==0) {
			handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
			handicData->word = strALLOC(hanword);
			handicData->freq = 1;
			hdicHash->hash_insert(handicData);
			new_handicwords++;
		}
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);

	
	return new_handicwords;
}


typedef struct _etriFiles{
	char *name;
	int nums;
} etriFiles;

etriFiles EF[7]= {
	{ "essay", 1},
	{ "expl", 40},
	{ "news", 25},
	{ "newsp", 16},
	{ "novel", 25},
	{ "record", 2},
	{ "study", 2}
};


int read_ALL_etri_file()
{
	char fname[256];
	int	i, j;
	int sum_inserted=0;

	for(i=0; i<7; i++) {
		for(j=1; j<=EF[i].nums; j++) {
			sprintf(fname, "/home/anydict/work_OLD/KoreanDic/ETRI/%s%02d.txt",
				EF[i].name, j);
			sum_inserted += read_ETRI_hdic_file(fname);
		}
	}
	printf("Sum_inserted (ETRI) : %d\n", sum_inserted);
	
	return sum_inserted;
}



int read_eomi_file( char *fname )
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	byte *ptr;
	int ch;	
	HanDicType *handicData;
	int zero_len=0;
	int new_handicwords=0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	
	printf("file: %s\n", fname);
	fgets(buf, 128, fp);
	while (fgets(buf, 128, fp) != NULL) {
		ptr = (byte*)skip_space(buf);
		
		ptr = (byte*)skip_nonspace((char*)ptr); // 첫 필드의 숫자를 skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // 한글이 아닌 문자로 시작하면 처리 안함 
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];
		
		if (ch  < 0xB0A1 || ch > 0xC8FE) continue;

		if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}

		cnt++;

		handicData = hdicHash->hash_search(hanword);
		if (handicData==0) {
			handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
			handicData->word = strALLOC(hanword);
			hdicHash->hash_insert(handicData);
			new_handicwords++;
		}
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);
	return ( new_handicwords );
}

int read_word_file( char *fname )
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	byte *ptr;
	int ch;	
	HanDicType *handicData;
	int zero_len=0;
	int new_handicwords=0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	fgets(buf, 128, fp);
	while (fgets(buf, 128, fp) != NULL) {
		ptr = (byte*)skip_space(buf);
		
		ptr = (byte*)skip_nonspace((char*)ptr); // 첫 필드의 숫자를 skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // 한글이 아닌 문자로 시작하면 처리 안함 
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];
		
		if (ch  < 0xB0A1 || ch > 0xC8FE) continue;

		if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}

		cnt++;

		handicData = hdicHash->hash_search(hanword);
		if (handicData==0) {
			handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
			handicData->word = strALLOC(hanword);
			hdicHash->hash_insert(handicData);
			new_handicwords++;
		}
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);
	return ( new_handicwords );
}





int read_unique_noun_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	byte *ptr;
	int ch;	
	HanDicType *handicData;
	int zero_len=0;
	int new_handicwords=0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	fgets(buf, 128, fp);
	while (fgets(buf, 128, fp) != NULL) {
		ptr = (byte*)skip_space(buf);
		
		ptr = (byte*)skip_nonspace((char*)ptr); // 첫 필드의 숫자를 skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // 한글이 아닌 문자로 시작하면 처리 안함 
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];
		
		if (ch  < 0xB0A1 || ch > 0xC8FE) continue;

		if (take_ONLY_hangul((byte*)ptr, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			//printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}

		cnt++;

	
		handicData = hdicHash->hash_search(hanword);
		if (handicData==0) {
			handicData = (HanDicType*)ALLOC(sizeof(HanDicType));
			handicData->word = strALLOC(hanword);
			hdicHash->hash_insert(handicData);
			new_handicwords++;
		}
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);
	return ( new_handicwords );
}

int read_ALL_ham_file()
{
	char fname[256];
	int	i, j;
	int sum_inserted=0;

	for(i=0; i<5; i++) {		
		sprintf(fname, "/home/anydict/work_OLD/KoreanDic/dict4morph/WordList/%d.txt",
			i);
		sum_inserted += read_HAM_hdic_file(fname);
		
	}
	printf("Sum_inserted (HAM WordList) : %d\n", sum_inserted);
	
	return sum_inserted;
}

// 한글 국어사전 -- PDA 사이트에서 구한것 
// 라인 형식: 단어 공백 탭 발음 [품사]
/*
가 (3)  	가3[조사] 1.모음으로 끝난 체언에 붙는 격조사. ㉠앞말이 주어의 자격을 가지게 하는 주격 조사. ¶아기가 웃는다. ㉡《‘되다’를 뒤따르게 하여》 ‘무엇이 변하여 그것으로 됨’을 뜻하는 보격 조사. ¶강물이 모여 바다가 된다. ㉢《‘아니다’를 뒤따르게 하여》 그 말이 부정의 대상임을 뜻하는 보격 조사. ¶고래는 물고기가 아니다. 2.일부 어미에 붙어, 그 말의 뜻을 강조하는 보조사. ¶어쩐지 처음부터 예사롭지가 않더라니. (참고)이6.
가가  	가ː가(假家)[명사] 1.임시로 지은 집. 가건물(假建物). 2.의 본딧말.	가가  	가ː가(可呵)[명사] ‘웃을 노릇임’의 뜻으로, 흔히 편지 글에서 쓰는 말.
가가대소  	가ː가―대소(呵呵大笑)[명사][하다형 자동사] 껄껄거리며 한바탕 크게 웃음.
*/
#define	NOUN	(0x01)
#define	PRON	(0x02) // pronoun
#define	ADJ	(0x04) // adjective
#define	ADV	(0x08)// adverb
#define	VERB	(0x10)
#define	GWANHYUNG	(0x20) // preposition
#define	EXCL	(0x40) // conjunction
#define	INTJ	(0x80) // inerjection
int get_hangul_headword(byte *src, char *headword)
{
	while (*src) {
		if (*src=='(' || *src=='\t') break;
		*headword++ = *src++;	
	}

	*headword = 0;
	// 뒤의 공백은 제거 
	while (*headword==' ') headword--;
	*++headword=0;
	return 1;
}

int get_hangul_PoS(byte *src, int len)
{
	char *ptr;
	int	n=0;
	ptr = (char *)src;
	
	while(n < len) {
		ptr = strchr((char*)ptr, '[');
		if (ptr==0) return 0;

		if (ptr[1]=='-') {
			ptr += 2;
			continue;
		}
		
		if (strncmp(ptr+1, "명사", 4)==0) return NOUN;
		if (strncmp(ptr+1, "자동사", 6)==0) return VERB;
		if (strncmp(ptr+1, "타동사", 6)==0) return VERB;
		if (strncmp(ptr+1, "형용사", 6)==0) return ADJ;
		if (strncmp(ptr+1, "부사", 4)==0) return ADV;
		if (strncmp(ptr+1, "관형사", 6)==0) return GWANHYUNG;
		if (strncmp(ptr+1, "감탄사", 6)==0) return EXCL;

		ptr += 2;
	}
	return 0;
}

// 일단,  표제어, 품사 정보 추출 
#define MAX_HW_LEN	(32)
typedef struct _korDict {
	byte headw[MAX_HW_LEN+1];
	byte	num; // (1), (2) 등으로 같은 철자의 표제어가 여러 개일 때  번호 
	byte no_pum;
	byte	pumsa[5][MAX_HW_LEN+1];
} korDict;

int	extract_headword(char *line, korDict *d)
{
	byte	buf[128], buf2[128];
	char *tabptr, *ptr;
	int	i;
	
	memset(d, 0, sizeof(korDict));
	tabptr = strchr((char*)line, '\t');
	if (tabptr==0) return -1;
	*tabptr = 0;

	ptr = strchr((char*)line, '(');
	if (ptr) {
		d->num = ptr[1] - '0';
		ptr--;
	}
	else {
		ptr = tabptr-1;
	}
	
	while(ptr>line && ISSPACE(*ptr))
		ptr--;
	ptr[1] = 0;
	
	strcpy((char*)d->headw, line);
	// 품사 부분: 품사 여러개 가능
	for(i=0, ptr = tabptr+1; i<5; ) {
		int	n=0;
		ptr = strchr(ptr, '[');
		if (ptr==0) break;
		if (	((byte)ptr[1]==0xA1 && (byte)ptr[2]==0xAA) 
				|| ptr[1]=='-' 
				|| ptr[1]=='~' 
				|| ISSPACE(ptr[1])) 
		{
			ptr += 2;
			continue;
		}
		n = copy_to_char((char*)d->pumsa[i], (char*)(ptr+1), MAX_HW_LEN, ']');		
		ptr += n+2;
		if (n>16) continue;
		//printf("%d %d %d %d\n", *(ptr-2), *(ptr-1), *(ptr-0), ']');
		if ( (byte)*(ptr-2)==0xA1 && (byte)*(ptr-1)==0xAA ) continue;
		//if (strchr((char*)d->pumsa[i], '|')) continue;
		i++;
	}
	d->no_pum = i;
	return i;
}

int read_KoreanDic_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[2048+1];
	//char hanword[64];
	byte *ptr;
	int ch, i;	
	HanDicType *handicData, tmpdicData;
	korDict	data;
	int zero_len=0;
	int new_handicwords=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	while (fgets(buf, 2048, fp) != NULL && cnt++<1000000) {
		ptr = (byte*)skip_space(buf);
		if (*ptr == ';') continue; // 화일 헤더 부분
		if ((*ptr & 0x80)==0) continue; // 한글이 아닌 문자로 시작하면 처리 안함 

		// 한글 코드 아닌 것은 제외 :: 코드 범위 : ‘가’(0xAC00) ~ ‘힣’(0xD7A3)
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];		
		//if (ch  < 0xB0A1 || ch > 0xC8FE) continue;
		

		//printf("%s\n", ptr);
		if (extract_headword((char*)ptr, &data) < 0) {
			printf("\t---\n");
			continue;
		}
		printf("%s", data.headw);
		if (data.num) printf("\t%d\t%d", data.num, data.no_pum);
		else printf("\t\t%d", data.no_pum);
		for(i=0; i<data.no_pum; i++) {
			printf("\t%s", data.pumsa[i]);
		}
		printf("\n");
		
		
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d, Inserted=%d\n", cnt, nohangul, new_handicwords);

	return new_handicwords;
}

int read_other_dict_file()
{
	int sum_inserted=0;

	prepare_my_mem_alloc(1024*1024, 50);
	init_hdic_hash();

	
	sum_inserted += read_ALL_etri_file();
	hdicHash->prn_hash_stat();

	
	sum_inserted += read_ALL_ham_file();
	hdicHash->prn_hash_stat();
	
	sum_inserted += read_FREQ_words_file("/home/anydict/work_OLD/KoreanDic/dict4morph/wf2000.txt");
	

	sum_inserted += read_ETRI_hdic_file("/home/anydict/work_OLD/KoreanDic/ETRI/news01.txt");	
	
	hdicHash->prn_hash_stat();

	hdicHash->sort_slots();
	hdicHash->prn_sorted_slots();

// 고유명사 화일 읽기
	sum_inserted += read_unique_noun_file("/home/anydict/work_OLD/KoreanDic/WORD/unique_name_freq.txt" );
	
// 단어 화일 읽기
	sum_inserted += read_word_file("/home/anydict/work_OLD/KoreanDic/WORD/word_freq.txt" );

// 어미화일 읽기 
	sum_inserted += read_eomi_file("/home/anydict/work_OLD/KoreanDic/WORD/word_freq.txt" );

	printf("Sum_inserted (ALL) : %d\n", sum_inserted);
	return sum_inserted;
}

CLog *gLog;
int main(int argc, char *argv[])
{
	int shm;
	int sum_inserted=0;

	#ifdef LOG
	//gLog = new CLog(anydict_log_dir, argv[0]);
	gLog = new CLog(".", argv[0]);
	#else
	init_Log(anydict_log_dir, argv[0]);
	#endif	

	PRN("=====>\n");

	// Korean-Korean Dict :: from PDA
	read_KoreanDic_file("/home/anydict/work_OLD/KoreanDic/Korean.txt");
	

	PRN("<=====\n\n");
	
}

