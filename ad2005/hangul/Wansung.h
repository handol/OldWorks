#ifndef WANSUNG_H
#define WANSUNG_H

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
http://www.kyug.net/bbs/view.php?id=download3&page=2&sn1=&divpage=1&sn=off&ss=on&sc=on&select_arrange=headnum&desc=asc&no=61

에서 형태소 분석기 개발 방법 참조.
강승식 교수의 논문을 분석하여 구현한 사례.

wsCodeInfo.property

** 한국어 음절 특성을 3-tuple 로 표시
      A = ( C, L, N)    C: 품사, L: 형태소 길이(정수) , N: 형태소에서 음절의 위치(정수)
      음절특성 함수
         A<i,j,k>  가 품사가 i, 형태소 길이 j , 음절위치가 k  인 음절의 집합이라고 하면음절 특성 함수
         C_A<i,j,k> 는 다음과 같이 정의
         C_A<i,j,k> : X -> { 0,1}    ; X 는 음절의 전체 집합
           C_A<i,j,k> (x) = 1, if x 가 A<i,j,k> 에 속하면 
                                =0   otherwise
*/

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

/* wsCodeInfo *X  */
#define	IS_SYL_D_IRRE(X) ((X)->property & P_D_IRRE)
#define	IS_SYL_B_IRRE(X) ((X)->property & P_B_IRRE)
#define	IS_SYL_S_IRRE(X) ((X)->property & P_S_IRRE)
#define	IS_SYL_H_IRRE(X) ((X)->property & P_H_IRRE)
#define	IS_SYL_REU_IRRE(X) ((X)->property & P_REU_IRRE)
#define	IS_SYL_GO_IRRE(X) ((X)->property & P_GO_IRRE)
#define	IS_SYL_COME_IRRE(X) ((X)->property & P_COME_IRRE)

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
조합형 한글에서 편리하게 초성, 중성, 종성에 해당하는 비트들을 얻어내기 위한 공용체형 hangul {{{
*/
union hangul {
    byte code[2];
    struct {
        unsigned t:5 ;    // 종성
        unsigned s:5 ;    // 중성
        unsigned f:5 ;    // 초성
        unsigned i:1 ;    // 한영 구분
    } han;
};

/**
용어 정리:
WS -- Wansung. 완성형.
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

/* 완성형 코드 정보 표 */

#define NUM_WS_UMJUL        2350 // 25 *94 ==  2350 // 가(0xB0A1) ~ 힝(0xC8FE)
#define NUM_WS_JAMO        51  // 자모 코드 =  0xA4A1 ~ 0xA4D3

/*  0xB0A1~0xC8FE : 6,238 */
#define	IS_WS_UMJUL(X)	( 0xB0A1 <= (X) && (X) <= 0xC8FE)
#define	IS_WS_JAMO(X)	( 0xA4A1 <= (X) && (X) <= 0xA4D3)
#define	UINT2_TO_BYTE(WS, HI, LO)	{ HI = (WS)>>8; LO = (WS) & 0xFF; }

#define	BYTE_TO_UINT2(HI, LO)	(uint2)( (byte)(HI)<<8 | (byte) (LO) )
#define	STR_TO_UINT2(STR)	(uint2)( (byte)(STR[0])<<8 | (byte) (STR[1]) )

#define	WS_UMJUL_TO_IDX(HI, LO)	( ((HI) - 0xb0) * 94 + ((LO) - 0xa1))
#define	WS_JAMO_TO_IDX(WS)	((int)((WS)-0xA4A1) + NUM_WS_UMJUL)

/* wsCodeInfo 의 자소 값 검사 : WsJhCodeTab.h 참조 */

#define	CHO_NG	(13)  // 자소 이응 ㅇ

#define	JONG_NOT	(1) // 받침 없는 것 
#define	JONG_N	(5)
#define	JONG_D	(8)
#define	JONG_L	(9)
#define	JONG_M	(17)
#define	JONG_B	(19)
#define	JONG_S	(21)
#define	JONG_SS		(22) // 자소 쌍 시옷 ㅆ
#define	JONG_H	(29)

/* X == wsCodeInfo *ptr */
#define	MU_JONGSUNG(X)	( X->jaso[2]==JONG_NOT)
#define	MU_CHOSUNG(X)	( X->jaso[0]==CHO_NG)
#define	DBL_S_BATCHIM(X) ( X->jaso[2]==JONG_SS)  // 쌍시옷  ㅆ 받침 

#define	IS_JONG_JOSA(X)	( Wansung::Jongsung[(X)->jaso[2]].property & S_JOSA)
#define	IS_JONG_EOMI(X)	( Wansung::Jongsung[(X)->jaso[2]].property & S_EOMI)
#define	IS_MID_POSI(X)	( (Wansung::Midsung[(X)->jaso[1]].property) & S_POSITIVE)

/* 조합 코드 표 */
#define	IS_JHCODE(X)	( 0x8861 <= (X) && (X) <= 0xd3b7)
#define	JH2WS_TABSIZE	(((int)0xd3b7 - (int)0x8861) + 1) //== 19,287
#define	JH_Code2Idx(X)	((int)(X) - (int)0x8861)

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

/*음절 코드 와 자모 코드 모두 테이블에 포함.
앞 2350 개 = 음절
뒤 51개 =  자모 코드 =  0xA4A1 ~ 0xA4D3

wsCodeInfo.nobatchim == 0 이면 자모 코드로 볼 수 있다.
*/
typedef struct _wsCodeInfo {
	uint2	wscode; // 이 데이타의 실제 Wansung code
	uint2	nobatchim; // 이 코드에서 받침이  빠진 것. Wansung code
	byte		jaso[3];
	uint2	chosungWs; // 초성 자모에 해당하는 완성형 코드값
	uint2	jongsungWs; // 종성 자모에 해당하는 완성형 코드값
	uint4	property; // 음절 특성 -- syllable property
} wsCodeInfo;

class	Wansung {
	private:
	/*--- tables ---*/
	static	char chosungEng[32][3];
	static	char jungsungEng[32][4];
	static	char jongsungEng[32][3];

	static	char HanFirstSound[32][3];
	static	char HanMiddleSound[32][3];
	static	char HanLastSound[32][3];

	
	
	
	static	uint2 Ws2Jh_table[NUM_WS_UMJUL];
	static	uint2 Ws2Jh_Jamo_tab[NUM_WS_JAMO] ;
	
	static	uint2 *Jh2Ws_table;
	
	static	wsCodeInfo *wsCodeTab;
	static	wsCodeInfo unknownCode;

	/*--- functions ---*/
	static	int	alloc();
	static	uint2 findNobatchim_wscode(int curr_idx);
	static	int	fillTables();
	static	void  jh2jaso(uint2 jh_code, byte jaso3[]);
	
	public: /*===  PUBLIC ==== */

	static	firstJasoInfo Chosung[32];
	static	secondJasoInfo Midsung[32];
	static	thirdJasoInfo Jongsung[32];
	
	static	int	prepare();	
	
	static	void  print_wslist(FILE *fp=stdout);
	static	void  printCodeTab(FILE *fp=stdout);
	static	void  printWS(wsCodeInfo *wsCode, FILE *fp=stdout);
	static	uint2	ws2jh(byte c1, byte c2);
	static	uint2	ws2jh(uint2 ws_code);
	static	uint2	jh2ws(uint2 jh_code);
	
	static	void	 ws2jaso(uint2 ws_code, byte jaso3[]);
	static	uint2 jaso2jh(byte jaso3[]);

	static	 uint2 jaso2wscode(byte jaso3[]);
	static	 void jaso2string(byte *result, byte jaso3[]);
	
	static	uint2	ws_jongsung(uint2 ws_code);
	
	static	wsCodeInfo* getWsInfo(uint2 ws_code);
	static	wsCodeInfo* getWsInfo(char *str);
	static	int	Wansung::getWsInfoArray(wsCodeInfo **array, int maxarr, char *str);

	static	 void	markSyllableProperty(char *syl, uint4 prop);
	static	void	Wansung::markSyl_Irregular(byte Conj, char *syllable);
	
	static	void	Wansung::makeNobatchim(char *dest, char *org);
	static	void	Wansung::makeBatchim(char *dest, char *org, byte jongsung);

	static	int	strncmpHan(char *str1, char *str2, int len);
	
	static	void	print_JasoTab();
	static	int	splitHanSyllable(char *result, char *str);
	
};


#endif
