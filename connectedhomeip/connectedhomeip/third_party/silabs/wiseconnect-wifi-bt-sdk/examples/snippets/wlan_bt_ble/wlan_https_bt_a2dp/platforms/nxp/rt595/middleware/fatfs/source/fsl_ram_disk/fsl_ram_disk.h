/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_RAMDISK_H__
#define __FSL_RAMDISK_H__

#include "ff.h"
#include "diskio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/
DSTATUS ram_disk_initialize(BYTE pdrv);
DSTATUS ram_disk_status(BYTE pdrv);
DRESULT ram_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
DRESULT ram_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT ram_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#if defined(__cplusplus)
}
#endif

#endif /* __FSL_RAMDISK_H__ */
