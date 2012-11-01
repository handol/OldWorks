#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // qsort()

#include "StrUtil.h"

#define	SP	(' ')
#define	TAB	('\t')
#define	NEWLINE 	('\n')


/**
string 의 공백문자를 제거.
*/
int StrUtil::trimLeft(char *dest, int max, char *str)
{
	int	n;
	for(n=0; n<max && *str; str++)
	{
		if(!ISSPACE(*str))
		{
			*dest++ = *str ;
			n++;
		}
	}
	*dest = 0;
	return n;
}

/**
string 의 오른쪽 끝의 공백문자를 제거.
*/
int StrUtil::trimRight(char *str, int n)
{
	int	i;
	if (n==0) n = strlen(str);
	if (n==0) return 0;

	str += (n-1);
	for(i=n; i>0; i--, str--)
	{
		if(ISSPACE(*str))
		{
			*str = 0;
		}
		else
			break;
	}
	return 0;
}

/**
*/
int StrUtil::trimFrom(char *str, int ch)
{
	char *ptr = strchr(str, ch);
	if(ptr) *ptr =0;
	return 0;
}

/**
string 의  특정 문자를 제거.
*/
int StrUtil::trimChars(char *dest, int max, char *str, char *remove)
{
	int	n;
	for(n=0; n<max && *str; str++)
	{
		if (strchr(remove, *str)==0)
		{
			*dest++ = *str;
			n++;
		}
	}

	*dest = 0;
	return n;
}

/**
string 의  특정 문자 쌍 사이 문자열 를 제거.
*/
int StrUtil::trimBetween(char *dest, int max, char *str, char begin, char end)
{
	int	n;
	for(n=0; n<max && *str; )
	{
		if (*str==begin)
		{
			str++;
			while (*str != end && *str)
				str++;

			if (*str==end) str++;
		}
		else
		{
			*dest++ = *str;
			n++;
			str++;
		}
	}

	*dest = 0;
	return n;
}

/**
string 의  특정 문자 쌍 사이 문자열 만 복사.
*/
int StrUtil::copyBetween(char *dest, int max, char *str, char begin, char end)
{
	int	n;
	for(n=0; n<max && *str; )
	{
		if (*str==begin)
		{
			str++;
			while (*str != end && *str )
			{
				*dest++ = *str++;
				n++;
			}
			if (*str==end) str++;
		}
		else
		{
			str++;
		}
	}
	*dest = 0;
	return n;
}

/**
orgstr 문자열의 뒷부분에서 alphabet 문자열을 구해온다.
*/
int	StrUtil::copyRightWord(char *dest, int max, char *orgstr, int orglen)
{
	int	i;
	int	n = orglen;
	char *str = orgstr;

	*dest = 0;
	if (n==0)
		n = strlen(orgstr);

	if (n==0)
		return 0;

	str += (n-1);
	for(i=n-1; i>=0 && ! isalpha(*str); i--, str--)
	{
	}

	if (i==0)
		return 0;

	int	endpos = i;

	for(; i>=0 && isalpha(*str); i--, str--)
	{
	}

	int	copylen = endpos - i;
	if(copylen > max)
	{
		copylen = max-1;
	}

	strncpy(dest, str+1, copylen);

	dest[copylen] = 0;

	return copylen;
}

int StrUtil::trimText(char *target, char *source, int trg_len)
{
	register char *src, *trg;
	register int ch;
	char *prev_line=0;
	int	n;
	int	whiteline=1, prev_white=1, istab;
	int	whitesp=0, n_alpha=0;

	for (n=0, src=source, trg=target; (ch=*src) && (trg-target) < trg_len; src++)
	{
		if (ch==SP || ch==TAB)
		{
			// 연속되는 공백은 하나의 공백으로 전환. tab이 포함되어 있으면 tab으로 전환
			whitesp++;
			istab = 0;
			while(*src==SP || *src==TAB)
			{
				if (*src==TAB) istab = 1;
				src++;
			}

			if (istab)
				*trg++ = TAB;
			else if (! whiteline)
					*trg++ = SP;

			ch = *src;
		}

		if (ch==0)
			break;

		if (ch=='\n')
		{
			//if (n_alpha==0 || (whitesp <= 4 && n_alpha>2 && n_alpha==n_upper))
			//2003.3.26
			if (n_alpha==0)
			{
				// 이런 라인은 제거
				if (prev_line)
					trg = prev_line;
				else
					trg = target;

				whiteline = 1;
			}

			if (!prev_white || !whiteline)
			{
				*trg++ = '\n';
				prev_line = trg;
			}

			prev_white = whiteline;
			whiteline = 1;

			whitesp = 0;
			n_alpha = 0;
			//n_upper = 0;
		}
		else if (ch!='\r')
		{
			*trg++ = ch;
			whiteline = 0;

			if (isalpha(ch))
			{
				n_alpha++;
				// 2003.3.26 n_upper는 사용되지 않음
				//if (isupper(ch)) n_upper++;
			}
		}
	}// for

	*trg=0;
	if ((int)(trg-target) < trg_len-5)
		memset(trg, 0, 4);

	return (trg-target);
}

/**
org 문자열 내에서 oldstr 문자열을 newstr 문자열로 대체하여 새로운 결과물을
dest 에 저장한다.
dest의 최대 길이는 max 이다.
*/
int	StrUtil::replaceStr(char *dest, int max, char *org, char *oldstr, char *newstr)
{
	char *pos=strstr(org, oldstr);
	if (pos==0)
	{
		strncpy(dest, org, max);
		dest[max] = 0;
		return 0;
	}

	int orglen = strlen(org);
	int newstrlen = strlen(newstr);
	int oldstrlen = strlen(oldstr);
	int newlen = orglen + (newstrlen - oldstrlen);

	if (max < newlen)
	{
		dest[0] = 0;
		return 0;
	}

	//int headlen = (unsigned int)pos - (unsigned int)org;
	int headlen = pos - org;
	strncpy(dest, org, headlen);
	strcpy(dest+headlen, newstr);
	strcpy(dest+headlen+newstrlen, pos+oldstrlen);
	return newlen;
}

// char 'org' --> 'new'
int StrUtil::replaceChar(char *str, int org, int newch)
{
	int	cnt=0;
	while(*str)
	{
		if (*str == org)
		{
			cnt++; *str = newch;
		}
		str++;
	}
	return cnt;
}

// char 'org' --> 'new'
int StrUtil::replaceChars(char *str, char* orgchars, int newch)
{
	int	cnt=0;
	while(*str)
	{
		if (strchr(orgchars, *str))
		{
			cnt++; *str = newch;
		}
		str++;
	}
	return cnt;
}

//----------
int StrUtil::copyReplaceChar(char *trg, char *src, int org, int newch)
{
	int	cnt=0;
	while(*src)
	{
		if (*src == org)
		{
			cnt++; *trg++ = newch;
		}
		else
			*trg++ = *src;

		src++;
	}
	*trg = 0;
	return cnt;
}

/*---------------------
// line_to_args() 쓰는 법
char	words[MAX_COLS][STR_LEN+1];
char	*ptrs[MAX_COLS];
int		n_cols;

char	buf[512];
int	i;

for(i=0; i<MAX_COLS; i++) ptrs[i] = words[i];
n_cols = line_to_args((char **)ptrs, MAX_COLS, STR_LEN, buf);
--------------------*/


int StrUtil::line2args(char **argv, int max_arr, int max_strlen, char *src)
{
	int i, n;
	char *ptr;
	for (i=0; i<max_arr; i++)
		*argv[i] = 0;
	for (i=0; i<max_arr && *src; i++) {
	    ptr = argv[i];
	    while(*src && ISSPACE(*src)) src++;
	    if (*src == 0 ) break;
	    n = 0;
	    while(n < max_strlen && *src && !ISSPACE(*src))
	        { *ptr++ = *src++; n++; }
	    *ptr = 0;
	}
	return (i);
}

int StrUtil::argv2line(char *line, int maxlen,  int argc, char **argv)
{
	int i,len=0, all_len=0;
	char *ptr = line;
	for (i=0; i<argc; i++) {
		len = strlen( argv[i] ) + 1;
		all_len += len;
	    if (all_len > maxlen) {
	    	int over = all_len - maxlen;
	    	strncpy(ptr, argv[i], len-over);
	    	ptr[len-over] = 0;
	    	return maxlen;
	    }
	    strcpy(ptr, argv[i]);
	    ptr[len-1] = ' ';
	    ptr += len;
	}
	ptr[all_len-1]=0;
	return (all_len-1);
}


/* return argc; the # of arguemnt (fields) */
// input : a line
// output: array of strings, # of strings
int StrUtil::line2words(char **argv, int max_arr, int max_strlen, char *src)
{
	int i, n;
	char *ptr;
	for (i=0; i<max_arr; i++)
		*argv[i] = 0;
	for (i=0; i<max_arr && *src; i++) {
	    ptr = argv[i];
	    while(*src && !isalpha(*src)) src++;
	    if (*src == 0 ) break;
	    n = 0;
	    while(n < max_strlen && isalpha(*src))
	        { *ptr++ = *src++; n++; }
	    *ptr = 0;

	    if(*src=='\'') {
	    	while(isalpha(*src)) src++;
	    }
	}
	return (i);
}


int StrUtil::dirList2array(char **array, int arr_size, char *dir_list_string)
{
	int	i=0, n;
	char *comma;

	while (*dir_list_string && i < arr_size) {
		comma = strchr(dir_list_string, ',');
		if (comma) {
			n = comma - dir_list_string;
			if (n==0) {
				dir_list_string++;
				continue;
			}
			array[i] = (char *)calloc(1, n+1);
			strncpy(array[i], dir_list_string, n);
			if (array[i][n-1]=='/') array[i][n-1]=0;
			else array[i++][n] = 0;
			dir_list_string = comma+1;
		}
		else {
			n = strlen(dir_list_string);
			array[i] = (char *)calloc(1, n+1);
			strcpy(array[i], dir_list_string);
			if (array[i][n-1]=='/') array[i][n-1]=0;
			i++;
			break;
		}
	}
	return i;
}


// 숙어를 공백이나 hyphen으로 조각으로 나눈다
int StrUtil::splitIdiom2words(char **argv, int max_arr, int max_strlen, char *src)
{
	int i, n;
	char *ptr;
	for (i=0; i<max_arr; i++)
		*argv[i] = 0;
	for (i=0; i<max_arr && *src; i++) {
	    ptr = argv[i];
	    while(*src && ISSPACE(*src)) src++;
	    if (*src == 0 ) break;
	    n = 0;
	    while(n < max_strlen && *src && *src!='-' && !ISSPACE(*src))
	        { *ptr++ = *src++; n++; }
	    *ptr = 0;

	    // 소유격 표시는 제거
	    /*
	    // 4.13에 사용.. 4.15일 comment out
	    if (n>2) {
	    	if (*(ptr-1)=='\'') *(ptr-1)=0;
	    	else if (
	    			*(ptr-2)=='\'' && *(ptr-1)=='s'
	    			&& strncmp(argv[i], "one", 3)!=0) // one's는 살려둔다
	    		*(ptr-2)=0;
	    }
	    */
	    if (*src=='-') src++;
	}
	return (i);
}


void StrUtil::invert(char *inverted, char *org, int org_len)
{
	int	i;
	if (org_len==0) org_len = strlen(org);

	org += (org_len - 1);
	for(i=0; i<org_len; i++)
		*inverted++ = *org--;
	*inverted=0;
}

void StrUtil::invertHangul(char *inverted, char *org, int org_len)
{
	int	i;
	if (org_len==0) org_len = strlen(org);

	org += (org_len - 1);
	for(i=0; i<org_len; i += 2) {
		inverted[1] = *org--;
		inverted[0] = *org--;
		inverted += 2;
	}
	*inverted=0;
}


// return the length of 'trg'
void StrUtil::copy2lower(char *trg, char *src)
{
	while (*src) {
		*trg++ = tolower(*src++);
	}
	*trg = 0;
}

// return the length of 'trg'
int StrUtil::copy2char(char *trg, char *src, int max_len, int ch)
{
	int	n=0;
	while (*src && *src!=ch && n < max_len) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}

// copy a nonspace string from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyUpperOnly(char *trg, char *src)
{
	int	n=0;
	while (*src) {
		if (isupper(*src)) {
			*trg++ = *src;
			n++;
		}
		src++;

	}
	*trg = 0;
	return (n);
}

int StrUtil::copyUpperOnlyMax(char *trg, char *src, int max)
{
	int	n=0;
	while (*src && n<max) {
		if (isupper(*src)) {
			*trg++ = *src;
			n++;
		}
		src++;

	}
	*trg = 0;
	return (n);
}

// copy a nonspace string from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyNonspace(char *trg, char *src, int max_len)
{
	int	n=0;
	while (*src && !ISSPACE(*src) && n < max_len) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}

// copy a nonspace string from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyNonspStop(char *trg, char *src, int max_len, int stop)
{
	int	n=0;
	while (*src && !ISSPACE(*src) && *src!=stop && n < max_len) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}

// copy a number from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyNumber(char *trg, const char *src, int max_len)
{
	int	n=0;
	while (ISDIGIT(*src) && n < max_len) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}

// copy a alphabet string from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyAlphabet(char *trg, char *src, int max_len)
{
	int	n=0;
	while (isalpha(*src) && n < max_len) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}

// copy a alphabet string from 'src' to 'trg',
// return the length of 'trg'
int StrUtil::copyVarName(char *trg, char *src, int max_len)
{
	int	n=0;
	while (n < max_len && (isalpha(*src) || ISDIGIT(*src) || *src=='_') ) {
		*trg++ = *src++;
		n++;
	}
	*trg = 0;
	return (n);
}



/*------------------------------------------*/

// for a dict word, check if it has a odd char
int StrUtil::hasOddChar(char *w)
{
	while (*w) {
		if (!isalpha(*w) && *w!=' ' && *w!='-' && *w!='\'' && *w!='.') return 1;
		w++;
	}
	return 0;
}

int StrUtil::isAllUpper(char *s)
{
	while (*s) {
		if (isalpha(*s) && !isupper(*s)) return 0;
		s++;
	}
	return 1;
}

char *StrUtil::myStrchrn(char *str, int ch, int len)
{
	int i=0;
	while (*str && i++<len)
		if (*str == ch) return str;
		else str++;
	return 0;
}

// string 앞부분 len 내에서 역으로 ch 찾기
// strrchr()는 string 맨끝에서 출발해서 찾는다.
// my_strrchr_n() 는 str[len] = 0으로 한 후  strrchr()한 것과 유사
// 즉, my_strrchr_n() 는 str[len]에서 출발하여 역순으로 ch를 찾는다.
char *StrUtil::myStrrchrn(char *str, int ch, int len)
{
	char *ptr;
	if (len < 1) return 0;
	ptr = str + len - 1;
	while (ptr >= str)
		if (*ptr == ch) return ptr;
		else ptr--;
	return 0;
}

// string 뒤 부분 len 내에서 역으로 ch 찾기
char *StrUtil::myStrrchrTailn(char *str, int ch, int len)
{
	char *ptr;
	int	i=0;

	ptr = str + strlen(str) - 1;
	while (i++ < len) {
		if (*ptr == ch) return ptr;
		ptr--;
	}
	return 0;
}

void StrUtil::copyWithoutExt(char *new_fname, char *fname)
{
	char *ptr;
	if ( (ptr=strrchr(fname, '.')) != NULL) {
		int	n;
		n = (ptr - fname);
		strncpy(new_fname, fname, n);
		new_fname[n] = 0;
	}
	else {
		strcpy(new_fname, fname);
	}
}

// change a filename's extension name
void StrUtil::changeExt(char *new_fname, char *fname, char *new_ext)
{
	char *ptr;
	if ( (ptr=strrchr(fname, '.')) != NULL) {
		int	n;
		n = (ptr - fname) + 1;
		strncpy(new_fname, fname, n);
		strcpy(new_fname+n, new_ext);
	}
	else {
		sprintf(new_fname, "%s.%s", fname, new_ext);
	}
}
// return 1 if 'str' end with 'end'
int StrUtil::strEndsWith(char *str, char *end,  int orglen, int sfix_len)
{
	if (orglen==0)	orglen = strlen(str);
	if (sfix_len==0) sfix_len = strlen(end);
	if (sfix_len > orglen) return 0;
	return ( (strcmp(str+(orglen-sfix_len), end)==0) );
}



int StrUtil::countChar(char *str, int ch)
{
	int	cnt=0;
	while(*str)
		if (*str++ == ch) cnt++;
	return cnt;
}

int StrUtil::countCharN(char *str, int ch, int len)
{
	int	cnt=0,n=0;
	while(*str && n++ < len)
		if (*str++ == ch) cnt++;
	return cnt;
}

int StrUtil::countAlphabet(char *str)
{
	int	cnt=0;
	while(*str)
		if (isalpha(*str++)) cnt++;
	return cnt;
}



// return 1 if it's a comment line; starts with '#'
// return 2 if it has only spaces (empty line)
// return 0 if it has any meaningful things
int StrUtil::isCmntLine(char *line)
{
	while(ISSPACE(*line)) line++;
	if (*line == 0) return 2;
	if (*line == '#') return 1;
	return 0;
}

// return the first non-space character in a line
int StrUtil::firstNonspace(char *line)
{
	while(ISSPACE(*line)) line++;
	return *line;
}

char *StrUtil::skipSpace(char *line)
{
	while(ISSPACE(*line)) line++;
	return line;
}

char *StrUtil::skipNonAlpha(char *line)
{
	while(*line && !isalpha(*line)) line++;
	return line;
}

char *StrUtil::skipNonAlnum(char *line)
{
	while(*line && !isalnum(*line)) line++;
	return line;
}

char *StrUtil::skipDigit(char *line)
{
	while(ISDIGIT(*line)) line++;
	return line;
}

byte *StrUtil::skipNonHangul(byte *line)
{
	while(*line && (*line & 0x80)==0 ) line++;
	return line;
}

char *StrUtil::skipNonspace(char *line)
{
	while(*line && ! ISSPACE(*line)) line++;
	return line;
}


char *StrUtil::skip2char(char *src, int ch)
{
	while (*src && *src!=ch) {
		src++;

	}
	return (src);
}

char *StrUtil::skip2charMax(char *src, int ch, int max_len)
{
	int	n=0;
	while (*src && *src!=ch && n < max_len) {
		src++;
		n++;
	}
	return (src);
}

/*------------------------*/
int StrUtil::hasDigit(char *str)
{
	while(*str)
		if (ISDIGIT(*str++)) return 1;
	return 0;
}

int StrUtil::isDigitStr(const char *s)
{
	while(*s)
	{
		if (! ISDIGIT(*s)) return 0;
		s++;
	}	
	return 1;
}


// title을 sql로 저장할 때 : ", ' 의미 없애기
int StrUtil::cleanForSqlQuery(char *trg, char *str, int maxlen)
{
	//char tmp[TITLE_LEN+20];
	//char *ptr=0, *trg=0;
	char *ptr;
	int n=0;
	int	cnt=0;
	ptr = str;

	while(*ptr && n <maxlen ) {
		//2002.11.10
		if (*ptr=='\'' || *ptr=='"') {
			*trg++ = '\\';
			n++;
			cnt++;
		}
		/*
		// 그냥 공백으로 치환 하는 방법
		if (*ptr=='\'' || *ptr=='"') {
			*trg++ = ' ';
			ptr++;
			cnt++;
		}
		*/

		*trg++ = *ptr++;
		n++;
	}
	*trg=0;

	return cnt;
}


/**
title을 sql로 저장할 때 : single quote  기호  없애기 : ' --> ''
single quete를 두개 연달아서 붙이면 된다.
search google: MSSQL escape quote

desc test;
insert into test values('aa''b', 10, 10);

@param str 원본 스트링
@param trg 복사 대상 스트링
@return 원본 또는 복사 스트링.  원본 스트링에 single quete가 없는 경우에는 원본을 그대로 return.
*/
char *StrUtil::mssqlEscapeQuote(char *trg, char *str, int maxlen)
{
	char *ptr;
	int n=0;
	char *start = 0;
	ptr = strchr(str, '\'');
	if (ptr==0) return str;

	start = trg;
	ptr = str;
	while(*ptr && n <maxlen ) {
		if (*ptr=='\'') {
			*trg++ = '\'';
			n++;
		}

		*trg++ = *ptr++;
		n++;
	}
	*trg=0;

	return start;
}

char * StrUtil::pathMerge(char *dest, char *home, char *fname)
{
	if (home) {
		sprintf(dest, "%s/%s", home, fname);
	}
	else {
		strcpy(dest, fname);
	}
	return dest;
}
