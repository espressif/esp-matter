/***************************************************************************//**
 * @file
 * @brief Routines for the Identify plugin, which implements the Identify
 *        cluster.
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

#ifndef SILABS_ZIGBEE_IDENTIFY_SERVER_H
#define SILABS_ZIGBEE_IDENTIFY_SERVER_H

/**
 * @defgroup identify Identify Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Identify Cluster Server Component
 *
 * Silicon Labs implementation of the Identify cluster. It requires extending
 * to integrate the code with the hardware, for example to
 * make a light blink during identify.
 *
 */

/**
 * @addtogroup identify
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup identify_cb Identify
 * @ingroup af_callback
 * @brief Callbacks for Identify Component
 *
 */

/**
 * @addtogroup identify_cb
 * @{
 */

/** @brief Start feedback.
 *
 * This function is called by the Identify plugin when identification begins.
 * It informs the Identify Feedback plugin that it should begin providing its
 * implemented feedback functionality (e.g., LED blinking, buzzer sounding,
 * etc.) until the Identify plugin tells it to stop. The identify time is
 * purely a matter of informational convenience. This plugin does not need to
 * know how long it will identify (the Identify plugin will perform the
 * necessary timekeeping.)
 *
 * @param endpoint The identifying endpoint Ver.: always
 * @param identifyTime The identify time Ver.: always
 */
void emberAfPluginIdentifyStartFeedbackCallback(uint8_t endpoint,
                                                uint16_t identifyTime);

/** @brief Stop feedback.
 *
 * This function is called by the Identify plugin when identification is
 * finished. It tells the Identify Feedback plugin to stop providing its
 * implemented feedback functionality.
 *
 * @param endpoint The identifying endpoint Ver.: always
 */
void emberAfPluginIdentifyStopFeedbackCallback(uint8_t endpoint);

/** @} */ // end of identify_cb
/** @} */ // end of name Callbacks
/** @} */ // end of identify

#endif // SILABS_ZIGBEE_IDENTIFY_SERVER_H
