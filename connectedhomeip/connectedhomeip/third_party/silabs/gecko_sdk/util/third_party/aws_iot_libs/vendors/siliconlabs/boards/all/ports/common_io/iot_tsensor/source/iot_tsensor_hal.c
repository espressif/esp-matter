/***************************************************************************//**
 * @file    iot_tsensor_hal.c
 * @brief   Silicon Labs implementation of Common I/O TempSensor API.
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
#include "em_device.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "string.h"

/* Common I/O interface layer */
#include "iot_tsensor.h"

/* Internal Temperature sensor driver */
#include "tempdrv.h"

/*******************************************************************************
 *                               DEFINES
 ******************************************************************************/

#define SL_TEMPERATURE_SCALE_FACTOR     (1000)

/*******************************************************************************
 *                               TYPEDEFS
 ******************************************************************************/

typedef struct IotTsensorDescriptor {
  /* instance ID */
  int32_t               instance;
  /* open flag */
  bool                  is_open;
  /* enable flag */
  bool                  is_enabled;
  /* callback */
  IotTsensorCallback_t  callback;
  void                  *callback_param;
  int32_t                low_temp_limit;
  int32_t                high_temp_limit;
} IotTsensorDescriptor;

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

static IotTsensorDescriptor tsensor_descriptor = { 0 };

/*******************************************************************************
 *                   MIDDLE LAYER: TMPDRV INTERFACE
 ******************************************************************************/
static void tempdrv_callback(int8_t temp, TEMPDRV_LimitType_t limit)
{
  IotTsensorStatus_t lStatus;
  (void)temp;

  if (TEMPDRV_LIMIT_LOW == limit) {
    lStatus = eTsensorMinThresholdReached;
  } else {
    lStatus = eTsensorMaxThresholdReached;
  }

  if (tsensor_descriptor.callback != NULL) {
    tsensor_descriptor.callback(lStatus, tsensor_descriptor.callback_param);
  }
}

/*******************************************************************************
 *                            iot_tsensor_open()
 ******************************************************************************/

/**
 * @brief   iot_tsensor_open is used to initialize the temperature sensor.
 *          It sets up the clocks  and power etc, if the sensor is internal
 *          and sets up the communication channel if the sensor is external.
 *
 * @param[in]   lTsensorInstance   The instance of the tsensor to initialize. The instance
 *                                 number is platform specific. i,e if you have more than one temperature
 *                                 ensors, then 2 instances point to 2 different sensors.
 *
 * @return
 *   - Handle to tsensor interface on success.
 *   - NULL if
 *      - lTsensorInstance is invalid.
 *      - instance is already open.
 */
IotTsensorHandle_t iot_tsensor_open(int32_t lTsensorInstance)
{
  IotTsensorHandle_t pSensorHandle = NULL;

  // instance 0 is the internal sensor
  if (lTsensorInstance != 0) {
    return NULL;
  }

  pSensorHandle = &tsensor_descriptor;

  if (pSensorHandle->is_open == true) {
    return NULL;
  }

  if (ECODE_EMDRV_TEMPDRV_OK == TEMPDRV_Init()) {
    pSensorHandle->is_open = true;
    // the init function enables internal sensor interrupts
    TEMPDRV_Enable(false);
    pSensorHandle->is_enabled = false;
  } else {
    pSensorHandle = NULL;
  }

  return pSensorHandle;
}

/*******************************************************************************
 *                          iot_tsensor_set_callback()
 ******************************************************************************/

/**
 * @brief   Set the callback to be called when a threshold is reached on the sensor.
 *          The caller must set the threshold level using IOCTL before the callback can be called.
 *
 * @note Single callback is used per instance for both min and max threshold points being reached.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @warning If threshold detection is not supported by the hardware, then the callback function will not work.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in open() call
 * @param[in]   xCallback       The callback function to be called on completion of transaction.
 * @param[in]   pvUserContext   user provid context
 */
void iot_tsensor_set_callback(IotTsensorHandle_t const xTsensorHandle,
                              IotTsensorCallback_t xCallback,
                              void * pvUserContext)
{
  if ((xTsensorHandle != NULL) && (xCallback != NULL)) {
    portENTER_CRITICAL();
    xTsensorHandle->callback = xCallback;
    xTsensorHandle->callback_param = pvUserContext;
    portEXIT_CRITICAL();
  }
  return;
}

/*******************************************************************************
 *                             iot_tsensor_enable()
 ******************************************************************************/

/**
 * @brief   iot_tsensor_enable is used to enable the temperature sensor
 *          to start reading the temperature and trigger thresholds (if any were set and supported)
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if xTsensorHandle is NULL.
 */
int32_t iot_tsensor_enable(IotTsensorHandle_t const xTsensorHandle)
{
  int32_t lStatus = IOT_TSENSOR_SUCCESS;

  if (xTsensorHandle == NULL) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_open == false) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (ECODE_EMDRV_TEMPDRV_OK == TEMPDRV_Enable(true)) {
    xTsensorHandle->is_enabled = true;
  } else {
    lStatus = IOT_TSENSOR_INVALID_VALUE;
  }

  return lStatus;
}

/*******************************************************************************
 *                             iot_tsensor_disable()
 ******************************************************************************/

/**
 * @brief   iot_tsensor_disable is used to disable the temperature sensor
 *          which stops monitoring the temperature.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if xTsensorHandle is NULL.
 */
int32_t iot_tsensor_disable(IotTsensorHandle_t const xTsensorHandle)
{
  int32_t lStatus = IOT_TSENSOR_SUCCESS;

  if (xTsensorHandle == NULL) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_open == false) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (ECODE_EMDRV_TEMPDRV_OK == TEMPDRV_Enable(false)) {
    xTsensorHandle->is_enabled = false;
  } else {
    lStatus = IOT_TSENSOR_INVALID_VALUE;
  }

  return lStatus;
}

/*******************************************************************************
 *                           iot_tsensor_get_temp()
 ******************************************************************************/

/**
 * @brief   iot_tsensor_get_temp is used to get the current temperature
 *          read from the sensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 * @param[out]  plTemp           temperature read from the sensor.
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if
 *      - xTsensorHandle is NULL
 *      - lTemp is NULL
 *   - IOT_TSENSOR_DISABLED if tsensor instance has been disabled with call to iot_tsensor_disable().
 *   - IOT_TSENSOR_GET_TEMP_FAILED if error occured reading the temperature.
 */
int32_t iot_tsensor_get_temp(IotTsensorHandle_t const xTsensorHandle,
                             int32_t * plTemp)
{
  int32_t lStatus = IOT_TSENSOR_SUCCESS;

  if (xTsensorHandle == NULL || plTemp == NULL) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_open == false) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_enabled == false) {
    return IOT_TSENSOR_DISABLED;
  }

  // tempdrv does not return a status code
  *plTemp = (int32_t)(TEMPDRV_GetTemp() * SL_TEMPERATURE_SCALE_FACTOR);

  return lStatus;
}

/*******************************************************************************
 *                             iot_tsensor_ioctl()
 ******************************************************************************/

/**
 * @brief   iot_tsensor_ioctl is used to set tsensor configuration
 *          and tsensor properties like minimum threshold, maximum threshold value, etc.
 *          Supported IOCTL requests are defined in aws_hal_Tsensor_Ioctl_Request_t
 *
 * @param[in]       xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 * @param[in]       xRequest    configuration request.
 * @param[in,out]   pvBuffer    buffer holding tsensor set and get values.
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE
 *      - xTsensorHandle is NULL
 *      - xRequest is invalid
 *      - pvBuffer is NULL (excluding eTsensorPerformCalibration)
 *   - IOT_TSENSOR_CLOSED if instance not in open state.
 *   - IOT_TSENSOR_DISABLED if tsensor instance has been disabled with call to iot_tsensor_disable().
 *   - IOT_TSENSOR_GET_TEMP_FAILED if error occured reading the temperature.
 *   - IOT_TSENSOR_SET_FAILED if set threshold operation failed.
 *   - IOT_TSENSOR_NOT_SUPPORTED valid if xRequest feature not supported.
 * @note:  If eTsensorSetMinThreshold or eTsensorSetMaxThreshold ioctl is supported, then the
 *         corresponding eTsensorGetMinThreshold and eTsensorGetMaxThreshold must also be supported.
 */
int32_t iot_tsensor_ioctl(IotTsensorHandle_t const xTsensorHandle,
                          IotTsensorIoctlRequest_t xRequest,
                          void * const pvBuffer)
{
  int32_t lStatus = IOT_TSENSOR_SUCCESS;
  int32_t *pValue;

  if (xTsensorHandle == NULL) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_open == false) {
    return IOT_TSENSOR_CLOSED;
  }

  if ((pvBuffer == NULL) && (xRequest != eTsensorPerformCalibration)) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  pValue = (int32_t *)pvBuffer;

  switch (xRequest) {
    case eTsensorSetMinThreshold:
      if (ECODE_EMDRV_TEMPDRV_OK == TEMPDRV_RegisterCallback((int8_t)(*pValue / SL_TEMPERATURE_SCALE_FACTOR),
                                                              TEMPDRV_LIMIT_LOW,
                                                              tempdrv_callback)) {
        xTsensorHandle->low_temp_limit = *pValue;
      } else {
        lStatus = IOT_TSENSOR_SET_FAILED;
      }
      break;
    case eTsensorSetMaxThreshold:
      if (ECODE_EMDRV_TEMPDRV_OK == TEMPDRV_RegisterCallback((int8_t)(*pValue / SL_TEMPERATURE_SCALE_FACTOR),
                                                              TEMPDRV_LIMIT_HIGH,
                                                              tempdrv_callback)) {
        xTsensorHandle->high_temp_limit = *pValue;
      } else {
        lStatus = IOT_TSENSOR_SET_FAILED;
      }
      break;
    case eTsensorGetMinThreshold:
      *pValue = xTsensorHandle->low_temp_limit;
      break;
    case eTsensorGetMaxThreshold:
      *pValue = xTsensorHandle->high_temp_limit;
      break;
    case eTsensorPerformCalibration:
      lStatus = IOT_TSENSOR_NOT_SUPPORTED;
      break;
    default:
      lStatus = IOT_TSENSOR_INVALID_VALUE;
  }

  return lStatus;
}

/*******************************************************************************
 *                             iot_tsensor_close()
 ******************************************************************************/

/**
 * @brief iot_tsensor_close is used to de-initialize Tsensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if
 *      - xTensorHandle is NULL
 *      - not in open state (already closed)
 */
int32_t iot_tsensor_close(IotTsensorHandle_t const xTsensorHandle)
{
  int32_t lStatus = IOT_TSENSOR_SUCCESS;

  if (xTsensorHandle == NULL) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  if (xTsensorHandle->is_open == false) {
    return IOT_TSENSOR_INVALID_VALUE;
  }

  portENTER_CRITICAL();
  xTsensorHandle->is_open         = false;
  xTsensorHandle->is_enabled      = false;
  xTsensorHandle->callback        = NULL;
  xTsensorHandle->callback_param  = NULL;
  portEXIT_CRITICAL();

  return lStatus;
}
