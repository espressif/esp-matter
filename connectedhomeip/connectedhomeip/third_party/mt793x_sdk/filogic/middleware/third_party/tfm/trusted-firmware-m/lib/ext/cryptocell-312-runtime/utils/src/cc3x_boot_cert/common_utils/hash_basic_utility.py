
#
# Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


import string
from global_defines import *
import hashlib

####################################################################
# Filename -    hashbasicutillity
# Description - This file contains the main functionality of the
#               secure boot utility. The utility creates a certificate
#               that is used in the secure boot process
####################################################################


########### Basic Utilities ###############################

# This function calculates HASH SHA256 on binary data and return the HASH result 
def HASH_SHA256(BinData, OutputRep):

    # Calculate SHA 256 on given binary data
    HashObj = hashlib.sha256()
    HashObj.update(BinData)

    if OutputRep == HASH_BINARY_REPRESENTATION:
        HashRes = HashObj.digest()
    else:
        HashRes = HashObj.hexdigest()
    
    return HashRes
# End of HASH_SHA256


######################################## END OF FILE ########################################
