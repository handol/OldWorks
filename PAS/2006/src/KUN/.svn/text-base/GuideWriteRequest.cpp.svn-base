#include "GuideWriteRequest.h"

GuideWriteRequest::GuideWriteRequest()
{
	packetType = PT_GuideWriteRequest;
}

int GuideWriteRequest::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, guideCode);	

	pushSpliter(ar);
	push(ar, skipGuide);	

	return 0;
}

int GuideWriteRequest::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, guideCode);
	pop(ar, skipGuide);

	return 0;
}









