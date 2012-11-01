//////////////////////////////////////////////////////////////////////
//
// GWInfo.h: interface for the CGWInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(GWINFO_H__7814D245_5214_4194_B155_56D55FED3282__INCLUDED_)
#define GWINFO_H__7814D245_5214_4194_B155_56D55FED3282__INCLUDED_

class CGWInfo  
{
private:
	
public:
	int GetGWInfo(const char* szProc, int &icpu, int &imemory);
	CGWInfo();
	virtual ~CGWInfo();
};

#endif // !defined(GWINFO_H__7814D245_5214_4194_B155_56D55FED3282__INCLUDED_)

