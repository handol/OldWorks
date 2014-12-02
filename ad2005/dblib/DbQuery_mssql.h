#ifndef QUERY_MSSQL_H
#define QUERY_MSSQL_H

#include "DbConn.h"
#include <stdio.h>
#include <stdlib.h>
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

#ifndef MIN 
#define	MIN(A, B) ( (A) > (B) )? (B) : (A)
#endif

#define ALIGNSIZE 4
#define ALIGNBUF(Length) Length % ALIGNSIZE ? \
                  Length + ALIGNSIZE - (Length % ALIGNSIZE) : Length

#define	DB_MAX_FIELDS	(10)

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

class DbQuery {
	public:
		DbQuery() {
			isHandleAllocated = 0;
			numCols = 0;
			dataBuffSize = 0;
			DataBuff = 0;
			iserror = 0;
			query_org = 0;
		};

		~DbQuery() {
			cleanup();
		};

		int	non_select(DbConn &dbConn, char *query, int querylen=0) {
			exec(dbConn, query, querylen);
			return count();
		}
		
		int	exec(DbConn &dbConn, char *query, int querylen=0);
		
		int	exec(SQLHDBC hDbcHandle, char *query, int querylen=0);
		
		int	fetch();
		int	count();
		
		int	columns() {
			return numCols;
		}
		
		char	*colStrVal(int col) {
			if (iserror) return "";
			return (char*)DataBuff + OffsetArray[col];
		}

		char	*colStrVal_trimmed(int col);
		/**
		숫자 타입을 binary 타입으로 bind 한 경우.
		높은 숫자는 높은 주소값을 가진다. (endian 문제)
		*/
		int	colIntVal2(int col) {
			int size = ActualLenArray[col];
			int	i;

			int	val=0;
			char *start;

			start = (char*)DataBuff + OffsetArray[col] + size -1 ;

			for(i=0; i<size; i++) {
				val <<= 8;
				val += (int) *((unsigned char *)start);
				start--;
			}
			return val;
		}
		

		/**
		숫자 타입을 string 타입으로 bind 한 경우.
		*/
		int	colIntVal(int col) {
			if (iserror) return 0;
			return (int)strtol( (char*)((char*)DataBuff + OffsetArray[col]), 0, 10);
		}

		int	colLen(int col) {
			if (iserror) return 0;
			return ActualLenArray[col];
		}
		
		void info();
		void	error(char *errmsgbuf=0);
		void	cleanup();
		
	private:
		int	alloc();
		
		SQLSMALLINT     GetDefaultCType(SQLINTEGER SQLType);
		void _goodhex(char *ptr, int size, int maxsize);
		void hexdump(char *src, int size);

		int		iserror;
		int		isHandleAllocated;
		int		numCols;
		SQLHSTMT	hStmtHandle;

		SQLSMALLINT	CTypeArray[DB_MAX_FIELDS]; // 각 column 별 타입 
		SQLINTEGER 	ColLenArray[DB_MAX_FIELDS];
		/* 결과 데이타 저장 버퍼 
		OffsetArray[i]: DataBuff 상에서 column i  값의 시작 위치. 
		*/
		SQLINTEGER 	OffsetArray[DB_MAX_FIELDS+1];  // 맨 마지막 column 의 끝 위치를 저장하기 위해 하나더 할당.
		SQLINTEGER 	ActualLenArray[DB_MAX_FIELDS];

		int		dataBuffSize;
		char		*DataBuff;

		char		*query_org;
};
#endif

