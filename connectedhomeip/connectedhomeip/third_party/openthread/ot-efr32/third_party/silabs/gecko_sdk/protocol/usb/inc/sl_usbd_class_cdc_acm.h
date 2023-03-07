/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (CDC)
 *        Abstract Control Model (ACM) - Serial Emulation
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

/****************************************************************************************************//**
 * @defgroup USBD_CDC_ACM USB Device ACM API
 * @ingroup USBD
 * @brief   USB Device CDC ACM API
 *
 * @addtogroup USBD_CDC_ACM
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CLASS_CDC_ACM_H
#define  SL_USBD_CLASS_CDC_ACM_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_usbd_core.h"
#include "sl_usbd_class_cdc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SL_USBD_CDC_ACM_NBR_NONE            255u

/********************************************************************************************************
 *                                           PORT SETTINGS DEFINES
 *******************************************************************************************************/

#define  SL_USBD_CDC_ACM_PARITY_NONE                       0u
#define  SL_USBD_CDC_ACM_PARITY_ODD                        1u
#define  SL_USBD_CDC_ACM_PARITY_EVEN                       2u
#define  SL_USBD_CDC_ACM_PARITY_MARK                       3u
#define  SL_USBD_CDC_ACM_PARITY_SPACE                      4u

#define  SL_USBD_CDC_ACM_STOP_BIT_1                        0u
#define  SL_USBD_CDC_ACM_STOP_BIT_1_5                      1u
#define  SL_USBD_CDC_ACM_STOP_BIT_2                        2u

/********************************************************************************************************
 *                                       LINE EVENTS FLAGS DEFINES
 *******************************************************************************************************/

#define  SL_USBD_CDC_ACM_CTRL_BREAK                   0x01u // BIT_00
#define  SL_USBD_CDC_ACM_CTRL_RTS                     0x02u // BIT_01
#define  SL_USBD_CDC_ACM_CTRL_DTR                     0x04u // BIT_02

#define  SL_USBD_CDC_ACM_STATE_DCD                    0x01u // BIT_00
#define  SL_USBD_CDC_ACM_STATE_DSR                    0x02u // BIT_01
#define  SL_USBD_CDC_ACM_STATE_BREAK                  0x04u // BIT_02
#define  SL_USBD_CDC_ACM_STATE_RING                   0x08u // BIT_03
#define  SL_USBD_CDC_ACM_STATE_FRAMING                0x10u // BIT_04
#define  SL_USBD_CDC_ACM_STATE_PARITY                 0x20u // BIT_05
#define  SL_USBD_CDC_ACM_STATE_OVERUN                 0x40u // BIT_06

/********************************************************************************************************
 *                                       CALL MANAGEMENT CAPABILITIES
 *
 * Note(s) : (1) See 'USB, Communications Class, Subclass Specification for PSTN Devices, Revision 1.2,
 *               February 9 2007', section '5.3.1 Call Management Functional Descriptor' for more details
 *               about the Call Management capabilities.
 *******************************************************************************************************/

#define  SL_USBD_CDC_ACM_CALL_MGMT_DEV                0x01u // BIT_00
#define  SL_USBD_CDC_ACM_CALL_MGMT_DATA_CCI_DCI       0x02u // BIT_01
#define  SL_USBD_CDC_ACM_CALL_MGMT_DATA_OVER_DCI     (0x02u | 0x01u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                           LINE CODING DATA TYPE
 *******************************************************************************************************/
/// line coding data type
typedef struct {
  uint32_t baudrate;
  uint8_t parity;
  uint8_t stop_bits;
  uint8_t data_bits;
} sl_usbd_cdc_acm_line_coding_t;

/********************************************************************************************************
 *                                             CDC ACM CALLBACKS
 *******************************************************************************************************/
/// CDC ACM Callbacks
typedef const struct {
  void (*enable)(uint8_t subclass_nbr);                                       ///< Callback for enable event

  void (*disable)(uint8_t subclass_nbr);                                      ///< Callback for disable event

  void (*line_control_changed)(uint8_t subclass_nbr,
                               uint8_t event,
                               uint8_t event_chngd);                          ///< Callback for line control changed event

  bool (*line_coding_changed)(uint8_t                       subclass_nbr,
                              sl_usbd_cdc_acm_line_coding_t *p_line_coding);  ///< Callback for line coding change event
} sl_usbd_cdc_acm_callbacks_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                               CDC ACM FUNCTIONS
 *******************************************************************************************************/
/****************************************************************************************************//**
 * @brief    Initialize the CDC ACM serial emulation subclass.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_init(void);

/****************************************************************************************************//**
 * @brief    Add a new instance of the CDC ACM serial emulation subclass.
 *
 * @param    line_state_interval     Line state notification interval in milliseconds (value must
 *                                   be a power of 2).
 *
 * @param    call_mgmt_capabilities  Call Management Capabilities bitmap. OR'ed of the following
 *                                   flags:
 *                                       -  SL_USBD_CDC_ACM_CALL_MGMT_DEV           Device handles call management itself.
 *                                       -  SL_USBD_CDC_ACM_CALL_MGMT_DATA_CCI_DCI  Device can send/receive call management
 *                                   information over a Data Class interface.
 *
 * @param    p_acm_callbacks         Optional pointers to callback functions to be called on various events.
 *
 * @param    p_subclass_nbr          Param to variable that will receive CDC ACM serial emulation subclass
 *                                   instance number.
 *
 * @return   Return SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 *
 * @internal
 * @note     (1) [INTERNAL] See Note #2 of the function usbd_cdc_acm_mgmt_interface_descriptor_cb() for more details
 *               about the Call Management capabilities.
 *
 * @note     (2) [INTERNAL] Depending on the operating system (Windows, Linux or Mac OS X), not all the possible
 *               flags combinations are supported. Windows and Linux support all combinations, whereas
 *               Mac OS X supports all except the combination (SL_USBD_CDC_ACM_CALL_MGMT_DEV).
 * @endinternal
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_create_instance(uint16_t                    line_state_interval,
                                            uint16_t                    call_mgmt_capabilities,
                                            sl_usbd_cdc_acm_callbacks_t *p_acm_callbacks,
                                            uint8_t                     *p_subclass_nbr);

/****************************************************************************************************//**
 * @brief    Add a CDC ACM subclass class instance into USB device configuration.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    config_nbr      Configuration index to add new test class interface to.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_add_to_configuration(uint8_t  subclass_nbr,
                                                 uint8_t  config_nbr);

/****************************************************************************************************//**
 * @brief    Get the CDC ACM serial emulation subclass enable state.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_enabled       Boolean to a variable that will receive enable status.
 *                           The variable is set to true, CDC ACM serial emulation is enabled.
 *                           The variable is set to false,  CDC ACM serial emulation is not enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_is_enabled(uint8_t  subclass_nbr,
                                       bool     *p_enabled);

/****************************************************************************************************//**
 * @brief    Receive data on the CDC ACM serial emulation subclass.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    timeout         Timeout, in milliseconds.
 *
 * @param    p_xfer_len      Pointer to a variable that will receive transfer length.
 *                           If no errors are returned, the variable is set to number of octets received,
 *                           otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_read(uint8_t  subclass_nbr,
                                 uint8_t  *p_buf,
                                 uint32_t buf_len,
                                 uint16_t timeout,
                                 uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data on the CDC ACM serial emulation subclass.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    timeout         Timeout in milliseconds.
 *
 * @param    p_xfer_len      Pointer to a variable that will receive transfer length.
 *                           If no errors are returned, the variable is set to number of octets transmitted,
 *                           otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_write(uint8_t  subclass_nbr,
                                  uint8_t  *p_buf,
                                  uint32_t buf_len,
                                  uint16_t timeout,
                                  uint32_t *p_xfer_len);

#if 0
sl_status_t USBD_ACM_SerialRxAsync(uint8_t               subclass_nbr,
                                   uint8_t               *p_buf,
                                   uint32_t              buf_len,
                                   USBD_ACM_SERIAL_ASYNC async,
                                   void                  *p_async_arg);

sl_status_t USBD_ACM_SerialTxAsync(uint8_t               subclass_nbr,
                                   uint8_t               *p_buf,
                                   uint32_t              buf_len,
                                   USBD_ACM_SERIAL_ASYNC async,
                                   void                  *p_async_arg);
#endif

/****************************************************************************************************//**
 * @brief    Return the state of control lines.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_line_ctrl     Pointer to the variable that will be set with the state of the control lines,
 *                           encoded in bit fields.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_get_line_control_state(uint8_t subclass_nbr,
                                                   uint8_t *p_line_ctrl);

/****************************************************************************************************//**
 * @brief    Get the current state of the line coding.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_line_coding   Pointer to the structure where the current line coding will be stored.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_get_line_coding(uint8_t                       subclass_nbr,
                                            sl_usbd_cdc_acm_line_coding_t *p_line_coding);

/****************************************************************************************************//**
 * @brief    Set a new line coding.
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    p_line_coding   Pointer to the structure that contains the new line coding.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_set_line_coding(uint8_t                       subclass_nbr,
                                            sl_usbd_cdc_acm_line_coding_t *p_line_coding);

/****************************************************************************************************//**
 * @brief    Set a line state event(s).
 *
 * @param    subclass_nbr    CDC ACM serial emulation subclass instance number.
 *
 * @param    events          Line state event(s) to set.  OR'ed of the following flags:
 *                               - SL_USBD_CDC_ACM_STATE_DCD
 *                               - SL_USBD_CDC_ACM_STATE_DSR
 *                               - SL_USBD_CDC_ACM_STATE_BREAK
 *                               - SL_USBD_CDC_ACM_STATE_RING
 *                               - SL_USBD_CDC_ACM_STATE_FRAMING
 *                               - SL_USBD_CDC_ACM_STATE_PARITY
 *                               - SL_USBD_CDC_ACM_STATE_OVERUN
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_set_line_state_event(uint8_t subclass_nbr,
                                                 uint8_t events);

/****************************************************************************************************//**
 * @brief          Clear a line state event(s).
 *
 * @param          subclass_nbr  CDC ACM serial emulation subclass instance number.
 *
 * @param          events        Line state event(s) set to be cleared. OR'ed of the following
 *                               flags (see Note #1) :
 *                                   - SL_USBD_CDC_ACM_STATE_DCD    Set DCD signal (Rx carrier).
 *                                   - SL_USBD_CDC_ACM_STATE_DSR    Set DSR signal (Tx carrier).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB PSTN spec ver 1.20 states: "For the irregular signals like break, the
 *                       incoming ring signal, or the overrun error state, this will reset their values
 *                       to zero and again will not send another notification until their state changes."
 *                       The irregular events are automatically cleared by the ACM serial emulation subclass.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_clear_line_state_event(uint8_t subclass_nbr,
                                                   uint8_t events);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
