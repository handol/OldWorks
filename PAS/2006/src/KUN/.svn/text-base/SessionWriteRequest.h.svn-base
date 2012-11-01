#ifndef __SESSION_WRITE_REQUEST_H__
#define __SESSION_WRITE_REQUEST_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"

/*
PacketType|MDN|LastConn|LastClose|ConnCount|TranCount|LastAddr|LastAuthTime|LastSantaTime
(int)|(int64)|(timestamp)|(timestamp)|(int)|(int)|(char[15])|(timestamp)|(timestamp)
*/

class SessionWriteRequest : public CommPacket
{
public:
	SessionWriteRequest();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	time_t lastConn;
	time_t lastClose;
	int connCount;
	int tranCount;
	ip_t lastAddr;
	time_t lastAuthTime;
	time_t lastSantaTime;
};

#endif
