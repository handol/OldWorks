#ifndef CTGLIST_H
#define CTGLIST_H

#include <stdio.h>
#include <string.h>

#include "simpleAlloc.h"

class	prefixList {

private:
		char *prefix;
		int	len;
		int	ctg;

public:
		prefixList() {
			prefix = 0;
			len = 0;
		}

		void	set(char *_prefix, int _ctg, int _len=0) {
			prefix = _prefix;
			ctg = _ctg;
			if (_len) len = _len;
			else len = strlen(prefix);
		}
		
		int	cmp(prefixList *other) {
			return strcmp(prefix, other->prefix);
		}
		
		int match(char *fullurl) {
			return strncmp(fullurl, prefix, len);
		}

		int	getCtg() {
			return ctg;
		}

		void print() {
			printf("%4d %2d %s\n", ctg, len, prefix);
		}
};

class ctgList
{

private:
		
	simpleAlloc mymem;
	prefixList *urlList;
	int	urlNums;

	
		
public:
		
	ctgList() {
		init();
	}

	void	init() {
		mymem.set(10); // block 10K
		urlList = 0;
		urlNums = 0;

	}
	
	int	load();
	int	reload();
	int	URL2Category(char *url);
	
	static	int cmplist(const void *Aptr, const void *Bptr);
	int sortlist();
	void	print() {
		for(int i=0; i<urlNums; i++)
			urlList[i].print();
	}
};

#endif
