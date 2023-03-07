/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "chacha_driver.h"
#include "chacha_driver_ext_dma.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "cc_util_pm.h"


extern CC_PalMutex CCSymCryptoMutex;

/* chacha mode, poly1305 disabled, 256 bit key, 20 rounds, 64 bit iv, do not reset the block counter (overwritten by the context) */
#define CHACHA_CONTROL_REG_VAL        (1 << DX_CHACHA_CONTROL_REG_INIT_FROM_HOST_BIT_SHIFT)
#define CHACHA_CONTROL_REG_USE_IV_96  (1 << DX_CHACHA_CONTROL_REG_USE_IV_96BIT_BIT_SHIFT)

/******************************************************************************
*               PRIVATE FUNCTIONS
******************************************************************************/

static drvError_t LoadChachaExtDmaState(uint32_t *pNonceBuf, chachaNonceSize_t nonceSizeFlag, uint32_t blockCounterLsb)
{
        /* verify user context pointer */
        if (pNonceBuf == NULL) {
                return CHACHA_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* write the initial counter value according to mode */
        if (nonceSizeFlag == NONCE_SIZE_64) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_BLOCK_CNT_MSB), 0);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_IV_0), pNonceBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_IV_1), pNonceBuf[1]);
        }
        else if (nonceSizeFlag == NONCE_SIZE_96) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_BLOCK_CNT_MSB), pNonceBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_IV_0), pNonceBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_IV_1), pNonceBuf[2]);
        }
        else {
                return CHACHA_DRV_ILLEGAL_NONCE_SIZE_ERROR;
        }
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_BLOCK_CNT_LSB), blockCounterLsb);

        return CHACHA_DRV_OK;
}

static drvError_t LoadChachaKeyExtDma(uint32_t *pKey)
{
        int enrtyNum = 0;

        /* verify user context pointer */
        if (pKey == NULL) {
            return CHACHA_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        for (enrtyNum = 0; enrtyNum < CHACHA_256_BIT_KEY_SIZE_WORDS; ++enrtyNum) {
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_KEY0) + (sizeof(uint32_t) * enrtyNum), pKey[enrtyNum]);
        }

        return CHACHA_DRV_OK;
}

/******************************************************************************
*               PUBLIC FUNCTIONS
******************************************************************************/
drvError_t InitChachaExtDma(uint32_t *pNonceBuf, chachaNonceSize_t nonceSizeFlag, uint32_t *keyBuf, uint32_t initialCounter, uint32_t dataLen)
{
    uint32_t irrVal = 0;
    uint32_t chachaCtrl = CHACHA_CONTROL_REG_VAL;
    drvError_t drvRc = CHACHA_DRV_OK;
    drvError_t rc = AES_DRV_OK;

    if (pNonceBuf == NULL || keyBuf == NULL) {
        return CHACHA_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
    }
    /* lock mutex for more chacha hw operation */
    drvRc = CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE);
    if (drvRc != 0) {
        CC_PalAbort("Fail to acquire mutex\n");
    }

    /* increase CC counter at the beginning of each operation */
    drvRc = CC_IS_WAKE;
    if (drvRc != 0) {
        CC_PalAbort("Fail to increase PM counter\n");
    }

    /* enable clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_CLK_ENABLE) ,SET_CLOCK_ENABLE);
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

    /* configure DIN-CHACHA-DOUT */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL) ,CONFIG_DIN_CHACHA_DOUT_VAL);

    /* write the initial counter value */
    drvRc = LoadChachaExtDmaState(pNonceBuf, nonceSizeFlag, initialCounter);
    if (drvRc != CHACHA_DRV_OK) {
            goto InitExit;
    }

    /* load key */
    drvRc = LoadChachaKeyExtDma(keyBuf);
    if (drvRc != CHACHA_DRV_OK) {
            goto InitExit;
    }

    /* configure the CHACHA mode */
    if (nonceSizeFlag == NONCE_SIZE_96) {
        chachaCtrl |= CHACHA_CONTROL_REG_USE_IV_96;
    }

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_CONTROL_REG), chachaCtrl);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_CPU_DATA_SIZE) , dataLen);

    return CHACHA_DRV_OK;

InitExit:

    rc = terminateChachaExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return drvRc;
}

drvError_t terminateChachaExtDma(void)
{
    drvError_t rc = AES_DRV_OK;

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CHACHA_CLK_ENABLE) ,SET_CLOCK_DISABLE);
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
