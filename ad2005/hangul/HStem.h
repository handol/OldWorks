#ifndef HSTEM_H
#define HSTEM_H

#include "HanChar.h"
#include "HDict.h"
#include "HGram.h"
#include "StrStream.h"

/* 
�� �ý����� ���� ���¼� �� ���ϰ�,  ���� ���¼Ҵ� category ȭ �Ͽ� ���� ������ ǥ���Ѵ�.

���� ���¼� == ���� ���¼� : HDict ���� ���� ���� ���� loading
���� ���¼� == ���� ���¼� : HGram ���� ���� ���¼� ���� loading

���¼� �м��� ���� ���� ���¼� �ĺ��� �ִ� ����.

*/
#define	MAX_STEMS	(5)
#define	MAX_HWORDLEN	(30)
#define	MAX_HWORDSYL	(MAX_HWORDLEN/2)  // ���¼� �м��� �ܾ��� �ִ� ���� �� 

/* match type */
enum {
	NotMatch = 0,
	OrgMatch = 1,
	JosaMatch,
	DaMatch,
	HaMatch,
	IrregMatch,
	ComboMatch,
	EtcMatch
};

typedef struct	_stemResult
{
	HwordType *stem;
	uint2	point;
	uint2	match;
	uint2	gramVal; // ���� ���¼��� �з� �� == hGramType.gramVal
} stemResType;

class HStem {
	public:
		HStem();
		void	printStem();
		void	printStem_stream(StrStream &stream);
		void	printBestStem_stream(StrStream &stream);
		stemResType *getBestStem();
		int	stem(char *hword, int hwlen);
		

	private:
		/* ���¼� �м� input �ܾ� ����� �α� */
		char	orgWord[MAX_HWORDSYL];
		int	orgLen;
		int	sylLen;
		
		/* ���¼� �м� ���  ��� */
		stemResType stemRes[MAX_STEMS];
		int	stemResCnt;
		SylInfo *wordInfo[MAX_HWORDSYL];

		int	addStem(HwordType *stem, int	match, int gramVal=0, int point=0);
		
		int	getHead(char *dest, char *orgword, int orglen, hGramType *tail);
		
		int	checkCompound(char *hword, int hwlen);
		int	checkDa(char *hword, int hwlen);
		int	checkHada(char *hword);
		int	checkHa(char *hword, int hwlen);
		/*
		int checkIrregular_S(char *hword, int hwlen);
		int checkIrregular_D(char *hword, int hwlen);
		int	checkIrregular_B(char *hword, int hwlen);
		*/
		int	checkIrregular_Reu(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr);
		
		int searchAddingDa(char *stemstr, int stemlen,  byte Conj);
		
		int checkIrregular_D(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr);
		int checkIrregular_S(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr);
		int checkIrregular_B(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr);
		int checkIrregular_H(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr);
		int checkRegular_L_omit(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr);
		
		int	stemJosa();
		HwordType *qualifyJosa(char *stemstr, int stemlen, hGramType *josa);
		int	stemEomi();

		HwordType *qualifyEomi(char *stemstr, int stemlen, char *eomistr, hGramType *eomi);
		HwordType* checkYongunSuffix(char *stem, int stemlen, hGramType *eomi);

		int  IrregularEomiOnly(SylInfo *fstSylEomi, char*stemstr, int stemlen,  char *eomistr);

		int	IrregularStemOnly(SylInfo *lastSylOfStem, char*stemstr, int stemlen,  char *eomistr);
		int  IrregularStemAndEomi(SylInfo *fstSylEomi, char*stemstr, int stemlen,  char *eomistr);
		//int	handleSuneomalEomi(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr);
		int	handleSuneomalEomi(int currSylPos);
		int	extendDoubleVowel(char *extended, int maxlen);
		int	splitStemEomi(SylInfo *info, char*stem_org, int stem_orglen,  char *eomi_org, int jasoSplittable);

		int	checkStempart(char *hword, int hwlen);
		
};

#endif
