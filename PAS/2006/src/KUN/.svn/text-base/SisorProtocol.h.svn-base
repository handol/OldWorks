#ifndef __SISOR_PROTOCOL_H__
#define __SISOR_PROTOCOL_H__

#include <ace/Message_Block.h>

// Sisor Protocol
/*

+------------+------------+----------------+--------+
|         HEADER          |      BODY      | FOOTER |
+------------+------------+----------------+--------+
| "SOX " (4) | length (4) | body (dynamic) | " EOX" |
+------------+------------+----------------+--------+

Sisor Protocol 은 4Bytes 문자열 "SOX "로 시작하며, " EOX"로 끝난다. (대문자)

[length]
body 의 길이를 의미한다.
ASCII 4 bytes 로 구성되어 있어서 0 ~ 9999 까지 표시한다.
1000 보다 작은 수는 space(' ')로 앞을 채운다. 
ex) 3482 => "3482", 238 => " 239", 7 => "   7"

[body]
실제 통기하기 위한 데이터로써 세부사항은 하부 프로토콜에서 별도로 정의한다.

*/

// Sisor Protocol Handler
class SIP
{
public:
	enum CHK_RESULT
	{
		CR_OK,
		CR_BAD,
		CR_NOT_ENOUGH
	};

	/// 시작 토큰 체크
	/**
	 * @return "SOX "(4bytes)로 시작하면 CR_OK
	 * @return 4bytes 보다 버퍼가 작으면 CR_NOT_ENOUGH
	 * @return "SOX "로 시작하지 않으면 CR_BAD 를 리턴한다.
	 **/
	static CHK_RESULT checkStartToken(const ACE_Message_Block& src);

	/// length 필드 체크
	/**
	 * @return 정상적인 length 필드일 경우 CR_OK
	 * @return 헤더 보다 버퍼가 작으면 CR_NOT_ENOUGH
	 * @return 헤더가 잘못 되어 있을 경우 CR_BAD 를 리턴한다.
	 **/
	static CHK_RESULT checkLength(const ACE_Message_Block& src);

	/// body 필드 체크
	/**
	 * @return 헤더와 body까지 정상적일 경우 CR_OK
	 * @return 헤더와 body 데이터 보다 src가 작을 경우 CR_NOT_ENOUGH
	 * @return 헤더가 잘 못 되어 있을 경우 CR_BAD 를 리턴한다.
	 **/
	static CHK_RESULT checkBody(const ACE_Message_Block& src);

	/// 종료 토큰 체크
	/**
	 * @return " EOX"(4bytes)로 끝이 나면 CR_OK
	 * @return 헤더 + 바디 + 풋터 보다 버퍼가 작으면 CR_NOT_ENOUGH
	 * @return checkStartToken() 과 checkLength() 를 정상 통과하지 못하거나 " EOX"로 끝나지 않을 경우 CR_BAD 를 리턴한다.
	 **/
	static CHK_RESULT checkEndToken(const ACE_Message_Block& src);

	/// length 필드 읽기
	/**
	 * @return 정상일 경우 length 필드 값, 에러일 경우 -1
	 **/
	static int getBodyLength(const ACE_Message_Block& src);

	/// body 필드 읽기
	/**
	 * @return 정상적일 경우 0, 에러일 경우 -1
	 **/
	static int getBody(ACE_Message_Block& dest, const ACE_Message_Block& src);

	/// 패킷의 전체 길이를 계산한다. (헤더, 바디, 풋터)
	/**
	 * "SOX "(4) + [length](4) + [body length] + " EOX"(4) 의 전체 길이를 반환
	 * 
	 * @return 전체 길이, 에러일 경우 -1
	 **/
	static int getTotalLength(const ACE_Message_Block& src);

	static int encode(ACE_Message_Block& dest, const ACE_Message_Block& src);
	static int encode(char* dest, int destSize, const char* src, int srcSize);
	

private:
};


#endif
