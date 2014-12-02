#ifndef WORDTYPE_H
#define WORDTYPE_H

#include "basicDef.h"

// Types of Example Exatraction
// 예문 추출 처리시에 처리되는 내용들: 어떤 종류까지 추출할 것인가 
#define	PROC_WORD_EX		(1)
#define	PROC_IDIOM_EX		(2)
#define	PROC_WCOMBI_EX	(4)
#define	PROC_ACRO_EX		(8)			//  약어 
#define	PROC_SAYINGS_EX	(16)		// 관용구, 속담, 격언 	

// word level

#define W_LVL_LOW (4)
#define W_LVL_MID (3)
#define W_LVL_HIGH (2)
#define W_LVL_UNIV (1)
#define W_LVL_GEN (0)

// word_type
#define W_SPACE (0x01)
#define W_HYPEN (0x02)
#define W_APOSTR (0x04)
#define W_NO_SINGLE (0x08)
#define W_UPPER (0x10)  /// 전체가 대문자 단어
#define W_DOT (0x20)
#define W_PRE_SUFFIX (0x40)
//#define W_COMBI (0x80)
#define W_HEAD (0x80) // 시작이 대문자 단어

// 일반적 문장이 필수로 포함하는 품사들 
#define	REQUISIT_PS (W_BASIC|PRON|PREP|ARTI|BE_VERB|AUX_VERB)
#define TOO_FREQ (W_BASIC|FREQ_W|ARTI|AUX_VERB|REL_QUEST)

/*
conjunction
등위[대등] 접속사 ((동격의 어구를 잇는 and, but 등))
종위[종속] 접속사 ((종속절을 주절에 잇는 if, though 등))

*/

// part of speech
#define	NOUN	(0x01)
#define	PRON	(0x02) // pronoun
#define	ADJ	(0x04) // adjective
#define	ADV	(0x08)// adverb
#define	VERB	(0x10)
#define	PREP	(0x20) // preposition
#define	CONJ	(0x40) // conjunction
#define	INTJ	(0x80) // interjection 감탄사 

#define	ARTI		(0x0100) // article
#define	BE_VERB		(0x0200) // relative
#define	AUX_VERB	(0x0400) // 조동사 
#define	REL_QUEST	(0x0800) // relative, 의문사 

#define	FREQ_W	(0x1000) // 아주 흔한 전치사 등의 단어
//#define	SUFF	(0x2000) 

#define W_BASIC (0x800000)
//
#define	SLANG	(0x01000000)

#define	IS_NOUN(X) (NOUN & (X))	

// inflect, conjugate
// derivitive

enum CONJUGATE_DERIVITIVE {
	N_PL	= (1),	// pl
	A_MORE	= (2),	 // -er
	A_MOST	= (3), 	// -est
	V_PRES	= (4),
	V_PAST	= (5),
	V_ING	= (6),

	DRV_NOUN = (7),
	DRV_ADJ = (8),
	DRV_ADV = (9)
} ;

#define	DEF_WORD	(1)
#define	DEF_IDIOM	(2)

/*-- 예문 정보 ---*/
typedef struct _exType
{
	uint4 doc_id; // 문서 ID
	uint4 s_pos;	// 문장 시작 위치
	uint2 s_len;	// 문장 길이
	uint2 w_pos;	// 문장내 단어 위치
	uint2 w_len;	//	단어 길이
	byte level;	//	문장 난이도
	byte	conjugate;	// 단어 변형/활용 여부 -- CONJUGATE_DERIVITIVE
	struct _exType *next;
} exType;


// 결과값 예문 리스트를 표현하기 위해, ExType에 매치된 텀 개수를 추가한 구조체
struct resExType : public exType
{
	int matchedTermCnt;
};

#define	W_IN_IDIOMS 	(10) // 숙어를 구성하는 단어의 최대 개수 
typedef struct _idiomType {
	char	*key;
	int	i_id;
	byte nums;
	void *words[W_IN_IDIOMS]; // wordType *
	int	numEx;
	exType	*ex;
} idiomType;


typedef struct _wordType {
	char	*key; // 색인어. 단어.
	char	*org_word; // not NULL when is_drv or is_conj. 단어의 원형
	struct _wordType *org_w;
	int		w_id;
	uint4	PS; // 품사

	byte	w_type; // 대문자, 소문자, 하이프 등의 포함 여부
	byte	level; // 단이도
	byte	has_drv; //has derivative word[s]
	byte	is_drv; // is this word a derivative?
	byte	is_conj; // is this a conjugate or a inflect
	byte	w_len;

	uint2 num_idioms; // 본 단어로 시작하는 숙어들의  총 개수
	uint2 cnt_idioms;	// 본 단어로 시작하는 숙어들의 실제 개수 -- idioms[]에 실제 채워진 개수 
	idiomType	**idioms;	//  본 단어로 시작하는 숙어들의 목록 
	int	numEx; // 예문 개수
	exType	*ex; // 예문
} wordType;

typedef struct _wordExType {
	idiomType	idiom;
	int	numEx;
	exType	*ex;
} wordExType;

typedef struct _idiomExType {
	idiomType	idiom;
	int	numEx;
	exType	*ex;
} idiomExType;


//-----------------------
typedef struct {
	int	id;
	char *name;
	//int	doc_cnt;
} idnameTYPE;

typedef struct _hosts
{
	int	host_id;
    char	*hostname;
    char	*sitename;
    int	doc_cnt;
} hostsType;


typedef struct _hanwordType {
	char	*key;
	int		w_id;
	uint4	PS; //	
	byte	level;
	byte	is_conj; // is this a conjugate or a inflect
	byte	w_len;	
	int	numEx;
	exType	*ex;
} hanwordType;



#define	WORD_EXAM	(1)
#define	IDIOM_EXAM	(2)
#define	WCOMBI_EXAM	(3)
#define	ACRO_EXAM	(4)

#endif
