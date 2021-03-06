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
STCtoks 내의 모든 token 정보를 출력.
*/
void TextProc::STCtoks_print()
{
	int i;
	for(i=0; i<no_STCtoks; i++)
		txtToken_print2(STCtoks[i]);
}

/**
STCtoks 내의 문장의 실제 문자열 출력
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
 문장 난이도를 계산하고, 예문을 추출하여 문서 정보에 담는다.
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
한 문서에 대해 예문 수집이  끝난 후에 문서 정보 정리. 수집 결과 정리.
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
텍스트를 파싱하며 문장을 추출하고, 문장에서 예문을 추출한다.

중복되는 예문, 단어 등을 처리해야 한다. 홈피 하단에 자주 나타나는 copyright 기호 등도 처리해야 한다.
사전에 없는 단어 처리.
합성어 처리.
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
		// 초기화가 중요하다 **** 이것 때문에 알수 없는  BUG 생겼슴


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


			// 각 라인에 대해 문장 여부를 검사
			if (found_stc_in_a_line)  {
				found_stc_in_a_line = 0;
				in_doubt_line = 0;
			}
			else if (words_in_a_line==0)
			{

				{
					// 빈 라인 == 문장 아님
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
			// 대문자 단어가 많거나, 숫 자 (기 호) 등의 많으면 의심스러운 라인
				in_doubt_line++;
				if (in_doubt_line > 1) {
				// 의심스런 라인이 연속될 경우  == 문장 아님
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
			// 약어인 경우 문장 부호가 아니다.
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
				//대문자로 시작, 길이가 3 이하 --> 약어일 가능성이 높다
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
				// quatation 경우 처리
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



			// 문장인지 검사  // article 이 아닌 경우.
			//  모두 대문자로 된것이 많다.  // 문장부호가 없다.
			//	한 줄에 단어 수가 너무 적다.
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

			//---- 문장 발견... example 구하기 ...
			get_examples();

			found_stc_in_a_line = 1;

		}// if tok==END_PUNCT
		else if ( tok.tok_val == MID_PUNCT)
		{
			if ( (tok.tok_realval==':' || tok.tok_realval==';')
				&& stc.n_words > 15 && stc.n_commons > 4) {
			// 너무 길어지는 걸 막기 위해
			//---- 문장 발견... example 구하기 ...
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
			// '['로 문장이 시작한 경우
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

			if (tok.tok_val == HAN_STR) // 한글 
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
				// dash 숙어가 아닌 경우에만 find_org_word()
				else {
					/*
					if ( (tok.tok_realval & (UP_W|APO_W|DASH_W|DOT_W|WEBSITE_W))==0) {
						// 신규 단어 리스트에 있지는 검사
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
					// 단어 찾은 경우


						// 일반적인 문장이 전부 대문자로 된 경우도 있다
						// 이럴때는 약어 추출 하지 않는다
						if (tok.tok_realval & UP_W) {
							stc.n_up_dict_words++;
							stc.n_odd_dict_words++;
							if (w->PS&W_BASIC)  // 기본
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
					/* 이 단어가 said 인 경우, 그 원본을 찾아간다. */
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
					stc.n_verb_conj++; // 동사,조동사, 접속사
				*/

				/**
				// NOT working good. 2005.7.18
				if (dq.nums==0 && ratio_in_page > 80 && check_copyright_stop(this_tok)) {
					PRN("Stop by Copyright at line %d !!\n", tok.line_num);
					break;
				}
				*/

			}

			// acronym 처리
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
			// 요부분이 필요한가 ??
			// 오류 가능성 높음
			/*
			if (words_in_a_line==1) {
				// 라인이 대문자시작 단어로 시작하는 경우
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
		// 같은 라인 상에 있는 한글 포함
		// 다음 줄까지
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
예문을 추출할지 말지 결정.
2005년 버젼에서는 그냥 모두 예문 추출 하자. 관사는 제외.
return 0: 예문 추출 하지 말기.
*/
int TextProc::probeExample(wordType *w)
{
	if (w->PS&ARTI)
		return 0;
	else
		return 1;

	/*-- 이 아래는 2002년 버젼. --*/
	// 기본 단어들은 생략
	if (w->PS&W_BASIC)
		return 0;


	if (w->PS&(FREQ_W|PREP|AUX_VERB|REL_QUEST))
		return 0;

	//  아주 흔한 단어 (일부 전치사, all, 일부 조동사 등등) --1/8
	if (w->PS&FREQ_W) {
		if (PROB_1_8(rand())) return 0;
	}

	// 전치사, 조동사, 의문사  등 -- 1/4
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
예문 중에 단어 부분만 출력.
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
// example 구하기
//-- 문장 분석 , 품사 분석 ...
// 단어 종류별 (단어하나, 합성어(hyphen), 단어조 합 )로 추출이 다르다.
// 단어하나 : 단어 level, 글 level 연관하여 수준이 낮으면 추출 확률 낮게
// 합성어 : 무조건 추출
// 단어 조합 : 구성 단어의 수준

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
	
	//예문 길이가 0인 경우가 발생했었다
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


	//문장 길이가 극히 짧은 것  (ex) I am sam. -- 9자
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

		if (w->w_type & W_HYPEN) { // hypen 포함 --> 합성어

		}

		/* 단어 예문  */

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
		
		/* 숙어 예문 */
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
// 영어 article를 입력으로 받아
// 영어 문장 복잡도, 단어 난이도 등을 분석하여
// 전체적을 수준을 평가한다.

// 문장 구조 분석 -- 아직 구현 안됌 --- 필수는 아님.
// 단문, 복문, 중문, 혼문 (simple, complex, compound, mixed)
// 복문 : 주절과 1개이상의 종속적
//		 that 절,  관 弱� 대 명 사 절, 부사절
int TextProc::calc_stc_level(stcInfo *stc)
{
	FuncLogger log("TextProc::calc_stc_level");
	int w_level_cnt[5]; // array-0 == 최상 , array-4 == 기본 단어
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
		//2002.11.10, 문장 길이를 난이도 게산에 추가
		int a=0, b=0;
		a = STCtoks[0]->offset;
		b = STCtoks[no_STCtoks-1]->offset;
		stc_length = b - a;
	}
		*/
	// 문장 -- 난이도 구하기
	// 1-- 단어 난이도 구하기

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

	// 2- 문장 복잡도 구하기
	for(i=0, ptr=STCtoks; i<no_STCtoks; i++, ptr++) {
		int	PS;
		int	comma_pos=0;
		tok = *ptr;

		// COMMA 는 복잡도에 포함하지 않음
		if (tok->tok_realval == ',') {
			// 긴 문장 중간 쉼표
			if (i-comma_pos > 3) {
				comma_cnt++;
				comma_pos = i;
			}
		}


		if (tok->w==0) continue;
		PS = tok->w->PS;
		// be동사, 접속사
		if ( (PS&BE_VERB) || (PS&CONJ))
			verb_cnt++;
		else if (i && (PS&VERB)) {
		// 조동사 + 동사, 주어 + 동사
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
	}// for (문장 복잡도 )

	#ifdef EX_TEST
	PRN("Stc Complex: Commad-%d  Verb-%d\n", comma_cnt, verb_cnt);
	#endif
	stc_complex = comma_cnt + verb_cnt;

	#ifdef BUG
	//PRN("High-to-Low: %d, %d, %d, %d, %d\n", w_level_cnt[0], w_level_cnt[1],
	//	w_level_cnt[2], w_level_cnt[3], w_level_cnt[4]);
	#endif

	if (w_level_cnt[1] > 1)
	{ // 대 학 + 일반
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
	{ // 고등 + 대학
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
	{ // 중학 + 고등
		level = 0;
		if (w_level_cnt[2] > 0 || stc->n_name_words) level++;
		if (stc_complex > 1) level++;
		if (stc_complex > 3) level++;
		//if (level<3 && high_words > 5) level++;
		if (high_words > (level*3) ) level++;
		else if ( high_words < (level<<1) ) level--;
	}

	if (level==0) level = 1; //최소 레벨 == 1
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

