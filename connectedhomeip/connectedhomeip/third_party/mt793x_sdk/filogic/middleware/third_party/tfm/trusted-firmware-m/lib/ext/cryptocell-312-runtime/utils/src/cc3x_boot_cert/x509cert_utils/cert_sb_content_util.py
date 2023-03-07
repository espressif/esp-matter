#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#




# This utility builds enabler debug certificate package:
#       If key certificate exists it should be located before the enabler certificate
#       x509 header containing: version of certificate
#                               serial number
#                               certificate algorithm identifier of the issuer signature
#                               Certificate Issuer name
#                               Validity period
#                               Subject name
#       certificate public key: public key algorithm ID
#                               public key - 3072 bits
#       certificate extensions: ARM certificate header: token, 
#                                                       version, 
#                                                       length, 
#                                                       flags: HBK use, valid LCS, isRma
#                               Barret Tag of public key (Np)
#                               Enabler certificate body: SW version - 32 bit
#                                                         Nonce - 8 bytes
#                                                         Data list
#       certificate signature: certificate algorithm identifier of the issuer signature
#                              signature of (x509 header + certificate public key + certificate extensions) - 3072 bits
#


import configparser
import sys

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
from key_data_structures import *
from global_defines import *
from cnt_data_structures import *
from x509_util_helper import *

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
        log_fname = "sb_cnt_cert.log"
    return config_fname, log_fname

def exit_main_func(log_file, config_fname, rc):
    log_file.close()
    config_fname.close()
    sys.exit(rc)


def CreateCertUtility(sysArgsList):
    try:          

        config_fname, log_fname = parse_shell_arguments()
        log_file = create_log_file(log_fname)
        print_and_log(log_file, str(datetime.now()) + ": Content Certificate Utility started (Logging to " + log_fname + ")\n")
    
        
        data_dict, config = content_cert_config_file_parser(config_fname, log_file)
        if data_dict == None:
            log_file.close()
            exit(1) 
            
        DLLHandle = LoadDLLGetHandle()    
            
        data_dict = x509_cert_config_file_parser("CNT-CFG", config, data_dict, log_file)
        if data_dict == None:
            print_and_log(log_file, "**** x509_cert_config_file_parser failed ****\n")
            exit_main_func(log_file, config_fname, result)

        print_and_log(log_file, "**** calling SBU_x509_CreateAndSetHeader **** \n") 
        result = DLLHandle.SBU_x509_CreateAndSetHeader(int(CC_X509_CERT_TYPE_CONTENT)) 
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_CreateAndSetHeader failed ****\n")
            exit_main_func(log_file, config_fname, result)
        
        #building the certificate body
        keyNonce = KeyNonce(data_dict['aes_ce_id'], DLLHandle)  
        nonceStr = keyNonce.VarsToBinString()  
                      
        RecList = ImageFileAnalyzer(log_file, data_dict['images_table'], data_dict['load_verify_scheme'], data_dict['aes_ce_id'], DLLHandle, data_dict['aes_enc_key'], keyNonce.randStr, data_dict['crypto_type'])
   
        print_and_log(log_file, "**** calling SBU_x509_AddPropHeaderExtension **** \n") 
        propHeader = CertHeader(log_file, int('0xf',16), CERT_TYPE_CONTENT, data_dict['aes_ce_id'], data_dict['load_verify_scheme'], data_dict['crypto_type'], len(RecList), PrjDefines)
        headerStr = propHeader.VarsToBinString()
        result = DLLHandle.SBU_x509_AddPropHeaderExtension(headerStr.encode('iso-8859-1')) 
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_AddPropHeaderExtension failed ****\n")
            exit_main_func(log_file, config_fname, result)
    
        print_and_log(log_file, "**** calling SBU_x509_AddPubKeyNpBuffExtension **** \n") 
        result = DLLHandle.SBU_x509_AddPubKeyNpBuffExtension(str.encode(data_dict["cert_keypair"]), str.encode(data_dict["cert_keypair_pwd"])) 
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_AddPubKeyNpBuffExtension failed ****\n")
            exit_main_func(log_file, config_fname, result)

        BinStr = str()
        # In a loop add the component objects to the binary string (only if not empty)
        for obj in RecList:
            BinStr = BinStr + obj.VarsToBinStringHashComp()

        print_and_log(log_file, "**** calling SBU_x509_AddContentCertBodyExtension **** \n") 
        result = DLLHandle.SBU_x509_AddContentCertBodyExtension(data_dict['nvcounter_val'], nonceStr.encode('iso-8859-1'), BinStr.encode('iso-8859-1'), len(RecList))
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_AddContentCertBodyExtension failed ****\n")
            exit_main_func(log_file, config_fname, result)
        
        print_and_log(log_file, "**** calling SBU_x509_SetKeyAndSign **** \n") 
        result = DLLHandle.SBU_x509_SetKeyAndSign(str.encode(data_dict["cert_keypair"]), str.encode(data_dict["cert_keypair_pwd"]))
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_SetKeyAndSign failed ****\n")
            exit_main_func(log_file, config_fname, result)

        # Add the none-signed info data        
        BinStrNotSigned = str()
        for obj in RecList:
            BinStrNotSigned = BinStrNotSigned + obj.VarsToBinStringParamComp()

        print_and_log(log_file, "**** calling SBU_x509_BuildCertPkg **** \n") 
        #Non-signed table is after certificate
        result = DLLHandle.SBU_x509_BuildCertPkg(BinStrNotSigned.encode('iso-8859-1'), len(BinStrNotSigned), 0, str.encode(data_dict["cert-pkg"]))
        if result != 0:
            print_and_log(log_file, "**** SBU_x509_BuildCertPkg failed ****\n")
            exit_main_func(log_file, config_fname, result)
    
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

