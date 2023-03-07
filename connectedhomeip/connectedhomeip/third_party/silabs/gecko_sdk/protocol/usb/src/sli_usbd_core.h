/***************************************************************************//**
 * @file
 * @brief USB Device Core Internal Functions
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SLI_USBD_CORE_H
#define  SLI_USBD_CORE_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#include "sl_enum.h"

#include "sl_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  SLI_USBD_CORE_MODULE
#define  SLI_USBD_CORE_EXTERN
#else
#define  SLI_USBD_CORE_EXTERN  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

// Fix the maximum quantity of usb device to 1
#define SL_USBD_DEVICE_QUANTITY 1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define SLI_USBD_SINGLE_BIT_MASK_16(bit)    ((uint16_t)((uint16_t)1u << (bit)))

#define SLI_USBD_SINGLE_BIT_MASK_32(bit)    ((uint32_t)((uint32_t)1u << (bit)))

/****************************************************************************************************//**
 *                                                   SLI_USBD_GET_MIN()
 *
 * @brief    Determine the minimum of two values.
 *
 * @param    a   First  value.
 *
 * @param    b   Second value.
 *
 * @return   Minimum of the two values.
 *******************************************************************************************************/

#define SLI_USBD_GET_MIN(a, b)    (((a) < (b)) ? (a) : (b))

/****************************************************************************************************//**
 *                                               SLI_USBD_IS_PWR2()
 *
 * @brief    Determine if a value is a power of 2.
 *
 * @param    nbr     Value.
 *
 * @return   true,  'nbr' is a power of 2.
 *           false, 'nbr' is not a power of 2.
 *******************************************************************************************************/

#define SLI_USBD_IS_PWR2(nbr)    ((((nbr) != 0u) && (((nbr) & ((nbr) - 1u)) == 0u)) ? true : false)

/****************************************************************************************************//**
 *                                           SLI_USBD_ROUND_INC_UP_PWR2()
 *
 * @brief    Round value up to the next (power of 2) increment.
 *
 * @param    nbr     Value to round.
 *
 * @param    inc     Increment to use. MUST be a power of 2.
 *
 * @return   Rounded up value.
 *******************************************************************************************************/

#define SLI_USBD_ROUND_INC_UP_PWR2(nbr, inc)    (((nbr) & ~((inc) - 1)) + (((nbr) & ((inc) - 1)) == 0 ? 0 : (inc)))

/****************************************************************************************************//**
 *                                         SLI_USBD_ASCII_IS_DIG_HEX()
 *
 * @brief    Determine whether a character is a hexadecimal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   true, the character is a hexadecimal-digit character.
 *           false, the character is NOT a hexadecimal-digit character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.12.(2) states that "isxdigit() ... tests for any
 *               hexadecimal-digit character".
 *******************************************************************************************************/

#define SLI_USBD_ASCII_CHAR_DIGIT_ZERO       0x30    ///< '0'
#define SLI_USBD_ASCII_CHAR_DIGIT_NINE       0x39    ///< '9'
#define SLI_USBD_ASCII_CHAR_LATIN_UPPER_A    0x41    ///< 'A'
#define SLI_USBD_ASCII_CHAR_LATIN_UPPER_F    0x46    ///< 'F'
#define SLI_USBD_ASCII_CHAR_LATIN_UPPER_Z    0x5A    ///< 'Z'
#define SLI_USBD_ASCII_CHAR_LATIN_LOWER_A    0x61    ///< 'a'
#define SLI_USBD_ASCII_CHAR_LATIN_LOWER_F    0x66    ///< 'f'
#define SLI_USBD_ASCII_CHAR_LATIN_LOWER_Z    0x7A    ///< 'z'

#define SLI_USBD_ASCII_IS_DIG_HEX(c)    (((((c) >= SLI_USBD_ASCII_CHAR_DIGIT_ZERO) && ((c) <= SLI_USBD_ASCII_CHAR_DIGIT_NINE))          \
                                          || (((c) >= SLI_USBD_ASCII_CHAR_LATIN_UPPER_A) && ((c) <= SLI_USBD_ASCII_CHAR_LATIN_UPPER_F)) \
                                          || (((c) >= SLI_USBD_ASCII_CHAR_LATIN_LOWER_A) && ((c) <= SLI_USBD_ASCII_CHAR_LATIN_LOWER_F))) ? (true) : (false))

/****************************************************************************************************//**
 *                                            SLI_USBD_ASCII_IS_UPPER()
 *
 * @brief    Determines whether a character is an uppercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   true, the character is uppercase alphabetic.
 *           false, the character is uppercase alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.11.(2) states that "isupper() returns true only for
 *               the uppercase letters".
 *******************************************************************************************************/

#define SLI_USBD_ASCII_IS_UPPER(c)    ((((c) >= SLI_USBD_ASCII_CHAR_LATIN_UPPER_A) && ((c) <= SLI_USBD_ASCII_CHAR_LATIN_UPPER_Z)) ? (true) : (false))

/****************************************************************************************************//**
 *                                            SLI_USBD_ASCII_IS_LOWER()
 *
 * @brief    Determines whether a character is a lowercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   true, the character is lowercase alphabetic.
 *           false, the character is lowercase alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.7.(2)  states that "islower() returns true only for
 *               the lowercase letters".
 *******************************************************************************************************/

#define SLI_USBD_ASCII_IS_LOWER(c)    ((((c) >= SLI_USBD_ASCII_CHAR_LATIN_LOWER_A) && ((c) <= SLI_USBD_ASCII_CHAR_LATIN_LOWER_Z)) ? (true) : (false))

/****************************************************************************************************//**
 *                                             SLI_USBD_ASCII_IS_ALPHA()
 *
 * @brief    Determines whether a character is an alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   true, the character is alphabetic.
 *           false, the character is NOT alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.2.(2) states that "isalpha() returns true only for the
 *               characters for which isupper() or islower() is true".
 *******************************************************************************************************/

#define SLI_USBD_ASCII_IS_ALPHA(c)    ((((SLI_USBD_ASCII_IS_UPPER(c)) == true) \
                                        || ((SLI_USBD_ASCII_IS_LOWER(c)) == true)) ? (true) : (false))

/****************************************************************************************************//**
 *                                        SLI_USBD_MEM_VAL_SET_INTxx_BIG()
 *
 * @brief    Encode big-endian data values to any CPU memory address.
 *
 * @param    addr    Lowest CPU memory address to encode data value.
 *
 * @param    val     Data value to encode.
 *******************************************************************************************************/

#define SLI_USBD_MEM_VAL_SET_INT16U_BIG(addr, val)    do {                                            \
    (*(((uint8_t *)(addr)) + 0)) = ((uint8_t)((((uint16_t)(val)) & (uint16_t)0xFF00u) >> (1u * 8u))); \
    (*(((uint8_t *)(addr)) + 1)) = ((uint8_t)((((uint16_t)(val)) & (uint16_t)0x00FFu) >> (0u * 8u))); \
} while (0)

#define SLI_USBD_MEM_VAL_SET_INT32U_BIG(addr, val)    do {                                                \
    (*(((uint8_t *)(addr)) + 0)) = ((uint8_t)((((uint32_t)(val)) & (uint32_t)0xFF000000u) >> (3u * 8u))); \
    (*(((uint8_t *)(addr)) + 1)) = ((uint8_t)((((uint32_t)(val)) & (uint32_t)0x00FF0000u) >> (2u * 8u))); \
    (*(((uint8_t *)(addr)) + 2)) = ((uint8_t)((((uint32_t)(val)) & (uint32_t)0x0000FF00u) >> (1u * 8u))); \
    (*(((uint8_t *)(addr)) + 3)) = ((uint8_t)((((uint32_t)(val)) & (uint32_t)0x000000FFu) >> (0u * 8u))); \
} while (0)

/****************************************************************************************************//**
 *                                         SLI_USBD_MEM_VAL_GET_INTxxU_BIG()
 *
 * @brief    Decode big-endian data values from any CPU memory address.
 *
 * @param    addr    Lowest CPU memory address of data value to decode.
 *
 * @return   Decoded data value from CPU memory address.
 *******************************************************************************************************/

#define SLI_USBD_MEM_VAL_GET_INT16U_BIG(addr)                                     \
  ((uint16_t)(((uint16_t)(((uint16_t)(*(((uint8_t *)(addr)) + 0))) << (1u * 8u))) \
              + ((uint16_t)(((uint16_t)(*(((uint8_t *)(addr)) + 1))) << (0u * 8u)))))

#define SLI_USBD_MEM_VAL_GET_INT32U_BIG(addr)                                       \
  ((uint32_t)(((uint32_t)(((uint32_t)(*(((uint8_t *)(addr)) + 0))) << (3u * 8u)))   \
              + ((uint32_t)(((uint32_t)(*(((uint8_t *)(addr)) + 1))) << (2u * 8u))) \
              + ((uint32_t)(((uint32_t)(*(((uint8_t *)(addr)) + 2))) << (1u * 8u))) \
              + ((uint32_t)(((uint32_t)(*(((uint8_t *)(addr)) + 3))) << (0u * 8u)))))

/****************************************************************************************************//**
 *                                       MEM_VAL_COPY_GET_INTU_BIG()
 *
 * @brief    Copy & decode big-endian data values from any CPU memory address to any CPU memory address
 *           for any sized data values.
 *
 * @param    addr_dest   Lowest CPU memory address to copy/decode source address's data value.
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy/decode.
 *
 * @param    val_size    Number of data value octets to copy/decode.
 *******************************************************************************************************/

#define SLI_USBD_MEM_VAL_COPY_GET_INTU_BIG(addr_dest, addr_src, val_size)  do { \
    size_t _i;                                                                  \
    size_t _j;                                                                  \
                                                                                \
    _j = (val_size) - 1;                                                        \
                                                                                \
    for (_i = 0; _i < (val_size); _i++) {                                       \
      (*(((uint8_t *)(addr_dest)) + _i)) = (*(((uint8_t *)(addr_src)) + _j));   \
      _j--;                                                                     \
    }                                                                           \
} while (0)

#define SLI_USBD_MEM_VAL_COPY_SET_INTU_BIG(addr_dest, addr_src, val_size)  SLI_USBD_MEM_VAL_COPY_GET_INTU_BIG((addr_dest), (addr_src), (val_size))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SLI_USBD_CFG_DBG_STATS_EN
#define  SLI_USBD_CFG_DBG_STATS_EN                             0u
#endif

// FIXME: decide what to do with LOG macros
#define   SLI_USBD_LOG_DBG(...)
#define   SLI_USBD_LOG_VRB(...)
#define   SLI_USBD_LOG_ERR(...)
#define   SLI_USBD_LOG_DBG_TO(...)
#define   SLI_USBD_LOG_VRB_TO(...)
#define   SLI_USBD_LOG_ERR_TO(...)

/********************************************************************************************************
 *                                               DEBUG STATS
 *******************************************************************************************************/

#if (SLI_USBD_CFG_DBG_STATS_EN == 1)
#ifdef  USBD_CFG_DBG_STATS_CNT_TYPE                             // Adjust size of the stats cntrs.
typedef USBD_CFG_DBG_STATS_CNT_TYPE sl_usbd_debug_stats_count_t;
#else
typedef uint8_t sl_usbd_debug_stats_count_t;
#endif

/// Device Stats
typedef struct {
  sl_usbd_debug_stats_count_t device_reset_event_nbr;                          ///< nbr of reset   events.
  sl_usbd_debug_stats_count_t device_suspend_event_nbr;                        ///< nbr of suspend events.
  sl_usbd_debug_stats_count_t device_resume_event_nbr;                         ///< nbr of resume  events.
  sl_usbd_debug_stats_count_t device_conn_event_nbr;                           ///< nbr of conn    events.
  sl_usbd_debug_stats_count_t device_disconnect_event_nbr;                     ///< nbr of disconn events.
  sl_usbd_debug_stats_count_t device_setup_event_nbr;                          ///< nbr of setup   events.

  sl_usbd_debug_stats_count_t std_req_device_nbr;                              ///< nbr of         std req with a recipient of 'dev'.
  sl_usbd_debug_stats_count_t std_req_device_stall_nbr;                        ///< nbr of stalled std req with a recipient of 'dev'.
  sl_usbd_debug_stats_count_t std_req_interface_nbr;                           ///< nbr of         std req with a recipient of 'IF'.
  sl_usbd_debug_stats_count_t std_req_interface_stall_nbr;                     ///< nbr of stalled std req with a recipient of 'IF'.
  sl_usbd_debug_stats_count_t std_req_endpoint_nbr;                            ///< nbr of         std req with a recipient of 'EP'.
  sl_usbd_debug_stats_count_t std_req_endpoint_stall_nbr;                      ///< nbr of stalled std req with a recipient of 'EP'.
  sl_usbd_debug_stats_count_t std_req_class_nbr;                               ///< nbr of         std req with a recipient of 'class'.
  sl_usbd_debug_stats_count_t std_req_class_stall_nbr;                         ///< nbr of stalled std req with a recipient of 'class'.

  sl_usbd_debug_stats_count_t std_req_set_address_nbr;                         ///< nbr of SET_ADDRESS       std req.
  sl_usbd_debug_stats_count_t std_req_set_configuration_nbr;                   ///< nbr of SET_CONFIGURATION std req.

  sl_usbd_debug_stats_count_t ctrl_rx_sync_exec_nbr;                           ///< nbr of sync ctrl rx exec'd.
  sl_usbd_debug_stats_count_t ctrl_rx_sync_success_nbr;                        ///< nbr of sync ctrl rx exec'd successfully.
  sl_usbd_debug_stats_count_t ctrl_tx_sync_exec_nbr;                           ///< nbr of sync ctrl tx exec'd.
  sl_usbd_debug_stats_count_t ctrl_tx_sync_success_nbr;                        ///< nbr of sync ctrl tx exec'd successfully.
  sl_usbd_debug_stats_count_t ctrl_rx_status_exec_nbr;                         ///< nbr of sync ctrl rx status exec'd.
  sl_usbd_debug_stats_count_t ctrl_rx_status_success_nbr;                      ///< nbr of sync ctrl rx status exec'd successfully.
  sl_usbd_debug_stats_count_t ctrl_tx_status_exec_nbr;                         ///< nbr of sync ctrl tx status exec'd.
  sl_usbd_debug_stats_count_t ctrl_tx_status_success_nbr;                      ///< nbr of sync ctrl tx status exec'd successfully.

  sl_usbd_debug_stats_count_t bulk_rx_sync_exec_nbr;                           ///< nbr of sync  bulk rx exec'd.
  sl_usbd_debug_stats_count_t bulk_rx_sync_success_nbr;                        ///< nbr of sync  bulk rx exec'd successfully.
  sl_usbd_debug_stats_count_t bulk_rx_async_exec_nbr;                          ///< nbr of async bulk rx exec'd.
  sl_usbd_debug_stats_count_t bulk_rx_async_success_nbr;                       ///< nbr of async bulk rx exec'd successfully.
  sl_usbd_debug_stats_count_t bulk_tx_sync_exec_nbr;                           ///< nbr of sync  bulk tx exec'd.
  sl_usbd_debug_stats_count_t bulk_tx_sync_success_nbr;                        ///< nbr of sync  bulk tx exec'd successfully.
  sl_usbd_debug_stats_count_t bulk_tx_async_exec_nbr;                          ///< nbr of async bulk tx exec'd.
  sl_usbd_debug_stats_count_t bulk_tx_async_success_nbr;                       ///< nbr of async bulk tx exec'd successfully.

  sl_usbd_debug_stats_count_t interrupt_rx_sync_exec_nbr;                      ///< nbr of sync  intr rx exec'd.
  sl_usbd_debug_stats_count_t interrupt_rx_sync_success_nbr;                   ///< nbr of sync  intr rx exec'd successfully.
  sl_usbd_debug_stats_count_t interrupt_rx_async_exec_nbr;                     ///< nbr of async intr rx exec'd.
  sl_usbd_debug_stats_count_t interrupt_rx_async_success_nbr;                  ///< nbr of async intr rx exec'd successfully.
  sl_usbd_debug_stats_count_t interrupt_tx_sync_exec_nbr;                      ///< nbr of sync  intr tx exec'd.
  sl_usbd_debug_stats_count_t interrupt_tx_sync_success_nbr;                   ///< nbr of sync  intr tx exec'd successfully.
  sl_usbd_debug_stats_count_t interrupt_tx_async_exec_nbr;                     ///< nbr of async intr tx exec'd.
  sl_usbd_debug_stats_count_t interrupt_tx_async_success_nbr;                  ///< nbr of async intr tx exec'd successfully.

#if (USBD_CFG_EP_ISOC_EN == 1)
  sl_usbd_debug_stats_count_t isochronous_rx_async_exec_nbr;                   ///< nbr of async isoc rx exec'd.
  sl_usbd_debug_stats_count_t isochronous_rx_async_success_nbr;                ///< nbr of async isoc rx exec'd successfully.
  sl_usbd_debug_stats_count_t isochronous_tx_async_exec_nbr;                   ///< nbr of async isoc tx exec'd.
  sl_usbd_debug_stats_count_t isochronous_tx_async_success_nbr;                ///< nbr of async isoc tx exec'd successfully.
#endif
} sl_usbd_debug_device_stats_t;

/// EP Stats
typedef struct {
  uint8_t         address;                                                     ///< EP address.

  sl_usbd_debug_stats_count_t endpoint_open_nbr;                               ///< nbr of times this EP addr has been opened.
  sl_usbd_debug_stats_count_t endpoint_abort_exec_nbr;                         ///< nbr of times EP has been aborted.
  sl_usbd_debug_stats_count_t endpoint_abort_success_nbr;                      ///< nbr of times EP has been aborted successfully.
  sl_usbd_debug_stats_count_t endpoint_close_exec_nbr;                         ///< nbr of times EP has been closed.
  sl_usbd_debug_stats_count_t endpoint_close_success_nbr;                      ///< nbr of times EP has been closed successfully.

  sl_usbd_debug_stats_count_t rx_sync_exec_nbr;                                ///< nbr of sync  rx exec'd.
  sl_usbd_debug_stats_count_t rx_sync_success_nbr;                             ///< nbr of sync  rx exec'd successfully.
  sl_usbd_debug_stats_count_t rx_sync_timeout_err_nbr;                         ///< nbr of sync  rx that timed-out.
  sl_usbd_debug_stats_count_t rx_async_exec_nbr;                               ///< nbr of async rx exec'd.
  sl_usbd_debug_stats_count_t rx_async_success_nbr;                            ///< nbr of async rx exec'd successfully.
  sl_usbd_debug_stats_count_t rx_zlp_exec_nbr;                                 ///< nbr of sync  rx ZLP exec'd.
  sl_usbd_debug_stats_count_t rx_zlp_success_nbr;                              ///< nbr of sync  rx ZLP exec'd successfully.

  sl_usbd_debug_stats_count_t tx_sync_exec_nbr;                                ///< nbr of sync  tx exec'd.
  sl_usbd_debug_stats_count_t tx_sync_success_nbr;                             ///< nbr of sync  tx exec'd successfully.
  sl_usbd_debug_stats_count_t tx_sync_timeout_err_nbr;                         ///< nbr of sync  tx that timed-out.
  sl_usbd_debug_stats_count_t tx_async_exec_nbr;                               ///< nbr of async tx exec'd.
  sl_usbd_debug_stats_count_t tx_async_success_nbr;                            ///< nbr of async tx exec'd successfully.
  sl_usbd_debug_stats_count_t tx_zlp_exec_nbr;                                 ///< nbr of sync  tx ZLP exec'd.
  sl_usbd_debug_stats_count_t tx_zlp_success_nbr;                              ///< nbr of sync  tx ZLP exec'd successfully.

  sl_usbd_debug_stats_count_t driver_rx_start_nbr;                             ///< nbr of            call to drv's RxStart().
  sl_usbd_debug_stats_count_t driver_rx_start_success_nbr;                     ///< nbr of successful call to drv's RxStart().
  sl_usbd_debug_stats_count_t driver_rx_nbr;                                   ///< nbr of            call to drv's Rx().
  sl_usbd_debug_stats_count_t driver_rx_success_nbr;                           ///< nbr of successful call to drv's Rx().
  sl_usbd_debug_stats_count_t driver_rx_zlp_nbr;                               ///< nbr of            call to drv's RxZLP().
  sl_usbd_debug_stats_count_t driver_rx_zlp_success_nbr;                       ///< nbr of successful call to drv's RxZLP().
  sl_usbd_debug_stats_count_t rx_cmpl_nbr;                                     ///< nbr of            call to RxCmpl().
  sl_usbd_debug_stats_count_t rx_cmpl_err_nbr;                                 ///< nbr of successful call to RxCmpl().

  sl_usbd_debug_stats_count_t driver_tx_nbr;                                   ///< nbr of            call to drv's Tx().
  sl_usbd_debug_stats_count_t driver_tx_success_nbr;                           ///< nbr of successful call to drv's Tx().
  sl_usbd_debug_stats_count_t driver_tx_start_nbr;                             ///< nbr of            call to drv's TxStart().
  sl_usbd_debug_stats_count_t driver_tx_start_success_nbr;                     ///< nbr of successful call to drv's TxStart().
  sl_usbd_debug_stats_count_t driver_tx_zlp_nbr;                               ///< nbr of            call to drv's TxZLP().
  sl_usbd_debug_stats_count_t driver_tx_zlp_success_nbr;                       ///< nbr of successful call to drv's TxZLP().
  sl_usbd_debug_stats_count_t tx_cmpl_nbr;                                     ///< nbr of            call to TxCmpl().
  sl_usbd_debug_stats_count_t tx_cmpl_err_nbr;                                 ///< nbr of successful call to TxCmpl().
} sl_usbd_debug_endpoint_stats_t;

SLI_USBD_CORE_EXTERN sl_usbd_debug_device_stats_t usbd_debug_stats_device;
SLI_USBD_CORE_EXTERN sl_usbd_debug_endpoint_stats_t  usbd_debug_stats_endpoint_table[32u];

#define  SLI_USBD_DBG_STATS_DEV_RESET()                          { \
    memset((void *)&usbd_debug_stats_device,                       \
           0,                                                      \
           (size_t) sizeof(sl_usbd_debug_device_stats_t));         \
}
#define  SLI_USBD_DBG_STATS_DEV_SET(stat, val)                 { \
    usbd_debug_stats_device.stat = val;                          \
}
#define  SLI_USBD_DBG_STATS_DEV_GET(stat)                      { \
    usbd_debug_stats_device.stat;                                \
}
#define  SLI_USBD_DBG_STATS_DEV_INC(stat)                      { \
    usbd_debug_stats_device.stat++;                              \
}
#define  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(stat, bool)        { \
    if (bool == true) {                                          \
      SLI_USBD_DBG_STATS_DEV_INC(stat);                          \
    }                                                            \
}

#define  SLI_USBD_DBG_STATS_EP_RESET(ep_ix)                    { \
    memset((void *)&usbd_debug_stats_endpoint_table[ep_ix],      \
           0,                                                    \
           (size_t) sizeof(sl_usbd_debug_endpoint_stats_t));     \
}
#define  SLI_USBD_DBG_STATS_EP_SET_ADDR(ep_ix, addr)           { \
    usbd_debug_stats_endpoint_table[ep_ix].address = addr;       \
}
#define  SLI_USBD_DBG_STATS_EP_SET(ep_ix, stat, val)           { \
    usbd_debug_stats_endpoint_table[ep_ix].stat = val;           \
}
#define  SLI_USBD_DBG_STATS_EP_GET(ep_ix, stat)                    usbd_debug_stats_endpoint_table[ep_ix].stat

#define  SLI_USBD_DBG_STATS_EP_INC(ep_ix, stat)                { \
    usbd_debug_stats_endpoint_table[ep_ix].stat++;               \
}
#define  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(ep_ix, stat, bool)  { \
    if (bool == true) {                                          \
      SLI_USBD_DBG_STATS_EP_INC(ep_ix, stat);                    \
    }                                                            \
}
#else
#define  SLI_USBD_DBG_STATS_DEV_RESET()
#define  SLI_USBD_DBG_STATS_DEV_SET(stat, val)
#define  SLI_USBD_DBG_STATS_DEV_GET(stat)
#define  SLI_USBD_DBG_STATS_DEV_INC(stat)
#define  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(stat, bool)

#define  SLI_USBD_DBG_STATS_EP_RESET(ep_ix)
#define  SLI_USBD_DBG_STATS_EP_SET_ADDR(ep_ix, addr)
#define  SLI_USBD_DBG_STATS_EP_SET(ep_ix, stat, val)
#define  SLI_USBD_DBG_STATS_EP_GET(ep_ix, stat)
#define  SLI_USBD_DBG_STATS_EP_INC(ep_ix, stat)
#define  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(ep_ix, stat, bool)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           CORE EVENTS DATA TYPE
 *******************************************************************************************************/

SL_ENUM(sli_usbd_event_code_t) {
  SLI_USBD_EVENT_BUS_RESET = 0u,
  SLI_USBD_EVENT_BUS_SUSPEND,
  SLI_USBD_EVENT_BUS_RESUME,
  SLI_USBD_EVENT_BUS_CONNECT,
  SLI_USBD_EVENT_BUS_DISCONNECT,
  SLI_USBD_EVENT_BUS_HS,
  SLI_USBD_EVENT_ENDPOINT,
  SLI_USBD_EVENT_SETUP
};

/****************************************************************************************************//**
 *                                           ENDPOINT STATES
 *******************************************************************************************************/

SL_ENUM(sli_usbd_endpoint_state_t) {
  SLI_USBD_ENDPOINT_STATE_CLOSE = 0,
  SLI_USBD_ENDPOINT_STATE_OPEN,
  SLI_USBD_ENDPOINT_STATE_STALL
};

/***************************************************************************************************//**
*                                           TRANSFER STATES
*
* Note(s): (1) If an asynchronous transfer cannot be fully queued in the driver, no more transfer can be
*               queued, to respect the transfers sequence.
*               For example, if a driver can queue only 512 bytes at once and the class/application needs
*               to queue 518 bytes, the first 512 bytes will be queued and it will be impossible to queue
*               another transaction. The remaining 6 bytes will only be queued when the previous (512
*               bytes) transaction completes. The state of the endpoint will be changed to
*               USBD_EP_XFER_TYPE_ASYNC_PARTIAL and other transfers could be queued after this one.
*******************************************************************************************************/

SL_ENUM(sli_usbd_transfer_state_t) {
  SLI_USBD_TRANSFER_STATE_NONE = 0,                                     // No xfer in progress.
  SLI_USBD_TRANSFER_STATE_SYNC,                                         // Sync xfer in progress.
  SLI_USBD_TRANSFER_STATE_ASYNC,                                        // Async xfer(s) in progress.
  SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL                                 // Partial async xfer(s) in progress (see Note #1).
};

/****************************************************************************************************//**
 *                                               URB STATES
 *******************************************************************************************************/

SL_ENUM(sli_usbd_urb_state_t) {
  SLI_USBD_URB_STATE_IDLE = 0,                                          // URB is in the memory pool, not used by any EP.
  SLI_USBD_URB_STATE_TRANSFER_SYNC,                                     // URB is used for a sync xfer.
  SLI_USBD_URB_STATE_TRANSFER_ASYNC                                     // URB is used for an async xfer.
};

/***************************************************************************************************//**
*                                   ENDPOINT USB REQUEST BLOCK DATA TYPE
*
* Note(s): (1) The 'flags' field is used as a bitmap. The following bits are used:
*
*                   D7..2 Reserved (reset to zero)
*                   D1    End-of-transfer:
*                               If this bit is set and transfer length is multiple of maximum packet
*                               size, a zero-length packet is transferred to indicate a short transfer to
*                               the host.
*                   D0    Extra URB:
*                               If this bit is set, it indicates that this URB is considered an 'extra'
*                               URB, that is shared amongst all endpoints. If this bit is cleared, it
*                               indicates that this URB is 'reserved' to allow every endpoint to have at
*                               least one URB available at any time.
*******************************************************************************************************/

typedef struct sli_usbd_urb {
  uint8_t                   *buf_ptr;                                     ///< Pointer to buffer.
  uint32_t                  buf_len;                                      ///< Buffer length.
  uint32_t                  transfer_len;                                 ///< Length that has been transferred.
  uint32_t                  next_xfer_len;                                ///< Length of the next transfer.
  uint8_t                   flags;                                        ///< flags (see Note #1).
  sli_usbd_urb_state_t      state;                                        ///< state of the transaction.
  sl_usbd_async_function_t  async_function;                               ///< Asynchronous notification function.
  void                      *async_function_arg;                          ///< Asynchronous function argument.
  sl_status_t               status;                                       ///< Error passed to callback, if any.
  struct sli_usbd_urb       *next_ptr;                                    ///< Pointer to next URB in list.
} sli_usbd_urb_t;

/****************************************************************************************************//**
 *                                   ENDPOINT INFORMATION DATA TYPE
 *
 * @note     (1) The endpoint information data type provides information about the USB device controller
 *               physical EPs.
 *               - (a) The 'attrib' bit-field defines the EP attributes. The EP attributes is combination
 *                   of the following flags:
 *                       - SL_USBD_ENDPOINT_INFO_TYPE_CTRL  Indicate control     type      capable.
 *                       - SL_USBD_ENDPOINT_INFO_TYPE_ISOC  Indicate isochronous type      capable.
 *                       - SL_USBD_ENDPOINT_INFO_TYPE_BULK  Indicate bulk        type      capable.
 *                       - SL_USBD_ENDPOINT_INFO_TYPE_INTR  Indicate interrupt   type      capable.
 *                       - SL_USBD_ENDPOINT_INFO_DIR_OUT    Indicate OUT         direction capable.
 *                       - SL_USBD_ENDPOINT_INFO_DIR_IN     Indicate IN          direction capable.
 *******************************************************************************************************/
typedef const struct {
  uint8_t  attrib;                                            ///< Endpoint attributes (see Note #1a).
  uint8_t  nbr;                                               ///< Endpoint number.
  uint16_t max_pkt_size;                                      ///< Endpoint maximum packet size.
} sli_usbd_driver_endpoint_info_t;

/****************************************************************************************************//**
 *                                           ENDPOINT DATA TYPE
 *******************************************************************************************************/

typedef struct sli_usbd_endpoint {
  sli_usbd_endpoint_state_t   state;                                      ///< EP state.
  sli_usbd_transfer_state_t   transfer_state;                             ///< Xfer state.
  uint8_t                     address;                                    ///< Address.
  uint8_t                     attrib;                                     ///< Attributes.
  uint16_t                    max_pkt_size;                               ///< Maximum packet size.
  uint8_t                     interval;                                   ///< interval.
  uint8_t                     trans_per_frame;                            ///< Transaction per microframe (HS only).
  uint8_t                     index;                                      ///< Allocation index.
#if (USBD_CFG_URB_EXTRA_EN == 1)
  bool                        urb_main_avail;                             ///< Flag indicating if main URB associated to EP avail.
#endif
  sli_usbd_urb_t              *urb_head_ptr;                              ///< USB request block head of the list.
  sli_usbd_urb_t              *urb_tail_ptr;                              ///< USB request block tail of the list.
} sli_usbd_endpoint_t;

/****************************************************************************************************//**
 *                                   ENDPOINT INFORMATION DATA TYPE
 *******************************************************************************************************/

typedef struct sli_usbd_endpoint_info {
  uint8_t                       address;                                  ///< Endpoint address.
  uint8_t                       attrib;                                   ///< Endpoint attributes.
  uint8_t                       interval;                                 ///< Endpoint interval.
  uint16_t                      max_pkt_size;
  uint8_t                       sync_addr;                                ///< Audio Class Only: associated sync endpoint.
  uint8_t                       sync_refresh;                             ///< Audio Class Only: sync feedback rate.
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  struct sli_usbd_endpoint_info *next_ptr;                                ///< Pointer to next interface group structure.
#endif
} sli_usbd_endpoint_info_t;

/****************************************************************************************************//**
 *                                   DEVICE INTERFACE GROUP DATA TYPE
 *******************************************************************************************************/

typedef struct sli_usbd_interface_group {
  uint8_t         class_code;                                             ///< IF class code.
  uint8_t         class_sub_code;                                         ///< IF sub class code.
  uint8_t         class_protocol_code;                                    ///< IF protocol  code.
  uint8_t         interface_start;                                        ///< IF index of the first IFs associated with a group.
  uint8_t         interface_count;                                        ///< Number of contiguous  IFs associated with a group.
#if (USBD_CFG_STR_EN == 1)
  const char     *name_ptr;                                               ///< IF group name.
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  struct sli_usbd_interface_group *next_ptr;                              ///< Pointer to next interface group structure.
#endif
} sli_usbd_interface_group_t;

/****************************************************************************************************//**
 *                                   INTERFACE ALTERNATE SETTING DATA TYPE
 *******************************************************************************************************/

typedef struct sli_usbd_alt_interface {
  void            *class_arg_ptr;                                   ///< Dev class drv arg ptr specific to alternate setting.
  uint32_t        endpoint_alloc_map;                               ///< EP allocation bitmap.
  uint8_t         endpoint_nbr_total;                               ///< Number of EP.

#if (USBD_CFG_STR_EN == 1)
  const char     *name_ptr;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  sli_usbd_endpoint_info_t       *endpoint_table_ptrs[SL_USBD_ENDPOINT_MAX_NBR];
  uint32_t                       endpoint_table_map;
#else
  sli_usbd_endpoint_info_t       *endpoint_head_ptr;
  sli_usbd_endpoint_info_t       *endpoint_tail_ptr;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  struct sli_usbd_alt_interface *next_ptr;                          ///< Pointer to next alternate setting structure.
#endif
} sli_usbd_alt_interface_t;

/****************************************************************************************************//**
 *                                           INTERFACE DATA TYPE
 *
 * Note(s):  (1) The interface structure contains information about the USB interfaces. It contains a
 *               list of all alternate settings (including the default interface).
 *
 *                               IFs         | ---------------------  Alt IF Settings ------------------- |
 *                                               Dflt           Alt_0          Alt_1     ...        Alt_n
 *                   -----       +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |  -----  | IF_0   |---->| IF_0_0  |--->| IF_0_0  |-->| IF_0_1  |-- ... --> | IF_0_1  |
 *                   |    |    +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |    |        |
 *                   |   GRP0      V
 *                   |    |    +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |    |    | IF_1   |---->| IF_1_0  |--->| IF_1_0  |-->| IF_1_1  |-- ... --> | IF_1_1  |
 *                   |  -----  +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |              |
 *           CONFIGx  |              V
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |         | IF_2   |---->| IF_1_0  |--->| IF_1_0  |-->| IF_1_1  |-- ... --> | IF_1_1  |
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |             .
 *                   |             .
 *                   |             .
 *                   |             |
 *                   |             V
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   |         | IF_n   |---->| IF_n_0  |--->| IF_n_0  |-->| IF_n_1  |-- ... --> | IF_n_1  |
 *                   |         +--------+     +---------+    +---------+   +---------+           +---------+
 *                   ------
 *
 *               (2) Interfaces can be combined together creating a logical group.  This logical group
 *                   represents a function. The device uses the Interface Association Descriptor (IAD)
 *                   to notify the host that multiple interfaces belong to one single function.    The
 *                   'group_nbr' stores the logical group number that the interface belongs to. By default,
 *                   it is defined to 'SL_USBD_INTERFACE_GROUP_NBR_NONE'.
 *
 *           (3)  The 'endpoint_alloc_map' is a bitmap of the allocated physical endpoints.
 *******************************************************************************************************/

typedef struct sli_usbd_interface {
  // Interface Class Information
  uint8_t                     class_code;                                       ///< Device interface     class code.
  uint8_t                     class_sub_code;                                   ///< Device interface sub class code.
  uint8_t                     class_protocol_code;                              ///< Device interface protocol  code.
  sl_usbd_class_driver_t      *class_driver_ptr;                                ///< Device class driver pointer.
  void                        *class_arg_ptr;                                   ///< Dev class drv arg ptr specific to interface.
  // Interface Alternate Settings
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  sli_usbd_alt_interface_t    *alt_table_ptrs[SL_USBD_ALT_INTERFACE_QUANTITY];  ///< IF alternate settings array.
#else
  sli_usbd_alt_interface_t    *alt_head_ptr;                                    ///< IF alternate settings linked-list.
  sli_usbd_alt_interface_t    *alt_tail_ptr;
#endif
  sli_usbd_alt_interface_t    *alt_cur_ptr;                                     ///< Pointer to current alternate setting.
  uint8_t                     alt_cur;                                          ///< Alternate setting selected by host.
  uint8_t                     alt_nbr_total;                                    ///< Number of alternate settings supported by this IF.
  uint8_t                     group_nbr;                                        ///< Interface group number.
  uint32_t                    endpoint_alloc_map;                               ///< EP allocation bitmap.
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  struct sli_usbd_interface   *next_ptr;                                        ///< Pointer to next interface structure.
#endif
} sli_usbd_interface_t;

/****************************************************************************************************//**
 *                                   DEVICE CONFIGURATION DATA TYPE
 *
 * Note(s):  (1) The configuration structure contains information about USB configurations. It contains a
 *               list of interfaces.
 *
 *                                       CONFIG       | ----------------- INTERFACES  ---------------- |
 *                           -----       +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_0 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_1 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                   DEVICEx |         | CONFIG_2 |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |             |
 *                           |             V
 *                           |             .
 *                           |             .
 *                           |             .
 *                           |             |
 *                           |             V
 *                           |         +----------+    +------+    +------+    +------+          +------+
 *                           |         | CONFIG_n |--->| IF_0 |--->| IF_1 |--->| IF_2 |-- ... -->| IF_n |
 *                       ------       +----------+    +------+    +------+    +------+          +------+
 *******************************************************************************************************/

// Configuration Structue
typedef struct sli_usbd_configuration {
  uint8_t                       attrib;                                                         ///< Configuration attributes.
  uint16_t                      max_power;                                                      ///< Maximum bus power drawn.
  uint16_t                      desc_len;                                                       ///< Configuration descriptor length.

#if (USBD_CFG_STR_EN == 1)
  const char                    *name_ptr;                                                      ///< Configuration name.
#endif

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  sli_usbd_interface_t          *interface_table_ptrs[SL_USBD_INTERFACE_QUANTITY];              ///< Interfaces list (array).
#if (SL_USBD_INTERFACE_GROUP_QUANTITY > 0)
  sli_usbd_interface_group_t    *interface_group_table_ptrs[SL_USBD_INTERFACE_GROUP_QUANTITY];  ///< Interfaces group list (array).
#endif
#else
  sli_usbd_interface_t          *interface_head_ptr;                                            ///< Interfaces list (linked list).
  sli_usbd_interface_t          *interface_tail_ptr;
  sli_usbd_interface_group_t    *interface_group_head_ptr;                                      ///< Interfaces group list (linked list).
  sli_usbd_interface_group_t    *interface_group_tail_ptr;
#endif

  uint8_t                       interface_nbr_total;                                            ///< Number of interfaces in this configuration.
  uint8_t                       interface_group_nbr_total;                                      ///< Number of interfaces group.
  uint32_t                      endpoint_alloc_map;                                             ///< EP allocation bitmap.

#if (USBD_CFG_HS_EN == 1)
  uint8_t                       config_other_speed;                                             ///< Other-speed configuration.
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  struct sli_usbd_configuration *next_ptr;
#endif
} sli_usbd_configuration_t;

/*****************************************************************************************************//**
 *                                           USB DEVICE DATA TYPE
 *
 * Note(s):  (1) A USB device could contain multiple configurations. A configuration is a set of
 *               interfaces.
 *
 *               USB Spec 2.0 section 9.2.6.6 states "device capable of operation at high-speed
 *               can operate in either full- or high-speed. The device always knows its operational
 *               speed due to having to manage its transceivers correctly as part of reset processing."
 *
 *               "A device also operates at a single speed after completing the reset sequence. In
 *               particular, there is no speed switch during normal operation. However, a high-
 *               speed capable device may have configurations that are speed dependent. That is,
 *               it may have some configurations that are only possible when operating at high-
 *               speed or some that are only possible when operating at full-speed. High-speed
 *               capable devices must support reporting their speed dependent configurations."
 *
 *               The device structure contains two list of configurations for HS and FS.
 *
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *                   | HS_CONFIG |--->| CONFIG_0 |--->| CONFIG_1 |--->| CONFIG_2 |-- ... -->| CONFIG_n |
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *                   | FS_CONFIG |--->| CONFIG_0 |--->| CONFIG_1 |--->| CONFIG_2 |-- ... -->| CONFIG_n |
 *                   +-----------+    +----------+    +----------+    +----------+          +----------+
 *
 *           (2) If the USB stack is optimized for speed, objects (Configs, IFs, EPs, etc) are implemented
 *               using a hash linking. Pointers are stored in an array allowing easy access by index.
 *
 *           (3) If the USB stack is optimized for size,  objects (Configs, IFs, EPs, etc) are implemented
 *               using a link list. Objects are linked dynamically reducing the overall memory footprint.
 *******************************************************************************************************/
// Device Structure
typedef struct {
  uint8_t                     address;                                                      ///< Device address assigned by host.
  sl_usbd_device_state_t      state;                                                        ///< Device state.
  sl_usbd_device_state_t      state_prev;                                                   ///< Device previous state.
  bool                        conn_status;                                                  // Device connection status.
  sl_usbd_device_speed_t      speed;                                                        ///< Device operating speed.
  sl_usbd_device_config_t     device_config;                                                ///< Device configuration.
  // Device Configurations
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  sli_usbd_configuration_t    *config_fs_speed_table_ptrs[SL_USBD_CONFIGURATION_QUANTITY];  ///< FS configuration array (see Note #2).
#if (USBD_CFG_HS_EN == 1)
  sli_usbd_configuration_t    *config_hs_speed_table_ptrs[SL_USBD_CONFIGURATION_QUANTITY];  ///< HS configuration array (see Note #2).
#endif
#else
  sli_usbd_configuration_t    *config_fs_head_ptr;                                          ///< FS configuration linked-list (see Note #3).
  sli_usbd_configuration_t    *config_fs_tail_ptr;
#if (USBD_CFG_HS_EN == 1)
  sli_usbd_configuration_t    *config_hs_head_ptr;                                          ///< HS configuration linked-list (see Note #3).
  sli_usbd_configuration_t    *config_hs_tail_ptr;
#endif
#endif
  sli_usbd_configuration_t    *config_cur_ptr;                                              ///< Current device configuration pointer.
  uint8_t                     config_cur_nbr;                                               ///< Current device configuration number.

  uint8_t                     config_fs_total_nbr;                                          ///< Number of FS configurations supported by the device.
#if (USBD_CFG_HS_EN == 1)
  uint8_t                     config_hs_total_nbr;                                          ///< Number of HS configurations supported by the device.
#endif
  // Configuration and String Descriptor Buffer
  uint8_t                     *actual_buf_ptr;                                              ///< Pointer to the buffer where data will be written.
  uint8_t                     *desc_buf_ptr;                                                ///< Configuration & string descriptor buffer.
  uint8_t                     desc_buf_index;                                               ///< Configuration & string descriptor buffer index.
  uint16_t                    desc_buf_req_len;                                             ///< Configuration & string descriptor requested length.
  uint16_t                    desc_buf_max_len;                                             ///< Configuration & string descriptor maximum length.
  sl_status_t                 *desc_buf_status_ptr;                                         ///< Configuration & string descriptor error pointer.
  // Endpoint InformatION
  uint16_t                    endpoint_max_ctrl_pkt_size;                                   ///< Ctrl EP maximum packet size.
  uint8_t                     endpoint_interface_table[SL_USBD_ENDPOINT_MAX_NBR];           ///< EP to IF number reference table.
  uint8_t                     endpoint_max_phy_nbr;                                         ///< EP Maximum physical number.
  // String Storage
#if (SL_USBD_STRING_QUANTITY > 0)
  char                        *str_descriptor_table[SL_USBD_STRING_QUANTITY];               ///< String pointers table.
  uint8_t                     str_max_index;                                                ///< Current String index.
#endif
#if (USBD_CFG_MS_OS_DESC_EN == 1)
  uint8_t                     str_microsoft_vendor_code;                                    ///< Microsoft Vendor code used in Microsoft OS str.
#endif

  sl_usbd_setup_req_t         setup_req;                                                    ///< Setup request.
  sl_usbd_setup_req_t         setup_req_next;                                               ///< Next setup request.

  bool                        self_power;                                                   ///< Device self powered?

  bool                        remote_wakeup;                                                ///< Remote Wakeup feature.

  uint8_t                     *ctrl_status_buf_ptr;                                         ///< Buf used for ctrl status xfers.
} sli_usbd_device_t;

/****************************************************************************************************//**
 *                                               USB CORE EVENTS
 *
 * Note(s) : (1) USB device driver queues bus and transaction events to the core task queue using
 *               the 'sli_usbd_core_event_t' structure.
 *******************************************************************************************************/

typedef struct {
  sli_usbd_event_code_t type;                                           ///< Core event type.
  uint8_t               endpoint_address;                               ///< Endpoint address.
  sl_status_t           status;                                         ///< Error Code returned by Driver, if any.
} sli_usbd_core_event_t;

/****************************************************************************************************//**
 *                                           USB OBJECTS POOL
 *
 * Note(s) : (1) USB objects (device, configuration, interfaces, alternative interface, endpoint, etc)
 *               are allocated from their pools.
 *
 *               (a) USB objects CANNOT be returned to the pool.
 *******************************************************************************************************/

typedef struct {
  // Device object.
  sli_usbd_device_t             device;

  // Configuration object pool.
  sli_usbd_configuration_t      config_table[SL_USBD_CONFIGURATION_QUANTITY];
  uint8_t                       config_nbr_next;

  // Interface object pool.
  sli_usbd_interface_t          interface_table[SL_USBD_INTERFACE_QUANTITY];
  uint8_t                       interface_next;

  // Alternative interface object pool.
  sli_usbd_alt_interface_t      alt_interface_table[SL_USBD_ALT_INTERFACE_QUANTITY];
  uint8_t                       alt_interface_nbr_next;

#if (SL_USBD_INTERFACE_GROUP_QUANTITY > 0)
  // Interface group object pool.
  sli_usbd_interface_group_t    interface_group_table[SL_USBD_INTERFACE_GROUP_QUANTITY];
  uint8_t                       interface_group_nbr_next;
#endif

  // Endpoints object pool.
  sli_usbd_endpoint_info_t      endpoint_info_table[SL_USBD_DESCRIPTOR_QUANTITY];
  uint8_t                       endpoint_info_nbr_next;

  sli_usbd_endpoint_t           endpoint_table[SL_USBD_OPEN_ENDPOINTS_QUANTITY];
  sli_usbd_endpoint_t           *endpoint_table_ptrs[SL_USBD_ENDPOINT_MAX_NBR];
  uint8_t                       endpoint_open_ctrl;
  uint32_t                      endpoint_open_bitmap;
  sli_usbd_urb_t                urb_table[SL_USBD_URB_MAX_NBR];
  sli_usbd_urb_t                *urb_table_ptr;

#if (USBD_CFG_STR_EN == 1)
  uint8_t                       str_quantity_per_device;
#endif

  uint16_t                      std_req_timeout_ms;

#if (USBD_CFG_URB_EXTRA_EN == 1)
  // nbr of extra URB currently used.
  uint8_t                       urb_extra_avail_count;
#endif
} sli_usbd_t;

extern sli_usbd_t *usbd_ptr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_core_task_handler()
 *
 * @brief    Processes all core events and core operations.
 *
 *******************************************************************************************************/
void sli_usbd_core_task_handler(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_endpoint_event()
 *
 * @brief    Sends a USB endpoint event to the core task.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    status      Error code returned by the USB device driver.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_endpoint_event(uint8_t      ep_addr,
                                         sl_status_t  status);

/****************************************************************************************************//**
 *                                          sli_usbd_core_open_control_endpoint()
 *
 * @brief    Opens control endpoints.
 *
 * @param    max_pkt_size    Maximum packet size.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_open_control_endpoint(uint16_t max_pkt_size);

/****************************************************************************************************//**
 *                                          sli_usbd_core_close_control_endpoint()
 *
 * @brief    Close control endpoint.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_close_control_endpoint(void);

/****************************************************************************************************//**
 *                                          sli_usbd_core_stall_control_endpoint()
 *
 * @brief    Stall control endpoint.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_stall_control_endpoint(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_get_control_rx_status()
 *
 * @brief    Handles the status stage from the host on a control (EP0) OUT endpoint.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_get_control_rx_status(uint16_t timeout_ms);

/****************************************************************************************************//**
 *                                           sli_usbd_core_get_control_tx_status()
 *
 * @brief    Handles status stage from host on control (EP0) IN endpoint.
 *
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_get_control_tx_status(uint16_t timeout_ms);

/****************************************************************************************************//**
 *                                               sli_usbd_core_init_endpoint()
 *
 * @brief    Initialize endpoint structures.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_init_endpoint(void);

/****************************************************************************************************//**
 *                                               sli_usbd_core_open_endpoint()
 *
 * @brief    Open non-control endpoint.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    max_pkt_size    Maximum packet size.
 *
 * @param    attrib          Endpoint attributes.
 *
 * @param    interval        Endpoint polling interval.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_open_endpoint(uint8_t  ep_addr,
                                        uint16_t max_pkt_size,
                                        uint8_t  attrib,
                                        uint8_t  interval);

/****************************************************************************************************//**
 *                                               sli_usbd_core_close_endpoint()
 *
 * @brief    Close non-control endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_close_endpoint(uint8_t ep_addr);

/****************************************************************************************************//**
 *                                       sl_usbd_core_endpoint_transfer_async()
 *
 * @brief    Read/write data asynchronously from/to non-control endpoints.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    xfer_err    Error Code returned by the Core Event.
 *                       Argument changed for 'xfer_err'.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) A USB device driver can notify the core about the Tx transfer completion using
 *               sl_usbd_core_endpoint_write_complete() or sl_usbd_core_endpoint_write_complete_extended(). The latter function allows to report a
 *               specific error code whereas sl_usbd_core_endpoint_write_complete() reports only a successful transfer.
 *               In the case of an asynchronous transfer, the error code reported by the USB device
 *               driver must be tested. In case of an error condition, the asynchronous transfer
 *               is marked as completed and the associated callback is called by the core task.
 *
 * @note     (2) This condition covers also the case where the transfer length is multiple of the
 *               maximum packet size. In that case, host sends a zero-length packet considered as
 *               a short packet for the condition.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_transfer_async(uint8_t      ep_addr,
                                                 sl_status_t  xfer_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *                                       DEFINED IN OS'S usbd_os.c
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_create_task()
 *
 * @brief    Create task and queue for task to pend on.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_task(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_create_endpoint_signal()
 *
 * @brief    Create an OS signal.
 *
 * @param    endpoint    Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_signal(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_delete_endpoint_signal()
 *
 * @brief    Delete an OS signal.

 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_signal(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_pend_endpoint_signal()
 *
 * @brief    Wait for a signal to become available.
 *
 * @param    endpoint       Endpoint index.
 *
 * @param    timeout_ms  Signal wait timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_pend_endpoint_signal(uint8_t  endpoint,
                                                  uint16_t timeout_ms);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_abort_endpoint_signal()
 *
 * @brief    Abort any wait operation on signal.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_abort_endpoint_signal(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_post_endpoint_signal()
 *
 * @brief    Make a signal available.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_post_endpoint_signal(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_create_endpoint_lock()
 *
 * @brief    Create an OS resource to use as an endpoint lock.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_lock(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_delete_endpoint_lock()
 *
 * @brief    Delete the OS resource used as an endpoint lock.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_lock(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_acquire_endpoint_lock()
 *
 * @brief    Wait for an endpoint to become available and acquire its lock.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_acquire_endpoint_lock(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_release_endpoint_lock()
 *
 * @brief    Release an endpoint lock.
 *
 * @param    endpoint       Endpoint index.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_release_endpoint_lock(uint8_t endpoint);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_get_core_event()
 *
 * @brief    Wait until a core event is ready.
 *
 * @param    p_event     Pointer to the event message buffer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_get_core_event(void *p_event);

/****************************************************************************************************//**
 *                                           sli_usbd_core_os_put_core_event()
 *
 * @brief    Queues core event.
 *
 * @param    p_event     Pointer to core event.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_put_core_event(void *p_event);

/********************************************************************************************************
 *                                           USBD CORE INTERNAL FUNCTIONS
 *
 * Note(s) : (1) USB Spec 2.0, Section 5.5 states "Control transfers allow access to different parts of
 *               a device.  Control transfers are intended to support configuration/command/status type
 *               communication flows between client software and its function".
 *
 *               (a) "Each USB device is required to implement the Default Control Pipe as a message
 *                       pipe.  This pipe is used by the USB System Software.  The Default Control Pipe
 *                       provides access to the USB device's configuration, status, and control information".
 *
 *                   The 'sl_usbd_core_ep_XXXX()' functions perform operations in the default endpoint.
 *                   Class drivers should use 'sl_usbd_core_ep_XXXX()' to send/receive class specific requests.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               sli_usbd_core_connect_event()
 *
 * @brief    Notifies the USB connection bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_connect_event(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_disconnect_event()
 *
 * @brief    Notifies the USB disconnection bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_disconnect_event(void);

/****************************************************************************************************//**
 *                                               sli_usbd_core_high_speed_event()
 *
 * @brief    Notifies the USB High-Speed bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_high_speed_event(void);

/****************************************************************************************************//**
 *                                               sli_usbd_core_reset_event()
 *
 * @brief    Notifies the USB reset bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_reset_event(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_suspend_event()
 *
 * @brief    Notifies the USB suspend bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_suspend_event(void);

/****************************************************************************************************//**
 *                                           sli_usbd_core_resume_event()
 *
 * @brief    Notifies the USB resume bus events to the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_resume_event(void);

/****************************************************************************************************//**
 *                                               sli_usbd_core_setup_event()
 *
 * @brief    Sends a USB setup event to the core task.
 *
 * @param    p_buf   Pointer to the setup packet.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_core_setup_event(void *p_buf);

/****************************************************************************************************//**
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
