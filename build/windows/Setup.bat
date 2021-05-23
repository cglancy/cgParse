set QTDIR=C:\Qt\5.15.2\msvc2019_64
set VSDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional
set QMAKESPEC=win32-msvc

set PARSE_ROOT=C:\Users\cglancy\Documents\GitHub\cgParse
set PARSE_DEBUG_DIR=%PARSE_ROOT%\build\Windows\src\debug
set PARSE_RELEASE_DIR=%PARSE_ROOT%\build\Windows\src\release
set CGPARSE_TEST_DIR=%PARSE_ROOT%\test
set CGPARSE_TEST_LOG=C:\Temp\cgparselog.txt

call "%VSDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64

%QTDIR%\bin\qmake.exe -tp vc -r ../../cgParse.pro

set PATH=%PATH%;%QTDIR%\bin;%~dp0\src\debug

devenv cgParse.sln

pause