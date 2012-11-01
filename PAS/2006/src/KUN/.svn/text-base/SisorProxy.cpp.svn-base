#include <ace/SOCK_Connector.h>

#include "PasLog.h"
#include "SisorProtocol.h"
#include "Archive.h"
#include "Exception.h"

#include "SisorProxy.h"

SisorProxy::~SisorProxy()
{
	if(isConnected())
		close();
}

void SisorProxy::connect(const host_t& host, int port)
{
	ACE_INET_Addr addr(port, host);
	ACE_SOCK_Connector connector;

	ACE_Time_Value connTimeout(1);
	int conResult = connector.connect(sock, addr, &connTimeout);
	if(conResult < 0)
	{
		ThrowException(ECODE_CONNECT_FAIL);
	}
}

bool SisorProxy::isConnected()
{
	return sock.get_handle() >= 0;
}

void SisorProxy::close()
{
	if(isConnected())
		sock.close();
}

SessionWriteResponse SisorProxy::query(const SessionWriteRequest& req)
{
	SessionWriteResponse res;
	query(res, req);
	return res;
}

GuideReadResponse SisorProxy::query(const GuideReadRequest& req)
{
	GuideReadResponse res;
	query(res, req);
	return res;
}

GuideWriteResponse SisorProxy::query(const GuideWriteRequest& req)
{
	GuideWriteResponse res;
	query(res, req);
	return res;
}

ServiceReadResponse SisorProxy::query(const ServiceReadRequest& req)
{
	ServiceReadResponse res;
	query(res, req);
	return res;
}

void SisorProxy::query(CommPacket& res, const CommPacket& req)
{
	//------------------
	// Send
	//------------------
	send(req);

	//------------------
	// Receive
	//------------------
	ACE_Message_Block recvBuf(MAX_RAW_PACKET_SIZE);

	while(true)
	{
		ACE_Time_Value timeout(1);
		int size = sock.recv(recvBuf.wr_ptr(), recvBuf.space(), &timeout);

		// data receive
		if(size > 0)
		{
			ACE_ASSERT(size <= recvBuf.space());
			recvBuf.wr_ptr(size);
			
			switch(procRecv(res, recvBuf))
			{
			case PROC_RECV_RESULT_SUCCESS:
				PAS_TRACE("Success Receive from Sisor.");
				break;

			case PROC_RECV_RESULT_NEED_MORE_DATA:
				PAS_TRACE("Need more data from Sisor.");
				continue;
				break;

			case PROC_RECV_RESULT_ERROR:
				PAS_NOTICE("Fail Receive from Sisor.");
				ThrowException(ECODE_ERROR);
				break;

			default:
				PAS_NOTICE("Unknown Error in procRecv() called in SisorProxy::query.");
				ThrowException(ECODE_ERROR);
				break;
			}

			// success
			return;
		}
		
		// disconnect
		else if(size == 0)
		{
			ThrowException(ECODE_DISCONNECTED);
		}		

		// recv fail
		else
		{
			ThrowException(ECODE_RECV_FAIL);
		}
	}
}

void SisorProxy::send(const CommPacket& packet)
{
	Archive ar;

	packet.serialize(ar);

	char sendData[MAX_RAW_PACKET_SIZE];
	int sendDataSize = SIP::encode(sendData, sizeof(sendData), ar.front(), ar.size());
	if(sendDataSize < 0)
	{
		PAS_NOTICE("Fail encoding to Sisor Protocol.");
		ThrowException(ECODE_NOT_ENOUGH_SPACE);
	}

	int sendResult = sock.send(sendData, sendDataSize);
	if(sendResult >= 0)
	{
		PAS_TRACE("Send to sisor is success.");
		PAS_TRACE_DUMP("Send Message", sendData, sendDataSize);
	}
	else
	{
		PAS_INFO("Can't send to sisor.");
		ThrowException(ECODE_SEND_FAIL);
	}
}

SisorProxy::ProcRecvResult SisorProxy::procRecv(CommPacket& destPacket, ACE_Message_Block& recvBuf)
{
	SIP::CHK_RESULT endTokenCheckRes = SIP::checkEndToken(recvBuf);
	if(endTokenCheckRes == SIP::CR_OK)
	{
		// 요청 내역 읽기
		int bodyLen = SIP::getBodyLength(recvBuf);
		ACE_Message_Block body(bodyLen);
		int ret = SIP::getBody(body, recvBuf);

		if(ret < 0)
		{
			// 프로토콜을 준수하지만 body 읽기를 실패 했을 경우
			PAS_NOTICE("Can't read body field.");
			return PROC_RECV_RESULT_ERROR;
		}

		// body를 오브젝트로 변환
		if(unserialize(destPacket, body) < 0)
		{
			return PROC_RECV_RESULT_ERROR;
		}
		else
		{
			return PROC_RECV_RESULT_SUCCESS;
		}
	}

	// 데이터를 더 수신해야 할 경우
	else if(endTokenCheckRes == SIP::CR_NOT_ENOUGH)
	{
		if(SIP::checkStartToken(recvBuf) == SIP::CR_NOT_ENOUGH)
		{
			PAS_INFO("Not enough received data for parse start token");
		}
		else if(SIP::checkLength(recvBuf) == SIP::CR_NOT_ENOUGH)
		{
			PAS_INFO("Not enough received data for parse length");
		}
		else if(SIP::checkBody(recvBuf) == SIP::CR_NOT_ENOUGH)
		{
			PAS_INFO("Not enough received data for parse body");
		}
		else
		{
			PAS_INFO("Not enough received data for parse end token");
		}

		return PROC_RECV_RESULT_NEED_MORE_DATA;
	}

	// 잘 못된 데이터가 수신됐을 때
	else
	{
		if(SIP::checkStartToken(recvBuf) == SIP::CR_BAD)
		{
			PAS_INFO("Wrong data received, incorrect start token");
		}
		else if(SIP::checkLength(recvBuf) == SIP::CR_BAD)
		{
			PAS_INFO("Wrong data received, incorrect length field");
		}
		else
		{
			PAS_INFO("Wrong data received, incorrect end token");
		}

		return PROC_RECV_RESULT_ERROR;
	}
}

int SisorProxy::unserialize(CommPacket& destPacket, ACE_Message_Block& rawResponse)
{
	Archive ar;

	try
	{
		ar.set(rawResponse.rd_ptr(), rawResponse.length());

		destPacket.unserialize(ar);
	}
	catch (Exception e)
	{
		PAS_NOTICE1("Response unserialize fail. Because of %s", e.toString());
		PAS_NOTICE_DUMP("Received data from Sisor", ar.front(), ar.size());		
		
		
		return -1;
	}

	return 0;
}
