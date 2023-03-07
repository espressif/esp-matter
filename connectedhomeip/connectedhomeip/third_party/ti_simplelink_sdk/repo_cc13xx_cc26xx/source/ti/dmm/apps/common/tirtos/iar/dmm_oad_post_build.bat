REM "%1 = TARGET_BPATH %2 = PROJ_DIR %3 = SDK_PATH"

ielftool.exe --ihex --verbose "%1.out" "%1.hex"

"%3/tools/common/oad/oad_image_tool.exe" --verbose iar "%2" 7 -hex1  "%1.hex"  -k "%3/tools/common/oad/private.pem" -o "%1_oad"

