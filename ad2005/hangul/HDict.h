#ifndef HDICT_H
#define HDICT_H

#include "simpleHash.h"
#include "StrStream.h"

#include "HanChar.h"
#include "wordType.h"

/** YBM 국어사전: 불규칙활용 종류.
 ㄷ불규칙활용
 ㄹ불규칙활용
 ㅂ불규칙활용
 ㅅ불규칙활용
 ㅎ불규칙활용
 거라불규칙활용
 너라불규칙활용
 러라불규칙활용
 러불규칙활용
 르불규칙활용
 여불규칙확용
 우불규칙활용
 으불규칙활용
*/

/* HwordType.Conj */
#if 0
/* HanChar.h 로  옮김. HanChar::markSyl_Irregular()  에서 참조하기에. */
enum {
	D_conj = 1,
	L_conj,
	B_conj,
	S_conj,
	H_conj,
	Go_conj,
	Come_conj,
	Leora_conj,  // 러라
	Leo_conj,  // 러 
	Reu_conj,  // 르
	Yeo_conj,  // 여 
	U_conj,  // 우
	Eu_conj  // 으 
};
#endif

/** YBM 국어사전: 품사 종류.


 감탄사
 관형사
 구
 대명사
 동사
 명사
 명사사
 보조동사
 보조형용사
 부사
 불완전자동사
 불완전타동사
 수사
 어미
 의존명사
 자동사
 접두사
 접미사
 조사
 준말
 타동사
 한의학
 형사
 형용사
*/

/* HwordType.PS */
#define	HAN_NOUN	(0x01 << 16) 
#define	HAN_VERB	(0x02 << 16)
#define	HAN_ADJ	(0x04 << 16) // 형용사
#define	HAN_ADVB	(0x08 << 16) // 부사

#define	HAN_ADJ_KOR	(0x10 << 16) // 관형사
#define	HAN_EXCL	(0x20 << 16) // 감탄사
#define	HAN_PHRASE	(0x40 << 16) // 감탄사

#define	HAN_EOMI	(0x80 << 16) // 어미
#define	HAN_JOSA	(0x0100 << 16) // 조사 
#define	HAN_PFIX	(0x0200 << 16) // 접두사
#define	HAN_SFIX	(0x0400 << 16) // 접미사

#define	HAN_VT	(0x01) //  타동사
#define	HAN_INCOMPLETE (0x02) // 불완전 
#define	HAN_AUX	(0x04) // 보조 동사/ 보조 형용사 ..
#define	HAN_DEP	(0x08) // 의존 ...  의존 명사.
#define	HAN_NUM	(0x0100) // 수사.
#define	HAN_PRONOUN	(0x0200) // 대명사.

#define	IS_H_VERB(X)	((X) & (HAN_ADJ|HAN_VERB))
#define	IS_H_NOUN(X)	((X) & (HAN_NOUN))

/*
http://woorimal.net/language/moonbub/word-structure-1.htm

실질적 의미의 유무에 따라
        ① 실질형태소 : 체언, 수식언, 독립언, 용언의 어간.
                예> 나는 넓-은 바다를 보-았-다. 

        ② 형식형태소 : 조사, 어미, 접사
                예> 철수가 이야기책을 읽-었-다. 
*/

#define	IS_H_REAL(X)	((X) & (HAN_ADJ|HAN_ADVB |HAN_ADJ_KOR|HAN_VERB|HAN_NOUN))
/**
HwordType.property: 단어 특성.
~스럽다.
~하다.
~다.
*/
#define	HType_Like	(0x01)
#define	HType_Hada	(0x02)
#define	HType_Da	(0x04)

/*
777||가무대대하다||가무대대-하다||
split[0] = 4 가 들어 간다.
*/
typedef struct _hDictType {
	int	id;
	char	*key;
	/* 단어를 음절 단위로 분리 */
	byte	split[4];
	int	PS; // 품사
	byte	wlen;
	byte	Conj; // 활용
	byte	property; // 특성.
	int	numEx;
	exType	*ex;
	
} HwordType;

struct _nameValPair {
	char *name;
	int	value;
};

class HDict {
	public:
		static	int	load(char *home, int debug=0);
		static	 HwordType * search(char *word);
		static	HwordType * searchByID(int word_id);
		static	void	printWord_stream(StrStream &stream, HwordType *h);
		static	void	printWord(HwordType *h);
		static	void	printList(); 
		static	void	printListConjOnly(); 
		static	void	printListAuxOnly();
		static	void	printListEomiOnly(); 
		static	void	printListJosaOnly(); 
		static	HwordType *	addNewHangulWord(char *newhanword);
		static	int	prepare(char *home);
		
	private:

		static	struct _nameValPair conjMapTable[];
		static	struct _nameValPair psMapTable[];
		
		static	simpleAlloc mymem;
		static simpleHash<HwordType> hashTab;
		static	HwordType **WordArray;
		static	int	wordNums;
		static	int	maxWordId;
		static	int	maxSize;
		

		static	int	CONJname_to_CONJval(char *CONJname);
		static	int	PSname_to_PSval(char *PSname);
		static	void test_nameValPairTable();
		static	int	proc_YBMKorDict_grammar(HwordType &hword, char *dict_body, int body_len);
		static	void	proc_YBMKorDict_split(HwordType &hword, char *split_word, int split_word_len);
		static	void	proc_YBMKorDict_type(HwordType &hword);
		
		static	int load_YBMKorDict(char *fname, int debug=0);
		static	int load_newhanword();
		static	int cmpWordList(const void *Aptr, const void *Bptr);

		static	int sortWordList();

		static	int alloc(int hashsize);
		static	HwordType * add(HwordType &hword);

		static	int checkWordArray();
		
};
#endif
