
#include "ace/Log_Msg.h"

#include "windowsDef.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

#include "StrUtil.h"
#include "StrStream.h"

#include "useCurl.h"
#include "FileUtil.h"
#include "TimeUtil.h"

#include "htmlLinks.h"
#include "htmlParse.h"
#include "TravExpr.h"

#include "URLParse.h"
#include "URLTrav.h"
#include "dbStore.h"

#include "categoryMapper.h"

#define DEB
#define PRN	printf

#if 0
int URLTrav::expr(int expr_argc, char *expr_argv[]) {
	travCond.parse( expr_argc,  expr_argv);
	return 0;
 }
#endif

int URLTrav::expr(int expr_argc, char *expr_argv[])
{
	travCond.parse( expr_argc, expr_argv );
	//if (debug) this->print();
	return 0;

	
}


int URLTrav::expr(char *expr_str) {
	MemSplit	args;
	args.alloc(20, 256);
	args.split(expr_str);
	//if (debug) args.print();
	
	travCond.parse( args.argc(), args.argv() );

	//if (debug) this->print();

	ACE_DEBUG((LM_DEBUG, "[%T] TRAV BEGIN: %s\n", expr_str));
	
	reqID = dbStore::storeDB_req("harvestreq", expr_str);
	//ACE_DEBUG((LM_DEBUG, "[%T] REQ ID = %d\n", reqID));
	return reqID;
 }
 
/**
HTML �� text �� ��ȯ�� ���� ȭ�Ϸ� ����.
*/
int URLTrav::savetext(docType *doc)
{
	FILE *out;
	int	n=0;

	if ( (out=fopen(doc->fpath, "w")) == NULL) return -1;
	fprintf(out, "### %s\n", doc->url);	
	fprintf(out, "### %s\n", doc->title);	
	fprintf(out, "%s", doc->text);
	fclose(out);
	return n;
}

/**
�־��� URL �ּҸ� �������� �ؽ�Ʈ ȭ���� �̸��� �����.
URL�� path�κи� �����ͼ�, '/'���ڸ� '_'�� �ٲٴ� ����� ����Ѵ�.
*/
int	URLTrav::setfname(StrStream	&pathname, URLParse &url)
{
	char *ptr = 0;
	char *sitename = 0;
	int	headch=0;
	char	yymmdd[10];
	char filename[256];
	int	len=0;
	
	
	ptr = strchr(url.url.host, '.');
	if (ptr) headch = toupper(ptr[1]);
	else	headch = '0';
	
	if (strncmp(url.url.host, "www.", 4)==0) {
		sitename = url.url.host + 4;
	}
	else {
		sitename = url.url.host;
	}

	pathname.add( "%s/txt/", myhome);
	FileUtil::check_this_directory(pathname.str() );
	
	pathname.add( "%c/", headch);
	FileUtil::check_this_directory(pathname.str() );

	pathname.add( "%s/", sitename);
	FileUtil::check_this_directory(pathname.str() );

	TimeUtil::get_yymmdd(yymmdd);
	pathname.add( "%s/", yymmdd);
	FileUtil::check_this_directory(pathname.str() );

	
	
	if (url.url.path[0]=='/')
		strcpy(filename, url.url.path+1);
	else
		strcpy(filename, url.url.path);
		
	/* windows ���� ȭ�� �̸��� ����� �� ���� ���� : \ / : * ? " < > |  */
	/* & ���� �߰� */
	StrUtil::replaceChars(filename, "\\/:*?\"<>|&",  '_');
	len = strlen(filename);
	if (url.url.file_len==0)
		pathname.add ("%sindex.txt", filename);
	else
		pathname.add ("%s.txt", filename);

	return 0;
}
	
void	URLTrav::initPerPage()
{
	linkPerPage = 0;
}

int	URLTrav::insertQ(char *url, int link_type)
{
	if (! travCond.linksPageOK(linkPerPage) ) return 0;
	if (! travCond.linksOK(traversedPages) ) return 0;
	
	list<string>::iterator iter;
	for (iter=urlQ.begin(); iter != urlQ.end(); iter++)
	{
		if ( strcmp(url, (*iter).c_str() )==0) return 0;
	}

	urlQ.push_back( url );
	if (link_type == FRAME_LINK)
		depthQ.push_back(currDepth);
	else
		depthQ.push_back( currDepth + 1);

	linkPerPage++;
	traversedPages++;

	PRN("INSERT: %d-%d %d %s\n", 
	  currDepth, linkPerPage, traversedPages, url);

	return 1;
}

/**
return 1: Queue �� ���� ������ ���.
*/
int URLTrav::addlink(char *link_url, int link_type)
{
	int	match_val=LINK_MATCH_UNDEF;
	int	insert_res=0;

	URLParse	currlink; // child
	
	if (strncmp(link_url, "javascript:", 10)==0)
		return 0;
	if (strchr(link_url, '#') )
		return 0;
	currlink.parse( link_url );
	//currlink.print();
	//currURL.print();

	currlink.inherit( currURL ); // currURL - parent

	match_val = travCond.match( currlink);

	if (debug) ACE_DEBUG((LM_DEBUG, "%s\n%s -- match=%d\n", 
		link_url,
		currlink.url.urlstr, match_val ));
	
	if (match_val  == LINK_MATCH_TRUE ) 
	{
		insert_res = insertQ(currlink.url.urlstr, link_type);
		if (insert_res) return 1;
	}
	
	return 0;
}


/**
return : ���ǿ� �´� link �� ����.

HTML���� ������ hlink �� �ϳ��� ���ϰ�,
TRAV expr ���ǿ� �´� ��쿡, Q�� insert.

���� : 

�� HTML ���� hlink ��.
urlQ�� �� hlink ��.
*/
int	URLTrav::getlinks(char *htmlbuf, int bufsize)
{
	htmlLinks	hlinks(htmlbuf, bufsize);
	int	link_type;
	char	hlinkurl[URLPARSE_URLLEN+1];
	int	hlink_count=0;
	int	ret;
	
	while (1) {
		link_type = hlinks.getHyperLink(hlinkurl, URLPARSE_URLLEN);
		if (link_type==0) break;

		ret = addlink(hlinkurl, link_type);
		if (ret) {
			//PRN("[%d] %s\n", hlink_count, hlinkurl);
			hlink_count++;
		}
	}
	return hlink_count;
}

/**
�ϳ��� HTML ������ ó���Ѵ�.

return 1: ������ ó���ϰ�, ���� ���� �� �ؽ�Ʈ�� ������ ���.
return 0: ó�� �� ���� �� ���� ������ ó������ ���� ���.
*/
int URLTrav::procDoc(useCurl &urlfetch, docType *docinfo, StrStream &stream)
{
	char	*textbuf=0;
	StrStream pathname;
	int	res=0;
	int	txtlen=0;
	pathname.init(1024);	

	/*
	UTF-8 ���� �˻��Ͽ�, UTF-8 �̸� EUC-KR �� ��ȯ�Ͽ� ȭ�Ͽ� ����.
	*/
	char	checkCharset[32];
	StrUtil::copy_to_lower(checkCharset, urlfetch.httpType(), sizeof(checkCharset)-1);
	if (strstr(checkCharset, "charset")!=0) {
		if (strstr(checkCharset, "utf-8")!=0)	
		{
		}
	}
	
	/* html2text ���� */		
	textbuf = new char[urlfetch.bodysize()+1];
	if (textbuf == NULL) return 0;
	txtlen = h2t.html2text(urlfetch.body(), urlfetch.bodysize(), textbuf, urlfetch.bodysize());

	/* ���� ���� �� �ؽ�Ʈ ȭ�� ���� */
	docinfo->doc_id = 0;
	docinfo->title = h2t.title();
	docinfo->url = currURL.url.urlstr;
	docinfo->fpath = pathname.str();
	docinfo->text = textbuf;
	docinfo->size = txtlen;

	
	if (travCond.getCategory()==0)
		/** URL�� category ������ ����. */
		docinfo->ctg = categoryMapper::getCategory(docinfo->url);
	else
		/* TravelExpression�� ������ category ������ ����.  */
		docinfo->ctg = travCond.getCategory();
	

	setfname(pathname, currURL);
	ACE_DEBUG((LM_DEBUG, "SAVE: %s\n%s\n", currURL.url.urlstr, pathname.str()));
	
	savetext(docinfo);

	res = dbStore::storeDB_doc(docinfo);
	if (res < 0) ACE_DEBUG((LM_INFO,"Query Failed\n%s\n%s\n", dbStore::errmsg, dbStore::querybuf));
	if (textbuf) delete [] textbuf;		

	if (debug) stream.add("DocID=%d\nCtg=%d\nURL=%s\nFile=%s\nTitle=%s\n", 
		docinfo->doc_id, docinfo->ctg, docinfo->url, docinfo->fpath, docinfo->title );

	return 1;
}


/**
HTML ������ ������ �����ͼ� text �� ��ȯ�Ͽ� �����ϰ�, DB �� ���� ������ �����Ѵ�.
�������� ���� (�ڽ�) ��ũ�� ���� ���鼭 ��� ó���Ѵ�.

@return :  ����� doc�� ��

@param expr_str : ������ �������� �ּ�. �ɼ�.
@param stream : ���� ���� �� ó���� ���� �� ����� ����ϱ� ���� string buff.
@param homedir : ���� ���� ó���� home dir ($ADHOME)
while (Q is empty AND  ���� ���� �Ѱ� �̳�)
{
	���� fetch;
	���� ID ��� �߰�;
}
*/
int URLTrav::traverse(StrStream &stream, char *homedir)
{
	myhome = homedir;	

	/*
	for(int j=0; j<200; j++) {
		stream.add("%d  ", j);
	}
	*/
	
	/* root URL �� Q �� �߰� */
	urlQ.push_front(travCond.urlstr() );
	depthQ.push_front(0); // root dept == 0

	h2t.prepare();	
	
	countDocs = 0;
	for( ;urlQ.size() > 0; urlQ.pop_front(), 	depthQ.pop_front() )
	{
		int	res=0;
		useCurl urlfetch;
		docType	docinfo;

		initPerPage();
		
		/* Q ���� URL �ϳ��� �����´� */		
		char *urlstr = (char *) urlQ.front().c_str();
		currURL.parse ( urlstr );
		currDepth = depthQ.front();

		if (dbStore::checkDB_host (currURL.url.host )==0) {
			res = dbStore::storeDB_host (currURL.url.host );
			
			if (debug) stream.add( "DB res = %d: host\n\n", res);
		}
		
		ACE_DEBUG((LM_DEBUG,"[%T] D=%d URL=%s\n", currDepth, urlstr));


		/* web ���� HTML ���� �������� */
		urlfetch.httpGet( currURL.url.urlstr, MAX_HTMLSIZE);

		ACE_DEBUG((LM_DEBUG,"[%T] HTTP RES: size=%d code=%d Content-Type: %s\n", 
			urlfetch.bodysize(), urlfetch.httpResult(), urlfetch.httpType() ));

		
		
		if (urlfetch.bodysize() <= 0 || urlfetch.httpResult() != 200) {
			failedCnt++;
			stream.add("FETCH failed: code=%d, %s\n", urlfetch.httpResult(), currURL.url.urlstr  );
			if (urlfetch.isRedirect()) {
				char *newurl = urlfetch.location() ;
				if (newurl) {
					stream.add("NEW location: %s\n", newurl );
					ACE_DEBUG((LM_DEBUG,"[%T] HTTP RES: NEW location: %s\n", newurl ));
				}
			}
			continue;
		}

		if (strstr(urlfetch.httpType(), "text")==0) {
			skipCnt++;
			stream.add("NOT HTML %s\n", currURL.url.urlstr );
			ACE_DEBUG((LM_DEBUG,"[%T] NOT HTML %s\n", currURL.url.urlstr ));
			continue;			
		}
		
		/* ������ �ߺ��Ǵ��� Ȯ�� �ʿ�. DB ��ȸ �� */
		res = dbStore::checkDB_doc (currURL.url.urlstr );
		if (res) {
			dupCnt++;
			stream.add( "$URL Dup$ %s\n", currURL.url.urlstr);
			ACE_DEBUG((LM_DEBUG, "[%T] URL Dup. Skip %s\n", currURL.url.urlstr));
		}
		else {
			// ������ ó�� 
			if (procDoc(urlfetch, &docinfo, stream)) {
				stream.add("$DOCID$ %d\n\n", docinfo.doc_id);
				ACE_DEBUG((LM_DEBUG, "[%T] New DOC ID [%d]\n", docinfo.doc_id ));
				docsList[countDocs] = (int) docinfo.doc_id;
				countDocs++;
			}
			else {
				failedCnt++;
			}
		}
		
		
		/*  Traverse -- HTML�������� hlink �ּҸ� �����Ͽ� urlQ�� �߰� */
		if (travCond.depthOK(currDepth)  ) {
			int	cnt = 0;
			cnt = getlinks(urlfetch.body(), urlfetch.bodysize() );
			ACE_DEBUG((LM_DEBUG, "[%T]  %d LINKS  from %s\n",
				cnt, currURL.url.urlstr));
		}
		
	}

	h2t.finish();

	ACE_DEBUG((LM_DEBUG, "[%T] TRAV END: %d docs\n", countDocs));
	
	stream.add("$NUMDOCS$ %d\n", countDocs);
	if (countDocs)  stream.add("$DOCIDS$ ");
	
	for(int i=0; i<countDocs; i++) {
		stream.add("%d ", docsList[i]);
	}
	stream.add("\n\n");
	dbStore::updateDB_req("harvestreq", reqID, "GOOD", countDocs );
	return countDocs;
}

void	URLTrav::printResInfo(StrStream &stream)
{
	stream.add("���� ����: succ=%d, fail=%d, skip=%d, dup=%d\n", countDocs, failedCnt, skipCnt, dupCnt);
}
