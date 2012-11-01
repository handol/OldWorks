#include "ServiceReadRequest.h"

ServiceReadRequest::ServiceReadRequest()
{
	packetType = PT_ServiceReadRequest;
}

int ServiceReadRequest::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);

	pushSpliter(ar);
	push(ar, productCode);	

	return 0;
}

int ServiceReadRequest::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, productCode);
	
	return 0;
}



