/**
for Anydict.com  dahee@netsgo.com
2002.8.7 ~

for SisaYBM.com handol@gmail.com
2005.3 ~
*/

#include <stdio.h>
#include <string.h> // strstr()
#include <ctype.h>
#include <stdlib.h> // strtol()

#include "basicDef.h"
#include "wordType.h"
#include "TextLex.h"

#include "HanChar.h"

#define PRN	printf

/*	
참조 : HAM 소스 --  readsent.c  -- sent_ending_mark_ksc()
KSC 5601 sentence ending marks.
EUC-KR 기준.
*/
int TextLex::sent_ending_mark_ksc(byte c, byte d)
{
	return ((c == 0xA3 && d == 0xAE) ||	/* '.' */		
		(c == 0xA3 && d == 0xBF) ||	/* '?' */		
		(c == 0xA3 && d == 0xA1) ||	/* '!' */		
		(c == 0xA1 && d == 0xA3) ||	/* circle '.' */		
		(c == 0xA1 && d == 0xA6) ||	/* '...' */		
		(c == 0xA1 && d == 0xBB));	/* right bracket */
}


/**
실제로 토큰을 구하는 함수.

토큰 단위의 제일 앞 문자를 분석하여  숫자, 영어, 한글 (FR_STR) 등으로 구분한다.
*/
int TextLex::txt_get_token_mem_actual(txtToken *tok)
{
	char *srcptr=0;
	register byte	ch=0;
	byte ch2=0;
	int	tok_val=0;
	int	tmp=0;

	/*
	if (tok_put_back) {
		memcpy(tok, &put_back, sizeof(txtToken));
		tok_put_back = 0;
		return tok->tok_len;
	}
	*/

	tok->tok_val = tok->tok_realval = tok->tok_len = 0;
	memset(tok->tok_str, 0, MAX_TOKSTR+1);

	tmp = tok->offset;
	srcptr = (char *)tok->src_mem + tok->offset;
	byte *byteptr = (byte *)srcptr;
	ch = byteptr[0];
	ch2 = byteptr[1];
	if (ch == 0) return TOK_EOF;

	if (strncmp(srcptr,  "##", 2)==0) {
		txt_get_one_line_tok(tok);
		tok_val = URL_CMNT;

	}
	else if (strncmp(srcptr,  "#---", 4)==0) {
		txt_get_one_line_tok(tok);
		tok_val = LINE_BREAK;
	}
	else if (IS_BIG_ASCII(ch) && IS_BIG_ASCII(ch2) ) {
		if (sent_ending_mark_ksc(ch, ch2))
		{
			printf("Hangul Punc: \n");
			tok->tok_realval = 0;
			tok->tok_len = 2;
			tok->curptr += 2;
			tok->offset += 2;
			tok_val = END_PUNCT;
			tok->tok_val = END_PUNCT;
		}
		else
		{
			printf("Hangul : \n");
			tok->tok_realval = 0;
			tok->tok_len = txt_get_big_ascii_tok(tok);
			// txt_get_big_ascii_tok() 내부에서 토큰 값을 정한 경우도 있다.
			// 공백 문자 등
			if (HanChar::isHangulWord(tok->tok_str, tok->tok_len))
			{
				tok_val = HAN_STR;
				tok->tok_val = HAN_STR;
			}
			else
			{
				tok_val = FR_STR;
				tok->tok_val = FR_STR;
			}
				
		}
	}
	else if (isalpha(ch)) {

		tok->tok_realval = txt_get_string_tok(tok);
		tok_val = WORDS;
	}

	else if (isdigit(ch)) {
		tok->tok_len = txt_get_number_tok(tok);
		tok_val = NUMBER;
	}
	else if (ch == ' ') {
		tok->tok_len = txt_get_space_tok(tok);
		tok_val = SPACES;
	}
	else if (ch == '\t') {
		tok->tok_len = txt_get_tab_tok(tok);
		tok_val = TABS;
	}
	else if (ch=='\r') {
		if (srcptr[1]=='\n') {
			tok->line_num++;
			tok->curptr += 2;
			tok->offset += 2;
			tok->tok_len = 1;
			tok->tok_realval = ch;
			tok_val = NEWLINES;

		}
		else {
			tok->curptr ++;
			tok->offset ++;
			tok->tok_len = 1;
			tok->tok_realval = ch;
		}
		tok_val = ch;
	}
	else if (ch=='\n') {
		tok->line_num++;
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = NEWLINES;
	}
	else if (ch=='.' || ch=='?' || ch=='!') {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = END_PUNCT;
	}
	else if (ch==',' || ch==';' || ch==':' ) {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = MID_PUNCT;
	}
	else if (ch=='(') {
		if (srcptr[2]==')') {
			tok->curptr +=3;
			tok->offset +=3;
			tok->tok_len = 3;
			strncpy(tok->tok_str, srcptr, 3);
			if (toupper((int)(srcptr[1]))=='C') tok_val = COPYRIGHT_MARK;
			else tok_val = NUMBERING;
		}
		else {
			tok->curptr ++;
			tok->offset ++;
			tok->tok_len = 1;
			tok->tok_realval = ch;
		}
	}
	else if (ch==')') {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = PARATH;
	}
	else if (ch=='-' && srcptr[1]=='-' && srcptr[2]!='-') { // '--'
		tok->curptr += 2;
		tok->offset += 2;
		tok->tok_len = 2;
		tok->tok_realval = ch;
		tok_val = MID_PUNCT;
	}
	else if (ch=='"') {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = D_QUOTE;
	}
	else if (ch=='\'') {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = S_QUOTE;
	}
	else if (ch=='[') {
		if (srcptr[1]=='[') {
			tok->tok_len = 2;
			tok_val = LEFT_BIG_M;
		}
		else if (srcptr[1]==' ' && srcptr[2]=='[') {
			tok->tok_len = 3;
			tok_val = LEFT_BIG_M;
		}
		else {
			tok->tok_len = 1;
			tok_val = LEFT_M;
		}
		tok->curptr += tok->tok_len;
		tok->offset += tok->tok_len;
	}
	else if (ch==']') {
		if (srcptr[1]==']') {
			tok->tok_len = 2;
			tok_val = RIGHT_BIG_M;
		}
		else if (srcptr[1]==' ' && srcptr[2]==']') {
			tok->tok_len = 3;
			tok_val = RIGHT_BIG_M;
		}
		else {
			tok->tok_len = 1;
			tok_val = RIGHT_M;
		}
		tok->curptr += tok->tok_len;
		tok->offset += tok->tok_len;
	}
	else if (ch=='|') {
		tok->curptr ++;
		tok->offset ++;
		tok->tok_len = 1;
		tok->tok_realval = ch;
		tok_val = BAR;
	}
	else {
		tok->tok_realval = ch;
		tok_val = 0;
		if (srcptr[1]==ch) {

			while(srcptr[1]==ch) {
				// default chars -- 연속
				tok->curptr ++;
				tok->offset ++;
				tok->tok_len++;
				srcptr++;
			}
		}
		else {	// default chars
			tok->curptr ++;
			tok->offset ++;
			tok->tok_len = 1;
		}
	}

	tok->tok_val = tok_val;
	return tok_val;
}

// space
// return ' '
int TextLex::txt_get_space_tok(txtToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0;
	//srcptr	= tok->curptr;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while (*srcptr==' ') {
		//*tokstr++ = *srcptr++;
		srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len = n;
	return n;
}

int TextLex::txt_get_tab_tok(txtToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0;
	//srcptr	= tok->curptr;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while (*srcptr=='\t') {
		//*tokstr++ = *srcptr++;
		srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len = n;
	return n;
}

int TextLex::txt_get_newline_tok(txtToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0, lines=0;
	//srcptr	= tok->curptr;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while (*srcptr=='\n' || *srcptr=='\r') {
		if (*srcptr=='\n') {
			tok->line_num++;
			//*tokstr++ = *srcptr;
			lines++;
		}
		srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len = lines;
	tok->tok_realval = lines;
	return lines;
}


int TextLex::txt_get_string_tok(txtToken *tok)
{
	register char *srcptr;
	char *tokstr;
	int last_ch;
	int	is_all_upper=1;
	//int	is_all_lower=1;
	int	is_head_upper=0;
	int	is_with_dash=0;
	int	is_with_dot=0; // maybe the word is Abbreviation (Acronym)
	int	is_with_apostrophe=0;
	int	n=0;
	int	tok_val=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;

	last_ch = *srcptr;
	if (isupper(*srcptr)) is_head_upper = 1;

	while (n < MAX_TOKSTR && (isalpha(*srcptr) || *srcptr=='-' || *srcptr=='\''))
	{
		last_ch = *srcptr++;
		if (islower(last_ch)) is_all_upper = 0;
		//else if (isupper(*srcptr)) is_all_lower = 0;
		else if (last_ch=='-') { //합성어
			if (*srcptr=='-') { // '--' 처리
				srcptr--;
				*tokstr = 0;
				last_ch = *(srcptr-2);
				break;
			}
			is_with_dash++;
		}
		else if (last_ch=='\'') is_with_apostrophe++;

		*tokstr++ = last_ch;
		n++;
	}

	if (last_ch=='-' && n > 1) {
		is_with_dash--;
		tokstr--;
		srcptr--;
		n--;
	}
	else if (last_ch=='\'' && n > 1) {
		is_with_apostrophe--;
		tokstr--;
		srcptr--;
		n--;
	}
	else if (n==1 && *srcptr=='.') {
	// 약어 : "A.P."
		*tokstr++ = *srcptr++;
		n++;
		while(n < MAX_TOKSTR-2 && isalpha(srcptr[0]) && srcptr[1]=='.') {
			*tokstr++ = *srcptr++;
			*tokstr++ = *srcptr++;
			n += 2;
		}
		is_with_dot = 1;
	}
	else if (*srcptr=='.' && isalpha(srcptr[1])) {
	// URL host name: web site
		*tokstr++ = *srcptr++;
		*tokstr++ = *srcptr++;
		n += 2;
		while(isalnum(*srcptr) || *srcptr=='.' || *srcptr=='-') {
			*tokstr++ = *srcptr++;
			n++;
		}
		if (*(srcptr-1)=='.') {
			n--;
			tokstr--;
			srcptr--;
		}
		*tokstr = 0;
		tok_val = WEBSITE_W;
		goto RETURN_WORD;

	}

	*tokstr = 0;

	tok_val = LOW_W;
	if (is_all_upper) tok_val |= UP_W;
	if (is_head_upper) tok_val |= HEAD_W;
	if (is_with_dot) tok_val |= DOT_W;
	if (is_with_dash) tok_val |= DASH_W;
	if (is_with_apostrophe) tok_val |= APO_W;


	tok->tok_val = WORDS;

RETURN_WORD:
	tok->tok_realval = tok_val;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len += n;
	return tok_val;
}

// comma, dot 이 포함된 숫자도 처리
int TextLex::txt_get_number_tok(txtToken *tok)
{
	register char *srcptr;
	char *tokstr;
	int	n=0;
	int	number_type = GEN_NUM;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while(n < MAX_TOKSTR) {
		while (isdigit(*srcptr)) {
			*tokstr++ = *srcptr++;
			n++;
		}
		if ( (*srcptr=='.' || *srcptr==',' || *srcptr=='-') && (isdigit(srcptr[1])) ) {
			if (*srcptr=='.') number_type = DOT_NUM;
			else if (*srcptr==',') number_type = COMMA_NUM;
			else if (*srcptr=='-') number_type = DASH_NUM;

			*tokstr++ = *srcptr++;
			n++;
		}
		else break;
	}
	*tokstr = 0;
	tok->tok_realval = number_type;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len = n;
	return n;
}

/**
최상위 비트가 1인 문자열. 주로 한글에 해당.
*/
int TextLex::txt_get_big_ascii_tok(txtToken *tok)
{
	byte *srcptr;
	char *tokstr;
	int	n=0;
	srcptr	= (byte*)tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;

	// 한글 공백 문자 인지 검사.
	while(srcptr[0]==0xA1 && srcptr[1]==0xA1) {
		srcptr += 2;
		n += 2;
	}

	if (n > 0) {
		*tokstr = 0;
		tok->curptr = (char*)srcptr+n;
		tok->offset += n;
		tok->tok_len = n;
		tok->tok_val = SPACES;
		return n;
	}

	// @todo 음절, 음소 처리 필요
	while (n < MAX_TOKSTR && IS_BIG_ASCII(*srcptr)) {
		if (srcptr[0]==0xA1 && srcptr[1]==0xA1) {
			// 공백 문자이면 멈춘다. 다음 번 호출에서 토큰은 공백이 될것이다.
			break;
		}
		*tokstr++ = *srcptr++;
		n++;
	}
	*tokstr = 0;
	tok->curptr = (char*)srcptr;
	tok->offset += n;
	tok->tok_len = n;
	return n;
}

// get one line, but NOT include the last newline '\n'
int TextLex::txt_get_one_line_tok(txtToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while (*srcptr && *srcptr!='\n') {
		srcptr++;
		//*tokstr++ = *srcptr++;
		n++;
	}
	/*
	// Let's not include '\n'
	if (*srcptr=='\n') {
		tok->line_num++;
		*tokstr++ = *srcptr++;
		n++;
	}
	*/
	*tokstr = 0;
	tok->curptr = srcptr;
	tok->offset += n;
	tok->tok_len = n;
	return n;
}

// 한 라인 전체를 스킵..
// 그다음 txt_get_token_mem()을 실행하면 newline이 나온다.
int TextLex::skip_to_newline(txtToken *tok)
{
	char *srcptr;
	char *tokstr;
	int	n=0;
	srcptr	= tok->src_mem + tok->offset;
	tokstr	= tok->tok_str;
	while (*srcptr && *srcptr!='\n') {
		srcptr++;
	}
	n = (srcptr - tok->src_mem) ;
	tok->offset = n;
	return n;
}

/*
================== stack method
*/

txtToken *TextLex::push_tok(txtToken *tok)
{
	txtToken *ptr=0;
	// modular Increase
	real_tok_top++;
	if (real_tok_top >= TOK_HIST_SIZE) real_tok_top = 0;

	ptr = tokHist + real_tok_top;
	memcpy(ptr, tok, sizeof(txtToken));
	if (n_tok_hist < TOK_HIST_SIZE) n_tok_hist++;
	return ptr;
}

txtToken *TextLex::pop_tok()
{
	txtToken *ptr;
	if (n_tok_hist) n_tok_hist--;
	else return 0;
	ptr = tokHist + real_tok_top;
	real_tok_top = (real_tok_top-1) % TOK_HIST_SIZE;

	return ptr;
}

txtToken *TextLex::top_tok()
{
	if (n_tok_hist) return tokHist + real_tok_top;
	else return 0;
}

txtToken *TextLex::get_tok(int n_back)
// if n_back==0, it means the last token inserted; must be n_back > 0
{
	int	idx;
	if (n_back > n_tok_hist) return 0;
	idx = real_tok_top - n_back;
	if (idx < 0) idx = idx + TOK_HIST_SIZE;
	return (tokHist + idx);
}

txtToken *TextLex::search_tok(int tok_val)
// if n_back==0, it means the last token inserted; must be n_back > 0
{
	int	idx;
	int n_back=0;
	for (n_back=0, idx = real_tok_top; n_back < n_tok_hist; n_back++) {
		if (tokHist[idx].tok_val == tok_val)
			return (tokHist + idx);
		idx--;
		if (idx < 0) idx = TOK_HIST_SIZE-1;
	}
	// 못찾으면 문서의 첫 token을 return
	return tokHist;
}

int TextLex::get_last_tok_pos()
{
	return (real_tok_top);
}

/*
================== actual method
*/
void	TextLex::init_token(txtToken *tok, char *textbuff, int textleng)
{
	memset(tok, 0, sizeof(txtToken));
	tok->src_mem = textbuff;
	tok->src_len = textleng;
	tok->line_num=1;
}

//---------------------------
// puctuation
// comma(,), periol(.), (?), (!), (:), (;), ("), ('), (-),

// 분석 하기
// 단어 단위 (space, 문장부호 등으로 분 리 ) 로 나눈다.
// 각 단어가 득수 문자 포함 판단 (소유격, 축약형 )



// for anydict, we read only memory-based HTML contents
// token을 구한 다음 stack에 push하고 새 token 메모리 위치를 리턴.
txtToken *TextLex::txt_get_token_mem(txtToken *tok)
{
	txtToken *newtok;
	int	tokval;

	if (last_tok_top != real_tok_top && n_tok_hist >0) {
	// txt_get_token_mem_ahead()가 실행된 것이다.
		last_tok_top++;
		if (last_tok_top >= TOK_HIST_SIZE) last_tok_top = 0;

		newtok = tokHist + last_tok_top;
		memcpy(tok, newtok, sizeof(txtToken) );
		return (newtok);
	}

	// 일반적 경우
	tokval = txt_get_token_mem_actual(tok);
	newtok = push_tok(tok);

	last_tok_top = real_tok_top;

	return newtok;
}

// look ahead를 했다가 (txt_get_token_mem_ahead())
// 다음에 txt_get_token_mem()를 하지 않고 앞으로 진행하고자 할때 eat_up을 한다.
// return 1: eat up한 경우에만
int TextLex::txt_eat_up_ahead_token(txtToken *tok)
{
	if (last_tok_top != real_tok_top && n_tok_hist >0) {
	// txt_get_token_mem_ahead()가 실행된 것이다.
		last_tok_top++;
		if (last_tok_top >= TOK_HIST_SIZE) last_tok_top = 0;
		return 1;
	}

	return 0;
}


// 중간에 끼어 들어서 token을 구하므로 text에 대한 정보가 없다.
// 그래서 현재 top token에서 정보를 가져온다음 token을 구한다.
txtToken *TextLex::txt_get_token_mem_ahead(int ahead_n)
{
	txtToken *newtok;
	txtToken tmp;
	int	tokval;
	int	looked_n=0;
	int	mytop=0;


	// 일반적인 txt_get_token_mem()없이 먼저 look ahead하기 불가능
	if (n_tok_hist == 0) return 0;

	looked_n = real_tok_top - last_tok_top;
	if (looked_n < 0) looked_n += TOK_HIST_SIZE;

	//printf("real_tok_top=%d, last_tok_top=%d, looked_n=%d, ahead_n=%d\n",
	//	real_tok_top , last_tok_top, looked_n, ahead_n);

	if (looked_n >= ahead_n) {
	// 이미 txt_get_token_mem_ahead()가 실행된 것이다.
		mytop = last_tok_top+ahead_n;
		if (mytop >= TOK_HIST_SIZE) mytop -= TOK_HIST_SIZE;

		newtok = tokHist + mytop;
		return (newtok);
	}

	// txt_get_token_mem_ahead()는 순차적으로 한개씩 해야 한다.
	// 현재 top보다 2개 이상 앞서 보기는 제공하지 않는다.
	if ( (ahead_n - looked_n) ==1)  {
		memcpy(&tmp, tokHist + real_tok_top, sizeof(txtToken) );
		tokval = txt_get_token_mem_actual(&tmp);
		// push_tok 내에서 real_tok_top 증가
		newtok = push_tok(&tmp);

		return newtok;
	}
	else
		return 0;
}


