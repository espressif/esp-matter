/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "aes_driver_ext_dma.h"
#include "cc_util_pm.h"

extern CC_PalMutex CCSymCryptoMutex;




/******************************************************************************
*       PUBLIC FUNCTIONS
******************************************************************************/
drvError_t AesExtDmaInit(cryptoDirection_t   encryptDecryptFlag,
        aesMode_t operationMode,
        keySizeId_t          keySizeId)
{
        uint32_t aesCtrl = 0;
        uint32_t irrVal = 0;
        cryptoDirection_t dir;
        drvError_t rc = AES_DRV_OK;
        uint32_t pIv[AES_BLOCK_SIZE_WORDS] = {0};

        /* verify aes valid mode */
        switch (operationMode) {
        case CIPHER_ECB:
        case CIPHER_CBC:
        case CIPHER_CTR:
        case CIPHER_CBC_MAC:
        case CIPHER_CMAC:
        case CIPHER_OFB:
                break;
        default:
                return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        }

        /* verify aes valid dir */
        if ( (encryptDecryptFlag != CRYPTO_DIRECTION_ENCRYPT) &&
             (encryptDecryptFlag != CRYPTO_DIRECTION_DECRYPT) ) {
                return AES_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR;
        }


        /* lock mutex for more aes hw operation */
        rc = CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE);
        if (rc != 0) {
            CC_PalAbort("Fail to acquire mutex\n");
        }

        /* increase CC counter at the beginning of each operation */
        rc = CC_IS_WAKE;
        if (rc != 0) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* enable clock */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_ENABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_ENABLE);

        /* make sure sym engines are ready to use */
        CC_HAL_WAIT_ON_CRYPTO_BUSY();

        /* clear all interrupts before starting the engine */
        CC_HalClearInterruptBit(0xFFFFFFFFUL);

        /* mask dma interrupts which are not required */
        irrVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IMR));
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SRAM_TO_DIN_MASK, irrVal, 1);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_SRAM_MASK, irrVal, 1);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, MEM_TO_DIN_MASK, irrVal, 1);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_MEM_MASK, irrVal, 1);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SYM_DMA_COMPLETED_MASK, irrVal, 1);
        CC_HalMaskInterrupt(irrVal);

        /* configure DIN-AES-DOUT */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_DIN_AES_DOUT_VAL);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), 0);

        /* configure AES direction (in case of CMAC - force only encrypt) */
        if ((operationMode == CIPHER_CBC_MAC) || (operationMode == CIPHER_CMAC)){
            dir = CRYPTO_DIRECTION_ENCRYPT;
        }
        else {
            dir = encryptDecryptFlag;
        }
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesCtrl, dir);

        /* configure AES mode */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesCtrl, operationMode);
        switch (keySizeId) {
        case KEY_SIZE_128_BIT:
        case KEY_SIZE_192_BIT:
        case KEY_SIZE_256_BIT:
            /* NK_KEY0 and NK_KEY1 are configured, only NK_KEY0 is in use (no tunneling in cc3x)*/
            CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, aesCtrl, keySizeId);
            break;
        default:
            goto end;
        }

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL) ,aesCtrl);

        /* zeroize IV in CMAC */
        if (operationMode == CIPHER_CMAC) {
            rc = AesExtDmaSetIv(operationMode, pIv);
            if (rc != 0) {
                goto end;
            }
        }

        return AES_DRV_OK;

end:
        rc = terminateAesExtDma();
        if (rc != 0) {
            CC_PalAbort("Failed to terminateAesExtDma \n");
        }
        return AES_DRV_ILLEGAL_KEY_SIZE_ERROR;
}


void AesExtDmaSetDataSize(uint32_t dataSize)
{
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), dataSize);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_CPU_DATA_SIZE) , dataSize);
}

drvError_t AesExtDmaSetIv(aesMode_t mode, uint32_t *pIv)
{
    drvError_t rc = AES_DRV_OK;
    /* write the initial counter value according to mode */
    switch (mode) {
    case(CIPHER_CTR):
    case(CIPHER_OFB):
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0) ,pIv[0]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1) ,pIv[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2) ,pIv[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3) ,pIv[3]);
        break;
    case(CIPHER_CMAC):
    case(CIPHER_CBC):
    case(CIPHER_CBC_MAC):
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0) ,pIv[0]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1) ,pIv[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2) ,pIv[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3) ,pIv[3]);
        break;
    case(CIPHER_ECB):
        break;
    default:
        rc = AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
    }

    return rc;
}


drvError_t AesExtDmaStoreIv(aesMode_t mode, uint32_t *pIv)
{
    drvError_t rc = AES_DRV_OK;
    /* write the initial counter value according to mode */
    switch (mode) {

    case(CIPHER_CMAC):
    case(CIPHER_CBC_MAC):
        pIv[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0));
        pIv[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1));
        pIv[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2));
        pIv[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3));
        break;
    default:
        rc = AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
    }
    return rc;
}



drvError_t AesExtDmaSetKey(aesMode_t mode, uint32_t *keyBuf, keySizeId_t keySizeId)
{
    /* verify user context pointer */
    if ( keyBuf == NULL ) {
        return AES_DRV_ILLEGAL_KEY_SIZE_ERROR;
    }
    if ((keySizeId != KEY_SIZE_256_BIT) && (keySizeId != KEY_SIZE_192_BIT) && (keySizeId != KEY_SIZE_128_BIT)) {
        return AES_DRV_ILLEGAL_KEY_SIZE_ERROR;
    }

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_0) ,keyBuf[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_1) ,keyBuf[1]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_2) ,keyBuf[2]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_3) ,keyBuf[3]);

    if (keySizeId == KEY_SIZE_192_BIT || keySizeId == KEY_SIZE_256_BIT) {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_4) ,keyBuf[4]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_5) ,keyBuf[5]);
    }
    if (keySizeId == KEY_SIZE_256_BIT) {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_6) ,keyBuf[6]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_7) ,keyBuf[7]);
    }

    /* initiate CMAC sub-keys calculation */
    if (mode == CIPHER_CMAC) {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CMAC_INIT) ,0x1);
    }

    return AES_DRV_OK;
}


drvError_t finalizeAesExtDma(aesMode_t mode, uint32_t *pIv)
{
        drvError_t drvRc = AES_DRV_OK;

        if (mode == CIPHER_CMAC || mode== CIPHER_CBC_MAC)
        {
            drvRc = AesExtDmaStoreIv(mode, pIv);
        }

        return drvRc;
}

drvError_t terminateAesExtDma(void)
{
        drvError_t rc = AES_DRV_OK;

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_DISABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_DISABLE);

        /* decrease CC counter at the end of each operation */
        rc = CC_IS_IDLE;
        if (rc != 0) {
            CC_PalAbort("Fail to decrease PM counter\n");
        }

        /* unlock mutex for more aes hw operation */
        rc = CC_PalMutexUnlock(&CCSymCryptoMutex);
        if (rc != 0) {
            CC_PalAbort("Fail to unlock mutex\n");
        }
        return rc;
}

