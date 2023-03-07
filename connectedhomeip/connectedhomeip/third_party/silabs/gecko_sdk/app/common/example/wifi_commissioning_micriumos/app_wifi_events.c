/***************************************************************************//**
 * @file
 * @brief Wi-Fi messages processing
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>
#include "sl_wfx_host.h"
#include "dhcp_server.h"
#include "app_webpage.h"
#include "app_wifi_events.h"

// Event Task Configurations
#define WFX_EVENTS_TASK_PRIO              21u
#define WFX_EVENTS_TASK_STK_SIZE        1024u
#define WFX_EVENTS_NB_MAX                 10u

void sl_wfx_connect_callback(sl_wfx_connect_ind_t *connect);
void sl_wfx_disconnect_callback(sl_wfx_disconnect_ind_t *disconnect);
void sl_wfx_start_ap_callback(sl_wfx_start_ap_ind_t *start_ap);
void sl_wfx_stop_ap_callback(sl_wfx_stop_ap_ind_t *stop_ap);
void sl_wfx_scan_result_callback(sl_wfx_scan_result_ind_t *scan_result);
void sl_wfx_scan_complete_callback(sl_wfx_scan_complete_ind_t *scan_complete);
void sl_wfx_generic_status_callback(sl_wfx_generic_ind_t *frame);
void sl_wfx_ap_client_connected_callback(sl_wfx_ap_client_connected_ind_t *ap_client_connected);
void sl_wfx_ap_client_rejected_callback(sl_wfx_ap_client_rejected_ind_t *ap_client_rejected);
void sl_wfx_ap_client_disconnected_callback(sl_wfx_ap_client_disconnected_ind_t *ap_client_disconnected);
void sl_wfx_host_received_frame_callback(sl_wfx_received_ind_t *rx_buffer);

extern char event_log[];
extern char softap_ssid[32 + 1];

OS_Q wifi_events;
scan_result_list_t scan_list[SL_WFX_MAX_SCAN_RESULTS];
uint8_t scan_count_web = 0;

static uint8_t scan_count = 0;
bool           scan_verbose   = true;
static CPU_STK wfx_events_task_stk[WFX_EVENTS_TASK_STK_SIZE];
static OS_TCB wfx_events_task_tcb;
extern OS_SEM scan_sem;

/**************************************************************************//**
 * Function processing the incoming Wi-Fi messages
 *****************************************************************************/
sl_status_t sl_wfx_host_process_event(sl_wfx_generic_message_t *event_payload)
{
  switch (event_payload->header.id) {
    /******** INDICATION ********/
    case SL_WFX_CONNECT_IND_ID:
    {
      sl_wfx_connect_callback((sl_wfx_connect_ind_t *)event_payload);
      break;
    }
    case SL_WFX_DISCONNECT_IND_ID:
    {
      sl_wfx_disconnect_callback((sl_wfx_disconnect_ind_t *)event_payload);
      break;
    }
    case SL_WFX_START_AP_IND_ID:
    {
      sl_wfx_start_ap_callback((sl_wfx_start_ap_ind_t *)event_payload);
      break;
    }
    case SL_WFX_STOP_AP_IND_ID:
    {
      sl_wfx_stop_ap_callback((sl_wfx_stop_ap_ind_t*)event_payload);
      break;
    }
    case SL_WFX_RECEIVED_IND_ID:
    {
      sl_wfx_received_ind_t* ethernet_frame = (sl_wfx_received_ind_t*) event_payload;
      if ( ethernet_frame->body.frame_type == 0 ) {
        sl_wfx_host_received_frame_callback(ethernet_frame);
      }
      break;
    }
    case SL_WFX_SCAN_RESULT_IND_ID:
    {
      sl_wfx_scan_result_callback((sl_wfx_scan_result_ind_t *)event_payload);
      break;
    }
    case SL_WFX_SCAN_COMPLETE_IND_ID:
    {
      sl_wfx_scan_complete_callback((sl_wfx_scan_complete_ind_t *)event_payload);
      break;
    }
    case SL_WFX_AP_CLIENT_CONNECTED_IND_ID:
    {
      sl_wfx_ap_client_connected_callback((sl_wfx_ap_client_connected_ind_t *)event_payload);
      break;
    }
    case SL_WFX_AP_CLIENT_REJECTED_IND_ID:
    {
      sl_wfx_ap_client_rejected_callback((sl_wfx_ap_client_rejected_ind_t *)event_payload);
      break;
    }
    case SL_WFX_AP_CLIENT_DISCONNECTED_IND_ID:
    {
      sl_wfx_ap_client_disconnected_callback((sl_wfx_ap_client_disconnected_ind_t *)event_payload);
      break;
    }
    case SL_WFX_GENERIC_IND_ID:
    {
      sl_wfx_generic_status_callback((sl_wfx_generic_ind_t *) event_payload);
      break;
    }
    case SL_WFX_EXCEPTION_IND_ID:
    {
      sl_wfx_exception_ind_t *firmware_exception = (sl_wfx_exception_ind_t*) event_payload;
      uint8_t *exception_tmp = (uint8_t *) firmware_exception;
      printf("firmware exception %lu\r\n", firmware_exception->body.reason);
      for (uint16_t i = 0; i < firmware_exception->header.length; i += 16) {
        printf("hif: %.8x:", i);
        for (uint8_t j = 0; (j < 16) && ((i + j) < firmware_exception->header.length); j++) {
          printf(" %.2x", *exception_tmp);
          exception_tmp++;
        }
        printf("\r\n");
      }
      break;
    }
    case SL_WFX_ERROR_IND_ID:
    {
      sl_wfx_error_ind_t *firmware_error = (sl_wfx_error_ind_t*) event_payload;
      uint8_t *error_tmp = (uint8_t *) firmware_error;
      printf("firmware error %lu\r\n", firmware_error->body.type);
      for (uint16_t i = 0; i < firmware_error->header.length; i += 16) {
        printf("hif: %.8x:", i);
        for (uint8_t j = 0; (j < 16) && ((i + j) < firmware_error->header.length); j++) {
          printf(" %.2x", *error_tmp);
          error_tmp++;
        }
        printf("\r\n");
      }
      break;
    }
    /******** CONFIRMATION ********/
    case SL_WFX_SEND_FRAME_CNF_ID:
    {
      break;
    }
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Callback for individual scan result
 *****************************************************************************/
void sl_wfx_scan_result_callback(sl_wfx_scan_result_ind_t *scan_result)
{
  scan_count++;

  if (scan_verbose) {
    /*Report one AP information*/
    printf(
      "# %2d %2d %02X %03d %02X:%02X:%02X:%02X:%02X:%02X  %s\r\n",
      scan_count,
      scan_result->body.channel,
      *(uint8_t *)&scan_result->body.security_mode,
      ((int16_t)(scan_result->body.rcpi - 220) / 2),
      scan_result->body.mac[0], scan_result->body.mac[1],
      scan_result->body.mac[2], scan_result->body.mac[3],
      scan_result->body.mac[4], scan_result->body.mac[5],
      scan_result->body.ssid_def.ssid);
    printf("\r\n");
  }

  if (scan_count <= SL_WFX_MAX_SCAN_RESULTS) {
    scan_list[scan_count - 1].ssid_def = scan_result->body.ssid_def;
    scan_list[scan_count - 1].channel = scan_result->body.channel;
    scan_list[scan_count - 1].security_mode = scan_result->body.security_mode;
    scan_list[scan_count - 1].rcpi = scan_result->body.rcpi;
    memcpy(scan_list[scan_count - 1].mac, scan_result->body.mac, 6);
  }
}

/**************************************************************************//**
 * Callback for scan complete
 *****************************************************************************/
void sl_wfx_scan_complete_callback(sl_wfx_scan_complete_ind_t *scan_complete)
{
  void * buffer;
  sl_status_t status;
  RTOS_ERR err;

  scan_count_web = scan_count;
  scan_count = 0;

  status = sl_wfx_host_allocate_buffer(&buffer,
                                       SL_WFX_RX_FRAME_BUFFER,
                                       scan_complete->header.length);
  if (status == SL_STATUS_OK) {
    memcpy(buffer, (void *)scan_complete, scan_complete->header.length);
    OSQPost(&wifi_events,
            buffer,
            scan_complete->header.length,
            OS_OPT_POST_FIFO,
            &err);
  }
}
/**************************************************************************//**
 * Callback when station connects
 *****************************************************************************/
void sl_wfx_connect_callback(sl_wfx_connect_ind_t *connect)
{
  void *buffer;
  sl_status_t status;
  RTOS_ERR err;

  switch (connect->body.status) {
    case WFM_STATUS_SUCCESS:
    {
      printf("Connected\r\n");
      sl_wfx_context->state |= SL_WFX_STA_INTERFACE_CONNECTED;

      status = sl_wfx_host_allocate_buffer(&buffer,
                                           SL_WFX_RX_FRAME_BUFFER,
                                           connect->header.length);
      if (status == SL_STATUS_OK) {
        memcpy(buffer, (void *)connect, connect->header.length);
        OSQPost(&wifi_events,
                buffer,
                connect->header.length,
                OS_OPT_POST_FIFO,
                &err);
      }
      break;
    }
    case WFM_STATUS_NO_MATCHING_AP:
    {
      strcpy(event_log, "Connection failed, access point not found");
      printf("%s\r\n", event_log);
      break;
    }
    case WFM_STATUS_CONNECTION_ABORTED:
    {
      strcpy(event_log, "Connection aborted");
      printf("%s\r\n", event_log);
      break;
    }
    case WFM_STATUS_CONNECTION_TIMEOUT:
    {
      strcpy(event_log, "Connection timeout");
      printf("%s\r\n", event_log);
      break;
    }
    case WFM_STATUS_CONNECTION_REJECTED_BY_AP:
    {
      strcpy(event_log, "Connection rejected by the access point");
      printf("%s\r\n", event_log);
      break;
    }
    case WFM_STATUS_CONNECTION_AUTH_FAILURE:
    {
      strcpy(event_log, "Connection authentication failure");
      printf("%s\r\n", event_log);
      break;
    }
    default:
    {
      strcpy(event_log, "Connection attempt error");
      printf("%s\r\n", event_log);
    }
  }
}

/**************************************************************************//**
 * Callback for station disconnect
 *****************************************************************************/
void sl_wfx_disconnect_callback(sl_wfx_disconnect_ind_t *disconnect)
{
  void *buffer;
  sl_status_t status;
  RTOS_ERR err;

  printf("Disconnected %d\r\n", disconnect->body.reason);
  sl_wfx_context->state &= ~SL_WFX_STA_INTERFACE_CONNECTED;

  status = sl_wfx_host_allocate_buffer(&buffer,
                                       SL_WFX_RX_FRAME_BUFFER,
                                       disconnect->header.length);
  if (status == SL_STATUS_OK) {
    memcpy(buffer, (void *)disconnect, disconnect->header.length);
    OSQPost(&wifi_events,
            buffer,
            disconnect->header.length,
            OS_OPT_POST_FIFO,
            &err);
  }
}

/**************************************************************************//**
 * Callback for AP started
 *****************************************************************************/
void sl_wfx_start_ap_callback(sl_wfx_start_ap_ind_t *start_ap)
{
  void *buffer;
  sl_status_t status;
  RTOS_ERR err;

  if (start_ap->body.status == 0) {
    printf("AP started\r\n");
    printf("Join the AP with SSID: %s\r\n", softap_ssid);
    sl_wfx_context->state |= SL_WFX_AP_INTERFACE_UP;

    status = sl_wfx_host_allocate_buffer(&buffer,
                                         SL_WFX_RX_FRAME_BUFFER,
                                         start_ap->header.length);
    if (status == SL_STATUS_OK) {
      memcpy(buffer, (void *)start_ap, start_ap->header.length);
      OSQPost(&wifi_events,
              buffer,
              start_ap->header.length,
              OS_OPT_POST_FIFO,
              &err);
    }
  } else {
    printf("AP start failed\r\n");
    strcpy(event_log, "AP start failed");
  }
}

/**************************************************************************//**
 * Callback for AP stopped
 *****************************************************************************/
void sl_wfx_stop_ap_callback(sl_wfx_stop_ap_ind_t *stop_ap)
{
  void *buffer;
  sl_status_t status;
  RTOS_ERR err;

  printf("SoftAP stopped\r\n");
  dhcpserver_clear_stored_mac();
  sl_wfx_context->state &= ~SL_WFX_AP_INTERFACE_UP;

  status = sl_wfx_host_allocate_buffer(&buffer,
                                       SL_WFX_RX_FRAME_BUFFER,
                                       stop_ap->length);
  if (status == SL_STATUS_OK) {
    memcpy(buffer, (void *)stop_ap, stop_ap->length);
    OSQPost(&wifi_events, buffer, stop_ap->length, OS_OPT_POST_FIFO, &err);
  }
}
/**************************************************************************//**
 * Callback for client connect to AP
 *****************************************************************************/
void sl_wfx_ap_client_connected_callback(sl_wfx_ap_client_connected_ind_t *ap_client_connected)
{
  printf("Client connected, MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
         ap_client_connected->body.mac[0],
         ap_client_connected->body.mac[1],
         ap_client_connected->body.mac[2],
         ap_client_connected->body.mac[3],
         ap_client_connected->body.mac[4],
         ap_client_connected->body.mac[5]);
  printf("Open a web browser and go to http://%d.%d.%d.%d\r\n",
         ap_ip_addr0, ap_ip_addr1, ap_ip_addr2, ap_ip_addr3);
}

/**************************************************************************//**
 * Callback for client rejected from AP
 *****************************************************************************/
void sl_wfx_ap_client_rejected_callback(sl_wfx_ap_client_rejected_ind_t *ap_client_rejected)
{
  struct eth_addr mac_addr;
  memcpy(&mac_addr, ap_client_rejected->body.mac, SL_WFX_BSSID_SIZE);
  dhcpserver_remove_mac(&mac_addr);
  printf("Client rejected, reason: %d, MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
         ap_client_rejected->body.reason,
         ap_client_rejected->body.mac[0],
         ap_client_rejected->body.mac[1],
         ap_client_rejected->body.mac[2],
         ap_client_rejected->body.mac[3],
         ap_client_rejected->body.mac[4],
         ap_client_rejected->body.mac[5]);
}

/**************************************************************************//**
 * Callback for AP client disconnect
 *****************************************************************************/
void sl_wfx_ap_client_disconnected_callback(sl_wfx_ap_client_disconnected_ind_t *ap_client_disconnected)
{
  struct eth_addr mac_addr;
  memcpy(&mac_addr, ap_client_disconnected->body.mac, SL_WFX_BSSID_SIZE);
  dhcpserver_remove_mac(&mac_addr);
  printf("Client disconnected, reason: %d, MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
         ap_client_disconnected->body.reason,
         ap_client_disconnected->body.mac[0],
         ap_client_disconnected->body.mac[1],
         ap_client_disconnected->body.mac[2],
         ap_client_disconnected->body.mac[3],
         ap_client_disconnected->body.mac[4],
         ap_client_disconnected->body.mac[5]);
}

/**************************************************************************//**
 * Callback for generic status received
 *****************************************************************************/
void sl_wfx_generic_status_callback(sl_wfx_generic_ind_t* frame)
{
  (void)(frame);
  printf("Generic status received\r\n");
}

/***************************************************************************//**
 * WFX events processing task.
 ******************************************************************************/
static void wfx_events_task(void *p_arg)
{
  RTOS_ERR err;
  OS_MSG_SIZE msg_size;
  sl_wfx_generic_message_t *msg;

  (void)p_arg;

  while (1) {
    msg = (sl_wfx_generic_message_t *)OSQPend(&wifi_events,
                                              0,
                                              OS_OPT_PEND_BLOCKING,
                                              &msg_size,
                                              NULL,
                                              &err);

    if (msg != NULL) {
      switch (msg->header.id) {
        case SL_WFX_CONNECT_IND_ID:
        {
          set_sta_link_up();

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
          if (!(wifi.state & SL_WFX_AP_INTERFACE_UP)) {
            // Enable the WFX power save mode
            // Note: this mode is independent from the host power saving
            //       but has been linked to simplicfy the example.
            sl_wfx_set_power_mode(WFM_PM_MODE_PS, WFM_PM_POLL_FAST_PS, 1);
            sl_wfx_enable_device_power_save();
          }
#endif
          break;
        }
        case SL_WFX_DISCONNECT_IND_ID:
        {
          set_sta_link_down();
          break;
        }
        case SL_WFX_START_AP_IND_ID:
        {
          set_ap_link_up();

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
          // Power save always disabled when SoftAP mode enabled
          sl_wfx_set_power_mode(WFM_PM_MODE_ACTIVE, WFM_PM_POLL_FAST_PS, 0);
          sl_wfx_disable_device_power_save();
#endif
          break;
        }
        case SL_WFX_STOP_AP_IND_ID:
        {
          set_ap_link_down();

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
          if (wifi.state & SL_WFX_STA_INTERFACE_CONNECTED) {
            // Enable the WFX power save mode
            // Note: this mode is independent from the host power saving
            //       but has been linked to simplicfy the example.
            sl_wfx_set_power_mode(WFM_PM_MODE_PS, WFM_PM_POLL_FAST_PS, 1);
            sl_wfx_enable_device_power_save();
          }
#endif
          break;
        }
        case SL_WFX_SCAN_COMPLETE_IND_ID:
        {
          OSSemPost(&scan_sem, OS_OPT_POST_ALL, &err);
          break;
        }
      }

      sl_wfx_host_free_buffer(msg, SL_WFX_RX_FRAME_BUFFER);
    }
  }
}

/***************************************************************************//**
 * Initialize the WFX and create a task processing Wi-Fi events.
 ******************************************************************************/
void app_wifi_events_start(void)
{
  RTOS_ERR err;

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#ifdef SL_CATALOG_WFX_BUS_SDIO_PRESENT
  sl_status_t status;
  status = sl_wfx_host_switch_to_wirq();
  // Check error code.
  APP_RTOS_ASSERT_DBG((status == SL_STATUS_OK), 1);
#endif
#endif

  OSQCreate(&wifi_events, "wifi events", WFX_EVENTS_NB_MAX, &err);
  // Check error code.
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  OSTaskCreate(&wfx_events_task_tcb,
               "WFX events task",
               wfx_events_task,
               DEF_NULL,
               WFX_EVENTS_TASK_PRIO,
               &wfx_events_task_stk[0],
               (WFX_EVENTS_TASK_STK_SIZE / 10u),
               WFX_EVENTS_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  // Check error code.
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}
