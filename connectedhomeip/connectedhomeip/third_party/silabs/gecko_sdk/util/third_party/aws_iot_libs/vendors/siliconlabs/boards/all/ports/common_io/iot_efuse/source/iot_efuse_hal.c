/***************************************************************************//**
 * @file    iot_efuse_hal.c
 * @brief   Silicon Labs implementation of Common I/O EFUSE API.
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

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"

/* eFUSE driver layer */
#include "iot_efuse_drv.h"

/* eFUSE iot layer */
#include "iot_efuse.h"

/*******************************************************************************
 *                            DESCRIPTOR TYPE
 ******************************************************************************/

typedef struct IotEfuseDescriptor {
  uint8_t            ucIsOpen;
  uint32_t           ulSize;
  SemaphoreHandle_t  xMutex;
  StaticSemaphore_t  xMutexBuf;
} IotEfuseDescriptor_t;

/*******************************************************************************
 *                            DESCRIPTOR LIST
 ******************************************************************************/

static IotEfuseDescriptor_t iot_efuse_hal_desc;

/*******************************************************************************
 *                            iot_efuse_open()
 ******************************************************************************/

/**
 * @brief iot_efuse_open is used to Initialize things needed to access efuse.
 *
 * @return
 *   - Handle to IotEfuseHandle_t on success
 *   -  NULL if the handle is already opened. handle must be closed before calling open again
 */
IotEfuseHandle_t iot_efuse_open(void)
{
  /* local variables */
  IotEfuseHandle_t   pxEfuseHandle = NULL;
  StaticSemaphore_t *pxMutexBuf    = NULL;
  sl_status_t        xSlStatus     = SL_STATUS_OK;
  int32_t            lStatus       = IOT_EFUSE_SUCCESS;

  /* validate iot_efuse_hal_desc open flag */
  if (iot_efuse_hal_desc.ucIsOpen == pdTRUE) {
    return NULL;
  }

  /* retrieve static buffer to store the mutex in */
  pxMutexBuf = &(iot_efuse_hal_desc.xMutexBuf);

  /* create the mutex */
  iot_efuse_hal_desc.xMutex = xSemaphoreCreateMutexStatic(pxMutexBuf);

  /* enter critical section */
  xSemaphoreTake(iot_efuse_hal_desc.xMutex, portMAX_DELAY);

  /* initialize hardware */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(driver_init)();
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_ERROR;
    }
  }

  /* retrieve eFUSE size */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    iot_efuse_hal_desc.ulSize = IOT_EFUSE_CFG_MAX_SIZE;
  }

  /* mark descriptor as open */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    iot_efuse_hal_desc.ucIsOpen = pdTRUE;
  }

  /* set the handle pointer to be returned */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    pxEfuseHandle = &iot_efuse_hal_desc;
  }

  /* exit critical section */
  xSemaphoreGive(iot_efuse_hal_desc.xMutex);

  /* return the handle pointer */
  return pxEfuseHandle;
}

/*******************************************************************************
 *                            iot_efuse_close()
 ******************************************************************************/

/**
 * @brief iot_efuse_close is used to de Initialize things needed to disable efuse access.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if succeeded,
 *   - IOT_EFUSE_INVALID_VALUE on NULL pxEfuseHandle
 *   - IOT_EFUSE_INVALID_VALUE if instance not previously opened.
 *   - IOT_EFUSE_CLOSE_FAIL if the underneath HW deinit api returns fail.
 */
int32_t iot_efuse_close(IotEfuseHandle_t const pxEfuseHandle)
{
  /* local variables */
  sl_status_t xSlStatus = SL_STATUS_OK;
  int32_t     lStatus   = IOT_EFUSE_SUCCESS;

  /* validate pxEfuseHandle */
  if (pxEfuseHandle == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate pxEfuseHandle open flag */
  if (pxEfuseHandle->ucIsOpen == pdFALSE) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* enter critical section */
  xSemaphoreTake(pxEfuseHandle->xMutex, portMAX_DELAY);

  /* de-initialize hardware */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(driver_deinit)();
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_CLOSE_FAIL;
    }
  }

  /* exit critical section */
  xSemaphoreGive(pxEfuseHandle->xMutex);

  /* delete semaphore */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    vSemaphoreDelete(pxEfuseHandle->xMutex);
  }

  /* mark descriptor as closed */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    pxEfuseHandle->ucIsOpen = pdFALSE;
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                       iot_efuse_read_32bit_word()
 ******************************************************************************/

/**
 * @brief Read 32-bit efuse word from specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to read. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[out] ulValue The receive buffer to read the data into
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if read succeeded,
 *   - IOT_EFUSE_READ_FAIL if read failed,
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle is NULL,  index is invalid, or ulValue is NULL.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 32-bit efuse word is not supported
 */
int32_t iot_efuse_read_32bit_word(IotEfuseHandle_t const pxEfuseHandle,
                                  uint32_t ulIndex,
                                  uint32_t * ulValue)
{
  /* local variables */
  uint32_t     ulAddr    = 0;
  uint8_t     *pucBuf    = NULL;
  sl_status_t  xSlStatus = SL_STATUS_OK;
  int32_t      lStatus   = IOT_EFUSE_SUCCESS;

  /* validate pxEfuseHandle */
  if (pxEfuseHandle == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate pxEfuseHandle open flag */
  if (pxEfuseHandle->ucIsOpen == pdFALSE) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulIndex */
  if (ulIndex*4 >= pxEfuseHandle->ulSize) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulValue */
  if (ulValue == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* enter critical section */
  xSemaphoreTake(pxEfuseHandle->xMutex, portMAX_DELAY);

  /* convert ulIndex and ulValue to something the driver can understand */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    ulAddr = ulIndex * 4;
    pucBuf = (uint8_t *) ulValue;
  }

  /* read byte 0 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+0, pucBuf+0);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* read byte 1 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+1, pucBuf+1);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* read byte 2 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+2, pucBuf+2);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* read byte 3 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+3, pucBuf+3);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* exit critical section */
  xSemaphoreGive(pxEfuseHandle->xMutex);

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                       iot_efuse_write_32bit_word()
 ******************************************************************************/

/**
 * @brief Write 32-bit value to the 32-bit efuse word at specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to write to. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[in] ulValue The 32-bit value to write.
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if write succeeded,
 *   - IOT_EFUSE_WRITE_FAIL if write failed
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle is NULL, or index is invalid.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 32-bit efuse word is not supported
 */
int32_t iot_efuse_write_32bit_word(IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint32_t ulValue)
{
  /* local variables */
  uint32_t     ulAddr    = 0;
  uint8_t     *pucBuf    = NULL;
  sl_status_t  xSlStatus = SL_STATUS_OK;
  int32_t      lStatus   = IOT_EFUSE_SUCCESS;

  /* validate pxEfuseHandle */
  if (pxEfuseHandle == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate pxEfuseHandle open flag */
  if (pxEfuseHandle->ucIsOpen == pdFALSE) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulIndex */
  if (ulIndex*4 >= pxEfuseHandle->ulSize) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* enter critical section */
  xSemaphoreTake(pxEfuseHandle->xMutex, portMAX_DELAY);

  /* convert ulIndex and ulValue to something the driver can understand */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    ulAddr = ulIndex * 4;
    pucBuf = (uint8_t *) &ulValue;
  }

  /* write byte 0 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+0, pucBuf+0);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* write byte 1 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+1, pucBuf+1);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* write byte 2 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+2, pucBuf+2);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* write byte 3 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+3, pucBuf+3);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* exit critical section */
  xSemaphoreGive(pxEfuseHandle->xMutex);

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                       iot_efuse_read_16bit_word()
 ******************************************************************************/

/**
 * @brief Read 16-bit efuse word from specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to read. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[out] ulValue The receive buffer to read the data into
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if read succeeded,
 *   - IOT_EFUSE_READ_FAIL if read failed
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle or ulValue is NULL, or index is invalid
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 16-bit efuse word is not supported
 */
int32_t iot_efuse_read_16bit_word(IotEfuseHandle_t const pxEfuseHandle,
                                  uint32_t ulIndex,
                                  uint16_t * ulValue)
{
  /* local variables */
  uint32_t     ulAddr    = 0;
  uint8_t     *pucBuf    = NULL;
  sl_status_t  xSlStatus = SL_STATUS_OK;
  int32_t      lStatus   = IOT_EFUSE_SUCCESS;

  /* validate pxEfuseHandle */
  if (pxEfuseHandle == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate pxEfuseHandle open flag */
  if (pxEfuseHandle->ucIsOpen == pdFALSE) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulIndex */
  if (ulIndex*2 >= pxEfuseHandle->ulSize) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulValue */
  if (ulValue == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* enter critical section */
  xSemaphoreTake(pxEfuseHandle->xMutex, portMAX_DELAY);

  /* convert ulIndex and ulValue to something the driver can understand */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    ulAddr = ulIndex * 2;
    pucBuf = (uint8_t *) ulValue;
  }

  /* read byte 0 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+0, pucBuf+0);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* read byte 1 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_read)(ulAddr+1, pucBuf+1);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_READ_FAIL;
    }
  }

  /* exit critical section */
  xSemaphoreGive(pxEfuseHandle->xMutex);

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                        iot_efuse_write_16bit_word()
 ******************************************************************************/

/**
 * @brief Write 16-bit value to the 16-bit efuse word at specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to write to. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[in] ulValue The 16-bit value to write.
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if write succeeded,
 *   - IOT_EFUSE_WRITE_FAIL if write failed
 *   - IOT_EFUSE_INVALID_VALUE if index is invalid, or pxEfuseHandle is NULL.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 16-bit efuse word is not supported
 */
int32_t iot_efuse_write_16bit_word(IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint16_t value)
{
  /* local variables */
  uint32_t     ulAddr    = 0;
  uint8_t     *pucBuf    = NULL;
  sl_status_t  xSlStatus = SL_STATUS_OK;
  int32_t      lStatus   = IOT_EFUSE_SUCCESS;

  /* validate pxEfuseHandle */
  if (pxEfuseHandle == NULL) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate pxEfuseHandle open flag */
  if (pxEfuseHandle->ucIsOpen == pdFALSE) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* validate ulIndex */
  if (ulIndex*2 >= pxEfuseHandle->ulSize) {
    return IOT_EFUSE_INVALID_VALUE;
  }

  /* enter critical section */
  xSemaphoreTake(pxEfuseHandle->xMutex, portMAX_DELAY);

  /* convert ulIndex and ulValue to something the driver can understand */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    ulAddr = ulIndex * 2;
    pucBuf = (uint8_t *) &value;
  }

  /* write byte 0 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+0, pucBuf+0);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* write byte 1 */
  if (lStatus == IOT_EFUSE_SUCCESS) {
    xSlStatus = IOT_EFUSE_DRV_API(byte_write)(ulAddr+1, pucBuf+1);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_EFUSE_WRITE_FAIL;
    }
  }

  /* exit critical section */
  xSemaphoreGive(pxEfuseHandle->xMutex);

  /* done */
  return lStatus;
}
