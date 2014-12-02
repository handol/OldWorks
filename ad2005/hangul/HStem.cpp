#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"

#include "HanChar.h"
#include "HStem.h"

HStem::HStem()
{
	stemResCnt = 0;
	memset(stemRes, 0, sizeof(HwordType *) * MAX_STEMS);
}

/*
���Ͽ��� ���¼� �м� ����� ��Ͽ� �߰� �Ѵ�.
*/
int	HStem::addStem(HwordType *stem, int	match, int gramVal, int point)
{
	printf("match[%d] %s - %d\n", stemResCnt, stem->key, match);

	if (point==0)
		point = match;
	if (gramVal==0)
		gramVal = match;
	
	if (stemResCnt < MAX_STEMS) {
		stemRes[stemResCnt].stem = stem;
		stemRes[stemResCnt].match = match;
		stemRes[stemResCnt].gramVal = gramVal;
		stemRes[stemResCnt].point = point;
		stemResCnt++;
		return 1;
	}
	return 0;
}


void	HStem::printStem()
{
	for(int i=0; i<stemResCnt; i++) {
		printf("[%d] %s (%d)\n", i+1, stemRes[i].stem->key, stemRes[i].match);
	}
}

void	HStem::printStem_stream(StrStream &stream)
{
	for(int i=0; i<stemResCnt; i++) {
		stream.add("[%d] %s (%d)\n", i+1, stemRes[i].stem->key, stemRes[i].match);
	}
}

void	HStem::printBestStem_stream(StrStream &stream)
{
	stemResType *best = getBestStem();
	if (best ==  NULL)
	{
		stream.add("NO stem found !!\n");
		return;
	}
	else
	{
		stream.add("Best Stem=%s  match=%d gramval=%d point=%d\n", best->stem->key, best->match, best->gramVal, best->point);
		ACE_DEBUG((LM_INFO,"Best Stem=%s  match=%d gramval=%d point=%d\n", best->stem->key, best->match, best->gramVal, best->point)); 
		return;
	}
}


/**
���� ������ ���¼Ҹ� ���Ѵ�.
match type �� �ܼ��� ���¸� �켱�Ѵ�.
*/
stemResType *HStem::getBestStem()
{
	if (stemResCnt < 1) 
		return NULL;

	stemResType *best= &(stemRes[0]);
	for(int i=1; i<stemResCnt; i++)
	{
		stemResType *curr = &(stemRes[i]);
		if (curr->match != NotMatch 
			&& curr->match < best->match)
			best = curr;
		/*
		if (curr->point > best->point)
			best = curr;
		*/
			
	}
	return best;
}

/**
���� ��� ���� ó��.

������ ������ �и��Ͽ� ���� �˻��� �� ����.

*/
int	HStem::checkCompound( char *hword, int hwlen)
{
	int	len;
	char	words[5][MAXLEN_HWORD+1];
	int	matchcnt = 0;
	HwordType *matchW[5];
	
	for(len=2; len<hwlen-2; len += 2) {
		strncpy(words[0], hword, len);
		words[0][len] = 0;

		strncpy(words[1], hword+len, hwlen-len);
		words[1][hwlen-len] = 0;

		matchW[0] = HDict::search(words[0]);
		matchW[1] = HDict::search(words[1]);
		if (matchW[0] && matchW[1]
			&& (matchW[0]->PS & HAN_NOUN) 
			&& (matchW[1]->PS & HAN_NOUN) 
			) 
		{
			//printf("Combo: %s %s\n", words[0], words[1]);
			matchcnt++;
			addStem(matchW[0], ComboMatch);
			addStem(matchW[1], ComboMatch);
		}
	}
	return matchcnt;
}

/**
��̿� ����� ��� ��ٿ� '��' �� �ٿ� ���� �˻��� ����.
*/
int HStem::checkDa(char *hword, int hwlen)
{
	HwordType *matchW =0;
	char	word[MAXLEN_HWORD+1];
	sprintf(word, "%s��", hword);
	matchW =  HDict::search(word);

	if (matchW) {
		//printf("match: %s\n", matchW->key);
		addStem(matchW, DaMatch);
		return 1;
	}

	
	return 0;
}

/**
���� ������ '�����ϴ�' �� ����Ǿ� �ְ�, '����' �� ���ٸ�...
'����' + '�ϴ�' �� �˻��� ���ƾ� �Ѵ�.
*/
int HStem::checkHada(char *hword)
{
	HwordType *matchW =0;
	char	word[MAXLEN_HWORD+1];
	sprintf(word, "%s�ϴ�", hword);
	matchW =  HDict::search(word);

	if (matchW) {
		//printf("match: %s\n", matchW->key);
		addStem(matchW, HaMatch);
		return 1;
	}

	
	return 0;
}

int HStem::checkHa(char *hword, int hwlen)
{
	HwordType *matchW =0;
	char	word[MAXLEN_HWORD+1];

	/* ~��, ~�ϴ� ���� 2���� �̻� ������ ��� */
	if (hwlen >= 6 && strncmp(hword+hwlen-2, "��", 2)==0) {
		/* '��'�� ���� �ϰ�  �˻� */
		strncpy(word, hword, hwlen-2);
		word[hwlen-2] = 0;
		
		matchW =  HDict::search(word);
		if (matchW) {
			//printf("match: %s\n", matchW->key);
			addStem(matchW, HaMatch);
			return 1;
		}

		/* '~�ϴ�'  �˻� */
		return checkHada(word);
		
	}
	return 0;
}

HwordType* HStem::checkYongunSuffix(char *stemstring, int stemlen, hGramType *eomi)
{
	char stemstr[MAX_HWORDLEN+1];
	HwordType *stem = 0;
	
	strcpy(stemstr, stemstring);
	
	strcpy(stemstr+stemlen, "��");
	stem = HDict::search(stemstr);
	if (stem && IS_H_VERB(stem->PS)) return stem;
	
	if (stemlen <= 2) return 0;
	if (strncmp(stemstr+stemlen-2, "��", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}
	else if (strncmp(stemstr+stemlen-2, "��", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}

	if (stemlen <= 4) return 0;
	
	if (strncmp(stemstr+stemlen-4, "��Ű", 4 )==0) {
		stemstr[stemlen-4] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}
	return 0;
}



/* ��� �ٲ�� �ұ�ĢȰ�� : ��, ��, ��, ��, �� */
/* ��̰� �ٲ�� �ұ�ĢȰ�� : ��, ��, �Ŷ�, �ʶ� */
/* �, ��� ��� ��ȭ : �� */

/**
'��' �� �ٿ� ���� �˻��� �����Ѵ�.

stemstr�� ������ ���ϰ� �ȴ�. 
*/
int HStem::searchAddingDa(char *stemstr, int stemlen, byte Conj)
{
	HwordType *matchW =0;
	char stemtmp[MAX_HWORDLEN+1];
	strncpy(stemtmp, stemstr, stemlen);
	strcpy(stemtmp+stemlen, "��");
	matchW =  HDict::search(stemtmp);

	if (matchW) {
		if (Conj > 0 && matchW->Conj == Conj) {
			addStem(matchW, Conj);
		}
		else {
			addStem(matchW, DaMatch);
		}
		return 1;
	}

	
	return 0;
}

/**
*/
int HStem::checkIrregular_D(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];

	// �� �ұ�Ģ 
	if (IS_SYL_D_IRRE(lastSylOfStem)) {
		printf("checkIrregular_D: %s\n", stemstr2);
		if (strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0) {
			strcpy(stemstr, stemstr2);
			HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_D);	

			if (searchAddingDa( stemstr, stemlen, D_conj)) return D_conj;

		}
	}
	return 0;
}

int HStem::checkIrregular_S(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	
	if (IS_SYL_S_IRRE(lastSylOfStem)) {
		printf("checkIrregular_S: %s\n", stemstr2);
		if (strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0) {
			strcpy(stemstr, stemstr2);
			HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_S);

			if (searchAddingDa( stemstr, stemlen, S_conj)) return S_conj;
		}
	}
	return 0;
}


/**
�� �ұ�Ģ : ��̰� �и��� �Ŀ� ��� ����.
�Ƹ��ٿ� --> �Ƹ��ٿ� + ��

@pre stemstr2 ���� �Ƹ��ٿ�
@pre eomistr ���� ��

*) �Ƹ��ٿ� --> �Ƹ��ٿ� + ��    �̷��� �и��� ������ ȣ��Ǿ�� ��.

*/
int HStem::checkIrregular_B(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	
	// �� 
	// �Ƹ��ٿ� == �Ƹ��ٿ� + ��
	// ��� ��������  �׻� ' �� ' .    
	// IS_SYL_B_IRRE() �ΰ��� ������ �ٷ� �� ����. 
	// 
	
	strcpy(stemstr, stemstr2);
	
	if (strncmp(stemstr+stemlen-2, "��", 2)==0) {
		printf("checkIrregular_B: %s\n", stemstr2);
		
		SylInfo *info = HanChar::getSylInfo(stemstr + stemlen -4);
		HanChar::printWS(info);
		if (IS_SYL_B_IRRE(info)) {
			
			HanChar::replaceLastSnd(stemstr+stemlen-4, info->jhcode, LastSnd_B);

			if (searchAddingDa( stemstr, stemlen-2, B_conj)) return B_conj;
			
		}
	}
	return 0;
}

int HStem::checkIrregular_H(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	HwordType *matchW =0;

	
		
	// �� �ұ�Ģ
	// �Ķ� + �� == �ķ���
	// �Ͼ� + �� == �Ͼ꼭   -- @TODO 
	if (IS_SYL_H_IRRE(lastSylOfStem) && stemlen >= 4) {
		printf("checkIrregular_H: %s\n", stemstr2);
		
		strcpy(stemstr, stemstr2);
		byte new_midsnd;		
		SylInfo *firstSyl = HanChar::getSylInfo(stemstr+stemlen-4);
		if (firstSyl==0) return 0;

		byte midsndOfFirst = JH_MidSnd(firstSyl->jhcode);
		//int	is_positive = Is_Posi_MidSnd(  (JH_MidSnd(firstSyl->jhcode)) );
		int	is_positive = Is_Posi_MidSnd( midsndOfFirst );
		
		// ���� : ������ ���� �ٲٰ�, �� ��ħ�� �ִ´�.
		// replace ������ �ٲٸ�  �ϼ����� 
		if (JH_MidSnd(lastSylOfStem->jhcode)==MidSnd_AE) {
			if (is_positive)
				new_midsnd = MidSnd_A;		// �ķ� --> �Ķ� 
			else
				new_midsnd = MidSnd_EO;		// ���� --> ���� 
				
		}
		else {
			if (is_positive)
				new_midsnd = MidSnd_YA;		// �Ͼ� --> �Ͼ�
			else
				new_midsnd = MidSnd_YEO;		// ��� --> �㿸 
		}
		
		HanChar::replaceMidAndLastSnd(stemstr+stemlen-2,lastSylOfStem->jhcode, new_midsnd, LastSnd_H);
		//printf("checkIrregular_H: %s\n", stemstr);
		
		if (searchAddingDa( stemstr, stemlen, H_conj)) return H_conj;

		
	}
	return 0;
}

/**
�� Ż�� . �ұ�Ģ�� �ƴ϶�
*/
int HStem::checkRegular_L_omit(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	HwordType *matchW =0;
	byte lastSndOfStem = JH_LastSnd(lastSylOfStem->jhcode);

	printf("checkRegular_L_omit: %s\n", stemstr2);

	/* �� Ż��. � �������� ��ħ�� ���� ��� */
	if (lastSndOfStem==LastSnd_Nothing) {
		if (strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0
			|| strncmp(eomistr, "��", 2)==0) 
		{
			strcpy(stemstr, stemstr2);
			HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_L);

			if (searchAddingDa( stemstr, stemlen, 0)) return 1; // �� Ż�� . �ұ�Ģ�� �ƴ϶� ����  ���� 

		}
		
	}
	return 0;
}

/**
�� �ұ�Ģ 
*/
int HStem::checkIrregular_Reu(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)
{
	SylInfo *lastSylOfStem=0;
	
	char eomistr_tmp[MAXLEN_GWORD+1];
	HwordType *stem = 0;
	hGramType *eomi = 0;

	char stemstr[MAX_HWORDLEN+1];
	
	if (strncmp(eomistr, "��", 2)==0  ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "��", 2);
	}
	else if (strncmp(eomistr, "��", 2)==0  ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "��", 2);
	}
	else return 0;
	
	eomi = HGram::search( eomistr_tmp );
	if (eomi==0) return 0;
	

	lastSylOfStem = HanChar::getSylInfo(stemstr2+stemlen-2);
	
	if (lastSylOfStem && IS_SYL_REU_IRRE(lastSylOfStem) ) {
		strcpy(stemstr, stemstr2);
		HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_Nothing);	

		strncpy(stemstr+stemlen, "��", 2);
		return searchAddingDa(stemstr, stemlen+2, Reu_conj);
	}

	return 0;
}


/**
��� ��縸 �ٲ�� �ұ�Ģ ������ �м� �˰���.

5���� :  ��, ��, ��, �� �ұ�Ģ. �� Ż�� ���� 
*/
int	HStem::IrregularStemOnly(SylInfo *lastSylOfStem, char*stemstr, int stemlen,  char *eomistr)
{
	checkIrregular_D(lastSylOfStem, stemstr, stemlen, eomistr);
	checkIrregular_S(lastSylOfStem, stemstr, stemlen, eomistr);
	checkIrregular_B(lastSylOfStem, stemstr, stemlen, eomistr);
	checkIrregular_H(lastSylOfStem, stemstr, stemlen, eomistr);
	checkRegular_L_omit(lastSylOfStem, stemstr, stemlen, eomistr);
	return 0;
}

/**
����� ��縸 �ٲ�� �ұ�Ģ ������ �м� �˰���

���� �˻� ���� ������ eomistr �� ù�κп� ���ԵǾ� �´�.

4 ���� : ��, �Ŷ�, �ʶ�, �� 

�� �ұ�Ģ : �̸��� = �̸� + ��
�� : -��- �� ������ ��𿡼��� �߻� 
*/
int  HStem::IrregularEomiOnly(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)
{
	char eomistr_tmp[MAXLEN_GWORD+1];
	HwordType *stem = 0;
	hGramType *eomi = 0;
	HwordType *matchW =0;

	char stemstr[MAX_HWORDLEN+1];
	
	strcpy(stemstr, stemstr2);
	
	if (strncmp(eomistr, "��", 2)==0 && strncmp(stemstr+stemlen-2, "��", 2)==0 ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "��", 2);

		eomi = HGram::search( eomistr_tmp );

		if (eomi) {
			
			searchAddingDa(stemstr, stemlen, Leo_conj);
			
		}
	}

	else if (strncmp(eomistr, "�Ŷ�", 4)==0 && strncmp(stemstr+stemlen-2, "��", 2)==0 ) {
		
		searchAddingDa(stemstr, stemlen, Go_conj);
	}

	else if (strncmp(eomistr, "�ʶ�", 4)==0 && strncmp(stemstr+stemlen-2, "��", 2)==0 ) {
		
		searchAddingDa(stemstr, stemlen, Come_conj);
	}

	else {
		//SylInfo *fstSylEomi = HanChar::getSylInfo(eomistr);
		if (JH_MidSnd(fstSylEomi->jhcode)==MidSnd_YEO && JH_FstSnd(fstSylEomi->jhcode)==FstSnd_Nothing ) {
			if (strncmp(stemstr+stemlen-2, "��", 2)==0) {
				searchAddingDa(stemstr, stemlen, Yeo_conj);
			}
		}
	}
	return 0;
}


/**
�, ��� ���  �ٲ�� �ұ�Ģ ������ �м� �˰���.

���� :
�� Ż�� :  �� = �� + ��    ��� = ���� + ��
Ǫ �ұ�Ģ : �� = Ǫ + ��
�� �ұ�Ģ : ���� = �� + ��  ��� = �� + ��
�� �ұ�Ģ : �귯 = �帣 + ��

*/
int	HStem::IrregularStemAndEomi(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)

{
	checkIrregular_Reu(fstSylEomi, stemstr2, stemlen, eomistr);
	return 0;
}

/**
orgWord ���� ������ �κ��� �и�/Ȯ���Ͽ� 
���ο� ���ڿ�����  extended �� �����Ѵ�.
extended �� �ִ� ���̴� maxlen �̴�.
*/
int	HStem::extendDoubleVowel(char *extended, int maxlen)
{
	int	s; // ���� ��ġ
	int	i; 
	int	copied; 
	char	split[5]; // ����� ������ 2������ �и� ����.
	byte	fstsnd, midsnd, lastsnd;
	SylInfo	*info;
	
	for(s=0, i=0, copied=0; s < sylLen && copied<maxlen; s++, i+=2) {

		info = wordInfo[s];
		fstsnd = JH_FstSnd(info->jhcode);		
		midsnd = JH_MidSnd(info->jhcode);
		lastsnd = JH_LastSnd(info->jhcode);

		int	splited = 0;
		if (HanChar::splitHanSyllable(split, info->jhcode)) {
			if (midsnd==MidSnd_A && lastsnd==LastSnd_SS) {
			}
			else {
				splited = 1;
			}
		}

		if (splited) {
			strncpy(extended, split, 4);
			extended += 4;
			copied += 4;
		}
		else {
			strncpy(extended, orgWord+i, 2);
			extended += 2;
			copied += 2;
		}

	}
	*extended = 0;
	return copied;
}

/**
��� ��� ó��.
��� ��� == prefinal-eomi

�ַ� '��' ��ħ.

ex) �� ��  ��   �� �� �� 

����̰ڴ� ??

stemstr -- �ܾ� �պκ� 
stemlen -- �ܾ� �պκ� ����
eomistr
*/
//int	HStem::handleSuneomalEomi(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr2)
int	HStem::handleSuneomalEomi(int currSylPos)
{
	int	currBytePos = currSylPos + currSylPos; 
	int	sylpos; // ���� ��ġ
	int	bytepos; 
	SylInfo *info;
	char *curr =0;

	char	split[5] = {0}; // ����� ������ 2������ �и� ����.
	char eomi_hidden[3] = {0};
	
	char	eomistr[MAX_HWORDLEN+1];
	char	stemstr[MAX_HWORDLEN+1];

	#define	SUNEOMAL_FUTURE	(0x01)
	#define	SUNEOMAL_PAST		(0x02)
	#define	SUNEOMAL_HONOR	(0x04)
	
	int	suneomalType=0;
	

	for(  sylpos=currSylPos, bytepos=currBytePos; 
		sylpos >= 0; 
		sylpos--, bytepos-=2 ) 
	{
		
		info = wordInfo[sylpos];
		curr = orgWord + bytepos;
		printf("\tsylpos=%d, word=%s\n", sylpos, curr);
		
		if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_HONOR;
		}
		else if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_HONOR;
			strcpy(eomi_hidden, "��");
		}
		else if (strncmp(curr, "��", 2)==0) {
		}
		else  if ( JH_LastSnd(info ->jhcode) != LastSnd_SS) {
			break;
		}
		
		if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_FUTURE;
		}
		else if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "��", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "��", 2)==0) {
			suneomalType |= (SUNEOMAL_HONOR | SUNEOMAL_PAST );
		}		
		else if ( HanChar::splitHanSyllable(split, info->jhcode) )  {
			suneomalType |= SUNEOMAL_PAST;
			break;
		}
	}

	/*
	 * ���� ���̰� 3������ �Ǿ�� ��� ��̰� �ִ� ���̴�.
	 * ����) �� ���� !
	 */
	if (suneomalType && sylLen >= 2) 
	{
		strncpy(stemstr, orgWord, currBytePos);
		if (split[0]) {
			strncpy(stemstr+ currBytePos, split, 2);
			stemstr[currBytePos+2] = 0;
		}
		else {		
			stemstr[currBytePos] = 0;
		}

		if (eomi_hidden[0]) {
			sprintf(eomistr, "%s%s", eomi_hidden, orgWord + (currBytePos + 2) );
		}
		else {
			strcpy(eomistr, orgWord + (currBytePos + 2 ) );
		}

		printf("handleSuneomalEomi:: %s %s\n", stemstr, eomistr);
		
		/*
		� , ��� �и��� �õ�
		*/
		if (eomistr[0]==0) // ��̰� ���� ���. ��� ��̵� ���� ���̴�.
		{
			return 0;
		}
		info = HanChar::getSylInfo(eomistr);
		if (info==NULL)
			return 0;
		splitStemEomi(info, stemstr, strlen(stemstr), eomistr, 0 );		
		
	}
	else {
		currBytePos = currSylPos<<1;
		strncpy(stemstr, orgWord, currBytePos);
		stemstr[currBytePos] = 0;
		strcpy(eomistr, orgWord + currBytePos );
		/*
		� , ��� �и��� �õ�
		*/
		if (eomistr[0]==0) // ��̰� ���� ���. 
		{
			return 0;
		}
		info = HanChar::getSylInfo(eomistr);
		if (info==NULL)
			return 0;
		splitStemEomi(info, stemstr, currBytePos, eomistr, 1 );
	}
		
	printf("handleSuneomalEomi <>\n");
	return 0;
}

/**
ü�� ���� ���� �˻�. ���� �˻�.
*/
HwordType *HStem::qualifyJosa(char *stemstr, int stemlen, hGramType *josa)
{
	HwordType *stem=0;

	if ((josa->property & GW_BUNGAN) && (josa->property & GW_CHOSUNG)) {
		SylInfo *lastsyl = HanChar::getSylInfo(stemstr + stemlen -2);
		if (lastsyl && Has_LastSnd(lastsyl->jhcode) ) {
			printf("%s + %s : �а����� ���� ���� \n", stemstr, josa->key);
			return 0;
		}
	}
	stem = HDict::search(stemstr);
	if (stem && IS_H_NOUN(stem->PS)) return stem;
	
	if (strncmp(stemstr+stemlen-2, "��", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem && IS_H_NOUN(stem->PS)) return stem;
	}
	return 0;
}

/**
 ������ �޺κп������� �˻��ϸ� ������ �̵�. 

 for loop:
   ���� ������ ������ ù������ �ش�Ǹ�
     ���� �����˻�.
     
   ���� ������ ��ħ�� ��, �� �̸� 
     ���� ���� �˻�.

   �̷����ؼ� �����ĺ��� �߰��ϸ�
     ����� �ܾ� �������� �˻�
     
*/
int	HStem::stemJosa()
{
	int	s; // ���� ��ġ
	int	i; 
	SylInfo *info;
	byte	lastsnd;
	char	josa[MAX_HWORDLEN+1];
	char	stem[MAX_HWORDLEN+1];
	int	stemlen = 0;
	hGramType *josa_c; //���� �ĺ� 
	int	sylchanged=0; // ��ħ �и� ������ �������� ������ ��� true.
	int	cnt=0;
	
	for(s=sylLen-1, i=orgLen-2; s>=0; s--, i-=2 ) {
		info = wordInfo[s];
		lastsnd = JH_LastSnd(info->jhcode);
		if ( info->property & P_JOSA1) {
			josa_c = HGram::search( orgWord+ i );	
		}
		else if (lastsnd==LastSnd_N || lastsnd == LastSnd_L) {
		/* ��, �� ��ħ : ���� �ĺ� */
			strncpy(josa, HanChar::HLastSnd[lastsnd].draw, 2);
			josa[2] = 0;
			strcpy(josa+2, orgWord+ i+2 );
			sylchanged = 1;
			josa_c = HGram::search( josa );	
		}
		else continue;

		/*
		�̷����ؼ� �����ĺ��� �߰��ϸ�
     		����� �ܾ� �������� �˻�
     		*/
     		
		if (josa_c && (josa_c->PS & GW_JOSA) ) {
			strncpy(stem, orgWord, i);
			stem[i] = 0;
			stemlen = i;
			
			if (sylchanged) {				
				uint2 new_jhcode = 
					JH_replaceLastSnd(info->jhcode, LastSnd_Nothing);		
				HanChar::jh2string( new_jhcode, stem+i);
				stem[i+2] = 0;
				stemlen = i + 2;
			}
			
			if (stemlen > 0) {
				HwordType *stemWithJosa = qualifyJosa(stem, stemlen, josa_c);
				if (stemWithJosa)
				{
					cnt++;
					printf("J [%d] %s + %s\n", cnt, stem, josa_c->key); 
					addStem(stemWithJosa, josa_c->property, josa_c->gramVal, josa_c->freqJosa);
				}
			}

		}
		else continue;
		
	}
	return cnt;
}



/**
���  ���� ���� �˻�. ���� �˻�.
@param stemstr ��� 
@param eomi ��� ���� (���� �������� ã�� ��)
*/
//HwordType *HStem::qualifyEomi(char *stemstr, int stemlen, hGramType *eomi)
HwordType *HStem::qualifyEomi(char *stemstr, int stemlen, char *eomistr, hGramType *eomi)
{
	HwordType *stem=0;
	SylInfo *lastSylOfStem = HanChar::getSylInfo(stemstr + stemlen -2);

	if (stemlen==0) return 0;

	printf("qualify E [%d] %s + %s\n", 0, stemstr, eomistr); 
	
	if ((eomi->property & GW_BUNGAN) && (eomi->property & GW_CHOSUNG)) {
		
		if (lastSylOfStem && Has_LastSnd(lastSylOfStem->jhcode) ) {
			printf("%s + %s : �а���̿��� ���� \n", stemstr, eomistr);
			return 0;
		}
	}
	
	stem = HDict::search(stemstr);
	// ����� �ܾ� �������� ã�� ���.
	// @TODO ��̰� ��, ��, ��, �� ���� �ڼ� �ϳ� �� ���� '~��' �� �ƴ� ���� ����� �ƴ� ���̴� 
	// �Ͼ� == �Ͼ� + �� (X)

	/*
	if (stem && IS_H_VERB(stem->PS)) {
		addStem(stem, 0);
	}
	*/
	if (stem && IS_H_VERB(stem->PS))
	{
		addStem(stem, eomi->property, eomi->gramVal, eomi->freqEomi);
		return stem;
	}

		
	// ����� �ܾ� ������ ���� ��쿡��
	// �ұ�Ģ �˻� �� ���ȭ ���̻縦 �˻� �Ѵ�.

	// �ұ�Ģ �˻� 
	IrregularStemOnly(lastSylOfStem, stemstr, stemlen, eomistr);

	// ���ȭ ���̻� �˻�
	stem = checkYongunSuffix(stemstr, stemlen, eomi);
	if (stem) {
		addStem(stem, DaMatch);
		return stem;
	}
	
	return 0;
}

/**
��� ��̸� ������ �Ŀ�  ��� + ��� �м�.

info -- ����� ù ������ ����.
*/
int	HStem::splitStemEomi(SylInfo *info, char*stem_org, int stem_orglen,  char *eomi_org,  int jasoSplittable)
{
	byte	fstsnd, midsnd, lastsnd;
	char	eomistr[MAX_HWORDLEN+1];
	char	stemstr[MAX_HWORDLEN+1];
	char	split[5]; // ����� ������ 2������ �и� ����.
	int	stemlen = 0;
	hGramType *eomistr_c; //��� �ĺ� 

	fstsnd = JH_FstSnd(info->jhcode);		
	midsnd = JH_MidSnd(info->jhcode);
	lastsnd = JH_LastSnd(info->jhcode);

	/*
	����� ��縸 �ٲ�� �ұ�Ģ ���� ó�� : 
	4 ���� : ��, �Ŷ�, �ʶ�, �� 	
	*/
	IrregularEomiOnly(info, stem_org, stem_orglen, eomi_org );


	/*
	�, ��� ���  �ٲ�� �ұ�Ģ ������ �м� 
	*/
	IrregularStemAndEomi(info, stem_org, stem_orglen, eomi_org );
		
	if ( info->property & P_EOMI1) {
		strcpy(eomistr, eomi_org );
		eomistr_c = HGram::search( eomistr);

		if (eomistr_c) {
			/*��� ���� */
			strncpy(stemstr, stem_org, stem_orglen);
			stemstr[stem_orglen] = 0;
			
			qualifyEomi(stemstr, stem_orglen, eomistr, eomistr_c);	

		}
		
	}

	/* ��, ��, ��, �� ��ħ : ��� �ĺ� 
	�ڸ�� �����ϴ� ��� ���� �˻�. 
	*/
	//else
	if (jasoSplittable && lastsnd==LastSnd_N || lastsnd == LastSnd_L
			|| lastsnd==LastSnd_M || lastsnd == LastSnd_B) 
	{
	
		/* ��̺� ���� */
		strncpy(eomistr, HanChar::HLastSnd[lastsnd].draw, 2);
		strcpy(eomistr+2, eomi_org+2);

		eomistr_c = HGram::search( eomistr );		

		if (eomistr_c) {
			/*��� ���� */
			strncpy(stemstr, stem_org, stem_orglen);
			HanChar::replaceLastSnd(stemstr+stem_orglen, info->jhcode, LastSnd_Nothing);
			stemstr[stem_orglen+2] = 0;
			qualifyEomi(stemstr, stem_orglen + 2, eomistr, eomistr_c);	

		}
	}

	/* ��, �� ��� �˻� */
	// @TODO
	//else 
	else if ((midsnd==MidSnd_A || midsnd==MidSnd_EO) && lastsnd==LastSnd_Nothing) {
		/* ��̺� ���� */
		if (midsnd==MidSnd_A)
		strcpy(eomistr, "��");
		else
		strcpy(eomistr, "��");
		strcpy(eomistr+2, eomi_org+2);

		eomistr_c = HGram::search( eomistr );

		/*��� ���� */
		strncpy(stemstr, orgWord, stem_orglen);
		stemstr[stem_orglen] = 0;

		if (eomistr_c) {
		
			if (stem_orglen > 0 && qualifyEomi(stemstr, stem_orglen, eomistr, eomistr_c)) {
			}
				

		}
	}

	/* ������ ���� ���� ���� ������ ���.
	���� ��� �̶�� ���������� Ī��.
	��� ��ο� ��̰� ������ �� ���� ��� ���� �߻��ϹǷ�,
	����� �����Ͽ� ��̸� ã�ƾ� �Ѵ�.
	*/
	//else 
	
	if (lastsnd == LastSnd_SS &&
		HanChar::splitHanSyllable(split, info->jhcode))
	{
		/* ��̺� ���� */
		
		strncpy(eomistr, split+2, 2);				
		strcpy(eomistr+2, eomi_org+2 );
		eomistr_c = HGram::search( eomistr );
		
		/*��� ���� */
		strncpy(stemstr, stem_org, stem_orglen);
		strncpy(stemstr+stem_orglen, split, 2);
		stemstr[stem_orglen+2] = 0;

		printf("\t double vowel: %s = %s + %s\n", split, stemstr, eomistr);

		if (eomistr_c) {		
			if (stemlen > 0 && qualifyEomi(stemstr, stem_orglen+2, eomistr, eomistr_c)) {
			}

		}
		
	}

	return 0;
}
	
/**
������ �޺κп������� �˻��ϸ� ������ �̵�. 
*/
int	HStem::stemEomi()
{
	int	s; // ���� ��ġ
	int	i; 
	SylInfo *info;
	byte	fstsnd, midsnd, lastsnd;
	char	eomistr[MAX_HWORDLEN+1];
	char	stemstr[MAX_HWORDLEN+1];
	int	stemlen = 0;
	hGramType *eomistr_c; //��� �ĺ� 
	int	sylchanged=0; // ��ħ �и� ������ �������� ������ ��� true.
	int	cnt=0;

	/* ������ �޺κп������� �˻��ϸ� ������ �̵�. 
	*/
	
	for(s=sylLen-1, i=orgLen-2; s>=0; s--, i-=2 ) {
		printf("Syl %d/%d i=%d  Ending=%s org=%s\n", s, sylLen, i, 
				orgWord+i, orgWord);

		stemstr[0] = 0;
		eomistr[0] = 0;
		eomistr_c = 0;
		
		info = wordInfo[s];
		fstsnd = JH_FstSnd(info->jhcode);		
		midsnd = JH_MidSnd(info->jhcode);
		lastsnd = JH_LastSnd(info->jhcode);

		if (s < orgLen - 1) {
			handleSuneomalEomi(s);
		}

		/*
		� , ��� �и��� �õ�
		*/
		//splitStemEomi(info, orgWord, i, orgWord+ i, 1 );		
		
	}
	return cnt;
}


/**
org ���ڿ��� ��  ���ڸ� ��ħ���� ���ڷ� ��ȯ.
*/
int	HStem::getHead(char *dest, char *orgword, int orglen, hGramType *tail)
{
	int headlen=0;

	if (orglen >= 2 && IS_JAMOHEAD(tail)) {
		headlen = orglen - tail->wlen + 2;
		strncpy(dest, orgword, headlen);
		dest[headlen] = 0;
		HanChar::replaceLastSnd(dest+headlen-2);
	}
	else {
		headlen = orglen - tail->wlen;
		strncpy(dest, orgword, headlen);
		dest[headlen] = 0;
	}
	return headlen;
}

/**
�ܾ��� �� 2, 3 ���������� ���� ���� �˻��� �غ���. 
*/
int	HStem::checkStempart(char *hword, int hwlen)
{
	char tmphword[64];
	
	if (hwlen >= 4)
	{
		STRNCPY(tmphword, hword, 4);
		HwordType *dtword = HDict::search(tmphword);
		if (dtword) {
			addStem(dtword, OrgMatch);
			//addStem(dtword, EtcMatch);
			return 1;
		}
	}

	if (hwlen >= 6)
	{
		STRNCPY(tmphword, hword, 6);
		HwordType *dtword = HDict::search(tmphword);
		if (dtword) {
			addStem(dtword, OrgMatch);
			//addStem(dtword, EtcMatch);
			return 1;
		}
	}

	return 0;
}

/**
�ϳ��� ������ �м��Ͽ� ���� ���¼� �� ã�� ã�� ������ �Ѵ�.

���� ���� �� ������ �յ� �м� ����� ���� Ȱ���ϴ� ���� �� ��Ȯ�� �м��� �� �� �ִ�.
--> ���� ���� �м� �ʿ�. 

0. ���� �׷��� ���� �˻� 
1. ����, ��� �˻�
2. ~�� �˻�
3. �ұ�Ģ Ȱ�� �˻�
4. ��� ���, � ��� �˻�

4. ���վ� �˻�
5. ���ξ�, ���̾� �˻�
6. ��� ���� ���� �˻�

*/
int	HStem::stem(char *hword, int hwlen)
{
	hGramType *matchGW[MAX_GW_MATCH] = {0};
	int	matchCnt = 0;

	//int i;
	//char	headpart[MAXLEN_HWORD+1];
	//char	tailpart[MAXLEN_HWORD+1];
	HwordType	*dtword = 0;
	
	if (hwlen == 0) 
		hwlen = strlen(hword);
	strcpy(orgWord, hword);
	orgLen = hwlen;
	sylLen = hwlen >> 1;
	printf("HStem: orgLen=%d sylLen=%d orgWord=%s\n", orgLen, sylLen, orgWord);

	if (hwlen % 2 == 1)
		return 0;
	
	int goodHangulCharLeng = HanChar::getSylInfo(wordInfo, MAX_HWORDSYL, hword);
	if (goodHangulCharLeng != sylLen)
	{
	/* ���� ���� �ѱ� �ƴ� ���ڰ� ���Ե� ��� �̴�. */
		hword[goodHangulCharLeng] = 0 ;
		hwlen = strlen(hword);
		orgLen = hwlen;
		sylLen = hwlen >> 1;

		printf("HStem: goodHangulCharLeng=%d orgLen=%d sylLen=%d orgWord=%s\n", 
				goodHangulCharLeng, orgLen, sylLen, orgWord);

		if (hwlen==0)
			return 0;
	}

	printf("\n== %s\n", hword);

	// ���� �״�� ������ �ִ� ���
	dtword = HDict::search(hword);
	if (dtword) {
		addStem(dtword, OrgMatch);
	}

	printf("=== Josa ===\n");
	stemJosa();

	if (stemResCnt > 0 && strlen(stemRes[0].stem->key) >= 4)
	{
		// 2 �� �̻��� �ܾ ã�Ҵٸ� �� �̻� ��� ����� �ʿ���ڴ�.
		return 0;
	}
	printf("=== Eomi ===\n");
	stemEomi();

	if (stemResCnt == 0)
	{
		checkStempart(hword, hwlen);
	}
	
	/*
		if (!stemFound && (matchGW[i]->PS & GW_EOMI)) {
			stemFound = checkDa(headpart, headlen);			
		}
		if (!stemFound && (matchGW[i]->PS & GW_EOMI)) {
			stemFound = checkHa(headpart, headlen);			
		}		
		if (headlen >= 4) {
			stemFound = checkCompound (headpart, headlen);
		}
	*/
		
	return 0;
	
}
