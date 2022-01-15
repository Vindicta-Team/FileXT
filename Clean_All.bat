IF EXIST "./_build" (
    RD /s /q "_build"
)

IF EXIST "./Build" (
    RD /s /q "Build"
)

IF EXIST "./Build_Linux" (
    RD /s /q "Build_Linux"
)

IF EXIST "./Build_Linux32" (
    RD /s /q "Build_Linux32"
)

IF EXIST "./Build_Linux64" (
    RD /s /q "Build_Linux64"
)

IF EXIST "./Build_Win32" (
    RD /s /q "Build_Win32"
)

IF EXIST "./Build_Win64" (
    RD /s /q "Build_Win64"
)

IF EXIST "./out" (
    RD /s /q "out"
)

IF EXIST "./filext.zip" (
    DEL "filext.zip"
)