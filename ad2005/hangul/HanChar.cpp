
#include "HanChar.h"

#include "HanCodeTab.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
2005.7.21
@todo
유성음, 무성음 구분 테이블

이중 모음, 복모음 결합에 의한 코드 변환 테이블

조합코드의 자소 값을 기준으로 하여야 할 것이다.

조합-to-완성 함수가 필요한가...
*/

/**
*/
SylInfo *HanChar::Jh2WsTab = 0;

/**
ws2jh_byte(완성형 한글의 첫번째 바이트, 두번째 바이트)

완성형 한글 한 글자를 넘겨받아 조합형 한글코드를 return.
 */

uint2  HanChar::string2jh(char *wsstr) {
	byte c1 = (byte) wsstr[0];
	byte c2 = (byte) wsstr[1];
	uint2 ws_code;
	uint2 jh_code;

	ws_code = (uint2) (c1 << 8) | (uint2) c2;

	if (IN_RANGE(ws_code, 0xB0A1, 0xC8FE)) {
		jh_code = Ws2JhTab[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IN_RANGE(ws_code, 0xA4A1, 0xA4D3)) {
		jh_code = Ws2JhTab_Jamo[c2-0xa1];
	}
	else
		jh_code = 0x2020;

	return jh_code;
}

/**
ws2jh_uint2(완성형 한글 코드)

완성형 한글 한 글자를 넘겨받아 조합형 한글코드를 return.
 */
uint2  HanChar::ws2jh(uint2 ws_code) {
	byte	c1, c2;
	uint2 jh_code;

	c1 = ws_code >> 8;
	c2 = ws_code & 0xFF;

	if (IN_RANGE(ws_code, 0xB0A1, 0xC8FE)) {
		jh_code = Ws2JhTab[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IN_RANGE(ws_code, 0xA4A1, 0xA4D3)) {
		 jh_code = Ws2JhTab_Jamo[(c2-0xa1)];
	}
	else jh_code = 0x2020;

	return jh_code;
}



inline void  HanChar::jh2jaso(uint2 jh_code, byte jaso3[])
{
	jaso3[0] = (jh_code & 0x7C00) >> 10;
	jaso3[1] = (jh_code & 0x03E0) >> 5;
	jaso3[2] = (jh_code & 0x001F);

	if ( jaso2jh(jaso3) != jh_code) {
		printf("jaso2jh() wrong!%X -  %X %X %X\n", jh_code, jaso3[0], jaso3[1], jaso3[2]);
	}
}

 inline void	 HanChar::ws2jaso(uint2 ws_code, byte jaso3[])
{
	jh2jaso ( ws2jh(ws_code), jaso3 );
}

/**
완성형 문자의 초성값 구하기.
*/
byte	 HanChar::wsstr2fstsnd(char *wsstr) {
	byte c1 = (byte) wsstr[0];
	byte c2 = (byte) wsstr[1];
	uint2 ws_code;

	ws_code = (uint2) (c1 << 8) | (uint2) c2;
	uint2 jh_code = ws2jh(ws_code);
	return ( (jh_code & 0x7C00) >> 10);
}
 
/**
jaso 3 개 (초, 중, 종성) 을 합쳐 하나의 조합 코드로 만든다.
촤상위 bit 는 1로 세팅해야 한다.
*/
uint2 HanChar::jaso2jh(byte jaso3[])
{
	/* 1 bit + 5 bit + 5 bit + 5 bit */
	return (uint2) ((uint2) ((byte)jaso3[0] << 10) | (uint2)((byte)jaso3[1] << 5) | (uint2)((byte)jaso3[2]) |(uint2)0x8000);
}

inline uint2  HanChar::jh2ws(uint2 jh_code)
{

	if (IS_JHCODE(jh_code)) {
		return Jh2WsTab[JH_Code2Idx(jh_code)].wscode;
	}
	else
		return 0;
}

void  HanChar::jh2string(uint2 jh_code, char *wsstr )
{
	uint2 ws_code=0;
	if (IS_JHCODE(jh_code)) {
		ws_code = Jh2WsTab[JH_Code2Idx(jh_code)].wscode;
		wsstr[0] = (byte) (ws_code >> 8);
		wsstr[1] = (byte) (ws_code & 0xFF);
	}
	else {
		wsstr[0] = wsstr[1] = 0;
	}
}

 inline uint2 HanChar::jaso2ws(byte jaso3[])
{
	return jh2ws ( jaso2jh(jaso3) );
}

 inline void HanChar::jaso2string(byte jaso3[], char *wsstr)
{
	uint2 wscode =  jh2ws ( jaso2jh(jaso3) );
	wsstr[0] = (byte) (wscode >> 8);
	wsstr[1] = (byte) (wscode & 0xFF);
}

SylInfo* HanChar::getSylInfo_wscode(uint2 ws_code)
{
	uint2 jh_code = ws2jh(ws_code);
	if (IS_JHCODE(jh_code)) {
		return &Jh2WsTab[JH_Code2Idx(jh_code)];
	}
	else return 0;
}

SylInfo* HanChar::getSylInfo(uint2 jh_code)
{
	if (IS_JHCODE(jh_code)) {
		return &Jh2WsTab[JH_Code2Idx(jh_code)];
	}
	else return 0;
}

SylInfo* HanChar::getSylInfo(char *str)
{
	uint2 jh_code = string2jh(str);
	if (IS_JHCODE(jh_code)) {
		return &Jh2WsTab[JH_Code2Idx(jh_code)];
	}
	else return 0;

}

/**
*/
int	HanChar::getSylInfo(SylInfo **array, int maxarr, char *str)
{
	int	i;
	SylInfo *info;
	for(i=0; i<maxarr; i++, str+=2) {
		info = getSylInfo(str);
		if (info==0) break;

		array[i] = info;
	}
	return i;
}

int	HanChar::syl2string(SylInfo **array, int maxarr, char *str)
{
	int	i;
	SylInfo *info;
	for(i=0; i<maxarr; i++, str+=2) {
		info = array[i];
		if (info==0) break;
		str[0] = (byte) (info->wscode >> 8);
		str[1] = (byte) (info->wscode & 0xFF);
	}
	return i;
}
/**
주어진 한 음절에서 , 주어진 받침값으로 변경한 새  음절을 만든다.


받침이 없는 문자는  조합형 종성값이 1 이다.
초성이  없는 문자는  조합형 초성값이 13 (ㅇ 에 해당) 이다.
*/
void	HanChar::replaceLastSnd(char *wsstr , byte newlastsnd)
{
	SylInfo *info = HanChar::getSylInfo(wsstr);
	if (info) {
		replaceLastSnd(wsstr, info->jhcode, newlastsnd);
	}
	else {
	}
}

/**
inline  함수로 만들고 싶었지만 inline 함수를 cpp 화일에 정의한 경우 (*.h 내에 정의한 것이 아닌 경우)
다른 cpp 화일에서 그 inline 함수를 참조 할 수 없다. --> "unresolved external"  오류 발생.

refer - http://www.parashift.com/c++-faq-lite/inline-functions.html
*/
void	 HanChar::replaceLastSnd(char *wsstr , uint2 curr_jhcode, byte newlastsnd)
{
	uint2 new_jhcode = JH_replaceLastSnd(curr_jhcode, newlastsnd);

	jh2string( new_jhcode, wsstr);
}

/**
*/
void	HanChar::replaceMidSnd(char *wsstr , byte newlastsnd)
{
	SylInfo *info = HanChar::getSylInfo(wsstr);
	if (info) {
		replaceMidSnd(wsstr, info->jhcode, newlastsnd);
	}
	else {
	}
}

void	 HanChar::replaceMidSnd(char *wsstr , uint2 curr_jhcode, byte newlastsnd)
{
	uint2 new_jhcode = JH_replaceMidSnd(curr_jhcode, newlastsnd);

	jh2string( new_jhcode, wsstr);
}

/**
중성, 종성을 한번에 바꾸기.

*/

void	HanChar::replaceMidAndLastSnd(char *wsstr , byte newmidsnd, byte newlastsnd)
{
	SylInfo *info = HanChar::getSylInfo(wsstr);
	if (info) {
		replaceMidAndLastSnd(wsstr, info->jhcode, newmidsnd, newlastsnd);
	}
	else {
	}
}

void	 HanChar::replaceMidAndLastSnd(char *wsstr , uint2 curr_jhcode, byte newmidsnd, byte newlastsnd)
{
	uint2 new_jhcode = JH_replaceMidSnd(curr_jhcode, newmidsnd);
	new_jhcode = JH_replaceLastSnd(new_jhcode, newlastsnd);

	jh2string( new_jhcode, wsstr);
}

/**
완성형 코드표 (음절 테이블 ) 에 조사, 어미 가 가능한 음절에 표시를 한다.
*/
 void	HanChar::markSyllableProperty(char *syl, uint4 prop)
{
	SylInfo *info = getSylInfo(syl);
	if (info) info->property |= prop;
}

/**
완성형 코드표 (음절 테이블 ) 에 불규칙 활용으로 만들어 지는 음절에 표시를 한다.


그외 불규칙 : 어간의 마지막 음절이 변화 한다.  ~~다.

르 불규칙 : 어간의 마지막 음절 앞부분이 변화한다.  ~~르다.
빠르다   -- 빨라

거라, 너라 는 어간이 변하지 않는다.
*/
void	HanChar::markSyl_Irregular(byte Conj, char *syllable)
{
	uint4 prop = 0;
	byte	newlastsnd= 0;

	switch (Conj) {
		case D_conj: prop = P_D_IRRE;  newlastsnd = LastSnd_L; // ㄷ --> ㄹ
			break;

		case B_conj: prop = P_B_IRRE;	newlastsnd = LastSnd_Nothing; // ㅂ 탈락
			break;

		case S_conj: prop = P_S_IRRE; newlastsnd = LastSnd_Nothing; // ㅅ 탈락
			break;

		case H_conj: prop = P_H_IRRE; newlastsnd = LastSnd_Nothing; // ㅎ 탈락
			break;

		case Reu_conj: prop = P_REU_IRRE; newlastsnd = LastSnd_L; // ㄹ 추가
			break;

		/*
		case Go_conj: prop = P_GO_IRRE; break;
		case Come_conj: prop = P_COME_IRRE; break;
		*/
	}

	if (prop) {
		SylInfo *info = HanChar::getSylInfo(syllable);

		if (info==0) {
			printf("Conj=%d syl=%s\n", Conj, syllable);
		}

		if (newlastsnd && info) {
			/* 종성 값이 변화된 새 완성형 값을 구해 , 거기에 표시 한다. */
			uint2 new_jhcode = JH_replaceLastSnd(info->jhcode, newlastsnd);
			char tmp[4];
			tmp[2] = 0;
			info = HanChar::getSylInfo(new_jhcode);
			if (info) info->property |= prop;



			if (Conj == H_conj) {
				byte midsnd = JH_MidSnd(new_jhcode);

				// ㅎ 불규칙의 경우: ㅎ 만 탈락하는 것이 아니라 모음도 바뀐다.
				// 누렇다 --> 누래
				// 파랗다 --> 파래
				// 하얗다 --> 하얘
				jh2string(new_jhcode, tmp);
				//printf("H : %s\n",  tmp);

				uint2 new_jhcode2;

				if (midsnd ==MidSnd_A || midsnd == MidSnd_EO)
					new_jhcode2 = JH_replaceMidSnd(new_jhcode, MidSnd_AE);
				else
					new_jhcode2 = JH_replaceMidSnd(new_jhcode, MidSnd_YAE);

				info = HanChar::getSylInfo(new_jhcode2);
				if (info) info->property |= prop;
				jh2string(new_jhcode2, tmp);
				//printf("H : %s\n",  tmp);
			}
		}
		else {
			if (info) info->property |= prop;
		}
	}
	//HanChar::markSyllableProperty(syllable, prop);
}

/**
축약형을 확장하는 것.

5. 축약의 종류

    아래의 종류 중 a)를 제외한 나머지는 처리하였음
    a) 여약어 : ㅐ <--> ㅏ + 여        예) 하+었다 <==> 하였다 <==> 했다
    b) 외약어 : ㅗㅣ <--> ㅗ + ㅣ ??
                ㅗㅐ <--> ㅗㅣ + 어    예) 되+었다 <==> 되었다 <==> 됐다
    c) 우약어 : ㅜㅓ <--> ㅜ + 어      예) 주+었다 <==> 주었다 <==> 줬다
    d) 와약어 : ㅗㅏ <--> ㅗ + 아      예) 오+았다 <==> 오았다 <==> 왔다
    e) 이약어 : ㅕ <--> ㅣ + 어        예) 지+었다 <==> 지었다 <==> 졌다
    f) 아약어 : ㅏ <--> ㅏ + 아        예) 가+았다 <==> 가았다 <==> 갔다
*/
int	HanChar::splitHanSyllable(char *result, char *str )
{
	SylInfo *codeInfo = getSylInfo(str);

	if (codeInfo==0) {
		return 0;
	}
	printWS(codeInfo);
	return splitHanSyllable(result, codeInfo->jhcode);
}

int	HanChar::splitHanSyllable(char *result, uint2  syl_jhcode)
{
	byte midsnd = JH_MidSnd(syl_jhcode);
	if ( HMidSnd[midsnd].org1 && HMidSnd[midsnd].org2 ) {
		byte	newjaso[3];

		newjaso[0] = JH_FstSnd(syl_jhcode);
		newjaso[1] = HMidSnd[midsnd].org1;
		newjaso[2] = LastSnd_Nothing;
		jaso2string(newjaso, result);

		newjaso[0] = FstSnd_Nothing;
		newjaso[1] = HMidSnd[midsnd].org2;
		newjaso[2] = JH_LastSnd(syl_jhcode);
		jaso2string(newjaso, result+2);

		result[4] = 0;
		return 1;
	}
	else {
		result[0] = 0;
		return 0;
	}
}

int	HanChar::alloc()
{
	if (Jh2WsTab) return 0;

	Jh2WsTab = new SylInfo [JH2WS_TABSIZE];
	memset(Jh2WsTab, 0, sizeof(SylInfo) * JH2WS_TABSIZE );
	return 0;
}

/**
Jh2WsTab 을 할당하고, 내용을 채우는 함수.

완성형 한글 코드를 형태소 분석하기 위해서는 자소 분리, 받침없는 글자 찾기 등의
기능이 필요하다.
이를 위해 중간단계로 조합형 코드를 이용한다.
즉, 완성형 코드를 조합형 코드로 mapping한 테이블을 바탕으로 자소 분리를
구현하고,
자소 분리를 바탕으로 받침없는 글자 찾기를 구현한다.

printCodeTab() 을 실행하여 Jh2WsTab 채우기가 제대로 되었는지 확인할 수 있다.
*/
int	HanChar::fillTables()
{
	int	n=0;
	byte c1, c2;
	uint2	ws_code;
	uint2	jh_code;

	int	cnt_nobatchim = 0;

	for(ws_code = 0xA4A1; ws_code <= 0xA4D3; ws_code++) {
		jh_code = ws2jh(ws_code);

		if (IS_JHCODE(jh_code)) {
			int	idx = JH_Code2Idx(jh_code);
			/* 조합형 코드 --> 완성형 코드 매핑 테이블 채우기 */
			Jh2WsTab[idx].wscode = ws_code;
			Jh2WsTab[idx].jhcode = jh_code;

		}
	}

	for (c1 = 0xB0; c1 <= 0xC8; c1++) {
		for (c2 = 0xA1; c2 <= 0xFE; c2++, n++) {
			ws_code = (c1<<8 | c2);
			jh_code = ws2jh(ws_code);

			if (IS_JHCODE(jh_code)) {
				int	idx = JH_Code2Idx(jh_code);
				/* 조합형 코드 --> 완성형 코드 매핑 테이블 채우기 */
				Jh2WsTab[idx].wscode = ws_code;
				Jh2WsTab[idx].jhcode = jh_code;

			}
		}
	}



	// 실행 결과 cnt_nobatchim 은 349 개가 나온다.  한글 전체적으로 가능한 개수는 395 개다.
	//printf("NO batchim: %d\n", cnt_nobatchim);
	return 0;
}


void	HanChar::printSylProperty(uint4 prop)
{
	int	i;
	int	cnt=0;
	SylInfo *info;
	printf("### syllable property : %X \n", prop);
	for(i=0, info = Jh2WsTab; i<JH2WS_TABSIZE; i++, info++) {
		if (info->property & prop) {
			printf("%c%c ", info->wscode >> 8, info->wscode & 0xFF);
			if (++cnt % 10 == 0) printf("\n");
		}
	}
	printf("\n# %d\n\n", cnt);
}

void	HanChar::printSylInfo()
{
	for(uint4 prop =1; prop <= P_COME_IRRE; prop <<= 1)
		printSylProperty(prop);
}

void  HanChar::printWS(SylInfo *info, FILE *fp)
{
	fprintf(fp,
		"%X %X %c%c %2s %2s %2s  %2d %2d %2d %X\n",

		info->wscode,
		info->jhcode,

		info->wscode >> 8,
		info->wscode & 0xFF,

		HFstSnd[JH_FstSnd(info->jhcode)].draw,
		HMidSnd[JH_MidSnd(info->jhcode)].draw,
		HLastSnd[JH_LastSnd(info->jhcode)].draw,

		JH_FstSnd(info->jhcode),
		JH_MidSnd(info->jhcode),
		JH_LastSnd(info->jhcode),

 		info->property
	);
}


/**
Jh2WsTab 이 제대로 되었는지 검사.
// KS 완성형 2,350 자

=== 실행 결과 ===
...
A4 B5 == A4B5
C8FD  힛  히 ㅎ ㅣ ㅅ ㅅ
A4 B7 == A4B7
C8FE  힝  히 ㅎ ㅣ ㅇ ㅇ
*/

void  HanChar::printCodeTab(FILE *fp)
{
	byte c1, c2;
	SylInfo *info;
	uint2	ws_code, jh_code;

	for (c1 = 0xB0; c1 <= 0xC8; c1++) {
		for (c2 = 0xA1; c2 <= 0xFE; c2++) {
			ws_code = BYTE_TO_UINT2(c1, c2);
			jh_code = ws2jh( ws_code  );
			if (! IS_JHCODE(jh_code)) {
				printf("JH code range wrong! [%X]\n", jh_code);
				continue;
			}
			info = & Jh2WsTab[ JH_Code2Idx(jh_code) ];

			if (info->wscode != ws_code || info->jhcode != jh_code ) {
				fprintf(fp, "Jh2WsTab is wrong !\n");
				break;
			}
			printWS(info);

		}
	}

	for(ws_code = 0xA4A1; ws_code <= 0xA4D3; ws_code++) {
		jh_code = ws2jh( ws_code  );
		info = & Jh2WsTab[ JH_Code2Idx(jh_code) ];
		if (! IS_JHCODE(jh_code)) {
				printf("JH code range wrong! [%X]\n", jh_code);
				continue;
		}
		if (info->wscode != ws_code || info->jhcode != jh_code ) {
			fprintf(fp, "Jh2WsTab is wrong !\n");
			break;
		}
		printWS(info);
	}
}

/**
한글음절 string 간의 비교.

무슨 말인들   -- ㄴ들
'인들' 과 'ㄴ들' 을 비교.
str2 에만 'ㄴ', 'ㄹ' 등의 자모 음절을 가질 수 있다.

st1 은 원형 어절.
str2 는 문법 형태소
*/
int	HanChar::strncmpHan(char *str1, char *str2, int len)
{
	int	n=0;
	while(*str1 && n < len) {
		if (str1[0] != str2[0] || str1[1] != str2[1]) {
			SylInfo *info1 = getSylInfo(str1);
			SylInfo *info2 = getSylInfo(str2);

			if (JH_LastSnd(info1->jhcode) == JH_LastSnd(info2->jhcode) )
			{
				continue;
			}
			else {
				return (int) (info1->wscode - info2->wscode);
			}
		}
		n += 2;
		str1 += 2;
		str2 += 2;
	}
	return 0;
}

int	HanChar::isHanBlank(byte *word)
{
	return (word[0]==0xA1 && word[1]==0xA1);	/*blank*/
}


/**
	KSC 5601 sentence ending marks.
*/
int	HanChar::isSentenceEndMark(byte *word)
{
	byte c=word[0];
	byte d=word[1];
	return ((c == 0xA3 && d == 0xAE) ||	/* '.' */
		(c == 0xA3 && d == 0xBF) ||	/* '?' */
		(c == 0xA3 && d == 0xA1) ||	/* '!' */
		(c == 0xA1 && d == 0xA3) ||	/* circle '.' */
		(c == 0xA1 && d == 0xA6) ||	/* '...' */
		(c == 0xA1 && d == 0xBB));	/* right bracket */
}

/**
	KSC 5601 sentence ending marks.
*/
int	HanChar::isSentenceEnd(byte *word, int wordlen)
{
	if (wordlen==0) wordlen = strlen((char*)word);
	if (wordlen >=4
		&& isHanBlank(word+(wordlen-2))  // KSC 5601 blank
		&&  isSentenceEndMark(word+(wordlen-4)))
		return 1;
	else if (wordlen >=2 && isSentenceEndMark(word+(wordlen-2)))
		return 1;
	else
		return 0;
}

/*
	Invalid characters like
		1. non-printable Ascii characters : control/graphic char.s
		2. incomplete Hangul syllables : Ja-Eum, Mo-Eum
		3. KSC 5601 2-byte symbols
	are replaced with blank.

	If you want to remove all Ascii symbols,
	change '!isprint(*s)' to '!isalnum(*s)'.
	In this case, be careful for 'Vitamin-A', 'a.m.'.
*/
void HanChar::removeSpecialChar(byte *s)
{
	byte c, d;

	while (*s) {
		c = *s; d = *(s+1);
		if (c & 0x80) {
			if (c < 0xb0 || !(d & 0x80)) {
				/* non-Hangul, non-Hanja --> blank */
				*s = ' '; *(s+1) = ' ';
			}
			s += 2;
		} else {	/* non-printable char --> blank */
			if (!isprint(*s)) *s = ' ';
			s++;
		}
	}
}

int	HanChar::prepare()
{
	int	res=0;
	alloc();
	res = fillTables();
	return res;
}

int	HanChar::getHangulOnly(char *dest, int maxlen, char *src)
{
	byte c1, c2;
	uint2 ws_code;
	int	n=0;
	while(*src && n<maxlen) {
		c1 = src[0];
		if ((c1 & 0x80) == 0) {
			src++;
			continue;
		}
		c2 = src[1];
		if (c2==0) break;
		src += 2;
		ws_code = BYTE_TO_UINT2(c1, c2);
		if (IS_WS_UMJUL(ws_code)) {
			*dest++ = c1;
			*dest++ = c2;
			n += 2;
		}
		
	}
	*dest = 0;
	return n;
}

/**
return 1 if 'src' is Hangul
return 0 if not
*/
int	HanChar::isHangulWord(char *src, int maxlen)
{
	byte c1, c2;
	uint2 ws_code;
	int	n=0;
	while(*src && n<maxlen) {
		c1 = src[0];
		if ((c1 & 0x80) == 0) {
			return 0;
		}
		c2 = src[1];
		if (c2==0) {
			return 0;
		}
		
		ws_code = BYTE_TO_UINT2(c1, c2);
		if (! IS_WS_UMJUL(ws_code)) {
			return 0;
		}

		src += 2;
		n += 2;
	}

	if (n % 2 == 1)
		return 0;
	
	return 1;
}


#ifdef TEST_MAIN
int	main()
{
	HanChar::prepare();
	//HanChar::printCodeTab();

	char	result[10];


	HanChar::splitHanSyllable(result, "겠");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "둬");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "였");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "됐");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "했");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "졌");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "갔");
	printf("%s\n", result);
	return 0;
}
#endif
