set QTDIR=D:\Qt\5.9.7\msvc2017_64
set VSDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0
set QMAKESPEC=win32-msvc

call "%VSDIR%\VC\vcvarsall.bat" x64

%QTDIR%\bin\qmake.exe -tp vc -r ../../cgParse.pro

set PATH=%PATH%;%QTDIR%\bin;%~dp0\src\debug

devenv cgParse.sln