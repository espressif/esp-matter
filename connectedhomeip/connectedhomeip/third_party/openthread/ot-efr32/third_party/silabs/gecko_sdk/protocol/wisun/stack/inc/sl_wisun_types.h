/***************************************************************************//**
 * @file sl_wisun_types.h
 * @brief Wi-SUN types
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_WISUN_TYPES_H
#define SL_WISUN_TYPES_H

#include <stdint.h>
#include "sl_common.h"

/**************************************************************************//**
 * @defgroup SL_WISUN_TYPES Wi-SUN API type definitions
 * @ingroup SL_WISUN_API
 * @{
 *****************************************************************************/

/// Maximum size of the Wi-SUN network name
#define SL_WISUN_NETWORK_NAME_SIZE 32
/// Size of a MAC address
#define SL_WISUN_MAC_ADDRESS_SIZE 8
/// Size of an IPv6 address
#define SL_WISUN_IP_ADDRESS_SIZE 16
/// Size of a channel mask
#define SL_WISUN_CHANNEL_MASK_SIZE 32
/// Size of the filter bitfield
#define SL_WISUN_FILTER_BITFIELD_SIZE ((SL_WISUN_TRACE_GROUP_COUNT + 7) / 8)

/// Enumerations for network size
typedef enum {
  /// Determine the size from PAN advertisements
  SL_WISUN_NETWORK_SIZE_AUTOMATIC     = 0,
  /// Small size (less than 100 nodes)
  SL_WISUN_NETWORK_SIZE_SMALL         = 1,
  /// Medium size (100 to 800 nodes)
  SL_WISUN_NETWORK_SIZE_MEDIUM        = 2,
  /// Large size (800 to 1500 nodes)
  SL_WISUN_NETWORK_SIZE_LARGE         = 3,
  /// Test network (a few nodes)
  SL_WISUN_NETWORK_SIZE_TEST          = 4,
  /// Certification configuration
  SL_WISUN_NETWORK_SIZE_CERTIFICATION = 5
} sl_wisun_network_size_t;

/// Enumerations for IP address type
typedef enum {
  /// Device link-local address
  SL_WISUN_IP_ADDRESS_TYPE_LINK_LOCAL       = 0,
  /// Device global unicast address
  SL_WISUN_IP_ADDRESS_TYPE_GLOBAL           = 1,
  /// Border router global unicast address
  SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER    = 2,
  /// Link-local address of the primary parent
  SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT   = 3,
  /// Link-local address of the secondary parent
  SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT = 4
} sl_wisun_ip_address_type_t;

/// Enumerations for socket protocol
typedef enum {
  /// User Datagram Protocol (UDP)
  SL_WISUN_SOCKET_PROTOCOL_UDP  = 0,
  /// Transmission Control Protocol (TCP)
  SL_WISUN_SOCKET_PROTOCOL_TCP  = 1,
  /// Internet Control Message Protocol (ICMP)
  SL_WISUN_SOCKET_PROTOCOL_ICMP = 2
} sl_wisun_socket_protocol_t;

/// Enumerations for certificate options
typedef enum {
  /// Empty option
  SL_WISUN_CERTIFICATE_OPTION_NONE    = 0,
  /// Certificate is appended to a chain
  SL_WISUN_CERTIFICATE_OPTION_APPEND  = 1,
  /// Certificate data will remain in scope
  SL_WISUN_CERTIFICATE_OPTION_IS_REF  = 2,
  /// Certificate has a private key
  SL_WISUN_CERTIFICATE_OPTION_HAS_KEY = 4
} sl_wisun_certificate_option_t;

/// Enumerations for private key options
typedef enum {
  /// Empty option
  SL_WISUN_PRIVATE_KEY_OPTION_NONE    = 0,
  /// Private key data will remain in scope
  SL_WISUN_PRIVATE_KEY_OPTION_IS_REF  = 1
} sl_wisun_private_key_option_t;

/// Enumerations for socket event mode
typedef enum {
  /// Received data is sent in an indication
  SL_WISUN_SOCKET_EVENT_MODE_INDICATION = 0,
  /// The amount of received data is sent in an indication
  SL_WISUN_SOCKET_EVENT_MODE_POLLING    = 1
} sl_wisun_socket_event_mode_t;

/// Enumerations for socket option
typedef enum {
  /// Option for socket event mode
  SL_WISUN_SOCKET_OPTION_EVENT_MODE = 0,
  /// Option for multicast group
  SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP = 1,
  /// Option for send buffer limit
  SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT = 2
} sl_wisun_socket_option_t;

/// Enumerations for statistics type
typedef enum {
  /// PHY/RF statistics
  SL_WISUN_STATISTICS_TYPE_PHY        = 0,
  /// MAC statistics
  SL_WISUN_STATISTICS_TYPE_MAC        = 1,
  /// Frequency hopping statistics
  SL_WISUN_STATISTICS_TYPE_FHSS       = 2,
  /// Wi-SUN statistics
  SL_WISUN_STATISTICS_TYPE_WISUN      = 3,
  /// 6LoWPAN/IP stack statistics
  SL_WISUN_STATISTICS_TYPE_NETWORK    = 4,
  /// Regional regulation
  SL_WISUN_STATISTICS_TYPE_REGULATION = 5
} sl_wisun_statistics_type_t;

/// Enumerations for regulatory domain
typedef enum {
  /// World-wide (2.4 GHz)
  SL_WISUN_REGULATORY_DOMAIN_WW       = 0,
  /// North America
  SL_WISUN_REGULATORY_DOMAIN_NA       = 1,
  /// Japan
  SL_WISUN_REGULATORY_DOMAIN_JP       = 2,
  /// Europe
  SL_WISUN_REGULATORY_DOMAIN_EU       = 3,
  /// China
  SL_WISUN_REGULATORY_DOMAIN_CN       = 4,
  /// India
  SL_WISUN_REGULATORY_DOMAIN_IN       = 5,
  /// Mexico
  SL_WISUN_REGULATORY_DOMAIN_MX       = 6,
  /// Brazil
  SL_WISUN_REGULATORY_DOMAIN_BZ       = 7,
  /// Australia
  SL_WISUN_REGULATORY_DOMAIN_AZ       = 8, // shared with NZ
  /// New Zealand
  SL_WISUN_REGULATORY_DOMAIN_NZ       = 8, // shared with AZ
  /// South Korea
  SL_WISUN_REGULATORY_DOMAIN_KR       = 9,
  /// Philippines
  SL_WISUN_REGULATORY_DOMAIN_PH       = 10,
  /// Malaysia
  SL_WISUN_REGULATORY_DOMAIN_MY       = 11,
  /// Hong Kong
  SL_WISUN_REGULATORY_DOMAIN_HK       = 12,
  /// Singapore
  SL_WISUN_REGULATORY_DOMAIN_SG       = 13,
  /// Thailand
  SL_WISUN_REGULATORY_DOMAIN_TH       = 14,
  /// Vietnam
  SL_WISUN_REGULATORY_DOMAIN_VN       = 15,
  /// Application-specific domain
  SL_WISUN_REGULATORY_DOMAIN_APP      = 255
} sl_wisun_regulatory_domain_t;

/// Enumerations for operating class
typedef enum {
  /// Operating class# 1
  SL_WISUN_OPERATING_CLASS_1         = 1,
  /// Operating class# 2
  SL_WISUN_OPERATING_CLASS_2         = 2,
  /// Operating class# 3
  SL_WISUN_OPERATING_CLASS_3         = 3,
  /// Operating class# 4
  SL_WISUN_OPERATING_CLASS_4         = 4,
  /// Operating class# 5
  SL_WISUN_OPERATING_CLASS_5         = 5,
  /// Application-specific class
  SL_WISUN_OPERATING_CLASS_APP       = 255
} sl_wisun_operating_class_t;

/// Enumerations for operating mode
typedef enum {
  /// Operating mode# 1a
  SL_WISUN_OPERATING_MODE_1A         = 0x1a,
  /// Operating mode# 1b
  SL_WISUN_OPERATING_MODE_1B         = 0x1b,
  /// Operating mode# 2a
  SL_WISUN_OPERATING_MODE_2A         = 0x2a,
  /// Operating mode# 2b
  SL_WISUN_OPERATING_MODE_2B         = 0x2b,
  /// Operating mode# 3
  SL_WISUN_OPERATING_MODE_3          = 0x03,
  /// Operating mode# 4a
  SL_WISUN_OPERATING_MODE_4A         = 0x4a,
  /// Operating mode# 4b
  SL_WISUN_OPERATING_MODE_4B         = 0x4b,
  /// Operating mode# 5
  SL_WISUN_OPERATING_MODE_5          = 0x05
} sl_wisun_operating_mode_t;

/// Enumerations for multicast group action
typedef enum {
  /// Join a multicast group
  SL_WISUN_MULTICAST_GROUP_ACTION_JOIN  = 0,
  /// Leave a multicast group
  SL_WISUN_MULTICAST_GROUP_ACTION_LEAVE = 1
} sl_wisun_multicast_group_action_t;

/// Enumerations for channel spacing
typedef enum {
  /// 100 kHz
  SL_WISUN_CHANNEL_SPACING_100HZ     = 0,
  /// 200 kHz
  SL_WISUN_CHANNEL_SPACING_200HZ     = 1,
  /// 400 kHz
  SL_WISUN_CHANNEL_SPACING_400HZ     = 2,
  /// 600 kHz
  SL_WISUN_CHANNEL_SPACING_600HZ     = 3
} sl_wisun_channel_spacing_t;

/// Enumerations for join state
typedef enum {
  /// Join state 0: Disconnected
  SL_WISUN_JOIN_STATE_DISCONNECTED       = 0,
  /// Join state 1: Select PAN
  SL_WISUN_JOIN_STATE_SELECT_PAN         = 1,
  /// Join state 2: Authenticate
  SL_WISUN_JOIN_STATE_AUTHENTICATE       = 2,
  /// Join state 3: Acquire PAN config
  SL_WISUN_JOIN_STATE_ACQUIRE_PAN_CONFIG = 3,
  /// Join state 4: Configure routing
  SL_WISUN_JOIN_STATE_CONFIGURE_ROUTING  = 4,
  /// Join state 5: Operational
  SL_WISUN_JOIN_STATE_OPERATIONAL        = 5
} sl_wisun_join_state_t;

/// Enumerations for network update flags
/// It indicates the position of a bit in the flags field
typedef enum {
  /// Global local IP modification flags
  SL_WISUN_NETWORK_UPDATE_FLAGS_GLOBAL_IP        = 0,
  /// Primary Parent modification flags
  SL_WISUN_NETWORK_UPDATE_FLAGS_PRIMARY_PARENT   = 1,
  /// Secondary parent modification flags
  SL_WISUN_NETWORK_UPDATE_FLAGS_SECONDARY_PARENT = 2
} sl_wisun_network_update_flags_t;

/// Wi-SUN Message API common header
SL_PACK_START(1)
typedef struct {
  /// Total length of the message in bytes, this field included
  uint16_t length;
  /// ID (request, confirmation, indication) of the message
  uint8_t id;
  /// Processing metadata for the message
  uint8_t info;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_header_t;
SL_PACK_END()

/// PHY/RF statistics
typedef struct {
  /// Number of CRC failures on reception.
  uint32_t crc_fails;
  /// Number of transmission timeouts.
  uint32_t tx_timeouts;
  /// Number of reception timeouts.
  uint32_t rx_timeouts;
} sl_wisun_statistics_phy_t;

/// MAC statistics
typedef struct {
  /// Current number of frames in the MAC transmission queue.
  uint16_t tx_queue_size;
  /// Highest number of frames in the MAC transmission queue.
  uint16_t tx_queue_peak;
  /// Number of successfully received MAC frames.
  uint32_t rx_count;
  /// Number of transmitted MAC frames.
  uint32_t tx_count;
  /// Number of successfully received broadcast MAC frames.
  uint32_t bc_rx_count;
  /// Number of transmitted broadcast MAC frames.
  uint32_t bc_tx_count;
  /// Number of successfully received MAC frames discarded during processing.
  uint32_t rx_drop_count;
  /// Amount of transmitted MAC data in bytes. FCS is not included.
  uint32_t tx_bytes;
  /// Amount of successfully received MAC data in bytes. FCS is not included.
  uint32_t rx_bytes;
  /// Number of failed MAC transmissions.
  uint32_t tx_failed_count;
  /// Number of retried MAC transmissions.
  uint32_t retry_count;
  /// Number of MAC CCA attempts.
  uint32_t cca_attempts_count;
  /// Number of failed MAC transmissions due to CCA.
  uint32_t failed_cca_count;
  /// Number of successfully received MAC frames using mode_switch.
  uint32_t rx_ms_count;
  /// Number of transmitted MAC frames using mode switch.
  uint32_t tx_ms_count;
  /// Number of failed reception using mode switch (no data after PHR or MDR Command).
  uint32_t rx_ms_failed_count;
  /// Number of failed MAC frames transmission using mode switch.
  uint32_t tx_ms_failed_count;
} sl_wisun_statistics_mac_t;

/// Frequency hopping statistics
typedef struct {
  /// Estimated clock drift to the parent in microseconds.
  int16_t drift_compensation;
  /// Estimated number of hops to the border router based on RPL rank.
  uint16_t hop_count;
  /// Number of seconds since last timing information from the parent.
  uint16_t synch_interval;
  /// Deprecated
  int16_t prev_avg_synch_fix;
  /// Deprecated
  uint32_t synch_lost;
  /// Number of times a transmission attempt has failed due to lack of timing information.
  uint32_t unknown_neighbor;
  /// Number of channel transmission retries.
  uint32_t channel_retry;
} sl_wisun_statistics_fhss_t;

/// Wi-SUN statistics
typedef struct {
  /// Number of received PAN control frames.
  uint32_t pan_control_rx_count;
  /// Number of completed PAN control transmission requests.
  uint32_t pan_control_tx_count;
} sl_wisun_statistics_wisun_t;

/// 6LoWPAN/IP stack statistics
typedef struct {
  /// Number of received IP6 packets.
  uint32_t ip_rx_count;
  /// Number of transmitted IPv6 packets.
  uint32_t ip_tx_count;
  /// Number of discarded IPv6 packets during processing.
  uint32_t ip_rx_drop;
  /// Number of discarded IPv6 packets due to a checksum error.
  uint32_t ip_cksum_error;
  /// Amount of transmitted IPv6 data in bytes.
  uint32_t ip_tx_bytes;
  /// Amount received IPv6 data in bytes.
  uint32_t ip_rx_bytes;
  /// Amount of forwarded IPv6 data in bytes.
  uint32_t ip_routed_up;
  /// Number of discarded IPv6 packets due to lack routing information.
  uint32_t ip_no_route;
  /// Number of fragmentation errors in received IPv6 packets.
  uint32_t frag_rx_errors;
  /// Number of fragmentation errors in transmitted IPv6 packets.
  uint32_t frag_tx_errors;
  /// Number of RPL parent changes due to better route cost.
  uint32_t rpl_route_routecost_better_change;
  /// Number of RPL packet forwarding errors due to inconsistent routing information.
  uint32_t ip_routeloop_detect;
  /// Sum of RPL object sizes that have failed allocation in bytes.
  uint32_t rpl_memory_overflow;
  /// Number of failed RPL transmissions to the parent.
  uint32_t rpl_parent_tx_fail;
  /// Number of discarded RPL packets due to an unknown DODAG instance.
  uint32_t rpl_unknown_instance;
  /// Number of times a local repair procedure has been triggered by the node.
  uint32_t rpl_local_repair;
  /// Number of times a global repair has been triggered by the border router.
  uint32_t rpl_global_repair;
  /// Number of discarded RPL packets due to malformed content.
  uint32_t rpl_malformed_message;
  /// Number of seconds without an RPL parent.
  uint32_t rpl_time_no_next_hop;
  /// Amount of memory currently allocated for RPL objects in bytes.
  uint32_t rpl_total_memory;
  /// Number of data buffer allocation attempts.
  uint32_t buf_alloc;
  /// Number of times data buffers have been resized due to lack of header space.
  uint32_t buf_headroom_realloc;
  /// Number of times data buffers have been reorganized due to lack of header space.
  uint32_t buf_headroom_shuffle;
  /// Number of times data buffer resizing has failed.
  uint32_t buf_headroom_fail;
  /// ETX of the primary parent.
  uint16_t etx_1st_parent;
  /// ETX of the secondary parent.
  uint16_t etx_2nd_parent;
  /// Current number of frames in the adaptation layer transmission queue.
  uint16_t adapt_layer_tx_queue_size;
  /// Highest number of frames in the adaptation layer transmission queue.
  uint16_t adapt_layer_tx_queue_peak;
} sl_wisun_statistics_network_t;

/** ARIB regulation statistics. */
typedef struct {
  uint32_t tx_duration_ms;  /**< Sum of transmission durations during the last
                             * hour in milliseconds. */
} sl_wisun_statistics_arib_regulation_t;

/** Regional regulation statistics. */
typedef union {
  sl_wisun_statistics_arib_regulation_t arib;  /**< ARIB statistics. */
} sl_wisun_statistics_regulation_t;

/// Statistics
typedef union {
  /// PHY/RF statistics
  sl_wisun_statistics_phy_t phy;
  /// MAC statistics
  sl_wisun_statistics_mac_t mac;
  /// Frequency hopping statistics
  sl_wisun_statistics_fhss_t fhss;
  /// Wi-SUN statistics
  sl_wisun_statistics_wisun_t wisun;
  /// 6LoWPAN/IP stack statistics
  sl_wisun_statistics_network_t network;
  /// Regional regulation statistics
  sl_wisun_statistics_regulation_t regulation;
} sl_wisun_statistics_t;

/// MAC address
SL_PACK_START(1)
typedef struct {
  /// @brief MAC address
  /// @details This field contains a MAC address (EUI-64) stored in canonical
  ///          format where the first byte of the array is the most-significant
  ///          byte of the MAC address.
  uint8_t address[SL_WISUN_MAC_ADDRESS_SIZE];
} SL_ATTRIBUTE_PACKED sl_wisun_mac_address_t;
SL_PACK_END()

/// IP address
SL_PACK_START(1)
typedef struct {
  /// @brief IP address
  /// @details This field contains an IPv6 address where the first byte of the
  ///          array is the most-significant byte of the IP address.
  uint8_t address[SL_WISUN_IP_ADDRESS_SIZE];
} SL_ATTRIBUTE_PACKED sl_wisun_ip_address_t;
SL_PACK_END()

/// Channel mask
SL_PACK_START(1)
typedef struct {
  /// @brief Bit mask of channels
  /// @details This field specifies a bit mask of channels, one bit per
  ///          channel. First byte of the array represents channel numbers
  ///          0 - 7, with bit 0 being channel 0. Second byte represents
  ///          channel numbers 8 - 15 and so forth.
  uint8_t mask[SL_WISUN_CHANNEL_MASK_SIZE];
} SL_ATTRIBUTE_PACKED sl_wisun_channel_mask_t;
SL_PACK_END()

/// Socket option for event mode
typedef struct {
  /// Socket event mode
  uint32_t mode;
} sl_wisun_socket_option_event_mode_t;

/// Socket option for multicast group
typedef struct {
  /// Multicast group action
  uint32_t action;
  /// Multicast group address
  sl_wisun_ip_address_t address;
} sl_wisun_socket_option_multicast_group_t;

/// Socket option for send buffer limit
typedef struct {
  /// Send buffer limit
  uint32_t limit;
} sl_wisun_socket_option_send_buffer_limit_t;

/// Socket option data
SL_PACK_START(1)
typedef union {
  /// Socket event mode
  sl_wisun_socket_option_event_mode_t event_mode;
  /// Socket multicast group
  sl_wisun_socket_option_multicast_group_t multicast_group;
  /// Socket send buffer limit
  sl_wisun_socket_option_send_buffer_limit_t send_buffer_limit;
} SL_ATTRIBUTE_PACKED sl_wisun_socket_option_data_t;
SL_PACK_END()

/// ID used identify a socket
typedef uint32_t sl_wisun_socket_id_t;

/// Socket ID value for an invalid socket
#define SL_WISUN_INVALID_SOCKET_ID 255

/// Enumeration for RPL neighbor types
typedef enum {
  /// Primary parent
  SL_WISUN_NEIGHBOR_TYPE_PRIMARY_PARENT     = 0,
  /// Secondary parent
  SL_WISUN_NEIGHBOR_TYPE_SECONDARY_PARENT   = 1,
  /// Child
  SL_WISUN_NEIGHBOR_TYPE_CHILD              = 2
} sl_wisun_neighbor_type_t;

/// RPL neighbor information
SL_PACK_START(1)
typedef struct {
  /// Link-local address
  sl_wisun_ip_address_t link_local_address;
  /// ULA/GUA address (unspecified address :: if unknown)
  sl_wisun_ip_address_t global_address;
  /// Neighbor type
  uint32_t type;
  /// Remaining lifetime (Link lifetime for parents, ARO lifetime for children)
  uint32_t lifetime;
  /// MAC TX packet count
  uint32_t mac_tx_count;
  /// MAC TX failed count
  uint32_t mac_tx_failed_count;
  /// MAC TX packet count using mode switch
  uint32_t mac_tx_ms_count;
  /// MAC TX failed count using mode switch
  uint32_t mac_tx_ms_failed_count;
  /// RPL Rank value for parents (0xffff if unknown or child)
  uint16_t rpl_rank;
  /// Measured ETX value if known (0xffff if unknown)
  uint16_t etx;
  /// Parent RSSI Out measured RSSI value (0xff if unknown)
  /// Calculated using EWMA specified by Wi-SUN from range of -174 (0) to +80 (254) dBm.
  uint8_t rsl_out;
  /// Parent RSSI In measured RSSI value (0xff if unknown)
  /// Calculated using EWMA specified by Wi-SUN from range of -174 (0) to +80 (254) dBm.
  uint8_t rsl_in;
} SL_ATTRIBUTE_PACKED sl_wisun_neighbor_info_t;
SL_PACK_END()

/// Enumeration for trace group
typedef enum {
  SL_WISUN_TRACE_GROUP_MAC     = 0,     /// Mac
  SL_WISUN_TRACE_GROUP_NW      = 1,     /// Network
  SL_WISUN_TRACE_GROUP_LLC     = 2,     /// LLC
  SL_WISUN_TRACE_GROUP_6LO     = 3,     /// 6lowpan
  SL_WISUN_TRACE_GROUP_IPV6    = 4,     /// IPV6
  SL_WISUN_TRACE_GROUP_TCP     = 5,     /// TCP
  SL_WISUN_TRACE_GROUP_UDP     = 6,     /// UDP
  SL_WISUN_TRACE_GROUP_ICMP    = 7,     /// ICMP
  SL_WISUN_TRACE_GROUP_DHCP    = 8,     /// DHCP
  SL_WISUN_TRACE_GROUP_MPL     = 9,     /// MPL
  SL_WISUN_TRACE_GROUP_DNS     = 10,    /// DNS
  SL_WISUN_TRACE_GROUP_RPL     = 11,    /// RPL
  SL_WISUN_TRACE_GROUP_TRIC    = 12,    /// Trickle
  SL_WISUN_TRACE_GROUP_ND      = 13,    /// Network Discovery
  SL_WISUN_TRACE_GROUP_RA      = 14,    /// Router Advertisement
  SL_WISUN_TRACE_GROUP_WS      = 15,    /// Wi-SUN Stack
  SL_WISUN_TRACE_GROUP_BOOT    = 16,    /// Wi-SUN Bootstrap
  SL_WISUN_TRACE_GROUP_WSR     = 17,    /// Wi-SUN Router
  SL_WISUN_TRACE_GROUP_WSBR    = 18,    /// Border router
  SL_WISUN_TRACE_GROUP_SEC     = 19,    /// Security
  SL_WISUN_TRACE_GROUP_TIME    = 20,    /// Time and timers
  SL_WISUN_TRACE_GROUP_NEIGH   = 21,    /// Neighbor
  SL_WISUN_TRACE_GROUP_STAT    = 22,    /// Statistics
  SL_WISUN_TRACE_GROUP_BUFF    = 23,    /// Dynamic Buffer
  SL_WISUN_TRACE_GROUP_ADDR    = 24,    /// Address Manipulation
  SL_WISUN_TRACE_GROUP_MON     = 25,    /// Monitoring
  SL_WISUN_TRACE_GROUP_SOCK    = 26,    /// Socket
  SL_WISUN_TRACE_GROUP_DENY    = 27,    /// Deny list
  SL_WISUN_TRACE_GROUP_ETX     = 28,    /// ETX
  SL_WISUN_TRACE_GROUP_FHSS    = 29,    /// FHSS
  SL_WISUN_TRACE_GROUP_ROUT    = 30,    /// Routing table
  SL_WISUN_TRACE_GROUP_EVLP    = 31,    /// Event loop
  SL_WISUN_TRACE_GROUP_NVM     = 32,    /// NVM
  SL_WISUN_TRACE_GROUP_CRYPTO  = 33,    /// Crypto
  SL_WISUN_TRACE_GROUP_RF      = 34,    /// Wi-SUN RF Driver
  // 35 to 63 reserved for future used
  SL_WISUN_TRACE_GROUP_INT     = 63,    /// Internal usage
  SL_WISUN_TRACE_GROUP_COUNT   = 64     /// Max number of trace group in this enum
} sl_wisun_trace_group_t;

/// Enumerations for trace level
typedef enum {
  /// No trace
  SL_WISUN_TRACE_LEVEL_NONE  = 0,
  /// Error only
  SL_WISUN_TRACE_LEVEL_ERROR = 1,
  /// Warning + error
  SL_WISUN_TRACE_LEVEL_WARN  = 2,
  /// Info + warning + error
  SL_WISUN_TRACE_LEVEL_INFO  = 3,
  /// Debug + info + warning + error
  SL_WISUN_TRACE_LEVEL_DEBUG = 4,
} sl_wisun_trace_level_t;

/// Configure the trace level of 1 group
typedef struct {
  /// Trace Group ID. Coded with enum sl_wisun_trace_group_t.
  uint8_t group_id;
  /// Maximum trace level to display for this group.
  /// It is coded using enum sl_wisun_trace_level_t.
  uint8_t trace_level;
} sl_wisun_trace_group_config_t;

/// Enumerations for regional regulation
typedef enum {
  /// No regulation
  SL_WISUN_REGULATION_NONE = 0,
  /// ARIB, can only be used with JP regulatory domain
  SL_WISUN_REGULATION_ARIB = 1,
} sl_wisun_regulation_t;

typedef struct {
  /// Max duration of a fragment in ms
  uint32_t fragment_duration_ms;
} sl_wisun_async_fragmentation_t;

/// Enumeration for Mode Switch mode
typedef enum {
  SL_WISUN_MODE_SWITCH_DISABLED     = 0,     /// Mode switch is not allowed
  SL_WISUN_MODE_SWITCH_ALL_UNICAST  = 1,     /// Mode switch is allowed for all unicast data frames
  SL_WISUN_MODE_SWITCH_ALWAYS       = 2,     /// Mode switch is allowed for all data frames
} sl_wisun_mode_switch_mode_t;

/// Enumeration for regional regulation transmission level.
/// Thresholds are define with #sl_wisun_set_regulation_tx_thresholds.
typedef enum {
  /// Transmission duration is compliant with regional regulation
  SL_WISUN_REGULATION_TX_LEVEL_LOW = 0,
  /// Transmission duration is above warning threshold
  SL_WISUN_REGULATION_TX_LEVEL_WARNING = 1,
  /// Transmission duration is above alert threshold
  SL_WISUN_REGULATION_TX_LEVEL_ALERT = 2
} sl_wisun_regulation_tx_level_t;

/// Enumeration for unicast transmission mode
typedef enum {
  /// Default unicast transmission
  SL_WISUN_UNICAST_TX_MODE_DEFAULT = 0,
  /// Allow unicast transmission only on a slot
  SL_WISUN_UNICAST_TX_MODE_SLOT = 1,
} sl_wisun_unicast_tx_mode_t;

/// Maximum framgent duration. Disables advert fragmentation.
#define SL_WISUN_ADVERT_FRAGMENT_DISABLE UINT32_MAX

/** @} (end SL_WISUN_TYPES) */

#endif  // SL_WISUN_TYPES_H
