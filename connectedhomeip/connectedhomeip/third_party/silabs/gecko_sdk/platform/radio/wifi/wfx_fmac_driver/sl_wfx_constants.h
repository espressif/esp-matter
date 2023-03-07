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

#ifndef SL_WFX_CONSTANTS_H
#define SL_WFX_CONSTANTS_H

#include "sl_wfx_configuration_defaults.h"
#include "sl_status.h"
#include "sl_wfx_api.h"
#include <stdint.h>

/******************************************************
*                      Macros
******************************************************/

#define SL_WFX_UNUSED_VARIABLE(x) (void)(x)
#define SL_WFX_UNUSED_PARAMETER(x) (void)(x)

#ifndef SL_WFX_ARRAY_COUNT
#define SL_WFX_ARRAY_COUNT(x) (sizeof (x) / sizeof *(x))
#endif /* ifndef SL_WFX_ARRAY_COUNT */

#ifndef SL_WFX_ROUND_UP
#define SL_WFX_ROUND_UP(x, y)    ((x) % (y) ? (x) + (y) - ((x) % (y)) : (x))
#endif /* ifndef SL_WFX_ROUND_UP */

#ifndef SL_WFX_ROUND_UP_EVEN
#define SL_WFX_ROUND_UP_EVEN(x)    ((x) + ((x) & 1))
#endif /* ifndef SL_WFX_ROUND_UP_EVEN */

#define SL_WAIT_FOREVER  0xFFFFFFFF

#define SL_WFX_ERROR_CHECK(__status__)               \
  do {                                               \
    if (((sl_status_t)__status__) != SL_STATUS_OK) { \
      goto error_handler;                            \
    }                                                \
  } while (0)

// little endian has Least Significant Byte First
#define sl_wfx_pack_32bit_little_endian(_ptr_, _val_) do {    \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0x000000FF);       \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x0000FF00) >> 8;  \
    *((uint8_t *)(_ptr_) + 2) = ((_val_) & 0x00FF0000) >> 16; \
    *((uint8_t *)(_ptr_) + 3) = ((_val_) & 0xFF000000) >> 24; \
} while (0)

#define sl_wfx_unpack_32bit_little_endian(_ptr_)    \
  (((uint32_t)(*((uint8_t *)(_ptr_)      )))        \
   | ((uint32_t)(*((uint8_t *)(_ptr_) + 1))) << 8   \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 2))) << 16 \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 3))) << 24)

#define sl_wfx_pack_24bit_little_endian(_ptr_, _val_) do {    \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0x000000FF);       \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x0000FF00) >> 8;  \
    *((uint8_t *)(_ptr_) + 2) = ((_val_) & 0x00FF0000) >> 16; \
} while (0)

#define sl_wfx_unpack_24bit_little_endian(_ptr_)  \
  (((uint32_t)(*((uint8_t *)(_ptr_)      )))      \
   | ((uint32_t)(*((uint8_t *)(_ptr_) + 1))) << 8 \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 2))) << 16)

#define sl_wfx_pack_16bit_little_endian(_ptr_, _val_) do {   \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0x000000FF);      \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x0000FF00) >> 8; \
} while (0)

#define sl_wfx_unpack_16bit_little_endian(_ptr_) \
  (((uint32_t)(*((uint8_t *)(_ptr_)      )))     \
   | ((uint32_t)(*((uint8_t *)(_ptr_) + 1))) << 8)

// big endian has Most Significant Byte First
#define sl_wfx_pack_32bit_big_endian(_ptr_, _val_) do {       \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0xFF000000) >> 24; \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x00FF0000) >> 16; \
    *((uint8_t *)(_ptr_) + 2) = ((_val_) & 0x0000FF00) >> 8;  \
    *((uint8_t *)(_ptr_) + 3) = ((_val_) & 0x000000FF);       \
} while (0)

#define sl_wfx_unpack_32bit_big_endian(_ptr_)       \
  (((uint32_t)(*((uint8_t *)(_ptr_)       ))) << 24 \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 1))) << 16 \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 2))) << 8  \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 3))))

#define sl_wfx_pack_24bit_big_endian(_ptr_, _val_) do {       \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0x00FF0000) >> 16; \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x0000FF00) >> 8;  \
    *((uint8_t *)(_ptr_) + 2) = ((_val_) & 0x000000FF);       \
} while (0)

#define sl_wfx_unpack_24bit_big_endian(_ptr_)       \
  (((uint32_t)(*((uint8_t *)(_ptr_)       ))) << 16 \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 1))) << 8  \
    | ((uint32_t)(*((uint8_t *)(_ptr_) + 2))))

#define sl_wfx_pack_16bit_big_endian(_ptr_, _val_) do {      \
    *((uint8_t *)(_ptr_)    ) = ((_val_) & 0x0000FF00) >> 8; \
    *((uint8_t *)(_ptr_) + 1) = ((_val_) & 0x000000FF);      \
} while (0)

#define sl_wfx_unpack_16bit_big_endian(_ptr_)     \
  (((uint16_t)(*((uint8_t *)(_ptr_)      ))) << 8 \
    | ((uint16_t)(*((uint8_t *)(_ptr_) + 1))))

#define sl_wfx_unpack_8bit(_ptr_) \
  ((uint8_t)(*((uint8_t *)(_ptr_))))

#define sl_wfx_swap_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define sl_wfx_swap_32(x) ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

static inline uint16_t bswap_16(uint16_t bsx)
{
  return sl_wfx_swap_16(bsx);
}

static inline uint32_t bswap_32(uint32_t bsx)
{
  return sl_wfx_swap_32(bsx);
}

static inline uint16_t uint16_identity(uint16_t x)
{
  return x;
}

static inline uint32_t uint32_identity(uint32_t x)
{
  return x;
}

#ifdef SL_WFX_BIG_ENDIAN
#define sl_wfx_htobe16(x) uint16_identity(x)
#define sl_wfx_htole16(x) bswap_16(x)
#define sl_wfx_htobe32(x) uint32_identity(x)
#define sl_wfx_htole32(x) bswap_32(x)
#else
#define sl_wfx_htobe16(x) bswap_16(x)
#define sl_wfx_htole16(x) uint16_identity(x)
#define sl_wfx_htobe32(x) bswap_32(x)
#define sl_wfx_htole32(x) uint32_identity(x)
#endif

/******************************************************
*                    Constants
******************************************************/

#define SL_WAIT_FOREVER  0xFFFFFFFF

#define SL_WFX_ROUND_UP_VALUE      SL_WFX_SDIO_BLOCK_SIZE

#ifndef SL_WFX_DEBUG_MASK
#define SL_WFX_DEBUG_MASK 0x0000
#endif

#define SL_WFX_DEBUG_ERROR   0x0001
#define SL_WFX_DEBUG_INIT    0x0002
#define SL_WFX_DEBUG_SLEEP   0x0004
#define SL_WFX_DEBUG_SLK     0x0008
#define SL_WFX_DEBUG_RX      0x0010
#define SL_WFX_DEBUG_RX_RAW  0x0020
#define SL_WFX_DEBUG_RX_REG  0x0040
#define SL_WFX_DEBUG_TX      0x0080
#define SL_WFX_DEBUG_TX_RAW  0x0100
#define SL_WFX_DEBUG_TX_REG  0x0200
#define SL_WFX_DEBUG_FW_LOAD 0x0400

#define SL_WFX_PDS_KEY_A                 'a'
#define SL_WFX_PDS_KEY_B                 'b'
#define SL_WFX_PDS_KEY_C                 'c'
#define SL_WFX_PDS_KEY_D                 'd'
#define SL_WFX_PDS_KEY_E                 'e'
#define SL_WFX_PDS_KEY_F                 'f'
#define SL_WFX_PDS_ANTENNA_SEL_KEY       'j'

#define SL_WFX_PTE_INFO                  0x0900C0C0
#define SL_WFX_MSG_ID_GENERAL_API_MASK   0x20
#define SL_WFX_MSG_INFO_INTERFACE_OFFSET 1
#define SL_WFX_MSG_INFO_INTERFACE_MASK   0x06
#define SL_WFX_OPN_SIZE                  14

#ifdef SL_WFX_USE_SECURE_LINK
#define SL_WFX_MSG_INFO_SECURE_LINK_OFFSET 6
#define SL_WFX_MSG_INFO_SECURE_LINK_MASK   0xC0
/* Secure link constants*/
#define SL_WFX_SECURE_LINK_MAC_KEY_LENGTH         32

#define SL_WFX_SECURE_LINK_SESSION_KEY_LENGTH          (16)

#define SL_WFX_SECURE_LINK_ENCRYPTION_BITMAP_SIZE      (32)
#define SL_WFX_SECURE_LINK_ENCRYPTION_NOT_REQUIRED     (0)
#define SL_WFX_SECURE_LINK_ENCRYPTION_REQUIRED         (1)

#define SL_WFX_SECURE_LINK_HEADER_SIZE                 (4)
#define SL_WFX_SECURE_LINK_CCM_TAG_SIZE                (16)
#define SL_WFX_SECURE_LINK_NONCE_SIZE_BYTES            (12)
#define SL_WFX_SECURE_LINK_NONCE_COUNTER_MAX           (0x3FFFFFFFUL)  // Top two bits are used to indicate which counter
#define SL_WFX_SECURE_LINK_OVERHEAD                    (SL_WFX_SECURE_LINK_HEADER_SIZE + SL_WFX_SECURE_LINK_CCM_TAG_SIZE)

#define SL_WFX_SECURE_LINK_SESSION_KEY_BIT_COUNT       (SL_WFX_SECURE_LINK_SESSION_KEY_LENGTH * 8)

// Maximum nonce value is 2 ^ 30, watermark is chosen as 2 ^ 29
#define SL_WFX_SECURE_LINK_NONCE_MAX_VALUE             1 << 30
#define SL_WFX_SECURE_LINK_NONCE_WATERMARK             1 << 29
#endif //SL_WFX_USE_SECURE_LINK

#define SL_WFX_ERROR_LOGS {                                                                                          \
    { SL_WFX_ERROR_FIRMWARE_ROLLBACK, "Rollback error", 0 },                                                         \
    { SL_WFX_ERROR_DEPRECATED_0, "Not used anymore", 0 },                                                            \
    { SL_WFX_ERROR_DEPRECATED_1, "Not used anymore", 0 },                                                            \
    { SL_WFX_ERROR_INVALID_SESSION_KEY, "Session key is invalid", 0 },                                               \
    { SL_WFX_ERROR_OOR_VOLTAGE, "Out-of-range power supply voltage", 4 },                                            \
    { SL_WFX_ERROR_PDS_VERSION, "Wrong PDS version", 0 },                                                            \
    { SL_WFX_ERROR_OOR_TEMPERATURE, "Out-of-range temperature", 0 },                                                 \
    { SL_WFX_ERROR_REQ_DURING_KEY_EXCHANGE, "Requests from Host are forbidden until the end of key exchange", 0 },   \
    { SL_WFX_ERROR_DEPRECATED_2, "Not used anymore", 0 },                                                            \
    { SL_WFX_ERROR_DEPRECATED_3, "Not used anymore", 0 },                                                            \
    { SL_WFX_ERROR_SECURELINK_DECRYPTION, "Error occured during message decryption", 0 },                            \
    { SL_WFX_ERROR_SECURELINK_WRONG_ENCRYPTION_STATE, "Encryption state of the received message doesn't match", 4 }, \
    { SL_WFX_SPI_OR_SDIO_FREQ_TOO_LOW, "Bus clock is too slow (<1kHz)", 0 },                                         \
    { SL_WFX_ERROR_DEPRECATED_4, "Not used anymore", 0 },                                                            \
    { SL_WFX_ERROR_DEPRECATED_5, "Not used anymore", 0 },                                                            \
    { SL_WFX_HIF_BUS_ERROR, "HIF HW has reported an error", 4 },                                                     \
    { SL_WFX_PDS_TESTFEATURE_MODE_ERROR, "Unknown TestFeatureMode", 0 }                                              \
}

/**************************************************************************//**
 * @addtogroup ENUM
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @enum sl_wfx_register_address_t
 * @brief Enum listing the registers of the WFx solution
 *****************************************************************************/
typedef enum {
  SL_WFX_CONFIG_REG_ID         = 0x0000,
  SL_WFX_CONTROL_REG_ID        = 0x0001,
  SL_WFX_IN_OUT_QUEUE_REG_ID   = 0x0002,
  SL_WFX_AHB_DPORT_REG_ID      = 0x0003,
  SL_WFX_SRAM_BASE_ADDR_REG_ID = 0x0004,
  SL_WFX_SRAM_DPORT_REG_ID     = 0x0005,
  SL_WFX_TSET_GEN_R_W_REG_ID   = 0x0006,
  SL_WFX_FRAME_OUT_REG_ID      = 0x0007,
} sl_wfx_register_address_t;

/**************************************************************************//**
 * @enum sl_wfx_state_t
 * @brief Enum listing the different state of the WFx chip
 *****************************************************************************/
typedef enum {
  SL_WFX_STARTED                 = (1 << 0),
  SL_WFX_STA_INTERFACE_CONNECTED = (1 << 1),
  SL_WFX_AP_INTERFACE_UP         = (1 << 2),
  SL_WFX_SLEEPING                = (1 << 3),
  SL_WFX_POWER_SAVE_ACTIVE       = (1 << 4),
} sl_wfx_state_t;

/**************************************************************************//**
 * @enum sl_wfx_interface_t
 * @brief Enum listing available interfaces in WFx Wi-Fi solution
 * @details For convenience, interface 0 is associated with the station
 * interface and interface 1 is associated with the softap interface.
 *****************************************************************************/
typedef enum {
  SL_WFX_STA_INTERFACE    = 0x00,   ///< Interface 0, linked to the station
  SL_WFX_SOFTAP_INTERFACE = 0x01,   ///< Interface 1, linked to the softap
} sl_wfx_interface_t;

/**************************************************************************//**
 * @enum sl_wfx_antenna_config_t
 * @brief Enum listing the different antenna configurations
 *****************************************************************************/
typedef enum {
  SL_WFX_ANTENNA_1_ONLY = 0,   ///< RF output 1 is used
  SL_WFX_ANTENNA_2_ONLY,       ///< RF output 2 is used
  SL_WFX_ANTENNA_TX1_RX2,      ///< RF output 1 is used for TX, RF 2 for RX
  SL_WFX_ANTENNA_TX2_RX1,      ///< RF output 2 is used for TX, RF 1 for RX
  SL_WFX_ANTENNA_DIVERSITY     ///< WF200 uses an antenna diversity algorithm
} sl_wfx_antenna_config_t;

/**************************************************************************//**
 * @enum sl_wfx_received_message_type_t
 * @brief Enum listing different message types received from WFx. The
 * information is found in the control register using
 * SL_WFX_CONT_FRAME_TYPE_INFO mask.
 *****************************************************************************/
typedef enum {
  SL_WFX_CONFIRMATION_MESSAGE  = 0,   ///< Frame type indicating a confirmation message is available
  SL_WFX_INDICATION_MESSAGE    = 1,   ///< Frame type indicating an indication message is available
  SL_WFX_MANAGEMENT_MESSAGE    = 2,   ///< Reserved from Low MAC interface
  SL_WFX_ETHERNET_DATA_MESSAGE = 3,   ///< Frame type indicating message encapsulating a data frame is available
} sl_wfx_received_message_type_t;

/**************************************************************************//**
 * @enum sl_wfx_buffer_type_t
 * @brief Enumerates the different types of buffer
 *****************************************************************************/
typedef enum {
  SL_WFX_TX_FRAME_BUFFER,
  SL_WFX_RX_FRAME_BUFFER,
  SL_WFX_CONTROL_BUFFER,
} sl_wfx_buffer_type_t;

/**************************************************************************//**
 * @enum sl_wfx_host_bus_transfer_type_t
 * @brief Enumerates the different types of bus transfers
 *****************************************************************************/
typedef enum {
  SL_WFX_BUS_WRITE = (1 << 0),
  SL_WFX_BUS_READ  = (1 << 1),
  SL_WFX_BUS_WRITE_AND_READ = SL_WFX_BUS_WRITE | SL_WFX_BUS_READ,
} sl_wfx_host_bus_transfer_type_t;

#ifdef SL_WFX_USE_SECURE_LINK
/**************************************************************************//**
 * @enum sl_wfx_secure_link_mode_t
 * @brief Enum listing the different secure link mode of a part
 *****************************************************************************/
typedef enum {
  SL_WFX_LINK_MODE_RESERVED      = 0,
  SL_WFX_LINK_MODE_UNTRUSTED     = 1,
  SL_WFX_LINK_MODE_TRUSTED_EVAL  = 2,
  SL_WFX_LINK_MODE_ACTIVE        = 3,
} sl_wfx_secure_link_mode_t;

/**************************************************************************//**
 * @enum sl_wfx_securelink_renegotiation_state_t
 * @brief Enumerates the states of the secure link key renegotiation
 *****************************************************************************/
typedef enum {
  SL_WFX_SECURELINK_DEFAULT               = 0,
  SL_WFX_SECURELINK_RENEGOTIATION_NEEDED  = 1,
  SL_WFX_SECURELINK_RENEGOTIATION_PENDING = 2,
} sl_wfx_securelink_renegotiation_state_t;
#endif //SL_WFX_USE_SECURE_LINK

/** @} end ENUM */

/******************************************************
*                    Structures
******************************************************/

/**************************************************************************//**
 * @struct sl_wfx_mac_address_t
 * @brief Structure to handle MAC address format
 *****************************************************************************/
typedef struct {
  uint8_t octet[6]; ///< Table to store a MAC address
} sl_wfx_mac_address_t;

/**************************************************************************//**
 * @struct sl_wfx_password_t
 * @brief Structure to handle password format
 *****************************************************************************/
typedef struct {
  uint8_t password[SL_WFX_PASSWORD_SIZE]; ///< Table to store a password
} sl_wfx_password_t;

/**************************************************************************//**
 * @struct sl_wfx_nonce_t
 * @brief Structure to maintain secure link counters
 *****************************************************************************/
typedef struct {
  uint32_t hp_packet_count; ///< High priority packet counter
  uint32_t rx_packet_count; ///< Received packet counter
  uint32_t tx_packet_count; ///< Sent packet counter
} sl_wfx_nonce_t;

/**************************************************************************//**
 * @struct sl_wfx_error_log_t
 * @brief Structure used to display error logs
 *****************************************************************************/
typedef struct {
  uint32_t val;
  const char *str;
  uint8_t param_length;
} sl_wfx_err_log_t;

/**************************************************************************//**
 * @struct sl_wfx_context_t
 * @brief Structure used to maintain the Wi-Fi solution context on the host
 * side
 *****************************************************************************/
typedef struct {
  uint8_t  event_payload_buffer[512];      ///< Event payload associated with the last posted event
  uint8_t  firmware_build;                 ///< Firmware build version
  uint8_t  firmware_minor;                 ///< Firmware minor version
  uint8_t  firmware_major;                 ///< Firmware major version
  uint16_t data_frame_id;                  ///< Frame ID incremented by ::sl_wfx_send_ethernet_frame
  uint16_t used_buffers;                   ///< Number of buffers currently in use by the WFx chip
  uint8_t  wfx_opn[SL_WFX_OPN_SIZE];       ///< OPN of the part
  sl_wfx_mac_address_t mac_addr_0;         ///< Mac address used by WFx interface 0, station
  sl_wfx_mac_address_t mac_addr_1;         ///< Mac address used by WFx interface 1, softap
  sl_wfx_state_t state;                    ///< State of the WFx Wi-Fi chip
#ifdef SL_WFX_USE_SECURE_LINK
  uint8_t  secure_link_mac_key[SL_WFX_SECURE_LINK_MAC_KEY_LENGTH];
  sl_wfx_nonce_t secure_link_nonce;
  uint8_t  encryption_bitmap[SL_WFX_SECURE_LINK_ENCRYPTION_BITMAP_SIZE];
  uint8_t  secure_link_session_key[SL_WFX_SECURE_LINK_SESSION_KEY_LENGTH];
  uint8_t  secure_link_renegotiation_state;
  sl_wfx_securelink_exchange_pub_keys_ind_t  secure_link_exchange_ind;
#endif //SL_WFX_USE_SECURE_LINK
} sl_wfx_context_t;

#endif // SL_WFX_CONSTANTS_H
