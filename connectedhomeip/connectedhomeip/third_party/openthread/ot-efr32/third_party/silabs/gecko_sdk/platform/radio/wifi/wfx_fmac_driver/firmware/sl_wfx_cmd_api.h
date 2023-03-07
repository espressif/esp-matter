/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef _SL_WFX_CMD_API_H_
#define _SL_WFX_CMD_API_H_

#include "sl_wfx_general_api.h"

/**
 * @addtogroup FULL_MAC_API
 * @brief Wireless LAN Full MAC (WFM) API
 *
 * This module describes the functionality and the API messages of the Upper MAC (UMAC)
 * component of the device. UMAC provides the Wireless LAN Full MAC (WFM) API. WFM API
 * is used with the general API messages described in @ref GENERAL_API.
 *
 * The device is capable of operating in different roles such as a Wi-Fi client and
 * as a Wi-Fi Access Point. To use multiple roles at
 * the same time, the device is split into logical units called interfaces.
 *
 * Two interfaces are available for WFM API. They are referenced with interface
 * IDs 0 and 1. For general information on how to format the API messages, see
 * @ref MESSAGE_CONSTRUCTION.
 *
 * For a list of WFM API messages, see @ref WFM_GROUP_MESSAGES for further details.
 * @{
 */

/**
 * @defgroup WFM_GROUP_CONCEPTS API concepts
 * @brief WFM API concepts.
 */

/**
 * @defgroup WFM_GROUP_CONSTANTS API constant values
 * @brief WFM API constant values.
 */

/**
 * @defgroup WFM_GROUP_TYPES API types
 * @brief WFM API types.
 */

/**
 * @defgroup WFM_GROUP_MESSAGE_IDS API message IDs
 * @brief WFM API message IDs.
 */

/**
 * @defgroup WFM_GROUP_MESSAGES API messages
 * @brief WFM API messages.
 */

/**
 * @defgroup WFM_GROUP_MESSAGES_BY_MODE API messages by mode
 * @brief WFM API messages organized by mode.
 * @{
 */

/**
 * @defgroup WFM_GROUP_MODE_IDLE API messages in idle mode
 * @brief WFM API messages applicable for an interface in idle mode.
 */

/**
 * @defgroup WFM_GROUP_MODE_STA API messages in station mode
 * @brief WFM API messages applicable for an interface in STA mode.
 */

/**
 * @defgroup WFM_GROUP_MODE_AP API messages in AP mode
 * @brief WFM API messages applicable for an interface in AP mode.
 */

/**
 * @}
 */

/**
 * @addtogroup WFM_GROUP_MESSAGE_IDS
 * @{
 */

/**
 * @brief WFM API request message IDs.
 */
typedef enum sl_wfx_requests_ids_e {
  SL_WFX_SET_MAC_ADDRESS_REQ_ID                  = 0x42,   ///< \b SET_MAC_ADDRESS request ID uses body SL_WFX_SET_MAC_ADDRESS_REQ_BODY and returns SL_WFX_SET_MAC_ADDRESS_CNF_BODY
  SL_WFX_CONNECT_REQ_ID                          = 0x43,   ///< \b CONNECT request ID uses body SL_WFX_CONNECT_REQ_BODY and returns SL_WFX_CONNECT_CNF_BODY
  SL_WFX_DISCONNECT_REQ_ID                       = 0x44,   ///< \b DISCONNECT request ID uses body SL_WFX_DISCONNECT_REQ_BODY and returns SL_WFX_DISCONNECT_CNF_BODY
  SL_WFX_START_AP_REQ_ID                         = 0x45,   ///< \b START_AP request ID uses body SL_WFX_START_AP_REQ_BODY and returns SL_WFX_START_AP_CNF_BODY
  SL_WFX_UPDATE_AP_REQ_ID                        = 0x46,   ///< \b UPDATE_AP request ID uses body SL_WFX_UPDATE_AP_REQ_BODY and returns SL_WFX_UPDATE_AP_CNF_BODY
  SL_WFX_STOP_AP_REQ_ID                          = 0x47,   ///< \b STOP_AP request ID uses body SL_WFX_STOP_AP_REQ_BODY and returns SL_WFX_STOP_AP_CNF_BODY
  SL_WFX_SEND_FRAME_REQ_ID                       = 0x4a,   ///< \b SEND_FRAME request ID uses body SL_WFX_SEND_FRAME_REQ_BODY and returns SL_WFX_SEND_FRAME_CNF_BODY
  SL_WFX_START_SCAN_REQ_ID                       = 0x4b,   ///< \b START_SCAN request ID uses body SL_WFX_START_SCAN_REQ_BODY and returns SL_WFX_START_SCAN_CNF_BODY
  SL_WFX_STOP_SCAN_REQ_ID                        = 0x4c,   ///< \b STOP_SCAN request ID uses body SL_WFX_STOP_SCAN_REQ_BODY and returns SL_WFX_STOP_SCAN_CNF_BODY
  SL_WFX_GET_SIGNAL_STRENGTH_REQ_ID              = 0x4e,   ///< \b GET_SIGNAL_STRENGTH request ID uses body SL_WFX_GET_SIGNAL_STRENGTH_REQ_BODY and returns SL_WFX_GET_SIGNAL_STRENGTH_CNF_BODY
  SL_WFX_DISCONNECT_AP_CLIENT_REQ_ID             = 0x4f,   ///< \b DISCONNECT_AP_CLIENT request ID uses body SL_WFX_DISCONNECT_AP_CLIENT_REQ_BODY and returns SL_WFX_DISCONNECT_AP_CLIENT_CNF_BODY
  SL_WFX_SET_PM_MODE_REQ_ID                      = 0x52,   ///< \b SET_PM_MODE request ID uses body SL_WFX_SET_PM_MODE_REQ_BODY and returns SL_WFX_SET_PM_MODE_CNF_BODY
  SL_WFX_ADD_MULTICAST_ADDR_REQ_ID               = 0x53,   ///< \b ADD_MULTICAST_ADDR request ID uses body SL_WFX_ADD_MULTICAST_ADDR_REQ_BODY and returns SL_WFX_ADD_MULTICAST_ADDR_CNF_BODY
  SL_WFX_REMOVE_MULTICAST_ADDR_REQ_ID            = 0x54,   ///< \b REMOVE_MULTICAST_ADDR request ID uses body SL_WFX_REMOVE_MULTICAST_ADDR_REQ_BODY and returns SL_WFX_REMOVE_MULTICAST_ADDR_CNF_BODY
  SL_WFX_SET_MAX_AP_CLIENT_COUNT_REQ_ID          = 0x55,   ///< \b SET_MAX_AP_CLIENT_COUNT request ID uses body SL_WFX_SET_MAX_AP_CLIENT_COUNT_REQ_BODY and returns SL_WFX_SET_MAX_AP_CLIENT_COUNT_CNF_BODY
  SL_WFX_SET_MAX_AP_CLIENT_INACTIVITY_REQ_ID     = 0x56,   ///< \b SET_MAX_AP_CLIENT_INACTIVITY request ID uses body SL_WFX_SET_MAX_AP_CLIENT_INACTIVITY_REQ_BODY and returns SL_WFX_SET_MAX_AP_CLIENT_INACTIVITY_CNF_BODY
  SL_WFX_SET_ROAM_PARAMETERS_REQ_ID              = 0x57,   ///< \b SET_ROAM_PARAMETERS request ID uses body SL_WFX_SET_ROAM_PARAMETERS_REQ_BODY and returns SL_WFX_SET_ROAM_PARAMETERS_CNF_BODY
  SL_WFX_SET_TX_RATE_PARAMETERS_REQ_ID           = 0x58,   ///< \b SET_TX_RATE_PARAMETERS request ID uses body SL_WFX_SET_TX_RATE_PARAMETERS_REQ_BODY and returns SL_WFX_SET_TX_RATE_PARAMETERS_CNF_BODY
  SL_WFX_SET_ARP_IP_ADDRESS_REQ_ID               = 0x59,   ///< \b SET_ARP_IP_ADDRESS request ID uses body SL_WFX_SET_ARP_IP_ADDRESS_REQ_BODY and returns SL_WFX_SET_ARP_IP_ADDRESS_CNF_BODY
  SL_WFX_SET_NS_IP_ADDRESS_REQ_ID                = 0x5A,   ///< \b SET_NS_IP_ADDRESS request ID uses body SL_WFX_SET_NS_IP_ADDRESS_REQ_BODY and returns SL_WFX_SET_NS_IP_ADDRESS_CNF_BODY
  SL_WFX_SET_BROADCAST_FILTER_REQ_ID             = 0x5B,   ///< \b SET_BROADCAST_FILTER request ID uses body SL_WFX_SET_BROADCAST_FILTER_REQ_BODY and returns SL_WFX_SET_BROADCAST_FILTER_CNF_BODY
  SL_WFX_SET_SCAN_PARAMETERS_REQ_ID              = 0x5C,   ///< \b SET_SCAN_PARAMETERS request ID uses body SL_WFX_SET_SCAN_PARAMETERS_REQ_BODY and returns SL_WFX_SET_SCAN_PARAMETERS_CNF_BODY
  SL_WFX_SET_UNICAST_FILTER_REQ_ID               = 0x5D,   ///< \b SET_UNICAST_FILTER request ID uses body SL_WFX_SET_UNICAST_FILTER_REQ_BODY and returns SL_WFX_SET_UNICAST_FILTER_CNF_BODY
  SL_WFX_ADD_WHITELIST_ADDR_REQ_ID               = 0x5E,   ///< \b ADD_WHITELIST_ADDR request ID uses body SL_WFX_ADD_WHITELIST_ADDR_REQ_BODY and returns SL_WFX_ADD_WHITELIST_ADDR_CNF_BODY
  SL_WFX_ADD_BLACKLIST_ADDR_REQ_ID               = 0x5F,   ///< \b ADD_BLACKLIST_ADDR request ID uses body SL_WFX_ADD_BLACKLIST_ADDR_REQ_BODY and returns SL_WFX_ADD_BLACKLIST_ADDR_CNF_BODY
  SL_WFX_SET_MAX_TX_POWER_REQ_ID                 = 0x60,   ///< \b SET_MAX_TX_POWER request ID uses body SL_WFX_SET_MAX_TX_POWER_REQ_BODY and returns SL_WFX_SET_MAX_TX_POWER_CNF_BODY
  SL_WFX_GET_MAX_TX_POWER_REQ_ID                 = 0x61,   ///< \b GET_MAX_TX_POWER request ID uses body SL_WFX_GET_MAX_TX_POWER_REQ_BODY and returns SL_WFX_GET_MAX_TX_POWER_CNF_BODY
  SL_WFX_GET_PMK_REQ_ID                          = 0x62,   ///< \b GET_PMK request ID uses body SL_WFX_GET_PMK_REQ_BODY and returns SL_WFX_GET_PMK_CNF_BODY
  SL_WFX_GET_AP_CLIENT_SIGNAL_STRENGTH_REQ_ID    = 0x63,   ///< \b GET_AP_CLIENT_SIGNAL_STRENGTH request ID uses body SL_WFX_GET_AP_CLIENT_SIGNAL_STRENGTH_BODY and returns SL_WFX_GET_AP_CLIENT_SIGNAL_STRENGTH_CNF_BODY
  SL_WFX_EXT_AUTH_REQ_ID                         = 0x64,   ///< \b EXT_AUTH request ID uses body SL_WFX_EXT_AUTH_BODY and returns SL_WFX_EXT_AUTH_CNF_BODY
  SL_WFX_AP_SCAN_REQUEST_FILTER_REQ_ID           = 0x65    ///< \b AP_SCAN_REQUEST request ID uses body SL_WFX_AP_SCAN_REQUEST_BODY and returns SL_WFX_AP_SCAN_REQUEST_CNF_BODY
} sl_wfx_requests_ids_t;

/**
 * @brief WFM API confirmation message IDs.
 */
typedef enum sl_wfx_confirmations_ids_e {
  SL_WFX_SET_MAC_ADDRESS_CNF_ID                  = 0x42,   ///< \b SET_MAC_ADDRESS confirmation Id. Returns body SL_WFX_SET_MAC_ADDRESS_CNF_BODY
  SL_WFX_CONNECT_CNF_ID                          = 0x43,   ///< \b CONNECT confirmation Id. Returns body SL_WFX_CONNECT_CNF_BODY
  SL_WFX_DISCONNECT_CNF_ID                       = 0x44,   ///< \b DISCONNECT confirmation Id. Returns body SL_WFX_DISCONNECT_CNF_BODY
  SL_WFX_START_AP_CNF_ID                         = 0x45,   ///< \b START_AP confirmation Id. Returns body SL_WFX_START_AP_CNF_BODY
  SL_WFX_UPDATE_AP_CNF_ID                        = 0x46,   ///< \b UPDATE_AP confirmation Id. Returns body SL_WFX_UPDATE_AP_CNF_BODY
  SL_WFX_STOP_AP_CNF_ID                          = 0x47,   ///< \b STOP_AP confirmation Id. Returns body SL_WFX_STOP_AP_CNF_BODY
  SL_WFX_SEND_FRAME_CNF_ID                       = 0x4a,   ///< \b SEND_FRAME confirmation Id. Returns body SL_WFX_SEND_FRAME_CNF_BODY
  SL_WFX_START_SCAN_CNF_ID                       = 0x4b,   ///< \b START_SCAN confirmation Id. Returns body SL_WFX_START_SCAN_CNF_BODY
  SL_WFX_STOP_SCAN_CNF_ID                        = 0x4c,   ///< \b STOP_SCAN confirmation Id. Returns body SL_WFX_STOP_SCAN_CNF_BODY
  SL_WFX_GET_SIGNAL_STRENGTH_CNF_ID              = 0x4e,   ///< \b GET_SIGNAL_STRENGTH confirmation Id. Returns body SL_WFX_GET_SIGNAL_STRENGTH_CNF_BODY
  SL_WFX_DISCONNECT_AP_CLIENT_CNF_ID             = 0x4f,   ///< \b DISCONNECT_AP_CLIENT confirmation Id. Returns body SL_WFX_DISCONNECT_AP_CLIENT_CNF_BODY
  SL_WFX_SET_PM_MODE_CNF_ID                      = 0x52,   ///< \b SET_PM_MODE confirmation Id. Returns body SL_WFX_SET_PM_MODE_CNF_BODY
  SL_WFX_ADD_MULTICAST_ADDR_CNF_ID               = 0x53,   ///< \b ADD_MULTICAST_ADDR confirmation Id. Returns body SL_WFX_ADD_MULTICAST_ADDR_CNF_BODY
  SL_WFX_REMOVE_MULTICAST_ADDR_CNF_ID            = 0x54,   ///< \b REMOVE_MULTICAST_ADDR confirmation Id. Returns body SL_WFX_REMOVE_MULTICAST_ADDR_CNF_BODY
  SL_WFX_SET_MAX_AP_CLIENT_COUNT_CNF_ID          = 0x55,   ///< \b SET_MAX_AP_CLIENT_COUNT confirmation Id. Returns body SL_WFX_SET_MAX_AP_CLIENT_COUNT_CNF_BODY
  SL_WFX_SET_MAX_AP_CLIENT_INACTIVITY_CNF_ID     = 0x56,   ///< \b SET_MAX_AP_CLIENT_INACTIVITY confirmation Id. Returns body SL_WFX_SET_MAX_AP_CLIENT_INACTIVITY_CNF_BODY
  SL_WFX_SET_ROAM_PARAMETERS_CNF_ID              = 0x57,   ///< \b SET_ROAM_PARAMETERS confirmation Id. Returns body SL_WFX_SET_ROAM_PARAMETERS_CNF_BODY
  SL_WFX_SET_TX_RATE_PARAMETERS_CNF_ID           = 0x58,   ///< \b SET_TX_RATE_PARAMETERS confirmation Id. Returns body SL_WFX_SET_TX_RATE_PARAMETERS_CNF_BODY
  SL_WFX_SET_ARP_IP_ADDRESS_CNF_ID               = 0x59,   ///< \b SET_ARP_IP_ADDRESS confirmation Id. Returns body SL_WFX_SET_ARP_IP_ADDRESS_CNF_BODY
  SL_WFX_SET_NS_IP_ADDRESS_CNF_ID                = 0x5A,   ///< \b SET_NS_IP_ADDRESS confirmation Id. Returns body SL_WFX_SET_NS_IP_ADDRESS_CNF_BODY
  SL_WFX_SET_BROADCAST_FILTER_CNF_ID             = 0x5B,   ///< \b SET_BROADCAST_FILTER confirmation Id. Returns body SL_WFX_SET_BROADCAST_FILTER_CNF_BODY
  SL_WFX_SET_SCAN_PARAMETERS_CNF_ID              = 0x5C,   ///< \b SET_SCAN_PARAMETERS confirmation Id. Returns body SL_WFX_SET_SCAN_PARAMETERS_CNF_BODY
  SL_WFX_SET_UNICAST_FILTER_CNF_ID               = 0x5D,   ///< \b SET_UNICAST_FILTER confirmation Id. Returns body SL_WFX_SET_UNICAST_FILTER_CNF_BODY
  SL_WFX_ADD_WHITELIST_ADDR_CNF_ID               = 0x5E,   ///< \b ADD_WHITELIST_ADDR confirmation Id. Returns body SL_WFX_ADD_WHITELIST_ADDR_CNF_BODY
  SL_WFX_ADD_BLACKLIST_ADDR_CNF_ID               = 0x5F,   ///< \b ADD_BLACKLIST_ADDR confirmation Id. Returns body SL_WFX_ADD_BLACKLIST_ADDR_CNF_BODY
  SL_WFX_SET_MAX_TX_POWER_CNF_ID                 = 0x60,   ///< \b SET_MAX_TX_POWER confirmation Id. Returns body SL_WFX_SET_MAX_TX_POWER_CNF_BODY
  SL_WFX_GET_MAX_TX_POWER_CNF_ID                 = 0x61,   ///< \b GET_MAX_TX_POWER confirmation Id. Returns body SL_WFX_GET_MAX_TX_POWER_CNF_BODY
  SL_WFX_GET_PMK_CNF_ID                          = 0x62,   ///< \b GET_PMK confirmation Id. Returns body SL_WFX_GET_PMK_CNF_BODY
  SL_WFX_GET_AP_CLIENT_SIGNAL_STRENGTH_CNF_ID    = 0x63,   ///< \b GET_AP_CLIENT_SIGNAL_STRENGTH confirmation Id. Returns body SL_WFX_GET_AP_CLIENT_SIGNAL_STRENGTH_CNF_BODY
  SL_WFX_EXT_AUTH_CNF_ID                         = 0x64,   ///< \b EXT_AUTH confirmation Id. Returns body SL_WFX_EXT_AUTH_BODY
  SL_WFX_AP_SCAN_REQUEST_FILTER_CNF_ID           = 0x65    ///< \b AP_SCAN_REQUEST confirmation Id. Returns body SL_WFX_AP_SCAN_REQUEST_CNF_BODY
} sl_wfx_confirmations_ids_t;

/**
 * @brief WFM API indication message IDs.
 */
typedef enum sl_wfx_indications_ids_e {
  SL_WFX_CONNECT_IND_ID                          = 0xc3,   ///< \b CONNECT indication id. Content is SL_WFX_CONNECT_IND_BODY
  SL_WFX_DISCONNECT_IND_ID                       = 0xc4,   ///< \b DISCONNECT indication id. Content is SL_WFX_DISCONNECT_IND_BODY
  SL_WFX_START_AP_IND_ID                         = 0xc5,   ///< \b START_AP indication id. Content is SL_WFX_START_AP_IND_BODY
  SL_WFX_STOP_AP_IND_ID                          = 0xc7,   ///< \b STOP_AP indication id. Content is SL_WFX_STOP_AP_IND_BODY
  SL_WFX_RECEIVED_IND_ID                         = 0xca,   ///< \b RECEIVED indication id. Content is SL_WFX_RECEIVED_IND_BODY
  SL_WFX_SCAN_RESULT_IND_ID                      = 0xcb,   ///< \b SCAN_RESULT indication id. Content is SL_WFX_SCAN_RESULT_IND_BODY
  SL_WFX_SCAN_COMPLETE_IND_ID                    = 0xcc,   ///< \b SCAN_COMPLETE indication id. Content is SL_WFX_SCAN_COMPLETE_IND_BODY
  SL_WFX_AP_CLIENT_CONNECTED_IND_ID              = 0xcd,   ///< \b AP_CLIENT_CONNECTED indication id. Content is SL_WFX_AP_CLIENT_CONNECTED_IND_BODY
  SL_WFX_AP_CLIENT_REJECTED_IND_ID               = 0xce,   ///< \b AP_CLIENT_REJECTED indication id. Content is SL_WFX_AP_CLIENT_REJECTED_IND_BODY
  SL_WFX_AP_CLIENT_DISCONNECTED_IND_ID           = 0xcf,   ///< \b AP_CLIENT_DISCONNECTED indication id. Content is SL_WFX_AP_CLIENT_DISCONNECTED_IND_BODY
  SL_WFX_EXT_AUTH_IND_ID                         = 0xd2,   ///< \b EXT_AUTH indication Id. Content is SL_WFX_EXT_AUTH_IND_BODY
  SL_WFX_PS_MODE_ERROR_IND_ID                    = 0xd3,   ///< \b PS_MODE_ERROR indication Id. Content is SL_WFX_PS_MODE_ERROR_IND_BODY
  SL_WFX_AP_SCAN_REQUEST_IND_ID                  = 0xd4    ///< \b AP_SCAN_REQUEST indication Id. Content is SL_WFX_AP_SCAN_REQUEST_IND_BODY
} sl_wfx_indications_ids_t;

/**
 * @brief WFM API message IDs.
 */
typedef union __attribute__((__packed__)) wfm_message_ids_u {
  /** Request messages sent from the host to the device. */
  sl_wfx_requests_ids_t requests;
  /** Confirmation messages sent from the device to the host. */
  sl_wfx_confirmations_ids_t confirmations;
  /** Indication messages sent from the device to the host. */
  sl_wfx_indications_ids_t indications;
} sl_wfx_message_ids_t;

/**
 * @}
 */

/**
 * @addtogroup WFM_GROUP_CONSTANTS
 * @{
 */

/** Length of the MAC address element. */
#define SL_WFX_FMAC_MAC_ADDR_SIZE                SL_WFX_MAC_ADDR_SIZE
/** Length of the BSSID element. */
#define SL_WFX_FMAC_BSSID_SIZE                   SL_WFX_BSSID_SIZE
/** Length of the password element. */
#define SL_WFX_PASSWORD_SIZE                     64
/** Maximum length of the channel list element. */
#define SL_WFX_CHANNEL_NUMBER_SIZE               14
/** Maximum length of the SSID list element. */
#define SL_WFX_SSID_DEF_SIZE                     2
/** Length of the Service Set Identifier (SSID) element. */
#define SL_WFX_FMAC_SSID_SIZE                    SL_WFX_SSID_SIZE
/** Length of the ARP IP address list element. */
#define SL_WFX_ARP_IP_ADDR_SIZE                  2
/** Length of the NS IP address list element. */
#define SL_WFX_NS_IP_ADDR_SIZE                   2
/** Length of the IPv6 address element. */
#define SL_WFX_IPV6_ADDR_SIZE                    16
/** Maximum size of IE data filter */
#define SL_WFX_IE_DATA_FILTER_SIZE               16

/**
 * @brief Client Isolation toggling.
 */
typedef enum sl_wfx_client_isolation_e {
  WFM_CLIENT_ISOLATION_DISABLED                  = 0x0,    ///< Client isolation disabled
  WFM_CLIENT_ISOLATION_ENABLED                   = 0x1     ///< Client isolation enabled
} sl_wfx_client_isolation_t;

/**
 * @brief Type of frame to be sent.
 *
 * Value unused, all frames are considered to be Data by default.@n
 * Support for the other types will be added later.
 */
typedef enum sl_wfx_frame_type_e {
  WFM_FRAME_TYPE_MGMT                            = 0x0,    ///< Management Frame
  WFM_FRAME_TYPE_ACTION                          = 0x4,    ///< Action Frame
  WFM_FRAME_TYPE_DATA                            = 0x8     ///< Data Frame
} sl_wfx_frame_type_t;

/**
 * @brief Hidden SSID toggling.
 */
typedef enum sl_wfx_hidden_ssid_e {
  WFM_HIDDEN_SSID_FALSE                          = 0x0,    ///< SSID not hidden
  WFM_HIDDEN_SSID_TRUE                           = 0x1     ///< SSID hidden
} sl_wfx_hidden_ssid_t;

/**
 * @brief Device Protected Management Frame mode.
 */
typedef enum sl_wfx_mgmt_frame_protection_e {
  WFM_MGMT_FRAME_PROTECTION_DISABLED             = 0x0,    ///< PMF disabled
  WFM_MGMT_FRAME_PROTECTION_OPTIONAL             = 0x1,    ///< PMF optional
  WFM_MGMT_FRAME_PROTECTION_MANDATORY            = 0x2     ///< PMF mandatory
} sl_wfx_mgmt_frame_protection_t;

/**
 * @brief Device power management mode.
 */
typedef enum sl_wfx_pm_mode_e {
  WFM_PM_MODE_ACTIVE                             = 0x0,    ///< Always on
  WFM_PM_MODE_PS                                 = 0x1,    ///< Use power_save and wake up on beacons
  WFM_PM_MODE_DTIM                               = 0x2     ///< Use power_save and wake up on DTIM
} sl_wfx_pm_mode_t;

/**
 * @brief Device power save polling strategy.
 */
typedef enum sl_wfx_pm_poll_e {
  WFM_PM_POLL_UAPSD                              = 0x0,    ///< Use U-APSD
  WFM_PM_POLL_FAST_PS                            = 0x1     ///< Use Fast PS
} sl_wfx_pm_poll_t;

/**
 * @brief Data priority level per 802.1D.
 */
typedef enum sl_wfx_priority_e {
  WFM_PRIORITY_BE0                               = 0x0,    ///< Best Effort
  WFM_PRIORITY_BK1                               = 0x1,    ///< Background
  WFM_PRIORITY_BK2                               = 0x2,    ///< Background
  WFM_PRIORITY_BE3                               = 0x3,    ///< Best Effort
  WFM_PRIORITY_VI4                               = 0x4,    ///< Video
  WFM_PRIORITY_VI5                               = 0x5,    ///< Video
  WFM_PRIORITY_VO6                               = 0x6,    ///< Voice
  WFM_PRIORITY_VO7                               = 0x7     ///< Voice
} sl_wfx_priority_t;

/**
 * @brief Reasons for AP to reject or disconnect a client.
 */
typedef enum sl_wfx_reason_e {
  WFM_REASON_UNSPECIFIED                         = 0x0,    ///< Unspecified reason (unused)
  WFM_REASON_TIMEOUT                             = 0x1,    ///< Client timed out
  WFM_REASON_LEAVING_BSS                         = 0x2,    ///< Client left
  WFM_REASON_UNKNOWN_STA                         = 0x3,    ///< Client not authenticated
  WFM_REASON_AP_FULL                             = 0x4,    ///< Too many clients already connected
  WFM_REASON_AUTHENTICATION_FAILURE              = 0x5     ///< WPA authentication failed
} sl_wfx_reason_t;

/**
 * @brief Reasons for STA disconnection
 */
typedef enum sl_wfx_disconnected_reason_e {
  WFM_DISCONNECTED_REASON_UNSPECIFIED            = 0x0,    ///< Unspecified reason
  WFM_DISCONNECTED_REASON_AP_LOST                = 0x1,    ///< AP timed out
  WFM_DISCONNECTED_REASON_REJECTED               = 0x2,    ///< Disconnected by AP
  WFM_DISCONNECTED_REASON_LEAVING_BSS            = 0x3,    ///< Leaving intentionally
  WFM_DISCONNECTED_REASON_WPA_COUNTERMEASURES    = 0x4     ///< WPA countermeasures triggered a disconnection
} sl_wfx_disconnected_reason_t;

/**
 * @brief Scan mode to be used.
 */
typedef enum sl_wfx_scan_mode_e {
  WFM_SCAN_MODE_PASSIVE                          = 0x0,    ///< Passive scan: listen for beacons only
  WFM_SCAN_MODE_ACTIVE                           = 0x1     ///< Active scan: send probe requests
} sl_wfx_scan_mode_t;

/**
 * @brief Security mode of a network.
 */
typedef enum sl_wfx_security_mode_e {
  WFM_SECURITY_MODE_OPEN                         = 0x0,    ///< No security
  WFM_SECURITY_MODE_WEP                          = 0x1,    ///< Use WEP
  WFM_SECURITY_MODE_WPA2_WPA1_PSK                = 0x2,    ///< Use WPA1 or WPA2
  WFM_SECURITY_MODE_WPA2_PSK                     = 0x4,    ///< Use only WPA2
  WFM_SECURITY_MODE_WPA3_SAE                     = 0x6     ///< Use WPA3 (STA mode only)
} sl_wfx_security_mode_t;

/**
 * @brief Type of the authentication message.
 */
typedef enum sl_wfx_ext_auth_data_type_e {
  WFM_EXT_AUTH_DATA_TYPE_SAE_START               = 0x0,    ///<
  WFM_EXT_AUTH_DATA_TYPE_SAE_COMMIT              = 0x1,    ///<
  WFM_EXT_AUTH_DATA_TYPE_SAE_CONFIRM             = 0x2,    ///<
  WFM_EXT_AUTH_DATA_TYPE_MSK                     = 0x3     ///<
} sl_wfx_ext_auth_data_type_t;

/**
 * @brief Full MAC (UMAC) confirmation possible values for a returned 'status' field.
 *
 * All Full MAC (UMAC) confirmation messages have a field 'status' just after the message header.@n
 * A value of zero indicates the request has completed successfully.
 */
typedef enum sl_wfx_fmac_status_e {
  WFM_STATUS_SUCCESS                             = 0x0,    ///< The device has successfully completed a request.
  WFM_STATUS_INVALID_PARAMETER                   = 0x1,    ///< A request contains one or more invalid parameters.
  WFM_STATUS_WRONG_STATE                         = 0x2,    ///< The request cannot be performed because the device is in an inappropriate state.
  WFM_STATUS_GENERAL_FAILURE                     = 0x3,    ///< The request failed due to an error.
  WFM_STATUS_CHANNEL_NOT_ALLOWED                 = 0x4,    ///< The request failed due to regulatory limitations.
  WFM_STATUS_WARNING                             = 0x5,    ///< The request is successful but some parameters have been ignored.
  WFM_STATUS_NO_MATCHING_AP                      = 0x6,    ///< The request failed because no suitable AP was found for the connection
  WFM_STATUS_CONNECTION_ABORTED                  = 0x7,    ///< The request failed because the user issued a WFM_HI_DISCONNECT_REQ before completing the connection
  WFM_STATUS_CONNECTION_TIMEOUT                  = 0x8,    ///< The request failed because a timeout occurred during connection
  WFM_STATUS_CONNECTION_REJECTED_BY_AP           = 0x9,    ///< The request failed because the AP rejected the connection
  WFM_STATUS_CONNECTION_AUTH_FAILURE             = 0xA,    ///< The request failed because the WPA handshake did not complete successfully
  WFM_STATUS_RETRY_EXCEEDED                      = 0x13,   ///< The request failed because the retry limit was exceeded.
  WFM_STATUS_TX_LIFETIME_EXCEEDED                = 0x14,   ///< The request failed because the MSDU life time was exceeded.
  WFM_STATUS_REQUEUE                             = 0x15    ///< The request failed because TX is suspended (temperature too high)
} sl_wfx_fmac_status_t;

/**
 * @}
 */

/**
 * @addtogroup WFM_GROUP_TYPES
 * @{
 */

/**
 * @brief Service Set Identifier (SSID) of a network.
 * @details Note that the Ssid element must always contain SL_WFX_SSID_SIZE bytes.
 *          Only the bytes up to SsidLength are considered to be valid, the rest should be set to zero.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ssid_def_s {
  /**
   * @brief Length of SSID data.
   * @details <B>0 - 32</B>: The amount of bytes.
   */
  uint32_t ssid_length;
  /** SSID data. */
  uint8_t  ssid[SL_WFX_SSID_SIZE];
} sl_wfx_ssid_def_t;

/**
 * @brief Security mode bitmask for scan results. Empty if Open network.
 */
typedef struct __attribute__((__packed__)) sl_wfx_security_mode_bitmask_s {
  uint8_t  wep : 1;                                        ///< Network supports WEP
  uint8_t  wpa : 1;                                        ///< Network supports WPA. If both WPA bits are set the network supports mixed mode.
  uint8_t  wpa2 : 1;                                       ///< Network supports WPA2. If both WPA bits are set the network supports mixed mode.
  uint8_t  wpa3 : 1;                                       ///< Network supports WPA3. If multiple WPA bits are set the network supports mixed mode.
  uint8_t  pmf : 1;                                        ///< Networks requires use of Protected Management Frames
  uint8_t  unused : 1;                                     ///< Reserved, set to zero
  uint8_t  psk : 1;                                        ///< Network supports Personal authentication
  uint8_t  eap : 1;                                        ///< Network supports Enterprise authentication
} sl_wfx_security_mode_bitmask_t;

/**
 * @brief Device TX rate set bitmask used in sl_wfx_set_tx_rate_parameters_req_body_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_rate_set_bitmask_s {
  uint8_t  b1Mbps : 1;                                     ///< If set, the device may use 802.11b 1Mbps data rate.
  uint8_t  b2Mbps : 1;                                     ///< If set, the device may use 802.11b 2Mbps data rate.
  uint8_t  b5P5Mbps : 1;                                   ///< If set, the device may use 802.11b 5.5Mbps data rate.
  uint8_t  b11Mbps : 1;                                    ///< If set, the device may use 802.11b 11Mbps data rate.
  uint8_t  unused : 4;                                     ///< Reserved, set to zero
  uint8_t  g6Mbps : 1;                                     ///< If set, the device may use 802.11g 6Mbps data rate.
  uint8_t  g9Mbps : 1;                                     ///< If set, the device may use 802.11g 9Mbps data rate.
  uint8_t  g12Mbps : 1;                                    ///< If set, the device may use 802.11g 12Mbps data rate.
  uint8_t  g18Mbps : 1;                                    ///< If set, the device may use 802.11g 18Mbps data rate.
  uint8_t  g24Mbps : 1;                                    ///< If set, the device may use 802.11g 24Mbps data rate.
  uint8_t  g36Mbps : 1;                                    ///< If set, the device may use 802.11g 36Mbps data rate.
  uint8_t  g48Mbps : 1;                                    ///< If set, the device may use 802.11g 48Mbps data rate.
  uint8_t  g54Mbps : 1;                                    ///< If set, the device may use 802.11g 54Mbps data rate.
  uint8_t  mcs0 : 1;                                       ///< If set, the device may use 802.11n 6.5Mbps data rate.
  uint8_t  mcs1 : 1;                                       ///< If set, the device may use 802.11n 13Mbps data rate.
  uint8_t  mcs2 : 1;                                       ///< If set, the device may use 802.11n 19.5Mbps data rate.
  uint8_t  mcs3 : 1;                                       ///< If set, the device may use 802.11n 26Mbps data rate.
  uint8_t  mcs4 : 1;                                       ///< If set, the device may use 802.11n 39Mbps data rate.
  uint8_t  mcs5 : 1;                                       ///< If set, the device may use 802.11n 52Mbps data rate.
  uint8_t  mcs6 : 1;                                       ///< If set, the device may use 802.11n 58.5Mbps data rate.
  uint8_t  mcs7 : 1;                                       ///< If set, the device may use 802.11n 65Mbps data rate.
  uint8_t  unused2;                                        ///< Reserved, set to zero
} sl_wfx_rate_set_bitmask_t;

/**
 * @brief NS IP address element.
 * @details Note that the IP element must always contain SL_WFX_IPV6_ADDR_SIZE bytes.
 *          Only the bytes up to SsidLength are considered to be valid, the rest should be set to zero.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ns_ip_addr_s {
  /** NS IP address. */
  uint8_t  ipv6_addr[SL_WFX_IPV6_ADDR_SIZE];
} sl_wfx_ns_ip_addr_t;

/**
 * @}
 */

/**
 * @addtogroup WFM_GROUP_CONCEPTS
 * @{
 */

/**
 * @page WFM_CONCEPT_SSID Service Set Identifier
 * @par
 * Service Set Identifier (SSID) is an unique identifier of a Wi-Fi network that can be
 * seen as a network name since it's typically an ASCII or UTF8 string.
 * @par
 * SSID contains 0 to 32 bytes of data. When SSID is 0 bytes long, it's known as
 * a broadcast SSID or a wildcard SSID.
 */

/**
 * @page WFM_CONCEPT_BSSID Basic Service Set Identifier
 * @par
 * Basic Service Set Identifier (BSSID) is an unique identifier of a Wi-Fi Access Point.
 * BSSID is a 6-byte field set in the same format as an IEEE 802 MAC address.
 * <BR>See @ref WFM_CONCEPT_MAC for further details.
 */

/**
 * @page WFM_CONCEPT_MAC Media Access Control address
 * @par Broadcast MAC address
 * When all bytes of the MAC address field are set to 0xFF, the MAC address is considered
 * to be a broadcast MAC address.
 * @code
 * { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
 * @endcode
 *
 * @par Multicast MAC address
 * When the lowest bit of the first byte is set, the MAC address is considered to
 * be a multicast MAC address.
 * @code
 * { 0x33, 0x33, 0x00, 0x00, 0x00, 0x01 }
 * @endcode
 *
 * @par Zero MAC address
 * When all bytes of the MAC address field are set to 0x00, the MAC address is considered
 * to be a zero MAC address.
 * @code
 * { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
 * @endcode
 */

/**
 * @page WFM_CONCEPT_VENDOR_IE Vendor-Specific IE
 * @par
 * 802.11 Vendor-specific element starts with a 1-byte Element ID (0xDD), followed by 1-byte Element Length
 * specifying the amount of bytes after the length field. The next field is Organization Identifier (OUI)
 * field which is typically at least 3 bytes, followed by the vendor-specific content.
 * @par
 * An example of a vendor-specific IE that uses a Silicon Laboratories OUI (90-FD-9F),
 * a vendor-specific type of 0 and vendor-specific data "HELLO".
 * @code
 * { 0xDD, 0x09, 0x90, 0xFD, 0x9F, 0x00, 0x48 0x,45 0x4C, 0x4C, 0x4F }
 * @endcode
 */

/**
 * @page WFM_CONCEPT_PASSWORD Passwords and pass phrases
 * @par
 * Note that the Password element must always contain SL_WFX_PASSWORD_SIZE bytes.
 * Only the bytes up to PasswordLength are considered to be valid, the rest are ignored.
 * @par An example of a 64-bit WEP key "12345" in ASCII format
 * @code
 * { 0x31, 0x32, 0x33, 0x34, 0x35 }
 * @endcode
 *
 * @par An example of a 64-bit WEP key "\x31\x32\x33\x34\x35" in HEX format
 * @code
 * { 0x33, 0x31, 0x33, 0x32, 0x33, 0x33, 0x33, 0x34, 0x33, 0x35 }
 * @endcode
 */

/**
 * @page WFM_CONCEPT_RCPI Received Channel Power Indicator
 * @par
 * Received Channel Power Indicator (RCPI) is a monotonically increasing,
 * logarithmic function of the received power level.
 * | Value     | Description |
 * |-----------|-------------|
 * |        0  | Power level is less than -109.5 dBm         |
 * |   1 - 219 | Power level is between -109.5 dBm and 0 dBm |
 * |       220 | Power level is equal or greater than 0 dBm  |
 * | 221 - 255 | Reserved
 *
 * The following equation can be used to convert the RCPI value to corresponding dBm value.
 * <BR>[signal level in dBm] = (RCPI / 2) - 110
 * <BR>RCPI 60 => signal level is -80 dBm
 * <BR>RCPI 80 => signal level is -70 dBm
 */

/**
 * @page WFM_CONCEPT_PM Power management
 * @par
 * In Wi-Fi power management, a device has two power modes: active and power save. In active mode,
 * the device can transmit and receive packets. When in power save, the device has signaled
 * other devices it's available only periodically and communicating with it requires specific
 * power management procedures.
 * @par
 * Wi-Fi power management is different from device power management. Even when the device
 * has enabled Wi-Fi power save, it doesn't necessarily mean the device is actually in a
 * low-power state. Wi-Fi power save must be enabled to use device power management
 * but not vice versa.
 * @par
 * Wi-Fi power save is only available in Wi-Fi client role. The functionality can be controlled
 * using sl_wfx_set_pm_mode_req_t message. The device has three different power management modes.
 * @par WFM_PM_MODE_ACTIVE
 * In this mode, the device does not use Wi-Fi power management. Since no special power management
 * procedures are required, this mode gives the best throughput and latency. Device power management
 * cannot be used in this mode. This is the default mode after the connection has been established.
 * @par WFM_PM_MODE_BEACON
 * In this mode, the device signals to the Wi-Fi Access Point it's in power save and therefore only
 * available on periodic intervals. This causes the Access Point to buffer packets destined
 * to the device until the device retrieves them and therefore causes extra delays on received
 * packets. However, the mode allows the device to use device power management.
 * @par WFM_PM_MODE_DTIM
 * This mode is similar to WFM_PM_MODE_BEACON but it uses a different periodic interval called
 * DTIM period. The length of the DTIM period depends on the access point used but it is at least
 * as long as the period in WFM_PM_MODE_BEACON.
 */

/**
 * @page WFM_CONCEPT_HIDDEN Hidden network
 * @par
 * Usually, a Wi-Fi Access Point will advertise its details in Beacon frames as well as in
 * Probe Response frames which are sent as a response to Probe Request frames. One of the
 * details advertised is the SSID of the network.
 * @par
 * When the network is hidden, the device will replace the actual SSID in Beacon frames with
 * a broadcast SSID and will stop responding to Probe Requests that do not specify the correct
 * SSID. In practice, other stations will still see there is a network near-by due to the
 * Beacon frames but they will not be able to determine the SSID.
 * @par
 * This feature shouldn't be seen as a security feature since it's fairly simple to determine
 * the SSID by passively observing stations that know the correct SSID.
 */

/**
 * @page WFM_CONCEPT_ISOLATION Client isolation
 * @par
 * When a Wi-Fi network has multiple stations connected, they communicate with each other
 * by first sending the data frame to the Access Point. Access Point then determines the
 * destination is one of its clients and resends the frame to the destination station.
 *
 * When the client isolation is enabled, the Access Point discards all data frames intended
 * for other stations. Therefore, the stations will only be able to communicate with the
 * access point.
 */

/**
 * @page WFM_CONCEPT_PACKET Packet types
 * @par Data frame with Ethernet II header
 * When transmitting and receiving data frames in this format, the payload message such as
 * an IP packet or an ARP packet is encapsulated in an Ethernet II frame header without the
 * trailing CRC checksum.
 * @par
 * Ethernet II headers is 14 bytes (6-byte Destination MAC address, 6-byte Source MAC Address,
 * 2-byte EtherType). The EtherType is written in big-endian format.
 * @par
 * An ARP packet (EtherType 0x0806) sent to the broadcast address (FF:FF:FF:FF:FF:FF)
 * from the source MAC address (00:01:02:03:04:05) therefore starts with the header
 * @code
 * { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x06 }
 * @endcode
 * and is immediately followed by the actual ARP packet.
 */

/**
 * @page WFM_CONCEPT_SCAN Scanning
 * @par Scan mode
 * The device supports two modes of scanning: active and passive. In passive mode, the device
 * switches to each channel to be scanned in turn and listens for Access Points operating
 * on the channel. After a time limit, it switches to the next channel.
 * @par
 * The active mode is similar to the passive mode but in addition to listening, the device
 * will solicit advertisements from the access points on the channel. Whether an Access
 * Point responds to a solicitation depends on the SSID(s) used in the scan. If the scan
 * is performed with a wildcard SSID, all access points on the channel will respond. If
 * performed with a specific SSID, only the access points having that SSID will respond.
 * @par
 * Note that even when scanning with a specific SSID, the scan results may contain other
 * access points if the device happened to hear the advertisement while on the channel.
 * @par Specific channel list
 * It's possible to specify a list of channels to be scanned. The channel list
 * is an array of bytes, one byte per channel. The channels are scanned in the order given.
 * @par
 * An example of channels list (1, 6, 11)
 * @code
 * { 0x01, 0x06, 0x11 }
 * @endcode
 * @par Specific SSID list
 * When performing an active scan, a list of SSIDs to be scanned can be specified. Only
 * the SSIDs on the list will be queried. The lists consists of multiple sl_wfx_ssid_def_t elements
 * concatenated together.
 * @par Scan Parameters
 * In the command sl_wfx_set_scan_parameters_req_t, it is possible to specify some scan parameters that will
 * apply to every subsequent scan. It must be noted that parameters that do not comply
 * with the current environment will be overridden internally (e.g., a very long scan
 * that would jeopardize the current connection will be made shorter). In this case, a WFM_STATUS_WARNING
 * will be issued in sl_wfx_start_scan_cnf_t. It is the advised that the host adjusts the scan parameters.
 */

/**
 * @page WFM_CONCEPT_OFFLOADING Offloading
 * @par
 * To facilitate low-power use-cases, there is a possibility to offload
 * replying to Address Resolution Protocol (ARP) requests and IPv6 Neighbor Solicitation (NS)
 * packets to the device. By default, the offloading functionality is disabled.
 * @par ARP offloading
 * When the functionality is enabled, the device will respond to ARP requests that
 * specify one of the configured IPv4 addresses. The request is not forwarded to
 * the host. The functionality can be controlled using sl_wfx_set_arp_ip_address_req_t message.
 * @par NS offloading
 * When the functionality is enabled, the device will respond to IPv6 NS packets that
 * specify one of the configured IPv6 addresses. The request is not forwarded to
 * the host. The functionality can be controlled using sl_wfx_set_ns_ip_address_req_t message.
 */

/**
 * @page WFM_CONCEPT_FILTERING Filtering
 * @par
 * By default, the device forwards all packets it receives from the network to the host.
 * In a busy network, processing the broadcast and the multicast traffic may be a significant
 * source of power consumption on the host. To facilitate low-power use-cases, the
 * device offers multiple options for discarding some of the received packets on the device.
 * @par Multicast filtering
 * Filtering of multicast traffic is based on a whitelist of destination multicast addresses.
 * Any received packet whose destination address does not match the whitelist is automatically
 * discarded. If the host is not interested in multicast traffic, it's also possible to
 * discard all multicast traffic. By default all multicast packets are accepted. The functionality
 * can be controlled using sl_wfx_add_multicast_addr_req_t and sl_wfx_remove_multicast_addr_req_t
 * messages.
 * @par Broadcast filtering
 * Broadcast filtering is a boolean option. When enabled, the device discards all received
 * packets sent to the broadcast address except ARP and DHCP messages. By default, all broadcast
 * packets are accepted. The functionality can be controlled using sl_wfx_set_broadcast_filter_req_t message.
 */

/**
 * @page WFM_CONCEPT_ROAM Roaming
 * @par
 * When operating in Wi-Fi client role, the device is capable of autonomously switching to
 * a different access point when the current access point is either lost or the signal strength
 * drops below the roaming threshold. The device will only consider access points that have
 * the same SSID and that otherwise have the same security capabilities as the previous access
 * point.
 * @par
 * The autonomous roaming functionality can be disabled by setting the corresponding option
 * in sl_wfx_connect_req_t message. The various roaming parameters may be adjusted using
 * sl_wfx_set_roam_parameters_req_t message. Parameter changes will be applied at the next connection.
 */

/**
 * @page WFM_CONCEPT_TX_RATE Transmit rate
 * @par
 * A Wi-Fi device may use a number of different data rates for transmission. When operating as a client,
 * a device adapts the rates it uses to match those the access point authorizes. However it is possible
 * to specify rates that the device will not use even if they are allowed by the network.
 * @par
 * The precise allowed rates may be adjusted using sl_wfx_set_tx_rate_parameters_req_t message.
 * Parameter changes will be applied at the next connection. If no overlap between these parameters
 * and the access point's supported rates list is found, it will default to using only 1Mbits/sec rate.
 */

/**
 * @}
 */

/**
 * @addtogroup WFM_GROUP_MESSAGES
 * @{
 */

/**
 * @brief Request message body for sl_wfx_set_mac_address_req_t.
 * @details
 *          | Field       | Default value    | Reset to default value |
 *          |:------------|:-----------------|:-----------------------|
 *          | MAC address | device dependent | when device reset      |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_mac_address_req_body_s {
  /** MAC address of the interface. */
  uint8_t  mac_addr[SL_WFX_MAC_ADDR_SIZE];
  /** Reserved, set to zero. */
  uint16_t reserved;
} sl_wfx_set_mac_address_req_body_t;

/**
 * @brief Request message to set MAC address of the interface.
 * @details The host can use this request to set the MAC address of an interface.
 *          If not set, the device will use a built-in MAC address. Note that
 *          if multiple interfaces are used, the MAC address MUST be different
 *          on each interface.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | No              |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_mac_address_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_mac_address_req_body_t body;
} sl_wfx_set_mac_address_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_mac_address_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_mac_address_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_mac_address_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_mac_address_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_mac_address_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_mac_address_cnf_body_t body;
} sl_wfx_set_mac_address_cnf_t;

/**
 * @brief Request message body for sl_wfx_connect_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_req_body_s {
  /**
   * @brief Service Set Identifier (SSID) of the network.
   */
  sl_wfx_ssid_def_t ssid_def;
  /**
   * @brief Basic Service Set Identifier (BSSID) of the Access Point.
   * @details <B>broadcast address</B>: The device will connect to any matching access point.
   *          <BR><B>unicast address</B>: The device will only connect to the given Access Point.
   *          <BR>See @ref WFM_CONCEPT_BSSID for further details.
   */
  uint8_t  bssid[SL_WFX_BSSID_SIZE];
  /**
   * @brief Channel of the Access Point.
   * @details <B>0</B>: The device will connect to a matching Access Point on any channel.
   *          <BR><B>1 - 13</B>: The device will only connect to a matching Access Point on the given channel.
   */
  uint16_t channel;
  /**
   * @brief Security mode of the network.
   * @details <B>WFM_SECURITY_MODE_OPEN</B>: The device will only connect to an unsecured Access Point.
   *          <BR><B>WFM_SECURITY_MODE_WEP</B>: The device will only connect to a WEP Access Point.
   *          <BR><B>WFM_SECURITY_MODE_WPA2_WPA1_PSK</B>: The device will only connect to a WPA-Personal or a WPA2-Personal Access Point.
   *          <BR><B>WFM_SECURITY_MODE_WPA2_PSK</B>: The device will only connect to a WPA2-Personal access point.
   *          <BR><B>WFM_SECURITY_MODE_WPA3_SAE</B>: The device will only connect to a WPA3-SAE access point.
   *          <BR>See wfm_security_mode for enumeration values.
   */
  uint8_t  security_mode;
  /**
   * @brief Boolean option to prevent roaming between access points.
   * @details <B>0</B>: The device may roam to any matching access point within the same network.
   *          <BR><B>1</B>: The device will not roam to any other access point.
   */
  uint8_t  prevent_roaming;
  /**
   * @brief Protected Management Frames (PMF) mode.
   * @details <B>WFM_MGMT_FRAME_PROTECTION_DISABLED</B>: The device will not use PMF even if supported by the access point.
   *          <BR><B>WFM_MGMT_FRAME_PROTECTION_OPTIONAL</B>: The device will use PMF if supported by the access point.
   *          <BR><B>WFM_MGMT_FRAME_PROTECTION_MANDATORY</B>: The device will only connect to an access point supporting PMF.
   *          <BR>See wfm_mgmt_frame_protection for enumeration values.
   */
  uint16_t mgmt_frame_protection;
  /**
   * @brief Length of the network password.
   * @details <B>0 - 64</B>: The amount of bytes.
   */
  uint16_t password_length;
  /**
   * @brief Password of the network.
   * @details <B>64-bit WEP key</B>: 5 bytes in ASCII format or 10 bytes in HEX format.
   *          <BR><B>128-bit WEP key</B>: 13 bytes in ASCII format or 26 bytes in HEX format.
   *          <BR><B>WPA pass phrase</B>: 8 - 63 bytes in ASCII format.
   *          <BR><B>WPA PMK</B>: 64 bytes in HEX format.
   *          <BR>See @ref WFM_CONCEPT_PASSWORD for further details.
   */
  uint8_t  password[SL_WFX_PASSWORD_SIZE];
  /**
   * @brief Length of vendor-specific Information Element (IE) data.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t ie_data_length;
  /**
   * @brief Vendor-specific IE data added to the 802.11 Association Request frames.
   * @details The IE data must be in 802.11 Vendor-Specific Element format.
   *          It may contain multiple concatenated IEs, up to the maximum length.
   *          <BR>See @ref WFM_CONCEPT_VENDOR_IE for further details.
   */
  uint8_t  ie_data[];
} sl_wfx_connect_req_body_t;

/**
 * @brief Request message for connecting to a Wi-Fi network.
 * @details The host can use this request to initiate a connection to a Wi-Fi network.
 *          An ongoing connection attempt can be canceled by sending sl_wfx_disconnect_req_t
 *          message. Completion of the connection request will be signaled by sl_wfx_connect_ind_t
 *          message. It may take up to 10 seconds to receive the message.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | No              |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_connect_req_body_t body;
} sl_wfx_connect_req_t;

/**
 * @brief Confirmation message body for sl_wfx_connect_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_cnf_body_s {
  /**
   * @brief Status of the connection request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the connection request was accepted. It will be completed by sl_wfx_connect_ind_t.
   *          <BR><B>any other value</B>: the connection request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_connect_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_connect_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_connect_cnf_body_t body;
} sl_wfx_connect_cnf_t;

/**
 * @brief Indication message body for sl_wfx_connect_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_ind_body_s {
  /**
   * @brief Status of the connection request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the connection request was completed successfully.
   *          <BR><B>any other value</B>: the connection request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief MAC address of the connected access point.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief Channel of the connected access point.
   * @details <B>1 - 13</B>: Channel number.
   */
  uint16_t channel;
  /**
   * @brief Beacon Interval of the connected access point.
   */
  uint8_t  beacon_interval;
  /**
   * @brief DTIM period of the connected access point.
   * @details <B>1 - 255</B>: DTIM period.
   */
  uint8_t  dtim_period;
  /**
   * @brief Maximum PHY data rate supported by the connection.
   * @details See sl_wfx_rate_index_t for enumeration values.
   */
  uint16_t max_phy_rate;
} sl_wfx_connect_ind_body_t;

/**
 * @brief Indication message used to signal the completion of a connection operation.
 * @details The device will send this indication to signal the connection request initiated
 *          with sl_wfx_connect_req_t has been completed. The indication is also sent when
 *          the device autonomously roams to another access point.
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_connect_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_connect_ind_body_t body;
} sl_wfx_connect_ind_t;

/**
 * @struct sl_wfx_disconnect_req_t
 * @brief Request message for disconnecting from a Wi-Fi network.
 * @details The host can use this request to initiate a disconnection from a Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef sl_wfx_header_t sl_wfx_disconnect_req_t;

/**
 * @brief Confirmation message body for sl_wfx_disconnect_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_cnf_body_s {
  /**
   * @brief Status of the disconnection request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the disconnection request was accepted. It will be completed by sl_wfx_disconnect_ind_t.
   *          <BR><B>any other value</B>: the disconnection request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_disconnect_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_disconnect_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_disconnect_cnf_body_t body;
} sl_wfx_disconnect_cnf_t;

/**
 * @brief Indication message body for sl_wfx_disconnect_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ind_body_s {
  /**
   * @brief MAC address of the access point.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief Reason for disconnection.
   * @details <B>WFM_DISCONNECTED_REASON_UNSPECIFIED</B>: The device disconnected because of an internal error.
   *          <BR><B>WFM_DISCONNECTED_REASON_AP_LOST</B>: The device lost the AP beacons for too long.
   *          <BR><B>WFM_DISCONNECTED_REASON_REJECTED</B>: The device was disconnected by the AP.
   *          <BR><B>WFM_DISCONNECTED_REASON_LEAVING_BSS</B>: Disconnection was requested through the device API.
   *          <BR><B>WFM_DISCONNECTED_REASON_WPA_COUNTERMEASURES</B>: WPA countermeasures triggered a disconnection
   *          <BR>See sl_wfx_disconnected_reason_t for enumeration values.
   */
  uint16_t reason;
} sl_wfx_disconnect_ind_body_t;

/**
 * @brief Indication message used to signal the completion of a disconnection operation.
 * @details The device will send this indication to signal the disconnection request initiated
 *          with sl_wfx_disconnect_req_t has been completed. The indication is also sent when
 *          the device has lost the connection to an access point and has been unable to regain it.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_disconnect_ind_body_t body;
} sl_wfx_disconnect_ind_t;

/**
 * @struct sl_wfx_get_signal_strength_req_t
 * @brief Request message for retrieving the signal strength of connection.
 * @details The host can use this request to retrieve the signal strength of the connection.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef sl_wfx_header_t sl_wfx_get_signal_strength_req_t;

/**
 * @brief Confirmation message body for sl_wfx_get_signal_strength_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_signal_strength_cnf_body_s {
  /**
   * @brief Status of the get request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the get request was completed.
   *          <BR><B>any other value</B>: the get request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief Received Channel Power Indicator (RCPI) of the connection.
   * @details See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint32_t rcpi;
} sl_wfx_get_signal_strength_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_get_signal_strength_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_signal_strength_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_get_signal_strength_cnf_body_t body;
} sl_wfx_get_signal_strength_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_pm_mode_req_t.
 * @details
 *          | Field          | Default value      | Reset to default value |
 *          |:---------------|:-------------------|:-----------------------|
 *          | power_mode      | WFM_PM_MODE_ACTIVE | when interface stopped |
 *          | listen_interval | 0                  | when interface stopped |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_pm_mode_req_body_s {
  /**
   * @brief Power management mode.
   * @details <B>WFM_PM_MODE_ACTIVE</B>: the device will not use Wi-Fi power management mechanisms.
   *          <BR><B>WFM_PM_MODE_BEACON</B>: the device will wake-up on beacons.
   *          <BR><B>WFM_PM_MODE_DTIM</B>: the device will wake-up on DTIMs.
   *          <BR>See wfm_pm_mode for enumeration values.
   */
  uint8_t power_mode;
  /**
   * @brief Power save polling strategy.
   * @details <B>WFM_PM_POLL_UAPSD</B>: the device will use U-APSD (default).
   *          <BR><B>WFM_PM_POLL_FAST_PS</B>: the device will use Fast Power Save.
   *          <BR>See WFM_PM_POLL for enumeration values.
   */
  uint8_t polling_strategy;
  /**
   * @brief Number of beacons/DTIMs to skip while sleeping.
   * @details <B>0</B>: wake-up on every beacon/DTIM.
   *          <BR><B>1 - 600</B>: the number of beacon/DTIMs to skip.
   *          <BR>See @ref WFM_CONCEPT_PM for further details.
   */
  uint16_t listen_interval;
} sl_wfx_set_pm_mode_req_body_t;

/**
 * @brief Request message for setting the power management mode of the device.
 * @details The host can use this request to enable or disable Wi-Fi power management mechanisms.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_pm_mode_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_pm_mode_req_body_t body;
} sl_wfx_set_pm_mode_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_pm_mode_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_pm_mode_cnf_body_s {
  /**
   * @brief Status of the power management request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the power management request was completed.
   *          <BR><B>any other value</B>: the power management request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_pm_mode_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_pm_mode_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_pm_mode_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_pm_mode_cnf_body_t body;
} sl_wfx_set_pm_mode_cnf_t;

/**
 * @brief Indication message used to signal that the device has switched to Fast Power Save.
 * @details This indication occurs when the devices switches to Fast PS as an attempt to mitigate
 *          poor performance with U-APSD. Listen interval falls back to 0 (listen to every beacon/DTIM).
 *          This can be overridden by issuing SL_WFX_SET_PM_MODE_REQ command again.
 * @ingroup WFM_GROUP_MESSAGES
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef sl_wfx_header_t sl_wfx_ps_mode_error_ind_t;

/**
 * @brief Request message body for sl_wfx_start_ap_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_req_body_s {
  /**
   * @brief Service Set Identifier (SSID) of the network.
   */
  sl_wfx_ssid_def_t ssid_def;
  /**
   * @brief Boolean option to hide the network.
   * @details <B>0</B>: The device will advertise the SSID of the network to any near-by stations.
   *          <BR><B>1</B>: The device will hide the SSID of the network and will only respond
   *                        to stations that specify the SSID.
   *          <BR>See @ref WFM_CONCEPT_HIDDEN for further details.
   */
  uint8_t  hidden_ssid;
  /**
   * @brief Boolean option to isolate connected clients from each other.
   * @details <B>0</B>: The device will allow connected clients to communicate with each other.
   *          <BR><B>1</B>: The device will prevent multiple connected clients from communicating.
   *          <BR>Regardless of the value, the connected stations will always be able to communicate with the device.
   *          <BR>See @ref WFM_CONCEPT_ISOLATION for further details.
   */
  uint8_t  client_isolation;
  /**
   * @brief Security mode of the Access Point.
   * @details <B>WFM_SECURITY_MODE_OPEN</B>: The device will only allow unsecured connections.
   *          <BR><B>WFM_SECURITY_MODE_WEP</B>: The device will only allow WEP connections.
   *          <BR><B>WFM_SECURITY_MODE_WPA2_WPA1_PSK</B>: The device will only allow WPA-Personal and WPA2-Personal connections.
   *          <BR><B>WFM_SECURITY_MODE_WPA2_PSK</B>: The device will only allow WPA2-Personal connections.
   *          <BR><B>WFM_SECURITY_MODE_WPA3_SAE</B>: Unsupported in AP mode
   *          <BR>See wfm_security_mode for enumeration values.
   */
  uint8_t  security_mode;
  /**
   * @brief Protected Management Frames (PMF) mode.
   * @details <B>WFM_MGMT_FRAME_PROTECTION_DISABLED</B>: The device will not use PMF even if supported by the connecting station.
   *          <BR><B>WFM_MGMT_FRAME_PROTECTION_OPTIONAL</B>: The device will use PMF if supported by the connecting station.
   *          <BR><B>WFM_MGMT_FRAME_PROTECTION_MANDATORY</B>: The device will only allow connecting stations that support PMF.
   *          <BR>See wfm_mgmt_frame_protection for enumeration values.
   */
  uint8_t  mgmt_frame_protection;
  /**
   * @brief Channel of the Access Point.
   * @details <B>1 - 13</B>: The device will create the access point on the given channel.
   */
  uint16_t channel;
  /**
   * @brief Length of the network password.
   * @details <B>0 - 64</B>: The amount of bytes.
   */
  uint16_t password_length;
  /**
   * @brief Password of the network.
   * @details <B>64-bit WEP key</B>: 5 bytes in ASCII format or 10 bytes in HEX format.
   *          <BR><B>128-bit WEP key</B>: 13 bytes in ASCII format or 26 bytes in HEX format.
   *          <BR><B>WPA pass phrase</B>: 8 - 63 bytes in ASCII format.
   *          <BR><B>WPA PMK</B>: 64 bytes in HEX format.
   *          <BR>See @ref WFM_CONCEPT_PASSWORD for further details.
   */
  uint8_t  password[SL_WFX_PASSWORD_SIZE];
  /**
   * @brief Length of vendor-specific Information Element (IE) data in 802.11 Beacon frames.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t beacon_ie_data_length;
  /**
   * @brief Length of vendor-specific Information Element (IE) data in 802.11 Probe Response frames.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t probe_resp_ie_data_length;
  /**
   * @brief Vendor-specific IE data added to the 802.11 Beacon frames and Probe Response frames.
   * @details The IE data must be in 802.11 Vendor-Specific Element format. It may contain
   *          multiple concatenated IEs, up to the maximum length. First part of the IE data,
   *          up to the amount of bytes specified in BeaconIeDataLength, is added to the Beacon
   *          frames. The remaining part whose length is defined in ProbeRespIeDataLength is added
   *          to the Probe Response frames.
   *          <BR>See @ref WFM_CONCEPT_VENDOR_IE for further details.
   */
  uint8_t  ie_data[];
} sl_wfx_start_ap_req_body_t;

/**
 * @brief Request message for starting a Wi-Fi network.
 * @details The host can use this request to initiate a Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | No              |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_start_ap_req_body_t body;
} sl_wfx_start_ap_req_t;

/**
 * @brief Confirmation message body for sl_wfx_start_ap_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_cnf_body_s {
  /**
   * @brief Status of the start request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the start request was accepted. It will be completed by sl_wfx_start_ap_ind_t.
   *          <BR><B>any other value</B>: the start request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_start_ap_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_start_ap_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_start_ap_cnf_body_t body;
} sl_wfx_start_ap_cnf_t;

/**
 * @brief Indication message body for sl_wfx_start_ap_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_ind_body_s {
  /**
   * @brief Status of the start request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the start request was completed successfully.
   *          <BR><B>any other value</B>: the start request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_start_ap_ind_body_t;

/**
 * @brief Indication message used to signal the completion of a start request.
 * @details The device will send this indication to signal the start request initiated
 *          with sl_wfx_start_ap_req_t has been completed.
 * @ingroup WFM_GROUP_MODE_IDLE
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_ap_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_start_ap_ind_body_t body;
} sl_wfx_start_ap_ind_t;

/**
 * @brief Request message body for sl_wfx_update_ap_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_update_ap_req_body_s {
  /**
   * @brief Length of vendor-specific Information Element (IE) data in 802.11 Beacon frames.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t beacon_ie_data_length;
  /**
   * @brief Length of vendor-specific Information Element (IE) data in 802.11 Probe Response frames.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t probe_resp_ie_data_length;
  /**
   * @brief Vendor-specific IE data added to the 802.11 Beacon frames and Probe Response frames.
   * @details The IE data must be in 802.11 Vendor-Specific Element format. It may contain
   *          multiple concatenated IEs, up to the maximum length. First part of the IE data,
   *          up to the amount of bytes specified in BeaconIeDataLength, is added to the Beacon
   *          frames. The remaining part whose length is defined in ProbeRespIeDataLength is added
   *          to the Probe Response frames.
   *          <BR>See @ref WFM_CONCEPT_VENDOR_IE for further details.
   */
  uint8_t  ie_data[];
} sl_wfx_update_ap_req_body_t;

/**
 * @brief Request message for updating parameters of the started Wi-Fi network.
 * @details The host can use this request to update parameters of the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_update_ap_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_update_ap_req_body_t body;
} sl_wfx_update_ap_req_t;

/**
 * @brief Confirmation message body for sl_wfx_update_ap_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_update_ap_cnf_body_s {
  /**
   * @brief Status of the update request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the update request was completed.
   *          <BR><B>any other value</B>: the update request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_update_ap_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_update_ap_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_update_ap_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_update_ap_cnf_body_t body;
} sl_wfx_update_ap_cnf_t;

/**
 * @struct sl_wfx_stop_ap_req_t
 * @brief Request message for stopping the started Wi-Fi network.
 * @details The host can use this request to stop the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef sl_wfx_header_t sl_wfx_stop_ap_req_t;

/**
 * @brief Confirmation message body for sl_wfx_stop_ap_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_stop_ap_cnf_body_s {
  /**
   * @brief Status of the stop request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the stop request was accepted. It will be completed by sl_wfx_stop_ap_ind_t.
   *          <BR><B>any other value</B>: the stop request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_stop_ap_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_stop_ap_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_stop_ap_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_stop_ap_cnf_body_t body;
} sl_wfx_stop_ap_cnf_t;

/**
 * @struct sl_wfx_stop_ap_ind_t
 * @brief Indication message used to signal the completion of a stop operation.
 * @details The device will send this indication to signal the stop request initiated
 *          with sl_wfx_stop_ap_req_t has been completed. The indication is also sent when
 *          the started network has encountered a fatal error.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef sl_wfx_header_t sl_wfx_stop_ap_ind_t;

/**
 * @brief Indication message body for sl_wfx_ap_client_connected_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_connected_ind_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_ap_client_connected_ind_body_t;

/**
 * @brief Indication message used to signal a connected station.
 * @details The device will send this indication to signal a station has connected
 *          to the started network.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_connected_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_ap_client_connected_ind_body_t body;
} sl_wfx_ap_client_connected_ind_t;

/**
 * @brief Indication message body for sl_wfx_ap_client_rejected_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_rejected_ind_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief Reason for rejection.
   * @details <B>WFM_REASON_TIMEOUT</B>: A timeout occurred during a station connection attempt.
   *          <BR><B>WFM_REASON_LEAVING_BSS</B>: The device sent a deauth before completing the connection.
   *          <BR><B>WFM_REASON_UNKNOWN_STA</B>: The device received data from a non-connected station.
   *          <BR><B>WFM_REASON_AP_FULL</B>: The device was not able to accommodate any more stations.
   *          <BR><B>WFM_REASON_AUTHENTICATION_FAILURE</B>: The device did not complete the WPA authentication successfully.
   *          <BR>See wfm_reason for enumeration values.
   */
  uint16_t reason;
} sl_wfx_ap_client_rejected_ind_body_t;

/**
 * @brief Indication message used to signal a rejected connection attempt from a station.
 * @details The device will send this indication to signal a station has attempted
 *          connection to the started network and was rejected by the device.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_rejected_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_ap_client_rejected_ind_body_t body;
} sl_wfx_ap_client_rejected_ind_t;

/**
 * @brief Request message body for sl_wfx_disconnect_ap_client_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ap_client_req_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_disconnect_ap_client_req_body_t;

/**
 * @brief Request message for disconnecting a client from the started Wi-Fi network.
 * @details The host can use this request to disconnect a client from the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ap_client_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_disconnect_ap_client_req_body_t body;
} sl_wfx_disconnect_ap_client_req_t;

/**
 * @brief Confirmation message body for sl_wfx_disconnect_ap_client_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ap_client_cnf_body_s {
  /**
   * @brief Status of the disconnect request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the disconnect request was completed.
   *          <BR><B>any other value</B>: the disconnect request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_disconnect_ap_client_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_disconnect_ap_client_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_disconnect_ap_client_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_disconnect_ap_client_cnf_body_t body;
} sl_wfx_disconnect_ap_client_cnf_t;

/**
 * @brief Indication message body for sl_wfx_ap_client_disconnected_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_disconnected_ind_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief Reason for disconnection.
   * @details <B>WFM_REASON_LEAVING_BSS</B>: The station was disconnected or it disconnected on its own.
   *          <BR>See wfm_reason for enumeration values.
   */
  uint16_t reason;
} sl_wfx_ap_client_disconnected_ind_body_t;

/**
 * @brief Indication message used to signal a disconnected station.
 * @details The device will send this indication to signal a station has left
 *          the started network.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_client_disconnected_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_ap_client_disconnected_ind_body_t body;
} sl_wfx_ap_client_disconnected_ind_t;

/**
 * @brief Request message body for sl_wfx_send_frame_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_send_frame_req_body_s {
  /**
   * @brief Packet type.
   * @details <B>0</B>: Data packet with Ethernet II frame header.
   */
  uint8_t  frame_type;
  /**
   * @brief User Priority level.
   * @details <B>0 - 7</B>: 802.1D Priority field value.
   */
  uint8_t  priority;
  /**
   * @brief Packet ID number.
   * @details <B>0 - 65535</B>: Host-assigned unique number for the packet.
   *          <BR>The number is returned in the corresponding confirmation message.
   */
  uint16_t packet_id;
  /**
   * @brief Length of packet data.
   * @details <B>1 - 1604</B>: The amount of bytes.
   */
  uint32_t packet_data_length;
  /**
   * @brief Data of the packet.
   * @details See @ref WFM_CONCEPT_PACKET for further details.
   */
  uint8_t  packet_data[];
} sl_wfx_send_frame_req_body_t;

/**
 * @brief Request message for sending a packet to the network.
 * @details The host can use this request to send a packet to the network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_send_frame_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_send_frame_req_body_t body;
} sl_wfx_send_frame_req_t;

/**
 * @brief Confirmation message body for sl_wfx_send_frame_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_send_frame_cnf_body_s {
  /**
   * @brief Status of the send request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the send request was completed.
   *          <BR><B>any other value</B>: the send request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief Packet ID number from the corresponding request.
   */
  uint16_t packet_id;
  /**
   * @brief Reserved.
   */
  uint16_t reserved;
} sl_wfx_send_frame_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_send_frame_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_send_frame_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_send_frame_cnf_body_t body;
} sl_wfx_send_frame_cnf_t;

/**
 * @brief Indication message body for sl_wfx_received_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_received_ind_body_s {
  /**
   * @brief Packet type.
   * @details <B>0</B>: Data packet with Ethernet II frame header.
   */
  uint8_t  frame_type;
  /**
   * @brief The amount of padding bytes before the packet data.
   * @details <B>0 - 3</B>: The amount of bytes.
   */
  uint8_t  frame_padding;
  /**
   * @brief Length of packet data excluding the padding bytes.
   * @details <B>1 - 2310</B>: The amount of bytes.
   */
  uint16_t frame_length;
  /**
   * @brief Packet data, including the padding bytes before the packet data.
   * @details See @ref WFM_CONCEPT_PACKET for further details.
   */
  uint8_t  frame[];
} sl_wfx_received_ind_body_t;

/**
 * @brief Indication message used to signal a received packet.
 * @details The device will send this indication to signal a packet
 *          has been received.
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_received_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_received_ind_body_t body;
} sl_wfx_received_ind_t;

/**
 * @brief Request message body for sl_wfx_start_scan_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_scan_req_body_s {
  /**
   * @brief Scan mode.
   * @details <B>WFM_SCAN_MODE_PASSIVE</B>: The device will perform a passive scan.
   *          <BR><B>WFM_SCAN_MODE_ACTIVE</B>: The device will perform an active scan.
   *          <BR>See wfm_scan_mode for enumeration values.
   *          <BR>See @ref WFM_CONCEPT_SCAN for further details.
   */
  uint16_t scan_mode;
  /**
   * @brief The amount of specific channels to scan.
   * @details <B>0</B>: The device will scan all channels
   *          <BR><B>1 - 13</B>: The amount of specific channels to scan.
   */
  uint16_t channel_list_count;
  /**
   * @brief The amount of specific SSIDs to scan.
   * @details <B>0</B>: The device will perform a broadcast scan
   *          <BR><B>1 - 2</B>: The amount of specific SSIDs to scan.
   */
  uint16_t ssid_list_count;
  /**
   * @brief Length of vendor-specific Information Element (IE) data in 802.11 Probe Request frames.
   * @details <B>0 - 255</B>: The amount of bytes.
   */
  uint16_t ie_data_length;
  /**
   * @brief Data for the variable-length scan parameters.
   * @details The variable-length scan parameters are concatenated together without any
   *          padding between the different parameters.
   * @par 1. List of specific channels to scan.
   * @details <B>1 - 13</B>: The channel number to scan per byte.
   *          <BR>Must contain the same number of channels as specified in ChannelListCount.
   *          <BR>See @ref WFM_CONCEPT_SCAN for further details.
   * @par 2. List of specific SSIDs to scan.
   * @details Must contain the same number of WfmHiSsidDef_t elements as specified in SsidListCount.
   *          <BR>This parameter is applicable only for an active scan. See @ref WFM_CONCEPT_SCAN for further details.
   * @par 3. Vendor-specific IE data added to the 802.11 Probe Request frames.
   * @details The IE data must be in 802.11 Vendor-Specific Element format.
   *          It may contain multiple concatenated IEs, up to the maximum length.
   *          <BR>This parameter is applicable only for an active scan. See @ref WFM_CONCEPT_VENDOR_IE for further details.
   * @par 4. Basic Service Set Identifier (BSSID) to scan.
   * @details <B>broadcast address</B>: The device will send Probe Request frames to all devices.
   *          <BR><B>unicast address</B>: The device will send Probe Request frames to a specific device.
   *          <BR>This parameter is applicable only for an active scan. If not specified, broadcast address is assumed.
   *          <BR>Note that an SSID list must be specified when using the unicast address, otherwise the received
   *          scan results may contain unexpected additional networks.
   *          <BR>See @ref WFM_CONCEPT_BSSID for further details.
   */
  uint8_t  data[];
} sl_wfx_start_scan_req_body_t;

/**
 * @brief Request message for starting a scan to detect near-by access points.
 * @details The host can use this request to start a scan operation to detect near-by access points.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_scan_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_start_scan_req_body_t body;
} sl_wfx_start_scan_req_t;

/**
 * @brief Confirmation message body for sl_wfx_start_scan_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_scan_cnf_body_s {
  /**
   * @brief Status of the scan request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the scan request was accepted. It will be completed by sl_wfx_scan_complete_ind_t.
   *          <BR><B>any other value</B>: the scan request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_start_scan_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_start_scan_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_start_scan_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_start_scan_cnf_body_t body;
} sl_wfx_start_scan_cnf_t;

/**
 * @struct sl_wfx_stop_scan_req_t
 * @brief Request message for stopping an ongoing scan.
 * @details The host can use this request to stop an ongoing scan operation.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | No              |
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef sl_wfx_header_t sl_wfx_stop_scan_req_t;

/**
 * @brief Confirmation message body for sl_wfx_stop_scan_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_stop_scan_cnf_body_s {
  /**
   * @brief Status of the stop request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the stop request was accepted. It will be completed by WFM_HI_SCAN_COMPLETE_IND.
   *          <BR><B>any other value</B>: the stop request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_stop_scan_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_stop_scan_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_stop_scan_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_stop_scan_cnf_body_t body;
} sl_wfx_stop_scan_cnf_t;

/**
 * @brief Indication message body for sl_wfx_scan_result_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_scan_result_ind_body_s {
  /**
   * @brief Service Set Identifier (SSID) of the network.
   */
  sl_wfx_ssid_def_t ssid_def;
  /**
   * @brief MAC address of the access point.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief Channel of the access point.
   * @details <B>1 - 13</B>: Channel number.
   */
  uint16_t channel;
  /**
   * @brief Security Capabilities of the network.
   */
  sl_wfx_security_mode_bitmask_t security_mode;
  /**
   * @brief Reserved.
   */
  uint8_t  reserved1;
  /**
   * @brief Reserved.
   */
  uint16_t reserved2;
  /**
   * @brief Received Channel Power Indicator (RCPI) of the access point.
   * @details See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint16_t rcpi;
  /**
   * @brief Length of access point Information Element (IE) data in bytes.
   */
  uint16_t ie_data_length;
  /**
   * @brief Access point IE data from the 802.11 Beacon or Probe Response frame.
   */
  uint8_t  ie_data[];
} sl_wfx_scan_result_ind_body_t;

/**
 * @brief Indication message used to signal an access point has been detected.
 * @details The device will send this indication to signal an access point has
 *          has been detected during the scan operation.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_scan_result_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_scan_result_ind_body_t body;
} sl_wfx_scan_result_ind_t;

/**
 * @brief Indication message body for sl_wfx_scan_complete_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_scan_complete_ind_body_s {
  /**
   * @brief Status of the scan request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the scan request was completed.
   *          <BR><B>any other value</B>: the scan request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_scan_complete_ind_body_t;

/**
 * @brief Indication message used to signal a scan was completed.
 * @details The device will send this indication to signal a scan operation
 *          has been completed.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_scan_complete_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_scan_complete_ind_body_t body;
} sl_wfx_scan_complete_ind_t;

/**
 * @brief Request message body for sl_wfx_add_multicast_addr_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_multicast_addr_req_body_s {
  /**
   * @brief MAC address to add.
   * @details <B>broadcast address</B>: The device will empty the whitelist and allow all multicast addresses.
   *          <BR><B>zero address</B>: The device will empty the whitelist and deny all multicast addresses.
   *          <BR><B>multicast address</B>: The device will add the given address to the whitelist.
   *          <BR>See @ref WFM_CONCEPT_BSSID for further details.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_add_multicast_addr_req_body_t;

/**
 * @brief Request message for adding a multicast address to the multicast filter whitelist.
 * @details The host can use this request to add a multicast address to the multicast filter
 *          whitelist. When the first address is added the whitelist, the device will discard
 *          all multicast frames whose destination address does not match any of the addresses
 *          on the list. The default state is to allow all multicast addresses. The whitelist
 *          is reset to the default state when the interface is reset. Up to 8 multicast addresses
 *          may be added.
 *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_multicast_addr_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_add_multicast_addr_req_body_t body;
} sl_wfx_add_multicast_addr_req_t;

/**
 * @brief Confirmation message body for sl_wfx_add_multicast_addr_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_multicast_addr_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the add request was completed successfully.
   *          <BR><B>any other value</B>: the add request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_add_multicast_addr_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_add_multicast_addr_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_multicast_addr_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_add_multicast_addr_cnf_body_t body;
} sl_wfx_add_multicast_addr_cnf_t;

/**
 * @brief Request message body for sl_wfx_remove_multicast_addr_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_remove_multicast_addr_req_body_s {
  /**
   * @brief MAC address to remove.
   * @details <B>multicast address</B>: The device will remove the given address from the whitelist.
   *          <BR>See @ref WFM_CONCEPT_BSSID for further details.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_remove_multicast_addr_req_body_t;

/**
 * @brief Request message for removing a multicast address from the multicast filter whitelist.
 * @details The host can use this request to remove a multicast address from the multicast filter
 *          whitelist.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_remove_multicast_addr_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_remove_multicast_addr_req_body_t body;
} sl_wfx_remove_multicast_addr_req_t;

/**
 * @brief Confirmation message body for sl_wfx_remove_multicast_addr_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_remove_multicast_addr_cnf_body_s {
  /**
   * @brief Status of the remove request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the remove request was completed successfully.
   *          <BR><B>any other value</B>: the remove request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_remove_multicast_addr_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_remove_multicast_addr_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_remove_multicast_addr_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_remove_multicast_addr_cnf_body_t body;
} sl_wfx_remove_multicast_addr_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_max_ap_client_count_req_t.
 * @details
 *          | Field     | Default value | Reset to default value |
 *          |:----------|:--------------|:-----------------------|
 *          | Count     | 8             | when interface stopped |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_count_req_body_s {
  /**
   * @brief Maximum number of connected clients.
   * @details <B>0</B>: The device will set the limit value to the device default.
   *          <BR><B>1 - 8</B>: The maximum number of connected clients.
   */
  uint32_t count;
} sl_wfx_set_max_ap_client_count_req_body_t;

/**
 * @brief Request message for setting the maximum number of connected clients.
 * @details The host can use this request to limit the number of stations that
 *          can connect the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_count_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_max_ap_client_count_req_body_t body;
} sl_wfx_set_max_ap_client_count_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_max_ap_client_count_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_count_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_max_ap_client_count_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_max_ap_client_count_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_count_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_max_ap_client_count_cnf_body_t body;
} sl_wfx_set_max_ap_client_count_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_max_ap_client_inactivity_req_t.
 * @details
 *          | Field             | Default value | Reset to default value |
 *          |:------------------|:--------------|:-----------------------|
 *          | inactivity_timeout | 65            | when interface stopped |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_inactivity_req_body_s {
  /**
   * @brief Maximum amount of client idle time.
   * @details <B>0</B>: The device will set the limit value to the device default.
   *          <BR><B>1 - 240</B>: The maximum number of seconds.
   */
  uint32_t inactivity_timeout;
} sl_wfx_set_max_ap_client_inactivity_req_body_t;

/**
 * @brief Request message for setting the maximum number of connected clients.
 * @details The host can use this request to limit the number of stations that
 *          can connect the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_inactivity_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_max_ap_client_inactivity_req_body_t body;
} sl_wfx_set_max_ap_client_inactivity_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_max_ap_client_count_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_inactivity_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_max_ap_client_inactivity_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_max_ap_client_inactivity_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_ap_client_inactivity_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_max_ap_client_inactivity_cnf_body_t body;
} sl_wfx_set_max_ap_client_inactivity_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_roam_parameters_req_t.
 * @details
 *          | Field            | Default value    | Reset to default value |
 *          |:-----------------|:-----------------|:-----------------------|
 *          | rcpi_threshold    | 60               | when device reset      |
 *          | rcpi_hysteresis   | 120              | when device reset      |
 *          | beacon_lost_count  | 10               | when device reset      |
 *          | channel_list_count | 13               | when device reset      |
 *          | channel_number    | 1 to 13          | when device reset      |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_roam_parameters_req_body_s {
  /**
   * @brief Received Channel Power Indicator (RCPI) limit for a roaming attempt.
   * @details <B>0</B>: The device will set the limit value to the device default.
   *          <BR><B>1 - 220</B>: RCPI limit for a roaming attempt.
   *          <BR>See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint8_t  rcpi_threshold;
  /**
   * @brief Upper RCPI limit that must be attained before triggering roaming again.
   * @details <B>0</B>: The device will set the limit value to the device default.
   *          <BR><B>1 - 220</B>: RCPI limit to re-enable automatic roaming based on RCPI.
   *          This value is used to prevent automatic roaming from triggering again and again
   *          while the signal strength stays close to the rcpi_threshold value.
   *          rcpi_hysteresis should be set to a value grater than rcpi_threshold.
   *          <BR>See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint8_t  rcpi_hysteresis;
  /**
   * @brief Beacon loss limit for a roaming attempt.
   * @details <B>0</B>: The device will set the limit value to the device default.
   *          <BR><B>1 - 255</B>: Beacon loss limit for a roaming attempt.
   */
  uint8_t  beacon_lost_count;
  /**
   * @brief The amount of specific channels to scan.
   * @details <B>0</B>: The device will set the channel list to the device default.
   *          <BR><B>1 - 13</B>: The amount of specific channels to scan.
   */
  uint8_t  channel_list_count;
  /**
   * @brief List of specific channels to scan.
   * @details <B>1 - 13</B>: The channel number to scan per byte.
   *          <BR>Must contain the same number of channels as specified in channel_list_count.
   *          <BR>See @ref WFM_CONCEPT_SCAN for further details.
   */
  uint8_t  channel_number[];
} sl_wfx_set_roam_parameters_req_body_t;

/**
 * @brief Request message for setting the roaming parameters.
 * @details The host can use this request to configure the roaming parameters
 *          used by the device.
 *          <BR>See @ref WFM_CONCEPT_ROAM for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_roam_parameters_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_roam_parameters_req_body_t body;
} sl_wfx_set_roam_parameters_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_roam_parameters_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_roam_parameters_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_roam_parameters_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_roam_parameters_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_roam_parameters_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_roam_parameters_cnf_body_t body;
} sl_wfx_set_roam_parameters_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_tx_rate_parameters_req_t.
 * @details
 *          | Field          | Default value                 | Reset to default value |
 *          |:---------------|:------------------------------|:-----------------------|
 *          | rate_set_bitmask | all 802.11bg rates, MCS 0 - 7 | when device reset      |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_tx_rate_parameters_req_body_s {
  /**
   * @brief Reserved, set to zero.
   */
  uint16_t reserved;
  /**
   * @brief Set to 1 to use Minstrel rate algorithm
   */
  uint16_t use_minstrel;
  /**
   * @brief TX rate set parameters.
   */
  sl_wfx_rate_set_bitmask_t rate_set_bitmask;
} sl_wfx_set_tx_rate_parameters_req_body_t;

/**
 * @brief Request message for setting the TX rate set parameters.
 * @details The host can use this request to configure the TX rate
 *          set parameters of the device. The rate set determines
 *          what data rates will be used by the device to transmit data
 *          frames.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_tx_rate_parameters_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_tx_rate_parameters_req_body_t body;
} sl_wfx_set_tx_rate_parameters_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_tx_rate_parameters_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_tx_rate_parameters_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_tx_rate_parameters_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_tx_rate_parameters_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_tx_rate_parameters_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_tx_rate_parameters_cnf_body_t body;
} sl_wfx_set_tx_rate_parameters_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_arp_ip_address_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_arp_ip_address_req_body_s {
  /**
   * @brief List of offloaded ARP IP addresses.
   * @details The device will automatically reply to an ARP request that matches
   *          one the addresses on the list. Note that addresses not in use must
   *          be set to zero. Offloading is disabled by setting all addresses to
   *          zero.
   *          <BR>See @ref WFM_CONCEPT_OFFLOADING for further details.
   */
  uint32_t arp_ip_addr[SL_WFX_ARP_IP_ADDR_SIZE];
} sl_wfx_set_arp_ip_address_req_body_t;

/**
 * @brief Request message for setting the Address Resolution Protocol (ARP) offloading state.
 * @details The host can use this request to offload handling of ARP requests to the device.
 *          When offloading is enabled, the device will automatically respond to ARP requests
 *          with an ARP reply. By default offloading is disabled. Offloading is reset to
 *          the default state when the interface is reset.
 *          <BR>See @ref WFM_CONCEPT_OFFLOADING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_arp_ip_address_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_arp_ip_address_req_body_t body;
} sl_wfx_set_arp_ip_address_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_arp_ip_address_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_arp_ip_address_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_arp_ip_address_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_arp_ip_address_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_arp_ip_address_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_arp_ip_address_cnf_body_t body;
} sl_wfx_set_arp_ip_address_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_ns_ip_address_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_ns_ip_address_req_body_s {
  /**
   * @brief List of offloaded NS IP addresses.
   * @details The device will automatically reply to a NS that matches one the
   *          addresses on the list. Note that addresses not in use must be set
   *          to zero. Offloading is disabled by setting all addresses to zero.
   *          <BR>See @ref WFM_CONCEPT_OFFLOADING for further details.
   */
  sl_wfx_ns_ip_addr_t ns_ip_addr[SL_WFX_NS_IP_ADDR_SIZE];
} sl_wfx_set_ns_ip_address_req_body_t;

/**
 * @brief Request message for setting the Neighbor Discovery Protocol (NDP) offloading state.
 * @details The host can use this request to offload handling of IPv6 Neighbor Solicitations
 *          to the device. When offloading is enabled, the device will automatically respond
 *          to a solicitation with a Neighbor Advertisement.  By default offloading is disabled.
 *          Offloading is reset to the default state when the interface is reset.
 *          <BR>See @ref WFM_CONCEPT_OFFLOADING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_ns_ip_address_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_ns_ip_address_req_body_t body;
} sl_wfx_set_ns_ip_address_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_ns_ip_address_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_ns_ip_address_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_ns_ip_address_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_ns_ip_address_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_ns_ip_address_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_ns_ip_address_cnf_body_t body;
} sl_wfx_set_ns_ip_address_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_broadcast_filter_req_t.
 * @details
 *          | Field  | Default value | Reset to default value |
 *          |:-------|:--------------|:-----------------------|
 *          | Filter | 0             | when interface stopped |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_broadcast_filter_req_body_s {
  /**
   * @brief Boolean option for broadcast filtering.
   * @details <B>0</B>: The device will forward all received broadcast frames to the host.
   *          <BR><B>1</B>: The device will only forward ARP and DHCP frames to the host,
   *                        other broadcast frames are discarded.
   *          <BR><B>2</B>: The device will discard all broadcast frames.
   *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
   */
  uint32_t filter;
} sl_wfx_set_broadcast_filter_req_body_t;

/**
 * @brief Request message for setting broadcast filter state.
 * @details The host can use this request to configure the state of the broadcast filter.
 *          When enabled, the device will only forward certain broadcast frames to the
 *          host and automatically discard the rest.
 *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_broadcast_filter_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_broadcast_filter_req_body_t body;
} sl_wfx_set_broadcast_filter_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_broadcast_filter_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_broadcast_filter_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_broadcast_filter_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_broadcast_filter_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_broadcast_filter_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_broadcast_filter_cnf_body_t body;
} sl_wfx_set_broadcast_filter_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_scan_parameters_req_t.
 * @details
 *          | Field              | Default value    | Reset to default value |
 *          |:-------------------|:-----------------|:-----------------------|
 *          | active_channel_time  | 50               | when device reset      |
 *          | passive_channel_time | 110              | when device reset      |
 *          | num_of_probe_requests | 1                | when device reset      |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_scan_parameters_req_body_s {
  /**
   * @brief Set to 0 for device default, 1 - 550 TUs
   */
  uint16_t active_channel_time;
  /**
   * @brief Set to 0 for device default, 1 - 550 TUs
   */
  uint16_t passive_channel_time;
  /**
   * @brief Set to 0 for device default, 1 - 2
   */
  uint16_t num_of_probe_requests;
  /**
   * @brief Reserved, set to zero.
   */
  uint16_t reserved;
} sl_wfx_set_scan_parameters_req_body_t;

/**
 * @brief Request message for setting scan parameters.
 * @details The host can use this request to configure the scan parameters
 *          used by the device.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | No              |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_scan_parameters_req_s {
  sl_wfx_header_t header;
  sl_wfx_set_scan_parameters_req_body_t body;
} sl_wfx_set_scan_parameters_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_scan_parameters_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_scan_parameters_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_scan_parameters_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_scan_parameters_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_scan_parameters_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_set_scan_parameters_cnf_body_t body;
} sl_wfx_set_scan_parameters_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_unicast_filter_req_t.
 * @details
 *          | Field  | Default value | Reset to default value |
 *          |:-------|:--------------|:-----------------------|
 *          | Filter | 1             | when interface stopped |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_unicast_filter_req_body_s {
  /**
   * @brief Boolean option for unicast filtering.
   * @details <B>0</B>: The device will forward all received unicast frames to the host.
   *          <BR><B>1</B>: The device will only forward unicast frames whose destination
   *                        address matches the device MAC address.
   *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
   */
  uint32_t filter;
} sl_wfx_set_unicast_filter_req_body_t;

/**
 * @brief Request message for setting unicast filter state.
 * @details The host can use this request to configure the state of the unicast filter.
 *          When enabled, the device will only forward certain unicast frames to the
 *          host and automatically discard the rest.
 *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_unicast_filter_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_unicast_filter_req_body_t body;
} sl_wfx_set_unicast_filter_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_unicast_filter_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_unicast_filter_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_unicast_filter_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_unicast_filter_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_unicast_filter_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_unicast_filter_cnf_body_t body;
} sl_wfx_set_unicast_filter_cnf_t;

/**
 * @brief Request message body for sl_wfx_add_whitelist_addr_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_whitelist_addr_req_body_s {
  /**
   * @brief MAC address to add.
   * @details <B>broadcast address</B>: The device will allow all MAC addresses.
   *          <BR><B>unicast address</B>: The device will add the given address to the whitelist.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_add_whitelist_addr_req_body_t;

/**
 * @brief Request message for adding a MAC address to the device whitelist.
 * @details The host can use this request to add a MAC address to the list of allowed MAC addresses.
 *          When the first address is added the whitelist, the device will prevent communication with
 *          any devices whose MAC address does not match any of the addresses on the list. Setting a
 *          whitelist will clear all entries from the device blacklist. The default state is to allow
 *          all MAC addresses. The whitelist is reset to the default state when the interface is reset.
 *          Up to 8 MAC addresses may be added.
 *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_whitelist_addr_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_add_whitelist_addr_req_body_t body;
} sl_wfx_add_whitelist_addr_req_t;

/**
 * @brief Confirmation message body for sl_wfx_add_whitelist_addr_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_whitelist_addr_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the add request was completed successfully.
   *          <BR><B>any other value</B>: the add request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_add_whitelist_addr_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_add_whitelist_addr_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_whitelist_addr_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_add_whitelist_addr_cnf_body_t body;
} sl_wfx_add_whitelist_addr_cnf_t;

/**
 * @brief Request message body for sl_wfx_add_blacklist_addr_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_blacklist_addr_req_body_s {
  /**
   * @brief MAC address to add.
   * @details <B>broadcast address</B>: The device will deny all MAC addresses.
   *          <BR><B>unicast address</B>: The device will add the given address to the blacklist.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_add_blacklist_addr_req_body_t;

/**
 * @brief Request message for adding a MAC address to the device blacklist.
 * @details The host can use this request to add a MAC address to the list of denied MAC addresses.
 *          When the first address is added the blacklist, the device will prevent communication with
 *          any device whose MAC address matches any of the addresses on the list. Setting a blacklist
 *          will clear all entries from the device whitelist. The default state is to allow all MAC
 *          addresses. The blacklist is reset to the default state when the interface is reset.
 *          Up to 8 MAC addresses may be added.
 *          <BR>See @ref WFM_CONCEPT_FILTERING for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_blacklist_addr_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_add_blacklist_addr_req_body_t body;
} sl_wfx_add_blacklist_addr_req_t;

/**
 * @brief Confirmation message body for sl_wfx_add_blacklist_addr_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_blacklist_addr_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the add request was completed successfully.
   *          <BR><B>any other value</B>: the add request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_add_blacklist_addr_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_add_blacklist_addr_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_add_blacklist_addr_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_add_blacklist_addr_cnf_body_t body;
} sl_wfx_add_blacklist_addr_cnf_t;

/**
 * @brief Request message body for sl_wfx_set_max_tx_power_req_t.
 * @details
 *          | Field      | Default value    | Reset to default value |
 *          |:-----------|:-----------------|:-----------------------|
 *          | max_tx_power | device dependent | when device reset      |
 *          <BR>
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_tx_power_req_body_s {
  /**
   * @brief Maximum transmit power to set
   * @details Expressed in units of 0.1d_bm.
   */
  int32_t  max_tx_power;
} sl_wfx_set_max_tx_power_req_body_t;

/**
 * @brief Request message for setting the maximum transmit power of the device.
 * @details The host can use this request to set a maximum transmit power.
 *          The device can still use a lower transmit power depending on regulatory rules
 *          or internal limitations.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_tx_power_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_set_max_tx_power_req_body_t body;
} sl_wfx_set_max_tx_power_req_t;

/**
 * @brief Confirmation message body for sl_wfx_set_max_tx_power_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_tx_power_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the add request was completed successfully.
   *          <BR><B>any other value</B>: the add request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_set_max_tx_power_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_set_max_tx_power_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_max_tx_power_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_set_max_tx_power_cnf_body_t body;
} sl_wfx_set_max_tx_power_cnf_t;

/**
 * @struct sl_wfx_get_max_tx_power_req_t
 * @brief Request message for getting the maximum transmit power
 * @details The host can use this request to get the current value of maximum transmit power.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | Yes             |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef sl_wfx_header_t sl_wfx_get_max_tx_power_req_t;

/**
 * @brief Confirmation message body for sl_wfx_get_max_tx_power_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_max_tx_power_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the add request was completed successfully.
   *          <BR><B>any other value</B>: the add request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief Maximum transmit power for RF port 1.
   * @details Expressed in units of 0.1d_bm.
   */
  int32_t  max_tx_power_rf_port1;
  /**
   * @brief Maximum transmit power for RF port 2.
   * @details Expressed in units of 0.1d_bm.
   */
  int32_t  max_tx_power_rf_port2;
} sl_wfx_get_max_tx_power_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_get_max_tx_power_req_t.
 * @ingroup WFM_GROUP_MODE_IDLE
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_max_tx_power_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_get_max_tx_power_cnf_body_t body;
} sl_wfx_get_max_tx_power_cnf_t;

/**
 * @struct sl_wfx_get_pmk_req_t
 * @brief Request message for getting the Pairwise Master Key (PMK).
 * @details The host can use this request to retrieve the Pairwise Master Key (PMK)
 *          used to connect to the current secure network. PMK can be given
 *          in the next sl_wfx_connect_req_t message to speed up connection
 *          process. See @ref WFM_CONCEPT_PASSWORD for further details.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | Yes             |
 *          | AP             | Yes             |
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef sl_wfx_header_t sl_wfx_get_pmk_req_t;

/**
 * @brief Confirmation message body for sl_wfx_get_pmk_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_pmk_cnf_body_s {
  /**
   * @brief Status of the add request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the get request was completed successfully.
   *          <BR><B>any other value</B>: the get request failed.
   *          <BR>See ::sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief Length of the Pairwise Master Key.
   * @details <B>0 - 64</B>: The amount of bytes.
   */
  uint32_t password_length;
  /**
   * @brief PMK of the network.
   * @details <B>WPA PMK</B>: 64 bytes in HEX format.
   *          <BR>See @ref WFM_CONCEPT_PASSWORD for further details.
   */
  uint8_t  password[SL_WFX_PASSWORD_SIZE];
} sl_wfx_get_pmk_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_get_pmk_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_pmk_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_get_pmk_cnf_body_t body;
} sl_wfx_get_pmk_cnf_t;

/**
 * @brief Request message body for sl_wfx_get_ap_client_signal_strength_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_ap_client_signal_strength_req_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
} sl_wfx_get_ap_client_signal_strength_req_body_t;

/**
 * @brief Request message for retrieving the signal strength of a client of the started Wi-Fi network.
 * @details The host can use this request to retrieve the signal strength of a client of the started Wi-Fi network.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_ap_client_signal_strength_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_get_ap_client_signal_strength_req_body_t body;
} sl_wfx_get_ap_client_signal_strength_req_t;

/**
 * @brief Confirmation message body for sl_wfx_get_ap_client_signal_strength_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_ap_client_signal_strength_cnf_body_s {
  /**
   * @brief Status of the get request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the get request was completed.
   *          <BR><B>any other value</B>: the get request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
  /**
   * @brief Received Channel Power Indicator (RCPI) of the client.
   * @details See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint32_t rcpi;
} sl_wfx_get_ap_client_signal_strength_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_get_ap_client_signal_strength_req_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_get_ap_client_signal_strength_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_get_ap_client_signal_strength_cnf_body_t body;
} sl_wfx_get_ap_client_signal_strength_cnf_t;

/**
 * @brief Request message body for sl_wfx_ext_auth_req_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ext_auth_req_body_s {
  /**
   * @brief Type of the authentication message
   * @details See ::sl_wfx_ext_auth_data_type_t for enumeration values.
   */
  uint16_t auth_data_type;
  /**
   * @brief Length of the authentication message
   */
  uint16_t auth_data_length;
  /**
   * @brief The authentication message
   */
  uint8_t auth_data[];
} sl_wfx_ext_auth_req_body_t;

/**
 */
typedef struct __attribute__((__packed__)) sl_wfx_ext_auth_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_ext_auth_req_body_t body;
} sl_wfx_ext_auth_req_t;

/**
 * @brief Confirmation message body for sl_wfx_ext_auth_cnf_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ext_auth_cnf_body_s {
  /**
   * @brief Status of the update request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the authentication request was completed.
   *          <BR><B>any other value</B>: the authentication request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_ext_auth_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_ext_auth_req_t.
 * @ingroup WFM_GROUP_MODE_STA
 */
typedef struct __attribute__((__packed__)) sl_wfx_ext_auth_cnf_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_ext_auth_cnf_body_t body;
} sl_wfx_ext_auth_cnf_t;

/**
 * @brief Indication message body for sl_wfx_ext_auth_ind_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ext_auth_ind_body_s {
  /**
   * @brief Type of the authentication message
   * @details See ::sl_wfx_ext_auth_data_type_t for enumeration values.
   */
  uint16_t auth_data_type;
  /**
   * @brief Length of the authentication message
   */
  uint16_t auth_data_length;
  /**
   * @brief The authentication message
   */
  uint8_t auth_data[];
} sl_wfx_ext_auth_ind_body_t;

/**
 */
typedef struct __attribute__((__packed__))  sl_wfx_ext_auth_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_ext_auth_ind_body_t body;
} sl_wfx_ext_auth_ind_t;

/**
 * @brief Request message body for sl_wfx_ap_scan_filter_req_body_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_req_body_s {
  /**
   * @brief Length of vendor-specific Information Element (IE) byte pattern.
   * @details <B>0</B>: The filter is disabled
   *          <BR><B>1 - SL_WFX_IE_DATA_FILTER_SIZE</B>: The amount of bytes.
   */
  uint16_t ie_data_length;
  /**
   * @brief Vendor-specific IE byte pattern, starting from the Organization
   *        Identifier field.
   */
  uint8_t ie_data[SL_WFX_IE_DATA_FILTER_SIZE];
  /**
   * @brief Vendor-specific IE byte pattern mask.
   * @details The byte pattern mask is ANDed with the frame data before
   *          pattern matching, fill with 0xFF to match the whole pattern.
   */
  uint8_t ie_data_mask[SL_WFX_IE_DATA_FILTER_SIZE];
} sl_wfx_ap_scan_filter_req_body_t;

/**
 * @brief Request message for setting probe request forwarding filter.
 * @details The host can use this request to set the filter for forwarded
 *          probe requests. Any probe request where one the vendor-specific
 *          IEs matches the given filter, is forwarded to the host using a
 *          sl_wfx_ap_scan_filter_ind_t indication message. The forwarding can
 *          be disabled by setting the IE byte pattern length to zero.
 *          | Interface mode | Request allowed |
 *          |:---------------|:----------------|
 *          | idle           | No              |
 *          | station        | No              |
 *          | AP             | Yes             |
 *          <BR>
 * @ingroup WFM_GROUP_MESSAGES
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_req_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Request message body. */
  sl_wfx_ap_scan_filter_req_body_t body;
} sl_wfx_ap_scan_filter_req_t;

/**
 * @brief Confirmation message body for sl_wfx_ap_scan_filter_cnf_body_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_cnf_body_s {
  /**
   * @brief Status of the set request.
   * @details <B>WFM_STATUS_SUCCESS</B>: the set request was completed successfully.
   *          <BR><B>any other value</B>: the set request failed.
   *          <BR>See sl_wfx_fmac_status_t for enumeration values.
   */
  uint32_t status;
} sl_wfx_ap_scan_filter_cnf_body_t;

/**
 * @brief Confirmation message for sl_wfx_ap_scan_filter_cnf_t.
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_cnf_t {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Confirmation message body. */
  sl_wfx_ap_scan_filter_cnf_body_t body;
} sl_wfx_ap_scan_filter_cnf_t;

/**
 * @brief Indication message body for sl_wfx_ap_scan_filter_ind_body_t.
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_ind_body_s {
  /**
   * @brief MAC address of the station.
   */
  uint8_t mac[SL_WFX_MAC_ADDR_SIZE];
  /**
   * @brief reserved field, set to 0
   */
  uint16_t reserved;
  /**
   * @brief Received Channel Power Indicator (RCPI) of the station.
   * @details See @ref WFM_CONCEPT_RCPI for further details.
   */
  uint16_t rcpi;
  /**
   * @brief Length of access point Information Element (IE) data in bytes.
   */
  uint16_t ie_data_length;
  /**
   * @brief Station IE data from the 802.11 Probe Request frame.
   */
  uint8_t ie_data[];
} sl_wfx_ap_scan_filter_ind_body_t;

/**
 * @brief Indication message used to signal a received probe request.
 * @details The device will send this indication when a probe request matching
 *          the filter set with sl_wfx_ap_scan_filter_req_t, has been
 *          received.
 * @ingroup WFM_GROUP_MESSAGES
 * @ingroup WFM_GROUP_MODE_AP
 */
typedef struct __attribute__((__packed__)) sl_wfx_ap_scan_filter_ind_s {
  /** Common message header. */
  sl_wfx_header_t header;
  /** Indication message body. */
  sl_wfx_ap_scan_filter_ind_body_t body;
} sl_wfx_ap_scan_filter_ind_t;

/**************************************************/

/**
 * @}
 */

/**
 * @}
 */

#endif /* _SL_WFX_CMD_API_H_ */
