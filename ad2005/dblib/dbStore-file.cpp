#include <stdio.h>
#include <ctype.h>
#include "dbStore.h"
#include "StrUtil.h"

DbConn	dbStore::dbConn	;
//DbQuery	dbStore::query;
char	dbStore::sqlbuf[1024*2] = {0};

FILE	*dbStore::wordfd = NULL;
FILE	*dbStore::idiomfd = NULL;

int	dbStore::storageType = 0;

#define	PRN	printf

int	dbStore::prepare(int	storage_type, char *homepath)
{
	int	ret;
	char	fpath[256];
	
	storageType = storage_type;

	if (storageType==STORAGE_TYPE_DB) {
		if ( (ret = dbConn.connect("ANYDICT", "inisoft", "gksehf")) < 0) {
			printf("DB login failed (%d)\n", ret);
			printf("Error: %d, %s\n", dbConn.errorcode(), dbConn.errormesg() );
			return -1;
		}
		
	}
	else if (storageType==STORAGE_TYPE_FILE) {
		StrUtil::path_merge(fpath, homepath, "word.ex");
		wordfd = fopen(fpath, "a");
		
		StrUtil::path_merge(fpath, homepath, "idiom.ex");
		idiomfd = fopen(fpath, "a");
		if (wordfd== NULL || idiomfd==NULL)
			return -1;
	}
	
	return 0;
}

int	dbStore::finish()
{
	if (storageType==STORAGE_TYPE_DB)  {
		dbConn.disconnect();
	}
	else if (storageType==STORAGE_TYPE_FILE) {
		if (wordfd) fclose(wordfd);
		if (idiomfd) fclose(idiomfd);
	}
	return 0;
}

int	dbStore::store_wordEx(wordType *word, exType *ex)
{
	if (storageType==STORAGE_TYPE_DB)  {
		return storeDB_wordEx(word, ex);
	}
	else if (storageType==STORAGE_TYPE_FILE) {
		return storeFILE_wordEx(word, ex);
	}
	return 0;
}

int	dbStore::storeDB_wordEx(wordType *word, exType *ex)
{
	int	res=0;
	DbQuery	query;
	
	sprintf(sqlbuf, "INSERT INTO wExam_%c VALUES ( %d, %d, %d, %d, %d, %d, %d, %d)",
		toupper(word->key[0]),
		word->w_id,
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level,
		ex->conjugate
		);

	query.exec(dbConn, sqlbuf);

	res = query.count();
	//printf("DB res = %d, %s\n", res, sqlbuf);
	printf("DB res = %d\n", res, sqlbuf);
	return res;
}


int	dbStore::storeFILE_wordEx(wordType *word, exType *ex)
{
	int	res=0;

	fprintf(wordfd, "%d %d %d %d %d %d %d\n",
		word->w_id,
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level
		);

	return res;
}


int	dbStore::store_idiomEx(idiomType *idiom, exType *ex)
{
	if (storageType==STORAGE_TYPE_DB)  {
		return storeDB_idiomEx(idiom, ex);
	}
	else if (storageType==STORAGE_TYPE_FILE) {
		return storeFILE_idiomEx(idiom, ex);
	}
	return 0;
}

int	dbStore::storeDB_idiomEx(idiomType *idiom, exType *ex)
{
	int	res=0;
	DbQuery	query;
	sprintf(sqlbuf, "INSERT INTO iExam_%c VALUES ( %d, %d, %d, %d, %d, %d, %d)",
		toupper(((wordType*)idiom->words[0])->key[0]),
		idiom->i_id,
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level
		);

	query.exec(dbConn, sqlbuf);

	res = query.count();
	//printf("DB res = %d, %s\n", res, sqlbuf);
	printf("DB res = %d\n", res, sqlbuf);
	return res;
}

int	dbStore::storeFILE_idiomEx(idiomType *idiom, exType *ex)
{
	int	res=0;

	fprintf(idiomfd, "%d %d %d %d %d %d %d\n",
		idiom->i_id,
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level
		);

	return res;
}

// return number of rows at given table
int	dbStore::countRows(char *table_name)
{
	int cnt;
	DbQuery	query;
	
	sprintf(sqlbuf, "select count(*) from %s", table_name);
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
		
	if (query.fetch()==0)
		return 0;
		
	cnt = query.colIntVal(0);
	PRN("TABLE %s: %d rows\n", table_name, cnt);
	return cnt;
}

// return max value of a field at given table
int	dbStore::getMaxValue(char *table_name, char *field)
{
	int max=0;
	DbQuery	query;
	
	sprintf(sqlbuf, "select MAX(%s) from %s", field, table_name);
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
		
	if (query.fetch()==0)
		return 0;
		
	max = query.colIntVal(0);
	PRN("TABLE %s, MAX %s: %d\n", table_name, field, max);
	return max;
}

/**
문서 수집기에서 사용. 문서의 URL의 host 부분만을 host 테이블에 저장.
어떤 곳에서 문서를 수집하였는지 참고하기 위해서.
*/
int	dbStore::storeDB_host(char *host)
{
	int	res=0;
	DbQuery	query;
	sprintf(sqlbuf, "INSERT INTO hosts VALUES ( '%s', 'enter description' )", 	host	);

	query.exec(dbConn, sqlbuf);
	return query.count();
}



/**
문서 수집기에서 사용. 주어진 URL의 문서가 존재하는지 검사. 같은 문서를 중복되어 처리하지 않기 위해서.
return 1: 존재하는 경우.
*/
int	dbStore::checkDB_doc(char *urlpath)
{
	DbQuery	query;
	sprintf(sqlbuf, "SELECT count(*) from docs where url='%s'", urlpath);
	query.exec(dbConn, sqlbuf);
	query.fetch();
	return query.colIntVal(0);
}


/**
문서 수집기에서 사용. 처음 문서 수집하여 저장하는 경우.

CREATE TABLE docs (
      id int  not null ,
      url char(256) not null,
      title char(128) not null,      
      fpath char(256) not null,
      fetchtime datetime not null,
      fsize int  not null,
      processed tinyint not null,   
      level tinyint  not null,  
    PRIMARY KEY(id)
);

CREATE INDEX docs_url_index ON docs(url);

*/
int	dbStore::storeDB_doc(docType *doc)
{
	int	res=0;
	int	maxdocid=0;
	
	DbQuery	query;

	maxdocid = getMaxValue("docs", "id");
	if (maxdocid <= 0) maxdocid = 0;
	doc->doc_id = maxdocid + 1;
	
	sprintf(sqlbuf, "INSERT INTO docs VALUES ( %d, '%s', '%s', '%s', GETDATE(),  %d, 0, 0 )",
		doc->doc_id,
		doc->url,
		doc->title,
		doc->fpath,
		doc->size
		);

	query.exec(dbConn, sqlbuf);
	res = query.count();
	if (res==0) doc->doc_id = 0;
	return res;
}

/**
예문 추출기에서 사용. 문서에서 예문을 추출하기 위해 문서 정보를 읽어 온다.
주어진 doc_id 에 해당하는 문서의 정보를 읽는다.
*/
int	dbStore::readDB_doc(docType *docinfo)
{
	DbQuery	query;
	sprintf(sqlbuf, "SELECT * from docs where id=%d ", docinfo->doc_id );
	query.exec(dbConn, sqlbuf);
	query.fetch();

	if (docinfo->url) strcpy(docinfo->url, query.colStrVal(1) );
	if (docinfo->title) strcpy(docinfo->title, query.colStrVal(2) );
	if (docinfo->fpath) strcpy(docinfo->fpath, query.colStrVal(3) );
	docinfo->size = query.colIntVal(5);
	docinfo->processed = query.colIntVal(6);
	docinfo->level = query.colIntVal(7);
	
	return query.colIntVal(0);
}

/**
예문 추출기에서 사용. 문서에서 예문을 수집하고, 처리하였음을 표시하기 위해 db  update
*/
int	dbStore::updateDB_doc(docType *docinfo)
{
	int	res=0;
	DbQuery	query;

	sprintf(sqlbuf, "UPDATE docs SET processed=%d, level=%d WHERE id=%d", 
			docinfo->processed, docinfo->level,
			docinfo->doc_id
		);

	query.exec(dbConn, sqlbuf);

	return query.count();
}


