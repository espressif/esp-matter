/*******************************************************************************
* @file  rsi_ble_main_task_DEMO_57.c
* @brief 
*******************************************************************************
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
/**
 * @file    rsi_ble_main_task_DEMO_57.c
 * @version 0.1
 * @date    01 Feb 2020
 *
 *
 *  @section Licenseremote_name
 *  This program should be used on your own responsibility.
 *  Silicon Labs assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains code to create multiple task instances and handling of ble events
 *
 *  @section Description  This file contains code to create multiple task instances and handling of ble events
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

#include <rsi_common_app.h>
//#if COEX_MAX_APP
#include <stdio.h>
#include <string.h>
#include <rsi_ble.h>
#include "rsi_driver.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_device_info.h"
#include "rsi_bt_common_apis.h"
#include "rsi_ble_config.h"
#include "rsi_wlan_config.h"
#include <rsi_wlan_sta_tx_config.h>
#include "rsi_common_config.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

#define RSI_BT_LE_SC_JUST_WORKS 0x01
#define RSI_BT_LE_SC_PASSKEY    0x02

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
/**
 * rsi_ble_states_bitmap - Represents the application level ble states
 * 1st bit - Scanning state
 * 2nd bit - Advertising state
 */
uint8_t rsi_ble_states_bitmap = 0x00;

rsi_task_handle_t ble_app_task_handle[TOTAL_CONNECTIONS] = { NULL };
uint8_t ble_conn_id = 0xFF, slave_connection_in_prgs = 0, slave_con_req_pending = 0;
uint32_t ble_main_app_event_task_map;
uint32_t ble_app_event_task_map[TOTAL_CONNECTIONS];
uint32_t ble_app_event_task_map1[TOTAL_CONNECTIONS];
uint8_t remote_device_role    = 0;
uint8_t master_task_instances = 0, slave_task_instances = 0;
volatile uint16_t rsi_disconnect_reason[TOTAL_CONNECTIONS] = { 0 };
volatile uint8_t ix, conn_done, conn_update_done, conn_update_master_done, num_of_conn_masters = 0,
                                                                           num_of_conn_slaves = 0;
volatile uint16_t rsi_ble_att1_val_hndl;
volatile uint16_t rsi_ble_att2_val_hndl;
volatile uint16_t rsi_ble_att3_val_hndl;
static uint8_t remote_dev_addr[RSI_REM_DEV_ADDR_LEN]      = { 0 };
static uint8_t remote_dev_addr_conn[RSI_REM_DEV_ADDR_LEN] = { 0 };
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
static uint32_t ble_app_event_mask;
static int8_t master_conn_id[2] = { -1, -1 };
static int8_t slave_conn_id     = -1;
#if (CONNECT_OPTION == CONN_BY_NAME)
static uint8_t remote_name[RSI_REM_DEV_NAME_LEN];
#endif
rsi_ble_t att_list;
rsi_ble_req_adv_t change_adv_param;
rsi_ble_req_scan_t change_scan_param;
rsi_semaphore_handle_t ble_conn_sem[TOTAL_CONNECTIONS];

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_ble_conn_info_t rsi_ble_conn_info[];
extern rsi_parsed_conf_t rsi_parsed_conf;
extern rsi_semaphore_handle_t ble_main_task_sem, ble_slave_conn_sem, bt_inquiry_sem, ble_scan_sem;
#if WLAN_SYNC_REQ
extern rsi_semaphore_handle_t sync_coex_ble_sem;
#endif
#if WLAN_TRANSIENT_CASE
extern rsi_semaphore_handle_t wlan_sync_coex_ble_sem;
extern uint32_t disable_factor_count;
uint8_t ble_scanning_is_there, ble_adv_is_there;
#endif
extern bool rsi_bt_running, rsi_ant_running, rsi_wlan_running, powersave_cmd_given;
extern rsi_mutex_handle_t power_cmd_mutex;
/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_task_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_app_set_task_event(uint8_t conn_id, uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_task_map[conn_id] |= BIT(event_num);
  } else {
    ble_app_event_task_map1[conn_id] |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&ble_conn_sem[conn_id]);
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
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
  ble_app_event_mask = 0xFFFFFFFF;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_app_set_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&ble_main_task_sem);

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

  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  } else {
    ble_app_event_map1 &= ~BIT((event_num - 32));
  }

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

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_check_dev_list
 * @brief      returns the status of slave devices state
 * @param[in]  remote_dev_name - remote device name.
 * 				adv_dev_addr - remote device address
 * @return     uint8_t
 *             NO_SLAVE_FOUND - slave address/name doesnt match
 *             SLAVE_FOUND   - slave address/name matches
 *             SLAVE_NOT_CONNECTED - slave address/name matches and slave not yet connected
 *             SLAVE_CONNECTED - slave address/name matches and slave already connected
 *
 * @section description
 * This function is called when advertise report received and when received checks the address/name with configured names
 */
uint8_t rsi_check_dev_list(uint8_t *remote_dev_name, uint8_t *adv_dev_addr)
{
  uint8_t i                  = 0;
  uint8_t slave_device_found = NO_SLAVE_FOUND;
  //These statements are added only to resolve compilation warning, value is unchanged
#if ((CONNECT_OPTION == CONN_BY_NAME) && RSI_DEBUG_EN)
  uint8_t *rem_dev_name        = remote_dev_name;
  uint8_t *advertised_dev_addr = adv_dev_addr;
#elif (CONNECT_OPTION == CONN_BY_NAME)
  UNUSED_PARAMETER(adv_dev_addr);
  uint8_t *rem_dev_name = remote_dev_name;
#else
  uint8_t *advertised_dev_addr = adv_dev_addr;
  UNUSED_PARAMETER(remote_dev_name);
#endif
#if (CONNECT_OPTION == CONN_BY_NAME)
  if ((strcmp((const char *)rem_dev_name, "")) == 0) {
    return slave_device_found;
  }

  if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME1)) == 0) {
    slave_device_found = SLAVE_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME2)) == 0) {
    slave_device_found = SLAVE_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME3)) == 0) {
    slave_device_found = SLAVE_FOUND;
  } else
    return slave_device_found;

  //! check if remote device already connected or advertise report received- TODO .  Can check efficiently?
  if (slave_device_found == SLAVE_FOUND) {
    for (i = 0; i < (RSI_BLE_MAX_NBR_SLAVES); i++) {
      if (!(strcmp((const char *)rsi_ble_conn_info[i].rsi_remote_name, (const char *)rem_dev_name))) {
        slave_device_found = SLAVE_CONNECTED;
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\n Device %s already connected!!!\r\n", advertised_dev_addr);
#endif
        break;
      }
    }
  }
#else
  if (!strcmp(RSI_BLE_DEV_1_ADDR, (char *)advertised_dev_addr)) {
    slave_device_found = SLAVE_FOUND;
  } else if (!strcmp(RSI_BLE_DEV_2_ADDR, (char *)advertised_dev_addr)) {
    slave_device_found = SLAVE_FOUND;
  } else if (!strcmp(RSI_BLE_DEV_3_ADDR, (char *)advertised_dev_addr)) {
    slave_device_found = SLAVE_FOUND;
  } else
    return slave_device_found;

  //! check if remote device already connected
  if (slave_device_found == SLAVE_FOUND) {
    for (i = 0; i < (RSI_BLE_MAX_NBR_SLAVES); i++) {
      if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, advertised_dev_addr, RSI_REM_DEV_ADDR_LEN)) {
        slave_device_found = SLAVE_CONNECTED;
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\n Device %s already connected!!!\r\n", advertised_dev_addr);
#endif
        break;
      }
    }
  }

#endif
  if (i == RSI_BLE_MAX_NBR_SLAVES) {
    slave_device_found = SLAVE_NOT_CONNECTED;
  }

  return slave_device_found;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_char_serv_att
 * @brief      this function is used to add characteristic service attribute..
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

  //! preparing the attribute service structure
  new_att.serv_handler       = serv_handler;
  new_att.handle             = handle;
  new_att.att_uuid.size      = 2;
  new_att.att_uuid.val.val16 = RSI_BLE_CHAR_SERV_UUID;
  new_att.property           = RSI_BLE_ATT_PROPERTY_READ;

  //! preparing the characteristic attribute value
  new_att.data_len = att_val_uuid.size + 4;
  new_att.data[0]  = val_prop;
  rsi_uint16_to_2bytes(&new_att.data[2], att_val_handle);
  if (new_att.data_len == 6) {
    rsi_uint16_to_2bytes(&new_att.data[4], att_val_uuid.val.val16);
  } else if (new_att.data_len == 8) {
    rsi_uint32_to_4bytes(&new_att.data[4], att_val_uuid.val.val32);
  } else if (new_att.data_len == 20) {
    memcpy(&new_att.data[4], &att_val_uuid.val.val128, att_val_uuid.size);
  }
  //! Add attribute to the service
  rsi_ble_add_attribute(&new_att);

  return;
}

/*==============================================*/
/**
 * @fn         rsi_gatt_add_attribute_to_list
 * @brief      This function is used to store characteristic service attribute.
 * @param[in]  p_val, pointer to homekit structure
 * @param[in]  handle, characteristic service attribute handle.
 * @param[in]  data_len, characteristic value length
 * @param[in]  data, characteristic value pointer
 * @param[in]  uuid, characteristic value uuid
 * @return     none.
 * @section description
 * This function is used to store all attribute records
 */
void rsi_gatt_add_attribute_to_list(rsi_ble_t *p_val,
                                    uint16_t handle,
                                    uint16_t data_len,
                                    uint8_t *data,
                                    uuid_t uuid,
                                    uint8_t char_prop)
{
  if ((p_val->DATA_ix + data_len) >= BLE_ATT_REC_SIZE) { //! Check for max data length for the characteristic value
    LOG_PRINT("\r\n no data memory for att rec values \r\n");
    return;
  }

  p_val->att_rec_list[p_val->att_rec_list_count].char_uuid     = uuid;
  p_val->att_rec_list[p_val->att_rec_list_count].handle        = handle;
  p_val->att_rec_list[p_val->att_rec_list_count].value_len     = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].max_value_len = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].char_val_prop = char_prop;
  memcpy(p_val->DATA + p_val->DATA_ix, data, data_len);
  p_val->att_rec_list[p_val->att_rec_list_count].value = p_val->DATA + p_val->DATA_ix;
  p_val->att_rec_list_count++;
  p_val->DATA_ix += p_val->att_rec_list[p_val->att_rec_list_count].max_value_len;

  return;
}

/*==============================================*/
/**
 * @fn         rsi_gatt_get_attribute_from_list
 * @brief      This function is used to retrieve attribute from list based on handle.
 * @param[in]  p_val, pointer to characteristic structure
 * @param[in]  handle, characteristic service attribute handle.
 * @return     pointer to the attribute
 * @section description
 * This function is used to store all attribute records
 */
rsi_ble_att_list_t *rsi_gatt_get_attribute_from_list(rsi_ble_t *p_val, uint16_t handle)
{
  uint16_t i;
  for (i = 0; i < p_val->att_rec_list_count; i++) {
    if (p_val->att_rec_list[i].handle == handle) {
      //*val_prop = p_val.att_rec_list[i].char_val_prop;
      //*length = p_val.att_rec_list[i].value_len;
      //*max_data_len = p_val.att_rec_list[i].max_value_len;
      return &(p_val->att_rec_list[i]);
    }
  }
  return 0;
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
                                     uint8_t data_len,
                                     uint8_t auth_read)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  //! preparing the attributes
  new_att.serv_handler  = serv_handler;
  new_att.handle        = handle;
  new_att.config_bitmap = auth_read;
  memcpy(&new_att.att_uuid, &att_type_uuid, sizeof(uuid_t));
  new_att.property = val_prop;

  if (data != NULL)
    memcpy(new_att.data, data, RSI_MIN(sizeof(new_att.data), data_len));

  //! preparing the attribute value
  new_att.data_len = data_len;

  //! add attribute to the service
  rsi_ble_add_attribute(&new_att);

  if ((auth_read == ATT_REC_MAINTAIN_IN_HOST) || (data_len > 20)) {
    if (data != NULL) {
      rsi_gatt_add_attribute_to_list(&att_list, handle, data_len, data, att_type_uuid, val_prop);
    }
  }

  //! check the attribute property with notification/Indication
  if ((val_prop & RSI_BLE_ATT_PROPERTY_NOTIFY) || (val_prop & RSI_BLE_ATT_PROPERTY_INDICATE)) {
    //! if notification/indication property supports then we need to add client characteristic service.

    //! preparing the client characteristic attribute & values
    memset(&new_att, 0, sizeof(rsi_ble_req_add_att_t));
    new_att.serv_handler       = serv_handler;
    new_att.handle             = handle + 1;
    new_att.att_uuid.size      = 2;
    new_att.att_uuid.val.val16 = RSI_BLE_CLIENT_CHAR_UUID;
    new_att.property           = RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE;
    new_att.data_len           = 2;

    //! add attribute to the service
    rsi_ble_add_attribute(&new_att);
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_simple_chat_serv
 * @brief      this function is used to add new servcie i.e.., simple chat service.
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */

static uint32_t rsi_ble_add_simple_chat_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[230]                     = { 1, 0 };

  //! adding new service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_NEW_SERVICE_UUID;
  rsi_ble_add_service(new_uuid, &new_serv_resp);

  //! adding characteristic service attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY | RSI_BLE_ATT_PROPERTY_WRITE,
                            new_serv_resp.start_handle + 2,
                            new_uuid);

  //! adding characteristic value attribute to the service
  rsi_ble_att1_val_hndl = new_serv_resp.start_handle + 2;
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY | RSI_BLE_ATT_PROPERTY_WRITE,
                           data,
                           sizeof(data),
                           1);

  return 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_simple_chat_serv3
 * @brief      this function is used to add new servcie i.e.., simple chat service.
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */
static uint32_t rsi_ble_add_simple_chat_serv3(void)
{
  //! adding the custom service
  // 0x6A4E3300-667B-11E3-949A-0800200C9A66
  uint8_t data1[231]                 = { 1, 0 };
  static const uuid_t custom_service = { .size             = 16,
                                         .reserved         = { 0x00, 0x00, 0x00 },
                                         .val.val128.data1 = 0x6A4E3300,
                                         .val.val128.data2 = 0x667B,
                                         .val.val128.data3 = 0x11E3,
                                         .val.val128.data4 = { 0x9A, 0x94, 0x00, 0x08, 0x66, 0x9A, 0x0C, 0x20 } };

  // 0x6A4E3304-667B-11E3-949A-0800200C9A66
  static const uuid_t custom_characteristic = {
    .size             = 16,
    .reserved         = { 0x00, 0x00, 0x00 },
    .val.val128.data1 = 0x6A4E3304,
    .val.val128.data2 = 0x667B,
    .val.val128.data3 = 0x11E3,
    .val.val128.data4 = { 0x9A, 0x94, 0x00, 0x08, 0x66, 0x9A, 0x0C, 0x20 }
  };

  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  rsi_ble_add_service(custom_service, &new_serv_resp);

  //! adding custom characteristic declaration to the custom service
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE, //Set read, write, write without response
                            new_serv_resp.start_handle + 2,
                            custom_characteristic);

  //! adding characteristic value attribute to the service
  rsi_ble_att2_val_hndl = new_serv_resp.start_handle + 2;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           custom_characteristic,
                           RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE, //Set read, write, write without response
                           data1,
                           sizeof(data1),
                           1);
  return 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_custom_service_serv
 * @brief      this function is used to add new servcie i.e.., custom service
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */

static uint32_t rsi_ble_add_custom_service_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[1]                       = { 90 };
  rsi_ble_pesentation_format_t presentation_format;
  uint8_t format_data[7];

  //!adding descriptor fileds
  format_data[0] = presentation_format.format = RSI_BLE_UINT8_FORMAT;
  format_data[1] = presentation_format.exponent = RSI_BLE_EXPONENT;
  presentation_format.unit                      = RSI_BLE_PERCENTAGE_UNITS_UUID;
  memcpy(&format_data[2], &presentation_format.unit, sizeof(presentation_format.unit));
  format_data[4] = presentation_format.name_space = RSI_BLE_NAME_SPACE;
  presentation_format.description                 = RSI_BLE_DESCRIPTION;
  memcpy(&format_data[5], &presentation_format.description, sizeof(presentation_format.description));

  //! adding new service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CUSTOM_SERVICE_UUID;
  rsi_ble_add_service(new_uuid, &new_serv_resp);

  //! adding characteristic service attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CUSTOM_LEVEL_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_INDICATE,
                            new_serv_resp.start_handle + 2,
                            new_uuid);
  //! adding characteristic value attribute to the service
  rsi_ble_att3_val_hndl = new_serv_resp.start_handle + 2;
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_CUSTOM_LEVEL_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_INDICATE,
                           data,
                           sizeof(data),
                           1);

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CHAR_PRESENTATION_FORMATE_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 4,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ,
                           format_data,
                           sizeof(format_data),
                           1);

  return 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_remote_features_event
 * @brief      invoked when LE remote features event is received.
 * @param[in] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_remote_features_event(rsi_ble_event_remote_features_t *rsi_ble_event_remote_features)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_remote_features->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id              = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].remote_dev_feature,
         rsi_ble_event_remote_features,
         sizeof(rsi_ble_event_remote_features_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_RECEIVE_REMOTE_FEATURES);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{
  uint8_t device_already_connected = 0;
  if (adv_report->report_type == 0x02)
    return; // return for NON CONN ADV PACKETS

  //! Need to ignore advertising reports until one slave connection is completed successfully
  if ((slave_connection_in_prgs) || (slave_con_req_pending)) {
    return;
  }

  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);

#if (CONNECT_OPTION == CONN_BY_NAME)
  memset(remote_name, 0, sizeof(remote_name));

  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);
#if RSI_DEBUG_EN
  //LOG_PRINT_D("\r\n advertised details remote_name = %s, dev_addr = %s \r\n",remote_name,(int8_t *) remote_dev_addr);
#endif

  device_already_connected = rsi_check_dev_list(remote_name, remote_dev_addr);
#else
  device_already_connected = rsi_check_dev_list(NULL, remote_dev_addr);
#endif

  if (device_already_connected == SLAVE_NOT_CONNECTED) {
    //! convert to ascii
    rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, adv_report->dev_addr);

    //! get conn_id
#if (CONNECT_OPTION == CONN_BY_NAME)
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, remote_name, rsi_strlen(remote_name));
#else
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#endif

    memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_app_adv_reports_to_app, adv_report, sizeof(rsi_ble_event_adv_report_t));

    //! set common event
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
    slave_con_req_pending = 1;
    slave_conn_id         = ble_conn_id;
  }
  return;
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
  uint8_t i;
  uint8_t RSI_NULL_BLE_ADDR[18] = { 0 };
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\nIn on conn cb\r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, resp_conn->dev_addr);

  //! Check whether the received connected event came from remote slave or master
  remote_device_role = rsi_get_remote_device_role(remote_dev_addr_conn);

  if (resp_conn->status != 0) {
    //LOG_PRINT("\r\n On connect event status report : %d", resp_conn->status);
    if (remote_device_role == SLAVE_ROLE) {
      slave_connection_in_prgs = 0;
    }
    //! Restarting scan
    rsi_ble_app_set_event(RSI_BLE_SCAN_RESTART_EVENT);
    return;
  }

  if (remote_device_role == SLAVE_ROLE) //check for the connection is from slave or master
  {
    //! get conn_id
#if (CONNECT_OPTION != CONN_BY_NAME)
    //! get conn_id
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

    //! copy to conn specific buffer
    memcpy(&rsi_ble_conn_info[ble_conn_id].conn_event_to_app, resp_conn, sizeof(rsi_ble_event_conn_status_t));

    conn_done                = ble_conn_id + 1;
    slave_connection_in_prgs = 0;

    //! set conn specific event
    rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_CONN_EVENT);

    //! unblock connection semaphore
    rsi_semaphore_post(&ble_slave_conn_sem);
  } else if (remote_device_role == MASTER_ROLE) {
    //! Check if already connected
    for (i = 0; i < RSI_BLE_MAX_NBR_MASTERS; i++) {
      if (!memcmp(rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_dev_addr,
                  RSI_NULL_BLE_ADDR,
                  RSI_REM_DEV_ADDR_LEN)) {

        memcpy(rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_dev_addr,
               remote_dev_addr_conn,
               sizeof(remote_dev_addr_conn));
        memcpy(&rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].conn_event_to_app,
               resp_conn,
               sizeof(rsi_ble_event_conn_status_t));
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].is_enhanced_conn   = false;
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].conn_id            = RSI_BLE_MAX_NBR_SLAVES + i;
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_device_role = MASTER_ROLE;
        master_conn_id[i]                                                = RSI_BLE_MAX_NBR_SLAVES + i;
        rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
        break;
      }
    }
  } else {
    LOG_PRINT("\r\n CHECK WHY THIS STATE OCCURS IN CONNECTION");
    while (1)
      ;
  }
}
/*==============================================*/
/**
 * @fn         ble_on_conn_update_complete_event
 * @brief      invoked when connection update complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void ble_on_conn_update_complete_event(rsi_ble_event_conn_update_t *resp_conn_update, uint16_t resp_status)
{
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\nIn conn update cb\r\n");
#endif
  if (resp_status != 0) {
    LOG_PRINT("\r\n RSI_BLE_CONN_UPDATE_EVENT FAILED : %d\r\n", resp_status);
    return;
  } else {
    //! convert to ascii
    rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, resp_conn_update->dev_addr);

    //! get conn_id
#if (CONNECT_OPTION != CONN_BY_NAME)
    //! get conn_id
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

    //! copy to conn specific buffer
    memcpy(&rsi_ble_conn_info[ble_conn_id].conn_update_resp, resp_conn_update, sizeof(rsi_ble_event_conn_update_t));

    //! set conn specific event
    rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_CONN_UPDATE_COMPLETE_EVENT);
  }
}
/*==============================================*/
/**
 * @fn         rsi_ble_on_remote_conn_params_request_event
 * @brief      invoked when remote conn params request is received
 * @param[out] remote_conn_param, emote conn params request information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_remote_conn_params_request_event(rsi_ble_event_remote_conn_param_req_t *remote_conn_param,
                                                        uint16_t status)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_conn_param->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_app_remote_device_conn_params,
         remote_conn_param,
         sizeof(rsi_ble_event_remote_conn_param_req_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, resp_disconnect->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_disconn_resp, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));

  rsi_disconnect_reason[ble_conn_id] = reason;

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_DISCONN_EVENT);
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
  uint8_t i;
  uint8_t RSI_NULL_BLE_ADDR[18] = { 0 };
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\nIn on_enhance_conn cb\r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, resp_enh_conn->dev_addr);

  //! Check whether the received connected event came from remote slave or master
  remote_device_role = rsi_get_remote_device_role(remote_dev_addr_conn);

  if (resp_enh_conn->status != 0 && resp_enh_conn->status != 63) {
    LOG_PRINT("\r\n On enhanced connect event status report : %d", resp_enh_conn->status);
    if (remote_device_role == SLAVE_ROLE) {
      slave_connection_in_prgs = 0;
    }
    rsi_ble_app_set_event(RSI_BLE_SCAN_RESTART_EVENT);
    return;
  }

  if (remote_device_role == SLAVE_ROLE) //check for the connection is from slave or master
  {
#if (CONNECT_OPTION != CONN_BY_NAME)
    //! get conn_id
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
    ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

    //! copy to conn specific buffer
    memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_enhc_conn_status,
           resp_enh_conn,
           sizeof(rsi_ble_event_enhance_conn_status_t));

    conn_done                = ble_conn_id + 1;
    slave_connection_in_prgs = 0;

    //! set conn specific event
    rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_ENHC_CONN_EVENT);

    //! unblock connection semaphore
    rsi_semaphore_post(&ble_slave_conn_sem);
  } else if (remote_device_role == MASTER_ROLE) {
    for (i = 0; i < (RSI_BLE_MAX_NBR_MASTERS); i++) {
      if (!memcmp(rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_dev_addr,
                  RSI_NULL_BLE_ADDR,
                  RSI_REM_DEV_ADDR_LEN)) {
        LOG_PRINT_D("\n\n\n  Master Index = %d \n\n\n", i);

        memcpy(rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_dev_addr,
               remote_dev_addr_conn,
               sizeof(remote_dev_addr_conn));
        memcpy(&rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].rsi_enhc_conn_status,
               resp_enh_conn,
               sizeof(rsi_ble_event_enhance_conn_status_t));
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].is_enhanced_conn   = true;
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].conn_id            = RSI_BLE_MAX_NBR_SLAVES + i;
        rsi_ble_conn_info[RSI_BLE_MAX_NBR_SLAVES + i].remote_device_role = MASTER_ROLE;
        master_conn_id[i]                                                = RSI_BLE_MAX_NBR_SLAVES + i;
        rsi_ble_app_set_event(RSI_BLE_ENHC_CONN_EVENT);
        //! set below flag to use whether the connection requested from remote device is enhanced connection or normal connection
        break;
      }
    }
  } else {
    LOG_PRINT("\r\n CHECK WHY THIS STATE OCCURS IN CONNECTION");
    while (1)
      ;
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_write_resp
 * @brief      its invoked when write response received.
 * @param[in]  event_id, it indicates read event id.
 * @param[in]  rsi_ble_read_req, write respsonse event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when read events are received
 */
static void rsi_ble_on_event_write_resp(uint16_t event_status, rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rs)
{
  //! convert to ascii
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n in write resp event\r\n");
#endif
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_set_att_rs->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_write_resp_event,
         rsi_ble_event_set_att_rs,
         sizeof(rsi_ble_set_att_resp_t));

  if (event_status != RSI_SUCCESS) {
    LOG_PRINT("\r\n write event response failed , error : 0x%x\r\n", event_status);
  } else {
    //! set conn specific event
    rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_WRITE_EVENT_RESP);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_event_indication_confirmation
 * @brief      this function will invoke when received indication confirmation event
 * @param[out] resp_id, response id
 * @param[out] status, status of the response
 * @return     none
 * @section description
 */
static void rsi_ble_on_event_indication_confirmation(uint16_t resp_status,
                                                     rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rsp)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(resp_status);
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_set_att_rsp->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_INDICATION_CONFIRMATION);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_write_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_gatt_write_event(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(event_id);
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n in write event \r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_write->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].app_ble_write_event, rsi_ble_write, sizeof(rsi_ble_event_write_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_WRITE_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_prepare_write_event
 * @brief      its invoked when prepared write events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_gatt_prepare_write_event(uint16_t event_id,
                                                rsi_ble_event_prepare_write_t *rsi_app_ble_prepared_write_event)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(event_id);
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n in rsi_ble_on_gatt_prepare_write_event \r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_app_ble_prepared_write_event->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].app_ble_prepared_write_event,
         rsi_app_ble_prepared_write_event,
         sizeof(rsi_ble_event_prepare_write_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_PREPARE_WRITE_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_execute_write_event
 * @brief      its invoked when prepared write events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_execute_write_event(uint16_t event_id, rsi_ble_execute_write_t *rsi_app_ble_execute_write_event)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(event_id);
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n in rsi_ble_on_execute_write_event \r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_app_ble_execute_write_event->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].app_ble_execute_write_event,
         rsi_app_ble_execute_write_event,
         sizeof(rsi_ble_execute_write_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_EXECUTE_WRITE_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_read_req_event
 * @brief      its invoked when read events are received.
 * @param[in]  event_id, it indicates read event id.
 * @param[in]  rsi_ble_read_req, read respsonse event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when read events are received
 */
static void rsi_ble_on_read_req_event(uint16_t event_id, rsi_ble_read_req_t *rsi_ble_read_req)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(event_id);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_read_req->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].app_ble_read_event, rsi_ble_read_req, sizeof(rsi_ble_read_req_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_READ_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_mtu_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_mtu_event(rsi_ble_event_mtu_t *rsi_ble_mtu)
{
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_mtu->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].app_ble_mtu_event, rsi_ble_mtu, sizeof(rsi_ble_event_mtu_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_MTU_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_profiles_list_event
 * @brief      invoked when response is received for get list of services.
 * @param[out] p_ble_resp_profiles, profiles list details
 * @return     none
 * @section description
 */
static void rsi_ble_profiles_list_event(uint16_t resp_status, rsi_ble_event_profiles_list_t *rsi_ble_event_profiles)
{

  if (resp_status == 0x4A0A) {
    return;
  }
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_profiles->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].get_allprofiles,
         rsi_ble_event_profiles,
         sizeof(rsi_ble_event_profiles_list_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_PROFILES);
}

/*==============================================*/
/**
 * @fn         rsi_ble_profile_event
 * @brief      invoked when the specific service details are received for
 *             supported specific services.
 * @param[out] rsi_ble_event_profile, specific profile details
 * @return     none
 * @section description
 * This is a callback function
 */
static void rsi_ble_profile_event(uint16_t resp_status, rsi_ble_event_profile_by_uuid_t *rsi_ble_event_profile)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(resp_status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_profile->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].get_profile, rsi_ble_event_profile, sizeof(rsi_ble_event_profile_by_uuid_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_PROFILE);
}

/*==============================================*/
/**
 * @fn         rsi_ble_char_services_event
 * @brief      invoked when response is received for characteristic services details
 * @param[out] rsi_ble_event_char_services, list of characteristic services.
 * @return     none
 * @section description
 */
static void rsi_ble_char_services_event(uint16_t resp_status,
                                        rsi_ble_event_read_by_type1_t *rsi_ble_event_char_services)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(resp_status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_char_services->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].get_char_services,
         rsi_ble_event_char_services,
         sizeof(rsi_ble_event_read_by_type1_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_read_resp_event
 * @brief      invoked when response is received for the characteristic descriptors details
 * @param[out] rsi_ble_event_att_value_t, list of descriptors response
 * @return     none
 * @section description
 */
static void rsi_ble_on_read_resp_event(uint16_t event_status, rsi_ble_event_att_value_t *rsi_ble_event_att_val)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(event_status);
#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n in Gatt descriptor response event \r\n");
#endif

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_att_val->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_char_descriptors,
         rsi_ble_event_att_val,
         sizeof(rsi_ble_event_att_value_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_DESC_SERVICES);
}

/*==============================================*/
/**
 * @fn         rsi_ble_phy_update_complete_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This Callback function indicates disconnected device information and status
 */
void rsi_ble_phy_update_complete_event(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete)
{
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_phy_update_complete->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
}

#if SMP_ENABLE
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
static void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_smp)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_smp->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_req, remote_smp, sizeof(rsi_bt_event_smp_req_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SMP_REQ_EVENT);
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
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_smp)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_smp->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_resp, remote_smp, sizeof(rsi_bt_event_smp_resp_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SMP_RESP_EVENT);
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
static void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *smp_pass_key)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, smp_pass_key->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_passkey, smp_pass_key, sizeof(rsi_bt_event_smp_passkey_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SMP_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey_display
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
static void rsi_ble_on_smp_passkey_display(rsi_bt_event_smp_passkey_display_t *smp_passkey_display)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, smp_passkey_display->dev_addr);

  //! get conn_id
#if (CONNECT_OPTION != CONN_BY_NAME)
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_passkey_display,
         smp_passkey_display,
         sizeof(rsi_bt_event_smp_passkey_display_t));
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_sc_passkey
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
static void rsi_ble_on_sc_passkey(rsi_bt_event_sc_passkey_t *sc_passkey)
{
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, sc_passkey->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_event_sc_passkey, sc_passkey, sizeof(rsi_bt_event_sc_passkey_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SC_PASSKEY_EVENT);
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
static void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *remote_dev_address)
{

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_dev_address->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_failed, remote_dev_address, sizeof(rsi_bt_event_smp_failed_t));

  LOG_PRINT("\r\n SMP failed for remote address : %s with status : %x", remote_dev_addr_conn, status);

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SMP_FAILED_EVENT);
}

#if SIG_VUL
/*==============================================*/
/**
 * @fn         rsi_ble_on_sc_method
 * @brief      its invoked when security method event is received.
 * @param[in]  scmethod, 1 means Justworks and 2 means Passkey.
 * @return     none.
 * @section description
 * This callback function is invoked when SC Method events is received
 */
void rsi_ble_on_sc_method(rsi_bt_event_sc_method_t *scmethod)
{
  if (scmethod->sc_method == RSI_BT_LE_SC_JUST_WORKS) {
    LOG_PRINT("\nOur Security method is Justworks, hence compare the 6 digit numeric value on both devices\r\n");
  } else if (scmethod->sc_method == RSI_BT_LE_SC_PASSKEY) {
    LOG_PRINT(
      "\nOur Security method is Passkey_Entry, hence same 6 digit Numeric value to be entered on both devices using "
      "keyboard, do not enter the numeric value displayed on one device into another device using keyboard \r\n");
  } else {
  }
}
#endif

/*==============================================*/
/**
 * @fn         rsi_ble_on_encrypt_started
 * @brief      its invoked when encryption started event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when encryption started events is received
 */
static void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, enc_enabled->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled,
         enc_enabled,
         sizeof(rsi_bt_event_encryption_enabled_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_ltk_req_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, le_ltk_req->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_le_ltk_resp, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_LTK_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_security_keys_event
 * @brief      invoked when security keys event is received
 * @param[in]  rsi_bt_event_le_security_keys_t, security keys information
 * @return     none.
 * @section description
 * This callback function indicates security keys information
 */
static void rsi_ble_on_le_security_keys_event(rsi_bt_event_le_security_keys_t *le_sec_keys)
{
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, le_sec_keys->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_le_security_keys, le_sec_keys, sizeof(rsi_bt_event_le_security_keys_t));

  //! set conn specific event
  //! signal conn specific task
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_SECURITY_KEYS_EVENT);
}
#endif

/*==============================================*/
/**
 * @fn         rsi_ble_gatt_error_event
 * @brief      this function will invoke when set the attribute value complete
 * @param[out] rsi_ble_gatt_error, event structure
 * @param[out] status, status of the response
 * @return     none
 * @section description
 */
static void rsi_ble_gatt_error_event(uint16_t resp_status, rsi_ble_event_error_resp_t *rsi_ble_gatt_error)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(resp_status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_gatt_error->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_gatt_err_resp, rsi_ble_gatt_error, sizeof(rsi_ble_event_error_resp_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_GATT_ERROR);
}

static void rsi_ble_more_data_req_event(rsi_ble_event_le_dev_buf_ind_t *rsi_ble_more_data_evt)
{

#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n Received more data event in main task\r\n");
#endif
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_more_data_evt->remote_dev_bd_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  //memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_more_data_evt, rsi_ble_more_data_evt, sizeof(rsi_ble_event_le_dev_buf_ind_t));

  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_MORE_DATA_REQ_EVT);

  return;
}
static void rsi_ble_on_event_mtu_exchange_info(
  rsi_ble_event_mtu_exchange_information_t *rsi_ble_event_mtu_exchange_info)
{
  LOG_PRINT("\r\n Received MTU EXCHANGE Information Event in main task\r\n");
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_event_mtu_exchange_info->dev_addr);
#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].mtu_exchange_info,
         rsi_ble_event_mtu_exchange_info,
         sizeof(rsi_ble_event_mtu_exchange_information_t));
  LOG_PRINT("Remote Device Address : %s\n", remote_dev_addr_conn);
  LOG_PRINT("RemoteMTU : %d \r\n", rsi_ble_conn_info[ble_conn_id].mtu_exchange_info.remote_mtu_size);
  LOG_PRINT("LocalMTU : %d\r\n", rsi_ble_conn_info[ble_conn_id].mtu_exchange_info.local_mtu_size);
  LOG_PRINT("Initated Role : 0x%x \r\n", rsi_ble_conn_info[ble_conn_id].mtu_exchange_info.initiated_role);
  //! set conn specific event
  rsi_ble_app_set_task_event(ble_conn_id, RSI_BLE_MTU_EXCHANGE_INFORMATION);
}
/*==============================================*/
/**
 * @fn         rsi_ble_dual_role
 * @brief      this is the application of ble GATT client api's test cases.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used at application.
 */
static int32_t rsi_ble_dual_role(void)
{
  int32_t status  = 0;
  uint8_t adv[31] = { 2, 1, 6 };

  //! This should be vary from one device to other, Present RSI dont have a support of FIXED IRK on every Reset
  uint8_t local_irk[16] = { 0x4d, 0xd7, 0xbd, 0x3e, 0xec, 0x10, 0xda, 0xab,
                            0x1f, 0x85, 0x56, 0xee, 0xa5, 0xc8, 0xe6, 0x93 };

  rsi_ble_add_simple_chat_serv();
  rsi_ble_add_simple_chat_serv3();
  //! adding BLE Custom  Service service
  rsi_ble_add_custom_service_serv();

  //! registering the GAP call back functions
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL, /*rsi_ble_phy_update_complete_event*/
                                 NULL, /*rsi_ble_data_length_change_event*/
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 ble_on_conn_update_complete_event,
                                 rsi_ble_on_remote_conn_params_request_event);
  //! registering the GATT call back functions
  rsi_ble_gatt_register_callbacks(NULL,
                                  NULL, /*rsi_ble_profile*/
                                  NULL, /*rsi_ble_char_services*/
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_gatt_write_event,
                                  rsi_ble_on_gatt_prepare_write_event,
                                  rsi_ble_on_execute_write_event,
                                  rsi_ble_on_read_req_event,
                                  rsi_ble_on_mtu_event,
                                  rsi_ble_gatt_error_event,
                                  NULL,
                                  rsi_ble_profiles_list_event,
                                  rsi_ble_profile_event,
                                  rsi_ble_char_services_event,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_read_resp_event,
                                  rsi_ble_on_event_write_resp,
                                  rsi_ble_on_event_indication_confirmation,
                                  NULL);

  rsi_ble_gatt_extended_register_callbacks(rsi_ble_on_event_mtu_exchange_info);

  rsi_ble_gap_extended_register_callbacks(rsi_ble_simple_peripheral_on_remote_features_event,
                                          rsi_ble_more_data_req_event);

#if SMP_ENABLE
  //! registering the SMP callback functions
  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request,
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 rsi_ble_on_smp_passkey_display,
                                 rsi_ble_on_sc_passkey,
                                 rsi_ble_on_le_ltk_req_event,
                                 rsi_ble_on_le_security_keys_event,
                                 NULL,
#if SIG_VUL
                                 rsi_ble_on_sc_method
#else
                                 NULL
#endif
  );
#endif

  //! initializing the application events map
  rsi_ble_app_init_events();

  //! Set local name
  rsi_bt_set_local_name(RSI_BLE_APP_GATT_TEST);

  //! Set local IRK Value
  //! This value should be fixed on every reset
  LOG_PRINT("\r\n Setting the Local IRK Value\r\n");
  status = rsi_ble_set_local_irk_value(local_irk);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Setting the Local IRK Value Failed = %x\r\n", status);
    return status;
  }

  //! TO-Do: this initialization should be taken care in parsing itself
  //! assign the remote data transfer service and characteristic UUID's to local buffer
  for (uint8_t i = 0; i < TOTAL_CONNECTIONS; i++) {
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_clientservice_uuid =
      RSI_BLE_CLIENT_WRITE_SERVICE_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_client_char_uuid = RSI_BLE_CLIENT_WRITE_CHAR_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_service_uuid =
      RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_char_uuid =
      RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_service_uuid =
      RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_char_uuid =
      RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_service_uuid =
      RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID_M1;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_char_uuid =
      RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID_M1;
  }

  //! Module advertises if master connections are configured
  if (RSI_BLE_MAX_NBR_MASTERS > 0) {
    //! prepare advertise data //local/device name
    adv[3] = strlen(RSI_BLE_APP_GATT_TEST) + 1;
    adv[4] = 9;
    strcpy((char *)&adv[5], RSI_BLE_APP_GATT_TEST);

    //! set advertise data
    rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_APP_GATT_TEST) + 5);
    uint8_t dummy_remote_dev_addr[6] = { 0x00, 0x00, 0x00, 0x11, 0x22, 0x33 };

    status = rsi_ble_set_ble_tx_power(ADV_ROLE, dummy_remote_dev_addr, 4);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\nSET BLE TX POWER FOR ADV_ROLE FAILED : 0x%x\n", status);
    } else {
      LOG_PRINT("\nSET BLE TX POWER FOR ADV_ROLE SUCCESS : 0x%x\n", status);
    }
    //! set device in advertising mode.
    if (!CHK_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE)) {
      status = rsi_ble_start_advertising();
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\n advertising failed \r\n");
      } else {
        LOG_PRINT("\r\n advertising started \r\n");
#if WLAN_TRANSIENT_CASE
        ble_adv_is_there = 1;
#endif
        SET_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE);
      }
    }
  }

  //! Module scans if slave connections are configured
  if ((RSI_BLE_MAX_NBR_SLAVES > 0) & !CHK_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE)) {
    //! start scanning
    status = rsi_ble_start_scanning();
    if (status != RSI_SUCCESS) {
      return status;
    }
    LOG_PRINT("\r\n scanning started \r\n");
#if WLAN_TRANSIENT_CASE
    ble_scanning_is_there = 1;
#endif
    SET_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
  }

  // update the new scan and advertise parameters
  memset(&change_adv_param, 0, sizeof(rsi_ble_req_adv_t));
  memset(&change_scan_param, 0, sizeof(rsi_ble_req_scan_t));

  //! advertise parameters
  change_adv_param.status           = RSI_BLE_START_ADV;
  change_adv_param.adv_type         = UNDIR_NON_CONN; //! non connectable advertising
  change_adv_param.filter_type      = RSI_BLE_ADV_FILTER_TYPE;
  change_adv_param.direct_addr_type = RSI_BLE_ADV_DIR_ADDR_TYPE;
  change_adv_param.adv_int_min      = RSI_BLE_ADV_INT_MIN; //advertising interval - 211.25ms
  change_adv_param.adv_int_max      = RSI_BLE_ADV_INT_MAX;
  change_adv_param.own_addr_type    = LE_PUBLIC_ADDRESS;
  change_adv_param.adv_channel_map  = RSI_BLE_ADV_CHANNEL_MAP;
  rsi_ascii_dev_address_to_6bytes_rev(change_adv_param.direct_addr, (int8_t *)RSI_BLE_ADV_DIR_ADDR);

  //! scan paramaters
  change_scan_param.status        = RSI_BLE_START_SCAN;
  change_scan_param.scan_type     = SCAN_TYPE_PASSIVE;
  change_scan_param.filter_type   = RSI_BLE_SCAN_FILTER_TYPE;
  change_scan_param.scan_int      = LE_SCAN_INTERVAL; //! scan interval 33.125ms
  change_scan_param.scan_win      = LE_SCAN_WINDOW;   //! scan window 13.375ms
  change_scan_param.own_addr_type = LE_PUBLIC_ADDRESS;

#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return status;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif
  return 0;
}

void rsi_ble_main_app_task()
{

  int32_t status   = RSI_SUCCESS;
  int32_t event_id = 0;

#if WLAN_SYNC_REQ
  //! FIXME: Workaround
  if (rsi_wlan_running) {
    LOG_PRINT("Waiting BLE, Wlan to unblock\n");
    rsi_semaphore_wait(&sync_coex_ble_sem, 0);
  }
#endif
  //! BLE dual role Initialization
  status = rsi_ble_dual_role();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("BLE DUAL role init failed \r\n");
  }

  //! start bt inquiry after ble scan
  if (rsi_bt_running) {
    //! wait for ant start if ANT is running
    if (rsi_ant_running) {
      rsi_semaphore_wait(&ble_scan_sem, 0);
    }
    //! ANT and BLE activities started , so start bt inquiry
    rsi_semaphore_post(&bt_inquiry_sem);
  }

  while (1) {
#if WLAN_TRANSIENT_CASE
    if (disable_factor_count == DISABLE_ITER_COUNT) {
      LOG_PRINT("Reach the disable factor in ble main task\r\n");
      if (ble_scanning_is_there || CHK_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE)) {
        status = rsi_ble_stop_scanning();
        if (status == 0) {
          LOG_PRINT("disabled ble scan activity \n");
          CLR_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
        }
        ble_scanning_is_there = 0;
      }
      if (ble_adv_is_there || CHK_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE)) {
        status = rsi_ble_stop_advertising();
        if (status == 0) {
          LOG_PRINT("disabled ble Adv activity \n");
          CLR_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE);
        }
        ble_adv_is_there = 0;
      }
      //! Releasing from here only when no connection.
      if (!(slave_task_instances || master_task_instances)) {
        if (rsi_wlan_running) {
          rsi_semaphore_post(&wlan_sync_coex_ble_sem);
        }
      }
    }
#endif

    //! checking for events list
    event_id = rsi_ble_app_get_event();
    if (event_id == -1) {
      //! wait on events
      rsi_semaphore_wait(&ble_main_task_sem, 0);
      continue;
    }

    switch (event_id) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        //! create task if max slave connections not reached
        if (slave_task_instances < RSI_BLE_MAX_NBR_SLAVES) {
          //! check for valid connection id
          if (slave_conn_id != -1) {
            //! store the connection identifier in individual connection specific buffer
            rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[slave_conn_id].conn_id = slave_conn_id;
            //! create task for processing new slave connection
            status = rsi_task_create((void *)rsi_ble_task_on_conn,
                                     (uint8_t *)"ble_slave_task",
                                     RSI_BLE_APP_TASK_SIZE,
                                     &rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[slave_conn_id],
                                     RSI_BLE_APP_TASK_PRIORITY,
                                     &ble_app_task_handle[slave_conn_id]);
            if (status != RSI_ERROR_NONE) {
              LOG_PRINT("\r\n task%d failed to create, reason = %d\r\n", slave_conn_id, status);
              slave_con_req_pending = 0;
              break;
            }

            //! clear the connection id as it is already used in creating task
            slave_conn_id = -1;
            slave_task_instances++;
          }
        } else {
          LOG_PRINT("\r\n Maximum slave connections reached\r\n");
        }
      } break;
      case RSI_BLE_CONN_EVENT: {
        //! event invokes when connection was completed
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\nIn on comm-conn evt\r\n");
#endif
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        if (remote_device_role == MASTER_ROLE) {
          if (master_task_instances < RSI_BLE_MAX_NBR_MASTERS) {
            for (uint8_t i = 0; i < RSI_BLE_MAX_NBR_MASTERS; i++) {
              //! check for valid connection id
              if (master_conn_id[i] != -1) {
                //! store the connection identifier in individual connection specific buffer
                rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[master_conn_id[i]].conn_id = master_conn_id[i];

                //! create task for processing new master connection
                status = rsi_task_create((void *)rsi_ble_task_on_conn,
                                         (uint8_t *)"ble_master_task",
                                         RSI_BLE_APP_TASK_SIZE,
                                         &rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[master_conn_id[i]],
                                         RSI_BLE_APP_TASK_PRIORITY,
                                         &ble_app_task_handle[master_conn_id[i]]);
                if (status != RSI_ERROR_NONE) {
                  LOG_PRINT("\r\n task%d failed to create\r\n", master_conn_id[i]);
                  break;
                }

                //! clear the connection id as it is already used in creating task
                master_conn_id[i] = -1;
                master_task_instances++;
              }
            }
          } else {
            LOG_PRINT("\r\n Max master connections reached\r\n");
          }
        } else {
          LOG_PRINT("\r\n check why this state occured?\r\n");
          while (1)
            ;
        }
      } break;
      case RSI_BLE_ENHC_CONN_EVENT: {
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\nIn on_enhance_conn evt\r\n");
#endif
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_ENHC_CONN_EVENT);
        if (remote_device_role == MASTER_ROLE) {
          if (master_task_instances < RSI_BLE_MAX_NBR_MASTERS) {
            for (uint8_t i = 0; i < RSI_BLE_MAX_NBR_MASTERS; i++) {
              if (master_conn_id[i] != -1) {
                //! store the connection identifier in individual connection specific buffer
                rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[master_conn_id[i]].conn_id = master_conn_id[i];

                //! create task for processing new master connection
                status = rsi_task_create((void *)rsi_ble_task_on_conn,
                                         (uint8_t *)"ble_master_task",
                                         RSI_BLE_APP_TASK_SIZE,
                                         &rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[master_conn_id[i]],
                                         RSI_BLE_APP_TASK_PRIORITY,
                                         &ble_app_task_handle[master_conn_id[i]]);
                if (status != RSI_ERROR_NONE) {
                  LOG_PRINT("\r\n task%d failed to create\r\n", master_conn_id[i]);
                  break;
                }
                //! clear the connection id as it is already used in creating task
                master_conn_id[i] = -1;
                master_task_instances++;
              }
            }
          } else {
            LOG_PRINT("\r\n Max master connections reached\r\n");
          }
        } else {
          LOG_PRINT("\r\n check why this state occured?");
          while (1)
            ;
        }
      } break;

      case RSI_BLE_GATT_PROFILES: {
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_PROFILES);
        LOG_PRINT("\r\n in gatt test:rsi_ble_gatt_profiles \r\n");
      } break;

      case RSI_BLE_GATT_PROFILE: {
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_PROFILE);
      } break;

      case RSI_BLE_GATT_CHAR_SERVICES: {
        rsi_ble_app_clear_event(RSI_BLE_GATT_CHAR_SERVICES);
      } break;

      case RSI_BLE_GATT_DESC_SERVICES: {
        rsi_ble_app_clear_event(RSI_BLE_GATT_DESC_SERVICES);
      } break;

      case RSI_BLE_RECEIVE_REMOTE_FEATURES: {

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_RECEIVE_REMOTE_FEATURES);
      } break;
      case RSI_BLE_CONN_UPDATE_COMPLETE_EVENT: {
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\nIn conn update evt\r\n");
#endif

        rsi_ble_app_clear_event(RSI_BLE_CONN_UPDATE_COMPLETE_EVENT);
      } break;
      case RSI_BLE_DISCONN_EVENT: {

        LOG_PRINT("\r\nIn dis-conn evt \r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
      } break;
      case RSI_BLE_GATT_WRITE_EVENT: {
        //! event invokes when write/notification events received
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_WRITE_EVENT);

      } break;
      case RSI_BLE_READ_REQ_EVENT: {
#if RSI_DEBUG_EN
        //! event invokes when write/notification events received
        LOG_PRINT_D("\r\nIn on GATT rd evt\r\n");
#endif
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_READ_REQ_EVENT);
      } break;
      case RSI_BLE_MTU_EVENT: {
#if RSI_DEBUG_EN
        //! event invokes when write/notification events received
        LOG_PRINT_D("\r\nIn on mtu evt\r\n");
#endif
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_MTU_EVENT);
      } break;
      case RSI_BLE_SCAN_RESTART_EVENT: {
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SCAN_RESTART_EVENT);

        if (CHK_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE)) {
          status = rsi_ble_stop_scanning();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n scanning stop cmd status = %x\r\n", status);
            //return status;	//! TODO
          } else {
#if WLAN_TRANSIENT_CASE
            ble_scanning_is_there = 0;
#endif
            CLR_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
          }
        }

        LOG_PRINT("\r\n Restarting scanning\r\n");

        if (!CHK_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE)) {
          status = rsi_ble_start_scanning();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n scanning start cmd status = %x\r\n", status);
            //return status;	//! TODO
          } else {
#if WLAN_TRANSIENT_CASE
            ble_scanning_is_there = 1;
#endif
            SET_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
          }
        }

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Restart_scanning\r\n");
          rsi_ble_app_set_event(RSI_BLE_SCAN_RESTART_EVENT);
        }
      } break;
      case RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ: {
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\nIn conn params req evt\r\n");
#endif
        //! remote device conn params request
        //! clear the conn params request event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);
      } break;
      case RSI_BLE_CONN_UPDATE_REQ: {
        rsi_ble_app_clear_event(RSI_BLE_CONN_UPDATE_REQ);
      } break;
      default:
        break;
    }
  }
}
//#endif
