#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


from datetime import datetime
#
import sys
import struct
from ctypes import *

#so name
SBU_CRYPTO_LIB_DIR = "lib"
SBU_CRYPTO_LIB_Name = SBU_CRYPTO_LIB_DIR + "/" + "lib_cmpuutil_crypto.so"
SBU_OSSL_CRYPTO_LIB_Name = "libcrypto.so.1.0.0"
SBU_OSSL_LIB_Name = "libssl.so.1.0.0"


CURRENT_PATH = sys.path[0]


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

# 
# The function loads the crypto DLL and returns its handle
def LoadDLLGetHandle():
    # Load the crypto libraries
    SBU_Crypto = cdll.LoadLibrary(GetDLLPath(SBU_CRYPTO_LIB_Name))
    return SBU_Crypto
# End of LoadDLLGetHandle

# The function free the DLL - // not being used in Linux
def FreeDLLGetHandle(DllHandle):
    # free the libraries
    cdll.FreeLibrary(DllHandle)
    return 0
# End of FreeDLLGetHandle


# Create a log file handle
def create_log_file (log_file_path):
    
    log_file = open(log_file_path, 'w')
    return log_file;

# Print (stdout) and output also to log file given text
def print_and_log (log_file, text):
    print (text)
    log_file.write(text)
    sys.stdout.flush()
    log_file.flush()


# Do synchronous print to appear immediately on the console
def print_sync (text):
    print (text)
    sys.stdout.flush()


# Do synchronous write to log file
def log_sync (log_file, text):
    log_file.write(text)
    log_file.flush()


# The GetDataFromBinFile gets the data from a binary file
def GetDataFromBinFile(logFile, fileName):
    binStr = str()
    try:
        # Open a binary file and write the data to it
        FileObj = open(fileName, "rb")
        binStr = FileObj.read()
        binSize = len(binStr)
        FileObj.close()

    except IOError as Error7:
        (errno, strerror) = Error7.args
        print_and_log(logFile, "Error in openning file - %s" %fileName)
        sys.exit(1)
    return binSize, binStr
