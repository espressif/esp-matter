/***************************************************************************//**
 * @file
 * @brief Joystick Driver User Config
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_JOYSTICK_CONFIG_H
#define SL_JOYSTICK_CONFIG_H

#include "em_gpio.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Joystick Voltage value Configuration

// <o REFERENCE_VOLTAGE> Reference voltage value for analog Joystick signal <f.d>
// <i> Vref magnitude expressed in millivolts. As per Joystick Hardware on Wireless Pro Kit, Vref = AVDD = 3300 mV
// <i> Default: 3300
#define REFERENCE_VOLTAGE 3300

// <o JOYSTICK_MV_C> Center position mV value <f.d>
// <i> Default: 3
#define JOYSTICK_MV_C    3

// <o JOYSTICK_MV_N> North position mV value <f.d>
// <i> Default: 2831
#define JOYSTICK_MV_N    2831

// <o JOYSTICK_MV_E> East position mV value <f.d>
// <i> Default: 2533
#define JOYSTICK_MV_E    2533

// <o JOYSTICK_MV_S> South position mV value <f.d>
// <i> Default: 1650
#define JOYSTICK_MV_S    1650

// <o JOYSTICK_MV_W> West position mV value <f.d>
// <i> Default: 1980
#define JOYSTICK_MV_W    1980

// <o JOYSTICK_MV_ERR_CARDINAL_ONLY> Joystick error mV value when enabled for Cardinal Directions only <f.d>
// <i> This value will not be used when joystick is enabled for secondary directions
// <i> Default: 150
#define JOYSTICK_MV_ERR_CARDINAL_ONLY    150

// <e> Enable secondary directions
// <i> Enables secondary directions (NW, NE, SW, SE)
// <i> Note: Joystick Hardware on Wireless Pro Kit does not support Secondary directions
#ifndef ENABLE_SECONDARY_DIRECTIONS
#define ENABLE_SECONDARY_DIRECTIONS 1
#endif
// <o JOYSTICK_MV_NE> Northeast position mV value <f.d>
// <i> Default: 2247
#define JOYSTICK_MV_NE    2247

// <o JOYSTICK_MV_NW> Northwest position mV value <f.d>
// <i> Default: 1801
#define JOYSTICK_MV_NW    1801

// <o JOYSTICK_MV_SE> Southeast position mV value <f.d>
// <i> Default: 1433
#define JOYSTICK_MV_SE    1433

// <o JOYSTICK_MV_SW> Southwest position mV value <f.d>
// <i> Default: 1238
#define JOYSTICK_MV_SW    1238

// <o JOYSTICK_MV_ERR_CARDINAL_AND_SECONDARY> Joystick error mV value when enabled for Cardinal and Secondary Directions <f.d>
// <i> Default: 75
#define JOYSTICK_MV_ERR_CARDINAL_AND_SECONDARY    75

// </e> end Joystick direction secondary directions selection

// </h> end Joystick Voltage value Configuration

// <h>Joystick signal sampling rate Configuration

// <o TIMER_CYCLES> Joystick signal sampling rate [samples/second]
// <i> Sets the sampling rate for Joystick signal
// <50000=> 100 samples/second
// <5000=> 1000 samples/second
// <1000=> 5000 samples/second
// <500=> 10000 samples/second
// <200=> 25000 samples/second
// <i> Default: 50000
#define TIMER_CYCLES      50000

// </h> end Joystick signal sampling rate Configuration

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <gpio> SL_JOYSTICK
// $[GPIO_SL_JOYSTICK]
#define SL_JOYSTICK_PORT                         gpioPortA
#define SL_JOYSTICK_PIN                          0

// [GPIO_SL_JOYSTICK]$

// <<< sl:end pin_tool >>>

#endif // SL_JOYSTICK_CONFIG_H
