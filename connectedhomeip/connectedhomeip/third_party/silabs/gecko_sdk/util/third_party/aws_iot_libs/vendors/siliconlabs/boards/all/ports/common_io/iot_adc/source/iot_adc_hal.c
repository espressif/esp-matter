/***************************************************************************//**
 * @file    iot_adc_hal.c
 * @brief   Silicon Labs implementation of Common I/O ADC API.
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

/* ADC driver layer */
#include "iot_adc_cb.h"
#include "iot_adc_desc.h"
#include "iot_adc_drv.h"

/* ADC iot layer */
#include "iot_adc.h"

/*******************************************************************************
 *                            HELPER FUNCTIONS
 ******************************************************************************/

void iot_adc_hal_callback(int32_t lAdcInstance)
{
  /* local variables */
  IotAdcHandle_t     pxAdc      = NULL;
  uint16_t           usSample   = 0;
  IotAdcCallback_t   xCallback  = NULL;
  void              *pvContext  = NULL;

  /* retrieve ADC descriptor */
  pxAdc = iot_adc_desc_get(lAdcInstance);

  /* retrieve callback info */
  xCallback = pxAdc->pvCallback;
  pvContext = pxAdc->pvContext;

  /* let ADC h/w return the last sample collected */
  iot_adc_drv_multi_read(pxAdc, &usSample);

  /* buffered I/O or not? */
  if (pxAdc->pusBufPtr == NULL) {
    /* execute callback function */
    if (xCallback != NULL) {
      xCallback(&usSample, pvContext);
    }

    /* start next conversion */
    iot_adc_drv_multi_start(pxAdc);
  } else {
    /* put sample in buffer */
    pxAdc->pusBufPtr[pxAdc->ucBufCtr] = usSample;

    /* buffer full? */
    if (pxAdc->ucBufCtr != pxAdc->ucBufMax-1) {
      /* prepare for next item */
      pxAdc->ucBufCtr++;

      /* start next conversion */
      iot_adc_drv_multi_start(pxAdc);
    } else {
      /* execute callback and pass the buffer */
      if (xCallback != NULL) {
        xCallback(pxAdc->pusBufPtr, pvContext);
      }

      /* disable scan interrupts */
      iot_adc_drv_irq_disable(pxAdc);

      /* deallocate analog bus */
      iot_adc_drv_bus_dealloc(pxAdc, pxAdc->ucBusyChannel);

      /* reset buffer counter */
      pxAdc->ucBufCtr = 0;

      /* update status */
      pxAdc->ucBusyStatus = pdFALSE;
    }
  }
}

/*******************************************************************************
 *                             iot_adc_open()
 ******************************************************************************/

/**
 * @brief Initializes ADC controller with default configuration.
 * init ADC controller, enable ADC clock, reset HW FIFO, set default
 * configuration parameters, etc. Also allocate all required resources
 * for ADC operation such as software data buffer etc.
 *
 * @warning Must be called prior to any other ADC api's so that a valid handle is obtained.
 * @warning Once opened, the same ADC instance must be closed before calling open again.
 *
 * @param[in] lAdc The instance of ADC controller to initialize.
 *
 * @return
 * - handle to the ADC controller on success
 * - NULL, if
 *    - invalid instance number
 *    - open same instance more than once before closing it.
 */
IotAdcHandle_t iot_adc_open(int32_t lAdc)
{
  /* local variables */
  IotAdcHandle_t    xAdcHandle   = NULL;

  /* retrieve xAdcHandle by instance number */
  xAdcHandle = iot_adc_desc_get(lAdc);

  /* xAdcHandle instance is valid? */
  if (xAdcHandle == NULL) {
    return NULL;
  }

  /* selected adc shouldn't be open */
  if (xAdcHandle->ucIsOpen == pdTRUE) {
    return NULL;
  }

  /* initialize ADC driver */
  iot_adc_drv_driver_init(xAdcHandle);

  /* enable ADC hardware */
  iot_adc_drv_hw_enable(xAdcHandle);

  /* enter critical section */
  portENTER_CRITICAL();

  /* reset ADC status */
  xAdcHandle->ucBusyStatus   = pdFALSE;
  xAdcHandle->ucBusyChannel  = 0;

  /* reset ADC callback config */
  xAdcHandle->pvCallback     = NULL;
  xAdcHandle->pvContext      = NULL;

  /* reset ADC buffer config */
  xAdcHandle->pusBufPtr      = NULL;
  xAdcHandle->ucBufMax       = 0;
  xAdcHandle->ucBufCtr       = 0;

  /* finally mark ADC as open */
  xAdcHandle->ucIsOpen = pdTRUE;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return xAdcHandle;
}

/*******************************************************************************
 *                             iot_adc_close()
 ******************************************************************************/

/**
 * @brief Close ADC controller.
 * All pending operation will be cancelled, put ADC module in reset state or low
 * power state if possible. Release all resources claimed during open call
 *
 * @param[in] pxAdc handle to ADC controller returned from iot_adc_open() call.
 *
 * @return
 * - IOT_ADC_SUCCESS, on success;
 * - IOT_ADC_INVALID_VALUE, if pxAdc is NULL
 * - IOT_ADC_NOT_OPEN, if is not in open state (already closed).
 */
int32_t iot_adc_close(IotAdcHandle_t const pxAdc)
{
  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_ADC_NOT_OPEN;
  }

  /* any pending operation? */
  if (pxAdc->ucBusyStatus == pdTRUE) {
    /* disable IRQs */
    iot_adc_drv_irq_disable(pxAdc);

    /* deallocate analog bus */
    iot_adc_drv_bus_dealloc(pxAdc, pxAdc->ucBusyChannel);

    /* update status */
    pxAdc->ucBusyStatus = pdFALSE;
  }

  /* exit critical section */
  portEXIT_CRITICAL();

  /* disable hardware */
  iot_adc_drv_hw_disable(pxAdc);

  /* deinit the driver */
  iot_adc_drv_driver_deinit(pxAdc);

  /* enter critical section again */
  portENTER_CRITICAL();

  /* reset ADC status */
  pxAdc->ucBusyStatus    = pdFALSE;
  pxAdc->ucBusyChannel   = 0;

  /* reset ADC callback config */
  pxAdc->pvCallback      = NULL;
  pxAdc->pvContext       = NULL;

  /* reset ADC buffer config */
  pxAdc->pusBufPtr       = NULL;
  pxAdc->ucBufMax        = 0;
  pxAdc->ucBufCtr        = 0;

  /* mark as closed */
  pxAdc->ucIsOpen        = pdFALSE;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return IOT_ADC_SUCCESS;
}

/*******************************************************************************
 *                           iot_adc_set_callback()
 ******************************************************************************/

/**
 * @brief Sets channel callback on availability of channel scan data.
 * On availability of ADC scan data, the application is notified with a
 * function callback. The callback function and user context for callback
 * are set using iot_adc_set_callback.
 *
 * @note This callback is per AdcChannel for each handle.
 * @note If input handle or AdcChannel is invalid, or if callback function is NULL,
 *       this function silently takes no action.
 *
 * @param[in] pxAdc The Adc handle returned in the open() call.
 * @param[in] ucAdcChannel The Adc channel for which the callback is set
 * @param[in] pvCallback The callback function to be called on availability of ADC channel data.
 * @param[in] pvUserContext The user context to be passed when callback is called.
 *
 * <b>Example Callback Function</b>
 * For asychronous ADC calls, a callback function is used to signal when the async
 * task is complete.  This example uses a Semaphore to signal the completion.
 * @code{c}
 * static void prvAdcChCallback( uint16_t * pusConvertedData,
 *                               void * pvUserContext )
 * {
 *     BaseType_t xHigherPriorityTaskWoken;
 *     xSemaphoreGiveFromISR( xIotAdcSemaphore, &xHigherPriorityTaskWoken );
 *     portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 * }
 * @endcode
 */
void iot_adc_set_callback(IotAdcHandle_t const pxAdc,
                          uint8_t ucAdcChannel,
                          IotAdcCallback_t pvCallback,
                          void * pvUserContext)
{
  /* we only support one callback function */
  (void) ucAdcChannel;

  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return;
  }

  /* pvCallback can't be null */
  if (pvCallback == NULL) {
    return;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return;
  }

  /* update callback info */
  pxAdc->pvCallback = pvCallback;
  pxAdc->pvContext = pvUserContext;

  /* exit critical section */
  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                              iot_adc_start()
 ******************************************************************************/

/**
 * @brief Start data acquisition for ADC channel until iot_adc_stop API is called.
 *        data will be passed back to client using callback function.
 *        by default each callback will pass back one data sample, however if client has
 *        used ioctl to pass in data buffer, only when buffer is full will callback
 *        be triggered
 *
 * @warning iot_adc_set_callback() must be called prior to this in order to get notification
 *          when adc scan is complete and data is available
 *
 * @note iot_adc_set_callback() must be called prior to iot_adc_start().
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to start data acquisition
 *
 * @return
 * - IOT_ADC_SUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel
 * - IOT_ADC_NOT_OPEN if ADC has not been opened yet.
 * - IOT_ADC_CH_BUSY if ADC operation has started but is not complete
 * - IOT_ADC_FAILED if not callback function has been set.
 * <b>Example Asynchronous read</b>
 * @code{c}
 * // ADC Instance to open
 * int32_t lAdcInstance = 0;
 *
 * // Return value of ADC functions
 * int32_t lRetVal;
 *
 * // ADC Channel to read
 * int32_t lAdcChannel = 0;
 *
 * // User/Driver context; if necessary
 * void xUserContext = NULL;
 *
 * // Open the ADC instance and get a handle.
 * xAdcHandle = iot_adc_open( lAdcInstance );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // set the callback function
 * iot_adc_set_callback( xAdcHandle, lAdcChannel, prvAdcChCallback, &xUserCntx );
 *
 * // start channel data scan on channel
 * lRetVal = iot_adc_start( xAdcHandle, lAdcChannel );
 *  // assert( IOT_ADC_SUCCESS == lRetVal );
 *
 *  // wait for the Adc operation to complete
 *  lRetVal = xSemaphoreTake( xIotAdcSemaphore, lIotAdcChWaitTime );
 *  // assert ( pdTRUE == lRetVal );
 *
 *  // stop channel data scan
 *  lRetVal = iot_adc_stop( xAdcHandle, lAdcChannel );
 *  assert( IOT_ADC_SUCCESS == lRetVal );
 *
 *  // close ADC module
 *  lRetVal = iot_adc_close( xAdcHandle );
 *  // assert( IOT_ADC_SUCCESS == lRetVal );
 * @endcode
 */
int32_t iot_adc_start(IotAdcHandle_t const pxAdc,
                      uint8_t ucAdcChannel)
{
  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* ucAdcChannel valid? */
  if (ucAdcChannel == 0) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_ADC_NOT_OPEN;
  }

  /* pxAdc is busy? */
  if (pxAdc->ucBusyStatus == pdTRUE) {
    portEXIT_CRITICAL();
    return IOT_ADC_CH_BUSY;
  }

  /* no callback function set? */
  if (pxAdc->pvCallback == NULL) {
    portEXIT_CRITICAL();
    return IOT_ADC_FAILED;
  }

  /* update status */
  pxAdc->ucBusyStatus = pdTRUE;

  /* store active channel */
  pxAdc->ucBusyChannel = ucAdcChannel;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* allocate analog bus */
  iot_adc_drv_bus_alloc(pxAdc, ucAdcChannel);

  /* setup scan mode */
  iot_adc_drv_multi_init(pxAdc, ucAdcChannel);

  /* enable irqs */
  iot_adc_drv_irq_enable(pxAdc);

  /* issue scan command */
  iot_adc_drv_multi_start(pxAdc);

  /* done */
  return IOT_ADC_SUCCESS;
}

/*******************************************************************************
 *                             iot_adc_stop()
 ******************************************************************************/

/**
 * @brief Stop data acquisition for ADC channel
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to stop data acquisition
 *
 * @return
 * - IOT_ADC_SCUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel
 * - IOT_ADC_NOT_OPEN if Adc has been closed without re-opening.
 */
int32_t iot_adc_stop(IotAdcHandle_t const pxAdc,
                     uint8_t ucAdcChannel)
{
  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* ucAdcChannel valid? */
  if (ucAdcChannel == 0) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    portEXIT_CRITICAL();
    return IOT_ADC_NOT_OPEN;
  }

  /* disable IRQs */
  iot_adc_drv_irq_disable(pxAdc);

  /* deallocate analog bus */
  iot_adc_drv_bus_dealloc(pxAdc, ucAdcChannel);

  /* update status */
  pxAdc->ucBusyStatus = pdFALSE;

  /* clear buffer info */
  pxAdc->pusBufPtr = NULL;
  pxAdc->ucBufMax = 0;
  pxAdc->ucBufCtr = 0;

  /* exit critical section */
  portEXIT_CRITICAL();

  /* done */
  return IOT_ADC_SUCCESS;
}

/*******************************************************************************
 *                           iot_adc_read_sample()
 ******************************************************************************/

/**
 * @brief read one ADC data sample. This API will return one ADC sample.
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to read data from
 * @param[out] pusAdcSample. ADC channel read sample value
 *
 * @return
 * - IOT_ADC_SCUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel
 * - IOT_ADC_CH_BUSY if ADC operation not complete.
 * - IOT_ADC_NOT_OPEN if Adc has been closed without re-opening.
 * <b>Example Synchronous read</b>
 * @code{c}
 * // ADC Instance to open
 * int32_t lAdcInstance = 0;
 *
 * // ADC Channel to read
 * int32_t lAdcChannel = 0;
 *
 * // Declare and ADC handle
 * IotAdcHandle_t xAdcHandle;
 *
 * // Return value of ADC functions
 * int32_t lRetVal;
 *
 * // ADC value read
 * uint16_t usSample;
 *
 * // Open the ADC instance and get a handle.
 * xAdcHandle = iot_adc_open( lAdcInstance );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // Read sample from ADC channel
 * lRetVal = iot_adc_read_sample( xAdcHandle, lAdcChannel, &usSample);
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // Close the ADC instance and get a handle.
 * lRetVal = iot_adc_close( xAdcHandle );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 * @endcode
 *
 */
int32_t iot_adc_read_sample(IotAdcHandle_t const pxAdc,
                            uint8_t ucAdcChannel,
                            uint16_t * pusAdcSample)
{
  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* ucAdcChannel valid? */
  if (ucAdcChannel == 0) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    return IOT_ADC_NOT_OPEN;
  }

  /* allocate analog bus */
  iot_adc_drv_bus_alloc(pxAdc, ucAdcChannel);

  /* setup single mode */
  iot_adc_drv_single_init(pxAdc, ucAdcChannel);

  /* start ADC conversion */
  iot_adc_drv_single_start(pxAdc);

  /* read one sample from ADC and store it */
  iot_adc_drv_single_read(pxAdc, pusAdcSample);

  /* deallocate analog bus */
  iot_adc_drv_bus_dealloc(pxAdc, ucAdcChannel);

  /* done */
  return IOT_ADC_SUCCESS;
}

/*******************************************************************************
 *                             iot_adc_ioctl()
 ******************************************************************************/

/**
 * @brief Used for various ADC control functions.
 *
 * @param[in] pxAdc The Adc handle returned in the open() call.
 * @param[in] xRequest ioctl request defined by IotAdcIoctlRequest_s enums
 * @param[in/out] pvBuffer data buffer for ioctl request
 * @param[in] pvBuffer size
 *
 * @return
 * - IOT_ADC_SCUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid request or NULL or invalid buffer pointer
 * - IOT_ADC_CH_BUSY if ADC operation not complete.
 * - IOT_ADC_NOT_OPEN if Adc has been closed without re-opening.
 * - IOT_ADC_FAILED if invalid ADC chain is requested.
 * - IOT_ADC_FUNCTION_NOT_SUPPORTED only valid for eSetAdcChain, if feature not supported
 */
int32_t iot_adc_ioctl(IotAdcHandle_t const pxAdc,
                      IotAdcIoctlRequest_t xRequest,
                      void * const pvBuffer)
{
  /* local variables */
  uint8_t            ucAdcIsBusy  = pdFALSE;
  uint8_t            ucAdcBusyCh  = 0;
  uint8_t            ucAdcChannel = 0;
  IotAdcChStatus_t  *pxChStatus   = NULL;
  IotAdcConfig_t    *pxConfig     = NULL;
  IotAdcChBuffer_t  *pxChBuffer   = NULL;
  int32_t            lStatus      = IOT_ADC_SUCCESS;

  /* pxAdc can't be null */
  if (pxAdc == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* buffer can't be null */
  if (pvBuffer == NULL) {
    return IOT_ADC_INVALID_VALUE;
  }

  /* enter critical section */
  portENTER_CRITICAL();

  /* pxAdc must be already open */
  if (pxAdc->ucIsOpen == pdFALSE) {
    lStatus = IOT_ADC_NOT_OPEN;
  }

  /* exit critical section */
  portEXIT_CRITICAL();

  /* process the request */
  switch(xRequest) {
    /******************/
    /* set ADC config */
    /******************/
    case eSetAdcConfig:
      /* set pxConfig to point to buffer */
      if (lStatus == IOT_ADC_SUCCESS) {
        pxConfig = (IotAdcConfig_t *) pvBuffer;
      }
      /* validate ulAdcSampleTime */
      if (lStatus == IOT_ADC_SUCCESS) {
        if (pxConfig->ulAdcSampleTime > 256) {
          lStatus = IOT_ADC_INVALID_VALUE;
        }
      }
      /* validate ucAdcResolution */
      if (lStatus == IOT_ADC_SUCCESS) {
        switch (pxConfig->ucAdcResolution) {
          case 6:
            break;
          case 8:
            break;
          case 12:
            break;
          default:
            lStatus = IOT_ADC_INVALID_VALUE;
            break;
        }
      }
      /* set driver config */
      if (lStatus == IOT_ADC_SUCCESS) {
        iot_adc_drv_config_set(pxAdc,
                               pxConfig->ulAdcSampleTime,
                               pxConfig->ucAdcResolution);
      }
      /* done */
      break;

    /******************/
    /* get ADC config */
    /******************/
    case eGetAdcConfig:
      /* set pxConfig to point to buffer */
      if (lStatus == IOT_ADC_SUCCESS) {
        pxConfig = (IotAdcConfig_t *) pvBuffer;
      }
      /* get driver config */
      if (lStatus == IOT_ADC_SUCCESS) {
        iot_adc_drv_config_get(pxAdc,
                               &pxConfig->ulAdcSampleTime,
                               &pxConfig->ucAdcResolution);
      }
      /* done */
      break;

    /**********************/
    /* get channel status */
    /**********************/
    case eGetChStatus:
      /* enter critical section */
      portENTER_CRITICAL();
      /* set pxChStatus to point to buffer */
      if (lStatus == IOT_ADC_SUCCESS) {
        pxChStatus = (IotAdcChStatus_t *) pvBuffer;
      }
      /* read channel number */
      if (lStatus == IOT_ADC_SUCCESS) {
        ucAdcChannel = pxChStatus->ucAdcChannel;
      }
      /* read bus status */
      if (lStatus == IOT_ADC_SUCCESS) {
        ucAdcIsBusy = pxAdc->ucBusyStatus;
        ucAdcBusyCh = pxAdc->ucBusyChannel;
      }
      /* store the result */
      if (lStatus == IOT_ADC_SUCCESS) {
        if (ucAdcIsBusy == pdTRUE && ucAdcBusyCh == ucAdcChannel) {
          pxChStatus->xAdcChState = eChStateBusy;
        } else {
          pxChStatus->xAdcChState = eChStateIdle;
        }
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* done */
      break;

    /**********************/
    /* set channel buffer */
    /**********************/
    case eSetChBuffer:
      /* enter critical section */
      portENTER_CRITICAL();
      /* set pxChBuffer to point to buffer */
      if (lStatus == IOT_ADC_SUCCESS) {
        pxChBuffer = (IotAdcChBuffer_t  *) pvBuffer;
      }
      /* read channel number */
      if (lStatus == IOT_ADC_SUCCESS) {
        ucAdcChannel = pxChBuffer->ucAdcChannel;
      }
      /* validate channel number */
      if (lStatus == IOT_ADC_SUCCESS) {
        if (ucAdcChannel == 0) {
          lStatus = IOT_ADC_INVALID_VALUE;
        }
      }
      /* validate buffer pointer */
      if (lStatus == IOT_ADC_SUCCESS) {
        if (pxChBuffer->pvBuffer == NULL) {
          lStatus = IOT_ADC_INVALID_VALUE;
        }
      }
      /* validate buffer length */
      if (lStatus == IOT_ADC_SUCCESS) {
        if ( pxChBuffer->ucBufLen == 0) {
          lStatus = IOT_ADC_INVALID_VALUE;
        }
      }
      /* set buffer */
      if (lStatus == IOT_ADC_SUCCESS) {
        pxAdc->pusBufPtr = pxChBuffer->pvBuffer;
        pxAdc->ucBufMax = pxChBuffer->ucBufLen;
        pxAdc->ucBufCtr = 0;
      }
      /* exit critical section */
      portEXIT_CRITICAL();
      /* done */
      break;

    /*****************/
    /* set ADC chain */
    /*****************/
    case eSetAdcChain:
      if (lStatus == IOT_ADC_SUCCESS) {
        lStatus = IOT_ADC_FUNCTION_NOT_SUPPORTED;
      }
      break;

    /***********/
    /* INVALID */
    /***********/
    default:
      if (lStatus == IOT_ADC_SUCCESS) {
        lStatus = IOT_ADC_INVALID_VALUE;
      }
      break;
  }

  /* done */
  return lStatus;
}
