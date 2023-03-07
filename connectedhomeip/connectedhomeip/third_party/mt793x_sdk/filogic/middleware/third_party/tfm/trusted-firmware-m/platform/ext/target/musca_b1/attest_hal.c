/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "platform/include/tfm_attest_hal.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "platform/include/tfm_plat_device_id.h"

#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "mbedtls_cc_mng_int.h"
#endif /* CRYPTO_HW_ACCELERATOR */

/*!
 * \def BOOT_SEED
 *
 * \brief Fixed value for boot seed used for test.
 */
#define BOOT_SEED   0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, \
                    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, \
                    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF

static const uint8_t boot_seed[BOOT_SEED_SIZE] = {BOOT_SEED};

/* Example verification service URL for initial attestation token */
static const char verification_service_url[] = "www.trustedfirmware.org";

/* Example profile definition document for initial attestation token */
static const char attestation_profile_definition[] = "PSA_IOT_PROFILE_1";

static const uint8_t implementation_id[] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
    0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
};

static const uint8_t example_ean_13[] = "060456527282910010";

#ifdef CRYPTO_HW_ACCELERATOR
static enum tfm_security_lifecycle_t
map_cc312_to_tfm_lifecycle(uint32_t cc312_lcs)
{
    enum tfm_security_lifecycle_t tfm_lcs;

    if (cc312_lcs == CC_MNG_LCS_CM) {
        tfm_lcs = TFM_SLC_ASSEMBLY_AND_TEST;
    } else if (cc312_lcs == CC_MNG_LCS_DM) {
        tfm_lcs = TFM_SLC_PSA_ROT_PROVISIONING;
    } else if (cc312_lcs == CC_MNG_LCS_SEC_ENABLED) {
        tfm_lcs = TFM_SLC_SECURED;
    } else if (cc312_lcs == CC_MNG_LCS_RMA) {
        tfm_lcs = TFM_SLC_DECOMMISSIONED;
    } else {
        tfm_lcs = TFM_SLC_UNKNOWN;
    }

    return tfm_lcs;
}
#endif

enum tfm_security_lifecycle_t tfm_attest_hal_get_security_lifecycle(void)
{
#ifdef CRYPTO_HW_ACCELERATOR
    int rc;
    uint32_t cc312_lcs;
    enum tfm_security_lifecycle_t tfm_lcs;

    rc = crypto_hw_accelerator_get_lcs(&cc312_lcs);
    if (rc) {
        return TFM_SLC_UNKNOWN;
    }

    tfm_lcs = map_cc312_to_tfm_lifecycle(cc312_lcs);

    return tfm_lcs;
#else
    return TFM_SLC_SECURED;
#endif
}

const char *
tfm_attest_hal_get_verification_service(uint32_t *size)
{
    *size = sizeof(verification_service_url) - 1;

    return verification_service_url;
}

const char *
tfm_attest_hal_get_profile_definition(uint32_t *size)
{
    *size = sizeof(attestation_profile_definition) - 1;

    return attestation_profile_definition;
}

enum tfm_plat_err_t tfm_plat_get_boot_seed(uint32_t size, uint8_t *buf)
{
    /* FixMe: - This getter function must be ported per target platform.
     *        - Platform service shall provide an API to further interact this
     *          getter function to retrieve the boot seed.
     */

    uint32_t i;
    uint8_t *p_dst = buf;
    const uint8_t *p_src = boot_seed;

    if (size != BOOT_SEED_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

/**
 * \brief Copy the device specific ID to the destination buffer
 *
 * \param[out]  p_dst  Pointer to buffer where to store ID
 * \param[in]   p_src  Pointer to the ID
 * \param[in]   size   Length of the ID
 */
static inline void copy_id(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    uint32_t i;

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }
}

enum tfm_plat_err_t tfm_plat_get_implementation_id(uint32_t *size,
                                                   uint8_t  *buf)
{
    const uint8_t *p_impl_id = implementation_id;
    uint32_t impl_id_size = sizeof(implementation_id);

    if (*size < impl_id_size) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    copy_id(buf, p_impl_id, impl_id_size);
    *size = impl_id_size;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_get_hw_version(uint32_t *size, uint8_t *buf)
{
    const uint8_t *p_hw_version = example_ean_13;
    uint32_t hw_version_size = sizeof(example_ean_13) - 1;

    if (*size < hw_version_size) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    copy_id(buf, p_hw_version, hw_version_size);
    *size = hw_version_size;

    return TFM_PLAT_ERR_SUCCESS;
}
