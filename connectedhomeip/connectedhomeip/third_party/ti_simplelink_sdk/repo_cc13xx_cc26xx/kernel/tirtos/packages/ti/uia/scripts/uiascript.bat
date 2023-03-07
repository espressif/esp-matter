@echo off

REM Path to the workspace
set WORKSPACE=C:\ti\ccsv6.1.0.00104\workspace_ti-rtos

REM Name of the ccs project in the workspace
set PROJROOT=uartlogging
set PROJNAME=%PROJROOT%_EK_TM4C1294XL_TI_TivaTM4C1294NCPDT
REM set PROJNAME=%PROJROOT%_MSP430F5529

REM Location of TI-RTOS, XDCTools and versions of products
set XDC_INSTALL_DIR=C:\ti\ccsv6.1.0.00104\xdctools_3_31_01_33_core
set TIRTOS_INSTALL_DIR=C:\ti\ccsv6.1.0.00104
set TIRTOS_VER=tirtos_tivac_2_14_01_20
REM set TIRTOS_VER=tirtos_msp43x_2_14_01_20
set UIA_VER=uia_2_00_02_39
set BIOS_VER=bios_6_42_02_29


REM CPU variant
REM set ext=em3
set ext=em4f

REM COM Port on which the dev board is enumerated with
set COM=COM48

REM Location of Java runtime
set JREDIR="C:\Program Files (x86)\Java\jre7"

REM Paths to products - Do not modify
set UIA_DIR=%TIRTOS_INSTALL_DIR%\%TIRTOS_VER%\products\%UIA_VER%
set BIOS_DIR=%TIRTOS_INSTALL_DIR%\%TIRTOS_VER%\products\%BIOS_VER%

set RTAXML=%WORKSPACE%\%PROJNAME%\Debug\configPkg\package\cfg\%PROJROOT%_p%ext%.rta.xml

REM Add the java bin directory into the PATH env variable
setlocal
set PATH=%JREDIR%\bin;%PATH%

REM For debugging
REM echo *********************************************************
REM echo Program file: %WORKSPACE%\%PROJNAME%\Debug\%PROJNAME%.out
REM echo RTA xml file: %RTAXML%
REM echo *********************************************************

REM Examines data directly from the UART (COM port).
REM Use the -s option to Save the data in
REM %WORKSPACE%\%PROJNAME%\systemAnalyzerData.bin
%XDC_INSTALL_DIR%\xs.exe ^
--cp %JREDIR%\lib\ext\RXTXcomm.jar ^
--xdcpath %UIA_DIR%\packages;%BIOS_DIR%\packages ^
-f %UIA_DIR%\packages\ti\uia\scripts\UIAHostFromUART.xs ^
-p %WORKSPACE%\%PROJNAME%\Debug\%PROJNAME%.out ^
-r %RTAXML% ^
-c %COM% ^
-s %WORKSPACE%\%PROJNAME%

REM Use the -d option to examine data file from a binary Tera Term log file
REM %XDC_INSTALL_DIR%\xs.exe ^
REM --cp %JREDIR%\lib\ext\RXTXcomm.jar ^
REM --xdcpath %UIA_DIR%\packages;%BIOS_DIR%\packages ^
REM -f %UIA_DIR%\packages\ti\uia\scripts\UIAHostFromUART.xs ^
REM -p %WORKSPACE%\%PROJNAME%\Debug\%PROJNAME%.out ^
REM -r %RTAXML% ^
REM -d C:\Users\A0273433\Desktop\teraterm.log ^
REM -s %WORKSPACE%\%PROJNAME%
