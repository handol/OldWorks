
/** 
@file       HotNumber.h
@author DMS

@brief VoiceN, WINC, HotNumber, 통합검색 등의 특수 URL을 magicn URL로 변환

Hot Number란, 고객이 단말에 114 등의 번호를 입력 후 매직엔 버튼을 누르면
미리 등록된 해당 CP로 연결해주는 서비스를 말한다.
Pasgw에서는 단말로부터 전송 받은 번호(예:#114)를 해당 magicn URL로 변환하여 CP로 요청한다.

@li Hot number
@li WINC
@li voice~~ 
*/

#define	MAXLEN_MAGICN_URL	(512)
class HotNumber
{

public:
	HotNumber() 
	{
		magicnUrl[0] = 0;
	}
	
	int convert(const char *orgurl, int urllength=0);
	int	getHotDigits(const char *src, char *dest, int destMax);
	char	*getConverted() 
	{
		return magicnUrl;
	}

	int	test();
private:
	char magicnUrl[MAXLEN_MAGICN_URL+1];
};
