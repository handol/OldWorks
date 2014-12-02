#include "DbConn.h"
#include <stdio.h>
#include <string.h>



/**
*/
int DbConn::connect(char *db_name, char *user_name, char *authen)
{
	#ifdef OLD_MYSQL
	if( !mysql_connect(&my_sql, NULL, user_name, authen))
	{
		DbConn::error();
		return -1;
	}
	/* dahee 이라는 db를 선택 */
	if( mysql_select_db(&my_sql, db_name)) 
	{
		DbConn::error();
		return -1;
	}
	#else
	mysql_init(&my_sql);
	mysql_options(&my_sql, MYSQL_READ_DEFAULT_GROUP, "mysql");

	/* host 부분이 NULL이면 localhost입니다. */
	if( !mysql_real_connect(&my_sql, NULL, user_name, authen, db_name, 0, NULL, 0))
	{
		DbConn::error();
		return -1;
	}
	#endif
	isConnected = 1; // set the flag TRUE. connection success.
	return 0;
}

/**
*/
int DbConn::disconnect()
{
	if (isConnected)
	{
		mysql_close( &my_sql ) ;
	}
	isConnected = 0;
	return 0;
}

void DbConn::error()
{
	errorCode  = mysql_errno(&my_sql);
	strncpy(szErrorMsg, mysql_error(&my_sql), sizeof(szErrorMsg) - 1);
	szErrorMsg[sizeof(szErrorMsg)-1] = 0;
	
	printf("mysql ERROR: %d, %s\n", errorCode, szErrorMsg);
}
 
