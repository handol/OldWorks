/**
@file PhoneTrace.h

@brief 특정 단말 번호 파일 로그 관리

trace.acl 에 단말번호를 추가한다.

@author KTF
@date 2006.09.12
*/
#ifndef PHONETRACE_H
#define PHONETRACE_H

#include "Common.h"

#include <map>

using namespace std;

class PhoneTraceInfo
{
	public:
	intMDN_t mdn;
	time_t	starttime;
	time_t	endtime;
	int	loglevel;		
};

typedef map<intMDN_t, PhoneTraceInfo *> PhoneTraceList;
class PhoneTrace
{
	public:
		
		PhoneTrace()
		{
		}
		
		
		~PhoneTrace()
		{
			traceList.clear();
		}

		bool isTrace(char * MDN);
		bool isTrace(intMDN_t MDN);
		PhoneTraceInfo *search(intMDN_t MDN);
		int	add(char * MDN);
		int	add(intMDN_t MDN);
		int	add(PhoneTraceInfo *info);
		int	size()
		{
			return traceList.size();
		}
		
		void	clear();
		void    prn(StrStream_t  &out);
		
		
	private:
		//map<intMDN_t, int> traceList;
		PhoneTraceList traceList;

	
};
#endif
