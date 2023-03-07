/***************************************************************************//**
 * @brief Zigbee IAS Zone Server component configuration header.
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

// <h>Zigbee IAS Zone Server configuration

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ZONE_TYPE> Zone Type
// <EMBER_ZCL_IAS_ZONE_TYPE_STANDARD_CIE=> Standard Cie
// <EMBER_ZCL_IAS_ZONE_TYPE_MOTION_SENSOR=> Motion Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_CONTACT_SWITCH=> Contact Switch
// <EMBER_ZCL_IAS_ZONE_TYPE_FIRE_SENSOR=> Fire Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_WATER_SENSOR=> Water Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_GAS_SENSOR=> Gas Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_PERSONAL_EMERGENCY_DEVICE=> Personal Emergency Device
// <EMBER_ZCL_IAS_ZONE_TYPE_VIBRATION_MOVEMENT_SENSOR=> Vibration Movement Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_REMOTE_CONTROL=> Remote Control
// <EMBER_ZCL_IAS_ZONE_TYPE_KEY_FOB=> Key Fob
// <EMBER_ZCL_IAS_ZONE_TYPE_KEYPAD=> Keypad
// <EMBER_ZCL_IAS_ZONE_TYPE_STANDARD_WARNING_DEVICE=> Standard Warning Device
// <EMBER_ZCL_IAS_ZONE_TYPE_GLASS_BREAK_SENSOR=> Glass Break Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_CARBON_MONOXIDE_SENSOR=> Carbon Monoxide Sensor
// <EMBER_ZCL_IAS_ZONE_TYPE_SECURITY_REPEATER=> Security Repeater
// <EMBER_ZCL_IAS_ZONE_TYPE_INVALID_ZONE_TYPE=> Invalid Zone Type
// <i> Default: EMBER_ZCL_IAS_ZONE_TYPE_KEYPAD
// <i> The configured zone type.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ZONE_TYPE   EMBER_ZCL_IAS_ZONE_TYPE_KEYPAD

// <q EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ENABLE_QUEUE> Queue Failed Transactions
// <i> Default: FALSE
// <i> Enabling this option will cause IAS events to be queued whenever the device is on the network and unable to communicate with the IAS Zone Client.  It will empty the contents of its queue by transmitting a zone status change command for each entry in the queue when it reestablishes communication with the IAS Client.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_ENABLE_QUEUE   0

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_QUEUE_DEPTH> State Change Queue Depth <1-128>
// <i> Default: 10
// <i> When transaction queueing is enabled, this option controls the number of entries in the transaction queue.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_QUEUE_DEPTH   10

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MIN_OTA_TIME_MS> Minimum OTA Time <250-3000>
// <i> Default: 500
// <i> When transaction queueing is enabled, this option controls the minimum OTA time in milliseconds. This is to limit the immediate retries to cause an activity storm.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MIN_OTA_TIME_MS   500

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC> First Backoff Time <1-255>
// <i> Default: 3
// <i> When transaction queueing is enabled, this option controls the first backoff time in seconds.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_FIRST_BACKOFF_TIME_SEC   3

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_BACKOFF_SEQUENCE_COMMON_RATIO> Backoff Sequence Common Ratio <1-255>
// <i> Default: 2
// <i> When transaction queueing is enabled, this option controls the common ratio (ie. multiplier) of the backoff sequence.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_BACKOFF_SEQUENCE_COMMON_RATIO   2

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_BACKOFF_TIME_SEC> Maximum Backoff Time <1-86400>
// <i> Default: 12
// <i> When transaction queueing is enabled, this option controls the maximum of the backoff time in seconds.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_BACKOFF_TIME_SEC   12

// <q EMBER_AF_PLUGIN_IAS_ZONE_SERVER_UNLIMITED_RETRIES> Unlimite Retries
// <i> Default: TRUE
// <i> When transaction queueing is enabled, this option controls unlimited retries.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_UNLIMITED_RETRIES   1

// <o EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_RETRY_ATTEMPTS> Maximum Number of Retry Attempts <1-254>
// <i> Default: 100
// <i> When transaction queueing is enabled, this option controls the maximum number of retry attempts. Note, this is discarded if the Unlimited Retries option is checked.
#define EMBER_AF_PLUGIN_IAS_ZONE_SERVER_MAX_RETRY_ATTEMPTS   100

// </h>

// <<< end of configuration section >>>
