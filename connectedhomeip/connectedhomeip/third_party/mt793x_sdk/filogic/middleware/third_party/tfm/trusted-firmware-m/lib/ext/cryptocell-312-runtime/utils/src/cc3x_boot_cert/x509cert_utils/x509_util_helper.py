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
import re

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


from global_defines import *
from cert_basic_utilities import *

BYTES_WITHIN_WORD = 4

CC_X509_CERT_TYPE_KEY = 1
CC_X509_CERT_TYPE_CONTENT = 2
CC_X509_CERT_TYPE_ENABLER_DBG = 3
CC_X509_CERT_TYPE_DEVELOPER_DBG = 4

# Parse given test configuration file and return test attributes as dictionary
def x509_cert_config_file_parser (section_name, config, local_dict, log_file):

    if config.has_option(section_name, 'issuer-name'):
        local_dict['issuer_name'] = str.encode(config.get(section_name, 'issuer-name'))
        log_sync(log_file, "issuer_name: " + str(local_dict['issuer_name']) + "\n")
    else:
        local_dict['issuer_name'] = ""

    if config.has_option(section_name, 'subject-name'):
        local_dict['subject_name'] = str.encode(config.get(section_name, 'subject-name'))
        log_sync(log_file, "subject_name: " + str(local_dict['subject_name']) + "\n")
    else:
        local_dict['subject_name'] = ""

    if config.has_option(section_name, 'not-before'):
        local_dict['not_before'] = int(eval(config.get(section_name, 'not-before')))
        log_sync(log_file, "not_before: " + str(local_dict['not_before']) + " =" + config.get(section_name, 'not-before') + "\n")
    else:
        local_dict['not_before'] = int(0)

    if config.has_option(section_name, 'not-after'):
        local_dict['not_after'] = int(eval(config.get(section_name, 'not-after')))
        log_sync(log_file, "not_after: " + str(local_dict['not_after']) + " =" + config.get(section_name, 'not-after') + "\n")
    else:
        local_dict['not_after'] = int(0)

    if config.has_option(section_name, 'serial-num'):
        local_dict['serial_num'] = int(eval(config.get(section_name, 'serial-num')))
        log_sync(log_file, "serial-num: " + str(local_dict['serial_num']) + " =" + config.get(section_name, 'serial-num') + "\n")
    else:
        local_dict['serial_num'] = int(0)

    return local_dict


# Parse bianry file into byte buffer
def parse_bin_file(file_name):
    buff = ""
    buff_len = 0
    if file_name != "":
        try:            
            # Get the image buffer from the file.
            fob = open(file_name, "rb")
            fileData = fob.read()
            fob.close()

            # convert read data to list, and get its size
            buff_len = len(fileData)
            #print "buff_len for " + file_name + " is " + str(buff_len) + "\n"

            IntArrayParam = c_ubyte * buff_len
            buff = IntArrayParam()
            i = 0
            for obj in fileData:
                buff[i] = obj
                i = i+1

        except IOError as Error1:
            (errno, strerror) = Error1.args
            print("Error in opening file - %s" %FileName)
            sys.exit()

    return buff, buff_len
