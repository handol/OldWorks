#include "harvestCron.h"


#include "CGI.h"
#include "StrStream.h"
#include "ace/Log_Msg.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "ace/INET_Addr.h"

#include "DbConn.h"
#include "DbQuery.h"
#include "dbStore.h"

#include "URLTrav.h"

#include <stdio.h>
#include <time.h>

#define PRN	printf

int	harvestCron::loadTable(int _tabsize)
{
	return loadTable_DB();
}

int	harvestCron::loadTable_DB()
{
	int	tabsize = 0;
	int	n=0;
	DbQuery	query;

	char	*cronjob[6];
	
	tabsize = 	dbStore::countRows("cronjob");
	ACE_DEBUG((LM_INFO, "NUMBER of cronjob = %d\n", tabsize));
	if (tabsize < 0) {
		PRN("DB query fail\n");
	}

	tabsize += 2;

	this->allocTable(tabsize);

	if ( query.exec(dbStore::dbConn, "select * from cronjob") < 0) {
		ACE_DEBUG((LM_INFO,"DB query error!\n"));
		return 0;
	}

	while (query.fetch()) {
		int	jobtype = 0;
		jobtype = query.colIntVal(6);;

		/*
		@todo
		jobtype == 1 : HARVEST
		jobtype == 2 : EXTRACT
		*/
		/* load only harvest jobs (jobtype==1) */
		if (jobtype!=1) continue;
		
		for (int i=0; i<5; i++)
			cronjob[i] = query.colStrVal(i+1);
		//cronjob[5] = query.colStrVal(7);
		cronjob[5] = query.colStrVal_trimmed(7);

		this->insertCronJob( cronjob );
	}
	return n;
}

/**
테스트 용
*/
int	harvestCron::loadTable_test(int _tabsize)
{
	cronTabType item;

	time_t		t_val;
	struct tm	*t;

	time(&t_val);
	t = localtime(&t_val);
	
	this->allocTable(_tabsize);
	for (int i=0; i<5; i++)
		item.times[i] = CRONTAB_WILDCARD;
		
	item.times[4] = 10;
	strcpy(item.jobdesc, "http://www.harvest.com D=0");

	this->fillTable(&item);

	item.times[3]  = t->tm_min+1;
	item.times[4]  = CRONTAB_WILDCARD;
	strcpy(item.jobdesc, "http://www.cnn.com");
	this->fillTable(&item);

	return this->tableCount;
}

/**
cronTable 클래스의 virtual 함수 do_job(). 
여기에 실제 할 일의 내용을 작성하면 된다.


*/
int	harvestCron::do_job(char *jobdesc)
{
	ACE_SOCK_Connector conn;
	ACE_SOCK_Stream  peer;
	ACE_Time_Value tv(3, 0);
	ACE_INET_Addr	addr(harvestPort, harvestIPaddr);
	StrStream	httpreq;

	printf("JobDesc: %s\n", jobdesc);

	/*
	CGI::escape(urlbuff, jobdesc);
	httpreq.init(2); // 2 kilobytes buff
	httpreq.rawadd("GET /harvest?expr=");
	httpreq.rawadd(urlbuff);
	httpreq.rawadd(" HTTP/1.1\n\n");

	httpreq.print();
	
	if ( conn.connect(peer, addr) < 0) {
		printf("conn failed!\n");
		return 0;
	}

	ret = peer.send( httpreq.str(), httpreq.len() );
	*/
	URLTrav trav;
	int	reqID=0;
	char *adhome = 0;
	StrStream resStr;
	resStr.init (10*1024);
	
	adhome = getenv("ADHOME");	

	trav.setDebug(0);
	
	reqID = trav.expr(jobdesc);
	resStr.add( "$REQID$ %d\n", reqID);
	
	trav.traverse( resStr, adhome );
	printf("%s\n", resStr.str() );
	return 0;
	
}
