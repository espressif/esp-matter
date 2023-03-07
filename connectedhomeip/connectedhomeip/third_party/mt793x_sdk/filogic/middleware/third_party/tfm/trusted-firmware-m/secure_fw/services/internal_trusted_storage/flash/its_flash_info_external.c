/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash.h"

#include "Driver_Flash.h"
#include "flash_layout.h"
#include "secure_fw/services/internal_trusted_storage/its_utils.h"
#include "secure_fw/services/secure_storage/sst_object_defs.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

#ifndef SST_FLASH_DEV_NAME
#error "SST_FLASH_DEV_NAME must be defined by the target in flash_layout.h"
#endif

#ifndef SST_FLASH_AREA_ADDR
#error "SST_FLASH_AREA_ADDR must be defined by the target in flash_layout.h"
#endif

/* Adjust to a size that will allow all assets to fit */
#ifndef SST_FLASH_AREA_SIZE
#error "SST_FLASH_AREA_SIZE must be defined by the target in flash_layout.h"
#endif

/* Adjust to match the size of the flash device's physical erase unit */
#ifndef SST_SECTOR_SIZE
#error "SST_SECTOR_SIZE must be defined by the target in flash_layout.h"
#endif

/* Adjust so that the maximum required asset size will fit in one block */
#ifndef SST_SECTORS_PER_BLOCK
#error "SST_SECTORS_PER_BLOCK must be defined by the target in flash_layout.h"
#endif

/* Adjust to match the size of the flash device's physical program unit */
#ifndef SST_FLASH_PROGRAM_UNIT
#error "SST_FLASH_PROGRAM_UNIT must be defined by the target in flash_layout.h"
#elif (SST_FLASH_PROGRAM_UNIT < 1 || SST_FLASH_PROGRAM_UNIT > SST_SECTOR_SIZE)
#error "SST_FLASH_PROGRAM_UNIT must be between 1 and SST_SECTOR_SIZE inclusive"
#elif (SST_FLASH_PROGRAM_UNIT & (SST_FLASH_PROGRAM_UNIT - 1) != 0)
#error "SST_FLASH_PROGRAM_UNIT must be a power of two"
#endif

/* Include the correct flash interface implementation */
#ifdef SST_RAM_FS
#include "its_flash_ram.h"
#define FLASH_INFO_INIT its_flash_ram_init
#define FLASH_INFO_READ its_flash_ram_read
#define FLASH_INFO_WRITE its_flash_ram_write
#define FLASH_INFO_FLUSH its_flash_ram_flush
#define FLASH_INFO_ERASE its_flash_ram_erase

#elif (SST_FLASH_PROGRAM_UNIT <= 16)
#include "its_flash_nor.h"
#define FLASH_INFO_INIT its_flash_nor_init
#define FLASH_INFO_READ its_flash_nor_read
#define FLASH_INFO_WRITE its_flash_nor_write
#define FLASH_INFO_FLUSH its_flash_nor_flush
#define FLASH_INFO_ERASE its_flash_nor_erase

/* Require each file in the filesystem to be aligned to the program unit */
#define SST_FLASH_ALIGNMENT SST_FLASH_PROGRAM_UNIT

#else
#include "its_flash_nand.h"
#define FLASH_INFO_INIT its_flash_nand_init
#define FLASH_INFO_READ its_flash_nand_read
#define FLASH_INFO_WRITE its_flash_nand_write
#define FLASH_INFO_FLUSH its_flash_nand_flush
#define FLASH_INFO_ERASE its_flash_nand_erase

/* The flash block is programmed in one shot, so no filesystem alignment is
 * required.
 */
#define SST_FLASH_ALIGNMENT 1
#endif

/* Calculate the block layout */
#define FLASH_INFO_BLOCK_SIZE (SST_SECTOR_SIZE * SST_SECTORS_PER_BLOCK)
#define FLASH_INFO_NUM_BLOCKS (SST_FLASH_AREA_SIZE / FLASH_INFO_BLOCK_SIZE)

/* Maximum file size */
#define FLASH_INFO_MAX_FILE_SIZE ITS_UTILS_ALIGN(SST_MAX_OBJECT_SIZE, \
                                                 SST_FLASH_ALIGNMENT)

/* Maximum number of files */
#define FLASH_INFO_MAX_NUM_FILES SST_MAX_NUM_OBJECTS

/* Default value of each byte in the flash when erased */
#define FLASH_INFO_ERASE_VAL 0xFFU

#ifdef TARGET_PLATFORM_MT793X
#define FLASH_INFO_DEV NULL
#else
#ifdef SST_RAM_FS
/* Allocate a static buffer to emulate storage in RAM */
static uint8_t sst_block_data[FLASH_INFO_BLOCK_SIZE * FLASH_INFO_NUM_BLOCKS];
#define FLASH_INFO_DEV sst_block_data
#else
/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH SST_FLASH_DEV_NAME;
#define FLASH_INFO_DEV &SST_FLASH_DEV_NAME
#endif
#endif /* #ifdef TARGET_PLATFORM_MT793X */

const struct its_flash_info_t its_flash_info_external = {
    .init = FLASH_INFO_INIT,
    .read = FLASH_INFO_READ,
    .write = FLASH_INFO_WRITE,
    .flush = FLASH_INFO_FLUSH,
    .erase = FLASH_INFO_ERASE,
    .flash_dev = (void *)FLASH_INFO_DEV,
    // The external flash area address is followed by the internal flash area space
    .flash_area_addr = SST_FLASH_AREA_ADDR,
    .sector_size = SST_SECTOR_SIZE,
    .block_size = FLASH_INFO_BLOCK_SIZE,
    .num_blocks = FLASH_INFO_NUM_BLOCKS,
    .program_unit = SST_FLASH_ALIGNMENT,
    .max_file_size = FLASH_INFO_MAX_FILE_SIZE,
    .max_num_files = FLASH_INFO_MAX_NUM_FILES,
    .erase_val = FLASH_INFO_ERASE_VAL,
};

/* Checks at compile time that the flash device configuration is valid */
#include \
"secure_fw/services/internal_trusted_storage/flash_fs/its_flash_fs_check_info.h"
