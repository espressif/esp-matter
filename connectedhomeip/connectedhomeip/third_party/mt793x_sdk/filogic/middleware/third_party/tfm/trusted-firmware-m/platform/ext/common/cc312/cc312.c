/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"

#include "cc_lib.h"
#include "cc_pal_buff_attr.h"
#include "cc_rnd_common.h"
#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls_cc_mng_int.h"
#include "arm_cmse.h"
#include "mbedtls_cc_util_key_derivation.h"
#include "tfm_attest_hal.h"
#include "prod_hw_defs.h"
#include "cc_otp_defs.h"

#define CC312_NULL_CONTEXT "NO SALT!"

CCRndContext_t*           CC312_pRndCtx         = NULL;
CCRndWorkBuff_t*          CC312_pRndWorkBuff    = NULL;
mbedtls_ctr_drbg_context* CC312_pRndState       = NULL;
mbedtls_entropy_context*  CC312_pMbedtlsEntropy = NULL;

CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer,
                                  size_t buffSize, uint8_t buffType,
                                  uint8_t *pBuffNs)
{
    CC_UNUSED_PARAM(buffType);

    *pBuffNs = DATA_BUFFER_IS_SECURE;
    if (cmse_check_address_range((void*)pDataBuffer, buffSize, CMSE_NONSECURE)) {
        *pBuffNs = DATA_BUFFER_IS_NONSECURE;
    }

    return CC_OK;
}

/*
 * \brief Initialize the CC312 crypto accelerator
 */

int crypto_hw_accelerator_init(void)
{
    int ret = 0;

    /* Allocate memory on heap */
    CC312_pRndCtx         = mbedtls_calloc(1, sizeof(CCRndContext_t));
    CC312_pRndWorkBuff    = mbedtls_calloc(1, sizeof(CCRndWorkBuff_t));
    CC312_pRndState       = mbedtls_calloc(1, sizeof(mbedtls_ctr_drbg_context));
    CC312_pMbedtlsEntropy = mbedtls_calloc(1, sizeof(mbedtls_entropy_context));

    /* Check if memory allocation was successful */
    if ( !CC312_pRndCtx || !CC312_pRndWorkBuff
      || !CC312_pRndState || !CC312_pMbedtlsEntropy) {
        mbedtls_free(CC312_pRndCtx);
        mbedtls_free(CC312_pRndWorkBuff);
        mbedtls_free(CC312_pRndState);
        mbedtls_free(CC312_pMbedtlsEntropy);

        return -1;
    }

    /* Init Rnd context's inner members */
    CC312_pRndCtx->rndState   = CC312_pRndState;
    CC312_pRndCtx->entropyCtx = CC312_pMbedtlsEntropy;

    /* Initialise CryptoCell library */
    ret = CC_LibInit(CC312_pRndCtx, CC312_pRndWorkBuff);
    if (ret != CC_LIB_RET_OK) {
        mbedtls_free(CC312_pRndCtx);
        mbedtls_free(CC312_pRndWorkBuff);
        mbedtls_free(CC312_pRndState);
        mbedtls_free(CC312_pMbedtlsEntropy);

        return ret;
    }

    return 0;
}

/*
 * \brief Deallocate the CC312 crypto accelerator
 */
int crypto_hw_accelerator_finish(void)
{
    int ret = 0;

    ret = CC_LibFini(CC312_pRndCtx);
    if(ret != CC_LIB_RET_OK) {
        return ret;
    }

    mbedtls_free(CC312_pRndCtx);
    mbedtls_free(CC312_pRndWorkBuff);
    mbedtls_free(CC312_pRndState);
    mbedtls_free(CC312_pMbedtlsEntropy);

    return 0;
}

int crypto_hw_accelerator_get_lcs(uint32_t *lcs)
{
    return mbedtls_mng_lcsGet(lcs);
}

int crypto_hw_accelerator_huk_derive_key(const uint8_t *label,
                                         size_t label_size,
                                         const uint8_t *context,
                                         size_t context_size,
                                         uint8_t *key,
                                         size_t key_size)
{

    if (context == NULL || context_size == 0) {
        /* The CC312 requires the context to not be null, so a default
         * is given.
         */
        context = (const uint8_t *)CC312_NULL_CONTEXT;
        context_size = sizeof(CC312_NULL_CONTEXT);
    }

    return mbedtls_util_key_derivation_cmac(CC_UTIL_ROOT_KEY, NULL,
                                            label, label_size,
                                            context, context_size,
                                            key, key_size);
}

/*
 * Count number of zero bits in 32-bit word.
 * Copied from:
 * lib/ext/cryptocell-312-runtime/host/src/ \
 * cc3x_productionlib/common/prod_util.c: CC_PROD_GetZeroCount(..)
 */
static int get_zero_bits_count(uint32_t *buf,
                               uint32_t  buf_word_size,
                               uint32_t *zero_count)
{
    uint32_t val;
    uint32_t index = 0;

    *zero_count = 0;
    for (index = 0; index < buf_word_size; index++) {
        val = buf[index];
        val = val - ((val >> 1) & 0x55555555);
        val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
        val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);
        *zero_count += (32 - val);
    }
    /* All 0's and all 1's is forbidden */
    if ((*zero_count == 0)
        || (*zero_count == buf_word_size*CC_BITS_IN_32BIT_WORD)) {
        *zero_count = 0;
        return -1;
    }

    return 0;
}

/*
 * Get attestation private key from CC312 OTP
 */
int crypto_hw_accelerator_get_attestation_private_key(uint8_t  *buf,
                                                      uint32_t *size)
{
    uint32_t *key = (uint32_t *)buf;
    uint32_t otp_val;
    uint32_t otp_zero_count;
    uint32_t zero_count;
    int i;
    int rc;

    if (key == NULL ||
        *size < CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS * sizeof(uint32_t)) {
        return -1;
    }
    *size = CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS * sizeof(uint32_t);

    /* Get provisioned key from OTP, 8 words */
    for (i = 0; i < CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS; i++) {
        CC_PROD_OTP_READ(otp_val, CC_OTP_ATTESTATION_KEY_OFFSET + i);
        *key = otp_val;
        key++;
    }

    /* Verify the zero number of private key */
    rc = get_zero_bits_count((uint32_t *)buf,
                             CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS,
                             &zero_count);
    if (rc) {
        return -1;
    }

    CC_PROD_OTP_READ(otp_zero_count, CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET);
    if (otp_zero_count != zero_count) {
        return -1;
    }

    return 0;
}

int crypto_hw_accelerator_get_rotpk_hash(uint8_t image_id,
                                         uint8_t *rotpk_hash,
                                         uint32_t *rotpk_hash_size)
{
    int32_t ret;
    mbedtls_mng_pubKeyType_t key_index;
    uint32_t rotpk_hash_size_in_words;

    if (image_id == 0) {
#if (MCUBOOT_IMAGE_NUMBER == 1)
        key_index = CC_MNG_HASH_BOOT_KEY_256B;
        rotpk_hash_size_in_words = 8;
#elif (MCUBOOT_IMAGE_NUMBER == 2)
        key_index = CC_MNG_HASH_BOOT_KEY_0_128B;
        rotpk_hash_size_in_words = 4;
    } else if (image_id == 1) {
        key_index = CC_MNG_HASH_BOOT_KEY_1_128B;
        rotpk_hash_size_in_words = 4;
#endif /* MCUBOOT_IMAGE_NUMBER == 1 */
    } else {
        return -1;
    }

    if (*rotpk_hash_size < rotpk_hash_size_in_words * sizeof(uint32_t)) {
        return -1;
    }
    *rotpk_hash_size = rotpk_hash_size_in_words * sizeof(uint32_t);

    ret = mbedtls_mng_pubKeyHashGet(key_index, (uint32_t *)rotpk_hash,
                                    rotpk_hash_size_in_words);
    if (ret) {
        return ret;
    }

    return 0;
}
