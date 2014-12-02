#include "ace/Log_Msg.h"
#ifdef WIN32
#include "windowsDef.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "TravExpr.h"
#include "StrUtil.h"
#include "StrCmp.h"

#define PRN	printf


// return -1: if the range of URL expr is wrong
// return 0: if NO range expr
// return 1: if number range
// return 2: if char range
int TravExpr::getRange(TRVexpr *trv_expr)
{
	char *str, *begin, tmp[10];
	int	n;

	trv_expr->from = trv_expr->to = 0;
	str = strstr(trv_expr->org_url_expr, "$[");
	if (str==NULL) 	return 0;
	trv_expr->head = (str - trv_expr->org_url_expr);
	begin = str;

	str += 2;
	if (isdigit(*str)) {
		n = StrUtil::copy_number(tmp, str, 8);
		str += n;		
		if (str[0]!='.' || str[1]!='.') return -1;
		trv_expr->from = (uint2) strtol(tmp, 0, 10);
		
		if (! isdigit(str[2])) return -1;
		str += 2;
		n = StrUtil::copy_number(tmp, str, 8);
		str += n;		
		if (str[0]!=']') return -1;
		trv_expr->to = (uint2) strtol(tmp, 0, 10);
		trv_expr->tail = (str - trv_expr->org_url_expr) + 1;
		return INT_LOOP;
	}
	else if (isalpha(*str)) {
		if (str[1]!='.' || str[2]!='.' || !isalpha(str[3]) || str[4]!=']') return -1;
		trv_expr->from = str[0];
		trv_expr->to= str[3];
		trv_expr->tail = (str - trv_expr->org_url_expr) + 5;
		return CHAR_LOOP;
	}
	return 0;
}

// U=username P=passwd D=depth F=file I=dir1,dir2 X=dir3,dir4
// NO domain spanning, NO host spanning
int TravExpr::parseArgs(TRVexpr *trv_expr, int argc, char *argv[])
{
	int	i=0;
	int	cnt_dirs;
	
	char	url_tmp[256];

	memset(trv_expr, 0, sizeof(TRVexpr));
	trv_expr->expr_all = (char *)mymem.alloc(MAX_TRAVEXPR_LEN+1);

	trv_expr->expr_len = StrUtil::argv_to_line(trv_expr->expr_all, MAX_TRAVEXPR_LEN, argc, argv);
	//ACE_DEBUG((LM_DEBUG, "TRAV EXPR: %s\n", trv_expr->expr_all));

	trv_expr->url_type=1;
	if (strncasecmp(argv[0], "file:", 5)==0) {
		char *root_ptr=0;
		trv_expr->url_type = 2;
		root_ptr = argv[0]+5;
		while(*root_ptr=='/') root_ptr++;
		
		if (*root_ptr!='.') // 상대 path가 아니면 
			root_ptr--; // "/"로 시작해야 하므로 (절대 path) 
		trv_expr->org_url_expr = mymem.strAlloc(root_ptr);
		
	}
	
	else if (strncmp(argv[0], "http://",7)==0) {
		trv_expr->org_url_expr = mymem.strAlloc(argv[0]);
	}
	else {
		sprintf(url_tmp, "http://%s", argv[0]);
		trv_expr->org_url_expr = mymem.strAlloc(url_tmp);
	}

	rootURLparse.parse( trv_expr->org_url_expr );
	
	trv_expr->loop_type = getRange(trv_expr);
	if (trv_expr->loop_type < 0) return -1;
	
		
	for(i=1; i < argc; i++) 
	{
		if (strncasecmp(argv[i], "-LINK", 4)==0) {
			trv_expr->link_prn_only = 1;
		}
		else if (strncasecmp(argv[i], "-TITLE", 6)==0) {
			i++;
			trv_expr->txt_title = mymem.strAlloc(argv[i]);
			StrUtil::replaceChar(trv_expr->txt_title, (int)'_', (int)' ');
		}

		else if (argv[i][1] == 'H') { // host spans
			trv_expr->host_span = 1;
		}
		else if (argv[i][1] == 'C' && argc > i+1 ) { // category ID
			i++;
			//trv_expr->ctg_id = (uint2) strtol(argv[i], NULL, 16);  //-- 16 진수
			trv_expr->ctg_id = (uint2) strtol(argv[i], NULL, 10); //-- 10 진수
			
		}
		else if (argv[i][1] == 'T' && argc > i+1 ) { // Doc Type
			i++;
			trv_expr->doctype_id = (uint2) strtol(argv[i], NULL, 16);
		}
		
		else if (argv[i][1] == 'M' && argc > i+1 ) { // max number of links
			i++;
			trv_expr->max_links = (uint2) strtol(argv[i], NULL, 10);
		}
		else if (argv[i][1] == 'm' && argc > i+1 ) { // max number of links
			i++;
			trv_expr->links_per_page = (uint2) strtol(argv[i], NULL, 10);
		}
		else if (argv[i][1] == 'D' && argc > i+1 ) { // depth
			i++;
			trv_expr->depth = (uint2) strtol(argv[i], NULL, 10);
		}
		else if (argv[i][1] == 'U' && argc > i+1 ) { // user
			i++;
			trv_expr->user = mymem.strAlloc(argv[i]);
		}
		else if (argv[i][1] == 'P' && argc > i+1 ) { // passwd
			i++;
			trv_expr->passwd = mymem.strAlloc(argv[i]);
		}
		else if (argv[i][1] == 'F' && argc > i+1 ) { // file
			i++;
			trv_expr->filename = mymem.strAlloc(argv[i]);
		}
		else if (argv[i][1] == 'K' && argc > i+1 ) { // include dir with Keyword
			i++;
			cnt_dirs = StrUtil::countChar(argv[i], ',')+1;
			trv_expr->key_dirs.alloc(cnt_dirs, MAX_DIR_LEN, mymem);
			trv_expr->key_dirs.split(argv[i], ',');

		}
		else if (argv[i][1] == 'I' && argc > i+1 ) { // include dir
			i++;
			cnt_dirs = StrUtil::countChar(argv[i], ',')+1;
			trv_expr->inc_dirs.alloc(cnt_dirs, MAX_DIR_LEN, mymem);
			trv_expr->inc_dirs.split(argv[i], ',');

		}
		
		else if (argv[i][1] == 'X' && argc > i+1 ) { // exclude dir
			i++;
			cnt_dirs = StrUtil::countChar(argv[i], ',')+1;
			trv_expr->exc_dirs.alloc(cnt_dirs, MAX_DIR_LEN, mymem);
			trv_expr->exc_dirs.split(argv[i], ',');
			
		}

	}


	return 0;
}

void TravExpr::free(TRVexpr *trv_expr)
{
	
}

void TravExpr::print(TRVexpr *u)
{

	PRN("URL EXPR: %s\n", u->org_url_expr);
	//if ( u->ctg_name ) PRN("\tCategory= %s\n", u->ctg_name);
	//if ( u->dtype_name ) PRN("\tDocType= %s\n", u->dtype_name);
	PRN("\tCTG=%X, DocType=%X\n", u->ctg_id, u->doctype_id);
	PRN("\tDepth= %d\n", u->depth);
	if (u->host_span) PRN("\tAll host\n");	
	if (u->loop_type==INT_LOOP)
		PRN("\t[%d..%d]\n", u->from, u->to);
	else if (u->loop_type==CHAR_LOOP)
		PRN("\t[%c..%c]\n", u->from, u->to);	

	if (u->user)
		PRN("\tUser/Passwd: %s %s\n", u->user, u->passwd);

	u->key_dirs.print("KEY: ");
	u->inc_dirs.print("INC: ");
	u->exc_dirs.print("EXC: ");
	
	if (u->filename)
		PRN("\tFile: %s\n", u->filename);

}


/**
주어진  givendir 이  기준DIR인  basedir 과 같은 DIR 이거나 , sub directory 이면 참.
아니면 거짓.
@return boolean
*/
int	TravExpr::isSubdir(char *givendir, char *basedir)
{
	int	baselen = strlen(basedir);
	if (baselen==0) return 0;
	if (basedir[baselen-1]=='/') baselen --;
	
	if (strncmp(givendir, basedir, baselen)==0 && 
			(givendir[baselen]==0 || givendir[baselen]=='/') )
		return 1;
	return 0;
}


// return TRUE if 'link_url' is included in the given TRV_expr (u)
// 2002.10.1
// exclude dir이 우선순위 1위
// include dir, keyword dir 는 OR relation  (우선순위 2)
// filename:
// root URL의 dir와 INC dir 는 OR 관게 
	/*
	// -- ( 2002.10.2)
	// dir_match_val : root URL의 dir와 link의 dir의  match 검사 결과
	// --> dir_match_val==TRUE 이면 inc_dir에서 MATCH_FALSE 불가 
	*/
/**
테스트 결과.

root가 다음과 같은 경우: "http://www.cnn.com/ASIA -D 1 -I /WORLD,/2005/SPORT"
ASIA를 화일로 인식함.


*/
int TravExpr::match(URLParse &linkurl)
{
	int	i, n;
	int	match=LINK_MATCH_UNDEF;
	char *ptr;
	URLst *hlink = &(linkurl.url);
	URLst *rootURL = &(rootURLparse.url);
	
	// host check
	if (expr.host_span==0 && hlink->host && 
		(strncmp(hlink->host, rootURL->host, rootURL->host_len)!=0)) 
	{
		if (debug) 	PRN("match(): false by host\n");
		return LINK_MATCH_FALSE;
	}

	
	// exclude dir
	for(i=0; i<(const int)expr.exc_dirs.num(); i++) {
		if (isSubdir (hlink->path, expr.exc_dirs.str(i)) ) 
		{
			match = LINK_MATCH_FALSE; 
			
			if (debug) PRN("match(): false by exclude dir\n");
			
			return LINK_MATCH_FALSE;
		}
	}

	//화일 이름에 wild match $* $? :: keyword dir, inc dir에 대해 AND 관게 (relation)
	if (expr.filename) {
		if (StrCmp::URLwildcmp(expr.filename, hlink->file)==0) 
			match = LINK_MATCH_TRUE;
		else {
			match = LINK_MATCH_FALSE;
			
			if (debug) PRN("match(): false by filename\n");
			
			return LINK_MATCH_FALSE;
		}		
	}
	
	// keyword 를 포함하는 dir
	for(i=0; i<(const int)expr.key_dirs.num(); i++) {
		n = strlen(expr.key_dirs.str(i));
		ptr = strstr(expr.key_dirs.str(i), hlink->path);
		// dir 이름중에 keyword 가 나타나면..
		if (ptr!=0 && !isalpha(ptr[n]) &&
			(ptr==hlink->path || !isalpha( *(ptr-1) ) ) ) {
			match = LINK_MATCH_TRUE; break;
		}
		
	}
	

	if (match!=LINK_MATCH_TRUE)
	{
	//---- URL expr의  ROOT URL의 같은 dir이하만 포함 --> 변경
		n = rootURL->dir_len;

		if (n<=1 && rootURL->file_len!=0) {
			// http://news.nationalgeographic.com/iraq.html 같은 경우
			// '/' 이하가 다 포함되는 문제 발생.
			// 그래서 DIR이 '/'인 경우는 URL type이 file 이 아닌 경우만 true 가능 
			match=LINK_MATCH_FALSE;
		}
		else if (n<=1 || linkurl.isSubDir(rootURLparse) )
		{
			match=LINK_MATCH_TRUE;
			return LINK_MATCH_TRUE;
		}
		else match=LINK_MATCH_FALSE;
		
		if (debug) PRN("match(): root path match = %d\n", match);
		
	}

	// include dir

	if (match != LINK_MATCH_TRUE) {
	// keyword dir에서 TRUE이면 여기 검사 필요없이 TRUE
	// root URL과 link.path가 다른 경우 INC_DIR 검사 
		for(i=0; i<(const int)expr.inc_dirs.num(); i++) {
			if (isSubdir (hlink->path, expr.inc_dirs.str(i)) ) 
			{
				match = LINK_MATCH_TRUE; break;
			}
		}
		// include dir에 해당하는 dir만 (Only) (2002.9.1)
		if (expr.inc_dirs.num() && match != LINK_MATCH_TRUE)
			match = LINK_MATCH_FALSE;
	}

	
	return match;
}



/**
순차적으로  root URL 주소값을  받아온다.
*/
int TravExpr::getRootURL(char *urlbuf, int maxbuflen)
{
	urlbuf[0] = 0;
	if (expr.loop_type == 0) {
		strncpy(urlbuf, expr.org_url_expr, maxbuflen);
		urlbuf[maxbuflen] = 0;
	}
	else  {
		char	tmp_num[10];
		int	loop_count;
		int	len;
		loop_count = loop_index + expr.from;
		if (loop_count > expr.to) return 0;
		
		
		strncpy(urlbuf, expr.org_url_expr, expr.head);
		if (expr.loop_type == INT_LOOP)
			sprintf(tmp_num, "%d",loop_index);
		else
			sprintf(tmp_num, "%c",loop_index);

		len = strlen(tmp_num);
		strcpy(urlbuf+expr.head, tmp_num);
		
		if (expr.tail ) {
			strcpy(urlbuf+expr.head+len, expr.org_url_expr+expr.tail);
		}
		loop_index++;
	}
	return 1;
}


