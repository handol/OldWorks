#include "AuthInfo.h"

AuthInfoMessageBlock::AuthInfoMessageBlock(AUTH::RequestBody *body, UserInfo *userinfo)
{
	memcpy( &reqBody, body, sizeof(reqBody) );
	memcpy( &userInfo, userinfo, sizeof(userInfo) );

	int total = sizeof(reqBody) + sizeof(userInfo);
	size( total );

	ACE_ASSERT(static_cast<int>(space()) >= total);

	// 원래의 ACE_Message_Block 의 용도에 의하면,
	// 메시지 블럭에 내용을 카피하여 전달하여야 하지만
	// 굳이 몇번의 메모리 복사를 감수할 필요가 없다.
	// 그러므로 이 기능은 쓰지 않지만 if(mesg->length()==0) 을 피하기 위하여
	// Write Pointer 만 증가시킨다.
/*
	memcpy((void*)wr_ptr(), (const void*)&reqBody, sizeof(reqBody));
	wr_ptr( sizeof(reqBody) );

	memcpy((void*)wr_ptr(), (const void*)&userInfo, sizeof(userInfo));
	wr_ptr( sizeof(userInfo) );
*/

	wr_ptr( total );
}

AuthInfoMessageBlock::~AuthInfoMessageBlock()
{
}

