//////////////////////////////////////////////////////////////////////
//
// HTTPParser.cpp: implementation of the CHTTPParser class.
//
//////////////////////////////////////////////////////////////////////

#include "HTTPParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define HTTPPARSER_HEADER_SIZE	65536
#define HTTPPARSER_BODY_SIZE	4 * (1024*1024)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPParser::CHTTPParser()
{
	m_Header = new unsigned char[HTTPPARSER_HEADER_SIZE];
	m_Body = new unsigned char[HTTPPARSER_BODY_SIZE];

	memset(m_Header, 0x00, HTTPPARSER_HEADER_SIZE);
	memset(m_Body, 0x00, HTTPPARSER_BODY_SIZE);

	m_bHeader = false;
	m_ContentLength = -1;
	m_BodySize = 0;
	m_HeaderSize = 0;

	m_pHeader = NULL;
	m_HeaderpPos = 0;

	memset(m_szStatus, 0x00, HTTPPARSER_STATUS_SIZE);
}

CHTTPParser::~CHTTPParser()
{
	if ( m_Header )
	{
		delete[] m_Header;
		m_Header = NULL;
	}

	if ( m_Body )
	{
		delete[] m_Body;
		m_Body = NULL;
	}
}

void CHTTPParser::init()
{
	memset(m_Header, 0x00, HTTPPARSER_HEADER_SIZE);
	memset(m_Body, 0x00, HTTPPARSER_BODY_SIZE);

	m_bHeader = false;
	m_ContentLength = -1;
	m_BodySize = 0;
	m_HeaderSize = 0;

	m_pHeader = NULL;
	m_HeaderpPos = 0;

	memset(m_szStatus, 0x00, HTTPPARSER_STATUS_SIZE);
}

bool CHTTPParser::isGetHeader()
{
	return m_bHeader;
}

int CHTTPParser::GetContentLength()
{
	return m_ContentLength;
}

int CHTTPParser::GetHeaderSize()
{
	return m_HeaderSize;
}

int CHTTPParser::GetBodySize()
{
	return m_BodySize;
}

const char* CHTTPParser::GetStatus()
{
	return m_szStatus;
}

int CHTTPParser::SetAddData(const char *pszData, int iSize)
{
	if ( pszData == NULL || iSize < 1 )
	{
		return 0;
	}

	if ( m_bHeader == false ) //헤더에 저장
	{
		if ( m_HeaderSize + iSize >= HTTPPARSER_HEADER_SIZE )
		{
			return -1;
		}

		memcpy(m_Header+m_HeaderSize, pszData, iSize);
		m_HeaderSize += iSize;

		m_bHeader = isCheckHeader();

		if ( m_bHeader )
		{
			m_ContentLength = ContentLength();
			
			if ( m_ContentLength > 0 && m_HeaderSize > m_HeaderpPos )
			{
				memcpy(m_Body, m_pHeader, m_HeaderSize - m_HeaderpPos);	
				m_BodySize = m_HeaderSize - m_HeaderpPos;
			}

			m_HeaderSize = m_HeaderpPos;

			m_Header[m_HeaderSize] = 0x00;
		}
	}
	else	//Body에 저장
	{
		if ( m_ContentLength == -1 )
		{
			return 0;
		}

		if ( m_ContentLength == 0 || HTTPPARSER_BODY_SIZE < m_BodySize + iSize )
		{
			return 0;
		}

		memcpy(m_Body + m_BodySize, pszData, iSize);
		m_BodySize += iSize;
	}

	return 1;
}

bool CHTTPParser::isCheckHeader()
{
	bool bCheck = false;

	for ( int i = 0; i < m_HeaderSize; i++ )
	{
		if ( !memcmp((void*)(m_Header+i), "\r\n\r\n", 4) || !memcmp((void*)(m_Header+i), "\n\n", 2) )
		{	
			m_pHeader = (char*)m_Header + i + strlen("\r\n\r\n");
			m_HeaderpPos = i + strlen("\r\n\r\n");
			bCheck = true;
			break;
		}

		if ( !memcmp((void*)(m_Header+i), "\n\n", 2) )
		{	
			m_pHeader = (char*)m_Header+i+strlen("\n\n");
			m_HeaderpPos = i + strlen("\n\n");
			bCheck = true;
			break;
		}

	}

	return bCheck;
}

int CHTTPParser::ContentLength()
{
	int iRet = -1;
	int iSize = strlen("Content-Length:");
	char buff[1024] = {0};

	char *p = NULL, *p1= NULL;
	if ( (p = strstr((char*)m_Header, "Content-Length:")) != NULL  )
	{
		if ( (p1 = strstr(p, "\r\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize);
			iRet = atoi(buff);
		}
		else if ( (p1 = strstr(p, "\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize);
			iRet = atoi(buff);
		}
	}
	else if( (p = strstr((char*)m_Header, "Content-length:")) != NULL )
	{
		if ( (p1 = strstr(p, "\r\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize);
			iRet = atoi(buff);
		}
		else if ( (p1 = strstr(p, "\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize);
			iRet = atoi(buff);
		}
	}
	else if( (p = strstr((char*)m_Header, "content-length:")) != NULL )
	{
		if ( (p1 = strstr(p, "\r\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize);
			iRet = atoi(buff);
		}
		else if ( (p1 = strstr(p, "\n")) != NULL )
		{
			if (  p1 - p - iSize <= 0 )
			{
				return -1;
			}

			strncpy(buff, p+iSize, p1 - p - iSize) ;
			iRet = atoi(buff);
		}
	}
	else
	{
		//Nothing
	}

	return iRet;
}

unsigned char* CHTTPParser::GetHeaderData()
{
	return m_Header;
}

unsigned char* CHTTPParser::GetBodyData()
{
	return m_Body;
}

