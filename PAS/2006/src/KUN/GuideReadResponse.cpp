#include "GuideReadResponse.h"

GuideReadResponse::GuideReadResponse()
{
	packetType = PT_GuideReadResponse;
	guideCode = GCODE_Unknown;
	skipGuide = true;
}

GuideReadResponse::GuideReadResponse(const GuideReadResponse& rhs)
{
	*this = rhs;
}

GuideReadResponse& GuideReadResponse::operator = (const GuideReadResponse& rhs)
{
	guideCode = rhs.guideCode;
	skipGuide = rhs.skipGuide;
	return *this;
}

int GuideReadResponse::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, guideCode);

	pushSpliter(ar);
	push(ar, skipGuide);

	return 0;
}

int GuideReadResponse::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, guideCode);
	pop(ar, skipGuide);

	return 0;
}















