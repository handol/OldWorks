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
구하여진 형태소 분석 결과를 목록에 추가 한다.
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
가장 적합한 형태소를 구한다.
match type 이 단순한 형태를 우선한다.
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
복합 명사 등의 처리.

음절을 적당히 분리하여 사전 검색을 해 본다.

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
어미와 연결된 경우 어근에 '다' 를 붙여 사전 검색해 본다.
*/
int HStem::checkDa(char *hword, int hwlen)
{
	HwordType *matchW =0;
	char	word[MAXLEN_HWORD+1];
	sprintf(word, "%s다", hword);
	matchW =  HDict::search(word);

	if (matchW) {
		//printf("match: %s\n", matchW->key);
		addStem(matchW, DaMatch);
		return 1;
	}

	
	return 0;
}

/**
국어 사전에 '공부하다' 만 등재되어 있고, '공부' 가 없다면...
'공부' + '하다' 를 검색해 보아야 한다.
*/
int HStem::checkHada(char *hword)
{
	HwordType *matchW =0;
	char	word[MAXLEN_HWORD+1];
	sprintf(word, "%s하다", hword);
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

	/* ~하, ~하다 에서 2음절 이상 어휘인 경우 */
	if (hwlen >= 6 && strncmp(hword+hwlen-2, "하", 2)==0) {
		/* '하'를 제거 하고  검색 */
		strncpy(word, hword, hwlen-2);
		word[hwlen-2] = 0;
		
		matchW =  HDict::search(word);
		if (matchW) {
			//printf("match: %s\n", matchW->key);
			addStem(matchW, HaMatch);
			return 1;
		}

		/* '~하다'  검색 */
		return checkHada(word);
		
	}
	return 0;
}

HwordType* HStem::checkYongunSuffix(char *stemstring, int stemlen, hGramType *eomi)
{
	char stemstr[MAX_HWORDLEN+1];
	HwordType *stem = 0;
	
	strcpy(stemstr, stemstring);
	
	strcpy(stemstr+stemlen, "다");
	stem = HDict::search(stemstr);
	if (stem && IS_H_VERB(stem->PS)) return stem;
	
	if (stemlen <= 2) return 0;
	if (strncmp(stemstr+stemlen-2, "하", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}
	else if (strncmp(stemstr+stemlen-2, "되", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}

	if (stemlen <= 4) return 0;
	
	if (strncmp(stemstr+stemlen-4, "시키", 4 )==0) {
		stemstr[stemlen-4] = 0;
		stem = HDict::search(stemstr);
		if (stem) return stem;
	}
	return 0;
}



/* 어간이 바뀌는 불규칙활용 : ㅅ, ㅂ, ㄷ, 르, 우 */
/* 어미가 바뀌는 불규칙활용 : 여, 러, 거라, 너라 */
/* 어간, 어미 모두 변화 : ㅎ */

/**
'다' 를 붙여 사전 검색을 수행한다.

stemstr의 내용이 변하게 된다. 
*/
int HStem::searchAddingDa(char *stemstr, int stemlen, byte Conj)
{
	HwordType *matchW =0;
	char stemtmp[MAX_HWORDLEN+1];
	strncpy(stemtmp, stemstr, stemlen);
	strcpy(stemtmp+stemlen, "다");
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

	// ㄷ 불규칙 
	if (IS_SYL_D_IRRE(lastSylOfStem)) {
		printf("checkIrregular_D: %s\n", stemstr2);
		if (strncmp(eomistr, "아", 2)==0
			|| strncmp(eomistr, "어", 2)==0
			|| strncmp(eomistr, "으", 2)==0) {
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
		if (strncmp(eomistr, "아", 2)==0
			|| strncmp(eomistr, "어", 2)==0
			|| strncmp(eomistr, "으", 2)==0) {
			strcpy(stemstr, stemstr2);
			HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_S);

			if (searchAddingDa( stemstr, stemlen, S_conj)) return S_conj;
		}
	}
	return 0;
}


/**
ㅂ 불규칙 : 어미가 분리된 후에 사용 가능.
아름다운 --> 아름다우 + ㄴ

@pre stemstr2 에는 아름다우
@pre eomistr 에는 ㄴ

*) 아름다워 --> 아름다우 + 어    이렇게 분리된 다음에 호출되어야 함.

*/
int HStem::checkIrregular_B(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	
	// ㅂ 
	// 아름다운 == 아름다우 + ㄴ
	// 어간의 마지막은  항상 ' 우 ' .    
	// IS_SYL_B_IRRE() 인것은 마지막 바로 전 어절. 
	// 
	
	strcpy(stemstr, stemstr2);
	
	if (strncmp(stemstr+stemlen-2, "우", 2)==0) {
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

	
		
	// ㅎ 불규칙
	// 파랗 + 서 == 파래서
	// 하얗 + 서 == 하얘서   -- @TODO 
	if (IS_SYL_H_IRRE(lastSylOfStem) && stemlen >= 4) {
		printf("checkIrregular_H: %s\n", stemstr2);
		
		strcpy(stemstr, stemstr2);
		byte new_midsnd;		
		SylInfo *firstSyl = HanChar::getSylInfo(stemstr+stemlen-4);
		if (firstSyl==0) return 0;

		byte midsndOfFirst = JH_MidSnd(firstSyl->jhcode);
		//int	is_positive = Is_Posi_MidSnd(  (JH_MidSnd(firstSyl->jhcode)) );
		int	is_positive = Is_Posi_MidSnd( midsndOfFirst );
		
		// 주의 : 모음을 먼저 바꾸고, ㅎ 받침을 넣는다.
		// replace 순서를 바꾸면  완성형에 
		if (JH_MidSnd(lastSylOfStem->jhcode)==MidSnd_AE) {
			if (is_positive)
				new_midsnd = MidSnd_A;		// 파래 --> 파랗 
			else
				new_midsnd = MidSnd_EO;		// 누래 --> 누렇 
				
		}
		else {
			if (is_positive)
				new_midsnd = MidSnd_YA;		// 하얘 --> 하얗
			else
				new_midsnd = MidSnd_YEO;		// 허얘 --> 허옇 
		}
		
		HanChar::replaceMidAndLastSnd(stemstr+stemlen-2,lastSylOfStem->jhcode, new_midsnd, LastSnd_H);
		//printf("checkIrregular_H: %s\n", stemstr);
		
		if (searchAddingDa( stemstr, stemlen, H_conj)) return H_conj;

		
	}
	return 0;
}

/**
ㄹ 탈락 . 불규칙이 아니라
*/
int HStem::checkRegular_L_omit(SylInfo *lastSylOfStem, char*stemstr2, int stemlen,  char *eomistr)
{
	char stemstr[MAX_HWORDLEN+1];
	HwordType *matchW =0;
	byte lastSndOfStem = JH_LastSnd(lastSylOfStem->jhcode);

	printf("checkRegular_L_omit: %s\n", stemstr2);

	/* ㄹ 탈락. 어간 마지막에 받침이 없는 경우 */
	if (lastSndOfStem==LastSnd_Nothing) {
		if (strncmp(eomistr, "ㄴ", 2)==0
			|| strncmp(eomistr, "ㅂ", 2)==0
			|| strncmp(eomistr, "ㅅ", 2)==0
			|| strncmp(eomistr, "오", 2)==0) 
		{
			strcpy(stemstr, stemstr2);
			HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_L);

			if (searchAddingDa( stemstr, stemlen, 0)) return 1; // ㄹ 탈락 . 불규칙이 아니라 값이  없다 

		}
		
	}
	return 0;
}

/**
르 불규칙 
*/
int HStem::checkIrregular_Reu(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)
{
	SylInfo *lastSylOfStem=0;
	
	char eomistr_tmp[MAXLEN_GWORD+1];
	HwordType *stem = 0;
	hGramType *eomi = 0;

	char stemstr[MAX_HWORDLEN+1];
	
	if (strncmp(eomistr, "러", 2)==0  ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "어", 2);
	}
	else if (strncmp(eomistr, "라", 2)==0  ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "아", 2);
	}
	else return 0;
	
	eomi = HGram::search( eomistr_tmp );
	if (eomi==0) return 0;
	

	lastSylOfStem = HanChar::getSylInfo(stemstr2+stemlen-2);
	
	if (lastSylOfStem && IS_SYL_REU_IRRE(lastSylOfStem) ) {
		strcpy(stemstr, stemstr2);
		HanChar::replaceLastSnd(stemstr+stemlen-2, lastSylOfStem->jhcode, LastSnd_Nothing);	

		strncpy(stemstr+stemlen, "르", 2);
		return searchAddingDa(stemstr, stemlen+2, Reu_conj);
	}

	return 0;
}


/**
어간의 모양만 바뀌는 불규칙 어절의 분석 알고리즘.

5가지 :  ㄷ, ㅅ, ㅂ, ㅎ 불규칙. ㄹ 탈락 현상 
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
어미의 모양만 바뀌는 불규칙 어절의 분석 알고리즘

현재 검사 중인 음절이 eomistr 의 첫부분에 포함되어 온다.

4 가지 : 러, 거라, 너라, 여 

러 불규칙 : 이르러 = 이르 + 어
여 : -하- 로 끝나는 용언에서만 발생 
*/
int  HStem::IrregularEomiOnly(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)
{
	char eomistr_tmp[MAXLEN_GWORD+1];
	HwordType *stem = 0;
	hGramType *eomi = 0;
	HwordType *matchW =0;

	char stemstr[MAX_HWORDLEN+1];
	
	strcpy(stemstr, stemstr2);
	
	if (strncmp(eomistr, "러", 2)==0 && strncmp(stemstr+stemlen-2, "르", 2)==0 ) {
		strcpy(eomistr_tmp, eomistr);
		strncpy(eomistr_tmp, "어", 2);

		eomi = HGram::search( eomistr_tmp );

		if (eomi) {
			
			searchAddingDa(stemstr, stemlen, Leo_conj);
			
		}
	}

	else if (strncmp(eomistr, "거라", 4)==0 && strncmp(stemstr+stemlen-2, "가", 2)==0 ) {
		
		searchAddingDa(stemstr, stemlen, Go_conj);
	}

	else if (strncmp(eomistr, "너라", 4)==0 && strncmp(stemstr+stemlen-2, "오", 2)==0 ) {
		
		searchAddingDa(stemstr, stemlen, Come_conj);
	}

	else {
		//SylInfo *fstSylEomi = HanChar::getSylInfo(eomistr);
		if (JH_MidSnd(fstSylEomi->jhcode)==MidSnd_YEO && JH_FstSnd(fstSylEomi->jhcode)==FstSnd_Nothing ) {
			if (strncmp(stemstr+stemlen-2, "하", 2)==0) {
				searchAddingDa(stemstr, stemlen, Yeo_conj);
			}
		}
	}
	return 0;
}


/**
어간, 어미 모두  바뀌는 불규칙 어절의 분석 알고리즘.

가지 :
으 탈락 :  써 = 쓰 + 어    모아 = 모으 + 아
푸 불규칙 : 퍼 = 푸 + 어
ㅂ 불규칙 : 구워 = 굽 + 어  고와 = 곱 + 아
르 불규칙 : 흘러 = 흐르 + 어

*/
int	HStem::IrregularStemAndEomi(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr)

{
	checkIrregular_Reu(fstSylEomi, stemstr2, stemlen, eomistr);
	return 0;
}

/**
orgWord 에서 복모음 부분을 분리/확장하여 
새로운 문자열값을  extended 에 저장한다.
extended 의 최대 길이는 maxlen 이다.
*/
int	HStem::extendDoubleVowel(char *extended, int maxlen)
{
	int	s; // 음절 위치
	int	i; 
	int	copied; 
	char	split[5]; // 축약됀 음절을 2음절로 분리 저장.
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
선어말 어미 처리.
선어말 어미 == prefinal-eomi

주로 'ㅆ' 받침.

ex) 었 았  겠   으 시 셨 

도우셨겠다 ??

stemstr -- 단어 앞부분 
stemlen -- 단어 앞부분 길이
eomistr
*/
//int	HStem::handleSuneomalEomi(SylInfo *fstSylEomi, char*stemstr2, int stemlen,  char *eomistr2)
int	HStem::handleSuneomalEomi(int currSylPos)
{
	int	currBytePos = currSylPos + currSylPos; 
	int	sylpos; // 음절 위치
	int	bytepos; 
	SylInfo *info;
	char *curr =0;

	char	split[5] = {0}; // 축약됀 음절을 2음절로 분리 저장.
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
		
		if (strncmp(curr, "시", 2)==0) {
			suneomalType |= SUNEOMAL_HONOR;
		}
		else if (strncmp(curr, "셔", 2)==0) {
			suneomalType |= SUNEOMAL_HONOR;
			strcpy(eomi_hidden, "어");
		}
		else if (strncmp(curr, "으", 2)==0) {
		}
		else  if ( JH_LastSnd(info ->jhcode) != LastSnd_SS) {
			break;
		}
		
		if (strncmp(curr, "겠", 2)==0) {
			suneomalType |= SUNEOMAL_FUTURE;
		}
		else if (strncmp(curr, "었", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "았", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "였", 2)==0) {
			suneomalType |= SUNEOMAL_PAST;
		}
		else if (strncmp(curr, "셨", 2)==0) {
			suneomalType |= (SUNEOMAL_HONOR | SUNEOMAL_PAST );
		}		
		else if ( HanChar::splitHanSyllable(split, info->jhcode) )  {
			suneomalType |= SUNEOMAL_PAST;
			break;
		}
	}

	/*
	 * 어절 길이가 3정도는 되어야 선어말 어미가 있는 것이다.
	 * 예외) 잘 가셔 !
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
		어간 , 어미 분리를 시도
		*/
		if (eomistr[0]==0) // 어미가 없는 경우. 선어말 어미도 없는 것이다.
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
		어간 , 어미 분리를 시도
		*/
		if (eomistr[0]==0) // 어미가 없는 경우. 
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
체언 결합 조건 검사. 사전 검색.
*/
HwordType *HStem::qualifyJosa(char *stemstr, int stemlen, hGramType *josa)
{
	HwordType *stem=0;

	if ((josa->property & GW_BUNGAN) && (josa->property & GW_CHOSUNG)) {
		SylInfo *lastsyl = HanChar::getSylInfo(stemstr + stemlen -2);
		if (lastsyl && Has_LastSnd(lastsyl->jhcode) ) {
			printf("%s + %s : 분간조사 연결 오류 \n", stemstr, josa->key);
			return 0;
		}
	}
	stem = HDict::search(stemstr);
	if (stem && IS_H_NOUN(stem->PS)) return stem;
	
	if (strncmp(stemstr+stemlen-2, "이", 2 )==0) {
		stemstr[stemlen-2] = 0;
		stem = HDict::search(stemstr);
		if (stem && IS_H_NOUN(stem->PS)) return stem;
	}
	return 0;
}

/**
 어절의 뒷부분에서부터 검사하면 앞으로 이동. 

 for loop:
   현재 음절이 조사의 첫음절에 해당되면
     조사 사전검색.
     
   현재 음절의 받침이 ㄴ, ㄹ 이면 
     조사 사전 검색.

   이렇게해서 조사후보를 발견하면
     어근을 단어 사전에서 검색
     
*/
int	HStem::stemJosa()
{
	int	s; // 음절 위치
	int	i; 
	SylInfo *info;
	byte	lastsnd;
	char	josa[MAX_HWORDLEN+1];
	char	stem[MAX_HWORDLEN+1];
	int	stemlen = 0;
	hGramType *josa_c; //조사 후보 
	int	sylchanged=0; // 받침 분리 등으로 음절값이 변형된 경우 true.
	int	cnt=0;
	
	for(s=sylLen-1, i=orgLen-2; s>=0; s--, i-=2 ) {
		info = wordInfo[s];
		lastsnd = JH_LastSnd(info->jhcode);
		if ( info->property & P_JOSA1) {
			josa_c = HGram::search( orgWord+ i );	
		}
		else if (lastsnd==LastSnd_N || lastsnd == LastSnd_L) {
		/* ㄴ, ㄹ 받침 : 조사 후보 */
			strncpy(josa, HanChar::HLastSnd[lastsnd].draw, 2);
			josa[2] = 0;
			strcpy(josa+2, orgWord+ i+2 );
			sylchanged = 1;
			josa_c = HGram::search( josa );	
		}
		else continue;

		/*
		이렇게해서 조사후보를 발견하면
     		어근을 단어 사전에서 검색
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
용언  결합 조건 검사. 사전 검색.
@param stemstr 어근 
@param eomi 어미 정보 (문법 사전에서 찾은 것)
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
			printf("%s + %s : 분간어미연결 오류 \n", stemstr, eomistr);
			return 0;
		}
	}
	
	stem = HDict::search(stemstr);
	// 어근을 단어 사전에서 찾은 경우.
	// @TODO 어미가 ㄴ, ㄹ, ㅁ, ㅂ 등의 자소 하나 일 때는 '~다' 가 아닌 것은 용언이 아닐 것이다 
	// 하얀 == 하야 + ㄴ (X)

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

		
	// 어근이 단어 사전에 없는 경우에는
	// 불규칙 검사 및 용언화 접미사를 검사 한다.

	// 불규칙 검사 
	IrregularStemOnly(lastSylOfStem, stemstr, stemlen, eomistr);

	// 용언화 접미사 검사
	stem = checkYongunSuffix(stemstr, stemlen, eomi);
	if (stem) {
		addStem(stem, DaMatch);
		return stem;
	}
	
	return 0;
}

/**
선어말 어미를 제거한 후에  어근 + 어미 분석.

info -- 어미의 첫 글자의 정보.
*/
int	HStem::splitStemEomi(SylInfo *info, char*stem_org, int stem_orglen,  char *eomi_org,  int jasoSplittable)
{
	byte	fstsnd, midsnd, lastsnd;
	char	eomistr[MAX_HWORDLEN+1];
	char	stemstr[MAX_HWORDLEN+1];
	char	split[5]; // 축약됀 음절을 2음절로 분리 저장.
	int	stemlen = 0;
	hGramType *eomistr_c; //어미 후보 

	fstsnd = JH_FstSnd(info->jhcode);		
	midsnd = JH_MidSnd(info->jhcode);
	lastsnd = JH_LastSnd(info->jhcode);

	/*
	어미의 모양만 바뀌는 불규칙 어절 처리 : 
	4 가지 : 러, 거라, 너라, 여 	
	*/
	IrregularEomiOnly(info, stem_org, stem_orglen, eomi_org );


	/*
	어간, 어미 모두  바뀌는 불규칙 어절의 분석 
	*/
	IrregularStemAndEomi(info, stem_org, stem_orglen, eomi_org );
		
	if ( info->property & P_EOMI1) {
		strcpy(eomistr, eomi_org );
		eomistr_c = HGram::search( eomistr);

		if (eomistr_c) {
			/*어간부 복원 */
			strncpy(stemstr, stem_org, stem_orglen);
			stemstr[stem_orglen] = 0;
			
			qualifyEomi(stemstr, stem_orglen, eomistr, eomistr_c);	

		}
		
	}

	/* ㄴ, ㄹ, ㅁ, ㅂ 받침 : 어미 후보 
	자모로 시작하는 어미 인지 검사. 
	*/
	//else
	if (jasoSplittable && lastsnd==LastSnd_N || lastsnd == LastSnd_L
			|| lastsnd==LastSnd_M || lastsnd == LastSnd_B) 
	{
	
		/* 어미부 복원 */
		strncpy(eomistr, HanChar::HLastSnd[lastsnd].draw, 2);
		strcpy(eomistr+2, eomi_org+2);

		eomistr_c = HGram::search( eomistr );		

		if (eomistr_c) {
			/*어간부 복원 */
			strncpy(stemstr, stem_org, stem_orglen);
			HanChar::replaceLastSnd(stemstr+stem_orglen, info->jhcode, LastSnd_Nothing);
			stemstr[stem_orglen+2] = 0;
			qualifyEomi(stemstr, stem_orglen + 2, eomistr, eomistr_c);	

		}
	}

	/* 아, 어 축약 검사 */
	// @TODO
	//else 
	else if ((midsnd==MidSnd_A || midsnd==MidSnd_EO) && lastsnd==LastSnd_Nothing) {
		/* 어미부 복원 */
		if (midsnd==MidSnd_A)
		strcpy(eomistr, "아");
		else
		strcpy(eomistr, "어");
		strcpy(eomistr+2, eomi_org+2);

		eomistr_c = HGram::search( eomistr );

		/*어간부 복원 */
		strncpy(stemstr, orgWord, stem_orglen);
		stemstr[stem_orglen] = 0;

		if (eomistr_c) {
		
			if (stem_orglen > 0 && qualifyEomi(stemstr, stem_orglen, eomistr, eomistr_c)) {
			}
				

		}
	}

	/* 모음이 축약된 이중 모음 음절인 경우.
	모음 축약 이라고 문법적으로 칭함.
	용언 어간부와 어미가 결합할 때 모음 축약 많이 발생하므로,
	축약을 복원하여 어미를 찾아야 한다.
	*/
	//else 
	
	if (lastsnd == LastSnd_SS &&
		HanChar::splitHanSyllable(split, info->jhcode))
	{
		/* 어미부 복원 */
		
		strncpy(eomistr, split+2, 2);				
		strcpy(eomistr+2, eomi_org+2 );
		eomistr_c = HGram::search( eomistr );
		
		/*어간부 복원 */
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
어절의 뒷부분에서부터 검사하면 앞으로 이동. 
*/
int	HStem::stemEomi()
{
	int	s; // 음절 위치
	int	i; 
	SylInfo *info;
	byte	fstsnd, midsnd, lastsnd;
	char	eomistr[MAX_HWORDLEN+1];
	char	stemstr[MAX_HWORDLEN+1];
	int	stemlen = 0;
	hGramType *eomistr_c; //어미 후보 
	int	sylchanged=0; // 받침 분리 등으로 음절값이 변형된 경우 true.
	int	cnt=0;

	/* 어절의 뒷부분에서부터 검사하면 앞으로 이동. 
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
		어간 , 어미 분리를 시도
		*/
		//splitStemEomi(info, orgWord, i, orgWord+ i, 1 );		
		
	}
	return cnt;
}


/**
org 문자열을 끝  문자를 받침없는 문자로 변환.
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
단어의 앞 2, 3 음절만으로 국어 사전 검색을 해본다. 
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
하나의 어절을 분석하여 실질 형태소 를 찾아 찾아 내고자 한다.

문장 내의 현 어절의 앞뒤 분석 결과를 같이 활용하는 것이 더 정확한 분석을 할 수 있다.
--> 문장 단위 분석 필요. 

0. 원형 그래도 사전 검사 
1. 조사, 어미 검사
2. ~다 검사
3. 불규칙 활용 검사
4. 선어말 어미, 어말 어미 검사

4. 복합어 검사
5. 접두어, 접미어 검사
6. 띄워 쓰기 오류 검사

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
	/* 한자 등의 한글 아닌 문자가 포함된 경우 이다. */
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

	// 원형 그대로 사전에 있는 경우
	dtword = HDict::search(hword);
	if (dtword) {
		addStem(dtword, OrgMatch);
	}

	printf("=== Josa ===\n");
	stemJosa();

	if (stemResCnt > 0 && strlen(stemRes[0].stem->key) >= 4)
	{
		// 2 자 이상의 단어를 찾았다면 더 이상 어미 조사는 필요없겠다.
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
