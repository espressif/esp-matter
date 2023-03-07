/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)

#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "cc_pal_mem.h"
#include "cc_hal.h"
#include "aesgcm_driver.h"
#include "cc_pal_abort.h"
#include "cc_pal_mutex.h"
#include "cc_util_pm.h"

extern CC_PalMutex CCSymCryptoMutex;

/******************************************************************************
*               PRIVATE FUNCTIONS
******************************************************************************/

void loadAesGcmGhashSubkey(AesGcmContext_t *pAesGcmCtx)
{
    /* Set H as the GHASH Key */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_SUBKEY_0_0), pAesGcmCtx->H[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_SUBKEY_0_1), pAesGcmCtx->H[1]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_SUBKEY_0_2), pAesGcmCtx->H[2]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_SUBKEY_0_3), pAesGcmCtx->H[3]);

    return;
}
void loadAesGcmKey(AesGcmContext_t *pAesGcmCtx)
{
    /* load key0 [127:0]*/
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_0) ,pAesGcmCtx->keyBuf[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_1) ,pAesGcmCtx->keyBuf[1]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_2) ,pAesGcmCtx->keyBuf[2]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_3) ,pAesGcmCtx->keyBuf[3]);

    if (pAesGcmCtx->keySizeId > KEY_SIZE_128_BIT) {
        /* load key0 [191:128]*/
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_4) ,pAesGcmCtx->keyBuf[4]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_5) ,pAesGcmCtx->keyBuf[5]);
    }

    if (pAesGcmCtx->keySizeId > KEY_SIZE_192_BIT) {
        /* load key0 [255:191]*/
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_6) ,pAesGcmCtx->keyBuf[6]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_KEY_0_7) ,pAesGcmCtx->keyBuf[7]);
    }

    return;
}

void inc32AesGcmJ0(AesGcmContext_t *pAesGcmCtx)
{
    uint32_t tmpWord = SWAP_ENDIAN(pAesGcmCtx->J0[3]);
    /* --- Inc32 LSW --- */
    /* Check overlap and inc. by 1 */
    if (BITMASK(CC_BITS_IN_32BIT_WORD) != tmpWord) {
        tmpWord++;
    }
    else {
        tmpWord = 0;
    }

    CC_PalMemCopy(pAesGcmCtx->aesCntrBuf, pAesGcmCtx->J0, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    pAesGcmCtx->aesCntrBuf[3] = SWAP_ENDIAN(tmpWord);

    return;
}

void loadAesGcmCntr(AesGcmContext_t *pAesGcmCtx)
{
    /* Write the initial counter value */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0), pAesGcmCtx->aesCntrBuf[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1), pAesGcmCtx->aesCntrBuf[1]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2), pAesGcmCtx->aesCntrBuf[2]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3), pAesGcmCtx->aesCntrBuf[3]);

    return;
}

void storeAesGcmCntr(AesGcmContext_t *pAesGcmCtx)
{
    /* Read the AES counter value */
    pAesGcmCtx->aesCntrBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_0));
    pAesGcmCtx->aesCntrBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_1));
    pAesGcmCtx->aesCntrBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_2));
    pAesGcmCtx->aesCntrBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CTR_0_3));

    return;
}

void loadAesGcmGhashIV(AesGcmContext_t *pAesGcmCtx)
{
    /* Write the initial counter value according to mode */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_0), pAesGcmCtx->ghashResBuf[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_1), pAesGcmCtx->ghashResBuf[1]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_2), pAesGcmCtx->ghashResBuf[2]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_3), pAesGcmCtx->ghashResBuf[3]);

    return;
}

void storeAesGcmGhashIV(AesGcmContext_t *pAesGcmCtx, CCBool storeInJ0)
{
    /* Write the result in GHASH Result Buffer */
    pAesGcmCtx->ghashResBuf[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_0));
    pAesGcmCtx->ghashResBuf[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_1));
    pAesGcmCtx->ghashResBuf[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_2));
    pAesGcmCtx->ghashResBuf[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_IV_0_3));

    if(CC_TRUE == storeInJ0) {
        /* Write the result in J0 */
        CC_PalMemCopy(pAesGcmCtx->J0, pAesGcmCtx->ghashResBuf, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    }

    return;
}

drvError_t initAesGcm(AesGcmContext_t* pAesGcmCtx)
{
    uint32_t auxReg = 0;
    uint32_t irrVal = 0;

    /* Verify user context pointer */
    if (NULL == pAesGcmCtx) {
            return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
    }

    /* verify valid dir */
    if ( (pAesGcmCtx->dir != CRYPTO_DIRECTION_ENCRYPT) &&
         (pAesGcmCtx->dir != CRYPTO_DIRECTION_DECRYPT) ) {
            return AES_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR;
    }

    /* Set to 0x00 */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), 0x00);

    /* Configure cores according to the process mode */
    switch (pAesGcmCtx->processMode) {
    case DRV_AESGCM_Process_CalcH:
        /* Set cryptographic flow to AES */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_DIN_AES_DOUT_VAL);
        /********************************************************************************/
        /***                      AES Configurations                                  ***/
        /********************************************************************************/
        /* Set Direction: Enc */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, auxReg, CRYPTO_DIRECTION_ENCRYPT);
        /* Set AES Mode to ECB */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, auxReg, CIPHER_ECB);
        /* Set AES Key length */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, auxReg, pAesGcmCtx->keySizeId);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL), auxReg);

        /* Load Key */
        loadAesGcmKey(pAesGcmCtx);

        /* Set to 0x10 */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), AES_BLOCK_SIZE);

        break;
    case DRV_AESGCM_Process_CalcJ0_FirstPhase:
    case DRV_AESGCM_Process_A:
        /* Set the GHASH Engine to be ready to a new GHASH operation */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, GHASH_INIT), GHASH_INIT_SET_VAL);
        /* Falls through. */
    case DRV_AESGCM_Process_CalcJ0_SecondPhase:
        /* Set cryptographic flow to HASH */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_HASH_MODE_VAL);
        /********************************************************************************/
        /***                    GHASH Configurations                                  ***/
        /********************************************************************************/
        /* Set H as the GHASH Key */
        loadAesGcmGhashSubkey(pAesGcmCtx);

        /* Set '0' as the GHASH IV */
        loadAesGcmGhashIV(pAesGcmCtx);

        /* Set Hash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, HASH_SEL_AES_MAC, auxReg, HASH_SEL_HASH_MOD);
        /* Set Ghash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, GHASH_SEL, auxReg, GHASH_SEL_GHASH_MOD);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), auxReg);

        /* Set '0' as the GHASH IV */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_XOR_DIN), HASH_XOR_DATA_VAL);

        break;
    case DRV_AESGCM_Process_DataIn:
        /* Set cryptographic flow according to crypto direction */
        if (CRYPTO_DIRECTION_ENCRYPT == pAesGcmCtx->dir) {
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_AES_TO_HASH_AND_DOUT_VAL);
        } else {
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_DIN_AES_AND_HASH_VAL);
        }
        /********************************************************************************/
        /***                      AES Configurations                                  ***/
        /********************************************************************************/
        /* Set Direction: Enc */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, auxReg, CRYPTO_DIRECTION_ENCRYPT);
        /* Set AES Mode to CTR */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, auxReg, CIPHER_CTR);
        /* Set AES Key length */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, auxReg, pAesGcmCtx->keySizeId);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL), auxReg);

        if(CC_FALSE == pAesGcmCtx->J0Inc32DoneFlg) {
            inc32AesGcmJ0(pAesGcmCtx);
            pAesGcmCtx->J0Inc32DoneFlg = CC_TRUE;
        }

        /* Load Counter */
        loadAesGcmCntr(pAesGcmCtx);

        /* Load Key */
        loadAesGcmKey(pAesGcmCtx);

        /* Set to data size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), pAesGcmCtx->dataSize);

        /********************************************************************************/
        /***                    GHASH Configurations                                  ***/
        /********************************************************************************/
        /* Set H as the GHASH Key */
        loadAesGcmGhashSubkey(pAesGcmCtx);
        /* Set former GHASH result as the new GHASH_IV value */
        loadAesGcmGhashIV(pAesGcmCtx);

        /* Set Hash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, HASH_SEL_AES_MAC, auxReg, HASH_SEL_HASH_MOD);
        /* Set Ghash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, GHASH_SEL, auxReg, GHASH_SEL_GHASH_MOD);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), auxReg);

        /* Set '0' as the GHASH IV */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_XOR_DIN), HASH_XOR_DATA_VAL);
        break;
    case DRV_AESGCM_Process_LenA_LenC:
        /* Set cryptographic flow to HASH */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_HASH_MODE_VAL);
        /********************************************************************************/
        /***                    GHASH Configurations                                  ***/
        /********************************************************************************/
        /* Set H as the GHASH Key */
        loadAesGcmGhashSubkey(pAesGcmCtx);
        /* Set former GHASH result as the new GHASH_IV value */
        loadAesGcmGhashIV(pAesGcmCtx);

        /* Set Hash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, HASH_SEL_AES_MAC, auxReg, HASH_SEL_HASH_MOD);
        /* Set Ghash select module */
        CC_REG_FLD_SET(HOST_RGF, HASH_SEL_AES_MAC, GHASH_SEL, auxReg, GHASH_SEL_GHASH_MOD);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), auxReg);

        /* Set '0' as the GHASH IV */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_XOR_DIN), HASH_XOR_DATA_VAL);
        break;
    case DRV_AESGCM_Process_GctrFinal:
        /* Set cryptographic flow to AES */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL), CONFIG_DIN_AES_DOUT_VAL);
        /********************************************************************************/
        /***                      AES Configurations                                  ***/
        /********************************************************************************/
        /* Set Direction: Enc */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, auxReg, CRYPTO_DIRECTION_ENCRYPT);
        /* Set AES Mode to CTR */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, auxReg, CIPHER_CTR);
        /* Set AES Key length */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, auxReg, pAesGcmCtx->keySizeId);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CONTROL), auxReg);

        /* Load Counter */
        CC_PalMemCopy(pAesGcmCtx->aesCntrBuf, pAesGcmCtx->J0, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
        loadAesGcmCntr(pAesGcmCtx);

        /* Load Key */
        loadAesGcmKey(pAesGcmCtx);

        /* Copy GHASH result to the Input address */
        CC_PalMemCopy(pAesGcmCtx->tempBuf, pAesGcmCtx->ghashResBuf, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);

        /* Set to GHASH Output size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_REMAINING_BYTES), CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
        break;
    default:
        return AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
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

    return AES_DRV_OK;
}


/******************************************************************************
*               PUBLIC FUNCTIONS
******************************************************************************/

drvError_t ProcessAesGcm(AesGcmContext_t *pAesGcmCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize)
{
    drvError_t drvRc  = AES_DRV_OK;
    uint32_t   irrVal = 0;
    uint32_t regVal = 0;
    uint32_t inputDataAddr, outputDataAddr;

    /* check input parameters */
    if ( (pInputBuffInfo == NULL) || (pOutputBuffInfo == NULL)) {
         return AES_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
    }

    /* Verify user context pointer */
    if (NULL == pAesGcmCtx) {
        return AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
    }

    /* Verify valid block size */
    if (blockSize >= DLLI_MAX_BUFF_SIZE) {
        return AES_DRV_ILLEGAL_MEM_SIZE_ERROR;
    }

    /* Lock mutex for all hw operation */
    if (CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE) != 0) {
        CC_PalAbort("Fail to acquire mutex\n");
    }

        /* increase CC counter at the beginning of each operation */
        drvRc = CC_IS_WAKE;
        if (drvRc != 0) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

    /********************************************************************************/
    /***                       Enable Clocks                                      ***/
    /********************************************************************************/
    /* Enable AES clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE),  SET_CLOCK_ENABLE);
    /* Enable Hash clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE), SET_CLOCK_ENABLE);
    /* Enable DMA clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE),  SET_CLOCK_ENABLE);

    /* Registers initializations */
    drvRc = initAesGcm(pAesGcmCtx);
    if (drvRc != AES_DRV_OK) {
            goto ProcessExit;
    }

    inputDataAddr = pInputBuffInfo->dataBuffAddr;
    outputDataAddr = pOutputBuffInfo->dataBuffAddr;

    /* configure the HW with the correct data buffer attributes (secure/non-secure) */
    CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
    CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_WRITE_HNONSEC, regVal, pOutputBuffInfo->dataBuffNs);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);

    /* For AES operations only */
    if((DRV_AESGCM_Process_CalcH     == pAesGcmCtx->processMode) ||
       (DRV_AESGCM_Process_DataIn    == pAesGcmCtx->processMode) ||
       (DRV_AESGCM_Process_GctrFinal == pAesGcmCtx->processMode)) {
        /* Configure destination address and size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD0) ,outputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD1) ,blockSize);
    }

    /* Configure source address and size */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,blockSize); // Kick operation

    /* Set DMA completion bit in IRR */
    CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);

    /* Wait for completion */
        drvRc = CC_HalWaitInterrupt(irrVal);
        if (drvRc != AES_DRV_OK) {
            goto ProcessExit_Err;
        }
    /* Post-processing */
    switch (pAesGcmCtx->processMode) {
        case DRV_AESGCM_Process_CalcH:
            break;
        case DRV_AESGCM_Process_CalcJ0_FirstPhase:
            storeAesGcmGhashIV(pAesGcmCtx, CC_FALSE);
            break;
        case DRV_AESGCM_Process_CalcJ0_SecondPhase:
            storeAesGcmGhashIV(pAesGcmCtx, CC_TRUE);
            break;
        case DRV_AESGCM_Process_A:
            storeAesGcmGhashIV(pAesGcmCtx, CC_FALSE);
            break;
        case DRV_AESGCM_Process_DataIn:
            storeAesGcmCntr(pAesGcmCtx);
            storeAesGcmGhashIV(pAesGcmCtx, CC_FALSE);
            break;
        case DRV_AESGCM_Process_LenA_LenC:
            storeAesGcmGhashIV(pAesGcmCtx, CC_FALSE);
            break;
        case DRV_AESGCM_Process_GctrFinal:
            break;
        default:
            drvRc = AES_DRV_ILLEGAL_OPERATION_MODE_ERROR;
            break;
        }

ProcessExit:

    /********************************************************************************/
    /***        Reset to default values in case of operation completion           ***/
    /********************************************************************************/
    if((DRV_AESGCM_Process_CalcH     != pAesGcmCtx->processMode) &&
       (DRV_AESGCM_Process_GctrFinal != pAesGcmCtx->processMode)) {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_EN) ,1);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AUTO_HW_PADDING) ,0);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_CFG) ,0);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), 0);
    }

ProcessExit_Err:
    /********************************************************************************/
    /***                      Disable Clocks                                      ***/
    /********************************************************************************/
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE),  SET_CLOCK_DISABLE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE), SET_CLOCK_DISABLE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE),  SET_CLOCK_DISABLE);

        /* decrease CC counter at the end of each operation */
        if (CC_IS_IDLE != 0) {
            CC_PalAbort("Fail to decrease PM counter\n");
        }

        /* Release mutex */
    if (CC_PalMutexUnlock(&CCSymCryptoMutex) != 0) {
        CC_PalAbort("Fail to release mutex\n");
    }

    return drvRc;
}
#endif
