#ifndef _COOKIE_H_
#define _COOKIE_H_

#include <ctype.h>

#include "Exception.h"
#include "Common.h"
#include "HttpTypes.h"
#include "CookieLine.h"
#include "CookieItem.h"

class CookieDB
{
public:
	typedef HTTP::path_t path_t;

public:
	/// 쿠키 입력
	/**
	 * HTTP 헤더에 "Set-cookie: pref=compact; domain="airtravelbargains.com"; path=/autos/" 와 같은 내용이 있을 경우
	 * cookie 파라미터에 "Set-cookie:" 부분을 제외한 나머지 "pref=compact; domain="airtravelbargains.com"; path=/autos/" 를
	 * 입력한다.
	 *
	 * @exception No throw 
	 *
	 * @date 2007/05/30 
	 * @author SeHoon Yang
	 **/
	void set(const cookie_t& cookie);

	/// 쿠키 읽기
	/**
	 * 현재 시스템 시간을 기준으로 만료된 쿠키를 포함되지 않는다.
	 *
	 * HTTP 헤더에 입력되어야 할 쿠키 내용이 반환된다.
	 * 반환 내용은 실제 쿠키로 전송해야 할 HTTP 헤더 내용이 "Cookie: pref=compact" 일 경우
	 * 이 함수의 반환 내용은 "pref=compact" 이다.
	 *
	 * 쿠키 내용이 없다면 빈문자열을 반환한다.
	 *
	 * @return 쿠키 내용
	 *
	 * @exception No throw 
	 *
	 * @date 2007/05/30 
	 * @author SeHoon Yang
	 **/
	cookie_t get(const host_t& domain, const path_t& path);

	// for debug
	void printLog();

private:
	/// Convert cookie date string to unix timestamp
	/**
	 * On entry:
	 *    szDate = zero-terminated HTTP date string
	 * On exit:
	 *    Returns WDATE date (seconds since some system-dependent point in time)
	 *            WDATE_INVALID if invalid.
	 *
	 * For now I just support the following date formats:
	 *    1) "Dow, dd Mon yyyy hh:mm:ss GMT"  --  'dow' and 'mon' are three-letter names  (standard HTTP date)
	 *    2) "Dow, dd-Mon-yyyy hh:mm:ss GMT"  --  standard HTTP Cookies date
	 *    3) Variations of the above where 'dow' is more than three letters
	 *    4) Variations of the above where 'yyyy' is 2-digits (pivot point is DATEPIVOTYEAR).
	 *
	 * NOTE: Time zone is not considered.  Time returned is in GMT.
	 *
	 * NOTE: One older format used in e-mail (unlikely to ever encounter in HTTP) is ctime()'s output,
	 *       and is currently NOT supported:  "Dow Mon dd hh:mm:ss yyyy"
	 **/
	time_t parseCookieDate(const char* date);

	/// Look for 3-letter month code (numbered 0..11).  Return TRUE if a matching one was found.
	/**
	 * If a month name is found, advance sz, set numResult, and return TRUE.
	 * Otherwise, return FALSE.
	 *
	 * If it looks a bit obfuscated . . . . hey, I needed some excitement.
	 *
	 * CCC : This could share code with wcookie.c, PrvIsDomainTooShort()
	 **/
	bool skipMon(const char* *ppsz, short *pnumResult);

	/// Read number at sz[], skipping preceding whitespace and one following punctionation mark (-/,:)
	/**
	 * If a number is found, advance sz, set numResult, and return TRUE.
	 * Otherwise, return FALSE.
	 **/
	bool skipNum(const char* *ppsz, short *pnumResult);

	/// Look for niddle[0..2] (case-insensitive) in a list of 3-letter sequence
	/**
	 * haystack[] = sequence of 3-letter sequences (E.g.: "janfebmar...")
	 *
	 * Note:  niddle[] and haystack[] are assumed to consist of letters.
	 * Note:  Length of haystack[] MUST be an integral multiple of 3.
	 */
	int match3Letter(const char* haystack, const char* niddle);

	/// 쿠키 아이템 등록
	/**
	 * 이미 존재하는 쿠키라면 내용을 update 하고, 존재하지 않는다면 추가한다. 
	 *
	 * @param item 등록할 쿠키 아이템
	 *
	 * @exception No throw 
	 *
	 * @date 2007/05/31 
	 * @author SeHoon Yang
	 **/
	void add(const CookieItem& item);

private:
	VCookieItem _items;
};

#endif
