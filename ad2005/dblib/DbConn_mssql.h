#ifndef DBCONN_H
#define DBCONN_H

#include <windows.h>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

/*
자료
mssql odbc 
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbc_part_2.asp

http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsqlexecdirect.asp


== Result-Generating and Result-Free Statements

http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcresult_generating_and_result_free_statements.asp
Result set?generating statements. These are SQL statements that generate a result set. For example, a SELECT statement. 
Row count?generating statements. These are SQL statements that generate a count of affected rows. For example, an UPDATE or DELETE statement. 

*/
class DbConn {
	public:
		DbConn() {
			isConnected = 0;
			isEnvCopied = 0;
			errorCode = 0;
		};

		/**
		기존 connection과 동일한 환경변수를 사용하고자 하는 경우에 사용.
		*/
		DbConn(DbConn &oldone) {
			isConnected = 0;
			isEnvCopied = 1;
			errorCode = 0;
			memcpy(&hEnvHandle, &(oldone.hEnvHandle), sizeof(SQLHENV) );
		};
		
		~DbConn() {
			disconnect();
		};
		
		int	connect(char *svr_name, char *user_name, char *authen);
		int	disconnect();
		
		int	errorcode() {
			return errorCode;
		}
		char	*errormesg() {
			return (char*) szErrorMsg;
		}


		SQLHDBC handle() {
			return hDbcHandle;
		}

		int	isconnected() {
			return isConnected;
		}
	private:
		int	allocEnv();
		void	error();

		int	isConnected;
		int	isEnvCopied;
		int	errorCode;
		SQLCHAR		szErrorMsg[256];

		SQLHENV		hEnvHandle;
		SQLHDBC		hDbcHandle;
		
};
#endif
