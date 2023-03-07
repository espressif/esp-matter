#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#



# This file contains the general functions that are used in both certificates

import configparser
import sys
from global_defines import *
from cert_basic_utilities import *
from cert_dbg_util_data import *

# The function GetRSAKeyParams reads the key file (in PEM format) parse it and gets the public and private RSA key data.
# Set the N buffer (in binary representation) and calculates the Np (Barrett n' value).
def GetRSAKeyParams(logFile, RSAKeyFileName, PassphraseFileName, SBU_Crypto):
    publicKey = create_string_buffer(RSA_SIGNATURE_SIZE_BYTES + NP_SIZE_IN_BYTES)
    result = SBU_Crypto.SBU_GetNAndNpFromKeyPair(str.encode(RSAKeyFileName), str.encode(PassphraseFileName), publicKey)
    if result != 0:
        print_and_log (logFile, "Error in public key | Np")
        sys.exit(1)
      
    # Create the public key object and return it in binary format
    return CertNPublicKey(publicKey)
# End of GetRSAKeyParams

# The function GetRSAPubKeyParams reads the public  key file (in PEM format) parse it and gets the public RSA key data.
# Set the N buffer (in binary representation) and calculates the Np (Barrett n' value). the function returns string of N + Np
def GetRSAPubKeyParams(logFile, RSAKeyFileName, SBU_Crypto):
    publicKey = create_string_buffer(RSA_SIGNATURE_SIZE_BYTES + NP_SIZE_IN_BYTES)
    result = SBU_Crypto.SBU_GetNAndNpFromPubKey(str.encode(RSAKeyFileName), publicKey)    
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
        Signature = create_string_buffer(RSA_SIGNATURE_SIZE_BYTES)
        
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
    return CertRSASignature(ReverseBytesinBinString(Signature))
# End of GetRSASignature

# This class holds the secondary N HASH value
class CertPubKeyHASHData:

    # Constructor
    def __init__(self, HASHData):
        self.PubHashData = HASHData

    # The method returns the signature size
    def __len__(self):
        return len(self.PubHashData)

    # This method returns the binary signature
    
    def VarsToBinString(self):
        DataBinStr = str()        

        for i in range(SHA_256_HASH_SIZE_IN_BYTES): 
            byte = self.PubHashData[i]
            DataBinStr = DataBinStr + byte2string(byte)

        return DataBinStr

# End of CertPubKeyHASHData 

# The function GetPubKeyHash returns the hash of the (N|Np).
def GetPubKeyHash(logFile, RSAPubKeyFileName, CryptoDLL_handle):
    hashData = create_string_buffer(SHA_256_HASH_SIZE_IN_BYTES)
        
    result = CryptoDLL_handle.SBU_GetHashOfNAndNpFromPubKey(str.encode(RSAPubKeyFileName), hashData, SHA_256_HASH_SIZE_IN_BYTES)
    if result != 0:
        print_and_log (logFile, "Error in hash(public key | Np)")
        sys.exit(1)
      
    # Create the public key object and return it in binary format
    return CertPubKeyHASHData(hashData)
# End of GetPubKeyHash


