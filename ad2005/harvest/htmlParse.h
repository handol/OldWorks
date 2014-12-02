#ifndef HTML_PARSE_H
#define HTML_PARSE_H
#include "htmlLex.h"
#include "htmlTagEntity.h"

#define	TAG_DEPTH	(2000)

#define HTMLTITLE_LEN (127)

typedef struct _tagStackType
{
	stTagProc	*tag; // HTML tag
	int	start_pos; // HTML 에서 변환되어 만들어지는  text 에서 현재 tag 내부의 텍스트 시작 위치.
	int	len_txt; // 현재 tag 내부의 텍스트 길이.
	int	depth;
} tagStackType;

class htmlParse 
{
	public:
		
	htmlParse() {
		stack_top = 0;
		html = text = 0;
		do_prn_hangul = 0;
	}
	
	int prepare();
	int finish();
	
	int html2text(char *html_mem, int html_size, char *txt_mem, int max_txt_size);
	int html2text_file(char *html_f, char *out_file);
	int html2text(char *html_fname);
	char *title() {
		return htmlTitle;
	}
	
	void set_prn_hangul();
	void unset_prn_hangul();
	
private:

	tagStackType	*tagStack;
	int	stack_top;
	int	max_txtlen;
	int	max_inclen;
	tagStackType *maxdelta_tag;
	
	char	*html; // HTML (source)
	char	*text; // Text (result)
	char	htmlTitle[HTMLTITLE_LEN+1];

	//2002.11.10
	//한글 출력 기능 optional
	int	do_prn_hangul;

	static char tagMarkBegin[10];
	static char tagMarkEnd[10];

	int	alloc_tagstack();
	void	free_tagstack();
	tagStackType	*top_tag();
	int push_tag(stTagProc	*tag_ptr, int depth, int start_pos);
	int end_tag(stTagProc	*tag_ptr,  int end_pos);
	tagStackType	*find_maxlen_tag();
	tagStackType	*find_maxlen_tag2();
	void print_maxlen_tag(char *textbuf);
	
	void prn_tok(stToken *tok);
	int get_hyperlink_title(char *html, char *title, int maxtitlelen);
	
	int skip_to_token(stToken *tok, int dest_tok);
	int skip_to_two_token(stToken *tok, int tok_a, int tok_b);

	int skip_ignore_part(stToken *tok, stTagProc *curr_tag);	

	int is_glossay_mode(char *doc_title);
	int parse(stToken *tok, char *text, int maxlen);
};

#endif

