#****************************************************************
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


import sys, traceback
import os
import re
import global_defines
from global_defines import *
from ctypes import *
import binascii


CURRENT_PATH = sys.path[0]

####################################################################
# Filename - basic_utilites.py
# Description - This file contains basic utilities functions
####################################################################

# The function returns the path of the DLL - fixed path (relative to script path)
def GetDLLPath(FileName):
    
    path = str()    
    path = CURRENT_PATH    
    # split according to dir names
    if sys.platform != "win32" :
        path_div = "/"    
    else : #platform = win32
        path_div = "\\"
    
    path_new = path + path_div + ".." + path_div
        
    path_new = path_new + FileName        
    return path_new
# End of GetDLLPath
        
# The function loads the crypto DLL and returns its handle
def LoadDLLGetHandle():
    # Load the crypto libraries
    if sys.platform != "win32" : # Unix paths
        global_defines.SBU_LibName = SBU_CRYPTO_LIB_Name
        global_defines.SBU_OpenSSLCrypto_LibName = SBU_OSSL_CRYPTO_LIB_Name
        global_defines.SBU_OpenSSLSSL_LibName = SBU_OSSL_LIB_Name
    SBU_Crypto = cdll.LoadLibrary(GetDLLPath(global_defines.SBU_LibName))
    return SBU_Crypto
# End of LoadDLLGetHandle

# log functions
# Create a log file handle
def create_log_file (log_file_path):
    try:
        log_file = open(log_file_path, 'w')
    except IOError as Error7:
        (errno, strerror) = Error7.args
        print("Error in openning file - %s" %log_file_path)
        sys.exit(1)
    return log_file;

# Print (stdout) and output also to log file given text
def print_and_log (log_file, text):
    print(text)
    log_file.write(text)
    sys.stdout.flush()
    log_file.flush()

# Print (stdout) and output also to log file given text
def print_exception ():
    traceback.print_exc(file=sys.stdout) 

# Do synchronous write to log file
def log_sync (log_file, text):
        log_file.write(text)
        log_file.flush()

# Convert bytes to string
def byte2stringBytesArray(DataBinBytes):
    ResStr = str()
    for i in range(len(DataBinBytes)):        
        ResStr = ResStr + chr(DataBinBytes.raw[i])
    return ResStr

# The ReverseBytesinBinString function takes a binary string and reverse it
def ReverseBytesinBinString(binStr): 
    return binStr[::-1]
# End of ReverseBytesinBinString

# The ReverseBytesInString function reverse the bytes in a string and returns the reversed string
def ReverseBytesInString(str1, strSize):   
    str2 = str() 
    if isinstance(str1, str):
        TempStr = str1
    else:              
        TempStr = str1.decode("utf-8")
    for i in range(strSize//2):
        str2 = str2 + TempStr[strSize-i*2-2:strSize-i*2]
    return str2
# End of ReverseBytesInString

# The ReverseBytesInString function reverse the bytes in a string and returns the reversed string
def ReverseBytesInBytesArray(array, size):   

    reversedArray = c_ubyte * size
    revArray = reversedArray()
    for i in range(size//4):
        # reverse each word
        for j in range(4):
            revArray[4*i + j] = array[4*i + 3 - j]
    return revArray
# End of ReverseBytesInString


# Convert bytes to string
def byte2string (DataBinStr):
    if type(DataBinStr).__name__ == 'str' :
        return DataBinStr
    ResStr = str()
    for i in range(len(DataBinStr)) :
        ResStr = ResStr + chr(DataBinStr[i])
    return ResStr

# Do synchronous print to appear immediately on the console
def print_sync (text):
    print (text)
    sys.stdout.flush()




# The CreateCertBinFile opens a binary and text file and writes the certificate data into it 
def CreateCertBinFile(logFile, binStr, certFileName):
    try:
        # Open a binary file and write the data to it
        FileObj = open(certFileName, "wb")
        FileObj.write(bytes(binStr.encode('iso-8859-1')))
        FileObj.close()

    except IOError as Error7:
        (errno, strerror) = Error7.args
        print_and_log(logFile, "Error in openning file - %s" %certFileName)
        sys.exit(1)
    return       
# End of CreateCertBinFile            

# The GetDataFromBinFile gets the data from a binary file
def GetDataFromBinFile(logFile, certFileName):
    binStr = str()
    try:
        # Open a binary file and write the data to it
        FileObj = open(certFileName, "rb")
        binStr = FileObj.read()
        FileObj.close()

    except IOError as Error7:
        (errno, strerror) = Error7.args
        print_and_log(logFile, "Error in openning file - %s" %certFileName)
        sys.exit(1)
    return binStr     



# parse configuration file according to list of parameters
def parseConfFile(FileName, listOfDefines):
    try:
        fob = open(FileName,'r')
        lines = fob.readlines()
        valuesList = list()
        for line in lines:
            for define in listOfDefines:
                if re.match(define,line):
                    tmpList=line.split("=")
                    valuesList.append(int(tmpList[len(tmpList)-1],16))
        fob.close()
    except IOError as Error1:
        (errno, strerror) = Error1.args
        print("\n Error in openning file - %s" %FileName)
        sys.exit(1)
        
    return valuesList
# End of parseConfFile



