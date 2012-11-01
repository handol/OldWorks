#ifndef __KUNHANDLER_H__
#define __KUNHANDLER_H__

#include "ClientHandler.h"

class KunHandler :
	public ClientHandler
{
public:
	KunHandler(ReactorInfo* rInfo);
	virtual ~KunHandler(void);

private:
	virtual int procACL(Transaction* tr);
	virtual int browserTypeCheck( Transaction * tr );
};

#endif // __KUNHANDLER_H__
