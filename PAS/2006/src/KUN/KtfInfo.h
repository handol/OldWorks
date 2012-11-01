#ifndef KTF_INFO_H
#define KTF_INFO_H

/**
@brief KTF 가상 번호 및 신규 브라우저 검사
*/

enum  KtfBrowserType {
	BROWSER_TYPE_ME =	1,
	BROWSER_TYPE_KUN =	2,
	BROWSER_TYPE_OTHER = 3
} ;


class KtfInfo
{
	public:
	static KtfBrowserType checkBrowserType(const char *browser);
	static bool isNewBrowser(const char *browser);
	static bool isVirtualNumber(const char *phoneNumber);
};

#endif
