<?php
// string 목록 만들기.
/*
$aa = <<< END
side by side
step by step
stem cell
END;
$wlist = explode('\n', $aa);
// 이방법은 실패.
*/

$wlist = array(
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
"crystal clear"

);


$imglist = array (
'<img src="/img/logo_hp.jpg" height=40 border=0 valign=middle>',
'<img src="/img/logo40.JPG" height=40 border=0 valign=middle>',
'<img src="/img/anydict-yahoostyle2.bmp" height=30 border=0 valign=middle>',
'<img src="/img/purpl2red.JPG" height=50 border=0 valign=middle>',
'<img src="/img/c7.jpg" height=40 border=0 valign=middle>',
'<img src="/img/han3_1.JPG" height=40 border=0 valign=middle>',
'<img src="/img/logo9-70.jpg" height=40 border=0 valign=middle>',
'<img src="/img/logo12-70.jpg" height=40 border=0 valign=middle>',
'<img src="/img/logo12-70.jpg" height=40 border=0 valign=middle>',
'<img src="/img/logo12-70.jpg" height=40 border=0 valign=middle>',
'<img src="/img/logo12-70.jpg" height=40 border=0 valign=middle>'
);



$rand_num = mt_rand(0, count($wlist)-1 );
$picked = $wlist[$rand_num];

$rand_num = mt_rand(0, count($imglist)-1 );
$imgfile = $imglist[$rand_num];
$imgfile = '<a href="/logos.html"> ' .  $imgfile . ' </a>';

//print "\n". $rand_num . " ,  " . $picked . "\n";

#print gettype($aa[$rand_num]);

/*
// string 목록 만들기.
// 약간 불편한 방식. array_push() 이용
$wlist = array();

array_push($wlist, "side by side");
array_push($wlist, "step by step");
array_push($wlist, "stem cell");

$rand_num = mt_rand(0, count($wlist)-1 );
*/

?>


<html>
<head>
<meta http-equiv=Content-Type content="text/html; charset=euc-kr">

<META HTTP-EQUIV="Keywords"
CONTENT="과외, 학습지, 대학, 상담, 장남감, 
 토이 스토리, pooh, 곰돌이, 인형, 애니딕, 영어,유학, 영작,  학습, 번역, 강좌,  예문, 문장, 숙어,
 학원,영영,영영한, 생생 예문, 영어 DVD, 생활 영어, 영화, 무비, 포닉스,
 논술고사, 유아, 유치원, 초등, 중등, phonics,읽기, 듣기, 쓰기,  DVD, 영화, PDA, 레고,
 AnyDict, AnyDic">


<title>애니딕 생생 영어예문사전: AnyDic, AnyDict</title>

<style><!--
body,td,a,p,.h{font-family:굴림,돋움,arial,sans-serif;line-height=110%;}
.ko{font-size: 10pt;}
.h{font-size: 20px;} .h{color:} .q{text-decoration:none; color:#0000cc;}
.g{text-decoration:none; color:#0000cc;}
//-->
</style>

<SCRIPT LANGUAGE="JavaScript"> 
<!--
function fo() { 
document.adform.W.select(); 
document.adform.W.focus(); 
}

function open_daum() {
daumwin = window.open('', "daumwin", "scrollbars=yes,toolbar=no,location=no, directories=no,width=650,height=550,resizable=yes,mebar=no");
}
-->
</script> 
</head>

<body bgcolor=#ffffff text=#000000 link=#0000cc vlink=#0000cc alink=#ff0000 onLoad="fo()">

<center>
<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
<TBODY>
<TR align="center">
<TD noWrap align="center" width=20%> <a href="/logos.html"><IMG alt="애니딕 로고를 볼 수 ..." src="/img/logo8-70.jpg" border=0></a> </TD>   
</TR>

<TR align="center">
<TD height=100% align="center" noWrap ><FONT color=green><B>애니딕 생생 영어예문사전</B></FONT> </td> 
</TR>
</TBODY></TABLE>

</center>
<br/> 

<center>
<TABLE cellSpacing=0 cellPadding=0 width="728" bgColor=#00b4da border=0>
<TBODY>
<TR height=49>
<TD>
  <TABLE cellSpacing=0 cellPadding=0 border=0>
	<form name="adform" action="/ad.dic">
	<TBODY>
	<TR height=2> <TD colSpan=4></TD></TR>
	<TR>
	  <TD class=base vAlign=middle noWrap rowSpan=3>
	   <FONT color=#ffffff>&nbsp;<B>영영/영한/예문/숙어 </B></FONT>&nbsp;&nbsp;</TD>
	  <TD style="PADDING-TOP: 8px">
	   <input type=hidden name="F" value=3>

<?
   print '<INPUT size=30 name="W"  maxlength=50 value=';
	print '"'.$picked.'"';		
	print '> </TD> ';
?>

	  <TD style="PADDING-TOP: 8px">
		<input name="" type=image height=21 alt=검색 hspace=5 src="img/srch2.gif" width=35 align=absMiddle border=0></TD>

	  <TD class=base style="COLOR: #ffffff; PADDING-TOP: 8px" 
		noWrap>&nbsp;&nbsp;&nbsp;<A href="/help.html">
		<FONT color=#ffffff>도움말</FONT></A> </TD>
	</TR>

	<TR height=2> <TD colSpan=4></TD></TR>
	 </FORM></TBODY></TABLE>
</TD>
</TR>
</TBODY></TABLE>


<br/> <br/>
<font size=-1>
단어의 의미를  표현하는 그림 데이타도 제공됩니다. (2,000 여개 단어)<br/>
</font>
<font size=-1> 좋은 영어 사이트에서 모은 생생한 예문들이 제공되고 있습니다. </font>

<br/> <br/>
<br/>

<center>
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
</center>


<center>
<br/> 
<hr size=1> 

<?
print $imgfile;
?>
<br/> <br/>

 <font size=-1>&copy;2002,2006 
&nbsp; &nbsp; 

<font size=-1>  <a href="/comment.html">Any comment on Anydict </a> </font> 
&nbsp; &nbsp;

<a href="http://www.dnsever.com" target="dnsever">
powered by DNSever
</a>

</center>

</body>
</html>
