/******************************************************************************

 @file  mac_comdef.h

 @brief Type definitions and macros.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef MACCOMDEF_H
#define MACCOMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"
#include "saddr.h"


/*********************************************************************
 * CONSTANTS
 */

 /*** Return Values ***/
#define ZSUCCESS                          SUCCESS


#define MAC_NV_EX_LEGACY                   0x0000


// OSAL NV item IDs
#define MAC_NV_EXTADDR                     0x0001

#define MAC_NV_INITIALIZED                 0x0010

#define MAC_NV_COORD_EXTENDED_ADDRESS      0x0021
#define MAC_NV_COORD_SHORT_ADDRESS         0x0022
#define MAC_NV_PAN_ID                      0x0023
#define MAC_NV_RX_ON_WHEN_IDLE             0x0024
#define MAC_NV_SHORT_ADDRESS               0x0025
#define MAC_NV_SECURITYENABLED             0x0025
#define MAC_NV_LOGICALCHANNEL              0x0026
#define MAC_NV_EXTENDED_ADDRESS            0x0027
#define MAC_NV_LOGICAL_CHANNEL             0x0028
#define MAC_NV_ASSOCIATION_PERMIT          0x0029
#define MAC_NV_DEVICE_RECORD               0x002A
#define MAC_NV_DEVICE_RECORD_NUM           0x002B
#define MAC_NV_SRC_MATCH_ENABLE            0x002C

#define MAC_NV_PAN_COORD_SHORT_ADDRESS     0x0030
#define MAC_NV_PAN_COORD_EXTENDED_ADDRESS  0x0031
#define MAC_NV_DEVICE_TABLE_ENTRIES        0x0032
#define MAC_NV_KEY_TABLE_ENTRIES           0x0033

#define MAC_NV_DEVICE_TABLE                0x0040 //50 allocated for the same
#define MAC_NV_KEY_TABLE                   0x0050



/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MACCOMDEF_H */
