@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

REM Build is where we will generate project files and compile the program into.
IF NOT EXIST Build_Win32/ (
    MKDIR Build_Win32
)

REM First we check if we have cmake installed at path.
WHERE cmake
IF %ERRORLEVEL% NEQ 0 (
    REM If there is no cmake at path, we will search for cmake installed alongside visual studio.
    GOTO CMAKE_FROM_VS_INSTALL
) ELSE (
    GOTO CMAKE_LITERAL
)

:CMAKE_FROM_VS_INSTALL
for /f "usebackq tokens=*" %%i in (`call "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set InstallDir=%%i
)
SET cmake="%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
GOTO RUN_CMAKE

:CMAKE_LITERAL
REM We found cmake, so we can just run the command as is.
SET cmake=cmake

REM fallthrough intentionally to running cmake.
:RUN_CMAKE

PUSHD Build_Win32
%cmake% .. -G "Visual Studio 16 2019" -A Win32
POPD