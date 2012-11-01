//////////////////////////////////////////////////////////////////////
//
// SYSMemInfo.cpp: implementation of the CSYSMemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SYSMemInfo.h"

#include <unistd.h>
#include <stdio.h>
#include <kstat.h>
#include <string.h>
#include <sys/sysinfo.h>

#define BYTES_PER_PAGE 8192
#define KBYTE          1024
#define MUL            (BYTES_PER_PAGE/KBYTE) / 1024

#define LOOP_COUNT	3


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSYSMemInfo::CSYSMemInfo()
{

}

CSYSMemInfo::~CSYSMemInfo()
{


}

int CSYSMemInfo::GetSysMemoryInfo(int &iMemSize, int &iFreeMem, int &iReservSwap, int &iAllocSwap, int &iAvailSwap, int &iFreeSwap)
{
	kstat_ctl_t    *kc;
  	kstat_t        *ksp;
  	int            first=1;
  	vminfo_t       vmlast,vm;
  	cpu_stat_t     statlast, cpu_stat;

	memset((char*)&vmlast, 0x00, sizeof(vminfo_t));

  	kc  = kstat_open();
	if ( kc == NULL ) return 0; 

  	for ( int i = 0; i < LOOP_COUNT; i++ )
	{
    		ksp = kstat_lookup(kc, (char*)"unix", 0, (char*)"vminfo");

    		if (ksp) 
		{
      			kstat_read(kc, ksp, &vm);
#ifdef DEBUG
        printf("---------------------------------\n");
        printf("Phycal Mem     =%8llu KByte\n", GetPhyMemory());
        printf("freemem       =%8llu KByte\n",(vm.freemem-vmlast.freemem)*MUL);
        printf("reserved swap =%8llu KByte\n",(vm.swap_resv-vmlast.swap_resv)*MUL);
        printf("allocated swap=%8llu KByte\n",(vm.swap_alloc-vmlast.swap_alloc)*MUL);
        printf("avail swap    =%8llu KByte\n",(vm.swap_avail-vmlast.swap_avail)*MUL);
        printf("free swap     =%8llu KByte\n",(vm.swap_free-vmlast.swap_free)*MUL);
#endif
			iMemSize = GetPhyMemory();
			iFreeMem = (vm.freemem-vmlast.freemem)*MUL;
			iReservSwap = (vm.swap_resv-vmlast.swap_resv)*MUL;
			iAllocSwap = (vm.swap_alloc-vmlast.swap_alloc)*MUL;
			iAvailSwap = (vm.swap_avail-vmlast.swap_avail)*MUL;
			iFreeSwap = (vm.swap_free-vmlast.swap_free)*MUL;

			vmlast=vm;
    		}
		else
		{
  			kstat_close(kc);	
			return 0;
		}

	    	sleep(1);
    		kstat_chain_update(kc);
	}

  	kstat_close(kc);	

	return 1;
}

int CSYSMemInfo::GetPhyMemory()
{
        long TotPage =  sysconf(_SC_PHYS_PAGES);
        long AllocPage = sysconf(_SC_AVPHYS_PAGES);
        long PageSize = sysconf(_SC_PAGESIZE);

	return TotPage*(PageSize/1024)/1024;
}
