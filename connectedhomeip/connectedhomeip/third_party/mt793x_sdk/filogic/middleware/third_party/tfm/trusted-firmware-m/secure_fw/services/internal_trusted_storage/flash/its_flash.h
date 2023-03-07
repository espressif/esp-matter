/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ITS_FLASH_H__
#define __ITS_FLASH_H__

#include <stddef.h>
#include <stdint.h>

#include "flash_layout.h"
#include "psa/error.h"
#include "secure_fw/services/internal_trusted_storage/its_utils.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Invalid block index */
#define ITS_BLOCK_INVALID_ID 0xFFFFFFFFU

/* FIXME: Duplicated from flash info */
#if (ITS_FLASH_PROGRAM_UNIT <= 16)
#define ITS_FLASH_ALIGNMENT ITS_FLASH_PROGRAM_UNIT
#else
#define ITS_FLASH_ALIGNMENT 1
#endif

#if (SST_FLASH_PROGRAM_UNIT <= 16)
#define SST_FLASH_ALIGNMENT SST_FLASH_PROGRAM_UNIT
#else
#define SST_FLASH_ALIGNMENT 1
#endif

/**
 * \brief Provides a compile-time constant for the maximum program unit required
 *        by any flash device that can be accessed through this interface.
 */
#define ITS_FLASH_MAX_ALIGNMENT ITS_UTILS_MAX(ITS_FLASH_ALIGNMENT, \
                                              SST_FLASH_ALIGNMENT)

/**
 * \brief Enumerates the available flash devices.
 *
 * \note The enumeration constants are used as array indexes. They should be
 *       sequential starting from zero to minimise the array size.
 */
enum its_flash_id_t {
    ITS_FLASH_ID_INTERNAL = 0,
    ITS_FLASH_ID_EXTERNAL,
};

/**
 * \struct its_flash_info_t
 *
 * \brief Structure containing the required information about a flash device to
 *        be used by the ITS Flash FS.
 */
struct its_flash_info_t {
    /**
     * \brief Initialize the Flash Interface.
     *
     * \param[in] info  Flash device information
     *
     * \return Returns PSA_SUCCESS if the function is executed correctly.
     *         Otherwise, it returns PSA_ERROR_STORAGE_FAILURE.
     */
    psa_status_t (*init)(const struct its_flash_info_t *info);

    /**
     * \brief Reads block data from the position specified by block ID and
     *        offset.
     *
     * \param[in]  info      Flash device information
     * \param[in]  block_id  Block ID
     * \param[out] buff      Buffer pointer to store the data read
     * \param[in]  offset    Offset position from the init of the block
     * \param[in]  size      Number of bytes to read
     *
     * \note This function assumes all input values are valid. That is, the
     *       address range, based on block_id, offset and size, is a valid range
     *       in flash.
     *
     * \return Returns PSA_SUCCESS if the function is executed correctly.
     *         Otherwise, it returns PSA_ERROR_STORAGE_FAILURE.
     */
    psa_status_t (*read)(const struct its_flash_info_t *info, uint32_t block_id,
                         uint8_t *buff, size_t offset, size_t size);

    /**
     * \brief Writes block data to the position specified by block ID and
     *        offset.
     *
     * \param[in] info      Flash device information
     * \param[in] block_id  Block ID
     * \param[in] buff      Buffer pointer to the write data
     * \param[in] offset    Offset position from the init of the block
     * \param[in] size      Number of bytes to write
     *
     * \note This function assumes all input values are valid. That is, the
     *       address range, based on block_id, offset and size, is a valid range
     *       in flash.
     *
     * \return Returns PSA_SUCCESS if the function is executed correctly.
     *         Otherwise, it returns PSA_ERROR_STORAGE_FAILURE.
     */
    psa_status_t (*write)(const struct its_flash_info_t *info,
                          uint32_t block_id, const uint8_t *buff, size_t offset,
                          size_t size);

    /**
     * \brief Flushes modifications to a block to flash. Must be called after a
     *        sequence of calls to write() (including via
     *        its_flash_block_to_block_move()) for one block ID, before any call
     *        to the same functions for a different block ID.
     *
     * \param[in] info  Flash device information
     *
     * \note It is permitted for write() to commit block updates immediately, in
     *       which case this function is a no-op.
     *
     * \return Returns PSA_SUCCESS if the function is executed correctly.
     *         Otherwise, it returns PSA_ERROR_STORAGE_FAILURE.
     */
    psa_status_t (*flush)(const struct its_flash_info_t *info);

    /**
     * \brief Erases block ID data.
     *
     * \param[in] info      Flash device information
     * \param[in] block_id  Block ID
     *
     * \note This function assumes the input value is valid.
     *
     * \return Returns PSA_SUCCESS if the function is executed correctly.
     *         Otherwise, it returns PSA_ERROR_STORAGE_FAILURE.
     */
    psa_status_t (*erase)(const struct its_flash_info_t *info,
                          uint32_t block_id);

    void *flash_dev;          /**< Pointer to the flash device */
    uint32_t flash_area_addr; /**< Start address of the flash area */
    uint16_t sector_size;     /**< Size of the flash device's physical erase
                               *   unit
                               */
    uint16_t block_size;      /**< Size of a logical erase unit presented by the
                               *   flash interface, a multiple of sector_size.
                               */
    uint16_t num_blocks;      /**< Number of logical erase blocks */
    uint16_t program_unit;    /**< Minimum size of a program operation */
    uint16_t max_file_size;   /**< Maximum file size */
    uint16_t max_num_files;   /**< Maximum number of files */
    uint8_t erase_val;        /**< Value of a byte after erase (usually 0xFF) */
};

/**
 * \brief Gets the flash info structure for the provided flash device.
 *
 * \param[in] id  Identifier of the flash device.
 *
 * \return Pointer to the flash info struct.
 */
const struct its_flash_info_t *its_flash_get_info(enum its_flash_id_t id);

/**
 * \brief Moves data from source block ID to destination block ID.
 *
 * \param[in] info        Flash device information
 * \param[in] dst_block   Destination block ID
 * \param[in] dst_offset  Destination offset position from the init of the
 *                        destination block
 * \param[in] src_block   Source block ID
 * \param[in] src_offset  Source offset position from the init of the source
 *                        block
 * \param[in] size        Number of bytes to moves
 *
 * \note This function assumes all input values are valid. That is, the address
 *       range, based on blockid, offset and size, is a valid range in flash.
 *       It also assumes that the destination block is already erased and ready
 *       to be written.
 *
 * \return Returns PSA_SUCCESS if the function is executed correctly. Otherwise,
 *         it returns PSA_ERROR_STORAGE_FAILURE.
 */
psa_status_t its_flash_block_to_block_move(const struct its_flash_info_t *info,
                                           uint32_t dst_block,
                                           size_t dst_offset,
                                           uint32_t src_block,
                                           size_t src_offset,
                                           size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_FLASH_H__ */
