#****************************************************************
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#



import struct
import math
from global_defines import *
from flags_global_defines import *
from cert_basic_utilities import *
from hash_basic_utility import *
from ctypes import *

####################################################################
# Filename - datastructures.py
# Description - This file contains the data structures used in the
#               SB utility
####################################################################

# This class represents the certificate header. The header contains the magic number, certificate version, certificate size and Flags
class CertHeader:
    flag = 0
    CertSize = 0
    MagicNum = 0
    CertVersion = 0
      
    # Constructor, creates the flag, size and initializes the magic number
    def __init__(self, log, hbkId, certType, codeEncId, loadVerifyScheme, cryptoType, numOfComps, prjDefines):
        self.PrjDefines = prjDefines
        self.CertHeaderFlagCreate(log, hbkId, codeEncId, loadVerifyScheme, cryptoType, numOfComps)
        self.CertHeaderLenCreate(log, numOfComps)
        self.CertVersion = ( (self.PrjDefines[LIST_OF_CONF_PARAMS.index("CERT_VERSION_MAJOR")]) << 16) | (self.PrjDefines[LIST_OF_CONF_PARAMS.index("CERT_VERSION_MINOR")])
        if certType == CERT_TYPE_CONTENT:
            self.MagicNum = CONTENT_CERT_TOKEN
        else:
            self.MagicNum = KEY_CERT_TOKEN
       
    # This method creates the flag word 
    def CertHeaderFlagCreate(self, log, hbkId, codeEncId, loadVerifyScheme, cryptoType, numOfComps):
        flag = 0
        # build the flag value, first add the hash algorithm
        flag = hbkId
                
        # Add is encrypted flag
        flag = flag| (codeEncId << CODE_ENCRYPTION_SUPPORT_BIT_POS)
   
        # Add load&verify field flag
        flag = flag| (loadVerifyScheme << LOAD_VERIFY_SCHEME_BIT_POS)

        # Add cryptoType field flag
        flag = flag| (cryptoType << CRYPTO_TYPE_BIT_POS)

        # Add num of images
        flag = flag | numOfComps << NUM_OF_SW_COMPS_BIT_POS 

        self.flag = flag
        return 

    # This method creates the certificate size
    def CertHeaderLenCreate(self, log, numOfComps):
        # Calculate the offset to the signature (the offset will be set in words) :
        # Header size = 4 words
        # RSA N = 96 words
        # RSA Np = 5 words 
        # sw version = 1 word                
        # HASH of pubkey = HASH size        
        # Set the number of components in the appropriate field

        if numOfComps > MAX_NUM_OF_IMAGES: # 16
            print_and_log(log, "\nIllegal number of s/w components !!")
            sys.exit()
   
        self.CertSize =  HEADER_SIZE_IN_WORDS + SW_VERSION_OBJ_SIZE_IN_WORDS + PUBKEY_SIZE_WORDS + NP_SIZE_IN_WORDS
        if numOfComps > 0: #this is a content certificate
            self.CertSize = self.CertSize + NONCE_SIZE_IN_WORDS + numOfComps * (HASH_ALGORITHM_SHA256_SIZE_IN_WORDS + SW_REC_ADDR32_SIGNED_DATA_SIZE_IN_WORDS)
        else: #this is a key certificate 
            self.CertSize = self.CertSize + HASH_ALGORITHM_SHA256_SIZE_IN_WORDS
        return

    # This method is used to return the header as binary string
    def VarsToBinString(self):
        DataBinStr = str()
        DataBinStr1 = str()
        DataBinStr2 = str()
        DataBinStr3 = str()
        
        DataBinStr = struct.pack('<I', self.MagicNum) 
        DataBinStr1 = struct.pack('<I', self.CertVersion) 
        DataBinStr2 = struct.pack('<I', int(self.CertSize))
        DataBinStr3 = struct.pack('<I', self.flag)
            
        DataBinStr =  DataBinStr +  DataBinStr1 + DataBinStr2 + DataBinStr3

        return byte2string(DataBinStr)

# End of class CertHeader


# This class represents SW version fields in the certificate
class CertSwVersion:
    SwCurrentVersion = 0    
    InternalNumOfObjects = 2 
    # Costructor
    def __init__(self, RecSwVersion=0):
        self.SwCurrentVersion = RecSwVersion

    # len returns the size of the object
    def __len__(self):
        return BYTES_WITHIN_WORD*InternalNumOfObjects
    
    # This method returns a binary string
    def VarsToBinString(self):
        DataBinStr = str()
        
        DataBinStr = struct.pack('<I', self.SwCurrentVersion) 
        
        return byte2string(DataBinStr)
# End of CertSwVersion   

# This class represents N public key data
class CertNPublicKey:

    # Constructor
    # Np can stand either for Np or for H
    def __init__(self, PubKeyBinStr):
        self.PubKey = PubKeyBinStr

    # The method __len__ returns the size of pubkey N and Np (string size in bytes)
    def __len__(self):
        return(RSA_SIGNATURE_SIZE_IN_BYTES + NP_SIZE_IN_BYTES)

    # This method returns a binary string of the N string and Np string (N is set as big endian
    # Np is set as little endian)   
    def VarsToBinString(self):
        DataBinStr = str()
        PubKey = self.PubKey

        for i in range(RSA_SIGNATURE_SIZE_IN_BYTES + NP_SIZE_IN_BYTES): 
            byte = PubKey[i]
            DataBinStr = DataBinStr + byte2string(byte)

        return DataBinStr

# End of CertNPublicKey

# This class represents N public key data
class CertNPublicKeyHData:

    # Constructor
    # Np can stand either for Np or for H
    def __init__(self, PubKeyBinStr, HStr):
        self.PubKey = PubKeyBinStr
        self.HStr = HStr

    # The method __len__ returns the size of pubkey N and H (string size in bytes)
    def __len__(self):
        return(RSA_SIGNATURE_SIZE_IN_BYTES + RSA_H_SIZE_IN_BYTES)

    # This method returns a binary string of the N string and Np string (N is set as big endian
    # Np is set as little endian)   
    def VarsToBinString(self):
        DataBinStr = str()
        DataBinStr1 = str()

        PubKey = self.PubKey
        for i in range(RSA_SIGNATURE_SIZE_IN_BYTES):            
            byte = PubKey[i]
            DataBinStr = DataBinStr +  byte2string(byte)         
            
        Hstr = self.HStr        
        for i in range(RSA_H_SIZE_IN_BYTES):  
            DataBinStr1 =  DataBinStr1 + chr(int(Hstr[i*2:i*2+2],16))
        
        
        DataBinStr = DataBinStr + DataBinStr1    
        return byte2string(DataBinStr)
# End of CertNPublicKeyHData

# This class holds the RSA signature
class CertRSASignature:

    # Constructor
    def __init__(self, SignatureBinStr):
        self.SignatureStr = SignatureBinStr

    # The method returns the signature size
    def __len__(self):
        return RSA_SIGNATURE_SIZE_IN_BYTES

    # This method returns the binary signature
    def VarsToBinString(self):
        DataBinStr = self.SignatureStr
     
        return byte2string(DataBinStr)

# End of CertRSASignature

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


