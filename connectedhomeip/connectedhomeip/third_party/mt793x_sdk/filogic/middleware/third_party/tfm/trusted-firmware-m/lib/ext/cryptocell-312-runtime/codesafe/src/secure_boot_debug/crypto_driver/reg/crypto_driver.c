/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/

#include "cc_pal_types.h"
#include "secureboot_basetypes.h"
#include "secureboot_error.h"
#include "secureboot_general_hwdefs.h"
#include "util.h"
#include "cc_hal_sb.h"
#include "crypto_driver_defs.h"
#include "crypto_driver.h"
#include "cc_hal_sb_plat.h"
#include "cc_pal_sb_plat.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Internal data ******************************/
static const uint32_t HASH_LARVAL_SHA256[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

/************************ Typedefs ******************************/

/************************ Global Data ******************************/


/*!
 * This function initializes the AES and HASH HW engines according to required crypto operations.
 * This should be the first function called.
 *
 * @param[in] hwBaseAddress     - cryptocell base address
 * @param[in] aesKeyAddr    - the address of the AES key
 * @param[in] aesIvAddr     - the address of the AES IV
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t
 *
 */
void SBROM_CryptoInitDriver(unsigned long hwBaseAddress, CCDmaAddr_t aesKeyAddr, CCDmaAddr_t aesIvAddr, CryptoDriverMode_t cryptoDriverMode)
{
    uint32_t aesCtrl = 0, address = 0,  irrVal = 0;
    uint32_t *ivPtr = (uint32_t*)aesIvAddr;
    uint32_t *keyPtr = (uint32_t*)aesKeyAddr;
    int32_t i = 0;

    /*poll on DOUT/DIN DMA busy */
    CC_SB_WAIT_ON_DOUT_DMA_BUSY(hwBaseAddress);
    CC_SB_WAIT_ON_DIN_DMA_BUSY(hwBaseAddress);

    /* clear all interrupts before starting the engine */
    SB_HalClearInterruptBit(hwBaseAddress, 0xFFFFFFFFUL);

    /* Initialze AES block */
    /************************/
    if (cryptoDriverMode != CRYPTO_DRIVER_HASH_MODE){

        CC_SB_WAIT_ON_AES_BUSY();

        /* enable aes clock */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,AES_CLK_ENABLE) ,SB_SET_CLK_ENABLE_VAL);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,DMA_CLK_ENABLE) ,SB_SET_CLK_ENABLE_VAL);

        /* Zero AES_REMAINING_BYTES */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_REMAINING_BYTES) ,0);

        /* write the key */
        address = SB_REG_ADDR(hwBaseAddress, AES_KEY_0_0);
        for (i=0; i<4; i++){

            SB_HAL_WRITE_REGISTER(address, keyPtr[i]);
            address = address + 4;
        }

        if (cryptoDriverMode == CRYPTO_DRIVER_AES_CMAC_MODE){

            aesCtrl |= (CC_SB_CMAC_MODE << SB_AES_CTRL_MODE_BIT_SHIFT);

            /* write 0 to initial counter for CMAC */
            address = SB_REG_ADDR(hwBaseAddress, AES_IV_0_0);
            for (i=0; i<4; i++){
                SB_HAL_WRITE_REGISTER(address, 0x0UL);
                address = address + 4;
            }

            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CMAC_INIT) ,0x1);

        } else {

            aesCtrl |= (CC_SB_CTR_MODE << SB_AES_CTRL_MODE_BIT_SHIFT);

            /* write the initial value for CTR */
            address = SB_REG_ADDR(hwBaseAddress, AES_CTR_0_0);
            for (i=0; i<4; i++){
                SB_HAL_WRITE_REGISTER(address, ivPtr[i]);
                address = address + 4;
            }
        }

        /* configure AES mode and direction */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CONTROL) ,aesCtrl);
    }

    /* Initialze Hash block */
    /************************/
    if (cryptoDriverMode == CRYPTO_DRIVER_HASH_MODE || cryptoDriverMode == CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE){

        CC_SB_WAIT_ON_HASH_BUSY();

        /* enable hash clock */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CLK_ENABLE) ,SB_SET_CLK_ENABLE_VAL);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DMA_CLK_ENABLE) ,SB_SET_CLK_ENABLE_VAL);

        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_PAD_EN) ,1);


        /* reset the current length of message being processed */
        address = SB_REG_ADDR(hwBaseAddress, HASH_CUR_LEN_0);
        for (i=0; i<2; i++){
            SB_HAL_WRITE_REGISTER(address, 0);
            address = address + 4;
        }

        /* setting the register HASH control register with the calculated value */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HASH_CONTROL) ,SB_HASH_CTL_SHA256_VAL);

        /* initialize the HASH vectors */
        address = SB_REG_ADDR(hwBaseAddress, HASH_H7);
        for (i=HASH_DIGEST_SIZE_IN_WORDS-1; i>=0; i--){
            SB_HAL_WRITE_REGISTER(address, HASH_LARVAL_SHA256[i]);
            address = address - 4;
        }

        /* wait on crypto busy to assure that Hash initialized values are set! */
        CC_SB_WAIT_ON_CRYPTO_BUSY(hwBaseAddress);
    }

    switch (cryptoDriverMode){
    case CRYPTO_DRIVER_HASH_MODE:
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,SB_CRYPTO_CTL_HASH_MODE);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_MEM_MASK, irrVal, 1);
        break;
    case CRYPTO_DRIVER_AES_CTR_MODE:
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,SB_CRYPTO_CTL_AES_MODE);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, MEM_TO_DIN_MASK, irrVal, 1);
        break;
    case CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE:
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,SB_CRYPTO_CTL_AES_TO_HASH_MODE);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, MEM_TO_DIN_MASK, irrVal, 1);
        break;
    case CRYPTO_DRIVER_AES_CMAC_MODE:
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,SB_CRYPTO_CTL_AES_MODE);
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_MEM_MASK, irrVal, 1);
    default:
        break;
    }

    /* mask unneeded interrupt */
    SB_HalMaskInterrupt(hwBaseAddress, irrVal);

    return;
}


/*!
 * This function is used to do cryptographic operations on a block(s) of data using HASH and/or AES machines.
 *
 * @param[in] hwBaseAddress     - cryptocell base address
 * @param[in] inputDataAddr     - address of the users data input buffer.
 * @param[out] outputDataAddr   - address of the users data output buffer.
 * @param[in] BlockSize      - number of bytes to update.
 *                                if it is not the last block, the size must be a multiple of AES blocks.
 * @param[in] isLastBlock   - if false, just updates the data; otherwise, enable hash padding
 * @param[in] cryptoDriverMode  - can be one of CryptoDriverMode_t
 * @param[in] isWaitForCryptoCompletion -enum for crypto operation completion mode
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from secureboot_error.h
 */
CCError_t SBROM_CryptoUpdateBlockDriver(unsigned long hwBaseAddress, CCDmaAddr_t inputDataAddr, CCDmaAddr_t outputDataAddr, uint32_t BlockSize,
                        uint8_t isLastBlock, CryptoDriverMode_t cryptoDriverMode,
                    CCSbCryptoCompletionMode_t isWaitForCryptoCompletion)
{
    uint32_t irrVal = 0;
    uint32_t *outputPtr = (uint32_t*)outputDataAddr;

    /* for IOT, we MUST handle the crypto operation in synchronic way */
    if (isWaitForCryptoCompletion != CC_SB_CRYPTO_COMPLETION_WAIT_UPON_END)
        return CC_SB_DRV_ILLEGAL_INPUT_ERR;

    if (cryptoDriverMode == CRYPTO_DRIVER_AES_CMAC_MODE){
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_REMAINING_BYTES) ,BlockSize);
    }

    if (cryptoDriverMode == CRYPTO_DRIVER_AES_CTR_MODE || cryptoDriverMode == CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE){

        /* configure destination - address and size */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DST_LLI_WORD0) ,outputDataAddr);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DST_LLI_WORD1) ,BlockSize);

        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, DOUT_TO_MEM_INT, irrVal, 1);
    } else {

        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, MEM_TO_DIN_INT, irrVal, 1);
    }

    /* use HW padding (NA for zero bytes) for last block */
    if (cryptoDriverMode == CRYPTO_DRIVER_HASH_MODE || cryptoDriverMode == CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE){
        if (isLastBlock) {
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AUTO_HW_PADDING) ,1);
        } else {
            /* if not last block, then, check the data in size */
            if ((BlockSize % SB_HASH_BLOCK_SIZE_BYTES) != 0) {
                return CC_SB_DRV_ILLEGAL_SIZE_ERR;
            }

            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AUTO_HW_PADDING) ,0);
        }
    }

    /* configure source - address and size */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD0) ,inputDataAddr);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD1) ,BlockSize);

    SB_HalWaitInterrupt(hwBaseAddress, irrVal);

    CC_SB_WAIT_ON_CRYPTO_BUSY(hwBaseAddress);

    /* read the results */
    if (cryptoDriverMode == CRYPTO_DRIVER_AES_CMAC_MODE){
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_0) ,outputPtr[0]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_1) ,outputPtr[1]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_2) ,outputPtr[2]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_3) ,outputPtr[3]);
    }

    if (isLastBlock) {
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,AES_CLK_ENABLE) ,SB_SET_CLK_DISABLE_VAL);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,DMA_CLK_ENABLE) ,SB_SET_CLK_DISABLE_VAL);
    }

    return CC_OK;
}


/*!
 * This function returns the digest result of crypto hash operation.
 *
 * @param[in] hwBaseAddress     - cryptocell base address
 * @param[out] hashResult   - the HASH result.
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from secureboot_error.h
 */
CCError_t SBROM_CryptoFinishDriver(unsigned long hwBaseAddress, CCDmaAddr_t hashResult)
{

    uint32_t *pHashRes = (uint32_t*)hashResult;
    uint32_t address = 0;
    int32_t i = 0;

    address = SB_REG_ADDR(hwBaseAddress, HASH_H0);
    for (i=0; i<HASH_DIGEST_SIZE_IN_WORDS; i++){
        SB_HAL_READ_REGISTER(address ,pHashRes[i]);
        address = address + 4;
        pHashRes[i] = UTIL_INVERSE_UINT32_BYTES(pHashRes[i]);
    }

    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,HASH_CLK_ENABLE) ,SB_SET_CLK_DISABLE_VAL);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress,DMA_CLK_ENABLE) ,SB_SET_CLK_DISABLE_VAL);

    return CC_OK;
}


