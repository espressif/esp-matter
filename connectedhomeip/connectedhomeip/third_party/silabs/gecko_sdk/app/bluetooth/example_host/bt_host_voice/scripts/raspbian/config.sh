#!/bin/bash

#List of required files
# init.sh
# config.sh
# run_voble_player.sh
# play.sh

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR

#Default directories
ALEXA_HOME_DIR="$HOME/Desktop/alexa-avs-sample-app"
VOBLE_HOME_DIR="$HOME/Desktop/host-app/voice/exe"

#Default file locatioin
AUDIO_FILE_NAME="audio_data"
TRANSFER_STATUS_FILE="transfer_status.log"
IMA_FILE_EXTENSION="ima"
PCM_FILE_EXTENSION="s16"
IMA_FILE=$AUDIO_FILE_NAME.$IMA_FILE_EXTENSION
PCM_FILE=$AUDIO_FILE_NAME.$PCM_FILE_EXTENSION
ACCESS_TOKEN_FILE_NAME="AccessToken.log"
ACCESS_TOKEN_FILE="$ALEXA_HOME_DIR/$ACCESS_TOKEN_FILE_NAME"

# Commands
REQUEST_COMMAND="request.sh"

# Applications
VOBLE_APP="voice"
#VOBLE_PARAMETERS="-v -p /dev/ttyUSB0 -b 2000000 -o $IMA_FILE -t 1"
VOBLE_PARAMETERS="-v -p /dev/ttyACM0 -b 115200 -o $IMA_FILE -t 1"

INOTIFY_WAIT_APP="inotifywait"
SOX_APP="sox"
PLAYER_APP="mpg123"
CURL_APP="curl"


is_file_exists()
{
  local status;
  if [ -f $1 ]; then
    status="true";
  else
    status="false";  
  fi
  echo "$status"
}

delete_file_if_exist()
{
  result=$(is_file_exists $1) 
  if [ $result == "true" ]; then
    echo "[INFO] Previously stored $1 file deleted." 
    rm $1
  fi
}

check_if_package_exists()
{
  package_exists=(`whereis -b $1`)

  if [ -z "${package_exists[1]}" ]; then
    echo "[ERR ] Missing $1 application. Use \"sudo apt-get install <package>\" for installing missing package."
    exit 1
  fi
}
