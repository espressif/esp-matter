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

ASSET_TYPE_ENC = 1
ASSET_TYPE_PROV = 2

ASSET_SIZE = 16
USER_DATA_SIZE = 16
KRTL_SIZE = 16

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
from cmpu_util_helper import *
import sys

# Parse given test configuration file and return test attributes as dictionary
def parse_config_file (config, log_file):
    local_dict = {}
    section_name = "CMPU-ASSET-CFG"
    if not config.has_section(section_name):
        log_sync(log_file, "section " + section_name + " wasn't found in cfg file\n")
        return None

    if config.get(section_name, 'asset-type') == "kpicv":
         local_dict['asset_type'] = int(ASSET_TYPE_PROV)
    elif config.get(section_name, 'asset-type') == "kceicv":
          local_dict['asset_type'] = int(ASSET_TYPE_ENC)
    else:
        log_sync(log_file, "Illegal asset-type defined - exiting\n")
        return None
    log_sync(log_file,"asset-type: " + str(local_dict['asset_type']) + "\n")

    local_dict['unique_data'] = config.get(section_name, 'unique-data')
    log_sync(log_file,"unique-data: " + str(local_dict['unique_data']) + "\n")
     
    local_dict['key_filename'] = config.get(section_name, 'key-filename')
    log_sync(log_file,"key-filename: " + str(local_dict['key_filename']) + "\n")
     
    if config.has_option(section_name, 'keypwd-filename'): #used for testing
            local_dict['keypwd_filename'] = str.encode(config.get(section_name, 'keypwd-filename'))
            log_sync(log_file,"keypwd-filename: " + str(local_dict['keypwd_filename']) + "\n")
    else:
        local_dict['keypwd_filename'] = ''

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


# close files and exit script
def exit_main_func(log_file, config_file, rc):
    log_file.close()
    config_file.close()
    sys.exit(rc)


def main():

    config_fname, log_fname = parse_shell_arguments()
    log_file = create_log_file(log_fname)
    print_and_log(log_file, str(datetime.now()) + ": Asset provisioning Utility started (Logging to " + log_fname + ")\n")

    DLLHandle = LoadDLLGetHandle()
    
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

        userData_size, userData_Str = GetDataFromBinFile(log_file, data_dict['unique_data'])
        if (userData_size < USER_DATA_SIZE) :
                print_and_log(log_file, "invalid key size \n")
                exit_main_func(log_file, config_file, 1)

        key_size, keyStr = GetDataFromBinFile(log_file, data_dict['key_filename'])
        if (key_size != KRTL_SIZE*2) :
                print_and_log(log_file, "invalid key size  \n")
                exit_main_func(log_file, config_file, 1)

        print_and_log(log_file, "**** Generate production Asset package ****\n")        

        result = DLLHandle.build_asset_pkg(keyStr, key_size, data_dict['keypwd_filename'],
                         data_dict['asset_type'],
                         userData_Str, int(USER_DATA_SIZE),
                         assetStr, int(ASSET_SIZE),
                         data_dict['pkg_filename'])
        if result != 0:
            raise NameError
       
        print_and_log(log_file, "**** Generate asset package completed successfully ****\n")
        exit_main_func(log_file, config_file, 0)

    else:
        print_and_log(log_file, "**** Invalid config file ****\n")
        exit_main_func(log_file, config_file, 1)

    FreeDLLGetHandle(DLLHandle)

#############################
if __name__ == "__main__":
    main()



