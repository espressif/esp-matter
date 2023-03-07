/******************************************************************************

 @file  ecc_rom.c

 @brief This is the implementation for the API to the ECC module built into
        ROM on the CC26xx.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated

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
 * INCLUDES
 */
#include "hal_types.h"
#include "ecc_rom.h"

/*********************************************************************
 * CONSTANTS
 */

// Low level types defined in ECC module.
#ifndef __LOWLEVELTYPES__
#define __LOWLEVELTYPES__
#ifndef UTYPES
#define UTYPES
  typedef unsigned long long int  u64;
  typedef unsigned long int       u32;
  typedef unsigned short int      u16;
  typedef unsigned char           u8;
#endif
#endif

#ifdef ECC_PRIME_NIST256_CURVE
//#define TEST_NIST256
//#define PARAM_P NIST256_p;
#define PARAM_P 0x10018b0c;

//#define PARAM_R NIST256_r;
#define PARAM_R 0x10018b30;

//#define PARAM_A NIST256_a;
#define PARAM_A 0x10018b54;

//#define PARAM_B NIST256_b;
#define PARAM_B 0x10018b78;

//#define PARAM_GX NIST256_Gx;
#define PARAM_GX 0x10018b9c;

//#define PARAM_GY NIST256_Gy;
#define PARAM_GY 0x10018bc0;

#endif

/*********************************************************************
 * TYPEDEFS
 */
typedef u8 (* ecc_keygen_t)(u32 *, u32 *,u32 *, u32 *);
ecc_keygen_t ecc_generatekey = (ecc_keygen_t)(0x10017dbd);

typedef u8 (* ecdsa_sign_t)(u32 *, u32 *,u32 *, u32 *, u32 *);
ecdsa_sign_t ecc_ecdsa_sign = (ecdsa_sign_t)(0x10017969);

typedef u8 (* ecdsa_verify_t)(u32 *, u32 *,u32 *, u32 *, u32 *);
ecdsa_verify_t ecc_ecdsa_verify = (ecdsa_verify_t)(0x10017b01);

typedef u8 (* ecdh_computeSharedSecret_t)(u32 *, u32 *,u32 *, u32 *, u32 *);
ecdh_computeSharedSecret_t ecdh_computeSharedSecret = (ecdh_computeSharedSecret_t)(0x10017ded);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 * Function: ECC_initialize
 *
 * Description: pass pointer to ECC memory allocation to ECC engine.
 *
 * Parameters:
 *             workZone   - pointer to memory allocated for computations, input.
 *             windowSize - operation window, determines speed vs RAM tradeoff.
 *                          2 is the most RAM efficient and slowest.  5 is
 *                          quickest but with the greatest RAM tradeoff.
 *
 * Return:      None
 */
void ECC_initialize(uint32_t *pWorkzone, uint8_t windowSize)
{
  // Initialize curve parameters
  //data_p  = (u32 *)PARAM_P;
  *((u32 **)0x20004f48) = (u32 *)PARAM_P;

  //data_r  = (u32 *)PARAM_R;
  *((u32 **)0x20004f4c) = (u32 *)PARAM_R;

  //data_a  = (u32 *)PARAM_A;
  *((u32 **)0x20004f50) = (u32 *)PARAM_A;

  //data_b  = (u32 *)PARAM_B;
  *((u32 **)0x20004fa8) = (u32 *)PARAM_B;

  //data_Gx = (u32 *)PARAM_GX;
  *((u32 **)0x20004fa0) = (u32 *)PARAM_GX;

  //data_Gy = (u32 *)PARAM_GY;
  *((u32 **)0x20004fa4) = (u32 *)PARAM_GY;

  // Initialize window size
  //win = (u8) ECC_WINDOW_SIZE;
  *((u8 *)0x20004f40) = (u8) windowSize;

  // Initialize work zone
  //workzone = (u32 *) pWorkzone;
  *((u32 **)0x20004f44) = (u32 *) pWorkzone;
}

/*
 * Function: ECC_generateKey
 *
 * Description: generates a key. This is used for both ECDH and ECDSA.
 *
 * Parameters:
 *             randString  - random string, input.
 *
 *             privateKey  - the private key, output.
 *             publicKey_x - public key X-coordinate, output.
 *             publicKey_y - public key Y-coordinate, output.
 *
 * Return: Status
 */
uint8_t ECC_generateKey(uint32_t *randString, uint32_t *privateKey,
                        uint32_t *publicKey_x, uint32_t *publicKey_y)
{
  //return (uint8_t)ECC_keyGen((u32*)randString, (u32*)privateKey,
  //                           (u32*)publicKey_x, (u32*)publicKey_y);

  return (uint8_t)ecc_generatekey((u32*)randString, (u32*)privateKey,
                                  (u32*)publicKey_x, (u32*)publicKey_y);

}

/*
 * Function: ECC_ECDH_computeSharedSecret
 *
 * Description: compute the shared secret the private key and temporary public
 *              key.
 *
 * Parameters:
 *             privateKey      - private key, input.
 *             publicKey_x     - public key X-coordinate, input.
 *             publicKey_y     - public key Y-coordinate, input.
 *
 *             sharedSecret_x  - shared secret X-coordinate, output.
 *             sharedSecret_y  - shared secret Y-coordinate, output.
 *
 * Return:     Status
 */
uint8_t ECC_ECDH_computeSharedSecret(uint32_t *privateKey,
                                     uint32_t *publicKey_x,
                                     uint32_t *publicKey_y,
                                     uint32_t *sharedSecret_x,
                                     uint32_t *sharedSecret_y)
{
  //return (uint8_t)ECDH_commonKey((u32*)privateKey, (u32*)publicKey_x,
  //                               (u32*)publicKey_y, (u32*)sharedSecret_x,
  //                               (u32*)sharedSecret_y);

  return (uint8_t)ecdh_computeSharedSecret((u32*)privateKey,
                                           (u32*)publicKey_x,
                                           (u32*)publicKey_y,
                                           (u32*)sharedSecret_x,
                                           (u32*)sharedSecret_y);
}


/*********************************************************************
*********************************************************************/
