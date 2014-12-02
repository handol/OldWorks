#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include "windowsDef.h"
#endif

#include "htmlLinks.h"
#include "htmlLex.h"
#include "StrUtil.h"

#define PRN	printf



/**
Extracting Hyperlinks from a html
process <frame src="../a.html">, <a href="/index.html">

@return 1이상. hyperlink를 구한 경우.
@return 0: hyperlink  없는 경우.
*/

int htmlLinks::procHlink(char *hlinkbuff, int maxbuff)
{
	char *ptr;
	char attr_name[MAX_ATTRNAME+1];
	int	n, ch;
	int	is_attr_href=0;
	int	is_attr_src=0;
	
	ptr =  (char *)token.src_mem + token.offset;
	while(*ptr) {
		ptr = StrUtil::skip_space(ptr);

		n = StrUtil::copy_alphabet(attr_name, ptr, MAX_ATTRNAME);
		if (n==0) break;

		if (strcasecmp(attr_name, "href")==0) 
			is_attr_href=1;
		else {
			is_attr_href=0;
			if (strcasecmp(attr_name, "src")==0)
				is_attr_src=1;
			else
				is_attr_src=0;
		}
				
		ptr += n;		
		ptr = StrUtil::skip_space(ptr);
		if (*ptr=='=') ptr++;
		ptr = StrUtil::skip_space(ptr);
		ch = *ptr;
		if (ch=='"' || ch=='\'') {
			ptr++;
						
			n = StrUtil::copy_to_char(hlinkbuff, ptr, maxbuff, ch);
			if (n==maxbuff) return 1;
			ptr += n;
			if (*ptr==ch) ptr++;
		}
		else {
			
			if (n==maxbuff) return 1;
			
			n = StrUtil::copy_nonsp_stop(hlinkbuff, ptr, maxbuff, '>');
			ptr += n;
		}
		#ifdef BUG
		// PRN("attr_name,hlinkbuff: %s %s\n", attr_name,hlinkbuff);
		#endif
		if (is_attr_src || is_attr_href) {
			if (curr_tag==htmlTagEntity::A_tag)
				return A_LINK;
			else if (curr_tag==htmlTagEntity::FRAME_tag )
				return FRAME_LINK;
			else if (curr_tag==htmlTagEntity::AREA_tag) 
				return AREA_LINK;
			else	
				return 0;
		}
		
	}// while

	return 0;
}
	
/**
HTML 문서에서 hyperlink의 URL 주소를 찾아 하나씩 return 한다.
hlinkaddrbuf 에 URL 주소를 담는다.

이 함수를 사용하는 쪽에서는 loop를 사용하여 getHyperLink()를 계속 호출하여야 한다.

@return tag를 종류를 return (<A>, <FRAME>, <AREA> ). 0을 return하면 종료를 의미.
@return hlinkaddrbuf - hlinkaddrbuf[0] == 0 이면 종료를 의미.
*/
int htmlLinks::getHyperLink(char *hlinkbuf, int bufsize)
{
	uint4		tmp;
	int		link_proc_result=0;
	
	hlinkbuf[0] = 0;
	while(token.offset < token.src_len) {
		tmp = token.offset;
		htmlLex::get_token_mem(&token);
		if (token.tok_val == TOK_EOF) break;
		if (token.tok_val == URL_CMNT) continue;
		if (token.tok_val == BTAG_BEGIN) {
			htmlLex::get_token_mem(&token);
			curr_tag = htmlTagEntity::tag_idx(token.tok_str);
			if (curr_tag==htmlTagEntity::A_tag 
				|| curr_tag==htmlTagEntity::FRAME_tag 
				|| curr_tag==htmlTagEntity::AREA_tag) 
			{
				link_proc_result = procHlink(hlinkbuf, bufsize);
				if (link_proc_result > 0) return link_proc_result;
			}
		}
		if (token.offset==tmp) token.offset++;
	}
		
	return 0;
}


