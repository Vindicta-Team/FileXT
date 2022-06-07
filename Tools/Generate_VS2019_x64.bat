@ECHO OFF

PUSHD ..
SET src=%CD%
POPD

CALL .\Util\GenerateProject.bat -src="%src%" -build-dir="x64-windows" -generator="Visual Studio 16 2019" -opengui -options="-A x64 -DIs64Bit:BOOL=TRUE"