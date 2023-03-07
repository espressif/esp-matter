/***************************************************************************//**
 * @file
 * @brief Routines for the Color Control Server plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_ZIGBEE_COLOR_CONTROL_SERVER_H
#define SILABS_ZIGBEE_COLOR_CONTROL_SERVER_H

/**
 * @defgroup color-control-server Color Control Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Color Control Cluster Server Component
 *
 * This component is an implementation of the color server.
 * It supports color temperature,
 * CIE 1931, and hue/saturation.  It also supports smooth transitions
 * within a particular color mode.
 *
 */

/**
 * @addtogroup color-control-server
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup color_control_cb Color Control Server
 * @ingroup af_callback
 * @brief Callbacks for Color Control Server Component
 *
 */

/**
 * @addtogroup color_control_cb
 * @{
 */

/** @brief Compute PWM from HSV.
 *
 * This function is called from the color server when the PWMs need to
 * be driven with a new value from the HSV values.
 *
 * @param endpoint The identifying endpoint Ver.: always
 */
void emberAfPluginColorControlServerComputePwmFromHsvCallback(uint8_t endpoint);

/** @brief Compute PWM from HSV.
 *
 * This function is called from the color server when the PWMs need to
 * be driven with a new value from the color X and color Y values.
 *
 * @param endpoint The identifying endpoint Ver.: always
 */
void emberAfPluginColorControlServerComputePwmFromXyCallback(uint8_t endpoint);

/** @brief Compute PWM from HSV.
 *
 * This function is called from the color server when the PWMs need to
 * be driven with a new value from the color temperature.
 *
 * @param endpoint The identifying endpoint Ver.: always
 */
void emberAfPluginColorControlServerComputePwmFromTempCallback(uint8_t endpoint);
/** @} */ // end of name color_control_cb
/** @} */ // end of name Callbacks
/** @} */ // end of color-control-server

#endif // SILABS_ZIGBEE_COLOR_CONTROL_SERVER_H
