@ECHO OFF

PUSHD ..
SET src=%CD%
POPD

CALL .\Util\GenerateProject.bat -src="%src%" -build-dir="x86-windows" -generator="Visual Studio 17 2022" -opengui -options="-A Win32 -DIs64Bit:BOOL=FALSE"