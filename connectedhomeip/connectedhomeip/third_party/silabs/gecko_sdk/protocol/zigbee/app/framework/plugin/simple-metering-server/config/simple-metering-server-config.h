/***************************************************************************//**
 * @brief Zigbee Simple Metering Server component configuration header.
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

// <h>Zigbee Simple Metering Server configuration

// <o EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_TYPE> Test Meter Type
// <i> Test Meter Type
// <EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_ELECTRIC_METER=> Electric meter
// <EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_GAS_METER=> Gas meter
// <i> Default: EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_ELECTRIC_METER
// <i> What kind of meter is going to be simulated
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_TYPE   EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_ELECTRIC_METER

// <o EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_BATTERY_RATE> Battery Consumption Rate (in minutes) <1-255>
// <i> Default: 5
// <i> Battery Life Remaining begins at 255. This will be decremented by one battery percentage point every X minutes.
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_BATTERY_RATE   5

// <o EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_PROFILES> Number of profiles <1-255>
// <i> Default: 5
// <i> How many profiles do you wish to support in a test meter
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_PROFILES   5

// <o EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS> Max Sampling sessions. <0-10>
// <i> Default: 5
// <i> Maximum number of sampling sessions.
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLING_SESSIONS   5

// <o EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLES_PER_SESSION> Max Samples per session <1-65535>
// <i> Default: 5
// <i> Maximum number of samples per session.
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_MAX_SAMPLES_PER_SESSION   5

// </h>

// <<< end of configuration section >>>
