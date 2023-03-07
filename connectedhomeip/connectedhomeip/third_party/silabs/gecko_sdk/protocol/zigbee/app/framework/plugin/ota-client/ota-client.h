/***************************************************************************//**
 * @file
 * @brief API for OTA Clients
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

#ifdef UC_BUILD
#include "ota-client-config.h"
#include "sl_component_catalog.h"
#if (EMBER_AF_PLUGIN_OTA_CLIENT_AUTO_START == 1)
#define AUTO_START
#endif
#if (EMBER_AF_PLUGIN_OTA_CLIENT_USE_PAGE_REQUEST == 1)
#define USE_PAGE_REQUEST
#endif
#if (EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_LOOPBACK_SERVER == 1)
#define IGNORE_LOOPBACK_SERVER
#endif
#if (EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_NON_TRUST_CENTER == 1)
#define IGNORE_NON_TRUST_CENTER
#endif
#ifdef SL_CATALOG_ZIGBEE_OTA_CLIENT_SIGNATURE_VERIFY_SUPPORT_PRESENT
#define SIGNATURE_VERIFICATION_SUPPORT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_AUTO_START
#define AUTO_START
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_USE_PAGE_REQUEST
#define USE_PAGE_REQUEST
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_LOOPBACK_SERVER
#define IGNORE_LOOPBACK_SERVER
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_NON_TRUST_CENTER
#define IGNORE_NON_TRUST_CENTER
#endif
#if defined(EMBER_AF_PLUGIN_OTA_CLIENT_SIGNATURE_VERIFICATION_SUPPORT) \
  || defined(SL_CATALOG_ZIGBEE_OTA_CLIENT_SIGNATURE_VERIFY_SUPPORT_PRESENT)
#define SIGNATURE_VERIFICATION_SUPPORT
#endif
#endif // UC_BUILD

/**
 * @defgroup ota-client OTA Client
 * @ingroup component cluster
 * @brief API and Callbacks for the OTA Cluster Client Component
 *
 * Silicon Labs implementation of the Zigbee Over-the-air Bootload Client Cluster
 * (a multi-hop, application bootloader). This implementation finds the OTA
 * server in the network, periodically queries the server for a new image to
 * download, downloads the data, and then waits for the server command to
 * tell it to upgrade. Optionally, it can cryptographically verify the
 * image before upgrade.
 *
 */

/**
 * @addtogroup ota-client
 * @{
 */

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_DELAY_MS)
// How often the client will ask for a piece of an upgrade image being
// actively downloaded.  A rate of 0 means the client will pull down the data
// as fast as it can.
  #define EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_DELAY_MS  0L
#endif

// How often the OTA client looks for an OTA server when there is NOT
// one present in the network.  Once it has found one, it queries the
// same one forever (or until it reboots).
#if defined(EMBER_AF_PLUGIN_OTA_CLIENT_SERVER_DISCOVERY_DELAY_MINUTES)
  #define EMBER_AF_OTA_SERVER_DISCOVERY_DELAY_MS \
  (EMBER_AF_PLUGIN_OTA_CLIENT_SERVER_DISCOVERY_DELAY_MINUTES * MINUTES_IN_MS)
#else
  #define EMBER_AF_OTA_SERVER_DISCOVERY_DELAY_MS     (2 * MINUTES_IN_MS)
#endif

// How often the OTA client asks the OTA server if there is a new image
// available
#if defined(EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_DELAY_MINUTES)
  #define EMBER_AF_OTA_QUERY_DELAY_MS \
  (EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_DELAY_MINUTES * MINUTES_IN_MS)
#else
  #define EMBER_AF_OTA_QUERY_DELAY_MS (5 * MINUTES_IN_MS)
#endif

// The number of query errors before re-discovery of an OTA
// server is discovered.
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_ERROR_THRESHOLD)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_ERROR_THRESHOLD 10
#endif

// The maximum number of sequential errors when downloading that will trigger
// the OTA client to abort the download.
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_ERROR_THRESHOLD)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_ERROR_THRESHOLD  10
#endif

// The delay between attempts to request to initiate the bootload
// of a successfully downloaded file.
#if defined(EMBER_AF_PLUGIN_OTA_CLIENT_RUN_UPGRADE_REQUEST_DELAY_MINUTES)
  #define EMBER_AF_RUN_UPGRADE_REQUEST_DELAY_MS \
  (EMBER_AF_PLUGIN_OTA_CLIENT_RUN_UPGRADE_REQUEST_DELAY_MINUTES * MINUTES_IN_MS)
#else
  #define EMBER_AF_RUN_UPGRADE_REQUEST_DELAY_MS (10 * MINUTES_IN_MS)
#endif

// The maximum number of sequential errors when asking the OTA Server when to
// upgrade that will cause the OTA client to apply the upgrade without the
// server telling it to do so.
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_UPGRADE_WAIT_THRESHOLD)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_UPGRADE_WAIT_THRESHOLD  10
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_SIZE)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_SIZE 1024
#endif

// The spacing requested by the client between the image blocks sent by the
// server to the client during a page request.
#if !defined(EMBER_AF_OTA_CLIENT_PAGE_REQUEST_SPACING_MS)
  #define EMBER_AF_OTA_CLIENT_PAGE_REQUEST_SPACING_MS 50L
#endif

// This is the time delay between calls to emAfOtaImageDownloadVerify().
// Verification can take a while (especially in the case of signature
// checking for Smart Energy) so this provides the ability for other
// parts of the system to run.
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_VERIFY_DELAY_MS)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_VERIFY_DELAY_MS    10L
#endif

#define NULL_EUI64 { 0, 0, 0, 0, 0, 0, 0, 0 }

// A NULL eui64 is an invalid signer.  It will never match.
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI0)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI0 NULL_EUI64
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI1)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI1 NULL_EUI64
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI2)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI2 NULL_EUI64
#endif

#define SECONDS_IN_MS (1000L)
#define MINUTES_IN_MS (60 * SECONDS_IN_MS)
#define HOURS_IN_MS (60 * MINUTES_IN_MS)

// By default if hardware version is not defined, it is not used.
// Most products do not limit upgrade images based on the version.
// Instead they have different images for different hardware.  However
// this can provide support for an image that only supports certain hardware
// revision numbers.
#define EMBER_AF_INVALID_HARDWARE_VERSION 0xFFFF

// TODO: ungate this when the facilities are in place to do so
// for generated CLI
//#if defined(EMBER_TEST)
extern uint8_t emAfOtaClientStopDownloadPercentage;
//#endif

/**
 * @name API
 * @{
 */

void emberAfOtaServerSendUpgradeRequest();

/**
 * @brief Set whether OTA clients use non TC servers.
 *
 * Sets the behavior of OTA clients when determining whether or not
 * to use non Trust Center OTA servers and abort any download that is in progress.
 *
 * @param IgnoreNonTc a bool determining whether client should ignore any
 *        non Trust Center OTA servers
 */
void emberAfPluginOtaClientSetIgnoreNonTrustCenter(bool ignoreNonTc);

/**
 * @brief Return the current value of ignoreNonTrustCenter for OTA clients.
 */
bool emberAfPluginOtaClientGetIgnoreNonTrustCenter();

/**
 * @brief Set whether OTA Clients will permit firmware downgrades.
 *
 * @param isDisabled A bool value indicating whether or not downgrades
 *        will be disabled
 */
void emberAfPluginSetDisableOtaDowngrades(bool isDisabled);

/**
 * @brief Return the current value of disableOtaDowngrades.
 */
bool emberAfPluginGetDisableOtaDowngrades();

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup ota_client_cb OTA Client
 * @ingroup af_callback
 * @brief Callbacks for OTA Client Component
 *
 */

/**
 * @addtogroup ota_client_cb
 * @{
 */

/** @brief Pre-Bootload Callback
 *
 * Called shortly before installing the downloaded image.
 *
 * @param srcEndpoint     Ver.: always
 * @param serverEndpoint  Ver.: always
 * @param serverNodeId    Ver.: always
 */
void emberAfPluginOtaClientPreBootloadCallback(uint8_t srcEndpoint,
                                               uint8_t serverEndpoint,
                                               EmberNodeId serverNodeId);

/** @} */ // end of ota_client_cb
/** @} */ // end of name Callbacks
/** @} */ // end of ota-client

void emAfOtaClientStop(void);
void emAfOtaClientPrintState(void);
void emAfSendImageBlockRequestTest(void);

void emAfSetPageRequest(bool pageRequest);
bool emAfUsingPageRequest(void);

void emAfOtaBootloadClusterClientResumeAfterErase(bool success);
