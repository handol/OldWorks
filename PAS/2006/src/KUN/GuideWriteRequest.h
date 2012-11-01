#ifndef __GUIDE_WRITE_REQUEST_H__
#define __GUIDE_WRITE_REQUEST_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"
#include "GuideCode.h"

/*
PacketType|MDN|GuideCode
(int)|(int64)|(int)
*/
class GuideWriteRequest : public CommPacket
{
public:
	GuideWriteRequest();

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	GuideCode guideCode;
	bool skipGuide;
};

#endif


