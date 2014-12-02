#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "basicDef.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "htmlLex.h"
#include "htmlParse.h"

#define	PRN		printf

char htmlParse::tagMarkBegin[10] = "<['[";
char htmlParse::tagMarkEnd[10] = ">]']";

void htmlParse::prn_tok(stToken *tok)
{
	printf("%d: offset=%5d, val=%3d, len=%2d, real=%3d, ",
  	 tok->line_num, tok->offset, tok->tok_val, tok->tok_len, tok->tok_realval);
	if (tok->tok_val != NUMBER  && tok->tok_len==1) printf("ch=%c\n", tok->tok_val);
	else printf("str=%s\n", tok->tok_str);
}

/** 2005.7 @dahee
*/
int	htmlParse::alloc_tagstack()
{
	max_txtlen = 0;
	max_inclen = 0;
	maxdelta_tag =  0;
	tagStack = new tagStackType [TAG_DEPTH];
	if (tagStack == 0) return -1;
	return 0;
}

void	htmlParse::free_tagstack()
{
	if (tagStack) {
		delete [] tagStack;
		tagStack = 0;
	}
}

tagStackType	*htmlParse::top_tag()
{
	if (stack_top>0)
		return &tagStack[stack_top-1];
	else return NULL;
}

/**
start tag를 push 한다.
<TABLE> xxxxx </TABLE>  에서 <TABLE>  을 만난 경우 호출된다.
*/
int htmlParse::push_tag(stTagProc	*tag_ptr, int depth, int start_pos)
{
	if (stack_top <TAG_DEPTH-1) {
		tagStack[stack_top].tag = tag_ptr;
		tagStack[stack_top].depth = depth;
		tagStack[stack_top].start_pos = start_pos;
		tagStack[stack_top].len_txt = 0;
		stack_top++;
		return stack_top;
	}
	else return -1;
}

/**
기존에 push 된 start tag 를 찾아서 텍스트 길이를 계산하여 저장.
<TABLE> xxxxx </TABLE>  에서 </TABLE>  을 만난 경우 호출된다.
*/
int htmlParse::end_tag(stTagProc	*tag_ptr,  int end_pos)
{
	int	i;
	int	txt_len=0;
	int	delta=0;
	for(i=stack_top-1; i>= 0; i--) {
		if (tagStack[i].tag == tag_ptr && tagStack[i].len_txt == 0) {
			txt_len = end_pos - tagStack[i].start_pos;
			tagStack[i].len_txt =  txt_len;

			if (txt_len > max_txtlen) {
				
				delta = txt_len - max_txtlen;
				max_txtlen = txt_len;
				
				if (delta > max_inclen) {
					max_inclen = delta;
					maxdelta_tag = &tagStack[i];
				}
			}
			else if (tagStack[i].len_txt < 80) {
			/* <TD> xxxxx </TD> 와 같은 경우는  텍스트 길이는 작으면서 자주 출현한다.
			속도 개선을 위해 이런 tag 들은 pop 해 버린다.
			*/
				stack_top--;
			}
			return 0;
		}
	}
	return -1;
}

tagStackType	*htmlParse::find_maxlen_tag()
{
	int	i;
	int	maxlen=0;
	tagStackType *maxtag=0;
	for(i=0; i<stack_top; i++) {
		if (tagStack[i].len_txt > maxlen) {
			maxlen = tagStack[i].len_txt;
			maxtag = &tagStack[i];
		}
	}
	
	return maxtag;
}

tagStackType	*htmlParse::find_maxlen_tag2()
{
	int	i;
	int	prevlen=0;
	int	maxlen=0;
	tagStackType *maxtag=0;
	for(i=stack_top-1; i>=0; i--) {
		if (tagStack[i].len_txt > maxlen) {
			maxlen = tagStack[i].len_txt;
			maxtag = &tagStack[i];
		}
	}
	
	return maxtag;
}

void htmlParse::print_maxlen_tag(char *textbuf)
{
	char tmpch=0;
	//tagStackType *maxtag = find_maxlen_tag();
	tagStackType *maxtag = maxdelta_tag;
	PRN("tagStack: stack_top = %d\n", stack_top);
	if (maxtag)
	PRN("tagStack: tag =%s, start=%d, leng=%d\n", 
		maxtag->tag->name, maxtag->start_pos, maxtag->len_txt);

	tmpch = textbuf[maxtag->start_pos + maxtag->len_txt];
	textbuf[maxtag->start_pos + maxtag->len_txt] = 0;
	FileUtil::save_file_2_from_mem(textbuf + maxtag->start_pos, "D:/SVN/ad2005/trimmed.txt");
	//PRN("%s\n", textbuf + maxtag->start_pos);
	textbuf[maxtag->start_pos + maxtag->len_txt] = tmpch;
}

// called by only proc_hyperlink()
// get hyperlink title:  <a href="/aaa/t.html"> good link </a>, "good link" is the title
// html: HTML source, title: save the result(==title)
// 타이틀 길이 제한 
int htmlParse::get_hyperlink_title(char *html, char *title, int maxtitlelen)
{
	char	*ptr=html, *start=title;
	int	n=0;
	#ifdef DEB
	PRN("get_hyperlink_title(): html=%X, title=%X\n", html, title);
	#endif
	if (html==0|| title==0) return 0;
	
	*title=0;
	while (*ptr && *ptr!='>')
		ptr++;
	if (*ptr) ptr++;
	ptr = StrUtil::skip_space(ptr);
	while (*ptr && *ptr!='<' && n < maxtitlelen-2) {
		*title++ = *ptr++;
		n++;
	}
	#ifdef DEB
	PRN("get_hyperlink_title(): n=%d, ptr=%X\n", n, ptr);
	#endif
	while (n>0 && ISSPACE(*(title-1)) ) {
		title--;
		n--;
	}
	#ifdef DEB
	PRN("get_hyperlink_title(): n=%d, ptr=%X\n", n, ptr);
	PRN("get_hyperlink_title(): html=%X, title=%X\n", html, title);
	#endif
	
	*title=0;
	return n;
}



// read out tokens till we meet 'dest_tok' token
int htmlParse::skip_to_token(stToken *tok, int dest_tok)
{
	int	n=0;
	int	prev_tok_val=0;
	stTagProc *this_tag;
	while(htmlLex::get_token_mem(tok) != TOK_EOF) {
		if (tok->tok_val == STRING) {
			this_tag = htmlTagEntity::tag_idx(tok->tok_str);
		}
		if (tok->tok_val == dest_tok) break;
		n++;
		prev_tok_val = tok->tok_val;
	}
	return n;
}


// read out tokens till we meet 'dest_tok' token
int htmlParse::skip_to_two_token(stToken *tok, int tok_a, int tok_b)
{
	int	n=0;
	int	prev_tok_val=0;
	stTagProc *this_tag;
	while(htmlLex::get_token_mem(tok) != TOK_EOF) {
		if (tok->tok_val == STRING) {
			this_tag = htmlTagEntity::tag_idx(tok->tok_str);
		}
		if (tok->tok_val == tok_a || tok->tok_val == tok_b) break;
		n++;
		prev_tok_val = tok->tok_val;
	}
	return n;
}




int htmlParse::skip_ignore_part(stToken *tok, stTagProc *curr_tag)
{
	stTagProc *this_tag;
	int	prev_tok_val=0;
	int	tmp=0;
	//fprintf(log,"--->IGNORE :%d :%d\n", tok->line_num, tok->offset);
	tmp = tok->line_num;
	while(htmlLex::get_token_mem(tok) != TOK_EOF) {
		if (tok->tok_val == CMNT_BEGIN) htmlLex::skip_to_cmnt_end(tok);
		else if (tok->tok_val == BTAG_BEGIN && curr_tag->pair==Opt_PAIR) {
			tok->offset --;
			break;
		}
		else if (tok->tok_val == ETAG_BEGIN) {
			if (curr_tag->pair==Must_PAIR) {
				skip_to_two_token(tok, STRING, TAG_END);
				if (tok->tok_val == STRING) {
					this_tag = htmlTagEntity::tag_idx(tok->tok_str);
					//fprintf(log,"%s<--:%d :%d\n", tok->tok_str, tok->line_num, tok->offset );
					
					if (curr_tag==this_tag) {
						skip_to_token(tok, TAG_END);
						break;
					}
				}		
				
			}
			else { // Pot_PAIR
				skip_to_token(tok, TAG_END);
				break;
			}
		}
		else if (tok->tok_val == STRING) {
			this_tag = htmlTagEntity::tag_idx(tok->tok_str);
		}

		prev_tok_val = tok->tok_val;
	} //	
	//fprintf(log,"<--- IGNORE :%d :%d [%d]\n", tok->line_num, tok->offset, tok->line_num - tmp);
	return 0;	
}


	
void htmlParse::set_prn_hangul()
{
	do_prn_hangul=1;
}

void htmlParse::unset_prn_hangul()
{
	do_prn_hangul=0;
}

int htmlParse::is_glossay_mode(char *doc_title)
{
	if (strstr(doc_title, "Glossar") ) return 1;
	if (strstr(doc_title, "Dictionar") ) return 1;
	if (strstr(doc_title, "Abbreviat") ) return 1;
	if (strstr(doc_title, "Abbr.") ) return 1;
	//if (strstr(doc_title, "Terms") && strstr(doc_title, "of Use")==0) return 1; // Terms of Use

	/*
	if (strstr(doc_title, "glossar") ) return 1;
	if (strstr(doc_title, "abbreviation") ) return 1;
	if (strstr(doc_title, "abbr.") ) return 1;
	if (strstr(doc_title, "terms") ) return 1;
	*/
	return 0;
}


/**
HTML 문서를 parsing 하면서 text로 변환.
*/
int htmlParse::parse(stToken *tok, char *text, int maxlen)
{
	int	tok_val=0, prev_tok_val=0, prev_ch=0;
	int	is_in_tag=0;
	uint4	tmp=0;
	int	not_moved=0;
	char *start=text;

	stTagProc *curr_tag=0;
	//stTagProc **prev_tag=0;
	stEntityProc *ent_proc;
	int	tag_depth=0;
	int	is_in_BODY_tag=0;
	int	is_in_A_tag=0;
	int	is_in_PRE_tag=0; // 2003.11.8
	int	hlink_chars = 0;
	//int	newline_delayed=0; // Optional Pair && Newline tag가 시작한 경우 TRUE
	//int	dont_newline=0; // TRUE means "dont write newline"
	int	pair_mark=0;
	int	words_in_line=0;
	int	b_tag_first_in_line=0; // 라인 첫머리에 B tag가 나온 경우 
	int	glossary_marked=0;
	int	is_glossary=0;

	tok->offset = 0;
	tok->line_num=1;
	*text = 0;

	maxlen -= 20;
	
	while(tok->offset <  tok->src_len && (int)((uint4)text-(uint4)start) < maxlen) {
	
		tmp = tok->offset;

		htmlLex::get_token_mem(tok);

		#ifdef BUG
		if (tok->tok_len>=20) {
			printf("long tok: line=%d, tok_val=%d, CH=%c\n",
			tok->line_num, tok->tok_val, tok->src_mem[tok->offset-1]);
			fflush(stdout);
			printf("tok=%s\n", tok->tok_str);
			fflush(stdout);
		}
		#endif
		
		#ifdef DEB
		/*
		if (tok->tok_val==0 || tok->line_num==1) {
			printf("line=%d, tok_val=%d, CH=%c\n",
			tok->line_num, tok->tok_val, tok->src_mem[tok->offset-1]);
			printf("offset=%d, text-start=%d\n", tok->offset, text-start);
		}
		*/
		#endif
		
		if (text > start) {
			prev_ch = *(text-1);
			if (prev_ch=='\n') {
				words_in_line = 0;
				b_tag_first_in_line = 0;
				glossary_marked = 0;
			}
		}
		
		if (tok->tok_val == TOK_EOF) break;

		if (tok->tok_val == URL_CMNT) {
			//printf("%s", tok->tok_str);
			continue;
		}
		
		if (tok->offset==tmp) {
			#ifdef BUG
			//printf("parse(): offset not moved: line=%d, CH=%d(%c)\n",
			//	tok->line_num, tok->src_mem[tok->offset-1], tok->src_mem[tok->offset-1]);
			#endif
			tok->offset++;
			if (++not_moved >= 2) break;
			else continue;
		}
		tok_val = tok->tok_val;

	
		if (tok_val == BTAG_BEGIN) {	
			skip_to_two_token(tok, STRING, TAG_END);
			if (tok->tok_val != STRING) { // must be a HTML tag
				skip_to_token(tok, TAG_END);
				continue;			
			}
			
			//prn_tabs();
			//fprintf(log,"%s-->:%d :%d\n", tok->tok_str, tok->line_num, tok->offset);
			
			curr_tag = htmlTagEntity::tag_idx(tok->tok_str);
			
			if (curr_tag == NULL) {
				skip_to_token(tok, TAG_END);
				continue;
			}

			tag_depth++;

			#ifdef MAIN_TEXT_ONLY
			if (curr_tag->pair != Not_PAIR && is_in_BODY_tag && curr_tag!=htmlTagEntity::A_tag)
			push_tag(curr_tag, tag_depth, (int)((uint4)text-(uint4)start));
			

			if (curr_tag==htmlTagEntity::BODY_tag) {
				is_in_BODY_tag = 1;
			}
			#endif
			
			if (curr_tag==htmlTagEntity::A_tag) {
				is_in_A_tag = 1;
			}
			else if (curr_tag==htmlTagEntity::PRE_tag) {
				is_in_PRE_tag = 1;
			}
			else if(curr_tag==htmlTagEntity::B_tag||curr_tag==htmlTagEntity::DT_tag) {
				if (words_in_line==0)
					b_tag_first_in_line = 1;
			}
			else if (curr_tag==htmlTagEntity::TITLE_tag) {							
				get_hyperlink_title(tok->src_mem + tok->offset  , htmlTitle, HTMLTITLE_LEN);
					
				#ifdef DEB
				PRN("tok->scr_mem=%X, offset=%d, title=%X\n", 
					tok->src_mem, tok->offset, htmlTitle);
				PRN("title: %s (%d)\n", htmlTitle, strlen(htmlTitle) );
				#endif
				
				is_glossary =  is_glossay_mode(htmlTitle);
		
			}						
			else if (curr_tag==htmlTagEntity::BR_tag) {
				*text++ = '\n';
			}
	
			if (curr_tag->proc == 0) { // 0
				if (curr_tag->pair != Not_PAIR)
					skip_ignore_part(tok, curr_tag);
				else
					skip_to_token(tok, TAG_END);
				if (curr_tag->newline) {
					if (prev_ch != '\n') *text++ = '\n';
				}				
				continue;
			}

			if (curr_tag==htmlTagEntity::TITLE_tag) {
				if (prev_ch != '\n') *text++ = '\n';
				*text++ = '[';
				*text++ = '[';
			}
			else if (curr_tag->proc == 1) {
			}
			else if (!pair_mark && curr_tag->proc == 2) {
				pair_mark = 2;
				*text++ = '<';
			}
			else if (!pair_mark && curr_tag->proc == 3) {
				pair_mark = 3;
				*text++ = '\'';
			}
			else if (!pair_mark && curr_tag->proc == 4) {
				pair_mark = 4;
				*text++ = '[';
			}
			else if (curr_tag->proc == 5) {
				if (prev_ch != '\n') *text++ = '\n';
				*text++ = '*';
				*text++ = ' ';
				words_in_line = 0;
			}
			else if (curr_tag->proc == 6) {
				if (prev_ch != '\n') *text++ = '\n';
				*text++ = '\n';
			}
			
				
			skip_to_token(tok, TAG_END);

			if (!pair_mark && prev_ch != ' ') *text++ = ' ';
			//if (!ISSPACE(*text)) *text++ = ' ';

		}// BTAG_BEGIN '<'
				
		else if (tok_val == ETAG_BEGIN) {
					
			skip_to_two_token(tok, STRING, TAG_END);
			if (tok->tok_val != STRING) { // must be a HTML tag
				skip_to_token(tok, TAG_END);
				continue;
			}
			
		
			curr_tag = htmlTagEntity::tag_idx(tok->tok_str);

			if (curr_tag == NULL) {
				skip_to_token(tok, TAG_END);
				continue;
			}

			#ifdef MAIN_TEXT_ONLY
			tag_depth--;

			if (curr_tag==htmlTagEntity::BODY_tag) {
				is_in_BODY_tag = 0;
			}
			
			if (curr_tag->pair != Not_PAIR && is_in_BODY_tag && curr_tag!=htmlTagEntity::A_tag)
			end_tag(curr_tag, (int)((uint4)text-(uint4)start));
			#endif
			
			if (curr_tag==htmlTagEntity::A_tag) {
				is_in_A_tag = 0;
				#ifdef MARK_A_LINK
				*text++ = '}';
				#endif
			}
			else if (curr_tag==htmlTagEntity::PRE_tag) {
				is_in_PRE_tag = 0;
			}
			else if(curr_tag==htmlTagEntity::DT_tag || 
				(curr_tag==htmlTagEntity::B_tag && (b_tag_first_in_line && is_glossary)))
			{
				if (glossary_marked==0) {
					*text++ = ' ';
					*text++ = ':';
					//*text++ = ' ';
					b_tag_first_in_line = 0;
					glossary_marked = 1;
				}
					
			}
			else if (curr_tag==htmlTagEntity::TITLE_tag) {
				*text++ = ']';
				*text++ = ']';
				*text++ = '\n';
			}
			else if (curr_tag->proc == 1) {
			}
			else if (pair_mark==2 && curr_tag->proc == 2) {
				pair_mark = 0;
				*text++ = '>';
			}
			else if (pair_mark==3 && curr_tag->proc == 3) {
				pair_mark = 0;
				*text++ = '\'';
			}
			else if (pair_mark==4 && curr_tag->proc == 4) {
				pair_mark = 0;
				*text++ = ']';
			}
			else if (curr_tag->proc == 5) {				
			}
			else if (curr_tag->proc == 6) {
				if (prev_ch != '\n') *text++ = '\n';
				*text++ = '\n';
			}

			if (curr_tag->newline) {
				*text++ = '\n';
			}			
		
	
			skip_to_token(tok, TAG_END);

		}// ETAG_BEGIN '</'
		
		else if (tok_val == STAG_END) {
			skip_to_token(tok, TAG_END);
			is_in_tag = 0;
			tag_depth--;
		}
		
		else if (tok_val == TAG_EXC) {			
			tmp = skip_to_token(tok, TAG_END);
			//fprintf(log,"<- > skipped %d\n", tmp);
		}
		
		else if (tok_val == CMNT_BEGIN) {
			//fprintf(log,"Cmnt -->: %d :%d\n", tok->line_num, tok->offset);
			htmlLex::skip_to_cmnt_end(tok);
			//fprintf(log,"Cmnt <--: %d :%d\n", tok->line_num, tok->offset);
		}		
		else if (tok_val == ENTITY_STR) {
			if (prev_ch !=' ') *text++ = ' '; //2002.12.2
			
			ent_proc = htmlTagEntity::entity_idx(tok->tok_str);

			if (ent_proc && ent_proc->conv[0]) {
				#ifdef BUG
				//prn_ent_proc(ent_proc);
				#endif
				strcpy(text, ent_proc->conv);
				text += strlen(ent_proc->conv);
			}
			else {
				//fprintf(log,"ignored entity = %s\n", tok->tok_str);
			}
		
		}
		else if (tok_val == ENTITY_NUM) {
			if (prev_ch !=' ') *text++ = ' '; //2002.12.2
			
			ent_proc = htmlTagEntity::entity_id_idx(tok->tok_realval);

			if (ent_proc && ent_proc->conv[0]) {
				#ifdef BUG
				//prn_ent_proc(ent_proc);
				#endif
				strcpy(text, ent_proc->conv);
				text += strlen(ent_proc->conv);
			}
			else {			
				if (tok->tok_realval < 0x80) {
				// 2005.7.19
					*text = (char)tok->tok_realval ;
					text++;
				}

			}

		}
		else {
			char *t = text;
			if (is_in_PRE_tag || tok->tok_len>1 || tok_val == STRING || tok_val == NUMBER) {
				//if (prev_tok_val == TAG_END && *text != ' ') *text++ = ' ';
				// 2003. 3.20
				//if (prev_tok_val == TAG_END && isalnum(prev_ch) ) 
				//	*text++ = ' ';
				words_in_line++;
				strcpy(text, tok->tok_str);
				text += tok->tok_len;
			}
			// 2002.10.16 한글은 space로 전환 
			else if (tok->tok_val == FR_STR) {
				if (do_prn_hangul) {
					if (prev_ch != ' ') *text++ = ' ';
					strcpy(text, tok->tok_str);
					text += tok->tok_len;
					words_in_line++;
				}
				else {
					if (prev_ch != ' ') *text++ = ' ';
				}
			}
			else if (tok->tok_len==1) {		
				
				if (curr_tag==htmlTagEntity::PRE_tag) {					
					#ifdef ODD_CHAR
					if ( (tok_val & 0xF0) != 0x90)
						*text++ = tok_val;
					else if (tok_val==0x92) {
						if (prev_tok_val != 0x92)
							*text++ = '\'';
					}
					#else
					*text++ = tok_val;
					#endif
				}
				//else if (tok_val == '\n' || tok_val == '\r') {
				else if (ISSPACE(tok_val)) {
					if (prev_ch != ' ') *text++ = ' ';
				}
								
				else {
					if (prev_ch==':' && tok_val==':') { }
					else *text++ = tok_val;
				}
				
			}
			if (is_in_A_tag)
				hlink_chars += (int)(text - t);
		}

		prev_tok_val = tok->tok_val;
		
	}// while(1)

	*text = 0;

	if ((int)(text-start) >= maxlen-5) {
		PRN("parse(): too far !! maxlen=%d, %d\n", maxlen, text-start);
	}
	if ((int)(text-start) < maxlen-5)
		memset(text, 0, 4);
	#ifdef DEB
		PRN("text=%X, start=%X, text=%d, start=%d\n", 
			text, start, (int)text % 10000, (int)start % 10000);
		PRN("start[0]=%d text[0]=%d\n", start[0], text[0]);
	#endif

	return ((int)text-(int)start);
}



// return -1 if error
int htmlParse::prepare()
{
	htmlTagEntity::init();
	#ifdef MAIN_TEXT_ONLY
	alloc_tagstack();
	#endif
	
	return 0;
	
}

int htmlParse::finish()
{
	if (html) {delete [] html; html=0;}
	if (text) {delete [] text; text=0;}
	
	#ifdef MAIN_TEXT_ONLY
	free_tagstack();
	#endif
	
	return 0;
}

/**
HTML 내용을 text 형식으로 변환.
return : text 길이.
*/
int htmlParse::html2text(char *html_mem, int html_size, char *txt_mem, int max_txt_size)
{
	stToken	tok;
	int	tmp_txt_n=0, txt_n=0;
	char	*	tmp_txt=0;

	htmlTitle[0] = 0;
	
	memset(&tok, 0, sizeof(tok));
	tok.src_mem = html_mem;
	tok.src_len = html_size;

	
	txt_mem[0] = 0;
	
	if ( (tmp_txt = new char [max_txt_size+1]) == NULL) {
		PRN("memory fail\n");
		return 0;
	}
		
	tmp_txt_n = parse(&tok, tmp_txt, max_txt_size);
	#ifdef MAIN_TEXT_ONLY
	print_maxlen_tag(tmp_txt);
	#endif
	
	#ifdef DEB
		PRN("html --> tmp_txt: %d --> %d\n", 
			html_size, tmp_txt_n);
		PRN("tmp_txt strlen=%d, max_txt_size=%d\n", strlen(tmp_txt), max_txt_size );
		//printf("%s", tmp_txt);
	#endif

	txt_n = StrUtil::trim_text(txt_mem, tmp_txt, max_txt_size);
	#ifdef DEB
		PRN("trim text:  %d --> %d\n", 
			tmp_txt_n, txt_n);
		PRN("txt_mem strlen=%d\n", strlen(txt_mem) );
	#endif

	delete [] tmp_txt;
	
	return txt_n;

}


int htmlParse::html2text_file(char *html_f, char *out_file)
{
	FILE	*out;	

	int		html_size=0, txt_size;

	if ( (out = fopen(out_file, "w")) == NULL)  {		
		out = stdout;		
	}

	if (html==0 && (html = new char[HTML_SIZE+1]) == NULL) {
		printf("memory fail\n");
		return -1;
	}

	if (text==0 && (text = new char[TEXT_SIZE+1]) == NULL) {
		printf("memory fail\n");
		return -1;
	}

	set_prn_hangul();
	if (prepare() < 0) return -1;
	
	html_size = FileUtil::load_file_2_given_mem(html, html_f, HTML_SIZE);

	txt_size = html2text(html, html_size, text,  TEXT_SIZE);
	
	fprintf(out, "### %s\n", html_f);
	fprintf(out, "### %d --> %d\n", html_size, txt_size);
	fprintf(out, "%s", text);
	fclose(out);
	finish();
	return 0;
}



int htmlParse::html2text(char *html_fname)
{	
	char out_file[256];	
	
	StrUtil::change_ext(out_file, html_fname, "txt");
	html2text_file(html_fname, out_file);
	return 0;
}

