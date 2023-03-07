set SLI_FILE=%1
set KEY_FILE=%2
set VENDOR=%3
set USE_OTP=%4
set OTP_FILE=%5

set BATFILEPATH=%~dp0
set BACKENDEXE=%BATFILEPATH%/SLImageCreator.exe


if [%KEY_FILE%] == [] goto NoKey
else goto Key


:NoKey
if [%VENDOR%] == [true] goto NoKeyVendor
set COMMAND=--file %SLI_FILE%
goto runcommand
:NoKeyVendor
if [%USE_OTP%] == [true] goto NoKeyVendorOtp
set COMMAND="--file %SLI_FILE% --vendor_cert"
goto runcommand
:NoKeyVendorOtp
set COMMAND="--file %SLI_FILE% --vendor_cert --otp_file %OTP_FILE%"
goto runcommand
:Key
if [%VENDOR%] == [true] goto KeyVendor
set COMMAND="--file %SLI_FILE% --key %KEY_FILE%"
goto runcommand
:KeyVendor
if [%USE_OTP%] == [true] goto KeyVendorOtp
set COMMAND="--file %SLI_FILE% --key %KEY_FILE% --vendor_cert"
goto runcommand
:KeyVendorOtp
set COMMAND="--file %SLI_FILE% --key %KEY_FILE% --vendor_cert --otp_file %OTP_FILE%"
goto runcommand

:runcommand
start /wait cmd.exe /c %BACKENDEXE% image program %COMMAND%

exit /B 1