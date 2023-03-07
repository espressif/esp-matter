#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


# This utility builds production asset  package:
# the package format is:
#                       token, version, asset length, user data (20 bytes)
#                       nonce(12 bytes)
#                       encrypted asset (up to 512 bytes - multiple of 16 bytes)
#                       aset tag (16 bytes)


# This file contains the general functions that are used in both certificates



import sys
# Definitions for paths
if sys.platform != "win32" :
    path_div = "//"    
else : #platform = win32
    path_div = "\\"

import configparser
from dmpu_util_helper import *
from dmpu_util_crypto_helper import *
import sys

UTILITY_LIB_DIR = "lib"
UTILITY_LIB_Name = SBU_CRYPTO_LIB_DIR + "/" + "lib_oem_asset_pkg.so"

# Parse given test configuration file and return test attributes as dictionary
def parse_config_file (config, log_file):
    local_dict = {}
    section_name = "DMPU-OEM-ASSET-CFG"
    if not config.has_section(section_name):
        log_sync(log_file, "section " + section_name + " wasn't found in cfg file\n")
        return None

    if config.get(section_name, 'asset-type') == "kce":
          local_dict['asset_type'] = int(ASSET_TYPE_ENC)
    elif config.get(section_name, 'asset-type') == "kcp":
          local_dict['asset_type'] = int(ASSET_TYPE_PROV)
    else:
        log_sync(log_file, "Illegal asset-type defined - exiting\n")
        return None
    log_sync(log_file,"asset-type: " + str(local_dict['asset_type']) + "\n")

    local_dict['icv_enc_oem_key'] = config.get(section_name, 'icv-enc-oem-key')
    log_sync(log_file,"icv-enc-oem-key: " + str(local_dict['icv_enc_oem_key']) + "\n")
     
    local_dict['oem_enc_keypair'] =  str.encode(config.get(section_name, 'oem-enc-keypair'))
    log_sync(log_file,"oem-enc-keypair: " + str(local_dict['oem_enc_keypair']) + "\n")
     
    if config.has_option(section_name, 'oem-enc-keypwd'): #used for testing
            local_dict['oem_enc_keypwd'] = str.encode(config.get(section_name, 'oem-enc-keypwd'))
            log_sync(log_file,"oem-enc-keypwd: " + str(local_dict['oem_enc_keypwd']) + "\n")
    else:
        local_dict['oem_enc_keypwd'] = ''

    local_dict['asset_filename'] = config.get(section_name, 'asset-filename')
    log_sync(log_file,"asset-filename: " + str(local_dict['asset_filename']) + "\n")

    local_dict['pkg_filename'] = str.encode(config.get(section_name, 'pkg-filename'))
    log_sync(log_file,"pkg-filename: " + str(local_dict['pkg_filename']) + "\n")

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
        log_fname = "asset_prov.log"
    return config_fname, log_fname

def main():

    config_fname, log_fname = parse_shell_arguments()
    log_file = create_log_file(log_fname)
    print_and_log(log_file, str(datetime.now()) + ": OEM Asset provisioning Utility started (Logging to " + log_fname + ")\n")

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
        # Get assets and encrypted key from files
        asset_size, assetStr = GetDataFromBinFile(log_file, data_dict['asset_filename'])
        if (asset_size != ASSET_SIZE) :
                print_and_log(log_file, "invalid asset size \n")
                exit_main_func(log_file, config_file, 1)

        encKey_size, encKey_Str = GetDataFromBinFile(log_file, data_dict['icv_enc_oem_key'])
        if (encKey_size != PUBKEY_SIZE_BYTES) :
                print_and_log(log_file, "invalid key size \n")
                exit_main_func(log_file, config_file, 1)


        print_and_log(log_file, "**** Generate DMPU Asset package ****\n")        

        result = DLLHandle.build_oem_asset_pkg(data_dict['oem_enc_keypair'], data_dict['oem_enc_keypwd'],
                          encKey_Str, encKey_size,
                         data_dict['asset_type'],
                         assetStr, asset_size,
                         data_dict['pkg_filename'])
        if result != 0:
            raise NameError
       
        print_and_log(log_file, "**** Generate OEM asset package completed successfully ****\n")
        exit_main_func(log_file, config_file, 0)

    else:
        print_and_log(log_file, "**** Invalid config file ****\n")
        exit_main_func(log_file, config_file, 1)

    FreeDLLGetHandle(DLLHandle)

#############################
if __name__ == "__main__":
    main()



