//////////////////////////////////////////////////////////////////////
//
// SYSCPUInfo.h: interface for the CSYSCPUInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SYSCPUINFO_H__648F1D3A_01A2_401C_9BD1_AEF7AB7D6D3E__INCLUDED_)
#define SYSCPUINFO_H__648F1D3A_01A2_401C_9BD1_AEF7AB7D6D3E__INCLUDED_

#include <string.h>

struct cpu_info_name{
	char name[128];
public:
	void init() { memset((char*)this, 0x00, sizeof(cpu_info_name)); }
};

class CSYSCPUInfo  
{
private:
	int m_cpu_count;
	cpu_info_name *stcpu;		
	int GetCPUCount();
public:
	int GetCPUInfo(int &iidle, int &iuser, int &ikernel, int & iwait);
	CSYSCPUInfo();
	virtual ~CSYSCPUInfo();

};

#endif // !defined(SYSCPUINFO_H__648F1D3A_01A2_401C_9BD1_AEF7AB7D6D3E__INCLUDED_)
