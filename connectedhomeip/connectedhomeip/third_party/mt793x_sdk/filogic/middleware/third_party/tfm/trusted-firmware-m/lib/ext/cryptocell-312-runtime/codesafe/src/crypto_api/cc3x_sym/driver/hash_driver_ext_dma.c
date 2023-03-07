/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "hash_driver.h"
#include "driver_defs.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "hash_driver_ext_dma.h"
#include "cc_util_pm.h"

extern CC_PalMutex CCSymCryptoMutex;


/* initial HASH values */
static const uint32_t HASH_LARVAL_SHA1[] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
static const uint32_t HASH_LARVAL_SHA224[] = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
static const uint32_t HASH_LARVAL_SHA256[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


/**********************************************************************************************/
/************************ Private Functions ***************************************************/
/**********************************************************************************************/

static void UpdateHashFinishExtDma(hashMode_t mode, uint32_t *digest)
{
    /* read and store the hash data registers */
    switch(mode) {
    case HASH_SHA224:
    case HASH_SHA256:
        digest[7] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H7));
        digest[6] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H6));
        digest[5] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H5));
        /* fall-thru */
    case HASH_SHA1:
        digest[4] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H4));
        digest[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H3));
        digest[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H2));
        digest[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H1));
        digest[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H0));
    default:
        break;
    }
}

/**********************************************************************************************/
/************************              Public Functions          ******************************/
/**********************************************************************************************/

drvError_t InitHashExtDma(hashMode_t mode, uint32_t dataSize)
{
    uint32_t hashCtrl = 0;
    uint32_t digest[MAX_DIGEST_SIZE_WORDS] = {0};
    drvError_t drvRc = HASH_DRV_OK;
    int rc;
    uint32_t irrVal = 0;

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
    /* verify hash valid mode */
    switch(mode) {
    case HASH_SHA1:
        hashCtrl |= HW_HASH_CTL_SHA1_VAL;
        CC_PalMemCopy((uint8_t *)digest, (uint8_t *)&HASH_LARVAL_SHA1, SHA1_DIGEST_SIZE_IN_BYTES );
        break;
    case HASH_SHA224:
        hashCtrl |= HW_HASH_CTL_SHA256_VAL;
        CC_PalMemCopy((uint8_t *)digest, (uint8_t *)&HASH_LARVAL_SHA224, SHA256_DIGEST_SIZE_IN_BYTES );
        break;
    case HASH_SHA256:
        hashCtrl |= HW_HASH_CTL_SHA256_VAL;
        CC_PalMemCopy((uint8_t *)digest, (uint8_t *)&HASH_LARVAL_SHA256, SHA256_DIGEST_SIZE_IN_BYTES );
        break;
    default:
        drvRc = HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }


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

    /* enable clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE) ,SET_CLOCK_ENABLE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_ENABLE);

    /* configure CC to hash */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL) ,CONFIG_HASH_MODE_VAL);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_EN) ,1);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_SEL_AES_MAC), 0);

    /* load the current length of message being processed */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_0), 0);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CUR_LEN_1), 0);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CONTROL) ,hashCtrl);

    /* initializing the init HASH values from context */
    switch(mode) {
    case HASH_SHA224:
    case HASH_SHA256:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H7) ,digest[7]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H6) ,digest[6]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H5) ,digest[5]);
        /* fall-thru */
    case HASH_SHA1:
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H4) ,digest[4]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H3) ,digest[3]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H2) ,digest[2]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H1) ,digest[1]);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_H0) ,digest[0]);
        break;
    default:
        drvRc = HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR;
        goto end;
    }
    if (dataSize == 0) {
        /* use DO_PAD to complete padding of previous operation */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_CFG) ,4);

    } else {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AUTO_HW_PADDING) ,1);
    }

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_CPU_DATA_SIZE) , dataSize);


    return HASH_DRV_OK;
end:
    rc = terminateHashExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateHashExtDma \n");
    }

    return drvRc;
}


drvError_t FinishHashExtDma(hashMode_t mode, uint32_t * digest)
{

    int i;
    drvError_t rc = AES_DRV_OK;

    UpdateHashFinishExtDma(mode, digest);
    /* reverse the bytes */
    for ( i = 0 ; i < MAX_DIGEST_SIZE_WORDS ; i++ ) {
        digest[i] = CC_COMMON_REVERSE32(digest[i]);
    }

    /* reset to default values */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_EN) ,1);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AUTO_HW_PADDING) ,0);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_PAD_CFG) ,0);


    rc = terminateHashExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateHashExtDma \n");
    }

    return HASH_DRV_OK;
}

drvError_t terminateHashExtDma(void)
{
    drvError_t rc = AES_DRV_OK;

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HASH_CLK_ENABLE) ,SET_CLOCK_DISABLE);
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
