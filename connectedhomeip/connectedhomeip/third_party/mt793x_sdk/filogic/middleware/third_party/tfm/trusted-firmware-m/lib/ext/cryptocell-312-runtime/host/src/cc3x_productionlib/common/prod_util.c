/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "cc_otp_defs.h"
#include "cc_pal_mem.h"
#include "cc_pal_init.h"
#include "cc_pal_mutex_plat.h"
#include "prod_hw_defs.h"
#include "cc_production_asset.h"
#include "cc_pal_log.h"
#include "cc_prod_error.h"
#include "prod_crypto_driver.h"
#include "dx_crys_kernel.h"
#include "dx_id_registers.h"
#include "aes_driver.h"
#include "prod_util.h"


// These are empty mutexs  - no mutexes are needed in production pahse
CC_PalMutex CCSymCryptoMutex;
CC_PalMutex CCSymCryptoMutex;
CC_PalMutex CCAsymCryptoMutex;
CC_PalMutex CCRndCryptoMutex;
CC_PalMutex *pCCRndCryptoMutex;
CC_PalMutex CCApbFilteringRegMutex;

/**
 * @brief This function
 *
 * @param[in]       package buffer(64 bytes),
 *                          asset ID(4 bytes),
 *                          label(7 bytes),
 *                          context (16 bytes)
 *
 * @param[out]     plain asset (16 bytes)
 *
 * @return 0 -success; otherwise failure

 */
uint32_t CC_PROD_PkgVerify(CCProdAssetPkg_t *pPkgAsset,
                            const uint8_t      *pAssetId, uint32_t assetIdSize,
                            const uint8_t     *pLabel, uint32_t labelSize,
                            uint8_t     *pContext, uint32_t contextSize,
                            CCPlainAsset_t plainAsset,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize) {
        uint32_t rc = 0;
        uint8_t     tmpKey[PROD_KEY_TMP_KEY_SIZE] = { 0 };
        uint8_t     provKey[PROD_KEY_TMP_KEY_SIZE] = { 0 };
        uint8_t     provKeyLabel[] = PROD_LABEL;
        CCProdAssetPkg_t localPkgAsset;

        /* Verify Inputs */
        if ((pPkgAsset == NULL) ||
            (pAssetId == NULL) || (assetIdSize == 0) ||
            (pLabel == NULL) || (labelSize == 0) ||
            (pContext == NULL) || (contextSize == 0) ||
            (plainAsset == NULL)) {
                CC_PAL_LOG_ERR("Invalid inputs \n");
                return CC_PROD_INVALID_PARAM_ERR;
        }

        /* Clear outputs */
        CC_PalMemSetZero(plainAsset, sizeof(CCPlainAsset_t));

        /* Verify Package header */
        if ((pPkgAsset->token != PROD_ASSET_PROV_TOKEN) ||
            (pPkgAsset->version != PROD_ASSET_PROV_VERSION) ||
            (pPkgAsset->assetSize != PROD_ASSET_SIZE) ||
            (pPkgAsset->reserved[0] != PROD_ASSET_RESERVED1_VAL) ||
            (pPkgAsset->reserved[1] != PROD_ASSET_RESERVED2_VAL)) {
                CC_PAL_LOG_ERR("Failed to parse Asset package \n");
                return CC_PROD_ASSET_PKG_PARAM_ERR;
        }

        /* Derive temporary key */
        // Calculate Ktmp = cmac(Krtl, 0x01 || ICV/OEM_label  || 0x0 || user context || 0x80)
        rc = CC_PROD_KeyDerivation(RTL_KEY,   NULL,
                                   pLabel, labelSize,
                                   pContext, contextSize,
                                   tmpKey);
        if (rc != 0) {
                CC_PAL_LOG_ERR("failed to derive Ktmp, rc %d\n", rc);
                return CC_PROD_ASSET_PKG_VERIFY_ERR;
        }

        /* Derive provisioning key */
        // Calculate Kprov= cmac(Ktmp, 0x01 || "P"  || 0x0 || asset_id || 0x80)
        rc = CC_PROD_KeyDerivation(USER_KEY, tmpKey,
                                   provKeyLabel, sizeof(provKeyLabel)-1,
                                   pAssetId, assetIdSize,
                                   (uint8_t *)provKey);
        if (rc != 0) {
                CC_PAL_LOG_ERR("failed to derive Kprov, rc %d\n", rc);
                return CC_PROD_ASSET_PKG_VERIFY_ERR;
        }

        /* Decrypt and authenticate the asset */
        // copy the package into local buffer to be physical and contig memory
        CC_PalMemCopy((uint8_t *)&localPkgAsset, (uint8_t *)pPkgAsset, sizeof(CCProdAssetPkg_t));
        rc = CC_PROD_AesCcmDecrypt(
            provKey,         CC_PROD_AES_BLOCK_SIZE_IN_BYTES,
            (uint8_t *)localPkgAsset.nonce, PROD_ASSET_NONCE_SIZE,
            (uint8_t *)&localPkgAsset, PROD_ASSET_ADATA_SIZE,
            (uint8_t *)localPkgAsset.encAsset, localPkgAsset.assetSize,
            (uint8_t *)plainAsset,
            PROD_ASSET_TAG_SIZE,  ((uint8_t *)localPkgAsset.encAsset) + localPkgAsset.assetSize,
            workspaceAddr,
            workspaceSize);
        if (rc != 0) {
                CC_PAL_LOG_ERR("failed to CC_CommonAesCmacEncrypt() for Kprov, rc %d\n", rc);
                return CC_PROD_ASSET_PKG_VERIFY_ERR;
        }

        return CC_OK;
}


uint32_t  CC_PROD_BitListFromNum(uint32_t *pWordBuff,
                                        uint32_t wordBuffSize,
                                        uint32_t numVal)
{
        uint32_t  index = 0;
        if (numVal != 0) {
                for (index = 0; index < wordBuffSize; index++) {
                        pWordBuff[index] = CC_32BIT_MAX_VALUE >> (CC_BITS_IN_32BIT_WORD - PROD_MIN(numVal, CC_BITS_IN_32BIT_WORD));
                        numVal -= PROD_MIN(numVal, CC_BITS_IN_32BIT_WORD);
                }
                if (numVal > 0) {
                        CC_PalMemSetZero(pWordBuff, (wordBuffSize)*CC_32BIT_WORD_SIZE);
                        return CC_PROD_INVALID_PARAM_ERR;
                }
                return CC_OK;
        }
        return CC_OK;
}


/* Count number of zeroes in 32-bit word */
uint32_t  CC_PROD_GetZeroCount(uint32_t *pBuff,
                               uint32_t buffWordSize,
                               uint32_t  *pZeroCount)               {
        uint32_t val;
        uint32_t index = 0;

        *pZeroCount = 0;
        for (index = 0; index < buffWordSize; index++) {
                val = pBuff[index];
                val = val - ((val >> 1) & 0x55555555);
                val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
                val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);
                *pZeroCount += (32 - val);
        }
        /* All 0's and all 1's is forbidden */
        if ((*pZeroCount == 0) || (*pZeroCount == buffWordSize*CC_BITS_IN_32BIT_WORD)) {
                *pZeroCount = 0;
                return CC_PROD_ILLEGAL_ZERO_COUNT_ERR;
        }
        return CC_OK;
}

static uint32_t VerifyPidVal(void)
{
    uint32_t pidReg[CC_BSV_PID_SIZE_WORDS] = {0};
    uint32_t pidVal1[CC_BSV_PID_SIZE_WORDS] = {CC_BSV_PID_0_VAL, CC_BSV_PID_1_VAL, CC_BSV_PID_2_VAL, CC_BSV_PID_3_VAL, CC_BSV_PID_4_VAL};
    uint32_t pidVal2[CC_BSV_PID_SIZE_WORDS] = {CC_BSV_PID_0_VAL, CC_BSV_PID_1_VAL, CC_BSV_PID_2_1_VAL, CC_BSV_PID_3_VAL, CC_BSV_PID_4_VAL};

    pidReg[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_0));
    pidReg[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_1));
    pidReg[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_2));
    pidReg[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_3));
    pidReg[4] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_4));

    if ((CC_PalMemCmp((uint8_t*)pidVal1, (uint8_t*)pidReg, sizeof(pidVal1)) != 0) &&
            (CC_PalMemCmp((uint8_t*)pidVal2, (uint8_t*)pidReg, sizeof(pidVal2)) != 0)) {
        return CC_PROD_INIT_ERR;
    }

    return CC_OK;
}

static uint32_t VerifyCidVal(void)
{
    uint32_t cidReg[CC_BSV_CID_SIZE_WORDS] = {0};
    uint32_t cidVal[CC_BSV_CID_SIZE_WORDS] = {CC_BSV_CID_0_VAL, CC_BSV_CID_1_VAL, CC_BSV_CID_2_VAL, CC_BSV_CID_3_VAL};

    cidReg[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_0));
    cidReg[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_1));
    cidReg[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_2));
    cidReg[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_3));

    if (CC_PalMemCmp((uint8_t*)cidVal, (uint8_t*)cidReg, sizeof(cidVal)) != 0){
        return CC_PROD_INIT_ERR;
    }

    return CC_OK;
}

uint32_t  CCProd_Init(void)
{
    uint32_t rc = CC_OK;
    uint32_t reg = 0, tempVal = 0;

    rc = CC_PalInit();
    if (rc != 0) {
        return CC_PROD_INIT_ERR;
    }

    /* verify peripheral ID (PIDR) */
    rc = VerifyPidVal();
    if (rc != 0) {
        rc = CC_PROD_INIT_ERR;
        goto end_init;
    }

    /* verify component ID (CIDR) */
    rc = VerifyCidVal();
    if (rc != 0) {
        rc = CC_PROD_INIT_ERR;
        goto end_init;
    }

    /* turn off the DFA since Cerberus doen't support it */
    reg = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_FORCE_DFA_ENABLE, reg, 0x0);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,reg );
    tempVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF,HOST_AO_LOCK_BITS));
    if(tempVal != reg) {
        rc = CC_PROD_INIT_ERR;
        goto end_init;
    }

    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_DFA_ENABLE_LOCK, reg, CC_TRUE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,reg );
    tempVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF,HOST_AO_LOCK_BITS));
    if(tempVal != reg) {
        rc = CC_PROD_INIT_ERR;
        goto end_init;
    }

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_DFA_IS_ON), 0x0UL);

#ifdef BIG__ENDIAN
/* Set DMA endianess to big */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN), 0xCCUL);
#else /* LITTLE__ENDIAN */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN), 0x00UL);
#endif

    end_init:
    if (rc != CC_OK) {
        CCPROD_Fini();
    }
    return rc;
}


void  CCPROD_Fini(void) {
        CC_PalTerminate();
}
