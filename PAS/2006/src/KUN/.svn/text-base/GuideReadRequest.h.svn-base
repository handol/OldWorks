#ifndef __GUIDE_READ_REQUEST_H__
#define __GUIDE_READ_REQUEST_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"
#include "GuideCode.h"

/*
PacketType|MDN|reqURL
(int)|(int64)|(char string)
*/
class GuideReadRequest : public CommPacket
{
public:
	GuideReadRequest();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	url_t reqURL;
};

#endif


