/******************************************************************************

 @file  ECCROMCC26XX.c

 @brief This file contains the source for the ECC in ROM Driver.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include <string.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include "ecc/ECCROMCC26XX.h"

/*********************************************************************
 * CONSTANTS
 */

// ECC Window Size.  Determines speed and workzone size of ECC operations.
// Recommended setting is 3.
#define ECC_WINDOW_SIZE                3

// Key size in uint32_t blocks.
#define ECC_UINT32_BLK_LEN(len)        (((len) + 3) / 4)

// Offset of Key field
#define ECC_KEY_OFFSET                 4

// Offset of Key Length field
#define ECC_KEY_LEN_OFFSET             0

// Total buffer size
#define ECC_BUF_TOTAL_LEN(len)         ((len) + ECC_KEY_OFFSET)

/*********************************************************************
 * EXTERNS
 */

// ECC ROM key generation functions.
extern uint8_t eccRom_genKeys(uint32_t *, uint32_t *, uint32_t *, uint32_t *);
extern uint8_t eccRom_genSharedSecret(uint32_t *, uint32_t *, uint32_t *,
                                      uint32_t *, uint32_t *);

// ECC ROM global window size and workzone buffer.
extern uint8_t eccRom_windowSize;
extern uint32_t *eccRom_workzone;

// ECC ROM global parameters
extern uint32_t *eccRom_param_p;
extern uint32_t *eccRom_param_r;
extern uint32_t *eccRom_param_a;
extern uint32_t *eccRom_param_b;
extern uint32_t *eccRom_param_Gx;
extern uint32_t *eccRom_param_Gy;

// NIST P-256 Curves in ROM
// Note: these are actually strings
extern uint32_t NIST_Curve_P256_p;
extern uint32_t NIST_Curve_P256_r;
extern uint32_t NIST_Curve_P256_a;
extern uint32_t NIST_Curve_P256_b;
extern uint32_t NIST_Curve_P256_Gx;
extern uint32_t NIST_Curve_P256_Gy;

/*********************************************************************
 * LOCAL VARIABLES
 */

// ECC driver semaphore used to synchronize access.
static Semaphore_Handle ECC_semaphore;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void ECC_initGlobals(ECCROMCC26XX_CurveParams *pCurve);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== ECCROMCC26XX_init ========
 */
void ECCROMCC26XX_init(void)
{
  static uint8_t isInit = 0;
  unsigned int key;

  // Enter critical section.
  key = Hwi_disable();

  if (!isInit)
  {
    Semaphore_Params semParams;

    // Only initialize once.
    isInit = 1;

    // Setup semaphore for sequencing accesses to ECC
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    ECC_semaphore = Semaphore_create(1, &semParams, NULL);
  }

  // Exit critical section.
  Hwi_restore(key);
}

/*
 *  ======== ECCROMCC26XX_Params_init ========
 */
void ECCROMCC26XX_Params_init(ECCROMCC26XX_Params *params)
{
  if (params == NULL)
  {
    return;
  }
  // Initialize Curve to NIST P-256 with window size 3 by default.
  params->curve.keyLen      = ECCROMCC26XX_NIST_P256_KEY_LEN_IN_BYTES;
  params->curve.workzoneLen = ECCROMCC26XX_NIST_P256_WORKZONE_LEN_IN_BYTES;
  params->curve.windowSize  = ECC_WINDOW_SIZE;
  params->curve.param_p     = &NIST_Curve_P256_p;
  params->curve.param_r     = &NIST_Curve_P256_r;
  params->curve.param_a     = &NIST_Curve_P256_a;
  params->curve.param_b     = &NIST_Curve_P256_b;
  params->curve.param_gx    = &NIST_Curve_P256_Gx;
  params->curve.param_gy    = &NIST_Curve_P256_Gy;

  // Initialize memory functions to NULL
  params->malloc            = NULL;
  params->free              = NULL;

  // Initialize timeout to block indefinitely
  params->timeout           = BIOS_WAIT_FOREVER;

  // Initialize status
  params->status            = ECCROMCC26XX_STATUS_SUCCESS;
}

/*
 *  ======== ECCROMCC26XX_genKeys ========
 */
int8_t ECCROMCC26XX_genKeys(uint8_t *privateKey, uint8_t *publicKeyX,
                            uint8_t *publicKeyY, ECCROMCC26XX_Params *params)
{
  int8_t  status;
  uint8_t *randStrBuf;
  uint8_t *pubKeyXBuf;
  uint8_t *pubKeyYBuf;

  // Check key buffers and params.
  if (privateKey == NULL || publicKeyX == NULL || publicKeyY == NULL ||
      params == NULL || params->malloc == NULL || params->free == NULL)
  {
    // Store status.
    if (params)
    {
      params->status = ECCROMCC26XX_STATUS_ILLEGAL_PARAM;
    }

    return ECCROMCC26XX_STATUS_ILLEGAL_PARAM;
  }

  // Pend on Semaphore.
  params->status = Semaphore_pend(ECC_semaphore, params->timeout);

  // If execution returned due to a timeout
  if (!params->status)
  {
    // Store status.
    params->status = ECCROMCC26XX_STATUS_TIMEOUT;

    return ECCROMCC26XX_STATUS_TIMEOUT;
  }

  // Allocate total memory for operation: workzone and 3 buffers
  if (!(eccRom_workzone = (uint32_t *)params->malloc(params->curve.workzoneLen + ECC_BUF_TOTAL_LEN(params->curve.keyLen) * 3)))
  {
    // Post Semaphore.
    Semaphore_post(ECC_semaphore);

    // Store status.
    params->status = ECCROMCC26XX_STATUS_MALLOC_FAIL;

    return ECCROMCC26XX_STATUS_MALLOC_FAIL;
  }

  // Split allocated memory into buffers
  randStrBuf = (uint8_t *) eccRom_workzone + params->curve.workzoneLen;
  pubKeyXBuf = randStrBuf + ECC_BUF_TOTAL_LEN(params->curve.keyLen);
  pubKeyYBuf = pubKeyXBuf + ECC_BUF_TOTAL_LEN(params->curve.keyLen);

  // Initialize ECC curve and globals.
  ECC_initGlobals(&params->curve);

  // Set length of keys in words in the first word of each buffer.
  *((uint32_t *)&randStrBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&pubKeyXBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&pubKeyYBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));

  // Copy in random string at key start offset.
  memcpy(randStrBuf + ECC_KEY_OFFSET, privateKey, params->curve.keyLen);

  // Generate ECC private/public key pair.
  // Note: the random string is an exact copy of the private key.
  status = eccRom_genKeys((uint32_t *)randStrBuf,
                          (uint32_t *)randStrBuf,
                          (uint32_t *)pubKeyXBuf,
                          (uint32_t *)pubKeyYBuf);

  // Move ECC buffer values to client buffers.
  memcpy(publicKeyX, pubKeyXBuf + ECC_KEY_OFFSET, params->curve.keyLen);
  memcpy(publicKeyY, pubKeyYBuf + ECC_KEY_OFFSET, params->curve.keyLen);

  // zero out workzone and 3 buffers as a precautionary measure.
  memset(eccRom_workzone, 0x00, params->curve.workzoneLen + params->curve.keyLen * 3);

  // Free allocated memory.
  params->free((uint8_t *)eccRom_workzone);

  // Post Semaphore.
  Semaphore_post(ECC_semaphore);

  // Map success code.
  if (((uint8_t)status) == ECCROMCC26XX_STATUS_ECDH_KEYGEN_OK)
  {
    status = ECCROMCC26XX_STATUS_SUCCESS;
  }

  // Store status.
  params->status = status;

  return status;
}

/*
 *  ======== ECCROMCC26XX_genDHKey ========
 */
int8_t ECCROMCC26XX_genDHKey(uint8_t *privateKey, uint8_t *publicKeyX,
                             uint8_t *publicKeyY, uint8_t *dHKeyX,
                             uint8_t *dHKeyY, ECCROMCC26XX_Params *params)
{
  int8_t  status;
  uint8_t *privKeyBuf;
  uint8_t *pubKeyXBuf;
  uint8_t *pubKeyYBuf;
  uint8_t *DHKeyXBuf;
  uint8_t *DHKeyYBuf;

  // Check key buffers and params.
  if (privateKey == NULL || publicKeyX == NULL || publicKeyY == NULL ||
      dHKeyX == NULL || dHKeyY == NULL || params == NULL ||
      params->malloc == NULL || params->free == NULL)
  {
    // Store status.
    if (params)
    {
      params->status = ECCROMCC26XX_STATUS_ILLEGAL_PARAM;
    }

    return ECCROMCC26XX_STATUS_ILLEGAL_PARAM;
  }

  // Pend on Semaphore.
  params->status = Semaphore_pend(ECC_semaphore, params->timeout);

  // If execution returned due to a timeout then leave here
  if (!params->status)
  {
    // Store status.
    params->status = ECCROMCC26XX_STATUS_TIMEOUT;

    return ECCROMCC26XX_STATUS_TIMEOUT;
  }

  // Allocate total memory for operation: workzone and 5 key buffers.
  if (!(eccRom_workzone = (uint32_t *)params->malloc(params->curve.workzoneLen + ECC_BUF_TOTAL_LEN(params->curve.keyLen) * 5)))
  {
    // Post Semaphore.
    Semaphore_post(ECC_semaphore);

    // Store status.
    params->status = ECCROMCC26XX_STATUS_MALLOC_FAIL;

    return ECCROMCC26XX_STATUS_MALLOC_FAIL;
  }

  // Split allocated memory into buffers
  privKeyBuf = (uint8_t *)eccRom_workzone + params->curve.workzoneLen;
  pubKeyXBuf = privKeyBuf + ECC_BUF_TOTAL_LEN(params->curve.keyLen);
  pubKeyYBuf = pubKeyXBuf + ECC_BUF_TOTAL_LEN(params->curve.keyLen);
  DHKeyXBuf  = pubKeyYBuf + ECC_BUF_TOTAL_LEN(params->curve.keyLen);
  DHKeyYBuf  = DHKeyXBuf  + ECC_BUF_TOTAL_LEN(params->curve.keyLen);

  // Initialize ECC curve and globals.
  ECC_initGlobals(&params->curve);

  // Set length of keys in words in the first word of each buffer.
  *((uint32_t *)&privKeyBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&pubKeyXBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&pubKeyYBuf[ECC_KEY_LEN_OFFSET]) = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&DHKeyXBuf[ECC_KEY_LEN_OFFSET])  = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));
  *((uint32_t *)&DHKeyYBuf[ECC_KEY_LEN_OFFSET])  = (uint32_t)(ECC_UINT32_BLK_LEN(params->curve.keyLen));

  // Copy input keys into buffers.
  memcpy(privKeyBuf + ECC_KEY_OFFSET, privateKey, params->curve.keyLen);
  memcpy(pubKeyXBuf + ECC_KEY_OFFSET, publicKeyX, params->curve.keyLen);
  memcpy(pubKeyYBuf + ECC_KEY_OFFSET, publicKeyY, params->curve.keyLen);

  // Generate ECC Diffie-Hellman shared secret.
  status = eccRom_genSharedSecret((uint32_t *)privKeyBuf,
                                  (uint32_t *)pubKeyXBuf,
                                  (uint32_t *)pubKeyYBuf,
                                  (uint32_t *)DHKeyXBuf,
                                  (uint32_t *)DHKeyYBuf);

  // Move ECC buffer values to client buffers.
  memcpy(dHKeyX, DHKeyXBuf + ECC_KEY_OFFSET, params->curve.keyLen);
  memcpy(dHKeyY, DHKeyYBuf + ECC_KEY_OFFSET, params->curve.keyLen);

 // zero out workzone and 5 buffers as a precautionary measure.
  memset(eccRom_workzone, 0x00, params->curve.workzoneLen + params->curve.keyLen * 5);

  // Free allocated memory.
  params->free((uint8_t *)eccRom_workzone);

  // Post Semaphore.
  Semaphore_post(ECC_semaphore);

  // Map success code.
  if (((uint8_t)status) == ECCROMCC26XX_STATUS_ECDH_COMMON_KEY_OK)
  {
    status = ECCROMCC26XX_STATUS_SUCCESS;
  }

  // Store status.
  params->status = status;

  return status;
}

/*
 *  ======== ECC_initGlobals ========
 */
static void ECC_initGlobals(ECCROMCC26XX_CurveParams *pCurve)
{
  // Store client parameters into ECC ROM parameters.
  eccRom_param_p  = pCurve->param_p;
  eccRom_param_r  = pCurve->param_r;
  eccRom_param_a  = pCurve->param_a;
  eccRom_param_b  = pCurve->param_b;
  eccRom_param_Gx = pCurve->param_gx;
  eccRom_param_Gy = pCurve->param_gy;

  // Initialize window size
  eccRom_windowSize = pCurve->windowSize;
}
