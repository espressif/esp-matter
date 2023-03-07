/*******************************************************************************
* @file  rsi_ble_app.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Wlan Station BLE
 * Provisioning
 * @section Description :
 * This application explains how to get the WLAN connection functionality using
 * BLE provisioning.
 * Silicon Labs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * Silicon Labs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

/**
 * Include files
 * */

// BLE include file to refer BLE APIs
#include <rsi_driver.h>
#include <rsi_ble_apis.h>
#include <rsi_wlan_apis.h>
#include <rsi_ble_config.h>
#include <rsi_wlan_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_common_apis.h>
#include <rsi_bt_common.h>
#include <string.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif
// Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000

// application defines

extern uint8_t ble_connected;
// Memory to initialize driver
uint8_t bt_global_buf[BT_GLOBAL_BUFF_LEN];

// BLE attribute service types uuid values
#define RSI_BLE_CHAR_SERV_UUID   0x2803
#define RSI_BLE_CLIENT_CHAR_UUID 0x2902

// BLE characteristic service uuid
#define RSI_BLE_NEW_SERVICE_UUID 0xAABB
#define RSI_BLE_ATTRIBUTE_1_UUID 0x1AA1
#define RSI_BLE_ATTRIBUTE_2_UUID 0x1BB1
#define RSI_BLE_ATTRIBUTE_3_UUID 0x1CC1

// max data length
#define RSI_BLE_MAX_DATA_LEN 66

// local device name
#define RSI_BLE_APP_DEVICE_NAME "BLE_CONFIGURATOR"

// attribute properties
#define RSI_BLE_ATT_PROPERTY_READ   0x02
#define RSI_BLE_ATT_PROPERTY_WRITE  0x08
#define RSI_BLE_ATT_PROPERTY_NOTIFY 0x10

// local device IO capability
#define RSI_BLE_SMP_IO_CAPABILITY 0x00
#define RSI_BLE_SMP_PASSKEY       0

// application event list
#define RSI_BLE_ENH_CONN_EVENT   0x01
#define RSI_BLE_DISCONN_EVENT    0x02
#define RSI_BLE_SMP_REQ_EVENT    0x05
#define RSI_BLE_WLAN_SCAN_RESP   0x03
#define RSI_BLE_WLAN_JOIN_STATUS 0x15

#define RSI_BLE_SMP_RESP_EVENT        0x06
#define RSI_BLE_SMP_PASSKEY_EVENT     0x07
#define RSI_BLE_SMP_FAILED_EVENT      0x08
#define RSI_BLE_ENCRYPT_STARTED_EVENT 0x09

#define RSI_BLE_WLAN_DISCONNECT_STATUS 0x14
#define RSI_CUSTOMERID_EVENT           0x0B
#define RSI_CUSTOMERID_EVENT1          0x0C
#define RSI_SSID                       0x0D
#define RSI_SECTYPE                    0x0E
#define RSI_BLE_WLAN_DISCONN_NOTIFY    0x0F
#define RSI_WLAN_ALREADY               0x10
#define RSI_WLAN_NOT_ALREADY           0x11
#define RSI_BLE_WLAN_TIMEOUT_NOTIFY    0x12
#define RSI_APP_FW_VERSION             0x13

#define RSI_FW_VERSION 0x01

/*
 *********************************************************************************************************
 *                                               DATA TYPES
 *********************************************************************************************************
 */

typedef enum rsi_app_cmd_e {
  RSI_DATA                = 0,
  RSI_WLAN_SCAN_RESP      = 1,
  RSI_WLAN_CONN_STATUS    = 2,
  RSI_WLAN_DISCONN_STATUS = 3,
  RSI_WLAN_DISCONN_NOTIFY = 4,
  RSI_WLAN_TIMEOUT_NOTIFY = 5

} rsi_app_cmd_t;

#define RSI_APP_BUF_SIZE 1600

uint8_t data[20], data1[40] = { 0 };
uint8_t scan_data[5] = { 0 };

rsi_wlan_app_cb_t rsi_wlan_app_cb;

// global parameters list
static uint32_t ble_app_event_map;
static rsi_ble_event_conn_status_t conn_event_to_app;
static rsi_ble_event_disconnect_t disconn_event_to_app;
static uint8_t rsi_ble_app_data[100];
static uint8_t rsi_ble_att1_val_hndl;
static uint16_t rsi_ble_att2_val_hndl;
static uint16_t rsi_ble_att3_val_hndl;
extern uint8_t connected, disassosiated;
extern uint8_t rsp_buf[18];
extern uint8_t retry;
uint8_t coex_ssid[50], dis_ssid[50], pwd[RSI_BLE_MAX_DATA_LEN], sec_type;
uint8_t cid[100];

#ifdef RSI_WITH_OS
extern rsi_semaphore_handle_t ble_thread_sem;
extern rsi_semaphore_handle_t wlan_thread_sem;
#endif

extern void rsi_wlan_app_set_event(uint32_t event_num);
// Maximum length of SSID
#define RSI_SSID_LEN 34
// MAC address length
#define RSI_MAC_ADDR_LEN 6
// Maximum Acccess points that can be scanned
#define RSI_AP_SCANNED_MAX 11

rsi_rsp_scan_t scanresult;

/*==============================================*/
/**
 * @fn         rsi_ble_add_char_serv_att
 * @brief      this function is used to add characteristic service attribute
 * @param[in]  serv_handler, service handler.
 * @param[in]  handle, characteristic service attribute handle.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  att_val_handle, characteristic value handle
 * @param[in]  att_val_uuid, characteristic value uuid
 * @return     none.
 * @section description
 * This function is used at application to add characteristic attribute
 */
static void rsi_ble_add_char_serv_att(void *serv_handler,
                                      uint16_t handle,
                                      uint8_t val_prop,
                                      uint16_t att_val_handle,
                                      uuid_t att_val_uuid)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  // preparing the attribute service structure
  new_att.serv_handler       = serv_handler;
  new_att.handle             = handle;
  new_att.att_uuid.size      = 2;
  new_att.att_uuid.val.val16 = RSI_BLE_CHAR_SERV_UUID;
  new_att.property           = RSI_BLE_ATT_PROPERTY_READ;

  // preparing the characteristic attribute value
  new_att.data_len = 6;
  new_att.data[0]  = val_prop;
  rsi_uint16_to_2bytes(&new_att.data[2], att_val_handle);
  rsi_uint16_to_2bytes(&new_att.data[4], att_val_uuid.val.val16);

  // add attribute to the service
  rsi_ble_add_attribute(&new_att);

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_char_val_att
 * @brief      this function is used to add characteristic value attribute.
 * @param[in]  serv_handler, new service handler.
 * @param[in]  handle, characteristic value attribute handle.
 * @param[in]  att_type_uuid, attribute uuid value.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  data, characteristic value data pointer.
 * @param[in]  data_len, characteristic value length.
 * @return     none.
 * @section description
 * This function is used at application to create new service.
 */

static void rsi_ble_add_char_val_att(void *serv_handler,
                                     uint16_t handle,
                                     uuid_t att_type_uuid,
                                     uint8_t val_prop,
                                     uint8_t *data,
                                     uint8_t data_len)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  // preparing the attributes
  new_att.serv_handler = serv_handler;
  new_att.handle       = handle;
  memcpy(&new_att.att_uuid, &att_type_uuid, sizeof(uuid_t));
  new_att.property = val_prop;

  // preparing the attribute value
  new_att.data_len = RSI_MIN(sizeof(new_att.data), data_len);
  memcpy(new_att.data, data, new_att.data_len);

  // add attribute to the service
  rsi_ble_add_attribute(&new_att);

  // check the attribute property with notification
  if (val_prop & RSI_BLE_ATT_PROPERTY_NOTIFY) {
    // if notification property supports then we need to add client characteristic service.

    // preparing the client characteristic attribute & values
    memset(&new_att, 0, sizeof(rsi_ble_req_add_att_t));
    new_att.serv_handler       = serv_handler;
    new_att.handle             = handle + 1;
    new_att.att_uuid.size      = 2;
    new_att.att_uuid.val.val16 = RSI_BLE_CLIENT_CHAR_UUID;
    new_att.property           = RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE;
    new_att.data_len           = 2;

    // add attribute to the service
    rsi_ble_add_attribute(&new_att);
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_chat_add_new_serv
 * @brief      this function is used to add new service i.e., simple chat service.
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */
static uint32_t rsi_ble_add_configurator_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[RSI_BLE_MAX_DATA_LEN]    = { 0 };

  new_uuid.size      = 2; // adding new service
  new_uuid.val.val16 = RSI_BLE_NEW_SERVICE_UUID;

  rsi_ble_add_service(new_uuid, &new_serv_resp);

  new_uuid.size      = 2; // adding characteristic service attribute to the service
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_WRITE,
                            new_serv_resp.start_handle + 2,
                            new_uuid);

  rsi_ble_att1_val_hndl = new_serv_resp.start_handle + 2; // adding characteristic value attribute to the service
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_WRITE,
                           data,
                           RSI_BLE_MAX_DATA_LEN);

  new_uuid.size      = 2; // adding characteristic service attribute to the service
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_2_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 3,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE,
                            new_serv_resp.start_handle + 4,
                            new_uuid);

  rsi_ble_att2_val_hndl = new_serv_resp.start_handle + 4; // adding characteristic value attribute to the service
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_2_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 4,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE,
                           data,
                           RSI_BLE_MAX_DATA_LEN);

  new_uuid.size      = 2; // adding characteristic service attribute to the service
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_3_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 5,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY,
                            new_serv_resp.start_handle + 6,
                            new_uuid);

  rsi_ble_att3_val_hndl = new_serv_resp.start_handle + 6; // adding characteristic value attribute to the service
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_3_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 6,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY,
                           data,
                           RSI_BLE_MAX_DATA_LEN);

  return 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  ble_app_event_map = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  ble_app_event_map |= BIT(event_num);
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&ble_thread_sem);
#endif
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  ble_app_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (ble_app_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  conn_event_to_app.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(conn_event_to_app.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  conn_event_to_app.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_BLE_ENH_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&conn_event_to_app, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_BLE_ENH_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This Callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason);
  memcpy(&disconn_event_to_app, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_request
 * @brief      its invoked when smp request event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP request events is received(we are in Master mode)
 * Note: slave requested to start SMP request, we have to send SMP request command
 */
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_dev_address)
{
  UNUSED_PARAMETER(remote_dev_address);
  rsi_ble_app_set_event(RSI_BLE_SMP_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_response
 * @brief      its invoked when smp response event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP response events is received(we are in slave mode)
 * Note: Master initiated SMP protocol, we have to send SMP response command
 */
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_dev_address)
{
  UNUSED_PARAMETER(remote_dev_address);
  rsi_ble_app_set_event(RSI_BLE_SMP_RESP_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *remote_dev_address)
{
  UNUSED_PARAMETER(remote_dev_address);
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_failed
 * @brief      its invoked when smp failed event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP failed events is received
 */
void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *remote_dev_address)
{
  UNUSED_PARAMETER(remote_dev_address);
  UNUSED_PARAMETER(status);
  rsi_ble_app_set_event(RSI_BLE_SMP_FAILED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_encrypt_started
 * @brief      its invoked when encryption started event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when encryption started events is received
 */
void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  UNUSED_PARAMETER(enc_enabled);
  UNUSED_PARAMETER(status);
  rsi_ble_app_set_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_write_event
 * @brief      this is call back function, it invokes when write/notify events received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This is a callback function
 */
static void rsi_ble_on_gatt_write_event(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write)
{
  UNUSED_PARAMETER(event_id);
  uint8_t cmdid;

  //  Requests will come from Mobile app
  if ((rsi_ble_att1_val_hndl) == *((uint16_t *)rsi_ble_write->handle)) {
    cmdid = rsi_ble_write->att_value[0];

    switch (cmdid) {
      // Scan command request
      case '3': //else if(rsi_ble_write->att_value[0] == '3')
      {
        LOG_PRINT("scan command is passed\n");
        retry = 0;
        memset(data, 0, sizeof(data));
        rsi_wlan_app_set_event(RSI_WLAN_SCAN_STATE);
      } break;

      // Sending SSID
      case '2': //else if(rsi_ble_write->att_value[0] == '2')                                           //The length of the ssid and pasword is not more than 17 character
      {
        LOG_PRINT("join command is passed\n");
        memset(coex_ssid, 0, sizeof(coex_ssid));
        strcpy((char *)coex_ssid, (const char *)&rsi_ble_write->att_value[3]);
        rsi_ble_app_set_event(RSI_SSID);
      } break;

      // Sending Security type
      case '5': //else if(rsi_ble_write->att_value[0] == '5')
      {
        LOG_PRINT("security command is passed\n");
        sec_type = rsi_ble_write->att_value[3];
        rsi_ble_app_set_event(RSI_SECTYPE);
      } break;

      // Sending Psk
      case '6': //else if(rsi_ble_write->att_value[0] == '6')
      {
        LOG_PRINT("psk command is passed\n");
        memset(data, 0, sizeof(data));
        strcpy((char *)pwd, (const char *)&rsi_ble_write->att_value[3]);
        rsi_wlan_app_set_event(RSI_WLAN_JOIN_STATE);
      } break;

      // WLAN Status Request
      case '7': //else if(rsi_ble_write->att_value[0] == '7')
      {
        LOG_PRINT("wlan status command is passed\n");
        memset(data, 0, sizeof(data));
        if (connected) {
          rsi_ble_app_set_event(RSI_WLAN_ALREADY);
        } else {
          rsi_ble_app_set_event(RSI_WLAN_NOT_ALREADY);
        }
      } break;

      // WLAN disconnect request
      case '4': //else if(rsi_ble_write->att_value[0] == '4')
      {
        LOG_PRINT("disconnect command is passed\n");
        memset(data, 0, sizeof(data));
        rsi_wlan_app_set_event(RSI_WLAN_DISCONN_NOTIFY_STATE);
      } break;

      // FW version request
      case '8': {
        LOG_PRINT("fw version command is passed\n");
        memset(data, 0, sizeof(data));
        rsi_ble_app_set_event(RSI_APP_FW_VERSION);
      } break;

      default:
        LOG_PRINT("default cmd case \n\n");
        break;
    }
  } else
    return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_init
 * @brief      initialize the BLE module.
 * @param[in]  none
 * @return     none.
 * @section description
 * This function is used to initialize the BLE module
 */
void rsi_ble_configurator_init(void)
{
  uint8_t adv[31] = { 2, 1, 6 };

  //  initializing the application events map
  rsi_ble_app_init_events();

  rsi_ble_add_configurator_serv(); // adding simple BLE chat service

  // registering the GAP callback functions
  rsi_ble_gap_register_callbacks(NULL,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  // registering the GATT callback functions
  rsi_ble_gatt_register_callbacks(NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_gatt_write_event,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL);

  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request, // registering the SMP call back functions
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL);

  // Set local name
  rsi_bt_set_local_name((uint8_t *)RSI_BLE_APP_DEVICE_NAME);

  // prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_APP_DEVICE_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_APP_DEVICE_NAME);

  // set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_APP_DEVICE_NAME) + 5);

  // set device in advertising mode.
  rsi_ble_start_advertising();
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_task
 * @brief      this function will execute when BLE events are raised.
 * @param[in]  none.
 * @return     none.
 * @section description
 */

void rsi_ble_configurator_task(void)
{
  int32_t status = 0;
  int32_t event_id;
  uint8_t data[RSI_BLE_MAX_DATA_LEN] = { 0 };
  uint8_t resp_buf[20];
  uint8_t scan_ix, length;
  uint8_t k;

#ifdef RSI_WITH_OS
  while (1) {
#endif

    // checking for events list
    event_id = rsi_ble_app_get_event();
    if (event_id == -1) {
#ifdef RSI_WITH_OS
      rsi_semaphore_wait(&ble_thread_sem, 0);
      // if events are not received loop will be continued.
      continue;
#else
    // if events are not received loop will not be continued.
    return;
#endif
    }

    switch (event_id) {
      case RSI_BLE_ENH_CONN_EVENT: {
        // event invokes when connection was completed

        // clear the served event
        rsi_ble_app_clear_event(RSI_BLE_ENH_CONN_EVENT);
        ble_connected = 1;
      } break;

      case RSI_BLE_DISCONN_EVENT: {
        // event invokes when disconnection was completed

        // clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);

        // set device in advertising mode.
adv:
// These lines are commented to avoid wlan disconnection if ble is disconnected.
#if 0
        if (connected) {
          LOG_PRINT("disconnect command is passed\n");
          memset(data, 0, RSI_BLE_MAX_DATA_LEN);
          rsi_wlan_app_set_event(RSI_WLAN_DISCONN_NOTIFY_STATE);
        }
#endif
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          goto adv;
        }
      } break;

      case RSI_BLE_GATT_WRITE_EVENT: {
        // event invokes when write/notification events received

        // clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_WRITE_EVENT);

        // set the local attribute value.
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, rsi_ble_app_data);
      } break;

      case RSI_APP_FW_VERSION: {
        rsi_ble_app_clear_event(RSI_APP_FW_VERSION);
        memset(data, 0, RSI_BLE_MAX_DATA_LEN);

        status  = rsi_wlan_get((rsi_wlan_query_cmd_t)RSI_FW_VERSION, resp_buf, 18);
        data[0] = 0x08;
        data[1] = 8;
        memcpy(&data[2], resp_buf, 8);

        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
        LOG_PRINT("\r\nFirmware version response: %s\r\n", resp_buf);
      } break;

        // Connected SSID name (response to '7' command if connection is already established)
      case RSI_WLAN_ALREADY: {
        rsi_ble_app_clear_event(RSI_WLAN_ALREADY);

        memset(data, 0, RSI_BLE_MAX_DATA_LEN);

        data[1] = connected; /*This index will indicate wlan AP connect or disconnect status to Android app*/
        data[0] = 0x07;
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
      } break;

        // NO WLAN connection (response to '7' command if connection is there already)
      case RSI_WLAN_NOT_ALREADY: {
        rsi_ble_app_clear_event(RSI_WLAN_NOT_ALREADY);
        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[0] = 0x07;
        data[1] = 0x00;
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
      } break;
      case RSI_BLE_WLAN_DISCONN_NOTIFY: {
        rsi_ble_app_clear_event(RSI_BLE_WLAN_DISCONN_NOTIFY);
        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[1] = 0x01;
        data[0] = 0x04;
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
      } break;
      case RSI_BLE_WLAN_TIMEOUT_NOTIFY: {
        rsi_ble_app_clear_event(RSI_BLE_WLAN_TIMEOUT_NOTIFY);
        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[0] = 0x02;
        data[1] = 0x00;
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
      } break;

      case RSI_BLE_WLAN_DISCONNECT_STATUS: {
        rsi_ble_app_clear_event(RSI_BLE_WLAN_DISCONNECT_STATUS);
        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[0] = 0x01;
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
      } break;
      case RSI_SSID: {
        rsi_ble_app_clear_event(RSI_SSID);
      } break;

      case RSI_SECTYPE: {
        rsi_ble_app_clear_event(RSI_SECTYPE);
        if (sec_type == '0') {
          rsi_wlan_app_set_event(RSI_WLAN_JOIN_STATE);
        }
      } break;

        // Scan results from device (response to '3' command)
      case RSI_BLE_WLAN_SCAN_RESP: //Send the SSID data to mobile ble application WYZBEE CONFIGURATOR
      {
        rsi_ble_app_clear_event(RSI_BLE_WLAN_SCAN_RESP); // clear the served event

        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[0] = 0x03;
        data[1] = scanresult.scan_count[0];
        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
        for (scan_ix = 0; scan_ix < scanresult.scan_count[0]; scan_ix++) {
          memset(data, 0, RSI_BLE_MAX_DATA_LEN);
          data[0] = scanresult.scan_info[scan_ix].security_mode;
          data[1] = ',';
          strcpy((char *)data + 2, (const char *)scanresult.scan_info[scan_ix].ssid);
          length = strlen((char *)data + 2);
          length = length + 2;

          rsi_ble_set_local_att_value(rsi_ble_att3_val_hndl, RSI_BLE_MAX_DATA_LEN, data);
          rsi_delay_ms(10);
        }
        LOG_PRINT("displayed scan list in silabs app\n\n");
      } break;

      // WLAN connection response status (response to '2' command)
      case RSI_BLE_WLAN_JOIN_STATUS: //Send the connected status to mobile ble application WYZBEE CONFIGURATOR
      {

        rsi_ble_app_clear_event(RSI_BLE_WLAN_JOIN_STATUS);

        // clear the served event

        memset(data, 0, RSI_BLE_MAX_DATA_LEN);
        data[0] = 0x02;
        data[1] = 0x01;
        data[2] = ',';

        for (k = 0; k < 6; k++) {
          data[k + 3] = rsp_buf[k];
        }
        data[k + 3] = ',';

        for (; k < 10; k++) {
          data[k + 4] = rsp_buf[k];
        }

        rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl,
                                    RSI_BLE_MAX_DATA_LEN,
                                    data); // set the local attribute value.

        LOG_PRINT("AP joined successfully\n\n");
      }

      break;
      default:
        break;
    }
#ifdef RSI_WITH_OS
  }
#else
  return;
#endif
}

/*==============================================*/
/**
 * @fn         rsi_wlan_app_send_to_ble
 * @brief      this function is used to send data to ble app.
 * @param[in]   msg_type, it indicates write/notification event id.
 * @param[in]  data, raw data pointer.
 * @param[in]  data_len, raw data length.
 * @return     none.
 * @section description
 * This is a callback function
 */
void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len)
{
  switch (msg_type) {
    case RSI_WLAN_SCAN_RESP:
      memset(&scanresult, 0, sizeof(scanresult));
      memcpy(&scanresult, (rsi_rsp_scan_t *)data, data_len);
      rsi_ble_app_set_event(RSI_BLE_WLAN_SCAN_RESP);
      break;
    case RSI_WLAN_CONN_STATUS:
      rsi_ble_app_set_event(RSI_BLE_WLAN_JOIN_STATUS);
      break;
    case RSI_WLAN_DISCONN_STATUS:
      rsi_ble_app_set_event(RSI_BLE_WLAN_DISCONNECT_STATUS);
      break;
    case RSI_WLAN_DISCONN_NOTIFY:
      rsi_ble_app_set_event(RSI_BLE_WLAN_DISCONN_NOTIFY);
      break;
    case RSI_WLAN_TIMEOUT_NOTIFY:
      rsi_ble_app_set_event(RSI_BLE_WLAN_TIMEOUT_NOTIFY);
      break;
    default:
      break;
  }
}
