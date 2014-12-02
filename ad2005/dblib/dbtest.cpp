#include "DbConn.h"
#include "DbQuery.h"
#include "dbStore.h"
#include <stdio.h>
#include <time.h>

void do_query(DbConn	&dbConn)
{
	DbQuery	query;
	int i;

	query.exec(dbConn, "select  * from test");
	
	query.info();
	printf("Rows = %d\n", query.count() );
	
	//for(i=0; i<3; i++) {
	for(i=0; ; i++) {
		printf("=== [%d]\n", i);
		if (query.fetch() == 0) break;
		printf("Rows = %d\n", query.count() );
		printf("%s\n", query.colStrVal(0) );
		printf("%s\n", query.colStrVal(1) );
		printf("%s\n", query.colStrVal(2) );
		//printf("%d\n", query.colIntVal(1) );
		//printf("%d\n", query.colIntVal(2) );
	}

	printf("finishing query ...\n");
}

int	main() {
	int	ret;
	DbConn	dbConn	;
	
	int i;
	/*
	if ( (ret = dbConn.connect("ANYDICT", "inisoft", "gksehf")) < 0) {
		printf("DB login failed (%d)\n", ret);
		printf("Error: %d, %s\n", dbConn.errorcode(), dbConn.errormesg() );
		return 0;
	}

	do_query(dbConn);
	*/
	/*
	for(i=0; i<1000000; i++) {
		printf("-");
		_sleep(1);
	}
	*/
	printf("finishing db ...\n");

	dbStore::prepare(STORAGE_TYPE_DB);
	
	printf("test = %d\n", dbStore::countRows("test") );
	printf("hosts = %d\n", dbStore::countRows("hosts") );

	dbStore::finish();	
	return 0;
}
