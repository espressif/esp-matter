@echo off

:: use PATH_SCMD env var to override default path for Simplicity Commander
if NOT "%PATH_SCMD%"=="" (
  set COMMANDER=%PATH_SCMD%\commander.exe
)

:: change the working dir to the dir of the batch file, which should be in the project root
cd %~dp0

:: Searching for the output s37 file and store the important details
for /f "delims=" %%i in ('dir *.s37 /b/s/od') do (
  set PATH=%%~pi
  set IMAGE_NAME=%%~ni
  set IMAGE_EXT=%%~xi
)

if "%PATH%"=="" (
  echo "Error: image not found! Was the project compiled and linked successfully?"
  pause
  goto:eof
)

%COMMANDER% gbl create "%PATH%\%IMAGE_NAME%.gbl" --app "%PATH%\%IMAGE_NAME%%IMAGE_EXT%" --force

pause
