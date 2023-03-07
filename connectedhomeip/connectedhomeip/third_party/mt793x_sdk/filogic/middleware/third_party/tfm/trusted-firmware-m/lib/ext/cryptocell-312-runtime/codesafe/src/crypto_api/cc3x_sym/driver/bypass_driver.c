/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_SYM_DRIVER

#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "bypass_driver.h"
#include "driver_defs.h"
#include "cc_sym_error.h"
#include "cc_hal.h"
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"


/******************************************************************************
*               PRIVATE FUNCTIONS
******************************************************************************/

drvError_t ProcessBypass(CCBuffInfo_t *pInputBuffInfo, dataAddrType_t inputDataAddrType,
                         CCBuffInfo_t *pOutputBuffInfo, dataAddrType_t outputDataAddrType,
                         uint32_t blockSize)
{
    drvError_t drvRc = BYPASS_DRV_OK;
    uint32_t irrVal = 0;
    uint32_t regVal = 0;
    uint32_t inputDataAddr, outputDataAddr;

    /* check input parameters */
    if ( (pInputBuffInfo == NULL) || (pOutputBuffInfo == NULL)) {
         return BYPASS_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR;
    }

    /* verify min block size */
    if (blockSize == 0)
        return BYPASS_DRV_ILLEGAL_BLOCK_SIZE_ERROR;

    /* verify aes valid input addr type */
    if( (inputDataAddrType != SRAM_ADDR) &&
        (inputDataAddrType != DLLI_ADDR) ) {
        return BYPASS_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR;
    }

    /* verify aes valid output addr type for ecb, cbc, ctr */
    if( (outputDataAddrType != SRAM_ADDR) &&
        (outputDataAddrType != DLLI_ADDR) ) {
        return BYPASS_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR;
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

    /* configure DIN-BYPASS-DOUT */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, CRYPTO_CTL) ,CONFIG_DIN_BYPASS_DOUT_VAL);

        /* enable clock */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_ENABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_ENABLE);

        inputDataAddr = pInputBuffInfo->dataBuffAddr;
        outputDataAddr = pOutputBuffInfo->dataBuffAddr;

        /* configure the HW with the correct data buffer attributes (secure/non-secure) */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, pInputBuffInfo->dataBuffNs);
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_WRITE_HNONSEC, regVal, pOutputBuffInfo->dataBuffNs);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AHBM_HNONSEC) ,regVal);

    /* configure destination address and size; set dout bit in irr */
    if (outputDataAddrType == DLLI_ADDR){
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD0) ,outputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DST_LLI_WORD1) ,blockSize);
    } else {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_DEST_ADDR) ,outputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DOUT_SRAM_BYTES_LEN) ,blockSize);
    }

    /* configure source address and size */
    if (inputDataAddrType == DLLI_ADDR){
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD0) ,inputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRC_LLI_WORD1) ,blockSize);
    } else {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_SRC_ADDR) ,inputDataAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_SRAM_BYTES_LEN) ,blockSize);
    }

        /* set dma completion bit in irr */
        irrVal = 0;
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
        drvRc = CC_HalWaitInterrupt(irrVal);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_CLK_ENABLE) ,SET_CLOCK_DISABLE);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DMA_CLK_ENABLE) ,SET_CLOCK_DISABLE);

    return drvRc;
}


