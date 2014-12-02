#ifndef EXTRACTER_H
#define EXTRACTER_H


#include "StrStream.h"
#include "Docs.h"


class Extracter
{
	public:
		static	void	extract_by_fname(char *textfname, StrStream &resStr);
		static	int	extract_by_docid(int docid, StrStream &resStr);
		static	int	extract_range(int fromID, int toID, int many, StrStream &resStr);
		static	int	do_extract(docType *docinfo, StrStream &resStr);
		static	int	test_ExtractOne(char *text);
		static	int load_file_2_given_mem(char *mem, char *fname, int max_size);

	private:


};

#endif

