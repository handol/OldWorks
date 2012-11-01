//////////////////////////////////////////////////////////////////////
//
// LogSearcher.h: interface for the CLogSearcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LOGSEARCHER_H__45C19EA0_FC55_465F_9F26_518736D9DF3F__INCLUDED_)
#define LOGSEARCHER_H__45C19EA0_FC55_465F_9F26_518736D9DF3F__INCLUDED_

#define MAX_LOG_BUFFUER_SIZE 512

struct STLogData{
	char path[MAX_LOG_BUFFUER_SIZE];
	char data[MAX_LOG_BUFFUER_SIZE];
public:
	void init() { memset((char*)this, 0x00, sizeof(STLogData)); }
};

class CLogSearcher  
{
private:
	int searchdir(const char *szPath, char fType);
	int m_DataCount;
	STLogData *m_stLogData;
	int m_LogLine;
	char* m_Data;
	
public:
	int GetFileContentQuery(int iType, char fType, const char* szfilelist, const char* szSearchString);
	int GetFileListQuery2(int iType, char fType);
	int GetFileListQuery(int iType, char fType);
	int GetList(STLogData* &pLogData, int &ListCount);
	char* GetList();
	int init();
	int GetPOpen(const char* szCommand);		
	CLogSearcher();
	virtual ~CLogSearcher();

};

#endif // !defined(LOGSEARCHER_H__45C19EA0_FC55_465F_9F26_518736D9DF3F__INCLUDED_)
