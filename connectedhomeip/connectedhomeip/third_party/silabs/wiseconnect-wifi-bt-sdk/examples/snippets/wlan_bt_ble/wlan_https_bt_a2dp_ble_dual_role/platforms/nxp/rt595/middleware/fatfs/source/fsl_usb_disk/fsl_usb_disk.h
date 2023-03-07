/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MSD_DISKIO_H_
#define _MSD_DISKIO_H_

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_msd.h"
#include "ff.h"
#include "diskio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief mass storage read/write retry time */
#define USB_HOST_FATFS_RW_RETRY_TIMES                   (2U)

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief fatfs call this function to initialize physical disk.
 *
 * @param pdrv   Physical drive number.
 *
 * @retval 0x00    success.
 */
extern DSTATUS USB_HostMsdInitializeDisk(BYTE pdrv);

/*!
 * @brief fatfs call this function to get physical disk status.
 *
 * @param pdrv           Physical drive number.
 *
 * @retval 0x00    OK.
 */
extern DSTATUS USB_HostMsdGetDiskStatus(BYTE pdrv);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param buff           Pointer to the data buffer to store read data.
 * @param sector         Start sector number.
 * @param count          Number of sectors to read.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    read disk error.
 */
extern DRESULT USB_HostMsdReadDisk(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param buff           Pointer to the data buffer to be written.
 * @param sector         Start sector number.
 * @param count          Number of sectors to read.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    write disk error.
 */
extern DRESULT USB_HostMsdWriteDisk(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param cmd            ioctl command, please reference to diskio.h
 * @param buff           Parameter or data buffer.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    write disk error.
 */
extern DRESULT USB_HostMsdIoctlDisk(BYTE pdrv, BYTE cmd, void *buff);

#endif /* _MSD_DISKIO_H_ */

