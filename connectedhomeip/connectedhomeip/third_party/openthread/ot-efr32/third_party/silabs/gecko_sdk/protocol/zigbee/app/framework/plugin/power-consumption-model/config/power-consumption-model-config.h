/***************************************************************************//**
 * @brief Zigbee Power Consumption Model component configuration header.
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

// <h>Zigbee Power Consumption Model configuration

// <o EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_DEFAULT_POWER_RATING> Power Rating for Lightbulb (W) <1-160>
// <i> Default: 10
// <i> Lightbulb Power Rating
#define EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_DEFAULT_POWER_RATING   10

// <o EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_POLLING_TIME_INTERVAL_MS> Polling time (mS) <100-10000>
// <i> Default: 1000
// <i> Time it takes to update attribute
#define EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_POLLING_TIME_INTERVAL_MS   1000

// <o EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_OFF_POWER_CONSUMPTION_MILLI_WATT> Off Power Consumption (mW) <10-2000>
// <i> Default: 500
// <i> Power consumed by lightbulb when light is off
#define EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_OFF_POWER_CONSUMPTION_MILLI_WATT   500

// <o EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_ON_POWER_CONSUMPTION> On Power Consumption (W) <1-10>
// <i> Default: 1
// <i> Power consumed by lightbulb when light is turned on (5%)
#define EMBER_AF_PLUGIN_POWER_CONSUMPTION_MODEL_ON_POWER_CONSUMPTION   1

// </h>

// <<< end of configuration section >>>
