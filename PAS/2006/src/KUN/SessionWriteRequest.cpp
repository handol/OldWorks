#include "PacketType.h"
#include "SessionWriteRequest.h"

SessionWriteRequest::SessionWriteRequest()
{
	packetType = PT_SessionWriteRequest;
}

int SessionWriteRequest::serialize(Archive& ar) const
{
	CommPacket::serialize(ar);
		
	pushSpliter(ar);
	push(ar, lastConn);	

	pushSpliter(ar);
	push(ar, lastClose);	
	
	pushSpliter(ar);
	push(ar, connCount);	
	
	pushSpliter(ar);
	push(ar, tranCount);	

	pushSpliter(ar);
	push(ar, lastAddr);	
	
	pushSpliter(ar);
	push(ar, lastAuthTime);	

	pushSpliter(ar);
	push(ar, lastSantaTime);

	return 0;
}

int SessionWriteRequest::unserialize(Archive& ar)
{
	CommPacket::unserialize(ar);

	pop(ar, lastConn);
	pop(ar, lastClose);
	pop(ar, connCount);
	pop(ar, tranCount);
	pop(ar, lastAddr);
	pop(ar, lastAuthTime);
	pop(ar, lastSantaTime);
	
	return 0;
}







