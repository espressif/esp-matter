/***************************************************************************//**
 * @file
 * @brief StandardizedRfTesting configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef STANDARDIZEDRFTESTING_CONFIG_H
#define STANDARDIZEDRFTESTING_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_SRFT_TX_POWER_CHANNEL_11> The TX power level for channel 11 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_11    3

// <o SL_SRFT_TX_POWER_CHANNEL_12> The TX power level for channel 12 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_12    3

// <o SL_SRFT_TX_POWER_CHANNEL_13> The TX power level for channel 13 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_13    3

// <o SL_SRFT_TX_POWER_CHANNEL_14> The TX power level for channel 14 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_14    3

// <o SL_SRFT_TX_POWER_CHANNEL_15> The TX power level for channel 15 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_15    3

// <o SL_SRFT_TX_POWER_CHANNEL_16> The TX power level for channel 16 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_16    3

// <o SL_SRFT_TX_POWER_CHANNEL_17> The TX power level for channel 17 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_17    3

// <o SL_SRFT_TX_POWER_CHANNEL_18> The TX power level for channel 18 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_18    3

// <o SL_SRFT_TX_POWER_CHANNEL_19> The TX power level for channel 19 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_19    3

// <o SL_SRFT_TX_POWER_CHANNEL_20> The TX power level for channel 20 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_20    3

// <o SL_SRFT_TX_POWER_CHANNEL_21> The TX power level for channel 21 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_21    3

// <o SL_SRFT_TX_POWER_CHANNEL_22> The TX power level for channel 22 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_22    3

// <o SL_SRFT_TX_POWER_CHANNEL_23> The TX power level for channel 23 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_23    3

// <o SL_SRFT_TX_POWER_CHANNEL_24> The TX power level for channel 24 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_24    3

// <o SL_SRFT_TX_POWER_CHANNEL_25> The TX power level for channel 25 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_25    3

// <o SL_SRFT_TX_POWER_CHANNEL_26> The TX power level for channel 26 <-8..20>
// <i> Default: 3
#define SL_SRFT_TX_POWER_CHANNEL_26    3

// <o SL_SRFT_DEFAULT_CHANNEL> The default channel <11..26>
// <i> Default: 11
#define SL_SRFT_DEFAULT_CHANNEL        11

// <o SL_SRFT_APP_VERSION_MAJOR> Major version of the application <0..255>
// <i> Default: 4
#define SL_SRFT_APP_VERSION_MAJOR      4

// <o SL_SRFT_APP_VERSION_MINOR> Minor version of the application <0..255>
// <i> Default: 8
#define SL_SRFT_APP_VERSION_MINOR      4

// <o SL_SRFT_APP_VERSION_MINOR> Minor version of the application <0..255>
// <i> Default: 8
#define SL_SRFT_APP_VERSION_MINOR      4

// <o SL_SRFT_DEVICE_HW_VERSION> Device hardware version <0..255>
// <i> Default: 255
#define SL_SRFT_DEVICE_HW_VERSION      255

// <o SL_SRFT_DEVICE_SW_VERSION> Device software version <0..255>
// <i> Default: 255
#define SL_SRFT_DEVICE_SW_VERSION      255

// <o SL_SRFT_TX_POWER_MODE> The Tx power mode
// <EMBER_TX_POWER_MODE_DEFAULT=> Standard
// <EMBER_TX_POWER_MODE_BOOST=> Boost
// <EMBER_TX_POWER_MODE_ALTERNATE=> Alternate
// <EMBER_TX_POWER_MODE_BOOST_AND_ALTERNATE=> Boost and Alternate
// <EMBER_TX_POWER_MODE_USE_TOKEN=> Token
// <i> Default: EMBER_TX_POWER_MODE_USE_TOKEN
#define SL_SRFT_TX_POWER_MODE          EMBER_TX_POWER_MODE_USE_TOKEN

// <o SL_SRFT_HIGH_CHANNEL> The highest channel to use <11..26>
// <i> Default: 26
#define SL_SRFT_HIGH_CHANNEL           26

// <o SL_SRFT_LOW_CHANNEL> The lowest channel to use <11..26>
// <i> Default: 11
#define SL_SRFT_LOW_CHANNEL            11

// <o SL_SRFT_PING_TIMEOUT_MS> The default ping timeout in ms <1..65535>
// <i> Default: 500
#define SL_SRFT_PING_TIMEOUT_MS        500

// <<< end of configuration section >>>

#endif  // STANDARDIZEDRFTESTING_CONFIG_H
