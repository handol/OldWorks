#ifndef HDICT_H
#define HDICT_H

#include "simpleHash.h"
#include "StrStream.h"

#include "HanChar.h"
#include "wordType.h"

/** YBM �������: �ұ�ĢȰ�� ����.
 ���ұ�ĢȰ��
 ���ұ�ĢȰ��
 ���ұ�ĢȰ��
 ���ұ�ĢȰ��
 ���ұ�ĢȰ��
 �Ŷ�ұ�ĢȰ��
 �ʶ�ұ�ĢȰ��
 ����ұ�ĢȰ��
 ���ұ�ĢȰ��
 ���ұ�ĢȰ��
 ���ұ�ĢȮ��
 ��ұ�ĢȰ��
 ���ұ�ĢȰ��
*/

/* HwordType.Conj */
#if 0
/* HanChar.h ��  �ű�. HanChar::markSyl_Irregular()  ���� �����ϱ⿡. */
enum {
	D_conj = 1,
	L_conj,
	B_conj,
	S_conj,
	H_conj,
	Go_conj,
	Come_conj,
	Leora_conj,  // ����
	Leo_conj,  // �� 
	Reu_conj,  // ��
	Yeo_conj,  // �� 
	U_conj,  // ��
	Eu_conj  // �� 
};
#endif

/** YBM �������: ǰ�� ����.


 ��ź��
 ������
 ��
 ����
 ����
 ���
 ����
 ��������
 ���������
 �λ�
 �ҿ����ڵ���
 �ҿ���Ÿ����
 ����
 ���
 �������
 �ڵ���
 ���λ�
 ���̻�
 ����
 �ظ�
 Ÿ����
 ������
 ����
 �����
*/

/* HwordType.PS */
#define	HAN_NOUN	(0x01 << 16) 
#define	HAN_VERB	(0x02 << 16)
#define	HAN_ADJ	(0x04 << 16) // �����
#define	HAN_ADVB	(0x08 << 16) // �λ�

#define	HAN_ADJ_KOR	(0x10 << 16) // ������
#define	HAN_EXCL	(0x20 << 16) // ��ź��
#define	HAN_PHRASE	(0x40 << 16) // ��ź��

#define	HAN_EOMI	(0x80 << 16) // ���
#define	HAN_JOSA	(0x0100 << 16) // ���� 
#define	HAN_PFIX	(0x0200 << 16) // ���λ�
#define	HAN_SFIX	(0x0400 << 16) // ���̻�

#define	HAN_VT	(0x01) //  Ÿ����
#define	HAN_INCOMPLETE (0x02) // �ҿ��� 
#define	HAN_AUX	(0x04) // ���� ����/ ���� ����� ..
#define	HAN_DEP	(0x08) // ���� ...  ���� ���.
#define	HAN_NUM	(0x0100) // ����.
#define	HAN_PRONOUN	(0x0200) // ����.

#define	IS_H_VERB(X)	((X) & (HAN_ADJ|HAN_VERB))
#define	IS_H_NOUN(X)	((X) & (HAN_NOUN))

/*
http://woorimal.net/language/moonbub/word-structure-1.htm

������ �ǹ��� ������ ����
        �� �������¼� : ü��, ���ľ�, ������, ����� �.
                ��> ���� ��-�� �ٴٸ� ��-��-��. 

        �� �������¼� : ����, ���, ����
                ��> ö���� �̾߱�å�� ��-��-��. 
*/

#define	IS_H_REAL(X)	((X) & (HAN_ADJ|HAN_ADVB |HAN_ADJ_KOR|HAN_VERB|HAN_NOUN))
/**
HwordType.property: �ܾ� Ư��.
~������.
~�ϴ�.
~��.
*/
#define	HType_Like	(0x01)
#define	HType_Hada	(0x02)
#define	HType_Da	(0x04)

/*
777||��������ϴ�||�������-�ϴ�||
split[0] = 4 �� ��� ����.
*/
typedef struct _hDictType {
	int	id;
	char	*key;
	/* �ܾ ���� ������ �и� */
	byte	split[4];
	int	PS; // ǰ��
	byte	wlen;
	byte	Conj; // Ȱ��
	byte	property; // Ư��.
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
