#ifndef STRUTIL_H
#define STRUTIL_H	

#include "basicDef.h"

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

class StrUtil {
	public:

	static	int trim_left_right(char *dest, int max, char *str);
	/* 2005.7.9 추가 */
	static int trim(char *dest, int max, char *str);
	static int trim_right(char *str, int	n=0);
	static int trim_text(char *target, char *source, int trg_len);
	static int trim_from(char *str, int ch);
	static int trim_chars(char *dest, int max, char *str, char *remove);
	static int trim_between(char *dest, int max, char *str, char begin, char end);
	static int copy_between(char *dest, int max, char *str, char begin, char end);
	static int copy_rightword(char *dest, int max, char *orgstr, int orglen);
	
	static int replaceStr(char *dest, int max, char *org, char *oldstr, char *newstr);
	static int replaceChars(char *str, char* orgchars, int newch);
	
	static int line_to_args(char **argv, int max_arr, int max_strlen, char *src);
	static int argv_to_line(char *line, int maxlen,  int argc, char **argv);
	static int line_to_words(char **argv, int max_arr, int max_strlen, char *src);
	static int dir_list_2_array(char **array, int arr_size, char *dir_list_string);
	static int split_idiom_to_words(char **argv, int max_arr, int max_strlen, char *src);

	static void invert(char *inverted, char *org, int org_len=0);
	static void invertHangul(char *inverted, char *org, int org_len=0);
		
	static int copy_replace_char(char *trg, char *src, int org, int newch);
	static int copy_upper_only(char *trg, char *src);
	static int copy_upper_only_max(char *trg, char *src, int max);
	static int copy_alphabet(char *trg, char *src, int max_len);
	static int copy_nonsp_stop(char *trg, char *src, int max_len, int stop);
	static int copy_nonspace(char *trg, char *src, int max_len);
	static int copy_number(char *trg, char *src, int max_len);
	static int copy_to_char(char *trg, char *src, int max_len, int ch);
	static void copy_to_lower(char *trg, char *src, int trglen=0);
	static int copy_var_name(char *trg, char *src, int max_len);

	
	static int has_odd_char(char *w);
	static int is_all_upper(char *s);
	static char *my_strchr_n(char *str, int ch, int len);
	static char *my_strrchr_n(char *str, int ch, int len);
	static char *my_strrchr_tail_n(char *str, int ch, int len);
	static void copy_without_ext(char *new_fname, char *fname);
	static void change_ext(char *new_fname, char *fname, char *new_ext);
	static int str_ends_with(char *str, char *end, int orglen=0, int sfix_len=0);
	static int replaceChar(char *str, int org, int newch);
	static int countChar(char *str, int ch);
	static int countCharN(char *str, int ch, int len);
	static int countAlphabet(char *str);
	static int is_cmnt_line(char *line);
	static int first_nonspace(char *line);
	static char *skip_space(char *line);
	static char *skip_non_alpha(char *line);
	static char *skip_non_alnum(char *line);
	static char *skip_digit(char *line);
	static byte *skip_non_hangul(byte *line);
	static char *skip_nonspace(char *line);
	static char *skip_to_char(char *src, int ch);
	static char *skip_to_char_max(char *src, int ch, int max_len);

	static int is_digit_str(char *s);
	static int has_digit(char *str);
	static int clean_for_sql_query(char *trg, char *str, int maxlen);
	static char *mssql_escape_quote(char *trg, char *str, int maxlen);
	static	char * path_merge(char *dest, char *home, char *fname);
};
#endif
