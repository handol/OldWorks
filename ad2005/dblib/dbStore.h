#ifndef DB_STOREL_H
#define DB_STOREL_H

#include "DbConn.h"
#include "DbQuery.h"

#include "wordType.h"
#include "HDict.h"
#include "Docs.h"

#include	<stdio.h>

class dbStore {
	public:
		static int	prepare(char *dbname=0, char *id=0, char *passwd=0);
		static int	finish();
		static	int store_wordEx(wordType *word, exType *ex);
		static	int store_idiomEx(idiomType *idiom, exType *ex);

		static	int store_hanwordEx(HwordType *hword, exType	 *ex);
		
		static	int storeFILE_wordEx(wordType *word, exType *ex);
		static	int storeFILE_idiomEx(idiomType *idiom, exType *ex);
		static	int	countRows(char *table_name);
		static	int	getMaxValue(char *table_name, char *field);
		static	int	checkDB_host(char *host);
		static	int	storeDB_host(char *host);
		static	int	checkDB_doc(char *urlpath);
		static	int	storeDB_doc(docType *doc);
		static	int	readDB_doc(docType *docinfo);
		static	int	updateDB_doc(docType *docinfo);
		static	int	selectDB_doc_id(int	*docid_list, int listsize, int fromID, int toID=0);
		static	int	storeDB_worddict(wordType *w);
		static	int	storeDB_idiomdict(char  *idiom);
		static	int	searchDB_worddict(char *word);
		static	int	searchDB_idiomdict(char *idiom);
		static	int	storeDB_req(char *tablename, char *req);
		static	int	updateDB_req(char *tablename, int id, char *result,  int rescnt);

		static	int	storeDB_newhanword(char  *hanword, int last_max_id);
			
		static	DbConn	dbConn	;

		#define	ERRMSG_MAX	(1024)
		#define	QUERY_MAX	(1024*2)
		static	char	errmsg[ERRMSG_MAX];
		static	char	querybuf[QUERY_MAX];
		
	private:
		
		static	FILE *wordfd;
		static	FILE *idiomfd;
};

#endif

