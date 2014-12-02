#ifndef HGRAM_H
#define HGRAM_H

#include "simpleHash.h"
#include "StrStream.h"

#include "HanChar.h"
#include "HDict.h"


#define	MAXLEN_HWORD	(30) //한글 단어의 최대 길이 byte
#define	MAXLEN_GWORD	(20)

/* 한글의 형태소 분석에서는 여러 개의 후보를 분석 결과로 제시 가능 */
#define	MAX_GW_MATCH	(5)  // 하나의 한글 어절에 대해 최대 match 할 수 있는 문법어 단어 개수

/*-- 조사, 어미 등의 문법어 목록 처리를 위해 -- */
#define	MAX_GWORD	(1500)  // Eomi_all.txt, Josa_all.txt  라인 수 합보다 많게 
#define	MAX_JOSANUM	(500)
#define	MAX_EOMINUM	(1000)

#define	GWORD_FREQ_MARK	(0x01)

/* 문법 형태소 특성 */
//#define	GW_FREQ	(0x01)
#define	GW_JAMOHEAD	(0x08) // 문법 형태소가 자모로 시작하는 경우 세팅. ㄴ, ㄹ, ㅁ, ㅂ 가능.	
#define	GW_CHOSUNG	(0x01) // 첫 음절에 초성이 있는 것.  ~이 == 0, ~가 ==1
#define	GW_BUNGAN	(0x40)	// 분간조사/어미 . 앞음절의 종성 유무에 따라 (이)가 처럼 구분되는 조사/어미.


/* 문법 형태소 품사 */
#define	GW_JOSA	(0x01)
#define	GW_EOMI	(0x02)


/* hGramType *X */
#define	IS_JAMOHEAD(X)	((X)->PS & GW_JAMOHEAD)

typedef struct _hGramType {
//	int	id;
	char	*key;
	char *invert;
	SylInfo **infos;
	uint2	PS; // 품사
	uint2 	freqJosa; // 빈도수  (백분율 x 10000: 0.1 % == 10)
	uint2	property; /* 문법 형태소 특성 */
	uint2 	freqEomi;
	uint2	gramVal; // 조서, 어미를 분류  값 : 문법 형태소의 분류 값
	byte	wlen;
	
} hGramType;

typedef struct _stemGuessType {
	char	stem[MAXLEN_HWORD+1];
} stemGuessType;

/*
어절을  어휘 형태소 + 문법 형태소로 분리하기 위해
어휘 사전과 조사/어미 사전을 검색하여 
(체언 + 조사 ) 또는 ( 용언 + 어미 ) 로 match 한 결과.

형태소 분석 결과의 후보가 된다.
*/
typedef struct _matchResType {
	// 어절 내에서 조사/ 어미 부분을 제거한 어휘 부분. 원 어절의 음절 모양과 달라질 수 있다.
	char	stem[MAXLEN_HWORD+1]; 
	byte kind; // match 종류 : 
	// 조사, 어미 
	hGramType *tail;
} matchResType;

class HGram {
	public:
		static	int	load(char *home);
		static	void	print();
		static	void	printGword(hGramType *w);
		static	void	printList();
		static	void	printList_invert();
		static	int	matchJosa(hGramType **matchGW, int maxMatch, char *hword, int hwlen);
		static	int	matchEomi(hGramType **matchGW, int maxMatch, char *hword, int hwlen);
		static	hGramType*search(char *word) {
			return hashTab.search(word);
		}
		
	private:

		static	simpleAlloc mymem;
		static simpleHash<hGramType> hashTab;
		static	hGramType **josaArray; // 조사 
		static	hGramType **josaArray_invert;
		static	hGramType **eomiArray; // 어미 
		static	hGramType **eomiArray_invert;
		static	int	josaNums;
		static	int	josaMaxNum;
		static	int	eomiNums;
		static	int	eomiMaxNum;

		static	int alloc(int jsize, int esize);
		static	int cmpgramList(const void *Aptr, const void *Bptr);
		static	int sortgramList();
		static	int cmpgramList_invert(const void *Aptr, const void *Bptr);
		static	int sortgramList_invert();

		static	int loadGramDict(char *fname, int PS,  int freqFlag);
		static	void	markGramDict(hGramType **gdictarray, int size,  uint2 PS);
		static	int load_grammar_freq(char *fname, int PS);
		static	int	cmpEomi(char *str1, char *str2, int len);
		static	void	testHash();
		static	void	insertNewword(hGramType	*newword, int PS);
		
};

#endif
