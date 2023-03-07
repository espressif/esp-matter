/***************************************************************************//**
 * @brief Zigbee OTA Bootload Cluster Client component configuration header.
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

// <h>Zigbee OTA Bootload Cluster Client configuration

// <q EMBER_AF_PLUGIN_OTA_CLIENT_AUTO_START> Auto Start on Stack Up (random delay)
// <i> Default: TRUE
// <i> Automatically start the OTA client on EMBER_NETWORK_UP.  This will include a random delay up to 5 minutes to prevent an issue where multiple devices in the network all query the server simultaneously after a power-outage.
#define EMBER_AF_PLUGIN_OTA_CLIENT_AUTO_START   1

// <o EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_DELAY_MINUTES> Query OTA Server Delay (minutes) <1-65535>
// <i> Default: 5
// <i> How often the client queries the OTA server for a new upgrade image.
#define EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_DELAY_MINUTES   5

// <o EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_ERROR_THRESHOLD> Query Error Rediscovery Threshold <1-255>
// <i> Default: 10
// <i> How many sequential query errors will cause a device to look for a new OTA server.
#define EMBER_AF_PLUGIN_OTA_CLIENT_QUERY_ERROR_THRESHOLD   10

// <o EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_DELAY_MS> Download Delay (ms) <0-65535>
// <i> Default: 0
// <i> How often a new block of data (or page) is requested during a download by the client.  A value of 0 means the client will request the blocks (or pages) as fast as the server responds.
#define EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_DELAY_MS   0

// <o EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_ERROR_THRESHOLD> Download Error Threshold <1-255>
// <i> Default: 10
// <i> How many sequential errors will cause a download to be aborted.
#define EMBER_AF_PLUGIN_OTA_CLIENT_DOWNLOAD_ERROR_THRESHOLD   10

// <o EMBER_AF_PLUGIN_OTA_CLIENT_UPGRADE_WAIT_THRESHOLD> Upgrade Wait Threshold <1-255>
// <i> Default: 10
// <i> How many sequential, missed responses to an upgrade end request will cause a download to be applied anyway.
#define EMBER_AF_PLUGIN_OTA_CLIENT_UPGRADE_WAIT_THRESHOLD   10

// <o EMBER_AF_PLUGIN_OTA_CLIENT_SERVER_DISCOVERY_DELAY_MINUTES> Server Discovery Delay (minutes) <1-65535>
// <i> Default: 10
// <i> How often a client will look for an OTA server in the network when it did not successfully discover one.  Once a client discovers the server, it will remember that server until it reboots, or until the Query Error Rediscovery Threshold is hit.
#define EMBER_AF_PLUGIN_OTA_CLIENT_SERVER_DISCOVERY_DELAY_MINUTES   10

// <o EMBER_AF_PLUGIN_OTA_CLIENT_RUN_UPGRADE_REQUEST_DELAY_MINUTES> Run Upgrade Request Delay (minutes) <>
// <i> Default: 10
// <i> How often the client will ask the server to apply a previously downloaded upgrade when the server has previously told the client to wait.
#define EMBER_AF_PLUGIN_OTA_CLIENT_RUN_UPGRADE_REQUEST_DELAY_MINUTES   10

// <q EMBER_AF_PLUGIN_OTA_CLIENT_USE_PAGE_REQUEST> Use Page Request
// <i> Default: FALSE
// <i> This causes the device to use an OTA Page Request command to ask for a large block of data all at once, rather than use individual image block requests for each block.
#define EMBER_AF_PLUGIN_OTA_CLIENT_USE_PAGE_REQUEST   0

// <o EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_SIZE> Page Request Size
// <i> Page Request Size
// <1024=> 1024
// <2048=> 2048
// <4096=> 4096
// <i> Default: 1024
// <i> The size of the page to request from the server.
#define EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_SIZE   1024

// <o EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_TIMEOUT_SECONDS> Page Request Timeout (seconds) <1-10>
// <i> Default: 5
// <i> The length of time to wait for all blocks from a page request to come in.  After this time has expired, missed packets will be requested individually with image block requests.
#define EMBER_AF_PLUGIN_OTA_CLIENT_PAGE_REQUEST_TIMEOUT_SECONDS   5

// <o EMBER_AF_PLUGIN_OTA_CLIENT_VERIFY_DELAY_MS> Verification Delay (ms) <0-255>
// <i> Default: 10
// <i> This controls how often an ongoing verification process executes.  When Signature verification is enabled this will control how often digest calculation is executed.  Digest calculation can take quite a long time for an OTA image.  Other processing for the system may be deemeded more important and therefore we add delays between calculations.  This also controls how often custom verification written by the application developer is executed.  A value of 0 means the calculations run to completion.
#define EMBER_AF_PLUGIN_OTA_CLIENT_VERIFY_DELAY_MS   10

// <o EMBER_AF_PLUGIN_OTA_CLIENT_MIN_BLOCK_PERIOD_UNITS> Minimum Block Period Units
// <i> Minimum Block Period Units
// <SECONDS=> Seconds
// <MILLISECONDS=> Milliseconds
// <DISABLE_FEATURE=> Disable Feature
// <i> Default: MILLISECONDS
// <i> The units applied to the Minimum Block Period field in an ImageBlockRequest and ImageBlockResponse. Optionally, the feature can be disabled.
#define EMBER_AF_PLUGIN_OTA_CLIENT_MIN_BLOCK_PERIOD_UNITS   MILLISECONDS

// <q EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_LOOPBACK_SERVER> Ignore loopback server endpoints
// <i> Default: TRUE
// <i> During OTA server discovery, ignore service discovery responses that come as loopback messages (from the local node) so the first remote server to respond is the one the client will use.  This allows a combination OTA client/server device to find other servers.
#define EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_LOOPBACK_SERVER   1

// <q EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_NON_TRUST_CENTER> Ignore non Trust Center for OTA server
// <i> Default: FALSE
// <i> Ignore non trust center
#define EMBER_AF_PLUGIN_OTA_CLIENT_IGNORE_NON_TRUST_CENTER   0

//  <a.8 EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI0> OTA_CLIENT_SIGNER_EUI0 <0..255> <f.h>
//  <d> { 0x00, 0x0D, 0x6F, 0x00, 0x00, 0x19, 0x8B, 0x36 }
#define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI0 { 0x00, 0x0D, 0x6F, 0x00, 0x00, 0x19, 0x8B, 0x36 }

//  <a.8 EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI1> OTA_CLIENT_SIGNER_EUI1 <0..255> <f.h>
//  <d> {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI1 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

//  <a.8 EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI2> OTA_CLIENT_SIGNER_EUI2 <0..255> <f.h>
//  <d> {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define EMBER_AF_PLUGIN_OTA_CLIENT_SIGNER_EUI2 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// </h>

// <<< end of configuration section >>>
