
#include "Wansung.h"

#include <stdio.h>
#include <string.h>

/**
2005.7.21
@todo
政失製, 巷失製 姥歳 砺戚鷺

戚掻 乞製, 差乞製 衣杯拭 税廃 坪球 痕発 砺戚鷺

繕杯坪球税 切社 葵聖 奄層生稽 馬食醤 拝 依戚陥.

繕杯-to-刃失 敗呪亜 琶推廃亜...
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
ws2jh_byte(刃失莫 廃越税 湛腰属 郊戚闘, 砧腰属 郊戚闘)
 
刃失莫 廃越 廃 越切研 角移閤焼 繕杯莫 廃越坪球研 return.
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
ws2jh_uint2(刃失莫 廃越 坪球)
 
刃失莫 廃越 廃 越切研 角移閤焼 繕杯莫 廃越坪球研 return.
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
jaso 3 鯵 (段, 掻, 曽失) 聖 杯団 馬蟹税 繕杯 坪球稽 幻窮陥.
智雌是 bit 澗 1稽 室特背醤 廃陥.
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
爽嬢遭 廃 製箭拭辞  閤徴蒸澗 製箭聖 幻窮陥.
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
爽嬢遭 廃 製箭拭辞 , 爽嬢遭 閤徴聖 蓄亜廃  製箭聖 幻窮陥.
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
刃失莫 坪球妊 (製箭 砺戚鷺 ) 拭 繕紫, 嬢耕 亜 亜管廃 製箭拭 妊獣研 廃陥.
*/
 void	Wansung::markSyllableProperty(char *syl, uint4 prop)
{
	wsCodeInfo *info = getWsInfo(syl);
	if (info) info->property |= prop;
}

/**
刃失莫 坪球妊 (製箭 砺戚鷺 ) 拭 災鋭帳 醗遂生稽 幻級嬢 走澗 製箭拭 妊獣研 廃陥.


益須 災鋭帳 : 嬢娃税 原走厳 製箭戚 痕鉢 廃陥.  ~~陥. 

牽 災鋭帳 : 嬢娃税 原走厳 製箭 蒋採歳戚 痕鉢廃陥.  ~~牽陥. 
匙牽陥   -- 察虞 

暗虞, 格虞 澗 嬢娃戚 痕馬走 省澗陥. 
*/
void	Wansung::markSyl_Irregular(byte Conj, char *syllable)
{
	uint4 prop = 0;
	byte	jongsung= 0;
	
	switch (Conj) {
		case D_conj: prop = P_D_IRRE;  jongsung = JONG_L; // ぇ --> ぉ
			break;
			
		case B_conj: prop = P_B_IRRE;	jongsung = JONG_NOT; // げ 纏喰 
			break;
			
		case S_conj: prop = P_S_IRRE; jongsung = JONG_NOT; // さ 纏喰 
			break;
			
		case H_conj: prop = P_H_IRRE; jongsung = JONG_NOT; // ぞ 纏喰 
			break;
			
		case Reu_conj: prop = P_REU_IRRE; jongsung = JONG_L; // ぉ 蓄亜 
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
			/* 曽失 葵戚 痕鉢吉 歯 刃失莫 葵聖 姥背 , 暗奄拭 妊獣 廃陥. */
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
逐鉦莫聖 溌舌馬澗 依.

5. 逐鉦税 曽嫌

    焼掘税 曽嫌 掻 a)研 薦須廃 蟹袴走澗 坦軒馬心製
    a) 食鉦嬢 : だ <--> た + 食        森) 馬+醸陥 <==> 馬心陥 <==> 梅陥
    b) 須鉦嬢 : でび <--> で + び ??
                でだ <--> でび + 嬢    森) 鞠+醸陥 <==> 鞠醸陥 <==> 菊陥
    c) 酔鉦嬢 : ぬっ <--> ぬ + 嬢      森) 爽+醸陥 <==> 爽醸陥 <==> 早陥
    d) 人鉦嬢 : でた <--> で + 焼      森) 神+紹陥 <==> 神紹陥 <==> 尽陥
    e) 戚鉦嬢 : づ <--> び + 嬢        森) 走+醸陥 <==> 走醸陥 <==> 然陥
    f) 焼鉦嬢 : た <--> た + 焼        森) 亜+紹陥 <==> 亜紹陥 <==> 穐陥
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
刃失莫 庚切 (廃 製箭 庚切 )税 曽失拭 背雁馬澗 切乞税 刃失莫 坪球 葵 姥馬奄.
@input ws_code 刃失莫 庚切 
@return 曽失拭 背雁馬澗 切乞税 刃失莫 坪球 葵 

STR_TO_UINT2() MACRO 拭辞 string聖 (byte)稽 莫痕発 背爽嬢醤 舛溌廃 葵戚 蟹紳陥.

2005.7.22 失因.
*/
uint2 Wansung::ws_jongsung(uint2 ws_code)
{
	byte c1, c2;
	c1 = ws_code >> 8;
	c2 = ws_code & 0xFF;

	//int	jongsung_val = wsCodeTab[WS_UMJUL_TO_IDX(c1, c2)].jaso[2];
	int	jongsung_val = getWsInfo(ws_code)->jaso[2];
	if (HanLastSound[jongsung_val][0]==0) return 0x2020; // 因拷 庚切.
	
	uint2 jongsung_wscode = STR_TO_UINT2(HanLastSound[jongsung_val]);
	return jongsung_wscode;
}

/**
閤徴戚 蒸澗 庚切澗  繕杯莫 曽失葵戚 1 戚陥.
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
wsCodeTab 聖 拝雁馬壱, 鎧遂聖 辰酔澗 敗呪.

刃失莫 廃越 坪球研 莫殿社 歳汐馬奄 是背辞澗 切社 歳軒, 閤徴蒸澗 越切 達奄 去税 
奄管戚 琶推馬陥.
戚研 是背 掻娃舘域稽 繕杯莫 坪球研 戚遂廃陥.
聡, 刃失莫 坪球研 繕杯莫 坪球稽 mapping廃 砺戚鷺聖 郊伝生稽 切社 歳軒研 
姥薄馬壱,
切社 歳軒研 郊伝生稽 閤徴蒸澗 越切 達奄研 姥薄廃陥.

printCodeTab() 聖 叔楳馬食 wsCodeTab 辰酔奄亜 薦企稽 鞠醸澗走 溌昔拝 呪 赤陥.
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

			/* 切社 歳軒 : 段失, 掻失, 曽失 葵 姥馬奄 */
			jaso3 = wsCodeTab[n].jaso;
			jh_code = ws2jh(ws_code);
			jh2jaso ( jh_code, jaso3 );

			wsCodeTab[n].chosungWs = STR_TO_UINT2( Chosung[jaso3[0]].draw);
			wsCodeTab[n].jongsungWs = STR_TO_UINT2( Jongsung[jaso3[2]].draw);
			
			if (IS_JHCODE(jh_code)) {
				/* 繕杯莫 坪球 --> 刃失莫 坪球 古芭 砺戚鷺 辰酔奄 */
				Jh2Ws_table[JH_Code2Idx(jh_code)] = ws_code;
			}

			/* 閤徴蒸澗 刃失莫 坪球葵 姥馬奄 */
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
		/* 切社 歳軒 : 段失, 掻失, 曽失 葵 姥馬奄 */
		jaso3 = wsCodeTab[n].jaso;
		jh_code = ws2jh(ws_code);
		jh2jaso ( jh_code, jaso3 );

		wsCodeTab[n].chosungWs = STR_TO_UINT2( Chosung[jaso3[0]].draw);
		wsCodeTab[n].jongsungWs = STR_TO_UINT2( Jongsung[jaso3[2]].draw);
	}
	
	// 叔楳 衣引 cnt_nobatchim 精 349 鯵亜 蟹紳陥.  廃越 穿端旋生稽 亜管廃 鯵呪澗 395 鯵陥.
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

// KS 刃失莫 2,350 切
// 刃失莫 坪球研 繕杯莫生稽 痕発板 切社研 姥拝 呪 赤陥.
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
wsCodeTab 戚 薦企稽 鞠醸澗走 伊紫.

=== 叔楳 衣引 ===
...
A4 B5 == A4B5
C8FD  眉  備 ぞ び さ さ
A4 B7 == A4B7
C8FE  美  備 ぞ び し し
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
廃越製箭 string 娃税 搾嘘.

巷充 源昔級   -- い級
'昔級' 引 'い級' 聖 搾嘘.
str2 拭幻 'い', 'ぉ' 去税 切乞 製箭聖 亜霜 呪 赤陥.

st1 精 据莫 嬢箭.
str2 澗 庚狛 莫殿社
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

	
	Wansung::splitHanSyllable(result, "畏");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "丘");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "心");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "菊");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "梅");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "然");	
	printf("%s\n", result);

	Wansung::splitHanSyllable(result, "穐");	
	printf("%s\n", result);
	return 0;
}
#endif
