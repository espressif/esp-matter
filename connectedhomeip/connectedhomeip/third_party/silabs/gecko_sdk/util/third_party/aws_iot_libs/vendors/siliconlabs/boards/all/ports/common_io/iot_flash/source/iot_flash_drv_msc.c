/***************************************************************************//**
 * @file    iot_flash_drv_msc.c
 * @brief   MSC Flash Driver
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
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_msc.h"

/* SDK utils layer */
#if defined(SEMAILBOX_PRESENT)
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"
#endif

/* Flash driver layer */
#include "iot_flash_desc.h"
#include "iot_flash_drv_msc.h"

/*******************************************************************************
 *                               MSC SIZES
 ******************************************************************************/

/* flash memory base address */
#ifndef IOT_FLASH_DRV_MSC_MEMORY_BASE
#define IOT_FLASH_DRV_MSC_MEMORY_BASE   0x00000000UL
#endif

/* flash memory total size */
#ifndef IOT_FLASH_DRV_MSC_MEMORY_SIZE
#define IOT_FLASH_DRV_MSC_MEMORY_SIZE   0x10000000UL
#endif

/* flash block size */
#ifndef IOT_FLASH_DRV_MSC_BLOCK_SIZE
#define IOT_FLASH_DRV_MSC_BLOCK_SIZE    (32*FLASH_PAGE_SIZE)
#endif

/* flash sector size (1 MSC page == 1 sector) */
#ifndef IOT_FLASH_DRV_MSC_SECTOR_SIZE
#define IOT_FLASH_DRV_MSC_SECTOR_SIZE   (FLASH_PAGE_SIZE)
#endif

/* flash page size */
#ifndef IOT_FLASH_DRV_MSC_PAGE_SIZE
#define IOT_FLASH_DRV_MSC_PAGE_SIZE     (256)
#endif

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

#if defined(SEMAILBOX_PRESENT)
/* secure element command context */
static sl_se_command_context_t sli_drv_flash_se_cmd_ctx = {0};
#endif

/*******************************************************************************
 *                     iot_flash_drv_msc_init_clk()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_init_clk()
{
#if defined(cmuClock_MSC)
  /* enable MSC clock if supported */
  CMU_ClockEnable(cmuClock_MSC, true);
#endif

  /* done */
  return 1;
}

/*******************************************************************************
 *                     iot_flash_drv_msc_init_se()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_init_se()
{
#if defined(SEMAILBOX_PRESENT)
  /* initialize secure element if exists */
  sl_se_init();
  sl_se_init_command_context(&sli_drv_flash_se_cmd_ctx);
  sl_se_set_yield(&sli_drv_flash_se_cmd_ctx, true);
#endif

  /* done */
  return 1;
}

/*******************************************************************************
 *                     iot_flash_drv_msc_page_erase()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_page_erase(uint32_t ulAddress)
{
  /* variables to hold the status of each operations */
  MSC_Status_TypeDef xHwStatus = mscReturnOk;

#if defined(SEMAILBOX_PRESENT)
  /* If SE is present, we need to use SE manager for locked pages */
  uint32_t     ulUserDataStart = (uint32_t) (USERDATA_BASE);
  uint32_t     ulUserDataEnd   = (uint32_t) (USERDATA_BASE+USERDATA_SIZE);
  sl_status_t  xSeStatus       = SL_STATUS_OK;
#endif

#if defined(SEMAILBOX_PRESENT)
  /* Secure Element is present */
  if (ulAddress >= ulUserDataStart && ulAddress <= ulUserDataEnd) {
    /* Ask Secure Element to erase user data */
    xSeStatus = sl_se_erase_user_data(&sli_drv_flash_se_cmd_ctx);
  } else {
    /* This will return mscReturnLock if pages are locked by SE */
    MSC_Init();
    xHwStatus = MSC_ErasePage((uint32_t*)(ulAddress));
    xSeStatus = (xHwStatus == mscReturnOk) ? SL_STATUS_OK : SL_STATUS_FAIL;
    MSC_Deinit();
  }
#else
  /* erase 1 sector (actually 1 MSC page) */
  MSC_Init();
  xHwStatus = MSC_ErasePage((uint32_t*)(ulAddress));
  MSC_Deinit();
#endif

#if defined(SEMAILBOX_PRESENT)
  /* done (use xSeStatus) */
  return (xSeStatus == SL_STATUS_OK);
#else
  /* done (use xHwStatus) */
  return (xHwStatus == mscReturnOk);
#endif
}

/*******************************************************************************
 *                     iot_flash_drv_msc_mass_erase()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_mass_erase()
{
  /* variables to hold the status of each operation */
  MSC_Status_TypeDef xHwStatus = mscReturnOk;

  /* Erase everything in main flash. */
  MSC_Init();
  xHwStatus = MSC_MassErase();
  MSC_Deinit();

  /* done */
  return (xHwStatus == mscReturnOk);
}

/*******************************************************************************
 *                     iot_flash_drv_msc_byte_read()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_byte_read(uint32_t ulAddress,
                                                uint8_t *pubData)
{
  /* read single byte from internal flash */
  *pubData = *((uint8_t *) ulAddress);

  /* done */
  return 1;
}

/*******************************************************************************
 *                     iot_flash_drv_msc_byte_write()
 ******************************************************************************/

static uint32_t iot_flash_drv_msc_byte_write(uint32_t ulAddress,
                                                 uint8_t ubData)
{
  /* MSC status */
  MSC_Status_TypeDef xHwStatus = mscReturnOk;

#if defined(SEMAILBOX_PRESENT)
  /* if SE is present, we need to use SE manager for locked pages */
  uint32_t     ulUserDataStart = (uint32_t) (USERDATA_BASE);
  uint32_t     ulUserDataEnd   = (uint32_t) (USERDATA_BASE+USERDATA_SIZE);
  sl_status_t  xSeStatus       = SL_STATUS_OK;
#endif

  /* temporary buffer for 4-bytes surrounding ulAddress */
  uint32_t     ulWordBuffer    = 0;

  /* read 4 bytes */
  ulWordBuffer = *((uint32_t *) (ulAddress & (~3)));

  /* modify one byte in ulWordBuffer, which will be wrriten as 4-byte word */
  ((uint8_t *) &ulWordBuffer)[ulAddress & 3] = ubData;

  /* align ulAddress to 4 bytes */
  ulAddress = ulAddress & (~3);

#if defined(SEMAILBOX_PRESENT)
  /* use SE manager for secured/locked areas if possible */
  if (ulAddress >= ulUserDataStart && ulAddress < ulUserDataEnd) {
    /* write data on SE user data */
    xSeStatus = sl_se_write_user_data(&sli_drv_flash_se_cmd_ctx,
                                      ulAddress-ulUserDataStart,
                                      &ulWordBuffer,
                                      4);
  } else {
    /* this will return mscReturnLock if pages are locked by SE */
    MSC_Init();
    xHwStatus = MSC_WriteWord((uint32_t *)(ulAddress), &ulWordBuffer, 4);
    xSeStatus = (xHwStatus == mscReturnOk) ? SL_STATUS_OK : SL_STATUS_FAIL;
    MSC_Deinit();
  }
#else
  /* SE is not present. Use MSC directly. */
  MSC_Init();
  xHwStatus = MSC_WriteWord((uint32_t *)(ulAddress), &ulWordBuffer, 4);
  MSC_Deinit();
#endif

#if defined(SEMAILBOX_PRESENT)
  /* done (use xSeStatus) */
  return (xSeStatus == SL_STATUS_OK);
#else
  /* done (use xHwStatus) */
  return (xHwStatus == mscReturnOk);
#endif
}

/*******************************************************************************
 *                    iot_flash_drv_msc_driver_init()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_driver_init(void *pvHndl)
{
  /* init status */
  uint32_t ulSuccess = 1;

  /* unused parameter */
  (void) pvHndl;

  /* initialize MSC clock if exists */
  if (ulSuccess) {
    ulSuccess = iot_flash_drv_msc_init_clk();
  }

  /* initialize Secure Element if exists */
  if (ulSuccess) {
    ulSuccess = iot_flash_drv_msc_init_se();
  }

  /* done */
  return ulSuccess == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_msc_driver_deinit()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_driver_deinit(void *pvHndl)
{
  /* unused parameter */
  (void) pvHndl;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_msc_get_flash_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_flash_size(void *pvHndl,
                                             uint32_t *ulFlashSize)
{
  /* unused parameter */
  (void) pvHndl;

  /* return flash size */
  *ulFlashSize =  IOT_FLASH_DRV_MSC_MEMORY_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_msc_get_block_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_block_size(void *pvHndl,
                                             uint32_t *ulBlockSize)
{
  /* unused parameter */
  (void) pvHndl;

  /* return block size */
  *ulBlockSize =  IOT_FLASH_DRV_MSC_BLOCK_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_msc_get_sector_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_sector_size(void *pvHndl,
                                              uint32_t *ulSectorSize)
{
  /* unused parameter */
  (void) pvHndl;

  /* return sector size */
  *ulSectorSize =  IOT_FLASH_DRV_MSC_SECTOR_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_msc_get_page_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_page_size(void *pvHndl,
                                            uint32_t *ulPageSize)
{
  /* unused parameter */
  (void) pvHndl;

  /* return page size */
  *ulPageSize =  IOT_FLASH_DRV_MSC_PAGE_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_msc_get_lock_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_lock_size(void *pvHndl,
                                            uint32_t *ulLockSize)
{
  /* unused parameter */
  (void) pvHndl;

  /* return lock size */
  *ulLockSize = 0;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_msc_get_async_flag()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_async_flag(void *pvHndl,
                                             uint8_t *ubAsyncSupport)
{
  /* unused parameter */
  (void) pvHndl;

  /* return async support */
  *ubAsyncSupport = 0;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_msc_get_device_id()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_get_device_id(void *pvHndl,
                                            uint32_t *ulDeviceId)
{
  /* unused parameter */
  (void) pvHndl;

  /* return device identifier (internal flash) */
  *ulDeviceId = 0xFFFFFF;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_msc_erase_sector()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_erase_sector(void *pvHndl, uint32_t ulAddress)
{
  /* erase status */
  uint32_t ulSuccess = 1;

  /* unused parameter */
  (void) pvHndl;

  /* 1 MSC page == 1 flash sector */
  ulSuccess = iot_flash_drv_msc_page_erase(ulAddress);

  /* done */
  return ulSuccess == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                    iot_flash_drv_msc_erase_chip()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_erase_chip(void *pvHndl)
{
  /* erase status */
  uint32_t ulSuccess = 1;

  /* unused parameter */
  (void) pvHndl;

  /* perform MSC mass erase */
  ulSuccess = iot_flash_drv_msc_mass_erase();

  /* done */
  return ulSuccess == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_msc_data_write()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_data_write(void *pvHndl,
                                         uint32_t ulAddress,
                                         uint8_t *pubBuffer,
                                         uint32_t ulSize)
{
  /* loop counter */
  uint32_t ulCtr     = 0;

  /* write status */
  uint32_t ulSuccess = 1;

  /* unused parameter */
  (void) pvHndl;

  /* loop over buffer */
  for (ulCtr = 0; ulSuccess == 1 && ulCtr < ulSize; ulCtr++) {
    ulSuccess = iot_flash_drv_msc_byte_write(ulAddress + ulCtr,
                                                 pubBuffer[ulCtr]);
  }

  /* done */
  return ulSuccess == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                    iot_flash_drv_msc_data_read()
 ******************************************************************************/

sl_status_t iot_flash_drv_msc_data_read(void *pvHndl,
                                        uint32_t ulAddress,
                                        uint8_t *pubBuffer,
                                        uint32_t ulSize)
{
  /* loop counter */
  uint32_t ulCtr     = 0;

  /* read status */
  uint32_t ulSuccess = 1;

  /* unused parameter */
  (void) pvHndl;

  /* loop over buffer */
  for (ulCtr = 0; ulSuccess == 1 && ulCtr < ulSize; ulCtr++) {
    ulSuccess = iot_flash_drv_msc_byte_read(ulAddress + ulCtr,
                                                &pubBuffer[ulCtr]);
  }

  /* done */
  return ulSuccess == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}
