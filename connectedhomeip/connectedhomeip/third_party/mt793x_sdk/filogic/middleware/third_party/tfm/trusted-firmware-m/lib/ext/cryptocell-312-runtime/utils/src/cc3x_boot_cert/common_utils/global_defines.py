####################################################################
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#




# Filename - globaldefines.py
# Description - This file contains global defines used in the secure
#               boot utility
####################################################################
from datetime import datetime
#
import sys
import re
import struct
from ctypes import *

BYTES_WITHIN_WORD = 4
PUBKEY_SIZE_BYTES = 384 #256 aligned with SB_CERT_RSA_KEY_SIZE_IN_BITS defined in cc_pka_hw_plat_defs.h
PUBKEY_SIZE_WORDS = (PUBKEY_SIZE_BYTES//BYTES_WITHIN_WORD)
NP_SIZE_IN_BYTES = 20
NP_SIZE_IN_WORDS = (NP_SIZE_IN_BYTES//BYTES_WITHIN_WORD)
SHA_256_HASH_SIZE_IN_BYTES = 32
RSA_SIGNATURE_SIZE_BYTES = 384 #256 aligned with SB_CERT_RSA_KEY_SIZE_IN_BITS defined in cc_pka_hw_plat_defs.h

CC_MNG_CHIP_MANUFACTURE_LCS = 0x0
CC_MNG_DEVICE_MANUFACTURE_LCS = 0x1
CC_MNG_SECURE_LCS = 0x5
CC_MNG_RMA_LCS = 0x7

#so name
SBU_CRYPTO_LIB_DIR = "lib"
SBU_CRYPTO_LIB_Name = SBU_CRYPTO_LIB_DIR + "/" + "libsbu_crypto.so"
SBU_OSSL_CRYPTO_LIB_Name = "libcrypto.so.1.0.0"
SBU_OSSL_LIB_Name = "libssl.so.1.0.0"


CURRENT_PATH = sys.path[0]


# DX default magic number for the certificate table
KEY_CERT_TOKEN = 0x53426b63
CONTENT_CERT_TOKEN = 0x53426363
DEBUG_ENABLER_TOKEN = 0x5364656E
DEBUG_DEVELOPER_TOKEN = 0x53646465

#E value, in a string mode. for the HASh calculation
E_VALUE_REVERSED = "00010001"
# Memory unload flag
MEM_ADDRESS_UNLOAD_FLAG = 0xFFFFFFFF

# RSA Modulus size = RSA signature size (RSA 3072 key size is used)
RSA_PRIVATE_KEY_SIZE = 3072

RSA_SIGNATURE_SIZE_IN_BYTES = 384
RSA_SIGNATURE_SIZE_IN_DOUBLE_BYTES = 768
RSA_SIGNATURE_SIZE_IN_WORDS = 96
# HASH size in SHA256 in bytes
SHA_256_HASH_SIZE_IN_BYTES = 32
HASH_ALGORITHM_SHA256_SIZE_IN_WORDS = (SHA_256_HASH_SIZE_IN_BYTES//BYTES_WITHIN_WORD)

# H size
RSA_H_SIZE_IN_BYTES = RSA_SIGNATURE_SIZE_IN_BYTES
RSA_H_SIZE_IN_WORDS = (RSA_SIGNATURE_SIZE_IN_BYTES//BYTES_WITHIN_WORD)
 
# Size of SW versions
SW_VERSION_OBJ_SIZE_IN_WORDS = 1

# header size in bytes
HEADER_SIZE_IN_BYTES = (4*BYTES_WITHIN_WORD)
HEADER_SIZE_IN_WORDS = (HEADER_SIZE_IN_BYTES//BYTES_WITHIN_WORD)
#size number of bytes in address
NUM_OF_BYTES_IN_ADDRESS = 4

SOC_ID_SIZE_IN_BYTES = 32

#enabler certificate flag offset
HBK_ID_FLAG_BIT_OFFSET = 0
LCS_ID_FLAG_BIT_OFFSET = 4
RMA_CERT_FLAG_BIT_OFFSET = 8


# HASH output representation
HASH_BINARY_REPRESENTATION = 1
HASH_HEX_REPRESENTATION    = 2 


# certificate output file prefix
Cert_FileName = "Cert"
# certificate output file suffix
Cert_FileExtBin = ".bin"
Cert_FileExtTxt = ".txt"
        
# definitions for code encryption
AES_IV_SIZE_IN_BYTES = 16
AES_DECRYPT_KEY_SIZE_IN_BYTES = 16
SW_COMP_FILE_NAME_POSTFIX = "_enc.bin"

NONCE_SIZE_IN_WORDS = 2
MAX_NUM_OF_IMAGES = 16

# Definitions for sw version legal values - 
#########################################
SW_REVOCATION_MAX_NUM_OF_BITS_HBK0 = 64
SW_REVOCATION_MAX_NUM_OF_BITS_HBK1 = 96
SW_REVOCATION_MAX_NUM_OF_BITS_HBK2 = 160

SW_REC_ADDR32_SIGNED_DATA_SIZE_IN_WORDS = 3

USE_AES_CE_ID_NONE = 0
USE_AES_CE_ID_KCEICV = 1
USE_AES_CE_ID_KCE = 2

LOAD_AND_VERIFY_IMAGE = 0
VERIFY_IMAGE_IN_FLASH = 1
VERIFY_IMAGE_IN_MEM = 2
LOADING_ONLY_IMAGE = 3


# Definitions for list of configurables parameters
##################################################
LIST_OF_CONF_PARAMS = ["CERT_VERSION_MAJOR","CERT_VERSION_MINOR"]
