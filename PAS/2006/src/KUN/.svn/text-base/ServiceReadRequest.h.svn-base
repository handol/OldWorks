#ifndef __SERVICE_READ_REQUEST_H__
#define __SERVICE_READ_REQUEST_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"

/*
PacketType|MDN|ProductCode
(int)|(int64)|(int64)
*/

class ServiceReadRequest : public CommPacket
{
public:
	ServiceReadRequest();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	int64_t productCode;
};

#endif

