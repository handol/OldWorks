#ifndef CRONTABLE_H
#define	CRONTABLE_H

#include <string.h>

#define	CRONTAB_WILDCARD	(99)
#define	MAXLEN_JOBDESC	(1023)

/**
사용법:

이 클래스 ( cronTable ) 를 상속받는 새 클래스를 정의하여 사용하면 된다.

새 클래스에서는 virtual 함수인 loadTable(), do_job() 함수를 구현하여야 한다.
*/

/*
cronjob의 시간값 설정 :

cronjob의 시간값 설정은 요일 일 시 분 초 -- 5 가지 설정할 수 있다.
시간값은 숫자 외에도 '*', '%' 의  특수 기호를 사용할 수 있다.
요일값은 0 이 일요일을 의미하며, 6이 토요일을 의미한다.

 	* : wild card.
 	% : 반복. 예) %10 -- 10 으로 나누어 지면 True. 10 분마다. 10 초마다


* -- wild card : 내부적으로는 99 값.
%alpha -- 내부적으로 100 + alpha 값으로 저장. alpha 주기

*/

/*

요일  월  일 시  분  초
 *     *   *  10  0  0
 *     *   *   *  30 0
 *     *   *   0   %10 0

 
*/

typedef struct _cronTabType
{
	/*
	short	dayofweek; // 요일 
	short	day;
	short	hour;
	short	minute;
	short	second;
	*/
	/* array  형태로 변경 */
	short	times[5];
	int	jobtype;
	char	jobdesc[MAXLEN_JOBDESC+1]; 
} cronTabType;

class cronTable 
{
	public:
		
		cronTable() {
			stopFlag = 0;
			Table = 0;
			tableSize =  0;
			timer = 0;
			initTable();
		}

		~cronTable() {
			if (Table) delete [] Table;
		}

		void	initTable() {
			stopFlag = 0;
			tableCount = 0;
			if (Table) {
				memset(Table, 0, sizeof(cronTabType) * tableSize);
			}
		}
		
		int	allocTable(int _size);
		int	fillTable(cronTabType *oneitem);	
		int	insertCronJob(char *cronjob[6]);
		int	checkWildCard(cronTabType *oneitem);
		int	svc();
		void	stop() {
			stopFlag = 1;
		}
		void	setTimer(int _timer) {
			timer = _timer;
		}
		void	print();
		
		virtual int	loadTable(int _tabsize)=0;
		virtual int	do_job(char *jobdesc)=0;

		static	int	matchTime(struct tm	*t, cronTabType *crontime);
		static	void	printTime(struct tm	*t);
	protected:
		int	timer; // 설정된 시간 만큼만 svc() 를 수행하기 위해.
		cronTabType *Table;
		int	tableCount;
		int	tableSize;
		int	stopFlag;
		
};

#endif
