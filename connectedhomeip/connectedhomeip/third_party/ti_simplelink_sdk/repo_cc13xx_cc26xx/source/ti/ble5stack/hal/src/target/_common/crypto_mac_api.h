/******************************************************************************

 @file  crypto_mac_api.h

 @brief Header for Crypto proxy for stack's interface to the crypto driver.

 Group: WCS, LPC/BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#if !defined(CC13X2R1_LAUNCHXL) && !defined(CC26X2R1_LAUNCHXL) && !defined(CC2652RB_LAUNCHXL) && !defined(CC13X2P1_LAUNCHXL) && !defined(CC13X2P_2_LAUNCHXL) && !defined(CC13X2P_4_LAUNCHXL) && !defined(CC2652PSIP_LP) && !defined (CC2652RSIP_LP) && !defined (CC2652R7_LP) && !defined (CC1352P7_1_LP) && !defined (CC1352P7_4_LP) && !defined (CC2651P3_LP) && !defined (CC2651R3_LP)
#include <ti/drivers/crypto/CryptoCC26XX.h>
#else
#include <ti/drivers/AESCCM.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#endif

#if defined(FEATURE_MAC_SECURITY) && defined(TIMAC_ROM_IMAGE_BUILD)
extern uint32_t *macCryptoDrvTblPtr;
#endif

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#if !defined(CC13X2R1_LAUNCHXL) && !defined(CC26X2R1_LAUNCHXL) && !defined(CC2652RB_LAUNCHXL) && !defined(CC13X2P1_LAUNCHXL) && !defined(CC13X2P_2_LAUNCHXL) && !defined(CC13X2P_4_LAUNCHXL) && !defined(CC2652PSIP_LP) && !defined (CC2652RSIP_LP) && !defined (CC2652R7_LP) && !defined (CC1352P7_1_LP) && !defined (CC1352P7_4_LP) && !defined (CC2651P3_LP) && !defined (CC2651R3_LP)
// Crypto proxy index for crypto driver API
#define CRYPTOCC26XX_CLOSE              0
#define CRYPTOCC26XX_INIT               1
#define CRYPTOCC26XX_OPEN               2
#define CRYPTOCC26XX_PARAMS_INIT        3
#define CRYPTOCC26XX_TRANSAC_INIT       4
#define CRYPTOCC26XX_ALLOCATEKEY        5
#define CRYPTOCC26XX_RELEASEKEY         6
#define CRYPTOCC26XX_TRANSACT           7
#define CRYPTOCC26XX_TRANSACTPOLLING    8
#define CRYPTOCC26XX_TRANSACTCALLBACK   9
#define CRYPTOCC26XX_LOADKEY            10

/*
** Crypto API Proxy
*/

#define CRYPTO_TABLE(index)           (*((uint32_t *)((uint32_t)macCryptoDrvTblPtr + (uint32_t)((index)*4))))

#define CryptoCC26XX_close            ((int                 (*)(CryptoCC26XX_Handle))                                             \
                                       CRYPTO_TABLE(CRYPTOCC26XX_CLOSE))
#define CryptoCC26XX_init             ((void                (*)(void))                                                            \
                                       CRYPTO_TABLE(CRYPTOCC26XX_INIT))
#define CryptoCC26XX_open             ((CryptoCC26XX_Handle (*)(unsigned int, bool, CryptoCC26XX_Params*))                        \
                                       CRYPTO_TABLE(CRYPTOCC26XX_OPEN))
#define CryptoCC26XX_Params_init      ((void                (*)(CryptoCC26XX_Params *))                                           \
                                       CRYPTO_TABLE(CRYPTOCC26XX_PARAMS_INIT))
#define CryptoCC26XX_Transac_init     ((void                (*)(CryptoCC26XX_Transaction *, CryptoCC26XX_Operation))              \
                                       CRYPTO_TABLE(CRYPTOCC26XX_TRANSAC_INIT))
#define CryptoCC26XX_allocateKey      ((int                 (*)(CryptoCC26XX_Handle, CryptoCC26XX_KeyLocation, const uint32_t *)) \
                                       CRYPTO_TABLE(CRYPTOCC26XX_ALLOCATEKEY))
#define CryptoCC26XX_releaseKey       ((int                 (*)(CryptoCC26XX_Handle, int *))                                      \
                                       CRYPTO_TABLE(CRYPTOCC26XX_RELEASEKEY))
#define CryptoCC26XX_transact         ((int                 (*)(CryptoCC26XX_Handle, CryptoCC26XX_Transaction *))                 \
                                       CRYPTO_TABLE(CRYPTOCC26XX_TRANSACT))
#define CryptoCC26XX_transactPolling  ((int                 (*)(CryptoCC26XX_Handle, CryptoCC26XX_Transaction *))                 \
                                       CRYPTO_TABLE(CRYPTOCC26XX_TRANSACTPOLLING))
#define CryptoCC26XX_transactCallback ((int                 (*)(CryptoCC26XX_Handle, CryptoCC26XX_Transaction *))                 \
                                       CRYPTO_TABLE(CRYPTOCC26XX_TRANSACTCALLBACK))
#define CryptoCC26XX_loadKey          ((int                 (*)(CryptoCC26XX_Handle, int, const uint32_t *))                      \
                                       CRYPTO_TABLE(CRYPTOCC26XX_LOADKEY))

#else
// Crypto proxy index for crypto driver API
#define AESCCM_INIT                       0
#define AESCCM_OPEN                       1
#define AESCCM_CLOSE                      2
#define AESCCM_PARAMS_INIT                3
#define AESCCM_OPERATION_INIT             4
#define AESCCM_ONESTEPENCRYPT             5
#define AESCCM_ONESTEPDECRYPT             6
#define AESECB_INIT                       7
#define AESECB_OPEN                       8
#define AESECB_CLOSE                      9
#define AESECB_PARAMS_INIT                10
#define AESECB_OPERATION_INIT             11
#define AESECB_ONESTEPENCRYPT             12
#define AESECB_ONESTEPDECRYPT             13
#define CRYPTOKEYPLAINTEXT_INITKEY        14
#define CRYPTOKEYPLAINTEXT_INITBLANKKEY   15

/*
** Crypto API Proxy
*/

#define CRYPTO_TABLE(index)           (*((uint32_t *)((uint32_t)macCryptoDrvTblPtr + (uint32_t)((index)*4))))

#define AESCCM_init                     ((void                (*)(void))                               \
                                          CRYPTO_TABLE(AESCCM_INIT))
#define AESCCM_open                     ((AESCCM_Handle (*)(uint8_t, AESCCM_Params*))                  \
                                          CRYPTO_TABLE(AESCCM_OPEN))
#define AESCCM_close                    ((int16_t             (*)(AESCCM_Handle))                      \
                                         CRYPTO_TABLE(AESCCM_CLOSE))
#define AESCCM_Params_init              ((void                (*)(AESCCM_Params *))                    \
                                          CRYPTO_TABLE(AESCCM_PARAMS_INIT))
#define AESCCM_Operation_init           ((void                (*)(AESCCM_Operation *))                 \
                                         CRYPTO_TABLE(AESCCM_OPERATION_INIT))
#define AESCCM_oneStepEncrypt           ((int16_t             (*)(AESCCM_Handle, AESCCM_Operation *))  \
                                         CRYPTO_TABLE(AESCCM_ONESTEPENCRYPT))
#define AESCCM_oneStepDecrypt           ((int16_t             (*)(AESCCM_Handle, AESCCM_Operation *))  \
                                         CRYPTO_TABLE(AESCCM_ONESTEPDECRYPT))
#define AESECB_init                     ((void                (*)(void))                               \
                                          CRYPTO_TABLE(AESECB_INIT))
#define AESECB_open                     ((AESECB_Handle (*)(unsigned int, AESECB_Params*))             \
                                          CRYPTO_TABLE(AESECB_OPEN))
#define AESECB_close                    ((int                 (*)(AESECB_Handle))                      \
                                         CRYPTO_TABLE(AESECB_CLOSE))
#define AESECB_Params_init              ((void                (*)(AESECB_Params *))                    \
                                          CRYPTO_TABLE(AESECB_PARAMS_INIT))
#define AESECB_Operation_init           ((void                (*)(AESECB_Operation *))                 \
                                         CRYPTO_TABLE(AESECB_OPERATION_INIT))
#define AESECB_oneStepEncrypt           ((int                (*)(AESECB_Handle, AESECB_Operation *))   \
                                         CRYPTO_TABLE(AESECB_ONESTEPENCRYPT))
#define AESECB_oneStepDecrypt           ((int                (*)(AESECB_Handle, AESECB_Operation *))   \
                                         CRYPTO_TABLE(AESECB_ONESTEPDECRYPT))
#define CryptoKeyPlaintext_initKey      ((int                 (*)(CryptoKey *, uint8_t *, size_t))     \
                                          CRYPTO_TABLE(CRYPTOKEYPLAINTEXT_INITKEY))
#define CryptoKeyPlaintext_initBlankKey ((int                 (*)(CryptoKey *, uint8_t *, size_t))     \
                                          CRYPTO_TABLE(CRYPTOKEYPLAINTEXT_INITBLANKKEY))

#endif

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_API_H */
