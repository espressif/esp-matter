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

if "%1"=="-3" (
    for /f "tokens=1,* delims= " %%a in ("%*") do set ARGS=%%b
) else (
    echo ERROR: Pigweed's mini py launcher only supports python3.
    exit /b 1
)

:: Ignore the `-3` flag, Pigweed's python is alwasy python3.
call python %ARGS%

::WINDOWS_ONLY
