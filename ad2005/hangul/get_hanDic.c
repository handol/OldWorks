
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
/* �ѱ� ó�� �ÿ� isspace () ����ϸ� ������ �Ǵ� ������ ����. */
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

// �� ���ο� �ѱۿ� ���ڰ� ������ return 0
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
int no_wordElement; // �� ������ �и��� �� ���� 
char wordElement[MAX_ELEMENT][64]; // ���¼� �м��� �ܾ� (�, ����, ��� �� ) 
char wordPoS[MAX_ELEMENT][5]; // ǰ�� 

/*----- ���� 
513000070	������	������/np
513000080	�ȳ��Ͻʴϱ�?	�ȳ�/nc+��/xsm+��/ep+���ϱ�/ef+?/s
513000090	KBS	KBS/f
513000100	9��	9/nn+��/nb
513000110	�����Դϴ�.	����/nc+��/co+���ϴ�/ef+./s

============================
3.2 ǰ�� �з� ���̺�
����1	����2	����3
1. s(��ȣ)		
2. f(�ܱ���)		
3. n(���)	3.1 nc(�ڸ����)	
	3.2 nb(�������)	
4. np(����)		
5. nn(����)		
6. pv(����)		
7. pa(�����)		
8. px(�������)		
9. co(������)		
10. ma(�λ�)	10.1 mag(�Ϲݺλ�)	
	10.2 maj(���Ӻλ�)	
11. mm(������)		
12. ii(��ź��)		
13. x(����)	13.1 xp(���λ�)	
	13.2 xs(���̻�)	13.2.1 xsn(��� �Ļ� ���̻�)
		13.2.2 xsv(���� �Ļ� ���̻�)
		13.2.3 xsm(����� �Ļ� ���̻�)
14. j(����)	14.1 jc(������)	
	14.2 jx(������)	
	14.3 jj(��������)	
	14.4 jm(�Ӱ�����)	
15. ep(������)		
16. e(����)	16.1 ef(������)	
	16.2 ec(������)	
	16.3 et(�������)	16.3.1 etn(��������)
		16.3.2 etm(���������)
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
		if (*ptr == ';') continue; // ȭ�� ��� �κ� 
		
		ptr = (byte*)skip_nonspace((char*)ptr); // ù �ʵ��� ���ڸ� skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // �ѱ��� �ƴ� ���ڷ� �����ϸ� ó�� ���� 
		ch = (int)*ptr << 8;		
		ch |= (int) ptr[1];
		
		if (ch  < 0xB0A1 || ch > 0xC8FE) continue;

		ptr = (byte*)skip_nonspace((char*)ptr); // �� ������ skip
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

			if (wordPoS[i][0] == 'j' // ���� 
				|| wordPoS[i][0] == 'e' // ��� 
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


// ���½� ���� (HAM) �ڷ� �� wordList
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

// ���½� ���� (HAM) �ڷ� �� �󵵼� �ڷ�
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
		
		ptr = (byte*)skip_nonspace((char*)ptr); // ù �ʵ��� ���ڸ� skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // �ѱ��� �ƴ� ���ڷ� �����ϸ� ó�� ���� 
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
		
		ptr = (byte*)skip_nonspace((char*)ptr); // ù �ʵ��� ���ڸ� skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // �ѱ��� �ƴ� ���ڷ� �����ϸ� ó�� ���� 
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
		
		ptr = (byte*)skip_nonspace((char*)ptr); // ù �ʵ��� ���ڸ� skip
		ptr = (byte*)skip_space((char*)ptr);

		if (*ptr == 0) continue;
		if ((*ptr & 0x80)==0) continue; // �ѱ��� �ƴ� ���ڷ� �����ϸ� ó�� ���� 
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

// �ѱ� ������� -- PDA ����Ʈ���� ���Ѱ� 
// ���� ����: �ܾ� ���� �� ���� [ǰ��]
/*
�� (3)  	��3[����] 1.�������� ���� ü�� �ٴ� ������. ���ո��� �־��� �ڰ��� ������ �ϴ� �ְ� ����. �ҾƱⰡ ���´�. �������Ǵ١��� �ڵ����� �Ͽ��� �������� ���Ͽ� �װ����� �ʡ��� ���ϴ� ���� ����. �Ұ����� �� �ٴٰ� �ȴ�. �������ƴϴ١��� �ڵ����� �Ͽ��� �� ���� ������ ������� ���ϴ� ���� ����. �Ұ��� ����Ⱑ �ƴϴ�. 2.�Ϻ� ��̿� �پ�, �� ���� ���� �����ϴ� ������. �Ҿ�¾�� ó������ ��������� �ʴ����. (����)��6.
����  	������(ʣʫ)[���] 1.�ӽ÷� ���� ��. ���ǹ�(ʣ��ڪ). 2.�� ������.	����  	������(ʦʧ)[���] ������ �븩�ӡ��� ������, ���� ���� �ۿ��� ���� ��.
�������  	�����������(ʧʧ����)[���][�ϴ��� �ڵ���] �����Ÿ��� �ѹ��� ũ�� ����.
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
	// ���� ������ ���� 
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
		
		if (strncmp(ptr+1, "���", 4)==0) return NOUN;
		if (strncmp(ptr+1, "�ڵ���", 6)==0) return VERB;
		if (strncmp(ptr+1, "Ÿ����", 6)==0) return VERB;
		if (strncmp(ptr+1, "�����", 6)==0) return ADJ;
		if (strncmp(ptr+1, "�λ�", 4)==0) return ADV;
		if (strncmp(ptr+1, "������", 6)==0) return GWANHYUNG;
		if (strncmp(ptr+1, "��ź��", 6)==0) return EXCL;

		ptr += 2;
	}
	return 0;
}

// �ϴ�,  ǥ����, ǰ�� ���� ���� 
#define MAX_HW_LEN	(32)
typedef struct _korDict {
	byte headw[MAX_HW_LEN+1];
	byte	num; // (1), (2) ������ ���� ö���� ǥ��� ���� ���� ��  ��ȣ 
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
	// ǰ�� �κ�: ǰ�� ������ ����
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
		if (*ptr == ';') continue; // ȭ�� ��� �κ�
		if ((*ptr & 0x80)==0) continue; // �ѱ��� �ƴ� ���ڷ� �����ϸ� ó�� ���� 

		// �ѱ� �ڵ� �ƴ� ���� ���� :: �ڵ� ���� : ������(0xAC00) ~ ���R��(0xD7A3)
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

// ������� ȭ�� �б�
	sum_inserted += read_unique_noun_file("/home/anydict/work_OLD/KoreanDic/WORD/unique_name_freq.txt" );
	
// �ܾ� ȭ�� �б�
	sum_inserted += read_word_file("/home/anydict/work_OLD/KoreanDic/WORD/word_freq.txt" );

// ���ȭ�� �б� 
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

