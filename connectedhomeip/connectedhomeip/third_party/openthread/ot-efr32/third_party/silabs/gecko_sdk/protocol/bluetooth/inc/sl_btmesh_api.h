/***************************************************************************//**
 * @brief SL_BTMESH_API command declarations
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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


#ifndef SL_BTMESH_API_H
#define SL_BTMESH_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_bgapi.h"





/**
 * @addtogroup sl_btmesh_node Mesh Node
 * @{
 *
 * @brief Mesh Node
 *
 * Bluetooth mesh stack API for unprovisioned devices and provisioned nodes.
 *
 * <b>Initialization:</b>
 *   - @ref sl_btmesh_node_init : Initialize node
 *   - @ref sl_btmesh_node_init_oob : Initialize node with out-of-band
 *     authentication
 *   - @ref sl_btmesh_evt_node_initialized : Node initialized
 *
 * <b>Provisioning a node:</b>
 *   - @ref sl_btmesh_node_get_uuid : Get device UUID
 *   - @ref sl_btmesh_node_start_unprov_beaconing : Start unprovisioned device
 *     beaconing
 *   - @ref sl_btmesh_node_stop_unprov_beaconing : Stop unprovisioned device
 *     beaconing
 *   - @ref sl_btmesh_evt_node_provisioning_started : Provisioning process has
 *     started
 *   - @ref sl_btmesh_evt_node_input_oob_request : Request to input out-of-band
 *     authentication data
 *   - @ref sl_btmesh_node_send_input_oob_request_response : Respond to input
 *     out-of-band authentication request
 *   - @ref sl_btmesh_evt_node_display_output_oob : Request to display output
 *     out-of-band authentication data
 *   - @ref sl_btmesh_evt_node_static_oob_request : Request for static
 *     out-of-band authentication data
 *   - @ref sl_btmesh_node_send_static_oob_request_response : Respond to static
 *     out-of-band authentication request
 *   - @ref sl_btmesh_evt_node_provisioned : Node has been provisioned
 *   - @ref sl_btmesh_evt_node_provisioning_failed : Provisioning process has
 *     failed
 *   - @ref sl_btmesh_node_set_provisioning_data : Pre-provision a device
 *
 * <b>Node Configuration:</b>
 *   - @ref sl_btmesh_evt_node_key_added : A cryptographic key has been added to
 *     the node
 *   - @ref sl_btmesh_evt_node_config_get : Node-wide configuration has been
 *     queried
 *   - @ref sl_btmesh_evt_node_config_set : Node-wide configuration has been
 *     modified
 *   - @ref sl_btmesh_evt_node_model_config_changed : Model configuration has
 *     been modified
 *   - @ref sl_btmesh_node_reset : Factory reset mesh node
 *
 * <b>Note on Bluetooth mesh addresses</b>
 *
 * Bluetooth mesh address space is divided into sections containing ranges of
 * addresses of various types. Different address types are used in different
 * contexts. Some requests accept only certain address types.
 *
 * The address types are as follows:
 *   - <b>0x0000 Unassigned address:</b> represents an address that has not been
 *     set
 *   - <b>0x0001..0x7fff Unicast addresses</b> are allocated by the Provisioner
 *     to provisioned nodes. Each element of a node has its own unicast address.
 *   - <b>0x8000..0xbfff Virtual addresses</b> are 16-bit shorthand for 128-bit
 *     label UUIDs which are pre-allocated to specific purposes in relevant
 *     Bluetooth SIG specifications. Virtual addresses can typically be used in
 *     the same context as group addresses. Some commands require specifying the
 *     full label UUID instead of the virtual address shorthand.
 *   - <b>0xc000..0xffef Group addresses</b> are allocated by the Provisioner
 *     for multicast communication.
 *   - <b>0xfff0..0xffff Fixed group addresses</b> are allocated in the Mesh
 *     specification for multicast communication in a particular context. They
 *     can be used in the same context as regular group addresses. The following
 *     addresses are currently defined:
 *       - 0xfffc All-proxies broadcast address
 *       - 0xfffd All-friends broadcast address
 *       - 0xfffe All-relays broadcast address
 *       - 0xffff All-nodes broadcast address
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_node_init_id                                       0x00140028
#define sl_btmesh_cmd_node_set_exportable_keys_id                        0x24140028
#define sl_btmesh_cmd_node_start_unprov_beaconing_id                     0x01140028
#define sl_btmesh_cmd_node_stop_unprov_beaconing_id                      0x16140028
#define sl_btmesh_cmd_node_get_rssi_id                                   0x17140028
#define sl_btmesh_cmd_node_send_input_oob_request_response_id            0x02140028
#define sl_btmesh_cmd_node_get_uuid_id                                   0x03140028
#define sl_btmesh_cmd_node_set_provisioning_data_id                      0x04140028
#define sl_btmesh_cmd_node_init_oob_id                                   0x05140028
#define sl_btmesh_cmd_node_set_ivrecovery_mode_id                        0x06140028
#define sl_btmesh_cmd_node_get_ivrecovery_mode_id                        0x07140028
#define sl_btmesh_cmd_node_get_statistics_id                             0x09140028
#define sl_btmesh_cmd_node_clear_statistics_id                           0x0a140028
#define sl_btmesh_cmd_node_set_net_relay_delay_id                        0x0b140028
#define sl_btmesh_cmd_node_get_net_relay_delay_id                        0x0c140028
#define sl_btmesh_cmd_node_get_ivupdate_state_id                         0x0d140028
#define sl_btmesh_cmd_node_request_ivupdate_id                           0x0e140028
#define sl_btmesh_cmd_node_get_seq_remaining_id                          0x0f140028
#define sl_btmesh_cmd_node_save_replay_protection_list_id                0x10140028
#define sl_btmesh_cmd_node_set_uuid_id                                   0x11140028
#define sl_btmesh_cmd_node_get_replay_protection_list_status_id          0x14140028
#define sl_btmesh_cmd_node_get_element_address_id                        0x12140028
#define sl_btmesh_cmd_node_send_static_oob_request_response_id           0x13140028
#define sl_btmesh_cmd_node_reset_id                                      0x15140028
#define sl_btmesh_cmd_node_set_beacon_reporting_id                       0x18140028
#define sl_btmesh_cmd_node_set_iv_update_age_id                          0x19140028
#define sl_btmesh_cmd_node_get_key_count_id                              0x1a140028
#define sl_btmesh_cmd_node_get_key_id                                    0x1b140028
#define sl_btmesh_cmd_node_get_networks_id                               0x1c140028
#define sl_btmesh_cmd_node_get_element_seqnum_id                         0x1d140028
#define sl_btmesh_cmd_node_set_model_option_id                           0x1e140028
#define sl_btmesh_cmd_node_get_local_dcd_id                              0x1f140028
#define sl_btmesh_cmd_node_power_off_id                                  0x21140028
#define sl_btmesh_cmd_node_set_adv_phy_id                                0x22140028
#define sl_btmesh_cmd_node_get_adv_phy_id                                0x23140028
#define sl_btmesh_cmd_node_set_unprov_beaconing_adv_interval_id          0x28140028
#define sl_btmesh_cmd_node_set_proxy_service_adv_interval_id             0x29140028
#define sl_btmesh_cmd_node_set_provisioning_service_adv_interval_id      0x30140028
#define sl_btmesh_rsp_node_init_id                                       0x00140028
#define sl_btmesh_rsp_node_set_exportable_keys_id                        0x24140028
#define sl_btmesh_rsp_node_start_unprov_beaconing_id                     0x01140028
#define sl_btmesh_rsp_node_stop_unprov_beaconing_id                      0x16140028
#define sl_btmesh_rsp_node_get_rssi_id                                   0x17140028
#define sl_btmesh_rsp_node_send_input_oob_request_response_id            0x02140028
#define sl_btmesh_rsp_node_get_uuid_id                                   0x03140028
#define sl_btmesh_rsp_node_set_provisioning_data_id                      0x04140028
#define sl_btmesh_rsp_node_init_oob_id                                   0x05140028
#define sl_btmesh_rsp_node_set_ivrecovery_mode_id                        0x06140028
#define sl_btmesh_rsp_node_get_ivrecovery_mode_id                        0x07140028
#define sl_btmesh_rsp_node_get_statistics_id                             0x09140028
#define sl_btmesh_rsp_node_clear_statistics_id                           0x0a140028
#define sl_btmesh_rsp_node_set_net_relay_delay_id                        0x0b140028
#define sl_btmesh_rsp_node_get_net_relay_delay_id                        0x0c140028
#define sl_btmesh_rsp_node_get_ivupdate_state_id                         0x0d140028
#define sl_btmesh_rsp_node_request_ivupdate_id                           0x0e140028
#define sl_btmesh_rsp_node_get_seq_remaining_id                          0x0f140028
#define sl_btmesh_rsp_node_save_replay_protection_list_id                0x10140028
#define sl_btmesh_rsp_node_set_uuid_id                                   0x11140028
#define sl_btmesh_rsp_node_get_replay_protection_list_status_id          0x14140028
#define sl_btmesh_rsp_node_get_element_address_id                        0x12140028
#define sl_btmesh_rsp_node_send_static_oob_request_response_id           0x13140028
#define sl_btmesh_rsp_node_reset_id                                      0x15140028
#define sl_btmesh_rsp_node_set_beacon_reporting_id                       0x18140028
#define sl_btmesh_rsp_node_set_iv_update_age_id                          0x19140028
#define sl_btmesh_rsp_node_get_key_count_id                              0x1a140028
#define sl_btmesh_rsp_node_get_key_id                                    0x1b140028
#define sl_btmesh_rsp_node_get_networks_id                               0x1c140028
#define sl_btmesh_rsp_node_get_element_seqnum_id                         0x1d140028
#define sl_btmesh_rsp_node_set_model_option_id                           0x1e140028
#define sl_btmesh_rsp_node_get_local_dcd_id                              0x1f140028
#define sl_btmesh_rsp_node_power_off_id                                  0x21140028
#define sl_btmesh_rsp_node_set_adv_phy_id                                0x22140028
#define sl_btmesh_rsp_node_get_adv_phy_id                                0x23140028
#define sl_btmesh_rsp_node_set_unprov_beaconing_adv_interval_id          0x28140028
#define sl_btmesh_rsp_node_set_proxy_service_adv_interval_id             0x29140028
#define sl_btmesh_rsp_node_set_provisioning_service_adv_interval_id      0x30140028

/**
 * @brief Flags for supported OOB authentication methods during provisioning,
 * which use a bitmap so that multiple methods can be supported.
 */
typedef enum
{
  sl_btmesh_node_auth_method_flag_none   = 0x1, /**< (0x1) Authentication
                                                     without OOB is supported */
  sl_btmesh_node_auth_method_flag_static = 0x2, /**< (0x2) Static OOB data
                                                     authentication is supported */
  sl_btmesh_node_auth_method_flag_input  = 0x4, /**< (0x4) Input OOB
                                                     authentication is supported */
  sl_btmesh_node_auth_method_flag_output = 0x8  /**< (0x8) Output OOB
                                                     authentication is supported */
} sl_btmesh_node_auth_method_flag_t;

/**
 * @brief Flags for supported input OOB actions during provisioning, which use a
 * bitmap so that multiple actions can be supported.
 */
typedef enum
{
  sl_btmesh_node_oob_input_action_flag_push    = 0x1, /**< (0x1) Push a button
                                                           on the device. */
  sl_btmesh_node_oob_input_action_flag_twist   = 0x2, /**< (0x2) Twist a dial on
                                                           the device. */
  sl_btmesh_node_oob_input_action_flag_numeric = 0x4, /**< (0x4) Input a numeric
                                                           authentication code. */
  sl_btmesh_node_oob_input_action_flag_alpha   = 0x8  /**< (0x8) Input an
                                                           alphanumeric
                                                           authentication code. */
} sl_btmesh_node_oob_input_action_flag_t;

/**
 * @brief Indicate the input OOB action selected by the Provisioner during
 * provisioning of the device.
 */
typedef enum
{
  sl_btmesh_node_oob_input_action_push    = 0x0, /**< (0x0) Push a button on the
                                                      device. */
  sl_btmesh_node_oob_input_action_twist   = 0x1, /**< (0x1) Twist a dial on the
                                                      device. */
  sl_btmesh_node_oob_input_action_numeric = 0x2, /**< (0x2) Input a numeric
                                                      authentication code. */
  sl_btmesh_node_oob_input_action_alpha   = 0x3  /**< (0x3) Input an
                                                      alphanumeric
                                                      authentication code. */
} sl_btmesh_node_oob_input_action_t;

/**
 * @brief Flags for supported output OOB actions during provisioning, which use
 * a bitmap so that multiple actions can be supported.
 */
typedef enum
{
  sl_btmesh_node_oob_output_action_flag_blink   = 0x1,  /**< (0x1) Blink a
                                                             light. */
  sl_btmesh_node_oob_output_action_flag_beep    = 0x2,  /**< (0x2) Emit a sound. */
  sl_btmesh_node_oob_output_action_flag_vibrate = 0x4,  /**< (0x4) Vibrate the
                                                             device. */
  sl_btmesh_node_oob_output_action_flag_numeric = 0x8,  /**< (0x8) Output a
                                                             numeric
                                                             authentication
                                                             code. */
  sl_btmesh_node_oob_output_action_flag_alpha   = 0x10  /**< (0x10) Output an
                                                             alphanumeric
                                                             authentication
                                                             code. */
} sl_btmesh_node_oob_output_action_flag_t;

/**
 * @brief Indicate the output OOB action selected by the Provisioner during
 * provisioning of the device.
 */
typedef enum
{
  sl_btmesh_node_oob_output_action_blink   = 0x0, /**< (0x0) Blink a light. */
  sl_btmesh_node_oob_output_action_beep    = 0x1, /**< (0x1) Emit a sound. */
  sl_btmesh_node_oob_output_action_vibrate = 0x2, /**< (0x2) Vibrate the device. */
  sl_btmesh_node_oob_output_action_numeric = 0x3, /**< (0x3) Output a numeric
                                                       authentication code. */
  sl_btmesh_node_oob_output_action_alpha   = 0x4  /**< (0x4) Output an
                                                       alphanumeric
                                                       authentication code. */
} sl_btmesh_node_oob_output_action_t;

/**
 * @brief Specify the type of a key in key manipulation commands.
 */
typedef enum
{
  sl_btmesh_node_key_type_net = 0x0, /**< (0x0) Network key */
  sl_btmesh_node_key_type_app = 0x1  /**< (0x1) Application key */
} sl_btmesh_node_key_type_t;

/**
 * @brief Specify the state to which a Configuration Client/Server command/event
 * applies.
 */
typedef enum
{
  sl_btmesh_node_dcd         = 0x8008, /**< (0x8008) Device Composition Data */
  sl_btmesh_node_beacon      = 0x8009, /**< (0x8009) Status of broadcasting
                                            Secure Network Beacons */
  sl_btmesh_node_default_ttl = 0x800c, /**< (0x800c) Default Time-To-Live for
                                            messages */
  sl_btmesh_node_friendship  = 0x800f, /**< (0x800f) Friend status */
  sl_btmesh_node_gatt_proxy  = 0x8012, /**< (0x8012) GATT proxy status */
  sl_btmesh_node_key_refresh = 0x8015, /**< (0x8015) Key refresh status */
  sl_btmesh_node_relay       = 0x8023, /**< (0x8023) Relay status */
  sl_btmesh_node_identity    = 0x8042, /**< (0x8042) Identity status */
  sl_btmesh_node_nettx       = 0x8024  /**< (0x8024) Network transmit status */
} sl_btmesh_node_config_state_t;

/**
 * @addtogroup sl_btmesh_evt_node_initialized sl_btmesh_evt_node_initialized
 * @{
 * @brief Node is initialized and operational.
 */

/** @brief Identifier of the initialized event */
#define sl_btmesh_evt_node_initialized_id                                0x001400a8

/***************************************************************************//**
 * @brief Data structure of the initialized event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_initialized_s
{
  uint8_t  provisioned; /**< 1 if node is provisioned into a network, 0 if
                             unprovisioned. */
  uint16_t address;     /**< Unicast address of the primary element of the node.
                             Ignored if unprovisioned. Secondary elements have
                             been assigned sequential unicast addresses
                             following the primary element address. */
  uint32_t iv_index;    /**< IV index for the first network of the node, ignore
                             if unprovisioned. */
});

typedef struct sl_btmesh_evt_node_initialized_s sl_btmesh_evt_node_initialized_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_initialized

/**
 * @addtogroup sl_btmesh_evt_node_provisioned sl_btmesh_evt_node_provisioned
 * @{
 * @brief The node has received provisioning data (address allocation and a
 * network key) from the Provisioner
 *
 * A @ref sl_btmesh_evt_node_key_added event will follow for the network key.
 *
 * The node is now ready for further configuration by the Provisioner but is not
 * yet ready for communication with other nodes in the network (it does not have
 * any application keys and its models have not been set up).
 */

/** @brief Identifier of the provisioned event */
#define sl_btmesh_evt_node_provisioned_id                                0x011400a8

/***************************************************************************//**
 * @brief Data structure of the provisioned event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_provisioned_s
{
  uint16_t address;  /**< The unicast address that the Provisioner allocated for
                          the primary element of the node. Secondary elements
                          have been assigned sequentially following unicast
                          addresses. */
  uint32_t iv_index; /**< Current IV index of the provisioned network */
});

typedef struct sl_btmesh_evt_node_provisioned_s sl_btmesh_evt_node_provisioned_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_provisioned

/**
 * @addtogroup sl_btmesh_evt_node_config_get sl_btmesh_evt_node_config_get
 * @{
 * @brief Informative; Configuration Client requested the current value of a
 * State in the Configuration Server Model.
 */

/** @brief Identifier of the config_get event */
#define sl_btmesh_evt_node_config_get_id                                 0x021400a8

/***************************************************************************//**
 * @brief Data structure of the config_get event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_config_get_s
{
  uint16_t netkey_index; /**< The network key index of the network to which the
                              command applies. 0xffff for node-wide states. */
  uint16_t id;           /**< Enum @ref sl_btmesh_node_config_state_t. Specifies
                              to which State the command applies */
});

typedef struct sl_btmesh_evt_node_config_get_s sl_btmesh_evt_node_config_get_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_config_get

/**
 * @addtogroup sl_btmesh_evt_node_config_set sl_btmesh_evt_node_config_set
 * @{
 * @brief Informative; Configuration Client changes the State in the
 * Configuration Server Model.
 */

/** @brief Identifier of the config_set event */
#define sl_btmesh_evt_node_config_set_id                                 0x031400a8

/***************************************************************************//**
 * @brief Data structure of the config_set event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_config_set_s
{
  uint16_t   netkey_index; /**< The network key index of the network to which
                                the command applies. 0xffff for node-wide
                                states. */
  uint16_t   id;           /**< Enum @ref sl_btmesh_node_config_state_t.
                                Specifies to which state the command applies */
  uint8array value;        /**< The new value */
});

typedef struct sl_btmesh_evt_node_config_set_s sl_btmesh_evt_node_config_set_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_config_set

/**
 * @addtogroup sl_btmesh_evt_node_display_output_oob sl_btmesh_evt_node_display_output_oob
 * @{
 * @brief Display output OOB data so Provisioner can input it.
 */

/** @brief Identifier of the display_output_oob event */
#define sl_btmesh_evt_node_display_output_oob_id                         0x041400a8

/***************************************************************************//**
 * @brief Data structure of the display_output_oob event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_display_output_oob_s
{
  uint8_t    output_action; /**< Enum @ref sl_btmesh_node_oob_output_action_t.
                                 Selected output action */
  uint8_t    output_size;   /**< Size of data to output in characters. */
  uint8array data;          /**< Raw 16-byte array containing the output data
                                 value. */
});

typedef struct sl_btmesh_evt_node_display_output_oob_s sl_btmesh_evt_node_display_output_oob_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_display_output_oob

/**
 * @addtogroup sl_btmesh_evt_node_input_oob_request sl_btmesh_evt_node_input_oob_request
 * @{
 * @brief The Provisioner is displaying an out of band authentication value
 *
 * The application on the node should provide the value to the Bluetooth mesh
 * stack using the @ref sl_btmesh_node_send_input_oob_request_response command.
 */

/** @brief Identifier of the input_oob_request event */
#define sl_btmesh_evt_node_input_oob_request_id                          0x051400a8

/***************************************************************************//**
 * @brief Data structure of the input_oob_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_input_oob_request_s
{
  uint8_t input_action; /**< Enum @ref sl_btmesh_node_oob_input_action_t.
                             Selected input action */
  uint8_t input_size;   /**< Size of data in the input in characters. */
});

typedef struct sl_btmesh_evt_node_input_oob_request_s sl_btmesh_evt_node_input_oob_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_input_oob_request

/**
 * @addtogroup sl_btmesh_evt_node_provisioning_started sl_btmesh_evt_node_provisioning_started
 * @{
 * @brief Provisioner has started provisioning this node.
 */

/** @brief Identifier of the provisioning_started event */
#define sl_btmesh_evt_node_provisioning_started_id                       0x061400a8

/***************************************************************************//**
 * @brief Data structure of the provisioning_started event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_provisioning_started_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
});

typedef struct sl_btmesh_evt_node_provisioning_started_s sl_btmesh_evt_node_provisioning_started_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_provisioning_started

/**
 * @addtogroup sl_btmesh_evt_node_provisioning_failed sl_btmesh_evt_node_provisioning_failed
 * @{
 * @brief Provisioning the node has failed.
 */

/** @brief Identifier of the provisioning_failed event */
#define sl_btmesh_evt_node_provisioning_failed_id                        0x071400a8

/***************************************************************************//**
 * @brief Data structure of the provisioning_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_provisioning_failed_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
});

typedef struct sl_btmesh_evt_node_provisioning_failed_s sl_btmesh_evt_node_provisioning_failed_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_provisioning_failed

/**
 * @addtogroup sl_btmesh_evt_node_key_added sl_btmesh_evt_node_key_added
 * @{
 * @brief Received when a Configuration Client has deployed a new network or
 * application key to the node.
 */

/** @brief Identifier of the key_added event */
#define sl_btmesh_evt_node_key_added_id                                  0x081400a8

/***************************************************************************//**
 * @brief Data structure of the key_added event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_key_added_s
{
  uint8_t  type;         /**< Type of the new key. Values are as follows:
                                - <b>0x00:</b> Network key
                                - <b>0x01:</b> Application key */
  uint16_t index;        /**< Key index of the new key */
  uint16_t netkey_index; /**< Network key index to which the application key is
                              bound, which is ignored for network keys */
});

typedef struct sl_btmesh_evt_node_key_added_s sl_btmesh_evt_node_key_added_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_key_added

/**
 * @addtogroup sl_btmesh_evt_node_model_config_changed sl_btmesh_evt_node_model_config_changed
 * @{
 * @brief Informative
 *
 * This event notifies that a remote Configuration Client has changed the
 * configuration of a local model.
 */

/** @brief Identifier of the model_config_changed event */
#define sl_btmesh_evt_node_model_config_changed_id                       0x091400a8

/***************************************************************************//**
 * @brief Data structure of the model_config_changed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_model_config_changed_s
{
  uint8_t  node_config_state; /**< The configuration state which has changed.
                                   Values are as follows:
                                     - <b>0x00:</b> Model application key
                                       bindings
                                     - <b>0x01:</b> Model publication parameters
                                     - <b>0x02:</b> Model subscription list */
  uint16_t element_address;   /**< Address of the element which contains the
                                   model */
  uint16_t vendor_id;         /**< Vendor ID of the model; value 0xffff is used
                                   for Bluetooth SIG models. */
  uint16_t model_id;          /**< Model ID of the model */
});

typedef struct sl_btmesh_evt_node_model_config_changed_s sl_btmesh_evt_node_model_config_changed_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_model_config_changed

/**
 * @addtogroup sl_btmesh_evt_node_reset sl_btmesh_evt_node_reset
 * @{
 * @brief Provisioner has instructed the node to reset
 *
 * This event is generated when the Provisioner has ordered the node to be
 * reset. Stack data has already been reset. This event is generated to inform
 * the application that it should do its own cleanup duties and reset the
 * hardware.
 */

/** @brief Identifier of the reset event */
#define sl_btmesh_evt_node_reset_id                                      0x0a1400a8

/** @} */ // end addtogroup sl_btmesh_evt_node_reset

/**
 * @addtogroup sl_btmesh_evt_node_ivrecovery_needed sl_btmesh_evt_node_ivrecovery_needed
 * @{
 * @brief Network IV index recovery needed
 *
 * This event is generated when the node detects the network IV index is too far
 * in the future to be automatically updated. See @ref
 * sl_btmesh_node_set_ivrecovery_mode command.
 */

/** @brief Identifier of the ivrecovery_needed event */
#define sl_btmesh_evt_node_ivrecovery_needed_id                          0x0b1400a8

/***************************************************************************//**
 * @brief Data structure of the ivrecovery_needed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_ivrecovery_needed_s
{
  uint32_t node_iv_index;    /**< Current IV index of the node */
  uint32_t network_iv_index; /**< Received network IV index */
});

typedef struct sl_btmesh_evt_node_ivrecovery_needed_s sl_btmesh_evt_node_ivrecovery_needed_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_ivrecovery_needed

/**
 * @addtogroup sl_btmesh_evt_node_changed_ivupdate_state sl_btmesh_evt_node_changed_ivupdate_state
 * @{
 * @brief Network IV index update state has changed.
 */

/** @brief Identifier of the changed_ivupdate_state event */
#define sl_btmesh_evt_node_changed_ivupdate_state_id                     0x0c1400a8

/***************************************************************************//**
 * @brief Data structure of the changed_ivupdate_state event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_changed_ivupdate_state_s
{
  uint32_t iv_index; /**< Current IV index */
  uint8_t  state;    /**< Indicates whether the IV index update is ongoing (1)
                          or not (0). */
});

typedef struct sl_btmesh_evt_node_changed_ivupdate_state_s sl_btmesh_evt_node_changed_ivupdate_state_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_changed_ivupdate_state

/**
 * @addtogroup sl_btmesh_evt_node_static_oob_request sl_btmesh_evt_node_static_oob_request
 * @{
 * @brief Static out of band authentication data is needed in the provisioning
 *
 * The application on the node should provide the value to the Bluetooth mesh
 * stack using the @ref sl_btmesh_node_send_static_oob_request_response command.
 */

/** @brief Identifier of the static_oob_request event */
#define sl_btmesh_evt_node_static_oob_request_id                         0x0d1400a8

/** @} */ // end addtogroup sl_btmesh_evt_node_static_oob_request

/**
 * @addtogroup sl_btmesh_evt_node_key_removed sl_btmesh_evt_node_key_removed
 * @{
 * @brief Received when a Configuration Client removes a network or application
 * key from the node.
 */

/** @brief Identifier of the key_removed event */
#define sl_btmesh_evt_node_key_removed_id                                0x0e1400a8

/***************************************************************************//**
 * @brief Data structure of the key_removed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_key_removed_s
{
  uint8_t  type;         /**< Type of the removed key. Values are as follows:
                                - <b>0x00:</b> Network key
                                - <b>0x01:</b> Application key */
  uint16_t index;        /**< Key index of the removed key */
  uint16_t netkey_index; /**< Network key index to which the application key is
                              bound, which is ignored for network keys */
});

typedef struct sl_btmesh_evt_node_key_removed_s sl_btmesh_evt_node_key_removed_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_key_removed

/**
 * @addtogroup sl_btmesh_evt_node_key_updated sl_btmesh_evt_node_key_updated
 * @{
 * @brief Received when a Configuration Client updates a network or application
 * key of the node.
 */

/** @brief Identifier of the key_updated event */
#define sl_btmesh_evt_node_key_updated_id                                0x0f1400a8

/***************************************************************************//**
 * @brief Data structure of the key_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_key_updated_s
{
  uint8_t  type;         /**< Type of the updated key. Values are as follows:
                                - <b>0x00:</b> Network key
                                - <b>0x01:</b> Application key */
  uint16_t index;        /**< Key index of the updated key */
  uint16_t netkey_index; /**< Network key index to which the application key is
                              bound. Ignore this value if the event is for
                              network key updates. */
});

typedef struct sl_btmesh_evt_node_key_updated_s sl_btmesh_evt_node_key_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_key_updated

/**
 * @addtogroup sl_btmesh_evt_node_heartbeat sl_btmesh_evt_node_heartbeat
 * @{
 * @brief Indicates reception of heartbeat message
 */

/** @brief Identifier of the heartbeat event */
#define sl_btmesh_evt_node_heartbeat_id                                  0x101400a8

/***************************************************************************//**
 * @brief Data structure of the heartbeat event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_heartbeat_s
{
  uint16_t src_addr; /**< Source address for the heartbeat message */
  uint16_t dst_addr; /**< Destination address for the heartbeat message */
  uint8_t  hops;     /**< Hops traveled by the heartbeat message */
});

typedef struct sl_btmesh_evt_node_heartbeat_s sl_btmesh_evt_node_heartbeat_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_heartbeat

/**
 * @addtogroup sl_btmesh_evt_node_heartbeat_start sl_btmesh_evt_node_heartbeat_start
 * @{
 * @brief Indicates start of heartbeat reception
 */

/** @brief Identifier of the heartbeat_start event */
#define sl_btmesh_evt_node_heartbeat_start_id                            0x111400a8

/***************************************************************************//**
 * @brief Data structure of the heartbeat_start event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_heartbeat_start_s
{
  uint16_t src_addr;   /**< Source address for the heartbeat message */
  uint16_t dst_addr;   /**< Destination address for the heartbeat message */
  uint32_t period_sec; /**< Heartbeat subscription period in seconds. */
});

typedef struct sl_btmesh_evt_node_heartbeat_start_s sl_btmesh_evt_node_heartbeat_start_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_heartbeat_start

/**
 * @addtogroup sl_btmesh_evt_node_heartbeat_stop sl_btmesh_evt_node_heartbeat_stop
 * @{
 * @brief Indicates end of heartbeat reception
 */

/** @brief Identifier of the heartbeat_stop event */
#define sl_btmesh_evt_node_heartbeat_stop_id                             0x121400a8

/***************************************************************************//**
 * @brief Data structure of the heartbeat_stop event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_heartbeat_stop_s
{
  uint16_t src_addr; /**< Source address for the heartbeat message */
  uint16_t dst_addr; /**< Destination address for the heartbeat message */
});

typedef struct sl_btmesh_evt_node_heartbeat_stop_s sl_btmesh_evt_node_heartbeat_stop_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_heartbeat_stop

/**
 * @addtogroup sl_btmesh_evt_node_beacon_received sl_btmesh_evt_node_beacon_received
 * @{
 * @brief Indicates reception of the secure network beacon
 */

/** @brief Identifier of the beacon_received event */
#define sl_btmesh_evt_node_beacon_received_id                            0x131400a8

/***************************************************************************//**
 * @brief Data structure of the beacon_received event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_beacon_received_s
{
  uint16_t netkey_index; /**< Index of the network key used to encrypt the
                              beacon */
  uint8_t  key_refresh;  /**< Indicates whether there is an ongoing key refresh. */
  uint8_t  iv_update;    /**< Indicates whether there is an ongoing IV update. */
  uint32_t iv_index;     /**< IV index contained in the beacon. */
});

typedef struct sl_btmesh_evt_node_beacon_received_s sl_btmesh_evt_node_beacon_received_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_beacon_received

/**
 * @addtogroup sl_btmesh_evt_node_local_dcd_data sl_btmesh_evt_node_local_dcd_data
 * @{
 * @brief Event reporting queried composition data page contents
 *
 * The contents are requested using the @ref sl_btmesh_node_get_local_dcd
 * command. More than one event may be generated. Page contents are terminated
 * by a @ref sl_btmesh_evt_node_local_dcd_data_end event. Note that the
 * interpretation of the received data is page-specific. Page 0 contains the
 * element and model layout of the node.
 */

/** @brief Identifier of the local_dcd_data event */
#define sl_btmesh_evt_node_local_dcd_data_id                             0x141400a8

/***************************************************************************//**
 * @brief Data structure of the local_dcd_data event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_local_dcd_data_s
{
  uint8_t    page; /**< Composition data page containing data */
  uint8array data; /**< Composition data page contents */
});

typedef struct sl_btmesh_evt_node_local_dcd_data_s sl_btmesh_evt_node_local_dcd_data_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_local_dcd_data

/**
 * @addtogroup sl_btmesh_evt_node_local_dcd_data_end sl_btmesh_evt_node_local_dcd_data_end
 * @{
 * @brief Terminating event for node composition data
 */

/** @brief Identifier of the local_dcd_data_end event */
#define sl_btmesh_evt_node_local_dcd_data_end_id                         0x151400a8

/***************************************************************************//**
 * @brief Data structure of the local_dcd_data_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_local_dcd_data_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
});

typedef struct sl_btmesh_evt_node_local_dcd_data_end_s sl_btmesh_evt_node_local_dcd_data_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_local_dcd_data_end

/**
 * @addtogroup sl_btmesh_evt_node_start_received sl_btmesh_evt_node_start_received
 * @{
 * @brief Provisioning Start PDU received
 *
 * This diagnostic event shows the algorithm, public key, and authentication
 * choices Provisioner made and communicated to the unprovisioned device.
 */

/** @brief Identifier of the start_received event */
#define sl_btmesh_evt_node_start_received_id                             0x161400a8

/***************************************************************************//**
 * @brief Data structure of the start_received event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_node_start_received_s
{
  uint8_t algorithm;             /**< Selected provisioning algorithm */
  uint8_t public_key;            /**< Selected OOB public key */
  uint8_t authentication_method; /**< Selected authentication method */
  uint8_t authentication_action; /**< Selected authentication action */
  uint8_t authentication_size;   /**< Selected authentication size */
});

typedef struct sl_btmesh_evt_node_start_received_s sl_btmesh_evt_node_start_received_t;

/** @} */ // end addtogroup sl_btmesh_evt_node_start_received

/***************************************************************************//**
 *
 * Initialize the Bluetooth mesh stack in Node role. When initialization is
 * complete, a @ref sl_btmesh_evt_node_initialized event will be generated.
 *
 * This command must be issued before any other Bluetooth Mesh commands, except
 * for @ref sl_btmesh_node_set_uuid command.
 *
 * Note that you may initialize a device either in the Provisioner or the Node
 * role, but not both.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_node_initialized
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_init();

/***************************************************************************//**
 *
 * Set the node in mode that allows exporting encryption keys. By default on
 * devices with PSA/ITS support the keys on normal node cannot be exported. This
 * command must called before either @ref sl_btmesh_node_init or @ref
 * sl_btmesh_node_init_oob. <b>On everey boot device boots to node role unless
 * this command is issued before node initialization</b>
 *
 * This command is to be used if device is originally in a node role and at
 * later stage switches to a provisioner role.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 * 
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_exportable_keys();

/***************************************************************************//**
 *
 * Start sending Unprovisioned Device Beacons.
 *
 * This command makes an unprovisioned device available for provisioning. The
 * device will start sending periodic unprovisioned device beacons containing
 * device UUID. It will also start listening for incoming Provisioner connection
 * attempts on the specified bearers (PB-ADV, PB-GATT, or both). For PB-GATT,
 * the device will also begin advertising its provisioning GATT service.
 *
 * At the beginning of a provisioning process, a @ref
 * sl_btmesh_evt_node_provisioning_started event will be generated. When the
 * device receives provisioning data from the Provisioner, a @ref
 * sl_btmesh_evt_node_provisioned event will be generated. If provisioning fails
 * with an error, a @ref sl_btmesh_evt_node_provisioning_failed event will be
 * generated.
 *
 * After it is provisioned, addresses are allocated for the node elements and a
 * network key is deployed to the node, making the node ready for further
 * configuration by the Provisioner. Note that, at this point, the node is not
 * yet fully ready to communicate with other nodes on the network.
 *
 * @param[in] bearer @parblock
 *   Bit mask for which bearer to use. Values are as follows:
 *     - <b>1 (bit 0):</b> PB-ADV
 *     - <b>2 (bit 1):</b> PB-GATT
 *
 *   Other bits are reserved and must not be used.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_node_provisioning_started
 *   - @ref sl_btmesh_evt_node_provisioned
 *   - @ref sl_btmesh_evt_node_provisioning_failed
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_start_unprov_beaconing(uint8_t bearer);

/***************************************************************************//**
 *
 * Stop sending Unprovisioned Device Beacons.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_stop_unprov_beaconing();

/***************************************************************************//**
 *
 * Get the latest RSSI value of a provisioned Bluetooth device.
 *
 * The value indicates the best signal strength received from any node within
 * the network. The value is cleared after calling this function meaning the
 * next call will fail if no new RSSI value is received.
 *
 * @param[out] rssi Latest RSSI value. Units: dBm. Ignore this parameter if the
 *   command fails.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_rssi(int8_t *rssi);

/***************************************************************************//**
 *
 * Provide the stack with the input out-of-band authentication data which the
 * Provisioner is displaying.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data Raw 16-byte array containing the authentication data.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_send_input_oob_request_response(size_t data_len,
                                                           const uint8_t* data);

/***************************************************************************//**
 *
 * Get the device UUID.
 *
 * Every mesh device has a 128-bit UUID identifying the device. It is used
 * primarily during provisioning, because it is broadcast in Unprovisioned
 * Device Beacons to indicate that the device is ready to be provisioned.
 *
 * This command is used for debugging purposes. During provisioning, the stack
 * automatically uses the UUID of the device, which does not need to be
 * explicitly specified when @ref sl_btmesh_node_start_unprov_beaconing is
 * started.
 *
 * If get uuid is used before @ref sl_btmesh_node_init, the uuid will be read
 * from the nvme if it was manually set by @ref sl_btmesh_node_set_uuid. If get
 * uuid is used without a prior @ref sl_btmesh_node_init or @ref
 * sl_btmesh_node_set_uuid, SL_STATUS_BT_MESH_DOES_NOT_EXIST will be returned.
 *
 * @param[out] uuid The 16-byte UUID of the device
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_uuid(uuid_128 *uuid);

/***************************************************************************//**
 *
 * Provision devices completely out-of-band. Populate the Provisioner's device
 * database with the corresponding values to make the device reachable and
 * configurable in the Provisioner's network.
 *
 * See also the Provisioner command for @ref sl_btmesh_prov_add_ddb_entry to
 * Provisioner's device database.
 *
 * <b>NOTE</b> : The device must be reset after this command has been issued.
 *
 * @param[in] device_key Device Key for this Device, shared by the Provisioner
 * @param[in] network_key Network key that the Provisioner has selected for this
 *   device
 * @param[in] netkey_index Index of the Network Key the Provisioner has selected
 *   for this device
 * @param[in] iv_index Current IV Index used in the network
 * @param[in] address Address the Provisioner has allocated for this device's
 *   primary element
 * @param[in] kr_in_progress Set to 1 if key refresh is currently in progress,
 *   otherwise 0.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_provisioning_data(aes_key_128 device_key,
                                                 aes_key_128 network_key,
                                                 uint16_t netkey_index,
                                                 uint32_t iv_index,
                                                 uint16_t address,
                                                 uint8_t kr_in_progress);

/***************************************************************************//**
 *
 * Initialize the Bluetooth mesh stack in the Node role. When initialization is
 * complete, a @ref sl_btmesh_evt_node_initialized event is generated.
 *
 * This command is the same as the @ref sl_btmesh_node_init command except for
 * parameters defining whether OOB authentication data stored on the device can
 * be used during provisioning.
 *
 * This command must be issued before any other Bluetooth mesh commands, except
 * for @ref sl_btmesh_node_set_uuid command.
 *
 * Note that you may initialize a device either in the Provisioner or the Node
 * role, but not both.
 *
 * @param[in] public_key If non-zero, use the ECC key stored in the persistent
 *   store during provisioning instead of an ephemeral key.
 * @param[in] auth_methods Enum @ref sl_btmesh_node_auth_method_flag_t. Allowed
 *   OOB authentication methods. The value is a bitmap so that multiple methods
 *   can be supported.
 * @param[in] output_actions Enum @ref sl_btmesh_node_oob_output_action_flag_t.
 *   Allowed OOB Output Action types
 * @param[in] output_size Maximum Output OOB size Valid values range from 0
 *   (feature not supported) to 8.
 * @param[in] input_actions Enum @ref sl_btmesh_node_oob_input_action_flag_t.
 *   Allowed OOB Input Action types
 * @param[in] input_size Maximum Input OOB size. Valid values range from 0
 *   (feature not supported) to 8.
 * @param[in] oob_location Defines the OOB data location bitmask. See @ref
 *   sl_btmesh_prov_oob_capabilities.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_node_initialized
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_init_oob(uint8_t public_key,
                                    uint8_t auth_methods,
                                    uint16_t output_actions,
                                    uint8_t output_size,
                                    uint16_t input_actions,
                                    uint8_t input_size,
                                    uint16_t oob_location);

/***************************************************************************//**
 *
 * Enable/disable the IV index recovery mode.
 *
 * If the node has not been in communication with the network for a long time
 * (e.g., because it was turned off), it may have missed IV index updates and
 * isn't able to communicate with other nodes. In this case, enable the IV index
 * recovery mode.
 *
 * @param[in] mode Zero to disable; non-zero to enable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_ivrecovery_mode(uint8_t mode);

/***************************************************************************//**
 *
 * Get the current IV index recovery mode state. See @ref
 * sl_btmesh_node_set_ivrecovery_mode for details.
 *
 * @param[out] mode If non-zero, IV recovery is enabled.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_ivrecovery_mode(uint8_t *mode);

/***************************************************************************//**
 *

 *
 * @param[in] max_statistics_size Size of output buffer passed in @p statistics
 * @param[out] statistics_len On return, set to the length of output data
 *   written to @p statistics
 * @param[out] statistics Raw statistics data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_statistics(size_t max_statistics_size,
                                          size_t *statistics_len,
                                          uint8_t *statistics);

/***************************************************************************//**
 *

 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_clear_statistics();

/***************************************************************************//**
 *
 * Set the network relay delay interval.
 *
 * This parameter determines the time a relay waits until it relays a network
 * PDU. The value used is a random number within the specified interval.
 *
 * Note that this value affects the first instance of the relayed network PDU.
 * If relay retransmissions are enabled, the interval between retransmissions is
 * defined by the relay state, set by the Provisioner of the network or by @ref
 * sl_btmesh_test_set_relay test command.
 *
 * @param[in] min_ms Minimum interval, in milliseconds
 * @param[in] max_ms Maximum interval, in milliseconds, which must be equal to
 *   or greater than the minimum.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_net_relay_delay(uint8_t min_ms, uint8_t max_ms);

/***************************************************************************//**
 *
 * Get network relay delay interval. See @ref sl_btmesh_node_set_net_relay_delay
 * command for details.
 *
 * @param[out] min_ms Minimum interval, in milliseconds
 * @param[out] max_ms Maximum interval, in milliseconds
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_net_relay_delay(uint8_t *min_ms,
                                               uint8_t *max_ms);

/***************************************************************************//**
 *
 * Get the current IV index update state in the network.
 *
 * @param[out] iv_index Current IV index
 * @param[out] state Indicates whether the IV index update is ongoing (1) or not
 *   (0).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_ivupdate_state(uint32_t *iv_index,
                                              uint8_t *state);

/***************************************************************************//**
 *
 * Attempt to request an IV index update in the network.
 *
 * Each network layer PDU that a node sends has a 24-bit sequence number
 * attached to it. Each node element keeps a sequence number counter, which is
 * incremented for every PDU sent out to the network. Repeating sequence numbers
 * for a given IV index value is not allowed. As a result, if a node determines
 * it is about to exhaust the available sequence numbers in one of its elements,
 * it needs to request an IV index update by issuing this command.
 *
 * Determining when a node may run out of sequence numbers has to be done at the
 * application level because the stack can't determine how often the application
 * plans to transmit to the network, i.e., how long the remaining sequence
 * numbers might last.
 *
 * See also the @ref sl_btmesh_node_get_seq_remaining command.
 *
 * Note that the call may fail for various reasons, for example if an IV index
 * update is already ongoing, or if not enough time has passed since the
 * previous IV index update.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_request_ivupdate();

/***************************************************************************//**
 *
 * Get the number of sequence numbers remaining on an element (before sequence
 * numbers are exhausted). Note that every node element keeps a separate
 * sequence number counter.
 *
 * @param[in] elem_index The index of queried element
 * @param[out] count Remaining sequence number count
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_seq_remaining(uint16_t elem_index,
                                             uint32_t *count);

/***************************************************************************//**
 *
 * Save the current replay protection list to the persistent store.
 *
 * The replay protection list keeps track of the packet sequence numbers from
 * different sources received by the node. The node will not process messages
 * associated with already used sequence numbers and is therefore protected from
 * replay attacks using previously recorded messages.
 *
 * The replay protection list is kept in RAM during runtime. It needs to be
 * saved to the persistent store periodically and always before the device
 * powers off. Because the stack is not aware when this will happen, the
 * application has to call this method while the node is getting ready to power
 * down but is still running.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_save_replay_protection_list();

/***************************************************************************//**
 *
 * Write device UUID into the persistent store. This command must be called
 * before initializing the Bluetooth mesh stack (before @ref sl_btmesh_node_init
 * or @ref sl_btmesh_node_init_oob), otherwise the change will not take effect
 * before a reboot.
 *
 * Ensure that the UUID conforms to the format defined in <a
 * href="https://tools.ietf.org/html/rfc4122">RFC 4122</a>
 *
 * Note that UUID must not be changed when the device is provisioned to a
 * network.
 *
 * Furthermore, ensure that the UUID remains constant if a device has received a
 * firmware update, which requires reprovisioning of the device after the update
 * has been applied (e.g., new elements are added by the update).
 *
 * @param[in] uuid UUID to set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_uuid(uuid_128 uuid);

/***************************************************************************//**
 *
 * Get the status of the replay protection list.
 *
 * @param[out] total_entries Total number of entries in use
 * @param[out] unsaved_entries Number of unsaved entries
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_replay_protection_list_status(uint16_t *total_entries,
                                                             uint16_t *unsaved_entries);

/***************************************************************************//**
 *
 * Get the unicast address configured to an element.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[out] address The address of the element. Returns 0x0000 if the address
 *   is not configured or if an error occurs.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_element_address(uint16_t elem_index,
                                               uint16_t *address);

/***************************************************************************//**
 *
 * Provide the stack with static out-of-band authentication data, which the
 * stack requested.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data Raw 16-byte array containing the authentication data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_send_static_oob_request_response(size_t data_len,
                                                            const uint8_t* data);

/***************************************************************************//**
 *
 * Factory reset of the mesh node.
 *
 * To complete procedure, the application should do its own cleanup duties and
 * reset the hardware.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_reset();

/***************************************************************************//**
 *
 * Set secure network beaconing on or off. When on, every received secure
 * network beacon will generate a @ref sl_btmesh_evt_node_beacon_received event.
 *
 * @param[in] report Turn reporting on (nonzero) or off (zero).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_beacon_reporting(uint8_t report);

/***************************************************************************//**
 *
 * Set the time since the last IV update. After the reboot, the node doesn't
 * know the time since the last IV update and assumes that it happened at the
 * time of the reboot.
 *
 * @param[in] age_sec Seconds since last IV update. Values from 0 to 345600
 *   (96h)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_iv_update_age(uint32_t age_sec);

/***************************************************************************//**
 *
 * Get the total number of keys in node.
 *
 * @param[in] type Enum @ref sl_btmesh_node_key_type_t. 0 for network key, 1 for
 *   application key
 * @param[out] count Number of keys of the given type on the device
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_key_count(uint8_t type, uint32_t *count);

/***************************************************************************//**
 *
 * Get a key by position. Only current key data exits in normal mode. Old key
 * data can be queried only during the key refresh.
 *
 * @param[in] type Enum @ref sl_btmesh_node_key_type_t. 0 for network key, 1 for
 *   application key
 * @param[in] index Key position, ranging from zero to key count minus one
 * @param[in] current 1: Current key, 0: Old key
 * @param[out] id Mesh key index of the key
 * @param[out] netkey_index For application keys, the network key index of the
 *   network key this key is bound to. Ignore for other key types.
 * @param[out] key Key data, 16 bytes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_key(uint8_t type,
                                   uint32_t index,
                                   uint8_t current,
                                   uint16_t *id,
                                   uint16_t *netkey_index,
                                   aes_key_128 *key);

/***************************************************************************//**
 *
 * Get a list of networks supported by the node.
 *
 * @param[in] max_networks_size Size of output buffer passed in @p networks
 * @param[out] networks_len On return, set to the length of output data written
 *   to @p networks
 * @param[out] networks List of network IDs. Each ID is two bytes in
 *   little-endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_networks(size_t max_networks_size,
                                        size_t *networks_len,
                                        uint8_t *networks);

/***************************************************************************//**
 *
 * Get the current sequence number of an element.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[out] seqnum Current sequence number of the element Ignore the value if
 *   the result code indicates an error (for example, when the element index is
 *   out of bounds).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_element_seqnum(uint16_t elem_index,
                                              uint32_t *seqnum);

/***************************************************************************//**
 *
 * Set a model-specific option.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] option Option to set. The following options are defined:
 *     - <b>0x80</b> Generic level delta behavior. Used only with generic level
 *       models.
 *     - <b>0xa0</b> Light models range status behavior. Used only with
 *       Lightness, Ctl and Hsl models. Note that state codes are different for
 *       states with status
 * @param[in] value @parblock
 *   Value for the option.
 *
 *   The following values are defined for generic level delta behavior option:
 *     - <b>0x0</b> Generic level delta behavior: pass raw delta request data to
 *       application
 *     - <b>0x1</b> Generic level delta behavior: pass processed delta request
 *       data to application (default)
 *
 *   The following values are defined for for Lightness, Ctl and Hsl range
 *   status message behavior option:
 *     - <b>0x0</b> Range Status behavior: pass status messages without status
 *       field(default)
 *     - <b>0x1</b> Range Status behavior: pass status messagages with status
 *       field
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t value);

/***************************************************************************//**
 *
 * Get the local Device Composition Data.
 *
 * @param[in] page Composition data page to query
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_node_local_dcd_data
 *   - @ref sl_btmesh_evt_node_local_dcd_data_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_local_dcd(uint8_t page);

/***************************************************************************//**
 *
 * Power off the mesh node. To be called prior power off to allow the device to
 * save its own state.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_power_off();

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Set PHY used for ADV traffic
 *
 * @param[in] phy   - 1: Primary and secondary channels are set to 1M
 *     - 2: Primary channels are 1M, secondary channels are 2M
 *     - 4: Primary and secondary channels are set to 125K Coded Phy (S=8)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_adv_phy(uint8_t phy);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Get PHY used for ADV traffic
 *
 * @param[out] phy   - 1: Primary and secondary channels are set to 1M
 *     - 2: Primary channels are 1M, secondary channels are 2M
 *     - 4: Primary and secondary channels are set to 125K Coded Phy (S=8)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_btmesh_node_get_adv_phy(uint8_t *phy);

/***************************************************************************//**
 *
 * Set Unprovisioned Device Beaconing advertisement interval timing parameters
 * both with and without URI.
 *
 * This command sets the timing parameters of Unprovisioned Device Beaconing
 * advertisement interval with and without URI. This setting will take effect
 * next time the Unprovisioned Device Beaconing advertisement is started.
 *
 * @param[in] adv_interval_min Minimum advertisement interval. Value is in units
 *   of 0.625 ms. Range: 0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 * @param[in] adv_interval_max Maximum advertisement interval. Value is in units
 *   of 0.625 ms. Must be equal to or greater than the minimum interval. Range:
 *   0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_unprov_beaconing_adv_interval(uint16_t adv_interval_min,
                                                             uint16_t adv_interval_max);

/***************************************************************************//**
 *
 * Set Mesh Proxy Service advertisement interval timing parameters.
 *
 * This command sets the timing parameters of Mesh Proxy Service advertisement
 * interval. This setting will take effect next time the Mesh Proxy Service
 * advertisement is started.
 *
 * @param[in] adv_interval_min Minimum advertisement interval. Value is in units
 *   of 0.625 ms. Range: 0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 * @param[in] adv_interval_max Maximum advertisement interval. Value is in units
 *   of 0.625 ms. Must be equal to or greater than the minimum interval. Range:
 *   0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_proxy_service_adv_interval(uint16_t adv_interval_min,
                                                          uint16_t adv_interval_max);

/***************************************************************************//**
 *
 * Set Mesh Provisioning Service advertisement interval timing parameters.
 *
 * This command sets the timing parameters of Mesh Provisioning Service
 * advertisement interval. This setting will take effect next time the Mesh
 * Provisioning Service advertisement is started.
 *
 * @param[in] adv_interval_min Minimum advertisement interval. Value is in units
 *   of 0.625 ms. Range: 0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 * @param[in] adv_interval_max Maximum advertisement interval. Value is in units
 *   of 0.625 ms. Must be equal to or greater than the minimum interval. Range:
 *   0x20 to 0xFFFF. Time range: 20 ms to 40.96 s
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_node_set_provisioning_service_adv_interval(uint16_t adv_interval_min,
                                                                 uint16_t adv_interval_max);

/** @} */ // end addtogroup sl_btmesh_node

/**
 * @addtogroup sl_btmesh_prov Bluetooth Mesh Stack Provisioner
 * @{
 *
 * @brief Bluetooth Mesh Stack Provisioner
 *
 * Bluetooth mesh stack API for the embedded Provisioner
 *
 * Commands in this class provision nodes in the mesh network and generate
 * security keys for the network.
 *
 * <b>Initialization:</b>
 *   - @ref sl_btmesh_prov_init
 *   - @ref sl_btmesh_evt_prov_initialized
 *   - @ref sl_btmesh_evt_prov_initialization_failed
 *
 * <b>Provisioning a node:</b>
 *   - @ref sl_btmesh_prov_scan_unprov_beacons : Scan for unprovisioned device
 *     beacons
 *   - @ref sl_btmesh_prov_stop_scan_unprov_beacons : Stop scanning for
 *     unprovisioned device beacons
 *   - @ref sl_btmesh_evt_prov_unprov_beacon : Unprovisioned device beacon seen
 *   - @ref sl_btmesh_evt_prov_uri : URI advertisement seen
 *   - @ref sl_btmesh_prov_create_provisioning_session : Create provisioning
 *     session
 *   - @ref sl_btmesh_prov_provision_adv_device : Provision a device over PB-ADV
 *   - @ref sl_btmesh_prov_provision_gatt_device : Provision a device over
 *     PB-GATT
 *   - @ref sl_btmesh_evt_prov_oob_display_input : Request to display input
 *     out-of-band data to the user to input on the node
 *   - @ref sl_btmesh_evt_prov_oob_pkey_request : Request for out-of-band public
 *     key of a node
 *   - @ref sl_btmesh_prov_send_oob_pkey_response : Provide stack with
 *     out-of-band public key of a node
 *   - @ref sl_btmesh_evt_prov_oob_auth_request : Request for out-of-band
 *     authentication data of a node
 *   - @ref sl_btmesh_prov_send_oob_auth_response : Provide stack with
 *     out-of-band authentication data of a node
 *   - @ref sl_btmesh_evt_prov_device_provisioned : Device Provisioned
 *   - @ref sl_btmesh_evt_prov_provisioning_failed : Provisioning a device
 *     failed
 *
 * <b>Key Management</b>
 *   - @ref sl_btmesh_prov_create_network : Create a new network key on the
 *     Provisioner
 *   - @ref sl_btmesh_prov_create_appkey : Create a new application key on the
 *     Provisioner
 *   - @ref sl_btmesh_prov_start_key_refresh : Start a key refresh procedure
 *   - @ref sl_btmesh_prov_suspend_key_refresh : Suspend an ongoing key refresh
 *     procedure
 *   - @ref sl_btmesh_prov_resume_key_refresh : Resume a suspended key refresh
 *     procedure
 *   - @ref sl_btmesh_prov_get_key_refresh_exclusion : Get node key refresh
 *     exclusion status
 *   - @ref sl_btmesh_prov_set_key_refresh_exclusion : Set node key refresh
 *     exclusion status
 *   - @ref sl_btmesh_prov_get_key_refresh_phase : Get node key refresh phase
 *   - @ref sl_btmesh_prov_start_key_refresh_from_phase : Start a key refresh
 *     procedure from the given phase
 *
 * <b>Device Database</b>
 *   - @ref sl_btmesh_prov_add_ddb_entry : Add a node to the device database
 *   - @ref sl_btmesh_prov_delete_ddb_entry : Remove a node from the device
 *     database
 *   - @ref sl_btmesh_prov_get_ddb_entry : Fetch node data from the device
 *     database
 *   - @ref sl_btmesh_prov_list_ddb_entries : Request a list of nodes in the
 *     device database
 *   - @ref sl_btmesh_evt_prov_ddb_list : Device database list result
 *   - @ref sl_btmesh_prov_update_device_netkey_index : Update default network
 *     key index for a device database entry
 *
 * These commands are available only if the Provisioner functionality is
 * compiled in the device. Otherwise, a "feature not implemented" error code
 * will be returned for all functions in this class.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_prov_init_id                                       0x00150028
#define sl_btmesh_cmd_prov_scan_unprov_beacons_id                        0x01150028
#define sl_btmesh_cmd_prov_create_provisioning_session_id                0x41150028
#define sl_btmesh_cmd_prov_set_provisioning_suspend_event_id             0x42150028
#define sl_btmesh_cmd_prov_provision_adv_device_id                       0x43150028
#define sl_btmesh_cmd_prov_provision_gatt_device_id                      0x44150028
#define sl_btmesh_cmd_prov_continue_provisioning_id                      0x45150028
#define sl_btmesh_cmd_prov_abort_provisioning_id                         0x47150028
#define sl_btmesh_cmd_prov_set_device_address_id                         0x46150028
#define sl_btmesh_cmd_prov_create_network_id                             0x03150028
#define sl_btmesh_cmd_prov_create_appkey_id                              0x07150028
#define sl_btmesh_cmd_prov_send_oob_pkey_response_id                     0x08150028
#define sl_btmesh_cmd_prov_send_oob_auth_response_id                     0x09150028
#define sl_btmesh_cmd_prov_set_oob_requirements_id                       0x0a150028
#define sl_btmesh_cmd_prov_start_key_refresh_id                          0x0b150028
#define sl_btmesh_cmd_prov_get_key_refresh_exclusion_id                  0x0c150028
#define sl_btmesh_cmd_prov_set_key_refresh_exclusion_id                  0x0d150028
#define sl_btmesh_cmd_prov_get_ddb_entry_id                              0x16150028
#define sl_btmesh_cmd_prov_delete_ddb_entry_id                           0x17150028
#define sl_btmesh_cmd_prov_add_ddb_entry_id                              0x18150028
#define sl_btmesh_cmd_prov_list_ddb_entries_id                           0x19150028
#define sl_btmesh_cmd_prov_initialize_network_id                         0x37150028
#define sl_btmesh_cmd_prov_get_key_refresh_appkey_exclusion_id           0x38150028
#define sl_btmesh_cmd_prov_set_key_refresh_appkey_exclusion_id           0x39150028
#define sl_btmesh_cmd_prov_stop_scan_unprov_beacons_id                   0x3a150028
#define sl_btmesh_cmd_prov_update_device_netkey_index_id                 0x3b150028
#define sl_btmesh_cmd_prov_suspend_key_refresh_id                        0x3c150028
#define sl_btmesh_cmd_prov_resume_key_refresh_id                         0x3d150028
#define sl_btmesh_cmd_prov_get_key_refresh_phase_id                      0x3e150028
#define sl_btmesh_cmd_prov_start_key_refresh_from_phase_id               0x3f150028
#define sl_btmesh_cmd_prov_flush_key_refresh_state_id                    0x40150028
#define sl_btmesh_cmd_prov_test_identity_id                              0x48150028
#define sl_btmesh_rsp_prov_init_id                                       0x00150028
#define sl_btmesh_rsp_prov_scan_unprov_beacons_id                        0x01150028
#define sl_btmesh_rsp_prov_create_provisioning_session_id                0x41150028
#define sl_btmesh_rsp_prov_set_provisioning_suspend_event_id             0x42150028
#define sl_btmesh_rsp_prov_provision_adv_device_id                       0x43150028
#define sl_btmesh_rsp_prov_provision_gatt_device_id                      0x44150028
#define sl_btmesh_rsp_prov_continue_provisioning_id                      0x45150028
#define sl_btmesh_rsp_prov_abort_provisioning_id                         0x47150028
#define sl_btmesh_rsp_prov_set_device_address_id                         0x46150028
#define sl_btmesh_rsp_prov_create_network_id                             0x03150028
#define sl_btmesh_rsp_prov_create_appkey_id                              0x07150028
#define sl_btmesh_rsp_prov_send_oob_pkey_response_id                     0x08150028
#define sl_btmesh_rsp_prov_send_oob_auth_response_id                     0x09150028
#define sl_btmesh_rsp_prov_set_oob_requirements_id                       0x0a150028
#define sl_btmesh_rsp_prov_start_key_refresh_id                          0x0b150028
#define sl_btmesh_rsp_prov_get_key_refresh_exclusion_id                  0x0c150028
#define sl_btmesh_rsp_prov_set_key_refresh_exclusion_id                  0x0d150028
#define sl_btmesh_rsp_prov_get_ddb_entry_id                              0x16150028
#define sl_btmesh_rsp_prov_delete_ddb_entry_id                           0x17150028
#define sl_btmesh_rsp_prov_add_ddb_entry_id                              0x18150028
#define sl_btmesh_rsp_prov_list_ddb_entries_id                           0x19150028
#define sl_btmesh_rsp_prov_initialize_network_id                         0x37150028
#define sl_btmesh_rsp_prov_get_key_refresh_appkey_exclusion_id           0x38150028
#define sl_btmesh_rsp_prov_set_key_refresh_appkey_exclusion_id           0x39150028
#define sl_btmesh_rsp_prov_stop_scan_unprov_beacons_id                   0x3a150028
#define sl_btmesh_rsp_prov_update_device_netkey_index_id                 0x3b150028
#define sl_btmesh_rsp_prov_suspend_key_refresh_id                        0x3c150028
#define sl_btmesh_rsp_prov_resume_key_refresh_id                         0x3d150028
#define sl_btmesh_rsp_prov_get_key_refresh_phase_id                      0x3e150028
#define sl_btmesh_rsp_prov_start_key_refresh_from_phase_id               0x3f150028
#define sl_btmesh_rsp_prov_flush_key_refresh_state_id                    0x40150028
#define sl_btmesh_rsp_prov_test_identity_id                              0x48150028

/**
 * @addtogroup sl_btmesh_prov_oob_capabilities OOB Capabilities
 * @{
 *
 * OOB capability bitmask constants
 */

/** Uncategorized */
#define SL_BTMESH_PROV_OOB_OTHER      0x1       

/** URI or other electronic */
#define SL_BTMESH_PROV_OOB_URI        0x2       

/** 2D machine-readable code */
#define SL_BTMESH_PROV_OOB_2D_MR_CODE 0x4       

/** Barcode */
#define SL_BTMESH_PROV_OOB_BAR_CODE   0x8       

/** NFC */
#define SL_BTMESH_PROV_OOB_NFC        0x10      

/** Number */
#define SL_BTMESH_PROV_OOB_NUMBER     0x20      

/** String */
#define SL_BTMESH_PROV_OOB_STRING     0x40      

/** Reserved */
#define SL_BTMESH_PROV_OOB_RFU_7      0x80      

/** Reserved */
#define SL_BTMESH_PROV_OOB_RFU_8      0x100     

/** Reserved */
#define SL_BTMESH_PROV_OOB_RFU_9      0x200     

/** Reserved */
#define SL_BTMESH_PROV_OOB_RFU_A      0x400     

/** On the box */
#define SL_BTMESH_PROV_OOB_LOC_ON_BOX 0x800     

/** Inside the box */
#define SL_BTMESH_PROV_OOB_LOC_IN_BOX 0x1000    

/** On a piece of paper */
#define SL_BTMESH_PROV_OOB_LOC_PAPER  0x2000    

/** In the device manual */
#define SL_BTMESH_PROV_OOB_LOC_MANUAL 0x4000    

/** On the device */
#define SL_BTMESH_PROV_OOB_LOC_DEVICE 0x8000    

/** Mask of reserved bits */
#define SL_BTMESH_PROV_OOB_RFU_MASK   0x780     

/** @} */ // end OOB Capabilities

/**
 * @addtogroup sl_btmesh_evt_prov_initialized sl_btmesh_evt_prov_initialized
 * @{
 * @brief Provisioner is initialized and operational
 */

/** @brief Identifier of the initialized event */
#define sl_btmesh_evt_prov_initialized_id                                0x001500a8

/***************************************************************************//**
 * @brief Data structure of the initialized event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_initialized_s
{
  uint8_t  networks; /**< Number of network keys that the Provisioner has */
  uint16_t address;  /**< Unicast address of the primary element of the
                          Provisioner */
  uint32_t iv_index; /**< IVI for network primary network (index 0) */
});

typedef struct sl_btmesh_evt_prov_initialized_s sl_btmesh_evt_prov_initialized_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_initialized

/**
 * @addtogroup sl_btmesh_evt_prov_provisioning_suspended sl_btmesh_evt_prov_provisioning_suspended
 * @{
 * @brief Provisioning suspended.
 */

/** @brief Identifier of the provisioning_suspended event */
#define sl_btmesh_evt_prov_provisioning_suspended_id                     0x181500a8

/***************************************************************************//**
 * @brief Data structure of the provisioning_suspended event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_provisioning_suspended_s
{
  uuid_128 uuid;   /**< UUID of the device */
  uint8_t  reason; /**< Reason for suspension. Values are as follows:
                          - <b>0:</b> Capabilities Event received
                          - <b>1:</b> Provisioning link opened */
});

typedef struct sl_btmesh_evt_prov_provisioning_suspended_s sl_btmesh_evt_prov_provisioning_suspended_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_provisioning_suspended

/**
 * @addtogroup sl_btmesh_evt_prov_capabilities sl_btmesh_evt_prov_capabilities
 * @{
 * @brief Provisioning capabilities message
 */

/** @brief Identifier of the capabilities event */
#define sl_btmesh_evt_prov_capabilities_id                               0x171500a8

/***************************************************************************//**
 * @brief Data structure of the capabilities event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_capabilities_s
{
  uuid_128 uuid;              /**< UUID of the device being provisioned */
  uint8_t  elements;          /**< Number of elements supported by the device */
  uint16_t algorithms;        /**< Supported algorithms and other capabilities. */
  uint8_t  pkey_type;         /**< Supported public key types */
  uint8_t  static_oob_type;   /**< Supported static OOB Types */
  uint8_t  ouput_oob_size;    /**< Maximum size of Output OOB supported */
  uint16_t output_oob_action; /**< Supported Output OOB Actions */
  uint8_t  input_oob_size;    /**< Maximum size of input OOB supported */
  uint16_t intput_oob_action; /**< Supported input OOB Actions */
});

typedef struct sl_btmesh_evt_prov_capabilities_s sl_btmesh_evt_prov_capabilities_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_capabilities

/**
 * @addtogroup sl_btmesh_evt_prov_provisioning_failed sl_btmesh_evt_prov_provisioning_failed
 * @{
 * @brief Provisioning a device failed.
 */

/** @brief Identifier of the provisioning_failed event */
#define sl_btmesh_evt_prov_provisioning_failed_id                        0x011500a8

/***************************************************************************//**
 * @brief Data structure of the provisioning_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_provisioning_failed_s
{
  uint8_t  reason; /**< Reason for failure. Values are as follows:
                          - <b>0:</b> Link closed
                          - <b>1:</b> Invalid PDU
                          - <b>2:</b> Invalid PDU format
                          - <b>3:</b> Unexpected PDU
                          - <b>4:</b> Confirmation failed
                          - <b>5:</b> Out of resources
                          - <b>6:</b> Decryption failed
                          - <b>7:</b> Unexpected error
                          - <b>8:</b> Unable to assign address */
  uuid_128 uuid;   /**< UUID of the device */
});

typedef struct sl_btmesh_evt_prov_provisioning_failed_s sl_btmesh_evt_prov_provisioning_failed_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_provisioning_failed

/**
 * @addtogroup sl_btmesh_evt_prov_device_provisioned sl_btmesh_evt_prov_device_provisioned
 * @{
 * @brief Device provisioned successfully.
 */

/** @brief Identifier of the device_provisioned event */
#define sl_btmesh_evt_prov_device_provisioned_id                         0x021500a8

/***************************************************************************//**
 * @brief Data structure of the device_provisioned event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_device_provisioned_s
{
  uint16_t address; /**< Address assigned to the node's primary element. If the
                         node has multiple elements, they have been assigned an
                         address in a consecutive sequence following the primary
                         element address. */
  uuid_128 uuid;    /**< UUID of the device */
});

typedef struct sl_btmesh_evt_prov_device_provisioned_s sl_btmesh_evt_prov_device_provisioned_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_device_provisioned

/**
 * @addtogroup sl_btmesh_evt_prov_unprov_beacon sl_btmesh_evt_prov_unprov_beacon
 * @{
 * @brief Unprovisioned beacon seen.
 */

/** @brief Identifier of the unprov_beacon event */
#define sl_btmesh_evt_prov_unprov_beacon_id                              0x031500a8

/***************************************************************************//**
 * @brief Data structure of the unprov_beacon event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_unprov_beacon_s
{
  uint16_t oob_capabilities; /**< OOB capabilities bitfield, which indicates the
                                  means by which out-of-band provisioning data
                                  may be retrieved. See @ref
                                  sl_btmesh_prov_oob_capabilities. */
  uint32_t uri_hash;         /**< Hash of the out-of-band URI, which is received
                                  in @ref sl_btmesh_evt_prov_uri. If the URI bit
                                  (bit 1) is not set in the OOB capabilities
                                  bitfield, this field is ignored. */
  uint8_t  bearer;           /**< Bearer on which the beacon was seen. Values
                                  are as follows:
                                    - <b>0:</b> PB-ADV
                                    - <b>1:</b> PB-GATT */
  bd_addr  address;          /**< Address of the device beaconing */
  uint8_t  address_type;     /**< Beaconing device address type. Values:
                                    - <b>0:</b> Public address
                                    - <b>1:</b> Random address */
  uuid_128 uuid;             /**< 16-byte UUID of the beaconing device. */
  int8_t   rssi;             /**< RSSI value of the beacon seen. Units: dBm. */
});

typedef struct sl_btmesh_evt_prov_unprov_beacon_s sl_btmesh_evt_prov_unprov_beacon_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_unprov_beacon

/**
 * @addtogroup sl_btmesh_evt_prov_oob_pkey_request sl_btmesh_evt_prov_oob_pkey_request
 * @{
 * @brief The Provisioner needs the OOB public key of the Device with given UUID
 *
 * Input the key using prov_oob_pkey_rsp.
 */

/** @brief Identifier of the oob_pkey_request event */
#define sl_btmesh_evt_prov_oob_pkey_request_id                           0x061500a8

/***************************************************************************//**
 * @brief Data structure of the oob_pkey_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_oob_pkey_request_s
{
  uuid_128 uuid; /**< UUID of the Device */
});

typedef struct sl_btmesh_evt_prov_oob_pkey_request_s sl_btmesh_evt_prov_oob_pkey_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_oob_pkey_request

/**
 * @addtogroup sl_btmesh_evt_prov_oob_auth_request sl_btmesh_evt_prov_oob_auth_request
 * @{
 * @brief Provide the Provisioner with the device's output or static data using
 * prov_oob_auth_rsp.
 */

/** @brief Identifier of the oob_auth_request event */
#define sl_btmesh_evt_prov_oob_auth_request_id                           0x071500a8

/***************************************************************************//**
 * @brief Data structure of the oob_auth_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_oob_auth_request_s
{
  uint8_t  output;        /**< Zero for static data, non-zero for output */
  uint8_t  output_action; /**< Enum @ref sl_btmesh_node_oob_output_action_t.
                               Output action type. Ignored for Static. */
  uint8_t  output_size;   /**< Size of output data. Ignored for Static. */
  uuid_128 uuid;          /**< UUID of the device */
});

typedef struct sl_btmesh_evt_prov_oob_auth_request_s sl_btmesh_evt_prov_oob_auth_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_oob_auth_request

/**
 * @addtogroup sl_btmesh_evt_prov_oob_display_input sl_btmesh_evt_prov_oob_display_input
 * @{
 * @brief Random OOB input data was generated and should be displayed to and
 * input with the device.
 */

/** @brief Identifier of the oob_display_input event */
#define sl_btmesh_evt_prov_oob_display_input_id                          0x081500a8

/***************************************************************************//**
 * @brief Data structure of the oob_display_input event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_oob_display_input_s
{
  uuid_128   uuid;         /**< UUID of the device */
  uint8_t    input_action; /**< Enum @ref sl_btmesh_node_oob_input_action_t.
                                Input action type */
  uint8_t    input_size;   /**< Number of digits or characters */
  uint8array data;         /**< Raw 16-byte array */
});

typedef struct sl_btmesh_evt_prov_oob_display_input_s sl_btmesh_evt_prov_oob_display_input_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_oob_display_input

/**
 * @addtogroup sl_btmesh_evt_prov_ddb_list sl_btmesh_evt_prov_ddb_list
 * @{
 * @brief Provisioner's device database list entry
 */

/** @brief Identifier of the ddb_list event */
#define sl_btmesh_evt_prov_ddb_list_id                                   0x091500a8

/***************************************************************************//**
 * @brief Data structure of the ddb_list event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_ddb_list_s
{
  uuid_128 uuid;     /**< UUID of the Device */
  uint16_t address;  /**< Unicast address of the primary element of the node */
  uint8_t  elements; /**< Number of elements the device has */
});

typedef struct sl_btmesh_evt_prov_ddb_list_s sl_btmesh_evt_prov_ddb_list_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_ddb_list

/**
 * @addtogroup sl_btmesh_evt_prov_uri sl_btmesh_evt_prov_uri
 * @{
 * @brief URI advertisement received from a nearby device
 */

/** @brief Identifier of the uri event */
#define sl_btmesh_evt_prov_uri_id                                        0x0d1500a8

/***************************************************************************//**
 * @brief Data structure of the uri event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_uri_s
{
  uint32_t   hash; /**< URI hash. If a Provisioner is provisioning a device
                        which supports out-of-band provisioning and has supplied
                        a URI hash value in its Unprovisioned Device beacon, the
                        Provisioner should check whether the hash matches this
                        value. */
  uint8array data; /**< Raw URI data, formatted as specified in Bluetooth Core
                        System Supplement v6. */
});

typedef struct sl_btmesh_evt_prov_uri_s sl_btmesh_evt_prov_uri_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_uri

/**
 * @addtogroup sl_btmesh_evt_prov_key_refresh_phase_update sl_btmesh_evt_prov_key_refresh_phase_update
 * @{
 * @brief Key refresh phase change for a network key has occurred
 *
 * This event is generated when all nodes participating in a key refresh
 * procedure have been moved to a new state (or have timed out, dropping them
 * from the key refresh procedure).
 */

/** @brief Identifier of the key_refresh_phase_update event */
#define sl_btmesh_evt_prov_key_refresh_phase_update_id                   0x141500a8

/***************************************************************************//**
 * @brief Data structure of the key_refresh_phase_update event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_key_refresh_phase_update_s
{
  uint16_t key;   /**< Network key index */
  uint8_t  phase; /**< Phase moved into */
});

typedef struct sl_btmesh_evt_prov_key_refresh_phase_update_s sl_btmesh_evt_prov_key_refresh_phase_update_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_key_refresh_phase_update

/**
 * @addtogroup sl_btmesh_evt_prov_key_refresh_node_update sl_btmesh_evt_prov_key_refresh_node_update
 * @{
 * @brief Key refresh phase change for a node has occurred
 *
 * This event is generated when a particular node has moved to a new key refresh
 * phase.
 */

/** @brief Identifier of the key_refresh_node_update event */
#define sl_btmesh_evt_prov_key_refresh_node_update_id                    0x151500a8

/***************************************************************************//**
 * @brief Data structure of the key_refresh_node_update event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_key_refresh_node_update_s
{
  uint16_t key;   /**< Network key index */
  uint8_t  phase; /**< Phase moved into */
  uuid_128 uuid;  /**< 16-byte UUID of the node. */
});

typedef struct sl_btmesh_evt_prov_key_refresh_node_update_s sl_btmesh_evt_prov_key_refresh_node_update_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_key_refresh_node_update

/**
 * @addtogroup sl_btmesh_evt_prov_key_refresh_complete sl_btmesh_evt_prov_key_refresh_complete
 * @{
 * @brief Key refresh for a network key has completed
 */

/** @brief Identifier of the key_refresh_complete event */
#define sl_btmesh_evt_prov_key_refresh_complete_id                       0x161500a8

/***************************************************************************//**
 * @brief Data structure of the key_refresh_complete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_key_refresh_complete_s
{
  uint16_t key;    /**< Network key index */
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
});

typedef struct sl_btmesh_evt_prov_key_refresh_complete_s sl_btmesh_evt_prov_key_refresh_complete_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_key_refresh_complete

/**
 * @addtogroup sl_btmesh_evt_prov_add_ddb_entry_complete sl_btmesh_evt_prov_add_ddb_entry_complete
 * @{
 * @brief Adding a DDB entry has been completed
 *
 * See the result code for operation status.
 */

/** @brief Identifier of the add_ddb_entry_complete event */
#define sl_btmesh_evt_prov_add_ddb_entry_complete_id                     0x191500a8

/***************************************************************************//**
 * @brief Data structure of the add_ddb_entry_complete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_add_ddb_entry_complete_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uuid_128 uuid;   /**< UUID of the Device */
});

typedef struct sl_btmesh_evt_prov_add_ddb_entry_complete_s sl_btmesh_evt_prov_add_ddb_entry_complete_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_add_ddb_entry_complete

/**
 * @addtogroup sl_btmesh_evt_prov_delete_ddb_entry_complete sl_btmesh_evt_prov_delete_ddb_entry_complete
 * @{
 * @brief Deleting a DDB entry has been completed
 *
 * See the result code for operation status.
 */

/** @brief Identifier of the delete_ddb_entry_complete event */
#define sl_btmesh_evt_prov_delete_ddb_entry_complete_id                  0x1a1500a8

/***************************************************************************//**
 * @brief Data structure of the delete_ddb_entry_complete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_delete_ddb_entry_complete_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uuid_128 uuid;   /**< UUID of the Device */
});

typedef struct sl_btmesh_evt_prov_delete_ddb_entry_complete_s sl_btmesh_evt_prov_delete_ddb_entry_complete_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_delete_ddb_entry_complete

/**
 * @addtogroup sl_btmesh_evt_prov_initialization_failed sl_btmesh_evt_prov_initialization_failed
 * @{
 * @brief Provisioner has not been initialized successfully and is not
 * operational
 *
 * It is not possible to use the device as a Provisioner. See the result code
 * for details.
 */

/** @brief Identifier of the initialization_failed event */
#define sl_btmesh_evt_prov_initialization_failed_id                      0x1b1500a8

/***************************************************************************//**
 * @brief Data structure of the initialization_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_initialization_failed_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
});

typedef struct sl_btmesh_evt_prov_initialization_failed_s sl_btmesh_evt_prov_initialization_failed_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_initialization_failed

/**
 * @addtogroup sl_btmesh_evt_prov_start_sent sl_btmesh_evt_prov_start_sent
 * @{
 * @brief Provisioning Start PDU sent
 *
 * This diagnostic event shows the algorithm, public key, and authentication
 * choices Provisioner made and communicated to the unprovisioned device.
 */

/** @brief Identifier of the start_sent event */
#define sl_btmesh_evt_prov_start_sent_id                                 0x1e1500a8

/***************************************************************************//**
 * @brief Data structure of the start_sent event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_prov_start_sent_s
{
  uuid_128 uuid;                  /**< UUID of the device */
  uint8_t  algorithm;             /**< Selected provisioning algorithm */
  uint8_t  public_key;            /**< Selected OOB public key */
  uint8_t  authentication_method; /**< Selected authentication method */
  uint8_t  authentication_action; /**< Selected authentication action */
  uint8_t  authentication_size;   /**< Selected authentication size */
});

typedef struct sl_btmesh_evt_prov_start_sent_s sl_btmesh_evt_prov_start_sent_t;

/** @} */ // end addtogroup sl_btmesh_evt_prov_start_sent

/***************************************************************************//**
 *
 * Initialize the Bluetooth mesh stack in the Provisioner role. Note that the
 * result code of this command only indicates that the request to initialize has
 * been accepted for processing. When initialization is completed successfully,
 * a @ref sl_btmesh_evt_prov_initialized will be generated. On failed
 * initialization a @ref sl_btmesh_evt_prov_initialization_failed event will be
 * generated. Note that the application must wait for an event to be generated
 * before executing further BGAPI commands.
 *
 * This command must be issued before any other Bluetooth mesh stack commands.
 * Note that the Bluetooth mesh stack can be initialized either in the
 * Provisioner or the Node role, but not both.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_initialized
 *   - @ref sl_btmesh_evt_prov_initialization_failed
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_init();

/***************************************************************************//**
 *
 * Start scanning for unprovisioned device beacons.
 *
 * Unprovisioned devices send out beacons containing their UUID. An @ref
 * sl_btmesh_evt_prov_unprov_beacon will be generated for each beacon seen. Once
 * the UUID of a device is known, the Provisioner may start provisioning the
 * device with the @ref sl_btmesh_prov_create_provisioning_session command.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_unprov_beacon
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_scan_unprov_beacons();

/***************************************************************************//**
 *
 * Initialize the provisioning session. After this command, it is possible to
 * set the provisioning parameter with the following command:
 *
 *   - @ref sl_btmesh_prov_set_provisioning_suspend_event
 *
 * After all optional parameters have been set, the provisioning process is
 * triggered by one of the following commands:
 *
 *   - @ref sl_btmesh_prov_provision_adv_device
 *   - @ref sl_btmesh_prov_provision_gatt_device
 *
 * @param[in] netkey_index Index of the initial network key, which is sent to
 *   the device during provisioning
 * @param[in] uuid UUID of the device
 * @param[in] attention_timer_sec Attention timer value, in seconds, which
 *   indicates the time that the provisioned device should attract human
 *   attention
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 * 
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_create_provisioning_session(uint16_t netkey_index,
                                                       uuid_128 uuid,
                                                       uint8_t attention_timer_sec);

/***************************************************************************//**
 *
 * Set whether the provisioning can be suspended at a specified point while it
 * is occurring. Currently, this can happen after reception on provisioning
 * capabilities message or when the provisioning link is opened.
 *
 * @param[in] status @parblock
 *   Controls when provisioning is suspended and @ref
 *   sl_btmesh_evt_prov_provisioning_suspended event is created. The value is a
 *   bitmap so multiple possibilities can be set.
 *
 *     - <b>Bit 0 set</b> provisioning will be suspended when Capabilities PDU
 *       is received from the unprovisioned device
 *     - <b>Bit 1 set</b> provisioning will be suspended when the provisioning
 *       session is opened
 *
 *   If no bits are set (value is zero) provisioning will not be suspended. This
 *   is the default.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_provisioning_suspended
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_set_provisioning_suspend_event(uint8_t status);

/***************************************************************************//**
 *
 * Provision device over PB-ADV
 *
 * @param[in] uuid UUID of the device
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_device_provisioned
 *   - @ref sl_btmesh_evt_prov_provisioning_failed
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_provision_adv_device(uuid_128 uuid);

/***************************************************************************//**
 *
 * Provision device over PB-GATT
 *
 * @param[in] uuid UUID of the device
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_device_provisioned
 *   - @ref sl_btmesh_evt_prov_provisioning_failed
 *   - @ref sl_btmesh_evt_prov_capabilities
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_provision_gatt_device(uuid_128 uuid,
                                                 uint8_t connection);

/***************************************************************************//**
 *
 * Continue provisioning after the suspension indicated by @ref
 * sl_btmesh_evt_prov_provisioning_suspended.
 *
 * @param[in] uuid UUID of the device being provisioned
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 * 
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_continue_provisioning(uuid_128 uuid);

/***************************************************************************//**
 *
 * Abort provisioning. This command is typically used after the suspension
 * indicated by @ref sl_btmesh_evt_prov_provisioning_suspended.
 *
 * @param[in] uuid UUID of the device being provisioned
 * @param[in] reason Reason for aborting. Values are as follows:
 *     - <b>1:</b> Invalid PDU
 *     - <b>2:</b> Invalid PDU format
 *     - <b>3:</b> Unexpected PDU
 *     - <b>4:</b> Confirmation failed
 *     - <b>5:</b> Out of resources
 *     - <b>6:</b> Decryption failed
 *     - <b>7:</b> Unexpected error
 *     - <b>8:</b> Unable to assign address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 * 
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_abort_provisioning(uuid_128 uuid, uint8_t reason);

/***************************************************************************//**
 *
 * Set the address for the device after the provisioning has been suspended, as
 * indicated by @ref sl_btmesh_evt_prov_provisioning_suspended event.
 *
 * @param[in] uuid UUID of the device being provisioned
 * @param[in] address Unicast address to be assigned for the primary element of
 *   the node.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 * 
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_set_device_address(uuid_128 uuid, uint16_t address);

/***************************************************************************//**
 *
 * Create a new network key on the Provisioner.
 *
 * The created key can be deployed on a node using the @ref
 * sl_btmesh_config_client_add_netkey command.
 *
 * @param[in] netkey_index Index to use for network key. Allowed values are from
 *   0x000 to 0xfff.
 * @param[in] key_len Length of data in @p key
 * @param[in] key Key value to use. Set to zero-length array to generate a
 *   random key.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_create_network(uint16_t netkey_index,
                                          size_t key_len,
                                          const uint8_t* key);

/***************************************************************************//**
 *
 * Create a new application key on the Provisioner.
 *
 * An application key is always bound to a network key. In other words, the
 * application key is only valid in the context of a particular network key. The
 * selected network key must exist on the Provisioner (see @ref
 * sl_btmesh_prov_create_network command).
 *
 * The created application key can be deployed on a node using the @ref
 * sl_btmesh_config_client_add_appkey command.
 *
 * @param[in] netkey_index Index of the network key to which the application key
 *   will be bound
 * @param[in] appkey_index Index to use for application key. Allowed values are
 *   from 0x000 to 0xfff.
 * @param[in] key_len Length of data in @p key
 * @param[in] key Key value to use; set to zero-length array to generate random
 *   key.
 * @param[in] max_application_key_size Size of output buffer passed in @p
 *   application_key
 * @param[out] application_key_len On return, set to the length of output data
 *   written to @p application_key
 * @param[out] application_key New application key. Ignore it if the result was
 *   non-zero.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_create_appkey(uint16_t netkey_index,
                                         uint16_t appkey_index,
                                         size_t key_len,
                                         const uint8_t* key,
                                         size_t max_application_key_size,
                                         size_t *application_key_len,
                                         uint8_t *application_key);

/***************************************************************************//**
 *
 * Respond to the prov_oob_pkey_request.
 *
 * @param[in] uuid UUID of the Device
 * @param[in] pkey_len Length of data in @p pkey
 * @param[in] pkey Public Key read out-of-band
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_send_oob_pkey_response(uuid_128 uuid,
                                                  size_t pkey_len,
                                                  const uint8_t* pkey);

/***************************************************************************//**
 *
 * Respond to the prov_oob_auth_request.
 *
 * @param[in] uuid UUID of the Device
 * @param[in] data_len Length of data in @p data
 * @param[in] data Output or static OOB data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_send_oob_auth_response(uuid_128 uuid,
                                                  size_t data_len,
                                                  const uint8_t* data);

/***************************************************************************//**
 *
 * Set the OOB requirements for a device being provisioned. This command can be
 * issued after @ref sl_btmesh_prov_create_provisioning_session.
 *
 * @param[in] uuid UUID of the Device being provisioned
 * @param[in] public_key The public key. Set to zero if the provisioning does
 *   not use OOB public Key.
 * @param[in] auth_methods Enum @ref sl_btmesh_node_auth_method_flag_t. Allowed
 *   OOB authentication methods The value is a bitmap so that multiple methods
 *   can be supported.
 * @param[in] output_actions Enum @ref sl_btmesh_node_oob_output_action_flag_t.
 *   Allowed OOB Output Action types
 * @param[in] input_actions Enum @ref sl_btmesh_node_oob_input_action_flag_t.
 *   Allowed OOB Input Action types
 * @param[in] min_size Minimum input/output OOB size. Values range from 0
 *   (input/output OOB not used) to 8.
 * @param[in] max_size Maximum input/output OOB size. Must be smaller than or
 *   equal to the minimum size. Values range from 0 (input/output OOB not used)
 *   to 8.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_set_oob_requirements(uuid_128 uuid,
                                                uint8_t public_key,
                                                uint8_t auth_methods,
                                                uint16_t output_actions,
                                                uint16_t input_actions,
                                                uint8_t min_size,
                                                uint8_t max_size);

/***************************************************************************//**
 *
 * Start a key refresh procedure in the network.
 *
 * A key refresh procedure updates a network key and, optionally, application
 * keys associated with it in all nodes of the network except for excluded
 * nodes. After the refresh procedure is complete, the old keys will be
 * discarded. Therefore, the excluded nodes, which did not receive new keys will
 * be shut out of the network at the completion of the procedure.
 *
 * @param[in] netkey_index Index of the network key to update
 * @param[in] num_appkeys Number of application keys to update; may be zero.
 * @param[in] appkey_indices_len Length of data in @p appkey_indices
 * @param[in] appkey_indices Indices of the application keys to update,
 *   represented as little endian two byte sequences. The array must contain
 *   num_appkeys indices and therefore 2*num_appkeys bytes total.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_key_refresh_node_update
 *   - @ref sl_btmesh_evt_prov_key_refresh_phase_update
 *   - @ref sl_btmesh_evt_prov_key_refresh_complete
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_start_key_refresh(uint16_t netkey_index,
                                             uint8_t num_appkeys,
                                             size_t appkey_indices_len,
                                             const uint8_t* appkey_indices);

/***************************************************************************//**
 *
 * Check the key refresh exclusion status of a node. Excluded nodes do not
 * participate in the key refresh procedure and can therefore be shut out of the
 * network.
 *
 * @param[in] key Network key index
 * @param[in] uuid UUID of the Device
 * @param[out] status Non-zero for excluded node
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_get_key_refresh_exclusion(uint16_t key,
                                                     uuid_128 uuid,
                                                     uint8_t *status);

/***************************************************************************//**
 *
 * Set the key refresh exclusion status of a node. Excluded nodes do not
 * participate in the key refresh procedure and can therefore be shut out of the
 * network.
 *
 * @param[in] key Network key index
 * @param[in] status Non-zero for excluded node
 * @param[in] uuid UUID of the Device
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_set_key_refresh_exclusion(uint16_t key,
                                                     uint8_t status,
                                                     uuid_128 uuid);

/***************************************************************************//**
 *
 * Get a Provisioner device database entry with a matching UUID.
 *
 * @param[in] uuid UUID of the Device to retrieve
 * @param[out] device_key Device Key
 * @param[out] netkey_index Index of the network key with which the node was
 *   initially provisioned. Used for network-level encryption of Configuration
 *   Client messages.
 * @param[out] address Unicast address of the primary element of the node
 * @param[out] elements Number of elements in the node
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_get_ddb_entry(uuid_128 uuid,
                                         aes_key_128 *device_key,
                                         uint16_t *netkey_index,
                                         uint16_t *address,
                                         uint8_t *elements);

/***************************************************************************//**
 *
 * Delete the node information from the Provisioner database. Note that a
 * successful result from this command only means the command has been accepted
 * for processing. The status of the actual operation will be returned in the
 * following event; application should not make new BGAPI requests until the
 * event is received. Note also that this should be followed by a @ref
 * sl_btmesh_prov_start_key_refresh updating the keys of the remaining nodes to
 * make sure the deleted node is shut off from the network.
 *
 * @param[in] uuid UUID of the node to delete
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_delete_ddb_entry_complete
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_delete_ddb_entry(uuid_128 uuid);

/***************************************************************************//**
 *
 * Add a new node entry to the Provisioner's device database. Note that a
 * successful result from this command only means the command has been accepted
 * for processing. The status of the actual operation will be returned in the
 * following event; application should not make new BGAPI requests until the
 * event is received. Note also that the device key, primary element address,
 * and network key need to be deployed to the node being added to ensure it's
 * configurable. See @ref sl_btmesh_node_set_provisioning_data command.
 *
 * @param[in] uuid UUID of the node to add
 * @param[in] device_key Device key value for the node
 * @param[in] netkey_index Index of the network key the node will be used for
 *   configuration
 * @param[in] address Unicast address to allocate for the node's primary element
 * @param[in] elements Number of elements the device has
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_add_ddb_entry_complete
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_add_ddb_entry(uuid_128 uuid,
                                         aes_key_128 device_key,
                                         uint16_t netkey_index,
                                         uint16_t address,
                                         uint8_t elements);

/***************************************************************************//**
 *
 * List nodes known by this Provisioner. A number of @ref
 * sl_btmesh_evt_prov_ddb_list events will be generated.
 *
 * @param[out] count Number of events that will follow
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_ddb_list
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_list_ddb_entries(uint16_t *count);

/***************************************************************************//**
 *
 * Initialize the mesh network and assign the provisioner address and IV index
 * for the network. If this command is not invoked prior to invoking @ref
 * sl_btmesh_prov_create_network, the network will be initialized with default
 * address and IV index.
 *
 * @param[in] address Address to assign for provisioner.
 * @param[in] ivi IV index of the network.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_initialize_network(uint16_t address, uint32_t ivi);

/***************************************************************************//**
 *
 * Check the application key refresh exclusion status of a node. Nodes which are
 * excluded for a given application key do not receive updates for that
 * particular application key, but do participate in the key refresh procedure
 * as a whole. This enables the Provisioner to set up and update restricted sets
 * of application keys across nodes.
 *
 * @param[in] netkey_index Network key index
 * @param[in] appkey_index Application key index
 * @param[in] uuid UUID of the Device
 * @param[out] status Non-zero for excluded node
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_get_key_refresh_appkey_exclusion(uint16_t netkey_index,
                                                            uint16_t appkey_index,
                                                            uuid_128 uuid,
                                                            uint8_t *status);

/***************************************************************************//**
 *
 * Set the application key refresh exclusion status of a node. Nodes which are
 * excluded for a given application key do not receive updates for that
 * particular application key, but do participate in the key refresh procedure
 * as a whole. This enables the Provisioner to set up and update restricted sets
 * of application keys across nodes.
 *
 * @param[in] netkey_index Network key index
 * @param[in] appkey_index Application key index
 * @param[in] status Non-zero for excluded node
 * @param[in] uuid UUID of the device
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_set_key_refresh_appkey_exclusion(uint16_t netkey_index,
                                                            uint16_t appkey_index,
                                                            uint8_t status,
                                                            uuid_128 uuid);

/***************************************************************************//**
 *
 * Stop scanning for unprovisioned device beacons.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_stop_scan_unprov_beacons();

/***************************************************************************//**
 *
 * Update a node's entry in the Provisioner's device database by setting a new
 * value to the netkey_index field. The netkey_index field is used to determine
 * the network key to use when encrypting and decrypting configuration model
 * messages to and from the node.
 *
 * @param[in] uuid UUID of the node
 * @param[in] netkey_index Index of the network key used in configuring the
 *   node.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_update_device_netkey_index(uuid_128 uuid,
                                                      uint16_t netkey_index);

/***************************************************************************//**
 *
 * Suspend an ongoing key refresh procedure.
 *
 * Suspending a key refresh procedure means no further requests for updating
 * keys or setting key refresh phase will be sent to the network by the
 * Provisioner until the key refresh procedure is resumed.
 *
 * @param[in] netkey_index Index of the network key identifying an ongoing key
 *   refresh procedure
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_suspend_key_refresh(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Resume a suspended key refresh procedure.
 *
 * By resuming a suspended key refresh procedure, the Provisioner will again
 * start to send requests for updating keys or setting key refresh phase to the
 * network.
 *
 * @param[in] netkey_index Index of the network key identifying a suspended key
 *   refresh procedure
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_resume_key_refresh(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Get the key refresh phase of an ongoing key refresh procedure.
 *
 * @param[in] netkey_index Index of the network key identifying an ongoing key
 *   refresh procedure
 * @param[out] phase The current key refresh phase. Values are as follows:
 *     - 0: Normal operation (no ongoing key refresh)
 *     - 1: First phase of key refresh procedure (key deployment)
 *     - 2: Second phase of key refresh procedure (new key activation)
 *     - 3: Third phase of key refresh procedure (old key deletion)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_get_key_refresh_phase(uint16_t netkey_index,
                                                 uint8_t *phase);

/***************************************************************************//**
 *
 * Start a key refresh procedure from a non-default phase. Before calling this
 * function, keys to be used in the key refresh procedure should have been
 * specified by calling @ref sl_btmesh_test_prov_prepare_key_refresh command.
 *
 * Note that this command should not normally be used. It is intended only for
 * resuming an interrupted key refresh procedure on a backup Provisioner when
 * the original Provisioner, that started the key refresh procedure, is no
 * longer available to complete the procedure.
 *
 * @param[in] phase Current key refresh phase
 * @param[in] netkey_index Index of the network key identifying a key refresh
 *   procedure
 * @param[in] num_appkeys Number of application keys to update; may be zero.
 * @param[in] appkey_indices_len Length of data in @p appkey_indices
 * @param[in] appkey_indices Indices of the application keys to update,
 *   represented as little endian two byte sequences. The array must contain
 *   num_appkeys indices and therefore 2*num_appkeys bytes total.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_prov_key_refresh_node_update
 *   - @ref sl_btmesh_evt_prov_key_refresh_phase_update
 *   - @ref sl_btmesh_evt_prov_key_refresh_complete
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_start_key_refresh_from_phase(uint8_t phase,
                                                        uint16_t netkey_index,
                                                        uint8_t num_appkeys,
                                                        size_t appkey_indices_len,
                                                        const uint8_t* appkey_indices);

/***************************************************************************//**
 *
 * Clear the key refresh state stored in persistent storage.
 *
 * Note that this command should not normally be used. It is intended only for
 * clearing stored key refresh state when a key refresh procedure has been
 * suspended and will not be resumed, either because the network key has been
 * deleted from all nodes or the responsibility for completing the key refresh
 * has been moved to another Provisioner.
 *
 * @param[in] netkey_index Index of the network key identifying a key refresh
 *   procedure
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_flush_key_refresh_state(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Test if data in the identity beacon matches the mesh device and network.
 *
 * @param[in] address Mesh address of the node
 * @param[in] netkey_index Network key index of the node.
 * @param[in] data_len Length of data in @p data
 * @param[in] data Contents of the identity beacon.
 * @param[out] match   - 0: Identity record did not match
 *     - 1: Identity record match
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_prov_test_identity(uint16_t address,
                                         uint16_t netkey_index,
                                         size_t data_len,
                                         const uint8_t* data,
                                         uint8_t *match);

/** @} */ // end addtogroup sl_btmesh_prov

/**
 * @addtogroup sl_btmesh_proxy Bluetooth Mesh Proxy Connections
 * @{
 *
 * @brief Bluetooth Mesh Proxy Connections
 *
 * Bluetooth mesh stack functions for GATT proxy connections
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_proxy_connect_id                                   0x00180028
#define sl_btmesh_cmd_proxy_disconnect_id                                0x01180028
#define sl_btmesh_cmd_proxy_set_filter_type_id                           0x02180028
#define sl_btmesh_cmd_proxy_allow_id                                     0x03180028
#define sl_btmesh_cmd_proxy_deny_id                                      0x04180028
#define sl_btmesh_cmd_proxy_optimisation_toggle_id                       0x05180028
#define sl_btmesh_rsp_proxy_connect_id                                   0x00180028
#define sl_btmesh_rsp_proxy_disconnect_id                                0x01180028
#define sl_btmesh_rsp_proxy_set_filter_type_id                           0x02180028
#define sl_btmesh_rsp_proxy_allow_id                                     0x03180028
#define sl_btmesh_rsp_proxy_deny_id                                      0x04180028
#define sl_btmesh_rsp_proxy_optimisation_toggle_id                       0x05180028

/**
 * @addtogroup sl_btmesh_evt_proxy_connected sl_btmesh_evt_proxy_connected
 * @{
 * @brief Indication that a connection has been successfully formed.
 */

/** @brief Identifier of the connected event */
#define sl_btmesh_evt_proxy_connected_id                                 0x001800a8

/***************************************************************************//**
 * @brief Data structure of the connected event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_proxy_connected_s
{
  uint32_t handle; /**< Proxy handle */
});

typedef struct sl_btmesh_evt_proxy_connected_s sl_btmesh_evt_proxy_connected_t;

/** @} */ // end addtogroup sl_btmesh_evt_proxy_connected

/**
 * @addtogroup sl_btmesh_evt_proxy_disconnected sl_btmesh_evt_proxy_disconnected
 * @{
 * @brief Indication that a connection has been disconnected or a connection
 * attempt failed.
 */

/** @brief Identifier of the disconnected event */
#define sl_btmesh_evt_proxy_disconnected_id                              0x011800a8

/***************************************************************************//**
 * @brief Data structure of the disconnected event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_proxy_disconnected_s
{
  uint32_t handle; /**< Proxy handle */
  uint16_t reason; /**< Reason for disconnection */
});

typedef struct sl_btmesh_evt_proxy_disconnected_s sl_btmesh_evt_proxy_disconnected_t;

/** @} */ // end addtogroup sl_btmesh_evt_proxy_disconnected

/**
 * @addtogroup sl_btmesh_evt_proxy_filter_status sl_btmesh_evt_proxy_filter_status
 * @{
 * @brief Proxy status report event
 */

/** @brief Identifier of the filter_status event */
#define sl_btmesh_evt_proxy_filter_status_id                             0x021800a8

/***************************************************************************//**
 * @brief Data structure of the filter_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_proxy_filter_status_s
{
  uint32_t handle; /**< Proxy handle */
  uint8_t  type;   /**< Filter type: 0x00 to allow traffic from the given
                        address, 0x01 to block traffic from the given address */
  uint16_t count;  /**< Current filter list length */
});

typedef struct sl_btmesh_evt_proxy_filter_status_s sl_btmesh_evt_proxy_filter_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_proxy_filter_status

/***************************************************************************//**
 *
 * Start connecting a proxy client to a proxy server. After the connection is
 * complete, a @ref sl_btmesh_evt_proxy_connected event will be generated.
 * LE-connection must be opened prior to opening the proxy connection.
 *
 * @param[in] connection Connection handle
 * @param[out] handle If a connection attempt is successfully initiated, a valid
 *   proxy handle will be returned.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_connect(uint8_t connection, uint32_t *handle);

/***************************************************************************//**
 *
 * Disconnect. This call can be used also for a connection, which is not yet
 * fully formed.
 *
 * @param[in] handle Proxy handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_disconnect(uint32_t handle);

/***************************************************************************//**
 *
 * Set up the proxy filtering type. At the proxy server side, this is a local
 * configuration, while on the proxy client a proxy configuration PDU will be
 * sent to the proxy server.
 *
 * @param[in] handle Proxy handle
 * @param[in] netkey_index Network key index used in encrypting the request to
 *   the proxy server
 * @param[in] type Filter type: 0x00 to allow traffic from the given address,
 *   0x01 to block traffic from the given address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_set_filter_type(uint32_t handle,
                                            uint16_t netkey_index,
                                            uint8_t type);

/***************************************************************************//**
 *
 * Allow messages meant for the given address to be forwarded over the proxy
 * connection to the proxy client. At the proxy server side, this is a local
 * configuration, while on the proxy client a proxy configuration PDU will be
 * sent to the proxy server.
 *
 * @param[in] handle Proxy handle
 * @param[in] netkey_index Network key index used in encrypting the request to
 *   the proxy server
 * @param[in] address Destination address to allow. The address may be either a
 *   unicast address, a group address, or a virtual address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_allow(uint32_t handle,
                                  uint16_t netkey_index,
                                  uint16_t address);

/***************************************************************************//**
 *
 * Block messages meant for the given address from being forwarded over the
 * proxy connection to the proxy client. At the proxy server side, this is a
 * local configuration, while on the proxy client a proxy configuration PDU will
 * be sent to the proxy server.
 *
 * @param[in] handle Proxy handle
 * @param[in] netkey_index Network key index used in encrypting the request to
 *   the proxy server
 * @param[in] address Destination address to block. The address may be either a
 *   unicast address, a group address, or a virtual address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_deny(uint32_t handle,
                                 uint16_t netkey_index,
                                 uint16_t address);

/***************************************************************************//**
 *
 * In case of unicast address, if proxy identified the destination, the message
 * will be forwarded only to that node, otherwise to all. This functionality
 * could be enabled or disabled with this function.
 *
 * @param[in] enable Non zero - enable, otherwise disable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_proxy_optimisation_toggle(uint8_t enable);

/** @} */ // end addtogroup sl_btmesh_proxy

/**
 * @addtogroup sl_btmesh_vendor_model Bluetooth Mesh Vendor Model
 * @{
 *
 * @brief Bluetooth Mesh Vendor Model
 *
 * Vendor model API provides functionality to send and receive vendor-specific
 * messages.
 *
 * Throughout the API, the manipulated model is identified by its element
 * address, vendor ID, and model ID.
 *
 * The API has functions for sending, receiving, and publishing messages. The
 * application has to implement additional complex functionality (state machines
 * or other model-specific logic).
 *
 * The stack will handle Mesh transaction layer segmentation and reassembly
 * automatically if the messages sent are long enough to require it.
 *
 * Note that as the application layer overhead for vendor messages is three
 * bytes (vendor ID and opcode) and the access layer MIC is at least four bytes,
 * the longest vendor application payload which can be sent using an unsegmented
 * transport layer PDU is eight bytes. On the other hand, the longest vendor
 * application payload which can be sent using transport layer segmentation is
 * 377 bytes (fragmented into 32 segments).
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_vendor_model_send_id                               0x00190028
#define sl_btmesh_cmd_vendor_model_set_publication_id                    0x01190028
#define sl_btmesh_cmd_vendor_model_clear_publication_id                  0x02190028
#define sl_btmesh_cmd_vendor_model_publish_id                            0x03190028
#define sl_btmesh_cmd_vendor_model_init_id                               0x04190028
#define sl_btmesh_cmd_vendor_model_deinit_id                             0x05190028
#define sl_btmesh_cmd_vendor_model_send_tracked_id                       0x06190028
#define sl_btmesh_cmd_vendor_model_set_publication_tracked_id            0x07190028
#define sl_btmesh_rsp_vendor_model_send_id                               0x00190028
#define sl_btmesh_rsp_vendor_model_set_publication_id                    0x01190028
#define sl_btmesh_rsp_vendor_model_clear_publication_id                  0x02190028
#define sl_btmesh_rsp_vendor_model_publish_id                            0x03190028
#define sl_btmesh_rsp_vendor_model_init_id                               0x04190028
#define sl_btmesh_rsp_vendor_model_deinit_id                             0x05190028
#define sl_btmesh_rsp_vendor_model_send_tracked_id                       0x06190028
#define sl_btmesh_rsp_vendor_model_set_publication_tracked_id            0x07190028

/**
 * @addtogroup sl_btmesh_evt_vendor_model_receive sl_btmesh_evt_vendor_model_receive
 * @{
 * @brief Vendor model message reception event
 *
 * Stack generates this event when a vendor message with a valid opcode is
 * received.
 *
 * Note that because of the bgapi event length restrictions, the message may be
 * fragmented into several events. If this is the case, the events will be
 * generated by the stack in the correct order and the last event will be marked
 * with the final flag set to a non-zero value. The application has to
 * concatenate the messages into a single buffer if necessary.
 */

/** @brief Identifier of the receive event */
#define sl_btmesh_evt_vendor_model_receive_id                            0x001900a8

/***************************************************************************//**
 * @brief Data structure of the receive event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_vendor_model_receive_s
{
  uint16_t   destination_address; /**< Address the message was sent to. It can
                                       be either the model element's unicast
                                       address or a subscription address of the
                                       model */
  uint16_t   elem_index;          /**< Receiving model element index */
  uint16_t   vendor_id;           /**< Vendor ID of the receiving model */
  uint16_t   model_id;            /**< Model ID of the receiving model */
  uint16_t   source_address;      /**< Unicast address of the model which sent
                                       the message */
  int8_t     va_index;            /**< Index of the destination Label UUID
                                       (valid only is the destination address is
                                       a virtual addres) */
  uint16_t   appkey_index;        /**< The application key index used */
  uint8_t    nonrelayed;          /**< If non-zero, indicates that the received
                                       message was not relayed (TTL was zero),
                                       which means that the devices are within
                                       direct radio range of each other. */
  uint8_t    opcode;              /**< Message opcode */
  uint8_t    final;               /**< Indicates whether this payload chunk is
                                       the final one of the message or whether
                                       more will follow */
  uint8array payload;             /**< Payload data (either complete or partial;
                                       see final parameter) */
});

typedef struct sl_btmesh_evt_vendor_model_receive_s sl_btmesh_evt_vendor_model_receive_t;

/** @} */ // end addtogroup sl_btmesh_evt_vendor_model_receive

/**
 * @addtogroup sl_btmesh_evt_vendor_model_send_complete sl_btmesh_evt_vendor_model_send_complete
 * @{
 * @brief Vendor model message send complete event
 *
 * Stack generates this event when a vendor message has been sent or published,
 * either successfully or unsuccessfully.
 *
 * The time this event is generated depends on the message being sent or
 * published. For an unsegmented message the event is generated immediately
 * after the network PDU is sent out, while for a segmented message the event is
 * generated once the segmented message transmission state machine completes.
 */

/** @brief Identifier of the send_complete event */
#define sl_btmesh_evt_vendor_model_send_complete_id                      0x011900a8

/***************************************************************************//**
 * @brief Data structure of the send_complete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_vendor_model_send_complete_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint16_t handle; /**< Tracking handle for the message sending attempt as
                        reported by @ref sl_btmesh_vendor_model_send_tracked or
                        @ref sl_btmesh_vendor_model_set_publication_tracked
                        command responses. */
});

typedef struct sl_btmesh_evt_vendor_model_send_complete_s sl_btmesh_evt_vendor_model_send_complete_t;

/** @} */ // end addtogroup sl_btmesh_evt_vendor_model_send_complete

/***************************************************************************//**
 *
 * Send vendor-specific data.
 *
 * Note that, because of the bgapi event length restrictions, the message sent
 * may need to be fragmented into several commands. If this is the case, the
 * application must issue the commands in the correct order and mark the command
 * carrying the last message fragment with the final flag set to a non-zero
 * value. The stack will not start sending the message until the complete
 * message is provided by the application. Fragments from multiple messages must
 * not be interleaved.
 *
 * @param[in] destination_address Destination address of the message. It can be
 *   a unicast address, a group address, or a virtual address.
 * @param[in] va_index Index of the destination Label UUID (used only is the
 *   destination address is a virtual address)
 * @param[in] appkey_index The application key index used
 * @param[in] elem_index Sending model element index
 * @param[in] vendor_id Vendor ID of the sending model
 * @param[in] model_id Model ID of the sending model
 * @param[in] nonrelayed If the message is a response to a received message, set
 *   this parameter according to what was received in the receive event.
 *   Otherwise, set to non-zero if the message affects only devices in the
 *   immediate radio neighborhood.
 * @param[in] opcode Message opcode
 * @param[in] final Indicates whether this payload chunk is the final one of the
 *   message or whether more will follow.
 * @param[in] payload_len Length of data in @p payload
 * @param[in] payload Payload data (either complete or partial; see final
 *   parameter).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_send(uint16_t destination_address,
                                        int8_t va_index,
                                        uint16_t appkey_index,
                                        uint16_t elem_index,
                                        uint16_t vendor_id,
                                        uint16_t model_id,
                                        uint8_t nonrelayed,
                                        uint8_t opcode,
                                        uint8_t final,
                                        size_t payload_len,
                                        const uint8_t* payload);

/***************************************************************************//**
 *
 * Set the vendor model publication message.
 *
 * The model publication message will be sent out when the model publication
 * occurs either periodically (if the model is configured for periodic
 * publishing) or explicitly (see @ref sl_btmesh_vendor_model_publish.
 *
 * Note that, because of bgapi length requirements, the message may need to be
 * fragmented over multiple commands. If this is the case, the application must
 * issue the commands in the correct order and mark the command carrying the
 * last message fragment with the final flag set to a non-zero value. The stack
 * will not assign the message to the model until the complete message is
 * provided by the application.
 *
 * To disable publication, the publication message may be erased using the @ref
 * sl_btmesh_vendor_model_clear_publication command.
 *
 * @param[in] elem_index Publishing model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 * @param[in] opcode Message opcode
 * @param[in] final Indicates whether this payload chunk is the final one of the
 *   message or whether more will follow.
 * @param[in] payload_len Length of data in @p payload
 * @param[in] payload Payload data (either complete or partial; see final
 *   parameter).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_set_publication(uint16_t elem_index,
                                                   uint16_t vendor_id,
                                                   uint16_t model_id,
                                                   uint8_t opcode,
                                                   uint8_t final,
                                                   size_t payload_len,
                                                   const uint8_t* payload);

/***************************************************************************//**
 *
 * Clear the vendor model publication message.
 *
 * Clearing the model publication message disables model publishing, which can
 * be re-enabled by defining the publication message using the @ref
 * sl_btmesh_vendor_model_set_publication command.
 *
 * @param[in] elem_index Publishing model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_clear_publication(uint16_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id);

/***************************************************************************//**
 *
 * Publish the vendor model publication message.
 *
 * Sends the stored publication message to the network using the application key
 * and destination address stored in the model publication parameters.
 *
 * @param[in] elem_index Publishing model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_publish(uint16_t elem_index,
                                           uint16_t vendor_id,
                                           uint16_t model_id);

/***************************************************************************//**
 *
 * Initialize the vendor model. This function has to be called before the model
 * can be used. Note that the model can be deinitialized if it is no longer
 * needed. See sl_btmesh_vendor_model_deinit.
 *
 * Opcodes that the model is able to receive at initialization must be defined.
 * This enables the stack to pass only valid messages up to the model during
 * runtime. Per Mesh specification there are up to 64 opcodes per vendor,
 * ranging from 0 to 63. Specifying opcodes outside of that range will result in
 * an error response. Duplicate opcodes in the array do not result in an error,
 * but will of course be recorded only once.
 *
 * @param[in] elem_index Model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 * @param[in] publish Indicates if the model is a publish model (non-zero) or
 *   not (zero).
 * @param[in] opcodes_len Length of data in @p opcodes
 * @param[in] opcodes Array of opcodes the model can handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_init(uint16_t elem_index,
                                        uint16_t vendor_id,
                                        uint16_t model_id,
                                        uint8_t publish,
                                        size_t opcodes_len,
                                        const uint8_t* opcodes);

/***************************************************************************//**
 *
 * Deinitialize the model. After this call, the model cannot be used until it is
 * initialized again. See @ref sl_btmesh_vendor_model_init.
 *
 * @param[in] elem_index Model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_deinit(uint16_t elem_index,
                                          uint16_t vendor_id,
                                          uint16_t model_id);

/***************************************************************************//**
 *
 * Send vendor-specific data with a delivery report.
 *
 * This command works otherwise as @ref sl_btmesh_vendor_model_send but also
 * provides a tracking handle if there is no immediate local error. Once the
 * message sending completes a @ref sl_btmesh_evt_vendor_model_send_complete
 * event is generated, and the handle received here can be used to correlate the
 * event to the sent message.
 *
 * @param[in] destination_address Destination address of the message. It can be
 *   a unicast address, a group address, or a virtual address.
 * @param[in] va_index Index of the destination Label UUID (used only is the
 *   destination address is a virtual address)
 * @param[in] appkey_index The application key index used
 * @param[in] elem_index Sending model element index
 * @param[in] vendor_id Vendor ID of the sending model
 * @param[in] model_id Model ID of the sending model
 * @param[in] nonrelayed If the message is a response to a received message, set
 *   this parameter according to what was received in the receive event.
 *   Otherwise, set to non-zero if the message affects only devices in the
 *   immediate radio neighborhood.
 * @param[in] segment If nonzero, instruct the stack to use transport layer
 *   segmentation, even if the data would be short enough to send as an
 *   unsegmented message. If zero, stack selects automatically based on data
 *   length whether to use segmentation or not.
 * @param[in] opcode Message opcode
 * @param[in] final Indicates whether this payload chunk is the final one of the
 *   message or whether more will follow.
 * @param[in] payload_len Length of data in @p payload
 * @param[in] payload Payload data (either complete or partial; see final
 *   parameter).
 * @param[out] handle Tracking handle for the message sending. Note that a valid
 *   handle is returned only when the final payload fragment is received by the
 *   stack (as indicated by the final parameter of this message) and the message
 *   is accepted for delivery (as indicated by the result code). Otherwise the
 *   value of this parameter should be ignored. attempt
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_send_tracked(uint16_t destination_address,
                                                int8_t va_index,
                                                uint16_t appkey_index,
                                                uint16_t elem_index,
                                                uint16_t vendor_id,
                                                uint16_t model_id,
                                                uint8_t nonrelayed,
                                                uint8_t segment,
                                                uint8_t opcode,
                                                uint8_t final,
                                                size_t payload_len,
                                                const uint8_t* payload,
                                                uint16_t *handle);

/***************************************************************************//**
 *
 * Set the vendor model publication message with a delivery report.
 *
 * This command works otherwise as @ref sl_btmesh_vendor_model_set_publication
 * but also sets up a tracking handle for monitoring message delivery. When the
 * message message publishing completes a @ref
 * sl_btmesh_evt_vendor_model_send_complete event is generated, and the handle
 * received here can be used to correlate the event to the sent message.
 *
 * Note that the same tracking handle will be reported multiple times in case of
 * periodic publication or publication retransmissions.
 *
 * @param[in] elem_index Publishing model element index
 * @param[in] vendor_id Vendor ID of the model
 * @param[in] model_id Model ID of the model
 * @param[in] segment If nonzero, instruct the stack to use transport layer
 *   segmentation, even if the data would be short enough to publish as an
 *   unsegmented message. If zero, stack selects automatically based on data
 *   length whether to use segmentation or not.
 * @param[in] opcode Message opcode
 * @param[in] final Indicates whether this payload chunk is the final one of the
 *   message or whether more will follow.
 * @param[in] payload_len Length of data in @p payload
 * @param[in] payload Payload data (either complete or partial; see final
 *   parameter).
 * @param[out] handle Tracking handle for the message publishing. Note that a
 *   valid handle is returned only when the final payload fragment is received
 *   by the stack (as indicated by the final parameter of this message) and the
 *   message is accepted for publication (as indicated by the result code).
 *   Otherwise the value of this parameter should be ignored. attempt
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_vendor_model_set_publication_tracked(uint16_t elem_index,
                                                           uint16_t vendor_id,
                                                           uint16_t model_id,
                                                           uint8_t segment,
                                                           uint8_t opcode,
                                                           uint8_t final,
                                                           size_t payload_len,
                                                           const uint8_t* payload,
                                                           uint16_t *handle);

/** @} */ // end addtogroup sl_btmesh_vendor_model

/**
 * @addtogroup sl_btmesh_health_client Bluetooth Mesh Health Client Model
 * @{
 *
 * @brief Bluetooth Mesh Health Client Model
 *
 * Bluetooth mesh health client model functionality
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_health_client_get_id                               0x001a0028
#define sl_btmesh_cmd_health_client_clear_id                             0x011a0028
#define sl_btmesh_cmd_health_client_test_id                              0x021a0028
#define sl_btmesh_cmd_health_client_get_period_id                        0x031a0028
#define sl_btmesh_cmd_health_client_set_period_id                        0x041a0028
#define sl_btmesh_cmd_health_client_get_attention_id                     0x051a0028
#define sl_btmesh_cmd_health_client_set_attention_id                     0x061a0028
#define sl_btmesh_rsp_health_client_get_id                               0x001a0028
#define sl_btmesh_rsp_health_client_clear_id                             0x011a0028
#define sl_btmesh_rsp_health_client_test_id                              0x021a0028
#define sl_btmesh_rsp_health_client_get_period_id                        0x031a0028
#define sl_btmesh_rsp_health_client_set_period_id                        0x041a0028
#define sl_btmesh_rsp_health_client_get_attention_id                     0x051a0028
#define sl_btmesh_rsp_health_client_set_attention_id                     0x061a0028

/**
 * @addtogroup sl_btmesh_evt_health_client_server_status sl_btmesh_evt_health_client_server_status
 * @{
 * @brief Receiving a Health Server fault status message generates this event
 *
 * The Client model may receive a status message because:
 *   - * it made a @ref sl_btmesh_health_client_get to which a Server model
 *     responded, or
 *   - * it made a @ref sl_btmesh_health_client_clear to which a Server model
 *     responded, or
 *   - * it made a @ref sl_btmesh_health_client_test to which a Server model
 *     responded.
 */

/** @brief Identifier of the server_status event */
#define sl_btmesh_evt_health_client_server_status_id                     0x001a00a8

/***************************************************************************//**
 * @brief Data structure of the server_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_health_client_server_status_s
{
  uint16_t   client_address; /**< Destination address the message was sent to */
  uint16_t   elem_index;     /**< Client model element index. Identifies the
                                  client model which received the status
                                  message. */
  uint16_t   server_address; /**< Address of the Server model which sent the
                                  message */
  uint16_t   result;         /**< Response status. If an error occurs (e.g.,
                                  request timeout), the parameters other than
                                  element index, client address, and server
                                  address are to be ignored. */
  uint8_t    current;        /**< Whether the event lists current fault array or
                                  registered fault array */
  uint8_t    test_id;        /**< Test ID */
  uint16_t   vendor_id;      /**< Bluetooth vendor ID used in the request */
  uint8array faults;         /**< Fault array. See the Bluetooth Mesh Profile
                                  specification for a list of defined fault IDs. */
});

typedef struct sl_btmesh_evt_health_client_server_status_s sl_btmesh_evt_health_client_server_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_health_client_server_status

/**
 * @addtogroup sl_btmesh_evt_health_client_server_status_period sl_btmesh_evt_health_client_server_status_period
 * @{
 * @brief Receiving a Health Server period status message generates this event
 */

/** @brief Identifier of the server_status_period event */
#define sl_btmesh_evt_health_client_server_status_period_id              0x011a00a8

/***************************************************************************//**
 * @brief Data structure of the server_status_period event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_health_client_server_status_period_s
{
  uint16_t client_address; /**< Destination address the message was sent to */
  uint16_t elem_index;     /**< Client model element index. Identifies the
                                client model, which received the status message. */
  uint16_t server_address; /**< Address of the Server model which sent the
                                message */
  uint16_t result;         /**< Response status. If an error occurs (e.g.,
                                request timeout), ignore the parameters other
                                than element index, client address, and server
                                address. */
  uint8_t  period;         /**< Health period divisor value */
});

typedef struct sl_btmesh_evt_health_client_server_status_period_s sl_btmesh_evt_health_client_server_status_period_t;

/** @} */ // end addtogroup sl_btmesh_evt_health_client_server_status_period

/**
 * @addtogroup sl_btmesh_evt_health_client_server_status_attention sl_btmesh_evt_health_client_server_status_attention
 * @{
 * @brief Receiving a Health Server attention status message generates this
 * event
 */

/** @brief Identifier of the server_status_attention event */
#define sl_btmesh_evt_health_client_server_status_attention_id           0x021a00a8

/***************************************************************************//**
 * @brief Data structure of the server_status_attention event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_health_client_server_status_attention_s
{
  uint16_t client_address; /**< Destination address the message was sent to */
  uint16_t elem_index;     /**< Client model element index. Identifies the
                                client model which received the status message. */
  uint16_t server_address; /**< Address of the Server model which sent the
                                message */
  uint16_t result;         /**< Response status. If an error occurs (e.g.,
                                request timeout), ignore the parameters other
                                than element index, client address, and server
                                address. */
  uint8_t  attention_sec;  /**< Current attention timer value in seconds */
});

typedef struct sl_btmesh_evt_health_client_server_status_attention_s sl_btmesh_evt_health_client_server_status_attention_t;

/** @} */ // end addtogroup sl_btmesh_evt_health_client_server_status_attention

/***************************************************************************//**
 *
 * Get the registered fault status of a Health Server model or models in the
 * network.
 *
 * Besides the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 * @param[in] vendor_id Bluetooth vendor ID used in the request
 *
 * @return If an error occurs locally, (for instance, because of invalid
 *   parameters) an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_get(uint16_t server_address,
                                        uint16_t elem_index,
                                        uint16_t appkey_index,
                                        uint16_t vendor_id);

/***************************************************************************//**
 *
 * Clear the fault status of a Health Server model or models in the network.
 *
 * Besides the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 * @param[in] vendor_id Bluetooth vendor ID used in the request
 * @param[in] reliable If non-zero, a reliable model message is used.
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters), an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_clear(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t appkey_index,
                                          uint16_t vendor_id,
                                          uint8_t reliable);

/***************************************************************************//**
 *
 * Execute a self test on a server model or models in the network.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 * @param[in] test_id Test ID used in the request
 * @param[in] vendor_id Bluetooth vendor ID used in the request
 * @param[in] reliable If non-zero, a reliable model message is used.
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters) an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_test(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index,
                                         uint8_t test_id,
                                         uint16_t vendor_id,
                                         uint8_t reliable);

/***************************************************************************//**
 *
 * Get the health period log of a Health Server model or models in the network.
 *
 * Except for the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address, which may be a
 *   unicast address or a group address
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 *
 * @return If an error occurs locally, (for instance, because of invalid
 *   parameters) an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_get_period(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index);

/***************************************************************************//**
 *
 * Set the health period divisor of a Health Server model or models in the
 * network.
 *
 * Except for the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index, which identifies the client
 *   model used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 * @param[in] period Health period divisor value
 * @param[in] reliable If non-zero, a reliable model message is used.
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters), an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_set_period(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t period,
                                               uint8_t reliable);

/***************************************************************************//**
 *
 * Get the attention timer value of a Health Server model or models in the
 * network.
 *
 * Besides the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters), an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_get_attention(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index);

/***************************************************************************//**
 *
 * Set the attention timer value of a Health Server model or models in the
 * network.
 *
 * Except for the immediate result code, the response or responses (if the
 * destination server address is a group address) from the network will generate
 * @ref sl_btmesh_evt_health_client_server_status.
 *
 * @param[in] server_address Destination server model address. May be a unicast
 *   address or a group address.
 * @param[in] elem_index Client model element index. Identifies the client model
 *   used for sending the request.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request
 * @param[in] attention_sec Attention timer period in seconds
 * @param[in] reliable If non-zero, a reliable model message is used.
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters), an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_client_set_attention(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t attention_sec,
                                                  uint8_t reliable);

/** @} */ // end addtogroup sl_btmesh_health_client

/**
 * @addtogroup sl_btmesh_health_server Bluetooth Mesh Health Server Model
 * @{
 *
 * @brief Bluetooth Mesh Health Server Model
 *
 * Bluetooth mesh health server model functionality
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_health_server_set_fault_id                         0x001b0028
#define sl_btmesh_cmd_health_server_clear_fault_id                       0x011b0028
#define sl_btmesh_cmd_health_server_send_test_response_id                0x021b0028
#define sl_btmesh_rsp_health_server_set_fault_id                         0x001b0028
#define sl_btmesh_rsp_health_server_clear_fault_id                       0x011b0028
#define sl_btmesh_rsp_health_server_send_test_response_id                0x021b0028

/**
 * @addtogroup sl_btmesh_evt_health_server_attention sl_btmesh_evt_health_server_attention
 * @{
 * @brief
 *
 * The attention timer on an element is set to a given value. This may happen,
 * for instance, during provisioning. The application should use suitable means
 * to get the user's attention, e.g., by vibrating or blinking an LED.
 */

/** @brief Identifier of the attention event */
#define sl_btmesh_evt_health_server_attention_id                         0x001b00a8

/***************************************************************************//**
 * @brief Data structure of the attention event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_health_server_attention_s
{
  uint16_t elem_index; /**< Index of the element for which attention timer was
                            set */
  uint8_t  timer_sec;  /**< Timer value in seconds. If zero, user attention is
                            no longer required. */
});

typedef struct sl_btmesh_evt_health_server_attention_s sl_btmesh_evt_health_server_attention_t;

/** @} */ // end addtogroup sl_btmesh_evt_health_server_attention

/**
 * @addtogroup sl_btmesh_evt_health_server_test_request sl_btmesh_evt_health_server_test_request
 * @{
 * @brief
 *
 * Health client request for a self test generates this event. After the test
 * has been executed, test results may need to be reported.
 */

/** @brief Identifier of the test_request event */
#define sl_btmesh_evt_health_server_test_request_id                      0x011b00a8

/***************************************************************************//**
 * @brief Data structure of the test_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_health_server_test_request_s
{
  uint16_t server_address;    /**< Destination address the message was sent to.
                                   It can be either the Server model element's
                                   unicast address, or a subscription address of
                                   the Server model. */
  uint16_t elem_index;        /**< Server model element index. Identifies the
                                   Server model that received the request as
                                   well as the element on which the test is to
                                   be performed. */
  uint16_t client_address;    /**< Address of the client model which sent the
                                   message */
  uint16_t appkey_index;      /**< The application key index to use in
                                   encrypting the request. Any response sent
                                   must be encrypted using the same key. */
  uint8_t  test_id;           /**< Test ID */
  uint16_t vendor_id;         /**< Bluetooth vendor ID used in the request */
  uint8_t  response_required; /**< Non-zero if client expects a response. The
                                   application should issue a @ref
                                   sl_btmesh_health_server_send_test_response
                                   once it has processed the request. */
});

typedef struct sl_btmesh_evt_health_server_test_request_s sl_btmesh_evt_health_server_test_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_health_server_test_request

/***************************************************************************//**
 *
 * Set the fault condition on an element.
 *
 * @param[in] elem_index Index of the element on which the fault is occurring
 * @param[in] id Fault ID. See the Mesh Profile specification for IDs defined by
 *   the Bluetooth SIG.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_server_set_fault(uint16_t elem_index, uint8_t id);

/***************************************************************************//**
 *
 * Clear the fault condition on an element.
 *
 * @param[in] elem_index Index of the element on which the fault is no longer
 *   occurring.
 * @param[in] id Fault ID. See the Mesh Profile specification for IDs defined by
 *   the Bluetooth SIG.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_server_clear_fault(uint16_t elem_index,
                                                uint8_t id);

/***************************************************************************//**
 *
 * Indicate to the stack that a test request has been completed and that the
 * status may be communicated to the Health Client, which made the test request.
 *
 * @param[in] client_address Address of the client model which sent the message
 * @param[in] elem_index Server model element index. Identifies the Server model
 *   that received the request as well as the element on which the test is to be
 *   performed.
 * @param[in] appkey_index The application key index to use in encrypting the
 *   request.
 * @param[in] vendor_id Bluetooth vendor ID used in the request
 *
 * @return If an error occurs locally (for instance, because of invalid
 *   parameters), an errorcode parameter is returned immediately.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_health_server_send_test_response(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint16_t vendor_id);

/** @} */ // end addtogroup sl_btmesh_health_server

/**
 * @addtogroup sl_btmesh_generic_client Bluetooth Mesh Generic Client Model
 * @{
 *
 * @brief Bluetooth Mesh Generic Client Model
 *
 * Generic client model API provides a functionality to send and receive
 * messages using Bluetooth SIG client models, including generic client models
 * and lighting client models.
 *
 * In the API, the client model that is used is identified by its element
 * address and model ID, while the server model responding to client model
 * requests is identified by its element address and model ID.
 *
 * The API has functions for querying server model states, requesting server
 * model state changes, and publishing messages. The application has to
 * implement more complex functionality (state machines or other model-specific
 * logic).
 *
 * Data for state change requests and server responses is passed as serialized
 * byte arrays through BGAPI. There are functions to convert byte arrays to and
 * from model state structures in the Bluetooth mesh SDK.
 *
 * The stack will handle Mesh transaction layer segmentation and reassembly
 * automatically if the messages sent are long enough to require it.
 *
 * <b>Note on time resolution</b>
 *
 * Because of message formatting, transition time and remaining time resolution
 * units depend on the requested or reported value. For example, until 6.2
 * seconds it is 100 ms; until 62 seconds it is 1 s; until 620 seconds it is 10
 * s; and until 620 minutes it is 10 minutes. The value cannot be longer than
 * 620 minutes. Therefore, it is not possible to request a delay of exactly 7500
 * ms. The resolution unit is 1 s between 6.2 and 62 seconds, so the value would
 * be rounded down to 7 s.
 *
 * Delay resolution is 5 ms and values will be rounded down to the closest 5 ms.
 * The value can't be longer than 1275 ms.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_generic_client_get_id                              0x001e0028
#define sl_btmesh_cmd_generic_client_set_id                              0x011e0028
#define sl_btmesh_cmd_generic_client_publish_id                          0x021e0028
#define sl_btmesh_cmd_generic_client_get_params_id                       0x031e0028
#define sl_btmesh_cmd_generic_client_init_id                             0x041e0028
#define sl_btmesh_cmd_generic_client_init_common_id                      0x051e0028
#define sl_btmesh_cmd_generic_client_init_on_off_id                      0x061e0028
#define sl_btmesh_cmd_generic_client_init_level_id                       0x071e0028
#define sl_btmesh_cmd_generic_client_init_default_transition_time_id     0x081e0028
#define sl_btmesh_cmd_generic_client_init_power_on_off_id                0x091e0028
#define sl_btmesh_cmd_generic_client_init_power_level_id                 0x0a1e0028
#define sl_btmesh_cmd_generic_client_init_battery_id                     0x0b1e0028
#define sl_btmesh_cmd_generic_client_init_location_id                    0x0c1e0028
#define sl_btmesh_cmd_generic_client_init_property_id                    0x0d1e0028
#define sl_btmesh_cmd_generic_client_init_lightness_id                   0x0e1e0028
#define sl_btmesh_cmd_generic_client_init_ctl_id                         0x0f1e0028
#define sl_btmesh_cmd_generic_client_init_hsl_id                         0x101e0028
#define sl_btmesh_rsp_generic_client_get_id                              0x001e0028
#define sl_btmesh_rsp_generic_client_set_id                              0x011e0028
#define sl_btmesh_rsp_generic_client_publish_id                          0x021e0028
#define sl_btmesh_rsp_generic_client_get_params_id                       0x031e0028
#define sl_btmesh_rsp_generic_client_init_id                             0x041e0028
#define sl_btmesh_rsp_generic_client_init_common_id                      0x051e0028
#define sl_btmesh_rsp_generic_client_init_on_off_id                      0x061e0028
#define sl_btmesh_rsp_generic_client_init_level_id                       0x071e0028
#define sl_btmesh_rsp_generic_client_init_default_transition_time_id     0x081e0028
#define sl_btmesh_rsp_generic_client_init_power_on_off_id                0x091e0028
#define sl_btmesh_rsp_generic_client_init_power_level_id                 0x0a1e0028
#define sl_btmesh_rsp_generic_client_init_battery_id                     0x0b1e0028
#define sl_btmesh_rsp_generic_client_init_location_id                    0x0c1e0028
#define sl_btmesh_rsp_generic_client_init_property_id                    0x0d1e0028
#define sl_btmesh_rsp_generic_client_init_lightness_id                   0x0e1e0028
#define sl_btmesh_rsp_generic_client_init_ctl_id                         0x0f1e0028
#define sl_btmesh_rsp_generic_client_init_hsl_id                         0x101e0028

/**
 * @addtogroup sl_btmesh_generic_client_get_state_type Generic Client Get State Types
 * @{
 *
 * Generic client get state type identifies the state which the client retrieves
 * from the remote server model.
 */

/** Generic on/off get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_ON_OFF                    0x0       

/** Generic on power up get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_ON_POWER_UP               0x1       

/** Generic level get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LEVEL                     0x2       

/** Generic power level get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_POWER_LEVEL               0x3       

/** Generic power level last get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_POWER_LEVEL_LAST          0x4       

/** Generic power level default get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_POWER_LEVEL_DEFAULT       0x5       

/** Generic power level range get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_POWER_LEVEL_RANGE         0x6       

/** Generic transition time get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_TRANSITION_TIME           0x6       

/** Generic battery get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_BATTERY                   0x8       

/** Generic global location get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LOCATION_GLOBAL           0x9       

/** Generic local location get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LOCATION_LOCAL            0xa       

/** Generic user property get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_USER             0xb       

/** Generic admin property get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_ADMIN            0xc       

/** Generic manufacturer property get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_MANUF            0xd       

/** Generic user property list get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_LIST_USER        0xe       

/** Generic admin property list get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_LIST_ADMIN       0xf       

/** Generic manufacturer property list get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_LIST_MANUF       0x10      

/** Generic client property list get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_PROPERTY_LIST_CLIENT      0x11      

/** Light actual lightness get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LIGHTNESS_ACTUAL          0x80      

/** Light linear lightness get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LIGHTNESS_LINEAR          0x81      

/** Light last lightness get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LIGHTNESS_LAST            0x82      

/** Light default lightness get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LIGHTNESS_DEFAULT         0x83      

/** Light lightness range get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_LIGHTNESS_RANGE           0x84      

/** Light lightness, color temperature, and delta UV server state identifier.
 * Not to be used by client get requests. */
#define SL_BTMESH_GENERIC_CLIENT_STATE_CTL                       0x85      

/** Light color temperature and delta UV get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_CTL_TEMPERATURE           0x86      

/** Light lightness, color temperature, and delta UV default get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_CTL_DEFAULT               0x87      

/** Light color temperature range get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_CTL_RANGE                 0x88      

/** Light lightness and color temperature get request. */
#define SL_BTMESH_GENERIC_CLIENT_STATE_CTL_LIGHTNESS_TEMPERATURE 0x89      

/** Light lightness, color hue, and color saturation current value get request.
 * */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL                       0x8a      

/** Light color hue get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL_HUE                   0x8b      

/** Light color saturation get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL_SATURATION            0x8c      

/** Light lightness, color hue, and color saturation default get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL_DEFAULT               0x8d      

/** Light color hue and saturation range get request */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL_RANGE                 0x8e      

/** Light lightness, color hue, and color saturation target value get request.
 * */
#define SL_BTMESH_GENERIC_CLIENT_STATE_HSL_TARGET                0x8f      

/** @} */ // end Generic Client Get State Types

/**
 * @addtogroup sl_btmesh_generic_client_set_request_type Generic Client Set Request Types
 * @{
 *
 * Generic client set request type identifies the state which the client
 * requests to be set to a new value on the remote server model.
 */

/** Generic on/off set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_ON_OFF              0x0       

/** Generic on power up set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_ON_POWER_UP         0x1       

/** Generic level set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LEVEL               0x2       

/** Generic level delta set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LEVEL_DELTA         0x3       

/** Generic level move set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LEVEL_MOVE          0x4       

/** Generic level halt request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LEVEL_HALT          0x5       

/** Generic power level set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_POWER_LEVEL         0x6       

/** Generic power level default set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_POWER_LEVEL_DEFAULT 0x7       

/** Generic power level range set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_POWER_LEVEL_RANGE   0x8       

/** Generic transition time set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_TRANSITION_TIME     0x9       

/** Generic global location set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LOCATION_GLOBAL     0xa       

/** Generic local location set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LOCATION_LOCAL      0xb       

/** Generic user property set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_PROPERTY_USER       0xc       

/** Generic admin property set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_PROPERTY_ADMIN      0xd       

/** Generic manufacturer property set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_PROPERTY_MANUF      0xe       

/** Light actual lightness set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LIGHTNESS_ACTUAL    0x80      

/** Light linear lightness set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LIGHTNESS_LINEAR    0x81      

/** Light default lightness set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LIGHTNESS_DEFAULT   0x82      

/** Light lightness range set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_LIGHTNESS_RANGE     0x83      

/** Light lightness, color temperature, and delta UV set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_CTL                 0x84      

/** Light color temperature and delta UV set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_CTL_TEMPERATURE     0x85      

/** Light lightness, color temperature, and delta UV default set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_CTL_DEFAULT         0x86      

/** Light color temperature range set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_CTL_RANGE           0x87      

/** Light lightness, color hue, and color saturation set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_HSL                 0x88      

/** Light color hue set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_HSL_HUE             0x89      

/** Light color saturation set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_HSL_SATURATION      0x8a      

/** Light lightness, color hue, and color saturation default set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_HSL_DEFAULT         0x8b      

/** Light color hue and color saturation range set request */
#define SL_BTMESH_GENERIC_CLIENT_REQUEST_HSL_RANGE           0x8c      

/** @} */ // end Generic Client Set Request Types

/**
 * @addtogroup sl_btmesh_evt_generic_client_server_status sl_btmesh_evt_generic_client_server_status
 * @{
 * @brief Status report sent by a server model
 *
 * This may be generated either because of a response to a get or set request
 * was received by the client model or because the client model received a
 * spontaneously generated status indication sent to an address the model was
 * subscribed to.
 */

/** @brief Identifier of the server_status event */
#define sl_btmesh_evt_generic_client_server_status_id                    0x001e00a8

/***************************************************************************//**
 * @brief Data structure of the server_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_generic_client_server_status_s
{
  uint16_t   client_address; /**< Address that the message was sent to, which
                                  can be either the model element's unicast
                                  address or a subscription address of the model */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   model_id;       /**< Client model ID */
  uint16_t   server_address; /**< Address of the server model which sent the
                                  message */
  uint32_t   remaining_ms;   /**< Time (in milliseconds) remaining before
                                  transition from the current state to target
                                  state is complete. Set to zero if no
                                  transition is taking place or if transition
                                  time does not apply to the message. */
  uint16_t   flags;          /**< Message flags. It is a bitmask of the
                                  following values:
                                    - <b>Bit 0:</b> Non-relayed. If non-zero,
                                      indicates a response to a non-relayed
                                      request. */
  uint8_t    type;           /**< Model-specific state type, identifying the
                                  kind of state reported in the status event.
                                  See get state types list for details. */
  uint8array parameters;     /**< Message-specific parameters, serialized into a
                                  byte array */
});

typedef struct sl_btmesh_evt_generic_client_server_status_s sl_btmesh_evt_generic_client_server_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_generic_client_server_status

/***************************************************************************//**
 *
 * Get the current state of a server model or models in the network. Besides the
 * immediate result code, the response or responses from the network will
 * generate server state report events for the replies received.
 *
 * The server model responses will be reported in @ref
 * sl_btmesh_evt_generic_client_server_status events.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] model_id Client model ID
 * @param[in] appkey_index The application key index to use
 * @param[in] type Model-specific state type, identifying the kind of state to
 *   retrieve. See get state types list for details.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_generic_client_server_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_get(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t model_id,
                                         uint16_t appkey_index,
                                         uint8_t type);

/***************************************************************************//**
 *
 * Set the current state of a server model or models in the network. Besides the
 * immediate result code, the response or responses from the network will
 * generate server state report events for the replies received.
 *
 * The server model responses will be reported in @ref
 * sl_btmesh_evt_generic_client_server_status events. Note that for responses to
 * be generated the corresponding flag needs to be set.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] model_id Client model ID
 * @param[in] appkey_index The application key index to use
 * @param[in] tid Transaction identifier. This applies to those messages the
 *   Mesh Model specification defines as transactional and can be left as zero
 *   for others.
 * @param[in] transition_ms @parblock
 *   Transition time (in milliseconds) for the state change. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 *   This applies to messages the Mesh Model specification defines to have
 *   transition and delay times and can be left as zero for others.
 *   @endparblock
 * @param[in] delay_ms @parblock
 *   Delay time (in milliseconds) before starting the state change. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 *   This applies to messages the Mesh Model specification defines to have
 *   transition and delay times and can be left as zero for others.
 *   @endparblock
 * @param[in] flags Message flags. Bitmask of the following:
 *     - <b>Bit 0:</b> Response required. If non-zero, the client expects a
 *       response from the server
 *     - <b>Bit 1:</b> Default transition timer. If non-zero, the client
 *       requests that server uses its default transition timer and the supplied
 *       transition and delay values are ignored.
 * @param[in] type Model-specific request type. See set request types list for
 *   details.
 * @param[in] parameters_len Length of data in @p parameters
 * @param[in] parameters Message-specific set request parameters serialized into
 *   a byte array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_generic_client_server_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_set(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t model_id,
                                         uint16_t appkey_index,
                                         uint8_t tid,
                                         uint32_t transition_ms,
                                         uint16_t delay_ms,
                                         uint16_t flags,
                                         uint8_t type,
                                         size_t parameters_len,
                                         const uint8_t* parameters);

/***************************************************************************//**
 *
 * Publish a set request to the network using the publish address and publish
 * application key of the model. The message will be received by the server
 * models which subscribe to the publish address, and there's no need to
 * explicitly specify a destination address or application key.
 *
 * The server model responses will be reported in @ref
 * sl_btmesh_evt_generic_client_server_status events. To generate responses, the
 * corresponding flag needs to be set.
 *
 * @param[in] elem_index Client model element index
 * @param[in] model_id Client model ID
 * @param[in] tid Transaction identifier
 * @param[in] transition_ms @parblock
 *   Transition time (in milliseconds) for the state change. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 *   This applies to messages the Mesh Model specification defines to have
 *   transition and delay times and can be left as zero for others.
 *   @endparblock
 * @param[in] delay_ms @parblock
 *   Delay time (in milliseconds) before starting the state change. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 *   This applies to messages the Mesh Model specification defines to have
 *   transition and delay times, and can be left as zero for others.
 *   @endparblock
 * @param[in] flags Message flags. Bitmask of the following:
 *     - <b>Bit 0:</b> Response required. If non-zero, the client expects a
 *       response from the server
 *     - <b>Bit 1:</b> Default transition timer. If non-zero client requests
 *       that server uses its default transition timer and the supplied
 *       transition and delay values are ignored.
 * @param[in] type Model-specific request type. See set request types list for
 *   details.
 * @param[in] parameters_len Length of data in @p parameters
 * @param[in] parameters Message-specific set request parameters serialized into
 *   a byte array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_generic_client_server_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_publish(uint16_t elem_index,
                                             uint16_t model_id,
                                             uint8_t tid,
                                             uint32_t transition_ms,
                                             uint16_t delay_ms,
                                             uint16_t flags,
                                             uint8_t type,
                                             size_t parameters_len,
                                             const uint8_t* parameters);

/***************************************************************************//**
 *
 * Get the current state of a server model or models in the network, with
 * additional parameters detailing the request. Besides the immediate result
 * code, the response or responses from the network will generate server state
 * report events for the replies received.
 *
 * The server model responses will be reported in @ref
 * sl_btmesh_evt_generic_client_server_status events.
 *
 * This call is used to query properties, for which the property ID is given as
 * a parameter.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] model_id Client model ID
 * @param[in] appkey_index The application key index to use.
 * @param[in] type Model-specific state type, identifying the kind of state to
 *   retrieve. See get state types list for details.
 * @param[in] parameters_len Length of data in @p parameters
 * @param[in] parameters Message-specific get request parameters serialized into
 *   a byte array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_generic_client_server_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_get_params(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t model_id,
                                                uint16_t appkey_index,
                                                uint8_t type,
                                                size_t parameters_len,
                                                const uint8_t* parameters);

/***************************************************************************//**
 *
 * Initialize generic client models. This command initializes all generic client
 * models on the device. Alternatively, only the necessary client models can be
 * initialized using model-specific initialization commands. Using
 * model-specific initialization can result in a smaller firmware image size for
 * SoC projects.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init();

/***************************************************************************//**
 *
 * Initialize the generic client model common functionality. This command should
 * be called after all model-specific initialization calls are done. It does not
 * need to be called if @ref sl_btmesh_generic_client_init is used.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_common();

/***************************************************************************//**
 *
 * Initialize generic on/off client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_on_off();

/***************************************************************************//**
 *
 * Initialize generic level client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_level();

/***************************************************************************//**
 *
 * Initialize generic default transition time client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_default_transition_time();

/***************************************************************************//**
 *
 * Initialize generic power on/off client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_power_on_off();

/***************************************************************************//**
 *
 * Initialize generic power level client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_power_level();

/***************************************************************************//**
 *
 * Initialize generic battery client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_battery();

/***************************************************************************//**
 *
 * Initialize generic location client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_location();

/***************************************************************************//**
 *
 * Initialize generic property client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_property();

/***************************************************************************//**
 *
 * Initialize light lightness client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_lightness();

/***************************************************************************//**
 *
 * Initialize light CTL client models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_ctl();

/***************************************************************************//**
 *
 * Initialize light HSL client models
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_client_init_hsl();

/** @} */ // end addtogroup sl_btmesh_generic_client

/**
 * @addtogroup sl_btmesh_generic_server Bluetooth Mesh Generic Server Model
 * @{
 *
 * @brief Bluetooth Mesh Generic Server Model
 *
 * Generic server model API provides functionality to send and receive messages
 * using Bluetooth SIG server models, including generic server models and
 * lighting server models.
 *
 * Throughout the API the server model being used is identified by its element
 * address and model ID, while the client model generating requests to the
 * server model is identified by its element address and model ID.
 *
 * The generic server model API is designed on the premise that the actual state
 * the model represents resides in and is owned by the application, not by the
 * stack.
 *
 * The model acts as a cache for client queries. In other words, the stack
 * handles state requests from the client automatically. The application does
 * not need to handle those. The cached value is also used for periodic
 * publication.
 *
 * The flip side of caching is that when the state represented by the model
 * changes in the application, it must update the cached value to the stack by
 * issuing a @ref sl_btmesh_generic_server_update command.
 *
 * When a client model requests a state change, the stack will generate a @ref
 * sl_btmesh_evt_generic_server_client_request event which the application must
 * process. Then, if the client needs a response the application has to issue a
 * @ref sl_btmesh_generic_server_respond command corresponding to the request.
 * Otherwise, the application only has to update the state with a @ref
 * sl_btmesh_generic_server_update command, which does not result in sending any
 * messages to the network.
 *
 * Note that, because the Mesh Model specification requires that certain states
 * are bound together and because the stack enforces that, updating one cached
 * state may result in an update of the corresponding bound state, for which the
 * stack generates a @ref sl_btmesh_evt_generic_server_state_changed event. For
 * example, when a dimmable light is switched off, the lightness level bound to
 * the on/off state, is also set to zero because the states are bound.
 *
 * Data for state change requests and server responses is passed as serialized
 * byte arrays through BGAPI. Bluetooth mesh SDK has functions to convert byte
 * arrays to and from model state structures.
 *
 * The stack will handle Mesh transaction layer segmentation and reassembly
 * automatically if the messages sent are long enough to require it.
 *
 * <b>Note on time resolution</b>
 *
 * Because of message formatting, transition time and remaining time resolution
 * units depend on the requested or reported value. For example, until 6.2
 * seconds it is 100 ms; until 62 seconds it is 1 s; until 620 seconds it is 10
 * s; and until 620 minutes it is 10 minutes. The value can't be longer than 620
 * minutes. Therefore, it is not possible to request a delay of exactly 7500 ms.
 * The resolution unit is 1 s between 6.2 and 62 seconds, so the value is
 * rounded down to 7 s.
 *
 * Delay resolution is 5 ms and values will be rounded down to the closest 5 ms.
 * The value can't be longer than 1275 ms.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_generic_server_respond_id                          0x001f0028
#define sl_btmesh_cmd_generic_server_update_id                           0x011f0028
#define sl_btmesh_cmd_generic_server_publish_id                          0x021f0028
#define sl_btmesh_cmd_generic_server_init_id                             0x041f0028
#define sl_btmesh_cmd_generic_server_init_common_id                      0x051f0028
#define sl_btmesh_cmd_generic_server_init_on_off_id                      0x061f0028
#define sl_btmesh_cmd_generic_server_init_level_id                       0x071f0028
#define sl_btmesh_cmd_generic_server_init_default_transition_time_id     0x081f0028
#define sl_btmesh_cmd_generic_server_init_power_on_off_id                0x091f0028
#define sl_btmesh_cmd_generic_server_init_power_level_id                 0x0a1f0028
#define sl_btmesh_cmd_generic_server_init_battery_id                     0x0b1f0028
#define sl_btmesh_cmd_generic_server_init_location_id                    0x0c1f0028
#define sl_btmesh_cmd_generic_server_init_property_id                    0x0d1f0028
#define sl_btmesh_cmd_generic_server_init_lightness_id                   0x0e1f0028
#define sl_btmesh_cmd_generic_server_init_ctl_id                         0x0f1f0028
#define sl_btmesh_cmd_generic_server_init_hsl_id                         0x101f0028
#define sl_btmesh_cmd_generic_server_get_cached_state_id                 0x111f0028
#define sl_btmesh_rsp_generic_server_respond_id                          0x001f0028
#define sl_btmesh_rsp_generic_server_update_id                           0x011f0028
#define sl_btmesh_rsp_generic_server_publish_id                          0x021f0028
#define sl_btmesh_rsp_generic_server_init_id                             0x041f0028
#define sl_btmesh_rsp_generic_server_init_common_id                      0x051f0028
#define sl_btmesh_rsp_generic_server_init_on_off_id                      0x061f0028
#define sl_btmesh_rsp_generic_server_init_level_id                       0x071f0028
#define sl_btmesh_rsp_generic_server_init_default_transition_time_id     0x081f0028
#define sl_btmesh_rsp_generic_server_init_power_on_off_id                0x091f0028
#define sl_btmesh_rsp_generic_server_init_power_level_id                 0x0a1f0028
#define sl_btmesh_rsp_generic_server_init_battery_id                     0x0b1f0028
#define sl_btmesh_rsp_generic_server_init_location_id                    0x0c1f0028
#define sl_btmesh_rsp_generic_server_init_property_id                    0x0d1f0028
#define sl_btmesh_rsp_generic_server_init_lightness_id                   0x0e1f0028
#define sl_btmesh_rsp_generic_server_init_ctl_id                         0x0f1f0028
#define sl_btmesh_rsp_generic_server_init_hsl_id                         0x101f0028
#define sl_btmesh_rsp_generic_server_get_cached_state_id                 0x111f0028

/**
 * @addtogroup sl_btmesh_evt_generic_server_client_request sl_btmesh_evt_generic_server_client_request
 * @{
 * @brief State change request sent by a client model
 *
 * This may be generated either because of a request directly to this model, or
 * a request sent to an address which is subscribed to by the model.
 */

/** @brief Identifier of the client_request event */
#define sl_btmesh_evt_generic_server_client_request_id                   0x001f00a8

/***************************************************************************//**
 * @brief Data structure of the client_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_generic_server_client_request_s
{
  uint16_t   server_address; /**< Address the message was sent to, which can be
                                  either the model element's unicast address, or
                                  model's subscription address */
  uint16_t   elem_index;     /**< Server model element index */
  uint16_t   model_id;       /**< Server model ID */
  uint16_t   client_address; /**< Address of the client model which sent the
                                  message */
  uint16_t   appkey_index;   /**< The application key index used in encrypting
                                  the request; Responses need to be encrypted
                                  with the same key. */
  uint32_t   transition_ms;  /**< Requested transition time (in milliseconds)
                                  for the state change. If both the transition
                                  time and the delay are zero, the transition is
                                  immediate.

                                  This applies to messages, which the Mesh Model
                                  specification defines to have transition and
                                  delay times and will be zero for others. */
  uint16_t   delay_ms;       /**< Delay time (in milliseconds) before starting
                                  the state change. If both the transition time
                                  and the delay are zero, the transition is
                                  immediate.

                                  This applies to messages, which the Mesh Model
                                  specification defines to have transition and
                                  delay times and will be zero for others. */
  uint16_t   flags;          /**< Message flags. Bitmask of the following
                                  values:
                                    - <b>Bit 0:</b> Non-relayed. If non-zero,
                                      indicates that the client message was not
                                      relayed (TTL was zero) and that the server
                                      is within direct radio range of the
                                      client.
                                    - <b>Bit 1:</b> Response required. If
                                      non-zero, the client expects a response
                                      from the server. */
  uint8_t    type;           /**< Model-specific request type. See set request
                                  types list for details. */
  uint8array parameters;     /**< Message-specific parameters serialized into a
                                  byte array */
});

typedef struct sl_btmesh_evt_generic_server_client_request_s sl_btmesh_evt_generic_server_client_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_generic_server_client_request

/**
 * @addtogroup sl_btmesh_evt_generic_server_state_changed sl_btmesh_evt_generic_server_state_changed
 * @{
 * @brief Cached model state changed
 *
 * This may happen either as a direct result of model state update by the
 * application, in which case the event can be ignored, or because the update of
 * one model state resulted in an update of a bound model state according to the
 * Mesh model specification. In this case, the application should update its own
 * value accordingly.
 */

/** @brief Identifier of the state_changed event */
#define sl_btmesh_evt_generic_server_state_changed_id                    0x011f00a8

/***************************************************************************//**
 * @brief Data structure of the state_changed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_generic_server_state_changed_s
{
  uint16_t   elem_index;   /**< Server model element index */
  uint16_t   model_id;     /**< Server model ID */
  uint32_t   remaining_ms; /**< Time (in milliseconds) remaining before
                                transition from current state to target state is
                                complete. Ignored if no transition is taking
                                place. */
  uint8_t    type;         /**< Model-specific state type, identifying the kind
                                of state reported in the state change event. See
                                get state types list for details. */
  uint8array parameters;   /**< Message-specific parameters, serialized into a
                                byte array */
});

typedef struct sl_btmesh_evt_generic_server_state_changed_s sl_btmesh_evt_generic_server_state_changed_t;

/** @} */ // end addtogroup sl_btmesh_evt_generic_server_state_changed

/**
 * @addtogroup sl_btmesh_evt_generic_server_state_recall sl_btmesh_evt_generic_server_state_recall
 * @{
 * @brief Cached model state changed due to scene recall operation
 */

/** @brief Identifier of the state_recall event */
#define sl_btmesh_evt_generic_server_state_recall_id                     0x021f00a8

/***************************************************************************//**
 * @brief Data structure of the state_recall event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_generic_server_state_recall_s
{
  uint16_t   elem_index;         /**< Server model element index */
  uint16_t   model_id;           /**< Server model ID */
  uint32_t   transition_time_ms; /**< Time (in milliseconds) remaining before
                                      transition from current state to target
                                      state should be complete. Ignored if no
                                      transition is taking place. */
  uint8_t    type;               /**< Model-specific state type, identifying the
                                      kind of state reported in the state change
                                      event. See get state types list for
                                      details. */
  uint8array parameters;         /**< Model state - specific parameters,
                                      serialized into a byte array */
});

typedef struct sl_btmesh_evt_generic_server_state_recall_s sl_btmesh_evt_generic_server_state_recall_t;

/** @} */ // end addtogroup sl_btmesh_evt_generic_server_state_recall

/***************************************************************************//**
 *
 * Server response to a client request. This command must be used when an
 * application updates the server model state as a response to a @ref
 * sl_btmesh_evt_generic_server_client_request event which required a response.
 *
 * @param[in] client_address Address of the client model which sent the message
 * @param[in] elem_index Server model element index
 * @param[in] model_id Server model ID
 * @param[in] appkey_index The application key index used
 * @param[in] remaining_ms Time (in milliseconds) remaining before transition
 *   from current state to target state is complete. Set to zero if no
 *   transition is taking place or if the transition time does not apply to the
 *   state change.
 * @param[in] flags Message flags. Bitmask of the following:
 *     - <b>Bit 0:</b> Non-relayed. If non-zero, indicates a response to a
 *       non-relayed request.
 * @param[in] type Model-specific state type, identifying the kind of state to
 *   be updated. See get state types list for details.
 * @param[in] parameters_len Length of data in @p parameters
 * @param[in] parameters Message-specific parameters serialized into a byte
 *   array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_respond(uint16_t client_address,
                                             uint16_t elem_index,
                                             uint16_t model_id,
                                             uint16_t appkey_index,
                                             uint32_t remaining_ms,
                                             uint16_t flags,
                                             uint8_t type,
                                             size_t parameters_len,
                                             const uint8_t* parameters);

/***************************************************************************//**
 *
 * Server state update. This command must be used when an application updates
 * the server model state as a response to a @ref
 * sl_btmesh_evt_generic_server_client_request event which did not require a
 * response, but also when the application state changes spontaneously or as a
 * result of some external (non-Mesh) event.
 *
 * @param[in] elem_index Server model element index
 * @param[in] model_id Server model ID
 * @param[in] remaining_ms Time (in milliseconds) remaining before transition
 *   from current state to target state is complete. Set to zero if no
 *   transition is taking place or if transition time does not apply to the
 *   state change.
 * @param[in] type Model-specific state type, identifying the kind of state to
 *   be updated. See get state types list for details.
 * @param[in] parameters_len Length of data in @p parameters
 * @param[in] parameters Message-specific parameters, serialized into a byte
 *   array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_update(uint16_t elem_index,
                                            uint16_t model_id,
                                            uint32_t remaining_ms,
                                            uint8_t type,
                                            size_t parameters_len,
                                            const uint8_t* parameters);

/***************************************************************************//**
 *
 * Publish the server state to the network using the publish parameters
 * configured in the model. The message is constructed using the cached state in
 * the stack.
 *
 * @param[in] elem_index Server model element index
 * @param[in] model_id Server model ID
 * @param[in] type Model-specific state type, identifying the kind of state used
 *   in the published message. See get state types list for details.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_publish(uint16_t elem_index,
                                             uint16_t model_id,
                                             uint8_t type);

/***************************************************************************//**
 *
 * Initialize generic server models. This command initializes all generic server
 * models on the device. Alternatively, only the necessary server models can be
 * initialized using model-specific initialization commands. Using
 * model-specific initialization can result in a smaller firmware image size for
 * SoC projects.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init();

/***************************************************************************//**
 *
 * Initialize the generic server model common functionality. This should be
 * called after all model-specific initialization calls are done, and does not
 * need to be called if @ref sl_btmesh_generic_server_init is used.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_common();

/***************************************************************************//**
 *
 * Initialize generic on/off server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_on_off();

/***************************************************************************//**
 *
 * Initialize generic level server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_level();

/***************************************************************************//**
 *
 * Initialize generic default transition time server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_default_transition_time();

/***************************************************************************//**
 *
 * Initialize generic power on/off server models, power on/off setup server
 * models, and all models they extend.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_power_on_off();

/***************************************************************************//**
 *
 * Initialize generic power level server models, power level setup server
 * models, and all models they extend.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_power_level();

/***************************************************************************//**
 *
 * Initialize generic battery server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_battery();

/***************************************************************************//**
 *
 * Initialize generic location and generic location setup server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_location();

/***************************************************************************//**
 *
 * Initialize generic property server models.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_property();

/***************************************************************************//**
 *
 * Initialize light lightness server models, light lightness setup server
 * models, and all models they extend.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_lightness();

/***************************************************************************//**
 *
 * Initialize light CTL server models, light CTL temperature server models,
 * light CTL setup server models, and all models they extend.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_ctl();

/***************************************************************************//**
 *
 * Initialize light HSL server models, light HSL hue server models, light HSL
 * saturation server models, light HSL setup server models, and all models they
 * extend
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_init_hsl();

/***************************************************************************//**
 *
 * Get model cached state. This command can be used to get cached model states
 * after scene recall when using compacted recall events. This command supports
 * only those states that would have been reported by @ref
 * sl_btmesh_evt_generic_server_state_recall events.
 *
 * @param[in] elem_index Server model element index
 * @param[in] model_id Server model ID
 * @param[in] type Model-specific state type, identifying the kind of state
 *   reported in the state change event. See get state types list for details.
 * @param[out] remaining_ms Time (in milliseconds) remaining before transition
 *   from current state to target state is complete. Ignored if no transition is
 *   taking place.
 * @param[in] max_parameters_size Size of output buffer passed in @p parameters
 * @param[out] parameters_len On return, set to the length of output data
 *   written to @p parameters
 * @param[out] parameters Message-specific parameters, serialized into a byte
 *   array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_generic_server_get_cached_state(uint16_t elem_index,
                                                      uint16_t model_id,
                                                      uint8_t type,
                                                      uint32_t *remaining_ms,
                                                      size_t max_parameters_size,
                                                      size_t *parameters_len,
                                                      uint8_t *parameters);

/** @} */ // end addtogroup sl_btmesh_generic_server

/**
 * @addtogroup sl_btmesh_test Bluetooth Mesh Test Utilities
 * @{
 *
 * @brief Bluetooth Mesh Test Utilities
 *
 * These commands are meant for development and testing. Do not use in
 * production software.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_test_get_nettx_id                                  0x00220028
#define sl_btmesh_cmd_test_set_nettx_id                                  0x01220028
#define sl_btmesh_cmd_test_get_relay_id                                  0x02220028
#define sl_btmesh_cmd_test_set_relay_id                                  0x03220028
#define sl_btmesh_cmd_test_set_adv_scan_params_id                        0x04220028
#define sl_btmesh_cmd_test_set_ivupdate_test_mode_id                     0x05220028
#define sl_btmesh_cmd_test_get_ivupdate_test_mode_id                     0x06220028
#define sl_btmesh_cmd_test_set_segment_send_delay_id                     0x07220028
#define sl_btmesh_cmd_test_set_ivupdate_state_id                         0x08220028
#define sl_btmesh_cmd_test_send_beacons_id                               0x09220028
#define sl_btmesh_cmd_test_bind_local_model_app_id                       0x0a220028
#define sl_btmesh_cmd_test_unbind_local_model_app_id                     0x0b220028
#define sl_btmesh_cmd_test_add_local_model_sub_id                        0x0c220028
#define sl_btmesh_cmd_test_remove_local_model_sub_id                     0x0d220028
#define sl_btmesh_cmd_test_add_local_model_sub_va_id                     0x0e220028
#define sl_btmesh_cmd_test_remove_local_model_sub_va_id                  0x0f220028
#define sl_btmesh_cmd_test_get_local_model_sub_id                        0x10220028
#define sl_btmesh_cmd_test_set_local_model_pub_id                        0x11220028
#define sl_btmesh_cmd_test_set_local_model_pub_va_id                     0x12220028
#define sl_btmesh_cmd_test_get_local_model_pub_id                        0x13220028
#define sl_btmesh_cmd_test_set_local_heartbeat_subscription_id           0x14220028
#define sl_btmesh_cmd_test_get_local_heartbeat_subscription_id           0x15220028
#define sl_btmesh_cmd_test_get_local_heartbeat_publication_id            0x16220028
#define sl_btmesh_cmd_test_set_local_heartbeat_publication_id            0x17220028
#define sl_btmesh_cmd_test_set_local_config_id                           0x18220028
#define sl_btmesh_cmd_test_get_local_config_id                           0x19220028
#define sl_btmesh_cmd_test_add_local_key_id                              0x1a220028
#define sl_btmesh_cmd_test_remove_local_key_id                           0x1b220028
#define sl_btmesh_cmd_test_update_local_key_id                           0x1c220028
#define sl_btmesh_cmd_test_set_sar_config_id                             0x1d220028
#define sl_btmesh_cmd_test_set_adv_bearer_state_id                       0x1f220028
#define sl_btmesh_cmd_test_prov_get_device_key_id                        0x23220028
#define sl_btmesh_cmd_test_prov_prepare_key_refresh_id                   0x24220028
#define sl_btmesh_cmd_test_cancel_segmented_tx_id                        0x25220028
#define sl_btmesh_cmd_test_set_iv_index_id                               0x26220028
#define sl_btmesh_cmd_test_set_element_seqnum_id                         0x27220028
#define sl_btmesh_cmd_test_set_model_option_id                           0x28220028
#define sl_btmesh_cmd_test_get_local_model_app_bindings_id               0x29220028
#define sl_btmesh_cmd_test_get_replay_protection_list_entry_id           0x2a220028
#define sl_btmesh_cmd_test_clear_replay_protection_list_entry_id         0x2b220028
#define sl_btmesh_cmd_test_set_replay_protection_list_diagnostics_id     0x2c220028
#define sl_btmesh_cmd_test_get_model_option_id                           0x2d220028
#define sl_btmesh_cmd_test_get_default_ttl_id                            0x2e220028
#define sl_btmesh_cmd_test_set_default_ttl_id                            0x2f220028
#define sl_btmesh_cmd_test_get_gatt_proxy_id                             0x30220028
#define sl_btmesh_cmd_test_set_gatt_proxy_id                             0x31220028
#define sl_btmesh_cmd_test_get_identity_id                               0x32220028
#define sl_btmesh_cmd_test_set_identity_id                               0x33220028
#define sl_btmesh_cmd_test_get_friend_id                                 0x34220028
#define sl_btmesh_cmd_test_set_friend_id                                 0x35220028
#define sl_btmesh_cmd_test_get_beacon_id                                 0x36220028
#define sl_btmesh_cmd_test_set_beacon_id                                 0x37220028
#define sl_btmesh_rsp_test_get_nettx_id                                  0x00220028
#define sl_btmesh_rsp_test_set_nettx_id                                  0x01220028
#define sl_btmesh_rsp_test_get_relay_id                                  0x02220028
#define sl_btmesh_rsp_test_set_relay_id                                  0x03220028
#define sl_btmesh_rsp_test_set_adv_scan_params_id                        0x04220028
#define sl_btmesh_rsp_test_set_ivupdate_test_mode_id                     0x05220028
#define sl_btmesh_rsp_test_get_ivupdate_test_mode_id                     0x06220028
#define sl_btmesh_rsp_test_set_segment_send_delay_id                     0x07220028
#define sl_btmesh_rsp_test_set_ivupdate_state_id                         0x08220028
#define sl_btmesh_rsp_test_send_beacons_id                               0x09220028
#define sl_btmesh_rsp_test_bind_local_model_app_id                       0x0a220028
#define sl_btmesh_rsp_test_unbind_local_model_app_id                     0x0b220028
#define sl_btmesh_rsp_test_add_local_model_sub_id                        0x0c220028
#define sl_btmesh_rsp_test_remove_local_model_sub_id                     0x0d220028
#define sl_btmesh_rsp_test_add_local_model_sub_va_id                     0x0e220028
#define sl_btmesh_rsp_test_remove_local_model_sub_va_id                  0x0f220028
#define sl_btmesh_rsp_test_get_local_model_sub_id                        0x10220028
#define sl_btmesh_rsp_test_set_local_model_pub_id                        0x11220028
#define sl_btmesh_rsp_test_set_local_model_pub_va_id                     0x12220028
#define sl_btmesh_rsp_test_get_local_model_pub_id                        0x13220028
#define sl_btmesh_rsp_test_set_local_heartbeat_subscription_id           0x14220028
#define sl_btmesh_rsp_test_get_local_heartbeat_subscription_id           0x15220028
#define sl_btmesh_rsp_test_get_local_heartbeat_publication_id            0x16220028
#define sl_btmesh_rsp_test_set_local_heartbeat_publication_id            0x17220028
#define sl_btmesh_rsp_test_set_local_config_id                           0x18220028
#define sl_btmesh_rsp_test_get_local_config_id                           0x19220028
#define sl_btmesh_rsp_test_add_local_key_id                              0x1a220028
#define sl_btmesh_rsp_test_remove_local_key_id                           0x1b220028
#define sl_btmesh_rsp_test_update_local_key_id                           0x1c220028
#define sl_btmesh_rsp_test_set_sar_config_id                             0x1d220028
#define sl_btmesh_rsp_test_set_adv_bearer_state_id                       0x1f220028
#define sl_btmesh_rsp_test_prov_get_device_key_id                        0x23220028
#define sl_btmesh_rsp_test_prov_prepare_key_refresh_id                   0x24220028
#define sl_btmesh_rsp_test_cancel_segmented_tx_id                        0x25220028
#define sl_btmesh_rsp_test_set_iv_index_id                               0x26220028
#define sl_btmesh_rsp_test_set_element_seqnum_id                         0x27220028
#define sl_btmesh_rsp_test_set_model_option_id                           0x28220028
#define sl_btmesh_rsp_test_get_local_model_app_bindings_id               0x29220028
#define sl_btmesh_rsp_test_get_replay_protection_list_entry_id           0x2a220028
#define sl_btmesh_rsp_test_clear_replay_protection_list_entry_id         0x2b220028
#define sl_btmesh_rsp_test_set_replay_protection_list_diagnostics_id     0x2c220028
#define sl_btmesh_rsp_test_get_model_option_id                           0x2d220028
#define sl_btmesh_rsp_test_get_default_ttl_id                            0x2e220028
#define sl_btmesh_rsp_test_set_default_ttl_id                            0x2f220028
#define sl_btmesh_rsp_test_get_gatt_proxy_id                             0x30220028
#define sl_btmesh_rsp_test_set_gatt_proxy_id                             0x31220028
#define sl_btmesh_rsp_test_get_identity_id                               0x32220028
#define sl_btmesh_rsp_test_set_identity_id                               0x33220028
#define sl_btmesh_rsp_test_get_friend_id                                 0x34220028
#define sl_btmesh_rsp_test_set_friend_id                                 0x35220028
#define sl_btmesh_rsp_test_get_beacon_id                                 0x36220028
#define sl_btmesh_rsp_test_set_beacon_id                                 0x37220028

/**
 * @brief Specify the type of a key in key manipulation commands.
 */
typedef enum
{
  sl_btmesh_test_key_type_net = 0x0, /**< (0x0) Network key */
  sl_btmesh_test_key_type_app = 0x1  /**< (0x1) Application key */
} sl_btmesh_test_key_type_t;

/**
 * @addtogroup sl_btmesh_evt_test_local_heartbeat_subscription_complete sl_btmesh_evt_test_local_heartbeat_subscription_complete
 * @{
 * @brief Indicate that the heartbeat subscription period is over.
 */

/** @brief Identifier of the local_heartbeat_subscription_complete event */
#define sl_btmesh_evt_test_local_heartbeat_subscription_complete_id      0x002200a8

/***************************************************************************//**
 * @brief Data structure of the local_heartbeat_subscription_complete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_test_local_heartbeat_subscription_complete_s
{
  uint16_t count;   /**< Number of received heartbeat messages */
  uint8_t  hop_min; /**< Minimum observed hop count in heartbeat messages */
  uint8_t  hop_max; /**< Maximum observed hop count in heartbeat messages */
});

typedef struct sl_btmesh_evt_test_local_heartbeat_subscription_complete_s sl_btmesh_evt_test_local_heartbeat_subscription_complete_t;

/** @} */ // end addtogroup sl_btmesh_evt_test_local_heartbeat_subscription_complete

/**
 * @addtogroup sl_btmesh_evt_test_replay_protection_list_entry_set sl_btmesh_evt_test_replay_protection_list_entry_set
 * @{
 * @brief Indication that a replay protection list entry has been set
 */

/** @brief Identifier of the replay_protection_list_entry_set event */
#define sl_btmesh_evt_test_replay_protection_list_entry_set_id           0x012200a8

/***************************************************************************//**
 * @brief Data structure of the replay_protection_list_entry_set event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_test_replay_protection_list_entry_set_s
{
  uint16_t address; /**< Source address for the replay protection list entry */
  uint8_t  cancel;  /**< Nonzero when replay protection list update relates to a
                         cancelled segmented reception */
});

typedef struct sl_btmesh_evt_test_replay_protection_list_entry_set_s sl_btmesh_evt_test_replay_protection_list_entry_set_t;

/** @} */ // end addtogroup sl_btmesh_evt_test_replay_protection_list_entry_set

/**
 * @addtogroup sl_btmesh_evt_test_replay_protection_list_entry_cleared sl_btmesh_evt_test_replay_protection_list_entry_cleared
 * @{
 * @brief Indication that a replay protection list entry has been cleared
 */

/** @brief Identifier of the replay_protection_list_entry_cleared event */
#define sl_btmesh_evt_test_replay_protection_list_entry_cleared_id       0x022200a8

/***************************************************************************//**
 * @brief Data structure of the replay_protection_list_entry_cleared event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_test_replay_protection_list_entry_cleared_s
{
  uint16_t address; /**< Source address for the replay protection list entry */
});

typedef struct sl_btmesh_evt_test_replay_protection_list_entry_cleared_s sl_btmesh_evt_test_replay_protection_list_entry_cleared_t;

/** @} */ // end addtogroup sl_btmesh_evt_test_replay_protection_list_entry_cleared

/**
 * @addtogroup sl_btmesh_evt_test_replay_protection_list_saved sl_btmesh_evt_test_replay_protection_list_saved
 * @{
 * @brief Indication that replay protection list has been saved
 */

/** @brief Identifier of the replay_protection_list_saved event */
#define sl_btmesh_evt_test_replay_protection_list_saved_id               0x032200a8

/***************************************************************************//**
 * @brief Data structure of the replay_protection_list_saved event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_test_replay_protection_list_saved_s
{
  uint16_t result;      /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint16_t saved_count; /**< Number of entries saved successfully */
  uint16_t total_count; /**< Number of entries in the list in total */
});

typedef struct sl_btmesh_evt_test_replay_protection_list_saved_s sl_btmesh_evt_test_replay_protection_list_saved_t;

/** @} */ // end addtogroup sl_btmesh_evt_test_replay_protection_list_saved

/**
 * @addtogroup sl_btmesh_evt_test_replay_protection_list_full sl_btmesh_evt_test_replay_protection_list_full
 * @{
 * @brief Indication that replay protection list is full when trying to process
 * a message
 */

/** @brief Identifier of the replay_protection_list_full event */
#define sl_btmesh_evt_test_replay_protection_list_full_id                0x042200a8

/** @} */ // end addtogroup sl_btmesh_evt_test_replay_protection_list_full

/***************************************************************************//**
 *
 * Get the network transmit state of a node.
 *
 * @param[out] count Number of network layer transmissions beyond the initial
 *   one. Range: 0-7.
 * @param[out] interval Transmit interval steps. The interval between
 *   transmissions is a random value between 10*(1+steps) and 10*(2+steps)
 *   milliseconds. For example, for a value of 2, the interval is between 30 and
 *   40 milliseconds. Range: 0-31.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_nettx(uint8_t *count, uint8_t *interval);

/***************************************************************************//**
 *
 * Set the network transmit state of a node locally. Normally, the network
 * transmit state is controlled by the Provisioner. This command overrides any
 * setting done by the Provisioner.
 *
 * @param[in] count Number of network layer transmissions beyond the initial
 *   one. Range: 0-7.
 * @param[in] interval Transmit interval steps. The interval between
 *   transmissions is a random value between 10*(1+steps) and 10*(2+steps)
 *   milliseconds. For example, for a value of 2 the interval would be between
 *   30 and 40 milliseconds. Range: 0-31.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_nettx(uint8_t count, uint8_t interval);

/***************************************************************************//**
 *

 *
 * @param[out] enabled State value indicating whether the relay functionality is
 *   not enabled on the node (0), is enabled on the node (1), or is not
 *   available (2).
 * @param[out] count Number of relay transmissions beyond the initial one.
 *   Range: 0-7.
 * @param[out] interval Relay retransmit interval steps. The interval between
 *   transmissions is 10*(1+steps) milliseconds. Range: 0-31.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_relay(uint8_t *enabled,
                                     uint8_t *count,
                                     uint8_t *interval);

/***************************************************************************//**
 *
 * Set the relay state and the relay retransmit state of a node locally.
 * Normally, these states are controlled by the Provisioner. This command
 * overrides any settings done by the Provisioner.
 *
 * @param[in] enabled Indicates whether the relay functionality is enabled on
 *   the node (1) or not (0); value indicating disabled (2) can't be set.
 * @param[in] count Number of relay transmissions beyond the initial one. Range:
 *   0-7.
 * @param[in] interval Relay retransmit interval steps. The interval between
 *   transmissions is 10*(1+steps) milliseconds. Range: 0-31.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_relay(uint8_t enabled,
                                     uint8_t count,
                                     uint8_t interval);

/***************************************************************************//**
 *
 * Set the non-default advertisement and scanning parameters used in mesh
 * communications. Call this command before @ref sl_btmesh_node_init or @ref
 * sl_btmesh_prov_init for the settings to take effect.
 *
 * @param[in] adv_interval_min Minimum advertisement interval. Value is in units
 *   of 0.625 ms. Default value is 32 (20 ms).
 * @param[in] adv_interval_max Maximum advertisement interval. Value is in units
 *   of 0.625 ms. Must be equal to or greater than the minimum interval. Default
 *   value is 32 (20 ms).
 * @param[in] adv_repeat_packets Number of times to repeat each packet on all
 *   selected advertisement channels. Range: 1-5. Default value is 1.
 * @param[in] adv_use_random_address Bluetooth address type. Range: 0: use
 *   public address, 1: use random address. Default value: 0 (public address).
 * @param[in] adv_channel_map Advertisement channel selection bitmask. Range:
 *   0x1-0x7. Default value: 7 (all channels)
 * @param[in] scan_interval Scan interval. Value is in units of 0.625 ms. Range:
 *   0x0004 to 0x4000 (time range of 2.5 ms to 10.24 s). Default value is 160
 *   (100 ms).
 * @param[in] scan_window Scan window. Value is in units of 0.625 ms. Must be
 *   equal to or less than the scan interval.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_adv_scan_params(uint16_t adv_interval_min,
                                               uint16_t adv_interval_max,
                                               uint8_t adv_repeat_packets,
                                               uint8_t adv_use_random_address,
                                               uint8_t adv_channel_map,
                                               uint16_t scan_interval,
                                               uint16_t scan_window);

/***************************************************************************//**
 *
 * By default, IV index update is limited in how often the update procedure can
 * be performed. This test command can be called to set IV update test mode
 * where any time limits are ignored.
 *
 * @param[in] mode Whether test mode is enabled (1) or disabled (0).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_ivupdate_test_mode(uint8_t mode);

/***************************************************************************//**
 *
 * Get the current IV update test mode. See @ref
 * sl_btmesh_test_set_ivupdate_test_mode for details.
 *
 * @param[out] mode Indicates whether test mode is enabled (1) or disabled (0).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_ivupdate_test_mode(uint8_t *mode);

/***************************************************************************//**
 *
 * Set the delay in milliseconds between sending consecutive segments of a
 * segmented message. The default value is 0. Note that this command needs to be
 * called before @ref sl_btmesh_node_init or @ref sl_btmesh_prov_init for the
 * settings to take effect.
 *
 * @param[in] delay Number of milliseconds to delay each segment after the first
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_segment_send_delay(uint8_t delay);

/***************************************************************************//**
 *
 * Forcefully change the IV update state on the device. Normally, the state
 * changes as a result of an IV index update procedure progressing from one
 * state to the next.
 *
 * @param[in] state Indicates whether IV update state should be entered (1) or
 *   exited (0).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_ivupdate_state(uint8_t state);

/***************************************************************************//**
 *
 * Send secure network beacons for every network key on the device, regardless
 * of beacon configuration state or how many beacons sent by other devices have
 * been observed.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_send_beacons();

/***************************************************************************//**
 *
 * Bind a Model to an Appkey locally.
 *
 * @param[in] elem_index The index of the target Element, 0 is primary element
 * @param[in] appkey_index The Appkey to use for binding
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for SIG
 *   models.
 * @param[in] model_id Model ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_bind_local_model_app(uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint16_t vendor_id,
                                                uint16_t model_id);

/***************************************************************************//**
 *
 * Remove a binding between a model and an application key locally.
 *
 * @param[in] elem_index The index of the target element, 0 is primary element
 * @param[in] appkey_index The Appkey to use for binding
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for SIG
 *   models.
 * @param[in] model_id Model ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_unbind_local_model_app(uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id);

/***************************************************************************//**
 *
 * Add an address to a local model's subscription list.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] sub_address The address to add to the subscription list
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_add_local_model_sub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t sub_address);

/***************************************************************************//**
 *
 * Remove an address from a local model's subscription list.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] sub_address The address to remove from the subscription list
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_remove_local_model_sub(uint16_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address);

/***************************************************************************//**
 *
 * Add a virtual address to a local model's subscription list.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] sub_address_len Length of data in @p sub_address
 * @param[in] sub_address The Label UUID to add to the subscription list. The
 *   array must be exactly 16 bytes long.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_add_local_model_sub_va(uint16_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  size_t sub_address_len,
                                                  const uint8_t* sub_address);

/***************************************************************************//**
 *
 * Remove a virtual address from a local model's subscription list.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] sub_address_len Length of data in @p sub_address
 * @param[in] sub_address The Label UUID to remove from the subscription list.
 *   The array must be exactly 16 bytes long.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_remove_local_model_sub_va(uint16_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     size_t sub_address_len,
                                                     const uint8_t* sub_address);

/***************************************************************************//**
 *
 * Get all entries in a local model's subscription list.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] max_addresses_size Size of output buffer passed in @p addresses
 * @param[out] addresses_len On return, set to the length of output data written
 *   to @p addresses
 * @param[out] addresses List of 16-bit Mesh addresses; empty if not subscribed
 *   to any address. Ignore if the result code is non-zero.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_local_model_sub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               size_t max_addresses_size,
                                               size_t *addresses_len,
                                               uint8_t *addresses);

/***************************************************************************//**
 *
 * Set a local model's publication address, key, and parameters.
 *
 * @param[in] elem_index The index of the target element, where 0 is the primary
 *   element
 * @param[in] appkey_index The application key index to use for the application
 *   messages published
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] pub_address The address to publish to
 * @param[in] ttl Time-to-Live value for published messages
 * @param[in] period Publication period encoded as step count and step
 *   resolution. The encoding is as follows:
 *     - <b>Bits 0..5:</b> Step count
 *     - <b>Bits 6..7:</b> Step resolution:
 *         - 00: 100 milliseconds
 *         - 01: 1 second
 *         - 10: 10 seconds
 *         - 11: 10 minutes
 * @param[in] retrans @parblock
 *   Retransmission count and interval, which controls number of times that the
 *   model re-publishes the same message after the initial publish transmission
 *   and the cadence of retransmissions.
 *
 *   Retransmission count is encoded in the three low bits of the value, ranging
 *   from 0 to 7. Default value is 0 (no retransmissions).
 *
 *   Retransmission interval is encoded in the five high bits of the value,
 *   ranging from 0 to 31, in 50-millisecond units. Value of 0 corresponds to 50
 *   ms, while value of 31 corresponds to 1600 ms.
 *   @endparblock
 * @param[in] credentials Friendship credentials flag
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_local_model_pub(uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t pub_address,
                                               uint8_t ttl,
                                               uint8_t period,
                                               uint8_t retrans,
                                               uint8_t credentials);

/***************************************************************************//**
 *
 * Set a model's publication virtual address, key, and parameters.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] appkey_index The application key index to use for the published
 *   messages
 * @param[in] vendor_id Vendor ID of the configured model. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID of the configured model
 * @param[in] ttl Publication time-to-live value
 * @param[in] period Publication period encoded as step count and step
 *   resolution. The encoding is as follows:
 *     - <b>Bits 0..5:</b> Step count
 *     - <b>Bits 6..7:</b> Step resolution:
 *         - 00: 100 milliseconds
 *         - 01: 1 second
 *         - 10: 10 seconds
 *         - 11: 10 minutes
 * @param[in] retrans See documentation of @ref
 *   sl_btmesh_test_set_local_model_pub for details.
 * @param[in] credentials Friendship credentials flag
 * @param[in] pub_address_len Length of data in @p pub_address
 * @param[in] pub_address The Label UUID to publish to. The byte array must be
 *   exactly 16 bytes long.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_local_model_pub_va(uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint8_t ttl,
                                                  uint8_t period,
                                                  uint8_t retrans,
                                                  uint8_t credentials,
                                                  size_t pub_address_len,
                                                  const uint8_t* pub_address);

/***************************************************************************//**
 *
 * Get a local model's publication address, key, and parameters.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[out] appkey_index The application key index used for the application
 *   messages published
 * @param[out] pub_address The address published to
 * @param[out] ttl Time-to-Live value for published messages
 * @param[out] period Publication period encoded as step count and step
 *   resolution. The encoding is as follows:
 *     - <b>Bits 0..5:</b> Step count
 *     - <b>Bits 6..7:</b> Step resolution:
 *         - 00: 100 milliseconds
 *         - 01: 1 second
 *         - 10: 10 seconds
 *         - 11: 10 minutes
 * @param[out] retrans See documentation of @ref
 *   sl_btmesh_test_set_local_model_pub for details.
 * @param[out] credentials Friendship credentials flag
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_local_model_pub(uint16_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t *appkey_index,
                                               uint16_t *pub_address,
                                               uint8_t *ttl,
                                               uint8_t *period,
                                               uint8_t *retrans,
                                               uint8_t *credentials);

/***************************************************************************//**
 *
 * Set local node heartbeat subscription parameters. Normally heartbeat
 * subscription is controlled by the Provisioner.
 *
 * @param[in] subscription_source Source address for heartbeat messages. Must be
 *   either a unicast address or the unassigned address, in which case heartbeat
 *   messages are not processed.
 * @param[in] subscription_destination Destination address for heartbeat
 *   messages. The address must be either the unicast address of the primary
 *   element of the node, a group address, or the unassigned address. If it is
 *   the unassigned address, heartbeat messages are not processed.
 * @param[in] period_log Heartbeat subscription period setting. Valid values are
 *   as follows:
 *     - <b>0x00:</b> Heartbeat messages are not received
 *     - <b>0x01 .. 0x11:</b> Node will receive heartbeat messages for 2^(n-1)
 *       seconds
 *     - <b>0x12 .. 0xff:</b> Prohibited
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_local_heartbeat_subscription(uint16_t subscription_source,
                                                            uint16_t subscription_destination,
                                                            uint8_t period_log);

/***************************************************************************//**
 *
 * Get the local node heartbeat subscription. state
 *
 * @param[out] count Number of received heartbeat messages
 * @param[out] hop_min Minimum observed hop count in heartbeat messages
 * @param[out] hop_max Maximum observed hop count in heartbeat messages
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_local_heartbeat_subscription(uint16_t *count,
                                                            uint8_t *hop_min,
                                                            uint8_t *hop_max);

/***************************************************************************//**
 *
 * Get the heartbeat publication state of a local node.
 *
 * @param[out] publication_address Heartbeat publication address
 * @param[out] count Heartbeat publication remaining count
 * @param[out] period_log Heartbeat publication period setting. Valid values are
 *   as follows:
 *     - <b>0x00:</b> Heartbeat messages are not sent
 *     - <b>0x01 .. 0x11:</b> Node will send a heartbeat message every 2^(n-1)
 *       seconds
 *     - <b>0x12 .. 0xff:</b> Prohibited
 * @param[out] ttl Time-to-live parameter for heartbeat messages
 * @param[out] features Heartbeat trigger setting
 * @param[out] publication_netkey_index Index of the network key used to encrypt
 *   heartbeat messages
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_local_heartbeat_publication(uint16_t *publication_address,
                                                           uint8_t *count,
                                                           uint8_t *period_log,
                                                           uint8_t *ttl,
                                                           uint16_t *features,
                                                           uint16_t *publication_netkey_index);

/***************************************************************************//**
 *
 * Set the heartbeat publication state of a local node.
 *
 * @param[in] publication_address Heartbeat publication address. The address
 *   can't be a virtual address. Note that it can be the unassigned address, in
 *   which case the heartbeat publishing is disabled.
 * @param[in] count_log Heartbeat publication count setting. Valid values are as
 *   follows:
 *     - <b>0x00:</b> Heartbeat messages are not sent
 *     - <b>0x01 .. 0x11:</b> Node will send 2^(n-1) heartbeat messages
 *     - <b>0x12 .. 0xfe:</b> Prohibited
 *     - <b>0xff:</b> Hearbeat messages are sent indefinitely
 * @param[in] period_log Heartbeat publication period setting. Valid values are
 *   as follows:
 *     - <b>0x00:</b> Heartbeat messages are not sent
 *     - <b>0x01 .. 0x11:</b> Node will send a heartbeat message every 2^(n-1)
 *       seconds
 *     - <b>0x12 .. 0xff:</b> Prohibited
 * @param[in] ttl Time-to-live parameter for heartbeat messages
 * @param[in] features @parblock
 *   Heartbeat trigger setting. For bits set in the bitmask, reconfiguration of
 *   the node feature associated with the bit will result in the node emitting a
 *   heartbeat message. Valid values are as follows:
 *     - <b>Bit 0:</b> Relay feature
 *     - <b>Bit 1:</b> Proxy feature
 *     - <b>Bit 2:</b> Friend feature
 *     - <b>Bit 3:</b> Low power feature
 *
 *   Remaining bits are reserved for future use.
 *   @endparblock
 * @param[in] publication_netkey_index Index of the network key used to encrypt
 *   heartbeat messages
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_local_heartbeat_publication(uint16_t publication_address,
                                                           uint8_t count_log,
                                                           uint8_t period_log,
                                                           uint8_t ttl,
                                                           uint16_t features,
                                                           uint16_t publication_netkey_index);

/***************************************************************************//**
 *
 * <b>Deprecated</b> . Use the following commands instead:
 *   - @ref sl_btmesh_test_set_beacon for setting secure network beacon state
 *   - @ref sl_btmesh_test_set_default_ttl for setting default TTL state
 *   - @ref sl_btmesh_test_set_friend for setting friend state
 *   - @ref sl_btmesh_test_set_gatt_proxy for setting GATT proxy state
 *   - @ref sl_btmesh_test_set_identity for setting node identity state
 *   - @ref sl_btmesh_test_set_nettx for setting network transmit state
 *   - @ref sl_btmesh_test_set_relay for setting relay and relay retransmit
 *     state
 *
 * Set a state to a value in the local Configuration Server model. Use for
 * testing and debugging purposes only.
 *
 * @param[in] id Enum @ref sl_btmesh_node_config_state_t. The State to modify
 * @param[in] netkey_index Network key index; ignored for node-wide states
 * @param[in] value_len Length of data in @p value
 * @param[in] value The new value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_btmesh_test_set_local_config(uint16_t id,
                                            uint16_t netkey_index,
                                            size_t value_len,
                                            const uint8_t* value);

/***************************************************************************//**
 *
 * <b>Deprecated</b> . Use the following commands instead:
 *   - @ref sl_btmesh_test_get_beacon for setting secure network beacon state
 *   - @ref sl_btmesh_test_get_default_ttl for setting default TTL state
 *   - @ref sl_btmesh_test_get_friend for setting friend state
 *   - @ref sl_btmesh_test_get_gatt_proxy for setting GATT proxy state
 *   - @ref sl_btmesh_test_get_identity for setting node identity state
 *   - @ref sl_btmesh_test_get_nettx for setting network transmit state
 *   - @ref sl_btmesh_test_get_relay for setting relay and relay retransmit
 *     state
 *
 * Get the value of a state in the Configuration Server model. Use this for
 * testing and debugging purposes only.
 *
 * @param[in] id Enum @ref sl_btmesh_node_config_state_t. The state to read
 * @param[in] netkey_index Network key index; ignored for node-wide states
 * @param[in] max_data_size Size of output buffer passed in @p data
 * @param[out] data_len On return, set to the length of output data written to
 *   @p data
 * @param[out] data Raw binary value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_btmesh_test_get_local_config(uint16_t id,
                                            uint16_t netkey_index,
                                            size_t max_data_size,
                                            size_t *data_len,
                                            uint8_t *data);

/***************************************************************************//**
 *
 * Add a network or application key locally.
 *
 * @param[in] key_type Enum @ref sl_btmesh_test_key_type_t. 0 for network key, 1
 *   for application key
 * @param[in] key Key data
 * @param[in] key_index Index for the added key (must be unused)
 * @param[in] netkey_index Network key index to which the application key is
 *   bound; ignored for network keys
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_add_local_key(uint8_t key_type,
                                         aes_key_128 key,
                                         uint16_t key_index,
                                         uint16_t netkey_index);

/***************************************************************************//**
 *
 * Delete a network or application key locally.
 *
 * @param[in] key_type Enum @ref sl_btmesh_test_key_type_t. 0 for network key, 1
 *   for application key
 * @param[in] key_index Index of the key to delete
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_remove_local_key(uint8_t key_type,
                                            uint16_t key_index);

/***************************************************************************//**
 *
 * Update the network or application key value locally.
 *
 * Copies the existing network key value to the old value and replaces the
 * current value with the given key data.
 *
 * Note that the standard way to update keys on the Provisioner as well as on
 * nodes is to run the key refresh procedure. This command is for debugging
 * only.
 *
 * @param[in] key_type Enum @ref sl_btmesh_test_key_type_t. 0 for network key, 1
 *   for application key
 * @param[in] key Key data
 * @param[in] key_index Index for the key to update
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_update_local_key(uint8_t key_type,
                                            aes_key_128 key,
                                            uint16_t key_index);

/***************************************************************************//**
 *
 * Change the transport layer segmentation and reassembly configuration values.
 * This command must be issued before initializing the Mesh stack or the changes
 * will not take effect.
 *
 * @param[in] incomplete_timer_ms Maximum timeout before a transaction expires,
 *   regardless of other parameters. Value is in milliseconds. Default = 10000
 *   (10 seconds).
 * @param[in] pending_ack_base_ms Base time to wait at the receiver before
 *   sending a transport layer acknowledgment. Value is in milliseconds. Default
 *   = 150.
 * @param[in] pending_ack_mul_ms TTL multiplier to add to the base
 *   acknowledgment timer. Value is in milliseconds. Default = 50.
 * @param[in] wait_for_ack_base_ms Base time to wait for an acknowledgment at
 *   the sender before retransmission. Value is in milliseconds. Default = 200.
 * @param[in] wait_for_ack_mul_ms TTL multiplier to add to the base
 *   retransmission timer. Value is in milliseconds. Default = 50.
 * @param[in] max_send_rounds Number of attempts to send fragments of a
 *   segmented message, including the initial TX. Default = 3.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_sar_config(uint32_t incomplete_timer_ms,
                                          uint32_t pending_ack_base_ms,
                                          uint32_t pending_ack_mul_ms,
                                          uint32_t wait_for_ack_base_ms,
                                          uint32_t wait_for_ack_mul_ms,
                                          uint8_t max_send_rounds);

/***************************************************************************//**
 *
 * Disable or enable advertisement bearer for sending.
 *
 * @param[in] state 0: disable advertisement, 1: enable advertisement.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_adv_bearer_state(uint8_t state);

/***************************************************************************//**
 *
 * Get the device key with the address of the node's primary element.
 *
 * @param[in] address Address of the node
 * @param[out] device_key Device key, 16-bytes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_prov_get_device_key(uint16_t address,
                                               aes_key_128 *device_key);

/***************************************************************************//**
 *
 * Prepare the key refresh by feeding the new network key and all needed
 * application keys. The function can be called multiple times to include more
 * application keys. The network key must be the same in all calls. If the
 * network key is changed, the network key from the 1st command is used.
 * Sending application key data with length zero results in all initialization
 * data being forgotten unless this is done in the first prepare command i.e.,
 * trying to update only the network key. Also starting the key refresh
 * procedure results in all the preparation data being forgotten.
 *
 * @param[in] net_key New net key
 * @param[in] app_keys_len Length of data in @p app_keys
 * @param[in] app_keys list of new application keys, 16-bytes each
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_prov_prepare_key_refresh(aes_key_128 net_key,
                                                    size_t app_keys_len,
                                                    const uint8_t* app_keys);

/***************************************************************************//**
 *
 * Cancel sending a segmented message.
 *
 * @param[in] src_addr Source address for the segmented message
 * @param[in] dst_addr Destination address for the segmented message
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_cancel_segmented_tx(uint16_t src_addr,
                                               uint16_t dst_addr);

/***************************************************************************//**
 *
 * Set the IV index value of the node.
 *
 * @param[in] iv_index IV Index value to use
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_iv_index(uint32_t iv_index);

/***************************************************************************//**
 *
 * Set the current sequence number of an element.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] seqnum Sequence number to set on the target element
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_element_seqnum(uint16_t elem_index,
                                              uint32_t seqnum);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Set the model-specific option.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] option Option to set
 * @param[in] value Value for the option
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t value);

/***************************************************************************//**
 *
 * Get the application key bindings of a model.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] max_appkeys_size Size of output buffer passed in @p appkeys
 * @param[out] appkeys_len On return, set to the length of output data written
 *   to @p appkeys
 * @param[out] appkeys List of 16-bit application key indices; empty if model
 *   has not been bound to any application key.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_local_model_app_bindings(uint16_t elem_index,
                                                        uint16_t vendor_id,
                                                        uint16_t model_id,
                                                        size_t max_appkeys_size,
                                                        size_t *appkeys_len,
                                                        uint8_t *appkeys);

/***************************************************************************//**
 *
 * Get the replay protection list entry for an address.
 *
 * @param[in] address Source address to check
 * @param[out] seq Unsigned 32-bit integer
 * @param[out] seq_ivindex Unsigned 32-bit integer
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_replay_protection_list_entry(uint16_t address,
                                                            uint32_t *seq,
                                                            uint32_t *seq_ivindex);

/***************************************************************************//**
 *
 * Clear the replay protection list entry for an address. Use this command
 * carefully because it may expose the node to replay attacks when misused.
 *
 * @param[in] address Source address to use in finding the entry
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_clear_replay_protection_list_entry(uint16_t address);

/***************************************************************************//**
 *
 * Enable or disable replay protection list diagnostic events. When enabled,
 * events related to the replay protection list changes are generated.
 *
 * @param[in] enable Enable (nonzero) or disable (zero) diagnostic events for
 *   replay protection list
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_replay_protection_list_diagnostics(uint8_t enable);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Get a model-specific option.
 *
 * @param[in] elem_index The index of the target element, 0 is the primary
 *   element
 * @param[in] vendor_id Vendor ID for vendor-specific models. Use 0xffff for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID
 * @param[in] option Option to get.
 * @param[out] value Value for the option.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_model_option(uint16_t elem_index,
                                            uint16_t vendor_id,
                                            uint16_t model_id,
                                            uint8_t option,
                                            uint32_t *value);

/***************************************************************************//**
 *
 * Get node default TTL state.
 *
 * @param[out] value Default TTL value. Valid value range is from 2 to 127 for
 *   relayed PDUs, and 0 to indicate non-relayed PDUs
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_default_ttl(uint8_t *value);

/***************************************************************************//**
 *
 * Set node default TTL state.
 *
 * @param[in] set_value Default TTL value. See @ref
 *   sl_btmesh_test_get_default_ttl for details.
 * @param[out] value Default TTL value. See @ref sl_btmesh_test_get_default_ttl
 *   for details.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_default_ttl(uint8_t set_value, uint8_t *value);

/***************************************************************************//**
 *
 * Get node GATT proxy state.
 *
 * @param[out] value GATT proxy value of the node. Valid values are:
 *     - 0: GATT proxy feature is disabled
 *     - 1: GATT proxy feature is enabled
 *     - 2: GATT proxy feature is not supported
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_gatt_proxy(uint8_t *value);

/***************************************************************************//**
 *
 * Set node GATT proxy state.
 *
 * @param[in] set_value GATT proxy value to set. Valid values are:
 *     - 0: Proxy feature is disabled
 *     - 1: Proxy feature is enabled
 * @param[out] value GATT proxy state value. See @ref
 *   sl_btmesh_test_get_gatt_proxy for details.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_gatt_proxy_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_gatt_proxy(uint8_t set_value, uint8_t *value);

/***************************************************************************//**
 *
 * Get node identity state.
 *
 * @param[in] get_netkey_index Network key index for which the state is queried
 * @param[out] netkey_index Network key index for which the state is queried
 * @param[out] value Identity state of the node for the used network index.
 *   Valid values are as follows:
 *     - 0: Node identity advertising is disabled
 *     - 1: Node identity advertising is enabled
 *     - 2: Node identity advertising is not supported
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_identity(uint16_t get_netkey_index,
                                        uint16_t *netkey_index,
                                        uint8_t *value);

/***************************************************************************//**
 *
 * Set node identity state.
 *
 * @param[in] set_netkey_index Network key index for which the state is
 *   configured
 * @param[in] set_value Identity value to set. Valid values are:
 *     - 0: Node identity advertising is disabled
 *     - 1: Node identity advertising is enabled
 * @param[out] netkey_index Network key index for which the state is set
 * @param[out] value Identity state of the node for the used network index. See
 *   @ref sl_btmesh_test_get_identity for details
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_identity(uint16_t set_netkey_index,
                                        uint8_t set_value,
                                        uint16_t *netkey_index,
                                        uint8_t *value);

/***************************************************************************//**
 *
 * Get node friend state.
 *
 * @param[out] value Friend state value. Valid values are:
 *     - 0: Friend feature is not enabled
 *     - 1: Friend feature is enabled
 *     - 2: Friend feature is not supported
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_friend(uint8_t *value);

/***************************************************************************//**
 *
 * Set node friend state.
 *
 * @param[in] set_value Friend value to set. Valid values are:
 *     - 0: Friend feature is not enabled
 *     - 1: Friend feature is enabled
 * @param[out] value Friend state value. See @ref sl_btmesh_test_get_friend for
 *   detais.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_friend(uint8_t set_value, uint8_t *value);

/***************************************************************************//**
 *
 * Get node secure network beacon state.
 *
 * @param[out] value Secure network beacon value. Valid values are:
 *     - 0: Node is not broadcasting secure network beacons
 *     - 1: Node is broadcasting secure network beacons
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_get_beacon(uint8_t *value);

/***************************************************************************//**
 *
 * Set node secure network beacon state.
 *
 * @param[in] set_value Secure network beacon value to set. Valid values are:
 *     - 0: Node is not broadcasting secure network beacons
 *     - 1: Node is broadcasting secure network beacons
 * @param[out] value Secure network beacon value of the node.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_test_set_beacon(uint8_t set_value, uint8_t *value);

/** @} */ // end addtogroup sl_btmesh_test

/**
 * @addtogroup sl_btmesh_lpn Bluetooth Mesh Low Power Node API
 * @{
 *
 * @brief Bluetooth Mesh Low Power Node API
 *
 * These commands and events are for low-power operation, available in nodes
 * which have the LPN feature.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_lpn_init_id                                        0x00230028
#define sl_btmesh_cmd_lpn_deinit_id                                      0x01230028
#define sl_btmesh_cmd_lpn_establish_friendship_id                        0x03230028
#define sl_btmesh_cmd_lpn_poll_id                                        0x04230028
#define sl_btmesh_cmd_lpn_terminate_friendship_id                        0x05230028
#define sl_btmesh_cmd_lpn_config_id                                      0x06230028
#define sl_btmesh_rsp_lpn_init_id                                        0x00230028
#define sl_btmesh_rsp_lpn_deinit_id                                      0x01230028
#define sl_btmesh_rsp_lpn_establish_friendship_id                        0x03230028
#define sl_btmesh_rsp_lpn_poll_id                                        0x04230028
#define sl_btmesh_rsp_lpn_terminate_friendship_id                        0x05230028
#define sl_btmesh_rsp_lpn_config_id                                      0x06230028

/**
 * @brief Key values to identify LPN configurations
 */
typedef enum
{
  sl_btmesh_lpn_queue_length    = 0x0, /**< (0x0) Minimum queue length that the
                                            friend must support. Choose an
                                            appropriate length based on the
                                            expected message frequency and LPN
                                            sleep period because messages that
                                            do not fit into the friend queue are
                                            dropped. Note that the given value
                                            is rounded up to the nearest power
                                            of 2. Range: 2..128 */
  sl_btmesh_lpn_poll_timeout    = 0x1, /**< (0x1) Poll timeout in milliseconds,
                                            which is the longest time that LPN
                                            sleeps in between querying its
                                            friend for queued messages. Long
                                            poll timeout allows the LPN to sleep
                                            for longer periods, at the expense
                                            of increased latency for receiving
                                            messages. Note that the given value
                                            is rounded up to the nearest 100 ms
                                            Range: 1 s to 95 h 59 min 59 s 900
                                            ms */
  sl_btmesh_lpn_receive_delay   = 0x2, /**< (0x2) Receive delay in milliseconds.
                                            Receive delay is the time between
                                            the LPN sending a request and
                                            listening for a response. Receive
                                            delay allows the friend node time to
                                            prepare the message and LPN to
                                            sleep. Range: 10 ms to 255 ms The
                                            default receive delay in 10 ms. */
  sl_btmesh_lpn_request_retries = 0x3, /**< (0x3) Request retry is the number of
                                            retry attempts to repeat e.g., the
                                            friend poll message if the friend
                                            update was not received by the LPN.
                                            Range is from 0 to 10, default is 3 */
  sl_btmesh_lpn_retry_interval  = 0x4, /**< (0x4) Time interval between retry
                                            attempts in milliseconds. Range is 0
                                            to 100 ms. */
  sl_btmesh_lpn_clock_accuracy  = 0x5  /**< (0x5) Clock accuracy in ppm, which
                                            will be taken into account when
                                            opening and closing the receive
                                            window, and determining the poll
                                            timeout. Should be used with care,
                                            because inaccurate clock can
                                            increase the receive window lenght
                                            to up to 2,5 times in some cases.
                                            Default value is 0. */
} sl_btmesh_lpn_settings_t;

/**
 * @addtogroup sl_btmesh_evt_lpn_friendship_established sl_btmesh_evt_lpn_friendship_established
 * @{
 * @brief Indicate that a friendship has been established.
 */

/** @brief Identifier of the friendship_established event */
#define sl_btmesh_evt_lpn_friendship_established_id                      0x002300a8

/***************************************************************************//**
 * @brief Data structure of the friendship_established event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lpn_friendship_established_s
{
  uint16_t netkey_index;   /**< Network key index */
  uint16_t friend_address; /**< Friend node address */
});

typedef struct sl_btmesh_evt_lpn_friendship_established_s sl_btmesh_evt_lpn_friendship_established_t;

/** @} */ // end addtogroup sl_btmesh_evt_lpn_friendship_established

/**
 * @addtogroup sl_btmesh_evt_lpn_friendship_failed sl_btmesh_evt_lpn_friendship_failed
 * @{
 * @brief Indicate that the friendship establishment has failed.
 */

/** @brief Identifier of the friendship_failed event */
#define sl_btmesh_evt_lpn_friendship_failed_id                           0x012300a8

/***************************************************************************//**
 * @brief Data structure of the friendship_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lpn_friendship_failed_s
{
  uint16_t netkey_index; /**< Network key index */
  uint16_t reason;       /**< Reason for friendship establishment failure */
});

typedef struct sl_btmesh_evt_lpn_friendship_failed_s sl_btmesh_evt_lpn_friendship_failed_t;

/** @} */ // end addtogroup sl_btmesh_evt_lpn_friendship_failed

/**
 * @addtogroup sl_btmesh_evt_lpn_friendship_terminated sl_btmesh_evt_lpn_friendship_terminated
 * @{
 * @brief Indicate that a friendship that was successfully established has been
 * terminated.
 */

/** @brief Identifier of the friendship_terminated event */
#define sl_btmesh_evt_lpn_friendship_terminated_id                       0x022300a8

/***************************************************************************//**
 * @brief Data structure of the friendship_terminated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lpn_friendship_terminated_s
{
  uint16_t netkey_index; /**< Network key index */
  uint16_t reason;       /**< Reason for friendship termination */
});

typedef struct sl_btmesh_evt_lpn_friendship_terminated_s sl_btmesh_evt_lpn_friendship_terminated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lpn_friendship_terminated

/***************************************************************************//**
 *
 * Initialize the Low Power node (LPN) mode. The node needs to be provisioned
 * before calling this command. After the LPN mode is initialized, the node
 * can't operate in the network without a Friend node. To establish a friendship
 * with a nearby Friend node, use the @ref sl_btmesh_lpn_establish_friendship
 * command. Make this call before calling the other commands in this class.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_init();

/***************************************************************************//**
 *
 * Deinitialize the LPN functionality. After calling this command, a possible
 * friendship with a Friend node is terminated and the node can operate in the
 * network independently. After calling this command, do not call any other
 * command in this class before the Low Power mode is @ref sl_btmesh_lpn_init
 * again.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_deinit();

/***************************************************************************//**
 *
 * Establish a friendship. After a friendship has been established, the node can
 * start saving power.
 *
 * @param[in] netkey_index Network key index used in friendship request
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_establish_friendship(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Poll the Friend node for stored messages and security updates. This command
 * may be used if the application is expecting to receive messages at a specific
 * time. However, it is not required for correct operation, because the
 * procedure will be performed autonomously before the poll timeout expires.
 *
 * @param[in] netkey_index Network key index used in poll
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_poll(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Terminate an already established friendship. @ref
 * sl_btmesh_evt_lpn_friendship_terminated event will be emitted when the
 * friendship termination has been completed.
 *
 * @param[in] netkey_index Network key index used in friendship request
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_terminate_friendship(uint16_t netkey_index);

/***************************************************************************//**
 *
 * Configure the parameters for friendship establishment and LPN behavior.
 *
 * @param[in] setting_id Enum @ref sl_btmesh_lpn_settings_t. Identifies the LPN
 *   setting to be updated.
 * @param[in] value New value for the given setting
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lpn_config(uint8_t setting_id, uint32_t value);

/** @} */ // end addtogroup sl_btmesh_lpn

/**
 * @addtogroup sl_btmesh_friend Bluetooth Mesh Friend Node API
 * @{
 *
 * @brief Bluetooth Mesh Friend Node API
 *
 * These commands and events are for the Friend operation, available in nodes
 * which have the Friend feature.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_friend_init_id                                     0x00240028
#define sl_btmesh_cmd_friend_deinit_id                                   0x01240028
#define sl_btmesh_rsp_friend_init_id                                     0x00240028
#define sl_btmesh_rsp_friend_deinit_id                                   0x01240028

/**
 * @addtogroup sl_btmesh_evt_friend_friendship_established sl_btmesh_evt_friend_friendship_established
 * @{
 * @brief Indicate that a friendship has been established.
 */

/** @brief Identifier of the friendship_established event */
#define sl_btmesh_evt_friend_friendship_established_id                   0x002400a8

/***************************************************************************//**
 * @brief Data structure of the friendship_established event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_friend_friendship_established_s
{
  uint16_t netkey_index; /**< Index of the network key used in friendship */
  uint16_t lpn_address;  /**< LPN node address */
});

typedef struct sl_btmesh_evt_friend_friendship_established_s sl_btmesh_evt_friend_friendship_established_t;

/** @} */ // end addtogroup sl_btmesh_evt_friend_friendship_established

/**
 * @addtogroup sl_btmesh_evt_friend_friendship_terminated sl_btmesh_evt_friend_friendship_terminated
 * @{
 * @brief Indicate that a friendship that was successfully established has been
 * terminated.
 */

/** @brief Identifier of the friendship_terminated event */
#define sl_btmesh_evt_friend_friendship_terminated_id                    0x012400a8

/***************************************************************************//**
 * @brief Data structure of the friendship_terminated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_friend_friendship_terminated_s
{
  uint16_t netkey_index; /**< Index of the network key used in friendship */
  uint16_t lpn_address;  /**< LPN node address */
  uint16_t reason;       /**< Reason for friendship termination */
});

typedef struct sl_btmesh_evt_friend_friendship_terminated_s sl_btmesh_evt_friend_friendship_terminated_t;

/** @} */ // end addtogroup sl_btmesh_evt_friend_friendship_terminated

/***************************************************************************//**
 *
 * Initialize the Friend mode. The node needs to be provisioned before calling
 * this command. After the Friend mode is initialized, it is ready to accept
 * friend requests from low-power nodes. This call has to be made before calling
 * the other commands in this class.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_friend_init();

/***************************************************************************//**
 *
 * Deinitialize the Friend functionality. After calling this command, a possible
 * friendship with a Low Power node is terminated and all friendships are
 * terminated. After calling this command, don't call other commands in this
 * class before the Friend mode is @ref sl_btmesh_friend_init again.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_friend_deinit();

/** @} */ // end addtogroup sl_btmesh_friend

/**
 * @addtogroup sl_btmesh_proxy_client Bluetooth Mesh GATT Proxy Client
 * @{
 *
 * @brief Bluetooth Mesh GATT Proxy Client
 *
 * Initialize the GATT Proxy client-side functionality. Mesh proxy commands are
 * in the mesh_proxy class. This class allows the linker to drop the GATT Proxy
 * client code if it is not needed. It is enough to initialize this BGAPI class.
 * It contains no commands or events.
 */

/* Command and Response IDs */

/** @} */ // end addtogroup sl_btmesh_proxy_client

/**
 * @addtogroup sl_btmesh_proxy_server Bluetooth Mesh GATT Proxy Server
 * @{
 *
 * @brief Bluetooth Mesh GATT Proxy Server
 *
 * Initialize the GATT Proxy server-side functionality. This class allows the
 * linker to drop the GATT Proxy server code if it is not needed. It is enough
 * to initialize this BGAPI class. It contains no commands or events.
 */

/* Command and Response IDs */

/** @} */ // end addtogroup sl_btmesh_proxy_server

/**
 * @addtogroup sl_btmesh_config_client Bluetooth Mesh Configuration Client
 * @{
 *
 * @brief Bluetooth Mesh Configuration Client
 *
 * Bluetooth mesh stack API for the Mesh Configuration Client
 *
 * Commands in this class configure nodes in the Mesh network, which includes
 * key management, publish and subscribe settings manipulation, and node feature
 * configuration.
 *
 * Requests to nodes are asynchronous. A handle is assigned to each request that
 * is pending a reply from a node in the network. The handle can be used to
 * query the request status, and to identify the response event from the node.
 * Multiple requests can be made in parallel (as long as they are destined to
 * different nodes; only one pending request per node is allowed).
 *
 * <b>Request Management</b>
 *   - @ref sl_btmesh_config_client_cancel_request : Cancel a request
 *   - @ref sl_btmesh_config_client_get_request_status : Query current status of
 *     a request
 *   - @ref sl_btmesh_config_client_get_default_timeout : Get default request
 *     timeout
 *   - @ref sl_btmesh_config_client_set_default_timeout : Set default request
 *     timeout
 *
 * <b>Key and Mesh Network Management</b>
 *   - @ref sl_btmesh_config_client_add_netkey : Deploy a network key to a node
 *   - @ref sl_btmesh_config_client_remove_netkey : Remove a network key from a
 *     node
 *   - @ref sl_btmesh_config_client_list_netkeys : List network keys on a node
 *   - @ref sl_btmesh_config_client_add_appkey : Deploy an application key to a
 *     node
 *   - @ref sl_btmesh_config_client_remove_appkey : Remove an application key
 *     from a node
 *   - @ref sl_btmesh_config_client_list_appkeys : List application keys bound
 *     to a network key on a node
 *
 * <b>Node Configuration</b>
 *   - @ref sl_btmesh_config_client_get_dcd : Get device composition data of a
 *     node
 *   - @ref sl_btmesh_config_client_reset_node : Reset a node
 *   - @ref sl_btmesh_config_client_get_default_ttl : Get node default TTL state
 *     value
 *   - @ref sl_btmesh_config_client_set_default_ttl : Set node default TTL state
 *     value
 *   - @ref sl_btmesh_config_client_get_beacon : Get node secure network beacon
 *     state value
 *   - @ref sl_btmesh_config_client_set_beacon : Set node secure network beacon
 *     state value
 *   - @ref sl_btmesh_config_client_get_identity : Get node identity advertising
 *     state value
 *   - @ref sl_btmesh_config_client_set_identity : Set node identity advertising
 *     state value
 *   - @ref sl_btmesh_config_client_get_friend : Get node friend state value
 *   - @ref sl_btmesh_config_client_set_friend : Set node friend state value
 *   - @ref sl_btmesh_config_client_get_lpn_polltimeout : Get node LPN poll
 *     timeout state value
 *   - @ref sl_btmesh_config_client_get_gatt_proxy : Get node GATT proxy state
 *     value
 *   - @ref sl_btmesh_config_client_set_gatt_proxy : Set node GATT proxy state
 *     value
 *   - @ref sl_btmesh_config_client_get_relay : Get node relay state value
 *   - @ref sl_btmesh_config_client_set_relay : Set node relay state value
 *   - @ref sl_btmesh_config_client_get_network_transmit : Get node network
 *     transmit state value
 *   - @ref sl_btmesh_config_client_set_network_transmit : Set node network
 *     transmit state value
 *
 * <b>Model Configuration</b>
 *   - @ref sl_btmesh_config_client_bind_model : Bind a model to an application
 *     key
 *   - @ref sl_btmesh_config_client_unbind_model : Remove a model to application
 *     key binding
 *   - @ref sl_btmesh_config_client_list_bindings : List model to application
 *     key bindings on a node
 *   - @ref sl_btmesh_config_client_add_model_sub : Add a subscription address
 *     to a model
 *   - @ref sl_btmesh_config_client_add_model_sub_va : Add a virtual
 *     subscription address to a model
 *   - @ref sl_btmesh_config_client_remove_model_sub : Remove a subscription
 *     address from a model
 *   - @ref sl_btmesh_config_client_remove_model_sub_va : Remove a virtual
 *     subscription address from a model
 *   - @ref sl_btmesh_config_client_set_model_sub : Overwrite the subscription
 *     list of a model with an address
 *   - @ref sl_btmesh_config_client_set_model_sub_va : Overwrite the
 *     subscription list of a model with a virtual address
 *   - @ref sl_btmesh_config_client_clear_model_sub : Clear the subscription
 *     list of a model
 *   - @ref sl_btmesh_config_client_list_subs : Get the subscription list of a
 *     model
 *   - @ref sl_btmesh_config_client_get_model_pub : Get a model's publication
 *     parameters
 *   - @ref sl_btmesh_config_client_set_model_pub : Set a model's publication
 *     parameters
 *
 * <b>Heartbeat</b>
 *   - @ref sl_btmesh_config_client_get_heartbeat_pub : Get node heartbeat
 *     publication settings
 *   - @ref sl_btmesh_config_client_set_heartbeat_pub : Set node heartbeat
 *     publication settings
 *   - @ref sl_btmesh_config_client_get_heartbeat_sub : Get node heartbeat
 *     subscription settings
 *   - @ref sl_btmesh_config_client_set_heartbeat_sub : Set node heartbeat
 *     subscription settings
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_config_client_cancel_request_id                    0x00270028
#define sl_btmesh_cmd_config_client_get_request_status_id                0x01270028
#define sl_btmesh_cmd_config_client_get_default_timeout_id               0x2e270028
#define sl_btmesh_cmd_config_client_set_default_timeout_id               0x2f270028
#define sl_btmesh_cmd_config_client_add_netkey_id                        0x02270028
#define sl_btmesh_cmd_config_client_remove_netkey_id                     0x03270028
#define sl_btmesh_cmd_config_client_list_netkeys_id                      0x04270028
#define sl_btmesh_cmd_config_client_add_appkey_id                        0x05270028
#define sl_btmesh_cmd_config_client_remove_appkey_id                     0x06270028
#define sl_btmesh_cmd_config_client_list_appkeys_id                      0x07270028
#define sl_btmesh_cmd_config_client_bind_model_id                        0x08270028
#define sl_btmesh_cmd_config_client_unbind_model_id                      0x09270028
#define sl_btmesh_cmd_config_client_list_bindings_id                     0x0a270028
#define sl_btmesh_cmd_config_client_get_model_pub_id                     0x0b270028
#define sl_btmesh_cmd_config_client_set_model_pub_id                     0x0c270028
#define sl_btmesh_cmd_config_client_set_model_pub_va_id                  0x0d270028
#define sl_btmesh_cmd_config_client_add_model_sub_id                     0x0e270028
#define sl_btmesh_cmd_config_client_add_model_sub_va_id                  0x0f270028
#define sl_btmesh_cmd_config_client_remove_model_sub_id                  0x10270028
#define sl_btmesh_cmd_config_client_remove_model_sub_va_id               0x11270028
#define sl_btmesh_cmd_config_client_set_model_sub_id                     0x12270028
#define sl_btmesh_cmd_config_client_set_model_sub_va_id                  0x13270028
#define sl_btmesh_cmd_config_client_clear_model_sub_id                   0x14270028
#define sl_btmesh_cmd_config_client_list_subs_id                         0x15270028
#define sl_btmesh_cmd_config_client_get_heartbeat_pub_id                 0x16270028
#define sl_btmesh_cmd_config_client_set_heartbeat_pub_id                 0x17270028
#define sl_btmesh_cmd_config_client_get_heartbeat_sub_id                 0x19270028
#define sl_btmesh_cmd_config_client_set_heartbeat_sub_id                 0x1a270028
#define sl_btmesh_cmd_config_client_get_beacon_id                        0x1b270028
#define sl_btmesh_cmd_config_client_set_beacon_id                        0x1c270028
#define sl_btmesh_cmd_config_client_get_default_ttl_id                   0x1d270028
#define sl_btmesh_cmd_config_client_set_default_ttl_id                   0x1e270028
#define sl_btmesh_cmd_config_client_get_gatt_proxy_id                    0x1f270028
#define sl_btmesh_cmd_config_client_set_gatt_proxy_id                    0x20270028
#define sl_btmesh_cmd_config_client_get_relay_id                         0x21270028
#define sl_btmesh_cmd_config_client_set_relay_id                         0x22270028
#define sl_btmesh_cmd_config_client_get_network_transmit_id              0x23270028
#define sl_btmesh_cmd_config_client_set_network_transmit_id              0x24270028
#define sl_btmesh_cmd_config_client_get_identity_id                      0x25270028
#define sl_btmesh_cmd_config_client_set_identity_id                      0x26270028
#define sl_btmesh_cmd_config_client_get_friend_id                        0x27270028
#define sl_btmesh_cmd_config_client_set_friend_id                        0x28270028
#define sl_btmesh_cmd_config_client_get_key_refresh_phase_id             0x29270028
#define sl_btmesh_cmd_config_client_get_lpn_polltimeout_id               0x2b270028
#define sl_btmesh_cmd_config_client_get_dcd_id                           0x2c270028
#define sl_btmesh_cmd_config_client_reset_node_id                        0x2d270028
#define sl_btmesh_rsp_config_client_cancel_request_id                    0x00270028
#define sl_btmesh_rsp_config_client_get_request_status_id                0x01270028
#define sl_btmesh_rsp_config_client_get_default_timeout_id               0x2e270028
#define sl_btmesh_rsp_config_client_set_default_timeout_id               0x2f270028
#define sl_btmesh_rsp_config_client_add_netkey_id                        0x02270028
#define sl_btmesh_rsp_config_client_remove_netkey_id                     0x03270028
#define sl_btmesh_rsp_config_client_list_netkeys_id                      0x04270028
#define sl_btmesh_rsp_config_client_add_appkey_id                        0x05270028
#define sl_btmesh_rsp_config_client_remove_appkey_id                     0x06270028
#define sl_btmesh_rsp_config_client_list_appkeys_id                      0x07270028
#define sl_btmesh_rsp_config_client_bind_model_id                        0x08270028
#define sl_btmesh_rsp_config_client_unbind_model_id                      0x09270028
#define sl_btmesh_rsp_config_client_list_bindings_id                     0x0a270028
#define sl_btmesh_rsp_config_client_get_model_pub_id                     0x0b270028
#define sl_btmesh_rsp_config_client_set_model_pub_id                     0x0c270028
#define sl_btmesh_rsp_config_client_set_model_pub_va_id                  0x0d270028
#define sl_btmesh_rsp_config_client_add_model_sub_id                     0x0e270028
#define sl_btmesh_rsp_config_client_add_model_sub_va_id                  0x0f270028
#define sl_btmesh_rsp_config_client_remove_model_sub_id                  0x10270028
#define sl_btmesh_rsp_config_client_remove_model_sub_va_id               0x11270028
#define sl_btmesh_rsp_config_client_set_model_sub_id                     0x12270028
#define sl_btmesh_rsp_config_client_set_model_sub_va_id                  0x13270028
#define sl_btmesh_rsp_config_client_clear_model_sub_id                   0x14270028
#define sl_btmesh_rsp_config_client_list_subs_id                         0x15270028
#define sl_btmesh_rsp_config_client_get_heartbeat_pub_id                 0x16270028
#define sl_btmesh_rsp_config_client_set_heartbeat_pub_id                 0x17270028
#define sl_btmesh_rsp_config_client_get_heartbeat_sub_id                 0x19270028
#define sl_btmesh_rsp_config_client_set_heartbeat_sub_id                 0x1a270028
#define sl_btmesh_rsp_config_client_get_beacon_id                        0x1b270028
#define sl_btmesh_rsp_config_client_set_beacon_id                        0x1c270028
#define sl_btmesh_rsp_config_client_get_default_ttl_id                   0x1d270028
#define sl_btmesh_rsp_config_client_set_default_ttl_id                   0x1e270028
#define sl_btmesh_rsp_config_client_get_gatt_proxy_id                    0x1f270028
#define sl_btmesh_rsp_config_client_set_gatt_proxy_id                    0x20270028
#define sl_btmesh_rsp_config_client_get_relay_id                         0x21270028
#define sl_btmesh_rsp_config_client_set_relay_id                         0x22270028
#define sl_btmesh_rsp_config_client_get_network_transmit_id              0x23270028
#define sl_btmesh_rsp_config_client_set_network_transmit_id              0x24270028
#define sl_btmesh_rsp_config_client_get_identity_id                      0x25270028
#define sl_btmesh_rsp_config_client_set_identity_id                      0x26270028
#define sl_btmesh_rsp_config_client_get_friend_id                        0x27270028
#define sl_btmesh_rsp_config_client_set_friend_id                        0x28270028
#define sl_btmesh_rsp_config_client_get_key_refresh_phase_id             0x29270028
#define sl_btmesh_rsp_config_client_get_lpn_polltimeout_id               0x2b270028
#define sl_btmesh_rsp_config_client_get_dcd_id                           0x2c270028
#define sl_btmesh_rsp_config_client_reset_node_id                        0x2d270028

/**
 * @addtogroup sl_btmesh_evt_config_client_request_modified sl_btmesh_evt_config_client_request_modified
 * @{
 * @brief Pending request parameters have been updated
 *
 * The application may call @ref sl_btmesh_config_client_get_request_status to
 * retrieve the current status of the request. This event is generated when the
 * timeout of a request is extended because the request is acknowledged by a
 * Friend node on behalf of the LPN, which is the destination of the request.
 */

/** @brief Identifier of the request_modified event */
#define sl_btmesh_evt_config_client_request_modified_id                  0x002700a8

/***************************************************************************//**
 * @brief Data structure of the request_modified event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_request_modified_s
{
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_request_modified_s sl_btmesh_evt_config_client_request_modified_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_request_modified

/**
 * @addtogroup sl_btmesh_evt_config_client_netkey_status sl_btmesh_evt_config_client_netkey_status
 * @{
 * @brief This event is created when a response for an @ref
 * sl_btmesh_config_client_add_netkey or a @ref
 * sl_btmesh_config_client_remove_netkey request is received, or the request
 * times out.
 */

/** @brief Identifier of the netkey_status event */
#define sl_btmesh_evt_config_client_netkey_status_id                     0x012700a8

/***************************************************************************//**
 * @brief Data structure of the netkey_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_netkey_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_netkey_status_s sl_btmesh_evt_config_client_netkey_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_netkey_status

/**
 * @addtogroup sl_btmesh_evt_config_client_netkey_list sl_btmesh_evt_config_client_netkey_list
 * @{
 * @brief This event contains a list of key indices for network keys that are
 * present on a node
 *
 * The list is requested using the @ref sl_btmesh_config_client_list_netkeys
 * command. More than one event may be generated. List contents are terminated
 * by a @ref sl_btmesh_evt_config_client_netkey_list_end event.
 */

/** @brief Identifier of the netkey_list event */
#define sl_btmesh_evt_config_client_netkey_list_id                       0x022700a8

/***************************************************************************//**
 * @brief Data structure of the netkey_list event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_netkey_list_s
{
  uint32_t   handle;         /**< Request handle */
  uint8array netkey_indices; /**< List of network key indices, two bytes per
                                  entry */
});

typedef struct sl_btmesh_evt_config_client_netkey_list_s sl_btmesh_evt_config_client_netkey_list_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_netkey_list

/**
 * @addtogroup sl_btmesh_evt_config_client_netkey_list_end sl_btmesh_evt_config_client_netkey_list_end
 * @{
 * @brief Terminating event for network key index list
 */

/** @brief Identifier of the netkey_list_end event */
#define sl_btmesh_evt_config_client_netkey_list_end_id                   0x032700a8

/***************************************************************************//**
 * @brief Data structure of the netkey_list_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_netkey_list_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_netkey_list_end_s sl_btmesh_evt_config_client_netkey_list_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_netkey_list_end

/**
 * @addtogroup sl_btmesh_evt_config_client_appkey_status sl_btmesh_evt_config_client_appkey_status
 * @{
 * @brief This event is created when a response for an @ref
 * sl_btmesh_config_client_add_appkey or a @ref
 * sl_btmesh_config_client_remove_appkey request is received or the request
 * times out.
 */

/** @brief Identifier of the appkey_status event */
#define sl_btmesh_evt_config_client_appkey_status_id                     0x042700a8

/***************************************************************************//**
 * @brief Data structure of the appkey_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_appkey_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_appkey_status_s sl_btmesh_evt_config_client_appkey_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_appkey_status

/**
 * @addtogroup sl_btmesh_evt_config_client_appkey_list sl_btmesh_evt_config_client_appkey_list
 * @{
 * @brief This event contains a list of key indices for application keys that
 * are present on a node and are bound to the network key specified in the
 * request
 *
 * The list is requested using the @ref sl_btmesh_config_client_list_appkeys
 * command. More than one event may be generated. List contents are terminated
 * by a @ref sl_btmesh_evt_config_client_appkey_list_end event.
 */

/** @brief Identifier of the appkey_list event */
#define sl_btmesh_evt_config_client_appkey_list_id                       0x052700a8

/***************************************************************************//**
 * @brief Data structure of the appkey_list event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_appkey_list_s
{
  uint32_t   handle;         /**< Request handle */
  uint8array appkey_indices; /**< List of application key indices, two bytes per
                                  entry */
});

typedef struct sl_btmesh_evt_config_client_appkey_list_s sl_btmesh_evt_config_client_appkey_list_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_appkey_list

/**
 * @addtogroup sl_btmesh_evt_config_client_appkey_list_end sl_btmesh_evt_config_client_appkey_list_end
 * @{
 * @brief Terminating event for application key index list
 */

/** @brief Identifier of the appkey_list_end event */
#define sl_btmesh_evt_config_client_appkey_list_end_id                   0x062700a8

/***************************************************************************//**
 * @brief Data structure of the appkey_list_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_appkey_list_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_appkey_list_end_s sl_btmesh_evt_config_client_appkey_list_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_appkey_list_end

/**
 * @addtogroup sl_btmesh_evt_config_client_binding_status sl_btmesh_evt_config_client_binding_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_bind_model and @ref
 * sl_btmesh_config_client_unbind_model application keys and models.
 */

/** @brief Identifier of the binding_status event */
#define sl_btmesh_evt_config_client_binding_status_id                    0x072700a8

/***************************************************************************//**
 * @brief Data structure of the binding_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_binding_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_binding_status_s sl_btmesh_evt_config_client_binding_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_binding_status

/**
 * @addtogroup sl_btmesh_evt_config_client_bindings_list sl_btmesh_evt_config_client_bindings_list
 * @{
 * @brief This event contains a list of key indices for the application keys
 * which are bound to a model
 *
 * The list is requested using the @ref sl_btmesh_config_client_list_bindings
 * command. More than one such event may be generated; the list contents are
 * terminated by a @ref sl_btmesh_evt_config_client_bindings_list_end event.
 */

/** @brief Identifier of the bindings_list event */
#define sl_btmesh_evt_config_client_bindings_list_id                     0x082700a8

/***************************************************************************//**
 * @brief Data structure of the bindings_list event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_bindings_list_s
{
  uint32_t   handle;         /**< Request handle */
  uint8array appkey_indices; /**< List of application key indices, two bytes per
                                  entry */
});

typedef struct sl_btmesh_evt_config_client_bindings_list_s sl_btmesh_evt_config_client_bindings_list_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_bindings_list

/**
 * @addtogroup sl_btmesh_evt_config_client_bindings_list_end sl_btmesh_evt_config_client_bindings_list_end
 * @{
 * @brief Terminating event for model-application key bindings list
 */

/** @brief Identifier of the bindings_list_end event */
#define sl_btmesh_evt_config_client_bindings_list_end_id                 0x092700a8

/***************************************************************************//**
 * @brief Data structure of the bindings_list_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_bindings_list_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_bindings_list_end_s sl_btmesh_evt_config_client_bindings_list_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_bindings_list_end

/**
 * @addtogroup sl_btmesh_evt_config_client_model_pub_status sl_btmesh_evt_config_client_model_pub_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_model_pub, @ref
 * sl_btmesh_config_client_set_model_pub, @ref
 * sl_btmesh_config_client_set_model_pub_va commands
 */

/** @brief Identifier of the model_pub_status event */
#define sl_btmesh_evt_config_client_model_pub_status_id                  0x0a2700a8

/***************************************************************************//**
 * @brief Data structure of the model_pub_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_model_pub_status_s
{
  uint16_t result;                 /**< SL_STATUS_OK if successful. Error code
                                        otherwise. */
  uint32_t handle;                 /**< Request handle */
  uint16_t address;                /**< The address to publish to. If this
                                        address is the unassigned address, the
                                        model is prevented from publishing. Note
                                        that, if state contains a Label UUID
                                        (full virtual address), the
                                        corresponding virtual address hash is
                                        returned in this parameter. */
  uint16_t appkey_index;           /**< The application key index used for the
                                        published messages */
  uint8_t  credentials;            /**< Friendship credentials flag */
  uint8_t  ttl;                    /**< Publication time-to-live value */
  uint32_t period_ms;              /**< Publication period in milliseconds */
  uint8_t  retransmit_count;       /**< Publication retransmission count */
  uint16_t retransmit_interval_ms; /**< Publication retransmission interval in
                                        milliseconds */
});

typedef struct sl_btmesh_evt_config_client_model_pub_status_s sl_btmesh_evt_config_client_model_pub_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_model_pub_status

/**
 * @addtogroup sl_btmesh_evt_config_client_model_sub_status sl_btmesh_evt_config_client_model_sub_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_add_model_sub : add
 * subscription address, @ref sl_btmesh_config_client_add_model_sub_va : add
 * subscription virtual address, @ref sl_btmesh_config_client_remove_model_sub :
 * remove subscription address, @ref sl_btmesh_config_client_remove_model_sub_va
 * : remove subscription virtual address, @ref
 * sl_btmesh_config_client_set_model_sub : set subscription address, @ref
 * sl_btmesh_config_client_set_model_sub_va : set subscription virtual address,
 * and @ref sl_btmesh_config_client_clear_model_sub : clear subscription address
 * list commands
 */

/** @brief Identifier of the model_sub_status event */
#define sl_btmesh_evt_config_client_model_sub_status_id                  0x0b2700a8

/***************************************************************************//**
 * @brief Data structure of the model_sub_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_model_sub_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_model_sub_status_s sl_btmesh_evt_config_client_model_sub_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_model_sub_status

/**
 * @addtogroup sl_btmesh_evt_config_client_subs_list sl_btmesh_evt_config_client_subs_list
 * @{
 * @brief This event contains a list of addresses that the queried model
 * subscribes to
 *
 * The list is requested using the @ref sl_btmesh_config_client_list_subs
 * command. More than one event may be generated. List contents are terminated
 * by a @ref sl_btmesh_evt_config_client_subs_list_end event. Note that, if the
 * subscription address list entry is a Label UUID (full virtual address), the
 * corresponding virtual address hash is returned in this event.
 */

/** @brief Identifier of the subs_list event */
#define sl_btmesh_evt_config_client_subs_list_id                         0x0c2700a8

/***************************************************************************//**
 * @brief Data structure of the subs_list event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_subs_list_s
{
  uint32_t   handle;    /**< Request handle */
  uint8array addresses; /**< List of subscription addresses, two bytes per entry */
});

typedef struct sl_btmesh_evt_config_client_subs_list_s sl_btmesh_evt_config_client_subs_list_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_subs_list

/**
 * @addtogroup sl_btmesh_evt_config_client_subs_list_end sl_btmesh_evt_config_client_subs_list_end
 * @{
 * @brief Terminating event for model subscription list
 */

/** @brief Identifier of the subs_list_end event */
#define sl_btmesh_evt_config_client_subs_list_end_id                     0x0d2700a8

/***************************************************************************//**
 * @brief Data structure of the subs_list_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_subs_list_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_subs_list_end_s sl_btmesh_evt_config_client_subs_list_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_subs_list_end

/**
 * @addtogroup sl_btmesh_evt_config_client_heartbeat_pub_status sl_btmesh_evt_config_client_heartbeat_pub_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_heartbeat_pub and
 * @ref sl_btmesh_config_client_set_heartbeat_pub commands
 */

/** @brief Identifier of the heartbeat_pub_status event */
#define sl_btmesh_evt_config_client_heartbeat_pub_status_id              0x0e2700a8

/***************************************************************************//**
 * @brief Data structure of the heartbeat_pub_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_heartbeat_pub_status_s
{
  uint16_t result;              /**< SL_STATUS_OK if successful. Error code
                                     otherwise. */
  uint32_t handle;              /**< Request handle */
  uint16_t destination_address; /**< Heartbeat publication destination address. */
  uint16_t netkey_index;        /**< Index of the network key used to encrypt
                                     heartbeat messages */
  uint8_t  count_log;           /**< Heartbeat publication count logarithm-of-2
                                     setting */
  uint8_t  period_log;          /**< Heartbeat publication period logarithm-of-2
                                     setting */
  uint8_t  ttl;                 /**< Time-to-live value for heartbeat messages */
  uint16_t features;            /**< Heartbeat trigger setting */
});

typedef struct sl_btmesh_evt_config_client_heartbeat_pub_status_s sl_btmesh_evt_config_client_heartbeat_pub_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_heartbeat_pub_status

/**
 * @addtogroup sl_btmesh_evt_config_client_heartbeat_sub_status sl_btmesh_evt_config_client_heartbeat_sub_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_heartbeat_sub and
 * @ref sl_btmesh_config_client_set_heartbeat_sub commands
 */

/** @brief Identifier of the heartbeat_sub_status event */
#define sl_btmesh_evt_config_client_heartbeat_sub_status_id              0x0f2700a8

/***************************************************************************//**
 * @brief Data structure of the heartbeat_sub_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_heartbeat_sub_status_s
{
  uint16_t result;              /**< SL_STATUS_OK if successful. Error code
                                     otherwise. */
  uint32_t handle;              /**< Request handle */
  uint16_t source_address;      /**< Source address for heartbeat messages */
  uint16_t destination_address; /**< Destination address for heartbeat messages */
  uint8_t  period_log;          /**< Heartbeat subscription remaining period
                                     logarithm-of-2 value */
  uint8_t  count_log;           /**< Received heartbeat message count
                                     logarithm-of-2 value */
  uint8_t  min_hops;            /**< Minimum hop value seen in received
                                     heartbeat messages */
  uint8_t  max_hops;            /**< Minimum hop value seen in received
                                     heartbeat messages */
});

typedef struct sl_btmesh_evt_config_client_heartbeat_sub_status_s sl_btmesh_evt_config_client_heartbeat_sub_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_heartbeat_sub_status

/**
 * @addtogroup sl_btmesh_evt_config_client_beacon_status sl_btmesh_evt_config_client_beacon_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_beacon and @ref
 * sl_btmesh_config_client_set_beacon commands.
 */

/** @brief Identifier of the beacon_status event */
#define sl_btmesh_evt_config_client_beacon_status_id                     0x102700a8

/***************************************************************************//**
 * @brief Data structure of the beacon_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_beacon_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
  uint8_t  value;  /**< Secure network beacon state of the node. Valid values
                        are:
                          - 0: Node is not broadcasting secure network beacons
                          - 1: Node is broadcasting secure network beacons */
});

typedef struct sl_btmesh_evt_config_client_beacon_status_s sl_btmesh_evt_config_client_beacon_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_beacon_status

/**
 * @addtogroup sl_btmesh_evt_config_client_default_ttl_status sl_btmesh_evt_config_client_default_ttl_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_default_ttl and @ref
 * sl_btmesh_config_client_set_default_ttl commands.
 */

/** @brief Identifier of the default_ttl_status event */
#define sl_btmesh_evt_config_client_default_ttl_status_id                0x112700a8

/***************************************************************************//**
 * @brief Data structure of the default_ttl_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_default_ttl_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
  uint8_t  value;  /**< Default TTL value. Valid value range is from 2 to 127
                        for relayed PDUs, and 0 to indicate non-relayed PDUs. */
});

typedef struct sl_btmesh_evt_config_client_default_ttl_status_s sl_btmesh_evt_config_client_default_ttl_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_default_ttl_status

/**
 * @addtogroup sl_btmesh_evt_config_client_gatt_proxy_status sl_btmesh_evt_config_client_gatt_proxy_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_gatt_proxy and @ref
 * sl_btmesh_config_client_set_gatt_proxy commands
 */

/** @brief Identifier of the gatt_proxy_status event */
#define sl_btmesh_evt_config_client_gatt_proxy_status_id                 0x122700a8

/***************************************************************************//**
 * @brief Data structure of the gatt_proxy_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_gatt_proxy_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
  uint8_t  value;  /**< GATT proxy value of the node. Valid values are:
                          - 0: GATT proxy feature is disabled
                          - 1: GATT proxy feature is enabled
                          - 2: GATT proxy feature is not supported */
});

typedef struct sl_btmesh_evt_config_client_gatt_proxy_status_s sl_btmesh_evt_config_client_gatt_proxy_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_gatt_proxy_status

/**
 * @addtogroup sl_btmesh_evt_config_client_relay_status sl_btmesh_evt_config_client_relay_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_relay and @ref
 * sl_btmesh_config_client_set_relay commands
 */

/** @brief Identifier of the relay_status event */
#define sl_btmesh_evt_config_client_relay_status_id                      0x132700a8

/***************************************************************************//**
 * @brief Data structure of the relay_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_relay_status_s
{
  uint16_t result;                 /**< SL_STATUS_OK if successful. Error code
                                        otherwise. */
  uint32_t handle;                 /**< Request handle */
  uint8_t  relay;                  /**< Relay state of the node. Valid values
                                        are as follows:
                                          - 0: Relaying disabled
                                          - 1: Relaying enabled
                                          - 2: Relaying not supported */
  uint8_t  retransmit_count;       /**< Relay retransmit count. Valid values
                                        range from 0 to 7; default value is 0
                                        (no retransmissions). */
  uint16_t retransmit_interval_ms; /**< Relay retransmit interval in
                                        milliseconds. Valid values range from 10
                                        ms to 320 ms, with a resolution of 10
                                        ms. The value will be zero if the
                                        retransmission count is zero. */
});

typedef struct sl_btmesh_evt_config_client_relay_status_s sl_btmesh_evt_config_client_relay_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_relay_status

/**
 * @addtogroup sl_btmesh_evt_config_client_network_transmit_status sl_btmesh_evt_config_client_network_transmit_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_network_transmit and
 * @ref sl_btmesh_config_client_set_network_transmit commands
 */

/** @brief Identifier of the network_transmit_status event */
#define sl_btmesh_evt_config_client_network_transmit_status_id           0x142700a8

/***************************************************************************//**
 * @brief Data structure of the network_transmit_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_network_transmit_status_s
{
  uint16_t result;               /**< SL_STATUS_OK if successful. Error code
                                      otherwise. */
  uint32_t handle;               /**< Request handle */
  uint8_t  transmit_count;       /**< Network transmit count. Valid values range
                                      from 1 to 8; default value is 1 (single
                                      transmission; no retransmissions). */
  uint16_t transmit_interval_ms; /**< Network transmit interval in milliseconds.
                                      Valid values range from 10 ms to 320 ms,
                                      with a resolution of 10 ms. The value will
                                      be zero if the transmission count is set
                                      to one. */
});

typedef struct sl_btmesh_evt_config_client_network_transmit_status_s sl_btmesh_evt_config_client_network_transmit_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_network_transmit_status

/**
 * @addtogroup sl_btmesh_evt_config_client_identity_status sl_btmesh_evt_config_client_identity_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_identity and @ref
 * sl_btmesh_config_client_set_identity commands.
 */

/** @brief Identifier of the identity_status event */
#define sl_btmesh_evt_config_client_identity_status_id                   0x152700a8

/***************************************************************************//**
 * @brief Data structure of the identity_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_identity_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
  uint8_t  value;  /**< Identity state of the node for the used network index.
                        Valid values are as follows:
                          - 0: Node identity advertising is disabled
                          - 1: Node identity advertising is enabled
                          - 2: Node identity advertising is not supported */
});

typedef struct sl_btmesh_evt_config_client_identity_status_s sl_btmesh_evt_config_client_identity_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_identity_status

/**
 * @addtogroup sl_btmesh_evt_config_client_friend_status sl_btmesh_evt_config_client_friend_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_friend and @ref
 * sl_btmesh_config_client_set_friend commands.
 */

/** @brief Identifier of the friend_status event */
#define sl_btmesh_evt_config_client_friend_status_id                     0x162700a8

/***************************************************************************//**
 * @brief Data structure of the friend_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_friend_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
  uint8_t  value;  /**< Friend value to set. Valid values are:
                          - 0: Friend feature is not enabled
                          - 1: Friend feature is enabled
                          - 2: Friend feature is not supported */
});

typedef struct sl_btmesh_evt_config_client_friend_status_s sl_btmesh_evt_config_client_friend_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_friend_status

/**
 * @addtogroup sl_btmesh_evt_config_client_key_refresh_phase_status sl_btmesh_evt_config_client_key_refresh_phase_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_key_refresh_phase
 * command.
 */

/** @brief Identifier of the key_refresh_phase_status event */
#define sl_btmesh_evt_config_client_key_refresh_phase_status_id          0x172700a8

/***************************************************************************//**
 * @brief Data structure of the key_refresh_phase_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_key_refresh_phase_status_s
{
  uint16_t result;       /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle;       /**< Request handle */
  uint16_t netkey_index; /**< Network key index for which the state applies */
  uint8_t  value;        /**< Key refresh phase state of the node for the given
                              network key. Valid values are:
                                - 0: Normal operation
                                - 1: First phase of key refresh procedure
                                - 2: Second phase of key refresh procedure */
});

typedef struct sl_btmesh_evt_config_client_key_refresh_phase_status_s sl_btmesh_evt_config_client_key_refresh_phase_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_key_refresh_phase_status

/**
 * @addtogroup sl_btmesh_evt_config_client_lpn_polltimeout_status sl_btmesh_evt_config_client_lpn_polltimeout_status
 * @{
 * @brief Status event for @ref sl_btmesh_config_client_get_lpn_polltimeout
 * command.
 */

/** @brief Identifier of the lpn_polltimeout_status event */
#define sl_btmesh_evt_config_client_lpn_polltimeout_status_id            0x182700a8

/***************************************************************************//**
 * @brief Data structure of the lpn_polltimeout_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_lpn_polltimeout_status_s
{
  uint16_t result;          /**< SL_STATUS_OK if successful. Error code
                                 otherwise. */
  uint32_t handle;          /**< Request handle */
  uint32_t poll_timeout_ms; /**< Poll timeout value, in milliseconds, for the
                                 specified LPN. The value reported is zero if
                                 the queried Friend does not have an ongoing
                                 friendship with the specified LPN. */
});

typedef struct sl_btmesh_evt_config_client_lpn_polltimeout_status_s sl_btmesh_evt_config_client_lpn_polltimeout_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_lpn_polltimeout_status

/**
 * @addtogroup sl_btmesh_evt_config_client_dcd_data sl_btmesh_evt_config_client_dcd_data
 * @{
 * @brief Event reporting queried composition data page contents
 *
 * The contents are requested using the @ref sl_btmesh_config_client_get_dcd
 * command. More than one event may be generated. Page contents are terminated
 * by a @ref sl_btmesh_evt_config_client_dcd_data_end event. Note that the
 * interpretation of the received data is page-specific. Page 0 contains the
 * element and model layout of the node.
 */

/** @brief Identifier of the dcd_data event */
#define sl_btmesh_evt_config_client_dcd_data_id                          0x192700a8

/***************************************************************************//**
 * @brief Data structure of the dcd_data event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_dcd_data_s
{
  uint32_t   handle; /**< Request handle */
  uint8_t    page;   /**< Composition data page containing data */
  uint8array data;   /**< Composition data page contents */
});

typedef struct sl_btmesh_evt_config_client_dcd_data_s sl_btmesh_evt_config_client_dcd_data_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_dcd_data

/**
 * @addtogroup sl_btmesh_evt_config_client_dcd_data_end sl_btmesh_evt_config_client_dcd_data_end
 * @{
 * @brief Terminating event for node composition data
 */

/** @brief Identifier of the dcd_data_end event */
#define sl_btmesh_evt_config_client_dcd_data_end_id                      0x1a2700a8

/***************************************************************************//**
 * @brief Data structure of the dcd_data_end event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_dcd_data_end_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_dcd_data_end_s sl_btmesh_evt_config_client_dcd_data_end_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_dcd_data_end

/**
 * @addtogroup sl_btmesh_evt_config_client_reset_status sl_btmesh_evt_config_client_reset_status
 * @{
 * @brief Indicate a node has received a @ref
 * sl_btmesh_config_client_reset_node.
 */

/** @brief Identifier of the reset_status event */
#define sl_btmesh_evt_config_client_reset_status_id                      0x1b2700a8

/***************************************************************************//**
 * @brief Data structure of the reset_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_config_client_reset_status_s
{
  uint16_t result; /**< SL_STATUS_OK if successful. Error code otherwise. */
  uint32_t handle; /**< Request handle */
});

typedef struct sl_btmesh_evt_config_client_reset_status_s sl_btmesh_evt_config_client_reset_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_config_client_reset_status

/***************************************************************************//**
 *
 * Cancel an ongoing request releasing resources allocated at the Configuration
 * Client. Note that this call does no undo any setting a node may have made if
 * it had received the request already.
 *
 * @param[in] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_cancel_request(uint32_t handle);

/***************************************************************************//**
 *
 * Get the status of a pending request.
 *
 * @param[in] handle Request handle
 * @param[out] server_address Address of the Configuration Server
 * @param[out] opcode Message opcode used in the request. Opcodes are defined in
 *   the Bluetooth mesh stack 1.0 specification.
 * @param[out] age_ms Time in milliseconds that the request has been pending
 * @param[out] remaining_ms Time in milliseconds before the request times out.
 *   Note that timeout may be adjusted if it's determined that the request is
 *   destined to an LPN, which may respond very slowly.
 * @param[out] friend_acked If non-zero, response has been acknowledged by a
 *   Friend node, so it is destined to an LPN and may take a long time to
 *   complete.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_request_status(uint32_t handle,
                                                       uint16_t *server_address,
                                                       uint16_t *opcode,
                                                       uint32_t *age_ms,
                                                       uint32_t *remaining_ms,
                                                       uint8_t *friend_acked);

/***************************************************************************//**
 *
 * Get the default timeout for the configuration client requests. If there is no
 * response when the timeout expires, a configuration request is considered to
 * have failed and an event with an error result will be generated. Note that,
 * if the Bluetooth mesh stack notices the request is destined to an LPN by
 * receiving an on-behalf-of acknowledgment from a Friend node, the timeout in
 * use will be changed to the LPN default timeout.
 *
 * @param[out] timeout_ms Timeout in milliseconds. Default timeout is 5 s (5000
 *   ms).
 * @param[out] lpn_timeout_ms Timeout in milliseconds when communicating with an
 *   LPN node. Default LPN timeout is 120 s (120000 ms).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_default_timeout(uint32_t *timeout_ms,
                                                        uint32_t *lpn_timeout_ms);

/***************************************************************************//**
 *
 * Set the default timeout for the configuration client requests.
 *
 * @param[in] timeout_ms Timeout in milliseconds. Default timeout is 5 s (5000
 *   ms).
 * @param[in] lpn_timeout_ms Timeout in milliseconds when communicating with an
 *   LPN node. Default LPN timeout is 120 s (120000 ms).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_default_timeout(uint32_t timeout_ms,
                                                        uint32_t lpn_timeout_ms);

/***************************************************************************//**
 *
 * Add a network key to a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Index of the network key to add
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_netkey_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_add_netkey(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint16_t netkey_index,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Remove a network key from a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Index of the network key to remove
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_netkey_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_remove_netkey(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint16_t netkey_index,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * List the network keys on a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_netkey_list
 *   - @ref sl_btmesh_evt_config_client_netkey_list_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_list_netkeys(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint32_t *handle);

/***************************************************************************//**
 *
 * Add an application key to a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] appkey_index Index of the application key to add
 * @param[in] netkey_index Index of the network key to bind the application key
 *   to on the node. Note that this may be different from the binding on other
 *   nodes or on the Configuration Client if desired.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_appkey_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_add_appkey(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint16_t appkey_index,
                                               uint16_t netkey_index,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Remove an application key from a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] appkey_index Index of the application key to remove
 * @param[in] netkey_index Index of the network key bound to the application key
 *   to on the node. Note that this may be different from the binding on other
 *   nodes or on the Configuration Client.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_appkey_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_remove_appkey(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint16_t appkey_index,
                                                  uint16_t netkey_index,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * List the application keys on a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Network key index for the key used as the query
 *   parameter. The result contains the indices of the application keys bound to
 *   this network key on the node.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_appkey_list
 *   - @ref sl_btmesh_evt_config_client_appkey_list_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_list_appkeys(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint32_t *handle);

/***************************************************************************//**
 *
 * Bind an application key to a model.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be configured
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] appkey_index Index of the application key to bind to the model
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_binding_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_bind_model(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t elem_index,
                                               uint16_t vendor_id,
                                               uint16_t model_id,
                                               uint16_t appkey_index,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Unbind an application key from a model
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be configured
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] appkey_index Index of the application key to unbind from the model
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_binding_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_unbind_model(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint8_t elem_index,
                                                 uint16_t vendor_id,
                                                 uint16_t model_id,
                                                 uint16_t appkey_index,
                                                 uint32_t *handle);

/***************************************************************************//**
 *
 * List application key bindings of a model.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be queried
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to query. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to query
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_bindings_list
 *   - @ref sl_btmesh_evt_config_client_bindings_list_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_list_bindings(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * Get the model publication state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be queried
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to query. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to query
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_pub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_model_pub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * Set the model publication state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be configured
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] address The address to publish to. It can be a unicast address, a
 *   virtual address, or a group address. It can also be the unassigned address
 *   to stop the model from publishing.
 * @param[in] appkey_index The application key index to use for the published
 *   messages.
 * @param[in] credentials Friendship credential flag. If zero, publication is
 *   done using normal credentials. If one, it is done with friendship
 *   credentials, meaning only the friend can decrypt the published message and
 *   relay it forward using the normal credentials. The default value is 0.
 * @param[in] ttl Publication time-to-live value
 * @param[in] period_ms Publication period in milliseconds. Note that the
 *   resolution of the publication period is limited by the specification to 100
 *   ms up to a period of 6.3 s, 1 s up to a period of 63 s, 10 s up to a period
 *   of 630 s, and 10 minutes above that. Maximum period allowed is 630 minutes.
 * @param[in] retransmit_count Publication retransmission count. Valid values
 *   range from 0 to 7.
 * @param[in] retransmit_interval_ms Publication retransmission interval in
 *   millisecond units. The range of value is 50 to 1600 ms, and the resolution
 *   of the value is 50 milliseconds.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_pub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_model_pub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t address,
                                                  uint16_t appkey_index,
                                                  uint8_t credentials,
                                                  uint8_t ttl,
                                                  uint32_t period_ms,
                                                  uint8_t retransmit_count,
                                                  uint16_t retransmit_interval_ms,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * Set the model publication state with a full virtual publication address.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be configured
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] address The Label UUID (full virtual address) to publish to
 * @param[in] appkey_index The application key index to use for the published
 *   messages
 * @param[in] credentials Friendship credential flag. If zero, publication is
 *   done using normal credentials. If one, it is done with friendship
 *   credentials, meaning only the friend can decrypt the published message and
 *   relay it forward using the normal credentials. The default value is 0.
 * @param[in] ttl Publication time-to-live value
 * @param[in] period_ms Publication period in milliseconds. Note that the
 *   resolution of the publication period is limited by the specification to 100
 *   ms up to a period of 6.3 s, 1 s up to a period of 63 s, 10 s up to a period
 *   of 630 s, and 10 minutes above that. Maximum period allowed is 630 minutes.
 * @param[in] retransmit_count Publication retransmission count. Valid values
 *   range from 0 to 7.
 * @param[in] retransmit_interval_ms Publication retransmission interval in
 *   millisecond units. The range of value is 50 to 1600 ms. The resolution of
 *   the value is 50 milliseconds.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_pub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_model_pub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 address,
                                                     uint16_t appkey_index,
                                                     uint8_t credentials,
                                                     uint8_t ttl,
                                                     uint32_t period_ms,
                                                     uint8_t retransmit_count,
                                                     uint16_t retransmit_interval_ms,
                                                     uint32_t *handle);

/***************************************************************************//**
 *
 * Add an address to the model subscription list.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The address to add to the subscription list. Note that
 *   the address has to be a group address.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_add_model_sub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * Add a Label UUID (full virtual address) to the model subscription list.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The full virtual address to add to the subscription
 *   list
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_add_model_sub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 sub_address,
                                                     uint32_t *handle);

/***************************************************************************//**
 *
 * Remove an address from the model subscription list.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The address to remove from the subscription list
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_remove_model_sub(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uint16_t sub_address,
                                                     uint32_t *handle);

/***************************************************************************//**
 *
 * Remove a Label UUID (full virtual address) from the model subscription list.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model to be configured
 *   resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The full virtual address to remove from the
 *   subscription list
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_remove_model_sub_va(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        uint8_t elem_index,
                                                        uint16_t vendor_id,
                                                        uint16_t model_id,
                                                        uuid_128 sub_address,
                                                        uint32_t *handle);

/***************************************************************************//**
 *
 * Set (overwrite) model subscription address list to a single address.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The address to set as the subscription list
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_model_sub(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  uint8_t elem_index,
                                                  uint16_t vendor_id,
                                                  uint16_t model_id,
                                                  uint16_t sub_address,
                                                  uint32_t *handle);

/***************************************************************************//**
 *
 * Set (overwrite) model subscription address list to a single virtual address.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[in] sub_address The full virtual address to set as the subscription
 *   list
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_model_sub_va(uint16_t enc_netkey_index,
                                                     uint16_t server_address,
                                                     uint8_t elem_index,
                                                     uint16_t vendor_id,
                                                     uint16_t model_id,
                                                     uuid_128 sub_address,
                                                     uint32_t *handle);

/***************************************************************************//**
 *
 * Clear (empty) the model subscription address list.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be configured resides on the node
 * @param[in] vendor_id Vendor ID for the model to configure. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to configure
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_model_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_clear_model_sub(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint8_t elem_index,
                                                    uint16_t vendor_id,
                                                    uint16_t model_id,
                                                    uint32_t *handle);

/***************************************************************************//**
 *
 * Get the subscription address list of a model.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] elem_index Index of the element where the model, which is about to
 *   be queried resides on the node
 * @param[in] vendor_id Vendor ID for the model to query. Use 0xFFFF for
 *   Bluetooth SIG models.
 * @param[in] model_id Model ID for the model to query
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_subs_list
 *   - @ref sl_btmesh_evt_config_client_subs_list_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_list_subs(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint8_t elem_index,
                                              uint16_t vendor_id,
                                              uint16_t model_id,
                                              uint32_t *handle);

/***************************************************************************//**
 *
 * Get the heartbeat publication state of a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_heartbeat_pub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_heartbeat_pub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint32_t *handle);

/***************************************************************************//**
 *
 * Set the heartbeat publication state of a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] destination_address Heartbeat publication destination address. The
 *   address can't be a virtual address. Note that it can be the unassigned
 *   address, in which case the heartbeat publishing is disabled.
 * @param[in] netkey_index Index of the network key used to encrypt heartbeat
 *   messages
 * @param[in] count_log Heartbeat publication count logarithm-of-2 setting.
 *   Valid values are as follows:
 *     - <b>0x00:</b> Heartbeat messages are not sent
 *     - <b>0x01 .. 0x11:</b> Node will send 2^(n-1) heartbeat messages
 *     - <b>0x12 .. 0xfe:</b> Prohibited
 *     - <b>0xff:</b> Hearbeat messages are sent indefinitely
 * @param[in] period_log Heartbeat publication period logarithm-of-2 setting.
 *   Valid values are as follows:
 *     - <b>0x00:</b> Heartbeat messages are not sent
 *     - <b>0x01 .. 0x11:</b> Node will send a heartbeat message every 2^(n-1)
 *       seconds
 *     - <b>0x12 .. 0xff:</b> Prohibited
 * @param[in] ttl Time-to-live value for heartbeat messages
 * @param[in] features @parblock
 *   Heartbeat trigger setting. For bits set in the bitmask, reconfiguration of
 *   the node feature associated with the bit will result in the node emitting a
 *   heartbeat message. Valid values are as follows:
 *     - <b>Bit 0:</b> Relay feature
 *     - <b>Bit 1:</b> Proxy feature
 *     - <b>Bit 2:</b> Friend feature
 *     - <b>Bit 3:</b> Low power feature
 *
 *   Remaining bits are reserved for future use.
 *   @endparblock
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_heartbeat_pub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_heartbeat_pub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint16_t destination_address,
                                                      uint16_t netkey_index,
                                                      uint8_t count_log,
                                                      uint8_t period_log,
                                                      uint8_t ttl,
                                                      uint16_t features,
                                                      uint32_t *handle);

/***************************************************************************//**
 *
 * Get the heartbeat subscription state of a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_heartbeat_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_heartbeat_sub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint32_t *handle);

/***************************************************************************//**
 *
 * Set the heartbeat subscription state of a node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] source_address Source address for heartbeat messages, which must
 *   be either a unicast address or the unassigned address, in which case
 *   heartbeat messages are not processed.
 * @param[in] destination_address Destination address for heartbeat messages.
 *   The address must be either the unicast address of the primary element of
 *   the node, a group address, or the unassigned address. If it is the
 *   unassigned address, heartbeat messages are not processed.
 * @param[in] period_log Heartbeat subscription period logarithm-of-2 setting.
 *   Valid values are as follows:
 *     - <b>0x00:</b> Heartbeat messages are not received
 *     - <b>0x01 .. 0x11:</b> Node will receive heartbeat messages for 2^(n-1)
 *       seconds
 *     - <b>0x12 .. 0xff:</b> Prohibited
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_heartbeat_sub_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_heartbeat_sub(uint16_t enc_netkey_index,
                                                      uint16_t server_address,
                                                      uint16_t source_address,
                                                      uint16_t destination_address,
                                                      uint8_t period_log,
                                                      uint32_t *handle);

/***************************************************************************//**
 *
 * Get node secure network beacon state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_beacon_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_beacon(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Set node secure network beacon state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] value Secure network beacon value to set. Valid values are:
 *     - 0: Node is not broadcasting secure network beacons
 *     - 1: Node is broadcasting secure network beacons
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_beacon_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_beacon(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t value,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Get node default TTL state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_default_ttl_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_default_ttl(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint32_t *handle);

/***************************************************************************//**
 *
 * Set node default TTL state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] value Default TTL value. Valid value range is from 2 to 127 for
 *   relayed PDUs, and 0 to indicate non-relayed PDUs
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_default_ttl_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_default_ttl(uint16_t enc_netkey_index,
                                                    uint16_t server_address,
                                                    uint8_t value,
                                                    uint32_t *handle);

/***************************************************************************//**
 *
 * Get node GATT proxy state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_gatt_proxy_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_gatt_proxy(uint16_t enc_netkey_index,
                                                   uint16_t server_address,
                                                   uint32_t *handle);

/***************************************************************************//**
 *
 * Set node GATT proxy state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] value GATT proxy value to set. Valid values are:
 *     - 0: Proxy feature is disabled
 *     - 1: Proxy feature is enabled
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_gatt_proxy_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_gatt_proxy(uint16_t enc_netkey_index,
                                                   uint16_t server_address,
                                                   uint8_t value,
                                                   uint32_t *handle);

/***************************************************************************//**
 *
 * Get node relay state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_relay_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_relay(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint32_t *handle);

/***************************************************************************//**
 *
 * Set node relay state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] value Relay value to set. Valid values are:
 *     - 0: Relay feature is disabled
 *     - 1: Relay feature is enabled
 * @param[in] retransmit_count Relay retransmit count. Valid values range from 0
 *   to 7; default value is 0 (no retransmissions).
 * @param[in] retransmit_interval_ms Relay retransmit interval in milliseconds.
 *   Valid values range from 10 ms to 320 ms, with a resolution of 10 ms. The
 *   value is ignored if the retransmission count is set to zero.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_relay_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_relay(uint16_t enc_netkey_index,
                                              uint16_t server_address,
                                              uint8_t value,
                                              uint8_t retransmit_count,
                                              uint16_t retransmit_interval_ms,
                                              uint32_t *handle);

/***************************************************************************//**
 *
 * Get node network transmit state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_network_transmit_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_network_transmit(uint16_t enc_netkey_index,
                                                         uint16_t server_address,
                                                         uint32_t *handle);

/***************************************************************************//**
 *
 * Set node network transmit state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] transmit_count Network transmit count. Valid values range from 1
 *   to 8; default value is 1 (single transmission; no retransmissions).
 * @param[in] transmit_interval_ms Network transmit interval in milliseconds.
 *   Valid values range from 10 ms to 320 ms, with a resolution of 10 ms. The
 *   value is ignored if the transmission count is set to one.
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_network_transmit_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_network_transmit(uint16_t enc_netkey_index,
                                                         uint16_t server_address,
                                                         uint8_t transmit_count,
                                                         uint16_t transmit_interval_ms,
                                                         uint32_t *handle);

/***************************************************************************//**
 *
 * Get node identity state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Network key index for which the state is queried
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_identity_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_identity(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint32_t *handle);

/***************************************************************************//**
 *
 * Set node identity state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Network key index for which the state is configured
 * @param[in] value Identity value to set. Valid values are:
 *     - 0: Node identity advertising is disabled
 *     - 1: Node identity advertising is enabled
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_identity_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_identity(uint16_t enc_netkey_index,
                                                 uint16_t server_address,
                                                 uint16_t netkey_index,
                                                 uint8_t value,
                                                 uint32_t *handle);

/***************************************************************************//**
 *
 * Get node friend state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_friend_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_friend(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Set node friend state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] value Friend value to set. Valid values are:
 *     - 0: Friend feature is not enabled
 *     - 1: Friend feature is enabled
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_friend_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_set_friend(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint8_t value,
                                               uint32_t *handle);

/***************************************************************************//**
 *
 * Get node key refresh phase state.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] netkey_index Network key index for which the state is queried
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_key_refresh_phase_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_key_refresh_phase(uint16_t enc_netkey_index,
                                                          uint16_t server_address,
                                                          uint16_t netkey_index,
                                                          uint32_t *handle);

/***************************************************************************//**
 *
 * Get the LPN poll timeout from a Friend node.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] lpn_address LPN address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_lpn_polltimeout_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_lpn_polltimeout(uint16_t enc_netkey_index,
                                                        uint16_t server_address,
                                                        uint16_t lpn_address,
                                                        uint32_t *handle);

/***************************************************************************//**
 *
 * Get composition data of a device.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] page Composition data page to query
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_dcd_data
 *   - @ref sl_btmesh_evt_config_client_dcd_data_end
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_get_dcd(uint16_t enc_netkey_index,
                                            uint16_t server_address,
                                            uint8_t page,
                                            uint32_t *handle);

/***************************************************************************//**
 *
 * Request a node to unprovision itself. Use this function when a node is
 * removed from the network.
 *
 * @param[in] enc_netkey_index Network key used to encrypt the request on the
 *   network layer
 * @param[in] server_address Destination node primary element address
 * @param[out] handle Request handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_config_client_reset_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_config_client_reset_node(uint16_t enc_netkey_index,
                                               uint16_t server_address,
                                               uint32_t *handle);

/** @} */ // end addtogroup sl_btmesh_config_client

/**
 * @addtogroup sl_btmesh_sensor_server Bluetooth Mesh Sensor Server Model
 * @{
 *
 * @brief Bluetooth Mesh Sensor Server Model
 *
 * This class provides the commands and messages to interface with the Sensor
 * Server model.
 *
 * A Sensor State consists of four states:
 *   - Sensor Descriptor
 *   - Sensor Setting
 *   - Sensor Cadence
 *   - Measurement value
 *
 * A multisensor setup is possible by having multiple sensor states within the
 * same model, provided that each sensor has a unique Sensor Property ID.
 *
 * Sensor Descriptor states are constant. Therefore, the stack can cache them
 * and enumerate the present sensors to clients when requested.
 *
 * Currently, the Sensor Server model does not cache the measurement data,
 * sensor settings, or cadence. When a client is querying sensor data, the
 * requests will be propagated to the application.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_sensor_server_init_id                              0x00470028
#define sl_btmesh_cmd_sensor_server_deinit_id                            0x01470028
#define sl_btmesh_cmd_sensor_server_send_descriptor_status_id            0x02470028
#define sl_btmesh_cmd_sensor_server_send_status_id                       0x03470028
#define sl_btmesh_cmd_sensor_server_send_column_status_id                0x04470028
#define sl_btmesh_cmd_sensor_server_send_series_status_id                0x05470028
#define sl_btmesh_rsp_sensor_server_init_id                              0x00470028
#define sl_btmesh_rsp_sensor_server_deinit_id                            0x01470028
#define sl_btmesh_rsp_sensor_server_send_descriptor_status_id            0x02470028
#define sl_btmesh_rsp_sensor_server_send_status_id                       0x03470028
#define sl_btmesh_rsp_sensor_server_send_column_status_id                0x04470028
#define sl_btmesh_rsp_sensor_server_send_series_status_id                0x05470028

/**
 * @addtogroup sl_btmesh_evt_sensor_server_get_request sl_btmesh_evt_sensor_server_get_request
 * @{
 * @brief Indicate an incoming Sensor Get request to get the Sensor Data
 * state(s)
 *
 * This event must be replied to by sending the Sensor Status message.
 */

/** @brief Identifier of the get_request event */
#define sl_btmesh_evt_sensor_server_get_request_id                       0x014700a8

/***************************************************************************//**
 * @brief Data structure of the get_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_server_get_request_s
{
  uint16_t server_address; /**< Destination server address */
  uint16_t elem_index;     /**< Server model element index */
  uint16_t client_address; /**< Source client address */
  uint16_t appkey_index;   /**< The application key index to use */
  uint8_t  flags;          /**< No flags defined currently */
  uint16_t property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                0x0ffff for a specific device property, or
                                0x0000 to get the values of all sensors. */
});

typedef struct sl_btmesh_evt_sensor_server_get_request_s sl_btmesh_evt_sensor_server_get_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_server_get_request

/**
 * @addtogroup sl_btmesh_evt_sensor_server_get_column_request sl_btmesh_evt_sensor_server_get_column_request
 * @{
 * @brief Indicate an incoming Sensor Column Get message to get Sensor Series
 * Column state
 *
 * The event must be replied to by sending a Sensor Column Status message.
 */

/** @brief Identifier of the get_column_request event */
#define sl_btmesh_evt_sensor_server_get_column_request_id                0x024700a8

/***************************************************************************//**
 * @brief Data structure of the get_column_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_server_get_column_request_s
{
  uint16_t   server_address; /**< Destination server address */
  uint16_t   elem_index;     /**< Server model element index */
  uint16_t   client_address; /**< Source client model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array column_ids;     /**< Raw value identifying a column */
});

typedef struct sl_btmesh_evt_sensor_server_get_column_request_s sl_btmesh_evt_sensor_server_get_column_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_server_get_column_request

/**
 * @addtogroup sl_btmesh_evt_sensor_server_get_series_request sl_btmesh_evt_sensor_server_get_series_request
 * @{
 * @brief Indicate an incoming Sensor Series Get message to get the Sensor
 * Series Column states
 *
 * This event must be replied to by sending a Sensor Series Status message.
 */

/** @brief Identifier of the get_series_request event */
#define sl_btmesh_evt_sensor_server_get_series_request_id                0x034700a8

/***************************************************************************//**
 * @brief Data structure of the get_series_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_server_get_series_request_s
{
  uint16_t   server_address; /**< Destination server address */
  uint16_t   elem_index;     /**< Server model element index */
  uint16_t   client_address; /**< Source client address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array column_ids;     /**< Optional raw values identifying starting and
                                  ending columns */
});

typedef struct sl_btmesh_evt_sensor_server_get_series_request_s sl_btmesh_evt_sensor_server_get_series_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_server_get_series_request

/**
 * @addtogroup sl_btmesh_evt_sensor_server_publish sl_btmesh_evt_sensor_server_publish
 * @{
 * @brief Indicate that the publishing period timer elapsed and the app should
 * publish its state.
 */

/** @brief Identifier of the publish event */
#define sl_btmesh_evt_sensor_server_publish_id                           0x044700a8

/***************************************************************************//**
 * @brief Data structure of the publish event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_server_publish_s
{
  uint16_t elem_index; /**< Client model element index */
  uint32_t period_ms;  /**< The current publishing period that can be used to
                            estimate the next tick, e.g., when the state should
                            be reported at higher frequency. */
});

typedef struct sl_btmesh_evt_sensor_server_publish_s sl_btmesh_evt_sensor_server_publish_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_server_publish

/***************************************************************************//**
 *
 * Initialize the Sensor Server model with Sensor Descriptors present at the
 * element. The descriptors are cached and Descriptor Get queries are served
 * without propagating it to the application. All incoming client queries are
 * checked against the cached data. However, only valid requests related to
 * existing sensors are propagated to the the application.
 *
 * @param[in] elem_index Server model element index
 * @param[in] descriptors_len Length of data in @p descriptors
 * @param[in] descriptors @parblock
 *   Sensor Descriptor State structures submitted as a byte array
 *
 *   A sensor descriptor represents the attributes describing the sensor data.
 *   It does not change throughout the lifetime of the element.
 *
 *   The following fields are required:
 *     - Sensor Property ID: 16 bits
 *     - Sensor Positive Tolerance: 12 bits
 *     - Sensor Negative Tolerance: 12 bits
 *     - Sensor Sampling Function: 8 bits
 *     - Sensor Measurement Period: 8 bits
 *     - Sensor Update Interval: 8 bits
 *
 *   Sensor Property ID is a 2-octet value referencing a device property that
 *   describes the meaning and the format of data reported by the sensor. The
 *   value 0x0000 is prohibited. Valid range is 0x0001 to 0xFFFF.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_init(uint16_t elem_index,
                                         size_t descriptors_len,
                                         const uint8_t* descriptors);

/***************************************************************************//**
 *
 * Deinitialize the Sensor Server functionality.
 * Note that the heap reserved space cannot be freed or reallocated.
 * Reinitializing with greater number of sensors than before will eventually
 * return an out of memory error until the device is reset.
 *
 * @param[in] elem_index Server model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Send a Descriptor Status message either as a reply to a Get Descriptor client
 * request.
 *
 * @param[in] client_address Destination client address. The address 0x0000 can
 *   be used to publish the message according to model configuration
 * @param[in] elem_index Server model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] descriptors_len Length of data in @p descriptors
 * @param[in] descriptors Serialized Sensor Descriptor states for all sensors
 *   within the element consisting one or more 8 bytes structures as follows:
 *     - Sensor Property ID: 16 bits
 *     - Sensor Positive Tolerance: 12 bits
 *     - Sensor Negative Tolerance: 12 bits
 *     - Sensor Sampling Function: 8 bits
 *     - Sensor Measurement Period: 8 bits
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_send_descriptor_status(uint16_t client_address,
                                                           uint16_t elem_index,
                                                           uint16_t appkey_index,
                                                           uint8_t flags,
                                                           size_t descriptors_len,
                                                           const uint8_t* descriptors);

/***************************************************************************//**
 *
 * Send Sensor Status message as a reply to a Get client request or as an
 * unsolicited message.
 *
 * @param[in] client_address Destination client address. The address 0x0000 can
 *   be used to publish the message according to model configuration
 * @param[in] elem_index Setup Server model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] sensor_data_len Length of data in @p sensor_data
 * @param[in] sensor_data @parblock
 *   Serialized Sensor Data consisting of one or more Sensor state for each
 *   sensor within the element. To simplify processing, the byte array is in TLV
 *   format:
 *     - 1st Property ID: 16 bits
 *     - Value Length: 8 bits
 *     - Value: variable
 *     - 2nd Property ID: 16 bits
 *     - Value Length: 8 bits
 *     - Value: variable
 *     - ...
 *
 *   If sensor data was requested for a Property ID that does not exist within
 *   the element, the reply must contain the given Property ID with zero length.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_send_status(uint16_t client_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                size_t sensor_data_len,
                                                const uint8_t* sensor_data);

/***************************************************************************//**
 *
 * Send Column Status message as a response to a Column Get client request or as
 * an unsolicited message
 *
 * @param[in] client_address Destination client address. The address 0x0000 can
 *   be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] sensor_data_len Length of data in @p sensor_data
 * @param[in] sensor_data @parblock
 *   Byte array containing the serialized Sensor Series Column state in the
 *   following format:
 *     - Sensor Raw Value X, variable length raw value representing the left
 *       corner of a column
 *     - Sensor Column Width, variable length raw value representing the width
 *       of the column
 *     - Sensor Raw Value Y, variable length raw value representing the height
 *       of the column
 *
 *   If the Property ID or the column ID (Raw value X) does not exist, the reply
 *   must contain only these two fields, omitting the optional Column Width and
 *   Raw Value Y fields.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_send_column_status(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint8_t flags,
                                                       uint16_t property_id,
                                                       size_t sensor_data_len,
                                                       const uint8_t* sensor_data);

/***************************************************************************//**
 *
 * Send Series Status message as a response to a Series Get client request or as
 * an unsolicited message.
 *
 * @param[in] client_address Destination client address. The address 0x0000 can
 *   be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] sensor_data_len Length of data in @p sensor_data
 * @param[in] sensor_data @parblock
 *   Byte array containing the serialized sequence of Sensor Series Column
 *   states in the following format:
 *     - 1st Sensor Raw Value X, variable length raw value representing the left
 *       corner of a column
 *     - 1st Sensor Column Width, variable length raw value representing the
 *       width of the column
 *     - 1st Sensor Raw Value Y, variable length raw value representing the
 *       height of the column
 *     - ...
 *     - Nth Sensor Raw Value X, variable length raw value representing the left
 *       corner of a column
 *     - Nth Sensor Column Width, variable length raw value representing the
 *       width of the column
 *     - Nth Sensor Raw Value Y, variable length raw value representing the
 *       height of the column
 *
 *   If Property ID does not exist in the element, the reply must contain only
 *   the given Property ID, omitting the other optional fields to column
 *   identifiers and column values.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_server_send_series_status(uint16_t client_address,
                                                       uint16_t elem_index,
                                                       uint16_t appkey_index,
                                                       uint8_t flags,
                                                       uint16_t property_id,
                                                       size_t sensor_data_len,
                                                       const uint8_t* sensor_data);

/** @} */ // end addtogroup sl_btmesh_sensor_server

/**
 * @addtogroup sl_btmesh_sensor_setup_server Bluetooth Mesh Sensor Setup Server
 * @{
 *
 * @brief Bluetooth Mesh Sensor Setup Server
 *
 * This class provides the commands and messages to interface with the Sensor
 * Setup Server model. Elements containing sensor model must have a setup server
 * model attached. Therefore, it is initialized/deinitialized internally
 * together with the server model.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_sensor_setup_server_send_cadence_status_id         0x00480028
#define sl_btmesh_cmd_sensor_setup_server_send_settings_status_id        0x01480028
#define sl_btmesh_cmd_sensor_setup_server_send_setting_status_id         0x02480028
#define sl_btmesh_rsp_sensor_setup_server_send_cadence_status_id         0x00480028
#define sl_btmesh_rsp_sensor_setup_server_send_settings_status_id        0x01480028
#define sl_btmesh_rsp_sensor_setup_server_send_setting_status_id         0x02480028

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_get_cadence_request sl_btmesh_evt_sensor_setup_server_get_cadence_request
 * @{
 * @brief Indicate an incoming Sensor Cadence Get request
 *
 * This event must be replied to by sending a Sensor Cadence Status message.
 */

/** @brief Identifier of the get_cadence_request event */
#define sl_btmesh_evt_sensor_setup_server_get_cadence_request_id         0x004800a8

/***************************************************************************//**
 * @brief Data structure of the get_cadence_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_get_cadence_request_s
{
  uint16_t server_address; /**< Address the request was directed to, either the
                                server's unicast address or a group address the
                                server subscribes to */
  uint16_t elem_index;     /**< Setup Server model element index */
  uint16_t client_address; /**< Requesting client model's address */
  uint16_t appkey_index;   /**< The application key index to use */
  uint8_t  flags;          /**< No flags defined currently */
  uint16_t property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                0x0ffff for a specific device property, the
                                value 0x0000 is prohibited. */
});

typedef struct sl_btmesh_evt_sensor_setup_server_get_cadence_request_s sl_btmesh_evt_sensor_setup_server_get_cadence_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_get_cadence_request

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_set_cadence_request sl_btmesh_evt_sensor_setup_server_set_cadence_request
 * @{
 * @brief Indicate an incoming Sensor Cadence Set request, which can be replied
 * to by sending a Sensor Cadence Status message
 *
 * Only Sensor Cadence Set (acknowledged) request results in a direct reply. In
 * addition, configuration changes must be reported by publishing the updated
 * cadence state according to model configuration.
 */

/** @brief Identifier of the set_cadence_request event */
#define sl_btmesh_evt_sensor_setup_server_set_cadence_request_id         0x014800a8

/***************************************************************************//**
 * @brief Data structure of the set_cadence_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_set_cadence_request_s
{
  uint16_t   server_address; /**< Address the request was directed to, either
                                  the server's unicast address or a group
                                  address the server subscribes to */
  uint16_t   elem_index;     /**< Setup Server model element index */
  uint16_t   client_address; /**< Requesting client model's address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< Bit 1 (0x02) defines whether response is
                                  required. If set to 1, the client sent the
                                  message with SET CADENCE opcode and expects a
                                  CADENCE STATUS message in return. */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8_t    period_divisor; /**< Fast Cadence Period Divisor, 7 bits defining
                                  the divisor for the Publish Period */
  uint8_t    trigger_type;   /**< Status Trigger Type, 1 bit: 0 = discrete
                                  value, 1 = delta percentage */
  uint8array params;         /**< Optional byte array containing serialized
                                  fields of Sensor Cadence state, excluding the
                                  property ID, period divisor, and trigger type
                                    - Fast Cadence Period Divisor, 7 bits
                                    - Status Trigger type, 1 bit (0 = discrete
                                      value, 1 = percentage)
                                    - Status Trigger Delta Down, variable length
                                    - Status Trigger Delta Up, variable length
                                    - Status Min Interval, 8 bits, representing
                                      a power of 2 milliseconds. Valid range is
                                      0-26
                                    - Fast Cadence Low, variable length, lower
                                      bound for the fast cadence range
                                    - Low Cadence Low, variable length, higher
                                      bound for the fast cadence range */
});

typedef struct sl_btmesh_evt_sensor_setup_server_set_cadence_request_s sl_btmesh_evt_sensor_setup_server_set_cadence_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_set_cadence_request

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_get_settings_request sl_btmesh_evt_sensor_setup_server_get_settings_request
 * @{
 * @brief Indicate an incoming Sensor Settings Get message to fetch the Sensor
 * Setting Property IDs configured for the given Sensor
 *
 * This event must be replied to by sending a Sensor Settings Status message.
 */

/** @brief Identifier of the get_settings_request event */
#define sl_btmesh_evt_sensor_setup_server_get_settings_request_id        0x024800a8

/***************************************************************************//**
 * @brief Data structure of the get_settings_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_get_settings_request_s
{
  uint16_t server_address; /**< Address the request was directed to, either the
                                server's unicast address or a group address the
                                server subscribes to */
  uint16_t elem_index;     /**< Setup Server model element index */
  uint16_t client_address; /**< Requesting client model's address */
  uint16_t appkey_index;   /**< The application key index to use */
  uint8_t  flags;          /**< No flags defined currently */
  uint16_t property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                0x0ffff for a specific device property, the
                                value 0x0000 is prohibited. */
});

typedef struct sl_btmesh_evt_sensor_setup_server_get_settings_request_s sl_btmesh_evt_sensor_setup_server_get_settings_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_get_settings_request

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_get_setting_request sl_btmesh_evt_sensor_setup_server_get_setting_request
 * @{
 * @brief Indicate an incoming Sensor Get Setting request to fetch the value of
 * a setting to a given sensor of a setting given by its ID
 *
 * This event must be replied to by sending a Sensor Setting Status message.
 */

/** @brief Identifier of the get_setting_request event */
#define sl_btmesh_evt_sensor_setup_server_get_setting_request_id         0x034800a8

/***************************************************************************//**
 * @brief Data structure of the get_setting_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_get_setting_request_s
{
  uint16_t server_address; /**< Address the request was directed to, either the
                                server's unicast address, or a group address the
                                server subscribes to */
  uint16_t elem_index;     /**< Setup Server model element index */
  uint16_t client_address; /**< Requesting client model's address */
  uint16_t appkey_index;   /**< The application key index to use */
  uint8_t  flags;          /**< No flags defined currently */
  uint16_t property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                0x0ffff for a specific device property, the
                                value 0x0000 is prohibited. */
  uint16_t setting_id;     /**< Sensor Setting Property ID field identifying the
                                device property of a setting. Range: 0x0001 -
                                0xffff, 0x0000 is prohibited. */
});

typedef struct sl_btmesh_evt_sensor_setup_server_get_setting_request_s sl_btmesh_evt_sensor_setup_server_get_setting_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_get_setting_request

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_set_setting_request sl_btmesh_evt_sensor_setup_server_set_setting_request
 * @{
 * @brief Indicate an incoming Sensor Set Setting request, which can be replied
 * to by sending a Sensor Setting Status message
 *
 * Only Setting Set (acknowledged) request is replied directly to the client. In
 * addition, configuration changes must be reported by publishing the new state
 * according to model publishing configuration.
 */

/** @brief Identifier of the set_setting_request event */
#define sl_btmesh_evt_sensor_setup_server_set_setting_request_id         0x044800a8

/***************************************************************************//**
 * @brief Data structure of the set_setting_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_set_setting_request_s
{
  uint16_t   server_address; /**< Address the request was directed to, either
                                  the server's unicast address, or a group
                                  address the server subscribes to. */
  uint16_t   elem_index;     /**< Setup Server model element index */
  uint16_t   client_address; /**< Requesting client address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< Bit 1 (0x02) defines whether response is
                                  required. If set to 1, the client sent the
                                  message with SET SETTING opcode and expects a
                                  SETTING STATUS message in return. */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint16_t   setting_id;     /**< Sensor Setting Property ID field identifying
                                  the device property of a setting. Range:
                                  0x0001 - 0xffff, 0x0000 is prohibited. */
  uint8array raw_value;      /**< Sensor Setting raw value. Size and
                                  representation depends on the type defined by
                                  the Sensor Setting Property ID. */
});

typedef struct sl_btmesh_evt_sensor_setup_server_set_setting_request_s sl_btmesh_evt_sensor_setup_server_set_setting_request_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_set_setting_request

/**
 * @addtogroup sl_btmesh_evt_sensor_setup_server_publish sl_btmesh_evt_sensor_setup_server_publish
 * @{
 * @brief Indicate that the publishing period timer elapsed and the app
 * should/can publish its state.
 */

/** @brief Identifier of the publish event */
#define sl_btmesh_evt_sensor_setup_server_publish_id                     0x054800a8

/***************************************************************************//**
 * @brief Data structure of the publish event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_setup_server_publish_s
{
  uint16_t elem_index; /**< Client model element index */
  uint32_t period_ms;  /**< The current publishing period that can be used to
                            estimate the next tick, e.g., when the state should
                            be reported at higher frequency. */
});

typedef struct sl_btmesh_evt_sensor_setup_server_publish_s sl_btmesh_evt_sensor_setup_server_publish_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_setup_server_publish

/***************************************************************************//**
 *
 * Reply to a Get/Set Cadence client request with a Cadence Status message. Only
 * Cadence Set (acknowledged) must be answered by sending the status message to
 * the client. In addition, configuration changes must be published according to
 * model publishing configuration.
 *
 * @param[in] client_address Destination client address The address 0x0000 can
 *   be used to publish the message according model configuration instead of a
 *   direct reply.
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] params_len Length of data in @p params
 * @param[in] params Optional byte array containing the serialized Sensor
 *   Cadence state, excluding the property ID. If not empty, the state consists
 *   of the following fields:
 *     - Fast Cadence Period Divisor, 7 bits
 *     - Status Trigger type, 1 bits (0 = discrete value, 1 = percentage)
 *     - Status Trigger Delta Down, variable length
 *     - Status Trigger Delta Up, variable length
 *     - Status Min Interval, 8 bits, representing a power of 2 milliseconds.
 *       Valid range is 0-26
 *     - Fast Cadence Low, variable length, lower bound for the fast cadence
 *       range
 *     - Low Cadence Low, variable length, higher bound for the fast cadence
 *       range
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_setup_server_send_cadence_status(uint16_t client_address,
                                                              uint16_t elem_index,
                                                              uint16_t appkey_index,
                                                              uint8_t flags,
                                                              uint16_t property_id,
                                                              size_t params_len,
                                                              const uint8_t* params);

/***************************************************************************//**
 *
 * Reply to a Get Settings client request with a Settings Status message.
 *
 * @param[in] client_address Destination client model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] setting_ids_len Length of data in @p setting_ids
 * @param[in] setting_ids Array of 16-bit Setting Property IDs of the settings
 *   the given sensor has
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_setup_server_send_settings_status(uint16_t client_address,
                                                               uint16_t elem_index,
                                                               uint16_t appkey_index,
                                                               uint8_t flags,
                                                               uint16_t property_id,
                                                               size_t setting_ids_len,
                                                               const uint8_t* setting_ids);

/***************************************************************************//**
 *
 * Reply to a Get/Set Setting client request with a Setting Status message. Only
 * Set Setting (acknowledged) request must be answered by sending a reply to the
 * unicast address of the sender. In addition, configuration changes must be
 * published if model publishing is set up.
 *
 * @param[in] client_address Destination client model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] setting_id Sensor Setting Property ID field identifying the device
 *   property of a setting. Range: 0x0001 - 0xffff, 0x0000 is prohibited.
 * @param[in] raw_value_len Length of data in @p raw_value
 * @param[in] raw_value Sensor Setting raw value. Size and representation
 *   depends on the type defined by the Sensor Setting Property ID.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_setup_server_send_setting_status(uint16_t client_address,
                                                              uint16_t elem_index,
                                                              uint16_t appkey_index,
                                                              uint8_t flags,
                                                              uint16_t property_id,
                                                              uint16_t setting_id,
                                                              size_t raw_value_len,
                                                              const uint8_t* raw_value);

/** @} */ // end addtogroup sl_btmesh_sensor_setup_server

/**
 * @addtogroup sl_btmesh_sensor_client Bluetooth Mesh Sensor Client
 * @{
 *
 * @brief Bluetooth Mesh Sensor Client
 *
 * This class provides the commands and messages to interface with the Sensor
 * Client model.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_sensor_client_init_id                              0x00490028
#define sl_btmesh_cmd_sensor_client_deinit_id                            0x01490028
#define sl_btmesh_cmd_sensor_client_get_descriptor_id                    0x02490028
#define sl_btmesh_cmd_sensor_client_get_id                               0x03490028
#define sl_btmesh_cmd_sensor_client_get_column_id                        0x04490028
#define sl_btmesh_cmd_sensor_client_get_series_id                        0x05490028
#define sl_btmesh_cmd_sensor_client_get_cadence_id                       0x06490028
#define sl_btmesh_cmd_sensor_client_set_cadence_id                       0x07490028
#define sl_btmesh_cmd_sensor_client_get_settings_id                      0x08490028
#define sl_btmesh_cmd_sensor_client_get_setting_id                       0x09490028
#define sl_btmesh_cmd_sensor_client_set_setting_id                       0x0a490028
#define sl_btmesh_rsp_sensor_client_init_id                              0x00490028
#define sl_btmesh_rsp_sensor_client_deinit_id                            0x01490028
#define sl_btmesh_rsp_sensor_client_get_descriptor_id                    0x02490028
#define sl_btmesh_rsp_sensor_client_get_id                               0x03490028
#define sl_btmesh_rsp_sensor_client_get_column_id                        0x04490028
#define sl_btmesh_rsp_sensor_client_get_series_id                        0x05490028
#define sl_btmesh_rsp_sensor_client_get_cadence_id                       0x06490028
#define sl_btmesh_rsp_sensor_client_set_cadence_id                       0x07490028
#define sl_btmesh_rsp_sensor_client_get_settings_id                      0x08490028
#define sl_btmesh_rsp_sensor_client_get_setting_id                       0x09490028
#define sl_btmesh_rsp_sensor_client_set_setting_id                       0x0a490028

/**
 * @addtogroup sl_btmesh_evt_sensor_client_descriptor_status sl_btmesh_evt_sensor_client_descriptor_status
 * @{
 * @brief Indicate an incoming Sensor Descriptor Status message
 */

/** @brief Identifier of the descriptor_status event */
#define sl_btmesh_evt_sensor_client_descriptor_status_id                 0x004900a8

/***************************************************************************//**
 * @brief Data structure of the descriptor_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_descriptor_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use. */
  uint8_t    flags;          /**< No flags defined currently */
  uint8array descriptors;    /**< One or more Sensor Descriptor states (N times
                                  8 bytes) As a reply to a Get message
                                  referencing a sensor that does not exist, the
                                  array contains only the requested Property ID.
                                  Format of the Descriptor state is as follows:
                                    - Property ID, 16 bits
                                    - Sensor Positive Tolerance, 12 bits
                                    - Sensor Negative Tolerance, 12 bits
                                    - Sensor Sampling Function, 8 bits
                                    - Sensor Measurement Period, 8 bits
                                    - Sensor Update Interval, 8 bits
                                  {/ul> */
});

typedef struct sl_btmesh_evt_sensor_client_descriptor_status_s sl_btmesh_evt_sensor_client_descriptor_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_descriptor_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_cadence_status sl_btmesh_evt_sensor_client_cadence_status
 * @{
 * @brief Indicate an incoming Sensor Cadence Status message
 */

/** @brief Identifier of the cadence_status event */
#define sl_btmesh_evt_sensor_client_cadence_status_id                    0x014900a8

/***************************************************************************//**
 * @brief Data structure of the cadence_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_cadence_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use. */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array params;         /**< Optional byte array containing serialized
                                  fields of Sensor Cadence state, excluding the
                                  property ID
                                    - Fast Cadence Period Divisor, 7 bits
                                    - Status Trigger type, 1 bit (0 = discrete
                                      value, 1 = percentage)
                                    - Status Trigger Delta Down, variable length
                                    - Status Trigger Delta Up, variable length
                                    - Status Min Interval, 8 bits, representing
                                      a power of 2 milliseconds. Valid range is
                                      0-26
                                    - Fast Cadence Low, variable length, lower
                                      bound for the fast cadence range
                                    - Low Cadence Low, variable length, higher
                                      bound for the fast cadence range */
});

typedef struct sl_btmesh_evt_sensor_client_cadence_status_s sl_btmesh_evt_sensor_client_cadence_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_cadence_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_settings_status sl_btmesh_evt_sensor_client_settings_status
 * @{
 * @brief Indicate an incoming Sensor Settings Status message
 */

/** @brief Identifier of the settings_status event */
#define sl_btmesh_evt_sensor_client_settings_status_id                   0x024900a8

/***************************************************************************//**
 * @brief Data structure of the settings_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_settings_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array setting_ids;    /**< Array of 16-bit Setting Property IDs of the
                                  settings the given sensor has */
});

typedef struct sl_btmesh_evt_sensor_client_settings_status_s sl_btmesh_evt_sensor_client_settings_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_settings_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_setting_status sl_btmesh_evt_sensor_client_setting_status
 * @{
 * @brief Indicate an incoming Sensor Setting Status message
 */

/** @brief Identifier of the setting_status event */
#define sl_btmesh_evt_sensor_client_setting_status_id                    0x034900a8

/***************************************************************************//**
 * @brief Data structure of the setting_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_setting_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint16_t   setting_id;     /**< Sensor Setting Property ID field identifying
                                  the device property of a setting. Range:
                                  0x0001 - 0xffff, 0x0000 is prohibited. */
  uint8array raw_value;      /**< Sensor Setting raw value. Size and
                                  representation depends on the type defined by
                                  the Sensor Setting Property ID. */
});

typedef struct sl_btmesh_evt_sensor_client_setting_status_s sl_btmesh_evt_sensor_client_setting_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_setting_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_status sl_btmesh_evt_sensor_client_status
 * @{
 * @brief Indicate an incoming Sensor Status event as a published data state
 * from the sensor server or as a reply to Sensor Get request
 */

/** @brief Identifier of the status event */
#define sl_btmesh_evt_sensor_client_status_id                            0x044900a8

/***************************************************************************//**
 * @brief Data structure of the status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint8array sensor_data;    /**< Serialized Sensor Data consisting of one or
                                  more Sensor state for each sensor within the
                                  element. To simplify processing, the byte
                                  array is in TLV format:
                                    - 1st Property ID: 16 bits
                                    - Value Length: 8 bits
                                    - Value: variable
                                    - 2nd Property ID: 16 bits
                                    - Value Length: 8 bits
                                    - Value: variable
                                    - ...

                                  If the requested Property ID does not exist at
                                  the server element, the reply contains only
                                  the given Property ID and zero length. */
});

typedef struct sl_btmesh_evt_sensor_client_status_s sl_btmesh_evt_sensor_client_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_column_status sl_btmesh_evt_sensor_client_column_status
 * @{
 * @brief Indicate an incoming Sensor Column Status event
 */

/** @brief Identifier of the column_status event */
#define sl_btmesh_evt_sensor_client_column_status_id                     0x054900a8

/***************************************************************************//**
 * @brief Data structure of the column_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_column_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array sensor_data;    /**< Byte array containing the serialized Sensor
                                  Series Column state in the following format:
                                    - Sensor Raw Value X, variable length raw
                                      value representing the left corner of a
                                      column
                                    - Sensor Column Width, variable length raw
                                      value representing the width of the column
                                    - Sensor Raw Value Y, variable length raw
                                      value representing the height of the
                                      column

                                  If the requested Property ID or column ID does
                                  not exist at the server elements, the reply
                                  status message contains only these two fields,
                                  omitting column width and height values. */
});

typedef struct sl_btmesh_evt_sensor_client_column_status_s sl_btmesh_evt_sensor_client_column_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_column_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_series_status sl_btmesh_evt_sensor_client_series_status
 * @{
 * @brief Indicate an incoming Sensor Series Status message
 */

/** @brief Identifier of the series_status event */
#define sl_btmesh_evt_sensor_client_series_status_id                     0x064900a8

/***************************************************************************//**
 * @brief Data structure of the series_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_series_status_s
{
  uint16_t   client_address; /**< Destination client model address */
  uint16_t   elem_index;     /**< Client model element index */
  uint16_t   server_address; /**< Source server model address */
  uint16_t   appkey_index;   /**< The application key index to use */
  uint8_t    flags;          /**< No flags defined currently */
  uint16_t   property_id;    /**< Property ID for the sensor. Range: 0x0001 -
                                  0x0ffff for a specific device property, the
                                  value 0x0000 is prohibited. */
  uint8array sensor_data;    /**< Byte array containing the serialized sequence
                                  of Sensor Serier Column states in the
                                  following format:
                                    - 1st Sensor Raw Value X, variable length
                                      raw value representing the left corner of
                                      a column
                                    - 1st Sensor Column Width, variable length
                                      raw value representing the width of the
                                      column
                                    - 1st Sensor Raw Value Y, variable length
                                      raw value representing the height of the
                                      column
                                    - ...
                                    - Nth Sensor Raw Value X, variable length
                                      raw value representing the left corner of
                                      a column
                                    - Nth Sensor Column Width, variable length
                                      raw value representing the width of the
                                      column
                                    - Nth Sensor Raw Value Y, variable length
                                      raw value representing the height of the
                                      column

                                  If a Property ID or column requested does not
                                  exist at the server element, the reply Series
                                  Status message contains only the given
                                  Property ID. */
});

typedef struct sl_btmesh_evt_sensor_client_series_status_s sl_btmesh_evt_sensor_client_series_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_series_status

/**
 * @addtogroup sl_btmesh_evt_sensor_client_publish sl_btmesh_evt_sensor_client_publish
 * @{
 * @brief Indicate that the publishing period timer elapsed and the app
 * should/can publish its state or any request.
 */

/** @brief Identifier of the publish event */
#define sl_btmesh_evt_sensor_client_publish_id                           0x074900a8

/***************************************************************************//**
 * @brief Data structure of the publish event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_sensor_client_publish_s
{
  uint16_t elem_index; /**< Client model element index */
  uint32_t period_ms;  /**< The current publishing period that can be used to
                            estimate the next tick, e.g., when the state should
                            be reported at higher frequency. */
});

typedef struct sl_btmesh_evt_sensor_client_publish_s sl_btmesh_evt_sensor_client_publish_t;

/** @} */ // end addtogroup sl_btmesh_evt_sensor_client_publish

/***************************************************************************//**
 *
 * Initialize the Sensor Client model. Sensor Client does not have any internal
 * configuration, it only activates the model in the Bluetooth mesh stack.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_init();

/***************************************************************************//**
 *
 * Deinitialize the Sensor Client model. There are no sensor-specific
 * configurations to reset. Normally, models are initialized at boot and never
 * deinitialized.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_deinit();

/***************************************************************************//**
 *
 * Get the Sensor Descriptor state of one specific sensor or all sensors within
 * a model. Results in a Sensor Descriptor Status event
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id ProperyID for the sensor (optional). Range: 0x0001 -
 *   0xffff for a specific device property ID or 0x0000 to get all (the value
 *   0x0000 is prohibited as a real ID).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_descriptor(uint16_t server_address,
                                                   uint16_t elem_index,
                                                   uint16_t appkey_index,
                                                   uint8_t flags,
                                                   uint16_t property_id);

/***************************************************************************//**
 *
 * Send a Sensor Get message to fetch the Sensor Data state of one specific
 * sensor given by its Property ID, results in a Sensor Status event. The
 * Property ID 0x0000 can be used to fetch all sensor values present at a server
 * element.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, or 0x0000 when not used to get values for
 *   all sensors present in the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get(uint16_t server_address,
                                        uint16_t elem_index,
                                        uint16_t appkey_index,
                                        uint8_t flags,
                                        uint16_t property_id);

/***************************************************************************//**
 *
 * Get a Sensor Series Column state, results in a Sensor Column Status event.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] column_id_len Length of data in @p column_id
 * @param[in] column_id Raw value identifying a column
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_column(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t flags,
                                               uint16_t property_id,
                                               size_t column_id_len,
                                               const uint8_t* column_id);

/***************************************************************************//**
 *
 * Get a Sensor Series Column state, which results in a Sensor Series Status
 * event.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] column_ids_len Length of data in @p column_ids
 * @param[in] column_ids Raw values identifying starting and ending columns
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_series(uint16_t server_address,
                                               uint16_t elem_index,
                                               uint16_t appkey_index,
                                               uint8_t flags,
                                               uint16_t property_id,
                                               size_t column_ids_len,
                                               const uint8_t* column_ids);

/***************************************************************************//**
 *
 * Send a Sensor Get Cadence message to get the Sensor Cadence state, which
 * results in a Sensor Cadence Status message.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_cadence(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id);

/***************************************************************************//**
 *
 * Send a Sensor Cadence Set message, either acknowledged or unacknowledged,
 * depending on the message flags. Acknowledged message results in a Cadence
 * Status reply message and event. The server must publish its new state in any
 * case.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET CADENCE message will be sent, zero will send SET CADENCE
 *   UNACKNOWLEDGED
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] params_len Length of data in @p params
 * @param[in] params Byte array containing serialized fields of Sensor Cadence
 *   state, excluding the property ID
 *     - Fast Cadence Period Divisor, 7 bits
 *     - Status Trigger type, 1 bit (0 = discrete value, 1 = percentage)
 *     - Status Trigger Delta Down, variable length
 *     - Status Trigger Delta Up, variable length
 *     - Status Min Interval, 8 bits, representing a power of 2 milliseconds.
 *       Valid range is 0-26
 *     - Fast Cadence Low, variable length, lower bound for the fast cadence
 *       range
 *     - Low Cadence Low, variable length, higher bound for the fast cadence
 *       range
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_set_cadence(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                size_t params_len,
                                                const uint8_t* params);

/***************************************************************************//**
 *
 * Send a Sensor Settings Get message to fetch the Sensor Property IDs present
 * for the given sensor, which results in a Sensor Settings Status event.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_settings(uint16_t server_address,
                                                 uint16_t elem_index,
                                                 uint16_t appkey_index,
                                                 uint8_t flags,
                                                 uint16_t property_id);

/***************************************************************************//**
 *
 * Send a Sensor Get Setting message to get the value of a specific setting for
 * the given sensor, which results in a Sensor Setting Status event.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags No flags defined currently
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] setting_id Sensor Setting Property ID field identifying the device
 *   property of a setting. Range: 0x0001 - 0xffff, 0x0000 is prohibited.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_setting(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                uint16_t setting_id);

/***************************************************************************//**
 *
 * Send Sensor Setting Set message to update the value of a specific setting for
 * the given sensor, either acknowledged or unacknowledged depending on the
 * message flags. Only acknowledged requests will have a direct Sensor Setting
 * Status reply. The server must publish its new state in any case.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET SETTING message is sent, zero will use SET SETTING
 *   UNACKNOWLEDGED.
 * @param[in] property_id Property ID for the sensor. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] setting_id Sensor Setting Property ID field identifying the device
 *   property of a setting. Range: 0x0001 - 0xffff, 0x0000 is prohibited.
 * @param[in] raw_value_len Length of data in @p raw_value
 * @param[in] raw_value Sensor Setting raw value. Size and representation
 *   depends on the type defined by the Sensor Setting Property ID.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_set_setting(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint16_t property_id,
                                                uint16_t setting_id,
                                                size_t raw_value_len,
                                                const uint8_t* raw_value);

/** @} */ // end addtogroup sl_btmesh_sensor_client

/**
 * @addtogroup sl_btmesh_lc_client Bluetooth Mesh Light Control Client Model
 * @{
 *
 * @brief Bluetooth Mesh Light Control Client Model
 *
 * Bluetooth Mesh LC Client model API provides functionality to send and receive
 * messages to/from the LC Server and LC Setup Server models.
 *
 * Throughout the API, the client model that's used is identified by its element
 * address and model ID, while the server model responding to the client model
 * requests is identified by its element address and model ID.
 *
 * The API has functions for querying server model states and requesting server
 * model state changes
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_lc_client_init_id                                  0x004c0028
#define sl_btmesh_cmd_lc_client_get_mode_id                              0x014c0028
#define sl_btmesh_cmd_lc_client_set_mode_id                              0x024c0028
#define sl_btmesh_cmd_lc_client_get_om_id                                0x044c0028
#define sl_btmesh_cmd_lc_client_set_om_id                                0x054c0028
#define sl_btmesh_cmd_lc_client_get_light_onoff_id                       0x074c0028
#define sl_btmesh_cmd_lc_client_set_light_onoff_id                       0x084c0028
#define sl_btmesh_cmd_lc_client_get_property_id                          0x094c0028
#define sl_btmesh_cmd_lc_client_set_property_id                          0x0a4c0028
#define sl_btmesh_rsp_lc_client_init_id                                  0x004c0028
#define sl_btmesh_rsp_lc_client_get_mode_id                              0x014c0028
#define sl_btmesh_rsp_lc_client_set_mode_id                              0x024c0028
#define sl_btmesh_rsp_lc_client_get_om_id                                0x044c0028
#define sl_btmesh_rsp_lc_client_set_om_id                                0x054c0028
#define sl_btmesh_rsp_lc_client_get_light_onoff_id                       0x074c0028
#define sl_btmesh_rsp_lc_client_set_light_onoff_id                       0x084c0028
#define sl_btmesh_rsp_lc_client_get_property_id                          0x094c0028
#define sl_btmesh_rsp_lc_client_set_property_id                          0x0a4c0028

/**
 * @addtogroup sl_btmesh_evt_lc_client_mode_status sl_btmesh_evt_lc_client_mode_status
 * @{
 * @brief Indicate an incoming LC Mode Status message
 */

/** @brief Identifier of the mode_status event */
#define sl_btmesh_evt_lc_client_mode_status_id                           0x004c00a8

/***************************************************************************//**
 * @brief Data structure of the mode_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_client_mode_status_s
{
  uint16_t destination_address; /**< Address of the client or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element for which received the
                                     status. */
  uint16_t server_address;      /**< Device which sent the status. */
  uint16_t appkey_index;        /**< Appkey used by server_address. */
  uint8_t  mode_status_value;   /**< Value reported by server. */
});

typedef struct sl_btmesh_evt_lc_client_mode_status_s sl_btmesh_evt_lc_client_mode_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_client_mode_status

/**
 * @addtogroup sl_btmesh_evt_lc_client_om_status sl_btmesh_evt_lc_client_om_status
 * @{
 * @brief Indicate an incoming LC Occupancy Mode Status message
 */

/** @brief Identifier of the om_status event */
#define sl_btmesh_evt_lc_client_om_status_id                             0x014c00a8

/***************************************************************************//**
 * @brief Data structure of the om_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_client_om_status_s
{
  uint16_t destination_address; /**< Address of the client or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element for which received the
                                     status. */
  uint16_t server_address;      /**< Device which sent the status. */
  uint16_t appkey_index;        /**< Appkey used by server_address. */
  uint8_t  om_status_value;     /**< Value reported by server. */
});

typedef struct sl_btmesh_evt_lc_client_om_status_s sl_btmesh_evt_lc_client_om_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_client_om_status

/**
 * @addtogroup sl_btmesh_evt_lc_client_light_onoff_status sl_btmesh_evt_lc_client_light_onoff_status
 * @{
 * @brief Indicate an incoming LC Light OnOff Status message
 */

/** @brief Identifier of the light_onoff_status event */
#define sl_btmesh_evt_lc_client_light_onoff_status_id                    0x024c00a8

/***************************************************************************//**
 * @brief Data structure of the light_onoff_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_client_light_onoff_status_s
{
  uint16_t destination_address; /**< Address of the client or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element for which received the
                                     status. */
  uint16_t server_address;      /**< Device which sent the status. */
  uint16_t appkey_index;        /**< Appkey used by server_address. */
  uint8_t  present_light_onoff; /**< Present value of the Light OnOff state */
  uint8_t  target_light_onoff;  /**< Target value of the Light OnOff state */
  uint32_t remaining_time_ms;   /**< Time (in milliseconds) remaining in
                                     transition */
});

typedef struct sl_btmesh_evt_lc_client_light_onoff_status_s sl_btmesh_evt_lc_client_light_onoff_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_client_light_onoff_status

/**
 * @addtogroup sl_btmesh_evt_lc_client_property_status sl_btmesh_evt_lc_client_property_status
 * @{
 * @brief Indicate an incoming LC Property Status message
 */

/** @brief Identifier of the property_status event */
#define sl_btmesh_evt_lc_client_property_status_id                       0x034c00a8

/***************************************************************************//**
 * @brief Data structure of the property_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_client_property_status_s
{
  uint16_t   destination_address; /**< Address of the client or the group
                                       address to which it was published. */
  uint16_t   elem_index;          /**< Index of the element for which received
                                       the status. */
  uint16_t   server_address;      /**< Device which sent the status. */
  uint16_t   appkey_index;        /**< Appkey used by server_address. */
  uint16_t   property_id;         /**< Property ID */
  uint8array property_value;      /**< Property value */
});

typedef struct sl_btmesh_evt_lc_client_property_status_s sl_btmesh_evt_lc_client_property_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_client_property_status

/***************************************************************************//**
 *
 * Initialize the LC Client model. LC Client does not have any internal
 * configuration. It only activates the model in the mesh stack.
 *
 * @param[in] elem_index Index of the client element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Get the mode status.
 *
 * @param[in] server_address Device to be queried. The address 0x0000 can be
 *   used to publish the message according to the model configuration.
 * @param[in] elem_index Index of the client element.
 * @param[in] appkey_index Appkey used by server_address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_mode_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_get_mode(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index);

/***************************************************************************//**
 *
 * Set mode
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether a response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 * @param[in] mode Mode value to set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_mode_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_set_mode(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index,
                                         uint8_t flags,
                                         uint8_t mode);

/***************************************************************************//**
 *
 * Get the OM status.
 *
 * @param[in] server_address Device to be queried. The address 0x0000 can be
 *   used to publish the message according to the model configuration.
 * @param[in] elem_index Index of the client element.
 * @param[in] appkey_index Appkey used by server_address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_om_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_get_om(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index);

/***************************************************************************//**
 *
 * Set occupancy mode.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 * @param[in] mode Mode value to set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_om_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_set_om(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index,
                                       uint8_t flags,
                                       uint8_t mode);

/***************************************************************************//**
 *
 * Get the Light OnOff status.
 *
 * @param[in] server_address Device to be queried. The address 0x0000 can be
 *   used to publish the message according to the model configuration.
 * @param[in] elem_index Index of the client element.
 * @param[in] appkey_index Appkey used by server_address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_light_onoff_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_get_light_onoff(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index);

/***************************************************************************//**
 *
 * Set the Light OnOff.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 * @param[in] target_state The target value of the Light LC Light OnOff state
 * @param[in] tid Transaction identifier
 * @param[in] transition_time_ms Transition time in milliseconds. Value of
 *   0xFFFFFFFF will cause this parameter as well as the "delay" parameter to be
 *   omitted.
 * @param[in] message_delay_ms Message execution delay in milliseconds. If the
 *   "transition_time" is 0xFFFFFFFF, this parameter is ignored. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_light_onoff_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_set_light_onoff(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t flags,
                                                uint8_t target_state,
                                                uint8_t tid,
                                                uint32_t transition_time_ms,
                                                uint16_t message_delay_ms);

/***************************************************************************//**
 *
 * Get the Property status.
 *
 * @param[in] server_address Device to be queried. The address 0x0000 can be
 *   used to publish the message according to the model configuration.
 * @param[in] elem_index Index of the client element.
 * @param[in] appkey_index Appkey used by server_address.
 * @param[in] property_id The property ID to query.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_property_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_get_property(uint16_t server_address,
                                             uint16_t elem_index,
                                             uint16_t appkey_index,
                                             uint16_t property_id);

/***************************************************************************//**
 *
 * Set a particular property.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 * @param[in] property_id Property ID for the LC Server. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] params_len Length of data in @p params
 * @param[in] params Byte array containing serialized fields of LC Property,
 *   excluding the property ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_lc_client_property_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_client_set_property(uint16_t server_address,
                                             uint16_t elem_index,
                                             uint16_t appkey_index,
                                             uint8_t flags,
                                             uint16_t property_id,
                                             size_t params_len,
                                             const uint8_t* params);

/** @} */ // end addtogroup sl_btmesh_lc_client

/**
 * @addtogroup sl_btmesh_lc_server Bluetooth Mesh Light Control Server Model
 * @{
 *
 * @brief Bluetooth Mesh Light Control Server Model
 *
 * Bluetooth Mesh Light Control Server model functionality.
 *
 * All LC Server state resides in and is own by the Model (stack). The state
 * update notification events to the application are informational: the
 * application is not required to react to them. The application may choose to
 * save the LC Server state in persistent storage and set the states in the LC
 * Server following a restart. To do this the application can utilize the
 * notification events and update command.
 *
 * Each LC Server instance requires that a Lightness Server is initialized in
 * the element preceding the LC Server element: LC Server controls the Lightness
 * Server residing in the preceding element. Each LC Server instance requires
 * that a generic OnOff Server is initialized in the same element as the LC
 * Server.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_lc_server_init_id                                  0x004d0028
#define sl_btmesh_cmd_lc_server_deinit_id                                0x014d0028
#define sl_btmesh_cmd_lc_server_update_mode_id                           0x024d0028
#define sl_btmesh_cmd_lc_server_update_om_id                             0x034d0028
#define sl_btmesh_cmd_lc_server_update_light_onoff_id                    0x044d0028
#define sl_btmesh_cmd_lc_server_init_all_properties_id                   0x054d0028
#define sl_btmesh_cmd_lc_server_set_publish_mask_id                      0x064d0028
#define sl_btmesh_cmd_lc_server_set_regulator_interval_id                0x074d0028
#define sl_btmesh_cmd_lc_server_set_event_mask_id                        0x084d0028
#define sl_btmesh_cmd_lc_server_get_lc_state_id                          0x094d0028
#define sl_btmesh_rsp_lc_server_init_id                                  0x004d0028
#define sl_btmesh_rsp_lc_server_deinit_id                                0x014d0028
#define sl_btmesh_rsp_lc_server_update_mode_id                           0x024d0028
#define sl_btmesh_rsp_lc_server_update_om_id                             0x034d0028
#define sl_btmesh_rsp_lc_server_update_light_onoff_id                    0x044d0028
#define sl_btmesh_rsp_lc_server_init_all_properties_id                   0x054d0028
#define sl_btmesh_rsp_lc_server_set_publish_mask_id                      0x064d0028
#define sl_btmesh_rsp_lc_server_set_regulator_interval_id                0x074d0028
#define sl_btmesh_rsp_lc_server_set_event_mask_id                        0x084d0028
#define sl_btmesh_rsp_lc_server_get_lc_state_id                          0x094d0028

/**
 * @brief These values define the possible states of Light Controller.
 */
typedef enum
{
  sl_btmesh_lc_server_lc_state_off                 = 0x0, /**< (0x0) The
                                                               controller is
                                                               turned off and
                                                               does not control
                                                               lighting. */
  sl_btmesh_lc_server_lc_state_standby             = 0x1, /**< (0x1) The
                                                               controller is
                                                               turned on and
                                                               awaits an event
                                                               from an occupancy
                                                               sensor or a
                                                               manual switch. */
  sl_btmesh_lc_server_lc_state_fade_on             = 0x2, /**< (0x2) The
                                                               controller has
                                                               been triggered
                                                               and gradually
                                                               transitions to
                                                               the Run phase,
                                                               gradually dimming
                                                               the lights up.> */
  sl_btmesh_lc_server_lc_state_run                 = 0x3, /**< (0x3) The lights
                                                               are on and the
                                                               timer counts down
                                                               (but may be
                                                               re-triggered by a
                                                               sensor or a
                                                               switch event). */
  sl_btmesh_lc_server_lc_state_fade                = 0x4, /**< (0x4) The Run
                                                               timer has expired
                                                               and the
                                                               controller
                                                               gradually
                                                               transitions to
                                                               the Prolong
                                                               state. */
  sl_btmesh_lc_server_lc_state_prolong             = 0x5, /**< (0x5) The lights
                                                               are at a lower
                                                               level and the
                                                               timer counts down
                                                               (but may be
                                                               re-triggered by a
                                                               sensor or a
                                                               switch event). */
  sl_btmesh_lc_server_lc_state_fade_standby_auto   = 0x6, /**< (0x6) The
                                                               controller
                                                               gradually returns
                                                               to the Standby
                                                               state */
  sl_btmesh_lc_server_lc_state_fade_standby_manual = 0x7  /**< (0x7) The
                                                               controller
                                                               gradually returns
                                                               to the Standby
                                                               state after
                                                               external event. */
} sl_btmesh_lc_server_lc_state_t;

/**
 * @brief These values identify optional diagnostic events that provide more
 * information to the application about LC behavior.
 */
typedef enum
{
  sl_btmesh_lc_server_lc_event_state_updated        = 0x1, /**< (0x1) Event
                                                                reporting LC
                                                                Server state
                                                                machine state
                                                                changes along
                                                                with the
                                                                remaining state
                                                                timer. */
  sl_btmesh_lc_server_lc_event_regulator_debug_info = 0x2  /**< (0x2) Event
                                                                reporting LC
                                                                Server PI
                                                                regulator
                                                                integral term
                                                                and regulator
                                                                output. */
} sl_btmesh_lc_server_lc_debug_events_t;

/**
 * @addtogroup sl_btmesh_evt_lc_server_mode_updated sl_btmesh_evt_lc_server_mode_updated
 * @{
 * @brief LC Mode state has been updated
 *
 * The update could be triggered by a reception of a client message or by an LC
 * Server State Machine action.
 */

/** @brief Identifier of the mode_updated event */
#define sl_btmesh_evt_lc_server_mode_updated_id                          0x004d00a8

/***************************************************************************//**
 * @brief Data structure of the mode_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_mode_updated_s
{
  uint16_t destination_address; /**< Message destination address if triggered by
                                     a message, 0 otherwise. */
  uint16_t elem_index;          /**< Index of the element where the update
                                     happened */
  uint16_t client_address;      /**< Message source address if triggered by a
                                     message, 0 otherwise. */
  uint16_t appkey_index;        /**< Message appkey index if triggered by a
                                     message, 0xFFFF otherwise. */
  uint8_t  mode_value;          /**< The value the LC Mode state is being set
                                     to. */
  uint8_t  manual_override;     /**< Light Control Mode state can be set to zero
                                     by a binding from Light Lightness Linear
                                     when it is modified by an action from the
                                     application or a Light Lightness Client
                                     command. In this case, the parameter is set
                                     to 0x01. In all other cases, this parameter
                                     is zero. For example, when LC Mode is
                                     modified by the application or by a LC
                                     Client command, this parameter will be set
                                     to 0. */
});

typedef struct sl_btmesh_evt_lc_server_mode_updated_s sl_btmesh_evt_lc_server_mode_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_mode_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_om_updated sl_btmesh_evt_lc_server_om_updated
 * @{
 * @brief LC Occupancy Mode state has been updated
 *
 * The update could be triggered by a reception of a client message or by an LC
 * Server State Machine action.
 */

/** @brief Identifier of the om_updated event */
#define sl_btmesh_evt_lc_server_om_updated_id                            0x014d00a8

/***************************************************************************//**
 * @brief Data structure of the om_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_om_updated_s
{
  uint16_t destination_address; /**< Message destination address if triggered by
                                     a message, 0 otherwise. */
  uint16_t elem_index;          /**< Index of the element where the update
                                     happened */
  uint16_t client_address;      /**< Message source address if triggered by a
                                     message, 0 otherwise. */
  uint16_t appkey_index;        /**< Message appkey index if triggered by a
                                     message, 0xFFFF otherwise. */
  uint8_t  om_value;            /**< The value the LC Occupancy Mode state is
                                     being set to. */
});

typedef struct sl_btmesh_evt_lc_server_om_updated_s sl_btmesh_evt_lc_server_om_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_om_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_light_onoff_updated sl_btmesh_evt_lc_server_light_onoff_updated
 * @{
 * @brief LC Light OnOff state has been updated
 *
 * The update could be triggered by a reception of a client message or by an LC
 * Server State Machine action.
 */

/** @brief Identifier of the light_onoff_updated event */
#define sl_btmesh_evt_lc_server_light_onoff_updated_id                   0x024d00a8

/***************************************************************************//**
 * @brief Data structure of the light_onoff_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_light_onoff_updated_s
{
  uint16_t destination_address; /**< Message destination address if triggered by
                                     a message, 0 otherwise. */
  uint16_t elem_index;          /**< Index of the element where the update
                                     happened */
  uint16_t source_address;      /**< Message source address if triggered by a
                                     message, 0 otherwise. */
  uint16_t appkey_index;        /**< Message appkey index if triggered by a
                                     message, 0xFFFF otherwise. */
  uint8_t  onoff_state;         /**< The target value of the Light LC Light
                                     OnOff state. */
  uint32_t onoff_trans_time_ms; /**< Amount of time (in milliseconds) the
                                     element will take to transition to the
                                     target state from the present state. */
});

typedef struct sl_btmesh_evt_lc_server_light_onoff_updated_s sl_btmesh_evt_lc_server_light_onoff_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_light_onoff_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_occupancy_updated sl_btmesh_evt_lc_server_occupancy_updated
 * @{
 * @brief LC Occupancy state has been updated
 *
 * The update could be triggered by a reception of a sensor message or by an LC
 * Server State Machine action.
 */

/** @brief Identifier of the occupancy_updated event */
#define sl_btmesh_evt_lc_server_occupancy_updated_id                     0x034d00a8

/***************************************************************************//**
 * @brief Data structure of the occupancy_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_occupancy_updated_s
{
  uint16_t destination_address; /**< Message destination address if triggered by
                                     a message, 0 otherwise. */
  uint16_t elem_index;          /**< Index of the element where the update
                                     happened */
  uint16_t source_address;      /**< Message source address if triggered by a
                                     message, 0 otherwise. */
  uint16_t appkey_index;        /**< Message appkey index if triggered by a
                                     message, 0xFFFF otherwise. */
  uint8_t  occupancy_value;     /**< The updated value of the LC Occupancy state */
});

typedef struct sl_btmesh_evt_lc_server_occupancy_updated_s sl_btmesh_evt_lc_server_occupancy_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_occupancy_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_ambient_lux_level_updated sl_btmesh_evt_lc_server_ambient_lux_level_updated
 * @{
 * @brief LC Ambient LuxLevel state has been updated
 *
 * The update is triggered by a reception of a sensor message.
 */

/** @brief Identifier of the ambient_lux_level_updated event */
#define sl_btmesh_evt_lc_server_ambient_lux_level_updated_id             0x044d00a8

/***************************************************************************//**
 * @brief Data structure of the ambient_lux_level_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_ambient_lux_level_updated_s
{
  uint16_t destination_address;     /**< Message destination address */
  uint16_t elem_index;              /**< Index of the element where the update
                                         happened */
  uint16_t source_address;          /**< Message source address */
  uint16_t appkey_index;            /**< Message appkey index */
  uint32_t ambient_lux_level_value; /**< The updated value of the LC Ambient
                                         LuxLevel state */
});

typedef struct sl_btmesh_evt_lc_server_ambient_lux_level_updated_s sl_btmesh_evt_lc_server_ambient_lux_level_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_ambient_lux_level_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_linear_output_updated sl_btmesh_evt_lc_server_linear_output_updated
 * @{
 * @brief LC Linear Output state has been updated
 *
 * The update is triggered by an LC Server State Machine action.
 */

/** @brief Identifier of the linear_output_updated event */
#define sl_btmesh_evt_lc_server_linear_output_updated_id                 0x054d00a8

/***************************************************************************//**
 * @brief Data structure of the linear_output_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_linear_output_updated_s
{
  uint16_t elem_index;          /**< Index of the element where the update
                                     happened */
  uint16_t linear_output_value; /**< The updated value of the LC Linear Output
                                     state */
});

typedef struct sl_btmesh_evt_lc_server_linear_output_updated_s sl_btmesh_evt_lc_server_linear_output_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_linear_output_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_state_updated sl_btmesh_evt_lc_server_state_updated
 * @{
 * @brief LC state machine state has been updated
 *
 * The update is triggered by LC mode switching on or off and transitions
 * between phases.
 */

/** @brief Identifier of the state_updated event */
#define sl_btmesh_evt_lc_server_state_updated_id                         0x064d00a8

/***************************************************************************//**
 * @brief Data structure of the state_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_state_updated_s
{
  uint16_t elem_index;      /**< Index of the element where the update happened */
  uint8_t  state;           /**< The updated value of the LC state */
  uint32_t transition_time; /**< Transition time defined for the current LC
                                 state. */
});

typedef struct sl_btmesh_evt_lc_server_state_updated_s sl_btmesh_evt_lc_server_state_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_state_updated

/**
 * @addtogroup sl_btmesh_evt_lc_server_regulator_debug_info sl_btmesh_evt_lc_server_regulator_debug_info
 * @{
 * @brief LC regulator calculation details
 */

/** @brief Identifier of the regulator_debug_info event */
#define sl_btmesh_evt_lc_server_regulator_debug_info_id                  0x074d00a8

/***************************************************************************//**
 * @brief Data structure of the regulator_debug_info event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_server_regulator_debug_info_s
{
  uint16_t elem_index; /**< Index of the element where LC server is located */
  uint16_t i;          /**< Integral term */
  uint16_t l;          /**< Regulator output */
});

typedef struct sl_btmesh_evt_lc_server_regulator_debug_info_s sl_btmesh_evt_lc_server_regulator_debug_info_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_server_regulator_debug_info

/***************************************************************************//**
 *
 * Initialize the LC Server model. The server does not have any internal
 * configuration. The command only activates the model in the mesh stack.
 *
 * Each LC Server instance requires that a Lightness Server is initialized in
 * the element preceding the LC Server element: LC Server controls the Lightness
 * Server residing in the preceding element. Each LC Server instance requires
 * that a generic OnOff Server is initialized in the same element as the LC
 * Server.
 *
 * LC properties are initialized as follows:
 *
 * PropertyID: PropertyValue 0x002B: 0x111111, 0x002C: 0x011111, 0x002D:
 * 0x001111, 0x002E: 0xf000, 0x002F: 0x0f00, 0x0030: 0x00f0, 0x031: 50, 0x032:
 * 25.0, 0x0033: 250.0, 0x0034: 80.0, 0x0035: 80.0, 0x0036: 3000, 0x0037: 3000,
 * 0x0038: 3000, 0x0039: 3000, 0x003A: 0, 0x003B: 3000, 0x003C: 3000
 *
 * PI Regulator interval (T) is initialized to 50ms
 *
 * The rest of the state values are initialized to zero
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * De-initializes the LC Server model.
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Update the LC Server model Mode state in the stack. Application may choose to
 * directly set the model state in the stack, this function will pass the state
 * value to the LC Server model.
 *
 * @param[in] elem_index Index of the element.
 * @param[in] mode Mode value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_update_mode(uint16_t elem_index, uint8_t mode);

/***************************************************************************//**
 *
 * Update the LC Server model Occupancy Mode state in the stack. The application
 * may choose to directly set the model state in the stack. This function will
 * pass the state value to the LC Server model.
 *
 * @param[in] elem_index Index of the element.
 * @param[in] om Occupancy Mode value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_update_om(uint16_t elem_index, uint8_t om);

/***************************************************************************//**
 *
 * Update the LC Server model Light OnOff state in the stack. The application
 * may choose to directly set the model state in the stack. This function will
 * pass the state value to the LC Server model.
 *
 * @param[in] elem_index Index of the element.
 * @param[in] light_onoff Light OnOff value
 * @param[in] transition_time_ms Amount of time (in milliseconds) that the
 *   element will take to transition to the target state from the present state.
 *   If set to 0, the transition will be immediate.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_update_light_onoff(uint16_t elem_index,
                                                   uint8_t light_onoff,
                                                   uint32_t transition_time_ms);

/***************************************************************************//**
 *
 * Initialize all LC properties in one attempt. The following values are used:
 *
 * PropertyID: PropertyValue 0x002B: 0x111111, 0x002C: 0x011111, 0x002D:
 * 0x001111, 0x002E: 0xf000, 0x002F: 0x0f00, 0x0030: 0x00f0, 0x031: 50, 0x032:
 * 25.0, 0x0033: 250.0, 0x0034: 80.0, 0x0035: 80.0, 0x0036: 3000, 0x0037: 3000,
 * 0x0038: 3000, 0x0039: 3000, 0x003A: 0, 0x003B: 3000, 0x003C: 3000
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_init_all_properties(uint16_t elem_index);

/***************************************************************************//**
 *
 * Update the bitmask that controls which messages are sent when the LC Server
 * publishes. By default, the bitmask will be enabled to publish all three
 * status messages.
 *
 * @param[in] elem_index Index of the element.
 * @param[in] status_type @parblock
 *   The type of status message to turn on/off. Options for this are:
 *
 *   LC Mode Status 0x8294 LC Occupancy Mode Status 0x8298 LC Light On Off
 *   Status 0x829C
 *   @endparblock
 * @param[in] value Turn on or off the status message.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_set_publish_mask(uint16_t elem_index,
                                                 uint16_t status_type,
                                                 uint8_t value);

/***************************************************************************//**
 *
 * Update the summation interval (T) at which the PI regulator is run. Only
 * valid when the regulator is disabled (Light LC Mode is 0).
 *
 * @param[in] elem_index Index of the element.
 * @param[in] value Valid values are 1 ms-100 ms. (Default: 50 ms)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_set_regulator_interval(uint16_t elem_index,
                                                       uint8_t value);

/***************************************************************************//**
 *
 * Enable or disable additional diagnostics events. See lc_debug_events.
 *
 * @param[in] elem_index Index of the element.
 * @param[in] event_type @parblock
 *   The type of event to enable/disable. Options are:
 *
 *   lc_event_state_updated = 0x01, state_updated event report state changes
 *   lc_event_regulator_debug_info = 0x02, regulator_debug_info Regulator
 *   calculation details
 *   @endparblock
 * @param[in] value Valid values are 0 and 1 to disable or enable the event
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_set_event_mask(uint16_t elem_index,
                                               uint16_t event_type,
                                               uint8_t value);

/***************************************************************************//**
 *
 * Fetch the current LC state. States can be as Off, Standby, Fade On, Run,
 * Fade, Prolong, Fade Standby Auto, Fade Standby Manual
 *
 * @param[in] elem_index Index of the element.
 * @param[out] state The current state of LC state machine
 * @param[out] transition_time Transition time left for the current LC state.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_server_get_lc_state(uint16_t elem_index,
                                             uint8_t *state,
                                             uint32_t *transition_time);

/** @} */ // end addtogroup sl_btmesh_lc_server

/**
 * @addtogroup sl_btmesh_lc_setup_server Bluetooth Mesh Light Control Setup Server Model
 * @{
 *
 * @brief Bluetooth Mesh Light Control Setup Server Model
 *
 * Bluetooth Mesh Light Control model Setup Server functionality.
 *
 * This class provides the API that the LC Setup server uses to inform the
 * application of its received events. The API is informational: application is
 * not required to react to these events.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_lc_setup_server_update_property_id                 0x004e0028
#define sl_btmesh_rsp_lc_setup_server_update_property_id                 0x004e0028

/**
 * @addtogroup sl_btmesh_evt_lc_setup_server_set_property sl_btmesh_evt_lc_setup_server_set_property
 * @{
 * @brief LC Property Set from the Client
 */

/** @brief Identifier of the set_property event */
#define sl_btmesh_evt_lc_setup_server_set_property_id                    0x004e00a8

/***************************************************************************//**
 * @brief Data structure of the set_property event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_lc_setup_server_set_property_s
{
  uint16_t   destination_address; /**< Address of the server or the group
                                       address to which it was published. */
  uint16_t   elem_index;          /**< Index of the element which received the
                                       command. */
  uint16_t   client_address;      /**< Device which sent the request. */
  uint16_t   appkey_index;        /**< Appkey used by client_address. */
  uint16_t   property_id;         /**< Property ID */
  uint8array property_value;      /**< Property value */
});

typedef struct sl_btmesh_evt_lc_setup_server_set_property_s sl_btmesh_evt_lc_setup_server_set_property_t;

/** @} */ // end addtogroup sl_btmesh_evt_lc_setup_server_set_property

/***************************************************************************//**
 *
 * Update the LC Server property. The application may choose to directly set
 * model properties in the stack. This function will pass the property value to
 * the LC Setup Server and on to the LC Server model.
 *
 * @param[in] elem_index Client model element index
 * @param[in] property_id Property ID for the LC Server. Range: 0x0001 - 0x0ffff
 *   for a specific device property, the value 0x0000 is prohibited.
 * @param[in] params_len Length of data in @p params
 * @param[in] params Byte array containing serialized fields of LC Property,
 *   excluding the property ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_lc_setup_server_update_property(uint16_t elem_index,
                                                      uint16_t property_id,
                                                      size_t params_len,
                                                      const uint8_t* params);

/** @} */ // end addtogroup sl_btmesh_lc_setup_server

/**
 * @addtogroup sl_btmesh_scene_client Bluetooth Mesh Scene Client Model
 * @{
 *
 * @brief Bluetooth Mesh Scene Client Model
 *
 * Bluetooth Mesh Scene Client model functionality to send and receive messages
 * to/from the Scene Server and Scene Setup Server models.
 *
 * Throughout the API, the client model that is used is identified by its
 * element address and model ID, while the server model responding to the client
 * model requests is identified by its element address and model ID.
 *
 * The API has functions for querying server model states and requesting server
 * model state changes.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_scene_client_init_id                               0x004f0028
#define sl_btmesh_cmd_scene_client_get_id                                0x014f0028
#define sl_btmesh_cmd_scene_client_get_register_id                       0x024f0028
#define sl_btmesh_cmd_scene_client_recall_id                             0x034f0028
#define sl_btmesh_cmd_scene_client_store_id                              0x044f0028
#define sl_btmesh_cmd_scene_client_delete_id                             0x054f0028
#define sl_btmesh_rsp_scene_client_init_id                               0x004f0028
#define sl_btmesh_rsp_scene_client_get_id                                0x014f0028
#define sl_btmesh_rsp_scene_client_get_register_id                       0x024f0028
#define sl_btmesh_rsp_scene_client_recall_id                             0x034f0028
#define sl_btmesh_rsp_scene_client_store_id                              0x044f0028
#define sl_btmesh_rsp_scene_client_delete_id                             0x054f0028

/**
 * @addtogroup sl_btmesh_evt_scene_client_status sl_btmesh_evt_scene_client_status
 * @{
 * @brief Event indicating an incoming Scene Status message
 */

/** @brief Identifier of the status event */
#define sl_btmesh_evt_scene_client_status_id                             0x004f00a8

/***************************************************************************//**
 * @brief Data structure of the status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_client_status_s
{
  uint16_t destination_address; /**< Address of the client or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     status. */
  uint16_t server_address;      /**< Device which sent the status. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
  uint8_t  status;              /**< Status of the request. */
  uint16_t current_scene;       /**< Currently selected scene. */
  uint16_t target_scene;        /**< Scene to be transitioning to. */
  uint32_t remaining_time_ms;   /**< Time (in milliseconds) remaining in
                                     transition. */
});

typedef struct sl_btmesh_evt_scene_client_status_s sl_btmesh_evt_scene_client_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_client_status

/**
 * @addtogroup sl_btmesh_evt_scene_client_register_status sl_btmesh_evt_scene_client_register_status
 * @{
 * @brief Indicate an incoming Scene Register Status message
 */

/** @brief Identifier of the register_status event */
#define sl_btmesh_evt_scene_client_register_status_id                    0x014f00a8

/***************************************************************************//**
 * @brief Data structure of the register_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_client_register_status_s
{
  uint16_t   destination_address; /**< Address of the client or the group
                                       address to which it was published. */
  uint16_t   elem_index;          /**< Index of the element for which received
                                       the status. */
  uint16_t   server_address;      /**< Device which sent the status. */
  uint16_t   appkey_index;        /**< Appkey used by client_address. */
  uint8_t    status;              /**< Status of the request. */
  uint16_t   current_scene;       /**< Currently selected scene. */
  uint8array scenes;              /**< List of stored scenes on the server
                                       derived from the Scene Register. It
                                       contains a variable length array of
                                       16-bit values representing Scene Numbers
                                       in little endian format. */
});

typedef struct sl_btmesh_evt_scene_client_register_status_s sl_btmesh_evt_scene_client_register_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_client_register_status

/***************************************************************************//**
 *
 * Initialize the Scene Client model. The Scene Client does not have any
 * internal configuration. It only activates the model in the mesh stack.
 *
 * @param[in] elem_index Index of the client element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Scene Get command.
 *
 * @param[in] server_address Device to be queried. The address 0x0000 can be
 *   used to publish the message according to model configuration
 * @param[in] elem_index Index of the client element.
 * @param[in] appkey_index Appkey used by server_address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_scene_client_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_get(uint16_t server_address,
                                       uint16_t elem_index,
                                       uint16_t appkey_index);

/***************************************************************************//**
 *
 * Scene Register Get command
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_scene_client_register_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_get_register(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index);

/***************************************************************************//**
 *
 * Recall a scene.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to model configuration
 * @param[in] elem_index Index of the client element.
 * @param[in] selected_scene Scene of interest
 * @param[in] appkey_index Appkey used by server_address.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent, zero will send SET PROPERTY
 *   UNACKNOWLEDGED
 * @param[in] tid Transaction ID
 * @param[in] transition_time_ms Amount of time (in milliseconds) allotted for
 *   the transition to take place. Value of 0xFFFFFFFF will cause this parameter
 *   as well as the "delay" parameter to be omitted. The transition will be
 *   immediate if both the transition time and the delay are zero.
 * @param[in] delay_ms Message execution delay in milliseconds. If the
 *   "transition_time" is 0xFFFFFFFF, this parameter is ignored. If both the
 *   transition time and the delay are zero, the transition is immediate.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_recall(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t selected_scene,
                                          uint16_t appkey_index,
                                          uint8_t flags,
                                          uint8_t tid,
                                          uint32_t transition_time_ms,
                                          uint16_t delay_ms);

/***************************************************************************//**
 *
 * Store a scene.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] selected_scene Scene of interest
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_store(uint16_t server_address,
                                         uint16_t elem_index,
                                         uint16_t selected_scene,
                                         uint16_t appkey_index,
                                         uint8_t flags);

/***************************************************************************//**
 *
 * Delete a scene.
 *
 * @param[in] server_address Destination server model address. The address
 *   0x0000 can be used to publish the message according to the model
 *   configuration.
 * @param[in] elem_index Client model element index
 * @param[in] selected_scene Scene of interest
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required.
 *   If set to 1, SET PROPERTY message will be sent. Zero will send SET PROPERTY
 *   UNACKNOWLEDGED.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_client_delete(uint16_t server_address,
                                          uint16_t elem_index,
                                          uint16_t selected_scene,
                                          uint16_t appkey_index,
                                          uint8_t flags);

/** @} */ // end addtogroup sl_btmesh_scene_client

/**
 * @addtogroup sl_btmesh_scene_server Bluetooth Mesh Scene Server Model
 * @{
 *
 * @brief Bluetooth Mesh Scene Server Model
 *
 * Bluetooth Mesh Scene model Server functionality.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_scene_server_init_id                               0x00500028
#define sl_btmesh_cmd_scene_server_deinit_id                             0x01500028
#define sl_btmesh_cmd_scene_server_reset_register_id                     0x02500028
#define sl_btmesh_cmd_scene_server_enable_compact_recall_events_id       0x03500028
#define sl_btmesh_rsp_scene_server_init_id                               0x00500028
#define sl_btmesh_rsp_scene_server_deinit_id                             0x01500028
#define sl_btmesh_rsp_scene_server_reset_register_id                     0x02500028
#define sl_btmesh_rsp_scene_server_enable_compact_recall_events_id       0x03500028

/**
 * @addtogroup sl_btmesh_evt_scene_server_get sl_btmesh_evt_scene_server_get
 * @{
 * @brief Get the status.
 */

/** @brief Identifier of the get event */
#define sl_btmesh_evt_scene_server_get_id                                0x015000a8

/***************************************************************************//**
 * @brief Data structure of the get event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_server_get_s
{
  uint16_t destination_address; /**< Address of the server or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     command. */
  uint16_t client_address;      /**< Device which sent the request. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
});

typedef struct sl_btmesh_evt_scene_server_get_s sl_btmesh_evt_scene_server_get_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_server_get

/**
 * @addtogroup sl_btmesh_evt_scene_server_register_get sl_btmesh_evt_scene_server_register_get
 * @{
 * @brief Get the status of a register.
 */

/** @brief Identifier of the register_get event */
#define sl_btmesh_evt_scene_server_register_get_id                       0x025000a8

/***************************************************************************//**
 * @brief Data structure of the register_get event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_server_register_get_s
{
  uint16_t destination_address; /**< Address of the server or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     command. */
  uint16_t client_address;      /**< Device which sent the request. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
});

typedef struct sl_btmesh_evt_scene_server_register_get_s sl_btmesh_evt_scene_server_register_get_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_server_register_get

/**
 * @addtogroup sl_btmesh_evt_scene_server_recall sl_btmesh_evt_scene_server_recall
 * @{
 * @brief Recall a scene.
 */

/** @brief Identifier of the recall event */
#define sl_btmesh_evt_scene_server_recall_id                             0x035000a8

/***************************************************************************//**
 * @brief Data structure of the recall event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_server_recall_s
{
  uint16_t destination_address; /**< Address of the server or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     command. */
  uint16_t selected_scene;      /**< Scene of interest */
  uint16_t client_address;      /**< Device which sent the request. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
  uint32_t transition_time_ms;  /**< Time (in milliseconds) allotted for the
                                     transition to take place */
});

typedef struct sl_btmesh_evt_scene_server_recall_s sl_btmesh_evt_scene_server_recall_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_server_recall

/**
 * @addtogroup sl_btmesh_evt_scene_server_publish sl_btmesh_evt_scene_server_publish
 * @{
 * @brief Indicate that the publishing period timer elapsed and the app
 * should/can publish its state or any request.
 */

/** @brief Identifier of the publish event */
#define sl_btmesh_evt_scene_server_publish_id                            0x045000a8

/***************************************************************************//**
 * @brief Data structure of the publish event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_server_publish_s
{
  uint16_t elem_index; /**< Client model element index */
  uint32_t period_ms;  /**< The current publishing period that can be used to
                            estimate the next tick, e.g., when the state should
                            be reported at higher frequency. */
});

typedef struct sl_btmesh_evt_scene_server_publish_s sl_btmesh_evt_scene_server_publish_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_server_publish

/**
 * @addtogroup sl_btmesh_evt_scene_server_compact_recall sl_btmesh_evt_scene_server_compact_recall
 * @{
 * @brief Recall a scene.
 */

/** @brief Identifier of the compact_recall event */
#define sl_btmesh_evt_scene_server_compact_recall_id                     0x055000a8

/***************************************************************************//**
 * @brief Data structure of the compact_recall event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_server_compact_recall_s
{
  uint8array states; /**< Byte array containind recalled states. Array consist
                          of 5 byte chunks as follows:

                            - Element id as 16bit unsigned integer in little
                              endian format
                            - Model id as 16bit unsigned integer in little
                              endian format
                            - Model-specific state type, identifying the kind of
                              state recalled See get state types list for
                              details.

                          after this event application can request recalled
                          states with @ref
                          sl_btmesh_generic_server_get_cached_state command */
});

typedef struct sl_btmesh_evt_scene_server_compact_recall_s sl_btmesh_evt_scene_server_compact_recall_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_server_compact_recall

/***************************************************************************//**
 *
 * Initialize the Scene Server model. Server does not have any internal
 * configurations. The command only activates the model in the mesh stack.
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_server_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * De-initializes the Scene Server model.
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_server_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Reset register value. This command should be invoked if state of a model has
 * been modified in such a manner that it can't be in the scene, which is
 * indicated by the scene register.
 *
 * @param[in] elem_index Index of the element. This can be either element of the
 *   updated model or the element of the scene model responsible for controlling
 *   the scene of the updated model.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_server_reset_register(uint16_t elem_index);

/***************************************************************************//**
 *
 * Switch to compact reporting for recalled states. Compact state reduces amount
 * buffering memory needed by the scene recall and is recommended for devices
 * with big amount of models or for devices in environment with lots of
 * bluetooth advertisement traffic.
 *
 * When compact mode is active @ref sl_btmesh_evt_scene_server_compact_recall is
 * generated instead of several @ref sl_btmesh_evt_generic_server_state_recall
 * events.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_server_enable_compact_recall_events();

/** @} */ // end addtogroup sl_btmesh_scene_server

/**
 * @addtogroup sl_btmesh_scene_setup_server Bluetooth Mesh Scene Setup Server Model
 * @{
 *
 * @brief Bluetooth Mesh Scene Setup Server Model
 *
 * Bluetooth Mesh Scene model Setup Server functionality.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_scene_setup_server_init_id                         0x00510028
#define sl_btmesh_rsp_scene_setup_server_init_id                         0x00510028

/**
 * @addtogroup sl_btmesh_evt_scene_setup_server_store sl_btmesh_evt_scene_setup_server_store
 * @{
 * @brief Scene Store from the Client
 */

/** @brief Identifier of the store event */
#define sl_btmesh_evt_scene_setup_server_store_id                        0x005100a8

/***************************************************************************//**
 * @brief Data structure of the store event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_setup_server_store_s
{
  uint16_t destination_address; /**< Address of the server or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     command. */
  uint16_t scene_id;            /**< Scene ID */
  uint16_t client_address;      /**< Device which sent the request. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
});

typedef struct sl_btmesh_evt_scene_setup_server_store_s sl_btmesh_evt_scene_setup_server_store_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_setup_server_store

/**
 * @addtogroup sl_btmesh_evt_scene_setup_server_delete sl_btmesh_evt_scene_setup_server_delete
 * @{
 * @brief Scene Delete from the Client
 */

/** @brief Identifier of the delete event */
#define sl_btmesh_evt_scene_setup_server_delete_id                       0x015100a8

/***************************************************************************//**
 * @brief Data structure of the delete event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_setup_server_delete_s
{
  uint16_t destination_address; /**< Address of the server or the group address
                                     to which it was published. */
  uint16_t elem_index;          /**< Index of the element which received the
                                     command. */
  uint16_t scene_id;            /**< Scene ID */
  uint16_t client_address;      /**< Device which sent the request. */
  uint16_t appkey_index;        /**< Appkey used by client_address. */
});

typedef struct sl_btmesh_evt_scene_setup_server_delete_s sl_btmesh_evt_scene_setup_server_delete_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_setup_server_delete

/**
 * @addtogroup sl_btmesh_evt_scene_setup_server_publish sl_btmesh_evt_scene_setup_server_publish
 * @{
 * @brief Indicate that the publishing period timer elapsed and the app
 * should/can publish its state or any request.
 */

/** @brief Identifier of the publish event */
#define sl_btmesh_evt_scene_setup_server_publish_id                      0x025100a8

/***************************************************************************//**
 * @brief Data structure of the publish event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scene_setup_server_publish_s
{
  uint16_t elem_index; /**< Client model element index */
  uint32_t period_ms;  /**< The current publishing period that can be used to
                            estimate the next tick, e.g., when the state should
                            be reported at higher frequency. */
});

typedef struct sl_btmesh_evt_scene_setup_server_publish_s sl_btmesh_evt_scene_setup_server_publish_t;

/** @} */ // end addtogroup sl_btmesh_evt_scene_setup_server_publish

/***************************************************************************//**
 *
 * Initialize the Scene Setup Server model. Server does not have any internal
 * configurations. The command only activates the model in the mesh stack.
 *
 * @param[in] elem_index Index of the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scene_setup_server_init(uint16_t elem_index);

/** @} */ // end addtogroup sl_btmesh_scene_setup_server

/**
 * @addtogroup sl_btmesh_scheduler_client Bluetooth Mesh Scheduler Client
 * @{
 *
 * @brief Bluetooth Mesh Scheduler Client
 *
 * This class provides commands and messages to interface with the Scheduler
 * Client model. Scheduler models uses multiple fields to define the occurrence
 * of an event and the type of event to be triggered.
 *
 * For the description of these fields, please see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server</a>
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_scheduler_client_init_id                           0x00540028
#define sl_btmesh_cmd_scheduler_client_deinit_id                         0x01540028
#define sl_btmesh_cmd_scheduler_client_get_id                            0x02540028
#define sl_btmesh_cmd_scheduler_client_get_action_id                     0x03540028
#define sl_btmesh_cmd_scheduler_client_set_action_id                     0x04540028
#define sl_btmesh_rsp_scheduler_client_init_id                           0x00540028
#define sl_btmesh_rsp_scheduler_client_deinit_id                         0x01540028
#define sl_btmesh_rsp_scheduler_client_get_id                            0x02540028
#define sl_btmesh_rsp_scheduler_client_get_action_id                     0x03540028
#define sl_btmesh_rsp_scheduler_client_set_action_id                     0x04540028

/**
 * @addtogroup sl_btmesh_evt_scheduler_client_status sl_btmesh_evt_scheduler_client_status
 * @{
 * @brief Scheduler Status response message to a Scheduler Get/Set command
 */

/** @brief Identifier of the status event */
#define sl_btmesh_evt_scheduler_client_status_id                         0x005400a8

/***************************************************************************//**
 * @brief Data structure of the status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scheduler_client_status_s
{
  uint16_t client_address; /**< Destination client model address */
  uint16_t elem_index;     /**< Client model element index */
  uint16_t server_address; /**< Source server model address */
  uint16_t appkey_index;   /**< The application key index to use. */
  uint16_t scheduler;      /**< Bit field indicating defined Actions in the
                                Schedule Register */
});

typedef struct sl_btmesh_evt_scheduler_client_status_s sl_btmesh_evt_scheduler_client_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_scheduler_client_status

/**
 * @addtogroup sl_btmesh_evt_scheduler_client_action_status sl_btmesh_evt_scheduler_client_action_status
 * @{
 * @brief Scheduler Status response message to a Scheduler Get/Set command
 *
 * For the description of these fields, see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server</a>
 */

/** @brief Identifier of the action_status event */
#define sl_btmesh_evt_scheduler_client_action_status_id                  0x015400a8

/***************************************************************************//**
 * @brief Data structure of the action_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scheduler_client_action_status_s
{
  uint16_t client_address;     /**< Destination client model address */
  uint16_t elem_index;         /**< Client model element index */
  uint16_t server_address;     /**< Source server model address */
  uint16_t appkey_index;       /**< The application key index to use. */
  uint8_t  index;              /**< Index of the Scheduler Register */
  uint8_t  year;               /**< Scheduled year for the action */
  uint16_t month;              /**< Scheduled month for the action */
  uint8_t  day;                /**< Scheduled day of the month for the action */
  uint8_t  hour;               /**< Scheduled hour for the action */
  uint8_t  minute;             /**< Scheduled minute for the action */
  uint8_t  second;             /**< Scheduled second for the action */
  uint8_t  day_of_week;        /**< Scheduled days of the week for the action */
  uint8_t  action;             /**< Action to be performed at the scheduled time */
  uint32_t transition_time_ms; /**< Transition time for this action */
  uint16_t scene_number;       /**< Scene number to be used for some actions */
});

typedef struct sl_btmesh_evt_scheduler_client_action_status_s sl_btmesh_evt_scheduler_client_action_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_scheduler_client_action_status

/***************************************************************************//**
 *
 * Initializes the Scheduler Client model
 *
 * @param[in] elem_index Client model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_client_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Deinitialize the Scheduler Client model
 *
 * @param[in] elem_index Client model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_client_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Send a Scheduler Get message to get the Scheduler Register status of a
 * Scheduler Server.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_scheduler_client_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_client_get(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index);

/***************************************************************************//**
 *
 * Send a Scheduler Action Get message to get action defined by a Schedule
 * Register state entry.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] index Index of the Scheduler Register entry to get
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_scheduler_client_action_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_client_get_action(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t index);

/***************************************************************************//**
 *
 * Send a Scheduler Action Set message to set the given entry of the Scheduler
 * Register state.
 *
 * For the description of these fields, see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server</a>
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] flags Bit 1 (0x02) defines whether response is required, othewise
 *   the unacknowledged message is used.
 * @param[in] index Index of the Scheduler Register entry to set
 * @param[in] year Scheduled year for the action
 * @param[in] month Scheduled month for the action
 * @param[in] day Scheduled day of the month for the action
 * @param[in] hour Scheduled hour for the action
 * @param[in] minute Scheduled minute for the action
 * @param[in] second Scheduled second for the action
 * @param[in] day_of_week Scheduled days of the week for the action
 * @param[in] action Action to be performed at the scheduled time
 * @param[in] transition_time_ms Transition time for this action
 * @param[in] scene_number Scene number to be used for some actions
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_btmesh_evt_scheduler_client_action_status
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_client_set_action(uint16_t server_address,
                                                  uint16_t elem_index,
                                                  uint16_t appkey_index,
                                                  uint8_t flags,
                                                  uint8_t index,
                                                  uint8_t year,
                                                  uint16_t month,
                                                  uint8_t day,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t second,
                                                  uint8_t day_of_week,
                                                  uint8_t action,
                                                  uint32_t transition_time_ms,
                                                  uint16_t scene_number);

/** @} */ // end addtogroup sl_btmesh_scheduler_client

/**
 * @addtogroup sl_btmesh_scheduler_server Bluetooth Mesh Scheduler Server
 * @{
 *
 * @brief Bluetooth Mesh Scheduler Server
 *
 * This class provides commands and messages to interface with the Scheduler
 * Server model
 *
 * The scheduler server uses multiple fields to define the occurrence of an
 * event and the type of event to be triggered. The field definitions are as
 * follows.
 *
 * <b>Year</b>
 *
 * The year field represents 2 last significant digits of the year of the
 * occurrence of the scheduled event.
 *   - 0x00-0x63: 2 least significant digits of the year (0-99)
 *   - 0x64: Any year
 *   - All other values are prohibited
 *
 * <b>Month</b>
 *
 * The month field represents the months of the occurrences of the scheduled
 * event, using a bitfield with one bit for each month.
 *
 *   - Bit 0: Scheduled in January
 *   - Bit 1: February
 *   - Bit 2: March
 *   - Bit 3: April
 *   - Bit 4: May
 *   - Bit 5: June
 *   - Bit 6: July
 *   - Bit 7: August
 *   - Bit 8: September
 *   - Bit 9: October
 *   - Bit 10: November
 *   - Bit 11: December
 *
 * <b>Day</b>
 *
 * The Day field represents the day of the month of the occurrence of the
 * scheduled event. If the day of the month has a number that is larger than the
 * number of days in the month, the event occurs in the last day of the month.
 * For example, in February if the day field holds the value 29, the action is
 * triggered on February 28th in a non-leap year or February 29th in a leap
 * year.
 *
 *   - 0x00: Any day
 *   - 0x01-0x1F: Day of the month (1-31)
 *   - All other values are prohibited
 *
 * <b>Hour</b>
 *
 * The Hour field represents the hour of the occurrence of the scheduled event.
 *
 *   - 0x00-0x17: Hour of the day (0-23)
 *   - 0x18: Any hour of the day
 *   - 0x19: Once a day (at a random hour)
 *   - All other values are prohibited
 *
 * <b>Minute</b>
 *
 * The Minute field represents the minute of the occurrence of the scheduled
 * event.
 *
 *   - 0x00-0x3B: Minute of the hour (0-59)
 *   - 0x3C: Any minute of the hour
 *   - 0x3D: Every 15 minutes (0, 15, 30, 45)
 *   - 0x3E: Every 20 minutes (0, 20, 40)
 *   - 0x3F: Once an hour (at a random minute)
 *   - All other values are prohibited
 *
 * <b>Second</b>
 *
 * The Second field represents the second of the occurrence of the scheduled
 * event.
 *
 *   - 0x00-0x3B: Seconds of the minute (0-59)
 *   - 0x3C: Any second of the minute
 *   - 0x3D: Every 15 seconds (0, 15, 30, 45)
 *   - 0x3E: Every 20 seconds (0, 20, 40)
 *   - 0x3F: Once a minute (at a random second)
 *   - All other values are prohibited
 *
 * <b>Day of Week</b>
 *
 * The DayOfWeek field represents the days of the week when the scheduled event
 * will trigger. The week days are represented by a bitfield, by one bit for
 * each day.
 *
 *   - Bit 0: Scheduled on Mondays
 *   - Bit 1: Scheduled on Tuesdays
 *   - Bit 2: Scheduled on Wednesdays
 *   - Bit 3: Scheduled on Thursdays
 *   - Bit 4: Scheduled on Fridays
 *   - Bit 5: Scheduled on Saturdays
 *   - Bit 6: Scheduled on Sundays
 *
 * <b>Action</b>
 *
 * The action field represents an action to be executed for a scheduled event
 *
 *   - 0x00: Turn Off
 *   - 0x01: Turn On
 *   - 0x02: Scene Recall
 *   - 0x0F: No action
 *   - All other values are prohibited
 *
 * <b>Transition time</b>
 *
 * This is a 1-octet value that consists of two fields: a 2-bit bit field
 * representing the step resolution and a 6-bit bit field representing the
 * number of transition steps.
 *
 * Bit 0-1: Transition Step Resolution
 *   - 0b00: The Default Transition Step Resolution is 100 milliseconds
 *   - 0b01: 1 second resolution
 *   - 0b10: 10 seconds resolution
 *   - 0b11: 10 minutes resolution
 *
 * Bit 2-7: Transition Number of Steps
 *   - 0x00: The Transition Time is immediate
 *   - 0x01-0x3E: The number of steps
 *   - 0x3F: The value is unknown. The state cannot be set to this value, but an
 *     element may report an unknown value if a transition is higher than 0x3E
 *     or not determined
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_scheduler_server_init_id                           0x00550028
#define sl_btmesh_cmd_scheduler_server_deinit_id                         0x01550028
#define sl_btmesh_cmd_scheduler_server_get_id                            0x02550028
#define sl_btmesh_cmd_scheduler_server_get_action_id                     0x03550028
#define sl_btmesh_cmd_scheduler_server_set_action_id                     0x04550028
#define sl_btmesh_rsp_scheduler_server_init_id                           0x00550028
#define sl_btmesh_rsp_scheduler_server_deinit_id                         0x01550028
#define sl_btmesh_rsp_scheduler_server_get_id                            0x02550028
#define sl_btmesh_rsp_scheduler_server_get_action_id                     0x03550028
#define sl_btmesh_rsp_scheduler_server_set_action_id                     0x04550028

/**
 * @addtogroup sl_btmesh_evt_scheduler_server_action_changed sl_btmesh_evt_scheduler_server_action_changed
 * @{
 * @brief Notification of a Scheduler Action change as the result of a Scheduler
 * Set message
 *
 * For the description of these fields, see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server.</a>
 */

/** @brief Identifier of the action_changed event */
#define sl_btmesh_evt_scheduler_server_action_changed_id                 0x015500a8

/***************************************************************************//**
 * @brief Data structure of the action_changed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scheduler_server_action_changed_s
{
  uint16_t elem_index;         /**< Scheduler server model element index */
  uint8_t  index;              /**< Index of the Scheduler Register */
  uint8_t  year;               /**< Scheduled year for the action */
  uint16_t month;              /**< Scheduled month for the action */
  uint8_t  day;                /**< Scheduled day of the month for the action */
  uint8_t  hour;               /**< Scheduled hour for the action */
  uint8_t  minute;             /**< Scheduled minute for the action */
  uint8_t  second;             /**< Scheduled second for the action */
  uint8_t  day_of_week;        /**< Scheduled days of the week for the action */
  uint8_t  action;             /**< Action to be performed at the scheduled time */
  uint32_t transition_time_ms; /**< Transition time for this action */
  uint16_t scene_number;       /**< Scene number to be used for some actions */
});

typedef struct sl_btmesh_evt_scheduler_server_action_changed_s sl_btmesh_evt_scheduler_server_action_changed_t;

/** @} */ // end addtogroup sl_btmesh_evt_scheduler_server_action_changed

/**
 * @addtogroup sl_btmesh_evt_scheduler_server_scene_changed sl_btmesh_evt_scheduler_server_scene_changed
 * @{
 * @brief Notification that scheduled scene change took place
 */

/** @brief Identifier of the scene_changed event */
#define sl_btmesh_evt_scheduler_server_scene_changed_id                  0x025500a8

/***************************************************************************//**
 * @brief Data structure of the scene_changed event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scheduler_server_scene_changed_s
{
  uint16_t elem_index;         /**< Scheduler server model element index */
  uint32_t transition_time_ms; /**< Transition time for this action */
  uint16_t scene_number;       /**< Scene number being activated */
});

typedef struct sl_btmesh_evt_scheduler_server_scene_changed_s sl_btmesh_evt_scheduler_server_scene_changed_t;

/** @} */ // end addtogroup sl_btmesh_evt_scheduler_server_scene_changed

/**
 * @addtogroup sl_btmesh_evt_scheduler_server_action_triggered sl_btmesh_evt_scheduler_server_action_triggered
 * @{
 * @brief Notification about a Scheduler Action that had its deadline expired"
 */

/** @brief Identifier of the action_triggered event */
#define sl_btmesh_evt_scheduler_server_action_triggered_id               0x035500a8

/***************************************************************************//**
 * @brief Data structure of the action_triggered event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_scheduler_server_action_triggered_s
{
  uint16_t elem_index;         /**< Scheduler server model element index */
  uint8_t  index;              /**< Index of the Scheduler Register */
  uint8_t  action;             /**< Action to be performed at the scheduled time */
  uint32_t transition_time_ms; /**< Transition time for this action */
  uint16_t scene_number;       /**< Scene number to be used for some actions */
});

typedef struct sl_btmesh_evt_scheduler_server_action_triggered_s sl_btmesh_evt_scheduler_server_action_triggered_t;

/** @} */ // end addtogroup sl_btmesh_evt_scheduler_server_action_triggered

/***************************************************************************//**
 *
 * Initialize the Scheduler Server model
 *
 * @param[in] elem_index Scheduler server model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_server_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Deinitialize the Scheduler Server model
 *
 * @param[in] elem_index Scheduler server model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_server_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Get Scheduler Register status of Scheduler Server.
 *
 * @param[in] elem_index Scheduler server model element index
 * @param[out] status Unsigned 16-bit integer
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_server_get(uint16_t elem_index,
                                           uint16_t *status);

/***************************************************************************//**
 *
 * Get the Scheduler Action defined by a Schedule Register state entry.
 *
 * For the description of returned fields, see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server</a>
 *
 * @param[in] elem_index Scheduler server model element index
 * @param[in] index Index of the Scheduler Register entry to get
 * @param[out] index_ Index of the Scheduler Register entry to set
 * @param[out] year Scheduled year for the action
 * @param[out] month Scheduled month for the action
 * @param[out] day Scheduled day of the month for the action
 * @param[out] hour Scheduled hour for the action
 * @param[out] minute Scheduled minute for the action
 * @param[out] second Scheduled second for the action
 * @param[out] day_of_week Scheduled days of the week for the action
 * @param[out] action Action to be performed at the scheduled time
 * @param[out] transition_time_ms Transition time for this action
 * @param[out] scene_number Scene number to be used for some actions
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_server_get_action(uint16_t elem_index,
                                                  uint8_t index,
                                                  uint8_t *index_,
                                                  uint8_t *year,
                                                  uint16_t *month,
                                                  uint8_t *day,
                                                  uint8_t *hour,
                                                  uint8_t *minute,
                                                  uint8_t *second,
                                                  uint8_t *day_of_week,
                                                  uint8_t *action,
                                                  uint32_t *transition_time_ms,
                                                  uint16_t *scene_number);

/***************************************************************************//**
 *
 * Set the given Scheduler Action entry of the Scheduler Register state.
 *
 * For the description of these fields, see <a
 * href="#cls_mesh_scheduler_server">Scheduler Server</a>
 *
 * @param[in] elem_index Scheduler server model element index
 * @param[in] index Index of the Scheduler Register entry to set
 * @param[in] year Scheduled year for the action
 * @param[in] month Scheduled month for the action
 * @param[in] day Scheduled day of the month for the action
 * @param[in] hour Scheduled hour for the action
 * @param[in] minute Scheduled minute for the action
 * @param[in] second Scheduled second for the action
 * @param[in] day_of_week Scheduled days of the week for the action
 * @param[in] action Action to be performed at the scheduled time
 * @param[in] transition_time_ms Transition time for this action
 * @param[in] scene_number Scene number to be used for some actions
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_scheduler_server_set_action(uint16_t elem_index,
                                                  uint8_t index,
                                                  uint8_t year,
                                                  uint16_t month,
                                                  uint8_t day,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t second,
                                                  uint8_t day_of_week,
                                                  uint8_t action,
                                                  uint32_t transition_time_ms,
                                                  uint16_t scene_number);

/** @} */ // end addtogroup sl_btmesh_scheduler_server

/**
 * @addtogroup sl_btmesh_time_server Bluetooth Mesh Time Server
 * @{
 *
 * @brief Bluetooth Mesh Time Server
 *
 * This class provides the commands and messages to interface with the Time
 * Server model
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_time_server_init_id                                0x00520028
#define sl_btmesh_cmd_time_server_deinit_id                              0x01520028
#define sl_btmesh_cmd_time_server_get_time_id                            0x02520028
#define sl_btmesh_cmd_time_server_set_time_id                            0x03520028
#define sl_btmesh_cmd_time_server_get_time_zone_offset_new_id            0x04520028
#define sl_btmesh_cmd_time_server_set_time_zone_offset_new_id            0x05520028
#define sl_btmesh_cmd_time_server_get_tai_utc_delta_new_id               0x06520028
#define sl_btmesh_cmd_time_server_set_tai_utc_delta_new_id               0x07520028
#define sl_btmesh_cmd_time_server_get_time_role_id                       0x08520028
#define sl_btmesh_cmd_time_server_set_time_role_id                       0x09520028
#define sl_btmesh_cmd_time_server_get_datetime_id                        0x0a520028
#define sl_btmesh_cmd_time_server_publish_id                             0x0b520028
#define sl_btmesh_cmd_time_server_status_id                              0x0c520028
#define sl_btmesh_rsp_time_server_init_id                                0x00520028
#define sl_btmesh_rsp_time_server_deinit_id                              0x01520028
#define sl_btmesh_rsp_time_server_get_time_id                            0x02520028
#define sl_btmesh_rsp_time_server_set_time_id                            0x03520028
#define sl_btmesh_rsp_time_server_get_time_zone_offset_new_id            0x04520028
#define sl_btmesh_rsp_time_server_set_time_zone_offset_new_id            0x05520028
#define sl_btmesh_rsp_time_server_get_tai_utc_delta_new_id               0x06520028
#define sl_btmesh_rsp_time_server_set_tai_utc_delta_new_id               0x07520028
#define sl_btmesh_rsp_time_server_get_time_role_id                       0x08520028
#define sl_btmesh_rsp_time_server_set_time_role_id                       0x09520028
#define sl_btmesh_rsp_time_server_get_datetime_id                        0x0a520028
#define sl_btmesh_rsp_time_server_publish_id                             0x0b520028
#define sl_btmesh_rsp_time_server_status_id                              0x0c520028

/**
 * @addtogroup sl_btmesh_evt_time_server_time_updated sl_btmesh_evt_time_server_time_updated
 * @{
 * @brief Indicate that Time State has been updated by external source, either
 * Time Set message from a Time Client, or a Time Status message
 */

/** @brief Identifier of the time_updated event */
#define sl_btmesh_evt_time_server_time_updated_id                        0x005200a8

/***************************************************************************//**
 * @brief Data structure of the time_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_server_time_updated_s
{
  uint16_t elem_index;       /**< Server model element index */
  uint64_t tai_seconds;      /**< The current TAI time in seconds since the
                                  epoch, 40-bit value */
  uint8_t  subsecond;        /**< The sub-second in units of 1/256th second.
                                  Range is 0-255. */
  uint8_t  uncertainty;      /**< The estimated uncertainty in 10-milliseconds
                                  steps. Range is 0-255, representing up 2.55
                                  seconds. */
  uint8_t  time_authority;   /**< 0: No Time Authority, the element does not
                                  have a trusted source of time such as GPS or
                                  NTP. 1: Time Authority, the element has a
                                  trusted source of time or a battery-backed
                                  properly initialized RTC. Other values are
                                  prohibited. */
  int32_t  tai_utc_delta;    /**< Current difference between TAI and UTC in
                                  seconds. Range is -255 to 32512. */
  int16_t  time_zone_offset; /**< The local time zone offset in 15-minute
                                  increments. Range is -64 to 191, representing
                                  \-16 to 47.75 hours. */
});

typedef struct sl_btmesh_evt_time_server_time_updated_s sl_btmesh_evt_time_server_time_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_server_time_updated

/**
 * @addtogroup sl_btmesh_evt_time_server_time_zone_offset_updated sl_btmesh_evt_time_server_time_zone_offset_updated
 * @{
 * @brief Indicate that upcoming time zone offset change has been updated by
 * external source
 */

/** @brief Identifier of the time_zone_offset_updated event */
#define sl_btmesh_evt_time_server_time_zone_offset_updated_id            0x015200a8

/***************************************************************************//**
 * @brief Data structure of the time_zone_offset_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_server_time_zone_offset_updated_s
{
  uint16_t elem_index;               /**< Server model element index */
  int16_t  time_zone_offset_current; /**< Current local time zone offset. Range
                                          is -64 to 191, representing -16 to
                                          47.75 hours. */
  int16_t  time_zone_offset_new;     /**< Upcoming local time zone offset. Range
                                          is -64 to 191, representing -16 to
                                          47.75 hours. */
  uint64_t tai_of_zone_change;       /**< Absolute TAI time when the Time Zone
                                          Offset will change from Current to New */
});

typedef struct sl_btmesh_evt_time_server_time_zone_offset_updated_s sl_btmesh_evt_time_server_time_zone_offset_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_server_time_zone_offset_updated

/**
 * @addtogroup sl_btmesh_evt_time_server_tai_utc_delta_updated sl_btmesh_evt_time_server_tai_utc_delta_updated
 * @{
 * @brief Indicate that the upcoming TAI-UTC Delta has been updated by external
 * source
 */

/** @brief Identifier of the tai_utc_delta_updated event */
#define sl_btmesh_evt_time_server_tai_utc_delta_updated_id               0x025200a8

/***************************************************************************//**
 * @brief Data structure of the tai_utc_delta_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_server_tai_utc_delta_updated_s
{
  uint16_t elem_index;            /**< Server model element index */
  int32_t  tai_utc_delta_current; /**< Current difference between TAI and UTC is
                                       seconds. Range is -255 to 32512. */
  int32_t  tai_utc_delta_new;     /**< Upcoming difference between TAI and UTC
                                       is seconds. Range is -255 to 32512. */
  uint64_t tai_of_delta_change;   /**< TAI Seconds time of the upcoming TAI-UTC
                                       Delta change */
});

typedef struct sl_btmesh_evt_time_server_tai_utc_delta_updated_s sl_btmesh_evt_time_server_tai_utc_delta_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_server_tai_utc_delta_updated

/**
 * @addtogroup sl_btmesh_evt_time_server_time_role_updated sl_btmesh_evt_time_server_time_role_updated
 * @{
 * @brief None
 */

/** @brief Identifier of the time_role_updated event */
#define sl_btmesh_evt_time_server_time_role_updated_id                   0x035200a8

/***************************************************************************//**
 * @brief Data structure of the time_role_updated event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_server_time_role_updated_s
{
  uint16_t elem_index; /**< Server model element index */
  uint8_t  time_role;  /**< Time Role */
});

typedef struct sl_btmesh_evt_time_server_time_role_updated_s sl_btmesh_evt_time_server_time_role_updated_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_server_time_role_updated

/***************************************************************************//**
 *
 * Initializes the Time Server model
 *
 * @param[in] elem_index Server model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Deinitialize the Time Server model
 *
 * @param[in] elem_index Server model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Get the current time from Time Server>
 *
 * @param[in] elem_index Server model element index
 * @param[out] tai_seconds The current TAI time in seconds
 * @param[out] subsecond The sub-second time in units of 1/256th second
 * @param[out] uncertainty The estimated uncertainty in 10 millisecond steps
 * @param[out] time_authority 0 = No Time Authority, 1 = Time Authority
 * @param[out] time_zone_offset Current local time zone offset. Range is -64 to
 *   191, representing -16 to 47.75 hours.
 * @param[out] tai_utc_delta Current difference between TAI and UTC in seconds.
 *   Range is -255 to 32512.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_get_time(uint16_t elem_index,
                                           uint64_t *tai_seconds,
                                           uint8_t *subsecond,
                                           uint8_t *uncertainty,
                                           uint8_t *time_authority,
                                           int16_t *time_zone_offset,
                                           int32_t *tai_utc_delta);

/***************************************************************************//**
 *
 * Set the current time for the element.
 *
 * @param[in] elem_index Server model element index
 * @param[in] tai_seconds The current TAI time in seconds
 * @param[in] subsecond The sub-second time in units of 1/256th second
 * @param[in] uncertainty The estimated uncertainty in 10 millisecond steps
 * @param[in] time_authority 0 = No Time Authority, 1 = Time Authority
 * @param[in] time_zone_offset Current local time zone offset. Range is -64 to
 *   191, representing -16 to 47.75 hours.
 * @param[in] tai_utc_delta Current difference between TAI and UTC in seconds.
 *   Range is -255 to 32512.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_set_time(uint16_t elem_index,
                                           uint64_t tai_seconds,
                                           uint8_t subsecond,
                                           uint8_t uncertainty,
                                           uint8_t time_authority,
                                           int16_t time_zone_offset,
                                           int32_t tai_utc_delta);

/***************************************************************************//**
 *
 * Get the upcoming time zone offset from Time Server
 *
 * @param[in] elem_index Server model element index
 * @param[out] new_offset Upcoming local time zone offset. Range is -64 to 191,
 *   representing -16 to 47.75 hours.
 * @param[out] tai_of_zone_change Absolute TAI time when the Time Zone Offset
 *   will change from Current to New
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_get_time_zone_offset_new(uint16_t elem_index,
                                                           int16_t *new_offset,
                                                           uint64_t *tai_of_zone_change);

/***************************************************************************//**
 *
 * Set the upcoming time zone offset for the element
 *
 * @param[in] elem_index Server model element index
 * @param[in] new_offset Upcoming local time zone offset. Range is -64 to 191,
 *   representing -16 to 47.75 hours.
 * @param[in] tai_of_zone_change Absolute TAI time when the Time Zone Offset
 *   will change from Current to New
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_set_time_zone_offset_new(uint16_t elem_index,
                                                           int16_t new_offset,
                                                           uint64_t tai_of_zone_change);

/***************************************************************************//**
 *
 * Get the upcoming TAI-UTC delta for the element
 *
 * @param[in] elem_index Server model element index
 * @param[out] new_delta Upcoming difference between TAI and UTC in seconds
 * @param[out] tai_of_delta_change Absolute TAI time when the TAI-UTC Delta will
 *   change from Current to New
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_get_tai_utc_delta_new(uint16_t elem_index,
                                                        int32_t *new_delta,
                                                        uint64_t *tai_of_delta_change);

/***************************************************************************//**
 *
 * Set the upcoming TAI-UTC delta for the element
 *
 * @param[in] elem_index Server model element index
 * @param[in] new_delta Upcoming difference between TAI and UTC in seconds
 * @param[in] tai_of_delta_change Absolute TAI time when the TAI-UTC Delta will
 *   change from Current to New
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_set_tai_utc_delta_new(uint16_t elem_index,
                                                        int32_t new_delta,
                                                        uint64_t tai_of_delta_change);

/***************************************************************************//**
 *
 * Get Time Role for the element
 *
 * @param[in] elem_index Server model element index
 * @param[out] time_role Time Role of the element
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_get_time_role(uint16_t elem_index,
                                                uint8_t *time_role);

/***************************************************************************//**
 *
 * Set Time Role for the element
 *
 * @param[in] elem_index Server model element index
 * @param[in] time_role Time Role of the element
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_set_time_role(uint16_t elem_index,
                                                uint8_t time_role);

/***************************************************************************//**
 *
 * Return the date and time from the Time Server
 *
 * @param[in] elem_index Server model element index
 * @param[out] year Year
 * @param[out] month Month
 * @param[out] day Day
 * @param[out] hour Hour
 * @param[out] min Minutes
 * @param[out] sec Seconds
 * @param[out] ms Milliseconds
 * @param[out] timezone Local time zone offset. Range is -64 to 191,
 *   representing -16 to 47.75 hours.
 * @param[out] day_of_week Day of week, 0..6 represents Monday to Sunday
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_get_datetime(uint16_t elem_index,
                                               uint16_t *year,
                                               uint8_t *month,
                                               uint8_t *day,
                                               uint8_t *hour,
                                               uint8_t *min,
                                               uint8_t *sec,
                                               uint16_t *ms,
                                               int16_t *timezone,
                                               uint8_t *day_of_week);

/***************************************************************************//**
 *
 * Publish Time Status containing the current time. Permitted only for Time
 * Server having the role of Time Authority.
 *
 * @param[in] elem_index Element index of the Time Server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_publish(uint16_t elem_index);

/***************************************************************************//**
 *
 * Send a Time Status message containing the current time as an unsolicited
 * message. Permitted only for Time Server having the role of Time Authority.
 *
 * @param[in] destination_address Destination address
 * @param[in] elem_index Element index of the Time Server
 * @param[in] appkey_index The application key index to use
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_server_status(uint16_t destination_address,
                                         uint16_t elem_index,
                                         uint16_t appkey_index);

/** @} */ // end addtogroup sl_btmesh_time_server

/**
 * @addtogroup sl_btmesh_time_client Bluetooth Mesh Time Client
 * @{
 *
 * @brief Bluetooth Mesh Time Client
 *
 * This class provides the commands and messages to interface with the Time
 * Client model
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_time_client_init_id                                0x00530028
#define sl_btmesh_cmd_time_client_deinit_id                              0x01530028
#define sl_btmesh_cmd_time_client_get_time_id                            0x02530028
#define sl_btmesh_cmd_time_client_set_time_id                            0x03530028
#define sl_btmesh_cmd_time_client_get_time_zone_id                       0x04530028
#define sl_btmesh_cmd_time_client_set_time_zone_id                       0x05530028
#define sl_btmesh_cmd_time_client_get_tai_utc_delta_id                   0x06530028
#define sl_btmesh_cmd_time_client_set_tai_utc_delta_id                   0x07530028
#define sl_btmesh_cmd_time_client_get_time_role_id                       0x08530028
#define sl_btmesh_cmd_time_client_set_time_role_id                       0x09530028
#define sl_btmesh_rsp_time_client_init_id                                0x00530028
#define sl_btmesh_rsp_time_client_deinit_id                              0x01530028
#define sl_btmesh_rsp_time_client_get_time_id                            0x02530028
#define sl_btmesh_rsp_time_client_set_time_id                            0x03530028
#define sl_btmesh_rsp_time_client_get_time_zone_id                       0x04530028
#define sl_btmesh_rsp_time_client_set_time_zone_id                       0x05530028
#define sl_btmesh_rsp_time_client_get_tai_utc_delta_id                   0x06530028
#define sl_btmesh_rsp_time_client_set_tai_utc_delta_id                   0x07530028
#define sl_btmesh_rsp_time_client_get_time_role_id                       0x08530028
#define sl_btmesh_rsp_time_client_set_time_role_id                       0x09530028

/**
 * @brief These values define the Time Role types used by the stack.
 */
typedef enum
{
  sl_btmesh_time_client_time_role_none      = 0x0, /**< (0x0) The element does
                                                        not participate in
                                                        propagation of time
                                                        information. */
  sl_btmesh_time_client_time_role_authority = 0x1, /**< (0x1) The element
                                                        publishes Time Status
                                                        messages but does not
                                                        process received Time
                                                        Status messages. */
  sl_btmesh_time_client_time_role_relay     = 0x2, /**< (0x2) The element
                                                        processes received and
                                                        publishes Time Status
                                                        messages. */
  sl_btmesh_time_client_time_role_client    = 0x3  /**< (0x3) The element does
                                                        not publish but
                                                        processes received Time
                                                        Status messages. */
} sl_btmesh_time_client_time_roles_t;

/**
 * @addtogroup sl_btmesh_evt_time_client_time_status sl_btmesh_evt_time_client_time_status
 * @{
 * @brief Time Status event
 */

/** @brief Identifier of the time_status event */
#define sl_btmesh_evt_time_client_time_status_id                         0x005300a8

/***************************************************************************//**
 * @brief Data structure of the time_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_client_time_status_s
{
  uint16_t client_address;   /**< Client address */
  uint16_t elem_index;       /**< Client model element index */
  uint16_t server_address;   /**< Unicast address of the server */
  uint16_t appkey_index;     /**< App key index. */
  uint64_t tai_seconds;      /**< The current TAI time in seconds */
  uint8_t  subsecond;        /**< The sub-second time in units of 1/256th second */
  uint8_t  uncertainty;      /**< The estimated uncertainty in 10-millisecond
                                  steps */
  uint8_t  time_authority;   /**< 0 = No Time Authority, 1 = Time Authority */
  int32_t  tai_utc_delta;    /**< Current difference between TAI and UTC in
                                  seconds. Range is -255 to 32512. */
  int16_t  time_zone_offset; /**< The local time zone offset in 15-minute
                                  increments. Range is -64 to 191, representing
                                  \-16 to 47.75 hours. */
});

typedef struct sl_btmesh_evt_time_client_time_status_s sl_btmesh_evt_time_client_time_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_client_time_status

/**
 * @addtogroup sl_btmesh_evt_time_client_time_zone_status sl_btmesh_evt_time_client_time_zone_status
 * @{
 * @brief Time Status event
 */

/** @brief Identifier of the time_zone_status event */
#define sl_btmesh_evt_time_client_time_zone_status_id                    0x015300a8

/***************************************************************************//**
 * @brief Data structure of the time_zone_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_client_time_zone_status_s
{
  uint16_t client_address;           /**< Client address */
  uint16_t elem_index;               /**< Client model element index */
  uint16_t server_address;           /**< Unicast address of the server */
  uint16_t appkey_index;             /**< App key index. */
  int16_t  time_zone_offset_current; /**< Current local time zone offset. Range
                                          is -64 to 191, representing -16 to
                                          47.75 hours. */
  int16_t  time_zone_offset_new;     /**< Upcoming local time zone offset */
  uint64_t tai_of_zone_change;       /**< TAI seconds time of the upcoming Time
                                          Zone offset change */
});

typedef struct sl_btmesh_evt_time_client_time_zone_status_s sl_btmesh_evt_time_client_time_zone_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_client_time_zone_status

/**
 * @addtogroup sl_btmesh_evt_time_client_tai_utc_delta_status sl_btmesh_evt_time_client_tai_utc_delta_status
 * @{
 * @brief Time Status event
 */

/** @brief Identifier of the tai_utc_delta_status event */
#define sl_btmesh_evt_time_client_tai_utc_delta_status_id                0x025300a8

/***************************************************************************//**
 * @brief Data structure of the tai_utc_delta_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_client_tai_utc_delta_status_s
{
  uint16_t client_address;        /**< Client address */
  uint16_t elem_index;            /**< Client model element index */
  uint16_t server_address;        /**< Unicast address of the server */
  uint16_t appkey_index;          /**< App key index. */
  int32_t  tai_utc_delta_current; /**< Current difference between TAI and UTC.
                                       Range is -255 to 32512. */
  int32_t  tai_utc_delta_new;     /**< Upcoming difference between TAI and UTC
                                       in seconds. Range is -255 to 32512. */
  uint64_t tai_of_delta_change;   /**< TAI seconds time of the upcoming TAI-UTC
                                       delta change */
});

typedef struct sl_btmesh_evt_time_client_tai_utc_delta_status_s sl_btmesh_evt_time_client_tai_utc_delta_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_client_tai_utc_delta_status

/**
 * @addtogroup sl_btmesh_evt_time_client_time_role_status sl_btmesh_evt_time_client_time_role_status
 * @{
 * @brief Time Status event
 */

/** @brief Identifier of the time_role_status event */
#define sl_btmesh_evt_time_client_time_role_status_id                    0x035300a8

/***************************************************************************//**
 * @brief Data structure of the time_role_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_btmesh_evt_time_client_time_role_status_s
{
  uint16_t client_address; /**< Client address */
  uint16_t elem_index;     /**< Client model element index */
  uint16_t server_address; /**< Unicast address of the server */
  uint16_t appkey_index;   /**< App key index. */
  uint8_t  time_role;      /**< The Time Role for the element */
});

typedef struct sl_btmesh_evt_time_client_time_role_status_s sl_btmesh_evt_time_client_time_role_status_t;

/** @} */ // end addtogroup sl_btmesh_evt_time_client_time_role_status

/***************************************************************************//**
 *
 * Initialize the Time Client model
 *
 * @param[in] elem_index Client model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_init(uint16_t elem_index);

/***************************************************************************//**
 *
 * Deinitialize the Time Client model
 *
 * @param[in] elem_index Client model element index
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_deinit(uint16_t elem_index);

/***************************************************************************//**
 *
 * Send a Time Get message to Time Server
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_get_time(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index);

/***************************************************************************//**
 *
 * Send a Time Set message to Time Server
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] tai_seconds The current TAI time in seconds since the epoch,
 *   40-bit value
 * @param[in] subsecond The sub-second in units of 1/256th second. Range is
 *   0-255.
 * @param[in] uncertainty The estimated uncertainty in 10-milliseconds steps.
 *   Range is 0-255, representing up to 2.55 seconds.
 * @param[in] time_authority 0: No Time Authority, the element does not have a
 *   trusted source of time such as GPS or NTP. 1: Time Authority, the element
 *   has a trusted source of time or a battery-backed properly initialized RTC.
 *   Other values are prohibited.
 * @param[in] tai_utc_delta Current difference between TAI and UTC in seconds.
 *   Range is -255 to 32512.
 * @param[in] time_zone_offset The local time zone offset in 15-minute
 *   increments. Range is -64 to 191, representing -16 to 47.75 hours.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_set_time(uint16_t server_address,
                                           uint16_t elem_index,
                                           uint16_t appkey_index,
                                           uint64_t tai_seconds,
                                           uint8_t subsecond,
                                           uint8_t uncertainty,
                                           uint8_t time_authority,
                                           int32_t tai_utc_delta,
                                           int16_t time_zone_offset);

/***************************************************************************//**
 *
 * Send a Time Zone Get message to Time Server
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_get_time_zone(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index);

/***************************************************************************//**
 *
 * Send a Time Zone Set message to Time Server to set the Time Zone New state
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] time_zone_offset_new Upcoming local time zone offset. Range is -64
 *   to 191, representing -16 to 47.75 hours.
 * @param[in] tai_of_zone_change TAI Seconds time of upcoming Time Zone offset
 *   change
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_set_time_zone(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                int16_t time_zone_offset_new,
                                                uint64_t tai_of_zone_change);

/***************************************************************************//**
 *
 * Send a TAI-UTC Delta Get message to Time Server
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_get_tai_utc_delta(uint16_t server_address,
                                                    uint16_t elem_index,
                                                    uint16_t appkey_index);

/***************************************************************************//**
 *
 * Send a TAI-UTC Delta Set message to Time Server, which responds with a
 * TAI-UTC Delta Status message.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] tai_utc_delta_new Upcoming difference between TAI and UTC is
 *   seconds. Range is -255 to 32512.
 * @param[in] tai_of_delta_change TAI Seconds time of the upcoming TAI-UTC Delta
 *   change
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_set_tai_utc_delta(uint16_t server_address,
                                                    uint16_t elem_index,
                                                    uint16_t appkey_index,
                                                    int32_t tai_utc_delta_new,
                                                    uint64_t tai_of_delta_change);

/***************************************************************************//**
 *
 * Send a Time Role Get message to Time Server, which responds with a Time Role
 * Status message.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_get_time_role(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index);

/***************************************************************************//**
 *
 * Send Time Role Get message to Time Server, which responds with a Time Role
 * Status message.
 *
 * @param[in] server_address Destination server model address
 * @param[in] elem_index Client model element index
 * @param[in] appkey_index The application key index to use.
 * @param[in] time_role The Time Role for the element.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_time_client_set_time_role(uint16_t server_address,
                                                uint16_t elem_index,
                                                uint16_t appkey_index,
                                                uint8_t time_role);

/** @} */ // end addtogroup sl_btmesh_time_client

/**
 * @addtogroup sl_btmesh_migration Bluetooth Mesh Key migration
 * @{
 *
 * @brief Bluetooth Mesh Key migration
 *
 * These commands are meant for migration from older releases to 3.2 and later
 * releases.
 */

/* Command and Response IDs */
#define sl_btmesh_cmd_migration_migrate_keys_id                          0x01600028
#define sl_btmesh_cmd_migration_migrate_ddb_id                           0x02600028
#define sl_btmesh_rsp_migration_migrate_keys_id                          0x01600028
#define sl_btmesh_rsp_migration_migrate_ddb_id                           0x02600028

/***************************************************************************//**
 *
 * Ugrade keys from pre 3.2 release. Must be executed prior to to node or
 * Provisioner initialization.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_migration_migrate_keys();

/***************************************************************************//**
 *
 * Ugrade Device Database from pre 3.2 release. This command must be executed
 * prior to provisioner initialization. Command may not be issued twice.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_migration_migrate_ddb();

/** @} */ // end addtogroup sl_btmesh_migration


/***************************************************************************//**
 * @addtogroup sl_btmesh_common_types BTMESH Common Types
 * @{
 *  @brief BTMESH common types
 */

/**
 * @brief Data structure of BTMESH API messages
 */
PACKSTRUCT( struct sl_btmesh_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t   header;

  /** Union of API event types */
  union {
    uint8_t handle;
    sl_btmesh_evt_node_initialized_t                             evt_node_initialized; /**< Data field for node initialized event*/
    sl_btmesh_evt_node_provisioned_t                             evt_node_provisioned; /**< Data field for node provisioned event*/
    sl_btmesh_evt_node_config_get_t                              evt_node_config_get; /**< Data field for node config_get event*/
    sl_btmesh_evt_node_config_set_t                              evt_node_config_set; /**< Data field for node config_set event*/
    sl_btmesh_evt_node_display_output_oob_t                      evt_node_display_output_oob; /**< Data field for node display_output_oob event*/
    sl_btmesh_evt_node_input_oob_request_t                       evt_node_input_oob_request; /**< Data field for node input_oob_request event*/
    sl_btmesh_evt_node_provisioning_started_t                    evt_node_provisioning_started; /**< Data field for node provisioning_started event*/
    sl_btmesh_evt_node_provisioning_failed_t                     evt_node_provisioning_failed; /**< Data field for node provisioning_failed event*/
    sl_btmesh_evt_node_key_added_t                               evt_node_key_added; /**< Data field for node key_added event*/
    sl_btmesh_evt_node_model_config_changed_t                    evt_node_model_config_changed; /**< Data field for node model_config_changed event*/
    sl_btmesh_evt_node_ivrecovery_needed_t                       evt_node_ivrecovery_needed; /**< Data field for node ivrecovery_needed event*/
    sl_btmesh_evt_node_changed_ivupdate_state_t                  evt_node_changed_ivupdate_state; /**< Data field for node changed_ivupdate_state event*/
    sl_btmesh_evt_node_key_removed_t                             evt_node_key_removed; /**< Data field for node key_removed event*/
    sl_btmesh_evt_node_key_updated_t                             evt_node_key_updated; /**< Data field for node key_updated event*/
    sl_btmesh_evt_node_heartbeat_t                               evt_node_heartbeat; /**< Data field for node heartbeat event*/
    sl_btmesh_evt_node_heartbeat_start_t                         evt_node_heartbeat_start; /**< Data field for node heartbeat_start event*/
    sl_btmesh_evt_node_heartbeat_stop_t                          evt_node_heartbeat_stop; /**< Data field for node heartbeat_stop event*/
    sl_btmesh_evt_node_beacon_received_t                         evt_node_beacon_received; /**< Data field for node beacon_received event*/
    sl_btmesh_evt_node_local_dcd_data_t                          evt_node_local_dcd_data; /**< Data field for node local_dcd_data event*/
    sl_btmesh_evt_node_local_dcd_data_end_t                      evt_node_local_dcd_data_end; /**< Data field for node local_dcd_data_end event*/
    sl_btmesh_evt_node_start_received_t                          evt_node_start_received; /**< Data field for node start_received event*/
    sl_btmesh_evt_prov_initialized_t                             evt_prov_initialized; /**< Data field for prov initialized event*/
    sl_btmesh_evt_prov_provisioning_suspended_t                  evt_prov_provisioning_suspended; /**< Data field for prov provisioning_suspended event*/
    sl_btmesh_evt_prov_capabilities_t                            evt_prov_capabilities; /**< Data field for prov capabilities event*/
    sl_btmesh_evt_prov_provisioning_failed_t                     evt_prov_provisioning_failed; /**< Data field for prov provisioning_failed event*/
    sl_btmesh_evt_prov_device_provisioned_t                      evt_prov_device_provisioned; /**< Data field for prov device_provisioned event*/
    sl_btmesh_evt_prov_unprov_beacon_t                           evt_prov_unprov_beacon; /**< Data field for prov unprov_beacon event*/
    sl_btmesh_evt_prov_oob_pkey_request_t                        evt_prov_oob_pkey_request; /**< Data field for prov oob_pkey_request event*/
    sl_btmesh_evt_prov_oob_auth_request_t                        evt_prov_oob_auth_request; /**< Data field for prov oob_auth_request event*/
    sl_btmesh_evt_prov_oob_display_input_t                       evt_prov_oob_display_input; /**< Data field for prov oob_display_input event*/
    sl_btmesh_evt_prov_ddb_list_t                                evt_prov_ddb_list; /**< Data field for prov ddb_list event*/
    sl_btmesh_evt_prov_uri_t                                     evt_prov_uri; /**< Data field for prov uri event*/
    sl_btmesh_evt_prov_key_refresh_phase_update_t                evt_prov_key_refresh_phase_update; /**< Data field for prov key_refresh_phase_update event*/
    sl_btmesh_evt_prov_key_refresh_node_update_t                 evt_prov_key_refresh_node_update; /**< Data field for prov key_refresh_node_update event*/
    sl_btmesh_evt_prov_key_refresh_complete_t                    evt_prov_key_refresh_complete; /**< Data field for prov key_refresh_complete event*/
    sl_btmesh_evt_prov_add_ddb_entry_complete_t                  evt_prov_add_ddb_entry_complete; /**< Data field for prov add_ddb_entry_complete event*/
    sl_btmesh_evt_prov_delete_ddb_entry_complete_t               evt_prov_delete_ddb_entry_complete; /**< Data field for prov delete_ddb_entry_complete event*/
    sl_btmesh_evt_prov_initialization_failed_t                   evt_prov_initialization_failed; /**< Data field for prov initialization_failed event*/
    sl_btmesh_evt_prov_start_sent_t                              evt_prov_start_sent; /**< Data field for prov start_sent event*/
    sl_btmesh_evt_proxy_connected_t                              evt_proxy_connected; /**< Data field for proxy connected event*/
    sl_btmesh_evt_proxy_disconnected_t                           evt_proxy_disconnected; /**< Data field for proxy disconnected event*/
    sl_btmesh_evt_proxy_filter_status_t                          evt_proxy_filter_status; /**< Data field for proxy filter_status event*/
    sl_btmesh_evt_vendor_model_receive_t                         evt_vendor_model_receive; /**< Data field for vendor_model receive event*/
    sl_btmesh_evt_vendor_model_send_complete_t                   evt_vendor_model_send_complete; /**< Data field for vendor_model send_complete event*/
    sl_btmesh_evt_health_client_server_status_t                  evt_health_client_server_status; /**< Data field for health_client server_status event*/
    sl_btmesh_evt_health_client_server_status_period_t           evt_health_client_server_status_period; /**< Data field for health_client server_status_period event*/
    sl_btmesh_evt_health_client_server_status_attention_t        evt_health_client_server_status_attention; /**< Data field for health_client server_status_attention event*/
    sl_btmesh_evt_health_server_attention_t                      evt_health_server_attention; /**< Data field for health_server attention event*/
    sl_btmesh_evt_health_server_test_request_t                   evt_health_server_test_request; /**< Data field for health_server test_request event*/
    sl_btmesh_evt_generic_client_server_status_t                 evt_generic_client_server_status; /**< Data field for generic_client server_status event*/
    sl_btmesh_evt_generic_server_client_request_t                evt_generic_server_client_request; /**< Data field for generic_server client_request event*/
    sl_btmesh_evt_generic_server_state_changed_t                 evt_generic_server_state_changed; /**< Data field for generic_server state_changed event*/
    sl_btmesh_evt_generic_server_state_recall_t                  evt_generic_server_state_recall; /**< Data field for generic_server state_recall event*/
    sl_btmesh_evt_test_local_heartbeat_subscription_complete_t   evt_test_local_heartbeat_subscription_complete; /**< Data field for test local_heartbeat_subscription_complete event*/
    sl_btmesh_evt_test_replay_protection_list_entry_set_t        evt_test_replay_protection_list_entry_set; /**< Data field for test replay_protection_list_entry_set event*/
    sl_btmesh_evt_test_replay_protection_list_entry_cleared_t    evt_test_replay_protection_list_entry_cleared; /**< Data field for test replay_protection_list_entry_cleared event*/
    sl_btmesh_evt_test_replay_protection_list_saved_t            evt_test_replay_protection_list_saved; /**< Data field for test replay_protection_list_saved event*/
    sl_btmesh_evt_lpn_friendship_established_t                   evt_lpn_friendship_established; /**< Data field for lpn friendship_established event*/
    sl_btmesh_evt_lpn_friendship_failed_t                        evt_lpn_friendship_failed; /**< Data field for lpn friendship_failed event*/
    sl_btmesh_evt_lpn_friendship_terminated_t                    evt_lpn_friendship_terminated; /**< Data field for lpn friendship_terminated event*/
    sl_btmesh_evt_friend_friendship_established_t                evt_friend_friendship_established; /**< Data field for friend friendship_established event*/
    sl_btmesh_evt_friend_friendship_terminated_t                 evt_friend_friendship_terminated; /**< Data field for friend friendship_terminated event*/
    sl_btmesh_evt_config_client_request_modified_t               evt_config_client_request_modified; /**< Data field for config_client request_modified event*/
    sl_btmesh_evt_config_client_netkey_status_t                  evt_config_client_netkey_status; /**< Data field for config_client netkey_status event*/
    sl_btmesh_evt_config_client_netkey_list_t                    evt_config_client_netkey_list; /**< Data field for config_client netkey_list event*/
    sl_btmesh_evt_config_client_netkey_list_end_t                evt_config_client_netkey_list_end; /**< Data field for config_client netkey_list_end event*/
    sl_btmesh_evt_config_client_appkey_status_t                  evt_config_client_appkey_status; /**< Data field for config_client appkey_status event*/
    sl_btmesh_evt_config_client_appkey_list_t                    evt_config_client_appkey_list; /**< Data field for config_client appkey_list event*/
    sl_btmesh_evt_config_client_appkey_list_end_t                evt_config_client_appkey_list_end; /**< Data field for config_client appkey_list_end event*/
    sl_btmesh_evt_config_client_binding_status_t                 evt_config_client_binding_status; /**< Data field for config_client binding_status event*/
    sl_btmesh_evt_config_client_bindings_list_t                  evt_config_client_bindings_list; /**< Data field for config_client bindings_list event*/
    sl_btmesh_evt_config_client_bindings_list_end_t              evt_config_client_bindings_list_end; /**< Data field for config_client bindings_list_end event*/
    sl_btmesh_evt_config_client_model_pub_status_t               evt_config_client_model_pub_status; /**< Data field for config_client model_pub_status event*/
    sl_btmesh_evt_config_client_model_sub_status_t               evt_config_client_model_sub_status; /**< Data field for config_client model_sub_status event*/
    sl_btmesh_evt_config_client_subs_list_t                      evt_config_client_subs_list; /**< Data field for config_client subs_list event*/
    sl_btmesh_evt_config_client_subs_list_end_t                  evt_config_client_subs_list_end; /**< Data field for config_client subs_list_end event*/
    sl_btmesh_evt_config_client_heartbeat_pub_status_t           evt_config_client_heartbeat_pub_status; /**< Data field for config_client heartbeat_pub_status event*/
    sl_btmesh_evt_config_client_heartbeat_sub_status_t           evt_config_client_heartbeat_sub_status; /**< Data field for config_client heartbeat_sub_status event*/
    sl_btmesh_evt_config_client_beacon_status_t                  evt_config_client_beacon_status; /**< Data field for config_client beacon_status event*/
    sl_btmesh_evt_config_client_default_ttl_status_t             evt_config_client_default_ttl_status; /**< Data field for config_client default_ttl_status event*/
    sl_btmesh_evt_config_client_gatt_proxy_status_t              evt_config_client_gatt_proxy_status; /**< Data field for config_client gatt_proxy_status event*/
    sl_btmesh_evt_config_client_relay_status_t                   evt_config_client_relay_status; /**< Data field for config_client relay_status event*/
    sl_btmesh_evt_config_client_network_transmit_status_t        evt_config_client_network_transmit_status; /**< Data field for config_client network_transmit_status event*/
    sl_btmesh_evt_config_client_identity_status_t                evt_config_client_identity_status; /**< Data field for config_client identity_status event*/
    sl_btmesh_evt_config_client_friend_status_t                  evt_config_client_friend_status; /**< Data field for config_client friend_status event*/
    sl_btmesh_evt_config_client_key_refresh_phase_status_t       evt_config_client_key_refresh_phase_status; /**< Data field for config_client key_refresh_phase_status event*/
    sl_btmesh_evt_config_client_lpn_polltimeout_status_t         evt_config_client_lpn_polltimeout_status; /**< Data field for config_client lpn_polltimeout_status event*/
    sl_btmesh_evt_config_client_dcd_data_t                       evt_config_client_dcd_data; /**< Data field for config_client dcd_data event*/
    sl_btmesh_evt_config_client_dcd_data_end_t                   evt_config_client_dcd_data_end; /**< Data field for config_client dcd_data_end event*/
    sl_btmesh_evt_config_client_reset_status_t                   evt_config_client_reset_status; /**< Data field for config_client reset_status event*/
    sl_btmesh_evt_sensor_server_get_request_t                    evt_sensor_server_get_request; /**< Data field for sensor_server get_request event*/
    sl_btmesh_evt_sensor_server_get_column_request_t             evt_sensor_server_get_column_request; /**< Data field for sensor_server get_column_request event*/
    sl_btmesh_evt_sensor_server_get_series_request_t             evt_sensor_server_get_series_request; /**< Data field for sensor_server get_series_request event*/
    sl_btmesh_evt_sensor_server_publish_t                        evt_sensor_server_publish; /**< Data field for sensor_server publish event*/
    sl_btmesh_evt_sensor_setup_server_get_cadence_request_t      evt_sensor_setup_server_get_cadence_request; /**< Data field for sensor_setup_server get_cadence_request event*/
    sl_btmesh_evt_sensor_setup_server_set_cadence_request_t      evt_sensor_setup_server_set_cadence_request; /**< Data field for sensor_setup_server set_cadence_request event*/
    sl_btmesh_evt_sensor_setup_server_get_settings_request_t     evt_sensor_setup_server_get_settings_request; /**< Data field for sensor_setup_server get_settings_request event*/
    sl_btmesh_evt_sensor_setup_server_get_setting_request_t      evt_sensor_setup_server_get_setting_request; /**< Data field for sensor_setup_server get_setting_request event*/
    sl_btmesh_evt_sensor_setup_server_set_setting_request_t      evt_sensor_setup_server_set_setting_request; /**< Data field for sensor_setup_server set_setting_request event*/
    sl_btmesh_evt_sensor_setup_server_publish_t                  evt_sensor_setup_server_publish; /**< Data field for sensor_setup_server publish event*/
    sl_btmesh_evt_sensor_client_descriptor_status_t              evt_sensor_client_descriptor_status; /**< Data field for sensor_client descriptor_status event*/
    sl_btmesh_evt_sensor_client_cadence_status_t                 evt_sensor_client_cadence_status; /**< Data field for sensor_client cadence_status event*/
    sl_btmesh_evt_sensor_client_settings_status_t                evt_sensor_client_settings_status; /**< Data field for sensor_client settings_status event*/
    sl_btmesh_evt_sensor_client_setting_status_t                 evt_sensor_client_setting_status; /**< Data field for sensor_client setting_status event*/
    sl_btmesh_evt_sensor_client_status_t                         evt_sensor_client_status; /**< Data field for sensor_client status event*/
    sl_btmesh_evt_sensor_client_column_status_t                  evt_sensor_client_column_status; /**< Data field for sensor_client column_status event*/
    sl_btmesh_evt_sensor_client_series_status_t                  evt_sensor_client_series_status; /**< Data field for sensor_client series_status event*/
    sl_btmesh_evt_sensor_client_publish_t                        evt_sensor_client_publish; /**< Data field for sensor_client publish event*/
    sl_btmesh_evt_lc_client_mode_status_t                        evt_lc_client_mode_status; /**< Data field for lc_client mode_status event*/
    sl_btmesh_evt_lc_client_om_status_t                          evt_lc_client_om_status; /**< Data field for lc_client om_status event*/
    sl_btmesh_evt_lc_client_light_onoff_status_t                 evt_lc_client_light_onoff_status; /**< Data field for lc_client light_onoff_status event*/
    sl_btmesh_evt_lc_client_property_status_t                    evt_lc_client_property_status; /**< Data field for lc_client property_status event*/
    sl_btmesh_evt_lc_server_mode_updated_t                       evt_lc_server_mode_updated; /**< Data field for lc_server mode_updated event*/
    sl_btmesh_evt_lc_server_om_updated_t                         evt_lc_server_om_updated; /**< Data field for lc_server om_updated event*/
    sl_btmesh_evt_lc_server_light_onoff_updated_t                evt_lc_server_light_onoff_updated; /**< Data field for lc_server light_onoff_updated event*/
    sl_btmesh_evt_lc_server_occupancy_updated_t                  evt_lc_server_occupancy_updated; /**< Data field for lc_server occupancy_updated event*/
    sl_btmesh_evt_lc_server_ambient_lux_level_updated_t          evt_lc_server_ambient_lux_level_updated; /**< Data field for lc_server ambient_lux_level_updated event*/
    sl_btmesh_evt_lc_server_linear_output_updated_t              evt_lc_server_linear_output_updated; /**< Data field for lc_server linear_output_updated event*/
    sl_btmesh_evt_lc_server_state_updated_t                      evt_lc_server_state_updated; /**< Data field for lc_server state_updated event*/
    sl_btmesh_evt_lc_server_regulator_debug_info_t               evt_lc_server_regulator_debug_info; /**< Data field for lc_server regulator_debug_info event*/
    sl_btmesh_evt_lc_setup_server_set_property_t                 evt_lc_setup_server_set_property; /**< Data field for lc_setup_server set_property event*/
    sl_btmesh_evt_scene_client_status_t                          evt_scene_client_status; /**< Data field for scene_client status event*/
    sl_btmesh_evt_scene_client_register_status_t                 evt_scene_client_register_status; /**< Data field for scene_client register_status event*/
    sl_btmesh_evt_scene_server_get_t                             evt_scene_server_get; /**< Data field for scene_server get event*/
    sl_btmesh_evt_scene_server_register_get_t                    evt_scene_server_register_get; /**< Data field for scene_server register_get event*/
    sl_btmesh_evt_scene_server_recall_t                          evt_scene_server_recall; /**< Data field for scene_server recall event*/
    sl_btmesh_evt_scene_server_publish_t                         evt_scene_server_publish; /**< Data field for scene_server publish event*/
    sl_btmesh_evt_scene_server_compact_recall_t                  evt_scene_server_compact_recall; /**< Data field for scene_server compact_recall event*/
    sl_btmesh_evt_scene_setup_server_store_t                     evt_scene_setup_server_store; /**< Data field for scene_setup_server store event*/
    sl_btmesh_evt_scene_setup_server_delete_t                    evt_scene_setup_server_delete; /**< Data field for scene_setup_server delete event*/
    sl_btmesh_evt_scene_setup_server_publish_t                   evt_scene_setup_server_publish; /**< Data field for scene_setup_server publish event*/
    sl_btmesh_evt_scheduler_client_status_t                      evt_scheduler_client_status; /**< Data field for scheduler_client status event*/
    sl_btmesh_evt_scheduler_client_action_status_t               evt_scheduler_client_action_status; /**< Data field for scheduler_client action_status event*/
    sl_btmesh_evt_scheduler_server_action_changed_t              evt_scheduler_server_action_changed; /**< Data field for scheduler_server action_changed event*/
    sl_btmesh_evt_scheduler_server_scene_changed_t               evt_scheduler_server_scene_changed; /**< Data field for scheduler_server scene_changed event*/
    sl_btmesh_evt_scheduler_server_action_triggered_t            evt_scheduler_server_action_triggered; /**< Data field for scheduler_server action_triggered event*/
    sl_btmesh_evt_time_server_time_updated_t                     evt_time_server_time_updated; /**< Data field for time_server time_updated event*/
    sl_btmesh_evt_time_server_time_zone_offset_updated_t         evt_time_server_time_zone_offset_updated; /**< Data field for time_server time_zone_offset_updated event*/
    sl_btmesh_evt_time_server_tai_utc_delta_updated_t            evt_time_server_tai_utc_delta_updated; /**< Data field for time_server tai_utc_delta_updated event*/
    sl_btmesh_evt_time_server_time_role_updated_t                evt_time_server_time_role_updated; /**< Data field for time_server time_role_updated event*/
    sl_btmesh_evt_time_client_time_status_t                      evt_time_client_time_status; /**< Data field for time_client time_status event*/
    sl_btmesh_evt_time_client_time_zone_status_t                 evt_time_client_time_zone_status; /**< Data field for time_client time_zone_status event*/
    sl_btmesh_evt_time_client_tai_utc_delta_status_t             evt_time_client_tai_utc_delta_status; /**< Data field for time_client tai_utc_delta_status event*/
    sl_btmesh_evt_time_client_time_role_status_t                 evt_time_client_time_role_status; /**< Data field for time_client time_role_status event*/
    uint8_t payload[SL_BGAPI_MAX_PAYLOAD_SIZE];
  } data;
});

/**
 * @brief Type definition for the data structure of BTMESH API messages
 */
typedef struct sl_btmesh_msg sl_btmesh_msg_t;

/** @} */ // end addtogroup sl_btmesh_common_types
/******************************************************************************/

/***************************************************************************//**
 * @addtogroup sl_bt_utility_functions Utility Functions
 *  @brief Utility functions for applications on SoC
 *  @{
 */

/**
 * Get the next event that requires processing by user application. Application
 * is not blocked if no event is waiting.
 *
 * @param event the pointer for storing the new event
 * @return SL_STATUS_OK if a new event is returned, or SL_STATUS_NOT_FOUND if no
 *   event is waiting; other value indicates an error occurred
 */
sl_status_t sl_btmesh_pop_event(sl_btmesh_msg_t* event);

/**
 * Check whether events are in queue pending for processing.
 * Call @ref sl_btmesh_pop_event to process pending events.
 *
 * @return true if event is pending; false otherwise
 */
bool sl_btmesh_event_pending(void);

/**
 * Check whether events are in queue pending for processing and return the next
 * event length in bytes if events are pending.
 * Call @ref sl_btmesh_pop_event to process pending events.
 *
 * @return the next event length if event is pending; 0 otherwise
 */
uint32_t sl_btmesh_event_pending_len(void);


/** @} */ // end addtogroup sl_bt_utility_functions
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif