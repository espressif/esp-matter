#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# secure boot process. 
# Key certificate structure is :
#       FIELD NAME                                               SIZE (words)
#       ----------                                              ------------
#       Header token                                               1
#       version number                                             1
#       size in words (offset to signature)                        1
#       Flags                                                      1
#       N Pub key                                                 96
#       Np                                                         5
#       active SW version                                          1
#       public key HASH                                            8
#       RSA Signature                                             96


import sys
import os 
import struct


# Definitions for paths
#######################
if sys.platform != "win32" :
    path_div = "//"    
else : #platform = win32
    path_div = "\\"

# Adding the utility python scripts to the python PATH
CURRENT_PATH = sys.path[0]
# In case the scripts were run from current directory
CURRENT_PATH_SCRIPTS = path_div + 'common_utils'

    
# this is the scripts local path, from where the program was called
sys.path.append(CURRENT_PATH+CURRENT_PATH_SCRIPTS)


from cert_cfg_parser_util import *


# this is the path of the proj config file
PROJ_CFG_PATH = "src" + path_div
PROJ_CONFIG = CURRENT_PATH + path_div + ".." + path_div + PROJ_CFG_PATH + 'proj.cfg'


from key_data_structures import *
import string
from global_defines import *
from ctypes import *
import global_defines
import configparser
from cert_basic_utilities import *

####################################################################
# Filename - sb_key_util.py
# Description - This file contains the main functionality of the key 
#               certificate generation.
####################################################################


########### Certificate utility functions ##########################

# The GetSWVersion function returns CertSwVersion object with S/W version value
def GetSWVersion(logFile, swVersionVal):    
        
    CertSwVersionObj = CertSwVersion(swVersionVal)

    return CertSwVersionObj
# End of GetSWVersion

# The BinStrToList function takes a binary string and returns a list with HEX
# representation for the bytes
def BinStrToList(str1):    
    TempList = list()
    ConvList = list(str1.encode('iso-8859-1'))
    for i in range(len(str1)):
        TempList.append("0x%02x" % ConvList[i])
        
    return TempList
# End of BinStrToList

# The CreateCertBinFile opens a binary and text file and writes the certificate data into it 
def Create_CertBinFile(logFile, binStr, txtList, certFileName):
    try:
        # Open a binary file and write the data to it
        FileObj = open(certFileName, "wb")
        FileObj.write(bytes(binStr.encode('iso-8859-1')))
        FileObj.close()

        # Assemble the text file name (cert + number 1 for primary , 2 for secondary + .txt)
        certFileNameTxt = certFileName[:-4] + '_' + Cert_FileName + Cert_FileExtTxt
        # Open a text file and write the data into it, in lines of 4 bytes
        FileObj = open(certFileNameTxt, "w")
        
        NumOfChars = len(txtList)
        FileObj.write("char cert_bin_image[] = {\n")
        for i in range(NumOfChars):   
            FileObj.write(txtList[i])
            if i !=  NumOfChars-1:
                FileObj.write(',')
            if (i+1) % 4 == 0:
                FileObj.write('\n')
        FileObj.write("}")
        FileObj.close()    
    except IOError as Error7:
        (errno, strerror) = Error7.args
        print_and_log(logFile, "Error in openning file - %s" %certFileName)
        sys.exit(1)
    return       
# End of CreateCertBinFile	

def CreateWordsListFromBytesList(BytesList):
    # Create words in reverse order
    wordsList = list()
    length = len(BytesList)/4        
    for i in range(int(length)):
        tmpStr = str()
        for j in range(4):                
            byte = str()
            byte = BytesList[i*4 + 4 - j - 1]
            byte = byte[2:]
            tmpStr = tmpStr + byte
        tmpStr = '0x' + tmpStr   
        wordsList.append(tmpStr)
    return wordsList    
            

########### certificate creation - Utility functions End ###########


# Parse script parameters
def parse_shell_arguments ():
    len_arg =  len(sys.argv)
    if len_arg < 2:
        print("len " + str(len_arg) + " invalid. Usage:" + sys.argv[0] + "<test configuration file>\n")
        for i in range(1,len_arg):
            print("i " + str(i) + " arg " + sys.argv[i] + "\n")
        sys.exit(1)
    config_fname = sys.argv[1]
    if len_arg == 3:
        log_fname = sys.argv[2]
    else:
        log_fname = "sb_key_cert.log"
    return config_fname, log_fname


# The function analyzes the input files and creates a key certificate binary file to be used in the
# The function does the following steps:
# 1. Create the certificate header and add to list 
# 2. Create RSA public key parameters and add to list
# 3. Create SW version parameters and add to list
# 4. Add the next public key HASH
# 5. In a loop create binary string out of the certificate so far (header + public key + sw version + HASH)
# 6. Do RSA signature over the HASH value of the certificate so far
# 7. Build the end of the certificate
# 8. Write the certificate as binary and text string to file
#
# In case an error occurs the function throws exception and exits
#################################################################################

def CreateCertUtility(sysArgsList):        
    try:          
        config_fname, log_fname =  parse_shell_arguments()
             
        log_file = create_log_file(log_fname)             
        # Check the input parameters and save it to list
        ArgsDict, config = key_cert_config_file_parser(config_fname, log_file)
        if ArgsDict == None:
               log_file.close()
               exit(1) 

                
        print_and_log(log_file, "**** Creating Key certificate Table **** ")
        # Create the certificate objects and add it to a list
        CertDataList = list()
        DLLHandle = LoadDLLGetHandle()        
        
        print_and_log(log_file, "\n Prepare certificate header ")
        # Create the certificate header and add to list -> header includes 
        CertDataList.append(CertHeader(log_file, ArgsDict['hbk_id'], CERT_TYPE_KEY, 0, 0, 0, 0, PrjDefines))

        print_and_log(log_file, "\n Create RSA public key parameters to insert to the certificate")        
        # Create RSA key parameters and add to list (according to which Public key derivative is used)               
        RSAPubKey = GetRSAKeyParams(log_file, ArgsDict['cert_keypair'], ArgsDict['cert_keypair_pwd'], DLLHandle)        
        
        CertDataList.append(RSAPubKey)

        print_and_log(log_file, "\n Get SW version parameters")
        # Create SW version parameters and add to list
        CertDataList.append(GetSWVersion(log_file, ArgsDict['nvcounter_val']))
        
        # Add HASH of next certificate public key 
        print_and_log(log_file, "\n Create HASH of public key and Np of the next certificate")
        CertDataList.append(GetPubKeyHash(log_file, ArgsDict['next_cert_pubkey'], DLLHandle))
        
        print_and_log(log_file, "\n Create the certificate as binary string and calculate RSA signature on it")
        # In a loop create binary string out of the certificate so far (header + public key + sw version + pub key HASH)
        BinStr = str()
        for obj in CertDataList:                        
            BinStr = BinStr + obj.VarsToBinString()

        # Do RSA signature  
        Signature = GetRSASignature(log_file, BinStr, ArgsDict['cert_keypair'], ArgsDict['cert_keypair_pwd'], DLLHandle)
        
        print_and_log(log_file, "\n Add the signature to the certificate ")
        # Build the end of the certificate - add the signature
        
        BinStr = BinStr + Signature.VarsToBinString()
        
        print_and_log(log_file, "\n Write the certificate to file ")
        # Write binary and text string to file    
        Create_CertBinFile(log_file, BinStr, BinStrToList(BinStr), ArgsDict['cert_pkg']) 
         
        print_and_log(log_file, "\n**** Certificate file creation has been completed successfully ****")
      
        
    except IOError as Error8: 
        (errno, strerror) = Error8.args 
        print_and_log(log_file, "I/O error(%s): %s" % (errno, strerror))
        raise
    except NameError:
        print_and_log(log_file, "Unexpected error, exiting program")
        raise  # Debug info
    except ValueError:
        print_and_log(log_file, "Illegal variable type")
        raise # Debug info


##################################
#       Main function
##################################
        
if __name__ == "__main__":

    import sys
    if sys.version_info<(3,0,0):
        print("You need python 3.0 or later to run this script")
        exit(1)

    if "-cfg_file" in sys.argv:
        PROJ_CONFIG = sys.argv[sys.argv.index("-cfg_file") + 1]
    print("Config File  - %s\n" %PROJ_CONFIG)

    # Get the project configuration values
    PrjDefines = parseConfFile(PROJ_CONFIG,LIST_OF_CONF_PARAMS)
    
    CreateCertUtility(sys.argv)




    



######################################## END OF FILE ########################################

