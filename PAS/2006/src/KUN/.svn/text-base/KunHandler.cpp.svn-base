#include "KtfInfo.h"
#include "ResponseBuilder.h"

#include "KunHandler.h"

KunHandler::KunHandler(ReactorInfo* rInfo) : ClientHandler(rInfo)
{

}

KunHandler::~KunHandler(void)
{
}

int KunHandler::procACL( Transaction* tr )
{
	HTTP::Request* pRequest = tr->getRequest();
	host_t destProxyHost;
	int destProxyPort;
	AclResult aclResult = applyACL(pRequest, destProxyHost, destProxyPort);

	if(aclResult == AR_CHANGE_PROXY)
	{
		// browser type 이 잘못이면서 멀티 프락시인 경우에는 브라우저 오류로 응답한다.
		/*-- Browser Type Check --*/
		KtfBrowserType browseType =KtfInfo::checkBrowserType(tr->browser);
		if (browseType == BROWSER_TYPE_ME)
		{
			PAS_INFO1("Wrong Browser: BROWSER_TYPE_ME %s", getMyInfo());
			PHTR_INFO1("Wrong Browser: BROWSER_TYPE_ME %s", getMyInfo());
			ResponseBuilder::InvalidProxy_toME( tr->getResponse() );
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

int KunHandler::browserTypeCheck( Transaction* tr )
{
	KtfBrowserType	browseType = KtfInfo::checkBrowserType(tr->browser);
	if (browseType == BROWSER_TYPE_ME)
	{
		PAS_INFO1("Wrong Browser: BROWSER_TYPE_ME %s", getMyInfo());
		PHTR_INFO1("Wrong Browser: BROWSER_TYPE_ME %s", getMyInfo());
		ResponseBuilder::InvalidProxy_toME( tr->getResponse() );
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
