/*******************************************************************************
* @file  rsi_nwk.h
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

#ifndef RSI_NWK_H
#define RSI_NWK_H
#include "rsi_os.h"
#include "rsi_pkt_mgmt.h"
// common includes

/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef RSI_IPV6
// flag bit to set IPV6
#define RSI_IPV6 BIT(0)
#endif
// flags bit to enable SSL
#define RSI_SSL_ENABLE BIT(1)

// flag bit to enable JSON associated to the webpage
#define RSI_WEB_PAGE_JSON_ENABLE BIT(3)

// HTTP client post big data support
#define RSI_SUPPORT_HTTP_POST_DATA BIT(5)

// HTTP version 1.1 support feature bitmap
#define RSI_SUPPORT_HTTP_V_1_1             BIT(6)
#define RSI_HTTP_USER_DEFINED_CONTENT_TYPE BIT(7)

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

int16_t rsi_nwk_register_callbacks(
  uint32_t callback_id,
  void (*callback_handler_ptr)(uint8_t command_type, uint32_t status, const uint8_t *buffer, const uint32_t length));
int16_t rsi_wlan_nwk_register_json_update_cb(
  uint32_t callback_id,
  void (*callback_handler_ptr)(uint8_t *filename, uint8_t *json_object, uint32_t length, uint32_t status));
int16_t rsi_wlan_nwk_register_webpage_req_cb(uint32_t callback_id,
                                             void (*callback_handler_ptr)(uint8_t type,
                                                                          uint8_t *url_name,
                                                                          uint8_t *post_content_buffer,
                                                                          uint32_t post_content_length,
                                                                          uint32_t status));
int16_t rsi_wlan_nwk_register_wireless_fw_upgrade_cb(uint32_t callback_id,
                                                     void (*callback_handler_ptr)(uint8_t type, uint32_t status));
int16_t rsi_wlan_nwk_register_json_event_cb(uint32_t callback_id,
                                            void (*callback_handler_ptr)(uint32_t status,
                                                                         uint8_t *json_object,
                                                                         uint32_t length));

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

// HTTP CLIENT application includes

/******************************************************
 * *                      Macros
 * ******************************************************/
// HTTP GET
#define RSI_HTTP_GET 0

// HTTP POST
#define RSI_HTTP_POST 1

// HTTP OTAF
#define RSI_HTTP_OTAF 2

// HTTP OFFSET
#define RSI_HTTP_OFFSET 12

// HTTP PUT OFFSET
#define RSI_HTTP_PUT_OFFSET 16

// Buffer length
#define RSI_HTTP_BUFFER_LEN 1200

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// HTTP Get/Post request structure
typedef struct rsi_req_http_client_s {

  // ip version
  uint8_t ip_version[2];

  // https enable
  uint8_t https_enable[2];

  // port number
  uint16_t port;

  // buffer
  uint8_t buffer[RSI_HTTP_BUFFER_LEN];

} rsi_req_http_client_t;

/******************************************************************/

// HTTP credentials structure
#define MAX_USERNAME_LENGTH 31
#define MAX_PASSWORD_LENGTH 31
typedef struct rsi_req_http_credentials_s {
  //username
  uint8_t username[MAX_USERNAME_LENGTH];

  //password
  uint8_t password[MAX_PASSWORD_LENGTH];

} rsi_req_http_credentials_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#if RSI_HTTP_STATUS_INDICATION_EN
int32_t rsi_http_client_async(uint8_t type,
                              uint8_t flags,
                              uint8_t *ip_address,
                              uint16_t port,
                              uint8_t *resource,
                              uint8_t *host_name,
                              uint8_t *extended_header,
                              uint8_t *user_name,
                              uint8_t *password,
                              uint8_t *post_data,
                              uint32_t post_data_length,
                              void (*callback)(uint16_t status,
                                               const uint8_t *buffer,
                                               const uint16_t length,
                                               uint32_t moredata,
                                               uint16_t status_code));
#else
int32_t rsi_http_client_async(
  uint8_t type,
  uint8_t flags,
  uint8_t *ip_address,
  uint16_t port,
  uint8_t *resource,
  uint8_t *host_name,
  uint8_t *extended_header,
  uint8_t *user_name,
  uint8_t *password,
  uint8_t *post_data,
  uint32_t post_data_length,
  void (*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length, uint32_t moredata));
#endif
int32_t rsi_http_otaf_async(uint8_t type,
                            uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t port,
                            uint8_t *resource,
                            uint8_t *host_name,
                            uint8_t *extended_header,
                            uint8_t *user_name,
                            uint8_t *password,
                            uint8_t *post_data,
                            uint32_t post_data_length,
                            void (*callback)(uint16_t status, const uint8_t *buffer));

// USER CLASS Application include
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_DHCP_USER_CLASS_MAX_COUNT    2
#define RSI_DHCP_USER_CLASS_DATA_MAX_LEN 64

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                    Structures
*******************************************************/

typedef struct user_class_data_s {
  uint8_t length;
  uint8_t data[RSI_DHCP_USER_CLASS_DATA_MAX_LEN];
} user_class_data_t;
typedef struct rsi_dhcp_user_class_s {
  // DHCP user class mode
  uint8_t mode;

  // User class list count
  uint8_t count;

  // User class data
  user_class_data_t user_class_data[RSI_DHCP_USER_CLASS_MAX_COUNT];

} rsi_dhcp_user_class_t;

// DNS Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

#define MAX_URL_LEN     90
#define MAX_DNS_REPLIES 10

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                    Structures
*******************************************************/

// DNS Server add request structure
typedef struct rsi_req_dns_server_add_s {
  // Ip version value
  uint8_t ip_version[2];

  // Dns mode to use
  uint8_t dns_mode[2];

  // Primary DNS addresss
  union {
    uint8_t primary_dns_ipv4[4];
    uint8_t primary_dns_ipv6[16];
  } ip_address1;

  // Seconadary DNS address
  union {
    uint8_t secondary_dns_ipv4[4];
    uint8_t secondary_dns_ipv6[16];
  } ip_address2;
} rsi_req_dns_server_add_t;

//  DNS Query request structure

typedef struct rsi_req_dns_query_s {
  // Ip version value
  uint8_t ip_version[2];

  // URL name
  uint8_t url_name[MAX_URL_LEN];

  // DNS servers count
  uint8_t dns_server_number[2];

} rsi_req_dns_query_t;

#define MAX_ZONE_NAME_LEN 31
#define MAX_HOSTNAME_LEN  31

//  DNS Update request structure
typedef struct rsi_req_dns_update_s {
  // Ip version value
  uint8_t ip_version;

  // Zone name
  uint8_t zone_name[MAX_ZONE_NAME_LEN];

  // Host name
  uint8_t host_name[MAX_HOSTNAME_LEN];

  // DNS servers count
  uint8_t dns_server_number[2];

  // DNS Hostname TTL
  uint8_t ttl[2];

} rsi_req_dns_update_t;

// DNS Server add response structure
typedef struct rsi_rsp_dns_server_add_s {
  // Primary DNS srever address
  union {
    uint8_t primary_dns_ipv4_addr[4];
    uint8_t primary_dns_ipv6_addr[16];
  } ip_address1;

  // Secondary DNS srever address
  union {
    uint8_t secondary_dns_ipv4_addr[4];
    uint8_t secondary_dns_ipv6_addr[16];
  } ip_address2;
} rsi_rsp_dns_server_add_t;

// DNS Query response structure

typedef struct rsi_rsp_dns_query_s {
  // Ip version of the DNS server
  uint8_t ip_version[2];

  // DNS response count
  uint8_t ip_count[2];

  // DNS address responses
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } ip_address[MAX_DNS_REPLIES];
} rsi_rsp_dns_query_t;

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_dns_req(uint8_t ip_version,
                    uint8_t *url_name,
                    uint8_t *primary_server_address,
                    uint8_t *secondary_server_address,
                    rsi_rsp_dns_query_t *dns_query_resp,
                    uint16_t length);
int32_t rsi_dns_update(uint8_t ip_version,
                       uint8_t *zone_name,
                       uint8_t *host_name,
                       uint8_t *server_address,
                       uint16_t ttl,
                       void (*dns_update_rsp_handler)(uint16_t status));

// FTP Client feature related prototypes
/******************************************************
 * *                      Macros
 * ******************************************************/

// Max length of FTP server login username
#define RSI_FTP_USERNAME_LENGTH 31
// Max length of FTP server login password
#define RSI_FTP_PASSWORD_LENGTH 31

// Max length of FTP file path
#define RSI_FTP_PATH_LENGTH 51
// Max chunk length in a command
#define RSI_FTP_MAX_CHUNK_LENGTH 1400

// FTP client modes
#define FTP_ENABLE_PASSIVE 1
#define FTP_ENABLE_ACTIVE  0

// FTP TRANSFER MODES
#define FTP_BLOCK_TRANSFER_MODE  1
#define FTP_STREAM_TRANSFER_MODE 0

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
// FTP client commands
typedef enum rsi_ftp_commands_e {
  // Creates FTP objects. This should be the first command for accessing FTP
  RSI_FTP_CREATE = 1,

  // Command to Connect to FTP server.
  RSI_FTP_CONNECT,

  // Command to create directory in a specified path.
  RSI_FTP_DIRECTORY_CREATE,

  // Command to delete directory in a specified path
  RSI_FTP_DIRECTORY_DELETE,

  // Command to change working directory to a specified path.
  RSI_FTP_DIRECTORY_SET,

  // Command to list directory contents in a specified path
  RSI_FTP_DIRECTORY_LIST,

  // Command to read the file
  RSI_FTP_FILE_READ,

  // Command to write the file
  RSI_FTP_FILE_WRITE,

  // Command to write the content in file already opened
  RSI_FTP_FILE_WRITE_CONTENT,

  // Command to delete the specified file
  RSI_FTP_FILE_DELETE,

  // Command to Rename the specified file
  RSI_FTP_FILE_RENAME,

  // Disconnects from FTP server.
  RSI_FTP_DISCONNECT,

  // Destroys FTP objects
  RSI_FTP_DESTROY,

  // Command to enable FTP client passive mode
  RSI_FTP_COMMAND_MODE_SET,

  // Command to enable FTP client active mode
  RSI_FTP_COMMAND_FILE_SIZE_SET
} rsi_ftp_commands_t;

/******************************************************
 * *                    Structures
*******************************************************/

// FTP connect request structure
typedef struct rsi_ftp_connect_s {
  // Command type
  uint8_t command_type;

  // FTP client IP version
  uint8_t ip_version;

  union {
    // IPv4 address
    uint8_t ipv4_address[4];

    // IPv6 address
    uint8_t ipv6_address[16];
  } server_ip_address;

  // FTP client username
  uint8_t username[RSI_FTP_USERNAME_LENGTH];

  // FTP client password
  uint8_t password[RSI_FTP_PASSWORD_LENGTH];

  // FTP server port number
  uint8_t server_port[4];

} rsi_ftp_connect_t;

// FTP file rename request structure
typedef struct rsi_ftp_file_rename_s {

  // Command type
  uint8_t command_type;

  // Directory or file path
  uint8_t old_file_name[RSI_FTP_PATH_LENGTH];

  // New file name
  uint8_t new_file_name[RSI_FTP_PATH_LENGTH];

} rsi_ftp_file_rename_t;

typedef struct rsi_ftp_mode_params_s {

  // Command type
  uint8_t command_type;
  uint8_t mode_type;

} rsi_ftp_mode_params_t;

typedef struct rsi_ftp_file_size_set_params_s {

  // Command type
  uint8_t command_type;
  uint8_t file_size[4];
} rsi_ftp_file_size_set_params_t;
// FTP file operations request structure
typedef struct rsi_ftp_file_ops_s {

  // Command type
  uint8_t command_type;

  // Directory or file path
  uint8_t file_name[RSI_FTP_PATH_LENGTH];

} rsi_ftp_file_ops_t;

// FTP file write request structure
typedef struct rsi_ftp_file_write_s {
  // command type
  uint8_t command_type;

  // End of file
  uint8_t end_of_file;

  // Path of file to write
  uint8_t file_content[RSI_FTP_MAX_CHUNK_LENGTH];

} rsi_ftp_file_write_t;

// FTP file operations response structure
typedef struct rsi_ftp_file_rsp_s {
  // command type
  uint8_t command_type;

  // Data pending
  uint8_t more;

  // data length
  uint16_t data_length;

  // Data content
  uint8_t data_content[1024];

} rsi_ftp_file_rsp_t;

/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_ftp_connect(uint16_t flags, int8_t *server_ip, int8_t *username, int8_t *password, uint32_t server_port);
int32_t rsi_ftp_disconnect(void);
int32_t rsi_ftp_file_write(int8_t *file_name);
int32_t rsi_ftp_file_size_set(uint32_t file_size);
int32_t rsi_ftp_file_write_content(uint16_t flags, int8_t *file_content, int16_t content_length, uint8_t end_of_file);

int32_t rsi_ftp_file_read_aysnc(
  int8_t *file_name,
  void (*call_back_handler_ptr)(uint16_t status, uint8_t *file_content, uint16_t content_length, uint8_t end_of_file));
int32_t rsi_ftp_file_delete(int8_t *file_name);
int32_t rsi_ftp_file_rename(int8_t *old_file_name, int8_t *new_file_name);
int32_t rsi_ftp_directory_create(int8_t *directory_name);
int32_t rsi_ftp_directory_delete(int8_t *directory_name);
int32_t rsi_ftp_directory_set(int8_t *directory_path);
int32_t rsi_ftp_directory_list_async(
  int8_t *directory_path,
  void (*call_back_handler_ptr)(uint16_t status, uint8_t *directory_list, uint16_t length, uint8_t end_of_list));

// SNTP client Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/
//SNTP client feature
#define RSI_SNTP_CREATE            1
#define RSI_SNTP_GETTIME           2
#define RSI_SNTP_GETTIME_DATE      3
#define RSI_SNTP_GETSERVER_ADDRESS 4
#define RSI_SNTP_DELETE            5
#define RSI_SNTP_GET_SERVER_INFO   6
#define RSI_SNTP_SERVER_ASYNC_RSP  7

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// Define for SNTP client initialization

typedef struct rsi_sntp_client {
  //SNTP client command type
  uint8_t command_type;
  //SNTP server ip version
  uint8_t ip_version;
  union {
    //server ipv4 address
    uint8_t ipv4_address[4];
    //server ipv6 address
    uint8_t ipv6_address[16];
  } server_ip_address;
  // SNTP server method
  uint8_t sntp_method;
  uint8_t sntp_timeout[2];
} rsi_sntp_client_t;

typedef struct rsi_sntp_server_info_rsp_t {
  uint8_t command_type;
  uint8_t ip_version;

  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];

  } server_ip_address;

  uint8_t sntp_method;

} rsi_sntp_server_info_rsp_t;

typedef struct rsi_sntp_server_rsp_t {
  uint8_t ip_version;
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
  uint8_t sntp_method;
} rsi_sntp_server_rsp_t;

// SMTP client Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

// SMTP feature

#define RSI_SMTP_BUFFER_LENGTH    1024
#define RSI_SMTP_CLIENT_CREATE    1
#define RSI_SMTP_CLIENT_INIT      2
#define RSI_SMTP_CLIENT_MAIL_SEND 3
#define RSI_SMTP_CLIENT_DEINIT    4

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// Define for SMTP client initialization

typedef struct rsi_smtp_client_init_s {
  // SMTP server ip version
  uint8_t ip_version;

  union {
    // Server ipv4 address
    uint8_t ipv4_address[4];

    // Server ipv6 address
    uint8_t ipv6_address[16];

  } server_ip_address;

  // SMTP server authentication type
  uint8_t auth_type;

  // SMTP server port number
  uint8_t server_port[4];

} rsi_smtp_client_init_t;

// Define for SMTP client mail send
typedef struct rsi_smtp_mail_send_s {
  // SMTP mail priority level
  uint8_t smtp_feature;

  // SMTP mail body length
  uint8_t mail_body_length[2];

} rsi_smtp_mail_send_t;

// Define SMTP client structure
typedef struct rsi_req_smtp_client_s {
  // SMTP client command type
  uint8_t command_type;

  // SMTP client command structure
  union {
    rsi_smtp_client_init_t smtp_client_init;
    rsi_smtp_mail_send_t smtp_mail_send;

  } smtp_struct;

  uint8_t smtp_buffer[RSI_SMTP_BUFFER_LENGTH];

} rsi_req_smtp_client_t;

// SMTP response structure
typedef struct rsi_rsp_smtp_client_s {
  // Receive SMTP command type
  uint8_t command_type;

} rsi_rsp_smtp_client_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

// multicast related prototypes
/******************************************************
 * *                      Macros
 * ******************************************************/

// Multicast join command
#define RSI_MULTICAST_JOIN 1

// Multicast leave command
#define RSI_MULTICAST_LEAVE 0

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// Multicast request structure
typedef struct rsi_req_multicast {
  // IP version
  uint8_t ip_version[2];

  // command type
  uint8_t type[2];

  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } multicast_address;

} rsi_req_multicast_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

// HTTP server Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_MAX_URL_LENGTH             40
#define RSI_MAX_FILE_NAME_LENGTH       24
#define RSI_MAX_POST_DATA_LENGTH       512
#define RSI_MAX_WEBPAGE_SEND_SIZE      1024
#define RSI_MAX_HOST_WEBPAGE_SEND_SIZE 1400

#define RSI_JSON_MAX_CHUNK_LENGTH    1024
#define RSI_WEB_PAGE_CLEAR_ALL_FILES 1
#define RSI_WEB_PAGE_HAS_JSON        1

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *                    Structures
*******************************************************/

typedef struct {
  uint8_t url_length;
  uint8_t url_name[RSI_MAX_URL_LENGTH];
  uint8_t request_type;
  uint8_t post_content_length[2];
  uint8_t post_data[RSI_MAX_POST_DATA_LENGTH];

} rsi_urlReqFrameRcv;

// Define Webpage load request structure
typedef struct rsi_webpage_load_s {
  // file name of the webpage
  uint8_t filename[RSI_MAX_FILE_NAME_LENGTH];

  // Total length of the webpage
  uint8_t total_len[2];

  // Current length of the webapge
  uint8_t current_len[2];

  // webpage associated json object flag
  uint8_t has_json_data;

  // Webpage content
  uint8_t webpage[RSI_MAX_WEBPAGE_SEND_SIZE];

} rsi_webpage_load_t;

// Define json object load request structure
typedef struct rsi_json_object_load_s {
  // File name of the json object
  uint8_t filename[RSI_MAX_FILE_NAME_LENGTH];

  // Total length of the json object data
  uint8_t total_length[2];

  // Current length of the json object
  uint8_t current_length[2];

  // Json object data
  uint8_t json_object[RSI_JSON_MAX_CHUNK_LENGTH];

} rsi_json_object_load_t;

// Define webpage erase all request structure
typedef struct rsi_webpage_erase_all_s {
  // Flag to clear the file
  uint8_t clear;

} rsi_webpage_erase_all_t;

// Define webpage erase request structure
typedef struct rsi_wbpage_erase_s {
  // Filename of the webpage or json object data
  char filename[RSI_MAX_FILE_NAME_LENGTH];

} rsi_wbpage_erase_t;

// Define json object erase request structure
typedef struct rsi_json_object_erase_s {
  // Filename of the webpage or json object data
  char filename[RSI_MAX_FILE_NAME_LENGTH];

} rsi_json_object_erase_t;

// Host web page send request structure
typedef struct rsi_webpage_send_s {
  // Total length of the webpage
  uint8_t total_len[4];

  // More chunks flag
  uint8_t more_chunks;

  // Webpage content
  uint8_t webpage[RSI_MAX_HOST_WEBPAGE_SEND_SIZE];

} rsi_webpage_send_t;

/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_webpage_load(uint8_t flags, uint8_t *file_name, uint8_t *webpage, uint32_t length);
int32_t rsi_json_object_create(uint8_t flags, const char *file_name, uint8_t *json_object, uint32_t length);
int32_t rsi_webpage_erase(uint8_t *file_name);
int32_t rsi_json_object_delete(uint8_t *file_name);
int32_t rsi_webpage_send(uint8_t flags, uint8_t *webpage, uint32_t length);
void rsi_update_nwk_cmd_state_to_free_state(void);
void rsi_update_nwk_cmd_state_to_progress_state(void);
void rsi_post_waiting_nwk_semaphore(void);
int32_t rsi_driver_process_recv_data_non_rom(rsi_pkt_t *pkt);
int32_t rsi_driver_send_data_non_rom(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr);
int32_t rsi_http_credentials(int8_t *username, int8_t *password);
int32_t rsi_req_wireless_fwup(void);
// MDNS-SD structure definations

/******************************************************
 * *                      Macros
 * ******************************************************/
// MDNSD buffer size
#define MDNSD_BUFFER_SIZE 1000

// MDNSD init
#define RSI_MDNSD_INIT 1

// MDNSD register service
#define RSI_MDNSD_REGISTER_SERVICE 3

// MDNSD deinit
#define RSI_MDNSD_DEINIT 6

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// mdnsd init structure
typedef struct rsi_mdnsd_init_s {
  // ip version
  uint8_t ip_version;

  // time to live
  uint8_t ttl[2];

} rsi_mdnsd_init_t;

// mdnsd register-service structure
typedef struct rsi_mdnsd_reg_srv_s {
  // port number
  uint8_t port[2];

  // time to live
  uint8_t ttl[2];

  // reset if it is last service in the list
  uint8_t more;

} rsi_mdnsd_reg_srv_t;

// mdnsd structure
typedef struct rsi_req_mdnsd_t {
  // command type 1-MDNSD init, 3- Register service, 6- Deinit
  uint8_t command_type;
  union {
    // mdnsd init
    rsi_mdnsd_init_t mdnsd_init;

    // mdnsd register
    rsi_mdnsd_reg_srv_t mdnsd_register_service;

  } mdnsd_struct;

  // buffer
  uint8_t buffer[MDNSD_BUFFER_SIZE];

} rsi_req_mdnsd_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

// POP3 Client Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

#define POP3_CLIENT_MAX_USERNAME_LENGTH 101
#define POP3_CLIENT_MAX_PASSWORD_LENGTH 101

// POP3 client session create command
#define POP3_CLIENT_SESSION_CREATE 1

// POP3 client get mail stats command
#define POP3_CLIENT_GET_MAIL_STATS 2

// POP3 client get mail list command
#define POP3_CLIENT_GET_MAIL_LIST 3

// POP3 client retrive mail command
#define POP3_CLIENT_RETR_MAIL 4

// POP3 client mark(deleted) mail stats command
#define POP3_CLIENT_MARK_MAIL 5

// POP3 client unmark all mails command
#define POP3_CLIENT_UNMARK_MAIL 6

// POP3 client get server status command
#define POP3_CLIENT_GET_SERVER_STATUS 7

// POP3 client session delete command
#define POP3_CLIENT_SESSION_DELETE 8

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

// Define POP3 client session create structure
typedef struct rsi_pop3_client_session_create {
  // POP3 server ip version
  uint8_t ip_version;

  union {
    // Server ipv4 address
    uint8_t ipv4_address[4];

    // Server ipv6 address
    uint8_t ipv6_address[16];

  } server_ip_address;

  // POP3 server port number
  uint8_t server_port_number[2];

  // POP3 client authentication type
  uint8_t auth_type;

  // POP3 client username
  uint8_t username[POP3_CLIENT_MAX_USERNAME_LENGTH];

  // POP3 client password
  uint8_t password[POP3_CLIENT_MAX_PASSWORD_LENGTH];

} rsi_pop3_client_session_create_t;

// Define POP3 client request structure
typedef struct rsi_req_pop3_client_s {
  // POP3 client command type
  uint8_t command_type;

  // POP3 client command structure
  union {
    // POP3 client session create structure
    rsi_pop3_client_session_create_t pop3_client_session_create;

    // POP3 client mail index
    uint8_t pop3_client_mail_index[2];

  } pop3_struct;

} rsi_req_pop3_client_t;

// Define POP3 client response structure
typedef struct rsi_pop3_client_resp_s {
  uint8_t command_type;

  // Total number of mails
  uint8_t mail_count[2];

  // Total size of all the mails
  uint8_t size[4];

} rsi_pop3_client_resp_t;

// Define POP3 client mail data response structure
typedef struct rsi_pop3_mail_data_resp_s {
  // Type of the POP3 client command
  uint8_t command_type;

  // More data pending flag
  uint8_t more;

  // Length the mail chunk
  uint8_t length[2];

} rsi_pop3_mail_data_resp_t;

// HTTP Client PUT Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

#define HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH 900

#define MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH 900

// HTTP client maximum post data chunk length
#define HTTP_CLIENT_POST_DATA_MAX_BUFFER_LENGTH 900
#define MAX_HTTP_CLIENT_POST_DATA_BUFFER_LENGTH 900

// HTTP client PUT create command
#define HTTP_CLIENT_PUT_CREATE 1

// HTTP client PUT START command
#define HTTP_CLIENT_PUT_START 2

// HTTP client PUT PKT command
#define HTTP_CLIENT_PUT_PKT 3

// HTTP client PUT DELETE command
#define HTTP_CLIENT_PUT_DELETE 4

// HTTP client PUT PKT OFFSET command
#define HTTP_CLIENT_PUT_OFFSET_PKT 5

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

// Define HTTP client PUT START create structure
typedef struct rsi_http_client_put_start_s {
  // HTTP server ip version
  uint8_t ip_version;

  // HTTPS bit map
  uint8_t https_enable[2];

  // HTTP server port number
  uint8_t port_number[4];

  // HTTP Content Length
  uint8_t content_length[4];

} rsi_http_client_put_start_t;

typedef struct rsi_http_client_put_data_req_s {
  // Current chunk length
  uint8_t current_length[2];
} rsi_http_client_put_data_req_t;

typedef struct rsi_http_client_put_req_s {
  // Command type
  uint8_t command_type;

  union {
    // HTTP PUT START command structure
    rsi_http_client_put_start_t http_client_put_start;

    // HTTP PUT PACKET command structure
    rsi_http_client_put_data_req_t http_client_put_data_req;

  } http_client_put_struct;

  // HTTP PUT buffer
  uint8_t http_put_buffer[MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH];

} rsi_http_client_put_req_t;

// HTTP Client PUT pkt server response structure
typedef struct http_Put_Data_s {
  uint32_t command_type;
  uint32_t more;
  uint32_t offset;
  uint32_t data_len;
} http_Put_Data_t;

// HTTP Client PUT response structure
typedef struct rsi_http_client_put_rsp_s {
  // Receive HTTP Client PUT command type
  uint8_t command_type;

} rsi_http_client_put_rsp_t;

// HTTP Client PUT PKT response structure
typedef struct rsi_http_client_put_pkt_rsp_s {
  // Receive HTTP client PUT command type
  uint8_t command_type;

  // End of resource content file
  uint8_t end_of_file;

} rsi_http_client_put_pkt_rsp_t;

// HTTP Client POST DATA PKT request structure
typedef struct rsi_http_client_post_data_req_s {
  // Current http data chunk length
  uint8_t current_length[2];

  // HTTP POST buffer
  uint8_t http_post_data_buffer[MAX_HTTP_CLIENT_POST_DATA_BUFFER_LENGTH];

} rsi_http_client_post_data_req_t;

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_EMB_MQTT_TOPIC_MAX_LEN    62
#define RSI_EMB_MQTT_WILL_MSG_MAX_LEN 100
#define RSI_EMB_MQTT_CLIENTID_MAX_LEN 62
#define RSI_EMB_MQTT_USERNAME_MAX_LEN 62
#define RSI_EMB_MQTT_PASSWORD_MAX_LEN 62
#define RSI_TCP_MAX_SEND_SIZE         1460
#define RSI_EMB_MQTT_PUB_MAX_LEN      RSI_TCP_MAX_SEND_SIZE
#define RSI_EMB_MQTT_SSL_PUB_MAX_LEN  (RSI_TCP_MAX_SEND_SIZE - RSI_SSL_HEADER_SIZE)
#define RSI_EMB_MQTT_FIXED_HDR_LEN    3
#define RSI_EMB_MQTT_PACKET_ID_LEN    2

// Emb MQTT init Flags
#define RSI_EMB_MQTT_CLEAN_SESSION BIT(0)
#define RSI_EMB_MQTT_SSL_ENABLE    BIT(1)
#define RSI_EMB_MQTT_IPV6_ENABLE   BIT(2)

// Emb MQTT Connect flags
#define RSI_EMB_MQTT_USER_FLAG   BIT(7)
#define RSI_EMB_MQTT_PWD_FLAG    BIT(6)
#define RSI_EMB_MQTT_WILL_RETAIN BIT(5)
#define RSI_EMB_MQTT_WILL_FLAG   BIT(2)

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
` * ******************************************************/

typedef enum rsi_emb_mqtt_state_e {
  RSI_EMB_MQTT_UNINIT_STATE    = 0,
  RSI_EMB_MQTT_CLIENT_INIT     = 1,
  RSI_EMB_MQTT_CONNECT         = 2,
  RSI_EMB_MQTT_SUBSCRIBE       = 3,
  RSI_EMB_MQTT_SND_PUB_PKT     = 4,
  RSI_EMB_MQTT_UNSUBSCRIBE     = 5,
  RSI_EMB_MQTT_TIMEOUT         = 6,
  RSI_EMB_MQTT_RCV_PUB_PKT     = 7,
  RSI_EMB_MQTT_DISCONNECT      = 8,
  RSI_EMB_MQTT_COMMAND_DESTROY = 9
} rsi_emb_mqtt_state_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

typedef struct rsi_mqtt_pubmsg_s {
  int8_t qos;

  int8_t retained;

  int8_t dup;

  void *payload;

  uint16_t payloadlen;

} rsi_mqtt_pubmsg_t;

typedef struct rsi_emb_mqtt_unsub_s {
  uint8_t command_type[4];
  // length of TOPIC
  uint8_t topic_len;
  // topic of unsubscribe message
  uint8_t topic[RSI_EMB_MQTT_TOPIC_MAX_LEN];

} rsi_emb_mqtt_unsub_t;

typedef struct rsi_emb_mqtt_snd_pub_s {
  uint8_t command_type[4];
  // length of TOPIC
  uint8_t topic_len;
  // topic of unsubscribe message
  uint8_t topic[RSI_EMB_MQTT_TOPIC_MAX_LEN];
  // message Qos, can be 0, 1, or 2
  uint8_t qos;
  // retained flag, can be 0 or 1
  uint8_t retained;
  // duplicate flag, can be 0 or 1
  uint8_t dup;
  // length of publish message(option), if set to 0 or
  //omitted, <message> will be parsed in text format, else
  //hexidecimai format
  uint8_t msg_len[2];
  // publish message
  int8_t *msg;

} rsi_emb_mqtt_snd_pub_t;

typedef struct rsi_emb_mqtt_sub_s {
  uint8_t command_type[4];
  // length of TOPIC
  uint8_t topic_len;
  // topic of subscribe message
  int8_t topic[RSI_EMB_MQTT_TOPIC_MAX_LEN];
  // message Qos, can be 0, 1, or 2
  int8_t qos;

} rsi_emb_mqtt_sub_t;

typedef struct rsi_emb_mqtt_connect_s {
  uint8_t command_type[4];
  // weather to use username (0-1)
  uint8_t usrFlag;
  //  weather to use pwdFlag (0-1)
  uint8_t pwdFlag;
  //  weather to set willmsg (0-1)
  uint8_t willFlag;
  //  retained flag(0-1)
  uint8_t willRetain;
  //  message Qos(0-2)
  uint8_t willQos;
  // length of topic
  uint8_t willTopic_len;
  //  topic name of will
  uint8_t will_topic[RSI_EMB_MQTT_TOPIC_MAX_LEN];
  // Length of Will message
  uint8_t will_message_len;
  //  message of will
  uint8_t will_msg[RSI_EMB_MQTT_WILL_MSG_MAX_LEN];

} rsi_emb_mqtt_connect_t;

typedef struct rsi_emb_mqtt_ip_addr_s {
  // IP version
  uint8_t ip_version[4];
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
} rsi_emb_mqtt_ip_addr_t;

typedef struct rsi_emb_mqtt_client_init_s {
  uint8_t command_type[4];
  // MQTT server IP address
  rsi_emb_mqtt_ip_addr_t server_ip;
  // MQTT server port
  uint8_t server_port[4];
  // Client ID Length
  uint8_t clientID_len;
  // client ID, should be unique
  int8_t client_id[RSI_EMB_MQTT_CLIENTID_MAX_LEN];
  // keep alive interval (s)
  uint8_t keep_alive_interval[2];
  // username Length
  uint8_t username_len;
  // user name
  uint8_t user_name[RSI_EMB_MQTT_USERNAME_MAX_LEN];
  // password Length
  uint8_t password_len;
  // password
  uint8_t password[RSI_EMB_MQTT_PASSWORD_MAX_LEN];
  // clean session(0-1)
  uint8_t clean;
  // 0 : TCP , 1 : SSl
  uint8_t encrypt;
  // MQTT  Client port
  uint8_t client_port[4];
} rsi_emb_mqtt_client_init_t;

typedef struct rsi_req_emb_mqtt_command_s {
  // MQTT command type
  uint8_t command_type[4];

} rsi_req_emb_mqtt_command_t;

#endif
