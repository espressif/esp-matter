@echo off

call config.bat

cd %VOBLE_HOME_DIR%

%VOBLE_APP% %VOBLE_APP_PARAM%

cd %~dp0
