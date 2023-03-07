/***************************************************************************//**
 * @brief Zigbee Interpan component configuration header.
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

// <h>Zigbee Interpan configuration

// <q EMBER_AF_PLUGIN_INTERPAN_ALLOW_REQUIRED_SMART_ENERGY_MESSAGES> Allow required Smart Energy interpan request messages
// <i> Default: TRUE
// <i> Modifies the interpan filters to accept the required interpan Smart Energy messages: Get Last Message, Get Current Price, and Get Scheduled Price.
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_REQUIRED_SMART_ENERGY_MESSAGES   1

// <q EMBER_AF_PLUGIN_INTERPAN_ALLOW_SMART_ENERGY_RESPONSE_MESSAGES> Allow Smart Energy interpan response messages.
// <i> Default: FALSE
// <i> Allows the Publish Price and Display Message commands to be accepted over interpan.  This is NOT required for Smart Energy certification.
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_SMART_ENERGY_RESPONSE_MESSAGES   0

// <q EMBER_AF_PLUGIN_INTERPAN_ALLOW_KEY_ESTABLISHMENT> Allow Key Establishment protocol over interpan (non-compliant)
// <i> Default: FALSE
// <i> Adds an interpan policy to accept key establishment messages received over interpan.  This feature is not ZigBee compliant.
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_KEY_ESTABLISHMENT   0

// <q EMBER_AF_PLUGIN_INTERPAN_ALLOW_APS_ENCRYPTED_MESSAGES> Allow any APS encrypted interpan message (non-compliant)
// <i> Default: FALSE
// <i> This option will permit the reception of any interpan message that successfully decrypts using a link key.  This bypasses the normal filters for non-encrypted (i.e. normal) interpan messages.  Only device specific link keys are allowed.  The global link key is not allowed.  This feature is not ZigBee compliant.
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_APS_ENCRYPTED_MESSAGES   0

// <o EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO> Deliver Interpan Messages to
// <i> Deliver Interpan Messages to
// <PRIMARY_ENDPOINT=> Primary Endpoint
// <SPECIFIED_ENDPOINT=> Specified Endpoint
// <ALL_ENDPOINTS=> All Endpoints
// <i> Default: PRIMARY_ENDPOINT
// <i> This controls where interpan messages are delivered to, since the interpan message format has no APS endpoint information.
#define EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO   PRIMARY_ENDPOINT

// <o EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO_SPECIFIED_ENDPOINT_VALUE> Deliver to Specified Endpoint <1-254>
// <i> Default: 1
// <i> When the option above is selected to deliver to a 'Specified Endpoint', this controls what endpoint the interpan messages are delivered to.
#define EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO_SPECIFIED_ENDPOINT_VALUE   1

// <q EMBER_AF_PLUGIN_INTERPAN_ALLOW_FRAGMENTATION> Allow interpan fragmentation (IPMFs) (non-compliant)
// <i> Default: FALSE
// <i> This option will permit the transmission and reception of interpan fragments.
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_FRAGMENTATION   0

// </h>

// <<< end of configuration section >>>
