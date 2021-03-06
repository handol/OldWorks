
#include "HanChar.h"

#include "HanCodeTab.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
SylInfo *HanChar::Jh2WsTab = 0;

/**
ws2jh_byte(刃失莫 廃越税 湛腰属 郊戚闘, 砧腰属 郊戚闘)

刃失莫 廃越 廃 越切研 角移閤焼 繕杯莫 廃越坪球研 return.
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
ws2jh_uint2(刃失莫 廃越 坪球)

刃失莫 廃越 廃 越切研 角移閤焼 繕杯莫 廃越坪球研 return.
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
刃失莫 庚切税 段失葵 姥馬奄.
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
jaso 3 鯵 (段, 掻, 曽失) 聖 杯団 馬蟹税 繕杯 坪球稽 幻窮陥.
智雌是 bit 澗 1稽 室特背醤 廃陥.
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
爽嬢遭 廃 製箭拭辞 , 爽嬢遭 閤徴葵生稽 痕井廃 歯  製箭聖 幻窮陥.


閤徴戚 蒸澗 庚切澗  繕杯莫 曽失葵戚 1 戚陥.
段失戚  蒸澗 庚切澗  繕杯莫 段失葵戚 13 (し 拭 背雁) 戚陥.
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
inline  敗呪稽 幻級壱 粛醸走幻 inline 敗呪研 cpp 鉢析拭 舛税廃 井酔 (*.h 鎧拭 舛税廃 依戚 焼観 井酔)
陥献 cpp 鉢析拭辞 益 inline 敗呪研 凧繕 拝 呪 蒸陥. --> "unresolved external"  神嫌 降持.

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
掻失, 曽失聖 廃腰拭 郊荷奄.

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
刃失莫 坪球妊 (製箭 砺戚鷺 ) 拭 繕紫, 嬢耕 亜 亜管廃 製箭拭 妊獣研 廃陥.
*/
 void	HanChar::markSyllableProperty(char *syl, uint4 prop)
{
	SylInfo *info = getSylInfo(syl);
	if (info) info->property |= prop;
}

/**
刃失莫 坪球妊 (製箭 砺戚鷺 ) 拭 災鋭帳 醗遂生稽 幻級嬢 走澗 製箭拭 妊獣研 廃陥.


益須 災鋭帳 : 嬢娃税 原走厳 製箭戚 痕鉢 廃陥.  ~~陥.

牽 災鋭帳 : 嬢娃税 原走厳 製箭 蒋採歳戚 痕鉢廃陥.  ~~牽陥.
匙牽陥   -- 察虞

暗虞, 格虞 澗 嬢娃戚 痕馬走 省澗陥.
*/
void	HanChar::markSyl_Irregular(byte Conj, char *syllable)
{
	uint4 prop = 0;
	byte	newlastsnd= 0;

	switch (Conj) {
		case D_conj: prop = P_D_IRRE;  newlastsnd = LastSnd_L; // ぇ --> ぉ
			break;

		case B_conj: prop = P_B_IRRE;	newlastsnd = LastSnd_Nothing; // げ 纏喰
			break;

		case S_conj: prop = P_S_IRRE; newlastsnd = LastSnd_Nothing; // さ 纏喰
			break;

		case H_conj: prop = P_H_IRRE; newlastsnd = LastSnd_Nothing; // ぞ 纏喰
			break;

		case Reu_conj: prop = P_REU_IRRE; newlastsnd = LastSnd_L; // ぉ 蓄亜
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
			/* 曽失 葵戚 痕鉢吉 歯 刃失莫 葵聖 姥背 , 暗奄拭 妊獣 廃陥. */
			uint2 new_jhcode = JH_replaceLastSnd(info->jhcode, newlastsnd);
			char tmp[4];
			tmp[2] = 0;
			info = HanChar::getSylInfo(new_jhcode);
			if (info) info->property |= prop;



			if (Conj == H_conj) {
				byte midsnd = JH_MidSnd(new_jhcode);

				// ぞ 災鋭帳税 井酔: ぞ 幻 纏喰馬澗 依戚 焼艦虞 乞製亀 郊駕陥.
				// 刊係陥 --> 刊掘
				// 督屈陥 --> 督掘
				// 馬冗陥 --> 馬剰
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
Jh2WsTab 聖 拝雁馬壱, 鎧遂聖 辰酔澗 敗呪.

刃失莫 廃越 坪球研 莫殿社 歳汐馬奄 是背辞澗 切社 歳軒, 閤徴蒸澗 越切 達奄 去税
奄管戚 琶推馬陥.
戚研 是背 掻娃舘域稽 繕杯莫 坪球研 戚遂廃陥.
聡, 刃失莫 坪球研 繕杯莫 坪球稽 mapping廃 砺戚鷺聖 郊伝生稽 切社 歳軒研
姥薄馬壱,
切社 歳軒研 郊伝生稽 閤徴蒸澗 越切 達奄研 姥薄廃陥.

printCodeTab() 聖 叔楳馬食 Jh2WsTab 辰酔奄亜 薦企稽 鞠醸澗走 溌昔拝 呪 赤陥.
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
			/* 繕杯莫 坪球 --> 刃失莫 坪球 古芭 砺戚鷺 辰酔奄 */
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
				/* 繕杯莫 坪球 --> 刃失莫 坪球 古芭 砺戚鷺 辰酔奄 */
				Jh2WsTab[idx].wscode = ws_code;
				Jh2WsTab[idx].jhcode = jh_code;

			}
		}
	}



	// 叔楳 衣引 cnt_nobatchim 精 349 鯵亜 蟹紳陥.  廃越 穿端旋生稽 亜管廃 鯵呪澗 395 鯵陥.
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
Jh2WsTab 戚 薦企稽 鞠醸澗走 伊紫.
// KS 刃失莫 2,350 切

=== 叔楳 衣引 ===
...
A4 B5 == A4B5
C8FD  眉  備 ぞ び さ さ
A4 B7 == A4B7
C8FE  美  備 ぞ び し し
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
廃越製箭 string 娃税 搾嘘.

巷充 源昔級   -- い級
'昔級' 引 'い級' 聖 搾嘘.
str2 拭幻 'い', 'ぉ' 去税 切乞 製箭聖 亜霜 呪 赤陥.

st1 精 据莫 嬢箭.
str2 澗 庚狛 莫殿社
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


	HanChar::splitHanSyllable(result, "畏");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "丘");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "心");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "菊");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "梅");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "然");
	printf("%s\n", result);

	HanChar::splitHanSyllable(result, "穐");
	printf("%s\n", result);
	return 0;
}
#endif
