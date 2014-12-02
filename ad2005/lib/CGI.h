
#ifndef CGI_H
#define	CGI_H

#include "StrStream.h"

#define	CGI_VARNAME_LEN	(15)
#define	CGI_METHOD_LEN		(15)

#define	CGI_VARVAL_LEN		(255)

#define	MAX_VARVALS		(5)

typedef struct _varval {

	char	name[CGI_VARNAME_LEN+1];
	char	strval[CGI_VARVAL_LEN+1];
	int	intval;
} VarVal;

class	CGI {
	public:
	CGI() {
		init();
	}
	
	CGI(char *httpreq) {
		init();
		parse(httpreq);
	}
	
	~CGI() {
		if (varvals) delete [] varvals;
	}
	
	int	parse(char *urlstr);
	static	void	unescape(char *dest, char *src);
	static	void	escape(char *dest, char *src);
	
	char *getVal(char *name);
	int	getIntVal(char *name);
	void	print();
	void	print(StrStream &stream);
	
	char *method() {
		return _method;
	}
	
	private:
	void	init();
	int	add(char *name, char *val);
	
	char	_method[CGI_METHOD_LEN+1];
	VarVal	*varvals;	
	int	numVals;
};

#endif
