//////////////////////////////////////////////////////////////////////
//
// SYSCPUInfo.cpp: implementation of the CSYSCPUInfo class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <kstat.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "SYSCPUInfo.h"

#define BYTES_PER_PAGE 8192
#define KBYTE          1024
#define MUL            (BYTES_PER_PAGE/KBYTE)
#define LOOP_COUNT	3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSYSCPUInfo::CSYSCPUInfo()
{
	m_cpu_count = 0;                
        stcpu = new cpu_info_name[128];           
}

CSYSCPUInfo::~CSYSCPUInfo()
{
	if ( stcpu )
	{
		delete[] stcpu;
		stcpu = NULL;
	}
}


int CSYSCPUInfo::GetCPUCount()
{
	kstat_ctl_t    *kc;
	kstat_t        *ksp;
	
	m_cpu_count = 0;

	kc  = kstat_open();
	if ( kc == NULL ) return 0;

	for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) 
	{
		if ( !strcmp(ksp->ks_module, "cpu_stat") && !strncmp(ksp->ks_name, "cpu_stat", 7) )
        	{
#ifdef DEBUG
                	printf("Module: %s Class: %s Name: %s\n", ksp->ks_module, ksp->ks_class, ksp->ks_name);
                	printf("ks_name:%s\n", ksp->ks_name);
#endif
                	strcpy(stcpu[m_cpu_count].name, ksp->ks_name);
                	m_cpu_count++;
        	}
  	}

	kstat_close(kc);
	
	return m_cpu_count;
}

int CSYSCPUInfo::GetCPUInfo(int &iidle, int &iuser, int &ikernel, int & iwait)
{
        kstat_ctl_t *kc;
        kstat_t *ksp;
        kstat_io_t      kio;
        kstat_t *kp = NULL;
	kstat_ctl_t *s;

        cpu_stat_t lastcpuinfo[128];
        cpu_stat_t cpuinfo;

	int bValid = 0;

	GetCPUCount();

        kc = kstat_open();
	s = kc;

	int t_idle = 0;
	int t_user = 0;
	int t_kernel = 0;
	int t_wait = 0;

	for (int iCount = 0; iCount < LOOP_COUNT; iCount++)	
	{
		t_idle = 0;        
        	t_user = 0;        
        	t_kernel = 0;        
        	t_wait = 0;
		bValid = 0;

		char buf[1024] = {0};
		for ( int icpuCount = 0; icpuCount < m_cpu_count; icpuCount++ ) 
		{
			kc = s;
        		for ( ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next)
        		{
                		if ( ksp->ks_type == KSTAT_TYPE_RAW ) 
                		{
                        		if ( !strcmp(ksp->ks_name, stcpu[icpuCount].name) )
                        		{
                                		kstat_read(kc, ksp, &cpuinfo);

                				if ( ksp == NULL )
                				{
                        				kstat_close(kc);
                        				return 0;
                				}
						else
						{
							t_idle += (cpuinfo.cpu_sysinfo.cpu[CPU_IDLE] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_IDLE]);
							t_user += (cpuinfo.cpu_sysinfo.cpu[CPU_USER] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_USER]);
							t_kernel += (cpuinfo.cpu_sysinfo.cpu[CPU_KERNEL] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_KERNEL]);
							t_wait += (cpuinfo.cpu_sysinfo.cpu[CPU_WAIT] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_WAIT]); 
							bValid++;

#ifdef DEBUG
	printf("cup idle:\t%d\n",  (cpuinfo.cpu_sysinfo.cpu[CPU_IDLE] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_IDLE]));
        printf("cup user:\t%d\n",  (cpuinfo.cpu_sysinfo.cpu[CPU_USER] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_USER]));
        printf("cup kernel:\t%d\n",  (cpuinfo.cpu_sysinfo.cpu[CPU_KERNEL] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_KERNEL]));
        printf("cup wait:\t%d\n",  (cpuinfo.cpu_sysinfo.cpu[CPU_WAIT] - lastcpuinfo[icpuCount].cpu_sysinfo.cpu[CPU_WAIT]));
        printf("===============================================================\n");
#endif
            	    					lastcpuinfo[icpuCount] = cpuinfo;
						}
                        		}
				}
                	}
        	}
#ifdef DEBUG
		printf("count:%d\n", bValid);
		printf("idle:%d\n", t_idle); 
		printf("user:%d\n", t_user); 
		printf("kernel:%d\n", t_kernel); 
		printf("wait:%d\n", t_wait); 
#endif
		iidle = 0;
		iuser = 0;
		ikernel = 0;
		iwait = 0;

		if ( t_idle != 0 )
			iidle = t_idle / bValid; 

		if ( t_user != 0 )
			iuser = t_user / bValid;

		if ( t_kernel != 0 )
			ikernel = t_kernel / bValid; 

		if ( t_wait != 0 )
			iwait = t_wait / bValid;

                sleep(1);

                kstat_chain_update(kc);
	}

	kstat_close(kc);

	return 1;
}
