"""
/******************************************************************************
 @file  imgBinUtil.py

 @brief This tool is used as a helper file for OAD Image Tool

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
"""
import struct
import binascii
import sys

# Identifiers for the image type field
BIN_TYPE_PERSISTENT_APP = 0
BIN_TYPE_APP_ONLY = 1
BIN_TYPE_STACK_ONLY = 2
BIN_TYPE_COMBINED = 3
BIN_TYPE_NETWORK_PROCESSOR = 4
BIN_TYPE_FACTORY_IMAGE = 5
BIN_TYPE_BIM = 6
BIN_TYPE_MERGED = 7

# Identifiers for the segment type field
SEG_TYPE_BOUNDARY = 0
SEG_TYPE_CONTIGUOUS = 1
SEG_TYPE_NONCONTIGUOUS = 2
SEG_TYPE_SECURITY = 3
SEG_TYPE_NVRAM = 4
SEG_TYPE_DELTA = 5

# Contstants related to the fixed image header
FIXED_HDR_LEN = 44              # Length of the fixed OAD image header in bytes
FIXED_CRC_ADDR = 8              # Address in the fixed image header when the CRC will be located
FIXED_IMAGE_TYPE_ADDR = 18      # Address of the Image type variable in the fixed image header
BIN_LEN_PTR = 24                # Address of the total image length in the OAD image header
IMG_END_ADDR_PTR = 36           # Address of the image end address in the fixed image header
IMG_END_ADDR_MAX = "1EFFFF"     # Maximum end address value for the stack, this value is only valid for Chameleon

# Constants needed for multiple segment types
IMG_LEN_SIZE = 4                # Size, in bytes, of the image length field
SEG_LEN_OFFSET = 4              # Offset, in bytes, of the payload address from the start of a segment
IMG_START_OFFSET = 8            # Offset, in bytes, of the start address field from the start of a contiguous/noncontiguous segment
CRC_DATA_START_ADDR = 12        # Address that we begin calculating the CRC from

# Security Segment related constants
SIGNATURE_PAYLOAD_OFFSET = 8    # Offset, in bytes, of the signature payload from the start of the security segment
TIMESTAMP_OFFSET = 9            # Offset, in bytes, of the timestamp field from the start of the security segment
SIGN_PAYLOAD_OFFSET = 13        # Offset, in bytes, of the sign payload from the start of the security segment
ECDSA_SIGNER_LEN = 8            # ECDSA signer info length, in bytes
ECDSA_SIGNATURE_LEN = 64        # ECDSA signature length, in bytes

# Contiguous Segment related constants
IMG_PAYLOAD_OFFSET = 12         # Offset, in bytes, of the image payload from the start of the contiguous segment


if sys.version_info[0] == 2:
    py2 = True
else:
    py2 = False


def computeAddr(addrbytes):
    """
    This function takes an address, given in bytes, and returns the address
    as an integer
    """
    adrTemp = ' '.join([str(a) for a in addrbytes]).split(' ')

    # Fourth byte will be zero for cc26xx
    addr = int(adrTemp[0]) + 256*int(adrTemp[1]) + 65536*int(adrTemp[2])
    return addr


def int2byte(val, width=32):
    """
    This function converts integers to a byte array for Python 2
    """
    bytes = [0] * 4
    for n in reversed(range(width//8)):
        bytes[n] = ((val >> 8*n) & 255)

    return bytes


def writeBytes(fileName, address, value, numBytes):
    """
    This function writes an integer value to the bin file at a given location
    """
    with open(fileName, 'r+b') as filePtr:
        filePtr.seek(address,0)
        if (py2):
            i=0
            dataBytes = util.int2byte(value)
            while (i<numBytes):
                filePtr.write(dataBytes[i].encode('utf-8'))
        else:
            filePtr.write(value.to_bytes(numBytes, byteorder = 'little'))


def getBoardType(fileName):
    """
    Takes in a bin file and returns the platform that it's intended for
    """
    with open(fileName, 'rb') as filePtr:
        filePtr.seek(0, 0)
        id = filePtr.read(8)
        if(id == 'OAD IMG '):
            return 'CHAMELEON'
        else:
            return 'AGAMA'


def getSegAddr(fileName, segType):
    """
    Takes in an OAD image and returns the address of the start of the security segment
    Returns -1 if the image does not contain the security segment
    """
    with open(fileName, 'rb') as filePtr:
        # Capture the length of the entire image
        filePtr.seek(BIN_LEN_PTR, 0)
        imgLen = int.from_bytes(filePtr.read(IMG_LEN_SIZE), byteorder='little')
        currSeg = FIXED_HDR_LEN

        # Checks each segment untill EOF is reached or the desired segment is found
        while currSeg < imgLen:
            # Move the file pointer to the start of the segment
            filePtr.seek(currSeg, 0)
            # Check whether we found desired segment
            if ord(filePtr.read(1)) == segType:
                return currSeg
            # Move the filePtr to the start of the next segment
            else:
                filePtr.seek(SEG_LEN_OFFSET - 1, 1)
                segLen = int.from_bytes(filePtr.read(IMG_LEN_SIZE), byteorder='little')
                currSeg += segLen
    return -1


def isSecure(binFile):
    """
    Returns whether the given binary file contains a security segment
    """
    secureSegAddr = getSegAddr(binFile, SEG_TYPE_SECURITY)

    if (secureSegAddr == -1):
        return False
    else:
        return True


def getImgSegAddr(fileName):
    """
    Takes in an OAD image and returns the address of the contiguous or noncontiguous segment
    Returns -1 if one is not found
    """
    segmentStart = getSegAddr(fileName, SEG_TYPE_CONTIGUOUS)
    if (segmentStart == -1):
        segmentStart = getSegAddr(fileName, SEG_TYPE_NONCONTIGUOUS)
    return segmentStart


def getSegLen(fileName, segStart):
    """
    Takes a file and the start of a segment and returns the length of the segment
    """
    segLen = -1
    with open(fileName, 'rb') as filePtr:
        filePtr.seek((segStart + SEG_LEN_OFFSET), 0)
        segLen = int.from_bytes(filePtr.read(IMG_LEN_SIZE), byteorder='little')

    return segLen


def getImgStartAddr(fileName):
    """
    Reads and returns the start address for the image
    """
    imgSegStart = getImgSegAddr(fileName)
    with open(fileName, 'rb') as filePtr:
        filePtr.seek(imgSegStart + IMG_START_OFFSET, 0)
        imgStartAddr = filePtr.read(IMG_LEN_SIZE)

    return computeAddr(imgStartAddr)


def getImgEndAddr(fileName):
    """
    Reads and returns the end address for the image
    """
    with open(fileName, 'rb') as filePtr:
        filePtr.seek(IMG_END_ADDR_PTR, 0)
        imgEndAddr = filePtr.read(IMG_LEN_SIZE)
    return computeAddr(imgEndAddr)


def getOverheadSize(fileName):
    """
    Returns the size, in bytes, of the entire bin file except the image segment
    """
    overhead = FIXED_HDR_LEN
    with open(fileName, 'rb') as filePtr:
        # Capture the length of the entire image
        filePtr.seek(BIN_LEN_PTR,0)
        imgLen = int.from_bytes(filePtr.read(IMG_LEN_SIZE), byteorder='little')

        currSeg = FIXED_HDR_LEN
        while currSeg < imgLen:
            # Move the file pointer to the start of the segment
            filePtr.seek(currSeg, 0)
            segType = ord(filePtr.read(1))
            # Move the filePos to the begining of the next segment
            # Read the segment payload length
            filePtr.seek(SEG_LEN_OFFSET - 1, 1)
            segLen = int.from_bytes(filePtr.read(IMG_LEN_SIZE), byteorder='little')
            if (segType != SEG_TYPE_CONTIGUOUS and segType != SEG_TYPE_NONCONTIGUOUS):
                overhead += segLen
            else:
                break
            # Move to the next segment
            currSeg += segLen
    return overhead


def updateFileType(fileName, fileType):
    """
    Update the file type bytes in the fixed image header
    """
    with open(fileName, 'r+b') as filePtr:
        filePtr.seek(FIXED_IMAGE_TYPE_ADDR, 0)
        filePtr.write(struct.pack('h', fileType))


def updateImgLen(fileName, imgStartAddr, imgEndAddr):
    """
    Updates the image end address, bin length, and image length
    """
    # Update the end adress from the image
    writeBytes(fileName, IMG_END_ADDR_PTR, imgEndAddr, IMG_LEN_SIZE)

    # Compute image length
    binLen = imgEndAddr - imgStartAddr + 1

    # Update the length value in the fixed image header and the segment
    writeBytes(fileName, BIN_LEN_PTR, binLen, IMG_LEN_SIZE)
    segLen = binLen - getOverheadSize(fileName)
    writeBytes(fileName, getImgSegAddr(fileName) + SEG_LEN_OFFSET, segLen, IMG_LEN_SIZE)


def updateMergedImageLen(fileName, binLen):
    """
    Updates the image end address, bin length, and image length
    """

   # Update end address
    newEndAddr = binLen + getImgStartAddr(fileName) - 1
    writeBytes(fileName, IMG_END_ADDR_PTR, newEndAddr, IMG_LEN_SIZE)

    # Update length of the entire bin
    writeBytes(fileName, BIN_LEN_PTR, binLen, IMG_LEN_SIZE)

    # Update length of the image segment
    segLen = binLen - getOverheadSize(fileName)
    writeBytes(fileName, getImgSegAddr(fileName) + SEG_LEN_OFFSET, segLen, IMG_LEN_SIZE)
