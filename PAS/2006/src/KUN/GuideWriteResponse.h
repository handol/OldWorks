#ifndef __GUIDE_WRITE_RESPONSE_H__
#define __GUIDE_WRITE_RESPONSE_H__

#include "CommPacket.h"
#include "PacketType.h"
#include "MDN.h"
#include "GuideCode.h"

/*
PacketType|MDN|GuideCode|SkipGuide
(int)|(int64)|(int)|(int)
*/

class GuideWriteResponse : public CommPacket
{
public:
	GuideWriteResponse();

	const GuideWriteResponse& operator = (const GuideWriteResponse& rhs);

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

public:
	bool resultState;
};

#endif


