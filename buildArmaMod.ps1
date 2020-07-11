Push-Location

"Copy extra files.."

Copy-Item "Arma mod\mod.cpp" "_build\filext"

"Copy DLLs..."
Copy-Item "x64\Release\filext_x64.dll" "_build\filext"

"Build pbos..."
$addonBuilderLoc = "C:\Program Files (x86)\Steam\steamapps\common\Arma 3 Tools\AddonBuilder"
$thisLoc = $PSScriptRoot

& "$addonBuilderLoc\AddonBuilder.exe" "$thisLoc\Arma mod\addons\filext" "$thisLoc\_build\filext\addons" -packonly -prefix="filext"


Pop-Location

"`n`nDone!"

pause