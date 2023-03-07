/***************************************************************************//**
 * @file
 * @brief APIs and defines for the OTA Server plugin.
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

#ifndef ZIGBEE_OTA_SERVER_H
#define ZIGBEE_OTA_SERVER_H

#ifdef UC_BUILD
#include "ota-server-config.h"
#if (EMBER_AF_PLUGIN_OTA_SERVER_PAGE_REQUEST_SUPPORT == 1)
#define PAGE_REQUEST_SUPPORT
#endif
#if (EMBER_AF_PLUGIN_OTA_SERVER_MIN_BLOCK_REQUEST_SUPPORT == 1)
#define MIN_BLOCK_REQUEST_SUPPORT
#endif
#if (EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT == 1)
#define DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_OTA_SERVER_PAGE_REQUEST_SUPPORT
#define PAGE_REQUEST_SUPPORT
#endif
#ifdef EMBER_AF_PLUGIN_OTA_SERVER_MIN_BLOCK_REQUEST_SUPPORT
#define MIN_BLOCK_REQUEST_SUPPORT
#endif
#ifdef EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
#define DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
#endif
#endif // UC_BUILD

/**
 * @defgroup ota-server OTA Server
 * @ingroup component cluster
 * @brief API and Callbacks for the OTA Cluster Server Component
 *
 * Silicon Labs implementation of the Zigbee Over-the-air Bootload Server
 * Cluster (a multi-hop, application bootloader). This
 * implementation serves up a file from an OTA storage
 * device and sends data to clients. It also controls
 * when they can upgrade to the downloaded file.
 *
 */

/**
 * @addtogroup ota-server
 * @{
 */

#include "app/framework/include/af.h"

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup ota_server_cb OTA Server
 * @ingroup af_callback
 * @brief Callbacks for OTA Server Component
 *
 */

/**
 * @addtogroup ota_server_cb
 * @{
 */

// This will eventually be moved into a plugin-specific callbacks file.
void emberAfOtaServerSendUpgradeCommandCallback(EmberNodeId dest,
                                                uint8_t endpoint,
                                                const EmberAfOtaImageId* id);

/** @brief Call when a block is sent to a device.
 *
 * This function will be called when a block is sent to a device.
 *
 * @param actualLength  The block length.
 * @param manufacturerId  The manufacturer ID.
 * @param imageTypeId  The image Type ID.
 * @param firmwareVersion  Firmware Version.
 */
void emberAfPluginOtaServerBlockSentCallback(uint8_t actualLength,
                                             uint16_t manufacturerId,
                                             uint16_t imageTypeId,
                                             uint32_t firmwareVersion);

/** @brief Call when an update has started.
 *.
 *
 * This function will be called when an update has started.
 *
 * @param manufacturerId  The manufacturer ID.
 * @param imageTypeId  The image Type ID.
 * @param firmwareVersion  Firmware Version.
 * @param maxDataSize  Maximum data size.
 * @param offset  Offset.
 */
void emberAfPluginOtaServerUpdateStartedCallback(uint16_t manufacturerId,
                                                 uint16_t imageTypeId,
                                                 uint32_t firmwareVersion,
                                                 uint8_t maxDataSize,
                                                 uint32_t offset);

/** @brief Call when an OTA update has finished.
 *
 * This function will be called when an OTA update has finished.
 *
 * @param manufacturerId  The manufacturer ID.
 * @param imageTypeId  The image Type ID.
 * @param firmwareVersion  Firmware Version.
 * @param source  The source node ID.
 * @param status  An update status.
 */
void emberAfPluginOtaServerUpdateCompleteCallback(uint16_t manufacturerId,
                                                  uint16_t imageTypeId,
                                                  uint32_t firmwareVersion,
                                                  EmberNodeId source,
                                                  uint8_t status);

/** @} */ // end of ota_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of ota-server

#if defined(EMBER_TEST) && !defined(EM_AF_TEST_HARNESS_CODE)
  #define EM_AF_TEST_HARNESS_CODE
#endif

uint8_t emAfOtaServerGetBlockSize(void);
uint8_t emAfOtaImageBlockRequestHandler(EmberAfImageBlockRequestCallbackStruct* callbackData);

bool emAfOtaPageRequestErrorHandler(void);

void emAfOtaPageRequestTick(uint8_t endpoint);

// Returns the status code to the request.
uint8_t emAfOtaPageRequestHandler(uint8_t clientEndpoint,
                                  uint8_t serverEndpoint,
                                  const EmberAfOtaImageId* id,
                                  uint32_t offset,
                                  uint8_t maxDataSize,
                                  uint16_t pageSize,
                                  uint16_t responseSpacing);

bool emAfOtaServerHandlingPageRequest(void);

// This used to be static in ota-server.c as prepareResponse,
// made global because SE 1.4 requires a special treatment for OTA requests.
void emAfOtaServerPrepareResponse(bool useDefaultResponse,
                                  uint8_t commandId,
                                  uint8_t status,
                                  uint8_t defaultResponsePayloadCommandId);
#endif // ZIGBEE_OTA_SERVER_H
