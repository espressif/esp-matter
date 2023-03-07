/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "aes.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "aes_driver.h"
#include "cc_aes_defs.h"
#include "mbedtls_ext_dma_error.h"
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_sym_error.h"
#include "cc_pal_perf.h"
#include "cc_pal_compiler.h"
#include "mbedtls_aes_ext_dma.h"
#include "aes_driver_ext_dma.h"


static aesMode_t CC2DriverAesMode(CCAesOperationMode_t operationMode)
{
    aesMode_t result;

    switch (operationMode) {
    case CC_AES_MODE_ECB:
        result = CIPHER_ECB;
        break;
    case CC_AES_MODE_CBC:
        result = CIPHER_CBC;
        break;
    case CC_AES_MODE_CTR:
        result = CIPHER_CTR;
        break;
    case CC_AES_MODE_CBC_MAC:
        result = CIPHER_CBC_MAC;
        break;
    case CC_AES_MODE_CMAC:
        result = CIPHER_CMAC;
        break;
    case CC_AES_MODE_OFB:
        result = CIPHER_OFB;
        break;
    default:
        result = CIPHER_NULL_MODE;
    }

    return result;
}



int mbedtls_aes_ext_dma_init(
    unsigned int keybits,
    int   encryptDecryptFlag,
    CCAesOperationMode_t operationMode)
{
    drvError_t drvRc = AES_DRV_OK;
    aesMode_t         mode;
    keySizeId_t       keySizeId;

    /* check if the operation mode is legal */
    if ( (operationMode != CC_AES_MODE_ECB) &&
         (operationMode != CC_AES_MODE_CBC) &&
         (operationMode != CC_AES_MODE_CTR) &&
         (operationMode != CC_AES_MODE_CBC_MAC) &&
         (operationMode != CC_AES_MODE_CMAC)  &&
         (operationMode != CC_AES_MODE_OFB)) {
        return EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR;
    }

    if (CC_AES_DECRYPT != encryptDecryptFlag && CC_AES_ENCRYPT != encryptDecryptFlag) {
        return EXT_DMA_AES_INVALID_ENCRYPT_MODE_ERROR;
    }

    /* in MAC,XCBC,CMAC modes enable only encrypt mode  */
    if ( ((operationMode == CC_AES_MODE_CBC_MAC) ||
          (operationMode == CC_AES_MODE_CMAC)) &&
         (encryptDecryptFlag != CC_AES_ENCRYPT) ) {
        return EXT_DMA_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE;
    }

    /* check key size validity */
    switch (keybits)
    {
    case 128:
        keySizeId =  KEY_SIZE_128_BIT;
        break;
    case 192:
        keySizeId =  KEY_SIZE_192_BIT;
        break;
    case 256:
        keySizeId =  KEY_SIZE_256_BIT;
        break;
    default:
        return EXT_DMA_AES_ILLEGAL_KEY_SIZE_ERROR;
    }

    mode = CC2DriverAesMode(operationMode);
    /* In case of failure, the resources will be freed within the driver */
    drvRc = AesExtDmaInit((cryptoDirection_t)encryptDecryptFlag, mode, keySizeId);
    // "add driver to CC conversion"
    return drvRc;
}

int  mbedtls_aes_ext_dma_set_key(
        CCAesOperationMode_t operationMode,
        const unsigned char *key,
        unsigned int keybits )
{
    drvError_t drvRc = AES_DRV_OK;
    drvError_t rc = AES_DRV_OK;
    keySizeId_t keySizeId;
    uint32_t  keyBuffer[CC_AES_KEY_MAX_SIZE_IN_WORDS];
    aesMode_t         mode;

    /* check the validity of the key data pointer */
    if (key == NULL) {
        drvRc = EXT_DMA_AES_ILLEGAL_KEY_SIZE_ERROR;
        goto end;
    }

    /* check if the operation mode is legal */
    if ( (operationMode != CC_AES_MODE_ECB) &&
         (operationMode != CC_AES_MODE_CBC) &&
         (operationMode != CC_AES_MODE_CTR) &&
         (operationMode != CC_AES_MODE_CBC_MAC) &&
         (operationMode != CC_AES_MODE_CMAC)  &&
         (operationMode != CC_AES_MODE_OFB)) {
        drvRc = EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }

    /* check key size validity */
    switch (keybits)
    {
    case 128:
        keySizeId =  KEY_SIZE_128_BIT;
        break;
    case 192:
        keySizeId =  KEY_SIZE_192_BIT;
        break;
    case 256:
        keySizeId =  KEY_SIZE_256_BIT;
        break;
    default:
        drvRc =  EXT_DMA_AES_ILLEGAL_KEY_SIZE_ERROR;
        goto end;
    }

    mode = CC2DriverAesMode(operationMode);
    CC_PalMemCopy(keyBuffer, key, keybits/8);
    drvRc = AesExtDmaSetKey(mode, keyBuffer, keySizeId);
    if (drvRc != AES_DRV_OK)
    {
        goto end;
    }
    return AES_DRV_OK;

end:
    rc = terminateAesExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return drvRc;
}

int mbedtls_aes_ext_dma_set_data_size(uint32_t dataSize, CCAesOperationMode_t operationMode)
{
    drvError_t drvRc = AES_DRV_OK;
    drvError_t rc = AES_DRV_OK;

    /* check if the operation mode is legal */
    if ( (operationMode != CC_AES_MODE_ECB) &&
         (operationMode != CC_AES_MODE_CBC) &&
         (operationMode != CC_AES_MODE_CTR) &&
         (operationMode != CC_AES_MODE_CBC_MAC) &&
         (operationMode != CC_AES_MODE_CMAC)  &&
         (operationMode != CC_AES_MODE_OFB)) {
        drvRc = EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }
    /* Check the dataSize */
    if ( (dataSize == 0) || (dataSize > CPU_DIN_MAX_SIZE) ){
        drvRc = EXT_DMA_ILLEGAL_INPUT_SIZE_ERROR;
        goto end;
    }
    if ( ( (operationMode == CC_AES_MODE_ECB) ||
            (operationMode == CC_AES_MODE_CBC) ||
            (operationMode == CC_AES_MODE_CBC_MAC) ) &&
            (dataSize % AES_BLOCK_SIZE != 0)  ){
        drvRc = EXT_DMA_ILLEGAL_INPUT_SIZE_ERROR;
        goto end;
    }

    /* Set data size */
    AesExtDmaSetDataSize(dataSize);
    return drvRc;

end:
    rc = terminateAesExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return drvRc;
}


int mbedtls_aes_ext_dma_set_iv(
    CCAesOperationMode_t operationMode,
    unsigned char       * iv,
    unsigned int          iv_size)
{

    drvError_t drvRc = AES_DRV_OK;
    int rc;
    aesMode_t  mode;
    uint32_t   ivBuff[CC_AES_IV_SIZE_IN_WORDS];

    /* check if the operation mode is legal */
    if ( (operationMode != CC_AES_MODE_ECB) &&
         (operationMode != CC_AES_MODE_CBC) &&
         (operationMode != CC_AES_MODE_CTR) &&
         (operationMode != CC_AES_MODE_CBC_MAC) &&
         (operationMode != CC_AES_MODE_CMAC)  &&
         (operationMode != CC_AES_MODE_OFB)) {
        drvRc = EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }

    if (iv == NULL || iv_size != CC_AES_IV_SIZE_IN_BYTES) {
        drvRc = EXT_DMA_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;
        goto end;
    }
    mode = CC2DriverAesMode(operationMode);
    CC_PalMemCopy((uint8_t *)ivBuff, iv, CC_AES_IV_SIZE_IN_BYTES);

    drvRc = AesExtDmaSetIv(mode, ivBuff);
    if (drvRc != AES_DRV_OK)
    {
        goto end;
    }
    return AES_DRV_OK;
end:
    rc = terminateAesExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return drvRc;
}


int mbedtls_aes_ext_dma_finish(
    CCAesOperationMode_t operationMode,
    unsigned char       *iv,
    unsigned int         iv_size )
{

    drvError_t      drvRc = AES_DRV_OK;
    drvError_t      rc = AES_DRV_OK;
    aesMode_t       mode;
    uint32_t        ivBuff[CC_AES_IV_SIZE_IN_WORDS];

    /* check if the operation mode is legal */
    if ( (operationMode != CC_AES_MODE_ECB) &&
         (operationMode != CC_AES_MODE_CBC) &&
         (operationMode != CC_AES_MODE_CTR) &&
         (operationMode != CC_AES_MODE_CBC_MAC) &&
         (operationMode != CC_AES_MODE_CMAC)  &&
         (operationMode != CC_AES_MODE_OFB)) {
        drvRc = EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }

    if ((operationMode == CC_AES_MODE_CBC_MAC || operationMode==CC_AES_MODE_CMAC) &&
        (iv == NULL || iv_size != CC_AES_IV_SIZE_IN_BYTES) ) {
        drvRc = EXT_DMA_AES_INVALID_IV_OR_TWEAK_PTR_ERROR;
        goto end;
    }
    mode = CC2DriverAesMode(operationMode);

    drvRc = finalizeAesExtDma(mode, ivBuff);

    if ((operationMode == CC_AES_MODE_CBC_MAC || operationMode==CC_AES_MODE_CMAC) && (drvRc == AES_DRV_OK)) {
        CC_PalMemCopy(iv, (uint8_t *)ivBuff, CC_AES_IV_SIZE_IN_BYTES);
    }

end:
    rc = terminateAesExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return drvRc;

}
