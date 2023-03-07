@echo off
set JSON_FILE=%1
set SLI_FILE=%2
set KEY_FILE=%3

set BATFILEPATH=%~dp0
set BACKENDLOG=%BATFILEPATH%/p10log.txt
set BACKENDEXE=%BATFILEPATH%/SLImageCreator.exe

if [%3]==[""] goto NoKey else goto Key

:Key
set COMMAND=--file %SLI_FILE% --json %JSON_FILE% --key %KEY_FILE%
echo KeyFile is %3
goto runcommand


:NoKey
set COMMAND=--file %SLI_FILE% --json %JSON_FILE%
echo NoKey
goto runcommand


@echo on
:runcommand
echo %BACKENDEXE% syscfg program %COMMAND% > %BACKENDLOG%
%BACKENDEXE% syscfg program %COMMAND%
echo %ERRORLEVEL% >> %BACKENDLOG%
pause
exit /B %ERRORLEVEL%

