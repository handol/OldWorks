
#include "Wansung.h"

#include <stdio.h>
#include <string.h>

/**
2005.7.21
@todo
������, ������ ���� ���̺�

���� ����, ������ ���տ� ���� �ڵ� ��ȯ ���̺�

�����ڵ��� �ڼ� ���� �������� �Ͽ��� �� ���̴�.

����-to-�ϼ� �Լ��� �ʿ��Ѱ�...
*/

/**
*/
wsCodeInfo *Wansung::wsCodeTab = 0;
uint2 *Wansung::Jh2Ws_table = 0;
wsCodeInfo Wansung::unknownCode =  {
	0, 0, {0,0,0}
};

#include "HanCodeTab.h"

/**
ws2jh_byte(�ϼ��� �ѱ��� ù��° ����Ʈ, �ι�° ����Ʈ)
 
�ϼ��� �ѱ� �� ���ڸ� �Ѱܹ޾� ������ �ѱ��ڵ带 return.
 */

uint2  Wansung::ws2jh(byte c1, byte c2) {
	uint2 ws_code;
	uint2 jh_code;

	ws_code = (uint2) (c1 << 8) | (uint2) c2;
	
	if (IN_RANGE(ws_code, 0xB0A1, 0xC8FE)) {
		jh_code = Ws2Jh_table[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IN_RANGE(ws_code, 0xA4A1, 0xA4D3)) {
		jh_code = Ws2Jh_Jamo_tab[c2-0xa1];
	}
	else
		jh_code = 0x2020;
	
	return jh_code;
}

/**
ws2jh_uint2(�ϼ��� �ѱ� �ڵ�)
 
�ϼ��� �ѱ� �� ���ڸ� �Ѱܹ޾� ������ �ѱ��ڵ带 return.
 */
uint2  Wansung::ws2jh(uint2 ws_code) {
	byte	c1, c2;
	uint2 jh_code;
	
	c1 = ws_code >> 8;
	c2 = ws_code & 0xFF;
	
	if (IN_RANGE(ws_code, 0xB0A1, 0xC8FE)) {
		jh_code = Ws2Jh_table[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IN_RANGE(ws_code, 0xA4A1, 0xA4D3)) {
		 jh_code = Ws2Jh_Jamo_tab[(c2-0xa1)];
	}
	else jh_code = 0x2020;
	
	return jh_code;
}



void  Wansung::jh2jaso(uint2 jh_code, byte jaso3[])
{
	jaso3[0] = (jh_code & 0x7C00) >> 10;
	jaso3[1] = (jh_code & 0x03E0) >> 5;
	jaso3[2] = (jh_code & 0x001F);

	if ( jaso2jh(jaso3) != jh_code) {
		printf("jaso2jh() wrong!%X -  %X %X %X\n", jh_code, jaso3[0], jaso3[1], jaso3[2]);
	}
}

 inline void	 Wansung::ws2jaso(uint2 ws_code, byte jaso3[])
{
	jh2jaso ( ws2jh(ws_code), jaso3 );
}

/**
jaso 3 �� (��, ��, ����) �� ���� �ϳ��� ���� �ڵ�� �����.
�һ��� bit �� 1�� �����ؾ� �Ѵ�.
*/
uint2 Wansung::jaso2jh(byte jaso3[])
{
	return (uint2) ((uint2) ((byte)jaso3[0] << 10) | (uint2)((byte)jaso3[1] << 5) | (uint2)((byte)jaso3[2]) |(uint2)0x8000);
}

uint2  Wansung::jh2ws(uint2 jh_code) {
	
	if (IS_JHCODE(jh_code)) {
		return Jh2Ws_table[JH_Code2Idx(jh_code)];
	}
	else
		return 0;
}

 inline uint2 Wansung::jaso2wscode(byte jaso3[])
{
	return jh2ws ( jaso2jh(jaso3) );
}

 inline void Wansung::jaso2string(byte *result, byte jaso3[])
{
	uint2 wscode =  jh2ws ( jaso2jh(jaso3) );
	result[0] = (byte) (wscode >> 8);
	result[1] = (byte) (wscode & 0xFF);
}

wsCodeInfo* Wansung::getWsInfo(uint2 ws_code)
{
	byte c1, c2;
	c1 = ws_code >> 8;
	c2 = ws_code & 0xFF;
	if (IS_WS_UMJUL(ws_code)) {
		return &wsCodeTab[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IS_WS_JAMO(ws_code)) {
		return &wsCodeTab[WS_JAMO_TO_IDX(ws_code)];
	}
	else return 0;
}

wsCodeInfo* Wansung::getWsInfo(char *str)
{
	byte c1 = (byte)str[0];
	byte c2 = (byte)str[1];
	uint2 ws_code = (uint2) (c1 << 8) | (uint2) c2;
	if (IS_WS_UMJUL(ws_code)) {
		return &wsCodeTab[WS_UMJUL_TO_IDX(c1, c2)];
	}
	else if (IS_WS_JAMO(ws_code)) {
		return &wsCodeTab[WS_JAMO_TO_IDX(ws_code)];
	}
	else return 0;	
}

/**
*/
int	Wansung::getWsInfoArray(wsCodeInfo **array, int maxarr, char *str)
{
	int	i;
	wsCodeInfo *info;
	for(i=0; i<maxarr; i++, str+=2) {
		info = getWsInfo(str);
		if (info==0) break;

		array[i] = info;
	}
	return i;
}

/**
�־��� �� ��������  ��ħ���� ������ �����.
*/
void	Wansung::makeNobatchim(char *dest, char *org)
{
	wsCodeInfo *info = Wansung::getWsInfo(org);
	if (info) {
		dest[0] = info->nobatchim >> 8;
		dest[1] = info->nobatchim & 0xFF;
	}
	else
		dest[0] = 0;
		
}

/**
�־��� �� �������� , �־��� ��ħ�� �߰���  ������ �����.
*/
void	Wansung::makeBatchim(char *dest, char *org, byte jongsung)
{
	wsCodeInfo *info = Wansung::getWsInfo(org);
	if (info) {
		byte	jaso3[3];

		memcpy(jaso3, info->jaso, 3);
		jaso3[2] = jongsung;
		jaso2string((byte*)dest, jaso3);
	}
	else
		dest[0] = 0;
		
}



/**
�ϼ��� �ڵ�ǥ (���� ���̺� ) �� ����, ��� �� ������ ������ ǥ�ø� �Ѵ�.
*/
 void	Wansung::markSyllableProperty(char *syl, uint4 prop)
{
	wsCodeInfo *info = getWsInfo(syl);
	if (info) info->property |= prop;
}

/**
�ϼ��� �ڵ�ǥ (���� ���̺� ) �� �ұ�Ģ Ȱ������ ����� ���� ������ ǥ�ø� �Ѵ�.


�׿� �ұ�Ģ : ��� ������ ������ ��ȭ �Ѵ�.  ~~��. 

�� �ұ�Ģ : ��� ������ ���� �պκ��� ��ȭ�Ѵ�.  ~~����. 
������   -- ���� 

�Ŷ�, �ʶ� �� ��� ������ �ʴ´�. 
*/
void	Wansung::markSyl_Irregular(byte Conj, char *syllable)
{
	uint4 prop = 0;
	byte	jongsung= 0;
	
	switch (Conj) {
		case D_conj: prop = P_D_IRRE;  jongsung = JONG_L; // �� --> ��
			break;
			
		case B_conj: prop = P_B_IRRE;	jongsung = JONG_NOT; // �� Ż�� 
			break;
			
		case S_conj: prop = P_S_IRRE; jongsung = JONG_NOT; // �� Ż�� 
			break;
			
		case H_conj: prop = P_H_IRRE; jongsung = JONG_NOT; // �� Ż�� 
			break;
			
		case Reu_conj: prop = P_REU_IRRE; jongsung = JONG_L; // �� �߰� 
			break;
			
		case Go_conj: prop = P_GO_IRRE; break;
		case Come_conj: prop = P_COME_IRRE; break;
	}
	
	if (prop) {
		wsCodeInfo *info = Wansung::getWsInfo(syllable);

		if (info==0) {
			printf("Conj=%d syl=%s\n", Conj, syllable);
		}
		if (jongsung && info) {	
			/* ���� ���� ��ȭ�� �� �ϼ��� ���� ���� , �ű⿡ ǥ�� �Ѵ�. */
			byte	newjaso[3];

			memcpy(newjaso, info->jaso, 3);
			newjaso[2] = jongsung;
			
			uint2 wscode_tomark =  jh2ws ( jaso2jh( newjaso ) );
			info = Wansung::getWsInfo(wscode_tomark);
			if (info) info->property |= prop;
		}
		else {
			if (info) info->property |= prop;
		}
	}
	//Wansung::markSyllableProperty(syllable, prop);
}

/**
������� Ȯ���ϴ� ��.

5. ����� ����

    �Ʒ��� ���� �� a)�� ������ �������� ó���Ͽ���
    a) ����� : �� <--> �� + ��        ��) ��+���� <==> �Ͽ��� <==> �ߴ�
    b) �ܾ�� : �Ǥ� <--> �� + �� ??
                �Ǥ� <--> �Ǥ� + ��    ��) ��+���� <==> �Ǿ��� <==> �ƴ�
    c) ���� : �̤� <--> �� + ��      ��) ��+���� <==> �־��� <==> ���
    d) �;�� : �Ǥ� <--> �� + ��      ��) ��+�Ҵ� <==> ���Ҵ� <==> �Դ�
    e) �̾�� : �� <--> �� + ��        ��) ��+���� <==> ������ <==> ����
    f) �ƾ�� : �� <--> �� + ��        ��) ��+�Ҵ� <==> ���Ҵ� <==> ����
*/
int	Wansung::splitHanSyllable(char *result, char *str )
{
	wsCodeInfo *codeInfo = getWsInfo(str);
	*result = 0;
	
	if (codeInfo==0) {		
		return 0;
	}
	printWS(codeInfo);
	
	int idx = codeInfo->jaso[1];
	if ( Midsung[idx].org1 && Midsung[idx].org2 ) {
		byte	newjaso[3];
		
		memcpy(newjaso, codeInfo->jaso, 3);		
		newjaso[1] = Midsung[idx].org1;
		newjaso[2] = JONG_NOT;
		jaso2string((byte*)result, newjaso);

		memcpy(newjaso, codeInfo->jaso, 3);
		newjaso[0] = CHO_NG;
		newjaso[1] = Midsung[idx].org2;
		jaso2string((byte*)result+2, newjaso);
		
		result[4] = 0;
		return 1;
	}
	else {
		return 0;
	}
}


/**
�ϼ��� ���� (�� ���� ���� )�� ������ �ش��ϴ� �ڸ��� �ϼ��� �ڵ� �� ���ϱ�.
@input ws_code �ϼ��� ���� 
@return ������ �ش��ϴ� �ڸ��� �ϼ��� �ڵ� �� 

STR_TO_UINT2() MACRO ���� string�� (byte)�� ����ȯ ���־�� ��Ȯ�� ���� ���´�.

2005.7.22 ����.
*/
uint2 Wansung::ws_jongsung(uint2 ws_code)
{
	byte c1, c2;
	c1 = ws_code >> 8;
	c2 = ws_code & 0xFF;

	//int	jongsung_val = wsCodeTab[WS_UMJUL_TO_IDX(c1, c2)].jaso[2];
	int	jongsung_val = getWsInfo(ws_code)->jaso[2];
	if (HanLastSound[jongsung_val][0]==0) return 0x2020; // ���� ����.
	
	uint2 jongsung_wscode = STR_TO_UINT2(HanLastSound[jongsung_val]);
	return jongsung_wscode;
}

/**
��ħ�� ���� ���ڴ�  ������ �������� 1 �̴�.
*/
uint2 Wansung::findNobatchim_wscode(int curr_idx)
{
	byte *curr_jaso = wsCodeTab[curr_idx].jaso;
	byte	*jaso;
	for	(int	i=curr_idx-1; i>=0; i--) {
		jaso = wsCodeTab[i].jaso;
		if (jaso[2]==1 && jaso[1]==curr_jaso[1] && jaso[1]==curr_jaso[1])
			return wsCodeTab[i].wscode;
	}
	return 0;
}

int	Wansung::alloc()
{
	if (wsCodeTab) return 0;
	
	wsCodeTab =  new wsCodeInfo [NUM_WS_UMJUL + NUM_WS_JAMO];
	if (wsCodeTab==0) return -1;

	memset(wsCodeTab, 0, sizeof(wsCodeInfo) * (NUM_WS_UMJUL + NUM_WS_JAMO) );

	Jh2Ws_table = new uint2 [JH2WS_TABSIZE];
	memset(Jh2Ws_table, 0, sizeof(uint2) * JH2WS_TABSIZE );
	return 0;
}

/**
wsCodeTab �� �Ҵ��ϰ�, ������ ä��� �Լ�.

�ϼ��� �ѱ� �ڵ带 ���¼� �м��ϱ� ���ؼ��� �ڼ� �и�, ��ħ���� ���� ã�� ���� 
����� �ʿ��ϴ�.
�̸� ���� �߰��ܰ�� ������ �ڵ带 �̿��Ѵ�.
��, �ϼ��� �ڵ带 ������ �ڵ�� mapping�� ���̺��� �������� �ڼ� �и��� 
�����ϰ�,
�ڼ� �и��� �������� ��ħ���� ���� ã�⸦ �����Ѵ�.

printCodeTab() �� �����Ͽ� wsCodeTab ä��Ⱑ ����� �Ǿ����� Ȯ���� �� �ִ�.
*/
int	Wansung::fillTables()
{
	int	n=0;
	byte c1, c2;
	uint2	ws_code;
	uint2	jh_code;
	byte		*jaso3;
	int	cnt_nobatchim = 0;
	for (c1 = 0xB0; c1 <= 0xC8; c1++) {
		for (c2 = 0xA1; c2 <= 0xFE; c2++, n++) {
			ws_code = (c1<<8 | c2);
			wsCodeTab[n].wscode = ws_code;

			/* �ڼ� �и� : �ʼ�, �߼�, ���� �� ���ϱ� */
			jaso3 = wsCodeTab[n].jaso;
			jh_code = ws2jh(ws_code);
			jh2jaso ( jh_code, jaso3 );

			wsCodeTab[n].chosungWs = STR_TO_UINT2( Chosung[jaso3[0]].draw);
			wsCodeTab[n].jongsungWs = STR_TO_UINT2( Jongsung[jaso3[2]].draw);
			
			if (IS_JHCODE(jh_code)) {
				/* ������ �ڵ� --> �ϼ��� �ڵ� ���� ���̺� ä��� */
				Jh2Ws_table[JH_Code2Idx(jh_code)] = ws_code;
			}

			/* ��ħ���� �ϼ��� �ڵ尪 ���ϱ� */
			if (jaso3[2]==1) {				
				wsCodeTab[n].nobatchim = ws_code;
				cnt_nobatchim++;
			}
			else {
				wsCodeTab[n].nobatchim = findNobatchim_wscode(n);
				if (wsCodeTab[n].nobatchim == 0) {
					printf("error: nobatchim NOT found for WS code %X (%c%c)\n", 
						ws_code, c1, c2);
				}
			}
		}
	}

	for(n=NUM_WS_UMJUL, ws_code = 0xA4A1; ws_code <= 0xA4D3; ws_code++, n++) {
		wsCodeTab[n].wscode = ws_code;
		wsCodeTab[n].nobatchim = 0;
		/* �ڼ� �и� : �ʼ�, �߼�, ���� �� ���ϱ� */
		jaso3 = wsCodeTab[n].jaso;
		jh_code = ws2jh(ws_code);
		jh2jaso ( jh_code, jaso3 );

		wsCodeTab[n].chosungWs = STR_TO_UINT2( Chosung[jaso3[0]].draw);
		wsCodeTab[n].jongsungWs = STR_TO_UINT2( Jongsung[jaso3[2]].draw);
	}
	
	// ���� ��� cnt_nobatchim �� 349 ���� ���´�.  �ѱ� ��ü������ ������ ������ 395 ����.
	//printf("NO batchim: %d\n", cnt_nobatchim);
	return 0;
}

void  Wansung::printWS(wsCodeInfo *wsCode, FILE *fp)
{
	fprintf(fp, 
		"%X %c%c %2s %2s %2s  %2d %2d %2d %c%c %X %X %X\n", 
		
		wsCode->wscode, 

		wsCode->wscode >> 8, 
		wsCode->wscode & 0xFF,

		Chosung[wsCode->jaso[0]].draw,
		Midsung[wsCode->jaso[1]].draw,
		Jongsung[wsCode->jaso[2]].draw,

		wsCode->jaso[0],
		wsCode->jaso[1],
		wsCode->jaso[2],
		
		wsCode->nobatchim >> 8,
		wsCode->nobatchim & 0xFF,
		wsCode->property,
		wsCode->chosungWs,
		wsCode->jongsungWs
	);
}

// KS �ϼ��� 2,350 ��
// �ϼ��� �ڵ带 ���������� ��ȯ�� �ڼҸ� ���� �� �ִ�.
void  Wansung::print_wslist(FILE *fp)
{
	byte c1, c2;
	uint2	ws_code;
	byte	jaso[3];
	
	for (c1 = 0xB0; c1 <= 0xC8; c1++) {
		for (c2 = 0xA1; c2 <= 0xFE; c2++) {
			ws_code = (c1<<8 | c2);	

			ws2jaso(ws_code, jaso);
			fprintf(fp, "%X  %c%c %s %s %s\n", ws_code, c1, c2, 
					HanFirstSound[jaso[0]],
					HanMiddleSound[jaso[1]],
					HanLastSound[jaso[2]]
			);
		}
	}
}

/**
wsCodeTab �� ����� �Ǿ����� �˻�.

=== ���� ��� ===
...
A4 B5 == A4B5
C8FD  ��  �� �� �� �� ��
A4 B7 == A4B7
C8FE  ��  �� �� �� �� ��
*/

void  Wansung::printCodeTab(FILE *fp)
{
	byte c1, c2;
	wsCodeInfo *wsCode;
	uint2	jongsung;
	byte	mu_jong[4];
	
	for (c1 = 0xB0; c1 <= 0xC8; c1++) {
		for (c2 = 0xA1; c2 <= 0xFE; c2++) {
			wsCode = & wsCodeTab[ WS_UMJUL_TO_IDX(c1, c2) ];

			if (wsCode->wscode != BYTE_TO_UINT2(c1, c2)) {
				fprintf(fp, "wsCodeTab is wrong !\n");
				break;
			}
			jongsung = ws_jongsung(wsCode->wscode);	

			memcpy(mu_jong, wsCode->jaso, 3);
			mu_jong[2] = 1;
			int jh_code = jaso2jh(mu_jong);
			int nobatchim = jh2ws(jh_code);

			//if (wsCode->property)
			fprintf(fp, 
					//"%X %c%c %2s %2s %2s  %2d %2d %2d %c%c  %c%c%X\n", 
					"%X %X %c%c %2s %2s %2s  %2d %2d %2d %c%c %X %08X\n", 
					
					wsCode->wscode, 
					jh_code,
					wsCode->wscode >> 8, 
					wsCode->wscode & 0xFF,

					Chosung[wsCode->jaso[0]].draw,
					Midsung[wsCode->jaso[1]].draw,
					Jongsung[wsCode->jaso[2]].draw,

					wsCode->jaso[0],
					wsCode->jaso[1],
					wsCode->jaso[2],
					
					wsCode->nobatchim >> 8,
					wsCode->nobatchim & 0xFF,
					nobatchim,
					wsCode->property
					/*
					,									
					jongsung >> 8, 
					jongsung & 0xFF,					
					*/
			);
		}
	}

	uint2 ws_code;
	for(ws_code = 0xA4A1; ws_code <= 0xA4D3; ws_code++) {
			wsCode = & wsCodeTab[ WS_JAMO_TO_IDX(ws_code) ];

			if (wsCode->wscode != ws_code) {
				fprintf(fp, "wsCodeTab is wrong !\n");
				break;
			}
			jongsung = ws_jongsung(wsCode->wscode);	

			memcpy(mu_jong, wsCode->jaso, 3);
			mu_jong[2] = 1;
			int jh_code = jaso2jh(mu_jong);
			int nobatchim = jh2ws(jh_code);

			//if (wsCode->property)
			fprintf(fp, 
					//"%X %c%c %2s %2s %2s  %2d %2d %2d %c%c  %c%c%X\n", 
					"%X %X %c%c %2s %2s %2s  %2d %2d %2d %X %08X\n", 
					
					wsCode->wscode, 
					jh_code,
					wsCode->wscode >> 8, 
					wsCode->wscode & 0xFF,

					Chosung[wsCode->jaso[0]].draw,
					Midsung[wsCode->jaso[1]].draw,
					Jongsung[wsCode->jaso[2]].draw,

					wsCode->jaso[0],
					wsCode->jaso[1],
					wsCode->jaso[2],
					
					nobatchim,
					wsCode->property
					/*
					,									
					jongsung >> 8, 
					jongsung & 0xFF,					
					*/
			);
		
	}
}

/**
�ѱ����� string ���� ��.

���� ���ε�   -- ����
'�ε�' �� '����' �� ��.
str2 ���� '��', '��' ���� �ڸ� ������ ���� �� �ִ�.

st1 �� ���� ����.
str2 �� ���� ���¼�
*/
int	Wansung::strncmpHan(char *str1, char *str2, int len)
{
	int	n=0;
	while(*str1 && n < len) {
		if (str1[0] != str2[0] || str1[1] != str2[1]) {			
			wsCodeInfo *info1 = getWsInfo(str1);
			int	ws2 = STR_TO_UINT2(str2);
			
			if (info1 && info1->jongsungWs == ws2 )
			{
				return 0;
			}
			else {
				int	ws1 = STR_TO_UINT2(str1);
				return (int) (ws1 - ws2);
			}
		}
		n += 2;
		str1 += 2;
		str2 += 2;
	}
	return 0;
}

int	Wansung::prepare()
{
	int	res=0;
	alloc();
	res = fillTables();
	return res;
}

#ifdef TEST_MAIN
int	main()
{
	Wansung::prepare();
	//Wansung::printCodeTab();

	char	result[10];

	
	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "��");	
	printf("%s\n", result);
	return 0;
}
#endif
