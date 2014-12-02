#ifndef QUERY_MSSQL_H
#define QUERY_MSSQL_H

#include "DbConn.h"
#include <stdio.h>
#include <stdlib.h>
/*
�ڷ�
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
/* �ѱ� ó�� �ÿ� isspace () ����ϸ� ������ �Ǵ� ������ ����. */
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
		���� Ÿ���� binary Ÿ������ bind �� ���.
		���� ���ڴ� ���� �ּҰ��� ������. (endian ����)
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
		���� Ÿ���� string Ÿ������ bind �� ���.
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

		SQLSMALLINT	CTypeArray[DB_MAX_FIELDS]; // �� column �� Ÿ�� 
		SQLINTEGER 	ColLenArray[DB_MAX_FIELDS];
		/* ��� ����Ÿ ���� ���� 
		OffsetArray[i]: DataBuff �󿡼� column i  ���� ���� ��ġ. 
		*/
		SQLINTEGER 	OffsetArray[DB_MAX_FIELDS+1];  // �� ������ column �� �� ��ġ�� �����ϱ� ���� �ϳ��� �Ҵ�.
		SQLINTEGER 	ActualLenArray[DB_MAX_FIELDS];

		int		dataBuffSize;
		char		*DataBuff;

		char		*query_org;
};
#endif

