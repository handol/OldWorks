#include "DbConn.h"
#include <stdio.h>

/**
새 SQL Env Handle를 할당한다.
*/
int DbConn::allocEnv()
{
	SQLRETURN	sqlret;
	
	sqlret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnvHandle);
	if (sqlret == SQL_ERROR)
	{
		return -1;
	}

	sqlret = SQLSetEnvAttr(hEnvHandle, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	if (sqlret == SQL_ERROR)
	{
		return -2;
	}
	return 0;
}

/**
*/
int DbConn::connect(char *svr_name, char *user_name, char *authen)
{
	SQLRETURN	sqlret;

	if (! isEnvCopied) {
		allocEnv();
	}
	
	sqlret = SQLAllocHandle(SQL_HANDLE_DBC, hEnvHandle, &hDbcHandle);
	if (sqlret == SQL_ERROR)
	{
		return -2;
	}

	sqlret = SQLConnect(hDbcHandle, 
							(SQLCHAR*)svr_name, SQL_NTS, 
							(SQLCHAR*)user_name, SQL_NTS, 
							(SQLCHAR*)authen, SQL_NTS);
	if (sqlret == SQL_ERROR)
	{
		error();
		return -1;
	}

	isConnected = 1; // set the flag TRUE. connection success.
	return 0;
}

/**
*/
int DbConn::disconnect()
{
	SQLRETURN	sqlret;

	if (isConnected) {
		sqlret = SQLDisconnect(hDbcHandle);
		sqlret = SQLFreeHandle(SQL_HANDLE_DBC, hDbcHandle);
		if (! isEnvCopied) {
			sqlret = SQLFreeHandle(SQL_HANDLE_ENV, hEnvHandle);
		}
	}

	isConnected = 0; 
	return 0;
}

void DbConn::error()
{
	SQLINTEGER	dNativeError;
	SQLSMALLINT	szErrorLen;
	
	SQLCHAR		szSqlState[6];
	
	
	memset(szSqlState, 0x00, sizeof(szSqlState) );
	memset(szErrorMsg, 0x00, sizeof(szErrorMsg) );
	
	SQLGetDiagRec(SQL_HANDLE_DBC, hDbcHandle, 1, szSqlState, &dNativeError, 
		szErrorMsg, sizeof(szErrorMsg)-1, &szErrorLen);

	errorCode = dNativeError;
	
	printf("Error State [%s]\n",szSqlState);
	printf("Error Msg [%s]\n",szErrorMsg);
}
 