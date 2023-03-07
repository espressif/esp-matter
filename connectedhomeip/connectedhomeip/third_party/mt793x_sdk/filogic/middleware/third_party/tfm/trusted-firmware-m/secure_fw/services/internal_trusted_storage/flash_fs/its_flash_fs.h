/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file  its_flash_fs.h
 *
 * \brief Internal Trusted Storage service filesystem abstraction APIs.
 *        The purpose of this abstraction is to have the ability to plug-in
 *        other filesystems or filesystem proxies (supplicant).
 */

#ifndef __ITS_FLASH_FS_H__
#define __ITS_FLASH_FS_H__

#include <stddef.h>
#include <stdint.h>

#include "its_flash_fs_mblock.h"
#include "psa/error.h"
#include "secure_fw/services/internal_trusted_storage/flash/its_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ITS flash filesytem context type, used to maintain state across FS
 *        operations.
 *
 * \details The user should allocate a variable of this type, initialised to
 *          zero, before calling its_flash_fs_prepare, and then pass it to each
 *          subsequent FS operation. The contents are internal to the filesytem.
 */
typedef struct its_flash_fs_ctx_t its_flash_fs_ctx_t;

/*!
 * \struct its_file_info_t
 *
 * \brief Structure to store the file information.
 */
struct its_file_info_t {
    size_t size_current; /*!< The current size of the flash file data */
    size_t size_max;     /*!< The maximum size of the flash file data in
                          * bytes.
                          */
    uint32_t flags;      /*!< Flags set when the file was created */
};

/**
 * \brief Prepares the filesystem to accept operations on the files.
 *
 * \param[in,out] fs_ctx      Filesystem context to prepare. Must have been
 *                            initialised by the caller.
 * \param[in]     flash_info  Struct containing information about the flash
 *                            device to associate with the context.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_prepare(its_flash_fs_ctx_t *fs_ctx,
                                  const struct its_flash_info_t *flash_info);

/**
 * \brief Wipes all files from the filesystem.
 *
 * \param[in,out] fs_ctx  Filesystem context to wipe. Must be prepared again
 *                        before further use.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_wipe_all(its_flash_fs_ctx_t *fs_ctx);

/**
 * \brief Checks if a file exists in the filesystem.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 *
 * \return Returns PSA_SUCCESS if the file exists. If file does not exist, it
 *         returns PSA_ERROR_DOES_NOT_EXIST. Otherwise, it returns error code as
 *         specified in \ref psa_status_t.
 */
psa_status_t its_flash_fs_file_exist(its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid);

/**
 * \brief Creates a file in the filesystem.
 *
 * \param[in,out] fs_ctx     Filesystem context
 * \param[in]     fid        File ID
 * \param[in]     max_size   Size of the file to be created
 * \param[in]     data_size  Size of the incoming buffer. This parameter is set
 *                           to 0 when the file is empty after the creation.
 * \param[in]     flags      Flags of the file
 * \param[in]     data       Pointer to buffer containing the initial data.
 *                           This parameter is set to NULL when the file is
 *                           empty after the creation.
 *
 * \return Returns PSA_SUCCESS if the file has been created correctly. If the
 *         fid is in use, it returns PSA_ERROR_INVALID_ARGUMENT. Otherwise, it
 *         returns error code as specified in \ref psa_status_t.
 */
psa_status_t its_flash_fs_file_create(its_flash_fs_ctx_t *fs_ctx,
                                      const uint8_t *fid,
                                      size_t max_size,
                                      size_t data_size,
                                      uint32_t flags,
                                      const uint8_t *data);

/**
 * \brief Gets the file information referenced by the file ID.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 * \param[out]    info    Pointer to the information structure to store the
 *                        file information values \ref its_file_info_t
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_get_info(its_flash_fs_ctx_t *fs_ctx,
                                        const uint8_t *fid,
                                        struct its_file_info_t *info);

/**
 * \brief Writes data to an existing file.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 * \param[in]     size    Size of the incoming buffer
 * \param[in]     offset  Offset in the file
 * \param[in]     data    Pointer to buffer containing data to be written
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_write(its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid,
                                     size_t size,
                                     size_t offset,
                                     const uint8_t *data);

/**
 * \brief Reads data from an existing file.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 * \param[in]     size    Size to be read
 * \param[in]     offset  Offset in the file
 * \param[out]    data    Pointer to buffer to store the data
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_read(its_flash_fs_ctx_t *fs_ctx,
                                    const uint8_t *fid,
                                    size_t size,
                                    size_t offset,
                                    uint8_t *data);

/**
 * \brief Deletes file referenced by the file ID.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_delete(its_flash_fs_ctx_t *fs_ctx,
                                      const uint8_t *fid);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_FLASH_FS_H__ */
