/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa/update.h"
#include "tfm_ns_interface.h"

#include <stdio.h>

// RTOS-specific initialization that is not declared in any header file
uint32_t tfm_ns_interface_init(void);

// PSA Firmware Update example - query the current firmware versions
void psa_thread(void *argument)
{
    printf("In psa_thread()\r\n");

    uint32_t ret = tfm_ns_interface_init();
    if (ret != 0) {
        printf("tfm_ns_interface_init() failed: %u\r\n", ret);
        return;
    }

    psa_status_t status;
    const psa_image_id_t id_s = FWU_CALCULATE_IMAGE_ID(FWU_IMAGE_ID_SLOT_ACTIVE, FWU_IMAGE_TYPE_SECURE, 0);
    psa_image_info_t info_s;
    status = psa_fwu_query(id_s, &info_s);
    if (status != PSA_SUCCESS) {
        printf("Failed to query secure firmware information. Error %u\r\n", status);
        return;
    }

    printf("Secure firmware version: %u.%u.%u-%u\r\n",
           info_s.version.iv_major,
           info_s.version.iv_minor,
           info_s.version.iv_revision,
           info_s.version.iv_build_num);

    const psa_image_id_t id_ns = FWU_CALCULATE_IMAGE_ID(FWU_IMAGE_ID_SLOT_ACTIVE, FWU_IMAGE_TYPE_NONSECURE, 0);
    psa_image_info_t info_ns;
    status = psa_fwu_query(id_ns, &info_ns);
    if (status != PSA_SUCCESS) {
        printf("Failed to query non-secure firmware information. Error %u\r\n", status);
        return;
    }

    printf("Non-secure firmware version: %u.%u.%u-%u\r\n",
           info_ns.version.iv_major,
           info_ns.version.iv_minor,
           info_ns.version.iv_revision,
           info_ns.version.iv_build_num);
}
