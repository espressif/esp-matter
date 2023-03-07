/******************************************************************************

 @file  ecc_rom.h

 @brief This header file is the API to the ECC module built into
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

#ifndef ECC_ROM_H
#define ECC_ROM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/* Window size, valid values are 2,3,4,5.
 * Higher the value, faster the computation at the expense of memory usage.
 *
 * Recommended workzone size (in 4-byte words)
 * Window size: 3, Workzone size: 275
 *
 */

/*
 * ECC Supported Curves, define one:
 * ECC_PRIME_NIST256_CURVE
 */
#define ECC_PRIME_NIST256_CURVE

/*
 * ECC Return Status Flags.
 */
// Scalar multiplication status
#define ECC_MODULUS_EVEN                   0xDC
#define ECC_MODULUS_LARGER_THAN_255_WORDS  0xD2
#define ECC_MODULUS_LENGTH_ZERO            0x08
#define ECC_MODULUS_MSW_IS_ZERO	           0x30
#define ECC_SCALAR_TOO_LONG                0x35
#define ECC_SCALAR_LENGTH_ZERO	           0x53
#define ECC_ORDER_TOO_LONG                 0xC6
#define ECC_ORDER_LENGTH_ZERO	           0x6C
#define ECC_X_COORD_TOO_LONG	           0x3C
#define ECC_X_COORD_LENGTH_ZERO	           0xC3
#define ECC_Y_COORD_TOO_LONG	           0x65
#define ECC_Y_COORD_LENGTH_ZERO	           0x56
#define ECC_A_COEF_TOO_LONG                0x5C
#define ECC_A_COEF_LENGTH_ZERO	           0xC5
#define ECC_BAD_WINDOW_SIZE                0x66
#define ECC_SCALAR_MUL_OK                  0x99

// ECDSA and ECDH status
#define ECC_ORDER_LARGER_THAN_255_WORDS	   0x28
#define ECC_ORDER_EVEN                     0x82
#define ECC_ORDER_MSW_IS_ZERO              0x23
#define ECC_ECC_KEY_TOO_LONG               0x25
#define ECC_ECC_KEY_LENGTH_ZERO            0x52
#define ECC_DIGEST_TOO_LONG                0x27
#define ECC_DIGEST_LENGTH_ZERO             0x72
#define ECC_ECDSA_SIGN_OK                  0x32
#define ECC_ECDSA_INVALID_SIGNATURE        0x5A
#define ECC_ECDSA_VALID_SIGNATURE          0xA5
#define ECC_SIG_P1_TOO_LONG                0x11
#define ECC_SIG_P1_LENGTH_ZERO             0x12
#define ECC_SIG_P2_TOO_LONG                0x22
#define ECC_SIG_P2_LENGTH_ZERO             0x21

#define ECC_ECDSA_KEYGEN_OK                ECC_SCALAR_MUL_OK
#define ECC_ECDH_KEYGEN_OK                 ECC_SCALAR_MUL_OK
#define ECC_ECDH_COMMON_KEY_OK             ECC_SCALAR_MUL_OK


/*********************************************************************
 * API FUNCTIONS
 */

/*
 * Function:    ECC_initialize
 *
 * Description: pass pointer to ECC memory allocation to ECC engine.  This
 *              function can be called again to point the ECC workzone at
 *              a different memory buffer.
 *
 * Parameters:
 *              pWorkzone  - pointer to memory allocated for computations, input.
 *                           See description at beginning of ECC section for
 *                           memory requirements.
 *              windowSize - operation window, determines speed vs RAM tradeoff.
 *                           2 is the most RAM efficient and slowest.  5 is
 *                           quickest but with the greatest RAM tradeoff.
 *
 * Return:      None
 */
extern void ECC_initialize(uint32_t *pWorkzone, uint8_t windowSize);

 /*
 * Function:    ECC_generateKey
 *
 * Description: generates a key.  This is used for both ECDH and ECDSA.
 *
 * Parameters:
 *              randString  - random string, input.
 *
 *              privateKey  - the private key, output.
 *              publicKey_x - public key X-coordinate, output.
 *              publicKey_y - public key Y-coordinate, output.
 *
 * Return:      Status
 */
extern uint8_t ECC_generateKey(uint32_t *randString, uint32_t *privateKey,
                               uint32_t *publicKey_x, uint32_t *publicKey_y);

/*
 * Function:    ECC_ECDH_computeSharedSecret
 *
 * Description: compute the shared secret.
 *
 * Parameters:
 *              privateKey     - private key, input.
 *              publicKey_x    - public key X-coordinate, input.
 *              publicKey_y    - public key Y-coordinate, input.
 *
 *              sharedSecret_x - shared secret X-coordinate, output.
 *              sharedSecret_y - shared secret Y-coordinate, output.
 *
 * Return:      Status
 */
extern uint8_t ECC_ECDH_computeSharedSecret(uint32_t *privateKey,
                                            uint32_t *publicKey_x,
                                            uint32_t *publicKey_y,
                                            uint32_t *sharedSecret_x,
                                            uint32_t *sharedSecret_y);

#ifdef __cplusplus
}
#endif

#endif /* ECC_ROM_H */
