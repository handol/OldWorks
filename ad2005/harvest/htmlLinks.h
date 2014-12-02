#ifndef HTML_LINKS_H
#define HTML_LINKS_H

/**
@file htmlLink.h
HTML을 분석해서 Link가 추출하는 기능 
*/

#include "htmlLex.h"
#include "htmlTagEntity.h"

enum {
	A_LINK=1,
	FRAME_LINK,
	AREA_LINK	
};

#define	MAX_ATTRNAME (127)
class htmlLinks
{
public:
	htmlLinks(char *html_mem, int html_size) {
		memset(&token, 0, sizeof(token));
		token.src_mem = html_mem;
		token.src_len = html_size;
		token.line_num=1;
		curr_tag = 0;
	}
	
	int procHlink(char *hlinkbuff, int maxbuff);
	int getHyperLink(char *hlinkbuf, int bufsize);

private:
	char	*htmlMem;
	char	*htmlSize;
	stToken	token;
	stTagProc *curr_tag;
	
};
#endif
