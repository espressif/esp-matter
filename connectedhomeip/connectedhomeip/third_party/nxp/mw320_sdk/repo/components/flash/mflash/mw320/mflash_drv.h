/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include "mflash_common.h"

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE (4096U)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (256U)
#endif

/* Device specific settings */
#ifndef MFLASH_QSPI
#define MFLASH_QSPI (QSPI)
#endif

#ifndef MFLASH_BASE_ADDRESS
#define MFLASH_BASE_ADDRESS (0x1F000000U)
#endif

#define FLASH_SIZE 0x00001000U /* 4MB in KB unit */

/*! @brief Write data of arbitrary length */
int32_t mflash_drv_write(uint32_t addr, uint32_t *buffer, uint32_t len);

/*! @brief Erase data of arbitrary length */
int32_t mflash_drv_erase(uint32_t addr, uint32_t len);
#endif
