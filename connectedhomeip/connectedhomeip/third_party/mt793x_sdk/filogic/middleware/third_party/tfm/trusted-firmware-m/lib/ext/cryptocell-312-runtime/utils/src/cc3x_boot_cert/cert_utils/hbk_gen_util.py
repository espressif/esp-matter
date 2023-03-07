#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#



import sys
import os 
from datetime import datetime

# Definitions for paths
#######################
if sys.platform != "win32" :
    path_div = "//"    
else : #platform = win32
    path_div = "\\"

CURRENT_PATH = sys.path[0]
# In case the scripts were run from current directory
CURRENT_PATH_SCRIPTS = path_div + 'common_utils'

# this is the scripts local path, from where the program was called
sys.path.append(CURRENT_PATH+CURRENT_PATH_SCRIPTS)

import string
from ctypes import *
from cert_basic_utilities import * 

OUTPUT_SIZE_SHA_256 = 32
OUTPUT_SIZE_SHA_256_TRUNC = int(OUTPUT_SIZE_SHA_256/2)
BITS_WITHIN_WORD = 32
BYTES_WITHIN_WORD = 4

# Prim key hash file name
PRIM_KEY_HASH_FILE_NAME = "prim_key_hash.txt"
ZEROS_NUM_FILES_NAME = "zero_bits_in_hash.txt" 

####################################################################
# Filename - hbk_gen_util.py
# Description - This file is responsible for creation of public key
#               HASH and its corresponding number of Zeroes
#               as it should be saved in the OTP/NVM
####################################################################

#################### Utility functions #############################

# The function creates words list out of a string that contains hex
# resrepesentation
def CreateWordList(buff, buffWordSize, endiannes):

    wordList = list()
    
    for i in range(int(buffWordSize)):
        word = 0
        if endiannes == 'L':
            #int(buff[i].encode('hex'),16)
            word = buff[i*4]
            word = word+(buff[i*4+1]<<8)
            word = word+(buff[i*4+2]<<16)
            word = word+(buff[i*4+3]<<24)
        else:
            word = buff[i*4+3]
            word = word+(buff[i*4+2]<<8)
            word = word+(buff[i*4+1]<<16)
            word = word+(buff[i*4]<<24)
        wordList.append(format(word, 'x'))

    return wordList    


####################################################################


# The function calculates the number of Zeroes for a given data
def CalculateZeroBitCountOnData(HASHwordList, size):    
#total number of zeros, start value
    zero_sum = int(0)
    
    for word in HASHwordList:
        if size == 0:
            break
        #1's bit count start value
        count = 0
        intWord = int(word, 16)        
        while (intWord):      
            count += (intWord & 1)
            intWord >>= 1                      
        zero_sum += (BITS_WITHIN_WORD - count)
        size -=1
    
    return zero_sum

      
# The function writes the output to output files 
# (a file for the HASH and a different file for the HBK num of Zeroes)
# File names are fixed
def WriteOutputToFile(HASHwordList, buffData, size, hash_file ,zeros_file):
    fobHash = open(hash_file, 'w')
    fobZeroNum = open(zeros_file, 'w')
    
    # The output is already set in the required endianess
    for i in range(size):
        if i != size - 1:
            # Unless it is the last word add comma
            fobHash.write(" 0x"+HASHwordList[i]+",")
        else:
            fobHash.write(" 0x"+HASHwordList[i] +" ")

    # Write the num of zeroes to output file    
    fobZeroNum.write(buffData)

    # Close files        
    fobHash.close()
    fobZeroNum.close()
    
    return


# This function prints the instructions of how to operate the utility
def usage():
    print("""Syntax: ./hbk_gen_util.py -key <RSA public-key file name> -endian <optional endianness> -hash_format <optional hash_format> \n
hbk_gen_util.py creates HASH over concatenation of N (public key) and Np (Barrett N' value) and calculates number of zero bits over the HASH value.\n
Input arguments:\n
-key <RSA public key file name> - mandatory, RSA public key in PEM format\n
-endian <endianness, B||L> - optional, endianness flag, B for Big endian or L for little endian; default is little endian\n
-hash_format <[SHA256 | SHA256_TRUNC]> - optional, HASH algorithm identifier determines the hash public key size; default is SHA256\n
outputs:\n
"prim_key_hash.txt" - file containing primary key hash text file in word format\n
"zero_bits_in_hash.txt" - file containing number of zeros for primary key hash\n 
""")
    sys.exit(1)


# The function parses the input parameters and returns a list of the given parameters 
def parse_shell_arguments():
    # Input parameters are :
    # -key [RSA pubkey name] - mandatory, RSA public key name
    # -endian <endianness, B||L> - optional, endianness flag, B for Big endian or L for little endian
    # -hash_format <[SHA256 | SHA256_TRUNC]> - optional, HASH algorithm identifier determines the hash public key size

    sysArgsList = sys.argv

    # check if the key exist if not return error
    if "-key" in sysArgsList:
        key_fname = sysArgsList[sysArgsList.index("-key") + 1]
    else:
        print("-key [RSA public key file name] is mandatory")
        usage()
        sys.exit(1)

    # Read the endianity and set it in the project defines list 
    if "-endian" in sysArgsList:
        endianity = sysArgsList[sysArgsList.index("-endian") + 1]
        if (endianity != 'B' and endianity != 'L'):
            print("-endian <[B | L]> is illegal")
            usage()
            sys.exit(1)
    else:
        endianity = "L"

    # check if the hash_format exist; if not, set default to SHA256
    if "-hash_format" in sysArgsList:
        hbk_format = sysArgsList[sysArgsList.index("-hash_format") + 1]
        if (hbk_format != 'SHA256' and hbk_format != 'SHA256_TRUNC'):
            print("-hash_format <[SHA256 | SHA256_TRUNC]> is illegal")
            usage()
            sys.exit(1)
    else:
        hbk_format = "SHA256"
        
    if "-hash_out" in sysArgsList:
        hash_file =  sysArgsList[sysArgsList.index("-hash_out") + 1]
    else:
        hash_file = PRIM_KEY_HASH_FILE_NAME
        
    if "-zeros_out" in sysArgsList:
        zeros_file =  sysArgsList[sysArgsList.index("-zeros_out") + 1] 
    else:
        zeros_file = ZEROS_NUM_FILES_NAME
    
    return key_fname, endianity, hbk_format, hash_file, zeros_file

##########################################################################################################

# Main functin, responsible for creating HASH value over N | Np and and the corresponding number of Zeroes value.
# The function expects the following inputs :
# 1. RSA public key file name mandatory (key file expected to be in openSSL PEM format)
# 2. Output format flag (L||B) little or big endian (optional, if not given L - little endian will be used) 
# 3. hash_format (SHA256||SHA256_TRUNC) (optional, if not given HASH SHA256 size will be used)
# The output will be written to two files: PrimKeyHASH.txt & ZeroBitsInHASH.txt
def main():

    pubKeyFileName, endianness, hash_format ,hash_file ,zeros_file= parse_shell_arguments()
    log_file = create_log_file("gen_hbk_log.log")
    print_and_log(log_file, str(datetime.now()) + ": generate HBK Utility started (Logging to gen_hbk_log.log)\n")

    #print_and_log(log_file, "**** loading library ****\n ") 
    DLLHandle = LoadDLLGetHandle()        

    # decide on hash output size
    if (hash_format == 'SHA256'):
        outputSize = OUTPUT_SIZE_SHA_256  
    else:
        outputSize = OUTPUT_SIZE_SHA_256_TRUNC  

    outputWordSize = int(round(outputSize/BYTES_WITHIN_WORD))  

    print_and_log(log_file, "\nStep 1 calculate hash")
    IntArrayParam = c_ubyte * outputSize
    publicKeyHash = IntArrayParam()
    result = DLLHandle.SBU_GetHashOfNAndNpFromPubKey(str.encode(pubKeyFileName), byref(publicKeyHash), outputSize)
    if result != 0:
        print_and_log(log_file,  "unable to calc hash for " + str.encode(pubKeyFileName) + "\n")
        sys.exit(1)

    HASHWordsList = CreateWordList(publicKeyHash, outputWordSize, endianness)

    print_and_log(log_file, "\nStep 2 - Calculate num of zero bits over the HASH")
    # Calculate the num of zeroes on the data & write it to files
    WriteOutputToFile(HASHWordsList, str(hex(CalculateZeroBitCountOnData(HASHWordsList,outputWordSize))), outputWordSize,hash_file ,zeros_file)
    
    print_and_log(log_file, "\nFunction completed successfully")

    sys.exit(0)


#############################
if __name__ == "__main__":
    main()
    
    


