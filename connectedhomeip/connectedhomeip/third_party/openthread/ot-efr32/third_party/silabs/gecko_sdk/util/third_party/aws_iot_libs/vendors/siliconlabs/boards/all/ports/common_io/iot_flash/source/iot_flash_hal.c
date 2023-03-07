/***************************************************************************//**
 * @file    iot_flash_hal.c
 * @brief   Silicon Labs implementation of Common I/O Flash API.
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

/* for size_t which is used in iot_flash.h without including stddef */
#include "stddef.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"

/* Flash driver layer */
#include "iot_flash_desc.h"
#include "iot_flash_drv.h"

/* Flash iot layer */
#include "iot_flash.h"

/*******************************************************************************
 *                            iot_flash_open()
 ******************************************************************************/

/*!
 * @brief   iot_flash_open is used to initialize the flash device.
 *          This must be called before using any other flash APIs.
 *          This function Initializes the peripheral, configures buses etc.
 *
 * @param[in]   lFlashInstance  The instance of the flash to initialize.
 *                              If there are more than one flash per device, the
 *                              first flash is 0, the second flash is 1 etc.
 *
 * @return
 *   - returns the handle IotFlashHandle_t on success
 *   - NULL if
 *       - instance number is invalid
 *       - same instance is already open.
 */
IotFlashHandle_t iot_flash_open(int32_t lFlashInstance)
{
  /* local variables */
  IotFlashHandle_t pxFlashHandle   = NULL;

  /* retrieve pxFlashHandle using instance number */
  pxFlashHandle = iot_flash_desc_get(lFlashInstance);

  /* validate pxFlashHandle */
  if (pxFlashHandle == NULL) {
    return NULL;
  }

  /* flash shouldn't be open */
  if (pxFlashHandle->ucIsOpen == pdTRUE) {
    return NULL;
  }

  /* initialize flash h/w */
  iot_flash_drv_driver_init(pxFlashHandle);

  /* mark flash descriptor as open */
  pxFlashHandle->ucIsOpen= pdTRUE;

  /* done */
  return pxFlashHandle;
}

/*******************************************************************************
 *                            iot_flash_getinfo()
 ******************************************************************************/

/*!
 * @brief   iot_flash_getinfo is used to get the information
 *          about the physical flash device.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return
 *   - the pointer to flash information structure IotFlashInfo_t
 *   - NULL if pxFlashHandle was invalid
 */
IotFlashInfo_t * iot_flash_getinfo(IotFlashHandle_t const pxFlashHandle)
{
  /* local variables */
  IotFlashInfo_t   *pxInfo         = NULL;
  sl_status_t       xSlStatus      = SL_STATUS_OK;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return NULL;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return NULL;
  }

  /* create a pointer to xFlashInfo */
  pxInfo = &pxFlashHandle->xFlashInfo;

  /* (1) get flash size */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_flash_size(pxFlashHandle,
                                           &pxInfo->ulFlashSize);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (2) get block size */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_block_size(pxFlashHandle,
                                           &pxInfo->ulBlockSize);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (3) get sector size */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_sector_size(pxFlashHandle,
                                            &pxInfo->ulSectorSize);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (4) get page size */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_page_size(pxFlashHandle,
                                          &pxInfo->ulPageSize);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (5) get lock size */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_lock_size(pxFlashHandle,
                                          &pxInfo->ulLockSupportSize);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (6) get async support */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_async_flag(pxFlashHandle,
                                           &pxInfo->ucAsyncSupported);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* (7) get device id */
  /* retrieve flash memory info */
  xSlStatus = iot_flash_drv_get_device_id(pxFlashHandle,
                                          &pxInfo->ulFlashID);
  if (xSlStatus == SL_STATUS_FAIL) {
    return NULL;
  }

  /* done */
  return pxInfo;
}

/*******************************************************************************
 *                           iot_flash_set_callback()
 ******************************************************************************/

/*!
 * @brief   iot_flash_set_callback is used to set the callback to be called upon completion
 *          of erase/program/read. The callback is used only if the underlying HW supports
 *          asynchronous operations. Caller can check if asynchronous operations are
 *          supported by checking the "ucAsyncSupported" flag in IotFlashInfo_t structure.
 *          If asynchronous operations are not supported, then erase/write/read operations
 *          are blocking operations, and this API has no affect, i.e even if a callback is set,
 *          it will never be called back.
 *
 * @note Single callback is used for asynchronous read / write / erase APIs.
 * @note Newly set callback overrides the one previously set
 * @note This callback will not be invoked when synchronous operation completes.
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   xCallback       callback function to be called.
 * @param[in]   pvUserContext   user context to be passed when callback is called.
 *
 * @return      None
 */
void iot_flash_set_callback(IotFlashHandle_t const pxFlashHandle,
                            IotFlashCallback_t xCallback,
                            void * pvUserContext)
{
  /* async mode is not supported */
  (void) pxFlashHandle;
  (void) xCallback;
  (void) pvUserContext;
  return;
}

/*******************************************************************************
 *                             iot_flash_ioctl()
 ******************************************************************************/

/*!
 * @brief   iot_flash_ioctl is used to configure the flash parameters and setup
 *          certain flash operations and also used to read flash info and configuration.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   xRequest        configuration request of type IotFlashIoctlRequest_t
 * @param[in,out] pvBuffer      configuration values to be written to flash or to be read from flash.
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success
 *   - IOT_FLASH_INVALID_VALUE on NULL pxFlashHandle, invalid xRequest, or NULL pvBuffer when required.
 *   - IOT_FLASH_DEVICE_BUSY if previous flash command still in progress.
 *   - IOT_FLASH_CTRL_OP_FAILED if ioctl operation failed for any reason.
 *   - IOT_FLASH_FUNCTION_NOT_SUPPORTED valid only for the following if not supported
 *      - eSetFlashBlockProtection / eGetFlashBlockProtection
 *      - eSuspendFlashProgramErase / eResumeFlashProgramErase
 */
int32_t iot_flash_ioctl(IotFlashHandle_t const pxFlashHandle,
                        IotFlashIoctlRequest_t xRequest,
                        void * const pvBuffer)
{
  /* local variables */
  IotFlashStatus_t *pxFlashStatus     = NULL;
  uint32_t         *pulFlashNoOfBytes = NULL;
  int32_t           lStatus           = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* process request */
  switch (xRequest) {
    case eSetFlashBlockProtection:
    case eGetFlashBlockProtection:
    case eSuspendFlashProgramErase:
    case eResumeFlashProgramErase:
      lStatus = IOT_FLASH_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetFlashStatus:
      pxFlashStatus = pvBuffer;
      *pxFlashStatus = eFlashIdle;
      break;

    case eGetFlashTxNoOfbytes:
    case eGetFlashRxNoOfbytes:
      pulFlashNoOfBytes = pvBuffer;
      *pulFlashNoOfBytes = 0;
      break;

    default:
      lStatus = IOT_FLASH_INVALID_VALUE;
      break;
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                          iot_flash_erase_sectors()
 ******************************************************************************/

/*!
 * @brief   iot_flash_erase_sectors is used to erase data in flash from the start
 *          of the address specified until the startAddress plus size passed.
 *          The address passed in 'ulAddress' must be aligned to ulSectorSize,
 *          and size must be a multiple of ulSectorSize. If there is another flash operation
 *          is in progress, the erase_sectors API will return an error and this usually happens if
 *          flash supports asynchronous erase/write/read operations.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulStartAddress  starting address(offset) in flash, from where erase starts. Aligned to ulSectorSize
 * @param[in]   xSize           size of the flash range to be erased. multiple of ulSectorSize
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_ERASE_FAILED on error.
 *
 * <b>Example</b>
 * @code{c}
 *   IotFlashHandle_t xFlashHandle;
 *   IotFlashInfo_t* pxFlashInfo;
 *   int32_t lRetVal;
 *   uint32_t ulChunkOffset;
 *
 *   // Open flash to initialize hardware.
 *   xFlashHandle = iot_flash_open(0);
 *   // assert(xFlashHandle == NULL );
 *
 *   // Get the flash information.
 *   pxFlashInfo = iot_flash_getinfo(xFlashHandle);
 *   // assert(pxFlashInfo == NULL);
 *
 *   // If Erase asyc is supported, register a callback
 *   if ( pxFlashInfo->ucAsyncSupported )
 *   {
 *       iot_flash_set_callback(xFlashHandle,
 *                              prvIotFlashEraseCallback,
 *                              NULL);
 *   }
 *
 *   // Erase 2 sectors
 *   lRetVal = iot_flash_erase_sectors(xFlashHandle,
 *                                     ultestIotFlashStartOffset,
 *                                     pxFlashInfo->ulSectorSize * 2);
 *   //assert(IOT_FLASH_SUCCESS != lRetVal);
 *
 *   if ( pxFlashInfo->ucAsyncSupported )
 *   {
 *       // Wait for the Erase to be completed and callback is called.
 *       lRetVal = xSemaphoreTake(xtestIotFlashSemaphore, portMAX_DELAY);
 *       //assert(pdTRUE != lRetVal);
 *   }
 *
 *   // Close the flash handle.
 *   lRetVal = iot_flash_close(xFlashHandle);
 *   //assert(IOT_FLASH_SUCCESS != lRetVal);
 *
 * @endcode
 */
int32_t iot_flash_erase_sectors(IotFlashHandle_t const pxFlashHandle,
                                uint32_t ulStartAddress,
                                size_t xSize)
{
  /* local variables */
  uint32_t    ulFlashSize    = 0;
  uint32_t    ulSectorSize   = 0;
  uint32_t    ulCurAddress   = 0;
  uint32_t    ulEndAddress   = 0;
  sl_status_t xSlStatus      = SL_STATUS_OK;
  int32_t     lStatus        = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* obtain flash size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_get_flash_size(pxFlashHandle, &ulFlashSize);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* obtain sector size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_get_sector_size(pxFlashHandle, &ulSectorSize);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* make sure ulStartAddress is within range */
  if (lStatus == IOT_FLASH_SUCCESS) {
    if (ulStartAddress >= ulFlashSize) {
      lStatus = IOT_FLASH_INVALID_VALUE;
    }
  }

  /* ulStartAddress must be multiple of sector size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    if (ulStartAddress & (ulSectorSize - 1)) {
      lStatus = IOT_FLASH_INVALID_VALUE;
    }
  }

  /* xSize must be multiple of sector size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    if (xSize & (ulSectorSize - 1)) {
      lStatus = IOT_FLASH_INVALID_VALUE;
    }
  }

  /* erase sectors one by one */
  if (lStatus == IOT_FLASH_SUCCESS) {
    /* set current address to first address */
    ulCurAddress = ulStartAddress;

    /* set last address */
    ulEndAddress = ulStartAddress + xSize;

    /* loop over sectors */
    while (lStatus == IOT_FLASH_SUCCESS && ulCurAddress < ulEndAddress) {
      /* erase current sector */
      xSlStatus = iot_flash_drv_erase_sector(pxFlashHandle, ulCurAddress);

      /* operation failed? */
      if (xSlStatus == SL_STATUS_FAIL) {
        lStatus = IOT_FLASH_ERASE_FAILED;
      }

      /* next sector */
      if (lStatus == IOT_FLASH_SUCCESS) {
        ulCurAddress += ulSectorSize;
      }
    }
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_flash_erase_chip()
 ******************************************************************************/

/*!
 * @brief   iot_erase_chip is used to erase the entire flash chip.
 *          If there is another flash operation is in progress, the erase_chip API will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_ERASE_FAILED on error.
 */
int32_t iot_flash_erase_chip(IotFlashHandle_t const pxFlashHandle)
{
  /* local variables */
  sl_status_t xSlStatus      = SL_STATUS_OK;
  int32_t     lStatus        = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* erase all pages */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_erase_chip(pxFlashHandle);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_flash_write_sync()
 ******************************************************************************/

/*!
 * @brief   iot_flash_write_sync is used to write data to flash starting at the address provided.
 *          The sector(s) being written to, must be erased first before any write can take place.
 *          This is a blocking operation and waits until the number of bytes are written before returning.
 *          If there is another flash operation is in progress, write will return an error.
 *
 * @warning writing to a sector that was not erased first, may result in incorrect data being written while
 *          the API returns IOT_FLASH_SUCCESS.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to write.
 * @param[in]   xBytes          number of bytes to write.
 * @param[in]   pvBuffer        data buffer to write to flash
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_WRITE_FAILED on error.
 */
int32_t iot_flash_write_sync(IotFlashHandle_t const pxFlashHandle,
                             uint32_t ulAddress,
                             uint8_t * const pvBuffer,
                             size_t xBytes)
{
  /* local variables */
  uint32_t     ulFlashSize    = 0;
  uint32_t     ulPageSize     = 0;
  uint32_t     ulCurAddress   = 0;
  uint32_t     ulCurSize      = 0;
  uint32_t     ulRemSize      = 0;
  uint8_t     *pubCurBuf      = NULL;
  sl_status_t  xSlStatus      = SL_STATUS_OK;
  int32_t      lStatus        = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* obtain flash size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_get_flash_size(pxFlashHandle, &ulFlashSize);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* obtain page size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_get_page_size(pxFlashHandle, &ulPageSize);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* make sure ulAddress is within range */
  if (lStatus == IOT_FLASH_SUCCESS) {
    if (ulAddress >= ulFlashSize) {
      lStatus = IOT_FLASH_INVALID_VALUE;
    }
  }

  /* write pages one page at a time */
  if (lStatus == IOT_FLASH_SUCCESS) {
    /* address of current page being written */
    ulCurAddress = ulAddress;

    /* current buffer pointer */
    pubCurBuf = pvBuffer;

    /* total size to be written */
    ulRemSize = xBytes;

    /* loop over pages */
    while (lStatus == IOT_FLASH_SUCCESS && ulRemSize > 0) {
      /* check if write is partial or full page write */
      ulCurSize = ulPageSize - (ulAddress & 0xFF);

      /* buffer is smaller than ulCurSize? */
      if (ulRemSize < ulCurSize) {
        ulCurSize = ulRemSize;
      }

      /* write data */
      xSlStatus = iot_flash_drv_data_write(pxFlashHandle,
                                           ulCurAddress,
                                           pubCurBuf,
                                           ulCurSize);

      /* update status flag based on the result of operation */
      if (xSlStatus == SL_STATUS_FAIL) {
        lStatus = IOT_FLASH_WRITE_FAILED;
      }

      /* continue to next page */
      if (xSlStatus == SL_STATUS_OK) {
        ulCurAddress += ulCurSize;
        pubCurBuf    += ulCurSize;
        ulRemSize    -= ulCurSize;
      }
    }
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_flash_read_sync()
 ******************************************************************************/

/*!
 * @brief   iot_flash_read_sync is used to read data from flash. This is a blocking operation
 *          and waits until the number of bytes are read before returning.
 *          If there is another flash operation is in progress, this will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to read.
 * @param[in]   xBytes          number of bytes to be read.
 * @param[out]  pvBuffer        data buffer to hold the data read from flash
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_READ_FAILED on error.
 */
int32_t iot_flash_read_sync(IotFlashHandle_t const pxFlashHandle,
                            uint32_t ulAddress,
                            uint8_t * const pvBuffer,
                            size_t xBytes)
{
  /* local variables */
  uint32_t    ulFlashSize    = 0;
  sl_status_t xSlStatus      = SL_STATUS_OK;
  int32_t     lStatus        = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* make sure pxFlashHandle is open */
  if (pxFlashHandle->ucIsOpen == pdFALSE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* obtain flash size */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_get_flash_size(pxFlashHandle, &ulFlashSize);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_ERASE_FAILED;
    }
  }

  /* make sure ulAddress is within range */
  if (lStatus == IOT_FLASH_SUCCESS) {
    if (ulAddress >= ulFlashSize) {
      lStatus = IOT_FLASH_INVALID_VALUE;
    }
  }

  /* read data */
  if (lStatus == IOT_FLASH_SUCCESS) {
    xSlStatus = iot_flash_drv_data_read(pxFlashHandle,
                                        ulAddress,
                                        pvBuffer,
                                        xBytes);
    if (xSlStatus == SL_STATUS_FAIL) {
      lStatus = IOT_FLASH_READ_FAILED;
    }
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_flash_write_async()
 ******************************************************************************/

/*!
 * @brief   iot_flash_write_async is used to write data to flash starting at the address provided.
 *          The sector(s) being written to, must be erased first before any write can take place.
 *          This is an asynchronous (non-blocking) operation and returns as soon as the write
 *          operation is started. When the write is completed, user callback is called to
 *          notify that the write is complete. The caller can check the status of the operation
 *          by using eGetStatus IOCTL. User must register for a callback when using the non-blocking
 *          operations to know when they are complete.
 *          If there is another flash operation is in progress, write will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to write.
 * @param[in]   xBytes          number of bytes to write.
 * @param[in]   pvBuffer        data buffer to write to flash
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_WRITE_FAILED on error.
 *   - IOT_FLASH_FUNCTION_NOT_SUPPORTED if asynchronous operation is not supported
 *        (i,e ucAsyncSupported is set to false)
 */
int32_t iot_flash_write_async(IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes)
{
  /* async mode is not supported */
  (void) pxFlashHandle;
  (void) ulAddress;
  (void) pvBuffer;
  (void) xBytes;
  return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

/*******************************************************************************
 *                           iot_flash_read_async()
 ******************************************************************************/

/*!
 * @brief   iot_flash_read_async is used to read data from flash.
 *          This is an asynchronous (non-blocking) operation and returns as soon as the read
 *          operation is started. When the read is completed, user callback is called to
 *          notify that the read is complete. The caller can check the status of the operation
 *          by using eGetStatus IOCTL and use the buffer. User must register for a callback when
 *          using the non-blocking operations to know when they are complete.
 *          If there is another flash operation is in progress, this will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to read.
 * @param[in]   xBytes          number of bytes to be read.
 * @param[out]  pvBuffer        data buffer to hold the data read from flash
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success.
 *   - IOT_FLASH_INVALID_VALUE if any parameter is invalid.
 *   - IOT_FLASH_DEVICE_BUSY if another asynchronous operation is currently being executed.
 *   - IOT_FLASH_READ_FAILED on error.
 *   - IOT_FLASH_FUNCTION_NOT_SUPPORTED if asynchronous operation is not supported
 *        (i,e ucAsyncSupported is set to false)
 */
int32_t iot_flash_read_async(IotFlashHandle_t const pxFlashHandle,
                             uint32_t ulAddress,
                             uint8_t * const pvBuffer,
                             size_t xBytes)
{
  /* async mode is not supported */
  (void) pxFlashHandle;
  (void) ulAddress;
  (void) pvBuffer;
  (void) xBytes;
  return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

/*******************************************************************************
 *                             iot_flash_close()
 ******************************************************************************/

/*!
 * @brief   iot_flash_close is used to close the flash device.
 *          If any operations are in progress when close is called,
 *          flash driver aborts those if possible.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return
 *   - IOT_FLASH_SUCCESS on success close
 *   - IOT_FLASH_INVALID_VALUE on invalid pxFlashHandle.
 */
int32_t iot_flash_close(IotFlashHandle_t const pxFlashHandle)
{
  /* local variables */
  int32_t lStatus = IOT_FLASH_SUCCESS;

  /* pxFlashHandle can't be null */
  if (pxFlashHandle == NULL) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* pxFlashHandle must be open */
  if (pxFlashHandle->ucIsOpen != pdTRUE) {
    return IOT_FLASH_INVALID_VALUE;
  }

  /* disable flash h/w */
  if (lStatus == IOT_FLASH_SUCCESS) {
    iot_flash_drv_driver_deinit(pxFlashHandle);
  }

  /* close the flash handle */
  if (lStatus == IOT_FLASH_SUCCESS) {
    pxFlashHandle->ucIsOpen = pdFALSE;
  }

  /* done */
  return lStatus;
}
