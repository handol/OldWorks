#ifndef MA_H
#define MA_H

enum{ dummy1, CHAON, YONGON, BUSA, KWANHYUNGSA, GAMTANSA, VERB_INCOMP };
enum
{
	dummy, CHAON_JOSA, EOGAN_PEOMI_EOMI, EOGAN_EOMI,
	CHAON_YI_PEOMI_EOMI, CHAON_YI_EOMI,
	S_CHAON, S_BUSA, S_KWANHYUNGSA, S_GAMTANSA
};
#define MAX_CHAON 20
#define MAX_JOSA 20
#define MAX_YONGON 20
#define MAX_PEOMI 10
#define MAX_EOMI 10
#define MAX_WORD 40
typedef struct s_chaon
{
	unsigned char chaon[MAX_CHAON];
	unsigned char josa[MAX_JOSA];
} STR_CHAON;
typedef struct s_yongon
{
	unsigned char eogan[MAX_YONGON];
	unsigned char peomi[MAX_PEOMI];
	unsigned char eomi[MAX_EOMI];
} STR_YONGON;
// 추후에 union 을 설정할지 결정 현재는 그냥 독립적으로
typedef struct s_list_result_ma
{
	int type;
	STR_CHAON chaon;
	STR_YONGON yongon;
	unsigned char single[MAX_WORD];
	struct s_list_result_ma *prev;
	struct s_list_result_ma *next;
} L_RESULT_MA;
typedef struct s_result_ma
{
	int no_of_result;
	L_RESULT_MA *p_list_result;
} STR_RESULT_MA;

// dahee
int b_search (int kind, FILE * fp, long start, long end, unsigned char *str, int len);

int is_belong_to_chaon_dic (unsigned char *str);

void token_to_ma (unsigned char *str, int nth, int position);

void guess_token_to_ma (unsigned char *str, int nth, int position);

void save_result_single_word (int nth_word, unsigned char *str, int type);

void print_result (int no_of_word);

void print_result_chaon (unsigned char *str_first, unsigned char *str_last) ;

void print_result_yongon (unsigned char *str_first, unsigned char *str_middle, unsigned char *str_last);



#endif
