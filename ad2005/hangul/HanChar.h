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
��� ����:
WS -- HanChar. �ϼ���.
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


/*=== �ϼ��ڵ� ���̺�, �����ڵ� ���̺� index �� ���õ� macro ===*/

/* �ϼ��� �ڵ� ���� ǥ */
#define NUM_WS_UMJUL        2350 // 25 *94 ==  2350 // ��(0xB0A1) ~ ��(0xC8FE)
#define NUM_WS_JAMO        51  // �ڸ� �ڵ� =  0xA4A1 ~ 0xA4D3

/*  0xB0A1~0xC8FE : 6,238 ������ . 2350 �ڸ� ���� ��� */
#define	IS_WS_UMJUL(X)	( 0xB0A1 <= (X) && (X) <= 0xC8FE)
#define	IS_WS_JAMO(X)	( 0xA4A1 <= (X) && (X) <= 0xA4D3)
#define	UINT2_TO_BYTE(WS, HI, LO)	{ HI = (WS)>>8; LO = (WS) & 0xFF; }

#define	BYTE_TO_UINT2(HI, LO)	(uint2)( (byte)(HI)<<8 | (byte) (LO) )
#define	STR_TO_UINT2(STR)	(uint2)( (byte)(STR[0])<<8 | (byte) (STR[1]) )

#define	WS_UMJUL_TO_IDX(HI, LO)	( ((HI) - 0xb0) * 94 + ((LO) - 0xa1))
#define	WS_JAMO_TO_IDX(WS)	((int)((WS)-0xA4A1) + NUM_WS_UMJUL)

/* ���� �ڵ� ǥ */
#define	IS_JHCODE(X)	( 0x8444 <= (X) && (X) <= 0xd3b7)
#define	JH2WS_TABSIZE	(((int)0xd3b7 - (int)0x8444) + 1) //== 19,287 + 64
#define	JH_Code2Idx(X)	((int)(X) - (int)0x8444)


/*=== ���� �ڵ� , �ڼ� �˻� ���� macro ===*/
#define	JH_FstSnd(J)	(byte)(((J) & 0x7C00) >> 10)	// �����ڵ��� �ʼ� 5bit ��
#define	JH_MidSnd(J)	(byte)(((J) & 0x03E0) >> 5)	// �����ڵ��� �߼� 5bit ��
#define	JH_LastSnd(J)	(byte)((J) & 0x001F) // �����ڵ��� ���� 5bit ��

#define	JH_replaceFstSnd(J, NewSnd)	(uint2)(( (J) & (~0x7C00)) | ((uint2)NewSnd << 10))
#define	JH_replaceMidSnd(J, NewSnd)	(uint2)(( (J) & (~0x03E0)) | ((uint2)NewSnd << 5))
#define	JH_replaceLastSnd(J, NewSnd)	(uint2)(( (J) & (~0x001F)) | ((uint2)NewSnd))

/*=== �ڼ� �� Ư�� ===*/
#define	FstSnd_Nothing	(13)  // �ڼ� ���� ��
#define	FstSnd_N	(4)
#define	FstSnd_B		(9)
#define	FstSnd_N	(4)

#define	MidSnd_A	(3) // ��
#define	MidSnd_EO	(7) // ��
#define	MidSnd_EU	(27) // ��
#define	MidSnd_AE	(4) // ��
#define	MidSnd_YAE	(6) // ��
#define	MidSnd_YA	(5) // ��
#define	MidSnd_YEO	(11) // ��
#define	LastSnd_Nothing	(1) // ��ħ ���� ��
#define	LastSnd_N	(5)
#define	LastSnd_D	(8)
#define	LastSnd_L	(9)
#define	LastSnd_M	(17)
#define	LastSnd_B	(19)
#define	LastSnd_S	(21)
#define	LastSnd_SS	(22) // �ڼ� �� �ÿ� ��
#define	LastSnd_H	(29)

/*
J : Johap Code
JASO: �ڸ� �и��� �ʼ�, �߼�, ���� ���� �ڸ� ��.

��ħ�� ���� ���ڴ�  ������ �������� 1 �̴�.
�ʼ���  ���� ���ڴ�  ������ �ʼ����� 13 (�� �� �ش�) �̴�.
*/
#define	Has_NO_FstSnd(J)	( JH_FstSnd(J)==FstSnd_Nothing)
#define	Has_FstSnd(J)	( JH_FstSnd(J)!=FstSnd_Nothing)
#define	Has_NO_LastSnd(J)	(JH_LastSnd(J)==LastSnd_Nothing)
#define	Has_LastSnd(J)	(JH_LastSnd(J)!=LastSnd_Nothing)


#define	Is_Josa_LastSnd(JASO)	( HanChar::HLastSnd[(JASO)].property & S_JOSA)
#define	Is_Eomi_LastSnd(JASO)	( HanChar::HLastSnd[(JASO)].property & S_EOMI)
#define	Is_Posi_MidSnd(JASO)	( (HanChar::HMidSnd[(JASO)].property) & S_POSITIVE)


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

/* SylInfo *X  */
#define	IS_SYL_D_IRRE(X) ((X)->property & P_D_IRRE)
#define	IS_SYL_B_IRRE(X) ((X)->property & P_B_IRRE)
#define	IS_SYL_S_IRRE(X) ((X)->property & P_S_IRRE)
#define	IS_SYL_H_IRRE(X) ((X)->property & P_H_IRRE)
#define	IS_SYL_REU_IRRE(X) ((X)->property & P_REU_IRRE)
#define	IS_SYL_GO_IRRE(X) ((X)->property & P_GO_IRRE)
#define	IS_SYL_COME_IRRE(X) ((X)->property & P_COME_IRRE)

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

/*
���� �ڵ� �� �ڸ� �ڵ� ��� ���̺� ����.
�� 2350 �� = ����
�� 51�� =  �ڸ� �ڵ� =  0xA4A1 ~ 0xA4D3
*/

/*
���� Ư�� (syllable).
������ �ڵ带 ���� �ѱ� �ڵ�� �����. �ڼ� �и�, �ڼ� ��ȯ ���� ������ ����Ƿ�.
������ �ڵ� ===> �ϼ��� �ڵ� ���� ���̺� ���� ����.
*/
typedef struct _JHSyllableInfo {
	uint2	jhcode;  // ���� �ڵ�
	uint2	wscode; /* ������ �ڵ� ===> �ϼ��� �ڵ� ���� ���̺� */
	uint4	property; // ���� Ư�� === syllable property
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
