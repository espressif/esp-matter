/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_sram_map.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "aesccm_driver.h"
#include "cc_util_pm.h"

extern CC_PalMutex CCSymCryptoMutex;


/******************************************************************************
*               PRIVATE FUNCTIONS
******************************************************************************/

static drvError_t InitAesCcm(AesCcmContext_t   *pAesCcmCtx)
{
        uint32_t aesControl = 0;
        uint32_t irrVal = 0;

        /* verify user context pointer */
        if ( pAesCcmCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* verify valid dir */
        if ( (pAesCcmCtx->dir != CRYPTO_DIRECTION_ENCRYPT) &&
             (pAesCcmCtx->dir != CRYPTO_DIRECTION_DECRYPT) ) {
                return AES_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR;
        }

        /* veriy mode and set aes control */
        switch (pAesCcmCtx->mode) {
        case CIPHER_CBC_MAC:
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesControl, CRYPTO_DIRECTION_ENCRYPT);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesControl, CIPHER_CBC_MAC);
                break;
        case CIPHER_CTR:
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesControl, pAesCcmCtx->dir);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesControl, CIPHER_CTR);
                break;
        case CIPHER_CCMPE:
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesControl, pAesCcmCtx->dir);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesControl, CIPHER_CTR);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY1, aesControl, CIPHER_CBC_MAC);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_TUNNEL_IS_ON, aesControl, 1);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_TUN_B1_USES_PADDED_DATA_IN, aesControl, 1);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_TUNNEL0_ENCRYPT, aesControl, 1);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_OUTPUT_MID_TUNNEL_DATA, aesControl, 1);
                break;
        case CIPHER_CCMPD:
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesControl, pAesCcmCtx->dir);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesControl, CIPHER_CTR);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY1, aesControl, CIPHER_CBC_MAC);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_TUNNEL_IS_ON, aesControl, 1);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_OUTPUT_MID_TUNNEL_DATA, aesControl, 1);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, AES_TUNNEL_B1_PAD_EN, aesControl, 1);
                break;
        default:
                return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        }

        /* verify keySizeID */
        switch (pAesCcmCtx->keySizeId) {
        case KEY_SIZE_128_BIT:
        case KEY_SIZE_192_BIT:
        case KEY_SIZE_256_BIT:
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, aesControl, pAesCcmCtx->keySizeId);
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY1, aesControl, pAesCcmCtx->keySizeId);
                break;
        default:
                return AES_DRV_ILLEGAL_KEY_SIZE_ERROR;
        }

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
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SYM_DMA_COMPLETED_MASK, irrVal, 0);
        CC_HalMaskInterrupt(irrVal);

        /* configure DIN-AES-DOUT */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL) ,CONFIG_DIN_AES_DOUT_VAL);

    /* Zero AES_REMAINING_BYTES */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES) ,0);

    /* configure AES control */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL) ,aesControl);

        return AES_DRV_OK;
}

static void LoadAesCcmdKey(AesCcmContext_t   *pAesCcmCtx)
{
        /* load key0 [127:0]*/
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_0) ,pAesCcmCtx->keyBuf[0]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_1) ,pAesCcmCtx->keyBuf[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_2) ,pAesCcmCtx->keyBuf[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_3) ,pAesCcmCtx->keyBuf[3]);

        /* load key1 [127:0]*/
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_0) ,pAesCcmCtx->keyBuf[0]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_1) ,pAesCcmCtx->keyBuf[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_2) ,pAesCcmCtx->keyBuf[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_3) ,pAesCcmCtx->keyBuf[3]);

        if (pAesCcmCtx->keySizeId >= KEY_SIZE_192_BIT) {
                /* load key0 [191:128]*/
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_4) ,pAesCcmCtx->keyBuf[4]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_5) ,pAesCcmCtx->keyBuf[5]);
                /* load key1 [191:128]*/
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_4) ,pAesCcmCtx->keyBuf[4]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_5) ,pAesCcmCtx->keyBuf[5]);
        }

        if (pAesCcmCtx->keySizeId == KEY_SIZE_256_BIT) {
                /* load key0 [255:191]*/
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_6) ,pAesCcmCtx->keyBuf[6]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_7) ,pAesCcmCtx->keyBuf[7]);
                /* load key1 [255:191]*/
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_6) ,pAesCcmCtx->keyBuf[6]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_1_7) ,pAesCcmCtx->keyBuf[7]);
        }

        return;
}

static void LoadAesCcmIvState(AesCcmContext_t   *pAesCcmCtx)
{
        if (pAesCcmCtx->mode == CIPHER_CBC_MAC) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0) ,pAesCcmCtx->ivBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1) ,pAesCcmCtx->ivBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2) ,pAesCcmCtx->ivBuf[2]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3) ,pAesCcmCtx->ivBuf[3]);
        } else {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_0) ,pAesCcmCtx->ivBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_1) ,pAesCcmCtx->ivBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_2) ,pAesCcmCtx->ivBuf[2]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_3) ,pAesCcmCtx->ivBuf[3]);
        }

        return;
}

static void LoadAesCcmCtrState(AesCcmContext_t   *pAesCcmCtx)
{
        /* write the initial counter value according to mode */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0) ,pAesCcmCtx->ctrStateBuf[0]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1) ,pAesCcmCtx->ctrStateBuf[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2) ,pAesCcmCtx->ctrStateBuf[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3) ,pAesCcmCtx->ctrStateBuf[3]);

        return;
}


static void StoreAesCcmIvState(AesCcmContext_t   *pAesCcmCtx)
{
        if (pAesCcmCtx->mode == CIPHER_CBC_MAC) {
                pAesCcmCtx->ivBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0));
                pAesCcmCtx->ivBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1));
                pAesCcmCtx->ivBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2));
                pAesCcmCtx->ivBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3));
        } else {
                pAesCcmCtx->ivBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_0));
                pAesCcmCtx->ivBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_1));
                pAesCcmCtx->ivBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_2));
                pAesCcmCtx->ivBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_1_3));
        }

        return;
}


static void StoreAesCcmCtrState(AesCcmContext_t   *pAesCcmCtx)
{
        /* write the initial counter value according to mode */
        pAesCcmCtx->ctrStateBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0));
        pAesCcmCtx->ctrStateBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1));
        pAesCcmCtx->ctrStateBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2));
        pAesCcmCtx->ctrStateBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3));

        return;
}


/******************************************************************************
*               PUBLIC FUNCTIONS
******************************************************************************/

drvError_t ProcessAesCcmDrv(AesCcmContext_t   *pAesCcmCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize)
{
        uint32_t irrVal = 0;
        drvError_t drvRc = AES_DRV_OK;
        uint32_t regVal = 0;
        uint32_t inputDataAddr, outputDataAddr;

        /* check input parameters */
        if ( (pInputBuffInfo == NULL) || (pOutputBuffInfo == NULL)) {
             return AES_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
        }

        /* verify user context pointer */
        if ( pAesCcmCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* verify valid block size */
    if (blockSize >= DLLI_MAX_BUFF_SIZE) {
                return AES_DRV_ILLEGAL_MEM_SIZE_ERROR;
    }

        /* lock mutex for more aes hw operation */
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
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_ENABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_ENABLE);

        drvRc = InitAesCcm(pAesCcmCtx);
        if (drvRc != AES_DRV_OK) {
                goto ProcessExit;
        }

        /* load key0 and key1 */
        LoadAesCcmdKey(pAesCcmCtx);

        inputDataAddr = pInputBuffInfo->dataBuffAddr;
        outputDataAddr = pOutputBuffInfo->dataBuffAddr;

        /* configure the HW with the correct data buffer attributes (secure/non-secure) */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_WRITE_HNONSEC, regVal, pOutputBuffInfo->dataBuffNs);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);

        /* load CTR in case of CTR / CCMPE / CCMPD */
        if (pAesCcmCtx->mode != CIPHER_CBC_MAC) {
        /* write the initial counter value according to mode */
                LoadAesCcmCtrState(pAesCcmCtx);

                /* configure destination address and size in case of ctr */
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD0) ,outputDataAddr);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD1) ,blockSize);
        }

        /* load IV in case of CBC_MAC / CCMPE / CCMPD */
        if (pAesCcmCtx->mode != CIPHER_CTR) {
        /* write the initial counter value according to mode */
                LoadAesCcmIvState(pAesCcmCtx);

        /* initiate CMAC sub-keys calculation */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CMAC_INIT) ,0x1);
        /* set the remaining bytes */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES) ,blockSize);

        }

        /* configure source address and size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,blockSize);

        /* set dma completion bit in irr */
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
        drvRc = CC_HalWaitInterrupt(irrVal);
        if (drvRc != AES_DRV_OK) {
            goto ProcessExit;
        }

        /* get CTR state in case of CTR / CCMPE / CCMPD */
        if (pAesCcmCtx->mode != CIPHER_CBC_MAC) {
                StoreAesCcmCtrState(pAesCcmCtx);
        }

        /* get IV state in case of CBC_MAC / CCMPE / CCMPD */
        if (pAesCcmCtx->mode != CIPHER_CTR) {
                StoreAesCcmIvState(pAesCcmCtx);
        }

ProcessExit:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_DISABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_DISABLE);

        /* decrease CC counter at the end of each operation */
        if (CC_IS_IDLE != 0) {
            CC_PalAbort("Fail to decrease PM counter\n");
        }

        /* release mutex */
        if (CC_PalMutexUnlock(&CCSymCryptoMutex) != 0) {
            CC_PalAbort("Fail to release mutex\n");
        }

        return drvRc;
}










