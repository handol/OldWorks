#ifndef HANCHAR_H
#define HANCHAR_H

#include <stdio.h>

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef uint2
typedef unsigned short uint2;
#endif

#ifndef uint4
typedef unsigned int uint4;
#endif

#ifndef IN_RANGE
#define IN_RANGE(X, A, B) ((A)<=(X) && (X) <= (B))
#endif

#ifndef OUT_RANGE
#define OUT_RANGE(X, A, B) ((A)>(X) || (X) > (B))
#endif

/**
한글 데이타는 완성형을 가지고 처리하도록 구현한다.
완성형 한글 코드를 형태소 분석하기 위해서는 자소 분리, 받침없는 글자 찾기 등의
기능이 필요하다.
이를 위해 중간단계로 조합형 코드를 이용한다.
즉, 완성형 코드를 조합형 코드로 mapping한 테이블을 바탕으로 자소 분리를
구현하고,
자소 분리를 바탕으로 받침없는 글자 찾기를 구현한다.

받침이 없는 문자는  조합형 종성값이 1 이다.
초성이  없는 문자는  조합형 초성값이 13 (ㅇ 에 해당) 이다.
*/


/**
용어 정리:
WS -- HanChar. 완성형.
JH -- Johap. 조합형.
자음 -- consonant, 子音
모음 -- vowel, 母音
*/
#define	S_JOSA	(0x100) // 종성의 ㄴ, ㄹ 은 조사 후보
#define	S_EOMI	(0x200) // 종성의 ㄴ, ㄹ, ㅁ, ㅂ 은 어미 후보

#define	S_CONSONANT	(0x20)  // 자음, 모음
#define	S_COMBO	(0x10)  // ㄺ , ㄶ 등의 복합 자음
#define	S_HARSH	(0x08) // ㅋ, ㅌ 등의 거친 소리
#define	S_DOUBLE	(0x04) // 이중 (복 ) 모음 , 자음 - 경음 ( ㄲ , ㄸ )
#define	S_VOICE	(0x02)	// 유성, 무성
#define	S_POSITIVE	(0x01) // 양성, 음성
#define	S_NONE		(0x00)

#define	IS_CONSONANT(X) ((X)&S_CONSONANT)
#define	IS_DOUBLE(X) ((X)&S_DOUBLE)


/*=== 완성코드 테이블, 조합코드 테이블 index 와 관련된 macro ===*/

/* 완성형 코드 정보 표 */
#define NUM_WS_UMJUL        2350 // 25 *94 ==  2350 // 가(0xB0A1) ~ 힝(0xC8FE)
#define NUM_WS_JAMO        51  // 자모 코드 =  0xA4A1 ~ 0xA4D3

/*  0xB0A1~0xC8FE : 6,238 이지만 . 2350 자만 실제 사용 */
#define	IS_WS_UMJUL(X)	( 0xB0A1 <= (X) && (X) <= 0xC8FE)
#define	IS_WS_JAMO(X)	( 0xA4A1 <= (X) && (X) <= 0xA4D3)
#define	UINT2_TO_BYTE(WS, HI, LO)	{ HI = (WS)>>8; LO = (WS) & 0xFF; }

#define	BYTE_TO_UINT2(HI, LO)	(uint2)( (byte)(HI)<<8 | (byte) (LO) )
#define	STR_TO_UINT2(STR)	(uint2)( (byte)(STR[0])<<8 | (byte) (STR[1]) )

#define	WS_UMJUL_TO_IDX(HI, LO)	( ((HI) - 0xb0) * 94 + ((LO) - 0xa1))
#define	WS_JAMO_TO_IDX(WS)	((int)((WS)-0xA4A1) + NUM_WS_UMJUL)

/* 조합 코드 표 */
#define	IS_JHCODE(X)	( 0x8444 <= (X) && (X) <= 0xd3b7)
#define	JH2WS_TABSIZE	(((int)0xd3b7 - (int)0x8444) + 1) //== 19,287 + 64
#define	JH_Code2Idx(X)	((int)(X) - (int)0x8444)


/*=== 조합 코드 , 자소 검사 관련 macro ===*/
#define	JH_FstSnd(J)	(byte)(((J) & 0x7C00) >> 10)	// 조합코드의 초성 5bit 값
#define	JH_MidSnd(J)	(byte)(((J) & 0x03E0) >> 5)	// 조합코드의 중성 5bit 값
#define	JH_LastSnd(J)	(byte)((J) & 0x001F) // 조합코드의 종성 5bit 값

#define	JH_replaceFstSnd(J, NewSnd)	(uint2)(( (J) & (~0x7C00)) | ((uint2)NewSnd << 10))
#define	JH_replaceMidSnd(J, NewSnd)	(uint2)(( (J) & (~0x03E0)) | ((uint2)NewSnd << 5))
#define	JH_replaceLastSnd(J, NewSnd)	(uint2)(( (J) & (~0x001F)) | ((uint2)NewSnd))

/*=== 자소 값 특성 ===*/
#define	FstSnd_Nothing	(13)  // 자소 이응 ㅇ
#define	FstSnd_N	(4)
#define	FstSnd_B		(9)
#define	FstSnd_N	(4)

#define	MidSnd_A	(3) // 아
#define	MidSnd_EO	(7) // 어
#define	MidSnd_EU	(27) // 으
#define	MidSnd_AE	(4) // 애
#define	MidSnd_YAE	(6) // 얘
#define	MidSnd_YA	(5) // 야
#define	MidSnd_YEO	(11) // 여
#define	LastSnd_Nothing	(1) // 받침 없는 것
#define	LastSnd_N	(5)
#define	LastSnd_D	(8)
#define	LastSnd_L	(9)
#define	LastSnd_M	(17)
#define	LastSnd_B	(19)
#define	LastSnd_S	(21)
#define	LastSnd_SS	(22) // 자소 쌍 시옷 ㅆ
#define	LastSnd_H	(29)

/*
J : Johap Code
JASO: 자모 분리된 초성, 중성, 종성 등의 자모 값.

받침이 없는 문자는  조합형 종성값이 1 이다.
초성이  없는 문자는  조합형 초성값이 13 (ㅇ 에 해당) 이다.
*/
#define	Has_NO_FstSnd(J)	( JH_FstSnd(J)==FstSnd_Nothing)
#define	Has_FstSnd(J)	( JH_FstSnd(J)!=FstSnd_Nothing)
#define	Has_NO_LastSnd(J)	(JH_LastSnd(J)==LastSnd_Nothing)
#define	Has_LastSnd(J)	(JH_LastSnd(J)!=LastSnd_Nothing)


#define	Is_Josa_LastSnd(JASO)	( HanChar::HLastSnd[(JASO)].property & S_JOSA)
#define	Is_Eomi_LastSnd(JASO)	( HanChar::HLastSnd[(JASO)].property & S_EOMI)
#define	Is_Posi_MidSnd(JASO)	( (HanChar::HMidSnd[(JASO)].property) & S_POSITIVE)


/* HwordType.Conj  :: HDict.h 에서 사용함. */
enum {
	D_conj = 1,
	L_conj,
	B_conj,
	S_conj,
	H_conj,
	Go_conj,
	Come_conj,
	Leora_conj,  // 러라
	Leo_conj,  // 러
	Reu_conj,  // 르
	Yeo_conj,  // 여
	U_conj,  // 우
	Eu_conj  // 으
};

#define	P_JOSA1	(0x01)
#define	P_JOSA2	(0x01 << 1)
#define	P_EOMI1	(0x01 << 2)
#define	P_EOMI2	(0x01 << 3)
#define	P_D_IRRE	(0x01 << 4)
#define	P_B_IRRE	(0x01 << 5)
#define	P_S_IRRE	(0x01 << 6)
#define	P_H_IRRE	(0x01 << 7)
#define	P_REU_IRRE	(0x01 << 8)
#define	P_GO_IRRE	(0x01 << 9)  // 거라 불규칙 - 가거라  go
#define	P_COME_IRRE	(0x01 << 10) // 너라 불규칙 - 오너라  come

/* SylInfo *X  */
#define	IS_SYL_D_IRRE(X) ((X)->property & P_D_IRRE)
#define	IS_SYL_B_IRRE(X) ((X)->property & P_B_IRRE)
#define	IS_SYL_S_IRRE(X) ((X)->property & P_S_IRRE)
#define	IS_SYL_H_IRRE(X) ((X)->property & P_H_IRRE)
#define	IS_SYL_REU_IRRE(X) ((X)->property & P_REU_IRRE)
#define	IS_SYL_GO_IRRE(X) ((X)->property & P_GO_IRRE)
#define	IS_SYL_COME_IRRE(X) ((X)->property & P_COME_IRRE)

typedef struct _firstJasoInfo {
	char	draw[4]; // 완성형 코드 문자열.
	byte	org;
	byte double_snd;
	byte	harsh_snd;
	uint4	property;
} firstJasoInfo;

typedef struct _secondJasoInfo {
	char	draw[4]; // 완성형 코드 문자열.
	/*
	이중 모음인 경우 org1 과 org2 에 이중 모음을 구성하는 기본 모음의 index 값을 저장.
	ㅑ , ㅛ 등의 이중 모음은 org1 만 존재.
	*/
	byte	org1;
	byte org2;
	byte	harsh_snd;
	uint4	property;
} secondJasoInfo;

typedef struct _thirdJasoInfo {
	char	draw[4]; // 완성형 코드 문자열.

	// 초성 자모 에 mapping
	byte	maptocho;

	/* 복합 자음인 경우 앞의 것은 종성 기준으로 index 값을   jong_pos 에 저장 , 뒤의 것은 초성 기준의 index 값
	복합자음이 아닌 경우 : 원형 ( 경음, 격음 이 아닌 자음 )의 종성 기준 index  값을 jong_pos 에  저장.
	*/
	byte	jong_pos;
	byte cho_pos;

	uint4	property;
} thirdJasoInfo;

/*
음절 코드 와 자모 코드 모두 테이블에 포함.
앞 2350 개 = 음절
뒤 51개 =  자모 코드 =  0xA4A1 ~ 0xA4D3
*/

/*
음절 특성 (syllable).
조합형 코드를 내부 한글 코드로 사용함. 자소 분리, 자소 변환 등의 연산이 쉬우므로.
조합형 코드 ===> 완성형 코드 매핑 테이블 역할 수행.
*/
typedef struct _JHSyllableInfo {
	uint2	jhcode;  // 조합 코드
	uint2	wscode; /* 조합형 코드 ===> 완성형 코드 매핑 테이블 */
	uint4	property; // 음절 특성 === syllable property
} SylInfo;

class	HanChar {
	private:
	/*=== tables ===*/
	static	uint2 Ws2JhTab[NUM_WS_UMJUL];
	static	uint2 Ws2JhTab_Jamo[NUM_WS_JAMO] ;
	static	SylInfo *Jh2WsTab;

	/*=== functions ===*/
	static	int	alloc();
	static	int	fillTables();
	static	void  jh2jaso(uint2 jh_code, byte jaso3[]);

	public:
	/*===  PUBLIC ==== */

	static	firstJasoInfo HFstSnd[32];
	static	secondJasoInfo HMidSnd[32];
	static	thirdJasoInfo HLastSnd[32];

	static	int	prepare();

	static	void  printCodeTab(FILE *fp=stdout);
	static	void  printWS(SylInfo *wsCode, FILE *fp=stdout);
	static	uint2	string2jh(char *wsstr);
	static	uint2	ws2jh(uint2 ws_code);
	static	uint2	jh2ws(uint2 jh_code);

	static	void	 ws2jaso(uint2 ws_code, byte jaso3[]);
	
	static	byte	 wsstr2fstsnd(char *wsstr);
	
	static	uint2 jaso2jh(byte jaso3[]);

	static	 uint2 jaso2ws(byte jaso3[]);
	static	 void jaso2string(byte jaso3[], char *wsstr);
	static	void  jh2string(uint2 jh_code, char *wsstr ) ;

	static	SylInfo* getSylInfo_wscode(uint2 ws_code);
	static	SylInfo* getSylInfo(uint2 ws_code);
	static	SylInfo* getSylInfo(char *str);
	static	int	getSylInfo(SylInfo **array, int maxarr, char *str);
	static	int	syl2string(SylInfo **array, int maxarr, char *str);

	static	 void	markSyllableProperty(char *syl, uint4 prop);
	static	void	markSyl_Irregular(byte Conj, char *syllable);

	static	void	printSylProperty(uint4 prop);
	static	void	printSylInfo();

	static	void	replaceLastSnd(char *wsstr, byte lastsnd=LastSnd_Nothing);
	static	void	 replaceLastSnd(char *wsstr , uint2 curr_jhcode, byte newlastsnd);

	static void	replaceMidSnd(char *wsstr , byte newlastsnd);
	static void	 replaceMidSnd(char *wsstr , uint2 curr_jhcode, byte newlastsnd);

	static	void	replaceMidAndLastSnd(char *wsstr , byte newmidsnd, byte newlastsnd);
	static	void	 replaceMidAndLastSnd(char *wsstr , uint2 curr_jhcode, byte newmidsnd, byte newlastsnd);

	static	int	strncmpHan(char *str1, char *str2, int len);

	static	int	splitHanSyllable(char *result, char *str);
	static	int	splitHanSyllable(char *result, uint2  syl_jhcode);
	static	int	isHanBlank(byte *word);
	static	int	isSentenceEnd(byte *word, int wordlen);
	static	int	isSentenceEndMark(byte *word);
	static	void removeSpecialChar(byte *s);

	static	int	getHangulOnly(char *dest, int maxlen, char *src);

	static	int	isHangulWord(char *src, int maxlen);
};

#endif
