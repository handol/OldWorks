#ifndef __CP_LIST_H__
#define __CP_LIST_H__

/**
@brief CP 목록 관리

이 클래스를 만들게된 취지는
빈번하게 CP 에 접속하게 됨으로써 접속/종료 에 대한 오버헤드가 발생하게 되어
CP 와의 커넥션을 유지하여 오버헤드를 줄이고 반응속도를 높이기 위해서 만들었다.

하지만, CP는 WEB 서버이고 타임아웃이 존재하므로, 예상만큼 오버헤드의 차가 없다.
현재(2006.11.15)는 config에서 ReuseCpConnection 설정이 on 일 때만 사용한다. 
*/

#include "Common.h"
#include "CpEventHandler.h"
#include <vector>
#include "HttpTypes.h"
#include <ace/Thread_Mutex.h>

typedef std::vector<CpHandler*> CpPtrs;

class CpList
{
// 멤버 함수
public:
	int add(CpHandler* pCP);
	int addIfNot(CpHandler* pCP);
	CpHandler* get(const host_t host, const int port);
	int del(CpHandler* pCP);
	int deleteAll();
	int size();

	bool isExist(const host_t host, const int port);
	bool isExist(const CpHandler* pCP);

	/// cpList 에 등록된 모든 CP에게 접속 종료 요청을 한다.
	void requestCloseToAll();
	
private:

// 멤버 변수
private:
	CpPtrs cpList;
};

#endif
