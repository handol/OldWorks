/*
for Anydict.com  dahee@netsgo.com
2002.4.2 ~
*/
#include <stdio.h>
#include <string.h> // strstr()
#include <ctype.h>
#include <stdlib.h> // strtol()
#include "htmlLex.h"
#include "htmlTagEntity.h"

// space and tab: trim out many spaces
// return ' ' or '\t'
int htmlLex::get_sptab_tok(stToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, max=0;
	int	is_tab=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str + tok->tok_len;
	max = MAX_STR - tok->tok_len;
	while (IS_SPTAB(*srcptr) && n < max) {
		if (*srcptr == '\t') is_tab = 1;
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->offset += n;
	tok->tok_len += n;
	
	// 2002.6.26
	return ' ';
	if (is_tab || n > 2) return '\t';
	else	return ' ';
}

int htmlLex::get_newline_tok(stToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, max=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str + tok->tok_len;
	max = MAX_STR - tok->tok_len;
	while ((*srcptr == '\n' || *srcptr == '\r')  && n < max) {
		if (*srcptr=='\n') tok->line_num++;
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->offset += n;
	tok->tok_len += n;
	return 	n;
}

int htmlLex::get_string_tok(stToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, max=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str + tok->tok_len;
	max = MAX_STR - tok->tok_len;
	while (isalpha(*srcptr) && n < max) {
		*tokstr++ = *srcptr++;
		n++;
	}
	if ( isdigit(*srcptr) ) {
	// <H1>, <H2>, ...
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->offset += n;
	tok->tok_len += n;
	return n;
}

int htmlLex::get_number_tok(stToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, max=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str + tok->tok_len;
	max = MAX_STR - tok->tok_len;
	while (isdigit(*srcptr) && n < max) {
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->offset += n;
	tok->tok_len += n;
	return n;
}


int htmlLex::get_big_ascii_tok(stToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, max=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str + tok->tok_len;
	max = MAX_STR - tok->tok_len;
	while (IS_BIG_ASCII(*srcptr) && n < max) {
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->offset += n;
	tok->tok_len += n;
	return n;
}

int htmlLex::skip_to_cmnt_end(stToken *tok)
{
	char *srcptr;
	//char *tokstr;
	int	n=0;
	char *found, *ptr;

	srcptr	= tok->src_mem + tok->offset;
	//tokstr	= tok->tok_str + tok->tok_len;

	if ( (found = strstr(srcptr, "-->")) ) {
		for(ptr=srcptr; ptr<found; ptr++)
			if (*ptr=='\n') tok->line_num++;
			
		n = (int) found - (int) srcptr;
		tok->offset += (n+3);
		tok->tok_len += (n+3);
		return (n+3);
	}
	else {
		tok->offset++;
		return 1;
	}
}

int htmlLex::skip_to_quote_end(stToken *tok)
{
	char *srcptr;
	//char *tokstr;
	int	n=0;
	int	ch;
	srcptr	= tok->src_mem + tok->offset;
	//tokstr	= tok->tok_str + tok->tok_len;
	
	while ( (ch=*srcptr) ) {
		//if (n < MAX_STR) tokstr[n] = ch;
		if (ch=='\n') tok->line_num++;
		n++;
		srcptr++;

		if (ch == '\"' ) {
			if (n==0) break;
			else if ( *(srcptr-1) != '\\') break;
		}
	}
	//if (n< MAX_STR) tokstr[n] = 0;
	//else tokstr[MAX_STR] = 0;
	tok->offset += n;
	tok->tok_len += n;
	return n;
}

int htmlLex::skip_to_tag_end(stToken *tok)
{
	char *srcptr;
	//char *tokstr;
	int	n=0;
	int	ch;
	int	res;
	srcptr	= (char *)tok->src_mem + tok->offset;
	//tokstr	= tok->tok_str + tok->tok_len;
	
	while ( (ch=*srcptr) ) {
		//if (n < MAX_STR) tokstr[n] = ch;
		if (ch=='\n') tok->line_num++;
		n++;
		srcptr++;
		tok->offset++;
		
		if (ch == '\"' ) {
			res = skip_to_quote_end(tok);			
			n += res;
			srcptr += res;
			//tokstr += res;
		}
		else if (ch == '>' ) {
			if (n==0) break;
			else if ( *(srcptr-1) != '\\') break;
		}
		
	}
	//if (n< MAX_STR) tokstr[n] = 0;
	//else tokstr[MAX_STR] = 0;
	tok->tok_str[0] = 0;
	tok->tok_len += n;
	return n;
}

// for anydict, we read only memory-based HTML contents
int htmlLex::get_token_mem(stToken *tok)
{
	char *srcptr, *tmpptr;
	int	n=0;
	int	res;
	int	ch, ch2;
	int	tok_val=0;
	int	tmp;

	tok->tok_val = tok->tok_realval = tok->tok_len = 0;
	memset(tok->tok_str, 0, MAX_STR);
	
	tmp = tok->offset;
	srcptr = (char *)tok->src_mem + tok->offset;
	ch = *(unsigned char *)srcptr;
	if (ch == 0) return TOK_EOF;

	// 2002.6.24
	// ### URL
	if (strncmp(srcptr,  "###", 3)==0) {
		tmpptr = strchr(srcptr, '\n');
		if (tmpptr==0)  {
			n = 1;
			tok_val = '#';
		}
		else {
			n = (tmpptr - srcptr) + 1;
			tok_val = URL_CMNT;
		}
		
	}
	//2003.4.2
	// 0x92, 0x97등의 이상 문자 제외 
	else if ( (ch & 0xF0) == 0x90) {
		//printf("%3d,%4d: %3d %04X %c\n", tok->line_num, tok->offset, ch, ch, ch);
		tok->tok_str[0] = 0;
		tok->offset++;
		ch2 = ((unsigned char *)srcptr)[1];
		if (ch==0x92) {
			if (ch2==0x92) tok->offset++;
			tok->tok_len = 1;
			tok->tok_val = '\'';
			return tok->tok_val;
		}
		else {			
			if ((ch2 & 0xF0) == 0x90)
				tok->offset++;
			return get_token_mem(tok);
		}
	}
	else if (isalpha(ch)) {
		tok->tok_len = get_string_tok(tok);
		tok_val = STRING;
	}
	else if (IS_BIG_ASCII(ch)) {
		tok->tok_len = get_big_ascii_tok(tok);
		tok_val = FR_STR;
	}
	else if (isdigit(ch)) {
		tok->tok_len = get_number_tok(tok);
		tok_val = NUMBER;
	}
	else if (ch == ' ' || ch == '\t') {
		tok_val = get_sptab_tok(tok);
		tok->tok_len = 1;
	}
	else if (ch == '\r') {
		if (srcptr[1] == '\n') {
			tok->line_num++;
			n = 1;
			tok_val = '\n';
		}
		else {
			n = 1;
			tok_val = '\r';
		}
	}
	else if (ch == '\n') {
		tok->line_num++;
		n = 1;
		tok_val = '\n';
	}
	else if (ch == '<') {
		if (srcptr[1]=='/') {
			// '</'
			n = 2;
			tok_val = ETAG_BEGIN;
		}
		else if (srcptr[1]=='!') {
			if (srcptr[2]=='-' && srcptr[3]=='-') {
				// '<!--'
				n = 4;
				tok_val = CMNT_BEGIN;
			}
			else {
				// '<!'
				n = 2;
				tok_val = TAG_EXC;
			}
		}
		else { 
			// '<'
			n = 1;
			tok_val = BTAG_BEGIN;
		}
	}
	else if (ch == '>') {
		n = 1;
		tok_val = TAG_END;
	}
	else if (ch == '/') {
		if (srcptr[1]=='>') {
			n = 2;
			tok_val = STAG_END;
		}
		else {
			n = 1;
			tok_val = ch;
		}
	}
	/*
	else if (ch == '-') {
		if (srcptr[1]=='-' && srcptr[2]=='>') {
			// '-->'
			n = 3;
			tok_val = CMNT_END;
		}
		else {
			n = 1;
			tok_val = ch;
		}
	}
	*/
	else if (ch == '&') {
		if (srcptr[1] == '#') {
			if (srcptr[2] == 'x') {
				// &#xnumber : hexadecimal 
				tok->offset += 3;
				res = get_number_tok(tok);
				tok->tok_len = res+3;
				tok->tok_realval = strtol(tok->tok_str, NULL, 16);
				tok_val = ENTITY_NUM;
				if (tok->src_mem[tok->offset] == ';')
					tok->offset++; // read out ';'
			}
			else {
				// &#number : decimal
				tok->offset += 2;
				res = get_number_tok(tok);
				tok->tok_len = res+2;
				tok->tok_realval = strtol(tok->tok_str, NULL, 10);
				tok_val = ENTITY_NUM;
				if (tok->src_mem[tok->offset] == ';')
					tok->offset++; // read out ';'
			}
		}
		
		else if (isalpha(srcptr[1])) {
			tmp = tok->offset;
			tok->offset += 1;
			res = get_string_tok(tok);
			tok->tok_len = res+1;
			tok_val = ENTITY_STR;
			if (tok->src_mem[tok->offset] == ';')
				tok->offset++; // read out ';'		
		}
		else {
			tok_val = ch;
			n = 1;
		}
	}
	// default chars
	else {
		tok_val = ch;
		n = 1;
	}

		
	if (n>0) {
		strncpy(tok->tok_str, srcptr, n);
		tok->tok_str[n] = 0;
		tok->offset += n;
		tok->tok_len = n;
	}

	tok->tok_val = tok_val;
	//printf("tok_val=%d, n=%d\n", tok_val, n);
/*
	if (tok->tok_val != tok_val ||  tok->tok_len != n)
	fprintf(out, "tok_val=%d, val=%d, tok_len=%d, n=%d\n", 
			tok->tok_val, tok_val, tok->tok_len, n);
	
	// prn_tok(tok);
*/
	//if (tok_val==0) prn_tok(tok);

	/* -- 2002.5.20
	if (tmp==tok->offset || tok_val==0 || tok->tok_val==0) {
		printf("!! %c%c%c%c !!\n", *(srcptr-1),srcptr[0],srcptr[1],srcptr[2]);
		return TOK_EOF;
	}
	*/
	return tok_val;
}
