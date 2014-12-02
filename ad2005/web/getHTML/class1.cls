VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "class1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

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

Private Function GetHTMLBin(URLaddress)
    Dim Http

    Set Http = CreateObject("Msxml2.ServerXMLHTTP")

    With Http
        .Open "GET", URLaddress, False
        .setRequestHeader "Content-Type", "text/xml"
        .setRequestHeader "Accept-Language", "ko"

        .Send
        GetHTMLBin = .ResponseText

    End With
    
    Set Http = Nothing
End Function

' ===========================
'  Function to BinToText
' ===========================
Private Function BinToText(varBinData, intDataSizeInBytes)    ' as String
    Dim objRs As Recordset
    Const adFldLong = "&H00000080"
    Const adVarChar = 200

    Set objRs = CreateObject("ADOR.Recordset")
    With objRs
        .Fields.Append "txt", adVarChar, intDataSizeInBytes, adFldLong
        .Open
        
        .AddNew
        .Fields("txt").AppendChunk varBinData
        BinToText = objRs("txt").Value
        .Close
    End With

    Set objRs = Nothing

End Function

'Private Function setArrReturn(strHtml As String, ByRef arrLVL, ByRef arrURL, ByRef arrSTC)
'    Dim Pos As Integer
'    Dim i As Integer
'    Dim iTotal As Integer
'
'    Pos = 1
'    Pos = InStr(Pos, strHtml, "$NUMEX$")
'
'    iTotal = Mid(strHtml, Pos + 8, 1)
'
'    ReDim arrLVL(iTotal)
'    ReDim arrURL(iTotal)
'    ReDim arrSTC(iTotal)
'
'    Pos = InStr(Pos, strHtml, "$DOCID$")
'
'    '*긁어온 소스에서 날씨를 나타내는 gif파일의 이름을 region 배열에 저장합니다.
'    For i = 0 To iTotal - 1
'        Pos = InStr(Pos, strHtml, "$LVL$")
'        arrLVL(i) = Mid(strHtml, Pos + 6, 1)
'
'        Pos = InStr(Pos, strHtml, "$URL$")
'        arrURL(i) = Mid(strHtml, Pos + 6)
'
'        Pos = InStr(Pos, strHtml, "$STC$")
'        arrSTC(i) = Mid(strHtml, Pos + 6)
'    Next
'
'
'End Function


Public Sub getHTML(ByVal strURL As String, ByRef total, ByRef arrLVL, ByRef arrURL, ByRef arrSTC)
    Dim getURL As String
    Dim HTMLBin As String
    Dim ret As String
    
    Dim Pos As Integer
    Dim i As Integer
    Dim iTotal As Integer
        
    
    getURL = strURL
    HTMLBin = GetHTMLBin(getURL)

    Pos = 1
    Pos = InStr(Pos, HTMLBin, "$NUMEX$")
    
    iTotal = Mid(HTMLBin, Pos + 8, 1)
    
    total = iTotal
    ReDim arrLVL(iTotal)
    ReDim arrURL(iTotal)
    ReDim arrSTC(iTotal)
    
    Pos = InStr(Pos, HTMLBin, "$DOCID$")
    
    '*긁어온 소스에서 날씨를 나타내는 gif파일의 이름을 region 배열에 저장합니다.
    For i = 0 To iTotal - 1
        Pos = InStr(Pos, HTMLBin, "$LVL$") + 6
        arrLVL(i) = Mid(HTMLBin, Pos, 1)
        
    
        Pos = InStr(Pos, HTMLBin, "$URL$") + 6

        arrURL(i) = Mid(HTMLBin, Pos, InStr(Pos, HTMLBin, "$STC$") - Pos)
    
        Pos = InStr(Pos, HTMLBin, "$STC$") + 6
        
        If i < iTotal - 1 Then
            arrSTC(i) = Mid(HTMLBin, Pos, InStr(Pos, HTMLBin, "$DOCID$") - Pos)
        Else
            arrSTC(i) = Mid(HTMLBin, Pos)
        End If
    Next
    
End Sub











