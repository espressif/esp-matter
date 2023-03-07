@echo off
set TI_DIR=c:\ti
set path=%TI_DIR%\bin\utilities\ccs_scripting\perl\bin;%TI_DIR%\bin\utilities\ccs_scripting;%path%
set PERL5LIB=%PERL5LIB%;%TI_DIR%\bin\utilities\ccs_scripting\perl\lib;%TI_DIR%\bin\utilities\ccs_scripting
perl .\ccs_start.pl %1 %2 %3 %4 %5
