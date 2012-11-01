#ifndef __SERVICE_READ_RESPONSE_H__
#define __SERVICE_READ_RESPONSE_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"

/*
PacketType|MDN|ProductCode|BlockProduct
(int)|(int64)|(int64)|(int)
*/

class ServiceReadResponse : public CommPacket
{
public:
	ServiceReadResponse();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	int64_t productCode;
	bool blockProduct;
};

#endif


