
#include "ace/Log_Msg.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "EDict.h"
#include "Exam.h"

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "DbQuery.h"
#include "dbStore.h"


#include "EWordCmp.h"


#define	PRN	printf
#define	errPRN	printf
#define	HASH_SEARCH(X)	(wordType *)hashTab.search(X)

EDict * EDict::_instance = 0;
simpleHash<wordType>  EDict::hashTab;

#ifdef MUTEX
ACE_Thread_Mutex EDict::hashmutex_;
#endif

simpleAlloc	EDict::mymem(100);

wordType	**EDict::wordArray = 0;
int			EDict::wordNums = 0;
idiomType	**EDict::idiomArray = (idiomType**)0;
int			EDict::idiomNums = 0;

EDict * EDict::getInstance(int hashsize)
{
	if (_instance==0)
	{
		_instance = new EDict();
		if (hashsize==0) hashsize = DICT_SIZE;
		hashsize = hashTab.init(hashsize) ;
		ACE_DEBUG((LM_INFO, "EDict: hash size=%d\n", hashsize ));		
	}
	
	return _instance;
}

wordType * EDict::search(char *word)
{
	return (wordType *)hashTab.search(word);
}

wordType * EDict::insert(char *word)
{
	wordType newone;
	wordType *inserted = 0;

	memset(&newone, 0, sizeof(wordType));
	strcpy(newone.key, word);

	#ifdef MUTEX
	hashmutex_.acquire();
	#endif
	inserted = hashTab.insert(&newone);
	#ifdef MUTEX
	hashmutex_.release();
	#endif

		/*
	PRN("EDict: cnt=%d, inserted=%X, word=%s\n", 
		hashTab.count(), inserted, word);
		*/

	return inserted;
}

wordType * EDict::insert(wordType *newone)
{
	wordType *inserted = 0;

	#ifdef MUTEX
	hashmutex_.acquire();
	#endif
	inserted = hashTab.insert(newone);
	#ifdef MUTEX
	hashmutex_.release();
	#endif

		/*
	PRN( "EDict: cnt=%d, inserted=%X, word=%s\n", 
		hashTab.count(), inserted, newone->key);
		*/
	return inserted;
}

/**
return 1: remove가 잘된 경우 
*/
int EDict::remove(wordType *delinfo)
{
	char word[64];
	wordType *removed = 0;

	strcpy(word, delinfo->key);
	#ifdef MUTEX
	hashmutex_.acquire();
	#endif
	removed = hashTab.remove(delinfo->key);
	#ifdef MUTEX
	hashmutex_.release();
	#endif
	PRN("EDict: cnt=%d, removed=%X, word=%s\n", 
		hashTab.count(), (uint4)removed, word);
	if (removed != NULL)
	{
		if (removed != delinfo) {
			PRN( "mismatching addr -- EDict::remove()");
			return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

/**
*/

int EDict::prepare(char *home)
{
	char fname[256];
	
	getInstance();

	// 중복 로드 방지 
	if (wordArray) return 0;
	
	//loadWords_FILE( StrUtil::path_merge(fname, home, "data/word.dict") );
	loadWords();
	markBasicWords( StrUtil::path_merge(fname, home, "data/basic.dict") );
	//loadIdioms_FILE( StrUtil::path_merge(fname, home, "data/idiom.dict") );
	loadIdioms();
	
	connectWordIdiom();

	sortWordList();
	sortIdiomList();

	#ifdef BUG
	mymem.print();
	#endif
	
	return 0;
}

/**
단어를 화일에서 로드.
*/
int EDict::loadWords_FILE(char *fname)
{

	FILE *fp;
	char buf[1024];
	int	ch;
	int	wordsEstimate; // 화일의 라인수에서 단어 개수를 대략 추정 
	wordType	w;
	wordType	*newword;
	MemSplit	line(12, 48);

	wordsEstimate = FileUtil::countLines(fname);
	ACE_DEBUG((LM_DEBUG, "# of words estimation: %d\n", wordsEstimate));

	wordArray = new wordType* [wordsEstimate];
	
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		int len=0;
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		
		line.split(buf);
		memset(&w, 0, sizeof(w));

		w.w_type = (byte)strtol( line.str(2), 0, 16);
		// 접두사/ 접미어도 포함 
		//if (w.w_type & W_PRE_SUFFIX) continue;
		
		len = strlen(line.str(0));
		w.w_len = len;
		w.key = (char *)mymem.strAlloc(line.str(0), len );
		
		w.w_id = wordNums + 1;
					
		w.level = (byte) strtol( line.str(4), 0, 10);
		w.PS = (uint4) strtol( line.str(5), 0, 16);
		w.has_drv = (byte) strtol( line.str(6), 0, 16);
		w.is_drv = (byte) strtol( line.str(7), 0, 16);
		w.is_conj = (byte) strtol( line.str(8), 0, 16);

		if (line.size() >= 10) 
			w.org_word = (char *)mymem.strAlloc(line.str(9), len );
		else 
			w.org_word = 0;

		// hash에 insert
		newword = insert(&w);

		// array 에 insert
		wordArray[wordNums] = newword;
		wordNums++;
	}
	
	fclose(fp);
	return wordNums;
}

#ifdef NO_MSSQL
#define	WDICT_TABNAME	"word_list"
#define	IDICT_TABNAME	"idiom_list"
#else
#define	WDICT_TABNAME	"worddict"
#define	IDICT_TABNAME	"idiomdict"
#endif

/**
단어를 화일에서 로드.
*/
int EDict::loadWords()
{
	DbQuery	query;
	int	wordsEstimate; // 화일의 라인수에서 단어 개수를 대략 추정 
	wordType	w;
	wordType	*newword;
	int	dup_cnt = 0;
	wordsEstimate = dbStore::getMaxValue(WDICT_TABNAME, "word_id");

	PRINTF(("loading TABLE %s\n", WDICT_TABNAME));
	
	ACE_DEBUG((LM_DEBUG, "# of words estimation: %d\n", wordsEstimate));

	wordArray = new wordType* [wordsEstimate];
	memset(wordArray, 0, sizeof(wordType*) * wordsEstimate);
	
	query.exec(dbStore::dbConn, "select * from " WDICT_TABNAME " order by word_id");

	while (query.fetch()) {
		int len=0;
		memset(&w, 0, sizeof(w));

		StrUtil::trim_right(query.colStrVal(0) );
		len = strlen(query.colStrVal(0));
		w.w_len = len;
		w.key = (char *)mymem.strAlloc(query.colStrVal(0), len );		
		w.w_id =  query.colIntVal(1);
		w.w_type = (byte) query.colIntVal(2);			

		w.level = (byte) query.colIntVal(3);
		w.PS = (uint4) query.colIntVal(4);
		w.is_conj = (byte) query.colIntVal(5);

		StrUtil::trim_right(query.colStrVal(6) );
		if ( isalpha(query.colStrVal(6)[0]) ) 
			w.org_word = (char *)mymem.strAlloc(query.colStrVal(6) );
		else 
			w.org_word = 0;

		// hash에 insert
		newword = insert(&w);
		if (newword==0) {
			//ACE_DEBUG((LM_DEBUG, " hash insert failed: %s\n", w.key));
			dup_cnt++;
			continue;
		}
		// array 에 insert
		wordArray[wordNums] = newword;
		wordNums++;
	}
	ACE_DEBUG((LM_DEBUG, "%d words are duplicate\n", dup_cnt));
	ACE_DEBUG((LM_DEBUG,"# of words loaded: %d\n", wordNums));
	PRINTF(("# of words loaded: %d\n", wordNums));
	return wordNums;
}

#if 0
/**
숙어를 구성하는 단어 중에 사전에 없는 단어인 경우, 
단어를 hash 에 추가하도록 한다.
단어 ID 문제로 실행 불가능. 2005. 7. 18
*/

int EDict::insertNewWord(char *word)
{
	wordType	w;
	wordType	*newword;
	int	len;
	
	memset(&w, 0, sizeof(w));
	len = strlen(word);
	w.w_len = len;
	w.key = (char *)mymem.strAlloc( word, len );	
}
#endif 

/**
숙어를 화일에서 로드.
return : 숙어 개수
*/
int EDict::loadIdioms_FILE(char *fname)
{
	FILE *fp;
	char buf[1024];
	int	ch;
	int	idiomsEstimate; // 화일의 라인수에서 단어 개수를 대략 추정 
	wordType	*w;
	byte conj_PS;
	idiomType	idiom; // tmp idiom
	idiomType	*newidiom; // 다이나믹 할당하는 숙어
	MemSplit	line(12, 64);

	idiomsEstimate = FileUtil::countLines(fname);
	PRN("# of idioms estimation: %d\n", idiomsEstimate);

	idiomArray = new idiomType* [idiomsEstimate];
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		int	n;
		int	numwords=0; // 숙어를 구성하는 단어의 개수 
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;

		line.split(buf);
		
		for(n=0; n<W_IN_IDIOMS+1; n++) {
			if (line.str(n)[0] == ':') numwords = n;
		}

		if (numwords==0) continue; // 숙어를 구성하는 단어수가 5를 넘으면 무시.

		// 숙어를 구성하는 단어들의 pointer를 구한다.
		memset(&idiom, 0, sizeof(idiomType));
		for(n=0; n<numwords; n++) {
			w = stemWord(line.str(n), line.len(n), &conj_PS);
			if (w==0) break;
			idiom.words[n] = (void *)w;
		}
		if (n < numwords) continue;

		// 새로 숙어를 목록에 추가한다.
		idiom.nums = numwords;
		idiom.i_id = idiomNums + 1;
		newidiom = (idiomType *) mymem.alloc( sizeof(idiomType) );
		memcpy(newidiom, &idiom, sizeof(idiomType) );

		// array 에 insert
		idiomArray[idiomNums] = newidiom;
		idiomNums++;
	}

	fclose(fp);
	return idiomNums;
}



/**
단어를 화일에서 로드.
*/
int EDict::loadIdioms()
{
	DbQuery	query;
	int	idiomsEstimate; // 화일의 라인수에서 단어 개수를 대략 추정 
	wordType	*w;
	byte conj_PS;
	idiomType	idiom; // tmp idiom
	idiomType	*newidiom; // 다이나믹 할당하는 숙어
	int	toolong_cnt = 0; // 숙어가 너무 긴 경우. 단어 수가 W_IN_IDIOMS 초과인 경우.
	int	no_word_cnt = 0; //  숙어내의 단어 중에 사전에 없는 단어가 있는 경우.
	MemSplit	words(W_IN_IDIOMS+1, 64);

	PRINTF(("loading TABLE %s\n", IDICT_TABNAME));
	
	idiomsEstimate = dbStore::getMaxValue(IDICT_TABNAME, "idiom_id");
	ACE_DEBUG((LM_DEBUG,"# of idioms estimation: %d\n", idiomsEstimate));

	idiomArray = new idiomType* [idiomsEstimate];
	
	query.exec(dbStore::dbConn, "select * from " IDICT_TABNAME " order by idiom_id");

	while (query.fetch()) {
		int	n;
		int	numwords=0; // 숙어를 구성하는 단어의 개수 

		StrUtil::trim_right(query.colStrVal(0) );
		numwords = words.splitWords(query.colStrVal(0));
		
		if (numwords==0 || numwords > W_IN_IDIOMS) {
			//ACE_DEBUG((LM_DEBUG, "Not idiom: %s\n", query.colStrVal(0) ));
			toolong_cnt++;
			continue; // 숙어를 구성하는 단어수가 5를 넘으면 무시.
		}

		// 숙어를 구성하는 단어들의 pointer를 구한다.
		memset(&idiom, 0, sizeof(idiomType));
		for(n=0; n<numwords; n++) {
			w = stemWord(words.str(n), words.len(n), &conj_PS);
			if (w==0) break;
			idiom.words[n] = (void *)w;
		}
		if (n < numwords) {
			no_word_cnt++;
			continue;
		}

		// 새로 숙어를 목록에 추가한다.
		
		idiom.key = (char *)mymem.strAlloc(query.colStrVal(0));
		idiom.nums = numwords;
		idiom.i_id = query.colIntVal(1);
		newidiom = (idiomType *) mymem.alloc( sizeof(idiomType) );
		memcpy(newidiom, &idiom, sizeof(idiomType) );

		// array 에 insert
		idiomArray[idiomNums] = newidiom;
		idiomNums++;
	}

	ACE_DEBUG((LM_DEBUG, "%d idioms are too long\n", toolong_cnt ));
	ACE_DEBUG((LM_DEBUG, "%d idioms have unknown words\n", no_word_cnt ));
	ACE_DEBUG((LM_DEBUG,"# of idioms loaded: %d\n", idiomNums));
	PRINTF(("# of idioms loaded: %d\n", idiomNums));
	return idiomNums;
}


/**
전치사, 대명사, 의문사, 관계사, 등의 기본적 단어들 목록을 화일에서 읽고,
그 단어들의 품사 정보에 세팅한다.
*/
int EDict::markBasicWords(char *fname)
{
	FILE *fp;
	char buf[128];
	
	wordType	*w;
	int	basicWcnt=0;
	
	MemSplit	line(12, 64);
	
	if ((fp = fopen(fname, "r"))==NULL) {
		errPRN("cannot read file %s !\n", fname);
		return 0;
	}

	while (fgets(buf, 128, fp) != NULL) {
		int	ch=0;
		int	PS=0;
		byte conj_PS=0;
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;

		line.split(buf);

		PS = strtol(line.str(1), 0, 16);
		w = stemWord(line.str(0),  line.len(0), &conj_PS);
		if (w==0) {
			wordType newword;
			memset(&newword, 0, sizeof(wordType) );
			newword.key = (char *)mymem.strAlloc(line.str(0), line.len(0) );
			w = insert(&newword);
		}
		
		if (w) {			
			// basic word list에 품사가 지정되어 잇다면 그 품사로만 지정 (bit OR가 아니라 value setting
			// 관사, 전치사, 대명사 등
			if (line.size() > 1) w->PS = PS;
			w->level = W_LVL_LOW;
			
			if (line.size() < 3) {
				w->PS |= W_BASIC;				
			}
			basicWcnt++;
		}
		else {			
				ACE_DEBUG((LM_INFO,"basic word NOT in the Dict: %s\n", line.str(0)));
		}
	}
	fclose(fp);
	ACE_DEBUG((LM_INFO,"read %d basic words\n", basicWcnt));
	return basicWcnt;
}


/**
단어별로, 그 단어로 시작하는 숙어 목록을 완성한다.
*/
int EDict::connectWordIdiom()
{
	int	i;
	wordType *word;
	idiomType *idiom;
	int	num_of_words_with_idioms = 0;;
	// 숙어의 첫 단어에 대해, 그 단어로 시작하는 숙어들의 개수를 증가.
	for(i=0 ; i<idiomNums; i++) {	
		idiom = idiomArray[i];
		word = (wordType *) idiom->words[0];
		word->num_idioms++;
	}
		
	// 단어별로 숙어 목록 저장을 위한 메모리를 할당한다.
	for(i=0; i<wordNums; i++) {
		word = wordArray[i];
		/* 원본 단어와 연결 시킨다 */
		if (word->org_word) {
			word->org_w = HASH_SEARCH(word->org_word);
		}
		
		if (word->num_idioms > 0) {
			num_of_words_with_idioms++;
			word->idioms = (idiomType**) mymem.alloc ( sizeof(idiomType *) * word->num_idioms);
		}
	}

	// 숙어별로, 그 숙어의 첫 단어의 숙어 목록에 해당 숙어를 추가.
	for(i=0; i<idiomNums; i++) {
		idiom = idiomArray[i];
		word = (wordType *) idiom->words[0];
		if (word->cnt_idioms >= word->num_idioms) {
			ACE_DEBUG((LM_INFO,"error in connectWordIdiom(): cnt_idioms(%d) >= num_idioms(%d)\n", 
				word->cnt_idioms , word->num_idioms));
			continue;
		}
		word->idioms[word->cnt_idioms] = idiom;
		word->cnt_idioms++;
	}

	ACE_DEBUG((LM_INFO,"connectWordIdiom(): # of Words which have idioms = %d\n", num_of_words_with_idioms));
	return 0;
	
}

// 기본 단어 목록에 없는 단어들 처리..
// 규칙, 불규칙 활용된 단어, 
// 동사:--ed, --s, --es, --ing
// 명사:--es, --s
// 형용사/부사 : --er, --est, --ier, --iest
// --ly, --ness
wordType *EDict::checkConjugate(char *word, int wlen, byte *conj_PS)
{
	wordType *w=0;
	char	*found=0;
	
	if (wlen==0) wlen = strlen(word);
	

	found = strchr(word, '\'');
	
	if (found) {
		if (strncmp(word+wlen-3, "n't", 3)==0) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		else if (strncmp(word+wlen-3, "'ve", 3)==0) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		else if (strncmp(word+wlen-3, "'ll", 3)==0) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		else if (strncmp(word+wlen-2, "'s", 2)==0) {
			word[wlen-2]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		else if (strncmp(word+wlen-2, "'d", 2)==0) {
			word[wlen-2]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		else if (strncmp(word+wlen-3, "'re", 3)==0) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
		
		return w;
	}
		
	// plurals --ies, --es, --s
	if (strncmp(word+wlen-3, "ies", 3)==0) {
		*conj_PS = N_PL;
		word[wlen-3]='y';
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	else if (strncmp(word+wlen-2, "es", 2)==0) {
		*conj_PS = N_PL; // 동사일수도 
		word[wlen-1]=0;
		w = HASH_SEARCH(word);
		if (w) return w;

		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	else if (word[wlen-1]=='s') {
		*conj_PS = N_PL; // 동사일수도 
		word[wlen-1]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	
	// --ing
	else if (strncmp(word+wlen-3, "ing", 3)==0) {
		word[wlen-3]=0;
		w = HASH_SEARCH(word);
		*conj_PS = V_ING;
		if (w) return w;
		word[wlen-3]='e';
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
		if (wlen>=5 && word[wlen-4]==word[wlen-5]) {
			word[wlen-4]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
	}
	// verb past --ed, 
	else if (strncmp(word+wlen-2, "ed", 2)==0) {
		*conj_PS = V_PAST;
		word[wlen-1]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
		if (wlen>=4 && word[wlen-3]==word[wlen-4]) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
	}
	
	// --er, --est
	else if (strncmp(word+wlen-2, "er", 2)==0) {
		*conj_PS = A_MORE;
		word[wlen-1]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
		if (wlen>=4 && word[wlen-3]==word[wlen-4]) {
			word[wlen-3]=0;
			w = HASH_SEARCH(word);
			if (w) return w;
		}
	}
	else if (strncmp(word+wlen-3, "est", 3)==0) {
		*conj_PS = A_MOST;
		word[wlen-3]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	// 불규칙활용 -- 사전에 존재 
	
	else if (strncmp(word+wlen-3, "ier", 3)==0) {
		*conj_PS = A_MORE;
		word[wlen-3]='y';
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	else if (strncmp(word+wlen-4, "iest", 4)==0) {
		*conj_PS = A_MOST;
		word[wlen-4]='y';
		word[wlen-3]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}

	//--- optional---
	else if (strncmp(word+wlen-2, "ly", 2)==0) {
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	else if (strncmp(word+wlen-4, "ness", 4)==0) {
		word[wlen-4]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	
	else if (strncmp(word+wlen-6, "liness", 6)==0) {
		word[wlen-6]=0;
		w = HASH_SEARCH(word);
		if (w) return w;
	}
	
	if (w==0) {		
		w = checkDerivative(word, wlen, conj_PS);
	}
	return w;
	
}



// 사전에 없는 이 단어가 사전에 잇는 단어에서 파생된 것인지 검사 
// org_word : 사전에 없는 단어

wordType *EDict::checkDerivative(char *word, int wlen, byte *conj_PS)
{

	//int	tmp_ch;
	wordType *w=0;

	*conj_PS = 0;
	//
	if (strncmp(word+wlen-2, "er", 2)==0) {
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_NOUN;
		if (w) return w;
	}
	else if (strncmp(word+wlen-2, "or", 2)==0) {
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_NOUN;
		if (w) return w;
	}
	else if (strncmp(word+wlen-3, "ist", 3)==0) {
		word[wlen-3]='y';
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_NOUN;
		if (w) return w;
	}
	else if (strncmp(word+wlen-3, "ic", 3)==0) {
		word[wlen-3]='y';
		word[wlen-2]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_ADJ;
		if (w) return w;
	}
	else if (strncmp(word+wlen-4, "ical", 4)==0) {
		word[wlen-4]='y';
		word[wlen-3]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_ADJ;
		if (w) return w;
	}
	else if (strncmp(word+wlen-6, "ically", 6)==0) {
		word[wlen-6]='y';
		word[wlen-5]=0;
		w = HASH_SEARCH(word);
		*conj_PS = DRV_ADV;
		if (w) return w;
	}
	
	*conj_PS = 0;
	return 0;

}



/**
stemmer.
// word : a word in the text, 
// org_w: the word in the dictionary that match 'word'
*/
wordType *EDict::stemWord(char *org_w, int wlen, byte *conj_PS)
{
	wordType *w=0;
	char	tmpword[WORD_LEN+1];

	*conj_PS=0;
	
	w = HASH_SEARCH(org_w);
	
	if (w) {
		if (w->org_w) return w->org_w;
		else		return w;
	}

	if (wlen==0) wlen = strlen(org_w);
	
	strncpy(tmpword, org_w, WORD_LEN);
	tmpword[WORD_LEN] = 0;
		
	if (islower(org_w[0]))
	{
		tmpword[0] = toupper(tmpword[0]);
		w = HASH_SEARCH(tmpword);
		if (w) return w;
	}
	
	
	// 모두 대문자로 된 경우 
	if ( isupper(org_w[0]) ) {
		StrUtil::copy_to_lower(tmpword, org_w);

		w = HASH_SEARCH(tmpword);
		if (w) return w;
		
		w = checkConjugate(tmpword, wlen, conj_PS);		
		if (w) return w;

		tmpword[0] = toupper( tmpword[0]) ;
		w = checkConjugate(tmpword, wlen, conj_PS);		
		if (w) return w;
		
	}
	else {
		

		w = checkConjugate(tmpword, wlen, conj_PS);		
		if (w) return w;
	}
		
	return 0;
}					

void	EDict::printWord(wordType *w)
{
	int i;
	printf("[id %d] %-20s : %02X  %d  %04X  %d  %d  %d  [%d]", 
		w->w_id, w->key, w->w_type, w->level, w->PS, 
		w->has_drv, w->is_drv, w->is_conj, w->numEx);
	if (w->org_word)	printf("org: %s\n", w->org_word);
	if (w->org_w)	printf("org: %s\n", w->org_w->key);
	else printf("\n");

	printf("%s : has %d idioms\n", w->key, w->cnt_idioms);

	for(i = 0; i<w->cnt_idioms; i++) {
		//printIdiom(w->idioms[i]);
	}

}

void	EDict::printWord(wordType *w, StrStream &stream)
{
	stream.add("[id %d] %-20s : ex=%d idioms=%d  %02X  %d  %04X  %d  %d  %d ", 
		w->w_id, w->key, w->numEx, w->cnt_idioms,
		w->w_type, w->level, w->PS, 
		w->has_drv, w->is_drv, w->is_conj);
	if (w->org_word)	stream.add("org: %s\n", w->org_word);
	if (w->org_w)	stream.add("org: %s\n", w->org_w->key);
	else stream.add("\n");
	stream.add("\n");
}

void	EDict::printIdiom(idiomType *idiom)
{
	int	i;
	wordType *word;
	char buf[100];
	char *ptr = buf;


	
	for(i = 0; i<idiom->nums; i++) {
		word = (wordType *)idiom->words[i];
		sprintf(ptr, "%s ", word->key);
		
		ptr += strlen(ptr);
	}
	
	PRINTF(("I: %d %s\n", idiom->i_id, buf));

}


void	EDict::printIdiom(idiomType *idiom, StrStream &stream)
{
	int	i;
	wordType *word;
	char buf[100];
	char *ptr = buf;

	stream.add("Idiom " );
	
	for(i = 0; i<idiom->nums; i++) {
		word = (wordType *)idiom->words[i];
		sprintf(ptr, "%s ", word->key);
		stream.add("%s ", word->key);
		ptr += strlen(ptr);
	}
	
	PRINTF(("I: %d %s\n", idiom->i_id, buf));

}

/*=== idiom list :: operations on idiomArray ===*/
/**
각 단어별로, 그 단어로 시작되는 숙어목록을 sort한다.
*/
int EDict::cmpIdiomList(const void *Aptr, const void *Bptr)
{
	return (*(idiomType **)Aptr)->i_id - (*(idiomType **)Bptr)->i_id;
}

int EDict::sortIdiomList()
{
	qsort (idiomArray, idiomNums, sizeof(idiomType *), cmpIdiomList);
	return 0;
}

/*=== word list :: operations on wordArray ===*/
/**
각 단어별로, 그 단어로 시작되는 숙어목록을 sort한다.
*/
int EDict::cmpWordList(const void *Aptr, const void *Bptr)
{
	return (*(wordType **)Aptr)->w_id - (*(wordType **)Bptr)->w_id;
}

int EDict::sortWordList()
{
	qsort (wordArray, wordNums, sizeof(wordType *), cmpWordList);
	return 0;
}

/**
주어진 id를 가진 word 찾기.
binary search 이용.
w_id 가 wordArray 에 순차적으로 있다면 binary search 말고 직접 index 가능.
*/
wordType *EDict::searchWord(int target_id)
{
	int low = 0, high = wordNums - 1;
	int	cmp;
	while( low <= high )    {
		int mid = ( low + high ) / 2;
		cmp = wordArray[mid]->w_id - target_id;
		if( cmp < 0 )
			low = mid + 1;
		else if( cmp > 0)
			high = mid - 1;
		else
			return wordArray[mid];   // Found
	}
	return NULL;     // NOT_FOUND 
}

/**
주어진 id를 가진 idiom 찾기.
*/
idiomType *EDict::searchIdiom(int target_id)
{
	int low = 0, high = idiomNums - 1;
	int	cmp;
	while( low <= high )    {
		int mid = ( low + high ) / 2;
		cmp = idiomArray[mid]->i_id - target_id;
		if( cmp < 0 )
			low = mid + 1;
		else if( cmp > 0)
			high = mid - 1;
		else
			return idiomArray[mid];   // Found
	}
	return NULL;     // NOT_FOUND 
}

/*==== idiom list in a word :: wordType-idioms ====*/
/**
각 단어별로, 그 단어로 시작되는 숙어목록을 sort한다.
*/
int EDict::sortIdioms_in_word()
{
	int	i;
	wordType *word;
		
	// 
	for(i=0; i<wordNums; i++) {
		word = wordArray[i];
		if (word->cnt_idioms > 0) {
			qsort (word->idioms, word->cnt_idioms, sizeof(idiomType*), cmpIdioms_in_word);
		}
	}
	return 0;
}

/**
한 단어 관련한 숙어들의  목록 정렬을 위한 compare 함수.
*/
int EDict::cmpIdioms_in_word(const void *Aptr, const void *Bptr)
{
	idiomType *A = *(idiomType **)Aptr;
	idiomType *B = *(idiomType **)Bptr;
	wordType	**aword, **bword;
	int	i, min;
	
	
	min = MIN(A->nums, B->nums);
	aword = (wordType	**)A->words;
	bword = (wordType	**)B->words;
	
	for(i=0; i<min; i++, aword++, bword++) {
		if (*aword < *bword) return -1;
		if (*aword > *bword) return 1;
	}
	if (A->nums < B->nums) return -1;
	if (A->nums > B->nums) return 1;
	return 0;
}



/**
주어진 단어들로 구성된 숙어를 찾는다.
*/
idiomType *EDict::searchIdiom(wordType **words, int num_words)
{
	int	i;
	int	match;
	wordType *firstW;
	idiomType *matchedIdiom = NULL;
	int	matchedRate = 0;

	if (num_words <= 0) return NULL;
	firstW = words[0];
	if (firstW->cnt_idioms == 0) return NULL;

	for(i=0; i<firstW->cnt_idioms; i++) {
		/*
		// "take it out" 과    숙어 take out을 매치 하기 위해.
		if (num_words != firstW->idioms[i]->nums)
			continue;
		*/
		
		
		//if (strcmp(firstW->key, "take")==0)
		//	printf("searchIdiom() : loop [%d/%d] %s\n", i,  firstW->cnt_idioms, firstW->idioms[i]->key);
		
		match = matchIdiom(words+1, num_words-1, 
					(wordType **)&(firstW->idioms[i]->words[1]), firstW->idioms[i]->nums -1);

		//if (match == num_words-1)
		if (match == firstW->idioms[i]->nums-1) // 2007.11.27
			return firstW->idioms[i];
	}

	return NULL;
	
}




/**
주어진 단어들로 구성된 숙어를 찾는다.
부분 매치까지 지원하는 버전. 아직 버그 있음.
@param words idiom 을 구성하는 단어들.
*/
idiomType *EDict::searchIdiom_partial(wordType **words, int num_words)
{
	int	i;
	int	match;
	wordType *firstW;
	idiomType *matchedIdiom = NULL;
	int	matchedRate = 0;

	if (num_words <= 0) return NULL;
	firstW = words[0];
	if (firstW->cnt_idioms == 0) return NULL;

	for(i=0; i<firstW->cnt_idioms; i++) {
		match = matchIdiom(words+1, num_words-1, 
					(wordType **)(firstW->idioms[i]->words)+1,  firstW->idioms[i]->nums -1);

		if (match < firstW->idioms[i]->nums -1) continue;
		
		if (match > matchedRate) {
			matchedIdiom = firstW->idioms[i];
			matchedRate = match;
		}
		else if (match == 0 && matchedIdiom != NULL)	{
		// match 되는 숙어를 발견한 이후에, 다른 숙어에서 match==0 이면 찾기를 멈춘다.
			//break;
		}
	}

	if (matchedIdiom) return matchedIdiom;
	else	return NULL;
	
}

/**
주어진 단어 목록과 숙어의 일치 정도를 측정한다.
여기서는 일치하는 단어의 개수를 리턴한다.
숙어내에 대명사 (it, them 등) 가 있으면 이를 제외하고 비교한다.

주어진 숙어 후보 : words, num_words
사전에 저장된 숙어 목록 : idiom_words, idiom_w_cnt
*/
int	EDict::matchIdiom(wordType **words, int num_words, wordType **idiom_words, int idiom_w_cnt)
{
	int	wordidx, idiomidx;

	for (wordidx=0, idiomidx=0;
		wordidx < num_words && idiomidx < idiom_w_cnt;
		words++, wordidx++) 
	{
		if (*words != NULL &&  *words != *idiom_words) {
			if ((*words)->PS & PRON)
			{
				// 대명사 인 경우
				
			}
			else
				break;
		}
		else
		{
			idiom_words++;
			idiomidx++;
		}
	
	}
	
	return idiomidx;
}

/**
여러 단어로 구성된 스트링을 각 단어로 나누어,  각 단어의 stem 단어 정보 (WordType)
를 구한다.

@param maxnum 최대 단어 개수 == W_IN_IDIOMS
*/
int	EDict::stemNwords(wordType	*wordInfos[], int maxnum, char *buf, char *firstWord)
{	
	byte conj_PS;		
	int	cnt_words=0;
	MemSplit	line(maxnum, 64);
	
	line.split(buf);	

	if (firstWord != NULL)
	{
		// 여러 단어 일 때 첫 단어만 추출해서 저장해 놓는다. 2007.11.21
		strncpy(firstWord, line.str(0), 60);
	}
	
	for(int n=0; n<line.size(); n++) {
		wordInfos[cnt_words] = stemWord(line.str(n), line.len(n), &conj_PS);
		// 2007.10.16 : 예문이 1개 이상인 경우만 처리. 
		if (wordInfos[cnt_words]) 
			cnt_words++;

		/*
		//아래 방식은 숙어 검색에 문제 발생.
		if (wordInfos[cnt_words] && wordInfos[cnt_words]->numEx > 0) 
			cnt_words++;
		*/
	}
	return cnt_words;
}

int	EDict::testIdiom(char *buf,  StrStream &stream)
{
	wordType	*words[W_IN_IDIOMS];
	int	cnt_words=0;
	idiomType *foundIdiom;

	cnt_words = stemNwords(words, W_IN_IDIOMS, buf);
	printf("%s --> %d words\n", buf, cnt_words);
	
	foundIdiom = searchIdiom(words, cnt_words);
	if (foundIdiom) {
		printf("idiom found: %s\n", foundIdiom->key);
		//printIdiom(foundIdiom, stream);
		//stream.print();
	}
	else {
		printf("No idiom found\n");
		return -1;	// TODO idiom이 없는 경우를 전달? 현재 AND 구현을 추가하기 위해 일단 이부분에서는 -1을 리턴
	}
	return 0;
}
