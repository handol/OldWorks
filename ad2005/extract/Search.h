#ifndef SEARCH_H
#define SEARCH_H

#include "wordType.h"
#include "simpleAlloc.h"
#include "StrStream.h"
#include "HDict.h"


class Search
{
	public:
	static	int	prepare(char *homepath);
	static	int	finish();
	static	int 	dict(char *query, StrStream &stream);
	static	int	search(char *_query, StrStream &stream, int searchMode=0);		
	static	int	searchEnglish(char *query, StrStream &stream, int searchMode=0);
	static	int	searchEngOneWord(wordType *w, StrStream &stream, char *firstWord=0);
	static	int	searchHangul(char *query, StrStream &stream, int searchMode=0);
	static	int	stemHangulWords(HwordType	*hwordInfos[],  int matchVals[], int maxnum, char *buf, char *firstWord=NULL);
	private:
	static	simpleAlloc searchmem;

};

#endif
