
#ifdef NO_MSSQL
#include "DbQuery_mysql.h"
#define	RETURN_VAL_OF_INSERT	(0)
#else
#include "DbQuery_mssql.h"
#define	RETURN_VAL_OF_INSERT	(0)
#endif

