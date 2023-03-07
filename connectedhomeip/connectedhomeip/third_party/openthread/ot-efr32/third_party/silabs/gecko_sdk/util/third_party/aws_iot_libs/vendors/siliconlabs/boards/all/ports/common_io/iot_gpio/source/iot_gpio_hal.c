/***************************************************************************//**
 * @file    iot_gpio_hal.c
 * @brief   Silicon Labs implementation of Common I/O GPIO API.
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
#include "em_gpio.h"
#include "em_cmu.h"

/* SDK emdrv layer */
#include "gpiointerrupt.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

/* Common I/O drivers layer */
#include "iot_gpio_desc.h"

/* Common I/O interface layer */
#include "iot_gpio.h"

/*******************************************************************************
 *                         MIDDLE-LAYER PROTOTYPES
 ******************************************************************************/

/* configure GPIO IRQ */
static int32_t sl_gpio_config_irq(IotGpioHandle_t pxGpio);

/* configure GPIO pin */
static void sl_gpio_config_pin(IotGpioHandle_t pxGpio);

/* validate GPIO handle */
static bool sl_gpio_validate(IotGpioHandle_t pxGpio);

/*******************************************************************************
 *                MIDDLE LAYER: INTERFACE WITH GPIO LOW-LEVEL
 ******************************************************************************/

/************************ sl_gpio_irq_callback() ******************************/

static void sl_gpio_irq_callback(uint8_t intNo)
{
  uint32_t table_size = iot_gpio_desc_get_table_size();
  for (uint32_t i = 0; i < table_size; i++) {
    // Validate interrupt number and if gpio is opened
    if (intNo == iot_gpio_desc[i].int_number && iot_gpio_desc[i].is_open) {
      // Validate callback and Interrupt enabled
      if (iot_gpio_desc[i].callback != NULL
          && iot_gpio_desc[i].interrupt != eGpioInterruptNone) {
        uint8_t state = GPIO_PinInGet(iot_gpio_desc[i].port, iot_gpio_desc[i].pin);

        iot_gpio_desc[i].callback(state, iot_gpio_desc[i].context);
      }
    }
  }
}

/************************* sl_gpio_config_irq() *******************************/

static int32_t sl_gpio_config_irq(IotGpioHandle_t pxGpioHandle)
{
  static bool gpio_int_init = false;
  bool disable_int = false;

  switch (pxGpioHandle->interrupt) {
    case eGpioInterruptNone:
      GPIO_ExtIntConfig(pxGpioHandle->port, pxGpioHandle->pin,
                        pxGpioHandle->int_number, false, false, false);
      disable_int = true;
      break;
    case eGpioInterruptRising:
      GPIO_ExtIntConfig(pxGpioHandle->port, pxGpioHandle->pin,
                        pxGpioHandle->int_number, true, false, true);
      gpio_int_init = true;
      break;
    case eGpioInterruptFalling:
      GPIO_ExtIntConfig(pxGpioHandle->port, pxGpioHandle->pin,
                        pxGpioHandle->int_number, false, true, true);
      gpio_int_init = true;
      break;
    case eGpioInterruptEdge:
      GPIO_ExtIntConfig(pxGpioHandle->port, pxGpioHandle->pin,
                        pxGpioHandle->int_number, true, true, true);
      gpio_int_init = true;
      break;
    case eGpioInterruptLow:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
    case eGpioInterruptHigh:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
  }

  if (disable_int && gpio_int_init) {
    GPIOINT_CallbackUnRegister(pxGpioHandle->int_number);
    return IOT_GPIO_SUCCESS;
  }

  if (!gpio_int_init) {
    GPIOINT_Init();
    gpio_int_init = true;
  }

  GPIOINT_CallbackRegister(pxGpioHandle->int_number, sl_gpio_irq_callback);

  return IOT_GPIO_SUCCESS;
}

/************************* sl_gpio_config_pin() *******************************/

static void sl_gpio_config_pin(IotGpioHandle_t pxGpio)
{
  switch (pxGpio->direction) {
    case eGpioDirectionInput:
      if (pxGpio->pull == eGpioPullUp) {
        GPIO_PinModeSet(pxGpio->port, pxGpio->pin, gpioModeInputPull, 1);
      } else if (pxGpio->pull == eGpioPullDown) {
        GPIO_PinModeSet(pxGpio->port, pxGpio->pin, gpioModeInputPull, 0);
      } else {
        GPIO_PinModeSet(pxGpio->port, pxGpio->pin, gpioModeInput,
                        GPIO_PinOutGet(pxGpio->port, pxGpio->pin));
      }
      break;

    case eGpioDirectionOutput:
      if (pxGpio->out_mode == eGpioOpenDrain) {
        if (pxGpio->pull == eGpioPullUp) {
          GPIO_PinModeSet(pxGpio->port, pxGpio->pin,
                          gpioModeWiredAndPullUp,
                          GPIO_PinOutGet(pxGpio->port, pxGpio->pin));
        } else {
          GPIO_PinModeSet(pxGpio->port, pxGpio->pin, gpioModeWiredAnd,
                          GPIO_PinOutGet(pxGpio->port, pxGpio->pin));
        }
      } else {
        GPIO_PinModeSet(pxGpio->port, pxGpio->pin, gpioModePushPull,
                        GPIO_PinOutGet(pxGpio->port, pxGpio->pin));
      }
      break;
  }
}

/************************** sl_gpio_validate() ********************************/

static bool sl_gpio_validate(IotGpioHandle_t pxGpio)
{
  if (pxGpio == NULL) {
    return false;
  }
  return pxGpio->is_open;
}

/*******************************************************************************
 *                             iot_gpio_open()
 ******************************************************************************/

/**
 * @brief   iot_gpio_open is used to open the GPIO handle.
 *          The application must call this function to open desired GPIO and use other functions.
 *
 * @param[in]   lGpioNumber The logical GPIO number to open. It depends on the
 *                          implementation to map logical GPIO number to physical GPIO port
 *                          and pin.
 *
 * @return
 *   - handle to the GPIO peripheral if everything succeeds
 *   - NULL, if
 *    - invalid instance number
 *    - open same instance more than once before closing it.
 */
IotGpioHandle_t iot_gpio_open(int32_t lGpioInstanceNumber)
{
  static bool clock_enabled = false;
  IotGpioHandle_t gpio_handle = NULL;

  if (!clock_enabled) {
    CMU_ClockEnable(cmuClock_GPIO, true);
    clock_enabled = true;
  }

  gpio_handle = iot_gpio_desc_get(lGpioInstanceNumber);

  if (gpio_handle == NULL) {
    return NULL;
  }

  if (gpio_handle->is_open) {
    return NULL;
  }

  sl_gpio_config_pin(gpio_handle);
  sl_gpio_config_irq(gpio_handle);

  gpio_handle->is_open = true;

  return gpio_handle;
}

/*******************************************************************************
 *                          iot_gpio_set_callback()
 ******************************************************************************/

/**
 * @brief   iot_gpio_set_callback is used to set the callback to be called when an
 *          interrupt is tirggered.
 *
 * @note Single callback is used for both read_async and write_async. Newly set callback overrides the one previously set
 * @note This callback will not be invoked when synchronous operation completes.
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in] pxGpio    The GPIO handle returned in the open() call.
 * @param[in] xGpioCallback The callback function to be called on interrupt.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_gpio_set_callback(IotGpioHandle_t const pxGpio,
                           IotGpioCallback_t xGpioCallback,
                           void * pvUserContext)
{
  portENTER_CRITICAL();

  if (!sl_gpio_validate(pxGpio) || xGpioCallback == NULL) {
    portEXIT_CRITICAL();
    return;
  }

  pxGpio->callback = xGpioCallback;
  pxGpio->context = pvUserContext;
  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                           iot_gpio_read_sync()
 ******************************************************************************/

/**
 * @brief   iot_gpio_read_sync is used to read data from GPIO pin in blocking mode.
 *
 * @param[in]   pxGpio      The GPIO handle returned in the open() call.
 * @param[out]  pucPinState The variable which reads state of the GPIO pin.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if pxGpio or pucPinState are NULL
 *   - IOT_GPIO_READ_FAILED on failure.
 *
 */
int32_t iot_gpio_read_sync(IotGpioHandle_t const pxGpio,
                           uint8_t * pucPinState)
{
  if (!sl_gpio_validate(pxGpio) || pucPinState == NULL) {
    return IOT_GPIO_INVALID_VALUE;
  }

  *pucPinState = (uint8_t)GPIO_PinInGet(pxGpio->port, pxGpio->pin);

  return IOT_GPIO_SUCCESS;
}

/*******************************************************************************
 *                           iot_gpio_write_sync()
 ******************************************************************************/

/**
 * @brief   iot_gpio_write_sync is used to write data into the GPIO pin in blocking mode.
 *
 * @param[in]   pxGpio      The GPIO handle returned in the open() call.
 * @param[in]   ucState     The value to write into the GPIO pin.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if pxGpio is NULL
 *   - IOT_GPIO_WRITE_FAILED on failure.
 */
int32_t iot_gpio_write_sync(IotGpioHandle_t const pxGpio,
                            uint8_t ucPinState)
{
  if (!sl_gpio_validate(pxGpio)) {
    return IOT_GPIO_INVALID_VALUE;
  }

  if (ucPinState > 1) {
    return IOT_GPIO_INVALID_VALUE;
  } else if (ucPinState) {
    GPIO_PinOutSet(pxGpio->port, pxGpio->pin);
  } else {
    GPIO_PinOutClear(pxGpio->port, pxGpio->pin);
  }

  return IOT_GPIO_SUCCESS;
}

/*******************************************************************************
 *                             iot_gpio_close()
 ******************************************************************************/

/**
 * @brief   iot_gpio_close is used to deinitializes the GPIO pin to default value and close the handle.
 *          The application should call this function to reset and deinitialize the GPIO pin.
 *
 * @param[in]   pxGpio The GPIO handle returned in the open() call.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if
 *      - pxGpio handle is NULL
 *      - if is not in open state (already closed).
 */
int32_t iot_gpio_close(IotGpioHandle_t const pxGpio)
{
  if (!sl_gpio_validate(pxGpio)) {
    return IOT_GPIO_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  pxGpio->interrupt = eGpioInterruptNone;
  pxGpio->direction = eGpioDirectionInput;
  pxGpio->pull = eGpioPullNone;
  pxGpio->callback = NULL;
  pxGpio->context = NULL;

  sl_gpio_config_pin(pxGpio);
  sl_gpio_config_irq(pxGpio);
  pxGpio->is_open = false;

  portEXIT_CRITICAL();

  return IOT_GPIO_SUCCESS;
}

/*******************************************************************************
 *                             iot_gpio_ioctl()
 ******************************************************************************/

/**
 * @brief   iot_gpio_ioctl is used to configure GPIO pin options.
 *          The application should call this function to configure various GPIO
 *          pin options: pin function, I/O direction, pin internal pull mode,
 *          drive strength, slew rate etc
 *
 * @param[in] pxGpio        The GPIO handle returned in the open() call.
 * @param[in] xRequest      One of IotGpioIoctlRequest_t enum
 * @param[in/out] pvBuffer  Buffer holding GPIO set or get values.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE on NULL handle, invalid request, or NULL buffer when required.
 *   - IOT_GPIO_FUNCTION_NOT_SUPPORTED is only valid for
 *      - eSetGpioFunction / eGetGpioFunction
 *      - eSetGpioSpeed / eGetGpioSpeed
 *      - eSetGpioDriveStrength / eGetGpioDriveStrength
 *      - eSetGpioInterrupt / eGetGpioInterrupt
 */
int32_t iot_gpio_ioctl(IotGpioHandle_t const pxGpio,
                       IotGpioIoctlRequest_t xRequest,
                       void * const pvBuffer)
{
  int32_t status = IOT_GPIO_SUCCESS;

  if (!sl_gpio_validate(pxGpio)) {
    return IOT_GPIO_INVALID_VALUE;
  }

  switch (xRequest) {
    case eSetGpioFunction:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    case eSetGpioDirection:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        pxGpio->direction = *(IotGpioDirection_t *)pvBuffer;
      }
      sl_gpio_config_pin(pxGpio);
      break;

    case eSetGpioPull:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        pxGpio->pull = *(IotGpioPull_t *)pvBuffer;
      }
      sl_gpio_config_pin(pxGpio);
      break;

    case eSetGpioOutputMode:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        pxGpio->out_mode = *(IotGpioOutputMode_t *)pvBuffer;
      }
      sl_gpio_config_pin(pxGpio);
      break;

    case eSetGpioInterrupt:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else if (*(IotGpioInterrupt_t *)pvBuffer == eGpioInterruptHigh
                 || *(IotGpioInterrupt_t *)pvBuffer == eGpioInterruptLow) {
        return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      } else if (*(IotGpioInterrupt_t *)pvBuffer == eGpioInterruptNone ) {
        return IOT_GPIO_INVALID_VALUE;
      }
      pxGpio->interrupt = *(IotGpioInterrupt_t *)pvBuffer;
      status = sl_gpio_config_irq(pxGpio);
      break;

    case eSetGpioSpeed:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    case eSetGpioDriveStrength:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetGpioFunction:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetGpioDirection:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        *(IotGpioDirection_t *)pvBuffer = pxGpio->direction;
      }
      break;

    case eGetGpioPull:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        *(IotGpioPull_t *)pvBuffer = pxGpio->pull;
      }
      break;

    case eGetGpioOutputType:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        *(IotGpioOutputMode_t *)pvBuffer = pxGpio->out_mode;
      }
      break;

    case eGetGpioInterrupt:
      if (pvBuffer == NULL) {
        return IOT_GPIO_INVALID_VALUE;
      } else {
        *(IotGpioInterrupt_t *)pvBuffer = pxGpio->interrupt;
      }
      break;

    case eGetGpioSpeed:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetGpioDriveStrength:
      return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
      break;

    default:
      return IOT_GPIO_INVALID_VALUE;
  }

  return status;
}
