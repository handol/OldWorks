#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#include "DbConn.h"
//#include "DbQuery.h"

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "Config.h"

#include "HDict.h"
#include "HGram.h"
#include "dbStore.h"

#define	PRN	printf

struct _nameValPair HDict::conjMapTable[] = {

 { "��", D_conj },
 { "��", L_conj },
 { "��", B_conj },
 { "��", S_conj },
 { "��", H_conj },
 { "�Ŷ�", Go_conj },
 { "�ʶ�", Come_conj },
 { "����", Leora_conj },
 { "��", Leo_conj },
 { "��", Reu_conj },
 { "��", Yeo_conj },
 { "��", U_conj },
 { "��", Eu_conj }
};
 

struct _nameValPair HDict::psMapTable[] = {

 { "��ź��", HAN_EXCL },
 { "������", HAN_ADJ_KOR },
 { "��", HAN_PHRASE },
 { "����", HAN_NOUN | HAN_PRONOUN },
 { "����", HAN_VERB },
 { "���", HAN_NOUN },
 { "����", HAN_NOUN }, //  ��� �� ��Ÿ�� ����.
 { "��������", HAN_VERB |  HAN_AUX},
 { "���������", HAN_VERB | HAN_AUX },
 { "�λ�", HAN_ADVB },
 { "�ҿ����ڵ���", HAN_VERB | HAN_INCOMPLETE },
 { "�ҿ���Ÿ����", HAN_VERB | HAN_INCOMPLETE | HAN_VT },
 { "����", HAN_NOUN | HAN_NUM  },
 { "���", HAN_EOMI },
 { "�������", HAN_NOUN |HAN_DEP  },
 { "�ڵ���", HAN_VERB },
 { "���λ�", HAN_PFIX },
 { "���̻�", HAN_SFIX },
 { "����", HAN_JOSA },
 { "�ظ�", 000 },
 { "Ÿ����", HAN_VERB | HAN_VT },
 { "������", HAN_NOUN },
 { "����", HAN_ADJ },  //  ����� �� ��Ÿ�� ����.
 { "�����", HAN_ADJ }

};

simpleHash<HwordType>  HDict::hashTab;
simpleAlloc HDict::mymem(100);

HwordType **HDict::WordArray=0;
int HDict::wordNums = 0;
int HDict::maxWordId = 0;
int	HDict::maxSize = 0;

/**
���� ������ ǥ�õ� ǰ�� �̸��� int ������ ��ȯ.
*/
int	HDict::CONJname_to_CONJval(char *CONJname)
{
	int	cmp;
	for(int i=0; i< sizeof(conjMapTable)/sizeof (struct _nameValPair); i++) {
		cmp = strncmp(CONJname, conjMapTable[i].name, 2);
		if (cmp==0) return conjMapTable[i].value;
		if (cmp < 0) break;
	}

	return 0;
}

/**
���� ������ ǥ�õ� ǰ�� �̸��� int ������ ��ȯ.
*/
int	HDict::PSname_to_PSval(char *PSname)
{
	int	cmp;
	for(int i=0; i< sizeof(psMapTable)/sizeof (struct _nameValPair); i++) {
		cmp = strcmp(PSname, psMapTable[i].name);
		if (cmp==0) return psMapTable[i].value;
		if (cmp < 0) break;
	}

	return 0;
}

void HDict::test_nameValPairTable()
{
	int	res;
	for(int i=0; i< sizeof(psMapTable)/sizeof (struct _nameValPair); i++) {
		res = PSname_to_PSval ( psMapTable[i].name );
		printf("%s %X %X\n", psMapTable[i].name, res, psMapTable[i].value);
	}
}


/**
������ �������� �ܾ��� ǰ��, �ұ�Ģ Ȱ�� ���� ���� ���Ѵ�.
*/
int	HDict::proc_YBMKorDict_grammar(HwordType &hword, char *dict_body, int body_len)
{
	char *PSptr=0;
	char *CONJptr=0;
	char	PSname[24];
	char	CONJname[24];
	int	len1=0, len2=0;
	char *body_ptr = dict_body;
	
	

	while ( (PSptr = strstr(body_ptr, "\\f3<")) && len1 < body_len) {
		int	copyed = StrUtil::copy_to_char(PSname, PSptr+4, sizeof(PSname)-1, '>');
		//printf("%s ", PSname);
		
		hword.PS |= PSname_to_PSval(PSname);
		body_ptr += (4+copyed+1);
		len1 += (4+copyed+1);
	}

	CONJptr = strstr(dict_body, "\\f4<");
	if (CONJptr) {
		StrUtil::copy_to_char(CONJname, CONJptr+4, sizeof(CONJname)-1, '>');
		char *ptr = strstr(CONJname, "�ұ�Ģ");
		if (ptr) *ptr = 0;
		//printf("%s " , CONJname);
		hword.Conj = CONJname_to_CONJval(CONJname);
	}
	//printf("\n");
	return 0;
}

/**
'-' ��ũ�� ����, ���� �и��� ������ ����ϱ� ���� ���̴�.

69||���赵||����-��||
72||����ҵ�||����-�ҵ�||
*/
void	HDict::proc_YBMKorDict_split(HwordType &hword, char *split_word, int split_word_len)
{
	int	len = 0;
	int	cnt = 0;
	char *ptr;
	while(len < split_word_len) {
		int	step=0;
		ptr = strchr(split_word, '-');
		if (ptr) {
			step = ptr - split_word; // �и��� �ܾ��� ����.
			hword.split[cnt] = (step >> 1);
			split_word += (step+1);
			len += (step+1);
		}
		else
			break;
			
		cnt++;
	}
	
	for (int i=cnt; i<4; i++)
		hword.split[i] = 0;
	if (cnt==0) {
		hword.split[0] = (hword.wlen >> 1);
	}
}

void	HDict::proc_YBMKorDict_type(HwordType &hword)
{
	if (StrUtil::str_ends_with(hword.key, "������", hword.wlen,  6) ) {
		hword.property = HType_Like;
	}
	else if (StrUtil::str_ends_with(hword.key, "�ϴ�", hword.wlen,  4) ) {
		hword.property = HType_Hada;
	}
	else if (StrUtil::str_ends_with(hword.key, "��", hword.wlen, 2) ) {
		hword.property = HType_Da;
	}
}

void	HDict::printWord_stream(StrStream &stream, HwordType *h)
{
	//printf("%s %08X %d %d %5d | ", h->key, h->PS, h->Conj, h->property, h->id);
	stream.add("%s %08X %d %d", h->key, h->PS, h->Conj, h->property);
	stream.add(" %d", h->split[0]);
	for (int i=1; ; i++) {
		if (h->split[i]==0) break;
		stream.add(" %d", h->split[i]);
	}
	stream.add("\n");
}

void	HDict::printWord(HwordType *h)
{
	//printf("%s %08X %d %d %5d | ", h->key, h->PS, h->Conj, h->property, h->id);
	printf("%s %08X %d %d", h->key, h->PS, h->Conj, h->property);
	printf(" %d", h->split[0]);
	for (int i=1; ; i++) {
		if (h->split[i]==0) break;
		printf(" %d", h->split[i]);
	}
	printf("\n");
}

void	HDict::printList()
{
	printf("=== list ===\n");
	for(int	i=0; i<wordNums; i++) {
		printf("%-3d ", i+1);
		printWord( WordArray[i] );
	}
}

void	HDict::printListAuxOnly()
{
	int	cnt=0;
	printf("#=== ���� ��� \n");
	for(int	i=0; i<wordNums; i++)
		if ( (WordArray[i]->PS & HAN_AUX) ) {
			cnt++;
			printWord( WordArray[i] );
			
		}
	printf("# Count = %d\n", cnt);
}

void	HDict::printListConjOnly()
{
	int	sum=0;
	for(int cj=D_conj; cj<=Eu_conj; cj++) {
		int	cnt=0;
		printf("#=== �ұ�Ģ Ȱ�� :  %d\n", cj);
		for(int	i=0; i<wordNums; i++) {
			if (WordArray[i]->Conj==cj) {
				cnt++;
				sum++;
				printWord( WordArray[i] );
			}
		}
		printf("# Count = %d\n", cnt);
	}
	printf("## Sum = %d\n", sum);
}


void	HDict::printListEomiOnly()
{
	int	cnt=0;
	printf("## ���\n");
	for(int	i=0; i<wordNums; i++)
		if (WordArray[i]->PS & HAN_EOMI) {
			cnt++;
			printWord( WordArray[i] );
		}
	printf("# Count = %d\n", cnt);

}

void	HDict::printListJosaOnly()
{
	int	cnt=0;
	printf("## ����\n");
	for(int	i=0; i<wordNums; i++)
		if (WordArray[i]->PS & HAN_JOSA) {
			cnt++;
			printWord( WordArray[i] );
		}
	printf("# Count = %d\n", cnt);
}



/**

�ѱ� �ܾ �̴� ���� ����.

147251||����||��-��||\f3<�λ�>\ff �մ޾� �ڸ� ���� Ǫ�� �Ҹ�. <br>���� \f3<��ź��>\ff �մ޾� �ڿ����� ġ�� �Ҹ�.||||||
147252||\ig00��||?��||\f3<����>\ff \f0<����>\ff �ϳ�.||old||||��

147345||���콺����||����-������|
147371||�����ϴ�||����-�ϴ�|

6068||�Ś����/��\ig0\ig0��||�Ś����/��??��|
*/
int HDict::load_YBMKorDict(char *fname, int debug)
{

	FILE *fp;
	char buf[1024*2];
	
	int	cnt=0;
	int	dup_cnt=0;
	char	prevword[64] = {0};
	SylInfo	*wsInfoArry[20];
	int	wsLen=0;
	MemSplit	line(10, 256);

	HwordType	hword;
	if ( (fp=fopen(fname, "r")) == NULL) {
		ACE_DEBUG((LM_INFO, "Read Failed (Korean Dictionary) -- %s\n", fname));
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}

	PRINTF(("loading Hangul Dict: %s\n", fname));
	ACE_DEBUG((LM_INFO, "loading Hangul Dict: %s\n", fname));
	
	while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
		int wordlen=0;	
		char *presword=0; // ǥ���� 
		uint2 wscode=0;
		char *ptr = 0;
		
		int	ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		if (! isdigit(ch) ) continue;

		line.split(buf, '|');
		
		//if (cnt%20==0) line.print();
		if (line.size() < 7) continue;

		presword = line.str(2);
		/*
		��������� �ϼ��� �ڵ�� ǥ���� �� �Ǵ� <����>�̳� (����) ����� ���ԵǾ� �־� ,
		�����ϰ� ó���Ѵ�.
		
		1216||��\ig0��||��?��||\f3<�ڵ���>\ff \f0<����>\ff
		334||�����\ig00��||�����-?��||\f3<�����>\ff\f4<���ұ�ĢȰ��>\ff \f9(����)

		120192||����||����||\f3<Ÿ����>\ff \f0<����>\ff
		*/
		
		/*
		wscode = STR_TO_UINT2(presword);
		if (! IS_WS_UMJUL(wscode) && ! IS_WS_JAMO(wscode) ) continue;
		*/
		
		if ( strstr(  presword, "ig") ) continue;
	
		ptr = strchr(  presword, '/');
		if (ptr) *ptr = 0;

		wordlen  = strlen( presword );

		/* <����>, (����) ���� �̻��� �ڵ尪�� �����ϱ� ���� */
		wsLen = HanChar::getSylInfo(wsInfoArry, 20, presword);
		if ( (wsLen << 1) < wordlen) continue;

		memset(&hword, 0, sizeof(hword));
		hword.PS = 0;
		hword.Conj = 0;
		hword.property = 0;
		hword.id = line.intval(0);
		hword.wlen = wordlen;
		hword.key = (char *)mymem.strAlloc(presword, hword.wlen);	
		
		proc_YBMKorDict_grammar(hword, line.str(6), line.len(6) );

		proc_YBMKorDict_type(hword);

		proc_YBMKorDict_split(hword, line.str(4), line.len(4) );
		if (debug) printWord(&hword);

		if (hword.Conj) {
			if (hword.Conj == Reu_conj && hword.wlen >= 6) {
			/* �� �ұ�Ģ : ��� ������ ���� �պκ��� ��ȭ�Ѵ�.  ~~����. 
			������   -- ���� 			
			*/
				HanChar::markSyl_Irregular(hword.Conj, hword.key+hword.wlen-6);
			}
			else {
			/* �׿� �ұ�Ģ : ��� ������ ������ ��ȭ �Ѵ�.  ~~��. */ 
			/*
				if (hword.Conj==H_conj)
					printf("H = %s\n", hword.key);
			*/
				HanChar::markSyl_Irregular(hword.Conj, hword.key+hword.wlen-4);
			}
		}

		HwordType * resAdd = add(hword);
		
		if (resAdd==NULL) {
			if (strcmp(prevword, hword.key) != 0) {
				HwordType *old = hashTab.search(hword.key);
				if (old) {
					old->PS |= hword.PS;
				}
				/* ���� ��� Odd hash Dup  �� ����.
					���� : YBM ���� ������ �ܾ��� ������ sorting �� �ȵ� �͵��� ����.
					���  ) ������ ����
				*/
				//ACE_DEBUG((LM_DEBUG, "Odd hash Dup: %s %s\n", prevword, hword.key));
			}
			dup_cnt++;
		}
		
		strcpy( prevword, hword.key );
		cnt++;
		//if (cnt % 1000 == 0) PRN("%d\n", cnt);
	}

	ACE_DEBUG((LM_INFO, "Korean Words %d/%d (%d dup) \n", wordNums, cnt, dup_cnt));
	ACE_DEBUG((LM_INFO, "Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));

	PRINTF(("Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));
	
	fclose(fp);
	return 0;
}



/**
�ű� �ܾ� ���̺� �ε�
*/
int HDict::load_newhanword()
{
	DbQuery	query;
	int	loadDbCnt = 0;	

	PRINTF(("load TABLE: newhanword\n"));
	ACE_DEBUG((LM_INFO, "load TABLE: newhanword\n"));
	
	query.exec(dbStore::dbConn, "select word, word_id from newhanword order by word_id");

	while (query.fetch()) {
		loadDbCnt++;

		StrUtil::trim_right(query.colStrVal(0) );
		
		HwordType hword;
		memset(&hword, 0, sizeof(hword));
		hword.PS = 0;
		hword.Conj = 0;
		hword.property = 0;
		hword.key = (char *)mymem.strAlloc(query.colStrVal(0));
		hword.id = query.colIntVal(1);
		hword.wlen = strlen(hword.key);
		
		add(hword);
	}

	ACE_DEBUG((LM_INFO,"# of words loaded from DB: %d\n", loadDbCnt));
	ACE_DEBUG((LM_INFO, "Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));

	PRINTF(("# of words loaded from DB: %d\n", loadDbCnt));
	PRINTF(("Max=%d, Words=%d, Id=%d\n",  maxSize, wordNums, maxWordId));
	return wordNums;
}

/*=== word list :: operations on wordArray ===*/
/**
�� �ܾ��, �� �ܾ�� ���۵Ǵ� �������� sort�Ѵ�.
*/
int HDict::cmpWordList(const void *Aptr, const void *Bptr)
{
	return strcmp ( (*(HwordType **)Aptr)->key , (*(HwordType **)Bptr)->key);
}

int HDict::sortWordList()
{
	qsort (WordArray, wordNums, sizeof(HwordType *), cmpWordList);
	return 0;
}

int HDict::alloc(int hashsize)
{
	hashsize = hashTab.init(hashsize) ;
	ACE_DEBUG((LM_INFO, "EDict: hash size=%d\n", hashsize ));

	if (WordArray==0) {
		maxSize = hashsize;
		WordArray = new HwordType * [maxSize];

		if (WordArray==0) return -1;
	}
	
	memset(WordArray, 0, sizeof(HwordType*) * maxSize);
	return hashsize;
}

int HDict::checkWordArray()
{
	/*
	�ѱ� ������ �ߺ��Ǵ� ǥ����  �ܾ ����. �׷��� NULL  �� ���� �����Ѵ�.
	*/
	int foundNull = 0;
	PRINTF(("maxSize=%d  maxWordId=%d\n", maxSize, maxWordId));
	for(int i=0; i<maxWordId; i++)
	{
		if (WordArray[i] == NULL )
		{
			//if (! foundNull)
			//	PRINTF(("Hangul WordArray[%d] = NULL\n", i));
			//foundNull = 1;
		}
		else	if ( ((unsigned int)WordArray[i]  & 0xCC000000) == 0xCC000000 )
		{
			if (! foundNull)
				PRINTF(("Hangul WordArray[%d] = %08X\n", i, WordArray[i]));
			foundNull = 1;
		}
		else if (foundNull)
		{
			PRINTF(("Hangul WordArray[%d] = OK\n", i));
			foundNull = 0;
		}
	}
	return 0;
}

/**
hash table �� �ܾ insert �Ѵ�.
�ű� �ܾ �ƴϸ� hash table�� insert fail �� �߻��Ѵ�.

return 0 if NOT added
return 1 if added
*/
HwordType * HDict::add(HwordType &hword)
{
	HwordType *inserted = 0;
		
	inserted = hashTab.insert(&hword);
	if (inserted == NULL)
		return NULL;

	if (hword.id == 0)
	{
		hword.id = maxWordId + 1;
		inserted->id = maxWordId + 1;
	}
	
	if (hword.id < maxSize) {
		// array �� insert
		
		WordArray[hword.id] = inserted;
		
		wordNums++;
		if (hword.id > maxWordId)
			maxWordId = hword.id;
	}
	return inserted;

#if 0		
		/*
		��ٿ� �ش��ϴ� �κе� ���� hash �� �߰�.

		777||��������ϴ�||�������-�ϴ�||
		'�������' �� �߰�.
		*/
		HwordType rt;
		char	rootword[64] = {0};
		
		memcpy(&rt, &hword, sizeof(HwordType));
		rt.wlen = hword.split[0] << 1;
		strncpy(rootword, hword.key, rt.wlen);
		rootword[rt.wlen] = 0;
		rt.key = rootword;
		
		inserted = hashTab.insert(&rt);
		if (inserted) inserted->key = (char *)mymem.strAlloc(rt.key, rt.wlen);	
#endif		

}

/**
�ű� �ѱ� �ܾ ���̺� �߰�. �޸𸮿��� ���ÿ� �߰�.
*/
HwordType * HDict::addNewHangulWord(char *newhanword)
{
	if (search(newhanword) != NULL) // �̹� �ִ� �ܾ�� ���� �߰����� �ʴ´�.
		return NULL;

	// DB �� �߰�
	int insert_id = dbStore::storeDB_newhanword(newhanword, maxWordId);

	if (insert_id <= 0)
	{
		ACE_DEBUG((LM_INFO,"new Hangul word DB insert fail: %s res=%d  maxWordId=%d\n", 
			newhanword, insert_id, maxWordId));
		return NULL;
	}
		
	HwordType hword;
	hword.PS = 0;
	hword.Conj = 0;
	hword.property = 0;
	hword.key = (char *)mymem.strAlloc(newhanword);
	hword.id = 0;
	hword.wlen = strlen(hword.key);

	// �޸𸮿� �߰�
	HwordType * inserted = add(hword);
	return inserted;
}

int HDict::load(char *home, int debug)
{
	char fname[256];

	int	maxid = dbStore::getMaxValue("newhanword", "word_id");
	int	allocSize = 150000;
	if (maxid > 0)
	{
		maxid += 1000;
		allocSize = MAX(maxid, allocSize);
	}
	alloc(allocSize);
	load_YBMKorDict( StrUtil::path_merge(fname, home, "dataH/sisa_kor.dict"),  debug );
	load_newhanword();

	/* wordList  �� sort �� �ʿ䰡 ����.  �ܾ� id �� �� array index */
	
	if (debug) hashTab.print();
	if (debug) mymem.print();
	
	return 0;
}

HwordType * HDict::search(char *word)
{
	
	return (HwordType *) HDict::hashTab.search(word);
}

HwordType * HDict::searchByID(int word_id)
{
	if (WordArray==0)
		return NULL;

	if (word_id >= maxSize || word_id < 0)
		return NULL;
	
	return WordArray[word_id];
}

int	HDict::prepare(char *home)
{
	HanChar::prepare();
	HDict::load(home);
	HDict::checkWordArray();
	HGram::load(home);
	return 0;
}