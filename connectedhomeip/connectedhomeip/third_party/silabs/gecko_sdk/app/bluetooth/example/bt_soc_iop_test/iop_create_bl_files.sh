#!/bin/bash

################################################################################
# Bluetooth - SoC Interoperability Test GBL generator
#
# This script generates the GBL files needed for SoC Interoperability Test.
# These files are used for the OTA-DFU tests.
#
# Prerequisites
# - Linux or Mac environment.
# - The presence of the sed (stream editor) package.
# - PATH_SCMD and PATH_GCCARM environment variables to be set. See readme.md
# for more information.
# - The Bluetooth - SoC Interoperability Test example generated either with
# the "Link SDK and copy project sources" or the "Copy contents" option.
# - The project is built with success.
#
# Usage
# - Run this script.
# - Copy the generated gbl files onto the storage of the phone.
# - In the EFR Connect app, open the Interoperability Test demo tile.
# - Start the test. When prompted to choose a gbl file for OTA-DFU, select the
# ota-dfu_ack.gbl file. When prompted again, select ota-dfu_non_ack.gbl.
#
# For a more detailed guide see the readme.md file of the example.
################################################################################

APP_1="ota-dfu_non_ack"
APP_2="ota-dfu_ack"
DEVICE_NAME_1="IOP_Test_1"
DEVICE_NAME_2="IOP_Test_2"

################################################################################
# Functions
################################################################################
# Checks the presence of a string in a binary.
function search_str_in_bin () {
  echo "Searching for $1 in $2"
  result="$(grep -a $1 $2 | tr -d '\0')"
  if [[ -z $result ]]; then
    echo "Error: $1 cannot be found in $2"
    read -rsp $'Press enter to continue...\n'
    exit
  else
    echo "Found: $result"
  fi
}

################################################################################
# Entry point
################################################################################
# Locate the create_bl_files.sh script
CREATE_BL_FILES="$(dirname $0)"/"create_bl_files.sh"
if [[ -z ${CREATE_BL_FILES} ]]; then
  echo "Error: ${CREATE_BL_FILES} was not found."
  echo "Was the project generated with the copy option?"
  read -rsp $'Press enter to continue...\n'
  exit
fi

echo "**********************************************************************"
echo "Generating gbl file for the default application."
echo "**********************************************************************"
echo

source ${CREATE_BL_FILES}
mv -f ${PATH_GBL}/${OTA_APPLI_NAME}.gbl ${PATH_GBL}/${APP_1}.gbl

# Check device name
search_str_in_bin ${DEVICE_NAME_1} "${PATH_GBL}/${APP_1}.gbl"

echo "**********************************************************************"
echo "Generating gbl file for the updated application."
echo "**********************************************************************"
echo

# Make a copy of the out file
cp "${PATH_OUT}" "${PATH_OUT}_backup"

# Change the device name by manipulating the out file
if [ "$(uname)" = "Darwin" ]; then
  LC_ALL=C sed -i '.backup' -e "s/${DEVICE_NAME_1}/${DEVICE_NAME_2}/" "${PATH_OUT}"
else
  sed -bi "s/${DEVICE_NAME_1}/${DEVICE_NAME_2}/" "${PATH_OUT}"
fi

retVal=$?
if [ $retVal -ne 0 ]; then
  echo "Error $retVal: Failed to change the device name in ${PATH_OUT}."
  read -rsp $'Press enter to continue...\n'
  exit
fi

bash ${CREATE_BL_FILES}
mv -f ${PATH_GBL}/${OTA_APPLI_NAME}.gbl ${PATH_GBL}/${APP_2}.gbl

# Clean up
# Restore the original out file
mv -f "${PATH_OUT}_backup" "${PATH_OUT}"
# Only keep the necessary gbl files
rm -f "${PATH_GBL}/${OTA_APPLO_NAME}.gbl"
rm -f "${PATH_GBL}/${OTA_APPLO_NAME}-crc.gbl"
rm -f "${PATH_GBL}/${OTA_APPLI_NAME}.gbl"
rm -f "${PATH_GBL}/${OTA_APPLI_NAME}-crc.gbl"
rm -f "${PATH_GBL}/${UARTDFU_FULL_NAME}.gbl"
rm -f "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.gbl"

# Check device name
search_str_in_bin ${DEVICE_NAME_2} "${PATH_GBL}/${APP_2}.gbl"
