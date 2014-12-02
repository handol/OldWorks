#include "DbQuery.h"

#include <stdio.h>
#include <ctype.h>

#ifndef	ALIGN_HALF_K
/* 512 byte 단위로 align */
#define	ALIGN_HALF_K(Len)	(((Len >> 9) + 1) << 9)
#endif

/*

참고 함수들.

SQLNumResultCols ()

-- result meta data
SQLNumResultCols, SQLDescribeCol, and SQLColAttribute

-- bind
SQLGetData
SQLBindCol 
SQLFreeStmt 

-- SQL to C Data Conversion Examples
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsql_to_c_data_conversion_examples.asp

-- Using SQLBindCol
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcusing_sqlbindcol.asp

-- SQLExecDirect 
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsqlexecdirect.asp
Returns
SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_NEED_DATA, SQL_STILL_EXECUTING, SQL_ERROR, SQL_NO_DATA, or SQL_INVALID_HANDLE.


*/

//#define BUG 1

void DbQuery::cleanup()
{
	
	if (isHandleAllocated) {
		SQLCloseCursor(hStmtHandle);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmtHandle);
		isHandleAllocated = 0;
	}

	if (DataBuff) delete [] DataBuff;
	
	DataBuff = 0;
}

int	DbQuery::exec(DbConn &dbConn, char *query, int querylen) 
{
	if (dbConn.isconnected())
		return exec(dbConn.handle(), query, querylen);
	else
		return -1;
}

int DbQuery::exec(SQLHDBC hDbcHandle, char *query, int querylen)
{
	
	SQLRETURN	sqlret;

	sqlret = SQLAllocHandle(SQL_HANDLE_STMT, hDbcHandle, &hStmtHandle);
	if (sqlret == SQL_ERROR)
	{
		iserror = 1;
		return -1;
	}

	isHandleAllocated =  1;
	if (querylen==0) querylen = strlen(query);
	sqlret = SQLExecDirect(hStmtHandle, (unsigned char *)query, querylen);
	if (sqlret == SQL_ERROR)
	{
		printf("SQL: %s\n", query);
		iserror = 1;
		error();
		return -1;
	}
	if (sqlret != SQL_NO_DATA) {
		// allocate memery for fetch
		if (alloc() == 0) {
			iserror = 1;
			return -1;
		}
	}

	query_org = query;
	return sqlret;
}


/**
@return 0 if no more data
@return 1 if data is fetched
*/
int DbQuery::fetch()
{
	SQLRETURN	sqlret;
	int col;

	if (iserror) return 0;
	memset(DataBuff, 0, dataBuffSize );
	sqlret = SQLFetch(hStmtHandle);
	if (sqlret == SQL_NO_DATA) return 0;

	#ifdef BUG
	printf("%s\n", DataBuff);
	#endif;

	// 각 colmun 별 데이타 값 끝 부분에 '\0'을 삽입.
	
	for(col=1; col<numCols+1; col++) {
		DataBuff [ OffsetArray[col] - 4 ] = 0;
		//printf("col[%d] %s\n", col-1, DataBuff + OffsetArray[col-1] );
		
		//hexdump(DataBuff + OffsetArray[col-1], ActualLenArray[col-1]);
		//printf("col[%d] %X\n", col-1, DataBuff + OffsetArray[col-1] );
	}
	return 1;
}

/**
  SELECT query와는 무관.
  SQLRowCount returns the number of rows affected by an UPDATE, INSERT, or DELETE statement;
*/
int DbQuery::count()
{
	SQLINTEGER	rowCount=0;
	SQLRowCount(hStmtHandle, &rowCount);
	return rowCount;
}



/**
allocate memery for fetch.

MSDN의 예제에는 각 타입 별로 정확한 크기만큼 메모리를 할당한다.
즉 smallint, int 를 구분하여 메모리 크기를 할당한다.
여기서는 모든 숫자 타입도 string 형태로 결과를 받는 걸로 하였다.
SQLBindCol() 에서 결과 타입을 SQL_C_CHAR로 하면 string형태로 받는다.
*/

int DbQuery::alloc()
{
	int	col;
	int	real_datasize=0;
	SQLINTEGER  SQLType, colLength;
	SQLSMALLINT sqlNumCols;
	
	if (numCols==0)	SQLNumResultCols(hStmtHandle, &sqlNumCols);
	numCols = sqlNumCols;

	if (numCols==0)	return 0;

		
	OffsetArray[0] = 0;
	
	for(col=0; col < numCols; col++) {
		// 각 column 별 타입 
		SQLColAttribute(hStmtHandle, ((SQLUSMALLINT) col) + 1, SQL_DESC_TYPE, NULL, 0, NULL, 
			(SQLPOINTER) &SQLType);

		CTypeArray[col] = GetDefaultCType(SQLType);
		
		// 각 column 별 데이타 길이
		SQLColAttribute(hStmtHandle, ((SQLUSMALLINT) col)+1, SQL_DESC_OCTET_LENGTH, NULL, 0, NULL, &colLength);
		
		/** 중요 @@dahee : DATE type의 경우 크기가 16*/
		if (colLength <= 4)
			ColLenArray[col] = 12;
		else if (colLength <= 32)
			ColLenArray[col] = 32;
		else
			ColLenArray[col] = ALIGNBUF(colLength);
		
		
		#ifdef BUG		
		printf("col[%d] data type %d --> %d, col leng %d --> %d\n",
			col, SQLType, CTypeArray[col], colLength,  ColLenArray[col] );
		#endif
		

		// +4는 각 column별로 뒤부분에 '\0'값을 넣기 위해 
		if (col) 
		OffsetArray[col] = OffsetArray[col-1]+ColLenArray[col-1] + 4; 
	}
	// DataBuff 의 총 길이.
	real_datasize = OffsetArray[numCols] = OffsetArray[numCols-1]+ColLenArray[numCols-1] + 4;

	dataBuffSize = ALIGN_HALF_K ( real_datasize );

	#ifdef BUG
	printf("DataBuff size: %d --> %d\n", real_datasize, dataBuffSize);
	#endif
	
	// 결과 데이타 저장 버퍼를 할당.
	DataBuff = new	char[dataBuffSize ];
	
	// Bind Columns
	for (col = 0; col < numCols; col++) {
		ActualLenArray[col] = 0;
		
		SQLBindCol(hStmtHandle,
			((SQLUSMALLINT) col) + 1, // index: 1, 2,3 순서
				//CTypeArray[col], // column type. 개별 타입 대신에 SQL_C_CHAR 로.
			SQL_C_CHAR,
			(SQLPOINTER)((SQLCHAR *)DataBuff + OffsetArray[col]), // 결과 데이타 버퍼
			ColLenArray[col], // 결과 데이타 최대 길이 
			&ActualLenArray[col] // 실제 길이 // @dahee
			); 
	}

	#ifdef BUG
	for(col=0; col<numCols; col++) {
		printf("col[%d] col len=%d, actual len=%d\n", col, ColLenArray[col], ActualLenArray[col]);
	}
	printf("\n");
	#endif;
	
	return numCols;

}

void DbQuery::info()
{
	int	col;
	for(col=0; col < numCols; col++) {
		printf("col[%d] Type=%d Len=%d actual len=%d Offset=%d\n", 
			col, CTypeArray[col], ColLenArray[col], ActualLenArray[col], OffsetArray[col] );
	}
	printf("Data size = %d --> %d\n", OffsetArray[numCols], dataBuffSize );
}

char	*DbQuery::colStrVal_trimmed(int col) {
	char	*start, *str;
	int	len;
	start =  (char*)DataBuff + OffsetArray[col];
	len = strlen(start);
	str = start + len -1;
	while (*str==' ') {
		*str = 0;
		str--;
	}
	
	return start;
}

void DbQuery::error(char *errmsgbuf)
{
	SQLINTEGER	dNativeError;
	SQLSMALLINT	szErrorLen;
	
	SQLCHAR		szSqlState[6];
	SQLCHAR		szErrorMsg[512];
	
	memset(szSqlState, 0x00, sizeof(szSqlState) );
	memset(szErrorMsg, 0x00, sizeof(szErrorMsg) );
	
	SQLGetDiagRec(SQL_HANDLE_STMT, hStmtHandle, 1, szSqlState, &dNativeError, 
		szErrorMsg, sizeof(szErrorMsg)-1, &szErrorLen);

	if (errmsgbuf==0)	{
		printf("Error Msg [%s]\n",szErrorMsg);
		if (query_org) printf("Query: %s\n", query_org);
	}	
	else {
		strcpy(errmsgbuf, (char*)szErrorMsg);
	}
}

/**


// source from http://www.experts-exchange.com/Programming/Programming_Languages/Dot_Net/Q_20800339.html
*/

SQLSMALLINT     DbQuery::GetDefaultCType(SQLINTEGER SQLType)
{
     SQLSMALLINT rtn = (SQLSMALLINT)SQLType;
     switch (SQLType)
          {
          case SQL_INTEGER:
               rtn = SQL_C_LONG;
               break;
          case SQL_SMALLINT:
               rtn = SQL_C_SHORT;
               break;
          case SQL_REAL:
               rtn = SQL_C_FLOAT;
               break;
          case SQL_DOUBLE:
               rtn = SQL_C_DOUBLE;
               break;
          case SQL_NUMERIC:     
               rtn = SQL_C_NUMERIC;
               break;
          //---
               
          case SQL_VARCHAR:
               rtn = SQL_C_CHAR;
               break;

          //---
          }
     return rtn;
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

