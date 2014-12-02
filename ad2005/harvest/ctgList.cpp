#include "ctgList.h"

#include "DbQuery.h"
#include "dbStore.h"

int	ctgList::load()
{
	DbQuery	query;
	int	n=0;
	
	urlNums = dbStore::countRows("urltoctg");
	if (urlNums <= 0) return -1;

	urlList = new prefixList [urlNums];
	
	query.exec(dbStore::dbConn, "select * from urltoctg");
	
	n=0;
	while (query.fetch()) {
		char *prefix = mymem.strAlloc( query.colStrVal_trimmed(0) );
		// "http://" 부분은 제외하고 목록에 로드.
		if (strncmp(prefix, "http://", 7)==0) 
			prefix += 7;
		urlList[n].set( prefix,  query.colIntVal(1), 0);		
		n++;
	}

	return n;
}

/**
@return url 에 해당하는 category  값을 리턴.
*/
int	ctgList::URL2Category(char *url)
{
	int	matchCtg = 0;
	// "http://" 부분은 제외하고 비교.
	if (strncmp(url, "http://", 7)==0) 
			url += 7;
	
	for(int i=0; i<urlNums; i++) {
		int cmp = urlList[i].match(url);
		if (cmp < 0) break;
		if (cmp == 0) {
			matchCtg = urlList[i].getCtg();
		}
	}
	return matchCtg;
}

int	ctgList::reload() {
	mymem.reuse();
	init();
	return load();
}

int ctgList::cmplist(const void *Aptr, const void *Bptr)
{
	return ((prefixList*)Aptr)->cmp( (prefixList*)Bptr );
}

int ctgList::sortlist()
{
	qsort (urlList, urlNums, sizeof(char *), cmplist);
	return 0;
}
