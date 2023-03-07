/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "mcuboot_config/mcuboot_config.h"
#include "boot_record.h"
#include "region_defs.h"
#include "tfm_boot_status.h"
#include "target.h"
#include "../ext/mcuboot/bootutil/src/bootutil_priv.h"
#include "bootutil/image.h"
#include "bootutil/sha256.h"
#include "flash_map/flash_map.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define SHA256_HASH_SIZE    (32u)
#if defined(MCUBOOT_SIGN_RSA) && defined(MCUBOOT_HW_KEY)
#   define SIG_BUF_SIZE     (MCUBOOT_SIGN_RSA_LEN / 8)
#endif

/*!
 * \def MAX_BOOT_RECORD_SZ
 *
 * \brief Maximum size of the measured boot record.
 *
 * Its size can be calculated based on the following aspects:
 *   - There are 5 allowed software component claims,
 *   - SHA256 is used as the measurement method for the other claims.
 * Considering these aspects, the only claim which size can vary is the
 * type of the software component. In case of single image boot it is
 * "NSPE_SPE" which results the maximum boot record size of 96.
 */
#define MAX_BOOT_RECORD_SZ  (96u)

/*!
 * \var shared_memory_init_done
 *
 * \brief Indicates whether shared memory area was already initialized.
 *
 */
static uint32_t shared_memory_init_done;

/*!
 * \def SHARED_MEMORY_UNINITIALZED
 *
 * \brief Indicates that shared memory is uninitialized.
 */
#define SHARED_MEMORY_UNINITIALZED (0u)

/*!
 * \def SHARED_MEMORY_INITIALZED
 *
 * \brief Indicates that shared memory was already initialized.
 */
#define SHARED_MEMORY_INITIALZED   (1u)

/* Compile time check to verify that shared data region is not overlapping with
 * non-secure data area.
 */
#if ((BOOT_TFM_SHARED_DATA_BASE  >= NS_DATA_START && \
      BOOT_TFM_SHARED_DATA_BASE  <= NS_DATA_LIMIT) || \
     (BOOT_TFM_SHARED_DATA_LIMIT >= NS_DATA_START && \
      BOOT_TFM_SHARED_DATA_LIMIT <= NS_DATA_LIMIT))
#error "Shared data area and non-secure data area is overlapping"
#endif

/* See in boot_record.h */
enum shared_memory_err_t
boot_add_data_to_shared_area(uint8_t        major_type,
                             uint16_t       minor_type,
                             size_t         size,
                             const uint8_t *data)
{
    struct shared_data_tlv_entry tlv_entry = {0};
    struct tfm_boot_data *boot_data;
    uint8_t *next_tlv;
    uint16_t boot_data_size;
    uintptr_t tlv_end, offset;

    boot_data = (struct tfm_boot_data *)BOOT_TFM_SHARED_DATA_BASE;

    /* Check whether first time to call this function. If does then initialise
     * shared data area.
     */
    if (shared_memory_init_done == SHARED_MEMORY_UNINITIALZED) {
        memset((void *)BOOT_TFM_SHARED_DATA_BASE, 0, BOOT_TFM_SHARED_DATA_SIZE);
        boot_data->header.tlv_magic   = SHARED_DATA_TLV_INFO_MAGIC;
        boot_data->header.tlv_tot_len = SHARED_DATA_HEADER_SIZE;
        shared_memory_init_done = SHARED_MEMORY_INITIALZED;
    }

    /* Check whether TLV entry is already added.
     * Get the boundaries of TLV section
     */
    tlv_end = BOOT_TFM_SHARED_DATA_BASE + boot_data->header.tlv_tot_len;
    offset  = BOOT_TFM_SHARED_DATA_BASE + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section looks for the same entry if found then
     * returns with error: SHARED_MEMORY_OVERWRITE
     */
    for (; offset < tlv_end; offset += tlv_entry.tlv_len) {
        /* Create local copy to avoid unaligned access */
        memcpy(&tlv_entry, (const void *)offset, SHARED_DATA_ENTRY_HEADER_SIZE);
        if (GET_MAJOR(tlv_entry.tlv_type) == major_type &&
            GET_MINOR(tlv_entry.tlv_type) == minor_type) {
            return SHARED_MEMORY_OVERWRITE;
        }
    }

    /* Add TLV entry */
    tlv_entry.tlv_type = SET_TLV_TYPE(major_type, minor_type);
    tlv_entry.tlv_len  = SHARED_DATA_ENTRY_SIZE(size);

    if (!boot_u16_safe_add(&boot_data_size, boot_data->header.tlv_tot_len,
                           tlv_entry.tlv_len)) {
        return SHARED_MEMORY_GEN_ERROR;
    }

    /* Verify overflow of shared area */
    if (boot_data_size > BOOT_TFM_SHARED_DATA_SIZE) {
        return SHARED_MEMORY_OVERFLOW;
    }

    next_tlv = (uint8_t *)boot_data + boot_data->header.tlv_tot_len;
    memcpy(next_tlv, &tlv_entry, SHARED_DATA_ENTRY_HEADER_SIZE);

    next_tlv += SHARED_DATA_ENTRY_HEADER_SIZE;
    memcpy(next_tlv, data, size);

    boot_data->header.tlv_tot_len += tlv_entry.tlv_len;

    return SHARED_MEMORY_OK;
}

/* See in boot_record.h */
enum boot_status_err_t
boot_save_boot_status(uint8_t sw_module,
                      const struct image_header *hdr,
                      const struct flash_area *fap)
{
    struct image_tlv_iter it;
    uint32_t offset;
    uint16_t len;
    uint8_t type;
    size_t record_len = 0;
    uint8_t image_hash[32]; /* SHA256 - 32 Bytes */
    uint8_t buf[MAX_BOOT_RECORD_SZ];
    uint32_t boot_record_found = 0;
    uint32_t hash_found = 0;
    uint16_t ias_minor;
    int32_t res;
    enum shared_memory_err_t res2;

    /* Manifest data is concatenated to the end of the image.
     * It is encoded in TLV format.
     */

    res = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_ANY, false);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    /* Traverse through the TLV area to find the boot record
     * and image hash TLVs.
     */
    while (true) {
        res = bootutil_tlv_iter_next(&it, &offset, &len, &type);
        if (res < 0) {
            return BOOT_STATUS_ERROR;
        } else if (res > 0) {
            break;
        }

        if (type == IMAGE_TLV_BOOT_RECORD) {
            if (len > sizeof(buf)) {
                return BOOT_STATUS_ERROR;
            }
            res = LOAD_IMAGE_DATA(hdr, fap, offset, buf, len);
            if (res) {
                return BOOT_STATUS_ERROR;
            }

            record_len = len;
            boot_record_found = 1;

        } else if (type == IMAGE_TLV_SHA256) {
            /* Get the image's hash value from the manifest section. */
            if (len > sizeof(image_hash)) {
                return BOOT_STATUS_ERROR;
            }
            res = LOAD_IMAGE_DATA(hdr, fap, offset, image_hash, len);
            if (res) {
                return BOOT_STATUS_ERROR;
            }

            hash_found = 1;

            /* The boot record TLV is part of the protected TLV area which is
             * located before the other parts of the TLV area (including the
             * image hash) so at this point it is okay to break the loop
             * as the boot record TLV should have already been found.
             */
            break;
        }
    }


    if (!boot_record_found || !hash_found) {
        return BOOT_STATUS_ERROR;
    }

    /* Update the measurement value (hash of the image) data item in the
     * boot record. It is always the last item in the structure to make
     * it easy to calculate its position.
     * The image hash is computed over the image header, the image itself and
     * the protected TLV area (which should already include the image hash as
     * part of the boot record TLV). For this reason this field has been
     * filled with zeros during the image signing process.
     */
    offset = record_len - sizeof(image_hash);
    /* Avoid buffer overflow. */
    if ((offset + sizeof(image_hash)) > sizeof(buf)) {
        return BOOT_STATUS_ERROR;
    }
    memcpy(buf + offset, image_hash, sizeof(image_hash));

    /* Add the CBOR encoded boot record to the shared data area. */
    ias_minor = SET_IAS_MINOR(sw_module, SW_BOOT_RECORD);
    res2 = boot_add_data_to_shared_area(TLV_MAJOR_IAS,
                                        ias_minor,
                                        record_len,
                                        buf);
    if (res2) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}
