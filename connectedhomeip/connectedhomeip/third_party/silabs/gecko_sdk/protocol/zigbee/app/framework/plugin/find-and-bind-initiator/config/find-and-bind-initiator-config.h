/***************************************************************************//**
 * @brief Zigbee Find and Bind Initiator component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Find and Bind Initiator configuration

// <o EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_TARGET_RESPONSES_COUNT> Target Responses Count <0-255>
// <i> Default: 5
// <i> The number of target responses that the initiator will accept.
#define EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_TARGET_RESPONSES_COUNT   5

// <o EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_TARGET_RESPONSES_DELAY_MS> Target Responses Delay <0-65535>
// <i> Default: MILLISECOND_TICKS_PER_SECOND*3
// <i> How long the initiator will listen for target responses, in milliseconds.
#define EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_TARGET_RESPONSES_DELAY_MS   MILLISECOND_TICKS_PER_SECOND * 3

// </h>

// <<< end of configuration section >>>
