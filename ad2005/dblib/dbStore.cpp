#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dbStore.h"
#include "StrUtil.h"
#include "HanChar.h"


DbConn	dbStore::dbConn	;

FILE	*dbStore::wordfd = NULL;
FILE	*dbStore::idiomfd = NULL;
char	dbStore::errmsg[ERRMSG_MAX] = {0};
char	dbStore::querybuf[QUERY_MAX] = {0};

#define	PRN	printf

int	dbStore::prepare(char *dbname, char *id, char *passwd)
{
	int	ret;
	
	if (dbname && id && passwd) {
		ret = dbConn.connect(dbname, id, passwd);
	}
	else {
		dbname = "ANYDICT";
		ret = dbConn.connect("ANYDICT", "inisoft", "gksehf");
	}	
	if ( ret < 0) {
		printf("Error: %d, %s\n", dbConn.errorcode(), dbConn.errormesg() );
		return -1;
	}
	else {
		printf("DB login Success: DB NAME=%s\n", dbname );
	}
	
	
	return 0;
}

int	dbStore::finish()
{
	dbConn.disconnect();

	return 0;
}


int	dbStore::store_wordEx(wordType *word, exType *ex)
{
	int	res=0;
	char	sqlbuf[512];
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

	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;

	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) {
		//printf("SQL failed\n%s\n", sqlbuf);
		strcpy(querybuf, sqlbuf);
		query.error(errmsg);
		return -1;
	}

	return res;
}

int	dbStore::store_hanwordEx(HwordType *hword, exType *ex)
{
	int	res=0;
	char	sqlbuf[512];
	DbQuery	query;
	
	int	fstsnd = HanChar::wsstr2fstsnd(hword->key);
	
	sprintf(sqlbuf, "INSERT INTO hExam_%d VALUES ( %d, %d, %d, %d, %d, %d, %d, %d)",
		fstsnd,
		hword->id,
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level,
		ex->conjugate
		);

	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;

	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) {
		//printf("SQL failed\n%s\n", sqlbuf);
		strcpy(querybuf, sqlbuf);
		query.error(errmsg);
		return -1;
	}

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
	int	res=0;
	char	sqlbuf[512];
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

	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;

	res = query.count();
	//printf("DB res = %d\n", res, sqlbuf);
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
	char	sqlbuf[512];
	DbQuery	query;
	
	sprintf(sqlbuf, "select count(*) from %s", table_name);
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
		
	if (query.fetch()==0)
		return 0;
		
	cnt = query.colIntVal(0);
	//PRN("TABLE %s: %d rows\n", table_name, cnt);
	return cnt;
}

// return max value of a field at given table
int	dbStore::getMaxValue(char *table_name, char *field)
{
	int max=0;
	char	sqlbuf[512];
	DbQuery	query;
	
	sprintf(sqlbuf, "select MAX(%s) from %s", field, table_name);
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
		
	if (query.fetch()==0)
		return 0;
		
	max = query.colIntVal(0);
	//PRN("TABLE %s, MAX %s: %d\n", table_name, field, max);
	return max;
}

/**
문서 수집기에서 사용. host 이름의 중복을 검사하기 위해.
*/
int	dbStore::checkDB_host(char *host)
{
	char	sqlbuf[512];
	DbQuery	query;
	sprintf(sqlbuf, "SELECT count(*) from hosts where hostname='%s'", host);
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
	
	query.fetch();
	return query.colIntVal(0);
}

/**
문서 수집기에서 사용. 문서의 URL의 host 부분만을 host 테이블에 저장.
어떤 곳에서 문서를 수집하였는지 참고하기 위해서.
*/
int	dbStore::storeDB_host(char *host)
{
	char	sqlbuf[512];
	DbQuery	query;
	sprintf(sqlbuf, "INSERT INTO hosts VALUES ( '%s', 'enter description' )", 	host	);

	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	return query.count();
}



/**
문서 수집기에서 사용. 주어진 URL의 문서가 존재하는지 검사. 같은 문서를 중복되어 처리하지 않기 위해서.
return 1: 존재하는 경우.
*/
int	dbStore::checkDB_doc(char *urlpath)
{
	char	sqlbuf[512];
	DbQuery	query;
	sprintf(sqlbuf, "SELECT count(*) from docs where url='%s'", urlpath);
	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
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
	char	sqlbuf[1024*2];
	char	clean_title[256];
	char *right_title = 0;
	DbQuery	query;

	maxdocid = getMaxValue("docs", "id");
	if (maxdocid <= 0) maxdocid = 0;
	doc->doc_id = maxdocid + 1;

	right_title = StrUtil::mssql_escape_quote(clean_title, doc->title, 255);  //-- title에 따옴표가 있는 경우에 제거. 
	//printf("### %s\n", doc->title);
	//printf("=== %s\n", right_title);

	// 타이틀 앞뒤로 공백 추가. UTF-8 일 때 SQL 오류 발생하여 수정함. 2007-10-10
	sprintf(sqlbuf, "INSERT INTO docs VALUES ( %d, '%s', ' %s ', '%s', GETDATE(),  %d, 0, 0, %d )",
		doc->doc_id,
		doc->url,
		right_title,
		doc->fpath,
		doc->size,
		doc->ctg
		);

	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) {
		//printf("SQL failed\n%s\n", sqlbuf);
		strcpy(querybuf, sqlbuf);
		query.error(errmsg);
		doc->doc_id = 0;
		return -1;
	}
	return res;
}

/**
예문 추출기에서 사용. 문서에서 예문을 추출하기 위해 문서 정보를 읽어 온다.
주어진 doc_id 에 해당하는 문서의 정보를 읽는다.
*/
int	dbStore::readDB_doc(docType *docinfo)
{
	char	sqlbuf[512];
	DbQuery	query;
	sprintf(sqlbuf, "SELECT * from docs where id=%d ", docinfo->doc_id );
	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	query.fetch();

	if (docinfo->url) strcpy(docinfo->url, query.colStrVal(1) );
	if (docinfo->title) strcpy(docinfo->title, query.colStrVal(2) );
	if (docinfo->fpath) strcpy(docinfo->fpath, query.colStrVal(3) );
	docinfo->size = query.colIntVal(5);
	docinfo->processed = query.colIntVal(6);
	docinfo->level = query.colIntVal(7);
	docinfo->ctg = query.colIntVal(8);
	return query.colIntVal(0);
}

/**
예문 추출기에서 사용. 문서에서 예문을 수집하고, 처리하였음을 표시하기 위해 db  update
*/
int	dbStore::updateDB_doc(docType *docinfo)
{
	char	sqlbuf[512];
	DbQuery	query;

	sprintf(sqlbuf, "UPDATE docs SET processed=%d, level=%d WHERE id=%d", 
			docinfo->processed, docinfo->level,
			docinfo->doc_id
		);

	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;

	if (query.count() < 0)
		return 0;
	else
		return 1;
}

int	dbStore::storeDB_worddict(wordType *w)
{
	int	res=0;
	int	maxid=0;
	char	word_clean[64];
	char *word_ptr;
	
	char	org_word[64];
	char	org_word_clean[64];
	char *org_word_ptr;
	
	char	sqlbuf[512];
	DbQuery	query;

	maxid = dbStore::getMaxValue("worddict", "word_id");
	if (maxid <= 0) maxid = 0;
	w->w_id = maxid + 1;
	if (w->org_word) strcpy(org_word, w->org_word);
	else	strcpy(org_word, "");

	// 단어에 single quete가 있는 경우 처리(escape)가 필요.
	word_ptr = StrUtil::mssql_escape_quote(word_clean, w->key, sizeof(word_clean)-1);
	org_word_ptr = StrUtil::mssql_escape_quote(org_word_clean, org_word, sizeof(org_word_clean)-1);
	
	sprintf(sqlbuf, "INSERT INTO worddict VALUES ( '%s',  %d, %d, %d, %d, %d,  '%s' )",
		word_ptr,
		w->w_id,
		w->w_type,
		w->level,
		w->PS,
		w->is_conj,
		org_word_ptr
		);
		
	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	
	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) w->w_id = 0;
	return w->w_id;
}

int	dbStore::storeDB_idiomdict(char  *idiom)
{
	int	res=0;
	int	maxid=0;
	char	sqlbuf[512];
	char	idiom_clean[64];
	char *idiom_ptr;
	DbQuery	query;

	maxid = dbStore::getMaxValue("idiomdict", "idiom_id");
	if (maxid <= 0) maxid = 0;
	maxid = maxid + 1;

	// 단어에 single quete가 있는 경우 처리(escape)가 필요.
	idiom_ptr = StrUtil::mssql_escape_quote(idiom_clean, idiom, sizeof(idiom_clean)-1);
	sprintf(sqlbuf, "INSERT INTO idiomdict VALUES ( '%s',  %d )",
		idiom_ptr, maxid
		);
		
	if (query.exec(dbConn, sqlbuf) < 0)
		return -1;
	
	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) return -1;
	return maxid;
}

int	dbStore::searchDB_worddict(char *word)
{
	char	sqlbuf[256];
	DbQuery	query;
	
	// 단어에 single quete가 있는 경우 처리(escape)가 필요.
	char	word_clean[64];
	char *word_ptr;
	word_ptr = StrUtil::mssql_escape_quote(word_clean, word, sizeof(word_clean)-1);
	
	sprintf(sqlbuf, "select count(*) from worddict where word='%s'", word_ptr);
		
	if (query.exec(dbStore::dbConn, sqlbuf) < 0) return 0;
	if (query.fetch()==0)	return 0;
	
	return query.colIntVal(0);
}

int	dbStore::searchDB_idiomdict(char *idiom)
{
	char	sqlbuf[256];
	DbQuery	query;

	// 단어에 single quete가 있는 경우 처리(escape)가 필요.
	char	word_clean[64];
	char *word_ptr;
	word_ptr = StrUtil::mssql_escape_quote(word_clean, idiom, sizeof(word_clean)-1);
	
	sprintf(sqlbuf, "select count(*) from idiomdict where idiom='%s'", word_ptr);
		
	if (query.exec(dbStore::dbConn, sqlbuf) < 0) return 0;
	if (query.fetch()==0)	return 0;
	
	return query.colIntVal(0);
}

int	dbStore::storeDB_req(char *tablename, char *req)
{
	int	res=0;
	int	maxid=0;
	char	sqlbuf[512];
	DbQuery	query;

	maxid = dbStore::getMaxValue(tablename, "id");
	if (maxid <= 0) maxid = 0;
	maxid = maxid + 1;
			
	sprintf(sqlbuf, "INSERT INTO %s VALUES ( %d, '%s', ' ', 0, GETDATE() )",
		tablename, 
		maxid, req
		);
		
	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	
	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) return 0;
	return maxid;
}

int	dbStore::updateDB_req(char *tablename, int id, char *result, int rescnt)
{
	char	sqlbuf[1024];
	DbQuery	query;

	sprintf(sqlbuf, "UPDATE %s SET result='%s', count=%d WHERE id=%d", 
			tablename,
			result,
			rescnt, 
			id
		);

	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;

	return query.count();
}

int	dbStore::selectDB_doc_id(int	*docid_list, int listsize, int fromID, int toID)
{
	char	sqlbuf[256];
	DbQuery	query;
	int	cnt = 0;
	if (toID==0) 
	sprintf(sqlbuf, "select id from docs where id >= %d AND processed=0 ", fromID);
	else
	sprintf(sqlbuf, "select id from docs where id >= %d AND id <= %d AND processed=0 ", fromID, toID);

	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	
	while (query.fetch() && cnt < listsize) {
		docid_list[cnt++] = query.colIntVal(0);		
	}
	return cnt;
}

/**
국어 사전 이후의 한글단어 추가를 위한 테이블

CREATE TABLE newhanword (
    word char(40) not null,
    word_id int  not null ,
    PRIMARY KEY(word_id)
);  

CREATE INDEX newhanword_index ON newhanword(word);

@param hanword  -- 신규 단어
@param last_max_id  -- 최고의 단어 id (마지막 단어)
*/
int	dbStore::storeDB_newhanword(char  *hanword, int last_max_id)
{
	int	res=0;
	int	maxid=0;
	char	sqlbuf[512];
	DbQuery	query;

	maxid = dbStore::getMaxValue("newhanword", "word_id");
	maxid = MAX(last_max_id, maxid);
	maxid = maxid + 1;

	sprintf(sqlbuf, "INSERT INTO newhanword VALUES ( '%s',  %d )",
		hanword, maxid
		);
		
	if (query.exec(dbConn, sqlbuf) < 0)
		return 0;
	
	res = query.count();
	if (res < RETURN_VAL_OF_INSERT) return 0;
	return maxid;
}

