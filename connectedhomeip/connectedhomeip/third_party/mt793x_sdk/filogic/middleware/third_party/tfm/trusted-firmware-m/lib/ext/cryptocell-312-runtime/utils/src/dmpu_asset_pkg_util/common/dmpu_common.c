/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include "common_util_log.h"
#include "common_crypto_sym.h"
#include "cc_production_asset.h"
#include "cc_pal_types.h"


uint8_t  isLibOpened = 0;

/**
* @brief initialize openSSL library
*
* @param[in] None
* @param[out] None
*
*/
/*********************************************************/
void InitOpenSsl(void)
{
    if (0 == isLibOpened) {
          OpenSSL_add_all_algorithms();
    }
    isLibOpened++;
}


/**
* @brief terminates and cleanup openSSL library
*
* @param[in]  None
* @param[out] None
*
*/
/*********************************************************/
void CloseOpenSsl(void)
{
    isLibOpened--;
    if (0 == isLibOpened) {
          EVP_cleanup();
          //CYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/
    }
}


/**
* @brief performs CMAC key derivation for Kprov using openSSL library
*
* @param[in]  pKey & keySize - Kpicv key and its size
*       lable & pContext & contextSize used to build the dataIn for derivation
* @param[out] pOutKey - Kprov
*
*/
/*********************************************************/
int AesCmacKeyDerivation(char *pKey, uint32_t keySize,
                uint8_t *pLabel, uint32_t labelSize,
                uint8_t *pContext, uint32_t contextSize,
                char *pOutKey, uint32_t outKeySize)
{
        #define MAX_DATA_IN_SIZE  (PROD_KEY_TMP_LABEL_SIZE + PROD_KEY_TMP_CONTEXT_SIZE + 3)  // +3 for: iteration, key size and 0x0
    int rc = 0;
    int index = 0;
    int8_t dataIn[MAX_DATA_IN_SIZE] = {0x0};

        if ((pKey == NULL) ||
            (keySize != PROD_KEY_TMP_KEY_SIZE) ||
            (pLabel == NULL) ||
            (labelSize > PROD_KEY_TMP_LABEL_SIZE) ||
            (pContext == NULL) ||
            (contextSize > PROD_KEY_TMP_CONTEXT_SIZE) ||
            (pOutKey == NULL) ||
            (outKeySize != PROD_KEY_TMP_KEY_SIZE)) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return (-1);
        }

        /* Create the input to the CMAC derivation
           since key size is 16 bytes, we have 1 iteration for cmac  derivation*
           the data or the derivation:
           0x1 || label || 0x0 || context || size of derived key in bits */
    dataIn[index++] = 0x1;
    memcpy(&dataIn[index], pLabel, labelSize);
    index += labelSize;
    dataIn[index++] = 0x0;
    memcpy(&dataIn[index], pContext, contextSize);
    index += contextSize;
    dataIn[index++] = outKeySize*CC_BITS_IN_BYTE; // size of the key in bits

    UTIL_LOG_BYTE_BUFF("dataIn", dataIn, index);
    UTIL_LOG_BYTE_BUFF("pKey", pKey, keySize);
    rc = CC_CommonAesCmacEncrypt(dataIn, index,
                     pKey, keySize, pOutKey);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCmacEncrypt(), rc %d\n", rc);
        return (-1);
    }
    UTIL_LOG_BYTE_BUFF("pOutKey", pOutKey, outKeySize);
    return rc;
}

