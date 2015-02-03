# Microsoft Developer Studio Project File - Name="hiir" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=hiir - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hiir.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hiir.mak" CFG="hiir - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hiir - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "hiir - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hiir - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /Zm500 /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "hiir - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /Zm500 /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "hiir - Win32 Release"
# Name "hiir - Win32 Debug"
# Begin Group "Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Array.h
# End Source File
# Begin Source File

SOURCE=.\Array.hpp
# End Source File
# Begin Source File

SOURCE=.\def.h
# End Source File
# Begin Source File

SOURCE=.\Downsampler2x3dnow.h
# End Source File
# Begin Source File

SOURCE=.\Downsampler2x3dnow.hpp
# End Source File
# Begin Source File

SOURCE=.\Downsampler2xFpu.h
# End Source File
# Begin Source File

SOURCE=.\Downsampler2xFpu.hpp
# End Source File
# Begin Source File

SOURCE=.\Downsampler2xSse.h
# End Source File
# Begin Source File

SOURCE=.\Downsampler2xSse.hpp
# End Source File
# Begin Source File

SOURCE=.\fnc.h
# End Source File
# Begin Source File

SOURCE=.\fnc.hpp
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPi3dnow.h
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPi3dnow.hpp
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPiFpu.h
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPiFpu.hpp
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPiSse.h
# End Source File
# Begin Source File

SOURCE=.\PhaseHalfPiSse.hpp
# End Source File
# Begin Source File

SOURCE=.\PolyphaseIir2Designer.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyphaseIir2Designer.h
# End Source File
# Begin Source File

SOURCE=.\StageData3dnow.h
# End Source File
# Begin Source File

SOURCE=.\StageDataSse.h
# End Source File
# Begin Source File

SOURCE=.\StageProc3dnow.h
# End Source File
# Begin Source File

SOURCE=.\StageProc3dnow.hpp
# End Source File
# Begin Source File

SOURCE=.\StageProcFpu.h
# End Source File
# Begin Source File

SOURCE=.\StageProcFpu.hpp
# End Source File
# Begin Source File

SOURCE=.\StageProcSse.h
# End Source File
# Begin Source File

SOURCE=.\StageProcSse.hpp
# End Source File
# Begin Source File

SOURCE=.\Upsampler2x3dnow.h
# End Source File
# Begin Source File

SOURCE=.\Upsampler2x3dnow.hpp
# End Source File
# Begin Source File

SOURCE=.\Upsampler2xFpu.h
# End Source File
# Begin Source File

SOURCE=.\Upsampler2xFpu.hpp
# End Source File
# Begin Source File

SOURCE=.\Upsampler2xSse.h
# End Source File
# Begin Source File

SOURCE=.\Upsampler2xSse.hpp
# End Source File
# End Group
# Begin Group "Tests"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AlignedObject.h
# End Source File
# Begin Source File

SOURCE=.\AlignedObject.hpp
# End Source File
# Begin Source File

SOURCE=.\BlockSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockSplitter.h
# End Source File
# Begin Source File

SOURCE=.\ClockCycleCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\ClockCycleCounter.h
# End Source File
# Begin Source File

SOURCE=.\ClockCycleCounter.hpp
# End Source File
# Begin Source File

SOURCE=.\FileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\FileOp.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "hiir - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "hiir - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResultCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\ResultCheck.h
# End Source File
# Begin Source File

SOURCE=.\SpeedTester.h
# End Source File
# Begin Source File

SOURCE=.\SpeedTester.hpp
# End Source File
# Begin Source File

SOURCE=.\SpeedTesterBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedTesterBase.h
# End Source File
# Begin Source File

SOURCE=.\SweepingSine.cpp
# End Source File
# Begin Source File

SOURCE=.\SweepingSine.h
# End Source File
# Begin Source File

SOURCE=.\test_conf.h
# End Source File
# Begin Source File

SOURCE=.\test_fnc.h
# End Source File
# Begin Source File

SOURCE=.\test_fnc.hpp
# End Source File
# Begin Source File

SOURCE=.\TestAllClassesFnc.h
# End Source File
# Begin Source File

SOURCE=.\TestAllClassesFnc.hpp
# End Source File
# Begin Source File

SOURCE=.\TestAllClassesSpeed.h
# End Source File
# Begin Source File

SOURCE=.\TestAllClassesSpeed.hpp
# End Source File
# Begin Source File

SOURCE=.\TestDownsampler2x.h
# End Source File
# Begin Source File

SOURCE=.\TestDownsampler2x.hpp
# End Source File
# Begin Source File

SOURCE=.\TestPhaseHalfPi.h
# End Source File
# Begin Source File

SOURCE=.\TestPhaseHalfPi.hpp
# End Source File
# Begin Source File

SOURCE=.\TestUpsampler2x.h
# End Source File
# Begin Source File

SOURCE=.\TestUpsampler2x.hpp
# End Source File
# End Group
# End Target
# End Project
