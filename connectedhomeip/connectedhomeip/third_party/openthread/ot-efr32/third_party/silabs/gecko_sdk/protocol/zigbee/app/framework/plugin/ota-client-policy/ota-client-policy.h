/***************************************************************************//**
 * @file
 * @brief Config for Zigbee Over-the-air bootload cluster for upgrading firmware and
 * downloading device specific file.
 *
 * This file defines the interface for the customer's application to
 * control the behavior of the OTA client.
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

#include "app/framework/plugin/ota-client/ota-client.h"

#ifdef UC_BUILD
#include "ota-client-policy-config.h"
#endif // UC_BUILD

// Note: EMBER_AF_MANUFACTURER_CODE defined in client's config

/**
 * @defgroup ota-client-policy  OTA Client Policy
 * @ingroup component
 * @brief API and Callbacks for the OTA Client Policy Component
 *
 * This sample implementation shows how to define the policies of the
 * Zigbee Over-the-air bootload cluster client (a multi-hop, application bootloader).
 * It allows the implementor to decide which manufacturer ID, image type ID,
 * and file version is used when querying the server.
 * It also defines callbacks, such as download
 * complete and ready to bootload.
 *
 */

/**
 * @addtogroup ota-client-policy
 * @{
 */

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID    0x5678
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION 0x00000005L
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_HARDWARE_VERSION)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_HARDWARE_VERSION EMBER_AF_INVALID_HARDWARE_VERSION
#endif

/** @} */ // end of ota-client-policy
