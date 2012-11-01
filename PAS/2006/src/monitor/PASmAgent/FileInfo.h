//////////////////////////////////////////////////////////////////////
//
// FileInfo.h: interface for the CFileInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FILEINFO_H__3CDD5604_2301_412E_A291_6A1B2838358E__INCLUDED_)
#define FILEINFO_H__3CDD5604_2301_412E_A291_6A1B2838358E__INCLUDED_

class CFileInfo  
{
private:

	char m_szGetLastError[2048];
public:
	char* GetLastError();	
	int GetFileInfo(const char* szFileName, int iType, char* szCreateDate, int &iFileSize, int &iStatus);
	CFileInfo();
	virtual ~CFileInfo();

};

#endif // !defined(FILEINFO_H__3CDD5604_2301_412E_A291_6A1B2838358E__INCLUDED_)

