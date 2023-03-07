/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_MMC_DISK_H_
#define _FSL_MMC_DISK_H_

#include <stdint.h>
#include "ff.h"
#include "diskio.h"

/*!
 * @addtogroup MMC Disk
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CD_USING_GPIO

/*************************************************************************************************
 * API
 ************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name MMC Disk Function
 * @{
 */

/*!
 * @brief Initializes MMC disk.
 *
 * @param physicalDrive Physical drive number.
 * @retval STA_NOINIT Failed.
 * @retval RES_OK Success.
 */
DSTATUS mmc_disk_initialize(uint8_t physicalDrive);

/*!
 * @brief Gets MMC disk status
 *
 * @param physicalDrive Physical drive number.
 * @retval STA_NOINIT Failed.
 * @retval RES_OK Success.
 */
DSTATUS mmc_disk_status(uint8_t physicalDrive);

/*!
 * @brief Reads MMC disk.
 *
 * @param physicalDrive Physical drive number.
 * @param buffer The data buffer pointer to store read content.
 * @param sector The start sector number to be read.
 * @param count The sector count to be read.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT mmc_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count);

/*!
 * @brief Writes MMC disk.
 *
 * @param physicalDrive Physical drive number.
 * @param buffer The data buffer pointer to store write content.
 * @param sector The start sector number to be written.
 * @param count The sector count to be written.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT mmc_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count);

/*!
 * @brief MMC disk IO operation.
 *
 * @param physicalDrive Physical drive number.
 * @param command The command to be set.
 * @param buffer The buffer to store command result.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT mmc_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer);

/* @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_MMC_DISK_H_ */
