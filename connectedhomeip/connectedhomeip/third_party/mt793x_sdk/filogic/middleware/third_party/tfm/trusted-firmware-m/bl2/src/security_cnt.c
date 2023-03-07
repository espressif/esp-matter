/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "../ext/mcuboot/include/security_cnt.h"
#include "../../platform/include/tfm_plat_nv_counters.h"
#include "../../platform/include/tfm_plat_defs.h"
#include <stdint.h>

#define TFM_BOOT_NV_COUNTER_0    PLAT_NV_COUNTER_3   /* NV counter of Image 0 */
#define TFM_BOOT_NV_COUNTER_1    PLAT_NV_COUNTER 4   /* NV counter of Image 1 */
#define TFM_BOOT_NV_COUNTER_MAX  PLAT_NV_COUNTER_MAX

static enum tfm_nv_counter_t get_nv_counter_from_image_id(uint32_t image_id)
{
    uint32_t nv_counter;

    /* Avoid integer overflow */
    if ((UINT32_MAX - TFM_BOOT_NV_COUNTER_0) < image_id) {
        return TFM_BOOT_NV_COUNTER_MAX;
    }

    nv_counter = TFM_BOOT_NV_COUNTER_0 + image_id;

    /* Check the existence of the enumerated counter value */
    if (nv_counter >= TFM_BOOT_NV_COUNTER_MAX) {
        return TFM_BOOT_NV_COUNTER_MAX;
    }

    return (enum tfm_nv_counter_t)nv_counter;
}

int32_t boot_nv_security_counter_init(void)
{
    enum tfm_plat_err_t err;

    err = tfm_plat_init_nv_counter();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }

    return 0;
}

int32_t boot_nv_security_counter_get(uint32_t image_id, uint32_t *security_cnt)
{
    enum tfm_nv_counter_t nv_counter;
    enum tfm_plat_err_t err;

    /* Check if it's a null-pointer. */
    if (!security_cnt) {
        return -1;
    }

    nv_counter = get_nv_counter_from_image_id(image_id);
    if (nv_counter == TFM_BOOT_NV_COUNTER_MAX) {
        return -1;
    }

    err = tfm_plat_read_nv_counter(nv_counter,
                                   sizeof(*security_cnt),
                                   (uint8_t *)security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }

    return 0;
}

int32_t boot_nv_security_counter_update(uint32_t image_id,
                                        uint32_t img_security_cnt)
{
    enum tfm_nv_counter_t nv_counter;
    enum tfm_plat_err_t err;

    nv_counter = get_nv_counter_from_image_id(image_id);
    if (nv_counter == TFM_BOOT_NV_COUNTER_MAX) {
        return -1;
    }

    err = tfm_plat_set_nv_counter(nv_counter, img_security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }

    return 0;
}
