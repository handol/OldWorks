#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include <stdio.h>
#include <ctype.h>
#include "dbStore.h"
#include "DbConn.h"
#include "DbQuery.h"

#include "MemSplit.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "Config.h"

#define	PRN	printf

typedef	unsigned char byte;

char	*adhome = NULL;
Config	config(50);

void	init_env()
{
	char	logfile[256];
	
	adhome = getenv("ADHOME");
	if (adhome) printf("ADHOME = %s\n", adhome);
	else	printf("ADHOME is NOT set !\n");

	StrUtil::path_merge(logfile, adhome, "log/dict.txt");

	
	ACE_OSTREAM_TYPE *output = new ofstream(logfile);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	config.open_home(adhome, "anydict.cfg");
}




/**
WordNet ����� ���� ����Ÿ�� �ܾ DB�� �߰�.
*/
int loadWords_wordnet(char *fname)
{

	FILE *fp;
	char buf[512];
	int	ch;
	int	cnt=0;
	wordType	w;
	MemSplit	line(12, 48);

	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL && cnt < 1000000) {
		int len=0;
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		
		line.split(buf);
		memset(&w, 0, sizeof(w));

		w.w_type = (byte)strtol( line.str(2), 0, 16);
		//line.print();
		// ���λ�/ ���̾ ���� 
		//if (w.w_type & W_PRE_SUFFIX) continue;
		
		len = strlen(line.str(0));
		w.w_len = len;
		w.key = (char *)line.str(0);
		
		w.w_id = 0;
					
		//w.category = 0;
		w.level = (byte) strtol( line.str(4), 0, 10);
		w.PS = (uint4) strtol( line.str(5), 0, 16);
		w.has_drv = (byte) strtol( line.str(6), 0, 16);
		w.is_drv = (byte) strtol( line.str(7), 0, 16);
		w.is_conj = (byte) strtol( line.str(8), 0, 16);

		if (line.size() >= 10 && strcmp(w.key, (char *)line.str(9))  != 0)
			w.org_word = (char *)line.str(9);
		else 
			w.org_word = 0;
			
		dbStore::storeDB_worddict(&w);
		if (w.w_id==0) {
			printf("insert fail: %s\n", w.key);
		}
		else { 
			cnt++;
		}
		if (cnt % 100 ==  0) printf("cnt=%d\n", cnt);
		if (w.org_word)
		{
			ACE_DEBUG((LM_DEBUG, "%s  %s %d [WN]\n", 
				w.org_word, w.key, w.w_id));
			
		}

		/*
		#ifdef WIN32
		Sleep(500); // miliseconds
		#else
		usleep(1000*100); // microseconds
		#endif
		*/
	}

	ACE_DEBUG((LM_DEBUG, "%d words inserted\n", cnt));
	ACE_DEBUG((LM_DEBUG, "LAST ID=%d\n", w.w_id));
	
	fclose(fp);
	return 0;
}


int	_sisaPS2anydictPS[14] = {
	0,
	INTJ,
	ARTI,
	PRON,
	VERB,
	NOUN,
	ADV,
	PREP,
	CONJ,
	ADJ,
	AUX_VERB,
	0,
	0,
	INTJ
};

int	sisaPS2anydictPS(char *pslist)
{
	MemSplit	line(5, 10);
	int	i;
	int anydictPS=0;
	line.split(pslist, ',');
	for(i=0; i<line.num(); i++) {
		int sisaps = line.intval(i);
		if (sisaps > 0 && sisaps < 14)
			anydictPS |= _sisaPS2anydictPS[sisaps];
	}
	return anydictPS;
}

int	cleanSisadata(char *dest, int max, char *word)
{
	int	i=0;
	byte	ch=0, ch2;
	while( (ch=*word++) && i<max) {
		if (ch & 0x80) {
			ch2 = *word++;
			if (ch==0xA1 && ch2==0xAF)	{
				*dest++ = '\'';
				i++;
			}
		}
		else {
			*dest++ =ch;
			i++;
		}
	}
	*dest = 0;
	return 0;
}

int	remove_parenthesis(char *dest, char *word)
{
	while(*word) {
		if (*word!='(' && *word!=')')
			*dest++ = *word;
		word++;
	}
	*dest = 0;
	return 0;
}

/**
YBM Sisa ���� ����Ÿ �ܾ DB�� �߰�.
�������� ���� ����: ������ �ְų� slash�� �ִ� ��.


�ܾ�|����|ǰ����

batter|2|0,4,5
battered child[baby] syndrome|0|0


0 : ǰ�� ����
1 : ��ź��
2 : ����
3 : ����
4 : ����
5 : ���
6 : �λ�
7 : ��ġ��
8 : ���ӻ�
9 : �����
10 : ������
11 : ���λ�
12 : ���̻�
13 : ������
*/
int loadWords_ybmsisa(char *fname)
{

	FILE *fp;
	char buf[512];
	int	ch;
	int	cnt=0;
	wordType	w;
	MemSplit	line(3, 100);
	char	word[100];
	char	word2[100];
	char *ptr=0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL && cnt++ < 1000000) {
		int len=0;
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		
		line.split(buf,'|');
		if (line.size() != 3) {
			ACE_DEBUG((LM_DEBUG, "line wrong: %s\n", buf));	
			continue;
		}
		if (strchr(line.str(0), ' ')) continue; // ����� ����
		if (strchr(line.str(0), '-')) continue; // ����� ����
		if (line.str(0)[0]=='-') continue;
		cleanSisadata(word, sizeof(word)-1, line.str(0) );
		if (dbStore::searchDB_worddict(word) ) continue; // DB ������ ����
		
		memset(&w, 0, sizeof(w));
		
		w.level = line.intval(1);
		w.PS =sisaPS2anydictPS( line.str(2) );

		ptr = strchr(word, '(');
		if (ptr) {
			remove_parenthesis(word2, word);
			*ptr = 0;
		}
		
		w.key = word;		
		dbStore::storeDB_worddict(&w);
		if (w.w_id==0) {
			printf("insert fail: %s\n", w.key);
		}
		else { 
			cnt++;
		}
		if (cnt % 100 ==  0) printf("cnt=%d\n", cnt);
		
		//ACE_DEBUG((LM_DEBUG, "%s %d %X\n", w.key, w.level, w.PS));	

		if (ptr && strcmp(word, word2) != 0) {
			w.key = word2;
			w.org_word = word;
			dbStore::storeDB_worddict(&w);
			ACE_DEBUG((LM_DEBUG, "%s %s %d [YBM]\n", w.key, w.org_word, w.w_id));	
			cnt++;
		}
		
	}
	
	fclose(fp);
	ACE_DEBUG((LM_DEBUG, "%d inserted from SISA data\n", cnt));
	return 0;
}






/**
WordNet ����� ���� ����Ÿ�� �ܾ DB�� �߰�.
*/
int loadIdioms_wordnet(char *fname)
{

	FILE *fp;
	char buf[512];
	int	ch;
	int	cnt=0;
	char	idiom[128];
	MemSplit	line(12, 48);
	int	id=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		int len=0;		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		line.split(buf);
		
		line.join(idiom, sizeof(idiom)-1, 0, line.find(":"), ' ');
		
		//printf("%s*\n", idiom);	
		id = dbStore::storeDB_idiomdict(idiom);
		if (id==0) {
			printf("insert failed: %s\n", idiom);
		}
		else { 
			cnt++;
		}
		if (cnt % 100 ==  0) printf("cnt=%d\n", cnt);
		
		//if (cnt > 10) break;
	}

	printf("%d idioms inserted\n", cnt);
	ACE_DEBUG((LM_DEBUG, "%d idioms inserted\n", cnt));
	ACE_DEBUG((LM_DEBUG, "LAST ID=%d\n",id));
	
	fclose(fp);
	return 0;
}

int	insert_Idiom(char *idiom)
{	
	int id=0;
	if (dbStore::searchDB_idiomdict(idiom) ) return 0; // DB ������ ����		
	//printf("%s\n", idiom);	
	id = dbStore::storeDB_idiomdict(idiom);
	if (id==0) {
			printf("insert failed: %s\n", idiom);
	}
	ACE_DEBUG((LM_DEBUG, "%s\n", idiom));
	return id;
}

/**
adamant : (as) hard as adamant �� ���� ���
hard as adamant, as hard as adamant �� �� ���� ��� ����� ����.
*/
int	proc_parenthesis(char *idiom)
{
	char clean_idiom[100];
	if (strchr(idiom, '(')) {
		StrUtil::trim_between(clean_idiom, sizeof(clean_idiom)-1, idiom, '(', ')' );
		insert_Idiom(clean_idiom);

		StrUtil::trim_chars(clean_idiom, sizeof(clean_idiom)-1, idiom, "()");
		insert_Idiom(clean_idiom);
		return 2;
	}
	else {
		insert_Idiom(idiom);
		return 1;
	}
	
}

/**
* sisa_idiom.dict ��� :
wrong : get[or have] (hold of) the wrong end of the stick
write : writ[or written] large
boner : pull[or make] a ~
born : of woman born =~ of woman.
ace : have[or keep] an ace[or a card] up one

* sisa_word.dict ��� :
acid precipitation[deposition]|0|5
accommodation bill[draft, note, paper]|0|5
absentee landlord [landowner]|0|5

�� �Լ� �Ķ���� main_word �� sisa_idiom.dict ��쿡�� �����Ǹ�,
sisa_word.dict ���� null �̴�.
*/

int	proc_multiple(char *idiom, int buflen, char *main_word)
{
	char *pos = strchr(idiom, '[');
	char clean_idiom[100];

	if (main_word && strchr(idiom, '~') ) {
		StrUtil::replaceStr(clean_idiom, sizeof(clean_idiom)-1, idiom, "~", main_word);
		strcpy(idiom, clean_idiom);
	}
	
	/* [ �� ���� ��쿡�� �׳� ó�� */
	if (pos==0) {
		return proc_parenthesis(idiom);
	}

	char	similiar[100]; // [] �ȿ� �� ���ڿ�.
	char sim_clean[100];
	char baseword[32]; // [] �ȿ� �� �ܾ���� �⺻ �ܾ�. get[or have]  ���� get �� ���ϱ� ����.
	
	int	len = (unsigned int)pos -  (unsigned int)idiom;
	MemSplit words(10, 64);

	
	StrUtil::copy_rightword(baseword, sizeof(baseword)-1, idiom, len);
	StrUtil::copy_between(similiar, sizeof(similiar)-1, idiom, '[', ']');
	StrUtil::trim_between(clean_idiom, sizeof(clean_idiom)-1, idiom, '[', ']' );
	if (strncmp(similiar, "or ",3)==0) {
		strcpy(sim_clean, similiar+3);
	}
	else if (strncmp(similiar, " or ",4)==0) {
		strcpy(sim_clean, similiar+4);
	}
	else {
		strcpy(sim_clean, similiar);
	}

	#ifdef DEBUG
	printf("%s\n", idiom);
	printf("baseword= %s\n", baseword);
	printf("similiar= %s\n", sim_clean);	
	printf("clean_idiom= %s\n", clean_idiom);
	#endif
	
	int	cnt = 0;
	int	i=0;
	char sim_word[32];
	cnt += proc_parenthesis(clean_idiom);
	
	/*
	[] �ȿ� ��� �ִ� �ܾ���� �����Ͽ�..
	*/
	words.split_trim(sim_clean, ',');

	/* ����� �ܾ 2 �ܾ� �̻����� ������ ��� ó�� ����. */
	for(i=0; i<words.size(); i++) {
		if (strchr(words.str(i), ' ') ) return cnt;
	}
	
	for(i=0; i<words.size(); i++) {
		StrUtil::copy_alphabet(sim_word, words.str(i), sizeof(sim_word)-1);
		#ifdef DEBUG
		//printf("[%d] %s\n", i, sim_word);
		#endif

		StrUtil::replaceStr(idiom, buflen, clean_idiom, baseword, sim_word);
		#ifdef DEBUG
		//printf("replaced: %s\n", idiom);
		#endif
		cnt += proc_multiple(idiom, buflen, main_word);
		
	}
	return cnt;	
	
	
}

/**
YBM Sisa ���� ����Ÿ ���� �� DB�� �߰�.
�ܾ� �������� : ������ �ְų� slash�� �ִ� ���� ����� ����
���վ� ���� ���� ����Ÿ�� ������.
*/
int loadCompounds_ybmsisa(char *fname)
{

	FILE *fp;
	char buf[512];
	int	ch;
	int	cnt=0;
	MemSplit	line(3, 100);
	char	idiom[100];
	char *ptr=0;
	
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		int len=0;
		
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		
		line.split(buf,'|');
		if (line.size() != 3) {
			ACE_DEBUG((LM_DEBUG, "line wrong: %s\n", buf));	
			continue;
		}
		
		if (strchr(line.str(0), ' ')==0 && strchr(line.str(0), '-')==0) continue; 
		if (strchr(line.str(0), ',') ) continue;
		if (strchr(line.str(0), '?') ) continue;
		if (line.str(0)[0]=='-') continue;
		
		cleanSisadata(idiom, sizeof(idiom)-1, line.str(0) );
		StrUtil::trim_right(idiom);
		if (strchr(idiom, ' ')==0) continue;

			
		cnt += proc_multiple(idiom, sizeof(idiom)-1, 0);
		//if (cnt > 10) break;

		
	}
	
	fclose(fp);
	ACE_DEBUG((LM_DEBUG, "%d inserted from SISA data\n", cnt));
	ACE_DEBUG((LM_DEBUG, "LAST ID=%d\n", 
		dbStore::getMaxValue("idiomdict", "idiom_id")  ));
	return 0;
}

/**
sisa ���� �������� �� ���� �� ó��.
return : ���� ����
*/
int	one_idiom_ybmsisa(char *line)
{
	char mainword[32];
	char	idiom[100];
	char *ptr;
	int	ch;
	
	ch = StrUtil::first_nonspace(line);		
	if (ch==0 || ch=='#') return 0;

	ptr = strchr(line, ':');
	if (ptr==0) return 0;
	StrUtil::copy_alphabet(mainword, line, sizeof(mainword)-1);
	StrUtil::trim_from(ptr+1, ';'); // ; ���� ���� ����.
	StrUtil::trim_from(ptr+1, '='); // ; ���� ���� ����.
	StrUtil::trim_from(ptr+1, '.'); // ; ���� ���� ����.
	StrUtil::trim_right(ptr+1); // ���� ����.
	if (StrUtil::countChar(ptr+1, '[') != StrUtil::countChar(ptr+1, ']') )  return 0;
	if (ISSPACE(ptr[1])) strcpy(idiom, ptr+2);
	else strcpy(idiom, ptr+1);
		
	return proc_multiple(idiom, sizeof(idiom)-1, mainword);
}

/**
�û� ����Ÿ�� ���� (�ַ� ���� ���� �߿� ���� �͵� - ���� + ��ġ�� ���) DB�� �߰�.

avenue : explore every avenue; leave no avenue unexplored
*/
int loadIdioms_ybmsisa(char *fname)
{

	FILE *fp;
	char buf[512];
	int	cnt=0;
	
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		int len=0;		
		cnt += one_idiom_ybmsisa(buf);		
		//if (cnt > 10) break;
	}

	ACE_DEBUG((LM_DEBUG, "%d idioms inserted\n", cnt));
	ACE_DEBUG((LM_DEBUG, "LAST ID=%d\n", 
		dbStore::getMaxValue("idiomdict", "idiom_id")  ));
	
	fclose(fp);
	return 0;
}

void test_proc_multi()
{
	char idiom[100];
	char mainword[32];

	strcpy(idiom, "ass over tincups[or teakettle, teacups, tit, appetite]");
	strcpy(mainword, "ass");
	proc_multiple(idiom, sizeof(idiom)-1, mainword);

	strcpy(idiom, "apply[or put on, hit, jam on, slam on] the[or one]");
	strcpy(mainword, "brake");
	proc_multiple(idiom, sizeof(idiom)-1, mainword);
}

void test_idiom_ybmsisa()
{
	char line[100];
	strcpy(line, "ass : ass over tincups[or teakettle, teacups, tit, appetite] =ass over tip.");
	one_idiom_ybmsisa(line);
	
	strcpy(line, "ass : get a person off his ass[or butt, dead ass, duff]");
	one_idiom_ybmsisa(line);
}
/**
�ܾ�, ��� DB  table �� �߰��ϱ� ���� ��.
*/

int
main (int argc, char **argv)
{
	int	res=0;

	/*
	if (argc < 2) {
		printf("option: w for word, i for idiom\n");
		exit(0);
	}
	*/
	init_env();	

	/*
	res = dbStore::prepare(
		config.GetStrVal("DBNAME", "ANYDICT"),
		config.GetStrVal("DBID", "inisoft"),
		config.GetStrVal("DBPASS", "gksehf")
		);
	*/
	res = dbStore::prepare( "ANYDICT", "inisoft", "gksehf");
	if (res < 0) {
		return -1;
	}

	res = dbStore::searchDB_worddict("took");
	ACE_DEBUG((LM_DEBUG, "searchDB_worddict() res = %d\n", res));
	
	char fname[256];

	if (argc > 1 && argv[1][0]=='w') {
	/* wordnet ����Ÿ �Է��� ���� �ϰ� sisa ����Ÿ �߰�.
	sisa ����Ÿ �߰��ÿ��� �ߺ��˻縦 �ϰ� �ִ�.
	*/
	loadWords_wordnet( StrUtil::path_merge(fname, adhome, "data/word.dict") );
	loadWords_ybmsisa( StrUtil::path_merge(fname, adhome, "data/sisa_word.dict") );
	}

	//test_idiom_ybmsisa();
	
	if (argc > 1 && argv[1][0]=='i') {
	
	loadIdioms_wordnet( StrUtil::path_merge(fname, adhome, "data/idiom.dict") );
	
	loadCompounds_ybmsisa( StrUtil::path_merge(fname, adhome, "data/sisa_word.dict") );

	loadIdioms_ybmsisa( StrUtil::path_merge(fname, adhome, "data/sisa_idiom.dict") );
	}
	return 0;
		
}

/*
DB �۾� ���:
wordnet ����Ÿ(word.dict)���� 89003 �ܾ�

sisa_word.dict ���� �� �ܾ�.
�� ���: 157063

select org_word from worddict where org_word!='';

----------------
wordnet ����Ÿ(idiom.dict)���� 63506 ����.
sisa ����: 34579 ����

��������.

1. wordnet ����Ÿ(idiom.dict)���� 62382 ���� ����.
LAST ID=62382

2. 58123 inserted from SISA data (sisa_word.dict)
LAST ID=103639

3. 43057 idioms inserted (sisa_idiom.dict - 34579 ���� )
LAST ID=137096

select count(*) from idiomdict where substring(idiom, 1,1) = ' ';
*/

