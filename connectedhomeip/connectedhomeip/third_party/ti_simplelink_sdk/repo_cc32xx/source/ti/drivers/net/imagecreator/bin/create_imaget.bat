@echo off
set JSON_FILE=%1
set SLI_FILE=%2

set BATFILEPATH=%~dp0
set BACKENDEXE=%BATFILEPATH%/create_image__v10.bat
set SDK_PATH=%~dp0\..\..\..\..\..\..\.

set BACKENDLOG=%BATFILEPATH%/cilog.txt


echo start /wait cmd.exe /c %BACKENDEXE% %JSON_FILE% %SDK_PATH% %SLI_FILE% >%BACKENDLOG%
start /wait cmd.exe /c %BACKENDEXE% %JSON_FILE% %SDK_PATH% %SLI_FILE%
echo %ERRORLEVEL% >> %BACKENDLOG%
exit /B %ERRORLEVEL%

echo %BACKENDEXE% syscfg create_image --json %JSON_FILE% --sdk_path %SDK_PATH% --file %SLI_FILE% > %BACKENDLOG%
%BACKENDEXE% syscfg create_image --json %JSON_FILE% --sdk_path %SDK_PATH% --file %SLI_FILE%
echo %ERRORLEVEL% >> %BACKENDLOG%

exit /B %ERRORLEVEL%