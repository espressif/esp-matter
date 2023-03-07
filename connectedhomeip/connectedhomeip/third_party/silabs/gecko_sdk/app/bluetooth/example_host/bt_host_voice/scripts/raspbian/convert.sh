#!/bin/bash

. config.sh

CURRENT_DIR=`pwd`

cd $VOBLE_HOME_DIR

convert_from=$1



if [ "$convert_from" != "ima" ] && [ "$convert_from" != "s16" ]; then
  convert_from=ima
fi


if [ "$convert_from" == "ima" ]; then
  echo Converting from ADPCM do WAV format ...
fi

if [ "$convert_from" == "s16" ]; then
  echo Converting from PCM do WAV format ...
fi

  
for file in *.$convert_from; do
  $SOX_APP -r 16000 -v 7 ${file} ${file}_converted.wav
done

echo Done

cd $CURRENT_DIR
