set QTDIR=C:\Qt\5.15.2\msvc2019_64
set VSDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional
set QMAKESPEC=win32-msvc
set CGPARSE_TEST_DIR=C:\Users\cglancy\Documents\GitHub\cgParse\test
set CGPARSE_TEST_LOG=C:\Temp\cgparselog.txt

call "%VSDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64

%QTDIR%\bin\qmake.exe -tp vc -r ../../cgParse.pro

set PATH=%PATH%;%QTDIR%\bin;%~dp0\src\debug

devenv cgParse.sln

pause