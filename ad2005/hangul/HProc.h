#ifndef	HPROC_H
#define	HPROC_H
class HProc
{
	public:
	static	int stem(char *query, StrStream &resStr);
	static	int dict(char *query, StrStream &resStr);

	static	int test_stemmer(char *fname, StrStream &resStr);

};

#endif
