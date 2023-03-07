/******************************************************************************

 @file  ll_enc.h

 @brief This file contains the Link Layer (LL) types, constants,
        API's etc. for the Bluetooth Low Energy (BLE) Controller
        CCM encryption and decryption.

        This API is based on ULP BT LE D09R23.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_ENC_H
#define LL_ENC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "ll_common.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#define LL_ENC_TX_DIRECTION_MASTER   1
#define LL_ENC_TX_DIRECTION_SLAVE    0
#define LL_ENC_RX_DIRECTION_MASTER   0
#define LL_ENC_RX_DIRECTION_SLAVE    1

#define LL_ENC_DATA_BANK_MASK        0xFF7F

#define LL_ENC_TRUE_RAND_BUF_SIZE    ((LL_ENC_IV_LEN/2) + (LL_ENC_SKD_LEN/2))

// Generate Session Key using LTK for key and SKD for plaintext.
#define LL_ENC_GenerateSK            MAP_LL_ENC_AES128_Encrypt

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// ALT: Remove caching as CC26xx is fast enough.
extern uint8 cachedTRNGdata[];

/*******************************************************************************
 * Functions
 */

extern void  LL_ENC_Init( void );
extern void  LL_ENC_LoadKey( uint8 *key );
extern void  LL_ENC_EncryptMsg( uint8 *nonce, uint8 pktHdr, uint8 pktLen, uint8 *pBuf );
extern void  LL_ENC_DecryptMsg( uint8 *nonce, uint8 pktHdr, uint8 pktLen, uint8 *pBuf, uint8 *mic );

// Random Number Generation
extern uint8 LL_ENC_GeneratePseudoRandNum( void );
extern uint8 LL_ENC_GenerateTrueRandNum( uint8 *buf, uint8 len );
extern uint8 LL_ENC_GenerateDRBGRandNum( uint8 *buf, uint8 len );
extern void  LL_ENC_GenerateDRBGSeedNum();
extern uint8 LL_ENC_GenerateTRNGRandNum( uint8 *buf, uint8 len );

// CCM Encryption
extern void  LL_ENC_AES128_Encrypt( uint8 *key, uint8 *plaintext,  uint8 *ciphertext );
extern void  LL_ENC_AES128_Decrypt( uint8 *key, uint8 *ciphertext, uint8 *plaintext );
extern void  LL_ENC_ReverseBytes( uint8 *buf, uint8 len );
extern void  LL_ENC_GenDeviceSKD( uint8 *SKD );
extern void  LL_ENC_GenDeviceIV( uint8 *IV );
extern void  LL_ENC_GenerateNonce( uint32 pktCnt, uint8 direction, uint8 *nonce );
extern void  LL_ENC_Encrypt( llConnState_t *connPtr, uint8 pktHdr, uint8 pktLen, uint8 *pBuf );
extern uint8 LL_ENC_Decrypt( llConnState_t *connPtr, uint8 pktHdr, uint8 pktLen, uint8 *pBuf );

#ifdef __cplusplus
}
#endif

#endif /* LL_ENC_H */
