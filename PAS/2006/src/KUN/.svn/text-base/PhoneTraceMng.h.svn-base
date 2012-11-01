/**
@file PhoneTrace.h

@brief phone trace 전화 번호 목록 관리

화일에서 데이타를 로드하고, search 기능을 제공한다.
mutex 시간도 줄이고, 오류 가능을 낮추기 위해 2개의 목록을 관리하면서, 2 개 목록을 번갈아 사용한다.
전화번호 목록을 PhoneTrace 클래스에 저장한다.

@author KTF
@date 2006.09.12
*/
#ifndef PHONETRACEMNG_H
#define PHONETRACEMNG_H

#include "PhoneTrace.h"
#include "MyLog.h"

class PhoneTraceMng:
	public ACE_Task<PAS_SYNCH>
{
public:
	static PhoneTraceMng *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~PhoneTraceMng(void);

	virtual int svc(void);

	void	setDataFile(char *fname);
	
	/// 주어진 전화번호가 trace 대상 번호인지 검사
	bool isTrace(intMDN_t MDN);

	/// 주어진 전화번호를 위한 Log 화일을 받는다.
	MyLog *getTraceLog(char * MDN);
	MyLog *getTraceLog(intMDN_t MDN);
	void stop();
	void    prn(StrStream_t  &out);
		
		
private:
	static PhoneTraceMng *oneInstance;
	PhoneTraceMng(ACE_Thread_Manager* threadManager=0);

	bool runFlag;

	/// 파일 변경 여부 검사
	bool check();

	/// 화일 로드
	int	load();
	// 데이타 로드 후에는 반드시 setRealList() 수행해야 한다.
	void	setRealList();

	int	last_mtime;
	int	last_fsize;
	char	dataFile[256];
	
	PasMutex lock;
	PhoneTrace *phoneList;
	PhoneTrace *newList;
	PhoneTrace dataA, dataB;

	
};
#endif
