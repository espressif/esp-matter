/** @file keystore.c
 *
 *  @brief This file provides  Key Store API
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#include <stddef.h>
#include <secure_boot.h>
#include <crc32.h>

#define KS_PRESENT 1
#define KS_ABSENT  -1
#define KS_UNINIT  0

static int8_t ks_present;
static uint8_t *keystore_addr = (uint8_t *)SB_KEYSTORE_ADDR;

void keystore_validate()
{
    if (ks_present != KS_UNINIT)
        return;

    crc32_init();

    if (tlv_store_validate((uint8_t *)SB_KEYSTORE_ADDR, BOOT2_KEYSTORE, crc32) == TLV_SUCCESS)
        ks_present = KS_PRESENT;
    else
        ks_present = KS_ABSENT;

    return;
}

int keystore_get(uint8_t type, uint16_t len, uint8_t *value)
{
    keystore_validate();
    if (ks_present != KS_PRESENT)
        return ERR_TLV_NOT_FOUND;

    return tlv_store_get((uint8_t *)SB_KEYSTORE_ADDR, type, len, value);
}

int keystore_get_ref(uint8_t type, uint16_t *plen, const uint8_t **pvalue)
{
    keystore_validate();
    if (ks_present != KS_PRESENT)
        return ERR_TLV_NOT_FOUND;

    return tlv_store_get_ref((uint8_t *)SB_KEYSTORE_ADDR, type, plen, pvalue);
}

void keystore_iterate_start()
{
    keystore_addr = (uint8_t *)SB_KEYSTORE_ADDR;
}

int keystore_iterate(uint8_t *ptype, uint16_t len, uint8_t *value)
{
    int ret;

    keystore_validate();
    if (ks_present != KS_PRESENT)
        return ERR_TLV_NOT_FOUND;

    ret = tlv_store_iterate(keystore_addr, ptype, len, value);
    if (ret != ERR_TLV_NOT_FOUND)
        keystore_addr = NULL;

    return ret;
}

int keystore_iterate_ref(uint8_t *ptype, uint16_t *plen, const uint8_t **pvalue)
{
    int ret;

    keystore_validate();
    if (ks_present != KS_PRESENT)
        return ERR_TLV_NOT_FOUND;

    ret = tlv_store_iterate_ref(keystore_addr, ptype, plen, pvalue);
    if (ret != ERR_TLV_NOT_FOUND)
        keystore_addr = NULL;

    return ret;
}
