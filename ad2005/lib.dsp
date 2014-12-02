# Microsoft Developer Studio Project File - Name="lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak" CFG="lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "ACE_wrappers" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\anydict.lib"

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ACE_wrappers" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\anydict.lib"

!ENDIF 

# Begin Target

# Name "lib - Win32 Release"
# Name "lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lib\CGI.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\CLog.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\cronTable.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\FileUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\HashUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\Log_Msg.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\MemSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\simpleAlloc.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\Spliter.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\StrCmp.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\StrStream.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\StrUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\TimeUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\URLParse.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lib\basicDef.h
# End Source File
# Begin Source File

SOURCE=.\lib\CGI.h
# End Source File
# Begin Source File

SOURCE=.\lib\CLog.h
# End Source File
# Begin Source File

SOURCE=.\lib\Comparable.h
# End Source File
# Begin Source File

SOURCE=.\lib\Config.h
# End Source File
# Begin Source File

SOURCE=.\lib\cronTable.h
# End Source File
# Begin Source File

SOURCE=.\lib\FileUtil.h
# End Source File
# Begin Source File

SOURCE=.\lib\HashUtil.h
# End Source File
# Begin Source File

SOURCE=.\lib\Log_Msg.h
# End Source File
# Begin Source File

SOURCE=.\lib\MemSplit.h
# End Source File
# Begin Source File

SOURCE=.\lib\oneDoc.h
# End Source File
# Begin Source File

SOURCE=.\lib\PoolMng.h
# End Source File
# Begin Source File

SOURCE=.\lib\simpleAlloc.h
# End Source File
# Begin Source File

SOURCE=.\lib\simpleHash.h
# End Source File
# Begin Source File

SOURCE=.\lib\simpleMemPool.h
# End Source File
# Begin Source File

SOURCE=.\lib\simpleQueue.h
# End Source File
# Begin Source File

SOURCE=.\lib\simpleStack.h
# End Source File
# Begin Source File

SOURCE=.\lib\sortedList.h
# End Source File
# Begin Source File

SOURCE=.\lib\Spliter.h
# End Source File
# Begin Source File

SOURCE=.\lib\StrCmp.h
# End Source File
# Begin Source File

SOURCE=.\lib\StrStream.h
# End Source File
# Begin Source File

SOURCE=.\lib\StrUtil.h
# End Source File
# Begin Source File

SOURCE=.\lib\URLParse.h
# End Source File
# Begin Source File

SOURCE=.\lib\windowsDef.h
# End Source File
# Begin Source File

SOURCE=.\lib\wordType.h
# End Source File
# End Group
# End Target
# End Project
