#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#include "DbConn.h"
//#include "DbQuery.h"

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "Config.h"

#include "HDict.h"
#include "HGram.h"
#include "dbStore.h"

#define	PRN	printf

struct _nameValPair HDict::conjMapTable[] = {

 { "ㄷ", D_conj },
 { "ㄹ", L_conj },
 { "ㅂ", B_conj },
 { "ㅅ", S_conj },
 { "ㅎ", H_conj },
 { "거라", Go_conj },
 { "너라", Come_conj },
 { "러라", Leora_conj },
 { "러", Leo_conj },
 { "르", Reu_conj },
 { "여", Yeo_conj },
 { "우", U_conj },
 { "으", Eu_conj }
};
 

struct _nameValPair HDict::psMapTable[] = {

 { "감탄사", HAN_EXCL },
 { "관형사", HAN_ADJ_KOR },
 { "구", HAN_PHRASE },
 { "대명사", HAN_NOUN | HAN_PRONOUN },
 { "동사", HAN_VERB },
 { "명사", HAN_NOUN },
 { "명사사", HAN_NOUN }, //  명사 의 오타로 보임.
 { "보조동사", HAN_VERB |  HAN_AUX},
 { "보조형용사", HAN_VERB | HAN_AUX },
 { "부사", HAN_ADVB },
 { "불완전자동사", HAN_VERB | HAN_INCOMPLETE },
 { "불완전타동사", HAN_VERB | HAN_INCOMPLETE | HAN_VT },
 { "수사", HAN_NOUN | HAN_NUM  },
 { "어미", HAN_EOMI },
 { "의존명사", HAN_NOUN |HAN_DEP  },
 { "자동사", HAN_VERB },
 { "접두사", HAN_PFIX },
 { "접미사", HAN_SFIX },
 { "조사", HAN_JOSA },
 { "준말", 000 },
 { "타동사", HAN_VERB | HAN_VT },
 { "한의학", HAN_NOUN },
 { "형사", HAN_ADJ },  //  형용사 의 오타로 보임.
 { "형용사", HAN_ADJ }

};

simpleHash<HwordType>  HDict::hashTab;
simpleAlloc HDict::mymem(100);

HwordType **HDict::WordArray=0;
int HDict::wordNums = 0;
int HDict::maxWordId = 0;
int	HDict::maxSize = 0;

/**
국어 사전에 표시된 품사 이름을 int 값으로 전환.
*/
int	HDict::CONJname_to_CONJval(char *CONJname)
{
	int	cmp;
	for(int i=0; i< sizeof(conjMapTable)/sizeof (struct _nameValPair); i++) {
		cmp = strncmp(CONJname, conjMapTable[i].name, 2);
		if (cmp==0) return conjMapTable[i].value;
		if (cmp < 0) break;
	}

	return 0;
}

/**
국어 사전에 표시된 품사 이름을 int 값으로 전환.
*/
int	HDict::PSname_to_PSval(char *PSname)
{
	int	cmp;
	for(int i=0; i< sizeof(psMapTable)/sizeof (struct _nameValPair); i++) {
		cmp = strcmp(PSname, psMapTable[i].name);
		if (cmp==0) return psMapTable[i].value;
		if (cmp < 0) break;
	}

	return 0;
}

void HDict::test_nameValPairTable()
{
	int	res;
	for(int i=0; i< sizeof(psMapTable)/sizeof (struct _nameValPair); i++) {
		res = PSname_to_PSval ( psMapTable[i].name );
		printf("%s %X %X\n", psMapTable[i].name, res, psMapTable[i].value);
	}
}


/**
사전의 본문에서 단어의 품사, 불규칙 활용 정보 등을 구한다.
*/
int	HDict::proc_YBMKorDict_grammar(HwordType &hword, char *dict_body, int body_len)
{
	char *PSptr=0;
	char *CONJptr=0;
	char	PSname[24];
	char	CONJname[24];
	int	len1=0, len2=0;
	char *body_ptr = dict_body;
	
	

	while ( (PSptr = strstr(body_ptr, "\\f3<")) && len1 < body_len) {
		int	copyed = StrUtil::copy_to_char(PSname, PSptr+4, sizeof(PSname)-1, '>');
		//printf("%s ", PSname);
		
		hword.PS |= PSname_to_PSval(PSname);
		body_ptr += (4+copyed+1);
		len1 += (4+copyed+1);
	}

	CONJptr = strstr(dict_body, "\\f4<");
	if (CONJptr) {
		StrUtil::copy_to_char(CONJname, CONJptr+4, sizeof(CONJname)-1, '>');
		char *ptr = strstr(CONJname, "불규칙");
		if (ptr) *ptr = 0;
		//printf("%s " , CONJname);
		hword.Conj = CONJname_to_CONJval(CONJname);
	}
	//printf("\n");
	return 0;
}

/**
'-' 마크로 음절, 어절 분리된 정보를 기록하기 위한 것이다.

69||가계도||가계-도||
72||가계소득||가계-소득||
*/
void	HDict::proc_YBMKorDict_split(HwordType &hword, char *split_word, int split_word_len)
{
	int	len = 0;
	int	cnt = 0;
	char *ptr;
	while(len < split_word_len) {
		int	step=0;
		ptr = strchr(split_word, '-');
		if (ptr) {
			step = ptr - split_word; // 분리된 단어의 길이.
			hword.split[cnt] = (step >> 1);
			split_word += (step+1);
			len += (step+1);
		}
		else
			break;
			
		cnt++;
	}
	
	for (int i=cnt; i<4; i++)
		hword.split[i] = 0;
	if (cnt==0) {
		hword.split[0] = (hword.wlen >> 1);
	}
}

void	HDict::proc_YBMKorDict_type(HwordType &hword)
{
	if (StrUtil::str_ends_with(hword.key, "스럽다", hword.wlen,  6) ) {
		hword.property = HType_Like;
	}
	else if (StrUtil::str_ends_with(hword.key, "하다", hword.wlen,  4) ) {
		hword.property = HType_Hada;
	}
	else if (StrUtil::str_ends_with(hword.key, "다", hword.wlen, 2) ) {
		hword.property = HType_Da;
	}
}

void	HDict::printWord_stream(StrStream &stream, HwordType *h)
{
	//printf("%s %08X %d %d %5d | ", h->key, h->PS, h->Conj, h->property, h->id);
	stream.add("%s %08X %d %d", h->key, h->PS, h->Conj, h->property);
	stream.add(" %d", h->split[0]);
	for (int i=1; ; i++) {
		if (h->split[i]==0) break;
		stream.add(" %d", h->split[i]);
	}
	stream.add("\n");
}

void	HDict::printWord(HwordType *h)
{
	//printf("%s %08X %d %d %5d | ", h->key, h->PS, h->Conj, h->property, h->id);
	printf("%s %08X %d %d", h->key, h->PS, h->Conj, h->property);
	printf(" %d", h->split[0]);
	for (int i=1; ; i++) {
		if (h->split[i]==0) break;
		printf(" %d", h->split[i]);
	}
	printf("\n");
}

void	HDict::printList()
{
	printf("=== list ===\n");
	for(int	i=0; i<wordNums; i++) {
		printf("%-3d ", i+1);
		printWord( WordArray[i] );
	}
}

void	HDict::printListAuxOnly()
{
	int	cnt=0;
	printf("#=== 보조 용언 \n");
	for(int	i=0; i<wordNums; i++)
		if ( (WordArray[i]->PS & HAN_AUX) ) {
			cnt++;
			printWord( WordArray[i] );
			
		}
	printf("# Count = %d\n", cnt);
}

void	HDict::printListConjOnly()
{
	int	sum=0;
	for(int cj=D_conj; cj<=Eu_conj; cj++) {
		int	cnt=0;
		printf("#=== 불규칙 활용 :  %d\n", cj);
		for(int	i=0; i<wordNums; i++) {
			if (WordArray[i]->Conj==cj) {
				cnt++;
				sum++;
				printWord( WordArray[i] );
			}
		}
		printf("# Count = %d\n", cnt);
	}
	printf("## Sum = %d\n", sum);
}


void	HDict::printListEomiOnly()
{
	int	cnt=0;
	printf("## 어미\n");
	for(int	i=0; i<wordNums; i++)
		if (WordArray[i]->PS & HAN_EOMI) {
			cnt++;
			printWord( WordArray[i] );
		}
	printf("# Count = %d\n", cnt);

}

void	HDict::printListJosaOnly()
{
	int	cnt=0;
	printf("## 조사\n");
	for(int	i=0; i<wordNums; i++)
		if (WordArray[i]->PS & HAN_JOSA) {
			cnt++;
			printWord( WordArray[i] );
		}
	printf("# Count = %d\n", cnt);
}



/**

한글 단어가 이닌 것은 제외.

147251||힝힝||힝-힝||\f3<부사>\ff 잇달아 코를 세게 푸는 소리. <br>―― \f3<감탄사>\ff 잇달아 코웃음을 치는 소리.||||||
147252||\ig00나||?나||\f3<수사>\ff \f0<옛말>\ff 하나.||old||||ㄱ

147345||가살스럽다||가살-스럽다|
147371||독살하다||독살-하다|

6068||거슯즈다/거\ig0\ig0다||거슯즈다/거??다|
*/
int HDict::load_YBMKorDict(char *fname, int debug)
{

	FILE *fp;
	char buf[1024*2];
	
	int	cnt=0;
	int	dup_cnt=0;
	char	prevword[64] = {0};
	SylInfo	*wsInfoArry[20];
	int	wsLen=0;
	MemSplit	line(10, 256);

	HwordType	hword;
	if ( (fp=fopen(fname, "r")) == NULL) {
		ACE_DEBUG((LM_INFO, "Read Failed (Korean Dictionary) -- %s\n", fname));
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}

	PRINTF(("loading Hangul Dict: %s\n", fname));
	ACE_DEBUG((LM_INFO, "loading Hangul Dict: %s\n", fname));
	
	while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
		int wordlen=0;	
		char *presword=0; // 표제어 
		uint2 wscode=0;
		char *ptr = 0;
		
		int	ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		if (! isdigit(ch) ) continue;

		line.split(buf, '|');
		
		//if (cnt%20==0) line.print();
		if (line.size() < 7) continue;

		presword = line.str(2);
		/*
		국어사전에 완성형 코드로 표현이 안 되는 <옛말>이나 (제주) 방언이 포함되어 있어 ,
		제외하고 처리한다.
		
		1216||가\ig0다||가?다||\f3<자동사>\ff \f0<옛말>\ff
		334||가드락\ig00다||가드락-?다||\f3<형용사>\ff\f4<여불규칙활용>\ff \f9(제주)

		120192||짛다||짛다||\f3<타동사>\ff \f0<옛말>\ff
		*/
		
		/*
		wscode = STR_TO_UINT2(presword);
		if (! IS_WS_UMJUL(wscode) && ! IS_WS_JAMO(wscode) ) continue;
		*/
		
		if ( strstr(  presword, "ig") ) continue;
	
		ptr = strchr(  presword, '/');
		if (ptr) *ptr = 0;

		wordlen  = strlen( presword );

		/* <옛말>, (제주) 등의 이상한 코드값을 제거하기 위해 */
		wsLen = HanChar::getSylInfo(wsInfoArry, 20, presword);
		if ( (wsLen << 1) < wordlen) continue;

		memset(&hword, 0, sizeof(hword));
		hword.PS = 0;
		hword.Conj = 0;
		hword.property = 0;
		hword.id = line.intval(0);
		hword.wlen = wordlen;
		hword.key = (char *)mymem.strAlloc(presword, hword.wlen);	
		
		proc_YBMKorDict_grammar(hword, line.str(6), line.len(6) );

		proc_YBMKorDict_type(hword);

		proc_YBMKorDict_split(hword, line.str(4), line.len(4) );
		if (debug) printWord(&hword);

		if (hword.Conj) {
			if (hword.Conj == Reu_conj && hword.wlen >= 6) {
			/* 르 불규칙 : 어간의 마지막 음절 앞부분이 변화한다.  ~~르다. 
			빠르다   -- 빨라 			
			*/
				HanChar::markSyl_Irregular(hword.Conj, hword.key+hword.wlen-6);
			}
			else {
			/* 그외 불규칙 : 어간의 마지막 음절이 변화 한다.  ~~다. */ 
			/*
				if (hword.Conj==H_conj)
					printf("H = %s\n", hword.key);
			*/
				HanChar::markSyl_Irregular(hword.Conj, hword.key+hword.wlen-4);
			}
		}

		HwordType * resAdd = add(hword);
		
		if (resAdd==NULL) {
			if (strcmp(prevword, hword.key) != 0) {
				HwordType *old = hashTab.search(hword.key);
				if (old) {
					old->PS |= hword.PS;
				}
				/* 실행 결과 Odd hash Dup  가 많음.
					이유 : YBM 국어 사전에 단어의 순서가 sorting 이 안된 것들이 많음.
					사례  ) 가경지 가경
				*/
				//ACE_DEBUG((LM_DEBUG, "Odd hash Dup: %s %s\n", prevword, hword.key));
			}
			dup_cnt++;
		}
		
		strcpy( prevword, hword.key );
		cnt++;
		//if (cnt % 1000 == 0) PRN("%d\n", cnt);
	}

	ACE_DEBUG((LM_INFO, "Korean Words %d/%d (%d dup) \n", wordNums, cnt, dup_cnt));
	ACE_DEBUG((LM_INFO, "Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));

	PRINTF(("Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));
	
	fclose(fp);
	return 0;
}



/**
신규 단어 테이블 로드
*/
int HDict::load_newhanword()
{
	DbQuery	query;
	int	loadDbCnt = 0;	

	PRINTF(("load TABLE: newhanword\n"));
	ACE_DEBUG((LM_INFO, "load TABLE: newhanword\n"));
	
	query.exec(dbStore::dbConn, "select word, word_id from newhanword order by word_id");

	while (query.fetch()) {
		loadDbCnt++;

		StrUtil::trim_right(query.colStrVal(0) );
		
		HwordType hword;
		memset(&hword, 0, sizeof(hword));
		hword.PS = 0;
		hword.Conj = 0;
		hword.property = 0;
		hword.key = (char *)mymem.strAlloc(query.colStrVal(0));
		hword.id = query.colIntVal(1);
		hword.wlen = strlen(hword.key);
		
		add(hword);
	}

	ACE_DEBUG((LM_INFO,"# of words loaded from DB: %d\n", loadDbCnt));
	ACE_DEBUG((LM_INFO, "Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));

	PRINTF(("# of words loaded from DB: %d\n", loadDbCnt));
	PRINTF(("Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));
	return wordNums;
}

/*=== word list :: operations on wordArray ===*/
/**
각 단어별로, 그 단어로 시작되는 숙어목록을 sort한다.
*/
int HDict::cmpWordList(const void *Aptr, const void *Bptr)
{
	return strcmp ( (*(HwordType **)Aptr)->key , (*(HwordType **)Bptr)->key);
}

int HDict::sortWordList()
{
	qsort (WordArray, wordNums, sizeof(HwordType *), cmpWordList);
	return 0;
}

int HDict::alloc(int hashsize)
{
	hashsize = hashTab.init(hashsize) ;
	ACE_DEBUG((LM_INFO, "EDict: hash size=%d\n", hashsize ));

	if (WordArray==0) {
		maxSize = hashsize;
		WordArray = new HwordType * [maxSize];

		if (WordArray==0) return -1;
	}
	
	memset(WordArray, 0, sizeof(HwordType*) * maxSize);
	return hashsize;
}

int HDict::checkWordArray()
{
	/*
	한글 사전에 중복되는 표제어  단어가 많다. 그래서 NULL  이 많이 존재한다.
	*/
	int foundNull = 0;
	PRINTF(("maxSize=%d  maxWordId=%d\n", maxSize, maxWordId));
	for(int i=0; i<maxWordId; i++)
	{
		if (WordArray[i] == NULL )
		{
			//if (! foundNull)
			//	PRINTF(("Hangul WordArray[%d] = NULL\n", i));
			//foundNull = 1;
		}
		else	if ( ((unsigned int)WordArray[i]  & 0xCC000000) == 0xCC000000 )
		{
			if (! foundNull)
				PRINTF(("Hangul WordArray[%d] = %08X\n", i, WordArray[i]));
			foundNull = 1;
		}
		else if (foundNull)
		{
			PRINTF(("Hangul WordArray[%d] = OK\n", i));
			foundNull = 0;
		}
	}
	return 0;
}

/**
hash table 에 단어를 insert 한다.
신규 단어가 아니면 hash table에 insert fail 이 발생한다.

return 0 if NOT added
return 1 if added
*/
HwordType * HDict::add(HwordType &hword)
{
	HwordType *inserted = 0;
		
	inserted = hashTab.insert(&hword);
	if (inserted == NULL)
		return NULL;

	if (hword.id == 0)
	{
		hword.id = maxWordId + 1;
		inserted->id = maxWordId + 1;
	}
	
	if (hword.id < maxSize) {
		// array 에 insert
		
		WordArray[hword.id] = inserted;
		
		wordNums++;
		if (hword.id > maxWordId)
			maxWordId = hword.id;
	}
	return inserted;

#if 0		
		/*
		어근에 해당하는 부분도 따로 hash 에 추가.

		777||가무대대하다||가무대대-하다||
		'가무대대' 를 추가.
		*/
		HwordType rt;
		char	rootword[64] = {0};
		
		memcpy(&rt, &hword, sizeof(HwordType));
		rt.wlen = hword.split[0] << 1;
		strncpy(rootword, hword.key, rt.wlen);
		rootword[rt.wlen] = 0;
		rt.key = rootword;
		
		inserted = hashTab.insert(&rt);
		if (inserted) inserted->key = (char *)mymem.strAlloc(rt.key, rt.wlen);	
#endif		

}

/**
신규 한글 단어를 테이블에 추가. 메모리에도 동시에 추가.
*/
HwordType * HDict::addNewHangulWord(char *newhanword)
{
	if (search(newhanword) != NULL) // 이미 있는 단어는 새로 추가하지 않는다.
		return NULL;

	// DB 에 추가
	int insert_id = dbStore::storeDB_newhanword(newhanword, maxWordId);

	if (insert_id <= 0)
	{
		ACE_DEBUG((LM_INFO,"new Hangul word DB insert fail: %s res=%d  maxWordId=%d\n", 
			newhanword, insert_id, maxWordId));
		return NULL;
	}
		
	HwordType hword;
	hword.PS = 0;
	hword.Conj = 0;
	hword.property = 0;
	hword.key = (char *)mymem.strAlloc(newhanword);
	hword.id = 0;
	hword.wlen = strlen(hword.key);

	// 메모리에 추가
	HwordType * inserted = add(hword);
	return inserted;
}

int HDict::load(char *home, int debug)
{
	char fname[256];

	int	maxid = dbStore::getMaxValue("newhanword", "word_id");
	int	allocSize = 150000;
	if (maxid > 0)
	{
		maxid += 1000;
		allocSize = MAX(maxid, allocSize);
	}
	alloc(allocSize);
	load_YBMKorDict( StrUtil::path_merge(fname, home, "dataH/sisa_kor.dict"),  debug );
	load_newhanword();

	/* wordList  를 sort 할 필요가 없다.  단어 id 가 곧 array index */
	
	if (debug) hashTab.print();
	if (debug) mymem.print();
	
	return 0;
}

HwordType * HDict::search(char *word)
{
	
	return (HwordType *) HDict::hashTab.search(word);
}

HwordType * HDict::searchByID(int word_id)
{
	if (WordArray==0)
		return NULL;

	if (word_id >= maxSize || word_id < 0)
		return NULL;
	
	return WordArray[word_id];
}

int	HDict::prepare(char *home)
{
	HanChar::prepare();
	HDict::load(home);
	HDict::checkWordArray();
	HGram::load(home);
	return 0;
}