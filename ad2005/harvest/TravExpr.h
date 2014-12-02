#ifndef TRAVEXPR_H
#define TRAVEXPR_H
#include "basicDef.h"
#include "URLParse.h"
#include "MemSplit.h"

#define	MAX_TRAVEXPR_LEN (511)
#define	MAX_DIR_LEN (63)

#define	INT_LOOP	(1)
#define	CHAR_LOOP	(2)


#define	LINK_MATCH_TRUE (1)
#define	LINK_MATCH_FALSE (0)
#define	LINK_MATCH_UNDEF (2) // implicitly TRUE, potentially TRUE

typedef struct _trv_expr {
	char	*expr_all; // orginal Trv Expr: 여러 args 합친 것 
	uint2	expr_len;
	
	


	char	*org_url_expr; // args 중에 URL 부분 원본 
	char	*txt_org_url; //2002.10.27; text 화일에서 바로 구하기 
	char	*txt_title; //
	short	loop_type; // 0 - NO loop, 1-int loop, 2-char loop
	uint2	from, to; // range $[1..10]
	uint2	head, tail;
	int		ctg_id;
	int		doctype_id;
	
	char	*subject;
	char	*user;
	char	*passwd;	
	char	*filename;
	MemSplit	inc_dirs;
	MemSplit exc_dirs;
	MemSplit key_dirs;
	uint2	depth; // default==0, root 페이지만 처리 
	uint2	host_span;
	uint2	links_per_page; // page당 최대 link 수 
	uint2	max_links; // if 0, NO limit
	byte	link_prn_only;
	byte	url_type;
} TRVexpr;

class TravExpr {
public:
	TravExpr() {
		debug = 0;
		loop_index = 0;
	}

	TravExpr(int argc, char *argv[]) {
		debug = 0;
		loop_index = 0;
		parseArgs(&expr, argc, argv);
	}
	
	~TravExpr() {
	}

	void	setDebug() { 
		debug = 1; 
	}
	
	char *urlstr() {
		return expr.org_url_expr;
	}
	int parse(int argc, char *argv[]) {
		parseArgs(&expr, argc, argv);
		return 0;
	}
	int isSubdir(char *givendir, char *basedir);
	int getRange(TRVexpr *trv_expr);
	int parseArgs(TRVexpr *trv_expr, int argc, char *argv[]);
	void free(TRVexpr *trv_expr);
	void	print() {
		rootURLparse.print();
		print (&expr) ;
	}
	void print(TRVexpr *u);
	
	int match(URLParse &linkurl);

	int	getCategory() {
		return expr.ctg_id;
	}
	
	int getRootURL(char *urlbuf, int maxbuflen);
	int batchFile(char *fname);

	int	depthOK(int depth) {
		if (depth < expr.depth) return 1;
		else return 0;
	}
	
	int	linksOK(int linkscount) {
		if (expr.max_links==0 || linkscount < expr.max_links) return 1;
		else return 0;
	}

	int	linksPageOK(int linkscount) {
		if (expr.links_per_page==0 || linkscount < expr.links_per_page) return 1;
		else return 0;
	}
	
	URLParse	rootURLparse; //현재 처리중인 root URL 데이타 
	TRVexpr	expr;
	
private:
	
	simpleAlloc	mymem;
	int	loop_index;
	int	debug;
};
#endif
