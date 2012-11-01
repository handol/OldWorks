#ifndef __CORRELATION_KEY_H__
#define __CORRELATION_KEY_H__

#include <time.h>

#include <ace/Time_Value.h>

#include "MDN.h"

class CorrelationKey
{
public:
	CorrelationKey();
	CorrelationKey(const MDN& mdn, const ACE_Time_Value& createTime);
	~CorrelationKey(void);

	/// Correlation key를 문자열로 반환한다.
	/**
	 * @return correlation key 
	 * @exception NONE
	 *
	 * @date 2007/03/23
	 * @author Sehoon Yang
	 *
	 * MDN, Create time, Host name 을 이용해서 생성된 correlation key 를 반환한다.
	 * 이 함수를 사용하기 이전에 생성자나 set 함수를 통해 mdn과 createTime이 
	 * 설정되어 있어야 정상적인 correlation key를 얻을 수 있다.
	 **/
	TinyString toString() const;

	void set(const MDN& mdn, const ACE_Time_Value& createTime);

private:
	/// Host name 을 통해 pasgw 의 서버번호를 알아온다.
	/**
	 * @return PAS Gateway server number
	 * @exception NONE
	 *
	 * @date 2007/03/23
	 * @author Sehoon Yang
	 *
	 * PAS Gateway 사용서버의 hostname은 pasgw1, pasgw2, ... , pasgw6 으로 구성되어 있다.
	 * hostname의 마지막 값을 int 형으로 반환한다.
	 * 만약 hostname을 통해 번호를 알 수 없을 경우 0을 리턴한다.
	 **/
	int getHostNo() const;

private:
	MDN _mdn;
	ACE_Time_Value _createTime;
};

#endif
