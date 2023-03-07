/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "mbedtls_cc_chacha.h"
#include "mbedtls_cc_chacha_error.h"
#include "driver_defs.h"
#include "cc_sym_error.h"
#include "chacha_driver_ext_dma.h"
#include "mbedtls_ext_dma_error.h"


static CCError_t Driver2ExtDMAChachaErr(drvError_t drvRc)
{
    switch (drvRc) {
    case CHACHA_DRV_OK:
        return CC_OK;
    case CHACHA_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR:
        return EXT_DMA_CHACHA_INVALID_ENCRYPT_MODE_ERROR;
    case CHACHA_DRV_ILLEGAL_NONCE_SIZE_ERROR:
        return EXT_DMA_CHACHA_INVALID_NONCE_ERROR;
    default:
        return CC_FATAL_ERROR;
    }
}


int mbedtls_ext_dma_chacha_init(uint8_t *  pNonce,
                                mbedtls_chacha_nonce_size_t   nonceSizeFlag,
                                uint8_t *  pKey,
                                uint32_t    keySizeBytes,
                                uint32_t    initialCounter,
                                mbedtls_chacha_encrypt_mode_t  EncryptDecryptFlag,
                                uint32_t    dataSize)
{
    drvError_t drvRc = CHACHA_DRV_OK;
    uint32_t keyBufferWords[CC_CHACHA_KEY_MAX_SIZE_IN_WORDS];
    uint32_t nonceBuffer[CC_CHACHA_NONCE_MAX_SIZE_IN_WORDS];
    uint32_t nonceSizeBytes=0;
    /* ............... checking the parameters validity ................... */
    /* -------------------------------------------------------------------- */
    /* if dataSize is 0 return an error */
    if ((dataSize == 0) || (dataSize > CPU_DIN_MAX_SIZE)){
        return EXT_DMA_CHACHA_ILLEGAL_INPUT_SIZE_ERROR;
    }
    /* if the pNonce is NULL return an error */
    if (pNonce == NULL) {
       return EXT_DMA_CHACHA_INVALID_NONCE_PTR_ERROR;
    }

    /* check the Encrypt / Decrypt flag validity */
    if (EncryptDecryptFlag >= CC_CHACHA_EncryptNumOfOptions) {
        return  EXT_DMA_CHACHA_INVALID_ENCRYPT_MODE_ERROR;
    }

    /*  check the validity of the key pointer */
    if (pKey == NULL) {
        return  EXT_DMA_CHACHA_INVALID_KEY_POINTER_ERROR;
    }
    if (keySizeBytes != CC_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return  EXT_DMA_CHACHA_ILLEGAL_KEY_SIZE_ERROR;

    }

    switch (nonceSizeFlag) {
    case CC_CHACHA_Nonce64BitSize:
        nonceSizeBytes = 8;
        break;
    case CC_CHACHA_Nonce96BitSize:
        nonceSizeBytes = 12;
        break;
    default:
        return  EXT_DMA_CHACHA_INVALID_NONCE_ERROR;
    }

    CC_PalMemCopy((uint8_t *)keyBufferWords, pKey, CC_CHACHA_KEY_MAX_SIZE_IN_BYTES);
    CC_PalMemCopy((uint8_t *)nonceBuffer, pNonce, nonceSizeBytes);

    drvRc = InitChachaExtDma(nonceBuffer, (chachaNonceSize_t)nonceSizeFlag, keyBufferWords, initialCounter, dataSize);

    return Driver2ExtDMAChachaErr(drvRc);
}


int mbedtls_chacha_ext_dma_finish(void)
{
    drvError_t drvRc = CHACHA_DRV_OK;

    drvRc = terminateChachaExtDma();

    return Driver2ExtDMAChachaErr(drvRc);
}
