#!/bin/sh

# use PATH_GCCARM env var to override default path for gcc-arm
if [[ -z ${PATH_GCCARM} ]]; then
  OBJCOPY="${ARM_GCC_DIR}/bin/arm-none-eabi-objcopy"
  if [[ `uname` == CYGWIN* ]]; then OBJCOPY="`cygpath ${OBJCOPY}`"; fi
else
  OBJCOPY="${PATH_GCCARM}/bin/arm-none-eabi-objcopy"
fi

# use PATH_SCMD env var to set path for Simplicity Commander
if [[ -n ${PATH_SCMD} ]]; then
  COMMANDER="${PATH_SCMD}/commander"
fi

# use PATH_OUT env var to override the full path for the .out file

# default file extension of GCC and IAR
FILE_EXTENSION_GCC="*.axf"
FILE_EXTENSION_IAR="*.out"

# names of the OTA and UART DFU output files
OTA_APPLO_NAME="apploader"
OTA_APPLI_NAME="application"
UARTDFU_FULL_NAME="full"

# names of the sign and encypt key files
GBL_SIGING_KEY_FILE="app-sign-key.pem"
GBL_ENCRYPT_KEY_FILE="app-encrypt-key.txt"

# bootlader file name
BOOTLOADER_FILE="bootloader-second-stage.s37"

# project path
PATH_PROJ="$1"

if [ -z "$PATH_PROJ" ]; then
  echo "No project path specified. Using script directory as root."
  echo
  PATH_PROJ="$(dirname $0)"
fi

# output path of the OTA and UART DFU gbl files
PATH_GBL="${PATH_PROJ}/output_gbl"

# out file path 
PATH_OUT=`find ${PATH_PROJ} -type f \( -iname ${FILE_EXTENSION_GCC} -or -iname ${FILE_EXTENSION_IAR} \) -exec echo {} \;`

if [[ -z ${PATH_OUT} ]]; then
  echo "Error: neither ${FILE_EXTENSION_GCC} nor ${FILE_EXTENSION_IAR} found"
  echo "Was the project compiled and linked successfully?"
  read -rsp $'Press enter to continue...\n'
  exit
fi

if [[ ! -f "${OBJCOPY}" ]]; then
  echo "Error: gcc-arm objcopy not found at '${OBJCOPY}'"
  echo "Use PATH_GCCARM env var to override default path for gcc-arm."
  echo "Please refer to AN1086 sections 2.3 and 3.10 on how to set the required"
  echo "environmental variables."
  read -rsp $'Press enter to continue...\n'
  exit
fi

if [[ ! -f "${COMMANDER}" ]]; then
  echo "Error: Simplicity Commander not found at '${COMMANDER}'"
  echo "Use PATH_SCMD env var to override default path for Simplicity Commander."
  echo "Please refer to AN1086 sections 2.3 and 3.10 on how to set the required"
  echo "environmental variables."
  read -rsp $'Press enter to continue...\n'
  exit
fi

if [[ -f "$BOOTLOADER_FILE" ]]; then
  echo "Bootloader file was found"
else
  echo "Bootloader file was not found"
  echo "---- BOOTLOADER GENERATION -------------------------------------------"
  echo "For adding bootloader to apploader copy a bootloader s37 image, e.g."
  echo "'bootloader-storage-internal-single.s37' to the project root folder,"
  echo "rename it to 'bootloader-second-stage.s37' and rerun the script file."
  echo "----------------------------------------------------------------------"
  echo
fi

if [[ -f "$GBL_ENCRYPT_KEY_FILE" ]]; then
  echo "Encryption key file was found"
else
  echo "Encryption key file was not found"
  echo "---- ENCRYPTED GBL FILE GENERATION -----------------------------------"
  echo "To generate an encrypted firmware upgrade file,"
  echo "copy an encryption key file into the root folder of the project,"
  echo "rename it to 'app-encrypt-key.txt' and rerun the script file."
  echo "----------------------------------------------------------------------"
  echo
fi

if [[ -f "$GBL_SIGING_KEY_FILE" ]]; then
  echo "Signing key file was found"
else
  echo "Signing key file was not found"
  echo "---- SIGNED GBL FILE GENERATION --------------------------------------"
  echo "To generate a signed firmware upgrade file,"
  echo "copy a signing key file into the root folder of the project,"
  echo "rename it to 'app-sign-key.pem' and rerun the script file."
  echo "----------------------------------------------------------------------"
  echo
fi

echo "**********************************************************************"
echo "Converting .out to .gbl files"
echo "**********************************************************************"
echo
echo ".out file used:"
echo "${PATH_OUT}"
echo
echo "output folder:"
echo "${PATH_GBL}"

mkdir -p ${PATH_GBL}

# create the GBL files
echo
echo "**********************************************************************"
echo "Creating ${OTA_APPLO_NAME}.gbl for OTA"
echo "**********************************************************************"
echo
"${OBJCOPY}" -O srec -j .text_apploader* "${PATH_OUT}" "${PATH_GBL}/${OTA_APPLO_NAME}.srec"
if [ $? -ne 0 ]; then
  read -rsp $'Press enter to continue...\n'
  exit
fi
if [[ -f $BOOTLOADER_FILE ]]; then
  echo "Bootloader file was found"
  "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}.srec" --bootloader ${BOOTLOADER_FILE}
else
  "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}.srec"
fi

echo 
echo "**********************************************************************"
echo "Creating ${OTA_APPLI_NAME}.gbl for OTA"
echo "**********************************************************************"
echo
"${OBJCOPY}" -O srec -R .text_apploader* -R .text_signature* "${PATH_OUT}" "${PATH_GBL}/${OTA_APPLI_NAME}.srec"
if [ $? -ne 0 ]; then
  read -rsp $'Press enter to continue...\n'
  exit
fi
"${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}.srec"

# create the full GBL files for UART DFU
echo
echo "**********************************************************************"
echo "Creating ${UARTDFU_FULL_NAME}.gbl for UART DFU"
echo "**********************************************************************"
echo
"${OBJCOPY}" -O srec -R .text_bootloader* "${PATH_OUT}" "${PATH_GBL}/${UARTDFU_FULL_NAME}.srec"
if [ $? -ne 0 ]; then
  read -rsp $'Press enter to continue...\n'
  exit
fi
"${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}.srec"

# create encrypted GBL file for secure boot if encrypt-key file exist
if [[ -f $GBL_ENCRYPT_KEY_FILE ]]; then
  echo
  echo "**********************************************************************"
  echo "Creating encrypted .gbl files"
  echo "**********************************************************************"
  echo
  if [[ -f $BOOTLOADER_FILE ]]; then
    echo "Bootloader file was found"
    "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --bootloader ${BOOTLOADER_FILE}
  else
    "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
  fi
  echo
  "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
  echo
  "${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}-encrypted.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
fi

# create signed GBL file for secure boot if sign-key file exists
if [[ -f $GBL_SIGING_KEY_FILE ]]; then
  echo
  echo "**********************************************************************"
  echo "Creating signed .gbl files"
  echo "**********************************************************************"
  echo
  "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLO_NAME}.srec" --secureboot --keyfile ${GBL_SIGING_KEY_FILE} -o "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec"
  if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" ]]; then
    if [[ -f $BOOTLOADER_FILE ]]; then
      echo "Bootloader file was found"
      "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader-signed.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" --sign ${GBL_SIGING_KEY_FILE} --bootloader ${BOOTLOADER_FILE}
    else
      "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-signed.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" --sign ${GBL_SIGING_KEY_FILE}
    fi
  fi
  echo
  "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLI_NAME}.srec" --secureboot --keyfile ${GBL_SIGING_KEY_FILE} -o "${PATH_GBL}/${OTA_APPLI_NAME}-signed.srec"
  "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}-signed.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}-signed.srec" --sign ${GBL_SIGING_KEY_FILE}
  echo
  if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" ]]; then
    "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" "${PATH_GBL}/${OTA_APPLI_NAME}-signed.srec" -o "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed.srec"
  else
    cp "${PATH_GBL}/${OTA_APPLI_NAME}-signed.srec" "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed.srec"
  fi
  "${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed.srec" --sign ${GBL_SIGING_KEY_FILE}
  
  # create signed and encrypted GBL file for if both sign-key and encrypt-key file exist
  if [[ -f $GBL_ENCRYPT_KEY_FILE ]]; then
    echo
    echo "**********************************************************************"
    echo "Creating signed and encrypted .gbl files"
    echo "**********************************************************************"
    echo
    if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" ]]; then
      if [[ -f $BOOTLOADER_FILE ]]; then
        echo "Bootloader file was found"
        "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader-signed-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --sign ${GBL_SIGING_KEY_FILE} --bootloader ${BOOTLOADER_FILE}
      else
        "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-signed-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-signed.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --sign ${GBL_SIGING_KEY_FILE}
      fi
      echo
    fi
    "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}-signed-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}-signed.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --sign ${GBL_SIGING_KEY_FILE}
    echo
    "${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed-encrypted.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}-signed.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --sign ${GBL_SIGING_KEY_FILE}
  fi
else
  echo
  echo "**********************************************************************"
  echo "Creating crc .gbl files"
  echo "**********************************************************************"
  echo
  "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLO_NAME}.srec" --crc -o "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec"
  if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" ]]; then
    if [[ -f $BOOTLOADER_FILE ]]; then
      echo "Bootloader file was found"
      "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader-crc.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" --bootloader ${BOOTLOADER_FILE}
    else
      "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-crc.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec"
    fi
  fi
  echo
  "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLI_NAME}.srec" --crc -o "${PATH_GBL}/${OTA_APPLI_NAME}-crc.srec"
  "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}-crc.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}-crc.srec"
  echo
  if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" ]]; then
    "${COMMANDER}" convert "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" "${PATH_GBL}/${OTA_APPLI_NAME}-crc.srec" -o "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.srec"
  else
    cp "${PATH_GBL}/${OTA_APPLI_NAME}-crc.srec" "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.srec"
  fi
  "${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.srec"

  # create crc and encrypted GBL file if encrypt-key file exist
  if [[ -f $GBL_ENCRYPT_KEY_FILE ]]; then
    echo
    echo "**********************************************************************"
    echo "Creating crc and encrypted .gbl files"
    echo "**********************************************************************"
    echo
    if [[ -f "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" ]]; then
      if [[ -f $BOOTLOADER_FILE ]]; then
        echo "Bootloader file was found"
        "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-bootloader-crc-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE} --bootloader ${BOOTLOADER_FILE}
      else
        "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLO_NAME}-crc-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLO_NAME}-crc.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
      fi
      echo
    fi
    "${COMMANDER}" gbl create "${PATH_GBL}/${OTA_APPLI_NAME}-crc-encrypted.gbl" --app "${PATH_GBL}/${OTA_APPLI_NAME}-crc.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
    echo
    "${COMMANDER}" gbl create "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc-encrypted.gbl" --app "${PATH_GBL}/${UARTDFU_FULL_NAME}-crc.srec" --encrypt ${GBL_ENCRYPT_KEY_FILE}
  fi
fi

# clean up output dir
rm "${PATH_GBL}"/*.srec

read -rsp $'Press enter to continue...\n'
