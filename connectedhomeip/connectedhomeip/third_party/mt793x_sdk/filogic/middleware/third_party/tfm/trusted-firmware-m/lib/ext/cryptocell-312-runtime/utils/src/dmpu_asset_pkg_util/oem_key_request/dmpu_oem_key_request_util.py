#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


# This utility builds key request certifiacte sent from OEM to ICV:
# the package Header format is:
#                       token, version, length, 
#                       certifiacte main PubKey (pub key)
#                       certifiacte encryption PubKey (pub key)
#                       certSign


import sys
# Definitions for paths
if sys.platform != "win32" :
    path_div = "//"    
else : #platform = win32
    path_div = "\\"


CURRENT_PATH = sys.path[0]
# In case the scripts were run from current directory
CURRENT_PATH_SCRIPTS = path_div
# this is the scripts local path, from where the program was called
sys.path.append(CURRENT_PATH+CURRENT_PATH_SCRIPTS)

import configparser
from dmpu_util_helper import *
from dmpu_util_crypto_helper import *
import sys

# Parse given test configuration file and return test attributes as dictionary
def parse_config_file (config, log_file):
    local_dict = {}
    section_name = "DMPU-OEM-KEY-REQ-CFG"
    if not config.has_section(section_name):
        log_sync(log_file, "section " + section_name + " wasn't found in cfg file\n")
        return None

    local_dict['main_keypair_filename'] = config.get(section_name, 'oem-main-keypair')
    log_sync(log_file,"oem-main-keypair: " + str(local_dict['main_keypair_filename']) + "\n")
     
    if config.has_option(section_name, 'oem-main-keypwd'): #used for testing
            local_dict['main_keypwd_filename'] = config.get(section_name, 'oem-main-keypwd')
            log_sync(log_file,"oem-main-keypwd: " + str(local_dict['main_keypwd_filename']) + "\n")
    else:
        local_dict['main_keypwd_filename'] = ''

    local_dict['enc_pubkey_filename'] = config.get(section_name, 'oem-enc-pubkey')
    log_sync(log_file,"oem-enc-pubkey: " + str(local_dict['enc_pubkey_filename']) + "\n")

    local_dict['cert_pkg_filename'] = str.encode(config.get(section_name, 'oem-cert-pkg'))
    log_sync(log_file,"oem-cert-pkg: " + str(local_dict['cert_pkg_filename']) + "\n")

    return local_dict


# Parse script parameters
def parse_shell_arguments ():
    len_arg =  len(sys.argv)
    if len_arg < 2:
        print_sync("len " + str(len_arg) + " invalid. Usage:" + sys.argv[0] + "<test configuration file>\n")
        for i in range(1,len_arg):
            print_sync("i " + str(i) + " arg " + sys.argv[i] + "\n")
        sys.exit(1)
    config_fname = sys.argv[1]
    if len_arg == 3:
        log_fname = sys.argv[2]
    else:
        log_fname = "key_request_cert.log"
    return config_fname, log_fname


def main():

    config_fname, log_fname = parse_shell_arguments()
    log_file = create_log_file(log_fname)
    print_and_log(log_file, str(datetime.now()) + ": OEM key request Utility started (Logging to " + log_fname + ")\n")

    UTILITY_LIB_Name = SBU_CRYPTO_LIB_DIR + "/" + "lib_oem_key_request.so"
    DLLHandle = LoadDLLGetHandle(UTILITY_LIB_Name)
    
    try:
        config_file = open(config_fname, 'r')
    except IOError as e:
        print_and_log(log_file,"Failed opening " + config_fname + " (" + e.strerror + ")\n")
        log_file.close()
        sys.exit(e.errno)

    config = configparser.ConfigParser()
    config.read(config_fname)
    data_dict = {}

    data_dict = parse_config_file(config, log_file)

    if (data_dict != None):
         certStrBin = str()
         dataToSign = str()
         print_and_log(log_file, "**** Generate OEM key requesting certificate ****\n")        

         token = DMPU_OEM_KEY_REQ_TOKEN
         certVersion = DMPU_OEM_KEY_REQ_VERSION
         # token || version || size || main public key + Np || enc public key + Np
         certLength = DMPU_CERT_HEADER_SIZE_IN_BYTES + PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES + PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES

         header = struct.pack('<I', token) + struct.pack('<I', certVersion) + struct.pack('<I', certLength)
         dataToSign = byte2string(header)


         # get the enabler certificate public key + Np
         mainNPublicKey = GetRSAKeyParams(log_file, data_dict['main_keypair_filename'], data_dict['main_keypwd_filename'], DLLHandle)
         dataToSign = dataToSign + mainNPublicKey.VarsToBinString()        

         # get encrypted  public key  (from public key)
         encNPublicKey = GetRSAPubKeyParams(log_file, data_dict['enc_pubkey_filename'], DLLHandle)
         dataToSign = dataToSign + encNPublicKey.VarsToBinString()

         # Do RSA signature  
         Signature = GetRSASignature(log_file, dataToSign, data_dict['main_keypair_filename'], data_dict['main_keypwd_filename'], DLLHandle)

         # Build the end of the certificate - add the signature       
         certStrBin = dataToSign + Signature.VarsToBinString()

         # add signature and write to binary file
         CreateCertBinFile(log_file, certStrBin, data_dict['cert_pkg_filename'])

         print_and_log(log_file, "**** Generate OEM key requesting successfully ****\n")
         exit_main_func(log_file, config_file, 0)
    else:
        print_and_log(log_file, "**** Invalid config file ****\n")
        exit_main_func(log_file, config_file, 1)

    FreeDLLGetHandle(DLLHandle)

#############################
if __name__ == "__main__":
    main()

