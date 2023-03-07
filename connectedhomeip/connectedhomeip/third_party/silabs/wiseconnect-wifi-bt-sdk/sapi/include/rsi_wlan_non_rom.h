/*******************************************************************************
* @file  rsi_wlan_non_rom.h
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

#ifndef RSI_WLAN_NON_ROM_H
#define RSI_WLAN_NON_ROM_H
#include "rsi_socket.h"
#include "rsi_os.h"

#include "rsi_scheduler.h"

// Max chunk size
#define CHUNK_SIZE 1472

#define MAX_RX_DATA_LENGTH (1460 * 3)
#define RX_DATA_LENGTH     16384
// Assertion value register address
#define RSI_ASSERT_VAL_RD_REG   0x41050034
#define RSI_ASSERT_INTR_CLR_REG 0x41050008

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

typedef enum {
  RSI_WLAN_RADIO_DEREGISTER = 1,
  RSI_WLAN_RADIO_REGISTER,
} wlan_radio;

#define RSI_SSL_HEADER_SIZE 90

#define RSI_MAX_NUM_CALLBACKS 19

#ifdef CHIP_9117
#define MAX_SIZE_OF_EXTENSION_DATA 256
#else
#define MAX_SIZE_OF_EXTENSION_DATA 64
#endif

typedef struct rsi_socket_info_non_rom_s {
  uint8_t max_tcp_retries;

  uint8_t vap_id;

  // TCP keepalive initial timeout
  uint8_t tcp_keepalive_initial_time[2];

  // Asynchronous accpet respone handler
  void (*accept_call_back_handler)(int32_t sock_id, int16_t dest_port, uint8_t *ip_addr, int16_t ip_version);
  // Asynchronous data tx done call back handler
  void (*rsi_sock_data_tx_done_cb)(int32_t sockID, int16_t status, uint16_t total_data_sent);

  // total data to be send
  int8_t *buffer;
  // total data sent
  int32_t offset;
  // remaining data to be send
  int32_t rem_len;
  // flag which indicates more data is present
  uint32_t more_data;
  // user flags
  int32_t flags;

  // ssl version select bit map
#ifdef CHIP_9117
  uint32_t ssl_bitmap;
#else
  uint8_t ssl_bitmap;
#endif

  // High performance socket
  uint8_t high_performance_socket;

  uint8_t socket_wait_bitmap;
  uint8_t recv_pending_bit;
  // tcp retry transmission timer
  uint8_t tcp_retry_transmit_timer;
  // TCP MSS
  uint16_t tcp_mss;

  rsi_semaphore_handle_t socket_sem;
  rsi_semaphore_handle_t sock_recv_sem;

  rsi_semaphore_handle_t sock_send_sem;
  uint8_t socket_terminate_indication;

  int32_t socket_status;
  // 2 bytes, Remote peer MSS size
  uint16_t mss;
  uint8_t wait_to_connect;
  uint8_t sock_non_block;
  uint16_t no_of_tls_extensions;
  uint16_t extension_offset;
  uint8_t tls_extension_data[MAX_SIZE_OF_EXTENSION_DATA];
  uint32_t window_size;
#ifdef SOCKET_CLOSE_WAIT
  uint8_t close_pending;
#endif
#ifdef RSI_PROCESS_MAX_RX_DATA
  uint8_t more_rx_data_pending;
#endif

#ifdef CHIP_9117
  uint16_t tos;
#else
  uint32_t tos;
#endif
#ifdef CHIP_9117
  //! max retransmission timeout value.
  uint8_t max_retransmission_timeout_value;
#endif
} rsi_socket_info_non_rom_t;

typedef struct rsi_tls_tlv_s {
  uint16_t type;
  uint16_t length;
} rsi_tls_tlv_t;
typedef struct rsi_req_tcp_window_update_s {
  // Socket ID
  uint32_t socket_id;

  // window size
  uint32_t window_size;

} rsi_req_tcp_window_update_t;
typedef struct rsi_rsp_tcp_window_update_s {
  // Socket ID
  uint32_t socket_id;

  // window size
  uint32_t window_size;

} rsi_rsp_tcp_window_update_t;

typedef struct rsi_max_available_rx_window_s {
  // Maximum TCP Rx window available
  uint32_t max_window_size;

  //window reset
  uint32_t window_reset;
} rsi_max_available_rx_window_t;

// call backs  structure
typedef struct rsi_callback_cb_s {
  // events to be registered explicitly: and has an event id

  // join fail handler pointer
  void (*join_fail_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // ip renewal fail handler pointer
  void (*ip_renewal_fail_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // remote socket terminate handler pointer
  void (*remote_socket_terminate_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // ip change notify handler pointer
  void (*ip_change_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // stations connect notify handler pointer
  void (*stations_connect_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // stations disconnect notify handler pointer
  void (*stations_disconnect_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // Raw data receive handler pointer
  void (*wlan_data_receive_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // events which are registered in APIS

  // Receive stats response handler pointer
  void (*wlan_receive_stats_response_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // Wifi direct device discovery noitfy handler pointer
  void (*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // wifi direct connection request notify handler pointer
  void (*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // async scan response handler pointer
  void (*wlan_scan_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length);

  // async join response handler pointer
  void (*wlan_join_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length);

  // async ping response handler pointer
  void (*wlan_ping_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length);

  //raw data response handler pointer
  void (*raw_data_receive_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);
  void (*auto_config_rsp_handler)(uint16_t status, uint8_t state);

  void (*socket_connect_response_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);
  void (*certificate_response_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);
  void (*wlan_async_module_state)(uint16_t status, uint8_t *buffer, const uint32_t length);

  void (*flash_write_response_handler)(uint16_t status);

  //Assertion response handler pointer
  void (*rsi_assertion_cb)(uint16_t assert_val, uint8_t *buffer, const uint32_t length);

  // Wlan tcp window update handler
  void (*rsi_max_available_rx_window)(uint16_t status, uint8_t *buffer, const uint32_t length);

  //TWT status response handler
  void (*twt_response_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);
} rsi_callback_cb_t;

// enumerations for call back types
typedef enum rsi_callback_id_e {
  RSI_JOIN_FAIL_CB                          = 0,
  RSI_IP_FAIL_CB                            = 1,
  RSI_REMOTE_SOCKET_TERMINATE_CB            = 2,
  RSI_IP_CHANGE_NOTIFY_CB                   = 3,
  RSI_STATIONS_DISCONNECT_NOTIFY_CB         = 4,
  RSI_STATIONS_CONNECT_NOTIFY_CB            = 5,
  RSI_WLAN_DATA_RECEIVE_NOTIFY_CB           = 6,
  RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB          = 7,
  RSI_WLAN_RECEIVE_STATS_RESPONSE_CB        = 8,
  RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB = 9,
  RSI_WLAN_SCAN_RESPONSE_HANDLER            = 10,
  RSI_WLAN_JOIN_RESPONSE_HANDLER            = 11,
  RSI_WLAN_RAW_DATA_RECEIVE_HANDLER         = 12,
  RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB         = 13,
  RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB    = 14,
  RSI_WLAN_ASYNC_STATS                      = 15,
  RSI_FLASH_WRITE_RESPONSE                  = 16,
  RSI_WLAN_ASSERT_NOTIFY_CB                 = 17,
  RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB         = 18,
  RSI_WLAN_TWT_RESPONSE_CB                  = 19
} rsi_callback_id_t;
typedef struct rsi_nwk_callback_s {

  void (*nwk_error_call_back_handler)(uint8_t command_type,
                                      uint32_t status,
                                      const uint8_t *buffer,
                                      const uint32_t length);

  // http response notify handler pointer
#if RSI_HTTP_STATUS_INDICATION_EN
  void (*http_client_response_handler)(uint16_t status,
                                       const uint8_t *buffer,
                                       const uint16_t length,
                                       const uint32_t more_data,
                                       uint16_t status_code);
#else
  void (*http_client_response_handler)(uint16_t status,
                                       const uint8_t *buffer,
                                       const uint16_t length,
                                       const uint32_t more_data);
#endif
  // HTTP OTAF response notify handler pointer
  void (*http_otaf_response_handler)(uint16_t status, const uint8_t *buffer);

  // sntp create response notify handler pointer
  void (*rsi_sntp_client_create_response_handler)(uint16_t status, const uint8_t cmd_type, const uint8_t *buffer);

  // smtp mail sent response notify handler pointer
  void (*smtp_client_mail_response_handler)(uint16_t status, const uint8_t cmd_type);

  // smtp client delete response notify handler pointer
  void (*smtp_client_delete_response_handler)(uint16_t status, const uint8_t cmd_type);

  // FTP client directory list
  void (*ftp_directory_list_call_back_handler)(uint16_t status,
                                               uint8_t *directory_list,
                                               uint16_t length,
                                               uint8_t end_of_list);

  // FTP client file read callback handler
  void (*ftp_file_read_call_back_handler)(uint16_t status,
                                          uint8_t *file_content,
                                          uint16_t content_length,
                                          uint8_t end_of_file);
  // Callback for data transfer complete notification
  void (*data_transfer_complete_handler)(int32_t sockID, uint16_t length);

  // webpage request handler
  void (*rsi_webpage_request_handler)(uint8_t type,
                                      uint8_t *url_name,
                                      uint8_t *post_content_buffer,
                                      uint32_t post_content_length,
                                      uint32_t status);

  // Json object upadte response handler
  void (*rsi_json_object_update_handler)(uint8_t *file_name, uint8_t *json_object, uint32_t length, uint32_t status);

  // wireless firmware upgrade handler
  void (*rsi_wireless_fw_upgrade_handler)(uint8_t type, uint32_t status);

  // Json object event response handler
  void (*rsi_json_object_event_handler)(uint32_t status, uint8_t *json_object_str, uint32_t length);

  // POP3 client response notify handler pointer
  void (*rsi_pop3_client_mail_response_handler)(uint16_t status, uint8_t cmd_type, const uint8_t *buffer);

  // HTTP CLIENT PUT response notify handler pointer
  void (*rsi_http_client_put_response_handler)(uint16_t status,
                                               uint8_t cmd_type,
                                               const uint8_t *buffer,
                                               uint16_t length,
                                               const uint8_t end_of_put_pkt);

  // HTTP CLIENT POST data response notify handler pointer
#if RSI_HTTP_STATUS_INDICATION_EN
  void (*rsi_http_client_post_data_response_handler)(uint16_t status,
                                                     const uint8_t *buffer,
                                                     const uint16_t length,
                                                     const uint32_t more_data,
                                                     uint16_t status_code);
#else
  void (*rsi_http_client_post_data_response_handler)(uint16_t status,
                                                     const uint8_t *buffer,
                                                     const uint16_t length,
                                                     const uint32_t more_data);
#endif

  // HTTP CLIENT POST data response notify handler pointer
  void (*rsi_ota_fw_up_response_handler)(uint16_t status, uint16_t chunk_number);

  // DNS client hostname update response notify handler pointer
  void (*rsi_dns_update_rsp_handler)(uint16_t status);

  // DHCP USER CLASS response notify handler pointer
  void (*rsi_dhcp_usr_cls_rsp_handler)(uint16_t status);

  // Remote terminate of Embedded mqtt socket call back handler
  void (*rsi_emb_mqtt_remote_terminate_handler)(uint16_t status, uint8_t *buffer, const uint32_t length);

  //  MQTT Call back for publish message
  void (*rsi_emb_mqtt_publish_message_callback)(uint16_t status, uint8_t *buffer, const uint32_t length);

  // MQTT  keep alive timeout callback handler
  void (*rsi_emb_mqtt_keep_alive_timeout_callback)(uint16_t status, uint8_t *buffer, const uint32_t length);

} rsi_nwk_callback_t;

// enumerations for nwk call back types
typedef enum rsi_nwk_callback_id_e {
  RSI_NWK_ERROR_CB                           = 0,
  RSI_WLAN_NWK_URL_REQ_CB                    = 1,
  RSI_WLAN_NWK_JSON_UPDATE_CB                = 2,
  RSI_WLAN_NWK_FW_UPGRADE_CB                 = 3,
  RSI_WLAN_NWK_JSON_EVENT_CB                 = 4,
  RSI_WLAN_NWK_EMB_MQTT_REMOTE_TERMINATE_CB  = 5,
  RSI_WLAN_NWK_EMB_MQTT_PUB_MSG_CB           = 6,
  RSI_WLAN_NWK_EMB_MQTT_KEEPALIVE_TIMEOUT_CB = 7
} rsi_nwk_callback_id_t;

// wlan radio req structure
typedef struct rsi_wlan_req_radio_s {
  uint8_t radio_req;
} rsi_wlan_req_radio_t;

/*==================================================*/
// Asynchronous message for connection state notification
typedef struct rsi_state_notification_s {
  uint8_t TimeStamp[4];
  uint8_t StateCode;
  uint8_t reason_code;
  uint8_t rsi_channel;
  uint8_t rsi_rssi;
  uint8_t rsi_bssid[6];
} rsi_state_notification_t;

/*==================================================*/
#define WLAN_MODULE_STATES 11
#define WLAN_REASON_CODES  6
struct rsi_bit_2_string {
  uint8_t bit;
  char *string;
};
static const struct rsi_bit_2_string STATE[WLAN_MODULE_STATES] = {
  { 0x10, "Beacon Loss (Failover Roam)" },
  { 0x20, "De-authentication (AP induced Roam / Disconnect from supplicant" },
  { 0x50, "Current AP is best" },
  { 0x60, "Better AP found" },
  { 0x70, "No AP found" },
  { 0x80, "Associated" },
  { 0x90, "Unassociated" },
  { 0x01, "Authentication denial" },
  { 0x02, "Association denial" },
  { 0x03, "AP not present" },
  { 0x05, "WPA2 key exchange failed" }

};

static const struct rsi_bit_2_string REASONCODE[WLAN_REASON_CODES] = {
  { 0x01, "Authentication denial" },       { 0x02, "Association denial" },
  { 0x10, "Beacon Loss (Failover Roam)" }, { 0x20, "De-authentication (AP induced Roam/Deauth from supplicant)" },
  { 0x07, "PSK not configured" },          { 0x05, "Roaming not enabled" },

};

int32_t rsi_socket_create_async(int32_t sockID, int32_t type, int32_t backlog);
int32_t rsi_get_socket_descriptor(uint8_t *src_port,
                                  uint8_t *dst_port,
                                  uint8_t *ip_addr,
                                  uint16_t ip_version,
                                  uint16_t sockid);
int32_t rsi_socket_connect(int32_t sockID, struct rsi_sockaddr *remoteAddress, int32_t addressLength);
int32_t rsi_socket_recvfrom(int32_t sockID,
                            int8_t *buffer,
                            int32_t buffersize,
                            int32_t flags,
                            struct rsi_sockaddr *fromAddr,
                            int32_t *fromAddrLen);
rsi_error_t rsi_wait_on_socket_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
int32_t rsi_accept_non_rom(int32_t sockID, struct rsi_sockaddr *ClientAddress, int32_t *addressLength);
int32_t rsi_socket_async_non_rom(int32_t protocolFamily,
                                 int32_t type,
                                 int32_t protocol,
                                 void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length));
int32_t rsi_socket_listen(int32_t sockID, int32_t backlog);
int32_t rsi_socket_shutdown(int32_t sockID, int32_t how);
int32_t rsi_select_get_status(int32_t selectid);
void rsi_select_set_status(int32_t status, int32_t selectid);
int32_t rsi_socket_bind(int32_t sockID, struct rsi_sockaddr *localAddress, int32_t addressLength);
void rsi_clear_sockets_non_rom(int32_t sockID);
int32_t rsi_sendto_async_non_rom(int32_t sockID,
                                 int8_t *msg,
                                 int32_t msgLength,
                                 int32_t flags,
                                 struct rsi_sockaddr *destAddr,
                                 int32_t destAddrLen,
                                 void (*data_transfer_complete_handler)(int32_t sockID, uint16_t length));
int32_t rsi_send_async_non_rom(int32_t sockID,
                               const int8_t *msg,
                               int32_t msgLength,
                               int32_t flags,
                               void (*data_transfer_complete_handler)(int32_t sockID, uint16_t length));
void rsi_wlan_socket_set_status(int32_t status, int32_t sockID);
int32_t rsi_wlan_socket_get_status(int32_t sockID);
int32_t rsi_get_socket_id(uint32_t src_port, uint32_t dst_port);
int32_t rsi_application_socket_descriptor(int32_t sock_id);
void rsi_post_waiting_socket_semaphore(int32_t sockID);
int32_t rsi_wlan_req_radio(uint8_t enable);
int32_t rsi_wlan_radio_deinit(void);
int32_t rsi_send_large_data_sync(int32_t sockID, const int8_t *msg, int32_t msgLength, int32_t flags);
uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb);
#ifdef RSI_PROCESS_MAX_RX_DATA
int32_t rsi_recv_large_data_sync(int32_t sockID,
                                 int8_t *buffer,
                                 int32_t buffersize,
                                 int32_t flags,
                                 struct rsi_sockaddr *fromAddr,
                                 int32_t *fromAddrLen);
#endif
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
void rsi_scheduler_non_rom(rsi_scheduler_cb_t *scheduler_cb);
void rsi_set_event_non_rom(uint32_t event_num);
void rsi_set_event_from_isr_non_rom(uint32_t event_num);
void rsi_unmask_event_from_isr_non_rom(uint32_t event_num);
void rsi_unmask_event_non_rom(uint32_t event_num);
#endif
int rsi_getsockopt(int32_t sockID, int level, int option_name, const void *option_value, rsi_socklen_t option_len);
int32_t rsi_tcp_window_update(uint32_t sockID, uint32_t new_size_bytes);
typedef enum {
  RSI_SOCKET_SELECT_STATE_INIT = 0,
  RSI_SOCKET_SELECT_STATE_CREATE,
} select_state;

// driver WLAN control block
typedef struct rsi_wlan_cb_non_rom_s {
  uint32_t tls_version;
  uint8_t join_bssid_non_rom[6];
  // to identify on which socket data is present
  uint32_t socket_bitmap;
#ifdef RSI_UART_INTERFACE
  uint8_t rsi_uart_data_ack_check;
#endif
  void (*switch_proto_callback)(uint16_t mode, uint8_t *bt_disabled_status);

  uint16_t read_data;

  // network callbacks
  rsi_nwk_callback_t nwk_callbacks;

  // application call back  list structure
  rsi_callback_cb_t callback_list;

  uint8_t wlan_radio;
  uint16_t ps_listen_interval;

  uint8_t emb_mqtt_ssl_enable;

  //! scan results to host
  uint8_t scan_results_to_host;

  //! socket connection request pending
  uint32_t socket_cmd_rsp_pending;
  //! network command request pending
  uint32_t nwk_cmd_rsp_pending;
} rsi_wlan_cb_non_rom_t;

#define RSI_AP_NOT_FOUND 0x3

// Network command Request pending bitmaps
#define PING_RESPONSE_PENDING BIT(0)
#define DNS_RESPONSE_PENDING  BIT(1)
#define SNTP_RESPONSE_PENDING BIT(2)
/*===================================================*/
/**
 * RTC time from host
 */

typedef struct module_rtc_time_s {
  uint8_t tm_sec[4];  //@ seconds [0-59]
  uint8_t tm_min[4];  //@ minutes [0-59]
  uint8_t tm_hour[4]; //@ hours since midnight [0-23]
  uint8_t tm_mday[4]; //@ day of the month [1-31]
  uint8_t tm_mon[4];  //@ months since January [0-11]
  uint8_t tm_year[4]; //@ year since 1990
  uint8_t tm_wday[4]; //@ Weekday from Sunday to Saturday [1-7]
} module_rtc_time_t;

typedef struct rsi_rsp_waiting_cmds_s {
  uint16_t waiting_cmds;
  uint8_t rx_driver_flag;
} rsi_rsp_waiting_cmds_t;

// socket select request structure
typedef struct rsi_socket_select_info_s {
  // read fds
  rsi_fd_set *rsi_sel_read_fds;

  // writes fds
  rsi_fd_set *rsi_sel_write_fds;

  //expection fds
  rsi_fd_set *rsi_sel_exception_fds;

  // read fds
  rsi_fd_set rsi_sel_read_fds_var;

  // writes fds
  rsi_fd_set rsi_sel_write_fds_var;

  //expection fds
  rsi_fd_set rsi_sel_exception_fds_var;

  // driver select control block semaphore
  rsi_semaphore_handle_t select_sem;

  int32_t select_status;

  uint8_t select_state;

  uint8_t select_id;

  void (*sock_select_callback)(rsi_fd_set *fd_read, rsi_fd_set *fd_write, rsi_fd_set *fd_except, int32_t status);

} rsi_socket_select_info_t;
// 11ax params

//TWT STATUS CODES
#define TWT_SESSION_SUCC                   0
#define TWT_UNSOL_SESSION_SUCC             1
#define TWT_SETUP_AP_REJECTED              4
#define TWT_SETUP_RSP_OUTOF_TOL            5
#define TWT_SETUP_RSP_NOT_MATCHED          6
#define TWT_SETUP_UNSUPPORTED_RSP          10
#define TWT_TEARDOWN_SUCC                  11
#define TWT_AP_TEARDOWN_SUCC               12
#define TWT_SETUP_FAIL_MAX_RETRIES_REACHED 15
#define TWT_INACTIVE_DUETO_ROAMING         16
#define TWT_INACTIVE_DUETO_DISCONNECT      17
#define TWT_INACTIVE_NO_AP_SUPPORT         18

typedef struct wlan_11ax_config_params_s {
  uint8_t guard_interval;
  uint8_t nominal_pe;
  uint8_t dcm_enable;
  uint8_t ldpc_enable;
  uint8_t ng_cb_enable;
  uint8_t ng_cb_values;
  uint8_t uora_enable;
  uint8_t trigger_rsp_ind;
  uint8_t ipps_valid_value;
  uint8_t tx_only_on_ap_trig;
  uint8_t twt_support;
} wlan_11ax_config_params_t;
#endif
