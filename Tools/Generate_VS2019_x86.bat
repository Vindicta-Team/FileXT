@ECHO OFF

PUSHD ..
SET src=%CD%
POPD

CALL .\Util\GenerateProject.bat -src="%src%" -build-dir="x86-windows" -generator="Visual Studio 16 2019" -opengui -options="-A Win32 -DIs64Bit:BOOL=FALSE"