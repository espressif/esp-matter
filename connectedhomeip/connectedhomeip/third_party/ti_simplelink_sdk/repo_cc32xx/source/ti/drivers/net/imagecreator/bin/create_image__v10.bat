@echo off

set JSON_FILE=%1
set SDK_PATH=%2
set SLI_FILE=%3

set BATFILEPATH=%~dp0
set BACKENDEXE=%BATFILEPATH%/SLImageCreator.exe
set BACKENDLOG=%BATFILEPATH%/p10log.txt


echo %BACKENDEXE% syscfg create_image --json %JSON_FILE% --sdk_path %SDK_PATH% --file %SLI_FILE%> %BACKENDLOG%
%BACKENDEXE% syscfg create_image --json %JSON_FILE% --sdk_path %SDK_PATH% --file %SLI_FILE%

echo %ERRORLEVEL% >> %BACKENDLOG%
pause
exit /B %ERRORLEVEL%