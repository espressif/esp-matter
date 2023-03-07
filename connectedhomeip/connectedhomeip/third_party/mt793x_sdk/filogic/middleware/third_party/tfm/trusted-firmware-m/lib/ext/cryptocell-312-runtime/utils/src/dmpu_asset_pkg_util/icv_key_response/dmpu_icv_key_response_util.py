#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


# This utility enables the ICV to build the encrypted OEM temporary key

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
UTILITY_LIB_Name = SBU_CRYPTO_LIB_DIR + "/" + "lib_icv_key_response.so"

# Parse given test configuration file and return test attributes as dictionary
def parse_config_file (config, log_file):
    local_dict = {}
    section_name = "DMPU-ICV-KEY-RES-CFG"
    if not config.has_section(section_name):
        log_sync(log_file, "section " + section_name + " wasn't found in cfg file\n")
        return None

    local_dict['oem_cert_pkg'] = config.get(section_name, 'oem-cert-pkg')
    log_sync(log_file,"oem-cert-pkg: " + str(local_dict['oem_cert_pkg']) + "\n")
     
    local_dict['key_filename'] = config.get(section_name, 'key-filename')
    log_sync(log_file,"key-filename: " + str(local_dict['key_filename']) + "\n")
     
    if config.has_option(section_name, 'keypwd-filename'): #used for testing
            local_dict['keypwd_filename'] = str.encode(config.get(section_name, 'keypwd-filename'))
            log_sync(log_file,"keypwd-filename: " + str(local_dict['keypwd_filename']) + "\n")
    else:
        local_dict['keypwd_filename'] = ''

    local_dict['pkg_filename'] = str.encode(config.get(section_name, 'icv-enc-oem-key'))
    log_sync(log_file,"icv-enc-oem-key: " + str(local_dict['pkg_filename']) + "\n")

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
    print_and_log(log_file, str(datetime.now()) + ": ICV key response Utility started (Logging to " + log_fname + ")\n")

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
        cert_size, certStr = GetDataFromBinFile(log_file, data_dict['oem_cert_pkg'])
        if (cert_size != OEM_KEY_REQ_CERT_SIZE) :
                print_and_log(log_file, "invalid certificate size " + str(cert_size) +" \n")
                exit_main_func(log_file, config_file, 1)

        key_size, keyStr = GetDataFromBinFile(log_file, data_dict['key_filename'])
        if (key_size != KRTL_SIZE*2) :
                print_and_log(log_file, "invalid key size  \n")
                exit_main_func(log_file, config_file, 1)

        print_and_log(log_file, "**** Generate OEM key package ****\n")        

        result = DLLHandle.generateIcvKeyRespPkg(keyStr, key_size, data_dict['keypwd_filename'],
                          certStr, cert_size,
                         data_dict['pkg_filename'])
        if result != 0:
            raise NameError
       
        print_and_log(log_file, "**** ICV key response utility completed successfully ****\n")
        exit_main_func(log_file, config_file, 0)

    else:
        print_and_log(log_file, "**** Invalid config file ****\n")
        exit_main_func(log_file, config_file, 1)

    FreeDLLGetHandle(DLLHandle)

#############################
if __name__ == "__main__":
    main()



