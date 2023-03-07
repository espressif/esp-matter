#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#



# This utility builds enabler debug certifiacte package:
# the package Header format is:
#                       key certificate, if exists
#                       token, version, length, flags (hbk,lcs, isRma)
#                       certifiacte PubKey (pub key + Np)
#                       debug Mask(4 words)
#                       debug lock(4 words)
#                       sha256 of devPubKey 
#                       certSign

import configparser
import sys
import os 


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


from cert_cfg_parser_util import *
from cert_dbg_util_gen import *
from global_defines import *
from cert_basic_utilities import *
from cert_dbg_util_data import *

# this is the path of the proj config file
PROJ_CFG_PATH = "src" + path_div
PROJ_CONFIG = CURRENT_PATH + path_div + ".." + path_div + PROJ_CFG_PATH + 'proj.cfg'


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
        log_fname = "sb_dbg1_cert.log"
    return config_fname, log_fname

def CreateCertUtility(sysArgsList):
    try:          

        config_fname, log_fname = parse_shell_arguments()
        log_file = create_log_file(log_fname)
        print_and_log(log_file, str(datetime.now()) + ": Enabler Debug Certificate Utility started (Logging to " + log_fname + ")\n")
    
        
        data_dict, config = enabler_cert_config_file_parser(config_fname, log_file)
        if data_dict == None:
            log_file.close()
            exit(1) 
            
        DLLHandle = LoadDLLGetHandle()        
    
        certStrBin = str()
        dataToSign = str()
        if data_dict['key_cert_pkg'] != '':
            isKeyExist = 1
        else:
            isKeyExist = 0
        print_and_log(log_file, "**** Generate debug certificate ****\n")        
        # if key package exists need to insert it into the enabler certificate 
        if data_dict['key_cert_pkg'] != "":
            keyStr = GetDataFromBinFile(log_file, data_dict['key_cert_pkg'])
            certStrBin = byte2string(keyStr)
    
        # create certificate header , get bin str and the header str
        dataToSign = build_certificate_header (DEBUG_ENABLER_TOKEN, PrjDefines, LIST_OF_CONF_PARAMS, data_dict['rma_mode'], data_dict['hbk_id'], data_dict['lcs'])
        
        # get the enabler certificate public key + Np
        enablerNPublicKey = GetRSAKeyParams(log_file, data_dict['cert_keypair'], data_dict['cert_keypair_pwd'], DLLHandle)
        dataToSign = dataToSign + enablerNPublicKey.VarsToBinString()        
    
        # add mask
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_mask0']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_mask1']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_mask2']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_mask3']))
    
        # add lock
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_lock0']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_lock1']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_lock2']))
        dataToSign = dataToSign + byte2string(struct.pack('<I', data_dict['debug_lock3']))
    
        # get developer certificate public key + Np (from public key)
        HashBootKey = GetPubKeyHash(log_file, data_dict['next_cert_pubkey'], DLLHandle);
        dataToSign = dataToSign + HashBootKey.VarsToBinString()
    
        # Sign on certificate
        Signature = GetRSASignature(log_file, dataToSign, data_dict['cert_keypair'], data_dict['cert_keypair_pwd'], DLLHandle)
    
        certStrBin = certStrBin + dataToSign + Signature.VarsToBinString()
    
        # add signature and write to binary file
        CreateCertBinFile(log_file, certStrBin, data_dict['cert_pkg'])
    
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

