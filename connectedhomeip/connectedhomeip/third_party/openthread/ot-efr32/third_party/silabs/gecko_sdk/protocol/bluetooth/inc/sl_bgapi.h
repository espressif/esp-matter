/***************************************************************************//**
 * @brief Silicon Labs BGAPI types and macros
 *******************************************************************************
 * # License
 * <b>Copyright 2019-2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BGAPI_H
#define SL_BGAPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Macros to declare deprecated functions */
#if defined(__IAR_SYSTEMS_ICC__)
  #define SL_BGAPI_DEPRECATED _Pragma("deprecated")
#elif defined(__GNUC__)
  #define SL_BGAPI_DEPRECATED __attribute__((deprecated))
#else
  #define SL_BGAPI_DEPRECATED
#endif

/* Compatibility */
#ifndef PACKSTRUCT
/*Default packed configuration*/
#ifdef __GNUC__
#ifdef _WIN32
#define PACKSTRUCT(decl) decl __attribute__((__packed__, gcc_struct))
#else
#define PACKSTRUCT(decl) decl __attribute__((__packed__))
#endif
#define ALIGNED __attribute__((aligned(0x4)))
#elif __IAR_SYSTEMS_ICC__

#define PACKSTRUCT(decl) __packed decl

#define ALIGNED
#elif _MSC_VER  /*msvc*/

#define PACKSTRUCT(decl) __pragma(pack(push, 1) ) decl __pragma(pack(pop) )
#define ALIGNED
#else
#define PACKSTRUCT(a) a PACKED
#endif
#endif

/**
 * The maximum BGAPI command payload size.
 */
#ifndef SL_BGAPI_MAX_PAYLOAD_SIZE
#define SL_BGAPI_MAX_PAYLOAD_SIZE 256
#endif

/***************************************************************************//**
 * @addtogroup sl_bgapi_types BGAPI Types
 * @brief Common types in BGAPI protocol
 * @{
 */

#ifndef SL_BT_TYPE_UINT8ARRARY
#define SL_BT_TYPE_UINT8ARRARY
/** @brief Variable-length uint8_t array. Maximum length: 255 */
typedef struct {
  uint8_t len;    /**< Number of bytes stored in @p data */
  uint8_t data[]; /**< Data bytes*/
} uint8array;
#endif

#ifndef SL_BT_TYPE_BYTE_ARRARY
#define SL_BT_TYPE_BYTE_ARRARY
/** @brief Variable-length int8_t array. Maximum length: 65535 */
typedef struct {
  uint16_t len;    /**< Number of bytes stored in @p data */
  int8_t   data[]; /**< Data bytes*/
} byte_array;
#endif

#ifndef SL_BT_TYPE_BDADDR
#define SL_BT_TYPE_BDADDR
/** @brief Bluetooth address */
typedef struct {
  uint8_t addr[6]; /**< @brief Bluetooth address in reverse byte order */
} bd_addr;
#endif

#ifndef SL_BT_TYPE_UUID128
#define SL_BT_TYPE_UUID128
/** @brief 128-bit UUID */
typedef struct {
  uint8_t data[16]; /**< 128-bit UUID */
} uuid_128;
#endif

#ifndef SL_BT_TYPE_AES_KEY128
#define SL_BT_TYPE_AES_KEY128
/** @brief 128-bit AES key */
typedef struct {
  uint8_t data[16]; /**< 128-bit AES key */
} aes_key_128;
#endif

#ifndef SL_BT_TYPE_UUID16
#define SL_BT_TYPE_UUID16
/** @brief 16-bit UUID */
typedef struct {
  uint8_t data[2]; /**< 16-bit UUID */
} sl_bt_uuid_16_t;
#endif

#ifndef SL_BT_TYPE_UUID64
#define SL_BT_TYPE_UUID64
/** @brief 64-bit UUID */
typedef struct {
  uint8_t data[8]; /**< 64-bit UUID */
} sl_bt_uuid_64_t;
#endif

/** @} */ // end addtogroup sl_bgapi_types
/******************************************************************************/

/** @brief Internal function prototype for BGAPI command handlers */
typedef void (*sl_bgapi_handler)(const void*);

typedef enum sl_bgapi_msg_types {
  sl_bgapi_msg_type_cmd = 0x00,
  sl_bgapi_msg_type_rsp = 0x00,
  sl_bgapi_msg_type_evt = 0x80
} sl_bgapi_msg_types_t;

enum sl_bgapi_dev_types {
  sl_bgapi_dev_type_bt = 0x20,
  sl_bgapi_dev_type_btmesh = 0x28,
};

/***************************************************************************//**
 * @addtogroup sl_bgapi_types BGAPI Types
 * @brief Common types in BGAPI protocol
 * @{
 */

/**
 * @brief The length of a BGAPI message header which is 4 bytes
 */
#define SL_BGAPI_MSG_HEADER_LEN (4)

/**
 * @brief Get the identifier of a BGAPI message including device type, class ID,
 * message type and message ID.
 *
 * @param HDR The header of the message as a uint32_t integer
 */
#define SL_BGAPI_MSG_ID(HDR) ((HDR) & 0xffff00f8)

/**
 * @brief Get the data payload length in a BGAPI message.
 *
 * @param HDR The header of the message as a uint32_t integer
 */
#define SL_BGAPI_MSG_LEN(HDR) ((((HDR) & 0x7) << 8) | (((HDR) & 0xff00) >> 8))

/** @} */ // end addtogroup sl_bgapi_types
/******************************************************************************/

/**
 * @brief The bit indicating whether data of a BGAPI message is encrypted
 */
#define SL_BGAPI_BIT_ENCRYPTED (1 << 6) // Bit

/**
 * @brief Check whether data of a BGAPI message is encrypted.
 *
 * @param HDR The BGAPI header of the message as a uint32_t integer
 */
#define SL_BGAPI_MSG_ENCRYPTED(HDR) ((HDR)&SL_BGAPI_BIT_ENCRYPTED)

/**
 * Internal function for setting the command handler function. Used by API library.
 *
 * @param cmd_handler_delegate Pointer to command handler delegate function
 */
void sli_bgapi_set_cmd_handler_delegate(void (*cmd_handler_delegate)(uint32_t,
                                                                     sl_bgapi_handler,
                                                                     const void*));

#ifdef __cplusplus
}
#endif

#endif
