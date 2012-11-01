#ifndef CLIENT_REMOVER_H
#define CLIENT_REMOVER_H

#include "PasEventHandler.h"

class ClientRemover:
	public ACE_Task<PAS_SYNCH>
{
public:
	static ClientRemover *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~ClientRemover(void);

	virtual int svc(void);

	int putWork(PasHandler *pHandler);

private:
	static ClientRemover *oneInstance;
	ClientRemover(ACE_Thread_Manager* threadManager=0);
};
#endif
