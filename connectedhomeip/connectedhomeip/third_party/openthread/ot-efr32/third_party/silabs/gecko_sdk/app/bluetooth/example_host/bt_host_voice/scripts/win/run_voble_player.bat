@echo off

call config.bat

cd %VOBLE_HOME_DIR%

set voble_cmd=%1%

if "%voble_cmd%"=="" (
  set voble_cmd=play
)

set file=%2%

if "%file%"=="" (
  set file=audio_data
)

set ifile=%file%.ima
set ofile=%file%.wav

set transfer_started=0
set transfer_stoped=0
set transfer_status=0

echo ***********************************************************************************
echo * Player is running in backgound mode. Please press start button for audio streaming. 
echo * Please Ctrl+C to break this script.
echo *************************************
goto %voble_cmd%


:play
set /p transfer_status=<transfer_status.log

IF %transfer_stoped%==0 IF %transfer_started%==0 IF %transfer_status%==1 (
  set transfer_started=1
  echo Transmission started
  start /MIN  ..\scripts\win\play.bat %ifile%
)

IF %transfer_stoped%==0 IF %transfer_started%==1 IF %transfer_status%==0 (
  set transfer_stoped=1
  taskkill /PID %PARENT_PID% /F /T >null

  echo Transmission stoped
)

IF %transfer_stoped%==0 IF %transfer_started%==1 IF %transfer_status%==1 (		  
  setlocal enabledelayedexpansion
  for /f "tokens=2 delims= " %%A IN ('tasklist ^| find "tail.exe"') do set PID=%%A  
  for /f "usebackq tokens=2 delims==" %%A	in (`wmic process where ^(processid^=!PID!^) get parentprocessid /value`) do (
    set PARENT_PID=%%A
  )
)


IF %transfer_stoped%==1 IF %transfer_started%==1 (
  set transfer_started=0
  set transfer_stoped=0
  set PID=
  set PARENT_PID=
)

timeout /t 1 >null
goto play


:convert
echo Converting
FOR %%i IN (*.ima) DO %SOX_APP% -r 16000 -v 7 %%i %%i_converted.wav
echo Done
goto end

:end
echo End

