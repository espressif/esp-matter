/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stddef.h>

#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "hash_driver.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_sram_map.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"

#include "cc_common.h"
#include "cc_common_math.h"

#include "cc_util_pm.h"

extern CC_PalMutex CCSymCryptoMutex;

/**********************************************************************************************/
/************************ Private Functions ***************************************************/
/**********************************************************************************************/

static void UpdateHashFinish(HashContext_t  *hashCtx)
{
    /* read and store the hash data registers */
    switch(hashCtx->mode) {
    case HASH_SHA224:
    case HASH_SHA256:
        hashCtx->digest[7] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H7));
        hashCtx->digest[6] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H6));
        hashCtx->digest[5] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H5));
                /* Fall-through. */
    case HASH_SHA1:
        hashCtx->digest[4] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H4));
        hashCtx->digest[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H3));
        hashCtx->digest[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H2));
        hashCtx->digest[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H1));
        hashCtx->digest[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H0));
    default:
        break;
    }

    /* read and store the current length of message that was processed */
    hashCtx->totalDataSizeProcessed[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_0));
    hashCtx->totalDataSizeProcessed[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_1));

    return;
}

/* initial HASH values */
static const uint32_t HASH_LARVAL_SHA1[] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
static const uint32_t HASH_LARVAL_SHA224[] = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
static const uint32_t HASH_LARVAL_SHA256[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


/**********************************************************************************************/
/************************              Public Functions          ******************************/
/**********************************************************************************************/

drvError_t InitHashDrv(void  *pCtx)
{
    HashContext_t  *hashCtx;

        /* verify user context pointer */
        if ( pCtx == NULL ) {
                return HASH_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }
    hashCtx = (HashContext_t  *)pCtx;

        /* verify hash valid mode */
        switch (hashCtx->mode) {
    case HASH_SHA1:
        CC_PalMemCopy((uint8_t *)&hashCtx->digest, (uint8_t *)&HASH_LARVAL_SHA1, SHA1_DIGEST_SIZE_IN_BYTES );
        break;
    case HASH_SHA224:
        CC_PalMemCopy((uint8_t *)&hashCtx->digest, (uint8_t *)&HASH_LARVAL_SHA224, SHA256_DIGEST_SIZE_IN_BYTES );
        break;
    case HASH_SHA256:
        CC_PalMemCopy((uint8_t *)&hashCtx->digest, (uint8_t *)&HASH_LARVAL_SHA256, SHA256_DIGEST_SIZE_IN_BYTES );
                break;
        default:
                return HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        }

    return HASH_DRV_OK;
}


drvError_t ProcessHashDrv( void         *pCtx,
                        CCBuffInfo_t *pInputBuffInfo,
                        uint32_t    dataInSize)
{
    uint32_t irrVal = 0;
    drvError_t drvRc = HASH_DRV_OK;
    uint32_t hashCtrl = 0;
    HashContext_t  *hashCtx;
    uint32_t regVal = 0;
    uint32_t inputDataAddr;

    /* check input parameters */
    if (pInputBuffInfo == NULL) {
         return HASH_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
    }

    /* verify user context pointer */
    if ( pCtx == NULL ) {
        return HASH_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
    }
    hashCtx = (HashContext_t  *)pCtx;

    /* verify hash valid mode */
    switch(hashCtx->mode) {
    case HASH_SHA1:
        hashCtrl |= HW_HASH_CTL_SHA1_VAL;
        break;
    case HASH_SHA224:
    case HASH_SHA256:
        hashCtrl |= HW_HASH_CTL_SHA256_VAL;
        break;
    default:
        return HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR;
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

    /* enable clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE) ,SET_CLOCK_ENABLE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_ENABLE);

    /* configure CC to hash */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL) ,CONFIG_HASH_MODE_VAL);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_EN) ,1);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), 0);

    /* load the current length of message being processed */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_0), hashCtx->totalDataSizeProcessed[0]);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_1), hashCtx->totalDataSizeProcessed[1]);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CONTROL) ,hashCtrl);

    /* initializing the init HASH values from context */
    switch(hashCtx->mode) {
    case HASH_SHA224:
    case HASH_SHA256:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H7) ,hashCtx->digest[7]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H6) ,hashCtx->digest[6]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H5) ,hashCtx->digest[5]);
        /* Fall-through. */
    case HASH_SHA1:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H4) ,hashCtx->digest[4]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H3) ,hashCtx->digest[3]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H2) ,hashCtx->digest[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H1) ,hashCtx->digest[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H0) ,hashCtx->digest[0]);
    default:
        break;
    }

    if (dataInSize == 0) {
        /* use DO_PAD to complete padding of previous operation */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_CFG) ,4);

    } else {
        /* use HW padding (NA for zero bytes) for last block */
        if (hashCtx->isLastBlockProcessed == 1){
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AUTO_HW_PADDING) ,1);
        }

        inputDataAddr = pInputBuffInfo->dataBuffAddr;

        /* configure the HW with the correct data buffer attributes (secure/non-secure) */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);

        /* configure source address and size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,dataInSize);

        /* set dma completion bit in irr */
        irrVal = 0;
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
        drvRc = CC_HalWaitInterrupt(irrVal);
        /* Note: Hash operation should be completed also in case of error! */
        /* drvRc will be tested later, and hash results will be cleared */
    }

    /* finishing the update operation */
    UpdateHashFinish(hashCtx);

    /* reset to default values in case of operation completion */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_EN) ,1);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AUTO_HW_PADDING) ,0);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_CFG) ,0);

    /* disable the HASH clock in the end of the process */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE) ,SET_CLOCK_DISABLE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_DISABLE);

    if (drvRc != HASH_DRV_OK) {
        /* Clear buffer results in case of error */
        CC_PalMemSetZero((uint8_t*)hashCtx->digest, MAX_DIGEST_SIZE_WORDS);
    }

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


drvError_t FinishHashDrv(void  *pCtx)
{

    HashContext_t  *hashCtx;
    int i;

        /* verify user context pointer */
        if ( pCtx == NULL ) {
                return HASH_DRV_INVALID_USER_CONTEXT_POINTER_ERROR;
        }
    hashCtx = (HashContext_t  *)pCtx;

    /* reverse the bytes */
    for ( i = 0 ; i < MAX_DIGEST_SIZE_WORDS ; i++ )
        hashCtx->digest[i] = CC_COMMON_REVERSE32(hashCtx->digest[i]);

    return HASH_DRV_OK;

}



