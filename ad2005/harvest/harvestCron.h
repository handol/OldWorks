#ifndef HARVESTCRON_H
#define	HARVESTCRON_H

#include "cronTable.h"

class harvestCron: public cronTable
{
	public:
	void	setHarvestAddr(char *ipaddr, int port) {
		strcpy(harvestIPaddr, ipaddr);
		harvestPort = port;
	}
	
	void	setExtractAddr(char *ipaddr, int port) {
		strcpy(extractIPaddr, ipaddr);
		extractPort= port;
	}
	
	 int	loadTable(int _tabsize);
	 int	do_job(char *jobdesc);
	 
	private:
		int	loadTable_DB();
		int	loadTable_test(int _tabsize);
		
		char	harvestIPaddr[30];
		int	harvestPort;
		char	extractIPaddr[30];
		int	extractPort;
};

#endif
