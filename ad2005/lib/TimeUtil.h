#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {

public:
static	char	yymmdd[12];
static	char *get_yymmdd(char *nowtime=0);

static	void get_nowtime_str_simple(char *nowtime);

static	void get_curr_time_str(char *nowtime);

static	void get_curr_time_str2(char *nowtime);

static	void get_time_str(char *timeval, long t_val);

static	void sleep_msec(int	m_sec);
};

#endif
