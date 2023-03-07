/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
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

#ifndef _SL_WFX_GENERAL_API_H_
#define _SL_WFX_GENERAL_API_H_

#include <stdint.h>

#include "sl_wfx_general_error_api.h"

//< API Internal Version encoding
#define SL_WFX_API_VERSION_MINOR                 0x08
#define SL_WFX_API_VERSION_MAJOR                 0x03

#define SL_WFX_SSID_SIZE                         32
#define SL_WFX_MAC_ADDR_SIZE                     6
#define SL_WFX_BSSID_SIZE                        SL_WFX_MAC_ADDR_SIZE

#define GENERAL_INTERFACE_ID                     2

#define SL_WFX_MSG_ID_MASK                       0x00FF
#define SL_WFX_MSG_TYPE_MASK                     0x80
#define SL_WFX_MSG_SEQ_RANGE                     0x0007 //range of field 'host_count' in msginfo_bitfield_t

/* Message bases */
#define SL_WFX_REQ_BASE                          0x00
#define SL_WFX_CNF_BASE                          0x00
#define SL_WFX_IND_BASE                          SL_WFX_MSG_TYPE_MASK

/**
 * @brief List of possible transmission rates.
 *
 * Note that ERP-PBCC is not supported by the hardware. The rate indices for 22 Mbit/s and 33 Mbit/s are only provided for standard compatibility.@n
 * Data rates (in the names) are for 20 MHz channel operation. Corresponding data rates for 10 MHz channel operation are half of them.
 *
 * In this API, some parameters such as 'basic_rate_set' encode a list of rates in a bitstream format.@n
 *     for instance SUPPORTED_B_RATES_MASK = 0x0000000F @n
 *                  SUPPORTED_G_RATES_MASK = 0x00003FC0 @n
 *                  SUPPORTED_N_RATES_MASK = 0x003FC000
 */
typedef enum sl_wfx_rate_index_e {
  SL_WFX_RATE_INDEX_B_1MBPS                      = 0,      ///< Data rate 802.11b 1Mbps
  SL_WFX_RATE_INDEX_B_2MBPS                      = 1,      ///< Data rate 802.11b 2Mbps
  SL_WFX_RATE_INDEX_B_5P5MBPS                    = 2,      ///< Data rate 802.11b 5.5Mbps
  SL_WFX_RATE_INDEX_B_11MBPS                     = 3,      ///< Data rate 802.11b 11Mbps
  SL_WFX_RATE_INDEX_PBCC_22MBPS                  = 4,      ///<ERP-PBCC, not supported
  SL_WFX_RATE_INDEX_PBCC_33MBPS                  = 5,      ///<ERP-PBCC, not supported
  SL_WFX_RATE_INDEX_G_6MBPS                      = 6,      ///< Data rate 802.11g 6Mbps
  SL_WFX_RATE_INDEX_G_9MBPS                      = 7,      ///< Data rate 802.11g 9Mbps
  SL_WFX_RATE_INDEX_G_12MBPS                     = 8,      ///< Data rate 802.11g 12Mbps
  SL_WFX_RATE_INDEX_G_18MBPS                     = 9,      ///< Data rate 802.11g 18Mbps
  SL_WFX_RATE_INDEX_G_24MBPS                     = 10,     ///< Data rate 802.11g 24Mbps
  SL_WFX_RATE_INDEX_G_36MBPS                     = 11,     ///< Data rate 802.11g 36Mbps
  SL_WFX_RATE_INDEX_G_48MBPS                     = 12,     ///< Data rate 802.11g 48Mbps
  SL_WFX_RATE_INDEX_G_54MBPS                     = 13,     ///< Data rate 802.11g 54Mbps
  SL_WFX_RATE_INDEX_N_6P5MBPS                    = 14,     ///< Data rate 802.11n 6.5Mbps
  SL_WFX_RATE_INDEX_N_13MBPS                     = 15,     ///< Data rate 802.11n 13Mbps
  SL_WFX_RATE_INDEX_N_19P5MBPS                   = 16,     ///< Data rate 802.11n 19.5Mbps
  SL_WFX_RATE_INDEX_N_26MBPS                     = 17,     ///< Data rate 802.11n 26Mbps
  SL_WFX_RATE_INDEX_N_39MBPS                     = 18,     ///< Data rate 802.11n 39Mbps
  SL_WFX_RATE_INDEX_N_52MBPS                     = 19,     ///< Data rate 802.11n 52Mbps
  SL_WFX_RATE_INDEX_N_58P5MBPS                   = 20,     ///< Data rate 802.11n 58.5Mbps
  SL_WFX_RATE_INDEX_N_65MBPS                     = 21,     ///< Data rate 802.11n 65Mbps
  SL_WFX_RATE_NUM_ENTRIES                        = 22      ///< Number of defined data rates
} sl_wfx_rate_index_t;

/**
 * @addtogroup MESSAGE_CONSTRUCTION
 * @brief interface message formating
 *
 * The WLAN API handles 3 types of messages.
 * REQUEST, CONFIRMATION and INDICATION
 *
   \msc
   arcgradient = 8;
   a [label="Wlan"],b [label="Host"];
   a=>b [label="INDICATION"];
   b=>b [label="Process indication"];
   ...;
    ---;
   ...;
   a<=b [label="REQUEST"];
   a=>a [label="Process request"];
   a=>b [label="CONFIRMATION"];
   \endmsc
 *
 * WLAN can send an INDICATION message to the host at any time.@n
 * Host can send REQUEST to the WLAN and the WLAN will always answer to it (either immediately or after a while) with a CONFIRMATION.
 *
 * A message is composed of a header and a body (see sl_wfx_generic_msg_t).
 * In the rest of the API description, only the body is detailed for each message.
 *
 * @{
 */

/**
 * @brief General Message header structure
 *
 */
typedef struct __attribute__((__packed__)) sl_wfx_header_s {
  uint16_t length;                                         ///< Message length in bytes including this uint16_t.
                                                           ///< Maximum value is 8188 but maximum Request size is FW dependent and reported in the ::sl_wfx_startup_ind_body_t::size_inp_ch_buf.
  uint8_t  id;                                             ///< Contains the message Id indexed by sl_wfx_general_commands_ids_t or sl_wfx_message_ids_t.
  uint8_t  info;                                           ///< TODO comment missing
} sl_wfx_header_t;

/**
 * @brief Generic message structure for all requests, confirmations and indications
 *
 */
typedef struct __attribute__((__packed__)) sl_wfx_generic_message_s {
  sl_wfx_header_t header;                                  ///<4 bytes header
  uint8_t  body[];                                        ///<variable size payload of the message
} sl_wfx_generic_message_t;

/**
 * @brief Generic confirmation message with the body reduced to the status field.
 *
 * This structure is not related to a specific confirmation ID. @n
 * It is a global simplified structure that can be used to easily access the header and status fields.
 *
 * All confirmation bodies start with a status word and in a lot of them it is followed by other data (not present in this structure).
 */
typedef struct __attribute__((__packed__)) sl_wfx_generic_confirmation_s {
  sl_wfx_header_t header;                                  ///<4 bytes header
  uint32_t status;                                         ///<See enum sl_wfx_status_t
} sl_wfx_generic_confirmation_t;

/**
 * @}
 */

/**
 * @addtogroup GENERAL_API
 * @brief General API messages available in both split and full MAC.
 *
 * Mainly used to boot and configure the part.@n
 * But some message are also used to report errors or information.@n
 *@n
 * \arg general \b requests are sl_wfx_generic_requests_ids_t@n
 * \arg general \b indications are sl_wfx_general_indications_ids_t@n
 * @n
 * @{
 */

/**
 * @brief General request message IDs
 *
 * API general request message IDs available in both split and full MAC.
 * These are messages from the host towards the WLAN.
 */
typedef enum sl_wfx_generic_requests_ids_e {
  SL_WFX_CONFIGURATION_REQ_ID                    = 0x09,   ///< \b CONFIGURATION request Id use body sl_wfx_configuration_req_body_t and returns sl_wfx_configuration_cnf_body_t
  SL_WFX_CONTROL_GPIO_REQ_ID                     = 0x26,   ///< \b CONTROL_GPIO request Id use body sl_wfx_control_gpio_req_body_t and returns sl_wfx_control_gpio_cnf_body_t
  SL_WFX_SET_SECURELINK_MAC_KEY_REQ_ID           = 0x27,   ///< \b SET_SECURELINK_MAC_KEY request Id use body sl_wfx_set_securelink_mac_key_req_body_t and returns sl_wfx_set_securelink_mac_key_cnf_body_t
  SL_WFX_SECURELINK_EXCHANGE_PUB_KEYS_REQ_ID     = 0x28,   ///< \b SECURELINK_EXCHANGE_PUB_KEYS request Id use body sl_wfx_securelink_exchange_pub_keys_req_body_t and returns sl_wfx_securelink_exchange_pub_keys_cnf_body_t
  SL_WFX_SECURELINK_CONFIGURE_REQ_ID             = 0x29,   ///< \b SECURELINK_CONFIGURE request Id use body sl_wfx_securelink_configure_req_body_t and returns sl_wfx_securelink_exchange_pub_keys_cnf_body_t
  SL_WFX_PREVENT_ROLLBACK_REQ_ID                 = 0x2a,   ///< \b PREVENT_ROLLBACK request Id use body sl_wfx_prevent_rollback_req_body_t and returns sl_wfx_prevent_rollback_cnf_body_t
  SL_WFX_PTA_SETTINGS_REQ_ID                     = 0x2b,   ///< \b PTA_SETTINGS request Id use body sl_wfx_pta_settings_req_body_t and returns sl_wfx_pta_settings_cnf_body_t
  SL_WFX_PTA_PRIORITY_REQ_ID                     = 0x2c,   ///< \b PTA_PRIORITY request Id use body sl_wfx_pta_priority_req_body_t and returns sl_wfx_pta_priority_cnf_body_t
  SL_WFX_PTA_STATE_REQ_ID                        = 0x2d,   ///< \b PTA_STATE request Id use body sl_wfx_pta_state_req_body_t and returns sl_wfx_pta_state_cnf_body_t
  SL_WFX_SET_CCA_CONFIG_REQ_ID                   = 0x2e,   ///< \b SET_CCA_CONFIG request Id uses body sl_wfx_set_cca_config_req_body_t and returns sl_wfx_set_cca_config_cnf_body_t
  SL_WFX_SHUT_DOWN_REQ_ID                        = 0x32,   ///< \b SHUT_DOWN request Id use body sl_wfx_shut_down_req_t and never returns
} sl_wfx_generic_requests_ids_t;

/**
 * @brief General confirmation message IDs
 *
 * API general confirmation message IDs returned by requests described in sl_wfx_general_requests_ids.
 * These are messages from the WLAN towards the host.
 */
typedef enum sl_wfx_general_confirmations_ids_e {
  SL_WFX_CONFIGURATION_CNF_ID                    = 0x09,   ///< \b CONFIGURATION confirmation Id returns body sl_wfx_configuration_cnf_body_t
  SL_WFX_CONTROL_GPIO_CNF_ID                     = 0x26,   ///< \b CONTROL_GPIO confirmation Id returns body sl_wfx_control_gpio_cnf_body_t
  SL_WFX_SET_SECURELINK_MAC_KEY_CNF_ID           = 0x27,   ///< \b SET_SECURELINK_MAC_KEY confirmation Id returns body sl_wfx_set_securelink_mac_key_cnf_body_t
  SL_WFX_SECURELINK_EXCHANGE_PUB_KEYS_CNF_ID     = 0x28,   ///< \b SECURELINK_EXCHANGE_PUB_KEYS confirmation Id returns body sl_wfx_securelink_exchange_pub_keys_cnf_body_t
  SL_WFX_SECURELINK_CONFIGURE_CNF_ID             = 0x29,   ///< \b SECURELINK_CONFIGURE confirmation Id returns body sl_wfx_securelink_configure_cnf_body_t
  SL_WFX_PREVENT_ROLLBACK_CNF_ID                 = 0x2a,   ///< \b PREVENT_ROLLBACK confirmation Id use body sl_wfx_prevent_rollback_cnf_body_t
  SL_WFX_PTA_SETTINGS_CNF_ID                     = 0x2b,   ///< \b PTA_SETTINGS confirmation Id returns sl_wfx_pta_settings_cnf_body_t
  SL_WFX_PTA_PRIORITY_CNF_ID                     = 0x2c,   ///< \b PTA_PRIORITY confirmation Id returns sl_wfx_pta_priority_cnf_body_t
  SL_WFX_PTA_STATE_CNF_ID                        = 0x2d,   ///< \b PTA_STATE confirmation Id returns sl_wfx_pta_state_cnf_body_t
  SL_WFX_SET_CCA_CONFIG_CNF_ID                   = 0x2e,   ///< \b SET_CCA_CONFIG confirmation Id returns body sl_wfx_set_cca_config_cnf_body_t
} sl_wfx_general_confirmations_ids_t;

/**
 * @brief General indications message IDs
 *
 * API general indication message IDs available in both split and full MAC.
 * These are messages from the WLAN towards the host.
 */
typedef enum sl_wfx_general_indications_ids_e {
  SL_WFX_EXCEPTION_IND_ID                        = 0xe0,   ///< \b EXCEPTION indication Id content is sl_wfx_exception_ind_body_t
  SL_WFX_STARTUP_IND_ID                          = 0xe1,   ///< \b STARTUP indication Id content is sl_wfx_startup_ind_body_t
  SL_WFX_WAKEUP_IND_ID                           = 0xe2,   ///< \b WAKE UP indication Id has no content
  SL_WFX_GENERIC_IND_ID                          = 0xe3,   ///< \b GENERIC indication Id content is sl_wfx_generic_ind_body_t
  SL_WFX_ERROR_IND_ID                            = 0xe4,   ///< \b ERROR indication Id content is sl_wfx_error_ind_body_t
  SL_WFX_SECURELINK_EXCHANGE_PUB_KEYS_IND_ID     = 0xe5,   ///< \b SECURELINK_EXCHANGE_PUB_KEYS indication Id content is sl_wfx_securelink_exchange_pub_keys_ind_body_t
} sl_wfx_general_indications_ids_t;

/**
 * @brief General command message IDs
 *
 * All general API message IDs.
 */
typedef union __attribute__((__packed__)) sl_wfx_general_commands_ids_u {
  sl_wfx_generic_requests_ids_t request;                   ///< Request from the host to the WLAN device
  sl_wfx_general_confirmations_ids_t confirmation;         ///< Confirmation of a request from the WLAN device to the host
  sl_wfx_general_indications_ids_t indication;             ///< Indication from the WLAN device to the host
} sl_wfx_general_commands_ids_t;

/**************************************************/

/**
 * @brief General confirmation possible values for returned 'status' field
 * WLAN
 * All general confirmation messages have a field 'status' just after the message header.@n
 * A value of zero indicates the request is completed successfully.
 *
 */
typedef enum sl_wfx_status_e {
  SL_WFX_STATUS_SUCCESS                          = 0x0,    ///<The firmware has successfully completed the request.
  SL_WFX_STATUS_FAILURE                          = 0x1,    ///<This is a generic failure code : other error codes do not apply.
  SL_WFX_INVALID_PARAMETER                       = 0x2,    ///<The request contains one or more invalid parameters.
  SL_WFX_STATUS_GPIO_WARNING                     = 0x3,    ///<Warning : the GPIO CMD is successful but the read value is not as expected (likely a drive conflict on the line)
  SL_WFX_ERROR_UNSUPPORTED_MSG_ID                = 0x4,    ///<Unknown request ID or wrong interface ID used
  /* Specific SecureLink statuses */
  SL_WFX_MAC_KEY_STATUS_SUCCESS                  = 0x5A,   ///<Key has been correctly written
  SL_WFX_MAC_KEY_STATUS_FAILED_KEY_ALREADY_BURNED = 0x6B,  ///<Key already exists in OTP
  SL_WFX_MAC_KEY_STATUS_FAILED_RAM_MODE_NOT_ALLOWED = 0x7C, ///<RAM mode is not allowed
  SL_WFX_MAC_KEY_STATUS_FAILED_UNKNOWN_MODE      = 0x8D,   ///<Unknown mode (should be RAM or OTP)
  SL_WFX_PUB_KEY_EXCHANGE_STATUS_SUCCESS         = 0x9E,   ///<Host Public Key authenticated
  SL_WFX_PUB_KEY_EXCHANGE_STATUS_FAILED          = 0xAF,   ///<Host Public Key authentication failed
  /* Specific Prevent Rollback statuses */
  SL_WFX_PREVENT_ROLLBACK_CNF_SUCCESS            = 0x1234, ///<OTP rollback value has been successfully updated
  SL_WFX_PREVENT_ROLLBACK_CNF_WRONG_MAGIC_WORD   = 0x1256  ///<Wrong magic word detected
} sl_wfx_status_t;

/**************************************************/

/**
 * @addtogroup General_Configuration
 * @brief General configuration commands
 *
 *
 * @{
 */
typedef enum sl_wfx_type_e {
  SL_WFX_FW_TYPE_ETF                             = 0x0,    ///<Test Firmware
  SL_WFX_FW_TYPE_WFM                             = 0x1,    ///<WLAN Full MAC (WFM)
  SL_WFX_FW_TYPE_WSM                             = 0x2     ///<WLAN Split MAC (WSM)
} sl_wfx_fw_type_t;

/**
 * @brief Capabilities offered by the WLAN used in command sl_wfx_startup_ind_body_t
 */
typedef struct __attribute__((__packed__)) sl_wfx_capabilities_s {
  uint8_t  linkmode : 2;                                   ///<Bit 0-1 : OTP SecureLink mode. 0=reserved, 1=untrusted (no secure link supported), 2=evaluation mode (no key burnt), 3=active
  uint8_t  reserved1 : 6;                                  ///<Bit 2-7 : Reserved
  uint8_t  reserved2;                                      ///<Bit 8-15 : Reserved
  uint8_t  reserved3;                                      ///<Bit 16-23 : Reserved
  uint8_t  reserved4;                                      ///<Bit 24-31 : Reserved
} sl_wfx_capabilities_t;

/**
 * @brief REGUL_SEL_MODE OTP field reported in command sl_wfx_startup_ind_body_t
 */
typedef struct __attribute__((__packed__)) sl_wfx_otp_regul_sel_mode_info_s {
  uint8_t  region_sel_mode : 4;                            ///<Bit 0-3 : indicates default FW behavior regarding DFS region setting
                                                           ///<          0=Unrestricted, 1=SuperGlobal (enforces FCC, CE and Japan regulations depending on the automatic region detection)
                                                           ///<          2=SingleRegion (enforces FCC, CE or Japan depending on OTP tables content), 3=Reserved
  uint8_t  reserved : 4;                                   ///<Bit 4-7 : Reserved
} sl_wfx_otp_regul_sel_mode_info_t;

/**
 * @brief OTP_PHY_INFO OTP field reported in command sl_wfx_startup_ind_body_t
 */
typedef struct __attribute__((__packed__)) sl_wfx_otp_phy_info_s {
  uint8_t  phy1_region : 3;                                ///<Bit 0-2 : DFS region corresponding to backoff vs. channel group table indexed 1
  uint8_t  phy0_region : 3;                                ///<Bit 3-5 : DFS region corresponding to backoff vs. channel group table indexed 0
  uint8_t  otp_phy_ver : 2;                                ///<Bit 6-7 : Revision of OTP info
} sl_wfx_otp_phy_info_t;

#define SL_WFX_OPN_SIZE                          14
#define SL_WFX_UID_SIZE                          8
#define SL_WFX_DISABLED_CHANNEL_LIST_SIZE        2
#define SL_WFX_FIRMWARE_LABEL_SIZE               128
/**
 * @brief Startup Indication message.
 * This is the first message sent to the host to confirm boot success.
 * It gives detailed information on the HW and FW versions and capabilities
 */
typedef struct __attribute__((__packed__)) sl_wfx_startup_ind_body_s {
  uint32_t status;                                         ///<Initialization status. A value of zero indicates the boot is completed successfully  (see enum sl_wfx_status_t)
  uint16_t hardware_id;                                    ///<=RO misc_read_reg7 register value
  uint8_t  opn[SL_WFX_OPN_SIZE];                           ///<=OTP part_OPN
  uint8_t  uid[SL_WFX_UID_SIZE];                           ///<=OTP UID
  uint16_t num_inp_ch_bufs;                                ///<Number of buffers available for request messages.
  uint16_t size_inp_ch_buf;                                ///<TX Buffer size in bytes=request message max size.
  uint8_t  num_links_aP;                                   ///<number of STA that are supported in AP mode
  uint8_t  num_interfaces;                                 ///<number of interfaces (WIFI link : STA or AP) that can be created by the user
  uint8_t  mac_addr[2][SL_WFX_MAC_ADDR_SIZE];              ///<MAC addresses derived from OTP
  uint8_t  api_version_minor;
  uint8_t  api_version_major;
  sl_wfx_capabilities_t capabilities;                      ///<List some FW options
  uint8_t  firmware_build;
  uint8_t  firmware_minor;
  uint8_t  firmware_major;
  uint8_t  firmware_type;                                  ///<See enum sl_wfx_fw_type_t
  uint8_t  disabled_channel_list[SL_WFX_DISABLED_CHANNEL_LIST_SIZE]; ///<=OTP Disabled channel list info
  sl_wfx_otp_regul_sel_mode_info_t regul_sel_mode_info;    ///<OTP region selection mode info
  sl_wfx_otp_phy_info_t otp_phy_info;                      ///<info on OTP backoff tables used to enforce the different DFS regulations.
  uint32_t supported_rate_mask;                            ///<A bit mask that indicates which rates are supported by the Physical layer. See enum api_rate_index.
  uint8_t  firmware_label[SL_WFX_FIRMWARE_LABEL_SIZE];     ///<Null terminated text string describing the loaded FW.
} sl_wfx_startup_ind_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_startup_ind_s {
  sl_wfx_header_t header;
  sl_wfx_startup_ind_body_t body;
} sl_wfx_startup_ind_t;

/**
 * @brief Wake-up indication message.
 * @since API1.4.0
 *
 * Its body is empty. It signals that the chip is awake and ready to receive commands after a wake-up request from the host.
 * It generates a DATA_IRQ to the host driver when ready.
 * It can be replaced or passed by another message that would have appeared concurrently like a Rx frame.
 * */

typedef sl_wfx_header_t sl_wfx_wakeup_ind_t;

/**
 * @brief Configure the device.
 * It sends a PDS compressed file that configures the device regarding board dependent parameters.
 * The PDS compressed file must fit in a command buffer and have less than 256 elements.
 *
 * @todo Need to create a specific doc to explain PDS*/
typedef struct __attribute__((__packed__)) sl_wfx_configuration_req_body_s {
  uint16_t length;                                         ///<pds_data length in bytes
  uint8_t  pds_data[];                                    ///<variable size PDS data byte array
} sl_wfx_configuration_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_configuration_req_s {
  sl_wfx_header_t header;
  sl_wfx_configuration_req_body_t body;
} sl_wfx_configuration_req_t;

/**
 * @brief Confirmation message of CONFIGURATION command sl_wfx_configuration_req_body_t */
typedef struct __attribute__((__packed__)) sl_wfx_configuration_cnf_body_s {
  uint32_t status;                                         ///<Configuration status. A value of zero indicates the boot is completed successfully (see enum sl_wfx_status_t)
} sl_wfx_configuration_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_configuration_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_configuration_cnf_body_t body;
} sl_wfx_configuration_cnf_t;

/**
 * @brief Configure GPIO mode. Used in sl_wfx_control_gpio_req_body_t
 * */
typedef enum sl_wfx_gpio_mode_e {
  SL_WFX_GPIO_MODE_D0                            = 0x0,    ///< Configure the GPIO to drive 0
  SL_WFX_GPIO_MODE_D1                            = 0x1,    ///< Configure the GPIO to drive 1
  SL_WFX_GPIO_MODE_OD0                           = 0x2,    ///< Configure the GPIO to open drain with pull_down to 0
  SL_WFX_GPIO_MODE_OD1                           = 0x3,    ///< Configure the GPIO to open drain with pull_up to 1
  SL_WFX_GPIO_MODE_TRISTATE                      = 0x4,    ///< Configure the GPIO to tristate
  SL_WFX_GPIO_MODE_TOGGLE                        = 0x5,    ///< Toggle the GPIO output value : switches between D0 and D1 or between OD0 and OD1
  SL_WFX_GPIO_MODE_READ                          = 0x6     ///< Read the level at the GPIO pin
} sl_wfx_gpio_mode_t;

/**
 * @brief Send a request to read or write a GPIO identified by its label (that is defined in the PDS)
 *
 * After a write it also read back the value to check there is no drive conflict */
typedef struct __attribute__((__packed__)) sl_wfx_control_gpio_req_body_s {
  uint8_t  gpio_label;                                     ///<Identify the GPIO by its label (defined in the PDS)
  uint8_t  gpio_mode;                                      ///<define how to set or read the GPIO (see enum sl_wfx_gpio_mode_t)
} sl_wfx_control_gpio_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_control_gpio_req_s {
  sl_wfx_header_t header;
  sl_wfx_control_gpio_req_body_t body;
} sl_wfx_control_gpio_req_t;

/**
 * @brief detailed error cause returned by CONTROL_GPIO confirmation message sl_wfx_control_gpio_cnf_body_t
 * */
typedef enum sl_wfx_gpio_error_e {
  SL_WFX_GPIO_ERROR_0                            = 0x0,    ///< Undefined GPIO_ID
  SL_WFX_GPIO_ERROR_1                            = 0x1,    ///< GPIO_ID not configured in GPIO mode (gpio_enabled =0)
  SL_WFX_GPIO_ERROR_2                            = 0x2     ///< Toggle not possible while in tristate
} sl_wfx_gpio_error_t;

/**
 * @brief Confirmation from request to read and write a GPIO */
typedef struct __attribute__((__packed__)) sl_wfx_control_gpio_cnf_body_s {
  uint32_t status;                                         ///<enum sl_wfx_status_t : a value of zero indicates the request is completed successfully.
  uint32_t value;                                          ///<the error detail (see enum sl_wfx_gpio_error_t) when ::sl_wfx_control_gpio_cnf_body_t::status reports an error else the gpio read value.
} sl_wfx_control_gpio_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_control_gpio_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_control_gpio_cnf_body_t body;
} sl_wfx_control_gpio_cnf_t;

/**
 * @brief SHUT_DOWN command.
 * A hardware reset and complete reboot is required to resume from that state.
 * There is no confirmation to this command.
 * It is effective when WUP register bit and WUP pin (when used) are both to 0.*/
typedef sl_wfx_header_t sl_wfx_shut_down_req_t;

/**
 * @brief specifies the type of data reported by the indication message sl_wfx_generic_ind_body_t
 *
 * */
typedef enum sl_wfx_generic_indication_type_e {
  SL_WFX_GENERIC_INDICATION_TYPE_RAW             = 0x0,    ///<Byte stream type, currently not used
  SL_WFX_GENERIC_INDICATION_TYPE_STRING          = 0x1,    ///<NULL terminating String
  SL_WFX_GENERIC_INDICATION_TYPE_RX_STATS        = 0x2,    ///<Rx statistics structure
  SL_WFX_GENERIC_INDICATION_TYPE_TX_PWR_LOOP_INFO = 0x3     ///<Tx power loop info structure
} sl_wfx_generic_indication_type_t;

/**
 * @brief RX stats from the GENERIC indication message sl_wfx_generic_ind_body_t
 */
typedef struct __attribute__((__packed__)) sl_wfx_rx_stats_s {
  uint32_t nb_rx_frame;                                    ///<Total number of frame received
  uint32_t nb_crc_frame;                                   ///<Number of frame received with bad CRC
  uint32_t per_total;                                      ///<PER on the total number of frame
  uint32_t throughput;                                     ///<Throughput calculated on correct frames received
  uint32_t nb_rx_by_rate[SL_WFX_RATE_NUM_ENTRIES];         ///<Number of frame received by rate
  uint16_t per[SL_WFX_RATE_NUM_ENTRIES];                   ///<PER*10000 by frame rate
  int16_t  snr[SL_WFX_RATE_NUM_ENTRIES];                   ///<SNR in Db*100 by frame rate
  int16_t  rssi[SL_WFX_RATE_NUM_ENTRIES];                  ///<RSSI in Dbm*100 by frame rate
  int16_t  cfo[SL_WFX_RATE_NUM_ENTRIES];                   ///<CFO in k_hz by frame rate
  uint32_t date;                                           ///<This message transmission date in firmware timebase (microsecond)
  uint32_t pwr_clk_freq;                                   ///<Frequency of the low power clock in Hz
  uint8_t  is_ext_pwr_clk;                                 ///<Indicate if the low power clock is external
  int8_t   current_temp;                                   ///<Current die temperature in Celsius
} sl_wfx_rx_stats_t;

/**
 * @brief TX power loop info from the GENERIC indication message sl_wfx_generic_ind_body_t
 */
typedef struct __attribute__((__packed__)) sl_wfx_tx_pwr_loop_info_s {
  uint16_t   dig_gain;                                     ///<Used Tx digital gain
  uint16_t   ppa_gain;                                     ///<Used Tx PA gain
  int16_t    target_pout;                                  ///<Power target in qdBm
  int16_t    pestimation;                                  ///<FEM output power in qdBm
  uint16_t   vpdet;                                        ///<Measured Vpdet in mV
  uint8_t    meas_index;                                   ///<Vpdet measurement index
  uint8_t    reserved;                                     ///<Reserved
} sl_wfx_tx_pwr_loop_info_t;

typedef union __attribute__((__packed__)) sl_wfx_indication_data_u {
  sl_wfx_rx_stats_t rx_stats;
  sl_wfx_tx_pwr_loop_info_t tx_pwr_loop_info;
  uint8_t  raw_data[376];
} sl_wfx_indication_data_t;

/**
 * @brief the Generic indication message.
 *
 * It reports different type of information that can be printed by the driver.
 * */
typedef struct __attribute__((__packed__)) sl_wfx_generic_ind_body_s {
  uint32_t indication_type;                                ///<Identify the indication data (see enum type sl_wfx_generic_indication_type_t)
  sl_wfx_indication_data_t indication_data;                ///<Indication data.
} sl_wfx_generic_ind_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_generic_ind_s {
  sl_wfx_header_t header;
  sl_wfx_generic_ind_body_t body;
} sl_wfx_generic_ind_t;

#define SL_WFX_EXCEPTION_DATA_SIZE_MAX          1600
/**
 * @brief Exception indication message
 *
 * It reports unexpected errors. A reboot is needed after this message.
 * */
typedef struct __attribute__((__packed__)) sl_wfx_exception_ind_body_s {
  uint32_t reason;                                         ///<Reason of the exception
  uint8_t  data[];                                         ///<Raw data array
} sl_wfx_exception_ind_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_exception_ind_s {
  sl_wfx_header_t header;
  sl_wfx_exception_ind_body_t body;
} sl_wfx_exception_ind_t;

/**
 * @brief Error indication message.
 *
 * It reports user configuration errors.
 * A reboot is needed after this message.
 * */
typedef struct __attribute__((__packed__)) sl_wfx_error_ind_body_s {
  uint32_t type;                                           ///<error type, see enum sl_wfx_error_t
  uint8_t  data[];                                         ///<Generic data buffer - contents depends on the error type.
} sl_wfx_error_ind_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_error_ind_s {
  sl_wfx_header_t header;
  sl_wfx_error_ind_body_t body;
} sl_wfx_error_ind_t;

/**
 * @}
 */
/* end of General_Configuration */

/******************************************************************************
 * Secure link section
 *****************************************************************************/
/**
 * @addtogroup Secure_Link
 * @brief APIs for Secure link configuration and usage.
 *
 * WFX family of product have the ability to encrypt the SDIO or SPI link.
 *
 * Link to more detailed documentation about the Secure Link feature : \ref SecureLink
 * @{
 */

/**
 * @brief *Secure Link' device state
 * */
typedef enum sl_wfx_secure_link_state_e {
  SECURE_LINK_NA_MODE                            = 0x0,    ///<Reserved
  SECURE_LINK_UNTRUSTED_MODE                     = 0x1,    ///<Untrusted mode - SecureLink not available
  SECURE_LINK_TRUSTED_MODE                       = 0x2,    ///<Trusted (Evaluation) mode
  SECURE_LINK_TRUSTED_ACTIVE_ENFORCED            = 0x3     ///<Trusted (Enforced) mode
} sl_wfx_secure_link_state_t;

/**
 * @brief destination of the *Secure Link MAC key*, used by request message sl_wfx_set_securelink_mac_key_req_body_t
 * */
typedef enum sl_wfx_securelink_mac_key_dest_e {
  SECURE_LINK_MAC_KEY_DEST_OTP                   = 0x78,   ///<Key will be stored in OTP
  SECURE_LINK_MAC_KEY_DEST_RAM                   = 0x87    ///<Key will be stored in RAM
} sl_wfx_securelink_mac_key_dest_t;

#define SL_WFX_KEY_VALUE_SIZE                    32

/**
 * @brief Set the Secure Link MAC key
 *
 * This API can be used for *Trusted Eval* devices in two contexts:
 * - to set a temporary *SecureLink MAC key* in RAM.
 * - to permanently burn the *SecureLink MAC key* in OTP memory. In that case, the OTP *SecureLink mode* will
 * switch to *Trusted Enforced* mode
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_securemink_mac_key_req_body_s {
  uint8_t  otp_or_ram;                                     ///<Key destination - OTP or RAM (see enum sl_wfx_securelink_mac_key_dest_t)
  uint8_t  key_value[SL_WFX_KEY_VALUE_SIZE];               ///<Secure Link MAC Key value
} sl_wfx_set_securelink_mac_key_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_set_securelink_mac_key_req_s {
  sl_wfx_header_t header;
  sl_wfx_set_securelink_mac_key_req_body_t body;
} sl_wfx_set_securelink_mac_key_req_t;

/**
 * @brief Confirmation for the Secure Link MAC key setting
 * */
typedef struct __attribute__((__packed__)) sl_wfx_set_securelink_mac_key_cnf_body_s {
  uint32_t status;                                         ///<Key upload status (see enum sl_wfx_status_t)
} sl_wfx_set_securelink_mac_key_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_set_securelink_mac_key_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_set_securelink_mac_key_cnf_body_t body;
} sl_wfx_set_securelink_mac_key_cnf_t;

#define SL_WFX_HOST_PUB_KEY_SIZE                 32
#define SL_WFX_HOST_PUB_KEY_MAC_SIZE             64

/**
 * @brief Session Key computation algorithms
 *
 */
typedef enum sl_wfx_secure_link_session_key_alg_e {
  SECURE_LINK_CURVE25519                         = 0x01,   ///< Session key is computed using curve25519 algorithm
  SECURE_LINK_KDF                                = 0x02    ///< Session key is computed using KDF algorithm (not available yet)
} sl_wfx_secure_link_session_key_alg_t;

/**
 * @brief Exchange Secure Link Public Keys
 *
 * This API is used by the Host to send its *curve25519* public key to Device, and get back Device public key in the confirmation message.
 * Once keys are exchanged and authenticated (using their respective MAC), each peer computes the Secure Link *session key* that will be used
 * to encrypt/decrypt future Host<->Device messages.
 */
typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_req_body_s {
  uint32_t algorithm;                                      ///<Choice of the cryptographic algorithm used in the session key computation (see enum sl_wfx_secure_link_session_key_alg_t)
  uint8_t  host_pub_key[SL_WFX_HOST_PUB_KEY_SIZE];         ///<Host Public Key
  uint8_t  host_pub_key_mac[SL_WFX_HOST_PUB_KEY_MAC_SIZE]; ///<Host Public Key MAC
} sl_wfx_securelink_exchange_pub_keys_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_req_s {
  sl_wfx_header_t header;
  sl_wfx_securelink_exchange_pub_keys_req_body_t body;
} sl_wfx_securelink_exchange_pub_keys_req_t;

/**
 * @brief Confirmation for exchange of Secure Link Public Keys
 * */
typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_cnf_body_s {
  uint32_t status;                                         ///<Request status (see enum sl_wfx_status_t)
} sl_wfx_securelink_exchange_pub_keys_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_securelink_exchange_pub_keys_cnf_body_t body;
} sl_wfx_securelink_exchange_pub_keys_cnf_t;

#define SL_WFX_NCP_PUB_KEY_SIZE                  32
#define SL_WFX_NCP_PUB_KEY_MAC_SIZE              64
/**
 * @brief Indication for exchange of Secure Link Public Keys
 * */
typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_ind_body_s {
  uint32_t status;                                         ///<Request status (see enum sl_wfx_status_t)
  uint8_t  ncp_pub_key[SL_WFX_NCP_PUB_KEY_SIZE];           ///<Device Public Key
  uint8_t  ncp_pub_key_mac[SL_WFX_NCP_PUB_KEY_MAC_SIZE];   ///<Device Public Key MAC
} sl_wfx_securelink_exchange_pub_keys_ind_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_securelink_exchange_pub_keys_ind_s {
  sl_wfx_header_t header;
  sl_wfx_securelink_exchange_pub_keys_ind_body_t body;
} sl_wfx_securelink_exchange_pub_keys_ind_t;

#define SL_WFX_SESSION_KEY_PROTECTION_DISABLE_MAGIC        0xfa21e603 ///< Magic word used to disable Session Key protection

#define SL_WFX_ENCR_BMP_SIZE                     32

/**
 * @brief Configure Secure Link Layer
 *
 * This API can be used to:
 * - Set/update the Secure Link *encryption bitmap*
 * - Disable Session Key Protection
 *
 * About the Session Key protection:
 * SecureLink underlying encryption algorithm is AES CCM. This algorithm is using an internal Nonce counter incremented each time a message is
 * encrypted/decrypted. This counter is not supposed to go beyond a given limit to guarantee AES CCM security properties.
 * This is why Host Driver is responsible for renegotiating the session key once the message counter is approaching the limit.
 * Disabling the Session Key protection will disable the check performed by the firmware that the Nonce counter is crossing the limit,
 * allowing Host Driver to use the same session key during the same power cycle, even during a very long time.
 * This behavior is not recommended.
 *
 * To disable the protection, a given magic word (SL_WFX_SESSION_KEY_PROTECTION_DISABLE_MAGIC) must be provided as _DisableSessionKeyProtection_ parameter value. Any other value will let the protection set.
 *
 * @note When SecureLink is activated, _SL Configure_ API must be called right after the key exchange. Issuing another command instead will result in an error.
 * @note It is not recommended to call this API a second time during the same power cycle.
 */
typedef struct __attribute__((__packed__)) sl_wfx_securelink_configure_req_body_s {
  uint8_t  encr_bmp[SL_WFX_ENCR_BMP_SIZE];                 ///<Encryption bitmap
  uint32_t disable_session_key_protection;                 ///<Force the firmware to authorize the use of the same session key during a complete power cycle, even very long. NOT RECOMMENDED
} sl_wfx_securelink_configure_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_securelink_configure_req_s {
  sl_wfx_header_t header;
  sl_wfx_securelink_configure_req_body_t body;
} sl_wfx_securelink_configure_req_t;

/**
 * @brief Confirmation of Secure Link Layer configuration sl_wfx_securelink_configure_req_body_t
 *
 * @return HI_STATUS_SUCCESS
 *
 * @note The host driver should wait for this confirmation to update its local bitmap with the returned value
 * */
typedef struct __attribute__((__packed__)) sl_wfx_securelink_configure_cnf_body_s {
  uint32_t status;                                         ///<Request status (see enum sl_wfx_status_t)
  uint8_t  encr_bmp[SL_WFX_ENCR_BMP_SIZE];                 ///<Encryption bitmap
} sl_wfx_securelink_configure_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_securelink_configure_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_securelink_configure_cnf_body_t body;
} sl_wfx_securelink_configure_cnf_t;

/**
 * @}
 */
/* end of Secure_Link */

/******************************************************************************
 * Prevent Rollback section
 *****************************************************************************/
/**
 * @addtogroup Prevent_Firmware_Rollback
 * @brief APIs for preventing Rollback of unsafe firmware images.
 *
 * By enabling this feature Device can prevent unsafe/outdated firmware from booting.
 *
 * Each firmware owns its internal *rollback revision number* which is compared to
 *   an equivalent revision number burned in Device OTP memory. Depending on the comparison result,
 *   two use cases can occur:
 * - Firmware revision number is higher or equal to the OTP number -> the firmware is allowed
 *   to proceed
 * - Firmware revision number is lower than the OTP value -> the firmware is not allowed to proceed.
 *   An *Error indication* will be returned to the driver indicating the cause of the error (SL_WFX_ERROR_FIRMWARE_ROLLBACK).
 *
 * @note The firmware *rollback revision number* is different that the *firmware version*.
 * The former is incremented only when some important fixes (i.e., Security patches) are provided
 * by a given version of the firmware,that MUST be applied to Device and should not be reverted.
 *  Usually, subsequent firmware versions are supposed to embed the same rollback revision number.
 *
 * The rollback capability relies on the use of a dedicated API sl_wfx_prevent_rollback_req_body_t.
 *
 * All Device drivers supporting *Rollback Prevention* should send this request just after booting a new firmware.
 *   This way, any newer *rollback revision number* included in a firmware will be burned in the OTP.
 *
 * @{
 */

/**
 *@brief Prevent Rollback request
 *
 * *Prevent Rollback* asks WLAN firmware to burn a new *Firmware Rollback* value in a dedicated OTP section.
 *
 * The new value is encoded in the firmware itself. After it is burned, this value will prevent from starting
 *  all firmwares whose internal rollback value is lower than the OTP value.
 *
 * *Magic Word* is used to prevent mistakenly sent requests from burning the OTP.
 *
 */
typedef struct __attribute__((__packed__)) sl_wfx_prevent_rollback_req_body_s {
  uint32_t magic_word;                                     ///<Magic Word - should be 0x5C8912F3
} sl_wfx_prevent_rollback_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_prevent_rollback_req_s {
  sl_wfx_header_t header;
  sl_wfx_prevent_rollback_req_body_t body;
} sl_wfx_prevent_rollback_req_t;

/**
 * @brief Confirmation of the *Prevent Rollback* request
 *
 * The request might have failed for the following reasons:
 * - Wrong *magic word* value
 *
 */
typedef struct __attribute__((__packed__)) sl_wfx_prevent_rollback_cnf_body_s {
  uint32_t status;                                         ///<Confirmation status, see enum sl_wfx_status_t
} sl_wfx_prevent_rollback_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_prevent_rollback_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_prevent_rollback_cnf_body_t body;
} sl_wfx_prevent_rollback_cnf_t;

/**
 * @}
 */
/* end of Prevent_Roll_Back */

/******************************************************************************
 * Packet Traffic Arbitration (PTA) section
 *****************************************************************************/
/**
 * @addtogroup PTA
 * @brief APIs to manage dynamic aspects of Packet Traffic Arbitration (PTA).
 *
 * Functions are provided to:
 * - configure the PTA using the ::sl_wfx_pta_settings_req_body_t request (Mandatory before calling ::sl_wfx_pta_state_req_body_t)
 * - define the level of priority used for arbitration using the ::sl_wfx_pta_priority_req_body_t request (Optional, default ::SL_WFX_PTA_PRIORITY_BALANCED)
 * - start or stop the PTA using ::sl_wfx_pta_state_req_body_t request
 *
 * @{
 */

/**
 * @brief PTA modes.
 */
typedef enum sl_wfx_pta_mode_e {
  SL_WFX_PTA_1W_WLAN_MASTER                      = 0,      ///< PTA 1-wire interface with WLAN master on PTA_TX_CONF pin (GRANT signal),
  SL_WFX_PTA_1W_COEX_MASTER                      = 1,      ///< PTA 1-wire interface with COEX master on PTA_RF_ACT pin (REQUEST signal),
  SL_WFX_PTA_2W                                  = 2,      ///< PTA 2-wire interface on PTA_RF_ACT and PTA_TX_CONF pins (respectively REQUEST and GRANT signals),
  SL_WFX_PTA_3W                                  = 3,      ///< PTA 3-wire interface on same pins as SL_WFX_PTA_2W, plus PTA_STATUS pin (PRIORITY signal)
  SL_WFX_PTA_4W                                  = 4       ///< PTA 4-wire interface on same pins as SL_WFX_PTA_3W, plus PTA_FREQ pin (FREQ signal).
} sl_wfx_pta_mode_t;

/**
 * @brief Signal levels.
 */
typedef enum sl_wfx_signal_level_e {
  SL_WFX_SIGNAL_LOW                              = 0,      ///< Signal level is low
  SL_WFX_SIGNAL_HIGH                             = 1       ///< Signal level is high
} sl_wfx_signal_level_t;

/**
 * @brief Coexistence types supported by PTA.
 */
typedef enum sl_wfx_coex_type_e {
  SL_WFX_COEX_TYPE_GENERIC                       = 0,      ///< IEEE 802.15.4 standards ZigBee SDK, Thread SDK, and so on.
  SL_WFX_COEX_TYPE_BLE                           = 1       ///< Bluetooth Low-Energy Stack
} sl_wfx_coex_type_t;

/**
 * @brief Grant states.
 */
typedef enum sl_wfx_grant_state_e {
  SL_WFX_NO_GRANT                                = 0,      ///< WLAN has the RF, Coex is not allowed to transmit
  SL_WFX_GRANT                                   = 1       ///< Coex is granted
} sl_wfx_grant_state_t;

/**
 * @brief Request sent by the host to set the PTA mode, the active levels on signals, the Coex type,
 * to define the timings, quotas, combined mode and default grant state.
 *
 * Depending on specified PTA mode, every setting is not necessarily used and can then be set to '0'.<br/>
 * The following table indicates the PTA mode for which the setting is significant.<br/>
 * Combined mode is activated during concurrent RX (WLAN and Coex) requests and if SimultaneousRxAccesses is set to '1'.
 *
 * | Settings                 | 1-wire WLAN Master | 1-wire Coex Master | 2-wire | 3-wire | 3-wire (combined) | 4-wire | 4-wire (combined) |
 * |--------------------------|:------------------:|:------------------:|:------:|:------:|:-----------------:|:------:|:-----------------:|
 * | PrioritySamplingTime     |                    |                    |        |    x   |          x        |    x   |          x        |
 * | TxRxSamplingTime         |                    |                    |        |        |          x        |        |          x        |
 * | FreqSamplingTime         |                    |                    |        |        |                   |    x   |          x        |
 * | GrantValidTime           |                    |                    |        |    x   |          x        |    x   |          x        |
 * | FemControlTime           |                    |                    |        |    x   |          x        |    x   |          x        |
 * | FirstSlotTime            |                    |                    |        |        |          x        |        |          x        |
 * | PeriodicTxRxSamplingTime |                    |                    |        |        |          x        |        |          x        |
 * | CoexQuota                |         x          |                    |    x   |        |                   |        |                   |
 * | WlanQuota                |         x          |                    |    x   |        |                   |        |                   |
 *
 * <br/>
 * @image html 3w-timings-sequence.svg "Sequence diagram with 3-wire PTA mode"
 * <br/>
 * @image html 4w-timings-sequence.svg "Sequence diagram with 4-wire PTA mode"
 *
 * @note Request will fail if PTA is started.
 *
 * @warning
 * The following assertions must be respected:
 * - priority_sampling_time < tx_rx_sampling_time < grant_valid_time <= first_slot_time
 * - freq_sampling_time < grant_valid_time < fem_control_time
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_settings_req_body_s {
  uint8_t  pta_mode;                                       ///< The PTA mode, see enum ::sl_wfx_pta_mode_t
  uint8_t  request_signal_active_level;                    ///< Active level on REQUEST signal (PTA_RF_ACT pin), provided by Coex to request the RF, see enum ::sl_wfx_signal_level_t
  uint8_t  priority_signal_active_level;                   ///< Active level on PRIORITY signal (PTA_STATUS pin), provided by Coex to set the priority of the request, see enum ::sl_wfx_signal_level_t
  uint8_t  freq_signal_active_level;                       ///< Active level on FREQ signal (PTA_FREQ pin), provided by Coex in 4-wire mode when Coex and WLAN share the same band, see enum ::sl_wfx_signal_level_t
  uint8_t  grant_signal_active_level;                      ///< Active level on GRANT signal (PTA_TX_CONF pin), generated by PTA to grant the RF to Coex, see enum ::sl_wfx_signal_level_t
  uint8_t  coex_type;                                      ///< The Coex type, see enum ::sl_wfx_coex_type_t
  uint8_t  default_grant_state;                            ///< The state of the GRANT signal before arbitration at grant_valid_time, see enum ::sl_wfx_grant_state_t
  uint8_t  simultaneous_rx_access;                         ///< Boolean to allow both Coex and WLAN to receive concurrently, also named combined mode
  uint8_t  priority_sampling_time;                         ///< The time (in microseconds) from the Coex request to the sampling of the priority on PRIORITY signal (1 to 31)
  uint8_t  tx_rx_sampling_time;                            ///< The time (in microseconds) from the Coex request to the sampling of the directionality on PRIORITY signal (priority_sampling_time to 63)
  uint8_t  freq_sampling_time;                             ///< The time (in microseconds) from the Coex request to the sampling of freq-match information on FREQ signal (1 to 127)
  uint8_t  grant_valid_time;                               ///< The time (in microseconds) from Coex request to the GRANT signal assertion (MAX(tx_rx_sampling_time, freq_sampling_time) to 255)
  uint8_t  fem_control_time;                               ///< The time (in microseconds) from Coex request to the control of FEM (grant_valid_time to 255)
  uint8_t  first_slot_time;                                ///< The time (in microseconds) from the Coex request to the beginning of reception or transmission (grant_valid_time to 255)
  uint16_t periodic_tx_rx_sampling_time;                   ///< The period (in microseconds) from first_slot_time of following samplings of the directionality on PRIORITY signal (1 to 1023)
  uint16_t coex_quota;                                     ///< The duration (in microseconds) for which RF is granted to Coex before it is moved to WLAN
  uint16_t wlan_quota;                                     ///< The duration (in microseconds) for which RF is granted to WLAN before it is moved to Coex
} sl_wfx_pta_settings_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_settings_req_s {
  sl_wfx_header_t header;
  sl_wfx_pta_settings_req_body_t body;
} sl_wfx_pta_settings_req_t;

/**
 * @brief Confirmation sent by WLAN firmware after a ::SL_WFX_PTA_SETTINGS_REQ_ID request.
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_settings_cnf_body_s {
  uint32_t status;                                         ///< Confirmation status, see enum ::sl_wfx_status_t
} sl_wfx_pta_settings_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_settings_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_pta_settings_cnf_body_t body;
} sl_wfx_pta_settings_cnf_t;

/**
 * @brief Priority levels used by PTA for concurrent (Coex and WLAN) request arbitration.
 */
typedef enum sl_wfx_pta_priority_e {
  SL_WFX_PTA_PRIORITY_COEX_MAXIMIZED             = 0x00000562,  ///< Maximizes priority to COEX
  SL_WFX_PTA_PRIORITY_COEX_HIGH                  = 0x00000462,  ///< High priority to COEX, targets low-latency to COEX
  SL_WFX_PTA_PRIORITY_BALANCED                   = 0x00001461,  ///< Balanced PTA arbitration
  SL_WFX_PTA_PRIORITY_WLAN_HIGH                  = 0x00001851,  ///< High priority to WLAN, protects WLAN transmissions
  SL_WFX_PTA_PRIORITY_WLAN_MAXIMIZED             = 0x00001A51   ///< Maximizes priority to WLAN
} sl_wfx_pta_priority_t;

/**
 * @brief Request sent by the host to define the level of priority used to arbitrate concurrent Coex and WLAN requests.
 *
 * Priority can be one value from enum ::sl_wfx_pta_priority_t but can also be an integer value whom definition is the following bitfield:
 *
 * @code{.c}
 * struct sl_wfx_pta_priority_s
 * {
 *   uint32_t coex_prio_low:3;    // Priority given to Coex for low-priority requests
 *   uint32_t reserved_1:1;       // Reserved for future use
 *   uint32_t coex_prio_high:3;   // Priority given to Coex for high-priority requests
 *   uint32_t reserved_2:1;       // Reserved for future use
 *   uint32_t grant_coex:1;       // Allows Coex to override WLAN
 *   uint32_t grant_wlan:1;       // Allows WLAN to override Coex whenever WLAN is not idle
 *   uint32_t protect_coex:1;     // WLAN grant is delayed until Coex has finished its present granted transaction
 *   uint32_t protect_wlan_tx:1;  // Prevents Coex from being granted when WLAN is transmitting (the protection is also extended to the response)
 *   uint32_t protect_wlan_rx:1;  // Prevents Coex from being granted when WLAN is receiving or waiting for a response to an already transmitted frame
 *   uint32_t reserved_3:19;      // Reserved for future use
 * }
 * @endcode
 *
 * @note Request will fail if PTA is started.
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_priority_req_body_s {
  uint32_t priority;                                       ///< The priority level, see enum ::sl_wfx_pta_priority_t
} sl_wfx_pta_priority_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_priority_req_s {
  sl_wfx_header_t header;
  sl_wfx_pta_priority_req_body_t body;
} sl_wfx_pta_priority_req_t;

/**
 * @brief Confirmation sent by WLAN firmware after a ::SL_WFX_PTA_PRIORITY_REQ_ID request.
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_priority_cnf_body_s {
  uint32_t status;                                         ///< Confirmation status, see enum ::sl_wfx_status_t
} sl_wfx_pta_priority_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_priority_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_pta_priority_cnf_body_t body;
} sl_wfx_pta_priority_cnf_t;

/**
 * @brief PTA states.
 */
typedef enum sl_wfx_pta_state_e {
  SL_WFX_PTA_OFF                                 = 0,      ///< PTA is off
  SL_WFX_PTA_ON                                  = 1       ///< PTA is on
} sl_wfx_pta_state_t;

/**
 * @brief Request sent by host to start or stop the PTA.
 * @note Starting PTA will fail if not first configured with ::sl_wfx_pta_settings_req_body_t request.
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_state_req_body_s {
  uint32_t pta_state;                                      ///< Requested PTA state, see enum ::sl_wfx_pta_state_t
} sl_wfx_pta_state_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_state_req_s {
  sl_wfx_header_t header;
  sl_wfx_pta_state_req_body_t body;
} sl_wfx_pta_state_req_t;

/**
 * @brief Confirmation sent by WLAN firmware after a ::SL_WFX_PTA_STATE_REQ_ID request.
 */
typedef struct __attribute__((__packed__)) sl_wfx_pta_state_cnf_body_s {
  uint32_t status;                                         ///< Confirmation status, see enum ::sl_wfx_status_t
} sl_wfx_pta_state_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_pta_state_cnf_s {
  sl_wfx_header_t header;
  sl_wfx_pta_state_cnf_body_t body;
} sl_wfx_pta_state_cnf_t;

/**
 * @}
 */
/* end of PTA section */

/**
 * @brief CCA Mode definition
 * */
typedef enum sl_wfx_cc_thr_mode_e {
  SL_WFX_CCA_THR_MODE_RELATIVE = 0x0,                      ///< Use CCA defer threshold relative to channel noise
  SL_WFX_CCA_THR_MODE_ABSOLUTE = 0x1                       ///< Use absolute CCA defer threshold
} sl_wfx_cc_thr_mode_t;

/**
 * @brief Request sent by host to set the Clear Channel Assessment configuration
 * Set the CCA mode and the defer threshold
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_cca_config_req_body_s {
  uint8_t  cca_thr_mode;                                   ///< CCA threshold mode. See enum ::sl_wfx_cc_thr_mode_t.
  uint8_t  reserved[3];                                    ///< reserved for future use, set to 0
} sl_wfx_set_cca_config_req_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_set_cca_config_req_s {
  sl_wfx_header_t header;
  sl_wfx_set_cca_config_req_body_t body;
} sl_wfx_set_cca_config_req_t;

/**
 * @brief Confirmation sent by Wlan firmware after a ::SL_WFX_SET_CCA_CONFIG_REQ_ID request.
 */
typedef struct __attribute__((__packed__)) sl_wfx_set_cca_config_cnf_body_s {
  uint32_t status;                                         ///< Confirmation status, see enum ::sl_wfx_status_t
} sl_wfx_set_cca_config_cnf_body_t;

typedef struct __attribute__((__packed__)) sl_wfx_set_cca_config_s {
  sl_wfx_header_t header;
  sl_wfx_set_cca_config_cnf_body_t body;
} sl_wfx_set_cca_config_t;

/**
 * @}
 */
/*end of GENERAL_API */

#endif /* _SL_WFX_GENERAL_API_H_ */
