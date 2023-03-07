#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
  ARCH=`uname -m`
  if [[ "$ARCH" == "i686" ]]; then
    IMG_CONV=../bin/mw_img_conv
  else
    IMG_CONV=../bin/mw_img_conv64
  fi
else
  IMG_CONV=../bin/mw_img_conv.exe
fi

if [ "$#" -ne 1 ] ; then
  echo "Usage: $0 <mcu_firmware>"
  exit 1
fi

path=`dirname $1`
name=`basename $1 .bin`
addr=`head -c8 $1 | od -An -t x1 -j7`

if [[ "$addr" == *"1f"* ]]; then
    echo "$IMG_CONV mcufw $1 $path/$name.fw.bin 0x1F000100"
    $IMG_CONV mcufw $1 $path/$name.fw.bin 0x1F000100
else
    echo "$IMG_CONV mcufw $1 $path/$name.fw.bin 0x00100000"
    $IMG_CONV mcufw $1 $path/$name.fw.bin 0x00100000
fi
