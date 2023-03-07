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
* \file <wiced_bt_mesh_app.h>
* Definitions for interface for mesh application interface library
*
******************************************************************************/

#ifndef __WICED_BT_MESH_APPS_H__
#define __WICED_BT_MESH_APPS_H__

#include "wiced_bt_mesh_event.h"
#include "wiced_bt_mesh_model_defs.h"
#include "wiced_bt_gatt.h"

#define TRACE_NONE      0
#define TRACE_CRITICAL  1
#define TRACE_ERROR     1
#define TRACE_WARNING   2
#define TRACE_INFO      3
#define TRACE_DEBUG     4

/*
 * Following bits may be set in the uuid[4] if SELF_CONFIG and/or EMBEDDED_PROVISION features supported
 */
#define CY_MAGIC_RPR_SUPPORTED          0x01
#define CY_MAGIC_SELF_CONFIG_SUPPORTED  0x02
#define CY_MAGIC_RELAY_SUPPORTED        0x04

#pragma pack(1)
#ifndef PACKED
#define PACKED
#endif

typedef PACKED struct
{
    uint16_t src;
    uint16_t dst;
    uint16_t app_key_idx;
    uint8_t  element_idx;
    int8_t   rssi;
    uint8_t  ttl;
    uint16_t company_id;
    uint16_t opcode;
    uint8_t  data[1];
} wiced_bt_mesh_hci_event_t;

/**
* @anchor WICED_BT_MESH_HCI_COMMAND_FLAGS
* @name Bits of the wiced_bt_mesh_hci_command_t::flags
* \details The following is the bits meaning of the field flags in the structure wiced_bt_mesh_hci_command_t.
* @{
*/
/**
* Bits meaning
*/
// Bits of the field wiced_bt_mesh_hci_command_t::flags
#define WICED_BT_MESH_HCI_COMMAND_FLAGS_SEND_SEGMENTED                  0x01  /**<  1 means core uses segmentation to send that message even if it fits into unsegmented message. */
#define WICED_BT_MESH_HCI_COMMAND_FLAGS_TAG_USE_DIRECTED                0x02  /**<  Tag Use Directed. */
#define WICED_BT_MESH_HCI_COMMAND_FLAGS_TAG_IMMUTABLE_CREDENTIALS       0x04  /**<  Tag Immutable Credentials */
/** @} WICED_BT_MESH_HCI_COMMAND_FLAGS */

// Represents WICED-HCI command. In the stream it is has LE bytes order
typedef PACKED struct
{
    uint16_t dst;
    uint16_t app_key_idx;
    uint8_t  element_idx;
    uint8_t  reply;
    uint8_t  flags;
    uint8_t  ttl;
    uint8_t  retrans_cnt;
    uint8_t  retrans_time;
    uint8_t  reply_timeout;
    uint8_t  data[1];
} wiced_bt_mesh_hci_command_t;

#pragma pack()

extern uint8_t mesh_mfr_name[WICED_BT_MESH_PROPERTY_LEN_DEVICE_MANUFACTURER_NAME];
extern uint8_t mesh_model_num[WICED_BT_MESH_PROPERTY_LEN_DEVICE_MODEL_NUMBER];
extern uint8_t mesh_system_id[8];

#ifdef __cplusplus
extern "C"
{
#endif

// Functions to be implemented by application

/**
 * \brief A callback function to perform application startup initialization.
 * \details The mesh_app_init function shall be implemented by the mesh application. It is executed at startup, when device is provisioned,
 * and when device is gets unprovisioned (processing of the Node Reset).
 *
 * @param[in]   is_provisioned If TRUE a device is provisioned.
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_app_init_t)(wiced_bool_t is_provisioned);

/**
 * \brief A callback function to perform application hardware initialization.
 * \details The mesh_app_hardware_init function shall be implemented by the mesh application if it
 * wants to control hardware. If application does not implement this callback the mesh application
 * library performs default action. It will register the interrupt callback for the application
 * button of the platform and perform the factory reset when the button is pushed.
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_app_hardware_init_t)(void);

/**
 * \details A callback function notifying the application when GATT connection has been established or lost.
 *
 * @param[in]   p_status The status of the GATT connection.
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_app_gatt_conn_status_t)(wiced_bt_gatt_connection_status_t *p_status);

/**
 * \brief A callback function to attract human attention by LED blinks or other.
 * \details Application shall implement the mesh_attention function to support Health Server Model and Attention
 * functionality during provisioning. A client device may request device to show itself.  The device should
 * blink or change colors, or make sound, or whatever device can do.
 *
 * @param[in]   element  Index of the element
 * @param[in]   time     0 - Off; 0x01 - 0xff - On, time to attract user attention in seconds.
 *
 * @return      None
 */
typedef void (*wiced_bt_mesh_app_attention_t)(uint8_t element, uint8_t time);

/**
 * \brief Notification period changed.
 * \details Application may be notified by the model library that it needs to periodically send
 * current Status notifications. Application for the sensor devices, i.e. declare
 * WICED_BT_MESH_MODEL_SENSOR_SERVER model on one of the elements, are required to handle this
 * callback. If the device does not have sensors, support for this callback is optional.
 * Applications that do not want to handle the callback, should make sure that they report
 * changes to the local states to the Mesh Models library. For example, an application for a light
 * where the brightness can be adjusted both over Bluetooth and locally, may report local changes
 * every time something is adjusted locally.  The Mesh Models library will remember
 * all the status changes and will automatically send notifications as appropriate.
 *
 * @param[in]   element  Index of the element
 * @param[in]   company_id      Company ID
 * @param[in]   model_id        Model ID
 * @param[in]   time     interval in milliseconds, if 0, application should not send periodic notifications.
 *
 * @return      Returns WICED_TRUE if application will take care of the periodic publications.
 */
typedef wiced_bool_t (*wiced_bt_mesh_app_notify_period_set_t)(uint8_t element, uint16_t company_id, uint16_t model_id, uint32_t time);

/**
 * \brief Processing of a AIROC HCI Command from the MCU.
 * \details Application shall implement this function to process AIROC HCI commands
 * received over the "WICED UART" or SPI transport.
 *
 * @param[in]   opcode AIROC HCI Command Opcode
 * @param[in]   p_data AIROC HCI Command parameters
 * @param[in]   length Length of the AIROC HCI Command parameters
 *
 * @return      WICED_TRUE if application processed opcode.
 */
typedef uint32_t (*wiced_bt_mesh_app_proc_rx_cmd_t)(uint16_t opcode, uint8_t *p_data, uint32_t length);

/**
 * \brief Low Power Node sleep.
 * \details Low Power Node application shall implement this function to receive notification
 * when sleep mode can be entered. Application can adjust timeout according to its preferences
 * and enter HID_OFF state. If HID_OFF is not desirable, application can just return.
 *
 * @param[in]   timeout Amount of time Mesh Core does not require processing in milliseconds
 *
 */
typedef void (*wiced_bt_mesh_app_lpn_sleep_t)(uint32_t duration);

/**
 * \brief Non-connectable adv packet application handler
 * \details A customer application can implement this function to handle application
 * specific non-connectable adv packets.
 *
 * @param[in]   rssi            Adv packet RSSI value
 * @param[in]   p_adv_data      Adv packet data
 * @param[in]   remote_bd_addr  Remote device address
 *
 * @return      WICED_TRUE if application processed adv and it does not need to be processed by the core
 */
typedef wiced_bool_t (*wiced_bt_mesh_app_non_conn_adv_handler_t)(int8_t rssi, const uint8_t *p_adv_data, const uint8_t* remote_bd_addr);

/**
 * \brief Factory reset notification.
 * \details Application shall implement this function if it has internal NVRAM storage
 * which needs to be deleted when device is instructed to perform the factory reset.
 *
 */
typedef void (*wiced_bt_mesh_app_factory_reset_t)(void);

typedef struct
{
    wiced_bt_mesh_app_init_t                p_mesh_app_init;
    wiced_bt_mesh_app_hardware_init_t       p_mesh_app_hw_init;
    wiced_bt_mesh_app_gatt_conn_status_t    p_mesh_app_gatt_conn_status;
    wiced_bt_mesh_app_attention_t           p_mesh_app_attention;
    wiced_bt_mesh_app_notify_period_set_t   p_mesh_app_notify_period_set;
    wiced_bt_mesh_app_proc_rx_cmd_t         p_mesh_app_proc_rx_cmd;
    wiced_bt_mesh_app_lpn_sleep_t           p_mesh_app_lpn_sleep;
    wiced_bt_mesh_app_factory_reset_t       p_mesh_app_factory_reset;
} wiced_bt_mesh_app_func_table_t;

extern wiced_bt_mesh_app_func_table_t wiced_bt_mesh_app_func_table;

extern void mesh_application_gen_uuid(uint8_t* uuid);
extern void mesh_application_factory_reset(void);
// If uuid_len == 16 then assigns UUID to the node. It does factory reset. And it does factory reset
extern void mesh_application_hard_reset(uint8_t* uuid, uint8_t uuid_len);

/**
 *@brief Returns the first usable by application NVRAM Identifier.
 * Application can use the NVRAM IDs starting from returned value.
**/
extern uint16_t mesh_application_get_nvram_id_app_start(void);

/**
 *@brief Register an application dependent non-connectable adv packet handler
 * Register an application dependent non-connectable adv packet handler
**/
extern void mesh_application_reg_non_conn_adv_handler(wiced_bt_mesh_app_non_conn_adv_handler_t handler);

#ifdef HCI_CONTROL
extern wiced_bt_mesh_hci_event_t *wiced_bt_mesh_create_hci_event(wiced_bt_mesh_event_t *p_event);

extern wiced_bt_mesh_hci_event_t *wiced_bt_mesh_alloc_hci_event(uint8_t element_idx);

extern wiced_bt_mesh_event_t *wiced_bt_mesh_create_event_from_wiced_hci(uint16_t opcode, uint16_t company_id, uint16_t model_id, uint8_t **p_data, uint32_t *len);

extern void wiced_bt_mesh_skip_wiced_hci_hdr(uint8_t **p_data, uint32_t *len);

extern uint8_t wiced_bt_mesh_get_element_idx_from_wiced_hci(uint8_t **p_data, uint32_t *len);

extern wiced_result_t mesh_transport_send_data(uint16_t opcode, uint8_t *p_trans_buf, uint16_t data_len);

extern void wiced_bt_mesh_send_hci_tx_complete(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_event_t *p_event);

// converts 6 bits of bin value to base 64 character (A-Z,a-z,0-9,+,/)
uint8_t wiced_bt_mesh_base64_encode_6bits(uint8_t bin);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __WICED_BT_MESH_APP_H__ */
