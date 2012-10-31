#!/usr/local/bin/python
# -*- coding: EUC-KR -*-

import random

WLIST = [
	"side by side",
	"anticipate",
	"scrutiny",
	"crab",
	"step by step",
	"stem cell",
	"take over",
	"get rid of",
	"crystal clear",
	"crystal",
	"as of",
	"strike up",
	"pay-as-you-go",
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



DEF_WORD = random.choice(WLIST)

#SMALL_LOGO = '<a href="/logos.html"> %s </a>' % random.choice(IMGLIST) 
SMALL_LOGO = '<a href="/"><img src="/img/han3_1.JPG" height=40 border=0 align="absmiddle"></a>'


KEYWORDS = """\
<META HTTP-EQUIV="Keywords"
CONTENT="유학,학원,번역,논술, 컴퓨터 조립, 전문 용어 사전 ">

"""

STYLE = """\
<style><!--
	body,td,a,p,.h{font-family:굴림,돋움,arial,sans-serif;line-height=110%;}
	.ko{font-size: 9pt;}
	.h{font-size: 20px;} .h{color:} .q{text-decoration:none; color:#0000cc;}
	.g{text-decoration:none; color:#0000cc;}
//-->
</style>
"""

SCRIPT = """\
<SCRIPT LANGUAGE="JavaScript"> 
	<!--
	function fo() { 
		document.adform.W.select(); 
		document.adform.W.focus(); 
	}
	-->
</script> 
<script src="/wclick.js" language=javascript></script>
"""

GOOGLE_AD = """\
<script type="text/javascript"><!--
google_ad_client = "pub-3002816070890467";
google_ad_width = 728;
google_ad_height = 90;
google_ad_format = "728x90_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "DFF2FD";
google_color_bg = "DFF2FD";
google_color_link = "03364C3";
google_color_url = "008000";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
"""

TITLE = "애니딕 생생 예문 사전: 컴퓨터, 인터넷"

BODY = """\
<body bgcolor=#ffffff text=#000000 link=#0000cc vlink=#0000cc alink=#ff0000 
	onLoad="fo()" ondblclick="ad_go()">
"""

LOGOPART = """\
<TABLE cellSpacing=0 cellPadding=0 width="728" border=0>
<TBODY>
	<TR> 
	<TD noWrap align="left" width=20%> 
	<a href="/logos.html">
		<IMG alt="애니딕" src="/img/logo8-60.jpg" border=0>
	</a> 
	</TD>
	<TD align="left" noWrap > &nbsp; &nbsp; 
		<FONT color=green><B>애니딕 생생 영어예문사전</B></FONT> 
	</td> 
	<td> 
<font color="white" size=-2>검색, 영어 번역, 유학, 초고속 인터넷, ADSL, 서버, 컴퓨터 조립, MP3, PDA, PS2 </font>
	</td> 
	</TR>
</TBODY>
</TABLE>
"""


AD_FORM = """\
<TABLE cellSpacing=0 cellPadding=0 width="728" bgColor=#00b4da border=0>
<TBODY>
<TR height=40>
<TD class=q vAlign=middle noWrap>
    <form name="adform" action="/anyd.py" method="get">
       <FONT color=#ffffff>&nbsp;
        영영/<font color="yellow">한영</font>/영한/예문/숙어/속담/그림
        </FONT>
        &nbsp;&nbsp;
       <input type=hidden name="F" value=3>
        <INPUT size=30 name="W"  maxlength=50 value="%s">
        <input name="" type=image height=21 alt=검색 hspace=5 src="/img/srch2.gif" width=35 align=absMiddle border=0>
 
        &nbsp;&nbsp;&nbsp;
        <A class="q" href="/help.html"> <FONT size=-1 color=#ffffff>도움말</FONT></A>
</TD>
</TR>
</TBODY></TABLE>
""" % DEF_WORD

ANNOUNCE = """
<ul>
<li> "죽은 예문은 가라" &nbsp; 인터넷 상에서 수집한 살아있는 영어 예문 제공.
<li> 영영, 동의어, 반의어, 숙어, 어구 검색 제공. - 기존 사전에 없는 구절도 검색 가능.
<li> <font color="red"> New </font>
그림 사전 (2,300 단어) - 이미지, 그림, 상황으로 배우면 기억이 오래 오래~~ 
<li> <font color="red"> New </font>
한영, 한글 검색, 속담 사전 제공.
<li> <font color="red"> New </font>
본문 중에 단어를 더블 클릭하면 자동 검색이 가능.
<li> 생생 예문 오른편의 <font color="green" size=-1>미리보기 </font>에서 문장 전후 내용을 볼 수 있어요.
</ul>
"""
 
FOOTER = """
<font size=-1>&copy;2002,2006 
&nbsp; 
<font color="green"><b> Anydic, Anydict </b> </font>
&nbsp; &nbsp;
<!--
%s
-->
&nbsp; &nbsp;
<!--
<font color="green"><b>Dictionary on Anything, for Anyone.</b></font>
-->
</font>
<font size=-1>
<a class="q" href="/comment.html">제안,소감</a>
&nbsp; &nbsp;
<a class="q" href="http://www.dnsever.com" target="dnsever">무료네임서버</a>
</font>
<br/>
<font color="white">컴퓨터 조립, 판매, MP3 플래이어, PDA, PS2 </font>
""" % (SMALL_LOGO)


#### util funcs

def breakline(lines=1):
	print "<BR/> " * lines 

def drawline():
	print "<HR size=1>"

def do_center(func_or_str, align=''):
	if align != '':
		print '<CENTER align="%s">' % align
	else:
		print "<CENTER>"

	if callable(func_or_str):
		func()
	else:
		print func_or_str

	print "</CENTER>"



####### print func

def prn_head():
	print "<html> <head>"
	print '<meta http-equiv=Content-Type content="text/html; charset=euc-kr">'

	print "<title> %s </title>" % TITLE
	print KEYWORDS
	print STYLE
	print SCRIPT
	print "</head>"
	print


def comp_naver():
	COMP_NAVER = """
"stay put" 의 뜻은 ?  "stay put"
"""
	print """<table><tr><td align="left"><font size=-1> %s </font></td></tr></table>""" % COMP_NAVER



def ann_format():
	#do_center("<table><tr><td align='left'><font size=-1> %s </font></td></tr></table>" % ANNOUNCE)
	print """<table><tr><td align="left"><font size=-1> %s </font></td></tr></table>""" % ANNOUNCE

def promote_click():
	if random.randint(1,8)==3:
		do_center( """<font size=-1>위의 광고 중에 하나를 1주일에 한번 정도만 클릭해 주시면 서비스 운영에 도움이 됩니다 ^^ (자주 클릭하면 효과 없습니다)""" )
		breakline(1)	

def do_all():
	prn_head()
	
	print BODY
	do_center ( LOGOPART )
	#print LOGOPART
	breakline()

	do_center ( AD_FORM )
	
	breakline(1)
	do_center(GOOGLE_AD)
	breakline(1)

	print """<center> <table><tr><td align="left" width=73%%>"""
	ann_format()
	print """</td>\n<td width=2%%>&nbsp;</td><td align="right" width=25%%>"""
	print """<center> \
<a href="/recent.py"> <font size=-1 color='green'> 최근 검색어 보기 </font></a><br/><br/>\
<!--
<a href="/logos.html"> <font size=-1 color='green'> 로고 후보 보기 </font></a><br/>\
-->
</center>"""
	#recent_words()
	print "</td></tr></table> </center>"

	breakline()
	drawline()
	breakline()

	promote_click()

	do_center(FOOTER)

	print "</BODY></HTML>"


## return true if  the word is English or full Korean word
def ishangul(word):
	if ord(word[0]) < 128: return 1

	i = 0
	while i <len(word) and ord(word[i]) > 128 :
		val = (ord(word[i]) << 8) + ord(word[i+1])

		if val >= 0xB0A1 and val <= 0xC8FE:
			i += 2
		else:
			break

	if i == len(word): return 1
	else: return 0

def recent_words():
	try:
		fd = open("/tmp/adword.log", "r")
	except:
		return

	wlist = []
	fd.seek(-170, 2)
	fd.readline()
	while 1:
		word = fd.readline().strip()
		if word == '': break
		if ishangul(word)==0: continue
		if wlist == [] or word != wlist[-1]:
			wlist.append(word)

	wlist.sort()

	print "<table>"
	prev = ''
	n = 0
	for word in wlist:
		if word != prev:
			if n % 2 == 0: print "<tr>"
			print """<td><font size=-1><a class="q" href="/anyd.py?F=3&W=%s">%s</a></font></td>""" % (word.replace(' ','+'), word)
			if n % 2 == 0: print "<td>&nbsp;</td>"
			if n % 2 == 1: print "</tr>"
			prev = word
			n += 1
	if n % 2 == 0:
			print "<td>&nbsp;</td></tr>"
	print "</table></font>"


if __name__ == "__main__":
	print 'Content-Type: text/html\n'
	do_all()
	
	
