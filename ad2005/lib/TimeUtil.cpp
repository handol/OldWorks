#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <Time.h>
#include <Winsock2.h>
#else

#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#endif

#include "TimeUtil.h"
#if 0
long curr_time()
{
	time_t		t_val;
	return (time(&t_val));
}

/* 
count the minutes from a given time to the current time 
*/
count_minutes(int hour, int minute)
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);

	return ( (t->tm_hour - hour) * 60 + (t->tm_min - minute) );
}

prn_date()
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	printf("%2d:%2d [%d]\n", t->tm_hour, t->tm_min, t->tm_wday);

}

is_weekends()
{
	time_t		t_val;
	struct tm	*t;
	time(&t_val);
	t = localtime(&t_val);

	if (t->tm_wday == 0 || t->tm_wday == 6) /* Sunday or Saturday */
		return 1;
	else	return 0;
}

/* 
count the days from a given date to today
*/
count_days(int year, int month, int day)
{
	time_t		old;
	struct tm	s;
	time_t		t_val;
	
	bzero(&s, sizeof(s));
	s.tm_year = year - 1900;
	s.tm_mon = month - 1;
	s.tm_mday = day;
	old = (long) mktime(&s);

	time(&t_val);
	return ( (int) ((long)t_val - (long)old) / (60*60*24) );
}

count_days_not_correct(int year, int month, int day)
{
	time_t		t_val;
	struct tm	*t;
	int	days;
	
	time(&t_val);
	t = localtime(&t_val);

	days = 
	(t->tm_year+1900 - year) * 365 + 
	(t->tm_mon+1 - month) * 31 - (t->tm_mon+1 - month)/2 +
	(t->tm_mday - day);
	return (days);
}

int	get_day_of_month()
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	return t->tm_mday;
}

#endif

char	TimeUtil::yymmdd[12] = {0};

char * TimeUtil::get_yymmdd(char *nowtime)
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	if (nowtime==0) 	nowtime = TimeUtil::yymmdd;
	
	sprintf(nowtime, "%04d%02d%02d", 
		t->tm_year+1900, t->tm_mon+1,
		t->tm_mday);
	return nowtime;
}

void TimeUtil::get_nowtime_str_simple(char *nowtime)
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	sprintf(nowtime, "%04d%02d%02d%02d%02d%02d", 
		t->tm_year+1900, t->tm_mon+1,
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

void TimeUtil::get_curr_time_str(char *nowtime)
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	sprintf(nowtime, "%04d/%02d/%02d %02d:%02d:%02d", 
		t->tm_year+1900, t->tm_mon+1,
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

void TimeUtil::get_curr_time_str2(char *nowtime)
{
	time_t		t_val;
	time(&t_val);
	strftime( nowtime, 24, "%Y/%m/%d %H:%M:%S", localtime(&t_val));
}

void TimeUtil::get_time_str(char *timeval, long t_val)
{
	strftime( timeval, 24, "%Y/%m/%d %H:%M:%S", 
			localtime((time_t *)&t_val));
}

void TimeUtil::sleep_msec(int	m_sec)
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = m_sec;
    select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &timeout);
}

#if 0
future_time(hour, minute, sec, future_t)
int	hour, minute, sec;
char		*future_t;
{
	time_t		t_val;
	struct tm	*t;
	long	add_t;
	
	time(&t_val);
	t = localtime(&t_val);
#if DEBUG
	PRN("future_time(): %s", asctime(t));
#endif

	add_t = hour * 3600 + minute * 60 + sec;
	t_val += add_t;

	t = localtime(&t_val);
	sprintf(future_t, "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1,
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	
#if DEBUG
	PRN("Time : %s", asctime(t));
#endif
}	


str_2_time(char *timestr, long *tval)
{
	struct tm	s;
	int     yy,mm,dd,hh,mi,ss;

	*tval = 0;
	sscanf(timestr,"%d/%d/%d %d:%d:%d", &yy,&mm,&dd,&hh,&mi,&ss);
#if DEBUG
	PRN("str_2_time(): time= %04d/%02d/%02d %02d:%02d:%02d\n",
			yy, mm, dd, hh, mi, ss);
#endif
	s.tm_year = yy - 1900;
	s.tm_mon = mm - 1;
	s.tm_mday = dd;
	s.tm_hour = hh;
	s.tm_min = mi;
	s.tm_sec = ss;
	*tval = (long) mktime(&s);
}

/*----------------------------------------------------------
newtime = oldtime + hours + minutes + secs
------------------------------------------------------------*/
mk_new_time(newtime, sec, oldtime)
char	*newtime;
int	sec;
char	*oldtime;
{
	time_t		t_val;
	struct tm	s, *t;
	int     yy,mm,dd,hh,mi,ss;

	sscanf(oldtime,"%d/%d/%d %d:%d:%d", &yy,&mm,&dd,&hh,&mi,&ss);
#if DEBUG
	PRN("mk_new_time(): old time= %04d/%02d/%02d %02d:%02d:%02d\n",
		 yy, mm, dd, hh, mi, ss);
#endif
	s.tm_year = yy - 1900;
	s.tm_mon = mm - 1;
	s.tm_mday = dd;
	s.tm_hour = hh;
	s.tm_min = mi;
	s.tm_sec = ss;

	t_val = mktime(&s);
	t_val += sec;

	t = localtime(&t_val);
	sprintf(newtime, "%04d/%02d/%02d %02d:%02d:%02d", 
		t->tm_year+1900, t->tm_mon+1,
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	
#if DEBUG
	PRN("mk_new_time(): new time= %s\n", newtime);
#endif
}

static	time_t	first,	second;

reset_sec()
{
	time(&first);
}

sec_ellapsed(max_gap)
int	max_gap;
{
	int	gap;
	time(&second);
	gap = (int)(second - first);
	if (gap >= max_gap) {
		return 1;
	}
	else	return 0;
}	


m_sec_ellapsed(msec)
int	msec;
{
	static long sec, usec;
	long	n_sec, n_usec;
	int	c, d, gap;
	struct	timeval now;
	struct	timezone tz;

	gettimeofday(&now, &tz);
	n_sec = now.tv_sec;
	n_usec = now.tv_usec;	
	c = (int) ((long)n_sec - (long)sec);
	d = (int) ((long)n_usec - (long)usec);
	gap = c * 1000000 + d;

	/*
	PRN("Now== sec : %d, usec : %d, msec: %d\n", n_sec, n_usec, n_usec/1000);
	PRN("Diff= sec : %d, usec : %d, msec: %d, gap= %d\n", c, d, d/1000, gap);
	*/

	if (sec == 0 || gap >= msec) {
		sec = n_sec;
		usec = n_usec;
		return 1;
	}
	else {
		return 0;
	}
}

sleep_msec(int	m_sec)
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = m_sec;
    select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &timeout);
}

sleep_100th(int	n)
{
	int	i;
	for(i=0; i<n; i++)
		sleep_msec(10000);
}

sleep_secs(int	n)
{
	int	i;
	for(i=0; i<n; i++) sleep(1);
}
#endif
