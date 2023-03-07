/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stddef.h>

#include "dx_nvm.h"
#include "secureboot_general_hwdefs.h"
#include "secureboot_stage_defs.h"
#include "bsv_error.h"
#include "bsv_otp_api.h"
#include "cc_otp_defs.h"

#ifdef CC_SB_SUPPORT_SB_RT
#include "driver_defs.h"
#include "cc_pal_buff_attr.h"
#include "cc_error.h"
#endif

/**********************************************************************************************/
/************************ Private declarations ************************************************/
/**********************************************************************************************/

static CCError_t LoadBsvAesKey(unsigned long    hwBaseAddress,
            CCBsvKeyType_t          keyType,
            uint32_t            *pUserKey,
            size_t              userKeySize)
{
    CCError_t error = CC_OK;
    uint32_t isKeyInUse = 0;

        switch (keyType) {

    case CC_BSV_HUK_KEY:
        /* validate HUK key */
        CC_BSV_IS_OTP_HUK_ERROR(hwBaseAddress, error);
        if (error) {
            error = CC_BSV_ILLEGAL_HUK_VALUE_ERR;
        }
                break;

    case CC_BSV_RTL_KEY:
                break;

    case CC_BSV_PROV_KEY:
        /* check if key in use */
        CC_BSV_IS_KCP_IN_USE(hwBaseAddress, isKeyInUse, error);
        if ( (error) || (isKeyInUse != CC_TRUE) ) {
            error = CC_BSV_ILLEGAL_KCP_VALUE_ERR;
            break;
        }

        /* validate provisioning key */
        CC_BSV_IS_OTP_KCP_ERROR(hwBaseAddress, error);
        if (error) {
            error = CC_BSV_ILLEGAL_KCP_VALUE_ERR;
        }
                break;

    case CC_BSV_CE_KEY:
        /* check if key in use */
        CC_BSV_IS_KCE_IN_USE(hwBaseAddress, isKeyInUse, error);
        if ( (error) || (isKeyInUse != CC_TRUE) ) {
            error = CC_BSV_ILLEGAL_KCE_VALUE_ERR;
            break;
        }

        /* validate code encryption key */
        CC_BSV_IS_OTP_KCE_ERROR(hwBaseAddress, error);
        if (error) {
            error = CC_BSV_ILLEGAL_KCE_VALUE_ERR;
        }
                break;

    case CC_BSV_ICV_PROV_KEY:
        /* check if key in use */
        CC_BSV_IS_KPICV_IN_USE(hwBaseAddress, isKeyInUse, error);
        if ( (error) || (isKeyInUse != CC_TRUE) ) {
            error = CC_BSV_ILLEGAL_KPICV_VALUE_ERR;
            break;
        }

        /* validate provisioning key */
        CC_BSV_IS_OTP_KPICV_ERROR(hwBaseAddress, error);
        if (error) {
            error = CC_BSV_ILLEGAL_KPICV_VALUE_ERR;
        }
                break;

    case CC_BSV_ICV_CE_KEY:
        /* check if key in use */
        CC_BSV_IS_KCEICV_IN_USE(hwBaseAddress, isKeyInUse, error);
        if ( (error) || (isKeyInUse != CC_TRUE) ) {
            error = CC_BSV_ILLEGAL_KCEICV_VALUE_ERR;
            break;
        }

        /* validate provisioning key */
        CC_BSV_IS_OTP_KCEICV_ERROR(hwBaseAddress, error);
        if (error) {
            error = CC_BSV_ILLEGAL_KCEICV_VALUE_ERR;
        }
                break;

    case CC_BSV_USER_KEY:
        /* load user key */
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_0) ,pUserKey[0]);
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_1) ,pUserKey[1]);
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_2) ,pUserKey[2]);
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_3) ,pUserKey[3]);

        if(userKeySize == CC_BSV_256BITS_KEY_SIZE_IN_BYTES) {
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_4) ,pUserKey[4]);
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_5) ,pUserKey[5]);
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_6) ,pUserKey[6]);
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_KEY_0_7) ,pUserKey[7]);
        }
                break;

    default:
        /* NOTE: KCE and KCEICV are not supported for CMAC KDF */
        error = CC_BSV_INVALID_KEY_TYPE_ERROR;
                break;
        }

    /* load secret key (HUK / KRTL / KCP / KPICV) */
    if ( (error==CC_OK) && (keyType!=CC_BSV_USER_KEY)) {
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, HOST_CRYPTOKEY_SEL) ,keyType);
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_SK) ,0x1);
    }

        return error;
}


static void LoadBsvAesIVState(unsigned long hwBaseAddress)
{
        /* write the initial counter value according to mode */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_0) ,0);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_1) ,0);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_2) ,0);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_3) ,0);

        return;
}


static void LoadBsvAesCtrState(unsigned long hwBaseAddress, uint32_t *pCtrStateBuf)
{
        /* write the initial counter value according to mode */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CTR_0_0) ,pCtrStateBuf[0]);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CTR_0_1) ,pCtrStateBuf[1]);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CTR_0_2) ,pCtrStateBuf[2]);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CTR_0_3) ,pCtrStateBuf[3]);

        return;
}


/**********************************************************************************************/
/************************              Public Functions          ******************************/
/**********************************************************************************************/

void InitBsvAes(unsigned long hwBaseAddress)
{
        uint32_t regVal = 0;

        /* enable clocks */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CLK_ENABLE) ,CC_BSV_CLOCK_ENABLE);
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

    /* configure CC to AES */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,BSV_CRYPTO_AES);

    /* zero AES_REMAINING_BYTES */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_REMAINING_BYTES) ,0);

    return;
}


void FreeBsvAes(unsigned long hwBaseAddress)
{
    /* disable clocks */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CLK_ENABLE) ,CC_BSV_CLOCK_DISABLE);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DMA_CLK_ENABLE) ,CC_BSV_CLOCK_DISABLE);

    return;
}


CCError_t ProcessBsvAes(unsigned long       hwBaseAddress,
            bsvAesMode_t        mode,
            CCBsvKeyType_t      keyType,
            uint32_t            *pUserKey,
            size_t              userKeySize,
            uint32_t        *pCtrStateBuf,
            uint32_t        inputDataAddr,
            uint32_t        outputDataAddr,
            uint32_t        dataSize,
            uint8_t         isLoadIv)
{
    CCError_t error = CC_OK;
    uint32_t irrVal = 0;
    uint32_t aesCtrl = 0;

#ifdef CC_SB_SUPPORT_SB_RT
    drvError_t drvRet = CC_OK;
    uint8_t  buffNs = 0;
    uint32_t regVal = 0;
#endif

    /* set AES configuration word */
    CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, DEC_KEY0, aesCtrl, BSV_AES_DIRECTION_ENCRYPT);
    CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, MODE_KEY0, aesCtrl, mode);
    if ( (keyType == CC_BSV_HUK_KEY) ||
         ((keyType == CC_BSV_USER_KEY) && (userKeySize == CC_BSV_256BITS_KEY_SIZE_IN_BYTES)) ){
        /* 256b keys: HUK, user key */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, aesCtrl, BSV_AES_KEY_SIZE_256BITS);
    } else {
        /* 128b keys: RTL, KCP, KCE, KPICV, KCEICV, user key */
        CC_REG_FLD_SET(HOST_RGF, AES_CONTROL, NK_KEY0, aesCtrl, BSV_AES_KEY_SIZE_128BITS);
    }
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CONTROL) ,aesCtrl);

    /* load AES key */
    error = LoadBsvAesKey(hwBaseAddress, keyType, pUserKey, userKeySize);
    if(error != CC_OK) {
        FreeBsvAes(hwBaseAddress);
        return error;
    }

    if(mode == BSV_AES_CIPHER_CTR)  {
        if (isLoadIv == CC_TRUE){
            /* load ctr regs */
            LoadBsvAesCtrState(hwBaseAddress, pCtrStateBuf);
        }


#ifdef CC_SB_SUPPORT_SB_RT
        /* Need to verify the outputDataAddr memory type and configure CC's AHBM accordingly */
        drvRet = CC_PalDataBufferAttrGet((uint8_t*)outputDataAddr, dataSize, OUTPUT_DATA_BUFFER, &buffNs);
        if (drvRet != CC_OK){
            FreeBsvAes(hwBaseAddress);
            return CC_FATAL_ERROR;
        }
        /* set it in regVal just if we have an output */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_WRITE_HNONSEC, regVal, buffNs);

#endif
        /* configure destination address in case of ctr */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DST_LLI_WORD0) ,outputDataAddr);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, DST_LLI_WORD1) ,dataSize);

    } else {
        /* case CMAC */
        if (isLoadIv == CC_TRUE){
            /* load iv length */
            LoadBsvAesIVState(hwBaseAddress);
            /* initiate CMAC sub-keys calculation */
            SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CMAC_INIT) ,CC_TRUE);
        }

        /* set the remaining bytes */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_REMAINING_BYTES) ,dataSize);
    }


    /* if data size is 0, only set HW reg to 1 */
    if(dataSize == 0)
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_CMAC_SIZE0_KICK) ,CC_TRUE);
    else {

#ifdef CC_SB_SUPPORT_SB_RT
        /* Need to verify the outputDataAddr memory type and configure CC's AHBM accordingly */
        drvRet = CC_PalDataBufferAttrGet((uint8_t*)inputDataAddr, dataSize, INPUT_DATA_BUFFER, &buffNs);
        if (drvRet != CC_OK){
            FreeBsvAes(hwBaseAddress);
            return CC_FATAL_ERROR;
        }
        /* if we had and output regVal already contains the output attribute */
        CC_REG_FLD_SET(HOST_RGF, AHBM_HNONSEC, AHB_READ_HNONSEC, regVal, buffNs);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, AHBM_HNONSEC) ,regVal);

#endif

        /* configure source address and size */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD0) ,inputDataAddr);
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, SRC_LLI_WORD1) ,dataSize);

        /* set dma completion bit in irr */
        CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, irrVal, 1);
        error = SB_HalWaitInterrupt(hwBaseAddress, irrVal);
        if(error != CC_OK) {
            FreeBsvAes(hwBaseAddress);
        }
    }

    return error;
}


void FinishBsvAes(unsigned long hwBaseAddress,
    bsvAesMode_t        mode,
    CCBsvCmacResult_t   cmacResBuf)
{

    if(mode == BSV_AES_CIPHER_CMAC) {
        /* read and store the hash data registers */
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_0), cmacResBuf[0]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_1), cmacResBuf[1]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_2), cmacResBuf[2]);
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, AES_IV_0_3), cmacResBuf[3]);
    }

    FreeBsvAes(hwBaseAddress);

    return;

}



