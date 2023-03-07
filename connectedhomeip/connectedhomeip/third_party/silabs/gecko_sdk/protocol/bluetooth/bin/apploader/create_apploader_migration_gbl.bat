@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: @file
:: @brief Create AppLoader migration GBL
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: License
:: <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: The licensor of this software is Silicon Laboratories Inc. Your use of this
:: software is governed by the terms of Silicon Labs Master Software License
:: Agreement (MSLA) available at
:: www.silabs.com/about-us/legal/master-software-license-agreement. This
:: software is distributed to you in Source Code format and is governed by the
:: sections of the MSLA applicable to Source Code.
::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Use PATH_SCMD env var to set path for Simplicity Commander
if NOT "%PATH_SCMD%"=="" (
  set COMMANDER=%PATH_SCMD%\commander.exe
)

if not exist "%COMMANDER%" (
  echo Error: Simplicity Commander not found at '%COMMANDER%'
  echo Use PATH_SCMD env var to override default path for Simplicity Commander.
  echo Please refer to AN1086 sections 2.3 and 3.10 on how to set the required
  echo environmental variables.
  pause
  goto:eof
)

:: Full path to Bootloader file
set BOOTLOADER_FILE=%~1

:: Full path to the output GBL file
set OUTPUT_GBL_FILE=%~2

:: Path to the output GBL file
set APPLOADER_FILE=apploader.s37

set missing_argument=false
if "%BOOTLOADER_FILE%" == "" set missing_argument=true
if "%OUTPUT_GBL_FILE%" == "" set missing_argument=true
if "%missing_argument%" == "true" (
  echo Error: Required arguments not specified. Specify the bootloader and output paths:
  echo create_apploader_migration_gbl.sh path/to/bootloader.s37 migration_gbl_out.gbl
  echo.
  pause
  goto:eof
)

if not exist "%APPLOADER_FILE%" (
  echo Error: Apploader file not found at '%APPLOADER_FILE%'
  echo Run this utility script in the project's 'apploader' folder'.
  pause
  goto:eof
)

:: Create the GBL file
echo.
echo **********************************************************************
echo Creating %OUTPUT_GBL_FILE% for AppLoader migration
echo **********************************************************************
echo.
"%COMMANDER%" gbl create --app "%APPLOADER_FILE%" --bootloader "%BOOTLOADER_FILE%" "%OUTPUT_GBL_FILE%"
