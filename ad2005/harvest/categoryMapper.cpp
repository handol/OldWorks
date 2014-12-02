#include "categoryMapper.h"

ctgList	categoryMapper::url2ctg;

int	categoryMapper::prepare()
{
	int	ctgcnt = url2ctg.load();
	url2ctg.print();
	return 0;
}

int	categoryMapper::finish()
{
	return 0;
}



