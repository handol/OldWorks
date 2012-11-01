#include "GuideReadRequest.h"

GuideReadRequest::GuideReadRequest()
{
	packetType = PT_GuideReadRequest;
}

int GuideReadRequest::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, reqURL);	

	return 0;
}

int GuideReadRequest::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, reqURL);

	return 0;
}









