
#include "ctgList.h"

class categoryMapper {
	public:
		static	int	prepare();
		static	int	finish();
		static	int	getCategory(char *url)
		{
			return url2ctg.URL2Category(url);
		};

	private:
		static	ctgList	url2ctg;
};

