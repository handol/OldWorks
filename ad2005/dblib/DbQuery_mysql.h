#ifndef QUERY_MSSQL_H
#define QUERY_MSSQL_H

#include "DbConn.h"
#include <stdio.h>
#include <stdlib.h>
/*
�ڷ�

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
/* �ѱ� ó�� �ÿ� isspace () ����ϸ� ������ �Ǵ� ������ ����. */
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
		���� Ÿ���� binary Ÿ������ bind �� ���.
		���� ���ڴ� ���� �ּҰ��� ������. (endian ����)
		*/
		int	colIntVal2(int col)
		{
			return colIntVal(col);
		}
		

		/**
		���� Ÿ���� string Ÿ������ bind �� ���.
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

