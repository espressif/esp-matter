/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_internal_trusted_storage.h"

#include "flash/its_flash.h"
#include "flash_fs/its_flash_fs.h"
#include "psa_manifest/pid.h"
#include "tfm_memory_utils.h"
#include "tfm_its_defs.h"
#include "tfm_its_req_mngr.h"
#include "its_utils.h"

#ifndef ITS_BUF_SIZE
/* By default, set the ITS buffer size to the max asset size so that all
 * requests can be handled in one iteration.
 */
#define ITS_BUF_SIZE ITS_MAX_ASSET_SIZE
#endif

/* Buffer to store asset data from the caller.
 * Note: size must be aligned to the max flash program unit to meet the
 * alignment requirement of the filesystem.
 */
static uint8_t asset_data[ITS_UTILS_ALIGN(ITS_BUF_SIZE,
                                          ITS_FLASH_MAX_ALIGNMENT)];

static uint8_t g_fid[ITS_FILE_ID_SIZE];
static struct its_file_info_t g_file_info;

static its_flash_fs_ctx_t fs_ctx_its;
static its_flash_fs_ctx_t fs_ctx_sst;

static its_flash_fs_ctx_t *get_fs_ctx(int32_t client_id)
{
    return (client_id == TFM_SP_STORAGE) ? &fs_ctx_sst : &fs_ctx_its;
}

/**
 * \brief Maps a pair of client id and uid to a file id.
 *
 * \param[in]  client_id  Identifier of the asset's owner (client)
 * \param[in]  uid        Identifier for the data
 * \param[out] fid        Identifier of the file
 */
static void tfm_its_get_fid(int32_t client_id,
                            psa_storage_uid_t uid,
                            uint8_t *fid)
{
    tfm_memcpy(fid, (const void *)&client_id, sizeof(client_id));
    tfm_memcpy(fid + sizeof(client_id), (const void *)&uid, sizeof(uid));
}

psa_status_t tfm_its_init(void)
{
    psa_status_t status;

    /* Initialise the ITS context */
    status = its_flash_fs_prepare(&fs_ctx_its,
                                  its_flash_get_info(ITS_FLASH_ID_INTERNAL));
#ifdef ITS_CREATE_FLASH_LAYOUT
    /* If ITS_CREATE_FLASH_LAYOUT is set, it indicates that it is required to
     * create a ITS flash layout. ITS service will generate an empty and valid
     * ITS flash layout to store assets. It will erase all data located in the
     * assigned ITS memory area before generating the ITS layout.
     * This flag is required to be set if the ITS memory area is located in
     * non-persistent memory.
     * This flag can be set if the ITS memory area is located in persistent
     * memory without a previous valid ITS flash layout in it. That is the case
     * when it is the first time in the device life that the ITS service is
     * executed.
     */
     if (status != PSA_SUCCESS) {
        /* Remove all data in the ITS memory area and create a valid ITS flash
         * layout in that area.
         */
        status = its_flash_fs_wipe_all(&fs_ctx_its);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Attempt to initialise again */
        status = its_flash_fs_prepare(&fs_ctx_its,
                                     its_flash_get_info(ITS_FLASH_ID_INTERNAL));
    }
#endif /* ITS_CREATE_FLASH_LAYOUT */

    /* Initialise the SST context */
    status = its_flash_fs_prepare(&fs_ctx_sst,
                                  its_flash_get_info(ITS_FLASH_ID_EXTERNAL));
#ifdef SST_CREATE_FLASH_LAYOUT
    /* If SST_CREATE_FLASH_LAYOUT is set, it indicates that it is required to
     * create a SST flash layout. SST service will generate an empty and valid
     * SST flash layout to store assets. It will erase all data located in the
     * assigned SST memory area before generating the SST layout.
     * This flag is required to be set if the SST memory area is located in
     * non-persistent memory.
     * This flag can be set if the SST memory area is located in persistent
     * memory without a previous valid SST flash layout in it. That is the case
     * when it is the first time in the device life that the SST service is
     * executed.
     */
     if (status != PSA_SUCCESS) {
        /* Remove all data in the SST memory area and create a valid SST flash
         * layout in that area.
         */
        status = its_flash_fs_wipe_all(&fs_ctx_sst);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Attempt to initialise again */
        status = its_flash_fs_prepare(&fs_ctx_sst,
                                     its_flash_get_info(ITS_FLASH_ID_EXTERNAL));
    }
#endif /* SST_CREATE_FLASH_LAYOUT */

    return status;
}

psa_status_t tfm_its_set(int32_t client_id,
                         psa_storage_uid_t uid,
                         size_t data_length,
                         psa_storage_create_flags_t create_flags)
{
    psa_status_t status;
    size_t write_size;
    size_t offset;

    /* Check that the UID is valid */
    if (uid == TFM_ITS_INVALID_UID) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that the create_flags does not contain any unsupported flags */
    if (create_flags & ~(PSA_STORAGE_FLAG_WRITE_ONCE |
                         PSA_STORAGE_FLAG_NO_CONFIDENTIALITY |
                         PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Set file id */
    tfm_its_get_fid(client_id, uid, g_fid);

    /* Read file info */
    status = its_flash_fs_file_get_info(get_fs_ctx(client_id), g_fid,
                                        &g_file_info);
    if (status == PSA_SUCCESS) {
        /* If the object exists and has the write once flag set, then it
         * cannot be modified. Otherwise it needs to be removed.
         */
        if (g_file_info.flags & PSA_STORAGE_FLAG_WRITE_ONCE) {
            return PSA_ERROR_NOT_PERMITTED;
        } else {
            status = its_flash_fs_file_delete(get_fs_ctx(client_id), g_fid);
            if (status != PSA_SUCCESS) {
                return status;
            }
        }
    } else if (status != PSA_ERROR_DOES_NOT_EXIST) {
        /* If the file does not exist, then do nothing.
         * If other error occurred, return it
         */
        return status;
    }

    /* Write as much of the data as will fit in the asset_data buffer */
    write_size = ITS_UTILS_MIN(data_length, sizeof(asset_data));

    /* Read asset data from the caller */
    (void)its_req_mngr_read(asset_data, write_size);

    /* Create the file in the file system */
    status = its_flash_fs_file_create(get_fs_ctx(client_id), g_fid, data_length,
                                      write_size, (uint32_t)create_flags,
                                      asset_data);
    if (status != PSA_SUCCESS) {
        return status;
    }

    offset = write_size;
    data_length -= write_size;

    /* Iteratively read data from the caller and write it to the filesystem, in
     * chunks no larger than the size of the asset_data buffer.
     */
    while (data_length > 0) {
        write_size = ITS_UTILS_MIN(data_length, sizeof(asset_data));

        /* Read asset data from the caller */
        (void)its_req_mngr_read(asset_data, write_size);

        /* Write to the file in the file system */
        status = its_flash_fs_file_write(get_fs_ctx(client_id), g_fid,
                                         write_size, offset, asset_data);
        if (status != PSA_SUCCESS) {
            /* Delete the file to avoid leaving partial data */
            (void)its_flash_fs_file_delete(get_fs_ctx(client_id), g_fid);
            return status;
        }

        offset += write_size;
        data_length -= write_size;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_its_get(int32_t client_id,
                         psa_storage_uid_t uid,
                         size_t data_offset,
                         size_t data_size,
                         size_t *p_data_length)
{
    psa_status_t status;
    size_t read_size;

#ifdef TFM_PARTITION_TEST_SST
    /* The SST test partiton can call tfm_its_get() through SST code. Treat it
     * as if it were SST.
     */
    if (client_id == TFM_SP_SST_TEST) {
        client_id = TFM_SP_STORAGE;
    }
#endif

    /* Check that the UID is valid */
    if (uid == TFM_ITS_INVALID_UID) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set file id */
    tfm_its_get_fid(client_id, uid, g_fid);

    /* Read file info */
    status = its_flash_fs_file_get_info(get_fs_ctx(client_id), g_fid,
                                        &g_file_info);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Boundary check the incoming request */
    if (data_offset > g_file_info.size_current) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Copy the object data only from within the file boundary */
    data_size = ITS_UTILS_MIN(data_size,
                              g_file_info.size_current - data_offset);

    /* Update the size of the output data */
    *p_data_length = data_size;

    /* Iteratively read data from the filesystem and write it to the caller, in
     * chunks no larger than the size of the asset_data buffer.
     */
    do {
        /* Read as much of the data as will fit in the asset_data buffer */
        read_size = ITS_UTILS_MIN(data_size, sizeof(asset_data));

        /* Read file data from the filesystem */
        status = its_flash_fs_file_read(get_fs_ctx(client_id), g_fid, read_size,
                                        data_offset, asset_data);
        if (status != PSA_SUCCESS) {
            *p_data_length = 0;
            return status;
        }

        /* Write asset data to the caller */
        its_req_mngr_write(asset_data, read_size);

        data_offset += read_size;
        data_size -= read_size;
    } while (data_size > 0);

    return PSA_SUCCESS;
}

psa_status_t tfm_its_get_info(int32_t client_id, psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    psa_status_t status;

    /* Check that the UID is valid */
    if (uid == TFM_ITS_INVALID_UID) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set file id */
    tfm_its_get_fid(client_id, uid, g_fid);

    /* Read file info */
    status = its_flash_fs_file_get_info(get_fs_ctx(client_id), g_fid,
                                        &g_file_info);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Copy file info to the PSA info struct */
    p_info->capacity = g_file_info.size_current;
    p_info->size = g_file_info.size_current;
    p_info->flags = g_file_info.flags;

    return PSA_SUCCESS;
}

psa_status_t tfm_its_remove(int32_t client_id, psa_storage_uid_t uid)
{
    psa_status_t status;

#ifdef TFM_PARTITION_TEST_SST
    /* The SST test partiton can call tfm_its_remove() through SST code. Treat
     * it as if it were SST.
     */
    if (client_id == TFM_SP_SST_TEST) {
        client_id = TFM_SP_STORAGE;
    }
#endif

    /* Check that the UID is valid */
    if (uid == TFM_ITS_INVALID_UID) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set file id */
    tfm_its_get_fid(client_id, uid, g_fid);

    status = its_flash_fs_file_get_info(get_fs_ctx(client_id), g_fid,
                                        &g_file_info);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* If the object exists and has the write once flag set, then it
     * cannot be deleted.
     */
    if (g_file_info.flags & PSA_STORAGE_FLAG_WRITE_ONCE) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    /* Delete old file from the persistent area */
    return its_flash_fs_file_delete(get_fs_ctx(client_id), g_fid);
}
