#include "KtfInfo.h"
#include "ResponseBuilder.h"
#include "CookieDB.h"
#include "SpeedUpTag.h"

#include "MeHandler.h"

MeHandler::MeHandler(ReactorInfo* rInfo) : ClientHandler(rInfo)
{
}

MeHandler::~MeHandler(void)
{
}

int MeHandler::procACL(Transaction* tr )
{
	HTTP::Request* pRequest = tr->getRequest();
	host_t destProxyHost;
	int destProxyPort;
	AclResult aclResult = applyACL(pRequest, destProxyHost, destProxyPort);

	if(aclResult == AR_CHANGE_PROXY)
	{
		// browser type 이 잘못이면서 멀티 프락시인 경우에는 브라우저 오류로 응답한다.
		/*-- Browser Type Check --*/
		KtfBrowserType	browseType =KtfInfo::checkBrowserType(tr->browser);
		if (browseType == BROWSER_TYPE_KUN)
		{
			PAS_INFO1("Wrong Browser: BROWSER_TYPE_KUN %s", getMyInfo());
			PHTR_INFO1("Wrong Browser: BROWSER_TYPE_KUN %s", getMyInfo());
			ResponseBuilder::InvalidProxy_toKUN( tr->getResponse() );
			tr->setErrorRespCode(RESCODE_WRONG_PROXY); // Over10 로그 출력을 위한 에러 코드
			afterCpTransaction(tr);
			return -1;
		}

		else
		{
			// PROXY 변경 요청
			// CLIENT에게 PROXY 변경을 알리는 응답페이지를 송신
			tr->setErrorRespCode(RESCODE_MULTIPROXY);
			ResponseBuilder::ProxyChange(tr->getResponse(), destProxyHost, destProxyPort);
			afterCpTransaction(tr);
			return -1;
		}
	}
	else if (aclResult == ACL_DNS_APPLIED)
	{
		tr->setCpConnInfo_apply_ACL(destProxyHost.toStr(), destProxyPort);		
	}

	return 0;
}

int MeHandler::browserTypeCheck( Transaction * tr )
{
	KtfBrowserType	browseType = KtfInfo::checkBrowserType(tr->browser);
	if (browseType == BROWSER_TYPE_KUN)
	{
		PAS_INFO1("Wrong Browser: BROWSER_TYPE_KUN %s", getMyInfo());
		PHTR_INFO1("Wrong Browser: BROWSER_TYPE_KUN %s", getMyInfo());
		ResponseBuilder::InvalidProxy_toKUN( tr->getResponse() );
		tr->setErrorRespCode( RESCODE_WRONG_PROXY ); // Over10 로그 출력을 위한 에러 코드
		afterCpTransaction(tr);
		return -1;
	}
	else if (browseType == BROWSER_TYPE_OTHER)
	{
		PAS_INFO1("Wrong Browser: BROWSER_TYPE_OTHER %s", getMyInfo());
		PHTR_INFO1("Wrong Browser: BROWSER_TYPE_OTHER %s", getMyInfo());
		ResponseBuilder::InvalidAgent( tr->getResponse() );
		tr->setErrorRespCode( RESCODE_WRONG_BROWSER ); // Over10 로그 출력을 위한 에러 코드
		afterCpTransaction(tr);
		return -1;
	}

	return 0;
}

void MeHandler::preAfterCpTransaction( Transaction* tr )
{
	readCookie(tr);
	procSpeedupTag(tr);
}

void MeHandler::preRequestToCP( Transaction *tr )
{
	writeCookie(tr);
}

void MeHandler::readCookie( Transaction* tr )
{
	if( !session.needCookieHandler() )
		return;

	const HTTP::Request* req = tr->getRequest();
	const HTTP::Response* res = tr->getResponse();
	const HTTP::RequestHeader* reqHeader = req->getHeader();

	// 헤더에 Cookie 정보가 있으면, 쿠키를 지원하는 단말기로 간주하고
	// 쿠키 핸들링을 별도로 하지 않는다.
	if( !reqHeader->getElement("Cookie").isEmpty() )
		return;

	PAS_INFO("Cookie handling, instead of phone.");

	cookie_t cookie = res->getHeader()->getElement("Set-Cookie");
	if( !cookie.isEmpty() )
	{
		PAS_DEBUG1("Set cookie to CookieDB. Contents is [%s]", cookie.c_str());
		userInfo->setCookie(cookie);
	}
}

void MeHandler::procSpeedupTag( Transaction* tr )
{
	ACE_Time_Value startTime(ACE_OS::gettimeofday());

	// Insert speed up tag
	if( tr->isNeedInsertSpeedupTag() )
	{
		PAS_DEBUG("Insert speed up tag.");

		const ACE_Message_Block* pRawBody = tr->getResponse()->getRawBody();
		ACE_ASSERT(pRawBody != NULL);

		char* buf = new char[pRawBody->length()*2];
		memcpy(buf, pRawBody->rd_ptr(), pRawBody->length());
		buf[pRawBody->length()] = '\0';

		SpeedUpTag::insert(buf, pRawBody->length());

		tr->getResponse()->setBody(buf, strlen(buf));
	}

	// Remove speed up tag
	else if( tr->isNeedRemoveSpeedupTag() )
	{
		PAS_DEBUG("Remove speed up tag.");

		const ACE_Message_Block* pRawBody = tr->getResponse()->getRawBody();
		ACE_ASSERT(pRawBody != NULL);

		char* buf = new char[pRawBody->length()*2];
		memcpy(buf, pRawBody->rd_ptr(), pRawBody->length());
		buf[pRawBody->length()] = '\0';

		SpeedUpTag::remove(buf, pRawBody->length());

		tr->getResponse()->setBody(buf, strlen(buf));
	}

	ACE_Time_Value endTime(ACE_OS::gettimeofday());

	writeSpeedupTagLog(tr, startTime, endTime);
}

void MeHandler::writeCookie( Transaction * tr )
{
	if( !session.needCookieHandler() )
		return;

	HTTP::Request* req = tr->getRequest();
	HTTP::RequestHeader* reqHeader = req->getHeader();

	// 헤더에 Cookie 정보가 있으면, 쿠키를 지원하는 단말기로 간주하고
	// 쿠키 핸들링을 별도로 하지 않는다.
	if( !reqHeader->getElement("Cookie").isEmpty() )
		return;

	PAS_INFO("Cookie handling, instead of phone.");

	// 기록된 쿠키가 있다면, 요청헤더에 쿠키를 셋팅한다.
	cookie_t cookie = userInfo->getCookie(reqHeader->getHost(), reqHeader->getPath());
	if(!cookie.isEmpty())
	{
		PAS_DEBUG1("Set cookie to header. Contents is [%s]", cookie.c_str());
		reqHeader->addElement("Cookie", cookie);
		req->setHeader(reqHeader->build());
	}
}

void MeHandler::writeSpeedupTagLog( Transaction *tr, const ACE_Time_Value& startTime, const ACE_Time_Value& endTime )
{
	SpeedUpTagStatus status = tr->getSpeedupStatus();

	ACE_Time_Value duration = endTime - startTime;

	MyLog log;
	filename_t filename = Config::instance()->getLogPrefix();
	filename += "speedup";
	log.openWithYear("./", filename);

	// MDN Duration GCode MSModel Browser URL
	log.logprint(LVL_INFO, "%-11s %2d %10s %d.%06d %s\n", 
		session.phoneNumber, 
		static_cast<int>(status), 
		session.msModel, 
		duration.sec(), duration.usec(), 
		session.getIpAddr());

	log.close();
}