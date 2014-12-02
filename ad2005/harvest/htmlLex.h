/*
for Anydict.com  dahee@netsgo.com
2002.4.2 ~
*/
#ifndef HTML_LEX_H
#define HTML_LEX_H
#include "basicDef.h"

#define MAX_STR	128

typedef struct _token {
	FILE	*src_file;
	char	*src_mem;
	uint4		line_num;
	uint4		src_len;
	uint4		offset; // offset in src_mem
	int		tok_val; // token value (kind)
	int		tok_realval; // token real value (number value, HTML tag id)
	int		tok_len;
	char	tok_str[MAX_STR+10];
} stToken;

typedef enum _tok_val {
	BTAG_BEGIN=256, // <   begining_tag
	TAG_END,		// >
	ETAG_BEGIN,		// </  ending_tag
	STAG_END,		// />, sole_tag
	TAG_EXC,		// <!   (260)
	CMNT_BEGIN,		// <!--
	CMNT_END,		// -->
	STRING, // (263)
	NUMBER,
	FR_STR, // foreign string (Non-English) (265)
	ENTITY_STR,
	ENTITY_NUM,
	URL_CMNT,
	TOK_EOF
} enTokval;


#define	IS_SPTAB(x)	((x)==' ' || (x)=='\t')
#define IS_BIG_ASCII(C)    ((C) & 0x80)  // ASCII code > 127

class htmlLex {
	public:
	
	static	int get_sptab_tok(stToken *tok );
	static	int get_newline_tok(stToken *tok );
	static	int get_string_tok(stToken *tok );
	static	int get_number_tok(stToken *tok );
	static	int get_big_ascii_tok(stToken *tok );
	static	int skip_to_cmnt_end(stToken *tok );
	static	int skip_to_quote_end(stToken *tok );
	static	int skip_to_tag_end(stToken *tok );
	static	int get_token_mem(stToken *tok );
};

#endif
