/***************************************************************************//**
 * @file    iot_uart_hal.c
 * @brief   Silicon Labs implementation of Common I/O UART API.
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

/* UART driver layer */
#include "iot_uart_desc.h"
#include "iot_uart_cb.h"
#include "iot_uart_drv.h"

/* UART HAL layer */
#include "iot_uart.h"

/*******************************************************************************
 *                            HELPER FUNCTIONS
 ******************************************************************************/

/************************ iot_uart_hal_cb_tx() ****************************/

void iot_uart_hal_cb_tx(int32_t lUartInstance)
{
  /* local variables */
  IotUARTHandle_t    pxUartPeripheral  = NULL;
  IotUARTCallback_t  pvCallback        = NULL;
  void              *pvContext         = NULL;
  uint8_t           *pvTxBuf           = NULL;
  uint32_t           ulTxMax           = 0;
  uint32_t           ulTxCtr           = 0;
  uint8_t            ucSample          = 0;

  /* retrieve pxUartPeripheral by instance number */
  pxUartPeripheral = iot_uart_desc_get(lUartInstance);

  /* retrieve callback pointers */
  pvCallback = pxUartPeripheral->pvCallback;
  pvContext  = pxUartPeripheral->pvContext;

  /* retrieve buffer info */
  pvTxBuf = pxUartPeripheral->pvTxBuf;
  ulTxMax = pxUartPeripheral->ulTxMax;
  ulTxCtr = pxUartPeripheral->ulTxCtr;

  /* read sample from buffer */
  ucSample = pvTxBuf[ulTxCtr];

  /* put sample to UART */
  iot_uart_drv_transfer_tx(pxUartPeripheral, &ucSample);

  /* increase buffer counter */
  pxUartPeripheral->ulTxCtr = ++ulTxCtr;

  /* operation complete? */
  if (ulTxCtr == ulTxMax) {
    /* execute callback and pass the buffer */
    if (pvCallback != NULL) {
      pvCallback(eUartWriteCompleted, pvContext);
    }

    /* stop TX transfer */
    iot_uart_drv_stop_tx(pxUartPeripheral);

    /* update busy status */
    pxUartPeripheral->ucTxIsBusy = pdFALSE;
  }
}

/************************ iot_uart_hal_cb_rx() ****************************/

void iot_uart_hal_cb_rx(int32_t lUartInstance)
{
  /* local variables */
  IotUARTHandle_t    pxUartPeripheral  = NULL;
  IotUARTCallback_t  pvCallback        = NULL;
  void              *pvContext         = NULL;
  uint8_t           *pvRxBuf           = NULL;
  uint32_t           ulRxMax           = 0;
  uint32_t           ulRxCtr           = 0;
  uint8_t            ucSample          = 0;

  /* retrieve pxUartPeripheral by instance number */
  pxUartPeripheral = iot_uart_desc_get(lUartInstance);

  /* retrieve callback pointers */
  pvCallback = pxUartPeripheral->pvCallback;
  pvContext  = pxUartPeripheral->pvContext;

  /* retrieve buffer info */
  pvRxBuf = pxUartPeripheral->pvRxBuf;
  ulRxMax = pxUartPeripheral->ulRxMax;
  ulRxCtr = pxUartPeripheral->ulRxCtr;

  /* read sample from UART */
  iot_uart_drv_transfer_rx(pxUartPeripheral, &ucSample);

  /* put sample in buffer */
  pvRxBuf[ulRxCtr] = ucSample;

  /* increase buffer counter */
  pxUartPeripheral->ulRxCtr = ++ulRxCtr;

  /* operation complete? */
  if (ulRxCtr == ulRxMax) {
    /* execute callback and pass the buffer */
    if (pvCallback != NULL) {
      pvCallback(eUartReadCompleted, pvContext);
    }

    /* stop RX transfer */
    iot_uart_drv_stop_rx(pxUartPeripheral);

    /* update busy status */
    pxUartPeripheral->ucRxIsBusy = pdFALSE;
  }
}

/*******************************************************************************
 *                             iot_uart_open()
 ******************************************************************************/

/**
 * @brief Initializes the UART peripheral of the board.
 *
 * The application should call this function to initialize the desired UART port.
 *
 * @warning Once opened, the same UART instance must be closed before calling open again.
 *
 * @param[in] lUartInstance The instance of the UART port to initialize.
 *
 * @return
 * - 'the handle to the UART port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 *
 * <b>Example</b>
 * @code{c}
 *  // These two buffers can contain 32 bytes for reading and writing.
 *  uint8_t cpBuffer[ 32 ] = { 0 };
 *  uint8_t cpBufferRead[ 32 ] = { 0 };
 *
 *  IotUARTHandle_t xOpen;
 *  int32_t lRead, lWrite, lClose;
 *  BaseType_t xCallbackReturn;
 *  uint8_t ucPort = 1; // Each UART peripheral will be assigned an integer.
 *
 *  xOpen = iot_uart_open( ucPort );
 *  if( xOpen != NULL )
 *  {
 *      iot_uart_set_callback( xOpen, prvReadWriteCallback, NULL );
 *
 *      lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_READ_BUFFER_LENGTH );
 *      // assert(IOT_UART_SUCCESS, lWrite)
 *      // Wait for asynchronous write to complete
 *      xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xUartSemaphore, IotUART_DEFAULT_SEMPAHORE_DELAY );
 *      // assert(xCallbackReturn, pdTrue)
 *
 *      lRead = iot_uart_read_async( xOpen, cpBufferRead, testIotUART_READ_BUFFER_LENGTH );
 *      // assert(IOT_UART_SUCCESS, lRead)
 *
 *      // Wait for asynchronous read to complete
 *      xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xUartSemaphore, IotUART_DEFAULT_SEMPAHORE_DELAY );
 *      // assert(xCallbackReturn, pdTrue)
 *  }
 *
 *  lClose = iot_uart_close( xOpen );
 *  // assert(IOT_UART_SUCCESS, lClose)
 *  @endcode
 */
IotUARTHandle_t iot_uart_open(int32_t lUartInstance)
{
  /* local variables */
  IotUARTHandle_t  xUartHandle  = NULL;

  /* retrieve xUartHandle by instance number */
  xUartHandle = iot_uart_desc_get(lUartInstance);

  /* xUartHandle instance is valid */
  if (xUartHandle == NULL) {
    return NULL;
  }

  /* selected uart shouldn't be open */
  if (xUartHandle->ucIsOpen == pdTRUE) {
    return NULL;
  }

  /* initialize UART driver */
  iot_uart_drv_driver_init(xUartHandle);

  /* enable UART h/w */
  iot_uart_drv_hw_enable(xUartHandle);

  portENTER_CRITICAL();

  /* callback info */
  xUartHandle->pvCallback    = NULL;
  xUartHandle->pvContext     = NULL;

  /* TX data */
  xUartHandle->ucTxIsBusy    = pdFALSE;
  xUartHandle->pvTxBuf       = NULL;
  xUartHandle->ulTxMax       = 0;
  xUartHandle->ulTxCtr       = 0;

  /* RX data */
  xUartHandle->ucRxIsBusy    = pdFALSE;
  xUartHandle->pvRxBuf       = NULL;
  xUartHandle->ulRxMax       = 0;
  xUartHandle->ulRxCtr       = 0;

  /* finally mark UART as open */
  xUartHandle->ucIsOpen      = pdTRUE;

  portEXIT_CRITICAL();

  /* done */
  return xUartHandle;
}

/*******************************************************************************
 *                           iot_uart_set_callback()
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
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction (This can be NULL).
 * @param[in] pvUserContext The user context to be passed back when callback is called (This can be NULL).
 */
void iot_uart_set_callback(IotUARTHandle_t const pxUartPeripheral,
                           IotUARTCallback_t xCallback,
                           void * pvUserContext)
{
  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return;
  }

  /* update callback info */
  pxUartPeripheral->pvCallback = xCallback;
  pxUartPeripheral->pvContext  = pvUserContext;

  portEXIT_CRITICAL();

  /* done */
  return;
}

/*******************************************************************************
 *                             iot_uart_read_sync()
 ******************************************************************************/

/**
 * @brief Starts receiving the data from UART synchronously.
 *
 * This function attempts to read certain number of bytes from transmitter device to a pre-allocated buffer, in synchronous way.
 * Partial read might happen, e.g. no more data is available.
 * And the number of bytes that have been actually read can be obtained by calling iot_uart_ioctl.
 *
 * @note If the number of bytes is not known, it is recommended that the application reads one byte at a time.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return IOT_UART_SUCCESS on successful completion of synchronous read,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY, IOT_UART_READ_FAILED  on error.
 * @return
 * - IOT_UART_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_READ_FAILED, if there is unknown driver error
 * - IOT_UART_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_uart_read_sync(IotUARTHandle_t const pxUartPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes)
{
  /* local variables */
  size_t   xCnt         = 0;

  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* RX operation is already running? */
  if (pxUartPeripheral->ucRxIsBusy == pdTRUE) {
    portEXIT_CRITICAL();
    return IOT_UART_BUSY;
  }

  portEXIT_CRITICAL();

  /* receive (and wait for) data from UART */
  for (xCnt = 0; xCnt < xBytes; xCnt++) {
    iot_uart_drv_transfer_rx(pxUartPeripheral, pvBuffer+xCnt);
  }

  /* done */
  return IOT_UART_SUCCESS;
}

/*******************************************************************************
 *                             iot_uart_write_sync()
 ******************************************************************************/

/**
 * @brief Starts the transmission of data from UART synchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a receiver device, in synchronous way.
 * Partial write might happen, e.g. receiver device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_uart_ioctl.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return
 * - IOT_UART_SUCCESS, on success (all the requested bytes have been write)
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_WRITE_FAILED, if there is unknown driver error
 * - IOT_UART_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_uart_write_sync(IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes)
{
  /* local variables */
  size_t   xCnt         = 0;

  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* TX operation is already running? */
  if (pxUartPeripheral->ucTxIsBusy == pdTRUE) {
    portEXIT_CRITICAL();
    return IOT_UART_BUSY;
  }

  portEXIT_CRITICAL();

  /* transmit (and wait for) data to UART */
  for (xCnt = 0; xCnt < xBytes; xCnt++) {
    iot_uart_drv_transfer_tx(pxUartPeripheral, pvBuffer+xCnt);
  }

  /* done */
  return IOT_UART_SUCCESS;
}

/*******************************************************************************
 *                             iot_uart_read_async()
 ******************************************************************************/

/**
 * @brief Starts receiving the data from UART asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_uart_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial read might happen.
 * And the number of bytes that have been actually read can be obtained by calling iot_uart_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_uart_set_callback must be called prior to this.
 * @warning pucBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_READ_FAILED, if there is unknown driver error
 */
int32_t iot_uart_read_async(IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes)
{
  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* RX operation is already running? */
  if (pxUartPeripheral->ucRxIsBusy == pdTRUE) {
    portEXIT_CRITICAL();
    return IOT_UART_BUSY;
  }

  /* update buffer information */
  pxUartPeripheral->pvRxBuf       = pvBuffer;
  pxUartPeripheral->ulRxMax       = xBytes;
  pxUartPeripheral->ulRxCtr       = 0;

  /* update busy status */
  pxUartPeripheral->ucRxIsBusy    = pdTRUE;

  portEXIT_CRITICAL();

  /* start async rx transfer */
  iot_uart_drv_start_rx(pxUartPeripheral);

  /* done */
  return IOT_UART_SUCCESS;
}

/*******************************************************************************
 *                            iot_uart_write_async()
 ******************************************************************************/

/**
 * @brief Starts the transmission of data from UART asynchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a receiver device, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_uart_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial write might happen.
 * And the number of bytes that have been actually written can be obtained by calling iot_uart_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_uart_set_callback must be called prior to this.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_WRITE_FAILED, if there is unknown driver error
 */
int32_t iot_uart_write_async(IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes)
{
  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* pvBuffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* TX operation is already running? */
  if (pxUartPeripheral->ucTxIsBusy == pdTRUE) {
    portEXIT_CRITICAL();
    return IOT_UART_BUSY;
  }

  /* update buffer info */
  pxUartPeripheral->pvTxBuf       = pvBuffer;
  pxUartPeripheral->ulTxMax       = xBytes;
  pxUartPeripheral->ulTxCtr       = 0;

  /* update busy status */
  pxUartPeripheral->ucTxIsBusy    = pdTRUE;

  portEXIT_CRITICAL();

  /* start async tx transfer */
  iot_uart_drv_start_tx(pxUartPeripheral);

  /* done */
  return IOT_UART_SUCCESS;
}

/*******************************************************************************
 *                              iot_uart_ioctl()
 ******************************************************************************/

/**
 * @brief Configures the UART port with user configuration.
 *
 *
 * @note eUartSetConfig sets the UART configuration.
 * This request expects the buffer with size of IotUARTConfig_t.
 *
 * @note eUartGetConfig gets the current UART configuration.
 * This request expects the buffer with size of IotUARTConfig_t.
 *
 * @note eGetTxNoOfbytes returns the number of written bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation was write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last operation was read, this returns 0.
 *
 * @note eGetRxNoOfbytes returns the number of read bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation was read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last operation was write, this returns 0.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xUartRequest The configuration request. Should be one of the values
 * from IotUARTIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the UART port.
 *
 * @return IOT_UART_SUCCESS on successful configuartion of UART port,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY,
 *         IOT_UART_FUNCTION_NOT_SUPPORTED on error.
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_UART_FUNCTION_NOT_SUPPORTED, if this board doesn't support this feature.
 *     - eUartSetConfig: specific configuration is not supported
 */
int32_t iot_uart_ioctl(IotUARTHandle_t const pxUartPeripheral,
                       IotUARTIoctlRequest_t xUartRequest,
                       void * const pvBuffer)
{
  /* local variables */
  IotUARTConfig_t  *pxUARTConfig = NULL;
  int32_t          *plNoOfBytes  = NULL;
  int32_t           lStatus      = IOT_UART_SUCCESS;

  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  /* buffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* process the request */
  switch(xUartRequest) {
    /* set UART config */
    case eUartSetConfig:
      /* ucTxIsBusy shouldn't be set */
      if (lStatus == IOT_UART_SUCCESS) {
        if (pxUartPeripheral->ucTxIsBusy == pdTRUE) {
          lStatus = IOT_UART_BUSY;
        }
      }
      /* ucRxIsBusy shouldn't be set */
      if (lStatus == IOT_UART_SUCCESS) {
        if (pxUartPeripheral->ucRxIsBusy == pdTRUE) {
          lStatus = IOT_UART_BUSY;
        }
      }
      /* pvBuffer is a config ptr */
      if (lStatus == IOT_UART_SUCCESS) {
        pxUARTConfig = (IotUARTConfig_t *) pvBuffer;
      }
      /* make sure baudrate is valid */
      if (lStatus == IOT_UART_SUCCESS) {
        if (pxUARTConfig->ulBaudrate == 0) {
          lStatus = IOT_UART_INVALID_VALUE;
        }
      }
      /* write configuration */
      if (lStatus == IOT_UART_SUCCESS) {
        iot_uart_drv_config_set
                           (pxUartPeripheral,
                            (uint32_t) pxUARTConfig->ulBaudrate,
                            (uint8_t)  pxUARTConfig->xParity,
                            (uint8_t)  pxUARTConfig->xStopbits,
                            (uint8_t)  pxUARTConfig->ucWordlength,
                            (uint8_t)  pxUARTConfig->ucFlowControl);
      }
      /* restart hardware */
      if (lStatus == IOT_UART_SUCCESS) {
        iot_uart_drv_hw_disable(pxUartPeripheral);
        iot_uart_drv_hw_enable(pxUartPeripheral);
      }
      /* done */
      break;

    /* get UART config */
    case eUartGetConfig:
      /* pvBuffer is a config ptr */
      if (lStatus == IOT_UART_SUCCESS) {
        pxUARTConfig = (IotUARTConfig_t *) pvBuffer;
      }
      /* read configuration */
      if (lStatus == IOT_UART_SUCCESS) {
        iot_uart_drv_config_get
                           (pxUartPeripheral,
                            (uint32_t *) &pxUARTConfig->ulBaudrate,
                            (uint8_t *)  &pxUARTConfig->xParity,
                            (uint8_t *)  &pxUARTConfig->xStopbits,
                            (uint8_t *)  &pxUARTConfig->ucWordlength,
                            (uint8_t *)  &pxUARTConfig->ucFlowControl);
      }
      /* done */
      break;

    /* get number of bytes written in last TX operation */
    case eGetTxNoOfbytes:
      /* pvBuffer is a long int ptr */
      if (lStatus == IOT_UART_SUCCESS) {
        plNoOfBytes = (int32_t *) pvBuffer;
      }
      /* read number of bytes */
      if (lStatus == IOT_UART_SUCCESS) {
        *plNoOfBytes = pxUartPeripheral->ulTxCtr;
      }
      /* done */
      break;

    /* get number of bytes read in last RX operation */
    case eGetRxNoOfbytes:
      /* pvBuffer is a long int ptr */
      if (lStatus == IOT_UART_SUCCESS) {
        plNoOfBytes = (int32_t *) pvBuffer;
      }
      /* read number of bytes */
      if (lStatus == IOT_UART_SUCCESS) {
        *plNoOfBytes = pxUartPeripheral->ulRxCtr;
      }
      /* done */
      break;

    /* invalid request */
    default:
      /* set error */
      if (lStatus == IOT_UART_SUCCESS) {
        lStatus = IOT_UART_INVALID_VALUE;
      }
      /* done */
      break;
  }

  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                             iot_uart_cancel()
 ******************************************************************************/

/**
 * @brief Aborts the operation on the UART port if any underlying driver allows
 * cancellation of the operation.
 *
 * The application should call this function to stop the ongoing operation.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 * - IOT_UART_NOTHING_TO_CANCEL, if there is no on-going transaction.
 * - IOT_UART_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
int32_t iot_uart_cancel(IotUARTHandle_t const pxUartPeripheral)
{
  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* No operation is already running? */
  if (pxUartPeripheral->ucTxIsBusy == pdFALSE &&
      pxUartPeripheral->ucRxIsBusy == pdFALSE) {
      portEXIT_CRITICAL();
      return IOT_UART_NOTHING_TO_CANCEL;
  }

  /* cancel any pending TX operation */
  if (pxUartPeripheral->ucTxIsBusy == pdTRUE) {
    iot_uart_drv_stop_tx(pxUartPeripheral);
    pxUartPeripheral->ucTxIsBusy = pdFALSE;
  }

  portEXIT_CRITICAL();

  portENTER_CRITICAL();

  /* cancel any pending RX operation */
  if (pxUartPeripheral->ucRxIsBusy == pdTRUE) {
    iot_uart_drv_stop_rx(pxUartPeripheral);
    pxUartPeripheral->ucRxIsBusy = pdFALSE;
  }

  portEXIT_CRITICAL();

  /* done */
  return IOT_UART_SUCCESS;
}

/*******************************************************************************
 *                              iot_uart_close()
 ******************************************************************************/

/**
 * @brief Stops the operation and de-initializes the UART peripheral.
 *
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 */
int32_t iot_uart_close(IotUARTHandle_t const pxUartPeripheral)
{
  /* pxUartPeripheral can't be null */
  if (pxUartPeripheral == NULL) {
    return IOT_UART_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* pxUartPeripheral must be already open */
  if (pxUartPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_UART_INVALID_VALUE;
  }

  /* cancel any pending TX operation */
  if (pxUartPeripheral->ucTxIsBusy == pdTRUE) {
    iot_uart_drv_stop_tx(pxUartPeripheral);
    pxUartPeripheral->ucTxIsBusy = pdFALSE;
  }

  /* cancel any pending RX operation */
  if (pxUartPeripheral->ucRxIsBusy == pdTRUE) {
    iot_uart_drv_stop_rx(pxUartPeripheral);
    pxUartPeripheral->ucRxIsBusy = pdFALSE;
  }

  portEXIT_CRITICAL();

  /* stop UART hardware */
  iot_uart_drv_hw_disable(pxUartPeripheral);

  /* deinitialize UART driver */
  iot_uart_drv_driver_deinit(pxUartPeripheral);

  portENTER_CRITICAL();

  /* mark as closed */
  pxUartPeripheral->ucIsOpen      = pdFALSE;

  /* uninitialize callback info */
  pxUartPeripheral->pvCallback    = NULL;
  pxUartPeripheral->pvContext     = NULL;

  /* uninitialize TX data */
  pxUartPeripheral->ucTxIsBusy    = pdFALSE;
  pxUartPeripheral->pvTxBuf       = NULL;
  pxUartPeripheral->ulTxMax       = 0;
  pxUartPeripheral->ulTxCtr       = 0;

  /* uninitialize RX data */
  pxUartPeripheral->ucRxIsBusy    = pdFALSE;
  pxUartPeripheral->pvRxBuf       = NULL;
  pxUartPeripheral->ulRxMax       = 0;
  pxUartPeripheral->ulRxCtr       = 0;

  portEXIT_CRITICAL();

  /* done */
  return IOT_UART_SUCCESS;
}
