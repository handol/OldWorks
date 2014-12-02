#ifndef QUERY_MSSQL_H
#define QUERY_MSSQL_H

#include "DbConn.h"
#include <stdio.h>
#include <stdlib.h>
/*
자료

http://dev.mysql.com/doc/refman/5.0/en/mysql-fetch-row.html

http://dev.mysql.com/doc/refman/5.0/en/mysql-fetch-lengths.html
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
			numCols = 0;
			iserror = 0;			
			my_sql = NULL;
			queryResult = NULL;
		};

		~DbQuery() {
			cleanup();
		};
		
		int	exec(DbConn &dbConn, char *query, int querylen=0);
		
		int	fetch();
		int	count();
		
		int	columns() {
			return numCols;
		}
		
		char	*colStrVal(int col);

		char	*colStrVal_trimmed(int col);
		/**
		숫자 타입을 binary 타입으로 bind 한 경우.
		높은 숫자는 높은 주소값을 가진다. (endian 문제)
		*/
		int	colIntVal2(int col)
		{
			return colIntVal(col);
		}
		

		/**
		숫자 타입을 string 타입으로 bind 한 경우.
		*/
		int	colIntVal(int col);

		int	colLen(int col);
		
		void info();
		void	error(char *errmsgbuf=0);
		void	cleanup();
		
	private:
		void _goodhex(char *ptr, int size, int maxsize);
		void hexdump(char *src, int size);

		int		iserror;
		int		numCols;

		MYSQL	*my_sql;
		MYSQL_STMT *stmt;
		MYSQL_RES *queryResult ;
		MYSQL_ROW queryRow ;
};
#endif

