/***************************************************************************//**
 * @file    iot_spi_hal.c
 * @brief   Silicon Labs implementation of Common I/O SPI API.
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

/* SPI driver layer */
#include "iot_spi_cb.h"
#include "iot_spi_desc.h"
#include "iot_spi_drv.h"

/* SPI HAL layer */
#include "iot_spi.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

#define SL_SPI_OP_READ         (0x1)
#define SL_SPI_OP_WRITE        (0x2)
#define SL_SPI_OP_TRANSFER     (0x3)

/*******************************************************************************
 *                           HELPER FUNCTIONS
 ******************************************************************************/

/************************* iot_spi_hal_cb_rx() ****************************/

void iot_spi_hal_cb_rx(int32_t lInstNum, sl_status_t xSlStatus)
{
  IotSPIHandle_t             pxSPIPeripheral = NULL;
  IotSPICallback_t           xCallback       = NULL;
  void                      *pvContext       = NULL;
  IotSPITransactionStatus_t  xStatus         = eSPISuccess;

  /* retrive descriptor */
  pxSPIPeripheral = iot_spi_desc_get(lInstNum);

  /* retrieve pvCallback information */
  xCallback = pxSPIPeripheral->pvCallback;
  pvContext = pxSPIPeripheral->pvContext;

  /* process xSlStatus */
  xStatus = xSlStatus == SL_STATUS_OK ? eSPISuccess : eSPIReadError;

  /* execute application pvCallback */
  if (xCallback) {
    xCallback(xStatus, pvContext);
  }
}

/************************* iot_spi_hal_cb_tx() ****************************/

void iot_spi_hal_cb_tx(int32_t lInstNum, sl_status_t xSlStatus)
{
  IotSPIHandle_t             pxSPIPeripheral = NULL;
  IotSPICallback_t           xCallback       = NULL;
  void                      *pvContext       = NULL;
  IotSPITransactionStatus_t  xStatus         = eSPISuccess;

  /* retrive descriptor */
  pxSPIPeripheral = iot_spi_desc_get(lInstNum);

  /* retrieve pvCallback information */
  xCallback = pxSPIPeripheral->pvCallback;
  pvContext = pxSPIPeripheral->pvContext;

  /* process xSlStatus */
  xStatus = xSlStatus == SL_STATUS_OK ? eSPISuccess : eSPIWriteError;

  /* execute application pvCallback */
  if (xCallback != NULL) {
    xCallback(xStatus, pvContext);
  }
}

/************************* iot_spi_hal_cb_xx() ****************************/

void iot_spi_hal_cb_xx(int32_t lInstNum, sl_status_t xSlStatus)
{
  IotSPIHandle_t             pxSPIPeripheral = NULL;
  IotSPICallback_t           xCallback       = NULL;
  void                      *pvContext       = NULL;
  IotSPITransactionStatus_t  xStatus         = eSPISuccess;

  /* retrive descriptor */
  pxSPIPeripheral = iot_spi_desc_get(lInstNum);

  /* retrieve pvCallback information */
  xCallback = pxSPIPeripheral->pvCallback;
  pvContext = pxSPIPeripheral->pvContext;

  /* process xSlStatus */
  xStatus = xSlStatus == SL_STATUS_OK ? eSPISuccess : eSPITransferError;

  /* execute application pvCallback */
  if (xCallback != NULL) {
    xCallback(xStatus, pvContext);
  }
}

/*******************************************************************************
 *                              iot_spi_open()
 ******************************************************************************/

/**
 * @brief Initializes SPI peripheral with default configuration.
 *
 * @warning Once opened, the same SPI instance must be closed before calling open again.
 *
 * @param[in] lSPIInstance The instance of the SPI driver to initialize.
 *
 * @return
 * - 'the handle to the SPI port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 */
IotSPIHandle_t iot_spi_open(int32_t lSPIInstance)
{
  IotSPIDescriptor_t *pxSPIPeripheral  = NULL;

  pxSPIPeripheral = iot_spi_desc_get(lSPIInstance);

  if (pxSPIPeripheral == NULL) {
    return NULL;
  }

  if (pxSPIPeripheral->ucIsOpen) {
    return NULL;
  }

  portENTER_CRITICAL();

  iot_spi_drv_driver_init(pxSPIPeripheral);
  iot_spi_drv_hw_enable(pxSPIPeripheral);

  pxSPIPeripheral->ucIsOpen = pdTRUE;

  portEXIT_CRITICAL();

  return pxSPIPeripheral;
}

/*******************************************************************************
 *                           iot_spi_set_callback()
 ******************************************************************************/

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * The callback is guaranteed to be invoked when the current asynchronous operation completes, either successful or failed.
 * This simply provides a notification mechanism to user's application. It has no impact if the callback is not set.
 *
 * @note This callback will not be invoked when synchronous operation completes.
 * @note This callback is per handle. Each instance has its own callback.
 * @note Single callback is used for both read_async and write_async. Newly set callback overrides the one previously set.
 * @warning If the input handle is invalid, this function silently takes no action.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of operation.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_spi_set_callback(IotSPIHandle_t const pxSPIPeripheral,
                          IotSPICallback_t xCallback,
                          void * pvUserContext)
{
  if (pxSPIPeripheral == NULL) {
    return;
  }

  portENTER_CRITICAL();

  if (!pxSPIPeripheral->ucIsOpen) {
    portEXIT_CRITICAL();
    return;
  }

  pxSPIPeripheral->pvCallback = xCallback;
  pxSPIPeripheral->pvContext = pvUserContext;

  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                             iot_spi_ioctl()
 ******************************************************************************/

/**
 * @brief Configures the SPI port with user configuration.
 *
 *
 * @note eSPISetMasterConfig sets the configurations for master.
 * This request expects the buffer with size of IotSPIMasterConfig_t.
 *
 * @note eSPIGetMasterConfig gets the current configuration for SPI master.
 * This request expects the buffer with size of IotSPIMasterConfig_t.
 *
 * @note eSPIGetTxNoOfbytes returns the number of written bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation only did write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last operation only did read, this returns 0.
 * - If the last operation did both write and read, this returns the number of written bytes.
 *
 * @note eSPIGetRxNoOfbytes returns the number of read bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation only did read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last operation only did write, this returns 0.
 * - If the last operation did both write and read, this returns the number of read bytes.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] xSPIRequest The configuration request from one of the
 * IotSPIIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the SPI port.
 *
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_SPI_BUS_BUSY, if the bus is busy for only following requests:
 *     - eSPISetMasterConfig
 */
int32_t iot_spi_ioctl(IotSPIHandle_t const pxSPIPeripheral,
                      IotSPIIoctlRequest_t xSPIRequest,
                      void * const pvBuffer)
{
  sl_status_t            xSlStatus   = SL_STATUS_OK;
  uint32_t               ulCount     = 0;
  uint16_t              *pusBytes    = pvBuffer;
  IotSPIMasterConfig_t  *pxConfig    = pvBuffer;
  int32_t                lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (pvBuffer == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  switch (xSPIRequest) {
    case eSPISetMasterConfig:
      if (lStatus == IOT_SPI_SUCCESS) {
        xSlStatus = iot_spi_drv_config_set
                                      (pxSPIPeripheral,
                                       (uint32_t) pxConfig->ulFreq,
                                       (uint8_t) pxConfig->eMode,
                                       (uint8_t) pxConfig->eSetBitOrder,
                                       (uint8_t) pxConfig->ucDummyValue);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_INVALID_VALUE;
        }
      }
      if (lStatus == IOT_SPI_SUCCESS) {
        xSlStatus = iot_spi_drv_hw_disable(pxSPIPeripheral);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_INVALID_VALUE;
        }
      }
      if (lStatus == IOT_SPI_SUCCESS) {
        xSlStatus = iot_spi_drv_hw_enable(pxSPIPeripheral);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_INVALID_VALUE;
        }
      }
      break;
    case eSPIGetMasterConfig:
      if (lStatus == IOT_SPI_SUCCESS) {
        xSlStatus = iot_spi_drv_config_get
                                      (pxSPIPeripheral,
                                       (uint32_t *) &pxConfig->ulFreq,
                                       (uint8_t *) &pxConfig->eMode,
                                       (uint8_t *) &pxConfig->eSetBitOrder,
                                       (uint8_t *) &pxConfig->ucDummyValue);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_INVALID_VALUE;
        }
      }
      break;
    case eSPIGetTxNoOfbytes:
      if (lStatus == IOT_SPI_SUCCESS) {
        if (pxSPIPeripheral->ucLastOp & SL_SPI_OP_WRITE) {
          xSlStatus = iot_spi_drv_status_tx(pxSPIPeripheral, &ulCount);
          if (xSlStatus == SL_STATUS_OK) {
            *pusBytes = ulCount;
          } else {
            lStatus = IOT_SPI_INVALID_VALUE;
          }
        } else {
          *pusBytes = 0u;
        }
      }
      break;
    case eSPIGetRxNoOfbytes:
      if (lStatus == IOT_SPI_SUCCESS) {
        if (pxSPIPeripheral->ucLastOp & SL_SPI_OP_READ) {
          xSlStatus = iot_spi_drv_status_rx(pxSPIPeripheral, &ulCount);
          if (xSlStatus == SL_STATUS_OK) {
            *pusBytes = ulCount;
          } else {
            lStatus = IOT_SPI_INVALID_VALUE;
          }
        } else {
          *pusBytes = 0u;
        }
      }
      break;
    default:
      lStatus = IOT_SPI_INVALID_VALUE;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                            iot_spi_read_sync()
 ******************************************************************************/

/**
 * @brief The SPI master starts reading from the slave synchronously.
 *
 * This function attempts to read certain number of bytes from slave device to a pre-allocated buffer, in synchronous way.
 * This function does not return on paritial read, unless there is an error.
 * And the number of bytes that have been actually read can be obtained by calling iot_spi_ioctl.
 *
 * @note Dummy data will be written to slave while reading. The dummy data value can be configured with iot_spi_ioctl.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_READ_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUS_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_read_sync(IotSPIHandle_t const pxSPIPeripheral,
                          uint8_t * const pvBuffer,
                          size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_read_sync(pxSPIPeripheral, pvBuffer, xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_READ_FAILED;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_READ;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                           iot_spi_read_async()
 ******************************************************************************/

/**
 * @brief The SPI master starts reading from the slave asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial read, unless there is an error.
 * And the number of bytes that have been actually read can be obtained by calling iot_spi_ioctl.
 *
 * @note Dummy data will be written to slave while reading. The dummy data value can be configured with iot_spi_ioctl.
 * @note In order to get notification when the asynchronous call is completed, iot_spi_set_callback must be called prior to this.
 *
 * @warning pvBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_READ_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_read_async(IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_read_async(pxSPIPeripheral, pvBuffer, xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_READ_FAILED;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_READ;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                            iot_spi_write_sync()
 ******************************************************************************/

/**
 * @brief The SPI master starts transmission of data to the slave synchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in synchronous way.
 * This function does not return on paritial write, unless there is an error.
 * And the number of bytes that have been actually written can be obtained by calling iot_spi_ioctl.
 *
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_WRITE_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUS_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_write_sync(IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_write_sync(pxSPIPeripheral, pvBuffer, xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_WRITE_FAILED;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_WRITE;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                           iot_spi_write_async()
 ******************************************************************************/

/**
 * @brief The SPI master starts transmission of data to the slave asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial write, unless there is an error.
 * And the number of bytes that have been actually written can be obtained by calling iot_spi_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_spi_set_callback must be called prior to this.
 *
 * @warning pvBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_WRITE_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_write_async(IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_write_async(pxSPIPeripheral, pvBuffer, xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_WRITE_FAILED;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_WRITE;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                         iot_spi_transfer_sync()
 ******************************************************************************/

/**
 * @brief The SPI master starts a synchronous transfer between master and the slave.
 *
 * This function attempts to read/write certain number of bytes from/to two pre-allocated buffers at the same time, in synchronous way.
 * This function does not return on paritial read/write, unless there is an error.
 * And the number of bytes that have been actually read or written can be obtained by calling iot_spi_ioctl.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read/written)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_TRANSFER_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUS_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_transfer_sync(IotSPIHandle_t const pxSPIPeripheral,
                              uint8_t * const pvTxBuffer,
                              uint8_t * const pvRxBuffer,
                              size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvTxBuffer == NULL) || (pvRxBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_transfer_sync
                                  (pxSPIPeripheral,
                                   pvTxBuffer,
                                   pvRxBuffer,
                                   xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_TRANSFER_ERROR;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_TRANSFER;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                          iot_spi_transfer_async()
 ******************************************************************************/

/**
 * @brief The SPI master starts a asynchronous transfer between master and the slave.
 *
 * This function attempts to read/write certain number of bytes from/to two pre-allocated buffers at the same time, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial read/write, unless there is an error.
 * And the number of bytes that have been actually read/write can be obtained by calling iot_spi_ioctl.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read/written)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_TRANSFER_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing opeartion.
 */
int32_t iot_spi_transfer_async(IotSPIHandle_t const pxSPIPeripheral,
                               uint8_t * const pvTxBuffer,
                               uint8_t * const pvRxBuffer,
                               size_t xBytes)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if ((pvTxBuffer == NULL) || (pvRxBuffer == NULL) || (xBytes == 0)) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    xSlStatus = iot_spi_drv_transfer_async
                                  (pxSPIPeripheral,
                                   pvTxBuffer,
                                   pvRxBuffer,
                                   xBytes);
    if (xSlStatus != SL_STATUS_OK) {
      lStatus = IOT_SPI_TRANSFER_ERROR;
    }
  }

  if (lStatus == IOT_SPI_SUCCESS) {
    pxSPIPeripheral->ucLastOp = SL_SPI_OP_TRANSFER;
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                            iot_spi_close()
 ******************************************************************************/

/**
 * @brief Stops the ongoing operation on SPI bus and de-initializes the SPI peripheral.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 */
int32_t iot_spi_close(IotSPIHandle_t const pxSPIPeripheral)
{
  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  iot_spi_drv_hw_disable(pxSPIPeripheral);
  iot_spi_drv_driver_deinit(pxSPIPeripheral);

  pxSPIPeripheral->pvCallback = NULL;
  pxSPIPeripheral->pvContext = NULL;

  pxSPIPeripheral->ucIsOpen = pdFALSE;

  portEXIT_CRITICAL();

  return IOT_SPI_SUCCESS;
}

/*******************************************************************************
 *                             iot_spi_cancel()
 ******************************************************************************/

/**
 * @brief This function is used to cancel the current operation in progress, if the underlying driver
 * allows the cancellation.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 * - IOT_SPI_NOTHING_TO_CANCEL, if there is no on-going operation.
 * - IOT_SPI_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
int32_t iot_spi_cancel(IotSPIHandle_t const pxSPIPeripheral)
{
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_SPI_SUCCESS;

  if (pxSPIPeripheral == NULL) {
    return IOT_SPI_INVALID_VALUE;
  }

  if (!pxSPIPeripheral->ucIsOpen) {
    return IOT_SPI_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if (lStatus == IOT_SPI_SUCCESS) {
    switch(pxSPIPeripheral->ucLastOp) {
      case SL_SPI_OP_READ:
        xSlStatus = iot_spi_drv_read_abort(pxSPIPeripheral);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_NOTHING_TO_CANCEL;
        }
        break;
      case SL_SPI_OP_WRITE:
        xSlStatus = iot_spi_drv_write_abort(pxSPIPeripheral);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_NOTHING_TO_CANCEL;
        }
        break;
      case SL_SPI_OP_TRANSFER:
        xSlStatus = iot_spi_drv_transfer_abort(pxSPIPeripheral);
        if (xSlStatus != SL_STATUS_OK) {
          lStatus = IOT_SPI_NOTHING_TO_CANCEL;
        }
        break;
      default:
        lStatus = IOT_SPI_NOTHING_TO_CANCEL;
        break;
    }
  }

  portEXIT_CRITICAL();

  return lStatus;
}

/*******************************************************************************
 *                          iot_spi_select_slave()
 ******************************************************************************/

/**
 * @brief This function is used to select spi slave.
 *
 * @param[in] lSPIInstance The instance of the SPI driver to initialize.
 * @param[in] lSPISlave Slave select number.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - lSPISlave is invalid
 */
int32_t iot_spi_select_slave(int32_t lSPIInstance,
                             int32_t lSPISlave)
{
  (void)lSPIInstance;
  (void)lSPISlave;

  return IOT_SPI_FUNCTION_NOT_SUPPORTED;
}
