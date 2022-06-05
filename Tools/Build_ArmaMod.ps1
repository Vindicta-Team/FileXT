param ([switch]$debug)

# The reason for this function is simply to not have the path "D:/SteamLibrary/steamapps/common/Arma 3 Tools" or similar hard coded
# If this gives you trouble, you can just return the path to your Arma 3 Tools directory (like above) from this function.
# It should be reliable, though - unless Steam changes it's vdf format or how it stores the steam install in the registry.
# Those have been stable for a very long time, though.
function Get-Arma3ToolsPath()
{
    # Strategy:
    # We're going to find the location of Arma 3 Tools by getting the steam install directory from the registry
    # We then look at the app install directories which are listed in steamapps/libraryfolders.vdf
    # In that file there may be multiple steam library locations. We look for the app ID of arma tools
    # and get the library location directly preceding it.
    # From there we go to <that library location>/<a hard coded path to the AddonBuilder tool>

    [string]$SteamPath = (Get-ItemProperty -Path HKCU:\SOFTWARE\Valve\Steam).SteamPath
    if($SteamPath -eq $null -or $SteamPath -eq '') {
        throw "Unrecoverable error: Could not find steam install path. Is steam installed?"
    }

    [string]$vdfPathPath = $SteamPath + "/steamapps/libraryfolders.vdf"
    if(-not (Test-Path -Path $vdfPathPath)) {
        throw "Unrecoverable error: Could not find steamapps/libraryfolders.vdf at the steam install path: $SteamPath"
    }

    [string]$vdf = Get-Content -Path $vdfPathPath
    if($vdf -eq $null -or $vdf -eq '') {
        throw "Unrecoverable error: Could not open $vdfPathPath"
    }

    [string]$armaToolsAppId = "233800"

    [int32]$armaToolsAppIdIndexInVdf = $vdf.IndexOf($armaToolsAppId)

    [int32]$lastInstallDirectoryIndex = $vdf.LastIndexOf('"path"');
    while($lastInstallDirectoryIndex -gt -1) {
        if($lastInstallDirectoryIndex -lt $armaToolsAppIdIndexInVdf) {
            break;
        }
        Write-Host $lastInstallDirectoryIndex of $vdf.length
        $lastInstallDirectoryIndex = $vdf.LastIndexOf('"path"', $lastInstallDirectoryIndex);
    }

    if($lastInstallDirectoryIndex -eq -1) {
        throw "Could not find the Arma 3 tools install directory. Are the Arma 3 tools installed through steam?"
    }

    $lastInstallDirectoryIndex += '"path"'.length;
    [int]$pathStartIndex = $vdf.IndexOf('"', $lastInstallDirectoryIndex);
    [int]$pathEndIndex = $vdf.IndexOf('"', $pathStartIndex+1);
    [string]$arma3ToolsLibraryPath = $vdf.SubString($pathStartIndex+1, $pathEndIndex-$pathStartIndex-1);

    $arma3ToolsLibraryPath = $arma3ToolsLibraryPath.Replace("\\", "/")
    $arma3ToolsLibraryPath = $arma3ToolsLibraryPath.Replace("\", "/")
    return "$arma3ToolsLibraryPath/steamapps/common/Arma 3 Tools"
}


###################################################################################################

Push-Location ..

"Ensure directories.."
New-Item "build/@filext" -ItemType Directory -Force
New-Item "build/@filext/storage" -ItemType Directory -Force

"Copy extra files.."

Copy-Item "Arma mod/mod.cpp" "build/@filext"

if($debug) {
    Write-Host "Copy DLLs... (debug)"
    Copy-Item "bin/Debug/filext.dll" "build/@filext/filext_x64.dll"
    Copy-Item "bin/Debug/filext.dll" "build/@filext/filext.dll"
    Copy-Item "bin/Debug/filext_x64.so" "build/@filext/filext_x64.so"
    Copy-Item "bin/Debug/filext.so" "build/@filext/filext.so"

    Copy-Item "bin/Debug/filext.pdb" "build/@filext/filext_x64.pdb"
    Copy-Item "bin/Debug/filext.pdb" "build/@filext/filext.pdb"
}
else {
    Write-Host "Copy DLLs... (release)"
    Copy-Item "bin/Release/filext_x64.dll" "build/@filext/filext_x64.dll"
    Copy-Item "bin/Release/filext.dll" "build/@filext/filext.dll"
    Copy-Item "bin/Release/filext_x64.so" "build/@filext/filext_x64.so"
    Copy-Item "bin/Release/filext.so" "build/@filext/filext.so"
}

"Build pbos..."
[string]$arma3ToolsPath = Get-Arma3ToolsPath

# Specific Arma 3 Tools
[string]$addonBuilderPath = "$arma3ToolsPath/AddonBuilder/AddonBuilder.exe"

[string]$thisLoc = Get-Location
[string]$addonPath = "$thisLoc/Arma mod/addons/filext"
[string]$builtPath = "$thisLoc/build/@filext/addons"

[string[]]$addonBuilderArgs=@(
    "`"$addonPath`"",
    "`"$builtPath`"",
    "-packonly",
    "-prefix=`"filext`""
)
Start-Process -FilePath $addonBuilderPath -ArgumentList $addonBuilderArgs -NoNewWindow -Wait

Pop-Location

"`n`nDone!"
