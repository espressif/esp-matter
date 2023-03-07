/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */


/**************************************************************************//**
* \file <wiced_bt_mesh_provision.h>
*
* Mesh Provisioning definitions.
******************************************************************************/
#ifndef __WICED_BT_MESH_PROVISION_H__
#define __WICED_BT_MESH_PROVISION_H__

#include "wiced_bt_mesh_event.h"
#include "wiced_bt_mesh_model_defs.h"
#include "wiced_bt_mesh_models.h"

#if defined _WIN32 || defined WICEDX_LINUX || defined __ANDROID__ || defined __APPLE__
#define PACKED
#pragma pack(1)
#else
#ifndef PACKED
#define PACKED
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* Maximum number of vendor specific opcode according to spec. */
#define WICED_BT_MESH_MAX_VENDOR_MODEL_OPCODES 64


/**
 * @addtogroup  wiced_bt_mesh_provisioning        Mesh Provisioning Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh provisioning is split into two parts.  The server side applies to all
 * devices and allows a provisioner to add this device to a mesh network.  It
 * is implemented in the Mesh application library so that each mesh application
 * can easily link this functionality in.  The client part is implemented in
 * the Mesh Provisioning library of the AIROC BTSDK which provide a simple method for
 * an application to integrate functionality to add other devices to the
 * mesh network.
 *
 * @{
 */

#define WICED_BT_MESH_KEY_LEN    16

/**
 * @anchor PROVISIONER_EVENT
 * @name Definition for messages exchanged between the Provisioner Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_COMMAND_STATUS                        200   /**< Local device processed command from the MCU */
#define WICED_BT_MESH_PROVISION_CONNECT_STATUS              202   /**< Provision Linke established or dropped */
#define WICED_BT_MESH_PROVISION_STARTED                     203   /**< Provisioning started */
#define WICED_BT_MESH_PROVISION_END                         204   /**< Provisioning completed */
#define WICED_BT_MESH_PROVISION_DEVICE_CAPABILITIES         205   /**< Provisioning device capabilities */
#define WICED_BT_MESH_PROVISION_GET_OOB_DATA                206   /**< Provisioning get out of band data request */
#define WICED_BT_MESH_PROXY_DEVICE                          207   /**< Proxy device info */
/** @} PROVISIONER_EVENT */

/**
 * @anchor CONFIG_EVENT
 * @name Definition for the mesh config messages exchanged between the Provisioner Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_CONFIG_NODE_RESET_STATUS              210  /**< Config Node Reset Status */
#define WICED_BT_MESH_CONFIG_FRIEND_STATUS                  211  /**< Config Friend Status */
#define WICED_BT_MESH_CONFIG_GATT_PROXY_STATUS              212  /**< Config GATT Proxy Status */
#define WICED_BT_MESH_CONFIG_RELAY_STATUS                   213  /**< Config Relay Status */
#define WICED_BT_MESH_CONFIG_BEACON_STATUS                  214  /**< Config Beacon Status */
#define WICED_BT_MESH_CONFIG_DEFAULT_TTL_STATUS             215  /**< Config Default TTL Status */
#define WICED_BT_MESH_CONFIG_NODE_IDENTITY_STATUS           216  /**< Config Node Identity Status */
#define WICED_BT_MESH_CONFIG_MODEL_PUBLICATION_STATUS       217  /**< Config Model Publication Status */
#define WICED_BT_MESH_CONFIG_MODEL_SUBSCRIPTION_STATUS      218  /**< Config Model Subscription Status */
#define WICED_BT_MESH_CONFIG_MODEL_SUBSCRIPTION_LIST        219  /**< Config Model Subscription List */
#define WICED_BT_MESH_CONFIG_NETKEY_STATUS                  220  /**< Config Network Key Status */
#define WICED_BT_MESH_CONFIG_NETKEY_LIST                    221  /**< Config Network Key List */
#define WICED_BT_MESH_CONFIG_APPKEY_STATUS                  222  /**< Config Application Key Status */
#define WICED_BT_MESH_CONFIG_APPKEY_LIST                    223  /**< Config Application List */
#define WICED_BT_MESH_CONFIG_MODEL_APP_BIND_STATUS          224  /**< Config Model Application Bind Status */
#define WICED_BT_MESH_CONFIG_MODEL_APP_BIND_LIST            225  /**< Config Model Application Bind List */
#define WICED_BT_MESH_CONFIG_COMPOSITION_DATA_STATUS        226  /**< Config Composition Data Status */
#define WICED_BT_MESH_CONFIG_HEARBEAT_SUBSCRIPTION_STATUS   227  /**< Config Heartbeat Subscription Status */
#define WICED_BT_MESH_CONFIG_HEARBEAT_PUBLICATION_STATUS    228  /**< Config Heartbeat Publication Status */
#define WICED_BT_MESH_CONFIG_NETWORK_TRANSMIT_STATUS        229  /**< Config Network Transmit Parameters Status */
#define WICED_BT_MESH_CONFIG_LPN_POLL_TIMEOUT_STATUS        230  /**< Low Power Node Poll Timeout Status */
#define WICED_BT_MESH_CONFIG_KEY_REFRESH_PHASE_STATUS       231  /**< Config Network Transmit Parameters Status */
 /** @} CONFIG_EVENT */

/**
 * @anchor HEALTH_EVENT
 * @name Definition for the mesh health messages exchanged between the Provisioner Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_HEALTH_CURRENT_STATUS                 232  /**< Current Health state of an element */
#define WICED_BT_MESH_HEALTH_FAULT_STATUS                   233  /**< Registered Fault state of an element */
#define WICED_BT_MESH_HEALTH_PERIOD_STATUS                  234  /**< Registered Fault state of an element */
#define WICED_BT_MESH_HEALTH_ATTENTION_STATUS               235  /**< Registered Fault state of an element */
/** @} HEALTH_EVENT */

/**
 * @anchor PROXY_EVENT
 * @name Definition for the mesh filter messages exchanged between the Provisioner Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_PROXY_FILTER_STATUS                   237  /**< Current filter status of the proxy */
/** @} PROXY_EVENT */

/**
 * @anchor CORE_EVENT
 * @name Definition for the mesh core messages exchanged between the Core Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_SEQ_CHANGED                           238  /**< Sequence number changed */
#define WICED_BT_MESH_IV_CHANGED                            239  /**< IV Index or/and IV_UPDATE flag changed */
/** @} CORE_EVENT */

/**
 * @anchor REMOTE_PROVISIONG_SERVER_EVENT
 * @name Definition for the mesh provisioning messages exchanged between the Provisioner Library and the Provisioner Client Application
 * @{ */
#define WICED_BT_MESH_PROVISION_SCAN_CAPABILITIES_STATUS     240  /**< Scan Info Status */
#define WICED_BT_MESH_PROVISION_SCAN_STATUS                  241  /**< Scan Status */
#define WICED_BT_MESH_PROVISION_SCAN_REPORT                  242  /**< Scan Report */
#define WICED_BT_MESH_PROVISION_SCAN_EXTENDED_REPORT         243  /**< Scan Extended Report */
#define WICED_BT_MESH_PROVISION_LINK_STATUS                  244  /**< Link Status */
#define WICED_BT_MESH_PROVISION_LINK_REPORT                  245  /**< Link Report */
#define WICED_BT_MESH_PROVISION_PDU_OUTBOUND_REPORT          247  /**< PDU Outbound Report */
#define WICED_BT_MESH_PROVISION_PDU_REPORT                   248  /**< PDU Report */
 /** @} REMOTE_PROVISIONG_SERVER_EVENT */

#define WICED_BT_MESH_TX_TO_FRIEND_COMPLETE                  252  /**< Transmission to friend completed */
#define WICED_BT_MESH_RAW_MODEL_DATA                         253  /**< Raw model data for the apps that handle model layer */
#define WICED_BT_MESH_PROXY_CONNECTION_STATUS                254  /**< Proxy connection status */
#define WICED_BT_MESH_TX_COMPLETE                            255  /**< Transmission completed, or timeout waiting for peer ack. */

#define WICED_BT_MESH_CONFIG_PRIVATE_BEACON_STATUS           262  /**< Private Beacon Status */
#define WICED_BT_MESH_CONFIG_PRIVATE_GATT_PROXY_STATUS       263  /**< Private GATT Proxy Status */
#define WICED_BT_MESH_CONFIG_PRIVATE_NODE_IDENTITY_STATUS    264  /**< Private Node Identity Status */
#define WICED_BT_MESH_CONFIG_ON_DEMAND_PRIVATE_PROXY_STATUS  265  /**< On-Demand Private Proxy Status */
#define WICED_BT_MESH_SOLICITATION_PDU_RPL_ITEMS_STATUS      266  /**< Solicitation PDU RPL Items Status */

#define WICED_BT_MESH_NETWORK_FILTER_STATUS                  267  /**< Network Filter Status */

#define WICED_BT_MESH_DF_DIRECTED_CONTROL_STATUS                   268
#define WICED_BT_MESH_DF_PATH_METRIC_STATUS                        269
#define WICED_BT_MESH_DF_DISCOVERY_TABLE_CAPABILITIES_STATUS       270
#define WICED_BT_MESH_DF_FORWARDING_TABLE_STATUS                   271
#define WICED_BT_MESH_DF_FORWARDING_TABLE_DEPENDENTS_STATUS        272
#define WICED_BT_MESH_DF_FORWARDING_TABLE_DEPENDENTS_GET_STATUS    273
#define WICED_BT_MESH_DF_FORWARDING_TABLE_ENTRIES_COUNT_STATUS     274
#define WICED_BT_MESH_DF_FORWARDING_TABLE_ENTRIES_STATUS           275
#define WICED_BT_MESH_DF_WANTED_LANES_STATUS                       276
#define WICED_BT_MESH_DF_TWO_WAY_PATH_STATUS                       277
#define WICED_BT_MESH_DF_PATH_ECHO_INTERVAL_STATUS                 278
#define WICED_BT_MESH_DF_DIRECTED_NETWORK_TRANSMIT_STATUS          279
#define WICED_BT_MESH_DF_DIRECTED_RELAY_RETRANSMIT_STATUS          280
#define WICED_BT_MESH_DF_RSSI_THRESHOLD_STATUS                     281
#define WICED_BT_MESH_DF_DIRECTED_PATHS_STATUS                     282
#define WICED_BT_MESH_DF_DIRECTED_PUBLISH_POLICY_STATUS            283
#define WICED_BT_MESH_DF_PATH_DISCOVERY_TIMING_CONTROL_STATUS      284
#define WICED_BT_MESH_DF_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS  285
#define WICED_BT_MESH_DF_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS  286

/* This structure contains information sent from the provisioner application to provisioner library to setup local device */
typedef struct
{
    uint16_t addr;                      /**< Local Node Address */
    uint8_t  dev_key[16];               /**< Local Device Key */
    uint8_t  network_key[16];           /**< Mesh Network Key */
    uint16_t net_key_idx;               /**< Network Key Index */
    uint32_t iv_idx;                    /**< Current Network IV Index */
    uint8_t  key_refresh;               /**< 1 if Key Refresh Phase 2 is in progress */
    uint8_t  iv_update;                 /**< 1 if IV Update procedure is in progress */
    uint8_t  model_level_access;        /**< 1 if application implements model level, 0 if application talks to model library */
} wiced_bt_mesh_local_device_set_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to set device key to be used in consecutive configuration messages.*/
typedef struct
{
    uint16_t dst;                       /**< Destination address */
    uint8_t  dev_key[16];               /**< Device Key */
    uint16_t net_key_idx;               /**< Network Key Used to provision this device */
} wiced_bt_mesh_set_dev_key_data_t;

/* This structure contains information provided by the application to add vendor model.*/
typedef struct
{
    uint16_t company_id;                                        /**< Company ID */
    uint16_t model_id;                                          /**< Model ID */
    uint8_t  num_opcodes;                                       /**< Number of opcodes */
    uint8_t  opcode[WICED_BT_MESH_MAX_VENDOR_MODEL_OPCODES*3];   /**< List of opcodes */
} wiced_bt_mesh_add_vendor_model_data_t;

/**
* This structure contains information sent to the provisioner application from the provisioner library with information from the proxy device advertisements
* while device advertises network identity
*/
typedef struct
{
    uint8_t  bd_addr[6];                /**< Proxy device address */
    uint8_t  bd_addr_type;              /**< Proxy device address type */
    int8_t   rssi;                      /**< Receive Signal Strength of Proxy advertisements */
    uint16_t net_key_idx;               /**< Network key index */
} wiced_bt_mesh_proxy_device_network_data_t;

/* This structure contains scan information status data */
typedef struct
{
    uint8_t  max_scanned_items;         /**< The maximum number of UUIDs that can be reported during scanning */
    uint8_t  active_scan_supported;     /**< Indication if active scan is supported */
} wiced_bt_mesh_provision_scan_capabilities_status_data_t;

/* This structure contains scan start data */
typedef struct
{
    uint8_t  scanned_items_limit;       /**< Max number of scanned items to report */
    uint8_t  timeout;                   /**< Time limit for a scan(in seconds) */
    uint8_t  scan_single_uuid;          /**< If true, client searches for a specific UUID */
    uint8_t  uuid[MESH_DEVICE_UUID_LEN];/**< Specific UUID client is interested in */
} wiced_bt_mesh_provision_scan_start_data_t;

/* This structure contains extended scan info get data */
typedef struct
{
    uint8_t  timeout;                   /**< Time limit for a scan(in seconds) */
#define WICED_BT_MESH_AD_FILTER_TYPES_MAX   7
    uint8_t  num_ad_filters;            /**< Number of ad_types */
    uint8_t  ad_filter_types[WICED_BT_MESH_AD_FILTER_TYPES_MAX];  /**< AD Types to be report */
    uint8_t  uuid_present;              /**< If True, next field is present */
    uint8_t  uuid[16];                  /**< UUID of the device to be scanned(Optional) */
} wiced_bt_mesh_provision_scan_extended_start_t;

/* This structure contains scan status data */
typedef struct
{
    uint8_t  status;                    /**< Status for the requesting message */
    uint8_t  state;                     /**< The Remote Provisioning Scanning Server State value */
    uint8_t  scanned_items_limit;       /**< Maximum number of the scanned items to be reported */
    uint8_t  timeout;                   /**< Time limit for a scan(in seconds) */
} wiced_bt_mesh_provision_scan_status_data_t;

/* This structure contains scan report data */
typedef struct
{
    int8_t   rssi;                      /**< Signed integer that is interpreted as an indication of received signal strength measured in dBm */
    uint8_t  uuid[16];                  /**< Value of the Device UUID */
    uint16_t oob;                       /**< OOB information */
    uint32_t uri_hash;                  /**< URI Hash if available from the device */
#ifdef PROVISION_SCAN_REPORT_INCLUDE_BDADDR
    uint8_t  bdaddr[BD_ADDR_LEN];
#endif
} wiced_bt_mesh_provision_scan_report_data_t;

/* This structure contains scan extended report data */
typedef struct
{
    uint8_t  status;                    /**< status of the extended scan start, 0 if success */
    uint8_t  uuid[16];                  /**< Value of the Device UUID */
    uint16_t oob;                       /**< out of band provisioning data */
    uint8_t  adv_data[62];              /**< Variable length ADV data for the UUID */
} wiced_bt_mesh_provision_scan_extended_report_data_t;

/* This structure contains provision link status data */
typedef struct
{
    uint8_t  status;                    /**< Status for the requesting message */
} wiced_bt_mesh_provision_link_status_data_t;

/* This structure contains provision link report data */
typedef struct
{
    uint8_t  link_status;               /**< Status of the provisioning bearer */
    uint8_t  rpr_state;                 /**< Remote Provisioner state */
    uint8_t  reason;                    /**< Link close reason code */
    uint8_t  over_gatt;                 /**< True if connection over GATT only if provisioning done by local device */
} wiced_bt_mesh_provision_link_report_data_t;

/* This structure contains provision outbound pdu report data */
typedef struct
{
    uint8_t  outbound_pdu_number;       /**< Outbound PDU Number */
} wiced_bt_mesh_provision_outbound_pdu_report_data_t;

/* This structure contains provision inbound pdu report data */
typedef struct
{
    uint8_t  inbound_pdu_number;        /**< Inbound PDU number */
    uint8_t  data[1];                   /**< Inbound PDU */
} wiced_bt_mesh_provision_inbound_pdu_report_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to establish provisioning link */
typedef struct
{
    uint8_t  uuid[16];                  /**< UUID of the device to provision */
    uint8_t  identify_duration;         /**< Duration of the identify state */
    uint8_t  procedure;
} wiced_bt_mesh_provision_connect_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to establish provisioning link */
typedef struct
{
    uint16_t provisioner_addr;          /**< Remote provisioning server address */
    uint16_t addr;                      /**< Address of the first element of the device.  Maybe 0 when unprovisioned. */
    uint8_t  connected;                 /**< If TRUE, connection has been established */
    uint8_t  over_gatt;                 /**< If TRUE, connection is over GATT */
} wiced_bt_mesh_connect_status_data_t;

/* This structure contains information sent from the provisioner application to the provisioner library to establish a proxy connection to a mesh network through the specified device. */
typedef struct
{
#define CONNECT_TYPE_NODE_ID     0      /**< Connect to a just provisioned device advertising Node Identity */
#define CONNECT_TYPE_NET_ID      1      /**< Connect to a proxy using network ID */
#define CONNECT_TYPE_BDADDR      2      /**< In some cases a device may connect to a specific BDADDR */
#define CONNECT_TYPE_UUID        3      /**< For provisioning connection needs to be established using UUID */
    uint8_t  connect_type;              /**< Specifies how GATT connection should be established */
    uint16_t node_id;                   /**< Address of the node to connect to */
    uint8_t  bd_addr[6];                /**< Proxy device address */
    uint8_t  bd_addr_type;              /**< Proxy device address type */
    uint8_t  uuid[16];                  /**< UUID of the device we need to connect to */
    uint8_t  scan_duration;             /**< timeout to wait for searching for the node to connect in seconds */
} wiced_bt_mesh_proxy_connect_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to start provisioning on the establish provisioning link */
typedef struct
{
    uint16_t addr;                      /**< Address to assign to the device during provisioning */
    uint16_t net_key_idx;               /**< The network key index to assign to the device during provisioning */
    uint8_t  algorithm;                 /**< The algorithm used for provisioning */
    uint8_t  public_key_type;           /**< If 1 Public Key OOB information available */
    uint8_t  auth_method;               /**< Authentication Method used */
    uint8_t  auth_action;               /**< Selected Output OOB Action or Input OOB Action */
    uint8_t  auth_size;                 /**< Size of the Output OOB used or size of the Input OOB used */
} wiced_bt_mesh_provision_start_data_t;

/* This structure contains provisioning configuration of the device */
typedef struct
{
    uint8_t   pub_key_type;             /**< Bitmap of Supported public key types (Static OOB information available 0x01 is the only one supported at 1.0 time) */
    uint8_t   static_oob_type;          /**< Supported static OOB Types (1 if available) */
    uint8_t   static_oob[16];           /**< static data if static_oob_type is 1 */
    uint8_t   output_oob_size;          /**< Maximum size of Output OOB supported (0 - device does not support output OOB, 1-8 max size in octets supported by the device) */
    uint16_t  output_oob_action;        /**< Output OOB Action field values (see @ref BT_MESH_OUT_OOB_ACT "Output OOB Action field values") */
    uint8_t   input_oob_size;           /**< Maximum size in octets of Input OOB supported */
    uint16_t  input_oob_action;         /**< Supported Input OOB Actions (see @ref BT_MESH_IN_OOB_ACT "Input OOB Action field values") */
} wiced_bt_mesh_provision_capabilities_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with provisioning result */
typedef struct
{
    uint16_t provisioner_addr;          /**< Address of the remote provisioning server, or 0 if provisioning is by local device */
    uint16_t addr;                      /**< address to assign */
    uint16_t net_key_idx;               /**< NetKeyIdx of the key provisioned to the device */
    uint8_t  result;                    /**< result of the provision operation */
    uint8_t  dev_key[WICED_BT_MESH_KEY_LEN];     /**< Device key generated during provisioning.  This key shall be used by provisioner to configure the provisioned device. */
} wiced_bt_mesh_provision_status_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with provisioning device capabilities */
typedef struct
{
    uint16_t provisioner_addr;          /**< Address of the remote provisioning server, or 0 if provisioning is by local device */
    uint8_t  elements_num;              /**< Number of elements supported by the device (1-255) */
    uint16_t algorithms;                /**< Bitmap of Supported algorithms and other capabilities (FIPS_P256_EC is the only one supported at 1.0 time) */
    uint8_t  pub_key_type;              /**< Bitmap of Supported public key types (Static OOB information available 0x01 is the only one supported at 1.0 time) */
    uint8_t  static_oob_type;           /**< Supported static OOB Types (1 if available) */
    uint8_t  output_oob_size;           /**< Maximum size of Output OOB supported (0 - device does not support output OOB, 1-8 max size in octets supported by the device) */
    uint16_t output_oob_action;         /**< Output OOB Action field values (see @ref BT_MESH_OUT_OOB_ACT "Output OOB Action field values") */
    uint8_t  input_oob_size;            /**< Maximum size in octets of Input OOB supported */
    uint16_t input_oob_action;          /**< Supported Input OOB Actions (see @ref BT_MESH_IN_OOB_ACT "Input OOB Action field values") */
} wiced_bt_mesh_provision_device_capabilities_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with out of band data request */
typedef struct
{
    uint16_t provisioner_addr;          /**< Address of the remote provisioning server, or 0 if provisioning is by local device */
    uint8_t  type;                      /**< Out of band type */
    uint8_t  size;                      /**< Number of octets (should be 1-8) */
    uint16_t action;                    /**< OOB Action (see @ref BT_MESH_OUT_OOB_ACT "Output OOB Action field values") */
} wiced_bt_mesh_provision_device_oob_request_data_t;

/* This structure contains information sent from the provisioner application to get Node Identity Status of a node */
typedef PACKED struct
{
    uint16_t net_key_idx;               /**< Index of the NetKey */
} wiced_bt_mesh_config_node_identity_get_data_t;

/* This structure contains information sent from the provisioner application to set current Node Identity state for a subnet */
typedef PACKED struct
{
    uint16_t net_key_idx;               /**< Index of the NetKey */
    uint8_t  identity;                  /**< New Node Identity state */
} wiced_bt_mesh_config_node_identity_set_data_t;

/* This structure contains information sent from the provisioner library to provisioner application with the status of the node reset */
typedef PACKED struct
{
    uint8_t  status;                    /**< Status Code for the requesting message */
    uint16_t net_key_idx;               /**< Index of the NetKey */
    uint8_t  identity;                  /**< Node Identity state */
} wiced_bt_mesh_config_node_identity_status_data_t;

/*  This structure contains information sent from the provisioner application to set Beacon  State of a node */
typedef PACKED struct
{
    uint8_t  state;                     /**< Target secure beacon state (see @ref MESH_BEACON_STATES) */
} wiced_bt_mesh_config_beacon_set_data_t;

/*  This structure contains information sent from the provisioner library to provisioner application with the status of the node reset */
typedef PACKED struct
{
    uint8_t  state;                     /**< Current secure beacon state (see @ref MESH_BEACON_STATES) */
} wiced_bt_mesh_config_beacon_status_data_t;

/*  This structure contains information sent from the provisioner application to get Composition Data of a node */
typedef PACKED struct
{
    uint8_t  page_number;               /**< page number of composition data to get */
} wiced_bt_mesh_config_composition_data_get_data_t;

/*  This structure contains information sent from the provisioner application to get Composition Data of a node */
typedef PACKED struct
{
    uint8_t  page_number;               /**< page number of composition data */
    uint16_t data_len;                  /**< composition data len */
    uint8_t  data[1];                   /**< first byte of the composition data */
} wiced_bt_mesh_config_composition_data_status_data_t;

/*  This structure contains information sent from the provisioner application to get Default TTL of a node */
typedef PACKED struct
{
    uint8_t  ttl;                       /**< New Default TTL value */
} wiced_bt_mesh_config_default_ttl_set_data_t;

/*  This structure contains information sent to the provisioner application with Default TTL state */
typedef PACKED struct
{
    uint8_t  ttl;                       /**< New Default TTL value */
    uint8_t  received_ttl;              /**< Value of the TTL that this message has been received.Client can calculate number of hops to the SRC compairing this and default TTL values. */
} wiced_bt_mesh_config_default_ttl_status_data_t;

/*  This structure contains information sent from the provisioner application to set GATT Proxy state of a node */
typedef PACKED struct
{
    uint8_t  state;                     /**< New GATT Proxy state value (see @ref MESH_GATT_PROXY_STATES) */
} wiced_bt_mesh_config_gatt_proxy_set_data_t;

/*  This structure contains information sent to the provisioner application with GATT Proxy state */
typedef PACKED struct
{
    uint8_t  state;                     /**< Current GATT Proxy state value (see @ref MESH_GATT_PROXY_STATES) */
} wiced_bt_mesh_config_gatt_proxy_status_data_t;

/*  This structure contains information sent from the provisioner application to set Friend state of a node */
typedef PACKED struct
{
    uint8_t  state;                     /**< New Friend state value (see @ref MESH_FRIEND_STATES) */
} wiced_bt_mesh_config_friend_set_data_t;

/*  This structure contains information sent to the provisioner application with Friend state */
typedef PACKED struct
{
    uint8_t  state;                     /**< Current Friend state value (see @ref MESH_FRIEND_STATES) */
} wiced_bt_mesh_config_friend_status_data_t;

/*  This structure contains information sent from the provisioner application to get Key Refresh Phase of a network */
typedef PACKED struct
{
    uint16_t net_key_idx;               /**< NetKey Index */
} wiced_bt_mesh_config_key_refresh_phase_get_data_t;

/*  This structure contains information sent from the provisioner application to set Key Refresh Phase of a network */
typedef PACKED struct
{
    uint16_t net_key_idx;               /**< NetKey Index */
    uint8_t  transition;                /**< New Key Refresh Phase Transition (see @ref MESH_KEY_REFRESH_PHASES) */
} wiced_bt_mesh_config_key_refresh_phase_set_data_t;

/*  This structure contains information sent to the provisioner application with Key Refresh Phase of a network */
typedef PACKED struct
{
    uint8_t  status;                    /**< status */
    uint16_t net_key_idx;               /**< NetKey Index */
    uint8_t  phase;                     /**< Key Refresh Phase State (see @ref MESH_KEY_REFRESH_PHASES) */
} wiced_bt_mesh_config_key_refresh_phase_status_data_t;

/*  This structure contains information sent from the provisioner application to set Relay state of a node */
typedef PACKED struct
{
    uint8_t  state;                     /**< New Relay state */
    uint8_t  retransmit_count;          /**< Number of retransmissions on advertising bearer for each Network PDU relayed by the node */
    uint16_t retransmit_interval;       /**< Interval in milliseconds between retransmissions */
} wiced_bt_mesh_config_relay_set_data_t;

/* This structure contains information sent to the provisioner application with GATT Proxy state */
typedef PACKED struct
{
    uint8_t  state;                     /**< Current Relay state */
    uint8_t  retransmit_count;          /**< Number of retransmissions on advertising bearer for each Network PDU relayed by the node */
    uint16_t retransmit_interval;       /**< Interval in milliseconds between retransmissions */
} wiced_bt_mesh_config_relay_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get information about model publication */
typedef PACKED struct
{
    uint16_t element_addr;              /**< Address of the element */
    uint16_t company_id;                /**< Company ID */
    uint16_t model_id;                  /**< Model ID */
} wiced_bt_mesh_config_model_publication_get_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to set model publication */
typedef PACKED struct
{
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint8_t  publish_addr[16];              /**< Value of the publish address */
    uint16_t app_key_idx;                   /**< Index of the application key */
    uint8_t  credential_flag;               /**< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;                   /**< Default TTL value for the outgoing messages */
    uint32_t publish_period;                /**< Period for periodic status publishing */
    uint8_t  publish_retransmit_count;      /**< Number of retransmissions for each published message */
    uint16_t publish_retransmit_interval;   /**< Interval in milliseconds between retransmissions */
} wiced_bt_mesh_config_model_publication_set_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current model publication information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint16_t publish_addr;                  /**< Value of the publish address */
    uint16_t app_key_idx;                   /**< Index of the application key */
    uint8_t  credential_flag;               /**< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;                   /**< Default TTL value for the outgoing messages */
    uint32_t publish_period;                /**< Period for periodic status publishing */
    uint8_t  publish_retransmit_count;      /**< Number of retransmissions for each published message */
    uint16_t publish_retransmit_interval;   /**< Interval in milliseconds between retransmissions */
} wiced_bt_mesh_config_model_publication_status_data_t;

#define OPERATION_ADD        0 /**< Subscription change: Add */
#define OPERATION_DELETE     1 /**< Subscription change: Delete */
#define OPERATION_OVERWRITE  2 /**< Subscription change: Overwrite */
#define OPERATION_UPDATE     2 /**< Subscription change: Update network key */
#define OPERATION_DELETE_ALL 3 /**< Subscription change: Delete */

/* This structure contains information sent from the provisioner application to provisioner library to change model subscription */
typedef PACKED struct
{
    uint8_t  operation;                     /**< Request operation */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint8_t  addr[16];                      /**< Value of the Address */
} wiced_bt_mesh_config_model_subscription_change_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get the list of current subscription list */
typedef PACKED struct
{
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
} wiced_bt_mesh_config_model_subscription_get_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current model subscription information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint16_t addr;                          /**< Value of the address */
} wiced_bt_mesh_config_model_subscription_status_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current model subscription list information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint8_t  num_addr;                      /**< Number of addresses in the subscription list */
    uint16_t addr[1];                       /**< Value of the first address */
} wiced_bt_mesh_config_model_subscription_list_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to perform operation on a network keys */
typedef PACKED struct
{
    uint8_t  operation;                     /**< Request operation */
    uint16_t net_key_idx;                   /**< NetKey Index */
    uint8_t  net_key[16];                   /**< Value of the NetKey */
} wiced_bt_mesh_config_netkey_change_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current network key information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t net_key_idx;                   /**< Index of the NetKey */
} wiced_bt_mesh_config_netkey_status_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current network key information */
typedef PACKED struct
{
    uint8_t  num_keys;                      /**< Number of keys in the list */
    uint16_t net_key_idx[1];                /**< Value of the netkey index */
} wiced_bt_mesh_config_netkey_list_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to perform operation on a application keys */
typedef PACKED struct
{
    uint8_t  operation;                     /**< Request operation */
    uint16_t net_key_idx;                   /**< NetKey Index */
    uint16_t app_key_idx;                   /**< AppKey Index */
    uint8_t  app_key[16];                   /**< Value of the NetKey */
} wiced_bt_mesh_config_appkey_change_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get the list of current application keys */
typedef PACKED struct
{
    uint16_t net_key_idx;                   /**< NetKey Index */
} wiced_bt_mesh_config_appkey_get_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current application key information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t net_key_idx;                   /**< Index of the NetKey */
    uint16_t app_key_idx;                   /**< Index of the AppKey */
} wiced_bt_mesh_config_appkey_status_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current application key list information */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t net_key_idx;                   /**< Index of the NetKey */
    uint8_t  num_keys;                      /**< Number of keys in the list */
    uint16_t app_key_idx[1];                /**< Value of the Appkey indexes */
} wiced_bt_mesh_config_appkey_list_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to perform operation on a model to application key binding */
typedef PACKED struct
{
#define OPERATION_BIND        0 /**< Add binding */
#define OPERATION_UNBIND      1 /**< Delete binding */
    uint8_t  operation;                     /**< Request operation */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint16_t app_key_idx;                   /**< AppKey Index */
} wiced_bt_mesh_config_model_app_bind_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get the list of current application keys bound to a model */
typedef PACKED struct
{
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
} wiced_bt_mesh_config_model_app_get_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current model to application binding */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint16_t app_key_idx;                   /**< AppKey Index */
} wiced_bt_mesh_config_model_app_bind_status_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current list of applications bound to a model */
typedef PACKED struct
{
    uint8_t  status;                        /**< Status Code for the requesting message */
    uint16_t element_addr;                  /**< Address of the element */
    uint16_t company_id;                    /**< Company ID */
    uint16_t model_id;                      /**< Model ID */
    uint8_t  num_keys;                      /**< Number of keys in the list */
    uint16_t app_key_idx[1];                /**< Value of the model_app indexes */
} wiced_bt_mesh_config_model_app_list_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to setup heartbeat subscription */
typedef PACKED struct
{
    uint16_t subscription_src;          /**< Source address for Heartbeat messages */
    uint16_t subscription_dst;          /**< Destination address for Heartbeat messages */
    uint32_t period;                    /**< Period in seconds for receiving Heartbeat messages */
} wiced_bt_mesh_config_heartbeat_subscription_set_data_t;

/* This structure contains information sent from the provisioner library to provisioner application with the status of the heartbeat subscription */
typedef PACKED struct
{
    uint8_t  status;                    /**< status code for the requesting message */
    uint16_t subscription_src;          /**< Source address for Heartbeat messages */
    uint16_t subscription_dst;          /**< Destination address for Heartbeat messages */
    uint32_t period;                    /**< Remaining Period in seconds for receiving Heartbeat messages */
    uint16_t count;                     /**< Number of hearbeat messages received */
    uint8_t  min_hops;                  /**< Minimum hops when receiving Heartbeat messages */
    uint8_t  max_hops;                  /**< Maximum hops when receiving Heartbeat messages */
} wiced_bt_mesh_config_heartbeat_subscription_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to setup heartbeat publication */
typedef PACKED struct
{
    uint16_t publication_dst;           /**< Destination address for Heartbeat messages */
    uint32_t count;                     /**< Number of Heartbeat messages to be sent */
    uint32_t period;                    /**< Period in seconds for sending Heartbeat messages */
    uint8_t  ttl;                       /**< TTL to be used when sending Heartbeat messages */
    uint8_t  feature_relay;             /**< Relay feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_proxy;             /**< Proxy feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_friend;            /**< Friend feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_low_power;         /**< Low Power feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint16_t net_key_idx;               /**< NetKey Index */
} wiced_bt_mesh_config_heartbeat_publication_set_data_t;

/* This structure contains information sent from the provisioner library to provisioner application with the status of the heartbeat publication */
typedef PACKED struct
{
    uint8_t  status;                    /**< status code for the requesting message */
    uint16_t publication_dst;           /**< Destination address for Heartbeat messages */
    uint16_t count;                     /**< Number of heartbeat messages received */
    uint32_t period;                    /**< Remaining Period in seconds for receiving Heartbeat messages */
    uint8_t  ttl;                       /**< TTL to be used when sending Heartbeat messages */
    uint8_t  feature_relay;             /**< Relay feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_proxy;             /**< Proxy feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_friend;            /**< Friend feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint8_t  feature_low_power;         /**< Low Power feature change triggers a Heartbeat message: 0 = False, 1 = True */
    uint16_t net_key_idx;               /**< NetKey Index */
} wiced_bt_mesh_config_heartbeat_publication_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to set network transmit parameters */
typedef PACKED struct
{
    uint8_t  count;                     /**< Number of transmissions for each Network PDU originating from the node */
    uint16_t interval;                  /**< Interval between transmissions in milliseconds */
} wiced_bt_mesh_config_network_transmit_set_data_t;

/* This structure contains information sent from the provisioner library to provisioner application with the status of network transmit parameters */
typedef PACKED struct
{
    uint8_t  count;                     /**< Number of transmissions for each Network PDU originating from the node */
    uint32_t interval;                  /**< Interval between transmissions in milliseconds */
} wiced_bt_mesh_config_network_transmit_status_data_t;

/* This structure contains information sent from the provisioner library to provisioner application to report the Current Health state of an element */
typedef PACKED struct
{
    uint16_t app_key_idx;               /**< application key to use */
    uint8_t  test_id;                   /**< Identifier of a most recently performed test */
    uint16_t company_id;                /**< 16-bit Bluetooth assigned Company Identifier */
    uint8_t  count;                     /**< Number of elements in the fault array */
    uint8_t  fault_array[10];           /**< a sequence of 1-octet fault values */
} wiced_bt_mesh_health_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get the current Registered Fault state identified by Company ID of an element */
typedef PACKED struct
{
    uint16_t company_id;                /**< 16-bit Bluetooth assigned Company Identifier */
} wiced_bt_mesh_health_fault_get_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to clear the current Registered Fault state identified by Company ID of an element */
typedef PACKED struct
{
    uint16_t company_id;                /**< 16-bit Bluetooth assigned Company Identifier */
} wiced_bt_mesh_health_fault_clear_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to invoke a self-test procedure of an element */
typedef PACKED struct
{
    uint8_t  id;                        /**< Identifier of a specific test to be performed */
    uint16_t company_id;                /**< 16-bit Bluetooth assigned Company Identifier */
} wiced_bt_mesh_health_fault_test_data_t;

/* This structure contains information sent from the provisioner library to provisioner application to report the current Registered Fault state of an element */
typedef PACKED struct
{
    uint16_t app_key_idx;               /**< application key to use */
    uint8_t  test_id;                   /**< Identifier of a most recently performed test */
    uint16_t company_id;                /**< 16-bit Bluetooth assigned Company Identifier */
    uint8_t  count;                     /**< Number of elements in the fault array */
    uint8_t  fault_array[10];           /**< a sequence of 1-octet fault values */
} wiced_bt_mesh_health_fault_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to set the current Health Period state of an element */
typedef PACKED struct
{
    uint8_t  divisor;                   /**< Divider for the Publish Period. */
} wiced_bt_mesh_health_period_set_data_t;

/* This structure contains information sent from the provisioner library to provisioner application to report the Health Period state of an element */
typedef PACKED struct
{
    uint16_t app_key_idx;               /**< application key to use */
    uint8_t  divisor;                   /**< Divider for the Publish Period. */
} wiced_bt_mesh_health_period_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to set the Attention Timer state of an element */
typedef PACKED struct
{
    uint8_t  timer;                     /**< Value of the Attention Timer state */
} wiced_bt_mesh_health_attention_set_data_t;

/*/ This structure contains information sent from the provisioner library to provisioner application to report the current Attention Timer state of an element */
typedef PACKED struct
{
    uint8_t  timer;                     /**< Value of the Attention Timer state */
} wiced_bt_mesh_health_attention_status_data_t;

/* This structure contains information sent from the provisioner application to provisioner library to get the Low Power Node Poll Timeout state of an element */
typedef PACKED struct
{
    uint16_t lpn_addr;                  /**< The unicast address of the Low Power node */
} wiced_bt_mesh_lpn_poll_timeout_get_data_t;

/* This structure contains information sent to the provisioner application from provisioner library with current Poll Timeout value of the low power node */
typedef PACKED struct
{
    uint16_t lpn_addr;                  /**< The unicast address of the Low Power node */
    uint32_t poll_timeout;              /**< The current value of the PollTimeout timer of the Low Power node */
} wiced_bt_mesh_lpn_poll_timeout_status_data_t;

/* This structure contains information sent from the provisioner application to the provisioner library with new filter type. */
typedef PACKED struct
{
    uint8_t  type;                      /**< The proxy filter type (0-accept list, 1-reject list). */
} wiced_bt_mesh_proxy_filter_set_type_data_t;

/* This structure contains information sent from the provisioner application to the provisioner library with addresses to be added or deleted from the filter. */
typedef PACKED struct
{
    uint8_t  addr_num;                  /**< Number of addresses in the array */
    uint16_t addr[1];                   /**< First address to delete */
} wiced_bt_mesh_proxy_filter_change_addr_data_t;

/* This structure contains information sent to the provisioner application from the provisioner library with current filter setup information. */
typedef PACKED struct
{
    uint8_t  type;                      /**< The proxy filter type (0-accept list, 1-reject list). */
    uint16_t list_size;                 /**< Number of addresses in the proxy filter list. */
} wiced_bt_mesh_proxy_filter_status_data_t;

/**
 * \brief Provision Server callback
 * \details The Provision Server callback is called by the Mesh Application library during
 * the provisioning of this device.  Application does not need to implement this callback
 * if the public key is not shared out of band and if out of band authentication is not
 * used
 *
 * @param       event Event that the application should process (see @ref PROVISION_EVENT "Mesh Provisioner Events")
 * @param       p_data Data with provisioning information.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_provision_server_callback_t)(uint16_t event, void *p_data);

/**
 * \brief Provision Server initialization
 * \details This function should be called by every Mesh Application during startup.
 * If the device is capable of sharing its public key out of band:  The pointer callback
 * is required if the device provides an out of band method to share the public key with
 * the Provisioner or if there is an out of band method to perform authentication.
 *
 * @param       pb_priv_key Device private key
 * @param       p_callback The application callback function that will be executed when the
 * application library needs out of band information.
 *
 * @return      None
 */

void wiced_bt_mesh_app_provision_server_init(uint8_t *pb_priv_key, wiced_bt_mesh_provision_server_callback_t *p_callback);

/**
 * \brief Provision Server Configuration
 * \details By default an application does not expose provisioning public key and does not use any of the
 * provisioning authentication methods. A real application should call this
 * function to provide device out of band provisioning capabilities.
 *
 * @param       p_config Out of band provisioning information
 *
 * @return      None
 */
void wiced_bt_mesh_app_provision_server_configure(wiced_bt_mesh_provision_capabilities_data_t *p_config);

/**
 * \brief Provision Set Out of Band data
 * \details Application should call this function to setup OOB data used during provisioing of the local device
 *
 * @param    p_oob Out of band data
 * @param    len   Out of band data length
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_set_oob(uint8_t *p_oob, uint8_t len);

/**
 * \brief Provision Set Out of Band data
 * \details Application should call this function to setup OOB data used during provisioing
 *
 * @param    p_event Pointer to the mesh event which identifies remote provisioning server
 * @param    p_oob Out of band data
 * @param    len   Out of band data length
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_client_set_oob(wiced_bt_mesh_event_t *p_event, uint8_t *p_oob, uint32_t len);

/**
 * \brief Provision Client callback
 * \details The Provision Client callback is called by the Mesh Provision library during the provisioning of a peer device
 *
 * @param       event Event that the application should process (see @ref PROVISION_EVENT "Mesh Provisioner Events")
 * @param       p_event Pointer to the data structure identifying the source of the message
 * @param       p_data Data with provisioning information.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_provision_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Provision Client Model initialization
 * \details The Provisioner Application should call this function during the startup to register a callback which will be executed during provisioning.
 *
 * @param       p_callback         The application callback function that will be executed by the Mesh Provisioning library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned     is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_provision_client_init(wiced_bt_mesh_provision_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Provision Local Device.
 * \details The application can call this function to setup local device to be a part of the network.
 *
 * @param[in]  p_data Pointer to the data structure with provisioning information.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_local_device_set(wiced_bt_mesh_local_device_set_data_t *p_data);

/**
 * \brief Set Device Key.
 * \details The application can call this function to setup device key.  Application can setup a key
 * once and then send multiple configuration messages.  When application decides to configure another
 * device, it needs to change associated device key.
 *
 * @param[in]  p_data Pointer to the data structure with device key information.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
void wiced_bt_mesh_provision_set_dev_key(wiced_bt_mesh_set_dev_key_data_t *p_data);

/**
 * \brief Add Vendor Model.
 * \details The application can call this function to add vendor model to mesh core.
 * Once added, mesh core forwards all vendor specific messages/event to vendor client message handler.
 *
 * @param[in]  p_data Pointer to the data structure with vendor model information.
 *
 * @return   None
 */
void wiced_bt_mesh_add_vendor_model(wiced_bt_mesh_add_vendor_model_data_t *p_data);

/**
 * \brief Set ADV Tx Power.
 * \details The application can call this function to set ADV Tx Power in the mesh core.
 * Once set, mesh core will use updated Tx power for subsequent advertisements.
 *
 * @param[in]  adv_tx_power ADV Tx Power value to set (0:min - 4:max).
 *
 * @return   None
 */
void wiced_bt_mesh_adv_tx_power_set(uint8_t adv_tx_power);

/* Maximum number of remembered device keys. Default value is 8. */
extern uint8_t wiced_bt_mesh_provision_dev_key_max_num;

/**
 * \brief Scan Unprovisioned Devices.
 * \details The application can call this function to start scanning for unprovisioned devices.
 *
 * @param[in]  start Set to 1 to start scan or 0 to stop scan.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_unprovisioned_devices(wiced_bool_t start);

/**
 * \brief Provision Connect.
 * \details The application can call this function to establish provisioning link.
 *
 * @param  p_event Pointer to the event structure which identified the remote provisioning server.
 * @param  p_data Pointer to the data structure with the uuid and address of the peer device.
 * @param  use_gatt If 1, provision over PB-GATT.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_connect(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_provision_connect_data_t *p_data, uint8_t use_gatt);

/**
 * \brief Provision Disconnect.
 * \details The application can call this function to disconnect provisioning link.
 *
 * @param  p_event Pointer to the event structure which identified the remote provisioning server.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_disconnect(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Provision Start.
 * \details The application can call this function to start provisioning of the device over established provisioning link.
 *
 * @param  p_event Pointer to the event structure which identified the remote provisioning server.
 * @param  p_data Pointer to the data structure with parameters that the application selected for provisioning.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_start(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_provision_start_data_t *p_data);

/**
 * \brief Proxy Connect.
 * \details The application can call this function to establish GATT Proxy connection.
 * Depending on the parameters the connection should be establishing either to a device
 * advertising it's identity, or a device advertising a mesh network
 *
 * @param  p_data Pointer to the data structure with the node identity or network.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_client_proxy_connect(wiced_bt_mesh_proxy_connect_data_t *p_data);

/**
 * \brief Proxy Disonnect.
 * \details The application can call this function to terminat GATT Proxy connection.
 *
 * @param  None
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_client_proxy_disconnect(void);

/**
 * \brief Config Client callback
 * \details The Config Client callback is called by the Mesh Core library on receiving a Foundation Model Configuration Status messages from the peer
 *
 * @param       event Event that the application should process (see @ref CONFIG_EVENT "Mesh Config Events")
 * @param       p_data Parsed data received in the status message.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_config_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Config Client Model initialization
 * \details The Provisioner Application should call this function during startup to register a callback which will be executed when a configuration status message is received.
 *
 * @param       p_callback          The application callback function that will be executed by the Mesh Model library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned      is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_config_client_init(wiced_bt_mesh_config_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Connection status change notification
 * \details The Provisioner Application should call this function to indicate when connection to the proxy goes up or down.
 *
 * @param       is_connected If TRUE, Connection to the application has been established.
 *
 * @return      None
 */
void wiced_bt_mesh_config_client_connection_state_change(wiced_bool_t is_connected);

/**
 * \brief Config Client Model Message Handler
 * \details Application Library typically calls this function when function to process a message received from the Config Server.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_config_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Node Reset.
 * \details The application can call this function to send
 * a Config Node Reset message to a peer device.
 *
 * @param[in]  p_data Pointer to the data structure with the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_node_reset(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Get Secure Beacon state.
 * \details The application can call this function to send
 * a Config Beacon Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_beacon_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Set Secure Beacon state.
 * \details The application can call this function to send
 * a Config Beacon Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set Pointer to the data structure  of Config Beacon Set message.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_beacon_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_beacon_set_data_t *p_set);

/**
 * \brief Get Composition Data of the device.
 * \details The application can call this function to send
 * a Composition Data Get message to a peer device.
 *
 * @param[in]  p_data  Pointer to the data structure with the address of the peer device.
 * @param[in]  p_get    Pointer to the data structure of Composition Data .
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_composition_data_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_composition_data_get_data_t *p_get);

/**
 * \brief Get Default TTL.
 * \details The application can call this function to send
 * a Config Default TTL Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_default_ttl_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Set Default TTL.
 * \details The application can call this function to send
 * a Config Default TTL Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of new Default TTL value.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_default_ttl_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_default_ttl_set_data_t *p_set);

/**
 * \brief Get Relay Status.
 * \details The application can call this function to send
 * a Config Relay Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_relay_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Set Relay Status.
 * \details The application can call this function to send
 * a Config Relay Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set Relay Status.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_relay_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_relay_set_data_t *p_set);

/**
 * \brief Get GATT Proxy State.
 * \details The application can call this function to send a Config GATT Proxy Get message to a peer device.
 *
 * @param[in]   p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_gatt_proxy_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Set GATT Proxy State.
 * \details The application can call this function to send a Config GATT Proxy Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set GATT Proxy State..
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_gatt_proxy_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_gatt_proxy_set_data_t *p_set);

/**
 * \brief Get Friend State.
 * \details The application can call this function to send
 * a Config Friend Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_friend_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Set Friend State.
 * \details The application can call this function to send
 * a Config Friend Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set Friend State.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_friend_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_friend_set_data_t *p_set);

/**
 * \brief Get Key Refresh Phase.
 * \details The application can call this function to to get the current Key Refresh
 * Phase state of the identified network key.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_get   Pointer to the data structure of Get Key Refresh Phase.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_key_refresh_phase_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_key_refresh_phase_get_data_t *p_get);

/**
 * \brief Set Key Refresh Phase.
 * \details The application can call this function to set
 * the Key Refresh Phase state of the identified network key.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set Key Refresh Phase.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_key_refresh_phase_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_key_refresh_phase_set_data_t *p_set);

/**
 * \brief Get Node Identity State.
 * \details The application can call this function to send
 * a Config Node Identity Get message to a peer device.
 *
 * @param[in]  p_event  Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_get    Pointer to the data structure of Get Node Identity State.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_node_identity_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_node_identity_get_data_t *p_get);

/**
 * \brief Set Node Identity State.
 * \details The application can call this function to send
 * a Config Node Identity message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set Node Identity State.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_node_identity_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_node_identity_set_data_t *p_set);

/**
 * \brief Get Model Publication Information.
 * \details The application can call this function to send
 * a Config Model Publication Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_get   Pointer to the data structure of Get Model Publication Information.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_publication_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_publication_get_data_t *p_get);

/**
 * \brief Set Model Publication.
 * \details The application can call this function to send
 * a Config Model Publication Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Set Model Publication.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_publication_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_publication_set_data_t *p_set);

/**
 * \brief Change Model Subscription.
 * \details The application can call this function to change a model subcription data by sending
 * a Config Model Subscription Add, Virtual Address Add, Delete, Virtual Address Delete, Overwrite, Virtual Address Overwrite, or Delete All message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Set Model Publication.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_subscription_change(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_subscription_change_data_t *p_data);

/**
 * \brief Get Model Subscription.
 * \details The application can call this function to get current model subcription list by sending
 * a Config Model Subscription Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Get Model Publication.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_subscription_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_subscription_get_data_t *p_data);

/**
 * \brief Change Network Key.
 * \details The application can call this function to change a device network keys by sending
 * a Config NetKey Add, Delete, or Update message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Change Network Key.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_netkey_change(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_netkey_change_data_t *p_data);

/**
 * \brief Get Network Keys.
 * \details The application can call this function to get current network keys list by sending
 * a Config NetKey Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_netkey_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Change Application Key.
 * \details The application can call this function to change a device application key by sending
 * a Config AppKey Add, Delete, or Update message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Change Application Key.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_appkey_change(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_appkey_change_data_t *p_data);

/**
 * \brief Get Application Keys.
 * \details The application can call this function to get current application keys list by sending
 * a Config AppKey Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Get Application Key.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_appkey_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_appkey_get_data_t *p_data);

/**
 * \brief Change Model to Application Key Binding.
 * \details The application can call this function to change a a model to application key binding by sending
 * a Config Model App Bind o Unbind message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Change Model to Application Key Binding.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_app_bind(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_app_bind_data_t *p_data);

/**
 * \brief Get Application Keys the Model is bound to.
 * \details The application can call this function to get current application keys list bound to a specific Model by sending
 * a Config Model App Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_data  Pointer to the data structure of Get Application Keys the Model is bound to.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_model_app_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_model_app_get_data_t *p_data);

/**
 * \brief Heartbeat Subscription Get.
 * \details The application can call the Heartbeat Subscription Get to send
 * Heartbeat Subscription Get message to a peer device.
 *
 * @param[in] p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_heartbeat_subscription_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Heartbeat Subscription Set.
 * \details The application can call the Heartbeat Subscription Set to send
 * Heartbeat Subscription Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Get Application Keys the Model is bound to.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_heartbeat_subscription_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_heartbeat_subscription_set_data_t *p_set);

/**
 * \brief Heartbeat Publication Get.
 * \details The application can call the Heartbeat Publication Get to send
 * Heartbeat Publication Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_heartbeat_publication_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Heartbeat Publication Set.
 * \details The application can call the Heartbeat Publication Set to send
 * Heartbeat Publication Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Heartbeat Publication Set information
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_heartbeat_publication_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_heartbeat_publication_set_data_t *p_set);

/**
 * \brief Network Transmit Parameters Get.
 * \details The application can call this function to send
 * a Config Network Transmit Get message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_network_transmit_params_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Network Transmit Parameters Set.
 * \details The application can call this function to send
 * a Config Network Transmit Set message to a peer device.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Network Transmit Parameters Set.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_config_network_transmit_params_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_config_network_transmit_set_data_t *p_set);

/**
 * \brief Health Client callback
 * \details The Health Client callback is called by the Mesh Core library on receiving a Foundation Model Health Status messages from the peer
 *
 * @param       event Event that the application should process (see @ref HEALTH_EVENT "Mesh Health Events")
 * @param       p_event Mesh event with information about received message or failed tx message.
 * @param       p_data Parsed data received in the status message.
 *
 * @return      None
*/
typedef void(wiced_bt_mesh_health_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Health Client Model initialization
 * \details The Provisioner Application should call this function during startup to register a callback which will be executed when a health status message is received.
 *
 * @param       p_callback         The application callback function that will be executed by the Mesh Model library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned     is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_health_client_init(wiced_bt_mesh_health_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Health Client Model Message Handler
 * \details Application Library typically calls this function when function to process a message received from the Health Server.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_health_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Health Fault Get.
 * \details The application can call this function to get the current Registered Fault state identified by Company ID of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_get   Pointer to the data structure of  Health Fault Get.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_fault_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_fault_get_data_t *p_get);

/**
 * \brief Health Fault Clear.
 * \details The application can call this function to clear the current Registered Fault state identified by Company ID of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_clear Pointer to the data structure of Health Fault Clear.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_fault_clear(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_fault_clear_data_t *p_clear);

/**
 * \brief Health Fault Test.
 * \details The application can call this function to invoke a self-test procedure of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_clear Pointer to the data structure of Health Fault Test.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_fault_test(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_fault_test_data_t *p_clear);

/**
 * \brief Health Period Get.
 * \details The application can call this function to get the current Health Period state of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_period_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Health Period Set.
 * \details The application can call this function to set the current Health Period state of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Health Period Set.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_period_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_period_set_data_t *p_set);

/**
 * \brief Health Attention Get.
 * \details The application can call this function to get the Attention Timer state of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_attention_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Health Attention Set.
 * \details The application can call this function to set the Attention Timer state of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of Health Attention Set.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_attention_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_attention_set_data_t *p_set);

/**
 * \brief LPN Poll Timeout Get.
 * \details The application can call this function to get the LPN Poll Timeout value.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_get   Pointer to the data structure of LPN Poll Timeout Get.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_lpn_poll_timeout_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_lpn_poll_timeout_get_data_t *p_get);

/**
 * \brief LPN Poll Timeout Set.
 * \details The application can call this function to set the Attention Timer state of an element.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure of LPN Poll Timeout Set.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_health_attention_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_health_attention_set_data_t *p_set);

/**
 * \brief Proxy Client callback
 * \details The Proxy Client callback is called by the Mesh Core library on receiving a Proxy Status messages from the peer
 *
 * @param       event Event that the application should process (see @ref WICED_BT_MESH_CORE_CMD_SPECIAL "Proxy Events")
 * @param       p_event Mesh event with information about received message or failed tx message.
 * @param       p_data Parsed data received in the status message.
 *
 * @return      None
*/
typedef void(wiced_bt_mesh_proxy_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief  Mesh GATT client initialization.
 * \details The Provisioner Application should call this function during startup to register a callback which will be executed when a proxy status message is received.
 *
 * @param       p_callback         The application callback function that will be executed by the Mesh Model library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned     is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_client_init(wiced_bt_mesh_provision_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Process connectable advertisement
 * \details Process connectable advertisement packet.
 *
 * @param       bd_addr Advertiser device address
 * @param       ble_addr_type Advertiser device address type
 * @param       rssi RSSI of received advertisements
 * @param       p_adv_data Pointer to advertising data
 *
 * @return      None
 */
void wiced_bt_mesh_gatt_client_process_connectable_adv(uint8_t *bd_addr, uint8_t ble_addr_type, int8_t rssi, uint8_t *p_adv_data);

/**
 * \brief Proxy Client Model initialization
 * \details The Provisioner Application should call this function during startup to register a callback which will be executed when a proxy status message is received.
 *
 * @param       p_callback         The application callback function that will be executed by the Mesh Model library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned     is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_proxy_client_init(wiced_bt_mesh_proxy_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Proxy Client Message Handler
 * \details Application Library typically calls this function when function to process a message received from the Proxy Server.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_proxy_client_message_handler(wiced_bt_mesh_event_t *p_event, const uint8_t *p_data, uint16_t data_len);

/**
 * \brief Proxy Client Set Filter Type.
 * \details The application can call this function to change the proxy filter type and clear the proxy filter list.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  p_set   Pointer to the data structure with the type of the filter to set.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_proxy_set_filter_type(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_proxy_filter_set_type_data_t *p_set);

/**
 * \brief Proxy Client Change Filter.
 * \details The function can be called by the application to add or delete addresses from the proxy filter list.
 *
 * @param[in]  p_event Mesh event with information about received message which contain the address of the peer device.
 * @param[in]  is_add   True: add address, False: delete address
 * @param[in]  p_addr   Pointer to the data structure of filter addresses
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_proxy_filter_change_addr(wiced_bt_mesh_event_t *p_event, wiced_bool_t is_add, wiced_bt_mesh_proxy_filter_change_addr_data_t *p_addr);

/**
 * \brief Remote Provisioning Server Model Message Handler
 * \details Application Library typically calls this function to process a message received from the Remote Provisioning Client.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param[in]       p_event Mesh event with information about received message.
 * @param[in]       p_data Pointer to the data portion of the message
 * @param[in]       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scan Capabilities Get.
 * \details The function can be called by the application to Send Remote Provisioning Scan Capabilities Get to Remote Provisioning Server.
 *
 * @param[in]       p_event Mesh event with information about contains information about destination address and transmission parameters.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_capabilities_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Scan Info Get.
 * \details The function can be called by the application to Send Remote Provisioning Scan Get to Remote Provisioning Server.
 *
 * @param[in]       p_event Mesh event with information about contains information about destination address and transmission parameters.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Scan Start.
 * \details The function can be called by the application to Send Remote Provisioning Scan Start to Remote Provisioning Server.
 *
 * @param[in]       p_event Mesh event with information about contains information about destination address and transmission parameters.
 * @param[in]       p_data Scan Parameters
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_start(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_provision_scan_start_data_t *p_data);

/**
 * \brief Extended Scan Info Get.
 * \details The function can be called by the application to Send Remote Provisioning Extended Scan Info Get message to Remote Provisioning Server.
 *
 * @param[in]       p_event Mesh event with information about contains information about destination address and transmission parameters.
 * @param[in]       p_data Extended Scan Info Get Parameters
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_extended_start(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_provision_scan_extended_start_t *p_data);

/**
 * \brief Scan Stop.
 * \details The function can be called by the application to Send Remote Provisioning Scan Stop to Remote Provisioning Server.
 *
 * @param[in]       p_event Mesh event with information about contains information about destination address and transmission parameters.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_scan_stop(wiced_bt_mesh_event_t *p_event);

#ifdef __cplusplus
}
#endif

#if defined _WIN32 || defined WICEDX_LINUX || defined __ANDROID__ || defined __APPLE__
#pragma pack()
#endif


/* @} wiced_bt_mesh_provisioning */

#endif /* __WICED_BT_MESH_PROVISION_H__ */
