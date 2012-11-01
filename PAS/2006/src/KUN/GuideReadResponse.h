#ifndef __GUIDE_READ_RESPONSE_H__
#define __GUIDE_READ_RESPONSE_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"
#include "GuideCode.h"

/*
PacketType|MDN|GuideCode|SkipGuide
(int)|(int64)|(int)|(int)
*/

class GuideReadResponse : public CommPacket
{
public:
	GuideReadResponse();
	GuideReadResponse(const GuideReadResponse& rhs);

	GuideReadResponse& operator = (const GuideReadResponse& rhs);

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	GuideCode guideCode;
	bool skipGuide;
};

#endif


