#include "GuideWriteResponse.h"

GuideWriteResponse::GuideWriteResponse()
{
	packetType = PT_GuideWriteResponse;
}

const GuideWriteResponse& GuideWriteResponse::operator = (const GuideWriteResponse& rhs)
{
	ACE_ASSERT(_spliter == rhs._spliter);
	ACE_ASSERT(packetType == rhs.packetType);

	mdn = rhs.mdn;
	resultState = rhs.resultState;

	return *this;
}

int GuideWriteResponse::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, resultState);

	return 0;
}

int GuideWriteResponse::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, resultState);

	return 0;
}















