#!/bin/bash

# Trick for setting default directory when user click symbolic link icon from Desktop
cd `dirname \`readlink -f VoBLE_App\``

# Include  configuration script
. config.sh

# Go to VoBLE directory
cd $VOBLE_HOME_DIR

# Delete previous *.ima file
if [ -f $IMA_FILE ]; then
  echo "[INFO] Previously stored $IMA_FILE file deleted."
  rm $IMA_FILE
fi
	
#Start VoBLE application
if [ -e "$VOBLE_APP" ]; then
  echo "[INFO] Running Voice over Bluetooth Low Energy aplication."
  ./$VOBLE_APP $VOBLE_PARAMETERS
else
  echo "[ERR ] Voice over Bluetooth Low Energy aplication does not exist. Exit."  
fi




