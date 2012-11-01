#include "ServiceReadResponse.h"

ServiceReadResponse::ServiceReadResponse()
{
	packetType = PT_ServiceReadResponse;
}

int ServiceReadResponse::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, productCode);

	pushSpliter(ar);
	push(ar, blockProduct);

	return 0;
}

int ServiceReadResponse::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, productCode);
	pop(ar, blockProduct);

	return 0;
}









