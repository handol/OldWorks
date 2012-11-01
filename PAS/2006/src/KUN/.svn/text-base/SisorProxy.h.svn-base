#ifndef __SISOR_PROXY_H__
#define __SISOR_PROXY_H__

#include <ace/SOCK_Stream.h>

#include "Common.h"

#include "CommPacket.h"
#include "SessionWriteRequest.h"
#include "SessionWriteResponse.h"
#include "ServiceReadRequest.h"
#include "ServiceReadResponse.h"
#include "GuideReadRequest.h"
#include "GuideReadResponse.h"
#include "GuideWriteRequest.h"
#include "GuideWriteResponse.h"

class SisorProxy
{
public:
	enum ProcRecvResult
	{
		PROC_RECV_RESULT_SUCCESS,
		PROC_RECV_RESULT_ERROR,
		PROC_RECV_RESULT_NEED_MORE_DATA
	};

	~SisorProxy();

	void connect(const host_t& host, int port);
	bool isConnected();
	void close();
	
	SessionWriteResponse query(const SessionWriteRequest& req);
	GuideReadResponse query(const GuideReadRequest& req);
	GuideWriteResponse query(const GuideWriteRequest& req);
	ServiceReadResponse query(const ServiceReadRequest& req);

private:
	void query(CommPacket& res, const CommPacket& req);
	void send(const CommPacket& packet);
	
	ProcRecvResult procRecv(CommPacket& destPacket, ACE_Message_Block& recvBuf);
	int unserialize(CommPacket& destPacket, ACE_Message_Block& rawResponse);

private:
	ACE_SOCK_Stream sock;
};

#endif




