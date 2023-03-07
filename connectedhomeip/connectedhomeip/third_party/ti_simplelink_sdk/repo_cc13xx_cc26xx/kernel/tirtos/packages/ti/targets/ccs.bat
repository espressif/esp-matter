@echo off
rem
rem  This bat file starts ccs with the specified executable
rem	usage: ccs [-p] executable
rem

if not defined TI_DIR (set TI_DIR=c:\CCStudio_v3.2)

set pflag=1
if "%1"=="-p" (
    set pflag=
    shift
)

if "%1"=="" (
    echo usage: ccs [-p] executable
    goto fail
)
if not exist %1 (
    echo %1 does not exist.
    goto fail
)

rem compute the generated .gel script name from the executable name
for /F "tokens=2 delims=." %%i in ("%~nx1") do (
    set gel=%~dp1package\cfg\%~n1_%%i.gel
)

if not exist %gel% (
    echo %gel% does not exist, try rebuilding %1
    goto fail
)
start %TI_DIR%\cc\bin\cc_app.exe %gel%

if defined pflag (
    set pflag=
    echo To load the %~nx1 exe  : GEL-^>XDC Package-^>load_executable
    echo To load %~nx1's project: GEL-^>XDC Package-^>open_project
    pause
)

:fail
