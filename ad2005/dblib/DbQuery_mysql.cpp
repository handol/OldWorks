#include "DbQuery.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef	ALIGN_HALF_K
/* 512 byte 단위로 align */
#define	ALIGN_HALF_K(Len)	(((Len >> 9) + 1) << 9)
#endif


//#define BUG 1

void DbQuery::cleanup()
{
	mysql_free_result(queryResult);
}


int DbQuery::exec(DbConn &dbConn, char *query, int querylen)
{
	if (! dbConn.isconnected())
		return -1;
	
	my_sql = dbConn.mysql();
	
	int res = mysql_query(my_sql,  query);
	if (res != 0)
	{
		printf("SQL: %s\n", query);
		iserror = 1;
		dbConn.error();
		return -1;
	}

	queryResult = mysql_use_result(my_sql);
	if (queryResult == NULL)
	{
		printf("SQL: %s\n", query);
		iserror = 1;
		dbConn.error();
		return -1;
	}

	numCols = mysql_num_fields(queryResult); 
	
	return 0;
}


/**
@return 0 if no more data
@return 1 if data is fetched
*/
int DbQuery::fetch()
{
	queryRow = mysql_fetch_row(queryResult);
	if (queryRow==NULL)
		return 0;
	else
		return 1;
}

/**
  SELECT query와는 무관.
  SQLRowCount returns the number of rows affected by an UPDATE, INSERT, or DELETE statement;
*/
int DbQuery::count()
{
	return mysql_affected_rows(my_sql);
}

void DbQuery::info()
{
}

int	DbQuery::colIntVal(int col)
{
	if (iserror) return 0;
	if (queryRow==NULL || queryRow[0]==0) return 0;
	
	if (col < 0 || col >= numCols)
		return 0;

	return (int)strtol( (char*)queryRow[col], 0, 10);
}

int	DbQuery::colLen(int col)
{
	if (iserror) return 0;
	if (queryRow==NULL || queryRow[0]==0) return 0;
	
	if (col < 0 || col >= numCols)
		return 0;
		
	return strlen((char*)queryRow[col]);
}

char	*DbQuery::colStrVal(int col)
{
	if (iserror) return "";
	if (queryRow==NULL || queryRow[0]==0) return "";
	if (col < 0 || col >= numCols)
		return "";
		
	return (char*)queryRow[col];
}
char	*DbQuery::colStrVal_trimmed(int col) {
	return "";
}

void DbQuery::error(char *errmsgbuf)
{
	strcpy(errmsgbuf, mysql_error(my_sql));
}

void DbQuery::_goodhex(char *ptr, int size, int maxsize)
{
    int i;
	int	half, full;
	full = MIN(maxsize, size);
	half = MIN(maxsize >> 1, size);
	
    for(i=0; i<half; i++)
        printf("%02X ", (char)ptr[i]);
    printf("  ");
		
	for(    ;i<full;i++)
		printf("%02X ", (char)ptr[i]);

	for(    ;i<maxsize;i++)
		printf("   ");
;
	
    for(i=0; i<full; i++)
        if (isprint(ptr[i])) printf("%c", ptr[i]);
    else printf(".");
    printf("\n");

}

void DbQuery::hexdump(char *src, int size)
{
    int i;
    printf("HEXDUMP %d bytes\n", size);
    for(i=0; i<size; i+= 16)
        _goodhex(src+i, size-i, 16);
	printf("\n");
}

