#include "harvestCron.h"
#include <stdio.h>

#include "dbStore.h"
#include "TimeUtil.h"

int main (int argc, char **argv)
{
	harvestCron	mycron;

	dbStore::prepare();
	
	mycron.setHarvestAddr("127.0.0.1", 9010);
	mycron.loadTable(100);
	mycron.print();
	mycron.svc();

	dbStore::finish();
	return 0;
}
