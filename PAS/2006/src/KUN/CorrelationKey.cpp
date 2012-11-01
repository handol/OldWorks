#include <netdb.h>

#include "PasLog.h"

#include "CorrelationKey.h"

CorrelationKey::CorrelationKey()
{
	_createTime = ACE_OS::gettimeofday();
}

CorrelationKey::CorrelationKey( const MDN& mdn, const ACE_Time_Value& createTime )
{
	_mdn = mdn;
	_createTime = createTime;
}

CorrelationKey::~CorrelationKey(void)
{
}

void CorrelationKey::set( const MDN& mdn, const ACE_Time_Value& createTime )
{
	_mdn = mdn;
	_createTime = createTime;
}

TinyString CorrelationKey::toString() const
{
	time_t cTime = _createTime.sec();

	tm createTM;
	localtime_r(&cTime, &createTM);

	TinyString str;
	str.sprintf("%011lld%04d%02d%02d%02d%02d%02d%05X%d", _mdn.toInt64(), 
		createTM.tm_year+1900, createTM.tm_mon+1, createTM.tm_mday,
		createTM.tm_hour, createTM.tm_min, createTM.tm_sec,
		_createTime.usec(), getHostNo());

	return str;
}

int CorrelationKey::getHostNo() const
{
	char hostname[MAXHOSTNAMELEN];
	if(gethostname(hostname, sizeof(hostname)) < 0)
		return 1;

	PAS_TRACE1("Hostname[%s]", hostname);

	if(strlen(hostname) == 0)
		return 0;

	char lastChar = hostname[strlen(hostname)-1];

	if(!('1' <= lastChar && lastChar <= '9'))
		return 0;

	return static_cast<int>(lastChar - '0');
}
