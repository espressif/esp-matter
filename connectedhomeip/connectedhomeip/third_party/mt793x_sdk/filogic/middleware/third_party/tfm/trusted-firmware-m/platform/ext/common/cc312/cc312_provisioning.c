/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cc_hal_plat.h"
#include "cc_prod.h"
#include "cc_cmpu.h"
#include "cc_dmpu.h"
#include "cc_pal_types.h"
#include "dx_reg_base_host.h"
#include "mbedtls/ecdsa.h"
#include "cc_rnd_common.h"
#include "cmpu_derivation.h"
#include "cmpu_llf_rnd.h"
#include "mbedtls_cc_mng_int.h"
#include "cc_prod_error.h"
#include "prod_util.h"

extern uint8_t rotpk_hash_0[];
extern uint8_t rotpk_hash_1[];

/* Workspace for provisioning internal use */
#define PROVISIONING_MEM_BUF_LEN  \
        (CMPU_WORKSPACE_MINIMUM_SIZE > DMPU_WORKSPACE_MINIMUM_SIZE) ? \
         CMPU_WORKSPACE_MINIMUM_SIZE : DMPU_WORKSPACE_MINIMUM_SIZE
__attribute__((aligned(CC_32BIT_WORD_SIZE)))
static uint8_t provisioning_mem_buf[PROVISIONING_MEM_BUF_LEN];

/*
 * Extract private key
 */
static int extract_private_key(mbedtls_ecdsa_context *key, uint8_t *buf)
{
    size_t len;

    len = mbedtls_mpi_size(&key->d);
    if (mbedtls_mpi_write_binary(&key->d, buf, len) != 0) {
        return -1;
    }

    return 0;
}

/*
 * Generate ECC P256 keypair
 */
static int cc312_generate_ecc_p256_keypair(void *rng_state, uint8_t *output,
                                           size_t len)
{
    uint32_t error = 0;
    uint32_t *pEntrSrc;
    uint32_t sourceSize;
    uint8_t  pKey[32] = { 0 };
    uint8_t  pIv[16] = { 0 };
    uint32_t *pRndWorkBuff = (uint32_t *)provisioning_mem_buf;

    error = CC_PROD_LLF_RND_GetTrngSource((uint32_t **)&pEntrSrc,
                                          &sourceSize,
                                          pRndWorkBuff);
    if (error != CC_OK) {
        return error;
    }

    error = CC_PROD_Derivation_Instantiate(pEntrSrc,
                                           sourceSize,
                                           pKey,
                                           pIv);
    if (error != CC_OK) {
        return error;
    }

    if (len <= 32) {
        memcpy(output, pKey, len);
    } else if (len > 32 && len <= 48) {
        memcpy(output, pKey, 32);
        memcpy(output + 32, pIv, len - 32);
    } else {
        memcpy(output, pKey, 32);
        memcpy(output + 32, pIv, 16);
        memset(output + 48, 0, len - 48);
    }

    return 0;
}

static int cc312_generate_attestation_key(mbedtls_ecp_group_id curve_type,
                                          uint8_t *private_key)
{
    int rc;
    mbedtls_ecdsa_context ecdsa;

    if (!private_key || curve_type == MBEDTLS_ECP_DP_NONE) {
        return -1;
    }

    mbedtls_ecdsa_init(&ecdsa);

    rc = mbedtls_ecdsa_genkey(&ecdsa, curve_type,
         cc312_generate_ecc_p256_keypair, NULL);
    if (rc) {
        goto exit;
    }

    rc = extract_private_key(&ecdsa, private_key);
    if (rc) {
        goto exit;
    }

    rc = 0;

exit:
    mbedtls_ecdsa_free(&ecdsa);

    return rc;
}

/*
 * All Musca-S1 platform-dependent defines (DX_PLAT_MUSCA_S1) are due to the
 * fact that the S1 board's OTP is just an ordinary register which is volatile.
 * The MRAM is used instead, and this is what the changes reflect.
 */
#ifdef DX_PLAT_MUSCA_S1

#define HAL_WRITE_MRAM_UINT32(val, regOffset)                      \
        (*((volatile uint32_t *)(DX_MRAM_CC + ((regOffset) *       \
        sizeof(uint32_t)))) = (val))

static void cc312_otp_copy_to_mram(const uint32_t offs, size_t size_in_words)
{
    uint32_t i = 0;
    uint32_t temp = 0;

    for(i = 0; i < size_in_words; i++) {
        CC_PROD_OTP_READ(temp, offs + i);
        HAL_WRITE_MRAM_UINT32(temp, offs + i);
    }
}

static void cc312_otp_copy_attestation_to_mram(void)
{
    cc312_otp_copy_to_mram(CC_OTP_ATTESTATION_KEY_OFFSET,
                           CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS);

    cc312_otp_copy_to_mram(CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET, 1);
}

static int32_t cc312_otp_copy_dm_content_to_mram(CCDmpuData_t *pDmpuData)
{
    uint32_t hbkSizeInWords;
    uint32_t hbkOtpWordOffset;
    uint32_t swVerOtpWordOffset;
    uint32_t swVerSizeInWords;
    uint32_t icvWord;

    if (pDmpuData == NULL) {
        return -1;
    }

    CC_PROD_OTP_READ(icvWord, CC_OTP_MANUFACTURE_FLAG_OFFSET);

    switch (pDmpuData->hbkType) {
    case DMPU_HBK_TYPE_HBK1:
            if (!IS_HBK0_USED(icvWord)) {
                return -1;
            }
            hbkSizeInWords = CC_OTP_HBK1_SIZE_IN_WORDS;
            hbkOtpWordOffset = CC_OTP_HBK1_OFFSET;
            swVerOtpWordOffset = CC_OTP_HBK1_MIN_VERSION_OFFSET;
            swVerSizeInWords = CC_OTP_HBK1_MIN_VERSION_SIZE_IN_WORDS;
            break;
    case DMPU_HBK_TYPE_HBK:
            if (IS_HBK0_USED(icvWord)) {
                return -1;
            }
            hbkSizeInWords = CC_OTP_HBK_SIZE_IN_WORDS;
            hbkOtpWordOffset = CC_OTP_HBK_OFFSET;
            swVerOtpWordOffset = CC_OTP_HBK_MIN_VERSION_OFFSET;
            swVerSizeInWords = CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS;
            break;
    default:
            return -1;
    }


    cc312_otp_copy_to_mram(CC_OTP_OEM_FLAG_OFFSET, 1);

    cc312_otp_copy_to_mram(hbkOtpWordOffset, hbkSizeInWords);

    cc312_otp_copy_to_mram(CC_OTP_DCU_OFFSET, CC_OTP_DCU_SIZE_IN_WORDS);

    cc312_otp_copy_to_mram(swVerOtpWordOffset, swVerSizeInWords);

    if (pDmpuData->kcpDataType != ASSET_NO_KEY) {
        cc312_otp_copy_to_mram(CC_OTP_KCP_OFFSET, CC_OTP_KCP_SIZE_IN_WORDS);
    }
    if (pDmpuData->kceDataType != ASSET_NO_KEY) {
        cc312_otp_copy_to_mram(CC_OTP_KCE_OFFSET, CC_OTP_KCE_SIZE_IN_WORDS);
    }

    return 0;
}

static int32_t cc312_otp_copy_cm_content_to_mram(CCCmpuData_t *pCmpuData)
{
    if (pCmpuData == NULL) {
        return -1;
    }

    cc312_otp_copy_to_mram(CC_OTP_MANUFACTURE_FLAG_OFFSET, 1);

    cc312_otp_copy_to_mram(CC_OTP_HUK_OFFSET, CC_OTP_HUK_SIZE_IN_WORDS);

    if (pCmpuData->uniqueDataType == CMPU_UNIQUE_IS_HBK0) {
        cc312_otp_copy_to_mram(CC_OTP_HBK0_OFFSET, CC_OTP_HBK0_SIZE_IN_WORDS);

        cc312_otp_copy_to_mram(CC_OTP_DCU_OFFSET, CC_OTP_DCU_SIZE_IN_WORDS);

        cc312_otp_copy_to_mram(CC_OTP_HBK0_MIN_VERSION_OFFSET,
        CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS);
    }
    if (pCmpuData->kpicvDataType  != ASSET_NO_KEY) {
        cc312_otp_copy_to_mram(CC_OTP_KPICV_OFFSET,
                               CC_OTP_KPICV_SIZE_IN_WORDS);
    }
    if (pCmpuData->kceicvDataType  != ASSET_NO_KEY) {
        cc312_otp_copy_to_mram(CC_OTP_KCEICV_OFFSET,
                               CC_OTP_KCEICV_SIZE_IN_WORDS);
    }
    cc312_otp_copy_to_mram(CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET, 1);

    return 0;
}
#endif /* DX_PLAT_MUSCA_S1 */



static int cc312_program_attestation_private_key(
                                    mbedtls_ecp_group_id curve_type)
{
    uint32_t private_key[8];
    uint32_t error;
    uint32_t zero_count;
    int ret;

    /* Check if the attestation key area has been programmed */
    CC_PROD_OTP_READ(zero_count, CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET);
    if (zero_count) {
        return -1;
    }

    ret = cc312_generate_attestation_key(curve_type,
                                        (uint8_t *)private_key);
    if (ret) {
        return -1;
    }

    /* Program private key to OTP */
    CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_ATTESTATION_KEY_OFFSET,
                                       private_key,
                                       CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS,
                                       error);
    if (error != CC_OK) {
        return -1;
     }

    CC_PROD_GetZeroCount(private_key,
                         CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS,
                         &zero_count);
    /* Program the number of zero bits in the private key. This is used to
     * detect whether the private key is overwritten or tampered.
     */
    CC_PROD_OTP_WRITE_VERIFY_WORD(CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET,
                                  zero_count,
                                  error);

    if (error != CC_OK) {
        return -1;
    }

#ifdef DX_PLAT_MUSCA_S1
    cc312_otp_copy_attestation_to_mram();
#endif

    return 0;
}

static int cc312_cmpu_provision(void)
{
    int rc;
    uint8_t *pWorkspaceBuf = provisioning_mem_buf;
    static const uint32_t DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE];
    static CCCmpuData_t cmpuData;

    /* Populate cmpuData with data */
#if (MCUBOOT_IMAGE_NUMBER == 1)
    /* HBK won't be provisioned */
    cmpuData.uniqueDataType = CMPU_UNIQUE_IS_USER_DATA;
#elif (MCUBOOT_IMAGE_NUMBER == 2)
    cmpuData.uniqueDataType = CMPU_UNIQUE_IS_HBK0;
    memcpy(cmpuData.uniqueBuff.hbk0, rotpk_hash_0, 16);
#else
#error "MCUBOOT_IMAGE_NUMBER is not defined or has invalid value"
#endif

    cmpuData.kpicvDataType = ASSET_NO_KEY;
    cmpuData.kceicvDataType = ASSET_NO_KEY;
    cmpuData.icvMinVersion = 0;
    cmpuData.icvConfigWord = 0;

    memcpy(cmpuData.icvDcuDefaultLock, DCU_DEFAULT_LOCK,
           sizeof(DCU_DEFAULT_LOCK));

    rc = CCProd_Cmpu(DX_BASE_CC, &cmpuData, (unsigned long)pWorkspaceBuf,
                     CMPU_WORKSPACE_MINIMUM_SIZE);

#ifdef DX_PLAT_MUSCA_S1
    if (rc) {
        return rc;
    }

    rc = cc312_otp_copy_cm_content_to_mram(&cmpuData);
#endif

    return rc;
}

static int cc312_dmpu_provision(void)
{
    int rc;
    uint8_t *pWorkspaceBuf = provisioning_mem_buf;
    static const uint32_t DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE];
    static CCDmpuData_t dmpuData;

    /* Populate dmpuData with data */
#if (MCUBOOT_IMAGE_NUMBER == 1)
    dmpuData.hbkType = DMPU_HBK_TYPE_HBK;
    memcpy(dmpuData.hbkBuff.hbk, rotpk_hash_0, 32);
#elif (MCUBOOT_IMAGE_NUMBER == 2)
    dmpuData.hbkType = DMPU_HBK_TYPE_HBK1;
    memcpy(dmpuData.hbkBuff.hbk1, rotpk_hash_1, 16);
#else
#error "MCUBOOT_IMAGE_NUMBER is not defined or has invalid value"
#endif

    dmpuData.kcpDataType = ASSET_NO_KEY;
    dmpuData.kceDataType = ASSET_NO_KEY;
    dmpuData.oemMinVersion = 0;

    memcpy(dmpuData.oemDcuDefaultLock, DCU_DEFAULT_LOCK,
           sizeof(DCU_DEFAULT_LOCK));

    rc = CCProd_Dmpu(DX_BASE_CC, &dmpuData, (unsigned long)pWorkspaceBuf,
                     DMPU_WORKSPACE_MINIMUM_SIZE);

#ifdef DX_PLAT_MUSCA_S1
    if (rc) {
        return rc;
    }

    rc = cc312_otp_copy_dm_content_to_mram(&dmpuData);
#endif

    return rc;
}

int crypto_hw_accelerator_otp_provisioning(void)
{
    int rc;
    uint32_t lcs;

    gCcRegBase = DX_BASE_CC;

    if (CCProd_Init() != CC_OK) {
        return -1;
    }

    rc = mbedtls_mng_lcsGet(&lcs);
    if (rc) {
        return rc;
    }

    /* First cycle - program the attestation key and HUK */
    if (lcs == CC_MNG_LCS_CM) {
        rc = cc312_program_attestation_private_key(MBEDTLS_ECP_DP_SECP256R1);
        if (rc) {
            return rc;
        }
        printf("First cycle: Attestation key is provisioned successfully\r\n");
        rc = cc312_cmpu_provision();
        if (rc) {
            return rc;
        }
        printf("First cycle: HUK is provisioned successfully\r\n");
        printf("Please reset the board to program ROTPK\r\n");
    /* Second cycle - program the ROTPK */
    } else if (lcs == CC_MNG_LCS_DM) {
        rc = cc312_dmpu_provision();
        if (rc) {
            return rc;
        }
        printf("Second cycle: ROTPK is provisioned successfully\r\n");
        printf("Provisioning finished, reset the board to get secure"
               " enabled lifecycle\r\n");
    } else {
        printf("Board is in invalid lifecycle for provisioning: %u\r\n", lcs);
    }

    return 0;
}
