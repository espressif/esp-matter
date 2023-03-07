"""
/******************************************************************************
 @file  oad_image_tool.py

 @brief This tool generates the OAD image files

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
"""

from struct import unpack
import argparse
import computeCRC32
import imgBinUtil as util
import intelhex
import os
import textwrap
import sys
from collections import namedtuple
import signUtil

__version__ = "2.1.2"

initialHeaderUnpack = '<8sIBBHBBBBIIIIIHH'
segment0Unpack = '<BHBIIIII'
segment1Unpack = '<BHBII'
segment3Unpack = '<BHBIBIQQQQQQQQQ'


def getAddress(fn, text):
    with open(fn) as f:
        for line in f:
            if len(line.strip()) != 0 :
                # Do something with the empty line
                if len(line.split()) == 2:
                    test = line.split()[1]
                    if test.split('=', 1 )[0] == text:
                        return test.split('=', 1 )[1]

    raise ValueError("Unable to find string " + text + " in " +
                     os.path.basename(fn))


def getBoundary(bdFileName):
    boundary_address = getAddress(bdFileName , "ICALL_STACK0_START")

    return int(boundary_address, 16)


def getTgtBoundary(tgtBdFileName):
    boundary_address = getAddress(tgtBdFileName , "OAD_TGT_FLASH_START")

    return int(boundary_address, 16)


def getAddress_CCS(fn, text):
    with open(fn) as f:
        for line in f:
            if line.find('--define=ICALL_STACK0_START=') != -1:
                return line[28:]


def getBoundary_CCS(bdFileName):
    boundary_address = getAddress_CCS(bdFileName, "--define=ICALL_STACK0_START")

    return int(boundary_address, 16)


def getAppRange(boundary_address):
    appImgStartAddr = "0000"

    app_boundary = hex(boundary_address - 1)[2:]
    app_range = appImgStartAddr + ":" + app_boundary

    return int(appImgStartAddr, 16), int(app_boundary, 16)


def getStackRange(boundary_address):
    stack_boundary = hex(boundary_address)[2:]
    stack_range = stack_boundary + ":" + util.IMG_END_ADDR_MAX

    return int(stack_boundary, 16), int(util.IMG_END_ADDR_MAX, 16)


def getMergeBinLen(flash_range):
    flash_end = flash_range.split(':', 2)[1]
    flash_end = flash_end.split('0x', 2)[1]
    binLen = int(flash_end, 16) + 1

    return binLen


def getMergeBinEndAddr(flash_range):
    flash_end = flash_range.split(':', 2)[1]
    flash_end = flash_end.split('0x', 2)[1]
    imgEndAddr = int(flash_end, 16)

    return imgEndAddr


def getStackImgRange(projdir, stackHexFileName, boundary_address, outputPath):
    stack_boundary, flash_limit = getStackRange(boundary_address)

    # Generate binary file
    if stackHexFileName.endswith('.hex'):
        outfileStackPath = os.path.abspath(stackHexFileName[:-4] + ".bin")
    else:
        raise Exception("Stack hex not valid")

    intelhex.hex2bin(stackHexFileName, outfileStackPath, start=stack_boundary, end=flash_limit)

    # Get the image range
    imgRange = util.getImgStartAddr(outfileStackPath), util.getImgEndAddr(outfileStackPath)
    return imgRange


def createTgtBinfile(projType, projdir, binaryFileType, appHexFileName,
                     tgtBdFileName, outputPath, keyFile):
    # Find boundary
    if projType == 'ccs':
        boundary_address = getBoundary_CCS(tgtBdFileName)
    else:
        boundary_address = getTgtBoundary(tgtBdFileName)


    stack_boundary, flash_limit = getStackRange(boundary_address)

    # Generate bin file
    flash_path = os.path.join(projdir, appHexFileName)
    outfileTargetPath = os.path.abspath(outputPath + ".bin")
    intelhex.hex2bin(flash_path, outfileTargetPath, start=stack_boundary,
                     end=flash_limit)

    util.updateImgLen(outfileTargetPath, stack_boundary, flash_limit)

    #Sign secure images
    if (util.isSecure(outfileTargetPath)):
        if keyFile:
            signUtil.signImage(outfileTargetPath, keyFile)
        else:
            raise Exception("Key file not specified for secure image!")

    computeCRC32.computeCRC32(outfileTargetPath)


def createAppStackBinfile(projType, projdir, binaryFileType, appHexFileName,
                          bdFileName, outputPath, keyFile, stackHexFileName=None):
    # Find boundary, if there is one
    if(bdFileName is not None):
        if projType == 'ccs':
            boundary_address = getBoundary_CCS(bdFileName)
        else:
            boundary_address = getBoundary(bdFileName)

    # App only or merged image
    if (binaryFileType == util.BIN_TYPE_APP_ONLY or binaryFileType == util.BIN_TYPE_MERGED or binaryFileType == util.BIN_TYPE_PERSISTENT_APP):

        # Create the paths
        app_path = os.path.join(projdir, appHexFileName)
        outfileAppPath = os.path.join(projdir, outputPath)
        outfileAppPath = os.path.abspath(outfileAppPath + ".bin")

        # Generate first application binary
        intelhex.hex2bin(app_path, outfileAppPath)
        imgStAddr = util.getImgStartAddr(outfileAppPath)
        imgEndAddr = util.getImgEndAddr(outfileAppPath)

        # Pad end if it is not word aligned for Agama
        binLen = imgEndAddr - imgStAddr + 1
        if (binLen & 3):
            imgEndAddr = imgEndAddr - (binLen & 3) + 4

        # Run the hex2bin to get correct length binary image
        intelhex.hex2bin(app_path, outfileAppPath, start=imgStAddr, end=imgEndAddr)

        # Update the fields that may be out of sync
        util.updateImgLen(outfileAppPath, imgStAddr, imgEndAddr)
        util.updateFileType(outfileAppPath, binaryFileType)

        #Sign secure images
        if (util.isSecure(outfileAppPath)):
            if keyFile:
                signUtil.signImage(outfileAppPath, keyFile)
            else:
                raise Exception("Key file not specified for secure image!")

        # Update crc32 field
        computeCRC32.computeCRC32(outfileAppPath)

    # Stack only image
    elif (binaryFileType == util.BIN_TYPE_STACK_ONLY):
        # Get the boundaries for the stack image
        stack_boundary, flash_limit = getStackRange(boundary_address)

        # Create the paths
        stack_path = os.path.join(projdir, appHexFileName)
        outfileStackPath = os.path.join(projdir, outputPath)
        outfileStackPath = outfileStackPath + ".bin"

        # Make the initial bin
        intelhex.hex2bin(stack_path, outfileStackPath)

        imgStAddr = util.getImgStartAddr(outfileStackPath)
        imgEndAddr = util.getImgEndAddr(outfileStackPath)

        # Pad end if it is not word aligned
        binLen = imgEndAddr - imgStAddr + 1
        if( binLen & 3):
            imgEndAddr = imgEndAddr - (binLen & 3) + 4

        # Rerun hex2bin to get correct length of binary image
        intelhex.hex2bin(stack_path, outfileStackPath, start=imgStAddr, end=imgEndAddr)

        util.updateImgLen(outfileStackPath, imgStAddr, imgEndAddr)

        # Sign secure images
        if (util.isSecure(outfileStackPath)):
            if keyFile:
                signUtil.signImage(outfileStackPath, keyFile)
            else:
                raise Exception("Key file not specified for secure image!")

        computeCRC32.computeCRC32(outfileStackPath)

    # Combined image
    elif (binaryFileType == util.BIN_TYPE_COMBINED):
        # Get app image range
        appImgStartAddr, app_boundary = getAppRange(boundary_address)

        # To get the actual stack range, need to build stack binary and then
        # read the image header to find the actual address
        stack_boundary, flash_limit = getStackImgRange(projdir, stackHexFileName, boundary_address, outputPath)

        app_path = os.path.join(projdir, appHexFileName)
        stack_path = os.path.join(projdir, stackHexFileName)

        outfileAppPath = os.path.join(projdir, outputPath) + ".bin"
        # Generate application binary
        intelhex.hex2bin(app_path, outfileAppPath)
        imgStAddr = util.getImgStartAddr(outfileAppPath)
        imgEndAddr = util.getImgEndAddr(outfileAppPath)

        # Pad end if it is not word aligned
        binLen = imgEndAddr - imgStAddr +1
        if (binLen & 3):
            imgEndAddr = imgEndAddr - (binLen & 3) + 4

        # Rerun the hex2bin to get correct length binary image
        intelhex.hex2bin(app_path, outfileAppPath, start=imgStAddr, end=imgEndAddr)

         # Update fields that may be incorrect
        util.updateImgLen(outfileAppPath, imgStAddr, imgEndAddr)
        util.updateFileType(outfileAppPath, binaryFileType)

        #Sign secure images
        if (util.isSecure(outfileAppPath)):
            if keyFile:
                signUtil.signImage(outfileAppPath, keyFile)
            else:
                raise Exception("Key file not specified for secure image!")

        # Update crc32 field
        computeCRC32.computeCRC32(outfileAppPath)

        # Get bounds for the merged hex
        flashRange = str(hex(appImgStartAddr)) + ':' + str(hex(flash_limit))
        mergedBinLen = getMergeBinLen(flashRange)


        # Create IntelHex objects for each hex
        appHex = intelhex.IntelHex(app_path)
        stackHex = intelhex.IntelHex(stack_path)

        # Define boundaries for each hex
        appHex = appHex[appImgStartAddr:app_boundary+1]
        stackHex = stackHex[stack_boundary:flash_limit+1]

        # Merge hex's
        inputHexFiles = [appHex, stackHex]
        mergedHex = intelhex.IntelHex()
        for hexFile in inputHexFiles:
            try:
                mergedHex.merge(hexFile, overlap="replace")
            except:
                print("Fatal Error: FAILED merge due to overlap when merging")
                sys.exit(1)

        # Define boundaries for merged hex
        mergedHex = mergedHex[appImgStartAddr:flash_limit+1]

        # Write hex to file
        mergeHexFilePath = os.path.join(projdir, outputPath) + "_merged.hex"
        mergedHex.write_hex_file(mergeHexFilePath)

        # Convert hex to bin
        outFileMergedPath = os.path.join(projdir, outputPath) + "_merged.bin"
        intelhex.hex2bin(mergeHexFilePath, outFileMergedPath, start=appImgStartAddr, end=flash_limit)

        # Update fields that will have changed
        mergedImgEndAddr = getMergeBinEndAddr(flashRange)
        util.updateMergedImageLen(outFileMergedPath, mergedBinLen)
        util.updateFileType(outFileMergedPath, util.BIN_TYPE_COMBINED)

        #Sign secure images
        if (util.isSecure(outFileMergedPath)):
            if keyFile:
                signUtil.signImage(outFileMergedPath, keyFile)
            else:
                raise Exception("Key file not specified for secure image!")

        # Add crc32 bytes
        computeCRC32.computeCRC32(outFileMergedPath)


def main(args):
    secondHexFileName = ""
    argLen = len(sys.argv)
    binaryFileType = args.BinaryType

    if binaryFileType == util.BIN_TYPE_APP_ONLY or binaryFileType == util.BIN_TYPE_COMBINED:
        if args.HexPath2 != None:
            secondHexFileName = args.HexPath2
        else:
            raise Exception("Second hex file path and file name required!")
    elif binaryFileType == util.BIN_TYPE_STACK_ONLY:
        if args.bdFilePath == None:
            raise Exception("Boundary file path and file name required!")
    elif binaryFileType == util.BIN_TYPE_MERGED or binaryFileType == util.BIN_TYPE_PERSISTENT_APP:
        if args.HexPath1 == None or args.output == None:
            raise Exception("Missing input or output file")
    else:
        raise Exception("Invalid binary file type!")

    createAppStackBinfile(args.ProjType, args.ProjDir, binaryFileType,
                              args.HexPath1, args.bdFilePath, args.output,
                              args.keyFile, args.HexPath2)

    # Open the created binary file for header info extraction
    outfile = os.path.join(args.ProjDir, args.output)
    outfile = outfile + ".bin"

    if(args.verbose):
        with open(outfile, 'r+b') as filePtr:

            # Get the bin file length, to ensure we do not go out of bounds
            filePtr.seek(util.BIN_LEN_PTR, 0)
            imgLen = int.from_bytes(filePtr.read(util.IMG_LEN_SIZE), byteorder='little')

            # Make the image header into a tuple and print it
            filePtr.seek(0,0)
            initialHeader = filePtr.read(util.FIXED_HDR_LEN)
            headerTuple = namedtuple('OadImgHdr','imgID crc32 bimVer metaVer techType imgCpStat crcStat imgType imgNo imgVld len prgEntry softVer imgEndAddr hdrLen rfu2')
            metaVector = headerTuple._make(unpack(initialHeaderUnpack,initialHeader))
            print_initHeader(metaVector)

            # Read in all segments as tuples and print them
            currSeg = util.FIXED_HDR_LEN
            while(currSeg < imgLen):
                # Get the segment type
                filePtr.seek(currSeg, 0)
                nextSegment = filePtr.read(1)

                # Get the segment length
                filePtr.seek(util.SEG_LEN_OFFSET - 1, 1)
                segLen = int.from_bytes(filePtr.read(util.IMG_LEN_SIZE), byteorder='little')

                # Rewind to start of segment
                filePtr.seek(currSeg, 0)

                # Boundary Segment
                if(ord(nextSegment) == util.SEG_TYPE_BOUNDARY):
                    segData = filePtr.read(segLen)
                    headerTuple = namedtuple('Seg0Hdr','segType wirelessTech rfuSeg payloadLen stackEntryAddr iCallStackAddr ramStartAddr ramEndAddr')
                    metaVector = headerTuple._make(unpack(segment0Unpack,segData))
                    print_segInfo(metaVector)
                    currSeg += segLen

                # Contiguous Segment
                elif(ord(nextSegment) == util.SEG_TYPE_CONTIGUOUS):
                    # For an image segment, we don't want to read the entire payload
                    segData = filePtr.read(util.IMG_PAYLOAD_OFFSET)
                    headerTuple = namedtuple('Seg1Hdr','segType wirelessTech rfuSeg payloadLen startAddr')
                    metaVector = headerTuple._make(unpack(segment1Unpack,segData))
                    print_segInfo(metaVector)
                    currSeg += segLen

                # Security Segment
                elif(ord(nextSegment) == util.SEG_TYPE_SECURITY):
                    segData = filePtr.read(segLen)
                    headerTuple = namedtuple('Seg3Hdr','segType wirelessTech rfuSeg payloadLen secVer timeStamp signInfo signature1 signature2 signature3 signature4 signature5 signature6 signature7 signature8')
                    metaVector = headerTuple._make(unpack(segment3Unpack, segData))
                    print_segInfo(metaVector)
                    currSeg += segLen

                # Delta Image Segment
                elif(ord(nextSegment) == util.SEG_TYPE_DELTA):
                    # Segment information is printed in the Turbo OAD Image Tool
                    currSeg += segLen

                else:
                    print ('invalid segment found')
                    break


def parse_args():
    script_description = "OAD Hex to Binary Converter version"
    parser = argparse.ArgumentParser(prog='oad_image_tool',
                                     description=script_description)
    parser.add_argument("-v", "--version", action="version",
                        version="%(prog)s " + __version__)
    parser.add_argument("-verbose", "--verbose", action='store_true')
    parser.add_argument("ProjType", help="Target project Type: (iar/ccs)")
    parser.add_argument("ProjDir", help="Target project directory")
    parser.add_argument("BinaryType", type=int, help="Persistent App = 0, "
                                                     "Application = 1, "
                                                     "Stack = 2, "
                                                     "Application + Stack Combined = 3, "
                                                     "Application + Stack Merged = 7")
    parser.add_argument("-hex1","--HexPath1", help="First .hex file path (app/stack)")
    parser.add_argument("-bdf" ,"--bdFilePath", help="Boundary file path")
    parser.add_argument("-hex2","--HexPath2", nargs='?',
                        help="Second .hex file name (app/stack)")
    parser.add_argument("-k", "--keyFile" ,nargs='?',
                            help="Provide the location of the file containing your private key")
    parser.add_argument("-o", "--output", required=True,
                        help="Path and filename of output file(s) without "
                             "extension")
    args = parser.parse_args()

    return args



## print_initHeader
## metaVector - a struct  containing the data read in from an OAD header
## This function prints out basic information contained in an OAD image header
def print_initHeader(metaVector):
    hdr = metaVector._asdict()
    print(textwrap.dedent("""
    ____________________________
           OAD IMG HDR
    ____________________________
    Field       |    Value
    imgID       |    {imgID}
    CRC         |    0x{crc32:X}
    bimVer      |    {bimVer}
    metaVer     |    {metaVer}
    techType    |    0x{techType:X}
    imgCpStat   |    0x{imgCpStat:X}
    crcStat     |    0x{crcStat:X}
    imgType     |    0x{imgType:X}
    imgNo       |    0x{imgNo:X}
    imgVld      |    0x{imgVld:X}
    len         |    0x{len:X}
    prgEntry    |    0x{prgEntry:X}
    softVer     |    0x{softVer:X}
    imgEndAddr  |    0x{imgEndAddr:X}
    hdrLen      |    0x{hdrLen:X}
        """.format(**hdr)))


## print_segInfo
## metaVecotr - a struct  containing the data read in from an OAD header
## This function prints out information based on detected sgements contained in an OAD image header
def print_segInfo(metaVector):
    hdr = metaVector._asdict()
    #Checks the the wireless tech type
    techValue = hdr['wirelessTech']
    hdr['wirelessTech'] = ''
    if(techValue==0):
        hdr['wirelessTech'] = 'No Wireless Technology'
    if(techValue&1==0):
        hdr['wirelessTech'] += '[BLE]'
    if(techValue&2==0):
        hdr['wirelessTech'] += '[TIMAC Sub1G]'
    if(techValue&4==0):
        hdr['wirelessTech'] += '[TIMAC 2.4G]'
    if(techValue&8==0):
        hdr['wirelessTech'] += '[Zigbee]'
    if(techValue&16==0):
        hdr['wirelessTech'] += '[RF4CE]'
    if(techValue&32==0):
        hdr['wirelessTech'] += '[Thread]'
    if(techValue&64==0):
        hdr['wirelessTech'] += '[EasyLink]'
    if(techValue&128==0):
        hdr['wirelessTech'] += '[MIOTY]'
    if(techValue&256==0):
        hdr['wirelessTech'] += '[WBMS]'
    #Prints out the corresponding segment type
    if(hdr['segType']==0):
        print(textwrap.dedent("""
   ____________________________
           Segment HDR
   ____________________________
   Field             |    Value
   Segment Type      |    Boundary Segment
   Wireless Tech     |    {wirelessTech}
   Segment Length    |    0x{payloadLen:X}
   STACK ENTRY ADDR  |    0x{stackEntryAddr:X}
   ICALL STACK ADDR  |    0x{iCallStackAddr:X}
   RAM START ADDR    |    0x{ramStartAddr:X}
   RAM END ADDR      |    0x{ramEndAddr:X}
        """.format(**hdr)))
    elif(hdr['segType']==1):
        print(textwrap.dedent("""
   ____________________________
           Segment HDR
   ____________________________
   Field            |      Value
   Segment Type     |      Contiguous Image Segment
   Wireless Tech    |      {wirelessTech}
   Segment Length   |      0x{payloadLen:X}
   Start Address    |      0x{startAddr:X}
        """.format(**hdr)))
    elif(hdr['segType']==3):
        print(textwrap.dedent("""
    ____________________________
           Segment HDR
    ____________________________
    Field            |      Value
    Segment Type     |      Security Segment
    Wireless Tech    |      {wirelessTech}
    Segment Length   |      0x{payloadLen:X}
    Security Version |      0x{secVer:X}
    Timestamp        |      0x{timeStamp:X}
    Signer Info      |      0x{signInfo:X}
    Signature        |      0x{signature1:X}{signature2:X}{signature3:X}{signature4:X}{signature5:X}{signature6:X}{signature7:X}{signature8:X}
        """.format(**hdr)))
    else:
        print ('invalid print command')

if __name__ == '__main__':
    oad_args = parse_args()
    main(oad_args)
