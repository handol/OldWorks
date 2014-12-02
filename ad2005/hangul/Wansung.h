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

���� ���¼� �м��� ���� ��� ����.
���½� ������ ���� �м��Ͽ� ������ ���.

wsCodeInfo.property

** �ѱ��� ���� Ư���� 3-tuple �� ǥ��
      A = ( C, L, N)    C: ǰ��, L: ���¼� ����(����) , N: ���¼ҿ��� ������ ��ġ(����)
      ����Ư�� �Լ�
         A<i,j,k>  �� ǰ�簡 i, ���¼� ���� j , ������ġ�� k  �� ������ �����̶�� �ϸ����� Ư�� �Լ�
         C_A<i,j,k> �� ������ ���� ����
         C_A<i,j,k> : X -> { 0,1}    ; X �� ������ ��ü ����
           C_A<i,j,k> (x) = 1, if x �� A<i,j,k> �� ���ϸ� 
                                =0   otherwise
*/

/* HwordType.Conj  :: HDict.h ���� �����. */
enum {
	D_conj = 1,
	L_conj,
	B_conj,
	S_conj,
	H_conj,
	Go_conj,
	Come_conj,
	Leora_conj,  // ����
	Leo_conj,  // �� 
	Reu_conj,  // ��
	Yeo_conj,  // �� 
	U_conj,  // ��
	Eu_conj  // �� 
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
#define	P_GO_IRRE	(0x01 << 9)  // �Ŷ� �ұ�Ģ - ���Ŷ�  go
#define	P_COME_IRRE	(0x01 << 10) // �ʶ� �ұ�Ģ - ���ʶ�  come

/* wsCodeInfo *X  */
#define	IS_SYL_D_IRRE(X) ((X)->property & P_D_IRRE)
#define	IS_SYL_B_IRRE(X) ((X)->property & P_B_IRRE)
#define	IS_SYL_S_IRRE(X) ((X)->property & P_S_IRRE)
#define	IS_SYL_H_IRRE(X) ((X)->property & P_H_IRRE)
#define	IS_SYL_REU_IRRE(X) ((X)->property & P_REU_IRRE)
#define	IS_SYL_GO_IRRE(X) ((X)->property & P_GO_IRRE)
#define	IS_SYL_COME_IRRE(X) ((X)->property & P_COME_IRRE)

/**
�ѱ� ����Ÿ�� �ϼ����� ������ ó���ϵ��� �����Ѵ�.
�ϼ��� �ѱ� �ڵ带 ���¼� �м��ϱ� ���ؼ��� �ڼ� �и�, ��ħ���� ���� ã�� ���� 
����� �ʿ��ϴ�.
�̸� ���� �߰��ܰ�� ������ �ڵ带 �̿��Ѵ�.
��, �ϼ��� �ڵ带 ������ �ڵ�� mapping�� ���̺��� �������� �ڼ� �и��� 
�����ϰ�,
�ڼ� �и��� �������� ��ħ���� ���� ã�⸦ �����Ѵ�.

��ħ�� ���� ���ڴ�  ������ �������� 1 �̴�.
�ʼ���  ���� ���ڴ�  ������ �ʼ����� 13 (�� �� �ش�) �̴�.
*/
/**
������ �ѱۿ��� ���ϰ� �ʼ�, �߼�, ������ �ش��ϴ� ��Ʈ���� ���� ���� ����ü�� hangul {{{
*/
union hangul {
    byte code[2];
    struct {
        unsigned t:5 ;    // ����
        unsigned s:5 ;    // �߼�
        unsigned f:5 ;    // �ʼ�
        unsigned i:1 ;    // �ѿ� ����
    } han;
};

/**
��� ����:
WS -- Wansung. �ϼ���.
JH -- Johap. ������.
���� -- consonant, ���
���� -- vowel, ٽ��
*/
#define	S_JOSA	(0x100) // ������ ��, �� �� ���� �ĺ�
#define	S_EOMI	(0x200) // ������ ��, ��, ��, �� �� ��� �ĺ�

#define	S_CONSONANT	(0x20)  // ����, ����
#define	S_COMBO	(0x10)  // �� , �� ���� ���� ���� 
#define	S_HARSH	(0x08) // ��, �� ���� ��ģ �Ҹ� 
#define	S_DOUBLE	(0x04) // ���� (�� ) ���� , ���� - ���� ( �� , �� )
#define	S_VOICE	(0x02)	// ����, ����
#define	S_POSITIVE	(0x01) // �缺, ���� 
#define	S_NONE		(0x00)

#define	IS_CONSONANT(X) ((X)&S_CONSONANT)
#define	IS_DOUBLE(X) ((X)&S_DOUBLE)

/* �ϼ��� �ڵ� ���� ǥ */

#define NUM_WS_UMJUL        2350 // 25 *94 ==  2350 // ��(0xB0A1) ~ ��(0xC8FE)
#define NUM_WS_JAMO        51  // �ڸ� �ڵ� =  0xA4A1 ~ 0xA4D3

/*  0xB0A1~0xC8FE : 6,238 */
#define	IS_WS_UMJUL(X)	( 0xB0A1 <= (X) && (X) <= 0xC8FE)
#define	IS_WS_JAMO(X)	( 0xA4A1 <= (X) && (X) <= 0xA4D3)
#define	UINT2_TO_BYTE(WS, HI, LO)	{ HI = (WS)>>8; LO = (WS) & 0xFF; }

#define	BYTE_TO_UINT2(HI, LO)	(uint2)( (byte)(HI)<<8 | (byte) (LO) )
#define	STR_TO_UINT2(STR)	(uint2)( (byte)(STR[0])<<8 | (byte) (STR[1]) )

#define	WS_UMJUL_TO_IDX(HI, LO)	( ((HI) - 0xb0) * 94 + ((LO) - 0xa1))
#define	WS_JAMO_TO_IDX(WS)	((int)((WS)-0xA4A1) + NUM_WS_UMJUL)

/* wsCodeInfo �� �ڼ� �� �˻� : WsJhCodeTab.h ���� */

#define	CHO_NG	(13)  // �ڼ� ���� ��

#define	JONG_NOT	(1) // ��ħ ���� �� 
#define	JONG_N	(5)
#define	JONG_D	(8)
#define	JONG_L	(9)
#define	JONG_M	(17)
#define	JONG_B	(19)
#define	JONG_S	(21)
#define	JONG_SS		(22) // �ڼ� �� �ÿ� ��
#define	JONG_H	(29)

/* X == wsCodeInfo *ptr */
#define	MU_JONGSUNG(X)	( X->jaso[2]==JONG_NOT)
#define	MU_CHOSUNG(X)	( X->jaso[0]==CHO_NG)
#define	DBL_S_BATCHIM(X) ( X->jaso[2]==JONG_SS)  // �ֽÿ�  �� ��ħ 

#define	IS_JONG_JOSA(X)	( Wansung::Jongsung[(X)->jaso[2]].property & S_JOSA)
#define	IS_JONG_EOMI(X)	( Wansung::Jongsung[(X)->jaso[2]].property & S_EOMI)
#define	IS_MID_POSI(X)	( (Wansung::Midsung[(X)->jaso[1]].property) & S_POSITIVE)

/* ���� �ڵ� ǥ */
#define	IS_JHCODE(X)	( 0x8861 <= (X) && (X) <= 0xd3b7)
#define	JH2WS_TABSIZE	(((int)0xd3b7 - (int)0x8861) + 1) //== 19,287
#define	JH_Code2Idx(X)	((int)(X) - (int)0x8861)

typedef struct _firstJasoInfo {
	char	draw[4]; // �ϼ��� �ڵ� ���ڿ�.
	byte	org;
	byte double_snd;
	byte	harsh_snd;
	uint4	property;
} firstJasoInfo;

typedef struct _secondJasoInfo {
	char	draw[4]; // �ϼ��� �ڵ� ���ڿ�.
	/*
	���� ������ ��� org1 �� org2 �� ���� ������ �����ϴ� �⺻ ������ index ���� ����.
	�� , �� ���� ���� ������ org1 �� ����.
	*/
	byte	org1;
	byte org2;
	byte	harsh_snd;
	uint4	property;
} secondJasoInfo;

typedef struct _thirdJasoInfo {
	char	draw[4]; // �ϼ��� �ڵ� ���ڿ�.

	// �ʼ� �ڸ� �� mapping
	byte	maptocho; 
	
	/* ���� ������ ��� ���� ���� ���� �������� index ����   jong_pos �� ���� , ���� ���� �ʼ� ������ index ��
	���������� �ƴ� ��� : ���� ( ����, ���� �� �ƴ� ���� )�� ���� ���� index  ���� jong_pos ��  ����.
	*/
	byte	jong_pos; 
	byte cho_pos;
	
	uint4	property;
} thirdJasoInfo;

/*���� �ڵ� �� �ڸ� �ڵ� ��� ���̺� ����.
�� 2350 �� = ����
�� 51�� =  �ڸ� �ڵ� =  0xA4A1 ~ 0xA4D3

wsCodeInfo.nobatchim == 0 �̸� �ڸ� �ڵ�� �� �� �ִ�.
*/
typedef struct _wsCodeInfo {
	uint2	wscode; // �� ����Ÿ�� ���� Wansung code
	uint2	nobatchim; // �� �ڵ忡�� ��ħ��  ���� ��. Wansung code
	byte		jaso[3];
	uint2	chosungWs; // �ʼ� �ڸ� �ش��ϴ� �ϼ��� �ڵ尪
	uint2	jongsungWs; // ���� �ڸ� �ش��ϴ� �ϼ��� �ڵ尪
	uint4	property; // ���� Ư�� -- syllable property
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
