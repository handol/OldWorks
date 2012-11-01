
#include "Common.h"
#include "Mutex.h"
#include "Util.h"
#include "Util2.h"
#include "StatFilterMng.h"
#include "Mutex.h"
#include "Config.h"

#include <iostream>
#include <fstream>

/**
@brief 공지 화일을 reload 하는 역할.

1. 화일 변경 체크를  "화일 크기 변경 OR 화일 시간 변경" 체크로 개선함.
2. 화일 변경임을 판단하고, reload를 했는데, 읽은 데이타 수가 0개이면 1초 후에 retry.
3. retry 후에도 읽은 데이타 수가 0개이면 기존 데이타를 유지한다.
*/

StatFilterMng *StatFilterMng::oneInstance = NULL;

StatFilterMng *StatFilterMng::instance(ACE_Thread_Manager* threadManager)
{
	if (oneInstance == NULL) 
		oneInstance = new StatFilterMng(threadManager);
		
	return oneInstance;

}


StatFilterMng::StatFilterMng(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	last_mtime = 0;
	last_fsize = 0;
	dataFile[0] = NULLCHAR;
	runFlag =  true;
	filterList = &dataA;
	newList = &dataB;
}

StatFilterMng::~StatFilterMng(void)
{
	if (filterList)
		filterList->clear();
}

void StatFilterMng::setDataFile(char *fname)
{
	STRNCPY(dataFile, fname, sizeof(dataFile)-1 )
}

void StatFilterMng::setRealList(void)
{
	WriteMutex<PasMutex> writeMutex(lock);
	StatFilter *tmp;

	// list 를 swap 한다.	
	tmp = filterList;
	filterList = newList;
	newList = tmp;

	// swap 하였으므로 newList 에는 old data 가 있다.
	if (newList != NULL)
		newList->clear();
}


/**
@breif thread 의 main 함수이다. [k_]stat.cfg 화일이 변경된 경우  다시 로드한다. 변경 확인은 5초마다 한다.
@return Never return. 무한 루프.

1. 화일 변경 체크를  "화일 크기 변경 OR 화일 시간 변경" 체크로 개선함.

2. 화일 변경임을 판단하고, reload를 했는데, 읽은 데이타 수가 0개이면 1초 후에 retry.

3. retry 후에도 읽은 데이타 수가 0개이면 기존 데이타를 유지한다.

4. 화일 변경임을 판단하고, reload하기 전에 0.1초 usleep() 후에 load하는 것으로 수정.

*/
int	StatFilterMng::svc(void)
{
	int period_sec = Config::instance()->process.statfilterinterval;

	ACE_Message_Block *mesg = 0;
	int lastsec = 0;
	
	PAS_NOTICE("StatFilterMng::svc start");
	
	// 주기적으로 화일 변경을 검사하여 데이타 리로딩.
	while(runFlag) {
		int	now = time(NULL);
		if  (now - lastsec >= period_sec)
		{
			now = lastsec;
			if (check()) // 화일이 변경된 경우.
			{
				// copy와 reload 가  거의 동시에 발생할때, 화일에 내용이 빈것 처럼 되는 것으로 추측된다.
				// 그래서 잠깐 기다렸다가 load 하자. -- 2006.12.01
				usleep(100000); 
				int loaded = 0;
				for (int trycount=0; trycount < 10;  trycount++)
				{
					PAS_NOTICE1("Stat File: try to load [%d times]", trycount+1);
					
					load();
					if (newList->count() == 0) // 로딩 실패인 경우 한 번 더 시도.
					{						
						sleep(1);
					}

					else if (newList->count() > 0)
					{
						setRealList();
						prn();
						loaded = 1;
						break;
					}					
				}

				if (! loaded) // 로딩 실패인 경우
				{
					// 신규 로딩에 아무 데이타가 없는 경우, 이전 데이타 유지
					PAS_NOTICE("Stat File: NO items. So, no update. Keeping the old stat filter data.");
				}
				
			}
		}

		// wait for 1 second
		ACE_Time_Value waitsec(ACE_OS::gettimeofday());
		waitsec += ACE_Time_Value(1,0);
		if (getq(mesg, &waitsec) >= 0)
		{
			if (mesg->length()==0) // 빈 메시지 수신 --> 종료를 의미.
			{
				break;
			}
		}
	}

	PAS_NOTICE("StatFilterMng::svc stop");
	return 0;
}

void StatFilterMng::stop()
{
	runFlag = false;
}


bool StatFilterMng::isBlocked(int iType , char *keystr, int iport,  char* notimesg)
{
	ReadMutex<PasMutex> readMutex(lock);
	return filterList->isBlocked(iType, keystr, iport, notimesg);
}

/**
@return k_stat.cfg 화일이 변경된 경우 return 1.   화일 변경 시각, 화일 크기를 비교.
*/
bool StatFilterMng::check()
{
	int mtime = Util2::get_file_mtime(dataFile);
	int fsize = Util2::get_file_size(dataFile);
	
	
	if (last_mtime == mtime && last_fsize == fsize) return false;

	last_mtime = mtime;
	last_fsize = fsize;
	return true;
}



/**
Stat.cfg 화일을 읽고 파싱하여 DOMAN, URL, MDN 목록에 추가한다.
*/
int StatFilterMng::load()
{
	return newList->load(dataFile);
}


void StatFilterMng::prn()
{
	filterList->prn();
}


#ifdef TEST_MAIN

int main()
{
	StatFilterMng traceMng;

	traceMng.activate();
	
}
#endif
