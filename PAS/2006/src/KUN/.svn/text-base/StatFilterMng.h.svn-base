/**
@file StatFilter.h

@brief Stat 공지 처리 파일 관리

@author DMS
@date 2006.09.12
*/
#ifndef STATFILTERMNG_H
#define STATFILTERMNG_H

#include "StatFilter.h"
#include "MyLog.h"


class StatFilterMng:
	public ACE_Task<PAS_SYNCH>
{
public:
	static StatFilterMng *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~StatFilterMng(void);

	virtual int svc(void);

	void	setDataFile(char *fname);
	
	bool isBlocked(int iType , char *keystr, int iport,  char* notimesg);
	
	void stop();
	void    prn();
		
		
private:
	static StatFilterMng *oneInstance;
	StatFilterMng(ACE_Thread_Manager* threadManager=0);

	bool runFlag;

	/// 파일 변경 여부 검사
	bool check();

	/// 화일 로드
	int	load();
	// 데이타 로드 후에는 반드시 setRealList() 수행해야 한다.
	void	setRealList();

	int	last_mtime; // 파일 변경 시간
	int	last_fsize; // 파일 크기
	char	dataFile[256];
	
	PasMutex lock;
	StatFilter *filterList;
	StatFilter *newList;
	StatFilter dataA, dataB;
	
};
#endif
