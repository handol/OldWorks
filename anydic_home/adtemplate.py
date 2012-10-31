#!/usr/local/bin/python
# -*- coding: EUC-KR -*-

import random

GOOGLE_AJAX_HEAD = """\
			<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
			<html xmlns="http://www.w3.org/1999/xhtml">
		<head> 
			<meta http-equiv="content-type" content="text/html; charset=euc-kr"/>    
					<!-- <title> 애니딕 영어 예문 사전 : Google AJAX Search </title>  --> 
		    <link href="http://www.google.com/uds/css/gsearch.css" type="text/css" rel="stylesheet"/>
			<script src="http://www.google.com/uds/api?file=uds.js&amp;v=1.0&amp;key=ABQIAAAAuJ3hfnP9diZwZOI8o_i3PxSwhidGAexeFr_Ys8opf9fuPuzevBTiT0isnEvqAs0r1fUMSnGe8Y0NmQ" type="text/javascript"></script>
			
			<script language="Javascript" type="text/javascript">    
			//<![CDATA[    
			function OnLoad() {      // Create a search control      
			var searchControl = new GSearchControl();      // Add in a full set of searchers      
			var localSearch = new GlocalSearch();
			searchControl.addSearcher(new GimageSearch());
			searchControl.addSearcher(new GwebSearch());      
			searchControl.addSearcher(new GblogSearch());      
			searchControl.addSearcher(new GvideoSearch());      
			// Set the Local Search center point      
			//localSearch.setCenterPoint("New York, NY");      

			// Tell the searcher to draw itself and tell it where to attach      
			var drawOptions = new GdrawOptions();
			drawOptions.setDrawMode(GSearchControl.DRAW_MODE_TABBED);
			searchControl.draw(document.getElementById("searchcontrol"), drawOptions);      

			// Execute an inital search      
			searchControl.execute("stringToSearch");    }    
			
			GSearch.setOnLoadCallback(OnLoad);  
			//]]>
			</script>

<META HTTP-EQUIV="Keywords" CONTENT="%s">
<TITLE> %s </TITLE>
<LINK REL="stylesheet" HREF="/def.css" TYPE="text/css">
<script src="/wclick.js" language=javascript></script>
			</head>
"""

HEAD= """\
<HTML>
<HEAD>
<META http-equiv=Content-Type content="text/html; charset=euc-kr">
<META HTTP-EQUIV="Keywords" CONTENT="%s">

<TITLE> %s </TITLE>
<LINK REL="stylesheet" HREF="/def.css" TYPE="text/css">
<LINK href="/favicon.ico" rel="SHORTCUT ICON" type="image/x-icon">
<script src="/wclick.js" language=javascript></script>
</HEAD>
"""


NEW_GOOGLE_AD = """\
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

GOOGLE_AD = """\
<script type="text/javascript"><!--
google_ad_client = "pub-3002816070890467";
google_ad_width = 728;
google_ad_height = 90;
google_ad_format = "728x90_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "EDF9FF";
/*
google_color_bg = "DFF2FD";
*/
google_color_bg = "EDF9FF";
google_color_link = "blue";
google_color_url = "green";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
<br/>
"""

GOOGLE_AD = NEW_GOOGLE_AD

GOOGLE_ANALYTICS = """\
<script src="http://www.google-analytics.com/urchin.js" type="text/javascript">
</script>
<script type="text/javascript">
_uacct = "UA-1321944-1";
urchinTracker();
</script>
"""

TITLE = "애니딕 "

BODY = """\
<body bgcolor=#ffffff text=#000000 link=#0000cc vlink=#0000cc alink=#ff0000 
	onLoad="" ondblclick="ad_go()">
"""

pkgad1 = """\
<script type="text/javascript"><!--
google_ad_client = "pub-3002816070890467";
google_ad_output = "textlink";
google_ad_format = "ref_text";
google_cpa_choice = "CAAQ7K-kyAIaCHhbgWRwFbrNKJT5uYsB";
google_ad_channel = "";
//--></script>
<script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
"""

pkgad2 = """\
<script type="text/javascript"><!--
google_ad_client = "pub-3002816070890467";
google_ad_width = 110;
google_ad_height = 32;
google_ad_format = "110x32_as_rimg";
google_cpa_choice = "CAAQ0LnEmwIaCKCSGkPG7dgLKLzHuYsB";
google_ad_channel = "";
//--></script>
<script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
"""


referal_ad = """
<script type="text/javascript"><!--
google_ad_client = "pub-3002816070890467";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as_rimg";
google_cpa_choice = "CAAQ9di1_wEaCJZej6mzxZUvKIGN4YcB";
google_ad_channel = "";
//-->
</script>
<script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
"""

Google_Pkg_Ad = """\
<TD width=200 >
<FONT size=+0>
%s
</FONT>
</TD>
""" % (referal_ad)

#<TD width=200 >

Google_Pkg_Ad = """\
<TD>
<FONT size=-1>
%s
</FONT>
</TD>
""" % (""" <a class="qg"  href="adsense/1.html" target="_new"> <font size=-1> 자신의 블로그, 홈피에서 수익 만들기(구글 애드센스 설명)</font></a>""" )



LOGO= """\
<!-- google_ad_section_start -->
<TABLE cellSpacing=0 cellPadding=0 width=728 border=0>
<TBODY>
	<TR> 
	<TD width=33%% align="left" noWrap > &nbsp; &nbsp; 
		<FONT color=green><B>애니딕 생생 영어예문사전</B></FONT> 
	</td> 
	<TD width=33%% noWrap align="left"> 
	<a href="/">
		<IMG alt="애니딕" src="/img/logo8-50.jpg" border=0>
	</a> 
	</TD>
	<TD width=33%% align="right" >  <FONT color="white" size=-1> %s </FONT> </td> 
	%s 
	</TR>
</TBODY>
</TABLE>
<!-- google_ad_section_end -->
""" 

CGI = "ad.py"

OLD_FORM_COLOR = "#00b4da"
FORM = """\
<!-- google_ad_section_start -->
<TABLE cellSpacing=0 cellPadding=0 width="728" bgColor=#00b4da border=0>
<TBODY>
<TR height=40>
<TD class=q vAlign=middle noWrap>
    <form name="adform" action="/%s" method="get">
		<b>
       <FONT size=-1 color="white">&nbsp;
        영영/<font color="white">한영</font>/영한/예문/숙어/속담/그림
        </FONT>
		</b>
        &nbsp;&nbsp;
       <input type=hidden name="F" value=3>
        <INPUT size=30 name="W"  maxlength=50 value="%s">
        <input name="" type=image height=21 alt=검색 hspace=5 src="/img/srch2.gif" width=35 align=absMiddle border=0>
 
        &nbsp;&nbsp;
        <A class="q" href="/help.html"> <FONT size=-1 color="yellow">도움말</FONT></A>
</TD>

	<TD nowrap align="left">
        &nbsp;&nbsp;
	<a class="qg" href="/recent.py"> <font size=-1 color="yellow"> 실시간 검색어 </font></a>
        &nbsp;
	</td> 

</TR>
</TBODY></TABLE>
<!-- google_ad_section_end -->
"""

 
MOTO = [ "또 하나의 사전", "Dictionary for Anything, Anyone"]

FOOT2 = \
"""
<font size=-1>
&nbsp; &nbsp;
<a class="qg" href="/comment.py">제안,소감</a>
&nbsp; &nbsp;
<a class="qg" href="/logos.html">로고 보기</a>
&nbsp; &nbsp;
<a class="qg" href="http://www.dnsever.com" target="dnsever">무료네임서버</a>
</font>
"""

FOOT1 =\
"""
<BR/>
<HR size=1>
<font size=-1>
<font color="black">
&copy;2002,2007 &nbsp;  <i> %s </i> &nbsp; <b> Anydic, Anydict </b>
""" % random.choice(MOTO)

FOOTER = FOOT1 + Google_Pkg_Ad + "<br/>" + FOOT2 + "<br/>"


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
		func_or_str()
	else:
		print func_or_str

	print "</CENTER>"



####### print func

RAND = 0

def adHead(keywords, title):
	print HEAD % (keywords, title)

def googleHead(word, keyword="애니딕 영어 예문 사전", title="애니딕 생생 영어 예문 사전"):
	header = GOOGLE_AJAX_HEAD.replace("stringToSearch", word);
	print header % (keyword, title)

def adLogo(keyForAds=''):
	print BODY

	#do_center ( LOGO)
	keyForAds = ""
	print LOGO % ("&nbsp;", keyForAds)
	#print LOGO % (keyForAds, Google_Pkg_Ad)
	breakline()

def adForm(cginame, searchword):
	print FORM % (cginame, searchword)
	breakline()
	#adGoogleSearch()
	

def adGoogleSearch():
	GSEARCH = """\
<!-- SiteSearch Google -->
<form method="get" action="http://www.google.co.kr/custom" target="google_window">
<table border="0" bgcolor="#ffffff">
<tr><td nowrap="nowrap" valign="top" align="left" height="32">
<a href="http://www.google.com/">
<img src="http://www.google.com/logos/Logo_25wht.gif" border="0" alt="Google" align="middle"></img></a>
</td>
<td nowrap="nowrap">
<input type="hidden" name="domains" value="anydict.com"></input>
<label for="sbi" style="display: none">검색어를 입력하십시오.</label>
<input type="text" name="q" size="32" maxlength="255" value="" id="sbi"></input>
<label for="sbb" style="display: none">검색양식 제출</label>
<input type="submit" name="sa" value="검색" id="sbb"></input>
</td></tr>
<tr>
<td>&nbsp;</td>
<td nowrap="nowrap">
<table>
<tr>
<td>
<input type="radio" name="sitesearch" value="" id="ss0"></input>
<label for="ss0" title="웹 검색"><font size="-1" color="#000000">Web</font></label></td>
<td>
<input type="radio" name="sitesearch" value="anydict.com" checked id="ss1"></input>
<label for="ss1" title="검색 anydict.com"><font size="-1" color="#000000">anydict.com</font></label></td>
</tr>
</table>
<input type="hidden" name="client" value="pub-3002816070890467"></input>
<input type="hidden" name="forid" value="1"></input>
<input type="hidden" name="ie" value="EUC-KR"></input>
<input type="hidden" name="oe" value="EUC-KR"></input>
<input type="hidden" name="cof" value="GALT:#008000;GL:1;DIV:#336699;VLC:663399;AH:center;BGC:FFFFFF;LBGC:FFFFFF;ALC:0000FF;LC:0000FF;T:000000;GFNT:0000FF;GIMP:0000FF;LH:50;LW:174;L:http://anydict.com/img/logo8-50.jpg;S:http://anydict.com;FORID:1"></input>
<input type="hidden" name="hl" value="ko"></input>
</td></tr></table>
</form>
<!-- SiteSearch Google -->
"""
	print GSEARCH
	print "<BR/>"

def adGoogle():
	#do_center(GOOGLE_AD)
	print GOOGLE_AD
	breakline()


def adPromote():
	return
	print """<BR/><table width=728><tr><td align="center"><font size=-1>위의 광고 중에 하나를 1주일에 한번 정도만 클릭해 주시면 서비스 운영에 도움이 됩니다 ^^ (자주 클릭하면 효과 없습니다) </td></tr></table>""" 

def adFooter(flag=0):
	#do_center(FOOTER)
	if flag:
		if random.randint(1,8)==1: adPromote()
		print "<BR/>"
		#adGoogle()
		print FOOTER
	else:
		print "<BR/>"
		#adGoogle()
		print FOOTER
	print GOOGLE_ANALYTICS
	print "</BODY> </HTML>"


if __name__ == "__main__":
	print 'Content-Type: text/html\n'

	adHead("여호텔 예약, 비행기 예매", "This test title")
	adLogo()
	adForm("ad.py","hello")
	adGoogle()
	adFooter()
	
