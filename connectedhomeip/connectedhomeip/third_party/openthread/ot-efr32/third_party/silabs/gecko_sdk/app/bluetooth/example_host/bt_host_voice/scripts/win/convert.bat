@echo off

call config.bat

cd %VOBLE_HOME_DIR%

set convert_from=%1%

if "%convert_from%"=="" (
  set convert_from=ima
)

if "%convert_from%"=="ima" (
  echo Converting from ADPCM do WAV format ...
  FOR %%i IN (*.ima) DO %SOX_APP% -r 16000 -v 7 %%i %%i_converted.wav
  echo Done
)

if "%convert_from%"=="s16" (
  echo Converting from PCM do WAV format ...
  FOR %%i IN (*.s16) DO %SOX_APP% -r 16000 -v 7 %%i %%i_converted.wav
  echo Done  
)

cd %CURRENT_DIR%
