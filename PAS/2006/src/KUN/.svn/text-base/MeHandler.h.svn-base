#ifndef __MEHANDLER_H__
#define __MEHANDLER_H__

#include "ClientHandler.h"

class MeHandler :
	public ClientHandler
{
public:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------
	MeHandler(ReactorInfo* rInfo);
	virtual ~MeHandler(void);

private:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------

	void procSpeedupTag( Transaction* tr );
	void readCookie( Transaction* tr );
	void writeCookie( Transaction * tr );

	/// CP로 부터 응답이 온 후 발생하는 이벤트
	/**
	* 쿠키 핸들링이 별도로 필요한 단말일 경우, 쿠키를 PAS에서 보관한 후, 다음 request 할 때
	* 헤더에 첨부해서 CP로 보낸다.
	*
	* @date 2007/05/29 
	* @author SeHoon Yang
	**/
	virtual void preAfterCpTransaction(Transaction* tr);

	virtual void preStartTransaction(Transaction* tr)
	{
		
	}

	virtual int browserTypeCheck( Transaction * tr );
	virtual int procACL(Transaction* tr);

	/// CP에게 웹페이지를 요청하기 직전의 이벤트
	/**
	* 쿠키 핸들링을 별도로 해줘야하는 단말일 경우, 기록된 쿠키를 헤더에 삽입한다.
	*
	* @date 2007/05/29 
	* @author SeHoon Yang
	**/
	virtual void preRequestToCP(Transaction *tr);

	void writeSpeedupTagLog(Transaction *tr, const ACE_Time_Value& startTime, const ACE_Time_Value& endTime);
};

#endif // __MEHANDLER_H__
