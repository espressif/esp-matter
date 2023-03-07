/***************************************************************************//**
 * @file    iot_i2c_hal.c
 * @brief   Silicon Labs implementation of Common I/O I2C API.
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

/* I2C driver layer */
#include "iot_i2c_cb.h"
#include "iot_i2c_desc.h"
#include "iot_i2c_drv.h"

/* I2C iot layer */
#include "iot_i2c.h"

/*******************************************************************************
 *                            HELPER FUNCTIONS
 ******************************************************************************/

/*********************** iot_i2c_update_status() **************************/

static int32_t iot_i2c_update_status(IotI2CHandle_t const pxI2CPeripheral)
{
  /* status as retrieved from the driver */
  sl_status_t xSlStatus;

  /* status as to be returned in IoT format */
  int32_t lStatus = IOT_I2C_SUCCESS;

  /* retrieve status of last operation from driver */
  xSlStatus = iot_i2c_drv_transfer_status(pxI2CPeripheral);

  /* interpret xSlStatus variable */
  switch (xSlStatus) {
    /* transfer complete */
    case SL_STATUS_OK:
      /* update op status */
      pxI2CPeripheral->xOpStatus = eI2CCompleted;
      /* update number of bytes */
      if (pxI2CPeripheral->ucDirection == 0) {
        pxI2CPeripheral->usRxCount = pxI2CPeripheral->usRxMax;
      } else {
        pxI2CPeripheral->usTxCount = pxI2CPeripheral->usTxMax;
      }
      /* transfer has terminated successfully */
      lStatus = IOT_I2C_SUCCESS;
      /* done */
      break;

    /* timeout */
    case SL_STATUS_TIMEOUT:
      /* update op status */
      pxI2CPeripheral->xOpStatus = eI2CMasterTimeout;
      /* determine return value */
      lStatus = IOT_I2C_BUS_TIMEOUT;
      /* done */
      break;

    /* NACK received */
    case SL_STATUS_NOT_READY:
      /* update op status */
      pxI2CPeripheral->xOpStatus = eI2CNackFromSlave;
      /* determine return value */
      lStatus = IOT_I2C_NACK;
      /* done */
      break;

    /* some other failure */
    default:
      /* update op status */
      pxI2CPeripheral->xOpStatus = eI2CDriverFailed;
      /* determine return value */
      if (pxI2CPeripheral->ucDirection == 0) {
        lStatus = IOT_I2C_READ_FAILED;
      } else {
        lStatus = IOT_I2C_WRITE_FAILED;
      }
      /* done */
      break;
  }

  /* return iot status */
  return lStatus;
}

/************************** iot_i2c_hal_cb() ******************************/

void iot_i2c_hal_cb(int32_t lI2CInstance)
{
  /* local variables */
  IotI2CHandle_t    pxI2CPeripheral = NULL;
  IotI2CCallback_t  xCallback       = NULL;
  void             *pvContext       = NULL;
  sl_status_t       xSlStatus       = SL_STATUS_OK;

  /* retrieve I2C descriptor */
  pxI2CPeripheral = iot_i2c_desc_get(lI2CInstance);

  /* get callback info */
  xCallback = pxI2CPeripheral->pvCallback;
  pvContext = pxI2CPeripheral->pvContext;

  /* execute next step in transfer */
  xSlStatus = iot_i2c_drv_transfer_step(pxI2CPeripheral);

  /* transfer complete? */
  if (xSlStatus != SL_STATUS_IN_PROGRESS) {
    /* finish up the transfer */
    iot_i2c_drv_transfer_finish(pxI2CPeripheral);

    /* disable IRQs */
    iot_i2c_drv_irq_disable(pxI2CPeripheral);

    /* update transfer status */
    iot_i2c_update_status(pxI2CPeripheral);

    /* set bus state to idle */
    pxI2CPeripheral->xBusStatus = eI2CBusIdle;

    /* call the callback function */
    if (xCallback != NULL) {
       xCallback(pxI2CPeripheral->xOpStatus, pvContext);
    }
  }
}

/*******************************************************************************
 *                             iot_i2c_open()
 ******************************************************************************/

/**
 * @brief Initiates and reserves an I2C instance as master.
 *
 * One instance can communicate with one or more slave devices.
 * Slave addresses need to be changed between actions to different slave devices.
 *
 * @warning Once opened, the same I2C instance must be closed before calling open again.
 *
 * @param[in] lI2CInstance The instance of I2C to initialize. This is between 0 and the number of I2C instances on board - 1.
 *
 * @return
 * - 'the handle to the I2C port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 */
IotI2CHandle_t iot_i2c_open(int32_t lI2CInstance)
{
  /* local variables */
  IotI2CHandle_t  xI2CHandle   = NULL;

  /* retrieve I2C descriptor using instance number */
  xI2CHandle = iot_i2c_desc_get(lI2CInstance);

  /* validate xI2CHandle */
  if (xI2CHandle == NULL) {
    return NULL;
  }

  /* selected I2C shouldn't be open */
  if (xI2CHandle->ucIsOpen == pdTRUE) {
    return NULL;
  }

  /* initialize I2C driver */
  iot_i2c_drv_driver_init(xI2CHandle);

  /* enable I2C h/w */
  iot_i2c_drv_hw_enable(xI2CHandle);

  /* enter critical section */
  portENTER_CRITICAL();

  /* callback info */
  xI2CHandle->pvCallback       = NULL;
  xI2CHandle->pvContext        = NULL;

  /* runtime parameters */
  xI2CHandle->usSlaveAddr      = 0xFFFF;

  /* I2C transfer progress */
  xI2CHandle->ucDirection      = 0;
  xI2CHandle->usTxMax          = 0;
  xI2CHandle->usRxMax          = 0;
  xI2CHandle->usTxCount        = 0;
  xI2CHandle->usRxCount        = 0;

  /* I2C status */
  xI2CHandle->xBusStatus       = eI2CBusIdle;
  xI2CHandle->xOpStatus        = eI2CCompleted;

  /* mark descriptor as open */
  xI2CHandle->ucIsOpen         = pdTRUE;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return xI2CHandle;
}

/*******************************************************************************
 *                         iot_i2c_set_callback()
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
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_i2c_set_callback(IotI2CHandle_t const pxI2CPeripheral,
                          IotI2CCallback_t xCallback,
                          void * pvUserContext)
{
  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return;
  }

  /* update callback info */
  pxI2CPeripheral->pvCallback = xCallback;
  pxI2CPeripheral->pvContext  = pvUserContext;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return;
}

/*******************************************************************************
 *                           iot_i2c_read_sync()
 ******************************************************************************/

/**
 * @brief Starts the I2C master read operation in synchronous mode.
 *
 * This function attempts to read certain number of bytes from slave device to a pre-allocated buffer, in synchronous way.
 * Partial read might happen, e.g. no more data is available.
 * And the number of bytes that have been actually read can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the address of register needs to be written before calling this function.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_READ_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 *
 * <b>Example</b>
 * @code{c}
 *  // Declare an I2C handle.
 *  IotI2CHandle_t xI2CHandle;
 *
 *  // Return value of I2C functions.
 *  int32_t lRetVal = IOT_I2C_SUCCESS;
 *
 *  // Register address on I2C slave device.
 *  uint8_t xDeviceRegisterAddress = 0x73;
 *
 *  // Number of read/write bytes.
 *  uint16_t usReadBytes = 0;
 *  uint16_t usWriteBytes = 0;
 *
 *  uint8_t ucReadBuffer[2] = {0};
 *
 *  // Configurations of I2C master device.
 *  IotI2CConfig_t xI2CConfig =
 *  {
 *      .ulBusFreq       = IOT_I2C_FAST_MODE_BPS,
 *      .ulMasterTimeout = 500
 *  };
 *
 *  // Open one of the I2C instance and get a handle.
 *  xI2CHandle = iot_i2c_open( 1 );
 *
 *  if ( xI2CHandle != NULL )
 *  {
 *      // Set I2C configuration.
 *      lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *
 *      // Set slave address.
 *      lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *
 *      // Write the register address as single byte, in a transaction.
 *      lRetVal = iot_i2c_write_sync( xI2CHandle, &xDeviceRegisterAddress, sizeof( xDeviceRegisterAddress ) );
 *
 *      if ( lRetVal == IOT_I2C_SUCCESS )
 *      {
 *          // Get the number of written bytes in last transaction.
 *          lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &usWriteBytes );
 *          // assert(lRetVal == IOT_I2C_SUCCESS);
 *          // assert(usWriteBytes == 1);
 *
 *          // Read two bytes of data to allocated buffer, in a transaction.
 *          lRetVal = iot_i2c_read_sync( xI2CHandle, &ucReadBuffer, sizeof( ucReadBuffer ) );
 *
 *          if ( lRetVal == IOT_I2C_SUCCESS )
 *          {
 *              // Get the number of read bytes in last transaction.
 *              lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &usReadBytes );
 *              // assert(lRetVal == IOT_I2C_SUCCESS);
 *              // assert(usReadBytes == 2);
 *          }
 *      }
 *
 *      lRetVal = iot_i2c_close( xI2CHandle );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *  }
 * @endcode
 */
int32_t iot_i2c_read_sync(IotI2CHandle_t const pxI2CPeripheral,
                          uint8_t * const pucBuffer,
                          size_t xBytes)
{
  /* local variables */
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_I2C_SUCCESS;

  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* pucBuffer can't be null */
  if (pucBuffer == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation is already running? */
  if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
    portEXIT_CRITICAL();
    return IOT_I2C_BUSY;
  }

  /* Slave address has been setup? */
  if (pxI2CPeripheral->usSlaveAddr == 0xFFFF) {
    portEXIT_CRITICAL();
    return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
  }

  /* store buffer info */
  pxI2CPeripheral->ucDirection = 0;
  pxI2CPeripheral->usRxMax     = xBytes;
  pxI2CPeripheral->usRxCount   = 0;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* start transfer */
  iot_i2c_drv_transfer_start(pxI2CPeripheral,
                                  pxI2CPeripheral->usSlaveAddr,
                                  0,
                                  pucBuffer,
                                  xBytes);

  /* keep polling by executing steps of the transfer until done */
  do {
    /* continue transfer */
    xSlStatus = iot_i2c_drv_transfer_step(pxI2CPeripheral);
  } while (xSlStatus == SL_STATUS_IN_PROGRESS);

  /* finish up the transfer */
  iot_i2c_drv_transfer_finish(pxI2CPeripheral);

  /* enter critical section */
  portENTER_CRITICAL();

  /* retrieve status of last transfer */
  lStatus = iot_i2c_update_status(pxI2CPeripheral);

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_i2c_write_sync()
 ******************************************************************************/

/**
 * @brief Starts the I2C master write operation in synchronous mode.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in synchronous way.
 * Partial write might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the first byte is treated as the register address and the following bytes are treated as data to be written.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success (all the requested bytes have been written)
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_WRITE_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 */
int32_t iot_i2c_write_sync(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pucBuffer,
                           size_t xBytes)
{
  /* local variables */
  sl_status_t  xSlStatus   = SL_STATUS_OK;
  int32_t      lStatus     = IOT_I2C_SUCCESS;

  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* pucBuffer can't be null */
  if (pucBuffer == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation is already running? */
  if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
    portEXIT_CRITICAL();
    return IOT_I2C_BUSY;
  }

  /* Slave address has been setup? */
  if (pxI2CPeripheral->usSlaveAddr == 0xFFFF) {
    portEXIT_CRITICAL();
    return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
  }

  /* store buffer info */
  pxI2CPeripheral->ucDirection = 1;
  pxI2CPeripheral->usTxMax     = xBytes;
  pxI2CPeripheral->usTxCount   = 0;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* start transfer */
  iot_i2c_drv_transfer_start(pxI2CPeripheral,
                                  pxI2CPeripheral->usSlaveAddr,
                                  1,
                                  pucBuffer,
                                  xBytes);

  /* keep polling by executing steps of the transfer until done */
  do {
    /* continue transfer */
    xSlStatus = iot_i2c_drv_transfer_step(pxI2CPeripheral);
  } while (xSlStatus == SL_STATUS_IN_PROGRESS);

  /* finish up the transfer */
  iot_i2c_drv_transfer_finish(pxI2CPeripheral);

  /* enter critical section */
  portENTER_CRITICAL();

  /* retrieve status of last transfer */
  lStatus = iot_i2c_update_status(pxI2CPeripheral);

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                           iot_i2c_read_async()
 ******************************************************************************/

/**
 * @brief Starts the I2C master read operation in asynchronous mode.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_i2c_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial read might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually read can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the address of register needs to be written before calling this function.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 * @note In order to get notification when the asynchronous call is completed, iot_i2c_set_callback must be called prior to this.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning pucBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_READ_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 */
int32_t iot_i2c_read_async(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pucBuffer,
                           size_t xBytes)
{
  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* pucBuffer can't be null */
  if (pucBuffer == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation is already running? */
  if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
    portEXIT_CRITICAL();
    return IOT_I2C_BUSY;
  }

  /* Slave address has been setup? */
  if (pxI2CPeripheral->usSlaveAddr == 0xFFFF) {
    portEXIT_CRITICAL();
    return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
  }

  /* store buffer info */
  pxI2CPeripheral->ucDirection = 0;
  pxI2CPeripheral->usRxMax     = xBytes;
  pxI2CPeripheral->usRxCount   = 0;

  /* mark bus as busy */
  pxI2CPeripheral->xBusStatus = eI2cBusBusy;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* enable IRQs */
  iot_i2c_drv_irq_enable(pxI2CPeripheral);

  /* start transfer */

  iot_i2c_drv_transfer_start(pxI2CPeripheral,
                                  pxI2CPeripheral->usSlaveAddr,
                                  0,
                                  pucBuffer,
                                  xBytes);

  /* done */
  return IOT_I2C_SUCCESS;
}

/*******************************************************************************
 *                          iot_i2c_write_async()
 ******************************************************************************/

/**
 * @brief Starts the I2C master write operation in asynchronous mode.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_i2c_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial write might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the first byte is treated as the register address and the following bytes are treated as data to be written.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 * @note In order to get notification when the asynchronous call is completed, iot_i2c_set_callback must be called prior to this.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_WRITE_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 */
int32_t iot_i2c_write_async(IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes)
{
  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* pucBuffer can't be null */
  if (pucBuffer == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* xBytes can't be 0 */
  if (xBytes == 0) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation is already running? */
  if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
    portEXIT_CRITICAL();
    return IOT_I2C_BUSY;
  }

  /* Slave address has been setup? */
  if (pxI2CPeripheral->usSlaveAddr == 0xFFFF) {
    portEXIT_CRITICAL();
    return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
  }

  /* store buffer info */
  pxI2CPeripheral->ucDirection = 1;
  pxI2CPeripheral->usTxMax     = xBytes;
  pxI2CPeripheral->usTxCount   = 0;

  /* mark bus as busy */
  pxI2CPeripheral->xBusStatus = eI2cBusBusy;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* enable IRQs */
  iot_i2c_drv_irq_enable(pxI2CPeripheral);

  /* start transfer */
  iot_i2c_drv_transfer_start(pxI2CPeripheral,
                                  pxI2CPeripheral->usSlaveAddr,
                                  1,
                                  pucBuffer,
                                  xBytes);

  /* done */
  return IOT_I2C_SUCCESS;
}

/*******************************************************************************
 *                              iot_i2c_ioctl()
 ******************************************************************************/

/**
 * @brief Configures the I2C master with user configuration.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] xI2CRequest Should be one of I2C_Ioctl_Request_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @note SetMasterConfig is expected only called once at beginning.
 * This request expects the buffer with size of IotI2CConfig_t.
 *
 * @note eI2CGetMasterConfig gets the current configuration for I2C master.
 * This request expects the buffer with size of IotI2CConfig_t.
 *
 * @note eI2CGetBusState gets the current bus state.
 * This request expects buffer with size of IotI2CBusStatus_t.
 *
 * @note eI2CSendNoStopFlag is called at every operation you want to not send stop condition.
 *
 * @note eI2CSetSlaveAddr sets either 7-bit address or 10-bit address, according to hardware's capability.
 * This request expects 2 bytes buffer (uint16_t)
 *
 * @note eI2CGetTxNoOfbytes returns the number of written bytes in last transaction.
 * This is supposed to be called in the caller task or application callback, right after last transaction completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last transaction only did write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last transaction only did read, this returns 0.
 * - If the last transaction did both write and read, this returns the number of written bytes.
 *
 * @note eI2CGetRxNoOfbytes returns the number of read bytes in last transaction.
 * This is supposed to be called in the caller task or application callback, right after last transaction completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last transaction only did read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last transaction only did write, this returns 0.
 * - If the last transaction did both write and read, this returns the number of read bytes.
 *
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_I2C_FUNCTION_NOT_SUPPORTED, if this board doesn't support this feature.
 *     - eI2CSetSlaveAddr: 10-bit address is not supported
 *     - eI2CSendNoStopFlag: explicitly not sending stop condition is not supported
 *     - eI2CBusReset: reset bus is not supported
 */
int32_t iot_i2c_ioctl(IotI2CHandle_t const pxI2CPeripheral,
                      IotI2CIoctlRequest_t xI2CRequest,
                      void * const pvBuffer)
{
  /* local variables */
  uint16_t          *pusSlaveAddr    = NULL;
  IotI2CConfig_t    *pxMasterConfig  = NULL;
  IotI2CBusStatus_t *pxBusStatus     = NULL;
  uint16_t          *pusNoOfBytes    = NULL;
  int32_t            lStatus         = IOT_I2C_SUCCESS;

  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* exit critical section */
  portEXIT_CRITICAL();

  /* process the request */
  switch(xI2CRequest) {
    /* SEND NO STOP FLAG */
    /* ----------------- */
    case eI2CSendNoStopFlag:
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* operation not supported */
        lStatus = IOT_I2C_FUNCTION_NOT_SUPPORTED;
      }
      /* done */
      break;

    /* SET SLAVE ADDRESS */
    /* ----------------- */
    case eI2CSetSlaveAddr:
      /* enter critical section */
      portENTER_CRITICAL();
      /* make sure no bus operation pending */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* bus is busy? */
        if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
          lStatus = IOT_I2C_BUSY;
        }
      }
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a short integer */
        pusSlaveAddr = (uint16_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* store slave address */
        pxI2CPeripheral->usSlaveAddr = *pusSlaveAddr;
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* done */
      break;

    /* SET MASTER CONFIG */
    /* ----------------- */
    case eI2CSetMasterConfig:
      /* enter critical section */
      portENTER_CRITICAL();
      /* make sure no bus operation pending */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* bus is busy? */
        if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
          lStatus = IOT_I2C_BUSY;
        }
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a master config */
        pxMasterConfig = (IotI2CConfig_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* store configuration */
        iot_i2c_drv_config_set
                          (pxI2CPeripheral,
                           (uint32_t) pxMasterConfig->ulMasterTimeout,
                           (uint32_t) pxMasterConfig->ulBusFreq);
        /* disable h/w */
        iot_i2c_drv_hw_disable(pxI2CPeripheral);
        /* enable h/w again */
        iot_i2c_drv_hw_enable(pxI2CPeripheral);
      }
      /* done */
      break;

    /* GET MASTER CONFIG */
    /* ----------------- */
    case eI2CGetMasterConfig:
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a master config */
        pxMasterConfig = (IotI2CConfig_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* restore configuration */
        iot_i2c_drv_config_get
                          (pxI2CPeripheral,
                           (uint32_t *) &pxMasterConfig->ulMasterTimeout,
                           (uint32_t *) &pxMasterConfig->ulBusFreq);
      }
      /* done */
      break;

    /* GET BUS STATE */
    /* ------------- */
    case eI2CGetBusState:
      /* enter critical section */
      portENTER_CRITICAL();
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a bus status enum */
        pxBusStatus = (IotI2CBusStatus_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* retrieve bus status */
        *pxBusStatus = pxI2CPeripheral->xBusStatus;
      }
      /* exit critical section */
      portEXIT_CRITICAL();

      /* done */
      break;

    /* BUS RESET */
    /* --------- */
    case eI2CBusReset:
      /* enter critical section */
      portENTER_CRITICAL();
      /* make sure no bus operation pending */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* bus is busy? */
        if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
          lStatus = IOT_I2C_BUSY;
        }
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* disable h/w */
        iot_i2c_drv_hw_disable(pxI2CPeripheral);
        /* enable h/w again */
        iot_i2c_drv_hw_enable(pxI2CPeripheral);
      }
      /* done */
      break;

    /* GET TX NUMBER OF BYTES */
    /* ---------------------- */
    case eI2CGetTxNoOfbytes:
      /* enter critical section */
      portENTER_CRITICAL();
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a short integer */
        pusNoOfBytes = (uint16_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* retrieve number of bytes */
        *pusNoOfBytes = pxI2CPeripheral->usTxCount;
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* done */
      break;

    /* GET RX NUMBER OF BYTES */
    /* ---------------------- */
    case eI2CGetRxNoOfbytes:
      /* enter critical section */
      portENTER_CRITICAL();
      /* validate buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer can't be null */
        if (pvBuffer == NULL) {
          lStatus = IOT_I2C_INVALID_VALUE;
        }
      }
      /* load buffer pointer */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* buffer is a short integer */
        pusNoOfBytes = (uint16_t *) pvBuffer;
      }
      /* perform operation */
      if (lStatus == IOT_I2C_SUCCESS) {
        /* retrieve number of bytes */
        *pusNoOfBytes = pxI2CPeripheral->usRxCount;
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* done */
      break;

    /* INVALID REQUEST */
    /* --------------- */
    default:
      /* set error */
      lStatus = IOT_I2C_INVALID_VALUE;
      /* done */
      break;
  }

  /* done */
  return lStatus;
}

/*******************************************************************************
 *                             iot_i2c_close()
 ******************************************************************************/

/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 */
int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral)
{
  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation is pending? */
  if (pxI2CPeripheral->xBusStatus == eI2cBusBusy) {
    /* disable interrupts */
    iot_i2c_drv_irq_disable(pxI2CPeripheral);

    /* cancel everything */
    iot_i2c_drv_transfer_finish(pxI2CPeripheral);

    /* update bus status to idle */
    pxI2CPeripheral->xBusStatus = eI2CBusIdle;
  }

  /* exit critical section */
  portEXIT_CRITICAL();

  /* disable h/w */
  iot_i2c_drv_hw_disable(pxI2CPeripheral);

  /* deinit driver */
  iot_i2c_drv_driver_deinit(pxI2CPeripheral);

  /* enter critical section */
  portENTER_CRITICAL();

  /* callback info */
  pxI2CPeripheral->pvCallback       = NULL;
  pxI2CPeripheral->pvContext        = NULL;

  /* runtime parameters */
  pxI2CPeripheral->usSlaveAddr      = 0xFFFF;

  /* I2C transfer progress */
  pxI2CPeripheral->ucDirection      = 0;
  pxI2CPeripheral->usTxMax          = 0;
  pxI2CPeripheral->usRxMax          = 0;
  pxI2CPeripheral->usTxCount        = 0;
  pxI2CPeripheral->usRxCount        = 0;

  /* I2C status */
  pxI2CPeripheral->xBusStatus       = eI2CBusIdle;
  pxI2CPeripheral->xOpStatus        = eI2CCompleted;

  /* mark as closed */
  pxI2CPeripheral->ucIsOpen         = pdFALSE;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return IOT_I2C_SUCCESS;
}

/*******************************************************************************
 *                             iot_i2c_cancel()
 ******************************************************************************/

/**
 * @brief This function is used to cancel the current operation in progress, if possible.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 * - IOT_I2C_NOTHING_TO_CANCEL, if there is no on-going transaction.
 * - IOT_I2C_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral)
{
  /* pxI2CPeripheral can't be null */
  if (pxI2CPeripheral == NULL) {
    return IOT_I2C_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxI2CPeripheral must be already open */
  if (pxI2CPeripheral->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_I2C_INVALID_VALUE;
  }

  /* I2C operation must be already running */
  if (pxI2CPeripheral->xBusStatus != eI2cBusBusy) {
    portEXIT_CRITICAL();
    return IOT_I2C_NOTHING_TO_CANCEL;
  }

  /* disable interrupts */
  iot_i2c_drv_irq_disable(pxI2CPeripheral);

  /* cancel everything */
  iot_i2c_drv_transfer_finish(pxI2CPeripheral);

  /* update bus status to idle */
  pxI2CPeripheral->xBusStatus = eI2CBusIdle;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return IOT_I2C_SUCCESS;
}
