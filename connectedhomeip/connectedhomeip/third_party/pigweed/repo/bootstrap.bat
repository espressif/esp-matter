:<<"::WINDOWS_ONLY"
@echo off
:: Copyright 2020 The Pigweed Authors
::
:: Licensed under the Apache License, Version 2.0 (the "License"); you may not
:: use this file except in compliance with the License. You may obtain a copy of
:: the License at
::
::     https://www.apache.org/licenses/LICENSE-2.0
::
:: Unless required by applicable law or agreed to in writing, software
:: distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
:: WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
:: License for the specific language governing permissions and limitations under
:: the License.
::WINDOWS_ONLY
:; echo "ERROR: Attempting to run Windows .bat from a Unix/POSIX shell!"
:; echo "Instead, run the following command."
:; echo ""
:; echo "    source ./bootstrap.sh"
:; echo ""
:<<"::WINDOWS_ONLY"

:: Pigweed Windows environment setup.

:: WARNING: Multi-line "if" statements can be dangerous!
::
:: Example:
::  call do_foo
::  if [expression] (
::    call cmd_a
::    set my_var = %ERRORLEVEL%
::    call final_script --flag %my_var%
::  )
:: Batch evaluates these expressions in a way that will produce unexpected
:: behavior. It appears that when each line is executed, it does not affect
:: local context until the entire expression is complete. In this example,
:: ERRORLEVEL does not reflect `call cmd_a`, but whatever residual state was
:: present from `do_foo`. Similarly, in the call to `final_script`, `my_var`
:: will NOT be valid as the variable `set` doesn't apply until the entire `if`
:: expression completes.
:: This script only uses multi-line if statements to `goto` after an operation.

:: If PW_CHECKOUT_ROOT is set, use it. Users should not set this variable.
:: It's used because when one batch script invokes another the Powershell magic
:: below doesn't work. To reinforce that users should not be using
:: PW_CHECKOUT_ROOT, it is cleared here after it is used, and other pw tools
:: will complain if they see that variable set.
:: TODO(mohrr) find out a way to do this without PW_CHECKOUT_ROOT.

:: ~dp0 is the batchism for the directory in which a .bat file resides.
if "%PW_CHECKOUT_ROOT%"=="" ^
set "PW_ROOT=%~dp0." &^
goto select_python

:: Since PW_CHECKOUT_ROOT is set, use it.
set "PW_ROOT=%PW_CHECKOUT_ROOT%"
set "PW_CHECKOUT_ROOT="

:select_python
:: Allow forcing a specific Python version through the environment variable
:: PW_BOOTSTRAP_PYTHON. Otherwise, use the system Python if one exists.
if not "%PW_BOOTSTRAP_PYTHON%" == "" (
  set "python=%PW_BOOTSTRAP_PYTHON%"
  goto find_environment_root
)

:: Detect python installation.
where python >NUL 2>&1
if %ERRORLEVEL% EQU 0 (
  set "python=python"
  goto find_environment_root
)

echo.
echo Error: no system Python present
echo.
echo   Pigweed's bootstrap process requires a local system Python.
echo   Please install Python on your system, add it to your PATH
echo   and re-try running bootstrap.
goto finish


:find_environment_root
:: PW_ENVIRONMENT_ROOT allows developers to specify where the environment should
:: be installed. _PW_ACTUAL_ENVIRONMENT_ROOT is where Pigweed keeps that
:: information. This separation allows Pigweed to assume PW_ENVIRONMENT_ROOT
:: came from the developer and not from a previous bootstrap possibly from
:: another workspace.

if "%PW_PROJECT_ROOT%"=="" set "PW_PROJECT_ROOT=%PW_ROOT%"

if "%PW_ENVIRONMENT_ROOT%"=="" (
  set "_PW_ACTUAL_ENVIRONMENT_ROOT=%PW_PROJECT_ROOT%\environment"
) else (
  set "_PW_ACTUAL_ENVIRONMENT_ROOT=%PW_ENVIRONMENT_ROOT%"
)

set "shell_file=%_PW_ACTUAL_ENVIRONMENT_ROOT%\activate.bat"

set "_pw_start_script=%PW_ROOT%\pw_env_setup\py\pw_env_setup\windows_env_start.py"

:: If PW_SKIP_BOOTSTRAP is set, only run the activation stage instead of the
:: complete env_setup.
if not "%PW_SKIP_BOOTSTRAP%" == "" goto skip_bootstrap

:: Without the trailing slash in %PW_ROOT%/, batch combines that token with
:: the --shell-file argument.
call "%python%" "%PW_ROOT%\pw_env_setup\py\pw_env_setup\env_setup.py" ^
    --pw-root "%PW_ROOT%" ^
    --shell-file "%shell_file%" ^
    --install-dir "%_PW_ACTUAL_ENVIRONMENT_ROOT%" ^
    --config-file "%PW_ROOT%/pw_env_setup/config.json" ^
    --project-root "%PW_PROJECT_ROOT%"
goto activate_shell

:skip_bootstrap
if exist "%shell_file%" (
  call "%python%" "%_pw_start_script%"
) else (
  call "%python%" "%_pw_start_script%" --no-shell-file
  goto finish
)

:activate_shell
call "%shell_file%"

:finish
::WINDOWS_ONLY
