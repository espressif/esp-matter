/******************************************************************************

 @file  ecc_api.h

 @brief Header for ECC proxy for stack's interface to the ECC driver.

 Group: WCS, BTS
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

#ifndef ECC_API_H
#define ECC_API_H

#ifdef __cplusplus
extern "C"
{
#endif
  
#include <ti/drivers/ECDH.h>

#if !defined(DeviceFamily_CC26X1)
#include <ti/drivers/ecdh/ECDHCC26X2.h>
#else
#include <ti/drivers/ecdh/ECDHCC26X1.h>
#endif

#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>

/*******************************************************************************
 * INCLUDES
 */


extern uint32_t *eccDrvTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// ECC proxy index for ECC driver API
#define ECDH_INIT                        0
#define ECDH_PARAMS_INIT                 1
#define ECDH_OPEN                        2
#define ECDH_CLOSE                       3
#define ECDH_GEN_PUBKEY_INIT             4
#define ECDH_COMP_SHAREDSEC_INIT         5
#define ECDH_GEN_PUBKEY                  6
#define ECDH_COMP_SHAREDSECKEY           7

/*
** ECC API Proxy
*/

#define ECDH_TABLE( index )   (*((uint32_t *)((uint32_t)eccDrvTblPtr + (uint32_t)((index)*4))))

#define ECDH_init                               ((void (*)(void))                                                      \
                                                                      ECDH_TABLE(ECDH_INIT))
#define ECDH_Params_init                        ((void (*)(ECDH_Params *))                                             \
                                                                      ECDH_TABLE(ECDH_PARAMS_INIT))
#define ECDH_open                               ((ECDH_Handle (*)(uint_least8_t, const ECDH_Params *))                 \
                                                                      ECDH_TABLE(ECDH_OPEN))
#define ECDH_close                              ((void (*)(ECDH_Handle))                                               \
                                                                      ECDH_TABLE(ECDH_CLOSE))
#define ECDH_OperationGeneratePublicKey_init    ((void (*)(ECDH_OperationGeneratePublicKey *))                         \
                                                                      ECDH_TABLE(ECDH_GEN_PUBKEY_INIT))
#define ECDH_OperationComputeSharedSecret_init  ((void (*)(ECDH_OperationComputeSharedSecret *))                       \
                                                                      ECDH_TABLE(ECDH_COMP_SHAREDSEC_INIT))
#define ECDH_generatePublicKey                  ((int_fast16_t (*)(ECDH_Handle, ECDH_OperationGeneratePublicKey  *))   \
                                                                      ECDH_TABLE(ECDH_GEN_PUBKEY))
#define ECDH_computeSharedSecret                ((int_fast16_t (*)(ECDH_Handle, ECDH_OperationComputeSharedSecret  *)) \
                                                                      ECDH_TABLE(ECDH_COMP_SHAREDSECKEY))

#ifdef __cplusplus
}
#endif

#endif /* ECC_API_H */
