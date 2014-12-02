#include <stdio.h>
#include <string.h> // strstr()
#include <ctype.h>
#include <stdlib.h> // strtol()


// 2003.2.25
#define	MAX_PREFIX	(100)
#define	MAX_SUFFIX	(100)
char prefix_list[MAX_PREFIX][10];
char suffix_list[MAX_SUFFIX][10];
int	nums_prefix=0;
int	nums_suffix=0;

char *prefix_ptr[MAX_PREFIX];
char *suffix_ptr[MAX_SUFFIX];

unsigned char prefix_len[MAX_PREFIX];
unsigned char suffix_len[MAX_SUFFIX];


int load_prefix_suffix(char *fname)
{
	FILE *fp;
	char buf[128], word[64];
	char inverse[64];
	int	ch;
	int	len=0;
	char *ptr=0;
	if ( (fp=fopen(fname, "r")) == NULL) {
		PRN("Read Fail: %s\n", fname);
		return -1;
	}
	while (fgets(buf, 256, fp) != NULL) {		
		ptr = skip_space(buf);		
		if (*ptr==0 || *ptr=='#') continue;		

		strcpy(word, ptr);
		len = strlen(word);
		if (word[len-1]=='\n') {
			word[len-1]=0;
			len--;
		}
		if (word[0]=='-' && nums_suffix<MAX_SUFFIX) { // suffix
			inverse_str(word+1, inverse, len-1);
			strcpy(suffix_list[nums_suffix++], inverse);
		}
		else if (word[len-1]=='-' && nums_prefix<MAX_PREFIX) { // prefix
			strcpy(prefix_list[nums_prefix], word);
			prefix_list[nums_prefix][len-1] = 0;
			nums_prefix++;
		}
	}	
	
	fclose(fp);
	return (nums_prefix+nums_suffix);
}


int make_prefix_suffix_idx()
{
	int i;
	for(i=0; i<nums_prefix; i++)
		prefix_ptr[i] = prefix_list[i];
	for(i=0; i<nums_suffix; i++)
		suffix_ptr[i] = suffix_list[i];

	sort_str_arr(prefix_ptr, nums_prefix);
	sort_str_arr(suffix_ptr, nums_suffix);

	#ifdef BUG2
	for(i=0; i<nums_prefix; i++)
		PRN("%s\n", prefix_ptr[i]);
	for(i=0; i<nums_suffix; i++)
		PRN("%s\n", suffix_ptr[i]);
	#endif

	for(i=0; i<nums_prefix; i++)
		prefix_len[i] = strlen(prefix_ptr[i]);
	for(i=0; i<nums_suffix; i++)
		suffix_len[i] = strlen(suffix_ptr[i]);
	return 0;
}


int search_prefix_suffix(char *word)
{
	int	len, i;
	char head_ch, tail_ch;
	char inverse[128];
	char tmp[128], org_suffix[128];
	int	found=0;
	wordType *w;
	
	len = strlen(word);
	head_ch = word[0];
	tail_ch = word[len-1];

	

	for(i=0; i<nums_prefix; i++) {
		if (head_ch < prefix_ptr[i][0]) break;
		if (strncmp(word, prefix_ptr[i], prefix_len[i])==0) {
			#ifdef BUG
			//printf("prefix: %s\n", prefix_ptr[i]);
			#endif
			
			//word[len-prefix_len[i]] = 0;
			strcpy(tmp, word+prefix_len[i]);
			w = HASH_SEARCH(tmp);
			if (w) {
				printf("%s <-- %s : prefix %s\n", word, w->word, prefix_ptr[i]);
				found = 1;
				break;
			}
			/*
			if (prefix_ptr[i][prefix_len[i]-1]=='i') {			
				// 'e'나 'y'를 붙여본다 
				word[len-prefix_len[i]] = 'y';
				word[len-prefix_len[i]+1] = 0; 
				w = HASH_SEARCH(word);
				if (w) printf("org %s\n", w->word);
			
				word[len-prefix_len[i]] = 'e';
				word[len-prefix_len[i]+1] = 0;
				w = HASH_SEARCH(word);
				if (w) printf("org %s\n", w->word);
			}
			*/
			//if (word in DICT)
			// found = 1;
		}
	}
	if (found) return 1;

	inverse_str(word, inverse, len);
	for(i=0; i<nums_suffix; i++) {
		if (tail_ch < suffix_ptr[i][0]) break;
		if (strncmp(inverse, suffix_ptr[i], suffix_len[i])==0) {
			#ifdef BUG
			//printf("suffix: %s\n", suffix_ptr[i]);
			#endif	
			//word[len-prefix_len[i]] = 0;
			strcpy(tmp, word);
			tmp[len-suffix_len[i]]=0;
			w = HASH_SEARCH(tmp);
			if (w) {
				inverse_str(suffix_ptr[i], org_suffix, suffix_len[i]);
				printf("%s <-- %s : suffix %s\n", word, w->word, org_suffix);
				found = 1;
				break;
			}
			
			/*
			if (suffix_ptr[i][suffix_len[i]-1]=='i') {
				
				// 'e'나 'y'를 붙여본다 
			}
			word[len-suffix_len[i]] = 0;

			word[len-suffix_len[i]] = 'y';
			word[len-suffix_len[i]+1] = 0; 

			word[len-suffix_len[i]] = 'e';
			word[len-suffix_len[i]+1] = 0; 
			//if (word in DICT)
			// found = 1;
			*/
		}
	}

	
	
	return found;
}

wordType *get_prefix_suffix_word(char *word)
{
	int	len, i;
	char head_ch, tail_ch;
	char inverse[128];
	char tmp[128], org_suffix[128];
	int	found=0;
	wordType *w;
	
	len = strlen(word);
	head_ch = word[0];
	tail_ch = word[len-1];

	for(i=0; i<nums_prefix; i++) {
		if (head_ch < prefix_ptr[i][0]) break;
		if (strncmp(word, prefix_ptr[i], prefix_len[i])==0) {
			#ifdef BUG
			//printf("prefix: %s\n", prefix_ptr[i]);
			#endif
			
			//word[len-prefix_len[i]] = 0;
			strcpy(tmp, word+prefix_len[i]);
			w = HASH_SEARCH(tmp);
			if (w==0) {
				//printf("%s <-- %s : prefix %s\n", word, w->word, prefix_ptr[i]);
				w = check_conj_simple(tmp);
			}
			
			if (w) {
				strcpy(word+prefix_len[i], w->word);
				return w;
			}
		}
	}

	inverse_str(word, inverse, len);
	for(i=0; i<nums_suffix; i++) {
		if (tail_ch < suffix_ptr[i][0]) break;
		if (strncmp(inverse, suffix_ptr[i], suffix_len[i])==0) {
			#ifdef BUG
			//printf("suffix: %s\n", suffix_ptr[i]);
			#endif	
			//word[len-prefix_len[i]] = 0;
			strcpy(tmp, word);
			tmp[len-suffix_len[i]]=0;
			w = HASH_SEARCH(tmp);
			if (w) {
				//inverse_str(suffix_ptr[i], org_suffix, suffix_len[i]);
				//printf("%s <-- %s : suffix %s\n", word, w->word, org_suffix);
				found = 1;
				return w;
			}
			
		}
	}

	return 0;
}


int prepare_prefix_suffix()
{
	char *home;
	char buf[256];
	int n;
	home = getenv("ANYDICT_HOME");
	sprintf(buf, "%s/data/prefix_suffix", home);
	n = load_prefix_suffix(buf);
	PRN("prefix, suffix : %d\n", n);
	make_prefix_suffix_idx();
	return n;
}

#ifdef AAA
int test_prefix_suffix()
{
	int	len;	
	char *home;
	char buf[256];
	int n;

	printf("== Prefix, Suffix Test ==\n");
	home = getenv("HOME");
	sprintf(buf, "%s/data/prefix_suffix", home);
	printf("Prefix, Suffix : %s\n", buf);
	n = load_prefix_suffix(buf);
	printf("prefix, suffix : %d\n", n);
	make_prefix_suffix_idx();
	
	
	printf("Enter word: ");
	while(scanf("%s", buf)!=0) {
		//buf[strlen(buf)-1]=0;
		search_prefix_suffix(buf);		
		printf("Enter word: ");
	}
	return 0;
}
#endif
