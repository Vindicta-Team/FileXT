@ECHO OFF

PUSHD ..
SET src=%CD%
POPD

CALL .\Util\GenerateProject.bat -src="%src%" -build-dir="x64-linux" -generator="Ninja Multi-Config" -wsl -options="-DIs64Bit:BOOL=TRUE"