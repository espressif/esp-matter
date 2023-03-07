#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

from ctypes import *
from dmpu_util_helper import *

# oem key request certificate struct is:
#  token || version || size || main N+Np || enc N + Np || signature
#   4           4              4       384+20            384+20           384
OEM_KEY_REQ_CERT_SIZE = 1204 
DMPU_CERT_HEADER_SIZE_IN_BYTES = 12     # token || version || size
DMPU_OEM_KEY_REQ_TOKEN = 0x52455144
DMPU_OEM_KEY_REQ_VERSION = 0x01

PUBKEY_SIZE_BYTES = 384     # 3072 bits
NP_SIZE_IN_BYTES = 20

KRTL_SIZE = 16


# This class represents N public key data
class CertNPublicKey:

    # Constructor
    # Np can stand either for Np or for H
    def __init__(self, PubKeyBinStr):
        self.PubKey = PubKeyBinStr

    # The method __len__ returns the size of pubkey N and Np (string size in bytes)
    def __len__(self):
        return(PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES)

    # This method returns a binary string of the N string and Np string (N is set as big endian
    # Np is set as little endian)   
    def VarsToBinString(self):
        DataBinStr = str()
        PubKey = self.PubKey

        for i in range(PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES): 
            byte = PubKey[i]
            DataBinStr = DataBinStr + byte2string(byte)

        return DataBinStr

# End of CertNPublicKey


# This class holds the RSA signature
class CertRSASignature:

    # Constructor
    def __init__(self, SignatureBinStr):
        self.SignatureStr = SignatureBinStr

    # The method returns the signature size
    def __len__(self):
        return PUBKEY_SIZE_BYTES

    # This method returns the binary signature
    def VarsToBinString(self):
        DataBinStr = str()
        SignatureStr = self.SignatureStr
        for i in range(PUBKEY_SIZE_BYTES): 
            byte = SignatureStr[i]
            DataBinStr = DataBinStr + byte2string(byte)

        return DataBinStr
     
# End of CertRSASignature

# The function GetRSAKeyParams reads the key file (in PEM format) parse it and gets the public and private RSA key data.
# Set the N buffer (in binary representation) and calculates the Np (Barrett n' value).
def GetRSAKeyParams(logFile, RSAKeyFileName, PassphraseFileName, crypto_lib):
    publicKey = create_string_buffer(PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES)
    result = crypto_lib.SBU_GetNAndNpFromKeyPair(str.encode(RSAKeyFileName), str.encode(PassphraseFileName), publicKey)
    if result != 0:
        print_and_log (logFile, "Error in public key | Np")
        sys.exit(1)
      
    # Create the public key object and return it in binary format
    return CertNPublicKey(publicKey)
# End of GetRSAKeyParams


# The function GetRSAPubKeyParams reads the public  key file (in PEM format) parse it and gets the public RSA key data.
# Set the N buffer (in binary representation) and calculates the Np (Barrett n' value). the function returns string of N + Np
def GetRSAPubKeyParams(logFile, RSAKeyFileName, crypto_lib):
    publicKey = create_string_buffer(PUBKEY_SIZE_BYTES + NP_SIZE_IN_BYTES)
    result = crypto_lib.SBU_GetNAndNpFromPubKey(str.encode(RSAKeyFileName), publicKey)    
    if result != 0:
        print_and_log (logFile, "Error in public key | Np")
        sys.exit(1)
      
    # Create the public key object and return it in binary format
    return CertNPublicKey(publicKey)
# End of GetRSAPubKeyParams

# The function GetRSASignature calculates the RSA signature
def GetRSASignature(logFile, DataIn, PrivKeyFile, PassphraseFile, CryptoDLL_handle):
    try:
        DataInSize = len(DataIn)
        Signature = create_string_buffer(PUBKEY_SIZE_BYTES)
        
        # Do Rsa Sign and get the signature
        # N, D and DataIn are sent to the function as int arrays     
        p1=str.encode(PrivKeyFile)
        p2=str.encode(PassphraseFile)
        p3=DataIn.encode('iso-8859-1')
        result = CryptoDLL_handle.SBU_RSA_Sign(1, p3, DataInSize, p1, p2, Signature)
        if result != 0:
            print_and_log(logFile, "\n SBU_CRYPTO_DLL.SBU_RSA_Sign returned an error !!")                
            raise NameError

    except NameError:        
        sys.exit(1)
    #return CertRSASignature(ReverseBytesinBinString(Signature))
    return CertRSASignature(Signature)
# End of GetRSASignature

