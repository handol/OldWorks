<%@ language=vbscript codepage= 65001%>
<% Response.CharSet = "utf-8" %>

<%






'Implements ObjectControl
'
'Private oObjectContext As COMSVCSLib.ObjectContext
'
'Private Sub ObjectControl_Activate()
'    Set oObjectContext = GetObjectContext
'End Sub
'
'Private Function ObjectControl_CanBePooled() As Boolean
'    ObjectControl_CanBePooled = True
'End Function
'
'Private Sub ObjectControl_Deactivate()
'    Set oObjectContext = Nothing
'End Sub

' ===========================
'  Function to GetHTMLBin
' ===========================

'Function GetHTMLBin(URLaddress)
'    Dim Http
'
'    Set Http = CreateObject("Msxml2.ServerXMLHTTP")
'
'    With Http
'        .Open "GET", URLaddress, False
'        .setRequestHeader "Content-Type", "text/xml"
'        .setRequestHeader "Accept-Language", "ko"
'
'        .Send
'        GetHTMLBin = .ResponseText
'
'    End With
'    
'    Set Http = Nothing
'End Function
'
'' ===========================
''  Function to BinToText
'' ===========================
'Function BinToText(varBinData, intDataSizeInBytes)    ' as String
'    Dim objRs
'    Const adFldLong = "&H00000080"
'    Const adVarChar = 200
'
'    Set objRs = CreateObject("ADOR.Recordset")
'    With objRs
'        .Fields.Append "txt", adVarChar, intDataSizeInBytes, adFldLong
'        .Open
'        
'        .AddNew
'        .Fields("txt").AppendChunk varBinData
'        BinToText = objRs("txt").Value
'        .Close
'    End With
'
'    Set objRs = Nothing
'
'End Function
'
'Sub getHTML( ByVal strURL, ByRef arrLVL, ByRef arrURL, ByRef arrSTC )
'    Dim getURL
'    Dim HTMLBin
'    Dim ret
'    
'    Dim Pos
'    Dim i
'    Dim iTotal
'           
'    getURL = strURL
'    HTMLBin = GetHTMLBin(getURL)
''    ret = setArrReturn(HTMLBin, arrLVL, arrURL, arrSTC)
''    getHTML = BinToText(HTMLBin, 32000)
'
'    Pos = 1
'    Pos = InStr(Pos, HTMLBin, "$NUMEX$")
'    
'    iTotal = Mid(HTMLBin, Pos + 8, 1)
'
'    ReDim arrLVL(iTotal)
'    ReDim arrURL(iTotal)
'    ReDim arrSTC(iTotal)
'    
'    Pos = InStr(Pos, HTMLBin, "$DOCID$")
'    
'    '*긁어온 소스에서 날씨를 나타내는 gif파일의 이름을 region 배열에 저장합니다.
'    For i = 0 To iTotal - 1
'        Pos = InStr(Pos, HTMLBin, "$LVL$") + 6
'        arrLVL(i) = Mid(HTMLBin, Pos, 1)
'		
'    
'        Pos = InStr(Pos, HTMLBin, "$URL$") + 6
'
'        arrURL(i) = Mid(HTMLBin, Pos, InStr(Pos, HTMLBin, "$STC$") - Pos )
'    
'        Pos = InStr(Pos, HTMLBin, "$STC$") + 6
'		
'		if i < iTotal - 1 then
'			arrSTC(i) = Mid(HTMLBin, Pos, InStr(Pos, HTMLBin, "$DOCID$") - Pos  )
'		else
'			arrSTC(i) = Mid(HTMLBin, Pos)
'		end if
'    Next
'    
'End Sub


dim html
dim query
query = Request("query")

set html = server.createobject("getHTML.class1")
call html.getHTML("http://222.233.121.95:9030/search?query="&query&"&Dummy=0", total, arrLVL, arrURL, arrSTC)
'call getHTML("http://222.233.121.95:9030/search?query=come&Dummy=0", arrLVL, arrURL, arrSTC)

if total = 0 then 
	response.write "검색결과가 없습니다."
else
	for i = 0 to total - 1
		response.write "레벨 : " & arrLVL(i) & "<br/><br/>"
		response.write "URL : " & arrURL(i) & "<br/><br/>"
		response.write "예문 : " & arrSTC(i) & "<br/><br/><p/>"
	next
end if


'-----------------------------­------------- 
'Function HTTP_SEND (sURL) 
'    Dim lsResult 
'    Const adFldLong = &H00000080 
'    Const adVarChar = 200 
'    'XMLHTTP를 이용하여 전송 
'    With Server.CreateObject("Msxml2.ServerXMLHTTP") 
'        .Open "POST", sURL, False 
''        .setRequestHeader "Accept-Language", "ko" 
''        .setRequestHeader "Accept-Encoding", "gzip, deflate" 
''        .setRequestHeader "Content-Type", "application/x-www-form-urlenc­oded" 
'
'		 .setRequestHeader "Accept-Language","ko"
'		 .setRequestHeader "Accept-Encoding","gzip, deflate"
'		 .setRequestHeader "Content-Type","application/x-www-form-urlencoded"
'		 .setRequestHeader "Connection","Keep-Alive"
'		 .setRequestHeader "Cache-Control","no-cache"
'		 .send sURL
'
'
''        .Send ' sData 
'        lsResult = .responseBody 
'    End With 
'    '받은 결과값을 한글 인코딩 처리 
'    With Server.CreateObject("ADODB.Recordset") 
'        .Fields.Append "txt", adVarChar, LenB(lsResult), adFldLong 
'        .Open 
'        .AddNew 
'        .Fields("txt").AppendChunk lsResult 
'        HTTP_SEND = .Fields("txt").Value 
'        .Close 
'    End With 
'    Set lsResult = Nothing 
'End Function 
''-----------------------------­------------- 
'
'
'sURL = "http://www.daum.net" 
''sData = "txtCode=" & Server.URLEncode(document.all.­txtCode.value) & "&" & _ 
''            "txtQuestion=" & 
''Server.URLEncode(document.all.­txtQuestion.value) & "&" & _ 
''            "txtAnswer=" & Server.URLEncode(document.all.­Answer.value) 
'
'
'resultCode = HTTP_SEND (sURL) 
'
'
'Response.Write resultCode 


 
'-------
'sFile = "x.htm" 
'sURL = "http://www.yahoo.co.kr" 
'
'
'Set objXMLHTTP = CreateObject("MSXML2.serverXMLHTTP") 
'
'
'objXMLHTTP.Open "GET", sURL, False 
'objXMLHTTP.Send 
'
'
'With CreateObject("ADODB.Stream") 
' .type = 1 'adTypeBinary 
' .open 
' .write objXMLHTTP.responseBody 
' .savetofile sFile, 2' adSaveCreateOverWrite 
' .close 
'End With 
'
'
'msgbox "done" 




%>
 <meta http-equiv="Content-Type" content="text/html; charset=utf-8">



