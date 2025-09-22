@echo off
REM Change to the directory where this script is located (project root)
pushd "%~dp0.."

setlocal enabledelayedexpansion

REM Path to dxc.exe (relative to project root)
set "DXC=External\DXC\bin\x64\dxc.exe"

REM Source and output folders
set "SRC=Source\Assets\Shaders"
set "OUT=Temp\Shaders"

REM Create output folder if it doesn't exist
if not exist "%OUT%" mkdir "%OUT%"

REM Compile vertex shaders
echo:
echo ----- Vertex Shaders in %SRC% ------
for /R "%SRC%" %%F in (*.vshader) do (
    echo:
    echo Compiling %%F
    set "filefolder=%%~dpF"
    set "relpath=!filefolder:%SRC%=!"

    rem Remove leading/trailing backslashes
    if not "!relpath!"=="" (
        if "!relpath:~0,1!"=="\" set "relpath=!relpath:~1!"
        if "!relpath:~-1!"=="\" set "relpath=!relpath:~0,-1!"
    )

    set "outdir=%OUT%"
    if not "!relpath!"=="" set "outdir=!relpath!!outdir!\"
    if not exist "!outdir!" mkdir "!outdir!"
    set "outfile=!outdir!%%~nF_vs.dxil"
    "%DXC%" -T vs_6_0 -E main -Fo "!outfile!" "%%F"
)

REM Compile pixel shaders
echo:
echo ----- Pixel Shaders in %SRC% ------
for /R "%SRC%" %%F in (*.pshader) do (
    echo:
    echo Compiling %%F
    set "filefolder=%%~dpF"
    set "relpath=!filefolder:%SRC%=!"

    rem Remove leading/trailing backslashes
    if not "!relpath!"=="" (
        if "!relpath:~0,1!"=="\" set "relpath=!relpath:~1!"
        if "!relpath:~-1!"=="\" set "relpath=!relpath:~0,-1!"
    )

    set "outdir=%OUT%"
    if not "!relpath!"=="" set "outdir=!relpath!!outdir!\"
    if not exist "!outdir!" mkdir "!outdir!"
    set "outfile=!outdir!%%~nF_ps.dxil"
    "%DXC%" -T ps_6_0 -E main -Fo "!outfile!" "%%F"
)

echo:
echo Done.
endlocal
popd