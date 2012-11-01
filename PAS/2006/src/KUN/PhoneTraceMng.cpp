
#include "Common.h"
#include "Mutex.h"
#include "Util.h"
#include "Util2.h"
#include "PhoneTraceMng.h"
#include "Config.h"


#include <iostream>
#include <fstream>

PhoneTraceMng *PhoneTraceMng::oneInstance = NULL;

PhoneTraceMng *PhoneTraceMng::instance(ACE_Thread_Manager* threadManager)
{
	if (oneInstance == NULL) 
		oneInstance = new PhoneTraceMng(threadManager);
		
	return oneInstance;

}


PhoneTraceMng::PhoneTraceMng(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	last_mtime = 0;
	last_fsize = 0;
	dataFile[0] = NULLCHAR;
	runFlag =  true;
	phoneList = &dataA;
	newList = &dataB;
}

PhoneTraceMng::~PhoneTraceMng(void)
{
	if (phoneList)
	{
		phoneList->clear();
	}
	if (newList)
	{
		newList->clear();
	}
	
}

void PhoneTraceMng::setDataFile(char *fname)
{
	STRNCPY(dataFile, fname, sizeof(dataFile)-1 )
}

void PhoneTraceMng::setRealList(void)
{
	WriteMutex<PasMutex> writeMutex(lock);
	PhoneTrace *tmp;

	// list 를 swap 한다.	
	tmp = phoneList;
	phoneList = newList;
	newList = tmp;

	// swap 하였으므로 newList 에는 old data 가 있다.
	if (newList != NULL)
		newList->clear();
}

/**
thread 의 main 함수이다.
현재 시간의 시분초 값 중에 분값을 확인하여 3분, 8분인 경우에 stat.cfg 화일을 다시 로드한다.
*/


int	PhoneTraceMng::svc(void)
{
	int period_sec = Config::instance()->process.phonetraceinterval;
	ACE_Message_Block *mesg = 0;
	int lastsec = 0;
	
	PAS_NOTICE("PhoneTraceMng::svc start");
	
	// 주기적으로 화일 변경을 검사하여 데이타 리로딩.
	while(runFlag) {
		int	now = time(NULL);
		if  (now - lastsec >= period_sec)
		{
			lastsec = now;
			if (check()) 
			{
				StrStream_t	out;
				load();
				setRealList();
				prn(out);
				PAS_INFO1("PhoneTraceMng:: MDN List \n%s\n", out.toStr());
			}
		}

		// wait for 1 seconds
		ACE_Time_Value waitsec(ACE_OS::gettimeofday());
		waitsec += ACE_Time_Value(1, 0);
		if(getq(mesg, &waitsec) >= 0)
		{
			if (mesg->length()==0) // 빈 메시지 수신 --> 종료를 의미.
			{
				break;
			}
		}
	}

	PAS_NOTICE("PhoneTraceMng::svc stop");
	return 0;
}

void PhoneTraceMng::stop()
{
	runFlag = false;
}

bool PhoneTraceMng::isTrace(intMDN_t MDN)
{
	if (phoneList==NULL)
		return false;
	
	ReadMutex<PasMutex> readMutex(lock);
	
	return phoneList->isTrace(MDN);
}

MyLog *PhoneTraceMng::getTraceLog(char* MDN)
{
	intMDN_t mdn = Util::phonenum2int(MDN);
	return getTraceLog(mdn);
}


MyLog *PhoneTraceMng::getTraceLog(intMDN_t MDN)
{
	
	PhoneTraceInfo *info=NULL;

	info = phoneList->search(MDN);
	if (info==NULL)
		return NULL;

	///printf("%d %d %d\n", info->mdn, info->starttime, info->endtime);
	time_t now = time(NULL);

	if (info->starttime != 0 && (now < info->starttime || now > info->endtime))
		return NULL;
		
	char fname[256];
	char yymmdd[32];
	Util2::get_yymmdd(yymmdd);


	snprintf(fname, sizeof(fname)-1, "0%d", MDN);
	MyLog *mylog = new MyLog();
	mylog->open((char*)"./", fname);
	mylog->set_priority(info->loglevel);
	return mylog;
}


/**
@return trace.acl 화일이 변경된 경우 return 1.   화일 변경 시각, 화일 크기를 비교.
*/
bool PhoneTraceMng::check()
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
int PhoneTraceMng::load()
{
	char buf[1024];
	ifstream    in;
	int lineno = 0;

	in.open(dataFile, ios::in);
	
	if (in.rdbuf()->is_open() == 0) {
		PAS_INFO1("Cannot read PhoneTrace file : %s", dataFile);
		return (-1);
	}

	PAS_INFO1("reading Phone Trace file : %s", dataFile);

	while (in.getline(buf, sizeof(buf)-1))
	{
		FastString<32> mdn(buf);
		PhoneTraceInfo info;
		char ch;
		
		lineno++;		
		mdn.trim();
		ch = mdn.firstCh();		
		if (ch == '#' || ch == 0) continue;

		info.mdn = Util::phonenum2int((char*)mdn.toStr());
		info.starttime = 0;
		info.endtime = 0;
		info.loglevel = 0;
		newList->add(&info );
	}  /* while : file read */

	
	PAS_NOTICE1("PhoneTraceMng:: load %d  phones", newList->size());
	
	return newList->size();
}


void PhoneTraceMng::prn(StrStream_t  &out)
{
	phoneList->prn(out);
	PAS_INFO1("%s", out.toStr());
}


#ifdef TEST_MAIN

int main()
{
	PhoneTraceMng traceMng;

	traceMng.activate();
	
}
#endif
