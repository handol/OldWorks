#include "URLTrav.h"
#include "htmlTagEntity.h"
#include <stdio.h>

#include "dbStore.h"
#include "StrUtil.h"
//extern "C" int server_main (int, char **);

extern  int server_main (int, char **);

char	*adhome = NULL;

void prepareDB()
{
	adhome = getenv("ADHOME");
	dbStore::prepare("anydict", "root", "wpxk00");
}

int main(int argc, char *argv[])
{
	prepareDB();

	
	URLTrav trav;
	TravExpr    travCond;
	StrStream	result;	

	MemSplit	args(10, 128);
	//args.split("xx http://www.anydict.com -D 1");

	char	addatapath[256];
	StrUtil::path_merge(addatapath, adhome, "data");
	htmlTagEntity::init(addatapath);

	if (argc < 2) {
		args.split("http://www.cnn.com/ASIA/ -D 1 -I /WORLD,/2005/SPORT");
		args.print();

		printf("ARGS: %d %X\n", args.argc(), (unsigned int)args.argv() );

		//trav.expr(args.argc(), args.argv() );
		//trav.expr("http://www.cnn.com/ASIA/ -D 1 -I /WORLD,/2005/SPORT");
		trav.expr("http://www.whitehouse.gov/news/releases/2005/04/20050421-4.html");
		trav.print();
	}
	else {
		trav.expr(argc-1, argv+1 );
		trav.print();
	}

	//URLParse	url("http://www.cnn.com/CNNI/Programs/techwatch/");
	//trav.travCond.match( &url.url );


	trav.traverse(result, "./" );

	
	//server_main(0, 0);
	return 0;
}
