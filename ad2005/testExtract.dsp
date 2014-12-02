# Microsoft Developer Studio Project File - Name="testExtract" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=testExtract - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "testExtract.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "testExtract.mak" CFG="testExtract - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testExtract - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "testExtract - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testExtract - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "curl" /I "netlib" /I "dblib" /I "lib" /I "harvest" /I "extract" /I "hangul" /I "c:\ACE-5.4" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libcurl.lib ACE.lib anydict.lib /nologo /subsystem:console /machine:I386 /libpath:"extlib" /libpath:"lib" /libpath:"Release"

!ELSEIF  "$(CFG)" == "testExtract - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "extract___Win32_Debug"
# PROP BASE Intermediate_Dir "extract___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "curl" /I "netlib" /I "dblib" /I "lib" /I "harvest" /I "extract" /I "hangul" /I "c:\ACE-5.4" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libcurl.lib ACEd.lib anydict.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"extlib" /libpath:"lib" /libpath:"Debug"

!ENDIF 

# Begin Target

# Name "testExtract - Win32 Release"
# Name "testExtract - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dblib\DbConn_mssql.cpp
# End Source File
# Begin Source File

SOURCE=.\dblib\DbQuery_mssql.cpp
# End Source File
# Begin Source File

SOURCE=.\dblib\dbStore.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\Docs.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\EDict.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\Exam.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\extract.cpp
# End Source File
# Begin Source File

SOURCE=.\hangul\HanChar.cpp
# End Source File
# Begin Source File

SOURCE=.\hangul\HDict.cpp
# End Source File
# Begin Source File

SOURCE=.\hangul\HGram.cpp
# End Source File
# Begin Source File

SOURCE=.\hangul\HProc.cpp
# End Source File
# Begin Source File

SOURCE=.\hangul\HStem.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\Search.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\StrStream.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\TextLex.cpp
# End Source File
# Begin Source File

SOURCE=.\extract\TextProc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dblib\DbConn.h
# End Source File
# Begin Source File

SOURCE=.\dblib\DbQuery.h
# End Source File
# Begin Source File

SOURCE=.\dblib\dbStore.h
# End Source File
# Begin Source File

SOURCE=.\extract\Docs.h
# End Source File
# Begin Source File

SOURCE=.\extract\EDict.h
# End Source File
# Begin Source File

SOURCE=.\extract\Exam.h
# End Source File
# Begin Source File

SOURCE=.\extract\ExtractHandler.h
# End Source File
# Begin Source File

SOURCE=.\extract\Search.h
# End Source File
# Begin Source File

SOURCE=.\lib\StrStream.h
# End Source File
# Begin Source File

SOURCE=.\extract\TextLex.h
# End Source File
# Begin Source File

SOURCE=.\extract\TextProc.h
# End Source File
# End Group
# End Target
# End Project
