//////////////////////////////////////////////////////////////////////
//
// HTTPAgent.h: interface for the CHTTPAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(HTTPAGENT_H__862642C1_84A6_42AB_BBE1_4FF81DB1FE9F__INCLUDED_)
#define HTTPAGENT_H__862642C1_84A6_42AB_BBE1_4FF81DB1FE9F__INCLUDED_

#include "TCPSocket.h"
#include "HTTPParser.h"

class CHTTPAgent 
{

private:
	CHTTPParser m_parser;
	int m_RequestSize;
	int m_ResponseSize;

	CTCPSocket m_conn;
	void clear();
	unsigned char* m_RequestData;
	unsigned char* m_ResponseData;	
public:
	int GetStatus();
	void init();
	int GetResponse(unsigned char* ResData, int iSize);
	int SetRequest(unsigned char* ReqData, int iSize);
	int Send(const char* szServer, unsigned short nPort);
	CHTTPAgent();
	virtual ~CHTTPAgent();
};

#endif // !defined(HTTPAGENT_H__862642C1_84A6_42AB_BBE1_4FF81DB1FE9F__INCLUDED_)
