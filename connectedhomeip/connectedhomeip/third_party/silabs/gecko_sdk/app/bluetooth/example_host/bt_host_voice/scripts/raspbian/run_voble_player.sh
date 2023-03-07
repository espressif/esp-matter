#!/bin/bash

. config.sh
. init.sh

PLAY_COMMAND="play.sh $VOBLE_HOME_DIR/$IMA_FILE"
echo $PLAY_COMMAND

# Check installed required applications
check_installed_packages

CURRENT_DIR=`pwd`

# Go to VoBLE directory
cd $VOBLE_HOME_DIR

# Get current audio transmission status
TRANSFER_STATUS=`cat $TRANSFER_STATUS_FILE`

# Cleas transmission flags
TRANSFER_STARTED=0
TRANSFER_STOPED=0

$INOTIFY_WAIT_APP -e close_write,moved_to,create -m . |
while read -r directory events filename; do
  if [ "$filename" = "$TRANSFER_STATUS_FILE" ]; then
    cd $VOBLE_HOME_DIR
    TRANSFER_STATUS=`cat $TRANSFER_STATUS_FILE`
    
    if [ $TRANSFER_STOPED -eq 0 ] && [ $TRANSFER_STARTED -eq 0 ] && [ $TRANSFER_STATUS -eq 1 ];then    
      TRANSFER_STARTED=1
      echo "Audio transmission started"
      cd $CURRENT_DIR 
      ./$PLAY_COMMAND 
      cd $VOBLE_HOME_DIR      
    fi
 
    if [ $TRANSFER_STOPED -eq 0 ] && [ $TRANSFER_STARTED -eq 1 ] && [ $TRANSFER_STATUS -eq 0 ]; then
      TRANSFER_STOPED=1
      echo "Audio transmission stoped"
    fi    
    
    if [ $TRANSFER_STOPED -eq 1 ] && [ $TRANSFER_STARTED -eq 1 ]; then
      TRANSFER_STOPED=0
      TRANSFER_STARTED=0
        
      cd $CURRENT_DIR 
      ps aux | killall tail
    fi        
  fi
done