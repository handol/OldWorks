//////////////////////////////////////////////////////////////////////
//
// HTTPParser.h: interface for the CHTTPParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(HTTPPARSER_H__00F6BB8C_4BC6_49D4_86F0_4DE72A4DDC31__INCLUDED_)
#define HTTPPARSER_H__00F6BB8C_4BC6_49D4_86F0_4DE72A4DDC31__INCLUDED_

#define HTTPPARSER_STATUS_SIZE	32

class CHTTPParser  
{
public:
	unsigned char* GetBodyData();
	unsigned char* GetHeaderData();
	int SetAddData(const char* pszData, int iSize);
	const char* GetStatus();
	void init();
	int GetBodySize();
	int GetHeaderSize();
	int GetContentLength();
	bool isGetHeader();
	CHTTPParser();
	virtual ~CHTTPParser();

private:
	int ContentLength();
	bool isCheckHeader();
	unsigned char* m_Header;
	unsigned char* m_Body;

	char *m_pHeader;
	int m_HeaderpPos;
	bool m_bHeader;
	int m_ContentLength;
	int m_BodySize;
	int m_HeaderSize;

	char m_szStatus[HTTPPARSER_STATUS_SIZE];
};

#endif // !defined(HTTPPARSER_H__00F6BB8C_4BC6_49D4_86F0_4DE72A4DDC31__INCLUDED_)

