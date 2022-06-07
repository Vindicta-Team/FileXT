@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

set "options=-src:".\" -build-dir:"" -generator:"" -options:"" -opengui: -wsl:"
CALL :PARSE_ARG_LIST %*

PUSHD !-src!
IF NOT EXIST ".\Tools\Util\GenerateProject.bat" (
    ECHO Incorrect -src parameter supplied. -src should be set to the source root.
    exit /b 1
)

CALL :ENSURE_BUILD_DIR %build-dir%
CALL :FIND_CMAKE
CALL :RUN_CMAKE

POPD

GOTO :EOF

REM ===========================
REM ======== Functions ========
REM ===========================

:ENSURE_BUILD_DIR
IF NOT EXIST build/ (
    MKDIR build
)
IF NOT EXIST "build/!-build-dir!/" (
    MKDIR "build/!-build-dir!"
)
GOTO :EOF

:FIND_CMAKE
IF "!-wsl!" == "1" (
    SET cmake=wsl cmake
    SET cmakegui=
) ELSE (
    ECHO %CD%
    CALL .\Tools\Util\FindCMake.bat
)

GOTO :EOF

:RUN_CMAKE
PUSHD "build/!-build-dir!"

%cmake% ../.. -G "!-generator!" !-options!

POPD

IF "!-opengui!" == "1" IF "!-wsl!" == "" (
    REM Run CMake GUI so user can select options
    IF NOT "%cmakegui%" == "" (
        START %cmakegui% -S "." -B "build/!-build-dir!"
    )
)
GOTO :EOF

:PARSE_ARG_LIST args
for %%O in (%options%) do for /f "tokens=1,* delims=:" %%A in ("%%O") do set "%%A=%%~B"

:loop
if not "%~1"=="" (
  set "test=!options:*%~1:=! "
  if "!test!"=="!options! " (
      echo Error: Invalid option %~1
  ) else if "!test:~0,1!"==" " (
      set "%~1=1"
  ) else (
      setlocal disableDelayedExpansion
      set "val=%~2"
      call :escapeVal
      setlocal enableDelayedExpansion
      for /f delims^=^ eol^= %%A in ("!val!") do endlocal&endlocal&set "%~1=%%A" !
      shift /1
  )
  shift /1
  goto :loop
)
goto :endArgs
:escapeVal
set "val=%val:^=^^%"
set "val=%val:!=^!%"
exit /b
:endArgs

set -
GOTO :EOF

















