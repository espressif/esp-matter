#!/usr/local/bin/python3
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#



# This file contains the general classes used in the code

import configparser
import sys
from cert_basic_utilities import *
from global_defines import *


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
        return RSA_SIGNATURE_SIZE_IN_BYTES

    # This method returns the binary signature
    def VarsToBinString(self):
        DataBinStr = self.SignatureStr
     
        return byte2string(DataBinStr)

# End of CertRSASignature

