/*
 *  Copyright (C) 2017, Linaro Ltd
 *  Copyright (c) 2018-2020, Arm Limited.
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 */

#ifndef H_TARGETS_TARGET_
#define H_TARGETS_TARGET_

/* Target specific defines: flash partitions; flash driver name, etc.
 * Comes from: platform/ext/target/<BOARD>/<SUBSYSTEM>/partition
 */
#include "flash_layout.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

#ifndef FLASH_BASE_ADDRESS
#error "FLASH_BASE_ADDRESS must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SECTOR_SIZE
#error "FLASH_AREA_IMAGE_SECTOR_SIZE must be defined by the target"
#endif

#ifdef MCUBOOT_RAM_LOADING
#ifndef IMAGE_EXECUTABLE_RAM_START
#error "If MCUBOOT_RAM_LOADING is set then IMAGE_EXECUTABLE_RAM_START must be \
defined by the target"
#endif

#ifndef IMAGE_EXECUTABLE_RAM_SIZE
#error "If MCUBOOT_RAM_LOADING is set then IMAGE_EXECUTABLE_RAM_SIZE must be \
defined by the target"
#endif
#endif /* MCUBOOT_RAM_LOADING */

#ifndef FLASH_AREA_0_OFFSET
#error "FLASH_AREA_0_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_0_SIZE
#error "FLASH_AREA_0_SIZE must be defined by the target"
#endif

#ifndef FLASH_AREA_2_OFFSET
#error "FLASH_AREA_2_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_2_SIZE
#error "FLASH_AREA_2_SIZE must be defined by the target"
#endif

#if (MCUBOOT_IMAGE_NUMBER == 2)
#ifndef FLASH_AREA_1_OFFSET
#error "FLASH_AREA_1_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_1_SIZE
#error "FLASH_AREA_1_SIZE must be defined by the target"
#endif

#ifndef FLASH_AREA_3_OFFSET
#error "FLASH_AREA_3_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_3_SIZE
#error "FLASH_AREA_3_SIZE must be defined by the target"
#endif
#endif /* (MCUBOOT_IMAGE_NUMBER == 2) */

#ifndef FLASH_AREA_SCRATCH_OFFSET
#error "FLASH_AREA_SCRATCH_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_SCRATCH_SIZE
#error "FLASH_AREA_SCRATCH_SIZE must be defined by the target"
#endif

#ifndef FLASH_DEV_NAME
#error "BL2 supports CMSIS flash interface and device name must be specified"
#endif

#ifndef MCUBOOT_STATUS_MAX_ENTRIES
#error "MCUBOOT_STATUS_MAX_ENTRIES must be defined by the target"
#endif

#ifndef MCUBOOT_MAX_IMG_SECTORS
#error "MCUBOOT_MAX_IMG_SECTORS must be defined by the target"
#endif

#endif /* H_TARGETS_TARGET_ */
