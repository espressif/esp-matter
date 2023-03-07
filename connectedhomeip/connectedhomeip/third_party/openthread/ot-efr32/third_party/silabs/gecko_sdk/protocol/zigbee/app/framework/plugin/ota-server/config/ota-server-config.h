/***************************************************************************//**
 * @brief Zigbee OTA Bootload Cluster Server component configuration header.
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

// <h>Zigbee OTA Bootload Cluster Server configuration

// <q EMBER_AF_PLUGIN_OTA_SERVER_PAGE_REQUEST_SUPPORT> Page Request Support
// <i> Default: FALSE
// <i> Whether the server supports clients making an OTA page request.
#define EMBER_AF_PLUGIN_OTA_SERVER_PAGE_REQUEST_SUPPORT   0

// <q EMBER_AF_PLUGIN_OTA_SERVER_MIN_BLOCK_REQUEST_SUPPORT> Minimum Block Request Support
// <i> Default: FALSE
// <i> Whether the server supports the Minimum Block Period support field in the Image Block Request/Response messages. This is used to rate limit clients.
#define EMBER_AF_PLUGIN_OTA_SERVER_MIN_BLOCK_REQUEST_SUPPORT   0

// <q EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT> Dynamic Block Request Support
// <i> Default: FALSE
// <i> If the Minimum Block Request Support is enabled, this additional support provides dynamically treating the Minimum Block Period field as milliseconds or seconds, depending on the OTA client's support. When a client starts an OTA transfer, it will be tested to determine whether it treats the Minimum Block Period as seconds or milliseconds, and the determination will be used throughout the OTA transfer.
#define EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT   0

// <o EMBER_AF_PLUGIN_OTA_SERVER_MAX_NUM_DYNAMIC_OTA_DOWNLOADS> Number of Simultaneous Dynamic Block Period Downloads <1-50>
// <i> Default: 5
// <i> If the Dynamic Minimum Block Request Support is enabled, this option's value dictates how many simultaneous OTA client transfers are supported at one time. When at full capacity, Query Next Image Requests will be responded to, but Image Block Requests and Page Requests will be told to delay until a free connection opens up, which will occur when a device concludes the OTA process. A device will be aged out as an active connection if it does not query a piece of the OTA file at least once every 'Dynamic Block Period Download Timeout' seconds.
#define EMBER_AF_PLUGIN_OTA_SERVER_MAX_NUM_DYNAMIC_OTA_DOWNLOADS   5

// <o EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_CLIENT_TIMEOUT_SEC> Dynamic Block Period Download Timeout <60-3600>
// <i> Default: 120
// <i> This option's value dictates the amount of inactivity in seconds before the server purges a client from the dynamic block period downloads. This value must be greater than the 'Test Block Period Value' below, otherwise a client who resumes querying after a test delay will be immediately purged.
#define EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_CLIENT_TIMEOUT_SEC   120

// <o EMBER_AF_PLUGIN_OTA_SERVER_TEST_BLOCK_PERIOD_VALUE> Test Block Period Value <30-3600>
// <i> Default: 60
// <i> The value a client is told to delay when determining whether it treats the Minimum Block Period as seconds or milliseconds. When a client begins an OTA download, it is sent a Minimum Block Period with this value. The amount of time that the client delays determines whether it treats this value as seconds or milliseconds (the client's delay time is judged against a threshold of this configured value). It is recommended that this value not be less than the longest expected delay time in the network, as sleepy end devices may delay longer than the Minimum Block Period amount, which tricks the OTA server into thinking that the client treats this value as seconds. Once the client is judged to be using seconds or milliseconds, the Minimum Block Period will revert to being the value contained in the otaMinimumBlockPeriodMs variable, which is settable with "plugin ota-server policy image-req-min-period."
#define EMBER_AF_PLUGIN_OTA_SERVER_TEST_BLOCK_PERIOD_VALUE   60

// </h>

// <<< end of configuration section >>>
