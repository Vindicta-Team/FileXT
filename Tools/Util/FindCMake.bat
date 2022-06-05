@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET cmake_=cmake
SET cmakegui_=cmake-gui

REM First we check if we have cmake installed at path.
WHERE /Q cmake
IF %ERRORLEVEL% NEQ 0 (
    SET cmake_=
)

WHERE /Q cmake-gui
IF %ERRORLEVEL% NEQ 0 (
    SET cmakegui_=
)

REM If there is no cmake at path, we will search for cmake installed alongside visual studio.
IF "%cmake_%" == "" (
    FOR /f "usebackq tokens=*" %%i IN (`call "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
        SET cmake_="%%i\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    )
)

(
    ENDLOCAL
    set "cmake=%cmake_%"
    set "cmakegui=%cmakegui_%"
)

exit /b