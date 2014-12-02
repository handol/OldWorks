
#ifndef URLTRAV_H
#define URLTRAV_H

#include <string>
#include <list>

#include "basicDef.h"
#include "oneDoc.h"
#include "URLParse.h"
#include "TravExpr.h"
#include "htmlParse.h"
#include "StrStream.h"
#include "Docs.h"
#include "useCurl.h"
#include "StrStream.h"


using namespace std;

#define	MAX_HTMLSIZE	(1024*100)
#define	MAX_TEXTSIZE	(1024*100)
#define	BUF_LEN		(1024*100)
#define	URLTRAV_MAX_DOCS	(200)

class URLTrav
{
public:
	URLTrav() {
		init();
	}

	URLTrav(int expr_argc, char *expr_argv[]) {
		init();
		expr(expr_argc, expr_argv);
	}

	URLTrav(char *expr_str) {
		init();
		expr(expr_str);
	}

	~URLTrav() {
		if (docsList) delete [] docsList;
	}

	void init() {
		currDepth=0;
		linkPerPage=0;
		traversedPages=0;
		myhome = 0;
		docsList = new int [URLTRAV_MAX_DOCS];
		debug = 0;
		reqID = 0;

		countDocs = 0;
		failedCnt = 0;
		skipCnt = 0;
		dupCnt = 0;
	}

	void setDebug (int _debug=0) {
		debug = _debug;
	}
	int expr(int expr_argc, char *expr_argv[]) ;

	int expr(char *expr_str) ;
		
	void	print() {
		travCond.print(&travCond.expr);
	}

	void	printResInfo(StrStream &stream);
	
	int addlink(char *link_url, int link_type);
	int getlinks(char *htmlbuf, int bufsize);
	
	int savetext(docType *doc);
	int savedocinfo(docType *doc);
	
	//int gettext(char *htmlbuf, int bufsize, URLParse &url);
	int procDoc(useCurl &urlfetch, docType *docinfo, StrStream &stream);
	
	int traverse(StrStream &stream, char *homedir=0);

	TravExpr	travCond;
private:
	
	void	initPerPage();
	int	insertQ(char *url, int link_type);
	int	setfname(StrStream	&pathname, URLParse &url);

	htmlParse	h2t;
	
	list < string > urlQ;
	list < int > depthQ;
	
	
	URLParse currURL;
	int currDepth;
	int linkPerPage;
	int traversedPages; 

	
	
	/* ���� ���� ��� ������ ���� */
	int	failedCnt; // ���� ���� ȸ�� (URL ����)
	int	skipCnt; // text ������ �ƴ϶� skip �� ���� ���� 
	int	dupCnt; // URL �ߺ����� ���� ���� ȸ�� 
	
	int	countDocs; // ���� ���� ȸ�� 
	int	*docsList;	
	oneDoc thisDoc;

	char	*myhome;
	char	debug;

	int	reqID;
};
#endif
