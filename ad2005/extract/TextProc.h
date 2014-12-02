//---------------

#ifndef TEXT_PROC_H
#define TEXT_PROC_H

#include "TextLex.h"
#include "oneDoc.h"

#include "DbQuery.h"
#include "Docs.h"

#include "StrStream.h"

typedef struct _text_info {
	int	size;
	int	no_lines;
	int	no_paragraphs;
	int	no_sentences;
	int	no_simp_sentnc;
	int	no_cmpnd_sentnc;
	int	no_cmplx_sentnc;
	
} TEXTinfo;

typedef struct _article_stat
{
	char	*article;
	int		n_bytes;
	int		n_lines;
	int		n_sentences;
	int		n_complex_stc; // complex sentences
	int		n_paragraphs;
	
} Article_stat;

//------------------
typedef struct _stc_info
{
	int	s_line;
	int	e_line;
	uint2	no_exams;
	uint2	level;
	uint2	no_toks;
	uint2	no_odd_toks;	
	uint2	n_words;
	uint2	n_no_dict_words;
	uint2	n_name_words; //고유명사 
	uint2	n_verb_conj;
	uint2	n_up_words;
	uint2	n_low_words;
	uint2	n_head_words;
	uint2	n_dict_words;
	uint2	n_commons;
	uint2	n_up_dict_words;
	uint2	n_head_dict_words;
	uint2	n_odd_dict_words;
	uint2	n_hangul_words;
	uint2	acronym_search_flag;
	short	parath;
	uint2	n_words_in_parath;
} stcInfo;

typedef struct _pair_mark {
	int	nums;
	int	b_line;
	int	e_line;
	int	w_cnt;
} pairMARK;


#define	PROB_1_2(X) ( (X)&1 )
#define	PROB_1_4(X) ( ((X)&3)==1 )
#define	PROB_3_4(X) ( ((X)&3)!=1) // probability of 3/4
#define	PROB_1_8(X) ( ((X)&7)==1 )
#define	PROB_1_16(X) ( ((X)&15)==1 )
//------

#define MIN_HANGUL_CHAR	(15)

class	TextProc {

public:
	TextProc() {
		hangul_words = 0;
		debug = 0;
	}
	
	static	void prepare();
	
	int txt_analyze(docType *docinfo);
	void	get_result(StrStream &resStr);
	void	get_result(oneDoc *doc);
	void	setDebug(int _debug=0) {
		debug = _debug;
	}
	
private:
	static	wordType *etc_word;
	static	wordType *vs_word;
	static	wordType *not_word;
	static	wordType *the_word;
	static	wordType *a_word;
	static	wordType *an_word;
	static	wordType *to_word;
	static	wordType *of_word;
	static	wordType *by_word;
	static	wordType *copyright_word;
	static	wordType *all_word;
	static	wordType *rights_word;
	static	wordType *reserved_word;
	static	wordType *reserve_word;

	TextLex	Lexer;
	int	debug;
	txtToken *STCtoks[TOK_HIST_SIZE];
	int	no_STCtoks;

	stcInfo stc;

	int hangul_words;

	int ratio_in_page;
	oneDoc	thisDoc;
	
	void txtToken_print2(txtToken *tok);
	void txtToken_print(txtToken *tok);

	void stcInfo_print(stcInfo *stc, const char *msg=0);

	void STCtoks_init();
	int STCtoks_push(txtToken *tok);
	txtToken *STCtoks_top();
	txtToken *STCtoks_pop();
	int printSentenceFromTokens();
	void STCtoks_print();

	int check_copyright_stop(txtToken *currTok);
	
	int init_sentence_proc(char *msg);
	void	finish_examples(docType *docinfo);
	
	int get_examples();
	
	int	get_trailing_hangul(char *start);
	int calc_stc_level(stcInfo *stc);
	void printExamWord(char *begin, int len);
	int probeExample(wordType *w);
	void printWordExamInfo(wordType *word, exType *ex);

	int	wordList_for_idiom(wordType **wpArr, int start_pos, int max_words);
	int get_example_from_stc(stcInfo *stc, oneDoc *Doc, int hangul_words);
	int handleHangulToken(oneDoc *Doc, stcInfo *stc, txtToken *tok);
};



#endif
