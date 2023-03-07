#!/bin/sh

################################################################################
# @file
# @brief Create AppLoader migration GBL
################################################################################
# License
# <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
################################################################################
#
# The licensor of this software is Silicon Laboratories Inc. Your use of this
# software is governed by the terms of Silicon Labs Master Software License
# Agreement (MSLA) available at
# www.silabs.com/about-us/legal/master-software-license-agreement. This
# software is distributed to you in Source Code format and is governed by the
# sections of the MSLA applicable to Source Code.
#
################################################################################

# Use PATH_SCMD env var to set path for Simplicity Commander
if [[ -n ${PATH_SCMD} ]]; then
  COMMANDER="${PATH_SCMD}/commander"
fi

if [[ ! -f "${COMMANDER}" ]]; then
  echo "Error: Simplicity Commander not found at '${COMMANDER}'"
  echo "Use PATH_SCMD env var to override default path for Simplicity Commander."
  echo "Please refer to AN1086 sections 2.3 and 3.10 on how to set the required"
  echo "environmental variables."
  read -rsp $'Press enter to continue...\n'
  exit
fi

# Full path to Bootloader file
BOOTLOADER_FILE="$1"

# Full path to the output GBL file
OUTPUT_GBL_FILE="$2"

# Path to the output GBL file
APPLOADER_FILE=apploader.s37

if [ -z "$BOOTLOADER_FILE" ] || [ -z "$OUTPUT_GBL_FILE" ] ; then
  echo "Error: Required arguments not specified. Specify the bootloader and output paths:"
  echo "create_apploader_migration_gbl.sh path/to/bootloader.s37 migration_gbl_out.gbl"
  echo
  read -rsp $'Press enter to continue...\n'
  exit
fi

if [[ ! -f "${APPLOADER_FILE}" ]]; then
  echo "Error: Apploader file not found at '${APPLOADER_FILE}'"
  echo "Run this utility script in the project's 'apploader' folder'."
  read -rsp $'Press enter to continue...\n'
  exit
fi

# Create the GBL file
echo
echo "**********************************************************************"
echo "Creating ${OUTPUT_GBL_FILE} for AppLoader migration"
echo "**********************************************************************"
echo
"${COMMANDER}" gbl create --app "${APPLOADER_FILE}" --bootloader "${BOOTLOADER_FILE}" "${OUTPUT_GBL_FILE}"
