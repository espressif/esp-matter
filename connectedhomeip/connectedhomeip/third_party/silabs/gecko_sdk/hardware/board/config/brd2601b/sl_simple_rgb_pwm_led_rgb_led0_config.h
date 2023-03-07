/***************************************************************************//**
 * @file
 * @brief Simple RGB PWM Led Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SIMPLE_RGB_PWM_LED_RGB_LED0_CONFIG_H
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Simple RGB PWM LED Configuration
// <o SL_SIMPLE_RGB_PWM_LED_RGB_LED0_FREQUENCY> PWM frequency [Hz]
// <i> Sets the frequency of the PWM signal
// <i> 0 = Don't care
// <i> Default: 10000
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_FREQUENCY      10000

// <o SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RESOLUTION> PWM resolution <2-65536>
// <i> Specifies the PWM (dimming) resolution. I.e. if you want a
// <i> dimming resolution that takes the input values from 0 to 99,
// <i> set this value to 100
// <i> Default: 256
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RESOLUTION     256

// <o SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RED_POLARITY> Red LED Polarity
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW=> Active low
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_HIGH=> Active high
// <i> Default: SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RED_POLARITY   SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW

// <o SL_SIMPLE_RGB_PWM_LED_RGB_LED0_GREEN_POLARITY> Green LED Polarity
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW=> Active low
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_HIGH=> Active high
// <i> Default: SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_GREEN_POLARITY SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW

// <o SL_SIMPLE_RGB_PWM_LED_RGB_LED0_BLUE_POLARITY> Blue LED Polarity
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW=> Active low
// <SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_HIGH=> Active high
// <i> Default: SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_BLUE_POLARITY  SL_SIMPLE_RGB_PWM_LED_POLARITY_ACTIVE_LOW
// </h> end led configuration

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <timer channel=RED,GREEN,BLUE> SL_SIMPLE_RGB_PWM_LED_RGB_LED0
// $[TIMER_SL_SIMPLE_RGB_PWM_LED_RGB_LED0]
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_PERIPHERAL TIMER0
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_PERIPHERAL_NO 0

#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RED_CHANNEL 0
// TIMER0 CC0 on PD02
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RED_PORT  gpioPortD
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_RED_PIN   2

#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_GREEN_CHANNEL 1
// TIMER0 CC1 on PA04
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_GREEN_PORT gpioPortA
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_GREEN_PIN 4

#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_BLUE_CHANNEL 2
// TIMER0 CC2 on PB00
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_BLUE_PORT gpioPortB
#define SL_SIMPLE_RGB_PWM_LED_RGB_LED0_BLUE_PIN  0

// [TIMER_SL_SIMPLE_RGB_PWM_LED_RGB_LED0]$

// <<< sl:end pin_tool >>>

#endif // SL_SIMPLE_RGB_PWM_LED_RGB_LED0_CONFIG_H
