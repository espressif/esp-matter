"""
/******************************************************************************
 @file  computeCRC32.py

 @brief This is a script containing helper functions for the BLE OAD Image tool related to the CRC 

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
"""
import zlib
import imgBinUtil as util

def computeCRC32(fileName):
    """
    Computes CRC32 over entire file and updates the image header
    """
    crcbytes = crc32_withOffset(fileName, util.CRC_DATA_START_ADDR)
    crcData = int(crcbytes, 16)
    util.writeBytes(fileName, util.FIXED_CRC_ADDR, crcData, util.IMG_LEN_SIZE)

    return 0


def crc32(fileName):
    """
    Computes CRC32 over entire file
    """
    with open(fileName,"rb") as filePtr:
        filePtr.seek(0,0)
        content = filePtr.readlines()
    crc = None
    for eachLine in content:
      if not crc:
          crc = zlib.crc32(eachLine)
      else:
          crc = zlib.crc32(eachLine, crc)

    # Return 8 digits crc
    return format((crc & 0xFFFFFFFF), '08x')


def crc32_withOffset(fileName, dataOffset):
    """
    Computes CRC32 over entire file starting from the dataOffset
    """
    # Read data from file
    with open(fileName,"rb") as filePtr:
        filePtr.seek(dataOffset, 0)  
        content = filePtr.readlines()
    crc = None
    for eachLine in content:
      if not crc:
          crc = zlib.crc32(eachLine)
      else:
          crc = zlib.crc32(eachLine, crc)

    # Return 8 digits crc
    return format((crc & 0xFFFFFFFF), '08x')
