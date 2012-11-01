#ifndef STRUTIL_H
#define STRUTIL_H

/**
@brief 문자열 처리

각 멤버 변수가 static 이므로 인스턴스 생성 없이 바로 사용 가능하다.
*/


#include "basicDef.h"

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

class StrUtil {
	public:

	static int trimLeft(char *dest, int max, char *str);
	static int trimRight(char *str, int	n=0);
	static int trimText(char *target, char *source, int trg_len);
	static int trimFrom(char *str, int ch);
	static int trimChars(char *dest, int max, char *str, char *remove);
	static int trimBetween(char *dest, int max, char *str, char begin, char end);
	static int copyBetween(char *dest, int max, char *str, char begin, char end);
	static int copyRightWord(char *dest, int max, char *orgstr, int orglen);

	static int replaceStr(char *dest, int max, char *org, char *oldstr, char *newstr);
	static int replaceChars(char *str, char* orgchars, int newch);

	static int line2args(char **argv, int max_arr, int max_strlen, char *src);
	static int argv2line(char *line, int maxlen,  int argc, char **argv);
	static int line2words(char **argv, int max_arr, int max_strlen, char *src);
	static int dirList2array(char **array, int arr_size, char *dir_list_string);
	static int splitIdiom2words(char **argv, int max_arr, int max_strlen, char *src);

	static void invert(char *inverted, char *org, int org_len=0);
	static void invertHangul(char *inverted, char *org, int org_len=0);

	static int copyReplaceChar(char *trg, char *src, int org, int newch);
	static int copyUpperOnly(char *trg, char *src);
	static int copyUpperOnlyMax(char *trg, char *src, int max);
	static int copyAlphabet(char *trg, char *src, int max_len);
	static int copyNonspStop(char *trg, char *src, int max_len, int stop);
	static int copyNonspace(char *trg, char *src, int max_len);
	static int copyNumber(char *trg, const char *src, int max_len);
	static int copy2char(char *trg, char *src, int max_len, int ch);
	static void copy2lower(char *trg, char *src);
	static int copyVarName(char *trg, char *src, int max_len);


	static int hasOddChar(char *w);
	static int isAllUpper(char *s);
	static char *myStrchrn(char *str, int ch, int len);
	static char *myStrrchrn(char *str, int ch, int len);
	static char *myStrrchrTailn(char *str, int ch, int len);
	static void copyWithoutExt(char *new_fname, char *fname);
	static void changeExt(char *new_fname, char *fname, char *new_ext);
	static int strEndsWith(char *str, char *end, int orglen=0, int sfix_len=0);
	static int replaceChar(char *str, int org, int newch);
	static int countChar(char *str, int ch);
	static int countCharN(char *str, int ch, int len);
	static int countAlphabet(char *str);
	static int isCmntLine(char *line);
	static int firstNonspace(char *line);
	static char *skipSpace(char *line);
	static char *skipNonAlpha(char *line);
	static char *skipNonAlnum(char *line);
	static char *skipDigit(char *line);
	static byte *skipNonHangul(byte *line);
	static char *skipNonspace(char *line);
	static char *skip2char(char *src, int ch);
	static char *skip2charMax(char *src, int ch, int max_len);

	static int isDigitStr(const char *s);
	static int hasDigit(char *str);
	static int cleanForSqlQuery(char *trg, char *str, int maxlen);
	static char *mssqlEscapeQuote(char *trg, char *str, int maxlen);
	static	char * pathMerge(char *dest, char *home, char *fname);
};
#endif
