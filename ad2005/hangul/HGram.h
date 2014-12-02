#ifndef HGRAM_H
#define HGRAM_H

#include "simpleHash.h"
#include "StrStream.h"

#include "HanChar.h"
#include "HDict.h"


#define	MAXLEN_HWORD	(30) //�ѱ� �ܾ��� �ִ� ���� byte
#define	MAXLEN_GWORD	(20)

/* �ѱ��� ���¼� �м������� ���� ���� �ĺ��� �м� ����� ���� ���� */
#define	MAX_GW_MATCH	(5)  // �ϳ��� �ѱ� ������ ���� �ִ� match �� �� �ִ� ������ �ܾ� ����

/*-- ����, ��� ���� ������ ��� ó���� ���� -- */
#define	MAX_GWORD	(1500)  // Eomi_all.txt, Josa_all.txt  ���� �� �պ��� ���� 
#define	MAX_JOSANUM	(500)
#define	MAX_EOMINUM	(1000)

#define	GWORD_FREQ_MARK	(0x01)

/* ���� ���¼� Ư�� */
//#define	GW_FREQ	(0x01)
#define	GW_JAMOHEAD	(0x08) // ���� ���¼Ұ� �ڸ�� �����ϴ� ��� ����. ��, ��, ��, �� ����.	
#define	GW_CHOSUNG	(0x01) // ù ������ �ʼ��� �ִ� ��.  ~�� == 0, ~�� ==1
#define	GW_BUNGAN	(0x40)	// �а�����/��� . �������� ���� ������ ���� (��)�� ó�� ���еǴ� ����/���.


/* ���� ���¼� ǰ�� */
#define	GW_JOSA	(0x01)
#define	GW_EOMI	(0x02)


/* hGramType *X */
#define	IS_JAMOHEAD(X)	((X)->PS & GW_JAMOHEAD)

typedef struct _hGramType {
//	int	id;
	char	*key;
	char *invert;
	SylInfo **infos;
	uint2	PS; // ǰ��
	uint2 	freqJosa; // �󵵼�  (����� x 10000: 0.1 % == 10)
	uint2	property; /* ���� ���¼� Ư�� */
	uint2 	freqEomi;
	uint2	gramVal; // ����, ��̸� �з�  �� : ���� ���¼��� �з� ��
	byte	wlen;
	
} hGramType;

typedef struct _stemGuessType {
	char	stem[MAXLEN_HWORD+1];
} stemGuessType;

/*
������  ���� ���¼� + ���� ���¼ҷ� �и��ϱ� ����
���� ������ ����/��� ������ �˻��Ͽ� 
(ü�� + ���� ) �Ǵ� ( ��� + ��� ) �� match �� ���.

���¼� �м� ����� �ĺ��� �ȴ�.
*/
typedef struct _matchResType {
	// ���� ������ ����/ ��� �κ��� ������ ���� �κ�. �� ������ ���� ���� �޶��� �� �ִ�.
	char	stem[MAXLEN_HWORD+1]; 
	byte kind; // match ���� : 
	// ����, ��� 
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
		static	hGramType **josaArray; // ���� 
		static	hGramType **josaArray_invert;
		static	hGramType **eomiArray; // ��� 
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
