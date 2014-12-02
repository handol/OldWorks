/*
for Anydict.com  dahee@netsgo.com
2002.4.2 ~
*/

#ifndef TEXT_LEX_H
#define	TEXT_LEX_H

#include "wordType.h"
#include "HDict.h"

#define MAX_TOKSTR	(40)

typedef struct _txttoken {
	char	*src_mem;
	int		line_num;
	int		src_len;

	char	*curptr;
	int		offset; // offset in src_mem
	uint4	conj_PS; // 2002.10.23;
	uint2		tok_val; // token value (kind)
	uint2		tok_realval; // token real value (number value, HTML tag id)
	uint2		tok_len;
	char	tok_str[MAX_TOKSTR+1];
	wordType	*w;
	wordType	*orgw;
	HwordType *hw;
} txtToken;

/*
	LOW_W, // all lower
	UP_W, // all upper
	HEAD_W, // start with upper char
	APO_W, // word with apostrophe
	DASH_W, // acronym
	DOT_W, // acronym
*/

/**
토큰이 WORDS 일때 그종류를 표하기 위해. 하위 바이트에 표시.
대문자로 시작 하는 단어.
소문자로 시작 하는 단어.
' 가 포함된 단어.
- 가 포함된 단어.
. 가 포함된 단어.
*/
#define LOW_W (0x01)
#define HEAD_W (0x02)
#define UP_W (0x04)
#define APO_W (0x08)
#define DASH_W (0x10)
#define DOT_W (0x20)
#define WEBSITE_W (0x800)

/** 토큰 값은 상위 바이트부터.
*/
typedef enum _txttok_val {
	SPACES=1000,
	TABS,
	NEWLINES,
	WORDS,

	NUMBER,
	HAN_STR,
	FR_STR,

	MID_PUNCT,
	END_PUNCT, // end of sentence
	D_QUOTE,
	S_QUOTE,
	LEFT_M,
	RIGHT_M,
	LEFT_BIG_M,
	RIGHT_BIG_M,
	PARATH,
	BAR,
	LINE_BREAK,
	URL_CMNT,
	COPYRIGHT_MARK,
	NUMBERING,
	TOK_EOF
} enTxtTokval;

#define GEN_NUM	(0)
#define DOT_NUM	(1)
#define COMMA_NUM	(2)
#define DASH_NUM	(3)


#define	IS_SPTAB(x)	((x)==' ' || (x)=='\t')
#define IS_BIG_ASCII(C)    ((C) & 0x80)  // ASCII code > 127


#define	TOK_HIST_SIZE	(1024*2) // token history size

class TextLex{

private:
	txtToken	tokHist[TOK_HIST_SIZE];
	int			n_tok_hist;
	int			real_tok_top; // 현재 top, 방금 push된 것 // pointer to last inserted element
	int	last_tok_top;

public:
	TextLex() {
		n_tok_hist=0;
		real_tok_top=-1;
		last_tok_top=-1;
	}

	txtToken *push_tok(txtToken *tok);
	txtToken *pop_tok();
	txtToken *top_tok();
	txtToken *get_tok(int n_back);
	txtToken *search_tok(int tok_val);
	int get_last_tok_pos();

	void	init_token(txtToken *tok, char *textbuff, int textleng);
	txtToken *txt_get_token_mem(txtToken *tok);
	int txt_eat_up_ahead_token(txtToken *tok);
	txtToken *txt_get_token_mem_ahead(int ahead_n);

	static	int sent_ending_mark_ksc(byte c, byte d);

private:
	int txt_get_token_mem_actual(txtToken *tok);
	int txt_get_space_tok(txtToken *tok);
	int txt_get_tab_tok(txtToken *tok);
	int txt_get_newline_tok(txtToken *tok);
	int txt_get_string_tok(txtToken *tok);
	int txt_get_number_tok(txtToken *tok);
	int txt_get_big_ascii_tok(txtToken *tok);
	int txt_get_one_line_tok(txtToken *tok);

	int skip_to_newline(txtToken *tok);

};

#endif
