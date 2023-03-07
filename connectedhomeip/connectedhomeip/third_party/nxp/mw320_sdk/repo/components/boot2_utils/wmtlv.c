/** @file wmtlv.c
 *
 *  @brief This file provides  API for Type Length Value
 *
 *  Copyright 2015-2020 NXP
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

#include <string.h>
#include <wmtlv.h>
#include <keystore.h>

int tlv_store_init(uint8_t *tlv_buf, uint32_t max_size, uint8_t store_type)
{
    struct tlv_hdr *thdr = (struct tlv_hdr *)tlv_buf;

    if (store_type == BOOT2_KEYSTORE)
        thdr->magic = SEC_KEYSTORE_MAGIC_SIG;
    else if (IMAGE_SECURE_HEADER)
        thdr->magic = SEC_FW_MAGIC_SIG;
    else
        thdr->magic = SEC_DEFAULT_MAGIC_SIG;

    thdr->len = sizeof(struct tlv_hdr);

    /* At the moment store max size in crc for buffer overflow detection */
    thdr->crc = max_size;
    return TLV_SUCCESS;
}

int tlv_store_add(uint8_t *tlv_buf, uint8_t type, uint16_t len, const uint8_t *value)
{
    struct tlv_hdr *thdr     = (struct tlv_hdr *)tlv_buf;
    struct tlv_entry *tentry = (struct tlv_entry *)(tlv_buf + thdr->len);

    if ((thdr->len + sizeof(tentry->type) + sizeof(tentry->len) + len) > thdr->crc)
        return ERR_TLV_INSUFFICIENT_MEMORY;

    tentry->type = type;
    tentry->len  = len;
    memcpy(tentry->value, value, len);

    thdr->len += sizeof(tentry->type) + sizeof(tentry->len) + len;

    return TLV_SUCCESS;
}

/* Special routine for XIP image where keystore has to be a
 * multiple of 4 bytes */
int tlv_store_xip_padding(uint8_t *tlv_buf, uint8_t *pad_value)
{
    struct tlv_hdr *thdr     = (struct tlv_hdr *)tlv_buf;
    struct tlv_entry *tentry = (struct tlv_entry *)(tlv_buf + thdr->len);
    uint32_t len;

    /* Already aligned so no padding */
    if (!(thdr->len & (XIP_ALIGN_SIZE - 1)))
        return TLV_SUCCESS;

    len = XIP_ALIGN_SIZE -
          ((thdr->len + sizeof(tentry->type) + sizeof(tentry->len) + XIP_ALIGN_SIZE) & (XIP_ALIGN_SIZE - 1));

    if ((thdr->len + len) > thdr->crc)
        return ERR_TLV_INSUFFICIENT_MEMORY;

    tentry->type = KEY_FILLER;
    tentry->len  = len;
    memset(tentry->value, *pad_value, tentry->len);

    thdr->len += sizeof(tentry->type) + sizeof(tentry->len) + tentry->len;

    return TLV_SUCCESS;
}

int tlv_store_get(uint8_t *tlv_buf, uint8_t type, uint16_t len, uint8_t *value)
{
    struct tlv_hdr *thdr     = (struct tlv_hdr *)tlv_buf;
    struct tlv_entry *tentry = (struct tlv_entry *)(tlv_buf + sizeof(struct tlv_hdr));

    while ((uint8_t *)tentry < tlv_buf + thdr->len)
    {
        if (tentry->type == type)
        {
            if (value && tentry->len <= len)
            {
                memcpy(value, tentry->value, tentry->len);
                return tentry->len;
            }
            else if (value == NULL)
                return tentry->len;
            else
                return ERR_TLV_BAD_BUF_LEN;
        }
        tentry = (struct tlv_entry *)((uint8_t *)tentry + sizeof(tentry->type) + sizeof(tentry->len) + tentry->len);
    }

    return ERR_TLV_NOT_FOUND;
}

int tlv_store_iterate(uint8_t *tlv_buf, uint8_t *ptype, uint16_t len, uint8_t *value)
{
    struct tlv_hdr *thdr;
    struct tlv_entry *tentry;

    if (tlv_buf)
    {
        thdr   = (struct tlv_hdr *)tlv_buf;
        tentry = (struct tlv_entry *)(tlv_buf + sizeof(struct tlv_hdr));
    }

    if ((uint8_t *)tentry < (uint8_t *)thdr + thdr->len)
    {
        if (ptype)
            *ptype = tentry->type;
        if (value == NULL || tentry->len > len)
            return ERR_TLV_BAD_BUF_LEN;

        memcpy(value, tentry->value, tentry->len);
        tentry = (struct tlv_entry *)((uint8_t *)tentry + sizeof(tentry->type) + sizeof(tentry->len) + tentry->len);
        return tentry->len;
    }

    return ERR_TLV_NOT_FOUND;
}

int tlv_store_get_ref(uint8_t *tlv_buf, uint8_t type, uint16_t *plen, const uint8_t **pvalue)
{
    struct tlv_hdr *thdr     = (struct tlv_hdr *)tlv_buf;
    struct tlv_entry *tentry = (struct tlv_entry *)(tlv_buf + sizeof(struct tlv_hdr));

    while ((uint8_t *)tentry < tlv_buf + thdr->len)
    {
        if (tentry->type == type)
        {
            if (plen)
                *plen = tentry->len;
            if (pvalue)
                *pvalue = tentry->value;
            return tentry->len;
        }
        tentry = (struct tlv_entry *)((uint8_t *)tentry + sizeof(tentry->type) + sizeof(tentry->len) + tentry->len);
    }

    if (plen)
        *plen = 0;
    if (pvalue)
        *pvalue = NULL;

    return ERR_TLV_NOT_FOUND;
}

int tlv_store_iterate_ref(uint8_t *tlv_buf, uint8_t *ptype, uint16_t *plen, const uint8_t **pvalue)
{
    struct tlv_hdr *thdr;
    struct tlv_entry *tentry;

    if (tlv_buf)
    {
        thdr   = (struct tlv_hdr *)tlv_buf;
        tentry = (struct tlv_entry *)(tlv_buf + sizeof(struct tlv_hdr));
    }

    if ((uint8_t *)tentry < (uint8_t *)thdr + thdr->len)
    {
        uint8_t len = tentry->len;

        if (ptype)
            *ptype = tentry->type;
        if (plen)
            *plen = tentry->len;
        if (pvalue)
            *pvalue = tentry->value;
        tentry = (struct tlv_entry *)((uint8_t *)tentry + sizeof(tentry->type) + sizeof(tentry->len) + tentry->len);
        return len;
    }

    if (plen)
        *plen = 0;
    if (pvalue)
        *pvalue = NULL;

    return ERR_TLV_NOT_FOUND;
}

int tlv_store_close(uint8_t *tlv_buf, crc_func_t crc_func)
{
    struct tlv_hdr *thdr = (struct tlv_hdr *)tlv_buf;

    thdr->crc = crc_func((tlv_buf + sizeof(struct tlv_hdr)), (thdr->len - sizeof(struct tlv_hdr)), 0);
    return TLV_SUCCESS;
}

int tlv_store_validate(uint8_t *tlv_buf, uint8_t store_type, crc_func_t crc_func)
{
    struct tlv_hdr *thdr = (struct tlv_hdr *)tlv_buf;
    uint32_t magic, crc;

    switch (store_type)
    {
        case BOOT2_KEYSTORE:
            magic = SEC_KEYSTORE_MAGIC_SIG;
            break;
        case IMAGE_SECURE_HEADER:
            magic = SEC_FW_MAGIC_SIG;
            break;
        default:
            magic = SEC_DEFAULT_MAGIC_SIG;
            break;
    }

    if (thdr->magic != magic)
        return ERR_TLV_MAGIC_FAILURE;

    crc = crc_func((tlv_buf + sizeof(struct tlv_hdr)), (thdr->len - sizeof(struct tlv_hdr)), 0);

    if (crc != thdr->crc)
        return ERR_TLV_CRC_FAILURE;

    return TLV_SUCCESS;
}

int tlv_store_length(uint8_t *tlv_buf)
{
    struct tlv_hdr *thdr = (struct tlv_hdr *)tlv_buf;

    return thdr->len;
}
