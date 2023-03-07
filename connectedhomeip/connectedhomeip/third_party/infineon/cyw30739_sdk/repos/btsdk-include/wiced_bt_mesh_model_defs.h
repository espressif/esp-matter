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
* \file <wiced_bt_mesh_model_defs.h>
*
* Mesh Model definitions
*
******************************************************************************/
#ifndef __MESH_MODEL_DEFS_H__
#define __MESH_MODEL_DEFS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @anchor WICED_BT_MESH_CORE_COMPANY_ID
 * @name Company identifiers
 * \details The following is the list of mesh company id
 * @{
 */
#define MESH_COMPANY_ID_UNUSED                          0xFFFE
#define MESH_COMPANY_ID_BT_SIG                          0xFFFF
#define MESH_COMPANY_ID_CYPRESS                         0x0131
/** @} WICED_BT_MESH_CORE_COMPANY_ID */


// device UUID length
#define MESH_DEVICE_UUID_LEN                            16

/**
* @anchor BT_MESH_GATT_UUID
* @name UUIDs for services and characteristics
* \details The following is the list of service and characteristic UUIDs that a mesh node can support.
* @{
*/
#define WICED_BT_MESH_CORE_UUID_SERVICE_PROVISIONING                    0x1827
#define WICED_BT_MESH_CORE_UUID_SERVICE_PROXY                           0x1828
#define WICED_BT_MESH_CORE_UUID_CHARACTERISTIC_PROVISIONING_DATA_IN     0x2ADB
#define WICED_BT_MESH_CORE_UUID_CHARACTERISTIC_PROVISIONING_DATA_OUT    0x2ADC
#define WICED_BT_MESH_CORE_UUID_CHARACTERISTIC_PROXY_DATA_IN            0x2ADD
#define WICED_BT_MESH_CORE_UUID_CHARACTERISTIC_PROXY_DATA_OUT           0x2ADE
/** @} BT_MESH_GATT_UUID */

/**
 * @anchor MESH_ELEM_LOC
 * @name Element location
 * \details Element location (defined in GATT Bluetooth Namespace Descriptors section of the Bluetooth SIG Assigned Numbers)
 * @{
 */
#define MESH_ELEM_LOC_UNKNOWN                                    0x0000
#define MESH_ELEM_LOC_FIRST                                      0x0001
#define MESH_ELEM_LOC_TWO_HUNDRED_AND_FIFTY_FIFTH                0x00ff
#define MESH_ELEM_LOC_FRONT                                      0x0100
#define MESH_ELEM_LOC_BACK                                       0x0101
#define MESH_ELEM_LOC_TOP                                        0x0102
#define MESH_ELEM_LOC_BOTTOM                                     0x0103
#define MESH_ELEM_LOC_UPPER                                      0x0104
#define MESH_ELEM_LOC_LOWER                                      0x0105
#define MESH_ELEM_LOC_MAIN                                       0x0106
#define MESH_ELEM_LOC_BACKUP                                     0x0107
#define MESH_ELEM_LOC_AUXILIARY                                  0x0108
#define MESH_ELEM_LOC_SUPPLEMENTARY                              0x0109
#define MESH_ELEM_LOC_FLASH                                      0x010a
#define MESH_ELEM_LOC_INSIDE                                     0x010b
#define MESH_ELEM_LOC_OUTSIDE                                    0x010c
#define MESH_ELEM_LOC_LEFT                                       0x010d
#define MESH_ELEM_LOC_RIGHT                                      0x010e
#define MESH_ELEM_LOC_INTERNAL                                   0x010f
#define MESH_ELEM_LOC_EXTERNAL                                   0x0110
/** @} MESH_ELEM_LOC */

/**
 * @anchor WICED_BT_MESH_MODEL_ID
 * @name Model identifiers
 * \details The following is the list of Blueoth SIG defined Model IDs that a mesh node can support.
 * @{
 */
#define WICED_BT_MESH_CORE_MODEL_ID_CONFIG_SRV                          0x0000
#define WICED_BT_MESH_CORE_MODEL_ID_CONFIG_CLNT                         0x0001
#define WICED_BT_MESH_CORE_MODEL_ID_HEALTH_SRV                          0x0002
#define WICED_BT_MESH_CORE_MODEL_ID_HEALTH_CLNT                         0x0003
#define WICED_BT_MESH_CORE_MODEL_ID_REMOTE_PROVISION_SRV                0x0004
#define WICED_BT_MESH_CORE_MODEL_ID_REMOTE_PROVISION_CLNT               0x0005

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV                   0x1000
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_CLNT                  0x1001

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV                   0x1002
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_CLNT                  0x1003

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV                   0x1004
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_CLNT                  0x1005

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV             0x1006
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV       0x1007
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_CLNT            0x1008

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SRV             0x1009
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SRV       0x100A
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_CLNT            0x100B

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_BATTERY_SRV                 0x100C
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_BATTERY_CLNT                0x100D

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_SRV                0x100E
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_SETUP_SRV          0x100F
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_CLNT               0x1010

#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ADMIN_PROPERTY_SRV          0x1011
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_MANUFACT_PROPERTY_SRV       0x1012
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_USER_PROPERTY_SRV           0x1013
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_CLIENT_PROPERTY_SRV         0x1014
#define WICED_BT_MESH_CORE_MODEL_ID_GENERIC_PROPERTY_CLNT               0x1015

#define WICED_BT_MESH_CORE_MODEL_ID_SENSOR_SRV                          0x1100
#define WICED_BT_MESH_CORE_MODEL_ID_SENSOR_SETUP_SRV                    0x1101
#define WICED_BT_MESH_CORE_MODEL_ID_SENSOR_CLNT                         0x1102

#define WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV                            0x1200
#define WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV                      0x1201
#define WICED_BT_MESH_CORE_MODEL_ID_TIME_CLNT                           0x1202
#define WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV                           0x1203
#define WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV                     0x1204
#define WICED_BT_MESH_CORE_MODEL_ID_SCENE_CLNT                          0x1205
#define WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV                       0x1206
#define WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV                 0x1207
#define WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_CLNT                      0x1208

#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV                 0x1300
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV           0x1301
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_CLNT                0x1302

#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV                       0x1303
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV                 0x1304
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_CLNT                      0x1305
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_TEMPERATURE_SRV           0x1306

#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV                       0x1307
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV                 0x1308
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_CLNT                      0x1309
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_HUE_SRV                   0x130A
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SATURATION_SRV            0x130B

#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SRV                       0x130C
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SETUP_SRV                 0x130D
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_CLNT                      0x130E

#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_SRV                        0x130F
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_SETUP_SRV                  0x1310
#define WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_CLNT                       0x1311

/** @} WICED_BT_MESH_MODEL_ID */

/**
 * @anchor WICED_BT_MESH_MODEL_OPCODE
 * @name Mesh model opcodes
 * \details The following is the list of the Mesh model opcodes.
 * @{
 */

/* Opcodes of the foundation models (Configuration and Health) */
#define WICED_BT_MESH_CORE_CMD_APPKEY_ADD                                       0x00
#define WICED_BT_MESH_CORE_CMD_APPKEY_DELETE                                    0x8000
#define WICED_BT_MESH_CORE_CMD_APPKEY_GET                                       0x8001
#define WICED_BT_MESH_CORE_CMD_APPKEY_LIST                                      0x8002
#define WICED_BT_MESH_CORE_CMD_APPKEY_STATUS                                    0x8003
#define WICED_BT_MESH_CORE_CMD_APPKEY_UPDATE                                    0x01

#define WICED_BT_MESH_CORE_CMD_ATTENTION_GET                                    0x8004
#define WICED_BT_MESH_CORE_CMD_ATTENTION_SET                                    0x8005
#define WICED_BT_MESH_CORE_CMD_ATTENTION_SET_UNACKED                            0x8006
#define WICED_BT_MESH_CORE_CMD_ATTENTION_STATUS                                 0x8007

#define WICED_BT_MESH_CORE_CMD_DEVICE_COMPOS_DATA_GET                           0x8008
#define WICED_BT_MESH_CORE_CMD_DEVICE_COMPOS_DATA_STATUS                        0x02

#define WICED_BT_MESH_CORE_CMD_CONFIG_BEACON_GET                                0x8009
#define WICED_BT_MESH_CORE_CMD_CONFIG_BEACON_SET                                0x800a
#define WICED_BT_MESH_CORE_CMD_CONFIG_BEACON_STATUS                             0x800b

#define WICED_BT_MESH_CORE_CMD_CONFIG_DEFAULT_TTL_GET                           0x800c
#define WICED_BT_MESH_CORE_CMD_CONFIG_DEFAULT_TTL_SET                           0x800d
#define WICED_BT_MESH_CORE_CMD_CONFIG_DEFAULT_TTL_STATUS                        0x800e

#define WICED_BT_MESH_CORE_CMD_CONFIG_FRIEND_GET                                0x800f
#define WICED_BT_MESH_CORE_CMD_CONFIG_FRIEND_SET                                0x8010
#define WICED_BT_MESH_CORE_CMD_CONFIG_FRIEND_STATUS                             0x8011

#define WICED_BT_MESH_CORE_CMD_CONFIG_GATT_PROXY_GET                            0x8012
#define WICED_BT_MESH_CORE_CMD_CONFIG_GATT_PROXY_SET                            0x8013
#define WICED_BT_MESH_CORE_CMD_CONFIG_GATT_PROXY_STATUS                         0x8014

#define WICED_BT_MESH_CORE_CMD_CONFIG_KEY_REFRESH_PHASE_GET                     0x8015
#define WICED_BT_MESH_CORE_CMD_CONFIG_KEY_REFRESH_PHASE_SET                     0x8016
#define WICED_BT_MESH_CORE_CMD_CONFIG_KEY_REFRESH_PHASE_STATUS                  0x8017

#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_PUBLICATION_GET                     0x8018
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_PUBLICATION_SET                     0x03
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_PUBLICATION_STATUS                  0x8019
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_PUBLICATION_VIRT_ADDR_SET           0x801a

#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_ADD                    0x801b
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_DELETE                 0x801c
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL             0x801d
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE              0x801e
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_STATUS                 0x801f
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_VIRT_ADDR_ADD          0x8020
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_VIRT_ADDR_DELETE       0x8021
#define WICED_BT_MESH_CORE_CMD_CONFIG_MODEL_SUBSCRIPTION_VIRT_ADDR_OVERWRITE    0x8022

#define WICED_BT_MESH_CORE_CMD_CONFIG_NETWORK_TRANSMIT_GET                      0x8023
#define WICED_BT_MESH_CORE_CMD_CONFIG_NETWORK_TRANSMIT_SET                      0x8024
#define WICED_BT_MESH_CORE_CMD_CONFIG_NETWORK_TRANSMIT_STATUS                   0x8025

#define WICED_BT_MESH_CORE_CMD_CONFIG_RELAY_GET                                 0x8026
#define WICED_BT_MESH_CORE_CMD_CONFIG_RELAY_SET                                 0x8027
#define WICED_BT_MESH_CORE_CMD_CONFIG_RELAY_STATUS                              0x8028

#define WICED_BT_MESH_CORE_CMD_CONFIG_SIG_MODEL_SUBSCRIPTION_GET                0x8029
#define WICED_BT_MESH_CORE_CMD_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST               0x802a
#define WICED_BT_MESH_CORE_CMD_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET             0x802b
#define WICED_BT_MESH_CORE_CMD_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST            0x802c

#define WICED_BT_MESH_CORE_CMD_CONFIG_LPN_POLL_TIMEOUT_GET                      0x802D
#define WICED_BT_MESH_CORE_CMD_CONFIG_LPN_POLL_TIMEOUT_STATUS                   0x802E
#define WICED_BT_MESH_CORE_CMD_HEALTH_CURRENT_STATUS                            0x04
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_CLEAR                               0x802F
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_CLEAR_UNACKED                       0x8030
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_GET                                 0x8031
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_STATUS                              0x05
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_TEST                                0x8032
#define WICED_BT_MESH_CORE_CMD_HEALTH_FAULT_TEST_UNACKED                        0x8033
#define WICED_BT_MESH_CORE_CMD_HEALTH_PERIOD_GET                                0x8034
#define WICED_BT_MESH_CORE_CMD_HEALTH_PERIOD_SET                                0x8035
#define WICED_BT_MESH_CORE_CMD_HEALTH_PERIOD_SET_UNACKED                        0x8036
#define WICED_BT_MESH_CORE_CMD_HEALTH_PERIOD_STATUS                             0x8037

#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_PUBLICATION_GET                        0x8038
#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_PUBLICATION_SET                        0x8039
#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_PUBLICATION_STATUS                     0x06
#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_SUBSCRIPTION_GET                       0x803A
#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_SUBSCRIPTION_SET                       0x803B
#define WICED_BT_MESH_CORE_CMD_HEARTBEAT_SUBSCRIPTION_STATUS                    0x803C

#define WICED_BT_MESH_CORE_CMD_MODEL_APP_BIND                                   0x803D
#define WICED_BT_MESH_CORE_CMD_MODEL_APP_STATUS                                 0x803E
#define WICED_BT_MESH_CORE_CMD_MODEL_APP_UNBIND                                 0x803F

#define WICED_BT_MESH_CORE_CMD_NETKEY_ADD                                       0x8040
#define WICED_BT_MESH_CORE_CMD_NETKEY_DELETE                                    0x8041
#define WICED_BT_MESH_CORE_CMD_NETKEY_GET                                       0x8042
#define WICED_BT_MESH_CORE_CMD_NETKEY_LIST                                      0x8043
#define WICED_BT_MESH_CORE_CMD_NETKEY_STATUS                                    0x8044
#define WICED_BT_MESH_CORE_CMD_NETKEY_UPDATE                                    0x8045

#define WICED_BT_MESH_CORE_CMD_NODE_IDENTITY_GET                                0x8046
#define WICED_BT_MESH_CORE_CMD_NODE_IDENTITY_SET                                0x8047
#define WICED_BT_MESH_CORE_CMD_NODE_IDENTITY_STATUS                             0x8048

#define WICED_BT_MESH_CORE_CMD_NODE_RESET                                       0x8049
#define WICED_BT_MESH_CORE_CMD_NODE_RESET_STATUS                                0x804A

#define WICED_BT_MESH_CORE_CMD_SIG_MODEL_APP_GET                                0x804B
#define WICED_BT_MESH_CORE_CMD_SIG_MODEL_APP_LIST                               0x804C
#define WICED_BT_MESH_CORE_CMD_VENDOR_MODEL_APP_GET                             0x804D
#define WICED_BT_MESH_CORE_CMD_VENDOR_MODEL_APP_LIST                            0x804E

/* Remote Provisioning Server opcodes */
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_CAPABILITIES_GET            0x804F
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_CAPABILITIES_STATUS         0x8050
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_GET                         0x8051
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_START                       0x8052
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_STOP                        0x8053
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_STATUS                      0x8054
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_REPORT                      0x8055
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_EXTENDED_START              0x8056
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_SCAN_EXTENDED_REPORT             0x8057
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_LINK_GET                         0x8058
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_LINK_OPEN                        0x8059
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_LINK_CLOSE                       0x805A
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_LINK_STATUS                      0x805B
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_LINK_REPORT                      0x805C
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_PDU_SEND                         0x805D
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_PDU_OUTBOUND_REPORT              0x805E
#define WICED_BT_MESH_CODE_REMOTE_PROVISIONING_PDU_REPORT                       0x805F

/* Generic OnOff opcodes */
#define WICED_BT_MESH_OPCODE_GEN_ONOFF_GET                                      0x8201
#define WICED_BT_MESH_OPCODE_GEN_ONOFF_SET                                      0x8202
#define WICED_BT_MESH_OPCODE_GEN_ONOFF_SET_UNACKED                              0x8203
#define WICED_BT_MESH_OPCODE_GEN_ONOFF_STATUS                                   0x8204

/* Generic Level */
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_GET                                      0x8205
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_SET                                      0x8206
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_SET_UNACKED                              0x8207
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_STATUS                                   0x8208
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_DELTA_SET                                0x8209
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_DELTA_SET_UNACKED                        0x820A
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_MOVE_SET                                 0x820B
#define WICED_BT_MESH_OPCODE_GEN_LEVEL_MOVE_SET_UNACKED                         0x820C

/* Generic Default Transition Time opcodes */
#define WICED_BT_MESH_OPCODE_GEN_DEFTRANSTIME_GET                               0x820d
#define WICED_BT_MESH_OPCODE_GEN_DEFTRANSTIME_SET                               0x820e
#define WICED_BT_MESH_OPCODE_GEN_DEFTRANSTIME_SET_UNACKED                       0x820f
#define WICED_BT_MESH_OPCODE_GEN_DEFTRANSTIME_STATUS                            0x8210

/* Generic Power OnOff */
#define WICED_BT_MESH_OPCODE_GEN_ONPOWERUP_GET                                  0x8211
#define WICED_BT_MESH_OPCODE_GEN_ONPOWERUP_STATUS                               0x8212

/* Generic Power OnOff Setup */
#define WICED_BT_MESH_OPCODE_GEN_ONPOWERUP_SET                                  0x8213
#define WICED_BT_MESH_OPCODE_GEN_ONPOWERUP_SET_UNACKED                          0x8214

/* Generic Power Level */
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_GET                                0x8215
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_SET                                0x8216
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_SET_UNACKED                        0x8217
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_STATUS                             0x8218
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_LAST_GET                           0x8219
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_LAST_STATUS                        0x821A
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_DEFAULT_GET                        0x821B
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_DEFAULT_STATUS                     0x821C
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_RANGE_GET                          0x821D
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_RANGE_STATUS                       0x821E

/* Generic Power Level Setup */
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_DEFAULT_SET                        0x821F
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_DEFAULT_SET_UNACKED                0x8220
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_RANGE_SET                          0x8221
#define WICED_BT_MESH_OPCODE_GEN_POWER_LEVEL_RANGE_SET_UNACKED                  0x8222

/* Generic Battery */
#define WICED_BT_MESH_OPCODE_GEN_BATTERY_GET                                    0x8223
#define WICED_BT_MESH_OPCODE_GEN_BATTERY_STATUS                                 0x8224

/* Generic Location */
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_GLOBAL_GET                            0x8225
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_GLOBAL_STATUS                         0x40
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_LOCAL_GET                             0x8226
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_LOCAL_STATUS                          0x8227

/* Generic Location Setup */
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_GLOBAL_SET                            0x41
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_GLOBAL_SET_UNACKED                    0x42
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_LOCAL_SET                             0x8228
#define WICED_BT_MESH_OPCODE_GEN_LOCATION_LOCAL_SET_UNACKED                     0x8229

/* Generic Manufacturer Property */
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTIES_GET                    0x822A
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTIES_STATUS                 0x43
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTY_GET                      0x822B
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTY_SET                      0x44
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTY_SET_UNACKED              0x45
#define WICED_BT_MESH_OPCODE_GEN_MANUFACTURER_PROPERTY_STATUS                   0x46

/* Generic Admin Property */
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTIES_GET                           0x822C
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTIES_STATUS                        0x47
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTY_GET                             0x822D
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTY_SET                             0x48
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTY_SET_UNACKED                     0x49
#define WICED_BT_MESH_OPCODE_GEN_ADMIN_PROPERTY_STATUS                          0x4A

/* Generic User Property */
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTIES_GET                            0x822E
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTIES_STATUS                         0x4B
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTY_GET                              0x822F
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTY_SET                              0x4C
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTY_SET_UNACKED                      0x4D
#define WICED_BT_MESH_OPCODE_GEN_USER_PROPERTY_STATUS                           0x4E

/* Generic Client Property */
#define WICED_BT_MESH_OPCODE_GEN_CLIENT_PROPERTIES_GET                          0x4F
#define WICED_BT_MESH_OPCODE_GEN_CLIENT_PROPERTIES_STATUS                       0x50

/* Sensor */
#define WICED_BT_MESH_OPCODE_SENSOR_DESCRIPTOR_GET                              0x8230
#define WICED_BT_MESH_OPCODE_SENSOR_DESCRIPTOR_STATUS                           0x51
#define WICED_BT_MESH_OPCODE_SENSOR_GET                                         0x8231
#define WICED_BT_MESH_OPCODE_SENSOR_STATUS                                      0x52
#define WICED_BT_MESH_OPCODE_SENSOR_COLUMN_GET                                  0x8232
#define WICED_BT_MESH_OPCODE_SENSOR_COLUMN_STATUS                               0x53
#define WICED_BT_MESH_OPCODE_SENSOR_SERIES_GET                                  0x8233
#define WICED_BT_MESH_OPCODE_SENSOR_SERIES_STATUS                               0x54

/* Sensor Setup */
#define WICED_BT_MESH_OPCODE_SENSOR_CADENCE_GET                                 0x8234
#define WICED_BT_MESH_OPCODE_SENSOR_CADENCE_SET                                 0x55
#define WICED_BT_MESH_OPCODE_SENSOR_CADENCE_SET_UNACKED                         0x56
#define WICED_BT_MESH_OPCODE_SENSOR_CADENCE_STATUS                              0x57
#define WICED_BT_MESH_OPCODE_SENSOR_SETTINGS_GET                                0x8235
#define WICED_BT_MESH_OPCODE_SENSOR_SETTINGS_STATUS                             0x58
#define WICED_BT_MESH_OPCODE_SENSOR_SETTING_GET                                 0x8236
#define WICED_BT_MESH_OPCODE_SENSOR_SETTING_SET                                 0x59
#define WICED_BT_MESH_OPCODE_SENSOR_SETTING_SET_UNACKED                         0x5A
#define WICED_BT_MESH_OPCODE_SENSOR_SETTING_STATUS                              0x5B

/* Time */
#define WICED_BT_MESH_OPCODE_TIME_GET                                           0x8237
#define WICED_BT_MESH_OPCODE_TIME_SET                                           0x5C
#define WICED_BT_MESH_OPCODE_TIME_STATUS                                        0x5D
#define WICED_BT_MESH_OPCODE_TIME_ROLE_GET                                      0x8238
#define WICED_BT_MESH_OPCODE_TIME_ROLE_SET                                      0x8239
#define WICED_BT_MESH_OPCODE_TIME_ROLE_STATUS                                   0x823A
#define WICED_BT_MESH_OPCODE_TIME_ZONE_GET                                      0x823B
#define WICED_BT_MESH_OPCODE_TIME_ZONE_SET                                      0x823C
#define WICED_BT_MESH_OPCODE_TIME_ZONE_STATUS                                   0x823D
#define WICED_BT_MESH_OPCODE_TAI_UTC_DELTA_GET                                  0x823E
#define WICED_BT_MESH_OPCODE_TAI_UTC_DELTA_SET                                  0x823F
#define WICED_BT_MESH_OPCODE_TAI_UTC_DELTA_STATUS                               0x8240

/* Scene */
#define WICED_BT_MESH_OPCODE_SCENE_GET                                          0x8241
#define WICED_BT_MESH_OPCODE_SCENE_RECALL                                       0x8242
#define WICED_BT_MESH_OPCODE_SCENE_RECALL_UNACKED                               0x8243
#define WICED_BT_MESH_OPCODE_SCENE_STATUS                                       0x5E
#define WICED_BT_MESH_OPCODE_SCENE_REGISTER_GET                                 0x8244
#define WICED_BT_MESH_OPCODE_SCENE_REGISTER_STATUS                              0x8245

/* Scene Setup */
#define WICED_BT_MESH_OPCODE_SCENE_STORE                                        0x8246
#define WICED_BT_MESH_OPCODE_SCENE_STORE_UNACKED                                0x8247
#define WICED_BT_MESH_OPCODE_SCENE_DELETE                                       0x829E
#define WICED_BT_MESH_OPCODE_SCENE_DELETE_UNACKED                               0x829F

/* Scheduler */
#define WICED_BT_MESH_OPCODE_SCHEDULER_ACTION_GET                               0x8248
#define WICED_BT_MESH_OPCODE_SCHEDULER_ACTION_STATUS                            0x5F
#define WICED_BT_MESH_OPCODE_SCHEDULER_GET                                      0x8249
#define WICED_BT_MESH_OPCODE_SCHEDULER_STATUS                                   0x824A

/* Scheduler Setup */
#define WICED_BT_MESH_OPCODE_SCHEDULER_ACTION_SET                               0x60
#define WICED_BT_MESH_OPCODE_SCHEDULER_ACTION_SET_UNACKED                       0x61

/* Light Lightness */
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_GET                                0x824B
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_SET                                0x824C
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_SET_UNACKED                        0x824D
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_STATUS                             0x824E
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LINEAR_GET                         0x824F
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LINEAR_SET                         0x8250
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LINEAR_SET_UNACKED                 0x8251
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LINEAR_STATUS                      0x8252
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LAST_GET                           0x8253
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_LAST_STATUS                        0x8254
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_DEFAULT_GET                        0x8255
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_DEFAULT_STATUS                     0x8256
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_RANGE_GET                          0x8257
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_RANGE_STATUS                       0x8258

/* Light Lightness Setup */
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_DEFAULT_SET                        0x8259
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKED                0x825A
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_RANGE_SET                          0x825B
#define WICED_BT_MESH_OPCODE_LIGHT_LIGHTNESS_RANGE_SET_UNACKED                  0x825C

/* Light CTL */
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_GET                                      0x825D
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_SET                                      0x825E
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_SET_UNACKED                              0x825F
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_STATUS                                   0x8260
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_GET                          0x8261
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_GET                    0x8262
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_STATUS                 0x8263
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_SET                          0x8264
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_SET_UNACKED                  0x8265
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_STATUS                       0x8266
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_DEFAULT_GET                              0x8267
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_DEFAULT_STATUS                           0x8268

/* Light CTL Setup */
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_DEFAULT_SET                              0x8269
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_DEFAULT_SET_UNACKED                      0x826A
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_SET                    0x826B
#define WICED_BT_MESH_OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKED            0x826C

/* Light HSL */
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_GET                                      0x826D
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_HUE_GET                                  0x826E
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_HUE_SET                                  0x826F
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_HUE_SET_UNACKED                          0x8270
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_HUE_STATUS                               0x8271
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SATURATION_GET                           0x8272
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SATURATION_SET                           0x8273
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SATURATION_SET_UNACKED                   0x8274
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SATURATION_STATUS                        0x8275
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SET                                      0x8276
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_SET_UNACKED                              0x8277
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_STATUS                                   0x8278
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_TARGET_GET                               0x8279
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_TARGET_STATUS                            0x827A
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_DEFAULT_GET                              0x827B
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_DEFAULT_STATUS                           0x827C
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_RANGE_GET                                0x827D
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_RANGE_STATUS                             0x827E

/* Light HSL Setup */
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_DEFAULT_SET                              0x827F
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_DEFAULT_SET_UNACKED                      0x8280
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_RANGE_SET                                0x8281
#define WICED_BT_MESH_OPCODE_LIGHT_HSL_RANGE_SET_UNACKED                        0x8282

/* Light xyL */
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_GET                                      0x8283
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_SET                                      0x8284
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_SET_UNACKED                              0x8285
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_STATUS                                   0x8286
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_TARGET_GET                               0x8287
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_TARGET_STATUS                            0x8288
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_DEFAULT_GET                              0x8289
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_DEFAULT_STATUS                           0x828A
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_RANGE_GET                                0x828B
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_RANGE_STATUS                             0x828C

/* Light xyL Setup */
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_DEFAULT_SET                              0x828D
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_DEFAULT_SET_UNACKED                      0x828E
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_RANGE_SET                                0x828F
#define WICED_BT_MESH_OPCODE_LIGHT_XYL_RANGE_SET_UNACKED                        0x8290

/* Light Control */
#define WICED_BT_MESH_OPCODE_LIGHT_LC_MODE_GET                                  0x8291
#define WICED_BT_MESH_OPCODE_LIGHT_LC_MODE_SET                                  0x8292
#define WICED_BT_MESH_OPCODE_LIGHT_LC_MODE_SET_UNACKED                          0x8293
#define WICED_BT_MESH_OPCODE_LIGHT_LC_MODE_STATUS                               0x8294
#define WICED_BT_MESH_OPCODE_LIGHT_LC_OM_GET                                    0x8295
#define WICED_BT_MESH_OPCODE_LIGHT_LC_OM_SET                                    0x8296
#define WICED_BT_MESH_OPCODE_LIGHT_LC_OM_SET_UNACKED                            0x8297
#define WICED_BT_MESH_OPCODE_LIGHT_LC_OM_STATUS                                 0x8298
#define WICED_BT_MESH_OPCODE_LIGHT_LC_LIGHT_ONOFF_GET                           0x8299
#define WICED_BT_MESH_OPCODE_LIGHT_LC_LIGHT_ONOFF_SET                           0x829A
#define WICED_BT_MESH_OPCODE_LIGHT_LC_LIGHT_ONOFF_SET_UNACKED                   0x829B
#define WICED_BT_MESH_OPCODE_LIGHT_LC_LIGHT_ONOFF_STATUS                        0x829C
#define WICED_BT_MESH_OPCODE_LIGHT_LC_PROPERTY_GET                              0x829D
#define WICED_BT_MESH_OPCODE_LIGHT_LC_PROPERTY_SET                              0x62
#define WICED_BT_MESH_OPCODE_LIGHT_LC_PROPERTY_SET_UNACKED                      0x63
#define WICED_BT_MESH_OPCODE_LIGHT_LC_PROPERTY_STATUS                           0x64

#define WICED_BT_MESH_OPCODE_UNKNOWN                                            0xFFFF

/* General purpose state update code */
#define WICED_BT_MODEL_STATE_UPDATE                                             0xFFFFFF
#define WICED_BT_MODEL_MAX_OPCODE_LEN                                           0x03

/** @} WICED_BT_MESH_MODEL_OPCODE */

#define WICED_BT_MESH_LOCATION_GLOBAL_LATITUDE_UNKNOWN                          0x8000000
#define WICED_BT_MESH_LOCATION_GLOBAL_LONGITUDE_UNKNOWN                         0x8000000

/**
 * @anchor WICED_BT_MESH_ON_POWER_UP_STATE
 * @name On Power Up State definitions
 * @{
 */
#define WICED_BT_MESH_ON_POWER_UP_STATE_OFF                             0x00 /**< Off. After being powered up, the element is in an off state. */
#define WICED_BT_MESH_ON_POWER_UP_STATE_DEFAULT                         0x01 /**< Default. After being powered up, the element is in an On state and uses default state values. */
#define WICED_BT_MESH_ON_POWER_UP_STATE_RESTORE                         0x02 /**< Restore. If a transition was in progress when powered down, the element restores the target state when powered up. Otherwise the element restores the state it was in when powered down. */
/** @} WICED_BT_MESH_ON_POWER_UP_STATE */

/**
 * @anchor WICED_BT_MESH_STATUS_CODE
 * @name Status code definitions
 * @{
 */
#define WICED_BT_MESH_STATUS_SUCCESS                                    0x00 /**< Command successfully processed */
#define WICED_BT_MESH_STATUS_CANNOT_SET_RANGE_MIN                       0x01 /**< The provided value for Range Min cannot be set */
#define WICED_BT_MESH_STATUS_CANNOT_SET_RANGE_MAX                       0x02 /**< The provided value for Range Max cannot be set */
/** @} WICED_BT_MESH_STATUS_CODE */

/**
 * @anchor WICED_BT_MESH_SCENE_STATUS_CODE
 * @name Scene operation Status code definitions
 * @{
 */
#define WICED_BT_MESH_SCENE_STATUS_SUCCESS                              0x00 /**< Command successfully processed */
#define WICED_BT_MESH_SCENE_STATUS_REGISTER_FULL                        0x01 /**< Scene register full */
#define WICED_BT_MESH_SCENE_STATUS_NOT_FOUND                            0x02 /**< Scene not found */
/** @} WICED_BT_MESH_SCENE_STATUS_CODE */

#define MESH_DEFAULT_TRANSITION_TIME_IN_MS                              0    /**< Default transition time */

/**
 * @anchor WICED_BT_MESH_BEACON_TYPE
 * @name While device is unprovisioned, it advertises Unprovisioned beacons. After provisioning switches to Secure Network Beacons
 * @{
 */
#define WICED_BT_MESH_BEACON_TYPE_UNPROVISIONED    0x00
#define WICED_BT_MESH_BEACON_TYPE_SECURE_NETWORK   0x01
#define WICED_BT_MESH_BEACON_TYPE_MESH_PRIVATE     0x02
/** @} WICED_BT_MESH_BEACON_TYPE */

/**
 * @anchor WICED_BT_MESH_UNPROVISIONED_BEACON_FIELDS
 * @name Unprovisioned beacon fields
 * @{
 */
#define WICED_BT_MESH_PROVISION_ADV_OOB_LEN       2
#define WICED_BT_MESH_PROVISION_ADV_URI_HASH_LEN  4
/** @} WICED_BT_MESH_UNPROVISIONED_BEACON_FIELDS */

/**
 * @anchor WICED_BT_MESH_PROPERTY
 * @name Mesh Device Properties as defined in the Mesh Device Property sepcification
 * @{
 */
#define WICED_BT_MESH_PROPERTY_UNKNOWN                                                  0x0000
#define WICED_BT_MESH_PROPERTY_AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY           0x0001
#define WICED_BT_MESH_PROPERTY_AVERAGE_INPUT_CURRENT                                    0x0002
#define WICED_BT_MESH_PROPERTY_AVERAGE_INPUT_VOLTAGE                                    0x0003
#define WICED_BT_MESH_PROPERTY_AVERAGE_OUTPUT_CURRENT                                   0x0004
#define WICED_BT_MESH_PROPERTY_AVERAGE_OUTPUT_VOLTAGE                                   0x0005
#define WICED_BT_MESH_PROPERTY_CENTER_BEAM_INTENSITY_AT_FULL_POWER                      0x0006
#define WICED_BT_MESH_PROPERTY_CHROMATICALLY_TOLERANCE                                  0x0007
#define WICED_BT_MESH_PROPERTY_COLOR_RENDERING_INDEX_R9                                 0x0008
#define WICED_BT_MESH_PROPERTY_COLOR_RENDERING_INDEX_RA                                 0x0009
#define WICED_BT_MESH_PROPERTY_DEVICE_APPEARANCE                                        0x000A
#define WICED_BT_MESH_PROPERTY_DEVICE_COUNTRY_OF_ORIGIN                                 0x000B
#define WICED_BT_MESH_PROPERTY_DEVICE_DATE_OF_MANUFACTURE                               0x000C
#define WICED_BT_MESH_PROPERTY_DEVICE_ENERGY_USE_SINCE_TURN_ON                          0x000D
#define WICED_BT_MESH_PROPERTY_DEVICE_FIRMWARE_REVISION                                 0x000E
#define WICED_BT_MESH_PROPERTY_DEVICE_GLOBAL_TRADE_ITEM_NUMBER                          0x000F
#define WICED_BT_MESH_PROPERTY_DEVICE_HARDWARE_REVISION                                 0x0010
#define WICED_BT_MESH_PROPERTY_DEVICE_MANUFACTURER_NAME                                 0x0011
#define WICED_BT_MESH_PROPERTY_DEVICE_MODEL_NUMBER                                      0x0012
#define WICED_BT_MESH_PROPERTY_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION         0x0013
#define WICED_BT_MESH_PROPERTY_DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES          0x0014
#define WICED_BT_MESH_PROPERTY_DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS                 0x0015
#define WICED_BT_MESH_PROPERTY_DEVICE_POWER_RANGE_SPECIFICATION                         0x0016
#define WICED_BT_MESH_PROPERTY_DEVICE_RUNTIME_SINCE_TURN_ON                             0x0017
#define WICED_BT_MESH_PROPERTY_DEVICE_RUNTIME_WARRANTY                                  0x0018
#define WICED_BT_MESH_PROPERTY_DEVICE_SERIAL_NUMBER                                     0x0019
#define WICED_BT_MESH_PROPERTY_DEVICE_SOFTWARE_REVISION                                 0x001A
#define WICED_BT_MESH_PROPERTY_DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS                0x001B
#define WICED_BT_MESH_PROPERTY_INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES            0x001C
#define WICED_BT_MESH_PROPERTY_INITIAL_CIE_CHROMATICITY_COORDINATES                     0x001D
#define WICED_BT_MESH_PROPERTY_INITIAL_CORRELATED_COLOR_TEMPERATURE                     0x001E
#define WICED_BT_MESH_PROPERTY_INITIAL_LUMINOUS_FLUX                                    0x001F
#define WICED_BT_MESH_PROPERTY_INITIAL_PLANCKIAN_DISTANCE                               0x0020
#define WICED_BT_MESH_PROPERTY_INPUT_CURRENT_RANGE_SPECIFICATION                        0x0021
#define WICED_BT_MESH_PROPERTY_INPUT_CURRENT_STATISTICS                                 0x0022
#define WICED_BT_MESH_PROPERTY_INPUT_OVER_CURRENT_EVENT_STATISTICS                      0x0023
#define WICED_BT_MESH_PROPERTY_INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS               0x0024
#define WICED_BT_MESH_PROPERTY_INPUT_OVER_VOLTAGE_EVENT_STATISTICS                      0x0025
#define WICED_BT_MESH_PROPERTY_INPUT_UNDER_CURRENT_EVENT_STATISTICS                     0x0026
#define WICED_BT_MESH_PROPERTY_INPUT_UNDER_VOLTAGE_EVENT_STATISTICS                     0x0027
#define WICED_BT_MESH_PROPERTY_INPUT_VOLTAGE_RANGE_SPECIFICATION                        0x0028
#define WICED_BT_MESH_PROPERTY_INPUT_VOLTAGE_RIPPLE_SPECIFICATION                       0x0029
#define WICED_BT_MESH_PROPERTY_INPUT_VOLTAGE_STATISTICS                                 0x002A
#define WICED_BT_MESH_PROPERTY_AMBIENT_LUX_LEVEL_ON                                     0x002B
#define WICED_BT_MESH_PROPERTY_AMBIENT_LUX_LEVEL_PROLONG                                0x002C
#define WICED_BT_MESH_PROPERTY_AMBIENT_LUX_LEVEL_STANDBY                                0x002D
#define WICED_BT_MESH_PROPERTY_LIGHTNESS_ON                                             0x002E
#define WICED_BT_MESH_PROPERTY_LIGHTNESS_PROLONG                                        0x002F
#define WICED_BT_MESH_PROPERTY_LIGHTNESS_STANDBY                                        0x0030
#define WICED_BT_MESH_PROPERTY_REGULATOR_ACCURACY                                       0x0031
#define WICED_BT_MESH_PROPERTY_REGULATOR_KID                                            0x0032
#define WICED_BT_MESH_PROPERTY_REGULATOR_KIU                                            0x0033
#define WICED_BT_MESH_PROPERTY_REGULATOR_KPD                                            0x0034
#define WICED_BT_MESH_PROPERTY_REGULATOR_KPU                                            0x0035
#define WICED_BT_MESH_PROPERTY_TIME_FADE                                                0x0036
#define WICED_BT_MESH_PROPERTY_TIME_FADE_ON                                             0x0037
#define WICED_BT_MESH_PROPERTY_TIME_FADE_STANDBY_AUTO                                   0x0038
#define WICED_BT_MESH_PROPERTY_TIME_FADE_STANDBY_MANUAL                                 0x0039
#define WICED_BT_MESH_PROPERTY_TIME_OCCUPANCY_DELAY                                     0x003A
#define WICED_BT_MESH_PROPERTY_TIME_PROLONG                                             0x003B
#define WICED_BT_MESH_PROPERTY_TIME_RUN_ON                                              0x003C
#define WICED_BT_MESH_PROPERTY_LUMEN_MAINTENANCE_FACTOR                                 0x003D
#define WICED_BT_MESH_PROPERTY_LUMINOUS_EFFICICACY                                      0x003E
#define WICED_BT_MESH_PROPERTY_LUMINOUS_ENERGY_SINCE_TURN_ON                            0x003F
#define WICED_BT_MESH_PROPERTY_LUMINOUS_EXPOSURE                                        0x0040
#define WICED_BT_MESH_PROPERTY_LUMINOUS_FLUX_RANGE                                      0x0041
#define WICED_BT_MESH_PROPERTY_MOTION_SENSED                                            0x0042
#define WICED_BT_MESH_PROPERTY_MOTION_THRESHOLD                                         0x0043
#define WICED_BT_MESH_PROPERTY_OPEN_CIRCUIT_EVENT_STATISTICS                            0x0044
#define WICED_BT_MESH_PROPERTY_OUTDOOR_STATISTICAL_VALUES                               0x0045
#define WICED_BT_MESH_PROPERTY_OUTPUT_CURRENT_RANGE                                     0x0046
#define WICED_BT_MESH_PROPERTY_OUTPUT_CURRENT_STATISTICS                                0x0047
#define WICED_BT_MESH_PROPERTY_OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION                      0x0048
#define WICED_BT_MESH_PROPERTY_OUTPUT_VOLTAGE_RANGE                                     0x0049
#define WICED_BT_MESH_PROPERTY_OUTPUT_VOLTAGE_STATISTICS                                0x004A
#define WICED_BT_MESH_PROPERTY_OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS              0x004B
#define WICED_BT_MESH_PROPERTY_PEOPLE_COUNT                                             0x004C
#define WICED_BT_MESH_PROPERTY_PRESENCE_DETECTED                                        0x004D
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_LIGHT_LEVEL                              0x004E
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_TEMPERATURE                              0x004F
#define WICED_BT_MESH_PROPERTY_PRESENT_CIE_CHROMATICITY_COORDINATES                     0x0050
#define WICED_BT_MESH_PROPERTY_PRESENT_CORRELATED_COLOR_TEMPERATURE                     0x0051
#define WICED_BT_MESH_PROPERTY_PRESENT_DEVICE_INPUT_POWER                               0x0052
#define WICED_BT_MESH_PROPERTY_PRESENT_DEVICE_OPERATING_EFFICIENCY                      0x0053
#define WICED_BT_MESH_PROPERTY_PRESENT_DEVICE_OPERATING_TEMPERATURE                     0x0054
#define WICED_BT_MESH_PROPERTY_PRESENT_ILLUMINANCE                                      0x0055
#define WICED_BT_MESH_PROPERTY_PRESENT_INDOOR_AMBIENT_TEMPERATURE                       0x0056
#define WICED_BT_MESH_PROPERTY_PRESENT_INPUT_CURRENT                                    0x0057
#define WICED_BT_MESH_PROPERTY_PRESENT_INPUT_RIPPLE_VOLTAGE                             0x0058
#define WICED_BT_MESH_PROPERTY_PRESENT_INPUT_VOLTAGE                                    0x0059
#define WICED_BT_MESH_PROPERTY_PRESENT_LUMINOUS_FLUX                                    0x005A
#define WICED_BT_MESH_PROPERTY_PRESENT_OUTDOOR_AMBIENT_TEMPERATURE                      0x005B
#define WICED_BT_MESH_PROPERTY_PRESENT_OUTPUT_CURRENT                                   0x005C
#define WICED_BT_MESH_PROPERTY_PRESENT_OUTPUT_VOLTAGE                                   0x005D
#define WICED_BT_MESH_PROPERTY_PRESENT_PLANCKIAN_DISTANCE                               0x005E
#define WICED_BT_MESH_PROPERTY_PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE                   0x005F
#define WICED_BT_MESH_PROPERTY_RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY            0x0060
#define WICED_BT_MESH_PROPERTY_RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE         0x0061
#define WICED_BT_MESH_PROPERTY_RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE           0x0062
#define WICED_BT_MESH_PROPERTY_RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE 0x0063
#define WICED_BT_MESH_PROPERTY_RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE 0x0064
#define WICED_BT_MESH_PROPERTY_RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE               0x0065
#define WICED_BT_MESH_PROPERTY_RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE               0x0066
#define WICED_BT_MESH_PROPERTY_SHORT_CIRCUIT_EVENT_STATISTICS                           0x0067
#define WICED_BT_MESH_PROPERTY_TIME_SINCE_MOTION_SENSED                                 0x0068
#define WICED_BT_MESH_PROPERTY_TIME_SINCE_PRESENCE_DETECTED                             0x0069
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_ENERGY_USE                                  0x006A
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_OFF_ON_CYCLES                               0x006B
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_POWER_ON_CYCLES                             0x006C
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_POWER_ON_TIME                               0x006D
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_RUNTIME                                     0x006E
#define WICED_BT_MESH_PROPERTY_TOTAL_LIGHT_EXPOSURE_TIME                                0x006F
#define WICED_BT_MESH_PROPERTY_TOTAL_LUMINOUS_ENERGY                                    0x0070
#define WICED_BT_MESH_PROPERTY_DESIRED_AMBIENT_TEMPERATURE                              0x0071
#define WICED_BT_MESH_PROPERTY_PRECISE_TOTAL_DEVICE_ENERGY_USE                          0x0072
#define WICED_BT_MESH_PROPERTY_POWER_FACTOR                                             0x0073
#define WICED_BT_MESH_PROPERTY_SENSOR_GAIN                                              0x0074
#define WICED_BT_MESH_PROPERTY_PRECISE_PRESENT_AMBIENT_TEMPERATURE                      0x0075
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_RELATIVE_HUMIDITY                        0x0076
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_CO2_CONCENTRAION                         0x0077
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_ORGRANIC_COMPOUNDS_CONCENTRATION         0x0078
#define WICED_BT_MESH_PROPERTY_PRESENT_AMBIENT_NOISE                                    0x0079
#define WICED_BT_MESH_PROPERTY_ACTIVE_ENERGY_LOADSIDE                                   0x0080
#define WICED_BT_MESH_PROPERTY_ACTOVE_POWER_LOADSIDE                                    0x0081
#define WICED_BT_MESH_PROPERTY_ACTOVE_AIR_PRESSURE                                      0x0082
#define WICED_BT_MESH_PROPERTY_APPARENT_ENERGY                                          0x0083
#define WICED_BT_MESH_PROPERTY_APPARENT_POWER                                           0x0084
#define WICED_BT_MESH_PROPERTY_APPARENT_WIND_DIRECTION                                  0x0085
#define WICED_BT_MESH_PROPERTY_APPARENT_WIND_SPEED                                      0x0086
#define WICED_BT_MESH_PROPERTY_DEW_POINT                                                0x0087
#define WICED_BT_MESH_PROPERTY_EXTERNAL_SUPPLY_VOLTAGE                                  0x0088
#define WICED_BT_MESH_PROPERTY_EXTERNAL_SUPPLY_VOLTAGE_FREQUENCY                        0x0089
#define WICED_BT_MESH_PROPERTY_GUST_FACTOR                                              0x008A
#define WICED_BT_MESH_PROPERTY_HEAT_INDEX                                               0x008B
#define WICED_BT_MESH_PROPERTY_LIGHT_DISTRIBUTION                                       0x008C
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_CURRENT                                     0x008D
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_ON_TIME_NOT_RESETTABLE                      0x008E
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_ON_TIME_RESETTABLE                          0x008F
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_OPEN_CIRCUIT_STATISTICS                     0x0090
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_OVERALL_FAILURE_STATISTICS                  0x0091
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_SHORT_CIRCUIT_STATISTICS                    0x0092
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_START_COUNTER_RESETTABLE                    0x0093
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_TEMPERATURE                                 0x0094
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_THERMAL_DERATING_STATISTICS                 0x0095
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_THERMAL_DERATING_SHUTDOWN_STATISTICS        0x0096
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_TOTAL_POWER_ON_CYCLES                       0x0097
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_VOLTAGE                                     0x0098
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_COLOR                                          0x0099
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_IDENTIFICATION_NUMBER                          0x009A
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_MANUFACTURER_GTIN                              0x009B
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_NOMINAL_INPUT_POWER                            0x009C
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_NOMINAL_MAXIMUM_AC_MAINS_VOLTAGE               0x009D
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_NOMINAL_MINIMUM_AC_MAINS_VOLTAGE               0x009E
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_POWER_AT_MINIMUM_DIM_LEVEL                     0x009F
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_TIME_OF_MANUFACTURE                            0x00A0
#define WICED_BT_MESH_PROPERTY_MAGNETIC_DECLINATION                                     0x00A1
#define WICED_BT_MESH_PROPERTY_MAGNETIC_FLUX_DENSITY_2D                                 0x00A2
#define WICED_BT_MESH_PROPERTY_MAGNETIC_FLUX_DENSITY_3D                                 0x00A3
#define WICED_BT_MESH_PROPERTY_NOMINAL_LIGHT_OUTPUT                                     0x00A4
#define WICED_BT_MESH_PROPERTY_OVERALL_FAILURE_CONDITION                                0x00A5
#define WICED_BT_MESH_PROPERTY_POLLEN_CONCENTRATION                                     0x00A6
#define WICED_BT_MESH_PROPERTY_PRESENT_INDOOR_RELATIVE_HUMIDITY                         0x00A7
#define WICED_BT_MESH_PROPERTY_PRESENT_OUTDOOR_RELATIVE_HUMIDITY                        0x00A8
#define WICED_BT_MESH_PROPERTY_PRESSURE                                                 0x00A9
#define WICED_BT_MESH_PROPERTY_RAINFALL                                                 0x00AA
#define WICED_BT_MESH_PROPERTY_RATED_MEDIAN_USEFUL_LIFE_OF_LUMINAIRE                    0x00AB
#define WICED_BT_MESH_PROPERTY_RATED_MEDIAN_USEFUL_LIGHT_SOURCE_STARTS                  0x00AC
#define WICED_BT_MESH_PROPERTY_REFERENCE_TEMPERATURE                                    0x00AD
#define WICED_BT_MESH_PROPERTY_TOTAL_DEVICE_STARTS                                      0x00AE
#define WICED_BT_MESH_PROPERTY_TRUE_WIND_DIRECTION                                      0x00AF
#define WICED_BT_MESH_PROPERTY_TRUE_WIND_SPEED                                          0x00B0
#define WICED_BT_MESH_PROPERTY_UV_INDEX                                                 0x00B1
#define WICED_BT_MESH_PROPERTY_WIND_CHILL                                               0x00B2
#define WICED_BT_MESH_PROPERTY_LIGHT_SOURCE_TYPE                                        0x00B3
#define WICED_BT_MESH_PROPERTY_LUMINAIRE_IDENTIFICATION_STRING                          0x00B4
#define WICED_BT_MESH_PROPERTY_OUTPUT_POWER_LIMITATION                                  0x00B5
#define WICED_BT_MESH_PROPERTY_THERMAL_DERATING                                         0x00B6
#define WICED_BT_MESH_PROPERTY_OUTPUT_CURRENT_PERCENT                                   0x00B7

#define WICED_BT_MESH_MAX_PROPERTY_ID                                                   WICED_BT_MESH_PROPERTY_OUTPUT_CURRENT_PERCENT

/* TBD need to replace 0s with real numbers */
#define WICED_BT_MESH_PROPERTY_LEN_UNKNOWN                                                  0
#define WICED_BT_MESH_PROPERTY_LEN_AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY           1
#define WICED_BT_MESH_PROPERTY_LEN_AVERAGE_INPUT_CURRENT                                    0
#define WICED_BT_MESH_PROPERTY_LEN_AVERAGE_INPUT_VOLTAGE                                    0
#define WICED_BT_MESH_PROPERTY_LEN_AVERAGE_OUTPUT_CURRENT                                   0
#define WICED_BT_MESH_PROPERTY_LEN_AVERAGE_OUTPUT_VOLTAGE                                   0
#define WICED_BT_MESH_PROPERTY_LEN_CENTER_BEAM_INTENSITY_AT_FULL_POWER                      0
#define WICED_BT_MESH_PROPERTY_LEN_CHROMATICALLY_TOLERANCE                                  0
#define WICED_BT_MESH_PROPERTY_LEN_COLOR_RENDERING_INDEX_R9                                 0
#define WICED_BT_MESH_PROPERTY_LEN_COLOR_RENDERING_INDEX_RA                                 0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_APPEARANCE                                        2
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_COUNTRY_OF_ORIGIN                                 0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_DATE_OF_MANUFACTURE                               0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_ENERGY_USE_SINCE_TURN_ON                          0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_FIRMWARE_REVISION                                 8
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_GLOBAL_TRADE_ITEM_NUMBER                          14
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_HARDWARE_REVISION                                 16
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_MANUFACTURER_NAME                                 36
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_MODEL_NUMBER                                      24
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION         0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES          0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS                 0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_POWER_RANGE_SPECIFICATION                         0
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_RUNTIME_SINCE_TURN_ON                             3
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_RUNTIME_WARRANTY                                  3
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_SERIAL_NUMBER                                     16
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_SOFTWARE_REVISION                                 8
#define WICED_BT_MESH_PROPERTY_LEN_DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS                0
#define WICED_BT_MESH_PROPERTY_LEN_INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES            0
#define WICED_BT_MESH_PROPERTY_LEN_INITIAL_CIE_CHROMATICITY_COORDINATES                     0
#define WICED_BT_MESH_PROPERTY_LEN_INITIAL_CORRELATED_COLOR_TEMPERATURE                     0
#define WICED_BT_MESH_PROPERTY_LEN_INITIAL_LUMINOUS_FLUX                                    0
#define WICED_BT_MESH_PROPERTY_LEN_INITIAL_PLANCKIAN_DISTANCE                               0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_CURRENT_RANGE_SPECIFICATION                        0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_CURRENT_STATISTICS                                 0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_OVER_CURRENT_EVENT_STATISTICS                      0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS               0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_OVER_VOLTAGE_EVENT_STATISTICS                      0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_UNDER_CURRENT_EVENT_STATISTICS                     0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_UNDER_VOLTAGE_EVENT_STATISTICS                     0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_VOLTAGE_RANGE_SPECIFICATION                        0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_VOLTAGE_RIPPLE_SPECIFICATION                       0
#define WICED_BT_MESH_PROPERTY_LEN_INPUT_VOLTAGE_STATISTICS                                 0
#define WICED_BT_MESH_PROPERTY_LEN_AMBIENT_LUX_LEVEL_ON                                     3
#define WICED_BT_MESH_PROPERTY_LEN_AMBIENT_LUX_LEVEL_PROLONG                                3
#define WICED_BT_MESH_PROPERTY_LEN_AMBIENT_LUX_LEVEL_STANDBY                                3
#define WICED_BT_MESH_PROPERTY_LEN_LIGHTNESS_ON                                             2
#define WICED_BT_MESH_PROPERTY_LEN_LIGHTNESS_PROLONG                                        2
#define WICED_BT_MESH_PROPERTY_LEN_LIGHTNESS_STANDBY                                        2
#define WICED_BT_MESH_PROPERTY_LEN_REGULATOR_ACCURACY                                       1
#define WICED_BT_MESH_PROPERTY_LEN_REGULATOR_KID                                            4
#define WICED_BT_MESH_PROPERTY_LEN_REGULATOR_KIU                                            4
#define WICED_BT_MESH_PROPERTY_LEN_REGULATOR_KPD                                            4
#define WICED_BT_MESH_PROPERTY_LEN_REGULATOR_KPU                                            4
#define WICED_BT_MESH_PROPERTY_LEN_TIME_FADE                                                3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_FADE_ON                                             3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_FADE_STANDBY_AUTO                                   3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_FADE_STANDBY_MANUAL                                 3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_OCCUPANCY_DELAY                                     3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_PROLONG                                             3
#define WICED_BT_MESH_PROPERTY_LEN_TIME_RUN_ON                                              3
#define WICED_BT_MESH_PROPERTY_LEN_LUMEN_MAINTENANCE_FACTOR                                 1
#define WICED_BT_MESH_PROPERTY_LEN_LUMINOUS_EFFICICACY                                      0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINOUS_ENERGY_SINCE_TURN_ON                            0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINOUS_EXPOSURE                                        0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINOUS_FLUX_RANGE                                      0
#define WICED_BT_MESH_PROPERTY_LEN_MOTION_SENSED                                            1
#define WICED_BT_MESH_PROPERTY_LEN_MOTION_THRESHOLD                                         1
#define WICED_BT_MESH_PROPERTY_LEN_OPEN_CIRCUIT_EVENT_STATISTICS                            0
#define WICED_BT_MESH_PROPERTY_LEN_OUTDOOR_STATISTICAL_VALUES                               0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_CURRENT_RANGE                                     0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_CURRENT_STATISTICS                                0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION                      1
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_VOLTAGE_RANGE                                     0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_VOLTAGE_STATISTICS                                0
#define WICED_BT_MESH_PROPERTY_LEN_OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS              0
#define WICED_BT_MESH_PROPERTY_LEN_PEOPLE_COUNT                                             2
#define WICED_BT_MESH_PROPERTY_LEN_PRESENCE_DETECTED                                        1
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_LIGHT_LEVEL                              3
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_TEMPERATURE                              1
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_CIE_CHROMATICITY_COORDINATES                     0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_CORRELATED_COLOR_TEMPERATURE                     0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_DEVICE_INPUT_POWER                               3
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_DEVICE_OPERATING_EFFICIENCY                      0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_DEVICE_OPERATING_TEMPERATURE                     0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_ILLUMINANCE                                      3
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_INDOOR_AMBIENT_TEMPERATURE                       1
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_INPUT_CURRENT                                    0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_INPUT_RIPPLE_VOLTAGE                             0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_INPUT_VOLTAGE                                    0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_LUMINOUS_FLUX                                    0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_OUTDOOR_AMBIENT_TEMPERATURE                      1
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_OUTPUT_CURRENT                                   0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_OUTPUT_VOLTAGE                                   0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_PLANCKIAN_DISTANCE                               0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE                   1
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY            0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE         0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE           0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE 0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE 0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE               0
#define WICED_BT_MESH_PROPERTY_LEN_RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE               0
#define WICED_BT_MESH_PROPERTY_LEN_SHORT_CIRCUIT_EVENT_STATISTICS                           0
#define WICED_BT_MESH_PROPERTY_LEN_TIME_SINCE_MOTION_SENSED                                 2
#define WICED_BT_MESH_PROPERTY_LEN_TIME_SINCE_PRESENCE_DETECTED                             2
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_ENERGY_USE                                  0
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_OFF_ON_CYCLES                               3
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_POWER_ON_CYCLES                             3
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_POWER_ON_TIME                               3
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_RUNTIME                                     3
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_LIGHT_EXPOSURE_TIME                                3
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_LUMINOUS_ENERGY                                    0
#define WICED_BT_MESH_PROPERTY_LEN_DESIRED_AMBIENT_TEMPERATURE                              0
#define WICED_BT_MESH_PROPERTY_LEN_PRECISE_TOTAL_DEVICE_ENERGY_USE                          4
#define WICED_BT_MESH_PROPERTY_LEN_POWER_FACTOR                                             0
#define WICED_BT_MESH_PROPERTY_LEN_SENSOR_GAIN                                              4
#define WICED_BT_MESH_PROPERTY_LEN_PRECISE_PRESENT_AMBIENT_TEMPERATURE                      0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_RELATIVE_HUMIDITY                        0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_CO2_CONCENTRAION                         0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_ORGRANIC_COMPOUNDS_CONCENTRATION         0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_AMBIENT_NOISE                                    0
#define WICED_BT_MESH_PROPERTY_LEN_ACTIVE_ENERGY_LOADSIDE                                   0
#define WICED_BT_MESH_PROPERTY_LEN_ACTOVE_POWER_LOADSIDE                                    0
#define WICED_BT_MESH_PROPERTY_LEN_ACTOVE_AIR_PRESSURE                                      0
#define WICED_BT_MESH_PROPERTY_LEN_APPARENT_ENERGY                                          0
#define WICED_BT_MESH_PROPERTY_LEN_APPARENT_POWER                                           0
#define WICED_BT_MESH_PROPERTY_LEN_APPARENT_WIND_DIRECTION                                  0
#define WICED_BT_MESH_PROPERTY_LEN_APPARENT_WIND_SPEED                                      0
#define WICED_BT_MESH_PROPERTY_LEN_DEW_POINT                                                0
#define WICED_BT_MESH_PROPERTY_LEN_EXTERNAL_SUPPLY_VOLTAGE                                  0
#define WICED_BT_MESH_PROPERTY_LEN_EXTERNAL_SUPPLY_VOLTAGE_FREQUENCY                        0
#define WICED_BT_MESH_PROPERTY_LEN_GUST_FACTOR                                              0
#define WICED_BT_MESH_PROPERTY_LEN_HEAT_INDEX                                               0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_DISTRIBUTION                                       0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_CURRENT                                     0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_ON_TIME_NOT_RESETTABLE                      0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_ON_TIME_RESETTABLE                          0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_OPEN_CIRCUIT_STATISTICS                     0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_OVERALL_FAILURE_STATISTICS                  0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_SHORT_CIRCUIT_STATISTICS                    0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_START_COUNTER_RESETTABLE                    0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_TEMPERATURE                                 0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_THERMAL_DERATING_STATISTICS                 0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_THERMAL_DERATING_SHUTDOWN_STATISTICS        0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_TOTAL_POWER_ON_CYCLES                       0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_VOLTAGE                                     0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_COLOR                                          0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_IDENTIFICATION_NUMBER                          0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_MANUFACTURER_GTIN                              0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_NOMINAL_INPUT_POWER                            0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_NOMINAL_MAXIMUM_AC_MAINS_VOLTAGE               0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_NOMINAL_MINIMUM_AC_MAINS_VOLTAGE               0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_POWER_AT_MINIMUM_DIM_LEVEL                     0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_TIME_OF_MANUFACTURE                            0
#define WICED_BT_MESH_PROPERTY_LEN_MAGNETIC_DECLINATION                                     0
#define WICED_BT_MESH_PROPERTY_LEN_MAGNETIC_FLUX_DENSITY_2D                                 0
#define WICED_BT_MESH_PROPERTY_LEN_MAGNETIC_FLUX_DENSITY_3D                                 0
#define WICED_BT_MESH_PROPERTY_LEN_NOMINAL_LIGHT_OUTPUT                                     0
#define WICED_BT_MESH_PROPERTY_LEN_OVERALL_FAILURE_CONDITION                                0
#define WICED_BT_MESH_PROPERTY_LEN_POLLEN_CONCENTRATION                                     0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_INDOOR_RELATIVE_HUMIDITY                         0
#define WICED_BT_MESH_PROPERTY_LEN_PRESENT_OUTDOOR_RELATIVE_HUMIDITY                        0
#define WICED_BT_MESH_PROPERTY_LEN_PRESSURE                                                 0
#define WICED_BT_MESH_PROPERTY_LEN_RAINFALL                                                 0
#define WICED_BT_MESH_PROPERTY_LEN_RATED_MEDIAN_USEFUL_LIFE_OF_LUMINAIRE                    0
#define WICED_BT_MESH_PROPERTY_LEN_RATED_MEDIAN_USEFUL_LIGHT_SOURCE_STARTS                  0
#define WICED_BT_MESH_PROPERTY_LEN_REFERENCE_TEMPERATURE                                    0
#define WICED_BT_MESH_PROPERTY_LEN_TOTAL_DEVICE_STARTS                                      0
#define WICED_BT_MESH_PROPERTY_LEN_TRUE_WIND_DIRECTION                                      0
#define WICED_BT_MESH_PROPERTY_LEN_TRUE_WIND_SPEED                                          0
#define WICED_BT_MESH_PROPERTY_LEN_UV_INDEX                                                 0
#define WICED_BT_MESH_PROPERTY_LEN_WIND_CHILL                                               0
#define WICED_BT_MESH_PROPERTY_LEN_LIGHT_SOURCE_TYPE                                        0
#define WICED_BT_MESH_PROPERTY_LEN_LUMINAIRE_IDENTIFICATION_STRING                          0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_POWER_LIMITATION                                  0
#define WICED_BT_MESH_PROPERTY_LEN_THERMAL_DERATING                                         0
#define WICED_BT_MESH_PROPERTY_LEN_OUTPUT_CURRENT_PERCENT                                   0
/** @} WICED_BT_MESH_PROPERTY */

/* Define the format of the Status Trigger Type */
#define WICED_BT_MESH_SENSOR_STATUS_TRIGGER_TYPE_FORMAT_NATIVE                              0
#define WICED_BT_MESH_SENSOR_STATUS_TRIGGER_TYPE_FORMAT_PERCENTAGE                          1
#define WICED_BT_MESH_SENSOR_MAX_SAMPLING_FUNCTION                                          7

#define WICED_BT_MESH_SENSOR_TOLERANCE_UNSPECIFIED                                          0

/* Define sensor sampling function types */
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_UNKNOWN                                      0
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_INSTANTANEOUS                                1
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_ARITHMATIC_MEAN                              2
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_RMS                                          3
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_MAXIMUM                                      4
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_MINIMUM                                      5
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_ACCUMULATED                                  6
#define WICED_BT_MESH_SENSOR_SAMPLING_FUNCTION_COUNT                                        7

#define WICED_BT_MESH_SENSOR_VAL_UNKNOWN                                                    0


// Only values of 0x00 through 0x3E shall be used to specify the value of the Transition Number of Steps field.
#define WICED_BT_MESH_VALID_TRANSITION_TIME(x) (((x) & 0x3f) != 0x3f)

/* Macros to convert light lightness linear to actual and actual to linear */
/* actual = 65535 * sqrt (linear / 65535) */
/* linear = Ceiling (65535 * square (actual / 65535) */
#define LIGHT_LIGHTNESS_LINEAR_FROM_ACTUAL(x)         (((uint32_t)(x) * (x) + 65534) / 65535)
#define LIGHT_LIGHTNESS_ACTUAL_FROM_LINEAR(x)         (utl_sqrt((uint32_t)65535 * (x)))

/* Color Temperature range as defined in Light CTL model */
#define WICED_BT_MESH_LIGHT_CTL_TEMPERATURE_MIN       0x0320
#define WICED_BT_MESH_LIGHT_CTL_TEMPERATURE_MAX       0x4e20

/* Time Roles defines the role of a node in propagation of time information in a mesh network. */
#define WICED_BT_MESH_TIME_ROLE_NONE                  0x00 /**< The element does not participate in propagation of time information */
#define WICED_BT_MESH_TIME_ROLE_MESH_TIME_AUTHORITY   0x01 /**< The element publishes Time Status messages but does not process received Time Status messages */
#define WICED_BT_MESH_TIME_ROLE_MESH_TIME_RELAY       0x02 /**< The element processes received and publishes Time Status messages */
#define WICED_BT_MESH_TIME_ROLE_MESH_TIME_CLIENT      0x03 /**< The element does not publish but processes received Time Status messages */

/**
 * @anchor MESH_BEACON_STATES
 * @name Mesh Beacon States
 * @{
 */
#define WICED_BT_MESH_BEACON_NOT_BROADCASTING           0x00 /**< The node is not broadcasting a Secure Network beacon */
#define WICED_BT_MESH_BEACON_BROADCASTING               0x01 /**< The node is broadcasting a Secure Network beacon */
/** @} MESH_BEACON_STATES */

/**
 * @anchor MESH_GATT_PROXY_STATES
 * @name Mesh GATT Proxy States
 * @{
 */
#define WICED_BT_MESH_GATT_PROXY_SERVICE_DISABLED       0x00 /**< The Mesh Proxy Service is running, Proxy feature is disabled */
#define WICED_BT_MESH_GATT_PROXY_SERVICE_ENABLED        0x01 /**< The Mesh Proxy Service is running, Proxy feature is enabled */
#define WICED_BT_MESH_GATT_PROXY_SERVICE_NOT_SUPPORTED  0x02 /**< The Mesh Proxy Service is not supported, Proxy feature is not supported */
/** @} MESH_GATT_PROXY_STATES */

/**
 * @anchor MESH_FRIEND_STATES
 * @name Mesh Friend States
 * @{
 */
#define WICED_BT_MESH_FRIEND_FEATURE_DISABLED           0x00 /**< The node supports Friend feature that is disabled */
#define WICED_BT_MESH_FRIEND_FEATURE_ENABLED            0x01 /**< The node supports Friend feature that is enabled */
#define WICED_BT_MESH_FRIEND_FEATURE_NOT_SUPPORTED      0x02 /**< The Friend feature is not supported */
/** @} MESH_GATT_PROXY_STATES */

/**
 * @anchor MESH_KEY_REFRESH_PHASES
 * @name Mesh Friend States
 * @{
 */
#define WICED_BT_MESH_KEY_REFRESH_PHASE_NORMAL          0x00 /**< Normal operation; Key Refresh procedure is not active */
#define WICED_BT_MESH_KEY_REFRESH_PHASE_FIRST           0x01 /**< First phase of Key Refresh procedure */
#define WICED_BT_MESH_KEY_REFRESH_PHASE_SECOND          0x02 /**< Second phase of Key Refresh procedure */
#define WICED_BT_MESH_KEY_REFRESH_PHASE_THIRD           0x03 /**< Third phase of Key Refresh procedure */
/** @} MESH_KEY_REFRESH_PHASES */

/**
 * @anchor MESH_KEY_REFRESH_TRANSITIONS
 * @name Mesh Friend Transitions
 * @{
 */
#define WICED_BT_MESH_KEY_REFRESH_TRANSITION_PHASE2     0x02 /**< Transition to key refresh phase 2 */
#define WICED_BT_MESH_KEY_REFRESH_TRANSITION_PHASE3     0x03 /**< Transition to key refresh phase 3 */
 /** @} MESH_KEY_REFRESH_TRANSITIONS */

/**
 * @anchor MESH_IDENTITY_STATES
 * @name Mesh Node Identity For a Subnet States
 * @{
 */
#define WICED_BT_MESH_GATT_NODE_IDENTITY_STOPPED        0x00 /**< Node Identity for a subnet is stopped */
#define WICED_BT_MESH_GATT_NODE_IDENTITY_RUNNING        0x01 /**< Node Identity for a subnet is running */
#define WICED_BT_MESH_GATT_NODE_IDENTITY_NOT_SUPPORTED  0x02 /**< Node Identity is not supported */
/** @} MESH_IDENTITY_STATES */

/**
 * @anchor MESH_PROVISIONING_DEVICE_KEY_REFRESH_PROCEDURES
 * @name Mesh Device Key Refresh Procedures
 * @{
 */
#define WICED_BT_MESH_PROVISION_PROCEDURE_DEV_KEY_REFRESH               0
#define WICED_BT_MESH_PROVISION_PROCEDURE_NODE_ADDRESS_REFRESH          1
#define WICED_BT_MESH_PROVISION_PROCEDURE_NODE_COMPOSITION_REFRESH      2

#define WICED_BT_MESH_PROVISION_PROCEDURE_PROVISION                     0xFF
 /** @} MESH_PROVISIONING_DEVICE_KEY_REFRESH_PROCEDURES */

/*------------------ Provisioning PDU types -------------------*/
/**
 * @anchor MESH_PROVISIONING_PDUS
 * @name Mesh Mesh Provisioning PDU Types
 * @{
 */
#define WICED_BT_MESH_PROVISION_PDU_TYPE_INVITE                         0x00
#define WICED_BT_MESH_PROVISION_PDU_TYPE_CAPABILITIES                   0x01
#define WICED_BT_MESH_PROVISION_PDU_TYPE_START                          0x02
#define WICED_BT_MESH_PROVISION_PDU_TYPE_PUBLIC_KEY                     0x03
#define WICED_BT_MESH_PROVISION_PDU_TYPE_INPUT_COMPLETE                 0x04
#define WICED_BT_MESH_PROVISION_PDU_TYPE_CONFIRMATION                   0x05
#define WICED_BT_MESH_PROVISION_PDU_TYPE_RANDOM                         0x06
#define WICED_BT_MESH_PROVISION_PDU_TYPE_DATA                           0x07
#define WICED_BT_MESH_PROVISION_PDU_TYPE_COMPLETE                       0x08
#define WICED_BT_MESH_PROVISION_PDU_TYPE_FAILED                         0x09
#define WICED_BT_MESH_PROVISION_PDU_TYPE_RECORD_REQUEST                 0x0A
#define WICED_BT_MESH_PROVISION_PDU_TYPE_RECORD_RESPONSE                0x0B
#define WICED_BT_MESH_PROVISION_PDU_TYPE_RECORDS_GET                    0x0C
#define WICED_BT_MESH_PROVISION_PDU_TYPE_RECORDS_LIST                   0x0D
 /** @} MESH_PROVISIONING_PDUS */

#define MESH_AUTH_VALUE_MAX_LEN                                         16  /**< static OOB max length is 128bits. For HMAC_SHA256_AES_CCM it two times longer(256 bits). input and output OOB max length is 8 bytes */

#define WICED_BT_MESH_REMOTE_PROVISIONING_SERVER_NO_SCANNING            0x00
#define WICED_BT_MESH_REMOTE_PROVISIONING_SERVER_MULTI_DEVICE_SCAN      0x01
#define WICED_BT_MESH_REMOTE_PROVISIONING_SERVER_SINGLE_DEVICE_SCAN     0x02

#define WICED_BT_MESH_REMOTE_PROVISIONING_LINK_OPEN_TIMEOUT_MIN         0x01
#define WICED_BT_MESH_REMOTE_PROVISIONING_LINK_OPEN_TIMEOUT_MAX         0x3C

#define WICED_BT_MESH_REMOTE_PROVISION_STATE_IDLE                       0x00    /**< Idle */
#define WICED_BT_MESH_REMOTE_PROVISION_STATE_LINK_OPENING               0x01    /**< Link Opening */
#define WICED_BT_MESH_REMOTE_PROVISION_STATE_LINK_ACTIVE                0x02    /**< Link Active */
#define WICED_BT_MESH_REMOTE_PROVISION_STATE_OUTBOUNT_PDU_TRANSFER      0x03    /**< Link Oubound PDU transfer */
#define WICED_BT_MESH_REMOTE_PROVISION_STATE_LINK_CLOSING               0x04    /**< Link Closing */

#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_SUCCESS                           0x00    /**< success */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_SCAN_CANNOT_START                 0x01    /**< Scanning Cannot Start  */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_INVALID_STATE                     0x02    /**< Invalid State */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_LIMITED_RESOURCES                 0x03    /**< Limited Resources */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CANNOT_OPEN                       0x04    /**< Link Cannot Open */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_OPEN_FAILED                       0x05    /**< Link Open Failed */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_BY_DEVICE                  0x06    /**< Link Closed by device */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_BY_SERVER                  0x07    /**< Link Closed by server */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_BY_CLIENT                  0x08    /**< Link Closed by client */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_CANNOT_RX_PDU              0x09    /**< Link Closed as Cannot Receive PDU */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_CANNOT_TX_PDU              0x0A    /**< Link Closed as Cannot Send PDU */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_FAIL_SEND_PDU_REPORT       0x0B    /**< Link Closed as Cannot Deliver PDU Report */
#define WICED_BT_MESH_REMOTE_PROVISION_STATUS_CLOSED_FAIL_SEND_PDU_OUT_REPORT   0x0C    /**< Link Closed as Cannot Deliver PDU Outbound Report */

#define WICED_BT_MESH_REMOTE_PROVISIONING_CLOSE_REASON_SUCCESS          0x00    /**< The operation completed successfully. */
#define WICED_BT_MESH_REMOTE_PROVISIONING_CLOSE_REASON_PROHIBITED       0x01    /**< Prohibited */
#define WICED_BT_MESH_REMOTE_PROVISIONING_CLOSE_REASON_FAIL             0x02    /**< Provisioning Failed */
#define WICED_BT_MESH_REMOTE_PROVISION_EXT_SCAN_DURATION_MIN            1       /**< Min length of time (in seconds) to collect information about the unprovisioned device */
#define WICED_BT_MESH_REMOTE_PROVISION_EXT_SCAN_DURATION_MAX            10      /**< Max length of time (in seconds) to collect information about the unprovisioned device */


/*------------------ Provisioning fields length -------------------*/
#define WICED_BT_MESH_PROVISION_PUBLIC_KEY_LEN    64
#define WICED_BT_MESH_PROVISION_ECDH_SECRET_LEN   32

#define WICED_BT_MESH_PROVISION_CONFIRMATION_LEN  16        /**< Mesh Confirmation length for algorithm CMAC_AES128_AES_CCM. algorithm HMAC_SHA256_AES_CCM uses two times bigger conformation */
#define WICED_BT_MESH_PROVISION_RANDOM_LEN        16        /**< Mesh Provisioning Random length for algorithm CMAC_AES128_AES_CCM. algorithm HMAC_SHA256_AES_CCM uses two times bigger random */
#define WICED_BT_MESH_IV_INDEX_LEN                4         /**< mesh IV (Initialization Vector) index length */
#define WICED_BT_MESH_ADDR_LEN                    2         /**< mesh device ID (address) length */
#define WICED_BT_MESH_PROVISION_DATA_LEN          (WICED_BT_MESH_KEY_LEN + 2 + 1 + WICED_BT_MESH_IV_INDEX_LEN + WICED_BT_MESH_ADDR_LEN)
#define WICED_BT_MESH_PROVISION_MIC_LEN           8
#define WICED_BT_MESH_PROVISION_MAX_PKT_LEN       (WICED_BT_MESH_PROVISION_PUBLIC_KEY_LEN + 1)

#define WICED_BT_MESH_IS_GROUP_ADDR(x)   (((x) & 0xC000) == 0xC000)
#define WICED_BT_MESH_IS_UNICAST_ADDR(x) (((x) & 0x8000) == 0)

#define WICED_BT_MESH_CONFIG_PERIOD_LOG_MAX                         0x11    /**< Max value of period log */
#define WICED_BT_MESH_CONFIG_SUB_COUNT_LOG_MAX                      0x10    /**< Max value of subscription count log */
#define WICED_BT_MESH_CONFIG_PUB_COUNT_LOG_MAX                      0x11    /**< Max value of publication count log */
#define WICED_BT_MESH_CONFIG_COUNT_LOG_NO_LIMIT                     0xFF    /**< Value of count log representing unlimited count */
#define WICED_BT_MESH_CONFIG_HOPS_MAX                               0x7f    /**< Max value of min hops or max hops */
#define WICED_BT_MESH_CONFIG_TTL_MAX                                0x7F    /**< Max value of the TTL */


#ifdef __cplusplus
}
#endif

#endif /* __FND_H__ */
