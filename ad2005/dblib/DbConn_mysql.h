#ifndef DBCONN_H
#define DBCONN_H

#include <mysql.h>

class DbConn {
	public:
		DbConn() {
			isConnected = 0;
			isEnvCopied = 0;
			errorCode = 0;
		};

		/**
		���� connection�� ������ ȯ�溯���� ����ϰ��� �ϴ� ��쿡 ���.
		*/
		DbConn(DbConn &oldone) {
			isConnected = 0;
			isEnvCopied = 1;
			errorCode = 0;
		};
		
		~DbConn() {
			disconnect();
		};
		
		int	connect(char *db_name, char *user_name, char *authen);
		int	disconnect();
		
		int	errorcode() {
			return errorCode;
		}
		char	*errormesg() {
			return (char*) szErrorMsg;
		}

		void	error();

		int handle() {
			return 0;
		}

		MYSQL *mysql()
		{
			return &my_sql;
		}

		int	isconnected() {
			return isConnected;
		}
		
	private:
		int	allocEnv();

		int	isConnected;
		int	isEnvCopied;
		int	errorCode;
		char		szErrorMsg[256];

		int		hEnvHandle;
		int		hDbcHandle;

		MYSQL my_sql ;
		
};
#endif
