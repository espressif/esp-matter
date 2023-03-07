/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_sram_map.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "dx_nvm.h"
#include "cc_otp_defs.h"
#include "mbedtls_cc_mng_int.h"
#include "cc_util_pm.h"
#include "cc_int_general_defs.h"

#define SET_ZEROS(buff, size) {\
    uint32_t i = 0;\
    for (i = 0; i< size; i++) {\
        buff[i] = 0x0;\
    }\
}


extern CC_PalMutex CCSymCryptoMutex;


/******************************************************************************
*               PRIVATE FUNCTIONS
******************************************************************************/

/* Note: InitAes is private function and it is called only by the driver! */
static drvError_t InitAes(AesContext_t *aesCtx)
{
        uint32_t aesCtrl = 0;
        uint32_t irrVal = 0;
        cryptoDirection_t dir;

        /* verify user context pointer */
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* verify aes valid mode */
        switch (aesCtx->mode) {
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
        if ( (aesCtx->dir != CRYPTO_DIRECTION_ENCRYPT) &&
             (aesCtx->dir != CRYPTO_DIRECTION_DECRYPT) ) {
                return AES_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR;
        }

        /* verify aes valid input addr type */
        if ( (aesCtx->inputDataAddrType != SRAM_ADDR) &&
             (aesCtx->inputDataAddrType != DLLI_ADDR) ) {
                return AES_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR;
        }

        /* verify aes valid output addr type for ecb, cbc, ctr */
        if ( (aesCtx->outputDataAddrType != SRAM_ADDR) &&
             (aesCtx->outputDataAddrType != DLLI_ADDR) ) {
                return AES_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR;
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

    /* configure AES direction (in case of CMAC - force only encrypt) */
    if ((aesCtx->mode == CIPHER_CBC_MAC) || (aesCtx->mode == CIPHER_CMAC))
        dir = CRYPTO_DIRECTION_ENCRYPT;
    else
        dir = aesCtx->dir;
    CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesCtrl, dir);

    /* configure AES mode */
    CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesCtrl, aesCtx->mode);
    switch (aesCtx->keySizeId) {
        case KEY_SIZE_128_BIT:
        case KEY_SIZE_192_BIT:
        case KEY_SIZE_256_BIT:
                /* NK_KEY0 and NK_KEY1 are configured, only NK_KEY0 is in use (no tunneling in cc3x)*/
                CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, aesCtrl, aesCtx->keySizeId);
                break;
        default:
                return AES_DRV_ILLEGAL_KEY_SIZE_ERROR;
        }
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL) ,aesCtrl);

        /* initiate CMAC sub-keys calculation */
        if (aesCtx->mode == CIPHER_CMAC) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CMAC_INIT) ,0x1);
        }

        return AES_DRV_OK;
}

static drvError_t LoadAesKey(AesContext_t *aesCtx)
{
        uint32_t   error = 0;
        uint32_t   isKeyInUse = 0;

        /* verify user context pointer */
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        switch (aesCtx->cryptoKey) {
        case USER_KEY:
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_0) ,aesCtx->keyBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_1) ,aesCtx->keyBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_2) ,aesCtx->keyBuf[2]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_3) ,aesCtx->keyBuf[3]);
                if (aesCtx->keySizeId == KEY_SIZE_192_BIT || aesCtx->keySizeId == KEY_SIZE_256_BIT) {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_4) ,aesCtx->keyBuf[4]);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_5) ,aesCtx->keyBuf[5]);
                }
                if (aesCtx->keySizeId == KEY_SIZE_256_BIT)
                {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_6) ,aesCtx->keyBuf[6]);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_7) ,aesCtx->keyBuf[7]);
                }
                break;
        case RKEK_KEY:
        /* Verify no integrity error */
        CC_IS_KEY_ERROR(KDR, error);
        if (error == CC_TRUE) {
            return AES_DRV_ILLEGAL_KEY_INTEGRITY_ERROR;
        }
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_CRYPTOKEY_SEL) ,aesCtx->cryptoKey);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_SK), 0x1);
                break;
        case KCP_KEY:
        /* Verify key is in use */
        CC_IS_OTP_KEY_IN_USE(OEM, KCP, error, isKeyInUse);
        if ( (error != CC_OK) || (isKeyInUse != CC_TRUE) ) {
            return AES_DRV_ILLEGAL_KEY_USE_ERROR;
        }

        /* Verify key is not locked */
        CC_IS_KEY_LOCKED(KCP, error);
        if (error == CC_TRUE) {
            return AES_DRV_ILLEGAL_KEY_LOCK_ERROR;
        }

        /* Verify no integrity error */
        CC_IS_KEY_ERROR(PROV, error);
        if (error == CC_TRUE) {
            return AES_DRV_ILLEGAL_KEY_INTEGRITY_ERROR;
        }

                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_CRYPTOKEY_SEL) ,aesCtx->cryptoKey);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_SK), 0x1);
                break;
        case KPICV_KEY:
        /* Verify key is in use */
        CC_IS_OTP_KEY_IN_USE(MANUFACTURE, KPICV, error, isKeyInUse);
        if ( (error != CC_OK) || (isKeyInUse != CC_TRUE) ) {
            return AES_DRV_ILLEGAL_KEY_USE_ERROR;
        }

        /* Verify key is not locked */
        CC_IS_KEY_LOCKED(KPICV, error);
        if (error == CC_TRUE) {
            return AES_DRV_ILLEGAL_KEY_LOCK_ERROR;
        }

        /* Verify no integrity error */
        CC_IS_KEY_ERROR(KPICV, error);
        if (error == CC_TRUE) {
            return AES_DRV_ILLEGAL_KEY_INTEGRITY_ERROR;
        }
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_CRYPTOKEY_SEL) ,aesCtx->cryptoKey);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_SK), 0x1);
                break;
        case RTL_KEY:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_CRYPTOKEY_SEL) ,1);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_SK), 0x1);
        break;
        default:
                return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;

       }

       /* For all HW keys, check if fatal error bit is set to ON */
       if (aesCtx->cryptoKey != USER_KEY) {
               CC_IS_FATAL_ERR_ON(error);
               if (error == CC_TRUE) {
                   return AES_DRV_ILLEGAL_FATAL_ERR_BIT_ERROR;
               }
       }
       return AES_DRV_OK;
}

static drvError_t LoadIVState(AesContext_t *aesCtx)
{
        /* verify user context pointer */
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* write the initial counter value according to mode */
        switch (aesCtx->mode) {
    case(CIPHER_CTR):
    case(CIPHER_OFB):
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0) ,aesCtx->ivBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1) ,aesCtx->ivBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2) ,aesCtx->ivBuf[2]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3) ,aesCtx->ivBuf[3]);
                break;
        case(CIPHER_CMAC):
        case(CIPHER_CBC):
    case(CIPHER_CBC_MAC):
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0) ,aesCtx->ivBuf[0]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1) ,aesCtx->ivBuf[1]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2) ,aesCtx->ivBuf[2]);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3) ,aesCtx->ivBuf[3]);
        case(CIPHER_ECB):
                break;
        default:
                return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        }

        return AES_DRV_OK;
}

static drvError_t StoreIVState(AesContext_t *aesCtx)
{
        /* verify user context pointer */
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* write the initial counter value according to mode */
        switch (aesCtx->mode) {
    case(CIPHER_CTR):
    case(CIPHER_OFB):
                aesCtx->ivBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0));
                aesCtx->ivBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1));
                aesCtx->ivBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2));
                aesCtx->ivBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3));
                break;
        case(CIPHER_CMAC):
        case(CIPHER_CBC):
    case(CIPHER_CBC_MAC):
                aesCtx->ivBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_0));
                aesCtx->ivBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_1));
                aesCtx->ivBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_2));
                aesCtx->ivBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_IV_0_3));
        case(CIPHER_ECB):
                break;
        default:
                return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        }

        return AES_DRV_OK;

}

static drvError_t finalizeCmac(AesContext_t *aesCtx, CCBuffInfo_t *pInputBuffInfo, uint32_t blockSize)
{
        drvError_t drvRc = AES_DRV_OK;
        uint32_t irrVal = 0;
        uint32_t regVal = 0;
        uint32_t inputDataAddr;

        /* check input parameters */
        if (pInputBuffInfo == NULL) {
             return AES_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
        }


    if (((aesCtx->inputDataAddrType == SRAM_ADDR) && (blockSize >= CC_SRAM_MAX_SIZE)) ||
        ((aesCtx->inputDataAddrType == DLLI_ADDR) && (blockSize >= DLLI_MAX_BUFF_SIZE))) {
                return AES_DRV_ILLEGAL_MEM_SIZE_ERROR;
    }

    // In ARM CryptoCell 3xx we can't have last block == 0, since as NIST 800-38B says:
    // "the final block in the partition is replaced with the exclusive-OR of the final block with the first subkey"
    // In ARM CryptoCell 3xx we enable/disable the AES clock for every block, so HW doesn't keep the first key and nither does the driver.
    // so we dont have the first key and we cant xor with it.
        if ((blockSize == 0) && (aesCtx->dataBlockType != FIRST_BLOCK)) {
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

        drvRc = InitAes(aesCtx);
        if (drvRc != AES_DRV_OK) {
                goto FinishExit;
        }

        /* load AES key and iv length and digest */
        drvRc = LoadAesKey(aesCtx);
        if (drvRc != 0){
            goto FinishExit;
        }

        drvRc = LoadIVState(aesCtx);
        if (drvRc != 0){
            goto FinishExit;
        }

        /* initiate CMAC sub-keys calculation */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CMAC_INIT) ,0x1);
        /* set the remaining bytes */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES) ,blockSize);

        inputDataAddr = pInputBuffInfo->dataBuffAddr;

        /* configure the HW with the correct data buffer attributes (secure/non-secure) */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);


        if (blockSize == 0) {
                if (aesCtx->dataBlockType == FIRST_BLOCK) {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CMAC_SIZE0_KICK) ,0x1);
                }
        } else {
                /* configure source address and size, set the relevant IRR bit */
                if (aesCtx->inputDataAddrType == DLLI_ADDR) {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,blockSize);
                } else {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_SRC_ADDR) ,inputDataAddr);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_SRAM_BYTES_LEN) ,blockSize);
                }
                /* set dma completion bit in irr */
                CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
                drvRc = CC_HalWaitInterrupt(irrVal);
                if (drvRc != 0){
                    goto FinishExit;
                }
        }

        /* get machine state */
        StoreIVState(aesCtx);
        /* Note: the upper layer should copy the mac result to outputDataAddr using PAL */

FinishExit:
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



/******************************************************************************
*       PUBLIC FUNCTIONS
******************************************************************************/

drvError_t ProcessAesDrv(AesContext_t *aesCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize)
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
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }
        if (((aesCtx->inputDataAddrType == SRAM_ADDR) && (blockSize >= CC_SRAM_MAX_SIZE)) ||
            ((aesCtx->inputDataAddrType == DLLI_ADDR) && (blockSize >= DLLI_MAX_BUFF_SIZE))) {
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

        drvRc = InitAes(aesCtx);
        if (drvRc != AES_DRV_OK) {
                goto ProcessExit;
        }

        drvRc = LoadAesKey(aesCtx);
        if (drvRc != AES_DRV_OK){
            goto ProcessExit;
        }

        drvRc = LoadIVState(aesCtx);
        if (drvRc != AES_DRV_OK) {
            goto ProcessExit;
        }

        inputDataAddr = pInputBuffInfo->dataBuffAddr;
        outputDataAddr = pOutputBuffInfo->dataBuffAddr;

        /* configure the HW with the correct data buffer attributes (secure/non-secure) */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_WRITE_HNONSEC, regVal, pOutputBuffInfo->dataBuffNs);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);

        if ((aesCtx->mode != CIPHER_CBC_MAC) && (aesCtx->mode != CIPHER_CMAC)) {
                /* configure destination address and size in case of ecb or cbc or ctr */
                /* and set dout bit in irr */
                if (aesCtx->outputDataAddrType == DLLI_ADDR) {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD0) ,outputDataAddr);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD1) ,blockSize);
                } else {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_DEST_ADDR) ,outputDataAddr);
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DOUT_SRAM_BYTES_LEN) ,blockSize);
                }
        }

        /* configure source address and size */
        if (aesCtx->inputDataAddrType == DLLI_ADDR) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,blockSize);
        } else {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_SRC_ADDR) ,inputDataAddr);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_SRAM_BYTES_LEN) ,blockSize);
        }

        /* set dma completion bit in irr */
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
    drvRc = CC_HalWaitInterrupt(irrVal);
    if (drvRc != 0){
        goto ProcessExit;
    }

        /* get machine state */
        StoreIVState(aesCtx);

        /* at least one block of data processed */
        aesCtx->dataBlockType =  MIDDLE_BLOCK;

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


drvError_t FinishAesDrv(AesContext_t *aesCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize)
{
        drvError_t drvRc = AES_DRV_OK;

        /* check input parameters */
        if ( (pInputBuffInfo == NULL) || (pOutputBuffInfo == NULL)) {
             return AES_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
        }

        /* verify user context pointer */
        if ( aesCtx == NULL ) {
                return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        switch (aesCtx->mode) {
        case CIPHER_CMAC:
                drvRc = finalizeCmac(aesCtx, pInputBuffInfo, blockSize);
                break;
        default:
                if (blockSize == 0) {
                        if ((aesCtx->mode == CIPHER_CBC_MAC) && (aesCtx->dataBlockType == FIRST_BLOCK)) { // according to ISO/IEC 9797-1 sec 6.1.3
                                blockSize = sizeof(aesCtx->tempBuff);
                                SET_ZEROS(aesCtx->tempBuff, blockSize);
                                pInputBuffInfo->dataBuffAddr = (uint32_t)aesCtx->tempBuff;
                        } else
                                return AES_DRV_OK;
                }
                drvRc = ProcessAesDrv(aesCtx, pInputBuffInfo, pOutputBuffInfo, blockSize);
        }

        return drvRc;
}




