#ifndef __SESSION_WRITE_RESPONSE_H__
#define __SESSION_WRITE_RESPONSE_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"

/*
PacketType|MDN|ResultState
(int)|(int64)|(int)
*/
class SessionWriteResponse : public CommPacket
{
public:
	SessionWriteResponse();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	bool resultState;
};

#endif

