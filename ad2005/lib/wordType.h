#ifndef WORDTYPE_H
#define WORDTYPE_H

#include "basicDef.h"

// Types of Example Exatraction
// ���� ���� ó���ÿ� ó���Ǵ� �����: � �������� ������ ���ΰ� 
#define	PROC_WORD_EX		(1)
#define	PROC_IDIOM_EX		(2)
#define	PROC_WCOMBI_EX	(4)
#define	PROC_ACRO_EX		(8)			//  ��� 
#define	PROC_SAYINGS_EX	(16)		// ���뱸, �Ӵ�, �ݾ� 	

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
#define W_UPPER (0x10)  /// ��ü�� �빮�� �ܾ�
#define W_DOT (0x20)
#define W_PRE_SUFFIX (0x40)
//#define W_COMBI (0x80)
#define W_HEAD (0x80) // ������ �빮�� �ܾ�

// �Ϲ��� ������ �ʼ��� �����ϴ� ǰ��� 
#define	REQUISIT_PS (W_BASIC|PRON|PREP|ARTI|BE_VERB|AUX_VERB)
#define TOO_FREQ (W_BASIC|FREQ_W|ARTI|AUX_VERB|REL_QUEST)

/*
conjunction
����[���] ���ӻ� ((������ ��� �մ� and, but ��))
����[����] ���ӻ� ((�������� ������ �մ� if, though ��))

*/

// part of speech
#define	NOUN	(0x01)
#define	PRON	(0x02) // pronoun
#define	ADJ	(0x04) // adjective
#define	ADV	(0x08)// adverb
#define	VERB	(0x10)
#define	PREP	(0x20) // preposition
#define	CONJ	(0x40) // conjunction
#define	INTJ	(0x80) // interjection ��ź�� 

#define	ARTI		(0x0100) // article
#define	BE_VERB		(0x0200) // relative
#define	AUX_VERB	(0x0400) // ������ 
#define	REL_QUEST	(0x0800) // relative, �ǹ��� 

#define	FREQ_W	(0x1000) // ���� ���� ��ġ�� ���� �ܾ�
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

/*-- ���� ���� ---*/
typedef struct _exType
{
	uint4 doc_id; // ���� ID
	uint4 s_pos;	// ���� ���� ��ġ
	uint2 s_len;	// ���� ����
	uint2 w_pos;	// ���峻 �ܾ� ��ġ
	uint2 w_len;	//	�ܾ� ����
	byte level;	//	���� ���̵�
	byte	conjugate;	// �ܾ� ����/Ȱ�� ���� -- CONJUGATE_DERIVITIVE
	struct _exType *next;
} exType;


// ����� ���� ����Ʈ�� ǥ���ϱ� ����, ExType�� ��ġ�� �� ������ �߰��� ����ü
struct resExType : public exType
{
	int matchedTermCnt;
};

#define	W_IN_IDIOMS 	(10) // ��� �����ϴ� �ܾ��� �ִ� ���� 
typedef struct _idiomType {
	char	*key;
	int	i_id;
	byte nums;
	void *words[W_IN_IDIOMS]; // wordType *
	int	numEx;
	exType	*ex;
} idiomType;


typedef struct _wordType {
	char	*key; // ���ξ�. �ܾ�.
	char	*org_word; // not NULL when is_drv or is_conj. �ܾ��� ����
	struct _wordType *org_w;
	int		w_id;
	uint4	PS; // ǰ��

	byte	w_type; // �빮��, �ҹ���, ������ ���� ���� ����
	byte	level; // ���̵�
	byte	has_drv; //has derivative word[s]
	byte	is_drv; // is this word a derivative?
	byte	is_conj; // is this a conjugate or a inflect
	byte	w_len;

	uint2 num_idioms; // �� �ܾ�� �����ϴ� �������  �� ����
	uint2 cnt_idioms;	// �� �ܾ�� �����ϴ� ������� ���� ���� -- idioms[]�� ���� ä���� ���� 
	idiomType	**idioms;	//  �� �ܾ�� �����ϴ� ������� ��� 
	int	numEx; // ���� ����
	exType	*ex; // ����
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
