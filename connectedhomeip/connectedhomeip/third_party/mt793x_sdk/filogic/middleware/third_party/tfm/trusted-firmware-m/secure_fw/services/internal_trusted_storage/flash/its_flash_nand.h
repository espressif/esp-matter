/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file its_flash_nand.h
 *
 * \brief Implementations of the flash interface functions for a NAND flash
 *        device. See its_flash.h for full documentation of functions.
 */

#include "its_flash.h"

/**
 * \brief Initialize the Flash Interface.
 */
psa_status_t its_flash_nand_init(const struct its_flash_info_t *info);

/**
 * \brief Reads block data from the position specified by block ID and offset.
 */
psa_status_t its_flash_nand_read(const struct its_flash_info_t *info,
                                 uint32_t block_id, uint8_t *buff,
                                 size_t offset, size_t size);

/**
 * \brief Writes block data to the position specified by block ID and offset.
 */
psa_status_t its_flash_nand_write(const struct its_flash_info_t *info,
                                  uint32_t block_id, const uint8_t *buff,
                                  size_t offset, size_t size);

/**
 * \brief Flushes modifications to a block to flash.
 */
psa_status_t its_flash_nand_flush(const struct its_flash_info_t *info);

/**
 * \brief Erases block ID data.
 */
psa_status_t its_flash_nand_erase(const struct its_flash_info_t *info,
                                  uint32_t block_id);
