//////////////////////////////////////////////////////////////////////
//
// UDRChecker.h: interface for the CUDRChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(UDRCHECKER_H__42F5515D_4AE2_4934_B5FA_BC38F15CC5EF__INCLUDED_)
#define UDRCHECKER_H__42F5515D_4AE2_4934_B5FA_BC38F15CC5EF__INCLUDED_

class CUDRChecker  
{
private:
	int FieldCheck(char *szLine, int sPos, int Size);
	int FileSize();
	char m_szFileName[256];
	char m_szGetLastError[256];

	int  m_TotalCount;
	int  m_SuccessCount;
	int  m_ErrorCount;
public:
	int GetUDRFileList(const char* szPath, const char* szGWInfo, char Type, const char* szCurrentDate, char* szFileName, int &filesize, char* filecreatedate);
	char* GetLastError();
	int GetValidate(int &iTot, int &iSuccess, int &iError);
	int SetFileName(const char* szFileName);
	CUDRChecker();
	virtual ~CUDRChecker();

};

#endif // !defined(UDRCHECKER_H__42F5515D_4AE2_4934_B5FA_BC38F15CC5EF__INCLUDED_)

