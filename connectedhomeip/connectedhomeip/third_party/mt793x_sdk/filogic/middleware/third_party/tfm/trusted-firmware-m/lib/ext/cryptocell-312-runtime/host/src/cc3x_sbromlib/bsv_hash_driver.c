/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stddef.h>

#include "secureboot_general_hwdefs.h"
#include "secureboot_stage_defs.h"

#ifdef CC_SB_SUPPORT_SB_RT
#include "driver_defs.h"
#include "cc_pal_buff_attr.h"
#include "cc_error.h"
#endif

/**********************************************************************************************/
/************************ Private declarations ************************************************/
/**********************************************************************************************/

/* initial HASH values */
static const uint32_t BSV_HASH_SHA256[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


/**********************************************************************************************/
/************************              Public Functions          ******************************/
/**********************************************************************************************/

void InitBsvHash(unsigned long hwBaseAddress)
{
    uint32_t regVal = 0;

    /* enable clocks */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CLK_ENABLE) ,CC_BSV_CLOCK_ENABLE);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DMA_CLK_ENABLE) ,CC_BSV_CLOCK_ENABLE);

    /* make sure sym engines are ready to use */
    CC_BSV_WAIT_ON_CRYPTO_BUSY();

    /* clear all interrupts before starting the engine */
    SB_HalClearInterruptBit(hwBaseAddress, 0xFFFFFFFFUL);

    /* mask dma interrupts which are not required */
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SRAM_TO_DIN_MASK, regVal, CC_TRUE);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_SRAM_MASK, regVal, CC_TRUE);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, MEM_TO_DIN_MASK, regVal, CC_TRUE);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_MEM_MASK, regVal, CC_TRUE);
    SB_HalMaskInterrupt(hwBaseAddress, regVal);

    /* configure CC to hash */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,BSV_CRYPTO_HASH);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_SEL_AES_MAC) ,0);

    /* set hash operation mode */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CONTROL) ,BSV_HASH_CTL_SHA256_VAL);

    /* enable HW padding */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_PAD_EN) ,CC_TRUE);

    /* load the current length of message being processed */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CUR_LEN_0), 0);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CUR_LEN_1), 0);

    /* initializing the init HASH values from context */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H7) ,BSV_HASH_SHA256[7]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H6) ,BSV_HASH_SHA256[6]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H5) ,BSV_HASH_SHA256[5]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H4) ,BSV_HASH_SHA256[4]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H3) ,BSV_HASH_SHA256[3]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H2) ,BSV_HASH_SHA256[2]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H1) ,BSV_HASH_SHA256[1]);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H0) ,BSV_HASH_SHA256[0]);

    /* do NOT use HW padding (to enable non-integrated operations) */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AUTO_HW_PADDING) ,CC_FALSE);

    return;
}


void FreeBsvHash(unsigned long hwBaseAddress)
{
    /* reset to default values in case of operation completion */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AUTO_HW_PADDING) ,CC_FALSE);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_PAD_CFG) ,CC_FALSE);

    /* disable clocks */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CLK_ENABLE) ,CC_BSV_CLOCK_DISABLE);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DMA_CLK_ENABLE) ,CC_BSV_CLOCK_DISABLE);

    return;
}


CCError_t ProcessBsvHash( unsigned long hwBaseAddress,
            uint32_t inputDataAddr,
            uint32_t dataInSize)
{
    uint32_t irrVal = 0;
    CCError_t error = CC_OK;
    CCHashResult_t  tempBuff;

#ifdef CC_SB_SUPPORT_SB_RT
    drvError_t drvRet = CC_OK;
    uint8_t  buffNs = 0;
    uint32_t regVal = 0;
#endif

    if (dataInSize != 0) {

#ifdef CC_SB_SUPPORT_SB_RT
        /* Need to verify the DataIn memory type and configure CC's AHBM accordingly */
        drvRet = CC_PalDataBufferAttrGet((uint8_t*)inputDataAddr, dataInSize, INPUT_DATA_BUFFER, &buffNs);
        if (drvRet != CC_OK){
            FreeBsvHash(hwBaseAddress);
            return CC_FATAL_ERROR;
        }

        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, buffNs);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AHBM_HNONSEC) ,regVal);

#endif
        /* configure source address and size */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD0) ,inputDataAddr);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD1) ,dataInSize);

        /* set dma completion bit in irr */
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
        error = SB_HalWaitInterrupt(hwBaseAddress, irrVal);
        if(error != CC_OK) {
            /* Note: Hash operation should be completed also in case of error! */

            /* Use DO_PAD to complete padding of full operation */
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_PAD_CFG) ,BSV_HASH_PAD_CFG_VAL);

            /* Empty Hash HW registers */
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H7), tempBuff[7]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H6), tempBuff[6]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H5), tempBuff[5]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H4), tempBuff[4]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H3), tempBuff[3]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H2), tempBuff[2]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H1), tempBuff[1]);
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H0), tempBuff[0]);

            /* Reset Hash temporary results */
            UTIL_MemSet((uint8_t*)tempBuff, 0, sizeof(CCHashResult_t));

            FreeBsvHash(hwBaseAddress);
        }

    }

    return error;
}


void FinishBsvHash(unsigned long hwBaseAddress, CCHashResult_t  hashBuff)
{
    int i;

    /* use DO_PAD to complete padding of full operation */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_PAD_CFG) ,BSV_HASH_PAD_CFG_VAL);

    /* read and store the hash data registers */
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H7), hashBuff[7]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H6), hashBuff[6]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H5), hashBuff[5]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H4), hashBuff[4]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H3), hashBuff[3]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H2), hashBuff[2]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H1), hashBuff[1]);
    SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_H0), hashBuff[0]);

    /* reverse the bytes */
    for ( i = 0 ; i < CC_BSV_SHA256_DIGEST_SIZE_IN_WORDS ; i++ ){
        hashBuff[i] = UTIL_INVERSE_UINT32_BYTES(hashBuff[i]);
    }

    FreeBsvHash(hwBaseAddress);

    return;

}



