@ECHO OFF
ECHO Clean All

PUSHD ..

REM delete files individually as to not delete any save files (if we are using this directory for mods locally)
IF EXIST ".\build\@filext\filext.dll" ( DEL ".\build\@filext\filext.dll" )
IF EXIST ".\build\@filext\filext_x64.dll" ( DEL ".\build\@filext\filext_x64.dll" )
IF EXIST ".\build\@filext\filext.so" ( DEL ".\build\@filext\filext.so" )
IF EXIST ".\build\@filext\filext_x64.so" ( DEL ".\build\@filext\filext_x64.so" )
IF EXIST ".\build\@filext\filext_x64.pdb" ( DEL ".\build\@filext\filext_x64.pdb" )
IF EXIST ".\build\@filext\filext.pdb" ( DEL ".\build\@filext\filext.pdb" )
IF EXIST ".\build\@filext\mod.cpp" ( DEL ".\build\@filext\mod.cpp" )

IF EXIST ".\bin" (
    RD /s /q ".\bin"
)

IF EXIST ".\build\@filext\addons" (
    RD /s /q ".\build\@filext\addons"
)

IF EXIST ".\build\x86-linux" (
    wsl rm -r build/x86-linux
)

IF EXIST ".\build\x64-linux" (
    wsl rm -r build/x64-linux
)

IF EXIST ".\build\x86-windows" (
    RD /s /q "build\x86-windows"
)

IF EXIST ".\build\x64-windows" (
    RD /s /q "build\x64-windows"
)

POPD