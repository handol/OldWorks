#include "cronTable.h"
#include "windowsDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef WIN32
#include <unistd.h>
#endif

void	cronTable::print() 
{
	int	i;
	printf("table =%X, Max=%d, Count=%d\n",	
		(unsigned)Table, tableSize, tableCount);
	for(i=0; i<tableCount; i++) {
		printf("[%d] DayOfWeek=%d Day=%02d,  %02d:%02d:%02d\n",
			i, Table[i].times[0] , Table[i].times[1] ,
			Table[i].times[2] , Table[i].times[3] , Table[i].times[4] );
	}
}


int	cronTable::allocTable(int _size)
{
	Table = new cronTabType [_size];
	if (Table) tableSize = _size;
	return 0;
}

/**
cronjob 테이블에 job item하나를 추가.

cronjob == char *weekofday, char *day, char *hour, char *minute, char *second, char *jobdesc.
array size = 6
*/
int	cronTable::insertCronJob(char *cronjob[6])
{
	cronTabType *item=0;

	if (tableCount >= tableSize) return -1;

	item = &Table[tableCount];
	
	for (int i=0; i<5; i++) {
		if (strchr(cronjob[i], '*') )
			item->times[i] = CRONTAB_WILDCARD;
		else if (strchr(cronjob[i], '%') ) {
			char *ptr = strchr(cronjob[i], '%');
			item->times[i] = 100 + (short)strtol(ptr+1, 0, 10);
		}
		else
			item->times[i] = (short)strtol(cronjob[i], 0, 10);
	}
	strncpy(item->jobdesc, cronjob[5], sizeof(item->jobdesc)-1);
	item->jobdesc[sizeof(item->jobdesc)-1] = 0;
	tableCount++;


	checkWildCard(item);
	return 1;
}

int	cronTable::fillTable(cronTabType *oneitem)
{
	if (tableCount >= tableSize) return -1;
	checkWildCard(oneitem);
	memcpy(&Table[tableCount], oneitem, sizeof(cronTabType) );
	tableCount++;
	return 0;
}

/**
hour 값이 특정 상수이면, 그 하위 단위는 wild 가 될 수 없다. 
함수 기능 -  wild 가 아닌 특정 값이 정해진 시간 단위의 이하 단위의  wild 는 0 으로 설정해 버린다.
예) ? 일 10 시 ?  분 ==> ? 일 10 시 0  분
*/
int	cronTable::checkWildCard(cronTabType *oneitem)
{
	int	idx;
	for (idx=0; idx<5; idx++)
		if (oneitem->times[idx] != CRONTAB_WILDCARD)
			break;

	for (idx++; idx<5; idx++)
		if (oneitem->times[idx] == CRONTAB_WILDCARD)
			oneitem->times[idx] = 0;
			
	return 0;
}

/**
무한 루프를 돌면서 현재 시간에 매치되는 job을 수행한다.

루프는 stop()의 의해 끝날 수 있다.
또한  setTimer()에 의해 정해진 시간만큼만 수행되고 끝날 수 있다.

job 수행은 do_job() 함수를 호출을 통해 수행한다.
do_job() 함수의 내용은  이 cronTable 클래스를 상속받은 클래스가 각자 정의한다.
*/
int	cronTable::svc()
{
	int	i;
	time_t	start_sec=0;
	time_t	last_sec=0;
	time_t		curr_sec;
	struct tm	*t;

	time(&start_sec);
	while(stopFlag==0) {
			
		time(&curr_sec);
		t = localtime(&curr_sec);

		if (last_sec==curr_sec) {			
			#ifdef WIN32
			Sleep(500); // miliseconds
			#else
			usleep(1000*100); // microseconds
			#endif
			
			continue;
		}
		else	last_sec = curr_sec;
		
		for(i=0; i<tableCount; i++) {
			if (matchTime(t, &Table[i]) )	{
				printTime(t);
				printf("match item =[%d]\n", i);
				do_job( Table[i].jobdesc);
			}
		}

		if (timer && (curr_sec - start_sec) > timer) break;


	}
	return 0;
}

int	cronTable::matchTime(struct tm	*t, cronTabType *crontime)
{
	int	match=0;
	if (crontime->times[4] > 100)
		match = (t->tm_sec % (crontime->times[4] - 100)  == 0);
	else
		match = (crontime->times[4] ==CRONTAB_WILDCARD || t->tm_sec == crontime->times[4] );
	if (!match) return 0;

	if (crontime->times[3] > 100)
		match = (t->tm_sec % (crontime->times[3] - 100)  == 0);
	else
		match = (crontime->times[3] ==CRONTAB_WILDCARD || t->tm_min == crontime->times[3] );
	if (!match) return 0;

	if (crontime->times[2] > 100)
		match = (t->tm_sec % (crontime->times[2] - 100)  == 0);
	else
		match = (crontime->times[2] ==CRONTAB_WILDCARD || t->tm_hour == crontime->times[2] );
	if (!match) return 0;

	if (crontime->times[1] > 100)
		match = (t->tm_sec % (crontime->times[1] - 100)  == 0);
	else
		match = (crontime->times[1] ==CRONTAB_WILDCARD || t->tm_mday == crontime->times[1] );
	if (!match) return 0;

	// Sunday == 0
	match = (crontime->times[0] ==CRONTAB_WILDCARD || t->tm_wday == crontime->times[0] );
	if (!match) return 0;
	
	return 1;
}

void	cronTable::printTime(struct tm	*t)
{
	printf("DayofWeek=%d, %04d/%02d/%02d  %02d:%02d:%02d\n", 
		t->tm_wday, 
		t->tm_year+1900, t->tm_mon+1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);
}
