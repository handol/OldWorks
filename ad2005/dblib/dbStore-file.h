#ifndef DB_STOREL_H
#define DB_STOREL_H

#include "DbConn.h"
#include "DbQuery.h"

#include "wordType.h"
#include "Docs.h"

#include	<stdio.h>

#define	STORAGE_TYPE_DB	(1)
#define	STORAGE_TYPE_FILE	(2)
class dbStore {
	public:
		static int	prepare(int	storage_type=STORAGE_TYPE_DB, char *homepath=0);
		static int	finish();
		static	int store_wordEx(wordType *word, exType *ex);
		static	int store_idiomEx(idiomType *idiom, exType *ex);
		
		static	int storeDB_wordEx(wordType *word, exType *ex);
		static	int storeDB_idiomEx(idiomType *idiom, exType *ex);
		
		static	int storeFILE_wordEx(wordType *word, exType *ex);
		static	int storeFILE_idiomEx(idiomType *idiom, exType *ex);
		static	int	countRows(char *table_name);
		static	int	getMaxValue(char *table_name, char *field);
		static	int	storeDB_host(char *host);
		static	int	checkDB_doc(char *urlpath);
		static	int	storeDB_doc(docType *doc);
		static	int	readDB_doc(docType *docinfo);
		static	int	updateDB_doc(docType *docinfo);

		static	DbConn	dbConn	;
	private:
		static	int		storageType; // 1--DB, 2--FILE
		
		//static	DbQuery	query;
		static	char	sqlbuf[1024*2];

		static	FILE *wordfd;
		static	FILE *idiomfd;
};


#endif