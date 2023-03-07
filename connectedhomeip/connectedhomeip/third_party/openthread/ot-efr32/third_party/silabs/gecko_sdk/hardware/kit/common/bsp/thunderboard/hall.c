/***************************************************************************//**
 * @file
 * @brief Driver for Hall Effect Sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include "i2cspm.h"
#include "gpiointerrupt.h"
#include "em_cmu.h"

#include "board.h"
#include "util.h"
#include "hall.h"
#include "hall_device.h"
#include "hall_config.h"

#include "stdio.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup HALL HALL - Hall Effect Sensor
 * @{
 * @brief Driver for Hall effect sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static HALL_IntCallback pinIntCallback;        /**<  Interrupt callback function   */
#ifdef BOARD_HALL_OUTPUT_PIN
static void gpioIntCallback(void);
#endif

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes the Hall sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t HALL_init(void)
{
  uint32_t status;

  /* Enable power to the sensor */
  BOARD_hallSensorEnable(true);

  /* Allow some time for the part to power up */
  UTIL_delay(10);

#ifdef BOARD_HALL_OUTPUT_PIN
  /* Enable interrupt */
  BOARD_hallSensorSetIRQCallback(gpioIntCallback);
  BOARD_hallSensorEnableIRQ(true);
#endif

  status = HALL_initDevice();

  pinIntCallback = NULL;

  return status;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the Hall sensor. Disables the sensor power domain.
 *
 * @return
 *    None
 ******************************************************************************/
void HALL_deInit(void)
{
#ifdef BOARD_HALL_OUTPUT_PIN
  /* Disable interrupts */
  BOARD_hallSensorEnableIRQ(false);
#endif

  /* Disable sensor power */
  BOARD_hallSensorEnable(false);

  HALL_deInitDevice();
}

/***************************************************************************//**
 * @brief
 *    Register GPIO interrupt callback function. This function will be called
 *    when the ALERT pin of the Hall sensor changes state and the interrupt
 *    functions are enabled.
 *
 * @param[in] callback
 *    Pointer to the callback function
 *
 * @return
 *    None
 ******************************************************************************/
void HALL_registerCallback(HALL_IntCallback callback)
{
  pinIntCallback = callback;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#ifdef BOARD_HALL_OUTPUT_PIN
/***************************************************************************//**
 * @brief
 *    GPIO interrupt callback function. It is called when the ALERT pin of the
 *    Hall sensor asserted and the interrupt functions are enabled.
 *
 * @param[in] pin
 *    The microcontroller pin connected to ALERT output (not used)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static void gpioIntCallback(void)
{
  bool pinIsHigh;

  if ( GPIO_PinInGet(HALL_GPIO_PORT_OUT, HALL_GPIO_PIN_OUT) ) {
    pinIsHigh = true;
  } else {
    pinIsHigh = false;
  }

  if ( pinIntCallback != NULL ) {
    pinIntCallback(pinIsHigh);
  }

  return;
}
#endif

/** @endcond */

/** @} {end addtogroup HALL} */
/** @} {end addtogroup TBSense_BSP} */
