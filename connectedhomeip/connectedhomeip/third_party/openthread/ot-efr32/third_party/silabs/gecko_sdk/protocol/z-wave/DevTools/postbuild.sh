#!/bin/bash

# Usage: bash postbuild.sh BOOTLOADER APPLICATION SERIES SDK
# BOOTLOADER: path/to/bootloader.s37
# APPLICATION: path/to/application/hex (don't include .hex in your argument)
# SERIES: 1 | 2
# SDK: path/to/sdk
# BUILD_GBL: True of False, depepending on should gbl file should be generated
# APPLICATION_GBL: Name of gbl file to be generated

# Functions are kept above all global variables to ensure that they are pure
check_return()
{
  ret=$1
  message=$2
  if [ $ret -ne 0 ]; then
    echo $message
    exit $ret
  fi  
}
sign_verify_btl()
{
  bootloader="$1"
  sign_key="$2"

  echo "Signing bootloader with ECDSA signature"
  commander convert ${bootloader} --secureboot --keyfile ${sign_key} --outfile ${bootloader}
  check_return $? "Signing bootloader failed"

  echo "Verifying bootloader signature"
  commander util verifysign ${bootloader} --verify ${sign_key}
  check_return $? "Bootloader signature verification failed"
}

sign_verify_app()
{
  hex_file="$1"
  sign_key="$2"

  echo "Signing application with ECDSA signature"
  commander convert ${hex_file} --secureboot --keyfile ${sign_key} --outfile ${hex_file}
  check_return $? "Signing application failed"

  echo "Verifying application signature"
  commander util verifysign ${hex_file} --verify ${sign_key}
  check_return $? "Verifying application signature failed"
}

generate_gbl_files()
{
  build_gbl="$1"
  series="$2"
  gbl_file="$3"
  hex_file="$4"
  sign_key="$5"
  encr_key="$6"

  if [[ "$build_gbl" == "False" ]]; then
    # Nothing to do
    return
  fi

  if [[ "$series" == "1" ]]; then
    compression="lz4"
  fi

  if [[ "$series" == "2" ]]; then
    compression="lzma"
  fi

  commander gbl create ${gbl_file} --app ${hex_file} --sign ${sign_key} --encrypt ${encr_key} --compress ${compression}
  check_return $? "Failed to generate GBL file"
}

BOOTLOADER=$1
APPLICATION=$2
SERIES=$3
SDK=$4
BUILD_GBL=$5
APPLICATION_GBL=$6

if [ $# -lt 5 ]; then
  # If argument 5 and 6 are not provided, assume that gbl file should be created
  # and use the same name as for application
  BUILD_GBL="True"
  APPLICATION_GBL=$APPLICATION
fi

# Set this to YES if we want to enable secure boot on ocelot(series2)
# using ECDSA signature verification during startup and EM4 wakeup.
# SL_APPLICATION_SIGNATURE must be set to APPLICATION_SIGNATURE_ECDSA_P256 in the
# application_properties_config.h

SECURE_BOOT="NO"

# Use controller keys for Serial API Controller apps (series 2 and series 1, ZG14) and
# sample keys for other apps.
# We have shipped Serial API Controller signed with sample key for other variants than ZG14.
# We must continue doing that so that customers can firmware upgrade existing Serial API
# Controller apps running on ZG13 / ZGM13.
# To check for the variant we must use the bootloader since the application name might not
# contain the OPN name
if [[ "$SERIES" == "2" && $APPLICATION == *"SerialAPI_Controller"* ]]; then
  SIGN_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/controller-keys/controller_sign.key
  ENCR_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/controller-keys/controller_encrypt.key
elif [[ "$SERIES" == "1" && $APPLICATION == *"SerialAPI_Controller"* && $BOOTLOADER == *"EFR32ZG14"* ]]; then
  SIGN_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/controller-keys/controller_sign.key
  ENCR_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/controller-keys/controller_encrypt.key
else
  SIGN_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/sample-keys/sample_sign.key
  ENCR_KEY=${SDK}/protocol/z-wave/platform/SiliconLabs/PAL/BootLoader/sample-keys/sample_encrypt.key
fi


# Make a local copy of the bootloader
APPLICATION_DIR="$(dirname "${APPLICATION}")"
BOOTLOADER_NAME="$(basename "${BOOTLOADER}")"
cp -v ${BOOTLOADER} ${APPLICATION_DIR}/${BOOTLOADER_NAME}
BOOTLOADER=${APPLICATION_DIR}/${BOOTLOADER_NAME}

# Enable secure boot by signing(ECDSA) the application and bootloader images.
if [[ "$SERIES" == "2" && "$SECURE_BOOT" == "YES" ]]; then
  # SL_APPLICATION_SIGNATURE must be set to APPLICATION_SIGNATURE_ECDSA_P256 in the
  # application_properties_config.h
  sign_verify_btl ${BOOTLOADER} ${SIGN_KEY}
  sign_verify_app "${APPLICATION}.hex" ${SIGN_KEY}
fi

echo "Generating gbl file"
generate_gbl_files ${BUILD_GBL} ${SERIES} "${APPLICATION_GBL}.gbl" "${APPLICATION}.hex" ${SIGN_KEY} ${ENCR_KEY}

# Check if make-v255-file.py exists
if [[ -e "${SDK}/protocol/z-wave/DevTools/make-v255-file.py" ]]; then
    echo "Making v255 hex and gbl file"
    python ${SDK}/protocol/z-wave/DevTools/make-v255-file.py ${APPLICATION}.out ${APPLICATION}_v255.out
    check_return $? "Failed to run make-v255-file.py"

    ${ARM_GCC_DIR}/bin/arm-none-eabi-objcopy -O ihex ${APPLICATION}_v255.out ${APPLICATION}_v255.hex
    check_return $? "Failed to run arm-none-eabi-objcopy"

    if [[ "$SERIES" == "2" && "$SECURE_BOOT" == "YES" ]]; then
      sign_verify_app "${APPLICATION}_v255.hex" ${SIGN_KEY}
    fi

    generate_gbl_files ${BUILD_GBL} ${SERIES} "${APPLICATION_GBL}_v255.gbl" "${APPLICATION}_v255.hex" ${SIGN_KEY} ${ENCR_KEY}
    # Cleanup temporary file that was used to generate v255.gbl only
    rm "${APPLICATION}_v255.out"
fi

# Merge is only done for series 2 devices
if [[ "$SERIES" == "2" ]]; then
    echo "Combining bootloader and application into one file"
    commander convert $BOOTLOADER $APPLICATION.hex --outfile $APPLICATION.hex
    check_return $? "Failed to merge bootloader and application"

    # Also merge the _v255 binary if it exists
    if [[ -e "${APPLICATION}_v255.hex" ]]; then
        echo "Combining bootloader and application v255 into one file"
        commander convert $BOOTLOADER ${APPLICATION}_v255.hex --outfile ${APPLICATION}_v255.hex
        check_return $? "Failed to merge bootloader and application v255"
    fi
fi

# Generate size report
echo "Generating size report"
${ARM_GCC_DIR}/bin/arm-none-eabi-size -A -x ${APPLICATION}.out > ${APPLICATION}_size.txt
check_return $? "Failed to run arm-none-eabi-size"

python $SDK/protocol/z-wave/DevTools/size_info_gen.py -i ${APPLICATION}_size.txt
check_return $? "Failed to run size_info_gen.py"

exit 0

# EOF
