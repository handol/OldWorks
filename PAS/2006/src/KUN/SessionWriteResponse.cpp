#include "SessionWriteResponse.h"

SessionWriteResponse::SessionWriteResponse()
{
	packetType = PT_SessionWriteResponse;
}

int SessionWriteResponse::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, resultState);

	return 0;
}

int SessionWriteResponse::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, resultState);
	
	return 0;
}





