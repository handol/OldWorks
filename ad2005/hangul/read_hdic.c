#include <stdio.h>
#include "hangul_hash_d.h"
#include "hantail_hash_d.h"
#include "str_util.h"
#include "mem_mng.h"
#include "CLog.h"

#ifdef AD_SHM
#define ALLOC SALLOC
#define strALLOC str_SHMalloc
#define strALLOC_n str_SHMalloc_n
#else

#define ALLOC GALLOC
#define strALLOC str_GALLOC
#define strALLOC_n str_GALLOC_n
#endif

typedef unsigned char byte;

CHangulHash *hdicHash=0;
int	hdicSize=0;
int init_hdic_hash()
{
	hdicHash = new CHangulHash();
	hdicSize = hdicHash->init_hash_table(30000);
	return hdicSize;
}

// 한 라인에 한글외 문자가 있으면 return 0
int get_hangul_only(byte *line)
{
	int	twobyte=0;
	while (*line) {
		if (*line=='\r' || *line=='\n') {
			*line = 0;
			return 1;
		}
		if ((*line) & 0x80) {
			twobyte = *line << 8;
			line++;
			twobyte |= *line;
			
			if (twobyte < 0xB0A1 || twobyte > 0xC8FE) {
				*line = 0;
				return 0;
			}
		}
		
		line++;
	}
	return 1;	
}

int take_hangul_only(byte *line, byte *hword)
{
	int	twobyte=0;
	int	n_hanchars=0;
	while (*line) {
		if (*line=='\r' || *line=='\n') {
			break;
		}
		if ((*line) & 0x80) {
			//twobyte = *line << 8;
			*hword++ = *line++;
			//twobyte |= *line;
			n_hanchars++;
					/*
			if (twobyte < 0xB0A1 || twobyte > 0xC8FE) {
				*line = 0;
				return 0;
			}
					*/
		}
		else
			line++;
	}

	*hword = 0;
	return n_hanchars;	
}

int read_hdic_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	int ch;	
	HangulType *hanguldata;
	int zero_len=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	while (fgets(buf, 128, fp) != NULL) {
		ch = first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		//if (get_hangul_only((byte*)buf)==0) {
		if (take_hangul_only((byte*)buf, (byte*)hanword)==0) {
			nohangul++;
			#ifdef BUG
			printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
		}
		cnt++;
		if (strlen(buf)==0) {
			zero_len++;
			continue;
		}
		
		hanguldata = (HangulType*)ALLOC(sizeof(HangulType));
		hanguldata->word = strALLOC(buf);
		hdicHash->hash_insert(hanguldata);
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d\n", cnt, nohangul);
	printf("zero_len=%d\n", zero_len);
	return 0;
}


int read_freq_words_file(char *fname)
{
	int cnt=0, nohangul=0;
	FILE *fp;
	char buf[128], hanword[64];
	int ch;	
	HangulType *hanguldata;
	int zero_len=0;
	int freq_rank=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	printf("file: %s\n", fname);
	
	while (fgets(buf, 128, fp) != NULL) {
		ch = first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		if (take_hangul_only((byte*)buf, (byte*)hanword)==0) {			
			nohangul++;
			#ifdef BUG
			printf("%d %d %d %s\n", cnt, nohangul, strlen(buf), buf);
			#endif
			continue;
		}
		cnt++;
		//hanguldata = (HangulType*)ALLOC(sizeof(HangulType));
		//hanguldata->word = strALLOC(buf);
		hanguldata = hdicHash->hash_search(hanword);
		#ifdef BUG
		if (hanguldata)
			printf("%s O\n", hanword);
		else {
			hanguldata = (HangulType*)ALLOC(sizeof(HangulType));
			hanguldata->word = strALLOC(buf);
			hanguldata->freq = freq_rank++;
			hdicHash->hash_insert(hanguldata);
		}
		/*
		if (hanguldata)
			printf("%s O\n", hanword);
		else
			printf("%s X\n", hanword);
		*/
		#endif
	}
	fclose(fp);
	printf("cnt=%d nohangul=%d\n", cnt, nohangul);
	printf("zero_len=%d\n", zero_len);
	return 0;
}


CLog *gLog;
int main(int argc, char *argv[])
{
	int shm;

	#ifdef LOG
	//gLog = new CLog(anydict_log_dir, argv[0]);
	gLog = new CLog(".", argv[0]);
	#else
	init_Log(anydict_log_dir, argv[0]);
	#endif	

	prepare_my_mem_alloc(1024*1024, 50);
	init_hdic_hash();
	read_hdic_file("dict4morph/WordList/1.txt");
	read_hdic_file("dict4morph/WordList/2.txt");
	read_hdic_file("dict4morph/WordList/3.txt");
	read_hdic_file("dict4morph/WordList/4.txt");
	read_hdic_file("dict4morph/WordList/5.txt");
	hdicHash->prn_hash_stat();
	
	read_freq_words_file("dict4morph/wf2000.txt");
	hdicHash->prn_hash_stat();
}
