@echo off

set AUDIODEV=0

set CURRENT_DIR=%~dp0
set VOBLE_HOME_DIR=%CURRENT_DIR%\..\..\exe

set VOBLE_APP=voice.exe
set VOBLE_APP_PARAM=-p COM8 -b 115200, -o audio_data.ima -t 1

set SOX_APP="C:\Program Files (x86)\sox-14-4-1\sox.exe"
