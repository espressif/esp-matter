@echo off
set JSON_FILE=%1
set SLI_FILE=%2
set KEY_FILE=%3

set BATFILEPATH=%~dp0
set BACKENDEXE=%BATFILEPATH%/program__v10.bat
set BACKENDLOG=%BATFILEPATH%/plog.txt

echo start /wait cmd.exe /c %BACKENDEXE% %JSON_FILE% %SLI_FILE% %KEY_FILE% >%BACKENDLOG%
start /wait cmd.exe /c %BACKENDEXE% %JSON_FILE%  %SLI_FILE% %KEY_FILE%
echo %ERRORLEVEL% >> %BACKENDLOG%
exit /B %ERRORLEVEL%