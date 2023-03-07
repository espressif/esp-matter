/***************************************************************************//**
 * @file    iot_flash_drv.h
 * @brief   Flash driver header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_FLASH_DRV_H_
#define _IOT_FLASH_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "iot_flash_desc.h"
#include "iot_flash_drv_msc.h"
#include "iot_flash_drv_spi.h"

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

/************************* iot_flash_drv_driver_init **************************/

static inline
sl_status_t iot_flash_drv_driver_init(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_driver_init(pvHndl);
  } else {
    return iot_flash_drv_spi_driver_init(pvHndl);
  }
}

/************************ iot_flash_drv_driver_deinit *************************/

static inline
sl_status_t iot_flash_drv_driver_deinit(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_driver_deinit(pvHndl);
  } else {
    return iot_flash_drv_spi_driver_deinit(pvHndl);
  }
}

/************************ iot_flash_drv_get_flash_size ************************/

static inline
sl_status_t iot_flash_drv_get_flash_size(void *pvHndl, uint32_t *ulFlashSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_flash_size(pvHndl, ulFlashSize);
  } else {
    return iot_flash_drv_spi_get_flash_size(pvHndl, ulFlashSize);
  }
}

/************************ iot_flash_drv_get_block_size ************************/

static inline
sl_status_t iot_flash_drv_get_block_size(void *pvHndl, uint32_t *ulBlockSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_block_size(pvHndl, ulBlockSize);
  } else {
    return iot_flash_drv_spi_get_block_size(pvHndl, ulBlockSize);
  }
}

/*********************** iot_flash_drv_get_sector_size ************************/

static inline
sl_status_t iot_flash_drv_get_sector_size(void *pvHndl, uint32_t *ulSectorSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_sector_size(pvHndl, ulSectorSize);
  } else {
    return iot_flash_drv_spi_get_sector_size(pvHndl, ulSectorSize);
  }
}

/************************ iot_flash_drv_get_page_size *************************/

static inline
sl_status_t iot_flash_drv_get_page_size(void *pvHndl, uint32_t *ulPageSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_page_size(pvHndl, ulPageSize);
  } else {
    return iot_flash_drv_spi_get_page_size(pvHndl, ulPageSize);
  }
}

/************************ iot_flash_drv_get_lock_size *************************/

static inline
sl_status_t iot_flash_drv_get_lock_size(void *pvHndl, uint32_t *ulLockSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_lock_size(pvHndl, ulLockSize);
  } else {
    return iot_flash_drv_spi_get_lock_size(pvHndl, ulLockSize);
  }
}

/************************ iot_flash_drv_get_async_flag ************************/

static inline
sl_status_t iot_flash_drv_get_async_flag(void *pvHndl, uint8_t *ubAsyncSupport)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_async_flag(pvHndl, ubAsyncSupport);
  } else {
    return iot_flash_drv_spi_get_async_flag(pvHndl, ubAsyncSupport);
  }
}

/************************ iot_flash_drv_get_device_id *************************/

static inline
sl_status_t iot_flash_drv_get_device_id(void *pvHndl, uint32_t *ulDeviceId)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_get_device_id(pvHndl, ulDeviceId);
  } else {
    return iot_flash_drv_spi_get_device_id(pvHndl, ulDeviceId);
  }
}

/************************* iot_flash_drv_erase_sector *************************/

static inline
sl_status_t iot_flash_drv_erase_sector(void *pvHndl, uint32_t ulAddress)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_erase_sector(pvHndl, ulAddress);
  } else {
    return iot_flash_drv_spi_erase_sector(pvHndl, ulAddress);
  }
}

/************************* iot_flash_drv_erase_chip ***************************/

static inline
sl_status_t iot_flash_drv_erase_chip(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_erase_chip(pvHndl);
  } else {
    return iot_flash_drv_spi_erase_chip(pvHndl);
  }
}

/************************* iot_flash_drv_data_write ***************************/

static inline
sl_status_t iot_flash_drv_data_write(void *pvHndl,
                                     uint32_t ulAddress,
                                     uint8_t *pubBuffer,
                                     uint32_t ulSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_data_write(pvHndl, ulAddress, pubBuffer, ulSize);
  } else {
    return iot_flash_drv_spi_data_write(pvHndl, ulAddress, pubBuffer, ulSize);
  }
}

/************************** iot_flash_drv_data_read ***************************/

static inline
sl_status_t iot_flash_drv_data_read(void *pvHndl,
                                    uint32_t ulAddress,
                                    uint8_t *pubBuffer,
                                    uint32_t ulSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->lInstType == 0) {
    return iot_flash_drv_msc_data_read(pvHndl, ulAddress, pubBuffer, ulSize);
  } else {
    return iot_flash_drv_spi_data_read(pvHndl, ulAddress, pubBuffer, ulSize);
  }
}

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_FLASH_DRV_H_ */
