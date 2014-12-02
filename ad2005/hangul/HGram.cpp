#include "ace/Log_Msg.h" // NOT Real ACE. My lib

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

//#include "dbStore.h"
//#include "DbConn.h"
//#include "DbQuery.h"

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "Config.h"

#include "HGram.h"

simpleHash<hGramType>  HGram::hashTab;
simpleAlloc HGram::mymem(100);
int HGram::josaMaxNum = 0;
int HGram::josaNums = 0;
int HGram::eomiMaxNum = 0;
int HGram::eomiNums = 0;
hGramType **HGram::josaArray = 0;
hGramType **HGram::josaArray_invert = 0;
hGramType **HGram::eomiArray = 0; // ��� 
hGramType **HGram::eomiArray_invert = 0;

/**
*/
int HGram::alloc(int jsize, int esize)
{
	hashTab.init(esize+jsize);
	
	if (josaArray) return 0;
	josaArray = new hGramType * [jsize];
	josaArray_invert = new hGramType * [jsize];
	if (josaArray==0 || josaArray_invert==0) return -1;
	josaMaxNum = jsize;
	
	eomiArray = new hGramType * [esize];
	eomiArray_invert = new hGramType * [esize];
	if (eomiArray==0 || eomiArray_invert==0) return -1;
	eomiMaxNum = esize;
	
	memset(josaArray, 0, sizeof(hGramType*) * josaMaxNum);
	memset(eomiArray, 0, sizeof(hGramType*) * eomiMaxNum);
	memset(josaArray_invert, 0, sizeof(hGramType*) * josaMaxNum);
	memset(eomiArray_invert, 0, sizeof(hGramType*) * eomiMaxNum);

	ACE_DEBUG((LM_INFO, "HGram:: HashTable size=%d, Array size=%d\n",
		hashTab.size(), josaMaxNum));
	return 0;
	
}

/*=== word list :: operations on wordArray ===*/
/**
�� �ܾ��, �� �ܾ�� ���۵Ǵ� �������� sort�Ѵ�.
*/
int HGram::cmpgramList(const void *Aptr, const void *Bptr)
{
	return strcmp ( (*(hGramType **)Aptr)->key , (*(hGramType **)Bptr)->key);
}

int HGram::sortgramList()
{
	qsort (josaArray, josaNums, sizeof(hGramType *), cmpgramList);
	qsort (eomiArray, eomiNums, sizeof(hGramType *), cmpgramList);
	return 0;
}

int HGram::cmpgramList_invert(const void *Aptr, const void *Bptr)
{
	return strcmp ( (*(hGramType **)Aptr)->invert , (*(hGramType **)Bptr)->invert);
}

int HGram::sortgramList_invert()
{
	qsort (josaArray_invert, josaNums, sizeof(hGramType *), cmpgramList_invert);
	qsort (eomiArray_invert, eomiNums, sizeof(hGramType *), cmpgramList_invert);
	return 0;
}

#if 0
	wscode = STR_TO_UINT2(line.str(0));
	if (
		(! IS_WS_UMJUL(wscode) && ! IS_WS_JAMO(wscode) )
		|| ( line.len(0) & 0x01)==1
	) 
	{
		/* ��ȿ�� �ϼ��� �ѱ� �ڵ尡 �ƴ� ���. �ѱ� �ڵ� ���̰� ¦���� �ƴ� ���. */
		/* isspace() �Լ��� �ѱ� �ڵ忡 ���� �߸� �����Ͽ� �̷� ��찡 �߻���.
		*/
		ACE_DEBUG((LM_DEBUG, "Wrong Hangul: %s %s", line.str(0), buf));
		continue;
	}
#endif

void	HGram::insertNewword(hGramType	*newword, int PS)
{
	if (PS==GW_JOSA && josaNums < josaMaxNum) {
		// array �� insert
		josaArray[josaNums] = newword;
		josaArray_invert[josaNums] = newword;
		josaNums++;
	}

	if (PS==GW_EOMI && eomiNums < eomiMaxNum) {
		// array �� insert
		eomiArray[eomiNums] = newword;
		eomiArray_invert[eomiNums] = newword;
		eomiNums++;
	}
}
/**

*/
int HGram::loadGramDict(char *fname, int PS, int freqFlag)
{
	FILE *fp;
	char buf[256];
	int	cnt=0;
	int	dup_cnt=0;
	MemSplit	line(3, 23);

	hGramType w;
	hGramType	*newword=0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		ACE_DEBUG((LM_DEBUG,"Read Failed: %s\n", fname)); 
		return -1;
	}
	while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
		
		int	ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		StrUtil::trim_from(buf, '#'); // ������ �� �κп� �ִ� comment ����
		
		line.reuse();
		line.split(buf);
		if (line.size()==0) continue;
		
		memset(&w, 0, sizeof(hGramType));
		
		//w.id = josaNums+1;
		w.key = (char *)mymem.strAlloc(line.str(0),  line.len(0));			
		w.wlen = line.len(0);
		if (PS) w.PS = PS;
		
		if (freqFlag && line.num() >= 2) {
			if (PS==GW_JOSA)
				w.freqJosa = line.intval(1);
			else if (PS==GW_EOMI)
				w.freqEomi = line.intval(1);
		}

		if (freqFlag==0 && line.num() >= 2) {
			// @todo gramval �� prefix �� ���� �ٸ� ���/���� �� ���� ���� �ο� 
			w.gramVal = line.intval(1);
			if (line.num() >= 3 && line.str(2)[0]=='B') {
				w.property |=  GW_BUNGAN;
			}
		}


		
		
		newword =  hashTab.insert(&w);
		if (newword == 0) {			
			dup_cnt++;
			hGramType *old = hashTab.search(w.key);
			if (old) {
				if (w.property) old->property |= w.property;
				if (w.freqEomi) old->freqEomi = w.freqEomi;
				if (w.freqJosa) old->freqJosa = w.freqJosa;
				if (w.gramVal) old->gramVal = w.gramVal;
				
				if (old->PS != PS) {
					ACE_DEBUG((LM_DEBUG, "Josa & Eomi: %s\n", w.key));
					old->PS |= PS;
					insertNewword(old, PS);
				}
				
			}
		}
		else {
			// @todo default setting
			// newword->freq = 30000;
			
			newword->invert = (char *)mymem.strAlloc(w.key,  w.wlen);
			StrUtil::invertHangul(newword->invert, w.key,  w.wlen);
		
			insertNewword(newword, PS);
		}
		
		cnt++;
		//if (cnt>100) break;

	}

	ACE_DEBUG((LM_DEBUG, "FILE %s :  count=%d, dup=%d\n", fname, cnt, dup_cnt));
	
	fclose(fp);
	return 0;
}

void	HGram::markGramDict(hGramType **gdictarray, int size, uint2 PS)
{
	int	i;
	hGramType *ptr=0;
	uint2	wscode=0;
	uint4	syll_prop = (PS == GW_JOSA)?P_JOSA1: P_EOMI1;
	char	prevword[MAXLEN_HWORD+1]={0};
	int	prevwlen = 0;
	uint2	prevgramVal = 0;
	char	*first_syl = 0; // ǰ�纰 ���� Ư�� ������ �����ϱ� ����. '��' ���� �ڸ�� �����ϴ� ��� �ڸ� ���ĸ� ������ ����.
	int	syl_len; // ���� ����
	for(i=0; i<size; i++) {
		ptr = gdictarray[i];

		if (ptr->gramVal==0) {
			if (strncmp(ptr->key, prevword, prevwlen)==0)
				ptr->gramVal = prevgramVal;
		}
		else {
			prevgramVal = ptr->gramVal;
			strncpy(prevword, ptr->key, MAXLEN_HWORD);
			prevword[MAXLEN_HWORD] = 0;
			prevwlen = strlen(prevword);
		}
		
		
		wscode = STR_TO_UINT2(ptr->key);
		
		if (IS_WS_JAMO(wscode)) {
			first_syl = ptr->key + 2;
			syl_len = (ptr->wlen >> 1) -1;
			ptr->property |= GW_JAMOHEAD;			
		}
		else {
			first_syl = ptr->key;
			syl_len = (ptr->wlen >> 1);
			
			SylInfo *info = HanChar::getSylInfo(ptr->key);
			if (info && ! Has_NO_FstSnd(info->jhcode)) {
			/* �ʼ��� '��' �ƴ� ��. �� �ʼ����� �����ϴ� ��. */
				ptr->property |= GW_CHOSUNG;
			}
		}

		/* ���� ���� ���̺� ��� / ���� ǥ�� */
		/* 1 ���� ��ġ ������ ǥ�� */
		
		
		if (syl_len > 0) HanChar::markSyllableProperty(first_syl, syll_prop);

		/* 2 ���� ��ġ ������ ǥ�� */
		if (syl_len > 1) HanChar::markSyllableProperty(first_syl+2, syll_prop << 1);
		
		
		ptr->infos = (SylInfo**) mymem.alloc( sizeof(SylInfo*) * (ptr->wlen>>1) );
		HanChar::getSylInfo(ptr->infos, (ptr->wlen>>1), ptr->key);
	}
}

void	HGram::printGword(hGramType *w)
{
	//ACE_DEBUG((LM_DEBUG, "%4d %02X %s %s\n",w->id, w->PS ,  w->key, w->invert));
	//printf( "%4d %02X %s %s\n",w->id, w->PS ,  w->key, w->invert);
	printf( "%s %04X %04X %4d %d %d\n",w->key, w->PS, w->property, 
		w->gramVal,	
		w->freqJosa, w->freqEomi);
}
void	HGram::printList()
{
	int	i;
	printf("#========= JOSA : %d\n", josaNums);
	for(i=0; i<josaNums; i++)
		printGword( josaArray[i] );
		
	printf("#========= EOMI : %d\n", eomiNums);
	for(i=0; i<eomiNums; i++)
		printGword( eomiArray[i] );
}

void	HGram::printList_invert()
{
	for(int	i=0; i<josaNums; i++)
		printGword( josaArray_invert[i] );
}

int	HGram::load(char *home)
{
	char fname[256];

	alloc(MAX_JOSANUM, MAX_EOMINUM  );

	//testHash();
	//return 0;
	loadGramDict ( StrUtil::path_merge(fname, home, "dataH/JosaInfo.dic"),  GW_JOSA, 0);
	//loadGramDict ( StrUtil::path_merge(fname, home, "dataH/EomiInfo.dic"), GW_EOMI, 0 );
	
	loadGramDict ( StrUtil::path_merge(fname, home, "dataH/FreqJosa.dic"), GW_JOSA, 1 );
	loadGramDict ( StrUtil::path_merge(fname, home, "dataH/FreqEomi.dic"), GW_EOMI, 1);

	loadGramDict ( StrUtil::path_merge(fname, home, "dataH/Josa_all.txt"),  GW_JOSA, 0);
	loadGramDict ( StrUtil::path_merge(fname, home, "dataH/Eomi_all.txt"), GW_EOMI, 0 );
	


	sortgramList();
	sortgramList_invert();

	markGramDict(josaArray, josaNums, GW_JOSA);
	markGramDict(eomiArray, eomiNums, GW_EOMI);
	
	return 0;
}

void	HGram::print()
{
	ACE_DEBUG((LM_DEBUG, "Word Count = %d\n",  josaNums));
	hashTab.print();

	/*
	printf("============\n");
	printList();
	printf("============\n");
	
	printList_invert();
	printf("============\n");
	*/
}
void	HGram::testHash()
{
	int	i,j,k;
	hGramType	*newword=0;
	hGramType w={0};
	MemSplit	line(3, 23);
	char	buf[100];
	strcpy(buf, "�Ѵ��� aaa ���ϴ� ����������\n");
	
	/*
	for(i='a'; i<='z'; i++)
		for(j='a'; j<='z'; j++)
			for(k='a'; k<='z'; k++) {
				 w.key[0] = i;
				 w.key[1] = j;
				 w.key[2] = k;
				 w.key[3] = 0;
				newword =  hashTab.insert(&w);
				if (newword==0) {
					printf("hash insert() fail: %s\n", w.key);
				}
			}
	*/		
	
	for(i=0xB0; i<=0xC8; i++)
		for(j=0xA1; j<=0xFE; j++)
			for(k='a'; k<='a'; k++) {
				line.reuse();
				line.split(buf);
				
				 w.key[0] = i;
				 w.key[1] = j;
				 w.key[2] = k;
				 w.key[3] = 0;
				newword =  hashTab.insert(&w);
				if (newword==0) {
					printf("hash insert() fail: %s\n", w.key);
				}
			}
}


/**
*/
int	HGram::matchJosa(hGramType **matchGW, int maxMatch, char *hword, int hwlen)
{
	if (hwlen == 0) strlen(hword);
	char	invert[MAXLEN_HWORD+1];
	
	
	StrUtil::invertHangul(invert, hword, hwlen);
	//printf("org inverted: %s\n", invert);

	int	i;
	int	matchCnt=0; // ��ġ�� ���� �� �߻� �� �� �ִ�.
	for(i=0; i<josaNums; i++) {
		int cmp = HanChar::strncmpHan(invert, josaArray_invert[i]->invert, josaArray_invert[i]->wlen);
		if (cmp < 0) break;
		if (cmp == 0 && matchCnt < maxMatch) {
			matchGW[matchCnt++] = josaArray_invert[i];
		}
	}

	if (matchCnt == 0) {
		printf("%s (X)\t", hword);
	}
	else {
		printf("%s (%d)\t", hword, matchCnt);
	}
	
	for(i=0; i<matchCnt; i++) {
		//printGword(matchGW[i]);
		printf("%s ", matchGW[i]->key);
		if ( (matchGW[i]->PS & GW_EOMI) ) printf("E");
		if ( (matchGW[i]->PS & GW_JOSA) ) printf("J");
		printf("  ");
	}
	printf("\n");
	
	if (matchCnt==0) return 0;
	return matchCnt;
}


/**
str1 : ���¼� �м��� ����
str2 : ��� /���� ���� ���� ���¼� 


�ѱ����� string ���� ��.

���� ���ε�   -- ����
'�ε�' �� '����' �� ��.
str2 ���� '��', '��' ���� �ڸ� ������ ���� �� �ִ�.

st1 �� ���� ����.
str2 �� ���� ���¼�
*/
int	HGram::cmpEomi(char *str1, char *str2, int len)
{
	int	n;
	for(n=0; str1[0] && str1[0] && str1[1] &&  n < len; n += 2, str1 += 2, str2 += 2) 
	{
		if (str1[0] != str2[0] || str1[1] != str2[1]) {			
			
			uint2	ws1 = STR_TO_UINT2(str1);
			uint2	ws2 = STR_TO_UINT2(str2);
			SylInfo *info1 = HanChar::getSylInfo(str1);
			int diff = (int) (ws1 - ws2);
			
			HanChar::printWS(info1);
			printf("%s %s %X %X (%d)\n", str1, str2, ws1, ws2, diff);
			
			
			if (info1==0) return diff;
			if (JH_LastSnd(info1->jhcode) == ws2 )
			{
				continue;
			}
			
			byte mid = JH_MidSnd(info1->jhcode);
			printf("midsung=%d\n", mid);
			if (strncmp(str2, "��", 2)==0) {
				if (mid==7 || mid==11  // �� Ż��
				|| mid==4 || mid==10   // �� ��� 
				|| mid==21 // �� ���
				|| mid==11 // �� ��� 
				|| mid ==15 // �� ��� 
				)
					continue;
			}

			if (strncmp(str2, "��", 2)==0) {
				if (mid==3   // �� Ż��
					|| mid==14  // �� ��� 
					|| mid==11 // �� ��� 
				)
					continue;
			}
			
			return diff;
			
		}
		
	}

	if (str1[0] != str2[0]) return str1[0] - str2[0];

	return 0;
}


/**
*/
int	HGram::matchEomi(hGramType **matchGW, int maxMatch, char *hword, int hwlen)
{
	if (hwlen == 0) strlen(hword);
	char	invert[MAXLEN_HWORD+1];
	
	
	StrUtil::invertHangul(invert, hword, hwlen);
	//printf("org inverted: %s\n", invert);

	int	i;
	int	matchCnt=0; // ��ġ�� ���� �� �߻� �� �� �ִ�.
	for(i=0; i<eomiNums; i++) {
		//printf("%s  %s\n", invert, eomiArray_invert[i]->invert);
		int cmp = HGram::cmpEomi(invert, eomiArray_invert[i]->invert, eomiArray_invert[i]->wlen);
		if (cmp < 0) break;
		if (cmp == 0 && matchCnt < maxMatch) {
			matchGW[matchCnt++] = eomiArray_invert[i];
		}
	}

	if (matchCnt == 0) {
		printf("%s (X)\t", hword);
	}
	else {
		printf("%s (%d)\t", hword, matchCnt);
	}
	
	for(i=0; i<matchCnt; i++) {
		//printGword(matchGW[i]);
		printf("%s ", matchGW[i]->key);
		if ( (matchGW[i]->PS & GW_EOMI) ) printf("E");
		if ( (matchGW[i]->PS & GW_JOSA) ) printf("J");
		printf("  ");
	}
	printf("\n");
	
	if (matchCnt==0) return 0;
	return matchCnt;
}



/**
http://nlp.kookmin.ac.kr/data/han-dic.html ���� ���� �ڷ� �� freq.txt ���� ����, ��� �� �� ������ ���ϱ� ���� ��.

�� �Լ��� ����Ͽ� FreqEomi.dic, FreqJosa.dic ��  ���Ͽ���.
*/
int HGram::load_grammar_freq(char *fname, int PS)
{

	FILE *fp;
	char buf[256];
	
	int	cnt=0;
	int	dup_cnt=0;
	MemSplit	line(3, 23);

	hGramType w;
	hGramType	*newword=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		ACE_DEBUG((LM_DEBUG,"Read Failed: %s\n", fname)); 
		return -1;
	}
	while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
		int	wscode=0;
		int	ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;

		line.reuse();
		line.split(buf);
		if (line.size()==0) continue;
		
		memset(&w, 0, sizeof(hGramType));
		
		//w.id = josaNums+1;
		w.key = (char *)mymem.strAlloc(line.str(0),  line.len(0));	
		
		w.wlen = line.len(0);
		if (PS) w.PS = PS;
		
		w.freqJosa = (uint2) ((float) strtod(line.str(2), NULL) * 10000 );
		
		
		
		newword =  hashTab.insert(&w);
		if (newword==0) {
			ACE_DEBUG((LM_DEBUG, "DUP: %s\n", w.key));
			dup_cnt++;
			hGramType *old = hashTab.search(w.key);
			if (old && PS) {
				old->PS |= PS;
				old->freqJosa = (old->freqJosa + w.freqJosa) /2;
			}
		}
		else {
			newword->invert = (char *)mymem.strAlloc(w.key,  w.wlen);
			StrUtil::invertHangul(newword->invert, w.key,  w.wlen);
		
			if (josaNums < josaMaxNum) {
			// array �� insert
			josaArray[josaNums] = newword;
			josaArray_invert[josaNums] = newword;
			josaNums++;
			}
		}
		
		cnt++;
		//if (cnt>100) break;

	}

	ACE_DEBUG((LM_DEBUG, "FILE %s :  count=%d, dup=%d\n", fname, cnt, dup_cnt));
	
	fclose(fp);
	return 0;
}

