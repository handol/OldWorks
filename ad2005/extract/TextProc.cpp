#include "ace/Log_Msg.h"

#include <stdio.h>
#include <string.h> // strstr()
#include <ctype.h>
#include <stdlib.h> // strtol()
#include <time.h>

#include "EDict.h"
#include "TextLex.h"
#include "TextProc.h"
#include "dbStore.h"
#include "Docs.h"
#include "HDict.h"
#include "HGram.h"
#include "HStem.h"

#include "ace/Log_Msg.h"

#include "FuncLogger.h"

#define HASH_SEARCH	EDict::search
#define WORD_SEARCH	EDict::stemWord


#define PRN	printf
#define errPRN	printf

#define BUG

wordType *TextProc::etc_word=0;
wordType *TextProc::vs_word=0;
wordType *TextProc::not_word=0;
wordType *TextProc::the_word=0;
wordType *TextProc::a_word=0;
wordType *TextProc::an_word=0;
wordType *TextProc::to_word=0;
wordType *TextProc::of_word=0;
wordType *TextProc::by_word=0;
wordType *TextProc::copyright_word=0;
wordType *TextProc::all_word=0;
wordType *TextProc::rights_word=0;
wordType *TextProc::reserved_word=0;
wordType *TextProc::reserve_word=0;


void TextProc::prepare()
{
	byte	PS=0;
	//prepare_prefix_suffix();

	PRINTF((" TextProc::prepare()\n"));
	if (all_word!=NULL) return;
	
	etc_word = HASH_SEARCH("etc");
	vs_word = HASH_SEARCH("vs");
	the_word = HASH_SEARCH("the");
	a_word = HASH_SEARCH("a");
	not_word = HASH_SEARCH("not");
	to_word = HASH_SEARCH("to");
	of_word = HASH_SEARCH("of");
	by_word = HASH_SEARCH("by");
	copyright_word = HASH_SEARCH("copyright");
	all_word = HASH_SEARCH("all");
	rights_word = HASH_SEARCH("right");
	reserved_word = HASH_SEARCH("reserved");
	reserve_word = HASH_SEARCH("reserve");

	if (etc_word==0)
		errPRN("'etc' NOT in dictionary\n");
	if (vs_word==0)
		errPRN("'vs' NOT in dictionary\n");

}

//---------------
void TextProc::txtToken_print2(txtToken *tok)
{
	PRN("%d: pos=%5d, val=%3d, len=%2d, real=%3d, ",
  	 tok->line_num, tok->offset, tok->tok_val, tok->tok_len, tok->tok_realval);
	if (tok->tok_len==1 && tok->tok_val < 128 ) PRN("ch=%c\n", tok->tok_val);
	else if (tok->tok_val == WORDS) {
		if (tok->w) PRN("str=%s, w=%s\n", tok->tok_str, tok->w->key);
		else PRN("str=%s\n", tok->tok_str);
	}
	else PRN("\n");
}

void TextProc::txtToken_print(txtToken *tok)
{
	PRN("%d: offset=%5d, val=%3d, len=%2d, real=%3d, ",
  	 tok->line_num, tok->offset, tok->tok_val, tok->tok_len, tok->tok_realval);
	if (tok->tok_val != NUMBER  && tok->tok_len==1) PRN("ch=%c\n", tok->tok_val);
	else PRN("str=%s\n", tok->tok_str);
}



//#define reset_stc(X) memset((X), 0, sizeof(stcInfo))

void TextProc::stcInfo_print(stcInfo *stc, const char *msg)
{
	if (msg==NULL)
		msg = "";
	PRN("%s [Line %d] Exam=%d Level=%d, toks:%d, words:%d, dict_w:%d, common_w:%d, up_w:%d, h_w:%d\n",
		msg,
		stc->no_exams,
		stc->s_line, 
		stc->level,  
		stc->no_toks, stc->n_words, stc->n_dict_words, stc->n_commons,
		stc->n_up_words, stc->n_head_words);
}

void TextProc::STCtoks_init()
{
	memset(STCtoks, 0, sizeof(txtToken*) *TOK_HIST_SIZE);
	no_STCtoks=0;
}

int TextProc::STCtoks_push(txtToken *tok)
{
	if (no_STCtoks < TOK_HIST_SIZE)
		STCtoks[no_STCtoks++] = tok;
	return (no_STCtoks);
}

txtToken *TextProc::STCtoks_top()
{
	if (no_STCtoks) return STCtoks[no_STCtoks-1];
	else return 0;
}

txtToken *TextProc::STCtoks_pop()
{
	if (no_STCtoks) return STCtoks[--no_STCtoks];
	else return 0;

}

/**
STCtoks ³»ÀÇ ¸ðµç token Á¤º¸¸¦ Ãâ·Â.
*/
void TextProc::STCtoks_print()
{
	int i;
	for(i=0; i<no_STCtoks; i++)
		txtToken_print2(STCtoks[i]);
}

/**
STCtoks ³»ÀÇ ¹®ÀåÀÇ ½ÇÁ¦ ¹®ÀÚ¿­ Ãâ·Â
*/
int  TextProc::printSentenceFromTokens()
{
	int len=0;
	char *ptr=0;
	txtToken *tok1=0, *tok2=0;
	char tmp=0;
	if (no_STCtoks==0) return 0;
	if (no_STCtoks>(TOK_HIST_SIZE/2)) return 0;
	tok1 = STCtoks[0];
	tok2 = STCtoks[no_STCtoks-1];

	if (tok1 == NULL || tok2 == NULL)
		return 0;

	if (tok1->src_mem == NULL)
		return 0;

	ptr = tok1->src_mem + (tok1->offset - tok1->tok_len);
	len = tok2->offset - (tok1->offset - tok1->tok_len);
	if (len>1024) len=1024;
	tmp = ptr[len];
	ptr[len] = 0;

	ACE_DEBUG((LM_DEBUG, "%d-%d %s\n\n", tok1->line_num, tok2->line_num, ptr));
	ptr[len] = tmp;
	return 0;
}


//#define COPYRIGHT "Copyright (C)"

int TextProc::check_copyright_stop(txtToken *currTok)
{
	txtToken *tok=0;
	wordType *w;
	int ahead=0;

	w = currTok->w;

	if (currTok->tok_val==COPYRIGHT_MARK) {
		tok = Lexer.txt_get_token_mem_ahead(++ahead);
		if (tok && tok->tok_val==SPACES)  tok = Lexer.txt_get_token_mem_ahead(++ahead);

		//printf("check_copyright_stop(): COPYRIGHT_MARK: tok=%d, line=%d\n", tok->tok_val, tok->line_num);
		if (tok && tok->tok_val==NUMBER) {
			return 1;
		}
	}
	else if (w==all_word) {
		tok = Lexer.txt_get_token_mem_ahead(++ahead);

		if (tok && tok->tok_val==SPACES)  tok = Lexer.txt_get_token_mem_ahead(++ahead);
		//printf("check_copyright_stop(): all: tok=%d, line=%d\n", tok->tok_val, tok->line_num);

		if (tok && tok->w==rights_word) {
			tok = Lexer.txt_get_token_mem_ahead(++ahead);
			if (tok && tok->tok_val==SPACES) tok = Lexer.txt_get_token_mem_ahead(++ahead);
			//printf("check_copyright_stop(): all: tok=%d\n", tok->tok_val);
			if (tok && (tok->w==reserved_word ||tok->w==reserve_word) )
				return 1;
		}
	}
	else if (w==copyright_word) {
		tok = Lexer.txt_get_token_mem_ahead(++ahead);
		if (tok->tok_val==SPACES) tok = Lexer.txt_get_token_mem_ahead(++ahead);
		//printf("check_copyright_stop(): copyright_word: tok=%d line=%d\n", tok->tok_val, tok->line_num);
		if (tok && (tok->tok_val==COPYRIGHT_MARK || tok->tok_val==NUMBER))
			return 1;
	}

	return 0;
}


/**
 ¹®Àå ³­ÀÌµµ¸¦ °è»êÇÏ°í, ¿¹¹®À» ÃßÃâÇÏ¿© ¹®¼­ Á¤º¸¿¡ ´ã´Â´Ù.
*/
int TextProc::get_examples()
{
	FuncLogger log("TextProc::get_examples");
	int	setence_level;
	int	no_examples;
	PRINTF(("get_examples()\n"));
	thisDoc.no_sentences++;
	setence_level = calc_stc_level(&stc);
	no_examples = get_example_from_stc(&stc, &thisDoc, hangul_words);

	thisDoc.level_sum += setence_level;

	thisDoc.no_examples += no_examples;

	init_sentence_proc("ex succ");
	ACE_DEBUG((LM_DEBUG, "[%d] level=%d, examples=%d\n", thisDoc.no_sentences, setence_level, no_examples));
	

	return 0;
}

/**
ÇÑ ¹®¼­¿¡ ´ëÇØ ¿¹¹® ¼öÁýÀÌ  ³¡³­ ÈÄ¿¡ ¹®¼­ Á¤º¸ Á¤¸®. ¼öÁý °á°ú Á¤¸®.
*/
void	TextProc::finish_examples(docType *docinfo)
{
	if (thisDoc.no_sentences)
		thisDoc.doc_level = (int)(thisDoc.level_sum / thisDoc.no_sentences) + 1;
	else
		thisDoc.doc_level = 0;

	docinfo->level = thisDoc.doc_level;
}

void	TextProc::get_result(StrStream &resStr)
{
	FuncLogger log("TextProc::get_result(StrStream)");
	resStr.add("%d Paragraphs, %d Sentences, %d Exams, Level=%d\n",
		thisDoc.no_paragraphs, thisDoc.no_sentences, thisDoc.doc_level,
		thisDoc.no_examples);
}

void	TextProc::get_result(oneDoc *doc)
{
	FuncLogger log("TextProc::get_result(oneDoc)");
	memcpy(doc, &thisDoc, sizeof(oneDoc) );
}

int TextProc::init_sentence_proc( char *msg)
{
	char msgbuf[256];
	if (msg==NULL)
		msg = "";
	sprintf(msgbuf, "init_sentence_proc() - %s ", msg);
	
	if (no_STCtoks) {
		stcInfo_print(&stc, msgbuf);
		printSentenceFromTokens();
	}

	no_STCtoks = 0;
	memset(&stc, 0, sizeof(stcInfo));
	stc.acronym_search_flag=1;
	return 0;
}

/**
ÅØ½ºÆ®¸¦ ÆÄ½ÌÇÏ¸ç ¹®ÀåÀ» ÃßÃâÇÏ°í, ¹®Àå¿¡¼­ ¿¹¹®À» ÃßÃâÇÑ´Ù.

Áßº¹µÇ´Â ¿¹¹®, ´Ü¾î µîÀ» Ã³¸®ÇØ¾ß ÇÑ´Ù. È¨ÇÇ ÇÏ´Ü¿¡ ÀÚÁÖ ³ªÅ¸³ª´Â copyright ±âÈ£ µîµµ Ã³¸®ÇØ¾ß ÇÑ´Ù.
»çÀü¿¡ ¾ø´Â ´Ü¾î Ã³¸®.
ÇÕ¼º¾î Ã³¸®.
*/

int TextProc::txt_analyze(docType *docinfo)
{
	FuncLogger log("TextProc::txt_analyze");
	txtToken	tok;
	txtToken	*this_tok=0, *last_tok=0, *next_tok=0;
	int	tmp;
	wordType *w;


	int	small_mark=0;
	int	sm_begin_line=0;
	int	sm_end_line=0;
	int	sm_w_cnt=0;

	int	big_mark=0;
	int	bm_begin_line=0;
	int	bm_end_line=0;
	int	bm_w_cnt=0;

	pairMARK dq, sq;

	int	line_start=1; // is this the first token on the line?
	int	last_tok_val=0;

	int	stc_consecutive=0;

	int begin_w_left_m=0;
	int	words_in_a_line=0;
	int	odds_in_a_line=0;
	int	in_doubt_line=0;
	int	found_stc_in_a_line=0;
	byte	PS=0;

	TextProc::prepare();
		
	if (docinfo->text==0 || docinfo->size==0) return 0;

	PRINTF(("DOC processing: id=%d path=%s size=%d\n", docinfo->doc_id, docinfo->fpath, docinfo->size));
	ACE_DEBUG((LM_DEBUG, "DOC processing: id=%d path=%s size=%d\n", docinfo->doc_id, docinfo->fpath, docinfo->size));

	ratio_in_page = 0;
	hangul_words =0;
	memset(&dq, 0, sizeof(dq));
	memset(&sq, 0, sizeof(sq));
	memset(&thisDoc, 0, sizeof(oneDoc));
	thisDoc.doc_id  = docinfo->doc_id;

	Lexer.init_token(&tok, docinfo->text, docinfo->size);
	STCtoks_init();
	init_sentence_proc(0);


	while(tok.offset < tok.src_len && tok.tok_val != TOK_EOF) {
		// process one line or one sentence.

		ratio_in_page = PERCENT(tok.offset, tok.src_len);

		tmp = tok.offset;
		tok.w = tok.orgw = w = 0;
		tok.conj_PS = 0;
		// ÃÊ±âÈ­°¡ Áß¿äÇÏ´Ù **** ÀÌ°Í ¶§¹®¿¡ ¾Ë¼ö ¾ø´Â  BUG »ý°å½¿


		this_tok = Lexer.txt_get_token_mem(&tok);

		if (tok.tok_val == TOK_EOF) break;
		if (tok.offset==tmp) {
			if (tok.src_mem[tok.offset]==0) break;
			PRN("-_-; -- %d (%d-%c)", tok.offset,
				tok.src_mem[tok.offset], tok.src_mem[tok.offset]);
			tok.offset++;
		}

		if (tok.tok_val==COPYRIGHT_MARK) {
		}

		if (tok.tok_val==URL_CMNT)
			goto NEXT_LOOP;
		if (tok.tok_val==HAN_STR) {
			hangul_words++;
			stc.n_hangul_words++;
			
		}

		if (tok.tok_val==SPACES || tok.tok_val==TABS)
			goto NEXT_LOOP;



		if (line_start) {
			stc.s_line = tok.line_num;
			line_start = 0;
		}

		if (tok.tok_val == BAR) {
			init_sentence_proc("BAR");
			goto NEXT_LOOP;
		}


		if (tok.tok_val == NEWLINES)
		{
			int big_words=0;

			if (dq.nums) {
				if (tok.line_num - dq.b_line > 1)
					dq.nums = 0;
			}
			big_words = stc.n_up_words+stc.n_head_words;
			line_start = 1;


			// °¢ ¶óÀÎ¿¡ ´ëÇØ ¹®Àå ¿©ºÎ¸¦ °Ë»ç
			if (found_stc_in_a_line)  {
				found_stc_in_a_line = 0;
				in_doubt_line = 0;
			}
			else if (words_in_a_line==0)
			{

				{
					// ºó ¶óÀÎ == ¹®Àå ¾Æ´Ô
					if (stc_consecutive) thisDoc.no_paragraphs++;
					stc_consecutive = 0;
					init_sentence_proc( "empty line");
					in_doubt_line = 0;
				}
			}
			/*
			else if (stc.n_dict_words==0)
			{
				init_sentence_proc("no word, no dict word");
				in_doubt_line = 0;
			}
			*/
			else if (last_tok_val==RIGHT_M) {
				//2002.10.5

				init_sentence_proc( "RIGHT_M");
				in_doubt_line = 0;
			}
			else if (odds_in_a_line >= 4) {
				init_sentence_proc( "odd char");
				in_doubt_line = 0;
			}
			else if ( ((big_words << 1) > (stc.n_words+1) && stc.n_words>1)
				|| (odds_in_a_line > words_in_a_line ))
			{
			// ´ë¹®ÀÚ ´Ü¾î°¡ ¸¹°Å³ª, ¼ý ÀÚ (±â È£) µîÀÇ ¸¹À¸¸é ÀÇ½É½º·¯¿î ¶óÀÎ
				in_doubt_line++;
				if (in_doubt_line > 1) {
				// ÀÇ½É½º·± ¶óÀÎÀÌ ¿¬¼ÓµÉ °æ¿ì  == ¹®Àå ¾Æ´Ô
					if (stc_consecutive) thisDoc.no_paragraphs++;
					stc_consecutive = 0;

					init_sentence_proc( "doubt line seq");
				}
			}
			else {
				in_doubt_line = 0;
			}
			words_in_a_line = 0;
			odds_in_a_line = 0;
			goto NEXT_LOOP;

		}

		STCtoks_push(this_tok);
		stc.no_toks++;

		if (tok.tok_val == PARATH) {
			if (tok.tok_realval=='(') stc.parath++;
			else stc.parath--;

			goto NEXT_LOOP;
		}
		if ( tok.tok_val == END_PUNCT)
		{
			PRINTF(("TextProc:: END_PUNCT\n"));
			stc.e_line = tok.line_num;
			// ¾à¾îÀÎ °æ¿ì ¹®Àå ºÎÈ£°¡ ¾Æ´Ï´Ù.
			if (last_tok->w != NULL && (last_tok->w==etc_word || last_tok->w==vs_word)) {
				// "etc.", "vs."
				PRINTF(("TextProc::END_PUNCT - etc. vs.  %X\n", etc_word));
				goto NEXT_LOOP;
			}

			if ((last_tok->tok_realval & DOT_W)) {
				PRINTF(("TextProc::END_PUNCT - DOT_W\n"));
				goto NEXT_LOOP;
			}

			if ((last_tok->tok_realval & HEAD_W) && last_tok->tok_len <=3) {
				//´ë¹®ÀÚ·Î ½ÃÀÛ, ±æÀÌ°¡ 3 ÀÌÇÏ --> ¾à¾îÀÏ °¡´É¼ºÀÌ ³ô´Ù
				PRINTF(("TextProc::END_PUNCT - HEAD_W\n"));
				goto NEXT_LOOP;
			}


			if (dq.nums==0)
			{
				// Axx xxx (hello!) aaaa
				/*
				int tokval;
				this_tok = Lexer.txt_get_token_mem_ahead(1);
				tokval = this_tok->tok_val;
				if (tokval!=NEWLINES && tokval!=SPACES && tokval!=TABS)
					goto NEXT_LOOP;
				*/
				if (stc.parath && stc.n_words_in_parath <4)
				{
					PRINTF(("TextProc::END_PUNCT - Too short sentence.\n"));
					goto NEXT_LOOP;
				}
			}
			else
			//if (dq.nums)
			{
				// quatation °æ¿ì Ã³¸®
				this_tok = Lexer.txt_get_token_mem(&tok);
				STCtoks_push(this_tok);
				stc.no_toks++;
				if (tok.tok_val == SPACES
					|| tok.tok_val == TABS) {
					this_tok= Lexer.txt_get_token_mem(&tok);
					STCtoks_push(this_tok);
					stc.no_toks++;
				}

				if (tok.tok_val == D_QUOTE
					|| tok.tok_val == NEWLINES
					|| tok.tok_val == TOK_EOF)
				{
					if (dq.nums==0) {
						dq.nums++;
						dq.b_line = tok.line_num;
					}
					else {
						dq.nums--;
						if (dq.nums < 0) dq.nums = 0;
						dq.e_line = tok.line_num;
					}

				}
				else
					goto NEXT_LOOP;
			}



			// ¹®ÀåÀÎÁö °Ë»ç  // article ÀÌ ¾Æ´Ñ °æ¿ì.
			//  ¸ðµÎ ´ë¹®ÀÚ·Î µÈ°ÍÀÌ ¸¹´Ù.  // ¹®ÀåºÎÈ£°¡ ¾ø´Ù.
			//	ÇÑ ÁÙ¿¡ ´Ü¾î ¼ö°¡ ³Ê¹« Àû´Ù.
			if (
				stc.n_odd_dict_words >= 4
				&&	(stc.n_odd_dict_words<<1) >= stc.n_dict_words
				)
			{
				if (stc_consecutive) thisDoc.no_paragraphs++;
				stc_consecutive = 0;

				init_sentence_proc( "dict word in upper");
				goto NEXT_LOOP;
			}
			else if (
				(stc.n_up_words+stc.n_head_words) >= stc.n_words-2)
			{
				if (stc_consecutive) thisDoc.no_paragraphs++;
				stc_consecutive = 0;

				init_sentence_proc( "many up/head word");
				goto NEXT_LOOP;
			}

			stc_consecutive = 1;

			//---- ¹®Àå ¹ß°ß... example ±¸ÇÏ±â ...
			get_examples();

			found_stc_in_a_line = 1;

		}// if tok==END_PUNCT
		else if ( tok.tok_val == MID_PUNCT)
		{
			if ( (tok.tok_realval==':' || tok.tok_realval==';')
				&& stc.n_words > 15 && stc.n_commons > 4) {
			// ³Ê¹« ±æ¾îÁö´Â °É ¸·±â À§ÇØ
			//---- ¹®Àå ¹ß°ß... example ±¸ÇÏ±â ...
				get_examples();

				found_stc_in_a_line = 1;
			}
		}

		else if ( tok.tok_val == D_QUOTE)
		{
			if (dq.nums==0) {
				dq.nums++;
				dq.b_line = tok.line_num;
			}
			else {
				dq.nums--;
				if (dq.nums < 0) dq.nums = 0;
				dq.e_line = tok.line_num;
			}
		}
		else if ( tok.tok_val == S_QUOTE)
		{

		}
		else if (tok.tok_val == LEFT_BIG_M) {
			big_mark++;
			bm_begin_line = tok.line_num;
		}
		else if (tok.tok_val == RIGHT_BIG_M) {
			big_mark--;
			if (big_mark < 0) big_mark = 0;
			bm_end_line = tok.line_num;

			init_sentence_proc( "RIGHT_BIG_M");
		}
		else if (tok.tok_val == LEFT_M) {
			if (stc.no_toks==1) begin_w_left_m = 1;
			small_mark++;
			sm_begin_line = tok.line_num;

		}
		else if (tok.tok_val == RIGHT_M) {
			small_mark--;
			if (small_mark < 0) small_mark = 0;
			sm_end_line = tok.line_num;

			if (begin_w_left_m) {
			// '['·Î ¹®ÀåÀÌ ½ÃÀÛÇÑ °æ¿ì
				if (stc.n_words==0 || stc.n_dict_words==0 ||
					stc.n_up_words >= stc.n_dict_words)
				{
					init_sentence_proc( "RIGHT_M: no dict, or many up w");
				}
				else if (stc.n_head_words >= 3 &&
					stc.n_head_words >= stc.n_dict_words)
				{
					init_sentence_proc( "RIGHT_M: many head w");
				}
				begin_w_left_m = 0;
			}

		}
		else if (tok.tok_val == WORDS || tok.tok_val == HAN_STR)
		{
			#ifdef HDEBUG
			if (tok.tok_val == HAN_STR)
				txtToken_print(&tok);
			#endif

			if (tok.tok_len==1 && toupper(tok.tok_str[0])!='A') goto NEXT_LOOP;
			if (tok.tok_realval==WEBSITE_W) goto NEXT_LOOP;


			words_in_a_line++;
			stc.n_words++;
			if (stc.parath) stc.n_words_in_parath++;

			if (tok.tok_realval == LOW_W) stc.n_low_words++;
			else if (tok.tok_realval & UP_W) {
				stc.n_up_words++;
						}
			else if (tok.tok_realval & HEAD_W) {
				stc.n_head_words++;
			}

			if (small_mark) sm_w_cnt++;
			if (big_mark) bm_w_cnt++;

			if ((tok.tok_realval & APO_W)) {
			// 2002.12.4
				if (tok.tok_str[tok.tok_len-1]=='\'')
					tok.tok_str[tok.tok_len-1] = 0;
			}

			if (tok.tok_val == HAN_STR) // ÇÑ±Û 
			{
				w = 0;

				/*
				StrStream resStr;
				resStr.init(1024*2);
				HStem stemmer;
				stemmer.stem(tok.tok_str, tok.tok_len );
				stemmer.printStem_stream(resStr);
				*/
				
				handleHangulToken(&thisDoc, &stc, &tok);

				if (tok.hw != NULL)
					stc.n_dict_words++;
			}

			if (tok.tok_val == WORDS)
				w = WORD_SEARCH(tok.tok_str, tok.tok_len, &PS);

			if (w==0 && tok.tok_val == WORDS)
			{
				if ((tok.tok_realval & DASH_W)) {
					//w = search_new_idiom(tok.tok_str);
					if (w==0) {
						//handle_new_dash_word(&tok);
						//w = tok.w;
					}
					#ifdef BUG

					#endif
				}
				//2003.3.30
				// dash ¼÷¾î°¡ ¾Æ´Ñ °æ¿ì¿¡¸¸ find_org_word()
				else {
					/*
					if ( (tok.tok_realval & (UP_W|APO_W|DASH_W|DOT_W|WEBSITE_W))==0) {
						// ½Å±Ô ´Ü¾î ¸®½ºÆ®¿¡ ÀÖÁö´Â °Ë»ç
						strcpy(org_word, tok.tok_str);
						org_word[0] = tolower(org_word[0]);
						w = search_tmp_new_word(org_word);
					}
					*/

					//if (w==0) w = find_org_word(&tok, org_word);
					if (w==0) {
						this_tok->w = 0;

						if (tok.tok_realval & HEAD_W)
							stc.n_name_words++; // maybe, this is
						else
							stc.n_no_dict_words++;

					} // unkown word

					else {
					// ´Ü¾î Ã£Àº °æ¿ì


						// ÀÏ¹ÝÀûÀÎ ¹®ÀåÀÌ ÀüºÎ ´ë¹®ÀÚ·Î µÈ °æ¿ìµµ ÀÖ´Ù
						// ÀÌ·²¶§´Â ¾à¾î ÃßÃâ ÇÏÁö ¾Ê´Â´Ù
						if (tok.tok_realval & UP_W) {
							stc.n_up_dict_words++;
							stc.n_odd_dict_words++;
							if (w->PS&W_BASIC)  // ±âº»
							{
								if (stc.n_up_dict_words>=2)
									stc.acronym_search_flag = 0;
							}
						}
						else if (tok.tok_realval & HEAD_W) {
							stc.n_head_dict_words++;
							stc.n_odd_dict_words++;
						}
					}
				}

			}

			if (w) {
				int PS;


				/*
				@dahee 2005.5.31
				if (tok.conj_PS==0 && w->is_conj)
					tok.conj_PS = w->is_conj;
				*/

				if (w->org_w) {
					/* ÀÌ ´Ü¾î°¡ said ÀÎ °æ¿ì, ±× ¿øº»À» Ã£¾Æ°£´Ù. */
					this_tok->conj_PS = w->is_conj;
					this_tok->w = w->org_w;
				}
				else
					this_tok->w = w;

				stc.n_dict_words++;
				PS = w->PS;
				if ( PS & REQUISIT_PS) stc.n_commons++;
				/*
				if ( (PS&BE_VERB) || (PS&CONJ) || (PS&AUX_VERB))
					stc.n_verb_conj++; // µ¿»ç,Á¶µ¿»ç, Á¢¼Ó»ç
				*/

				/**
				// NOT working good. 2005.7.18
				if (dq.nums==0 && ratio_in_page > 80 && check_copyright_stop(this_tok)) {
					PRN("Stop by Copyright at line %d !!\n", tok.line_num);
					break;
				}
				*/

			}

			// acronym Ã³¸®
			if (

				(tok.tok_realval & (UP_W|HEAD_W))
				&& (tok.tok_realval & (APO_W|DASH_W))==0
			  )
			{
					/*
				if (w==0 || (w->w_type & W_UPPER) )
					search_acro_words(&tok);
					*/
			}


			//*****************
			//2003.1.19
			// ¿äºÎºÐÀÌ ÇÊ¿äÇÑ°¡ ??
			// ¿À·ù °¡´É¼º ³ôÀ½
			/*
			if (words_in_a_line==1) {
				// ¶óÀÎÀÌ ´ë¹®ÀÚ½ÃÀÛ ´Ü¾î·Î ½ÃÀÛÇÏ´Â °æ¿ì
				if (w && (tok.tok_realval & HEAD_W) && islower(w->key[0])) {
					memset(&stc, 0, sizeof(stcInfo));
					init_sentence_proc();

					STCtoks_push(this_tok);
					stc.no_toks++;
				}

			}
			*/




		} // if tok==WORDS

		else if (tok.tok_val >= SPACES) {
		//--> tok.tok_val == NUMBER
			stc.no_odd_toks++;
			odds_in_a_line++;
		}
		else {
			stc.no_odd_toks++;
			odds_in_a_line++;
		}


	NEXT_LOOP:
		last_tok_val = tok.tok_val;
		last_tok = this_tok;

	} // while (text file)

	//stcInfo_print(&stc);
	
	finish_examples(docinfo);
	return 0;
}


int	TextProc::get_trailing_hangul(char *start)
{
	int ch=0, i;
	int cnt=0;
	char *ptr=start;

	for(i=0; i<2 && cnt < MIN_HANGUL_CHAR; i++) {
		// °°Àº ¶óÀÎ »ó¿¡ ÀÖ´Â ÇÑ±Û Æ÷ÇÔ
		// ´ÙÀ½ ÁÙ±îÁö
		while(*ptr && *ptr!='\n') {
			ch = *(unsigned char *)ptr++;
			if (IS_BIG_ASCII(ch)) {
				if(++cnt >= MIN_HANGUL_CHAR) break;
			}
		}
	}

	while(*ptr && *ptr!='\n') ptr++;
	return ((uint4) ptr - (uint4)start);
}

/**
¿¹¹®À» ÃßÃâÇÒÁö ¸»Áö °áÁ¤.
2005³â ¹öÁ¯¿¡¼­´Â ±×³É ¸ðµÎ ¿¹¹® ÃßÃâ ÇÏÀÚ. °ü»ç´Â Á¦¿Ü.
return 0: ¿¹¹® ÃßÃâ ÇÏÁö ¸»±â.
*/
int TextProc::probeExample(wordType *w)
{
	if (w->PS&ARTI)
		return 0;
	else
		return 1;

	/*-- ÀÌ ¾Æ·¡´Â 2002³â ¹öÁ¯. --*/
	// ±âº» ´Ü¾îµéÀº »ý·«
	if (w->PS&W_BASIC)
		return 0;


	if (w->PS&(FREQ_W|PREP|AUX_VERB|REL_QUEST))
		return 0;

	//  ¾ÆÁÖ ÈçÇÑ ´Ü¾î (ÀÏºÎ ÀüÄ¡»ç, all, ÀÏºÎ Á¶µ¿»ç µîµî) --1/8
	if (w->PS&FREQ_W) {
		if (PROB_1_8(rand())) return 0;
	}

	// ÀüÄ¡»ç, Á¶µ¿»ç, ÀÇ¹®»ç  µî -- 1/4
	if (w->PS&(PREP|AUX_VERB|REL_QUEST) ) {
		if (PROB_1_4(rand())) return 0;
	}

	return 1;
}

int	TextProc::wordList_for_idiom(wordType **wpArr, int start_pos, int max_words)
{
	int	idx;
	int	max_pos;
	int	w_count = 0;
	txtToken **ptr;

	max_pos = start_pos + max_words;
	if (max_pos > no_STCtoks) max_pos = no_STCtoks;

	for(idx = start_pos, ptr = STCtoks+start_pos ; idx < max_pos; idx++, ptr++) {
		if ((*ptr)->w != NULL)
		{
			wpArr[w_count++] = (*ptr)->w;
		}		
		else 
			break;
	}
	return w_count;
}

/**
¿¹¹® Áß¿¡ ´Ü¾î ºÎºÐ¸¸ Ãâ·Â.
*/
void TextProc::printExamWord(char *begin, int len)
{
	char tmpword[64];

	if (len > sizeof(tmpword)-1) len = sizeof(tmpword)-1;

	strncpy(tmpword, begin, len);
	tmpword[len] = 0;

	ACE_DEBUG((LM_DEBUG, "%s\n", tmpword));
}

void TextProc::printWordExamInfo(wordType *word, exType *ex)
{
	if (word) ACE_DEBUG((LM_DEBUG, "W=%s WID=%d ", word->key, word->w_id));

	ACE_DEBUG((LM_DEBUG, "EX: Doc=%d, Spos=%d, Slen=%d, Wpos=%d, Wlen=%d, Conj=%d\n",
				ex->doc_id, ex->s_pos, ex->s_len, ex->w_pos, ex->w_len, ex->conjugate));
}

/**

//return : the # of example sentence
// example ±¸ÇÏ±â
//-- ¹®Àå ºÐ¼® , Ç°»ç ºÐ¼® ...
// ´Ü¾î Á¾·ùº° (´Ü¾îÇÏ³ª, ÇÕ¼º¾î(hyphen), ´Ü¾îÁ¶ ÇÕ )·Î ÃßÃâÀÌ ´Ù¸£´Ù.
// ´Ü¾îÇÏ³ª : ´Ü¾î level, ±Û level ¿¬°üÇÏ¿© ¼öÁØÀÌ ³·À¸¸é ÃßÃâ È®·ü ³·°Ô
// ÇÕ¼º¾î : ¹«Á¶°Ç ÃßÃâ
// ´Ü¾î Á¶ÇÕ : ±¸¼º ´Ü¾îÀÇ ¼öÁØ

*/
int TextProc::get_example_from_stc(stcInfo *stc, oneDoc *Doc, int hangul_words)
{
	FuncLogger log("TextProc::get_example_from_stc");
	int	no_exams=0;
	int	i;
	txtToken **ptr = 0;
	txtToken *tok = 0;
	wordType *w=0;
	HwordType *hw=0;


	idiomType	*idiom =0;

	txtToken *tok1=0, *tok2=0;

	int	no_words=0;

	exType	ex;

	memset(&ex, 0, sizeof(ex));
	
	//¿¹¹® ±æÀÌ°¡ 0ÀÎ °æ¿ì°¡ ¹ß»ýÇß¾ú´Ù
	if (no_STCtoks <= 3) return 0;


	tok1 = STCtoks[0];
	tok2 = STCtoks[no_STCtoks-1];

	ex.doc_id = Doc->doc_id;
	ex.s_pos = (tok1->offset - tok1->tok_len);
	ex.s_len = tok2->offset - ex.s_pos;
	ex.level = (byte) stc->level;

	ex.w_pos = 0;
	ex.w_len = 0;


	if (debug) ACE_DEBUG((LM_DEBUG,"no_STCtoks=%d, s_pos=%d, s_len=%d\n",
		no_STCtoks, ex.s_pos, ex.s_len));


	//¹®Àå ±æÀÌ°¡ ±ØÈ÷ ÂªÀº °Í  (ex) I am sam. -- 9ÀÚ
	if (ex.s_len < 7) return 0;

	for(i=0, ptr=STCtoks; i<no_STCtoks; i++, ptr++) {
		int dbres=0;

		tok = *ptr;
		w = tok->w;
		hw = tok->hw;

		if (tok->tok_val==WORDS)
		{
			if (w==0)
				continue;
		}
		else if (tok->tok_val==HAN_STR)
		{
			if (hw==0)
				continue;
		}
		else
		{
			continue;
		}

		if (tok->tok_val==WORDS && probeExample(w)==0) {
			if (debug) ACE_DEBUG((LM_DEBUG, "Skip: %s\n", w->key));
			continue;
		}

		// no_words++;
		//if (w) printf(" - %s\n", w->key); // @dahee

		ex.w_pos = (tok->offset - ex.s_pos - tok->tok_len);

		if (w->w_type & W_HYPEN) { // hypen Æ÷ÇÔ --> ÇÕ¼º¾î

		}

		/* ´Ü¾î ¿¹¹®  */

		ex.w_pos = (tok->offset - ex.s_pos - tok->tok_len);
		ex.w_len = tok->tok_len;
		ex.conjugate = tok->conj_PS;
		if (tok->tok_val==WORDS && debug) {
			printWordExamInfo(w, &ex);
			printExamWord(tok->src_mem+ex.s_pos+ex.w_pos, ex.w_len);
		}

		if (tok->tok_val==WORDS && ex.doc_id > 0)
		{
			dbres = dbStore::store_wordEx(w, &ex);
			printf("Eng-Ex: %s stc pos=%d word pos=%d dbres=%d\n", w->key, ex.s_pos, ex.w_pos, dbres);
			if (dbres >= 0)
				no_exams++;
		}

		if (tok->tok_val==HAN_STR && ex.doc_id > 0)
			
		{
			dbres = dbStore::store_hanwordEx(hw, &ex);
			printf("Han-Ex: %s stc pos=%d word pos=%d dbres=%d\n", hw->key, ex.s_pos, ex.w_pos, dbres);
			if (dbres >= 0)
				no_exams++;
		}
		
		if (dbres < 0) ACE_DEBUG((LM_INFO,"Query Failed\n%s\n%s\n", dbStore::errmsg, dbStore::querybuf));
		

		if (tok->tok_val!=WORDS)
			continue;
		
		/* ¼÷¾î ¿¹¹® */
		if (w->num_idioms > 0)
		{
			int	wcntIdiom=0;
			wordType *wpArr[W_IN_IDIOMS] = {0};
	

			wcntIdiom = wordList_for_idiom(wpArr, i, 3);
			if (wcntIdiom > 1)
			{
				printf("idiom: word=%s,  wcntIdiom=%d\n", wpArr[0]->key, wcntIdiom);
				wcntIdiom = 3;
				idiom = EDict::searchIdiom(wpArr, wcntIdiom);
				
				if (idiom == NULL)
				{
					wcntIdiom =  2;
					idiom = EDict::searchIdiom(wpArr, wcntIdiom);		
				}
			}
	
			if (idiom) {
				printf("Idiom-Ex: %s stc pos=%d word pos=%d\n", idiom->key, ex.s_pos, ex.w_pos);
				if (debug) EDict::printIdiom(idiom);

				tok = *(ptr + (wcntIdiom-1));
				ex.w_len = tok->offset - ex.s_pos - ex.w_pos;
				if (debug) printExamWord(tok->src_mem+ex.s_pos+ex.w_pos, ex.w_len);

				if (ex.doc_id > 0)
				{
					dbStore::store_idiomEx(idiom, &ex);
					if (dbres >= 0)
						no_exams++;
				}
			}
			
		}

	}// for

	stc->no_exams = no_exams;
	return no_exams;

}

//----------------------------------------------
// ¿µ¾î article¸¦ ÀÔ·ÂÀ¸·Î ¹Þ¾Æ
// ¿µ¾î ¹®Àå º¹Àâµµ, ´Ü¾î ³­ÀÌµµ µîÀ» ºÐ¼®ÇÏ¿©
// ÀüÃ¼ÀûÀ» ¼öÁØÀ» Æò°¡ÇÑ´Ù.

// ¹®Àå ±¸Á¶ ºÐ¼® -- ¾ÆÁ÷ ±¸Çö ¾È‰Î --- ÇÊ¼ö´Â ¾Æ´Ô.
// ´Ü¹®, º¹¹®, Áß¹®, È¥¹® (simple, complex, compound, mixed)
// º¹¹® : ÁÖÀý°ú 1°³ÀÌ»óÀÇ Á¾¼ÓÀû
//		 that Àý,  °ü å°è ´ë ¸í »ç Àý, ºÎ»çÀý
int TextProc::calc_stc_level(stcInfo *stc)
{
	FuncLogger log("TextProc::calc_stc_level");
	int w_level_cnt[5]; // array-0 == ÃÖ»ó , array-4 == ±âº» ´Ü¾î
	int	stc_complex=0;
	int	i;
	txtToken **ptr = 0;
	txtToken *tok = 0;
	int	level = 0;
	int	comma_cnt=0;
	int	verb_cnt=0;
	//int	stc_length=0;
	int	high_words=0;

	high_words = stc->n_words - stc->n_commons;

		/*
		//2003.3.28
	{
		//2002.11.10, ¹®Àå ±æÀÌ¸¦ ³­ÀÌµµ °Ô»ê¿¡ Ãß°¡
		int a=0, b=0;
		a = STCtoks[0]->offset;
		b = STCtoks[no_STCtoks-1]->offset;
		stc_length = b - a;
	}
		*/
	// ¹®Àå -- ³­ÀÌµµ ±¸ÇÏ±â
	// 1-- ´Ü¾î ³­ÀÌµµ ±¸ÇÏ±â

	memset(w_level_cnt, 0, sizeof(int)*5);

	for(i=0, ptr=STCtoks; i<no_STCtoks; i++, ptr++) {
		tok = *ptr;
		if (tok->w==0) continue;

		level = tok->w->level;
		#ifdef EX_TEST
		PRN("Word L: %s %d\n", tok->w->key, level);
		#endif
		if (level >= 0 && level <= 3)
			w_level_cnt[level]++;
	}
	w_level_cnt[0] += stc->n_no_dict_words;

	#ifdef EX_TEST
	PRN("High-to-Low: Words per Level: %d, %d, %d, %d, %d\n", w_level_cnt[0], w_level_cnt[1],
		w_level_cnt[2], w_level_cnt[3], w_level_cnt[4]);
	#endif

	for(i=1; i<4; i++)
		w_level_cnt[i] += w_level_cnt[i-1];
	w_level_cnt[4] = stc->n_commons;
	#ifdef EX_TEST
	PRN("High-to-Low: Words per Level: %d, %d, %d, %d, %d\n", w_level_cnt[0], w_level_cnt[1],
		w_level_cnt[2], w_level_cnt[3], w_level_cnt[4]);
	#endif

	// 2- ¹®Àå º¹Àâµµ ±¸ÇÏ±â
	for(i=0, ptr=STCtoks; i<no_STCtoks; i++, ptr++) {
		int	PS;
		int	comma_pos=0;
		tok = *ptr;

		// COMMA ´Â º¹Àâµµ¿¡ Æ÷ÇÔÇÏÁö ¾ÊÀ½
		if (tok->tok_realval == ',') {
			// ±ä ¹®Àå Áß°£ ½°Ç¥
			if (i-comma_pos > 3) {
				comma_cnt++;
				comma_pos = i;
			}
		}


		if (tok->w==0) continue;
		PS = tok->w->PS;
		// beµ¿»ç, Á¢¼Ó»ç
		if ( (PS&BE_VERB) || (PS&CONJ))
			verb_cnt++;
		else if (i && (PS&VERB)) {
		// Á¶µ¿»ç + µ¿»ç, ÁÖ¾î + µ¿»ç
			wordType *prev_w;
			int PS2;
			// ******
			prev_w = (*(ptr-1))->w;

			if (prev_w) {
				if (prev_w==not_word)
					verb_cnt++;
				else {
					PS2 = (*(ptr-1))->w->PS;
					if ( (PS2&AUX_VERB) || (PS2&NOUN) || (PS2&PRON) )
						verb_cnt++;
				}
			}
		}
	}// for (¹®Àå º¹Àâµµ )

	#ifdef EX_TEST
	PRN("Stc Complex: Commad-%d  Verb-%d\n", comma_cnt, verb_cnt);
	#endif
	stc_complex = comma_cnt + verb_cnt;

	#ifdef BUG
	//PRN("High-to-Low: %d, %d, %d, %d, %d\n", w_level_cnt[0], w_level_cnt[1],
	//	w_level_cnt[2], w_level_cnt[3], w_level_cnt[4]);
	#endif

	if (w_level_cnt[1] > 1)
	{ // ´ë ÇÐ + ÀÏ¹Ý
		level = 5;
		if (stc_complex > 3) level++;
		if (stc_complex > 6) level++;
		if (w_level_cnt[0] > 3) level++;
		if (w_level_cnt[0] > 6) level++;
		if (stc_complex > 8 || w_level_cnt[0] > 8) level++;
		if ( high_words < (level<<1) ) level--;
		//if (stc_complex < 3) level--;
	}
	else if (w_level_cnt[2] > 1)
	{ // °íµî + ´ëÇÐ
		level = 2;
		if (stc_complex > 2) level++;
		if (stc_complex > 5) level++;
		if (w_level_cnt[1] > 0) level++;
		//if (stc_complex < 2) level--;
		//if (level<6 && high_words > 8) level++;
		if (high_words > (level*3) ) level++;
		else if ( high_words < (level<<1) ) level--;
	}
	else //
	{ // ÁßÇÐ + °íµî
		level = 0;
		if (w_level_cnt[2] > 0 || stc->n_name_words) level++;
		if (stc_complex > 1) level++;
		if (stc_complex > 3) level++;
		//if (level<3 && high_words > 5) level++;
		if (high_words > (level*3) ) level++;
		else if ( high_words < (level<<1) ) level--;
	}

	if (level==0) level = 1; //ÃÖ¼Ò ·¹º§ == 1
	stc->level = level;
	
	PRN("calc_stc_level(): Stc Level = %d\n", level);
	
	return level;
}

int TextProc::handleHangulToken(oneDoc *Doc, stcInfo *stc, txtToken *tok)
{
	StrStream resStr;
	resStr.init(1024*2);
	HStem stemmer;
	stemmer.stem(tok->tok_str, tok->tok_len );
	stemmer.printStem_stream(resStr);
	resStr.print();

	stemResType *beststem = stemmer.getBestStem();
	if (beststem==NULL)
	{
		HwordType * new_inserted = HDict::addNewHangulWord(tok->tok_str);
		if (new_inserted)
		{
			tok->hw = new_inserted;
			ACE_DEBUG((LM_INFO,"New Hangul word inserted: %s  id=%d\n", tok->tok_str, new_inserted->id ));
			return 1;
		}
		else
		{
			ACE_DEBUG((LM_INFO,"New Hangul word inserted FAIL: %s\n", tok->tok_str));
			return 0;
		}
	}
	else
	{
		PRINTF(("Hangul Stem: %s\n", beststem->stem->key));
		tok->hw = beststem->stem;
		return 1;
	}

}

