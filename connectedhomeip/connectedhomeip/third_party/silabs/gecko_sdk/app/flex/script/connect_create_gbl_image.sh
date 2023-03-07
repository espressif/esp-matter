#!/bin/sh

# use PATH_SCMD env var to set path for Simplicity Commander
if [[ -n ${PATH_SCMD} ]]; then
  COMMANDER="${PATH_SCMD}/commander"
fi

# project path
PATH_PROJ="$1"

if [ -z "$PATH_PROJ" ]; then
  echo "No project path specified. Using current directory."
  PATH_PROJ=`pwd`
fi

IMAGE=`find . -type f \( -iname *.s37 \)`
IMAGE_NAME=$(basename "${IMAGE%.*}")
PATH=$(dirname "$IMAGE")
IMAGE_EXT=${IMAGE##*.}

if [[ ! -f ${IMAGE} ]]; then
  echo "Error: no firmware image found"
  echo "Was the project compiled and linked successfully?"
  read -rsp $'Press enter to continue...\n'
  exit
fi

 "${COMMANDER}" gbl create "${PATH}/${IMAGE_NAME}.gbl" --app "${PATH}/${IMAGE_NAME}.${IMAGE_EXT}" --force

read -rsp $'Press enter to continue...\n'
