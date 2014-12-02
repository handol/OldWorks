#ifndef CRONTABLE_H
#define	CRONTABLE_H

#include <string.h>

#define	CRONTAB_WILDCARD	(99)
#define	MAXLEN_JOBDESC	(1023)

/**
����:

�� Ŭ���� ( cronTable ) �� ��ӹ޴� �� Ŭ������ �����Ͽ� ����ϸ� �ȴ�.

�� Ŭ���������� virtual �Լ��� loadTable(), do_job() �Լ��� �����Ͽ��� �Ѵ�.
*/

/*
cronjob�� �ð��� ���� :

cronjob�� �ð��� ������ ���� �� �� �� �� -- 5 ���� ������ �� �ִ�.
�ð����� ���� �ܿ��� '*', '%' ��  Ư�� ��ȣ�� ����� �� �ִ�.
���ϰ��� 0 �� �Ͽ����� �ǹ��ϸ�, 6�� ������� �ǹ��Ѵ�.

 	* : wild card.
 	% : �ݺ�. ��) %10 -- 10 ���� ������ ���� True. 10 �и���. 10 �ʸ���


* -- wild card : ���������δ� 99 ��.
%alpha -- ���������� 100 + alpha ������ ����. alpha �ֱ�

*/

/*

����  ��  �� ��  ��  ��
 *     *   *  10  0  0
 *     *   *   *  30 0
 *     *   *   0   %10 0

 
*/

typedef struct _cronTabType
{
	/*
	short	dayofweek; // ���� 
	short	day;
	short	hour;
	short	minute;
	short	second;
	*/
	/* array  ���·� ���� */
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
		int	timer; // ������ �ð� ��ŭ�� svc() �� �����ϱ� ����.
		cronTabType *Table;
		int	tableCount;
		int	tableSize;
		int	stopFlag;
		
};

#endif
