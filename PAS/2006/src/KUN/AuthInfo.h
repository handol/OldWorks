#ifndef __AUTH_INFO_H__
#define __AUTH_INFO_H__

/**
@brief AuthAgent에 새로운 Job을 추가할 때 사용

userInfo = UserInfo 정보
reqBody  = Auth 서버에 인증 정보를 보낼 데이터
respBody = Auth 서버에서 인증 받은 정보가 저장된다.(AuthAgent 가 사용한다.)
*/

#include <ace/Message_Block.h>
#include "AuthTypes.h"
#include "UserInfo.h"

class AuthInfo
{
public:
	AuthInfo() {}
	virtual ~AuthInfo() {}

	AUTH::ResponseBody respBody;
	AUTH::RequestBody reqBody;
	UserInfo userInfo;
};

class AuthInfoMessageBlock : public ACE_Message_Block
{
	// Member Functions
public:
	AuthInfoMessageBlock(AUTH::RequestBody *body, UserInfo *userinfo);
	virtual ~AuthInfoMessageBlock();

private:
protected:

	// Member Variables
public:
	AUTH::ResponseBody respBody;
	AUTH::RequestBody reqBody;
	UserInfo userInfo;

private:
protected:

};

#endif
