@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD ..

CALL "./Tools/Clean_All.bat"

REM build... is where we will generate project files and compile the program into.
IF NOT EXIST build/ (
    MKDIR build
)

CALL :FIND_WSL
CALL .\Tools\Util\FindCMake.bat
CALL :RUN_CMAKE %*

POPD
GOTO :EOF

:RUN_CMAKE
IF "%have_wsl%"=="0" (
    ECHO Warning: Not building Linux binaries. Please install WSL sub-system with GCC/G++ packages.
)

IF "%~1"=="-debug" (
    echo Building - debug
    
    CALL .\Tools\Util\GenerateProject.bat -build-dir="x64-windows" -generator="Visual Studio 17 2022" -options="-A x64 -DIs64Bit:BOOL=ON"
    CALL .\Tools\Util\GenerateProject.bat -build-dir="x86-windows" -generator="Visual Studio 17 2022" -options="-A Win32 -DIs64Bit:BOOL=OFF"
    %cmake% --build "build/x64-windows" --config=Debug
    %cmake% --build "build/x86-windows" --config=Debug
    
    IF "%have_wsl%"=="1" (
        CALL .\Tools\Util\GenerateProject.bat -build-dir="x64-linux" -generator="Ninja Multi-Config" -wsl -options="-DIs64Bit:BOOL=TRUE"
        CALL .\Tools\Util\GenerateProject.bat -build-dir="x86-linux" -generator="Ninja Multi-Config" -wsl -options="-DIs64Bit:BOOL=FALSE"
        wsl cmake --build "build/x64-linux" --config=Debug
        wsl cmake --build "build/x86-linux" --config=Debug
    )
    
    PUSHD Tools
    Powershell -executionpolicy remotesigned -File .\Build_ArmaMod.ps1 "-debug"
    POPD
) ELSE (
    echo Building - release
    
    CALL .\Tools\Util\GenerateProject.bat -build-dir="x64-windows" -generator="Visual Studio 17 2022" -options="-A x64 -DIs64Bit:BOOL=ON"
    CALL .\Tools\Util\GenerateProject.bat -build-dir="x86-windows" -generator="Visual Studio 17 2022" -options="-A Win32 -DIs64Bit:BOOL=OFF"
    %cmake% --build "build/x64-windows" --config=Release
    %cmake% --build "build/x86-windows" --config=Release
    
    IF "%have_wsl%"=="1" (
        CALL .\Tools\Util\GenerateProject.bat -build-dir="x64-linux" -generator="Ninja Multi-Config" -wsl -options="-DIs64Bit:BOOL=TRUE"
        CALL .\Tools\Util\GenerateProject.bat -build-dir="x86-linux" -generator="Ninja Multi-Config" -wsl -options="-DIs64Bit:BOOL=FALSE"
        wsl cmake --build "build/x64-linux" --config=Release
        wsl cmake --build "build/x86-linux" --config=Release
    )
    
    PUSHD Tools
    Powershell -executionpolicy remotesigned -File .\Build_ArmaMod.ps1
    POPD
)
GOTO :EOF

:FIND_WSL
WHERE /Q wsl
IF %ERRORLEVEL% NEQ 0 (
    SET have_wsl=0
) ELSE (
    SET have_wsl=1
)
GOTO :EOF