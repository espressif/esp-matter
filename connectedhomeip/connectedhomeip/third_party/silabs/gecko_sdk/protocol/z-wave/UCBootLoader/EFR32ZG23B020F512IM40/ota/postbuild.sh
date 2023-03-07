#!/bin/bash

# Post Build processing for bootloader
echo "Post Build processing for bootloader"

# default file extension of GCC and IAR
FILE_EXTENSION_GCC="*.axf"
FILE_EXTENSION_IAR="*.out"

FILENAME=$1
SDK_PATH=$2
COMMANDER=$3

# project path
PATH_PROJ="$1"
#sdk path
SDK_PATH="$2"
echo "sdk path:${SDK_PATH}"


if [ -z "${COMMANDER}" ]; then
if [ -z "${SL_COMMANDER_PATH}" ]; then
  commander_file="adapter_packs/commander/commander.exe"
  commander_path=${SDK_PATH}
  COMMANDER=${commander_path/sdks*/$commander_file}
else
  COMMANDER="${SL_COMMANDER_PATH}"
fi
fi

echo "commander : $COMMANDER"

if [ ! -f "${COMMANDER}" ]; then
  echo "Error: Simplicity Commander not found at '${COMMANDER}'"
  echo "Use PATH_SCMD env var to override default path for Simplicity Commander."
  read -rsp $'Press enter to continue...\n'
  exit
fi

if [ -z "$PATH_PROJ" ]; then
  echo "No project path specified. Using current directory."
  echo
  PATH_PROJ="$(dirname $(readlink -f $0))"
fi

# out file path 
PATH_OUT=`find ${PATH_PROJ} -type f \( -iname ${FILE_EXTENSION_GCC} -or -iname ${FILE_EXTENSION_IAR} \) -exec echo {} \;`

if [ -z "${PATH_OUT}" ]; then
  echo "Error: neither ${FILE_EXTENSION_GCC} nor ${FILE_EXTENSION_IAR} found"
  echo "Was the project compiled and linked successfully?"
  exit
fi

find ${PATH_PROJ} -type f \( -iname ${FILE_EXTENSION_GCC} -or -iname ${FILE_EXTENSION_IAR} \) -print0 | while IFS= read -r -d '' P_OUT
do
  FILENAME="${P_OUT%.*}"
  echo " "
  echo "Add checksum to image (${FILENAME}-crc.s37)"
  echo " "
  "${COMMANDER}" convert "${FILENAME}.s37" --crc -o "${FILENAME}-crc.s37"

  if [ -f $PATH_PROJ/autogen/first_stage_btl_*.s37 ]
  then
    echo " "
    echo "Add first stage bootloader to image (${FILENAME}-combined.s37)"
    echo " "
    "${COMMANDER}" convert "$(ls $PATH_PROJ/autogen/first_stage_btl_*.s37)" "${FILENAME}-crc.s37" -o "${FILENAME}-combined.s37"
  fi
done