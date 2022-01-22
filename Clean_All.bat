@ECHO OFF
ECHO Clean All

REM delete files individually as to not delete any save files (if we are using this directory for mods locally)
IF EXIST ".\_build\@filext\filext_x64.dll" ( DEL ".\_build\@filext\filext_x64.dll" )
IF EXIST ".\_build\@filext\filext.dll" ( DEL ".\_build\@filext\filext.dll" )
IF EXIST ".\_build\@filext\filext_x64.so" ( DEL ".\_build\@filext\filext_x64.so" )
IF EXIST ".\_build\@filext\filext_x64.pdb" ( DEL ".\_build\@filext\filext_x64.pdb" )
IF EXIST ".\_build\@filext\filext.pdb" ( DEL ".\_build\@filext\filext.pdb" )
IF EXIST ".\_build\@filext\mod.cpp" ( DEL ".\_build\@filext\mod.cpp" )

IF EXIST ".\_build\@filext\addons" (
    RD /s /q ".\_build\@filext\addons"
)

IF EXIST "./Build" (
    RD /s /q "Build"
)

IF EXIST "./Build_Linux" (
    RD /s /q "Build_Linux"
)

IF EXIST "./Build_Win32" (
    RD /s /q "Build_Win32"
)

IF EXIST "./Build_Win64" (
    RD /s /q "Build_Win64"
)
