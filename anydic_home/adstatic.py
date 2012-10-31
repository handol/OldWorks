#!/usr/local/bin/python
# -*- coding: EUC-KR -*-

import random
import adtemplate

WLIST = [
	#"side by side",
	#"raise issue",
	"anticipate",
	"good Samaritan",
	"scrutiny",
	"crab",
	#"step by step",
	"stem cell",
	"take over",
	"get rid of",
	"crystal clear",
	"crystal",
	#"as of",
	"strike up",
	#"pay-as-you-go",
	"Java",
	"전화",
	"영웅",
	"캐스터",
	"뉴스",
	"crystal clear"
]


IMGLIST = [
	'<img src="/img/logo_hp.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo40.JPG" height=40 border=0 align="absmiddle">',
	'<img src="/img/anydict-yahoostyle2.bmp" height=30 border=0 align="absmiddle">',
	'<img src="/img/purpl2red.JPG" height=50 border=0 align="absmiddle">',
	'<img src="/img/c7.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/han3_1.JPG" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo9-70.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo12-70.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo12-70.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo12-70.jpg" height=40 border=0 align="absmiddle">',
	'<img src="/img/logo12-70.jpg" height=40 border=0 align="absmiddle">'
]



CON_KEYS = [
"과외, 학습지, 대학, 논술, 유아, 초등 하교, 중학교, 고등하교, SAT, 유학, 교복, 생일 선물 ",
"번역, 영화, DVD, 서버, 광랜,  랜카드, 초고속 인터넷 ",
"여행, 레저, 스포츠, 콘도, 팬션, 여행, 예약, 예매, 비행기, 기차",
"유아, 초등 학교, 유치원, 놀이방, 어린이집, 보육, ",
"영어, 유학, 영작, 강좌, 포토샵, 디자인, 교복 ",
"만화, 영화, 방송, 인형, 곰돌이, 디즈니, 놀이 동산, 에버랜드, 롯데월드, 서울랜드",
"노트북 중고 수리, TV, 냉장고, 공기 청정기, 디지털, DTV, DMB, 위성 방송 ",
"TV, 냉장고, 공기 청정기, 디지털, DTV, DMB, 위성 방송 ",
"MP3, 아이팟, 전자 제품, PSP, X box, 오락, 게임, 휴대용 DPA, 휴대폰, 전화기, 애니콜 " ]



#SMALL_LOGO = '<a href="/logos.html"> %s </a>' % random.choice(IMGLIST) 
SMALL_LOGO = '<a href="/"><img src="/img/han3_1.JPG" height=40 border=0 align="absmiddle"></a>'


KEYWORDS = """\
<META HTTP-EQUIV="Keywords"
CONTENT="유학,학원,번역,논술, 컴퓨터 조립, 전문 용어 사전 ">

"""


TITLE = "애니딕 생생 예문 사전: 컴퓨터, 인터넷"



ANNOUNCE = """
<DIV class="dahee">
<ul>
<li> 인터넷 상에서 수집한 살아있는 영어 예문 제공.
<li> Webster 영영 사전, WordNet 영영 및 동의어 사전 제공.
<li> 동의어, 반의어, 숙어, 어구 검색 제공. - 기존 사전에 없는 구절도 검색 가능.
<li> <font color="red"> New </font>
그림 사전 (2,300 단어) - 이미지, 그림, 상황으로 배우면 기억이 오래 오래~~ 
<li> <font color="red"> New </font>
한영, 한글 검색, 속담 사전 제공.
<li> <font color="red"> New </font>
본문 중에 단어를 더블 클릭하면 자동 검색이 가능.
<li> 생생 예문 오른편의 <font color="green" size=-1>미리보기 </font>에서 문장 전후 내용을 볼 수 있어요.
</ul>
</DIV>
"""
 
MOTO = [ "또 하나의 사전", "Dictionary for Any"]
FOOTER = """
<BR/> <BR/>
<table width=728><tr><td align="center">
<HR size=1>
<font size=-1>
<font color="black">
&copy;2002,2006 &nbsp; <i> %s </i> &nbsp; <b> Anydic, Anydict </b>
</font>
</font>
<font size=-1>
&nbsp; &nbsp;
<a class="qg" href="/comment.py">제안,소감</a>
&nbsp; &nbsp;
<a class="qg" href="/logos.html">로고 보기</a>
&nbsp; &nbsp;
<a class="qg" href="http://www.dnsever.com" target="dnsever">무료네임서버</a>
</font>
</td></tr></table>
<br/>
<font color="white">컴퓨터 조립, 판매, MP3 플래이어, PDA, PS2 </font>
&nbsp; &nbsp;
""" % (random.choice(MOTO))




AD = """
		<script type="text/javascript"><!--
		google_ad_client = "pub-3002816070890467";
		google_ad_width = 728;
		google_ad_height = 90;
		google_ad_format = "728x90_as";
		google_ad_type = "text_image";
		google_ad_channel = "";
		google_language = "ko";
		/*
		google_color_border = "336699";
		*/
		google_color_border = "FFFFFF";
		google_color_bg = "FFFFFF";
		google_color_link = "0000FF";
		google_color_text = "000000";
		google_color_url = "008000";
		//-->
		</script>
		<script type="text/javascript"
		  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
		  </script>
		  """

def adFooter():
	if random.randint(1,8)==1:
		adPromote()

	#adtemplate.do_center( FOOTER )
	print FOOTER 

def adAnnounce():
	print """
	<!-- google_ad_section_start -->
	<table><tr><td align='left'><font size=-1> %s </font></td> 
	</tr> <tr> <br/> <td align="center"> &nbsp; %s </td></tr></table>
	<!-- google_ad_section_end -->
	""" % (ANNOUNCE, AD)
	#print ANNOUNCE
	pass


def adRecent():
	print """<table><tr><td align="left" width=73%%>"""
	adAnnounce()

	print """</td></tr></table> """


def adPromote():
	print """<BR/><table width=728><tr><td align="center"><font size=-1>위의 광고 중에 하나를 1주일에 한번 정도만 클릭해 주시면 서비스 운영에 도움이 됩니다 ^^ (자주 클릭하면 효과 없습니다) </td></tr></table>""" 


def randWord():
	return random.choice(WLIST)

def randKeywords(word):
	if len(word) > 0:
		r = ord(word[0])
		r %= len(CON_KEYS)
		keywords = CON_KEYS[r]
	else:
		keywords = CON_KEYS[0]
	return keywords


if __name__ == "__main__":
	print 'Content-Type: text/html\n'
	
	
