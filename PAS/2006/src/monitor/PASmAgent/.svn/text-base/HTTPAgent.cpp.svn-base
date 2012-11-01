//////////////////////////////////////////////////////////////////////
//
// HTTPAgent.cpp: implementation of the CHTTPAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "HTTPAgent.h"
#include "HTTPParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define HTTP_HEADER_SIZE	256*256
#define HTTP_BODY_SIZE		1024*1024*4
#define HTTP_MAX_SIZE	(HTTP_HEADER_SIZE) + (HTTP_BODY_SIZE)
#define SOCKET_BUFF_SIZE	8196


CHTTPAgent::CHTTPAgent()
{
	m_RequestData = new unsigned char[HTTP_MAX_SIZE];	
	m_ResponseData = new unsigned char[HTTP_MAX_SIZE];	
        m_RequestSize = 0;
        m_ResponseSize = 0;

	init();
}

CHTTPAgent::~CHTTPAgent()
{
	clear();
}

void CHTTPAgent::clear()
{
	if ( m_RequestData )
	{
		delete[] m_RequestData;
		m_RequestData = NULL;
	}

	if ( m_ResponseData )
	{
		delete[] m_ResponseData;
		m_ResponseData = NULL;
	}
}

void CHTTPAgent::init()
{
	memset(m_RequestData, 0x00, HTTP_MAX_SIZE);
	memset(m_ResponseData, 0x00, HTTP_MAX_SIZE);
        m_RequestSize = 0;
        m_ResponseSize = 0;
	m_conn.Close();
	m_conn.init();
}

int CHTTPAgent::GetResponse(unsigned char* ResData, int iSize)
{
	if ( iSize )
	{

	}

	return 1;
}

int CHTTPAgent::SetRequest(unsigned char* ReqData, int iSize)
{
	if ( ReqData == NULL || iSize < 0 )
	{
		return 0;
	}

	memcpy(m_RequestData, ReqData, iSize);
	m_RequestSize = iSize;

	return 1;
}

int CHTTPAgent::Send(const char* szServer, unsigned short nPort)
{
	int iRet = 0;
	iRet = m_conn.Connect(szServer, nPort);
	
#ifdef DEBUG
	cerr << "Connect:" << iRet << endl;
#endif
	if ( iRet != 0 )
	{
		return -1;
	}

	int nByte = 0;
	int nSize = m_RequestSize;

	while( m_RequestSize > nByte) 
	{
		int ret;
		ret = m_conn.Send((char*)m_RequestData+nByte, nSize);
		
#ifdef DEBUG
	cerr << "Send:" << ret << endl;
#endif
		if ( ret < 0 )
		{
			return -2;
		}

		nByte += ret;
		nSize -= ret;
	}

#ifdef DEBUG
	cerr << "Total Send:" << nByte << endl;
	cerr << "DATA\n" << m_RequestData << "\n" << endl;
#endif
	nByte = 0;
	nSize = 0;

	m_parser.init();

	while ( HTTP_MAX_SIZE > nByte + SOCKET_BUFF_SIZE)
	{
		int ret;
		char buff[SOCKET_BUFF_SIZE] = {0};
		ret = m_conn.Recv(buff, SOCKET_BUFF_SIZE, 10, 0); 

#ifdef DEBUG_
        cerr << "Recv:" << ret << endl;
	cerr << m_ResponseData << endl;
#endif
		if ( ret < 0 )
		{
#ifdef DEBUG
        cerr << "Recv Error...-" << ret << endl;
#endif
			return -4;
		}
		
		memcpy((char*)m_ResponseData+nByte, buff, ret);
		nByte += ret;
		
		m_parser.SetAddData(buff, ret);
		
		if ( m_parser.GetBodySize() >= m_parser.GetContentLength() )
		{
#ifdef DEBUG
        cerr << "Total Recv:" << nByte << endl;
        cerr << "GetHeaderSize:" << m_parser.GetHeaderSize() << endl;
        cerr << "GetBodySize:" << m_parser.GetBodySize() << endl;
        cerr << "GetContentLength:" << m_parser.GetContentLength() << endl;
#endif
			break;
		}
	}

#ifdef DEBUG
	cerr << "Header+Body=" <<  m_parser.GetHeaderSize() << "+" <<  m_parser.GetBodySize() << "=";
	cerr << m_parser.GetHeaderSize() + m_parser.GetBodySize() << endl;
	//cerr << m_parser.GetHeaderData() << endl;
	cerr << m_parser.GetBodyData() << endl;
#endif	
	nSize = m_ResponseSize;

	return 1;
}

int CHTTPAgent::GetStatus()
{
	int iRet = 0;

	if ( m_parser.GetHeaderSize() > 10 )
	{
		char buff[8196] = {0};

		char szhttp[128] = {0};
		char szstatus[128] = {0}; 
		char szvalue[128] = {0};

		char *p1 = NULL;
		char *p2 = NULL;
		
		p1 = (char*)m_parser.GetHeaderData();

		if ( (p2 = strstr((char*)m_parser.GetHeaderData(), "\r\n")) != NULL )
		{
			strncpy(buff, (char*)m_parser.GetHeaderData(), (p2-p1)); 
			sscanf(buff, "%s %s %s", szhttp, szstatus, szvalue);

			return atoi(szstatus);
		} 
		
	} 

	return iRet;
}
