#ifndef DOCS_H
#define DOCS_H
#include "wordType.h"
#include "simpleAlloc.h"

typedef struct _docs
{
	uint4	doc_id;
	uint4	ctg;
	char	*url;
	char	*title;
	char	*fpath;
	char	*text;
	int	size;
	byte	processed;
	byte	level;	
} docType;

class Docs
{
	public:
	static	int	load(char *homepath);
	static	int	load_docinfo();
	static	int	load_docinfo_FILE(char *fname);
	static	int	load_doctext();
	static	int	cmpDocList(const void *Aptr, const void *Bptr);
	static	int	sortDocList();
	static	docType *searchDoc(int target_id);
	static	docType *searchDoc_old(int target_id);
	static	void	print(docType *doc);

		
	private:
	static	simpleAlloc docmem;

	static	docType	*docArray;
	static	int	docArraySize;
	static	int	docNums;
};

#endif
