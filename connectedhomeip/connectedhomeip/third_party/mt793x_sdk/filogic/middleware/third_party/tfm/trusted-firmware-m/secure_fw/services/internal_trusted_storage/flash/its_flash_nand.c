/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash_nand.h"
#include "Driver_Flash.h"
#include "tfm_memory_utils.h"

static uint32_t buf_block_id = ITS_BLOCK_INVALID_ID;
/* FIXME: calculation duplicated from flash info */
static uint8_t write_buf[ITS_SECTOR_SIZE * ITS_SECTORS_PER_BLOCK];

/**
 * \brief Gets physical address of the given block ID.
 *
 * \param[in] info      Flash device information
 * \param[in] block_id  Block ID
 * \param[in] offset    Offset position from the init of the block
 *
 * \returns Returns physical address for the given block ID.
 */
static uint32_t get_phys_address(const struct its_flash_info_t *info,
                                 uint32_t block_id, size_t offset)
{
    return info->flash_area_addr + (block_id * info->block_size) + offset;
}

psa_status_t its_flash_nand_init(const struct its_flash_info_t *info)
{
    int32_t err;

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t its_flash_nand_read(const struct its_flash_info_t *info,
                                 uint32_t block_id, uint8_t *buff,
                                 size_t offset, size_t size)
{
    int32_t err;
    uint32_t addr = get_phys_address(info, block_id, offset);

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->ReadData(addr, buff, size);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t its_flash_nand_write(const struct its_flash_info_t *info,
                                  uint32_t block_id, const uint8_t *buff,
                                  size_t offset, size_t size)
{
    (void)info;

    if (buf_block_id == ITS_BLOCK_INVALID_ID) {
        buf_block_id = block_id;
    } else if (buf_block_id != block_id) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Buffer the write data */
    (void)tfm_memcpy(write_buf + offset, buff, size);

    return PSA_SUCCESS;
}

psa_status_t its_flash_nand_flush(const struct its_flash_info_t *info)
{
    int32_t err;
    uint32_t addr = get_phys_address(info, buf_block_id, 0);

    /* Flush the buffered write data to flash*/
    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->ProgramData(addr, write_buf,
                                                             info->block_size);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    /* Clear the write buffer */
    (void)tfm_memset(write_buf, 0, sizeof(write_buf));
    buf_block_id = ITS_BLOCK_INVALID_ID;

    return PSA_SUCCESS;
}

psa_status_t its_flash_nand_erase(const struct its_flash_info_t *info,
                                  uint32_t block_id)
{
    int32_t err;
    uint32_t addr;
    size_t offset;

    for (offset = 0; offset < info->block_size; offset += info->sector_size) {
        addr = get_phys_address(info, block_id, offset);

        err = ((ARM_DRIVER_FLASH *)info->flash_dev)->EraseSector(addr);
        if (err != ARM_DRIVER_OK) {
            return PSA_ERROR_STORAGE_FAILURE;
        }
    }

    return PSA_SUCCESS;
}
