/*******************************************************************************
* @file  rsi_spp_master_slave.c
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

/**
 * Include files
 * */

//! BT include file to refer BT APIs
#include <rsi_bt_apis.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_bt_config.h>
#include <rsi_bt.h>
#include <stdio.h>

//! Common include file
#include <rsi_common_apis.h>
#include "rsi_driver.h"
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif
//! application defines

#define SPP_SLAVE  0
#define SPP_MASTER 1

#define SPP_MODE                     SPP_SLAVE
#define BT_THROUGHPUT_ENABLE_LOGGING 0 //enable macro for SPP prints on console

//! Braktooth vulnerability test define
#if (SPP_MODE == SPP_SLAVE)
#define BT_VUL_TEST 0 //! By default this define is disabled
#if (BT_VUL_TEST == 1)
#define BT_MULTIPLE_HOST_CONN_VUL 0
#endif
#endif

#if (SPP_MODE == SPP_MASTER)
#define RSI_BT_LOCAL_NAME "SPP_MASTER"
#define INQUIRY_ENABLE    0 //! To scan for device and connect
#if INQUIRY_ENABLE
#define MAX_NO_OF_RESPONSES 10
#define INQUIRY_DURATION    10000
#define INQUIRY_TYPE        2
#define MAX_NAME_LENGTH     10
#endif
#else
#define RSI_BT_LOCAL_NAME "SPP_SLAVE"
#endif

#define PIN_CODE       "4321"
#define REMOTE_BD_ADDR "4c:4f:ee:4f:1f:b9" //00:1B:DC:07:2C:F0" //"E4:92:FB:F7:28:BA"

//! application events list
#define RSI_APP_EVENT_CONNECTED    1
#define RSI_APP_EVENT_PINCODE_REQ  2
#define RSI_APP_EVENT_LINKKEY_SAVE 3
#define RSI_APP_EVENT_AUTH_COMPLT  4
#define RSI_APP_EVENT_DISCONNECTED 5
#define RSI_APP_EVENT_LINKKEY_REQ  6
#define RSI_APP_EVENT_SPP_CONN     7
#define RSI_APP_EVENT_SPP_DISCONN  8
#define RSI_APP_EVENT_SPP_RX       9
/** ssp related defines********/
#define RSI_APP_EVENT_PASSKEY_DISPLAY 10
#define RSI_APP_EVENT_PASSKEY_REQUEST 11
#define RSI_APP_EVENT_SSP_COMPLETE    12
#define RSI_APP_EVENT_CONFIRM_REQUEST 13
/*** SNIFF RELATED DEFINES********/
#define RSI_APP_EVENT_MODE_CHANGED    14
#define RSI_APP_EVENT_SNIFF_SUBRATING 15

#define RSI_APP_EVENT_UNBOND_STATUS 16

#define RSI_APP_EVENT_REMOTE_NAME_REQ 17
#define RSI_APP_EVENT_INQUIRY_COMPLT  18

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000
#endif

//! Sniff Parameters
#define SNIFF_MAX_INTERVAL 0xA0
#define SNIFF_MIN_INTERVAL 0XA0
#define SNIFF_ATTEMPT      0X04
#define SNIFF_TIME_OUT     0X02

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 3000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

void rsi_wireless_driver_task(void);

#endif

/*=======================================================================*/
//!    Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
uint8_t power_save_given = 0;
#endif

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };

//! Application global parameters.

static rsi_bt_resp_get_local_name_t local_name = { 0 };
static uint8_t str_conn_bd_addr[18];
static uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
static uint8_t local_dev_addr[RSI_DEV_ADDR_LEN]  = { 0 };
static uint8_t data[RSI_BT_MAX_PAYLOAD_SIZE];
static uint16_t data_len;
static uint8_t linkkey[RSI_LINKKEY_REPLY_SIZE];
uint8_t auth_resp_status = 0;
uint8_t sniff_mode;
app_state_t app_state = (app_state_t)0;

rsi_semaphore_handle_t bt_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;

#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
static uint8_t inq_responses_count;
uint8_t rsi_inq_resp_list[MAX_NO_OF_RESPONSES][RSI_DEV_ADDR_LEN] = { 0 };
uint8_t rsi_inq_resp_list_name_length[MAX_NAME_LENGTH]           = { 0 };
static uint8_t inq_resp_name_length_index;
#endif
static uint16_t g_MaxTxMTU;

#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//!    Firmware logging configurations
/*=======================================================================*/
//! Firmware logging task defines
#define RSI_FW_TASK_STACK_SIZE (512 * 2)
#define RSI_FW_TASK_PRIORITY   1
//! Firmware logging variables
extern rsi_semaphore_handle_t fw_log_app_sem;
rsi_task_handle_t fw_log_task_handle = NULL;
//! Firmware logging prototypes
void sl_fw_log_callback(uint8_t *log_message, uint16_t log_message_length);
void sl_fw_log_task(void);
#endif

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_bt_app_init_events()
{
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
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
void rsi_bt_app_set_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&bt_main_task_sem);

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
static void rsi_bt_app_clear_event(uint32_t event_num)
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
static int32_t rsi_bt_app_get_event(void)
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
 * @fn         rsi_bt_app_on_conn
 * @brief      invoked when connection complete event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] conn_event, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_bt_app_on_conn(uint16_t resp_status, rsi_bt_event_bond_t *conn_event)
{
  if (resp_status == 0) {
    app_state |= (1 << CONNECTED);
    rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
  } else {
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
#if (BT_VUL_TEST == 1)
    LOG_PRINT("\non connection event resp status = %x \n", resp_status);
    return;
#endif
  }
  memcpy((int8_t *)remote_dev_addr, conn_event->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_conn: str_conn_bd_addr: %s, resp_status: 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, conn_event->dev_addr),
            resp_status);
}
void rsi_bt_on_unbond_status(uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_UNBOND_STATUS);
  memcpy((int8_t *)remote_dev_addr, unbond_status->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on unbond status event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii((uint8_t *)str_conn_bd_addr, unbond_status->dev_addr));
  LOG_PRINT("\n Unbond status reason:0x%x\n", resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_pincode_req
 * @brief      invoked when pincode request event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] user_pincode_request, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pincode request from remote device
 */
void rsi_bt_app_on_pincode_req(uint16_t resp_status, rsi_bt_event_user_pincode_request_t *user_pincode_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_PINCODE_REQ);
  memcpy((int8_t *)remote_dev_addr, user_pincode_request->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_pin_code_req: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_pincode_request->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_on_linkkey_req
 * @brief      invoked when linkkey request event is received
 * @param[out] status, connection status of the connected device.
 * @param[out] user_linkkey_req, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey request from remote device
 */
void rsi_ble_app_on_linkkey_req(uint16_t status, rsi_bt_event_user_linkkey_request_t *user_linkkey_req)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_REQ);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_req->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("linkkey_req: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_req->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_on_linkkey_save
 * @brief      invoked when linkkey save event is received
 * @param[out] status, connection status of the connected device.
 * @param[out] user_linkkey_req, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey save from local device
 */
void rsi_ble_app_on_linkkey_save(uint16_t status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_SAVE);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_save->dev_addr, RSI_DEV_ADDR_LEN);
  memcpy(linkkey, user_linkkey_save->linkKey, RSI_LINKKEY_REPLY_SIZE);
  LOG_PRINT("linkkey_save: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_save->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_auth_complete
 * @brief      invoked when authentication event is received
 * @param[out] resp_status, authentication status
 * @param[out] auth_complete, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pairing status and remote device information
 */
void rsi_bt_app_on_auth_complete(uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete)
{
  /* Keeping track of Auth complete response status will be used in unbond event */
  auth_resp_status = resp_status;
  if (resp_status == 0) {
    app_state |= (1 << AUTHENTICATED);
    rsi_bt_app_set_event(RSI_APP_EVENT_AUTH_COMPLT);
  } else {
    memset(linkkey, 0, RSI_LINKKEY_REPLY_SIZE);
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
  memcpy((int8_t *)remote_dev_addr, auth_complete->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("auth_complete: str_conn_bd_addr: %s, resp_status: 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, auth_complete->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_disconn
 * @brief      invoked when disconnect event is received
 * @param[out] resp_status, disconnect status/error
 * @param[out] bt_disconnected, disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the disconnected device information
 */
void rsi_bt_app_on_disconn(uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnected)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  memcpy((int8_t *)remote_dev_addr, bt_disconnected->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_disconn: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, bt_disconnected->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_connect
 * @brief      invoked when spp profile connected event is received
 * @param[out] spp_connect, spp connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the spp connected remote device information
 */
void rsi_bt_app_on_spp_connect(uint16_t resp_status, rsi_bt_event_spp_connect_t *spp_connect)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  app_state |= (1 << SPP_CONNECTED);
  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_CONN);
  memcpy((int8_t *)remote_dev_addr, spp_connect->dev_addr, RSI_DEV_ADDR_LEN);
  g_MaxTxMTU = spp_connect->tx_mtu_size;
  LOG_PRINT("spp_conn: str_conn_bd_addr: %s spp max tx: %d, spp max receive: %d\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, spp_connect->dev_addr),
            spp_connect->tx_mtu_size,
            spp_connect->rx_mtu_size);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_disconnect
 * @brief      invoked when spp profile disconnected event is received
 * @param[out] spp_disconn, spp disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the spp disconnected event
 */
void rsi_bt_app_on_spp_disconnect(uint16_t resp_status, rsi_bt_event_spp_disconnect_t *spp_disconn)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  app_state &= ~(1 << SPP_CONNECTED);
  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_DISCONN);
  memcpy((int8_t *)remote_dev_addr, spp_disconn->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("spp_disconn: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, spp_disconn->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_passkey_display
 * @brief      invoked when passkey display event is received
 * @param[out] resp_status, event status
 * @param[out] bt_event_user_passkey_display, passkey information
 * @return     none.
 * @section description
 * This callback function indicates the passkey display event
 */
void rsi_bt_on_passkey_display(uint16_t resp_status, rsi_bt_event_user_passkey_display_t *bt_event_user_passkey_display)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t ix;
  rsi_bt_app_set_event(RSI_APP_EVENT_PASSKEY_DISPLAY);
  //rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, bt_event_user_passkey_display->passkey);
  memcpy(data, bt_event_user_passkey_display->passkey, 4);
  for (ix = 0; ix < 4; ix++) {
    LOG_PRINT(" 0x%02x,", bt_event_user_passkey_display->passkey[ix]);
  }

  LOG_PRINT("\r\n");
  LOG_PRINT("passkey: %ld", *((uint32_t *)bt_event_user_passkey_display->passkey));
  LOG_PRINT("\r\n");
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_passkey_request
 * @brief      invoked when passkey request event is received
 * @param[out] resp_status, event status
 * @param[out] user_passkey_request, passkey information
 * @return     none.
 * @section description
 * This callback function indicates the passkey request event
 */
void rsi_bt_on_passkey_request(uint16_t resp_status, rsi_bt_event_user_passkey_request_t *user_passkey_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_PASSKEY_REQUEST);
  memcpy((int8_t *)remote_dev_addr, user_passkey_request->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("passkey_request: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_passkey_request->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_ssp_complete
 * @brief      invoked when ssp complete event is received
 * @param[out] resp_status, event status
 * @param[out] ssp_complete, secure simple pairing information
 * @return     none.
 * @section description
 * This callback function indicates the spp status
 */
void rsi_bt_on_ssp_complete(uint16_t resp_status, rsi_bt_event_ssp_complete_t *ssp_complete)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_SSP_COMPLETE);
  memcpy((int8_t *)remote_dev_addr, ssp_complete->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("ssp_complete: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, ssp_complete->dev_addr));
  UNUSED_PARAMETER(resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_confirm_request
 * @brief      invoked when ssp confirm request event is received
 * @param[out] resp_status, event status
 * @param[out] ssp_complete, secure simple pairing requested remote device information
 * @return     none.
 * @section description
 * This callback function indicates the ssp confirm event
 */
void rsi_bt_on_confirm_request(uint16_t resp_status,
                               rsi_bt_event_user_confirmation_request_t *user_confirmation_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t ix;
  rsi_bt_app_set_event(RSI_APP_EVENT_CONFIRM_REQUEST);

  memcpy(data, user_confirmation_request->confirmation_value, 4);
  for (ix = 0; ix < 4; ix++) {
    LOG_PRINT(" 0x%02x,", user_confirmation_request->confirmation_value[ix]);
  }
  LOG_PRINT("\r\n");
  LOG_PRINT("data: %s", user_confirmation_request->confirmation_value);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_mode_change
 * @brief      invoked when mode is changed event is received
 * @param[out] mode_change, mode related information
 * @return     none.
 * @section description
 * This callback function indicates the mode change in BT device.
 */

void rsi_bt_on_mode_change(uint16_t resp_status, rsi_bt_event_mode_change_t *mode_change)
{

  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  sniff_mode = mode_change->current_mode;
  /* 0x00 Active Mode.
		 * 0x01 Hold Mode.
		 * 0x02 Sniff Mode. */
  LOG_PRINT("sniff_mode:%d", sniff_mode);
  rsi_bt_app_set_event(RSI_APP_EVENT_MODE_CHANGED);
  memcpy((int8_t *)remote_dev_addr, mode_change->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("mode_change_event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, mode_change->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_sniff_subrating
 * @brief      invoked when mode is changed(sniff subrating) event is received
 * @param[out] mode_change, mode related information
 * @return     none.
 * @section description
 * This callback function indicates the mode change in BT device.
 */
void rsi_bt_on_sniff_subrating(uint16_t resp_status, rsi_bt_event_sniff_subrating_t *mode_change)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_SNIFF_SUBRATING);
  memcpy((int8_t *)remote_dev_addr, mode_change->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("mode_change_event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, mode_change->dev_addr));
}
#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
/*==============================================*/
/**
 * @fn         rsi_bt_inq_response
 * @brief      invoked when bt inquiry response is received
 * @param[out] status - status of inquiry
 *        resp_event - bt inquiry response
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote bt inquiry response is received
 */
void rsi_bt_inq_response(uint16_t status, rsi_bt_event_inquiry_response_t *resp_event)
{
  uint8_t tmp_str_addr[BD_ADDR_ARRAY_LEN] = { 0 };
  if (status != 0) {
    return;
  }
  rsi_6byte_dev_address_to_ascii(tmp_str_addr, resp_event->dev_addr);

  memcpy(&rsi_inq_resp_list[inq_responses_count][0],
         rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, (int8_t *)tmp_str_addr),
         RSI_DEV_ADDR_LEN);
  rsi_inq_resp_list_name_length[inq_responses_count++] = resp_event->name_length;
  LOG_PRINT("\r\n inq_type : %d, Bdaddr : %s, class : %d%d%d, rssi : %d, name : %s ",
            resp_event->inquiry_type,
            tmp_str_addr,
            resp_event->cod[0],
            resp_event->cod[1],
            resp_event->cod[2],
            resp_event->rssi,
            (resp_event->name_length) ? resp_event->remote_device_name : NULL);
}
/*==============================================*/
/**
 * @fn         rsi_remote_name_response
 * @brief      invoked when remote name request is called
 * @param[out] status - status of remote name request
 *        remote_name_response_event - remote name response.
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote name request initiated in application
 */
void rsi_remote_name_response(uint16_t status, rsi_bt_event_remote_device_name_t *remote_name_response_event)
{
  uint8_t tmp_str_addr[BD_ADDR_ARRAY_LEN] = { 0 };
  if (status != 0) {
    LOG_PRINT("\r\n Remote name response error with status : %x", status);
  } else {
    LOG_PRINT("\r\n remote name response received \n");
    rsi_6byte_dev_address_to_ascii(tmp_str_addr, remote_name_response_event->dev_addr);
    LOG_PRINT("\r\n Bdaddr : %s, name_length : %d, name : %s\n ",
              tmp_str_addr,
              remote_name_response_event->name_length,
              remote_name_response_event->remote_device_name);
  }
  inq_resp_name_length_index++;
  rsi_bt_app_set_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
}

/*==============================================*/
/**
 * @fn         rsi_bt_inquiry_complete
 * @brief      invoked when bt inquiry is completed
 * @param[out] status - status of inquiry
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote bd addresses inquiry is completed
 */
void rsi_bt_inquiry_complete(uint16_t status)
{
  if (status != 0) {
    LOG_PRINT("\r\n Inq complete error with status : %x", status);
  }
  LOG_PRINT("\r\n Inq completed \n");
  rsi_bt_app_set_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
}
#endif
/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_data_rx
 * @brief      invoked when spp data rx event is received
 * @param[out] spp_receive, spp data from remote device
 * @return     none.
 * @section description
 * This callback function indicates the spp data received event
 */
void rsi_bt_app_on_spp_data_rx(uint16_t resp_status, rsi_bt_event_spp_receive_t *spp_receive)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  uint16_t ix;

  app_state |= (1 << SPP_TRANSMISSION);
  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_RX);
  data_len = spp_receive->data_len;
  memset(data, 0, sizeof(data));
  spp_receive->data[spp_receive->data_len] = '\0';
  memcpy(data, spp_receive->data, spp_receive->data_len);
#if BT_THROUGHPUT_ENABLE_LOGGING
  /* RSC-9583 To achieve higher BT throughputs, while continuous Tx/Rx BT_THROUGHPUT_ENABLE_LOGGING needs to be disabled
 * to ensure there is no delay in Rx packet receiving */
  LOG_PRINT("spp_rx: data_len: %d, data: ", spp_receive->data_len);
  LOG_PRINT("\r\n");
  LOG_PRINT("data: %s", spp_receive->data);
#endif
}

void switch_proto_async(uint16_t mode, uint8_t *bt_disabled_status)
{
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(
    bt_disabled_status); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\n IN ASYNC \n");
}

/*==============================================*/
/**
 * @fn         rsi_bt_spp_slave
 * @brief      Tests the BT Classic SPP Slave role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the SPP Slave role.
 */
int32_t rsi_bt_spp_slave(void)
{
  int32_t status          = 0;
  int32_t temp_event_map  = 0;
  uint8_t str_bd_addr[18] = { 0 };
  uint8_t eir_data[200]   = { 2, 1, 0 };

#ifdef FW_LOGGING_ENABLE
  //Fw log component level
  sl_fw_log_level_t fw_component_log_level;
#endif

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#if (SPP_MODE == SPP_MASTER)
  int32_t tx_ix           = 0;
  uint8_t spp_tx_data_len = 0;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

#ifdef RSI_WITH_OS
  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BT_CLASSIC);

  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_switch_proto(1, NULL);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n bt enable fail \n");
    //return status;
  }

#ifdef FW_LOGGING_ENABLE
  //! Set log levels for firmware components
  sl_set_fw_component_log_levels(&fw_component_log_level);

  //! Configure firmware logging
  status = sl_fw_log_configure(FW_LOG_ENABLE,
                               FW_TSF_GRANULARITY_US,
                               &fw_component_log_level,
                               FW_LOG_BUFFER_SIZE,
                               sl_fw_log_callback);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Firmware Logging Init Failed\r\n");
  }

#ifdef RSI_WITH_OS
  //! Create firmware logging semaphore
  rsi_semaphore_create(&fw_log_app_sem, 0);
  //! Create firmware logging task
  rsi_task_create((rsi_task_function_t)sl_fw_log_task,
                  (uint8_t *)"fw_log_task",
                  RSI_FW_TASK_STACK_SIZE,
                  NULL,
                  RSI_FW_TASK_PRIORITY,
                  &fw_log_task_handle);
#endif
#endif

  //! BT register GAP callbacks:
  rsi_bt_gap_register_callbacks(NULL, //role_change
                                rsi_bt_app_on_conn,
                                rsi_bt_on_unbond_status, //
                                rsi_bt_app_on_disconn,
#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
                                rsi_bt_inq_response,      //NULL, //scan_resp
                                rsi_remote_name_response, //NULL, //remote_name_req
#else
                                NULL,
                                NULL,
#endif
                                rsi_bt_on_passkey_display, //passkey_display
                                NULL,                      //remote_name_req+cancel
                                rsi_bt_on_confirm_request, //confirm req
                                rsi_bt_app_on_pincode_req,
                                rsi_bt_on_passkey_request, //passkey request
#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
                                rsi_bt_inquiry_complete, //NULL, //inquiry complete
#else
                                NULL,
#endif
                                rsi_bt_app_on_auth_complete,
                                rsi_ble_app_on_linkkey_req, //linkkey request
                                rsi_bt_on_ssp_complete,     //ssp coplete
                                rsi_ble_app_on_linkkey_save,
                                NULL, //get services
                                NULL,
                                rsi_bt_on_mode_change,
                                rsi_bt_on_sniff_subrating, //search service
                                NULL);
  //! create ble main task if bt protocol is selected
  rsi_semaphore_create(&bt_main_task_sem, 0);
  //! initialize the event map
  rsi_bt_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(local_dev_addr);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_bd_address: %s\r\n", rsi_6byte_dev_address_to_ascii(str_bd_addr, local_dev_addr));

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BT_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&local_name);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_name: %s\r\n", local_name.name);

  //! prepare Extended Response Data
  eir_data[3] = strlen(RSI_BT_LOCAL_NAME) + 1;
  eir_data[4] = 9;
  strcpy((char *)&eir_data[5], RSI_BT_LOCAL_NAME);
  //! set eir data
  rsi_bt_set_eir_data(eir_data, strlen(RSI_BT_LOCAL_NAME) + 5);
#if (SPP_MODE != SPP_MASTER)
  //! start the discover mode
  status = rsi_bt_start_discoverable();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! start the connectability mode
  status = rsi_bt_set_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#else
  //! stop the discover mode
  status = rsi_bt_stop_discoverable();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! stop the connectability mode
  status = rsi_bt_set_non_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
  //! start the ssp mode
  status = rsi_bt_set_ssp_mode(1, 1);
  if (status != RSI_SUCCESS) {
    return status;
  }
  //! initilize the SPP profile
  status = rsi_bt_spp_init();
  if (status != RSI_SUCCESS) {
    return status;
  }

  LOG_PRINT("spp init success\n");
  //! register the SPP profile callback's
  rsi_bt_spp_register_callbacks(rsi_bt_app_on_spp_connect, rsi_bt_app_on_spp_disconnect, rsi_bt_app_on_spp_data_rx);
#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
  inq_responses_count = 0;
  //! Start inquiry
  status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
  if (status != 0) {
    LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
  }
  LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
#endif

#if ((SPP_MODE == SPP_MASTER) && (!INQUIRY_ENABLE))
  status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("rsi_bt_connect: failed status 0x%x", status);
  }
  LOG_PRINT("bt_conn resp is 0x%x \n", status);
#endif
  while (1) {
    //! Application main loop

#ifndef RSI_WITH_OS
    //! run the non OS scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_bt_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      rsi_semaphore_wait(&bt_main_task_sem, 0);
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONNECTED);
      } break;
      case RSI_APP_EVENT_UNBOND_STATUS: {
        //! remote device connected event

        //! clear the connected event.
        LOG_PRINT("\n Unbond status EVENT \n");
        rsi_bt_app_clear_event(RSI_APP_EVENT_UNBOND_STATUS);
#if (SPP_MODE == SPP_MASTER)
        /* Initialize BT connect only if auth resp status is success */
        if (auth_resp_status == 0) {
          status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n reconnect status = %x \n", status);
          }
          LOG_PRINT("bt_conn resp is 0x%x \n", status);
        } else {
          /* Clearing auth response status */
          auth_resp_status = 0;
        }
        LOG_PRINT("bt_conn resp is 0x%x \n", status);
#endif
      } break;

      case RSI_APP_EVENT_PINCODE_REQ: {
        //! pincode request event
        uint8_t *pin_code = (uint8_t *)PIN_CODE;

        //! clear the pincode request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PINCODE_REQ);

        //! sending the pincode requet reply
        status = rsi_bt_pincode_request_reply((uint8_t *)remote_dev_addr, pin_code, 1);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_pincode_request_reply: failed status 0x%x", status);
        }
      } break;
      case RSI_APP_EVENT_LINKKEY_SAVE: {
        //! linkkey save event

        //! clear the likkey save event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_SAVE);
      } break;
      case RSI_APP_EVENT_AUTH_COMPLT: {
        //! authentication complete event

        //! clear the authentication complete event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_AUTH_COMPLT);
#if (SPP_MODE == SPP_MASTER)
        rsi_delay_ms(500);
        status = rsi_bt_spp_connect(remote_dev_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_spp_connect: failed status 0x%x", status);
        }
#endif
      } break;
      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device connected event

        //! clear the disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        app_state = (app_state_t)0;
#if (BT_VUL_TEST == 1)
        //! start the discover mode
        status = rsi_bt_start_discoverable();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_start_discoverable: failed status 0x%x", status);
        }

        //! start the connectability mode
        status = rsi_bt_set_connectable();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_set_connectable: failed status 0x%x", status);
        }
#endif

#if (ENABLE_POWER_SAVE)
        //! initiating Active mode in BLE mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in keep Module in ACTIVE mode \r\n");
        }
        //! initiating Active mode in wlan mode
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in keep Module in ACTIVE mode \r\n");
        }
        LOG_PRINT("In Active mode:\n");
        power_save_given = 0;

#endif
#if (SPP_MODE == SPP_MASTER)
        LOG_PRINT("Looking for Device\r\n");
        status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_connect: failed status 0x%x", status);
        }
#endif
      } break;
      case RSI_APP_EVENT_LINKKEY_REQ: {
        //! linkkey request event

        //! clear the linkkey request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_REQ);
        if ((linkkey[0] == 0) && (linkkey[15] == 0)) {
          LOG_PRINT("linkkey request negative reply\r\n");
          //! sending the linkkey request negative reply
          rsi_bt_linkkey_request_reply((uint8_t *)remote_dev_addr, NULL, 0);
        } else {
          LOG_PRINT("linkkey request positive reply\r\n");
          //! sending the linkkey request positive reply
          rsi_bt_linkkey_request_reply((uint8_t *)remote_dev_addr, linkkey, 1);
        }
      } break;

      case RSI_APP_EVENT_SPP_CONN: {
        //! spp connected event

        //! clear the spp connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_CONN);
#if (BT_VUL_TEST == 1)
#if (BT_MULTIPLE_HOST_CONN_VUL == 1)
        //! stop the discover mode
        status = rsi_bt_stop_discoverable();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_stop_discoverable: failed status 0x%x", status);
          //return status;
        }

        //! stop the connectability mode
        status = rsi_bt_set_non_connectable();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_set_non_connectable: failed status 0x%x", status);
          //return status;
        }

        LOG_PRINT("\nAttempt to connect to same BD addr device from host");
        status = rsi_bt_connect(remote_dev_addr);
        LOG_PRINT("\nConnection status: %x", status);
        break;
#endif
#endif
#if (SPP_MODE == SPP_MASTER)
        strcpy((char *)data, "spp_test_sample_1");
        spp_tx_data_len       = strlen((char *)data);
        data[spp_tx_data_len] = (tx_ix++) % 10;
        status                = rsi_bt_spp_transfer(remote_dev_addr, data, strlen(data));
#endif
        /* here we call the sniff_mode command*/
        status =
          rsi_bt_sniff_mode(remote_dev_addr, SNIFF_MAX_INTERVAL, SNIFF_MIN_INTERVAL, SNIFF_ATTEMPT, SNIFF_TIME_OUT);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("rsi_bt_sniff_mode: failed status 0x%x", status);
        }

      } break;

      case RSI_APP_EVENT_SPP_DISCONN: {
        //! spp disconnected event

        //! clear the spp disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_DISCONN);
      } break;

      case RSI_APP_EVENT_SPP_RX: {
        //! spp receive event

        //! clear the spp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_RX);

        //! SPP data transfer (loop back)
#if (SPP_MODE != SPP_MASTER)
        rsi_bt_spp_transfer(remote_dev_addr, data, data_len);
#else
        strcpy((char *)data, "spp_test_sample_1");
        spp_tx_data_len       = strlen((char *)data);
        data[spp_tx_data_len] = (tx_ix++) % 10;
        LOG_PRINT("tx_ix: %d\r\n", tx_ix);
        status = rsi_bt_spp_transfer(remote_dev_addr, data, strlen(data));
#endif
      } break;

      case RSI_APP_EVENT_PASSKEY_DISPLAY: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PASSKEY_DISPLAY);
      } break;

      case RSI_APP_EVENT_PASSKEY_REQUEST: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PASSKEY_REQUEST);
      } break;

      case RSI_APP_EVENT_SSP_COMPLETE: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SSP_COMPLETE);
        LOG_PRINT(" SSP conection completed\n");
      } break;

      case RSI_APP_EVENT_CONFIRM_REQUEST: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONFIRM_REQUEST);
        LOG_PRINT("Confirmation is requested\n");
        rsi_bt_accept_ssp_confirm((uint8_t *)remote_dev_addr);

      } break;

      case RSI_APP_EVENT_MODE_CHANGED: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_MODE_CHANGED);
        //! initiating bt powersave mode
        if (sniff_mode == 2) { //sniff mode
#if (ENABLE_POWER_SAVE)
          if (!power_save_given) {
            //! enable wlan radio
            status = rsi_wlan_radio_init();
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\n radio init failed \n");
            }
            //! initiating power save in BLE mode
            status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n Failed in initiating power save \r\n");
            }
            //! initiating power save in wlan mode
            status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n Failed in initiating power save \r\n");
            }
            LOG_PRINT("In Power Save mode:\n");
            power_save_given = 1;
          }
#endif
        }
        //need to exit sniff in case of disconn to support for reconn
#if 1
        if ((sniff_mode == 0) && (app_state & (1 << SPP_CONNECTED))) //0 - unsniff mode
        {
          /* here we are initiating sniff mode req again*/
          status =
            rsi_bt_sniff_mode(remote_dev_addr, SNIFF_MAX_INTERVAL, SNIFF_MIN_INTERVAL, SNIFF_ATTEMPT, SNIFF_TIME_OUT);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("rsi_bt_sniff_mode: failed status 0x%x", status);
          }
        }
#endif
      } break;

      case RSI_APP_EVENT_SNIFF_SUBRATING: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SNIFF_SUBRATING);
        LOG_PRINT("SNIFF SUBRATING IS COMPLETED\n");
      } break;

#if ((SPP_MODE == SPP_MASTER) && (INQUIRY_ENABLE))
      case RSI_APP_EVENT_REMOTE_NAME_REQ: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
        uint8_t inq_triggered = 0;
        for (; inq_resp_name_length_index < inq_responses_count; inq_resp_name_length_index++) {
          if (!rsi_inq_resp_list_name_length[inq_resp_name_length_index]) {
            inq_triggered = 1;
            status        = rsi_bt_remote_name_request_async(&rsi_inq_resp_list[inq_resp_name_length_index][0], NULL);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n status= %x Remote name request failed \n", status);
            } else {
              LOG_PRINT("\r\n Remote Name Requested Succesfully, Response will come asynchronously\n");
            }
            break;
          }
        }
        if (!inq_triggered)
          rsi_bt_app_set_event(RSI_APP_EVENT_INQUIRY_COMPLT);
      } break;

      case RSI_APP_EVENT_INQUIRY_COMPLT: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_INQUIRY_COMPLT);
        uint8_t bt_device_found = 0;
        //! Check in all devices, whether the intended slave is present or not.
        for (int i = 0; i < inq_responses_count; i++) {
          if (memcmp(&rsi_inq_resp_list[i][0],
                     rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR),
                     RSI_DEV_ADDR_LEN)
              == 0) {
            bt_device_found = 1;
            break;
          }
        }
        if (bt_device_found) {
          status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n rsi_bt_connect failed : %x \n", status);
          }
          LOG_PRINT("bt_conn resp is 0x%x \n", status);
        } else {
          inq_responses_count        = 0;
          inq_resp_name_length_index = 0;
          //! Start inquiry again
          status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
          if (status != 0) {
            LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
          }
          LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
        }
      } break;
#endif
      default: {
      }
    }
  }
}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop(void)
{
  while (1) {
    rsi_wireless_driver_task();
  }
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main(void)
{
  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t bt_task_handle = NULL;

#endif
#ifndef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS
  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! Call BLE Peripheral application
  status = rsi_bt_spp_slave();

  //! Application main loop
  main_loop();

  return status;
#endif

#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_bt_spp_slave,
                  (uint8_t *)"bt_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  rsi_start_os_scheduler();
  return status;
#endif
}
