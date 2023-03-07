@echo off

call config.bat

set file=%1%

tail -f %file% | %SOX_APP% -t ima -c 1 -r 16000 -v 7 - -d