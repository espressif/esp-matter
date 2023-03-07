/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (Cdc)
 *        Abstract Control Model (Acm) - Serial Emulation
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
 * @note     (1) This implementation is compliant with the PSTN subclass specification revision 1.2
 *               February 9, 2007.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_status.h"

#include "em_core.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_cdc.h"
#include "sl_usbd_class_cdc_acm.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_CDC_ACM_CTRL_REQ_TIMEOUT_mS                  5000u

/********************************************************************************************************
 *                               ACM FUNCTIONAL DESCRIPTORS SIZES DEFINES
 *
 * Note(s) : (1) Table 3 and 4 from the PSTN specification version 1.2 defines the Call management
 *               and ACM management functional descriptors.
 *******************************************************************************************************/

#define  SLI_USBD_CDC_ACM_DESC_CALL_MGMT_SIZE                     5u
#define  SLI_USBD_CDC_ACM_DESC_SIZE                               4u
#define  SLI_USBD_CDC_ACM_DESC_TOT_SIZE                (SLI_USBD_CDC_ACM_DESC_CALL_MGMT_SIZE \
                                                        + SLI_USBD_CDC_ACM_DESC_SIZE)

/********************************************************************************************************
 *                                   ACM SERIAL NOTIFICATIONS DEFINES
 *******************************************************************************************************/

#define  SLI_USBD_CDC_ACM_REQ_STATE                     0x20u    // Serial state notification code.
#define  SLI_USBD_CDC_ACM_REQ_STATE_SIZE                   2u    // Serial state notification data size.

#define  SLI_USBD_CDC_ACM_STATE_BUF_SIZE        (SL_USBD_CDC_NOTIFICATION_HEADER \
                                                 + SLI_USBD_CDC_ACM_REQ_STATE_SIZE)

/********************************************************************************************************
 *                                   ABSTRACT STATE FEATURE DEFINES
 *******************************************************************************************************/

#define  SLI_USBD_CDC_ACM_ABSTRACT_DATA_MUX      0x02u // BIT_01
#define  SLI_USBD_CDC_ACM_ABSTRACT_IDLE          0x01u // BIT_00

/********************************************************************************************************
 *                                       LINE STATE SIGNAL DEFINES
 *******************************************************************************************************/
// Consistent signals.
#define  SLI_USBD_CDC_ACM_EVENTS_CONS           (SL_USBD_CDC_ACM_STATE_DCD \
                                                 | SL_USBD_CDC_ACM_STATE_DSR)

// Irregular signals.
#define  SLI_USBD_CDC_ACM_EVENTS_IRRE           (SL_USBD_CDC_ACM_STATE_BREAK     \
                                                 | SL_USBD_CDC_ACM_STATE_RING    \
                                                 | SL_USBD_CDC_ACM_STATE_PARITY  \
                                                 | SL_USBD_CDC_ACM_STATE_FRAMING \
                                                 | SL_USBD_CDC_ACM_STATE_OVERUN)

/********************************************************************************************************
 *                                   SET CONTROL LINE STATE DEFINES
 *
 * Note(s): (1) The PSTN specification version 1.2 defines the 'SetControlLineState' request as:
 *
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *                   | bmRequestType |    bRequestCode   |      wValue      |  wIndex   | wLength | Data |
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *                   | 00100001B     | SET_CONTROL_LINE_ |  Control Signal  | Interface |  Zero   | None |
 *                   |               |       STATE       |     Bitmap       |           |         |      |
 *                   +---------------+-------------------+------------------+-----------+---------+------+
 *
 *               (a) Table 18 from the PSTN specification defines the control signal bitmap values for
 *                   the SET_CONTROL_LINE_STATE
 *
 *                   Bit Position
 *                   -------------
 *                       D1          Carrier control for half duplex modems. This signal correspond to
 *                                   V.24 signal 105 and RS-232 signal RTS.
 *
 *                       D0          Indicates to DCE if DTE is present or not. This signal corresponds to
 *                                   V.24 signal 108/2 and RS-232 signal DTR.
 *******************************************************************************************************/

#define SLI_USBD_CDC_ACM_REQ_DTR                 0x01u // BIT_00
#define SLI_USBD_CDC_ACM_REQ_RTS                 0x02u // BIT_01

/********************************************************************************************************
 *                                   COMMUNICATION FEATURE SELECTOR DEFINES
 ** Note(s): (1) The PSTN specification version 1.2 defines the 'GetCommFeature' request as:
 *
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *                   | bmRequestType |    bRequestCode   |      wValue      |  wIndex   |  wLength    |  Data  |
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *                   | 00100001B     | GET_COMM_FEATURE  | Feature Selector | Interface | Length of   | Status |
 *                   |               |                   |     Bitmap       |           | Status Data |        |
 *                   +---------------+-------------------+------------------+-----------+-------------+--------+
 *
 *               (a) Table 14 from the PSTN specification defines the communication feature selector codes:
 *
 *                       Feature             Code     Description
 *                       Selector
 *                   ----------------   --------   ----------------------------------------------------
 *                       ABSTRACT_STATE     01h       Two bytes of data describing multiplexed state and idle
 *                                                   state for this Abstract Model communications device.
 *
 *                       COUNTRY_SETTING    02h       Country code in hexadecimal format as defined in
 *                                                   [ISO3166], release date as specified in offset 3 of
 *                                                   the Country Selection Functional Descriptor. This
 *                                                   selector is only valid for devices that provide a
 *                                                   Country Selection Functional Descriptor, and the value
 *                                                   supplied shall appear as supported country in the
 *                                                   Country Selection Functional Descriptor.
 *******************************************************************************************************/

#define  SLI_USBD_CDC_ACM_ABSTRACT_STATE                0x01u
#define  SLI_USBD_CDC_ACM_COUNTRY_SETTING               0x02u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CDC ACM SERIAL CTRL DATA TYPE
 *******************************************************************************************************/
/// ACM Subclass Control Information
typedef struct {
  uint8_t                        nbr;
  bool                           idle;
  sl_usbd_cdc_acm_callbacks_t   *callbacks;
  sl_usbd_cdc_acm_line_coding_t  line_coding;
  uint8_t                        line_ctrl;
  uint8_t                        line_state;
  uint16_t                       line_state_interval;
  uint8_t                       *line_state_buf_ptr;
  bool                           line_state_sent;
  uint8_t                        call_mgmt_capabilities;
  uint8_t                       *req_buf_ptr;
} sli_usbd_cdc_acm_ctrl_t;

/********************************************************************************************************
 *                                   CDC ACM SERIAL SUBCLASS ROOT STRUCT
 *******************************************************************************************************/

typedef struct {
  sli_usbd_cdc_acm_ctrl_t ctrl_table[SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY]; // Table of CDC ACM subclass instance structure.
  uint8_t                 ctrl_nbr_next;                                          // Index to next CDC ACM subclass instance to allocate.
} sli_usbd_cdc_acm_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static sli_usbd_cdc_acm_t usbd_cdc_acm;

static uint32_t req_buffer[2];
static uint32_t line_state_buffer[((SLI_USBD_CDC_ACM_STATE_BUF_SIZE - (SLI_USBD_CDC_ACM_STATE_BUF_SIZE % 4) + 4)) / 4];

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_cdc_acm_enable(void *p_subclass_arg);

static void usbd_cdc_acm_disable(void *p_subclass_arg);

static bool usbd_cdc_acm_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                               void                      *p_subclass_arg);

static void usbd_cdc_acm_notification_complete_cb(void *p_subclass_arg);

static void usbd_cdc_acm_mgmt_interface_descriptor_cb(void    *p_subclass_arg,
                                                      uint8_t first_dci_if_nbr);

static uint16_t usbd_cdc_acm_get_mgmt_interface_descriptor_size(void *p_subclass_arg);

/********************************************************************************************************
 *                                           CDC ACM CLASS DRIVER
 *******************************************************************************************************/

static sl_usbd_cdc_subclass_driver_t usbd_cdc_acm_driver = {
  usbd_cdc_acm_enable,
  usbd_cdc_acm_disable,
  usbd_cdc_acm_class_request_handler,
  usbd_cdc_acm_notification_complete_cb,
  usbd_cdc_acm_mgmt_interface_descriptor_cb,
  usbd_cdc_acm_get_mgmt_interface_descriptor_size,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           APPLICATION FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialise CDC ACM subclass
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_init(void)
{
  uint8_t           ix;
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;

  usbd_cdc_acm.ctrl_nbr_next = SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY;

  // Init ACM serial ctrl.
  for (ix = 0u; ix < usbd_cdc_acm.ctrl_nbr_next; ix++) {
    p_ctrl = &usbd_cdc_acm.ctrl_table[ix];
    p_ctrl->nbr = SL_USBD_CDC_NBR_NONE;
    p_ctrl->idle = false;
    p_ctrl->line_coding.baudrate = 9600u;
    p_ctrl->line_coding.parity = SL_USBD_CDC_ACM_PARITY_NONE;
    p_ctrl->line_coding.stop_bits = SL_USBD_CDC_ACM_STOP_BIT_1;
    p_ctrl->line_coding.data_bits = 8u;

    p_ctrl->line_ctrl = 0x00u;

    p_ctrl->line_state_sent = false;
    p_ctrl->line_state_interval = 0u;
    p_ctrl->line_state = 0u;
    p_ctrl->call_mgmt_capabilities = 0u;

    // Alloc control buffers.
    p_ctrl->req_buf_ptr = (uint8_t *)req_buffer;
    if (p_ctrl->req_buf_ptr == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }

    p_ctrl->line_state_buf_ptr = (uint8_t *)line_state_buffer;
    if (p_ctrl->line_state_buf_ptr == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }

    memset(p_ctrl->line_state_buf_ptr, 0, SLI_USBD_CDC_ACM_STATE_BUF_SIZE);
  }
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a new instance of the CDC ACM serial emulation subclass
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_create_instance(uint16_t                    line_state_interval,
                                            uint16_t                    call_mgmt_capabilities,
                                            sl_usbd_cdc_acm_callbacks_t *p_acm_callbacks,
                                            uint8_t                     *p_subclass_nbr)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  uint8_t           subclass_nbr;
  uint8_t           class_nbr;
  uint8_t           data_if_nbr;
  sl_status_t            status;
  CORE_DECLARE_IRQ_STATE;

  if (p_subclass_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_cdc_acm.ctrl_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_subclass_nbr = SL_USBD_CDC_ACM_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_cdc_acm.ctrl_nbr_next--;
  // Alloc new CDC ACM serial emulation subclass.
  subclass_nbr = usbd_cdc_acm.ctrl_nbr_next;
  CORE_EXIT_ATOMIC();
  // Init control struct.
  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];
  // Create new CDC device.
  status = sl_usbd_cdc_create_instance(SL_USBD_CDC_SUBCLASS_ACM,
                                       &usbd_cdc_acm_driver,
                                       (void *)p_ctrl,
                                       SL_USBD_CDC_COMM_PROTOCOL_AT_V250,
                                       true,
                                       line_state_interval,
                                       &class_nbr);

  if (status != SL_STATUS_OK) {
    *p_subclass_nbr = SL_USBD_CDC_ACM_NBR_NONE;
    return status;
  }
  // Add data IF class to CDC device.
  status = sl_usbd_cdc_add_data_interface(class_nbr,
                                          false,
                                          SL_USBD_CDC_DATA_PROTOCOL_NONE,
                                          &data_if_nbr);

  if (status != SL_STATUS_OK) {
    *p_subclass_nbr = SL_USBD_CDC_ACM_NBR_NONE;
    return status;
  }

  p_ctrl->line_state_interval = line_state_interval;
  p_ctrl->call_mgmt_capabilities = call_mgmt_capabilities;        // See Note #2.
  p_ctrl->callbacks = p_acm_callbacks;
  p_ctrl->nbr = class_nbr;

  *p_subclass_nbr = subclass_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Add CDC ACM subclass instance to configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_add_to_configuration(uint8_t  subclass_nbr,
                                                 uint8_t  config_nbr)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  sl_status_t status;

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  status = sl_usbd_cdc_add_to_configuration(p_ctrl->nbr, config_nbr);

  return status;
}

/****************************************************************************************************//**
 * Get the CDC ACM enable state
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_is_enabled(uint8_t  subclass_nbr,
                                       bool     *p_enabled)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;

  if (p_enabled == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  sl_usbd_cdc_is_enabled(p_ctrl->nbr, p_enabled);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Read data on the CDC ACM subclass
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_read(uint8_t  subclass_nbr,
                                 uint8_t  *p_buf,
                                 uint32_t buf_len,
                                 uint16_t timeout,
                                 uint32_t *p_xfer_len)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  bool          conn;
  sl_status_t            status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  sl_usbd_cdc_is_enabled(p_ctrl->nbr, &conn);

  if ((conn == false) || (p_ctrl->idle == true)) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  status = sl_usbd_cdc_read_data(p_ctrl->nbr,
                                 0u,
                                 p_buf,
                                 buf_len,
                                 timeout,
                                 p_xfer_len);

  return status;
}

/****************************************************************************************************//**
 * Send data on the CDC ACM subclass
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_write(uint8_t  subclass_nbr,
                                  uint8_t  *p_buf,
                                  uint32_t buf_len,
                                  uint16_t timeout,
                                  uint32_t *p_xfer_len)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  bool          conn;
  sl_status_t            status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  sl_usbd_cdc_is_enabled(p_ctrl->nbr, &conn);

  if ((conn == false) || (p_ctrl->idle == true)) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  status = sl_usbd_cdc_write_data(p_ctrl->nbr,
                                  0u,
                                  p_buf,
                                  buf_len,
                                  timeout,
                                  p_xfer_len);

  return status;
}

/****************************************************************************************************//**
 * Returns the state of control lines
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_get_line_control_state(uint8_t subclass_nbr,
                                                   uint8_t *p_line_ctrl)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;

  if (p_line_ctrl == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];
  *p_line_ctrl = p_ctrl->line_ctrl;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the current state of the line coding
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_get_line_coding(uint8_t                       subclass_nbr,
                                            sl_usbd_cdc_acm_line_coding_t *p_line_coding)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  if (p_line_coding == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_line_coding->baudrate = p_ctrl->line_coding.baudrate;
  p_line_coding->data_bits = p_ctrl->line_coding.data_bits;
  p_line_coding->stop_bits = p_ctrl->line_coding.stop_bits;
  p_line_coding->parity = p_ctrl->line_coding.parity;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sets a new line coding
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_set_line_coding(uint8_t                       subclass_nbr,
                                            sl_usbd_cdc_acm_line_coding_t *p_line_coding)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  if (p_line_coding == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((p_line_coding->data_bits != 5u)
      && (p_line_coding->data_bits != 6u)
      && (p_line_coding->data_bits != 7u)
      && (p_line_coding->data_bits != 8u)
      && (p_line_coding->data_bits != 16u)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((p_line_coding->stop_bits != SL_USBD_CDC_ACM_STOP_BIT_1)
      && (p_line_coding->stop_bits != SL_USBD_CDC_ACM_STOP_BIT_1_5)
      && (p_line_coding->stop_bits != SL_USBD_CDC_ACM_STOP_BIT_2)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((p_line_coding->parity != SL_USBD_CDC_ACM_PARITY_NONE)
      && (p_line_coding->parity != SL_USBD_CDC_ACM_PARITY_ODD)
      && (p_line_coding->parity != SL_USBD_CDC_ACM_PARITY_EVEN)
      && (p_line_coding->parity != SL_USBD_CDC_ACM_PARITY_MARK)
      && (p_line_coding->parity != SL_USBD_CDC_ACM_PARITY_SPACE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  CORE_ENTER_ATOMIC();
  p_ctrl->line_coding.baudrate = p_line_coding->baudrate;
  p_ctrl->line_coding.data_bits = p_line_coding->data_bits;
  p_ctrl->line_coding.stop_bits = p_line_coding->stop_bits;
  p_ctrl->line_coding.parity = p_line_coding->parity;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sets a line state event(s)
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_set_line_state_event(uint8_t subclass_nbr,
                                                 uint8_t events)

{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  uint8_t           line_state_chngd;
  bool          conn;
  bool          result;
  sl_status_t            status;
  CORE_DECLARE_IRQ_STATE;

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  sl_usbd_cdc_is_enabled(p_ctrl->nbr, &conn);

  if (conn == false) {
    return SL_STATUS_INVALID_STATE;
  }

  events &= (SLI_USBD_CDC_ACM_EVENTS_CONS | SLI_USBD_CDC_ACM_EVENTS_IRRE);

  CORE_ENTER_ATOMIC();
  line_state_chngd = events ^ p_ctrl->line_state;

  if (line_state_chngd != 0x00u) {
    p_ctrl->line_state |= (events & SLI_USBD_CDC_ACM_EVENTS_CONS);

    if (p_ctrl->line_state_sent == false) {
      p_ctrl->line_state_sent = true;
      p_ctrl->line_state_buf_ptr[SL_USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->line_state
                                                                    | (events & SLI_USBD_CDC_ACM_EVENTS_IRRE);
      CORE_EXIT_ATOMIC();

      status = sl_usbd_cdc_notify_host(p_ctrl->nbr,
                                       SLI_USBD_CDC_ACM_REQ_STATE,
                                       0u,
                                       &p_ctrl->line_state_buf_ptr[0],
                                       SLI_USBD_CDC_ACM_REQ_STATE_SIZE,
                                       &result);
      if (status != SL_STATUS_OK) {
        return status;
      }
    } else {
      p_ctrl->line_state |= (events & SLI_USBD_CDC_ACM_EVENTS_IRRE);
      CORE_EXIT_ATOMIC();
    }
  } else {
    CORE_EXIT_ATOMIC();
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Clears a line state event(s)
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_acm_clear_line_state_event(uint8_t subclass_nbr,
                                                   uint8_t events)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  uint8_t           line_state_clr;
  bool          conn;
  bool          result;
  sl_status_t            status;
  CORE_DECLARE_IRQ_STATE;

  if (subclass_nbr >= SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_acm.ctrl_table[subclass_nbr];

  sl_usbd_cdc_is_enabled(p_ctrl->nbr, &conn);

  if (conn == false) {
    return SL_STATUS_INVALID_STATE;
  }

  events &= SLI_USBD_CDC_ACM_EVENTS_CONS;
  CORE_ENTER_ATOMIC();
  line_state_clr = events & p_ctrl->line_state;

  if (line_state_clr != 0x00u) {
    p_ctrl->line_state &= ~(uint32_t)events;

    if (p_ctrl->line_state_sent == false) {
      p_ctrl->line_state_sent = true;
      p_ctrl->line_state_buf_ptr[SL_USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->line_state;
      CORE_EXIT_ATOMIC();

      status = sl_usbd_cdc_notify_host(p_ctrl->nbr,
                                       SLI_USBD_CDC_ACM_REQ_STATE,
                                       0u,
                                       p_ctrl->line_state_buf_ptr,
                                       SLI_USBD_CDC_ACM_REQ_STATE_SIZE,
                                       &result);
      if (status != SL_STATUS_OK) {
        return status;
      }
    } else {
      CORE_EXIT_ATOMIC();
    }
  } else {
    CORE_EXIT_ATOMIC();
  }

  return SL_STATUS_OK;
}
/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       usbd_cdc_acm_enable()
 *
 * @brief    Called when CDC class receives enable event
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *******************************************************************************************************/
static void usbd_cdc_acm_enable(void *p_subclass_arg)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;

  p_ctrl = (sli_usbd_cdc_acm_ctrl_t *)p_subclass_arg;

  if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->enable != NULL) {
    p_ctrl->callbacks->enable(p_ctrl->nbr);
  }

  return;
}

/****************************************************************************************************//**
 *                                       usbd_cdc_acm_disable()
 *
 * @brief    Called when CDC class receives disable event
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *******************************************************************************************************/
static void usbd_cdc_acm_disable(void *p_subclass_arg)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;

  p_ctrl = (sli_usbd_cdc_acm_ctrl_t *)p_subclass_arg;

  if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->disable != NULL) {
    p_ctrl->callbacks->disable(p_ctrl->nbr);
  }

  return;
}

/****************************************************************************************************//**
 *                                           usbd_cdc_acm_class_request_handler()
 *
 * @brief    CDC ACM serial emulation class management request.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *
 * @return   true,   if NO error(s) occurred and request is supported.
 *           false, otherwise.
 *
 * @note     (1) CDC ACM defines the following requests :
 *               - (a) SET_COMM_FEATURE        This request controls the settings for a particular communications
 *                                             feature of a particular target.
 *               - (b) GET_COMM_FEATURE        This request returns the current settings for the communications
 *                                             feature as selected
 *               - (c) CLEAR_COMM_FEATURE      This request controls the settings for a particular communications
 *                                             feature of a particular target, setting the selected feature
 *                                             to its default state.
 *               - (d) SET_LINE_CODING         This request allows the host to specify typical asynchronous
 *                                             line-character formatting properties, which may be required by
 *                                             some applications.
 *               - (e) GET_LINE_CODING         This request allows the host to find out the currently configured
 *                                             line coding.
 *               - (f) SET_CONTROL_LINE_STATE  This request generates RS-232/V.24 style control signals.
 *               - (g) SEND_BREAK              This request sends special carrier modulation that generates an
 *                                             RS-232 style break.
 *               See 'Universal Serial Bus Communications Class Subclass Specification for PSTN Devices
 *               02/09/2007, Version 1.2', section 6.2.2 for more details about ACM requests.
 *
 * @note     (2) 'SEND_BREAK' with variable length is not implemented in most USB host stacks.
 *               This feature may be implemented in the future.
 *******************************************************************************************************/
static bool usbd_cdc_acm_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                               void                      *p_subclass_arg)
{
  sli_usbd_cdc_acm_ctrl_t        *p_ctrl;
  uint8_t                  request_code;
  sl_usbd_cdc_acm_line_coding_t line_coding;
  uint8_t                  event_chngd;
  uint8_t                  event_state;
  bool                 valid;
  bool                 result;
  uint32_t                  xfer_len;
  sl_status_t                   status;
  CORE_DECLARE_IRQ_STATE;

  p_ctrl = (sli_usbd_cdc_acm_ctrl_t *)p_subclass_arg;
  request_code = p_setup_req->bRequest;
  valid = false;

  switch (request_code) {
    case SL_USBD_CDC_REQ_SET_COMM_FEATURE:
      // Only 'ABSTRACT_STATE' feature is supported.
      if (p_setup_req->wValue == SLI_USBD_CDC_ACM_ABSTRACT_STATE) {
        status = sl_usbd_core_read_control_sync((void *)p_ctrl->req_buf_ptr,
                                                2u,
                                                SLI_USBD_CDC_ACM_CTRL_REQ_TIMEOUT_mS,
                                                &xfer_len);
        // Multiplexing call management command on data is not supported
        if ((status == SL_STATUS_OK)
            && (SL_IS_BIT_CLEAR(p_ctrl->req_buf_ptr[0], SLI_USBD_CDC_ACM_ABSTRACT_DATA_MUX))) {
          CORE_ENTER_ATOMIC();
          p_ctrl->idle = SL_IS_BIT_SET(p_ctrl->req_buf_ptr[0], SLI_USBD_CDC_ACM_ABSTRACT_IDLE);
          CORE_EXIT_ATOMIC();
          valid = true;
        }
      }
      break;

    case SL_USBD_CDC_REQ_GET_COMM_FEATURE:
      if (p_setup_req->wValue == SLI_USBD_CDC_ACM_ABSTRACT_STATE) {
        p_ctrl->req_buf_ptr[0] = (p_ctrl->idle == false) ? SLI_USBD_CDC_ACM_ABSTRACT_IDLE : 0x00u;
        p_ctrl->req_buf_ptr[1] = 0x00u;

        status = sl_usbd_core_write_control_sync((void *)&p_ctrl->req_buf_ptr[0],
                                                 2u,
                                                 SLI_USBD_CDC_ACM_CTRL_REQ_TIMEOUT_mS,
                                                 false,
                                                 &xfer_len);
        if (status == SL_STATUS_OK) {
          valid = true;
        }
      }
      break;

    case SL_USBD_CDC_REQ_CLR_COMM_FEATURE:
      if (p_setup_req->wValue == SLI_USBD_CDC_ACM_ABSTRACT_STATE) {
        CORE_ENTER_ATOMIC();
        p_ctrl->idle = false;
        CORE_EXIT_ATOMIC();
        valid = true;
      }
      break;

    case SL_USBD_CDC_REQ_SET_LINE_CODING:
      status = sl_usbd_core_read_control_sync((void *)&p_ctrl->req_buf_ptr[0],
                                              7u,
                                              SLI_USBD_CDC_ACM_CTRL_REQ_TIMEOUT_mS,
                                              &xfer_len);

      if (status == SL_STATUS_OK) {
        memcpy(&line_coding.baudrate, &p_ctrl->req_buf_ptr[0], 4u);
        line_coding.stop_bits = p_ctrl->req_buf_ptr[4];
        line_coding.parity = p_ctrl->req_buf_ptr[5];
        line_coding.data_bits = p_ctrl->req_buf_ptr[6];

        valid = true;
        if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->line_coding_changed != NULL) {
          valid = p_ctrl->callbacks->line_coding_changed(p_ctrl->nbr, &line_coding);
        }

        if (valid == true) {
          CORE_ENTER_ATOMIC();
          p_ctrl->line_coding.baudrate = line_coding.baudrate;
          p_ctrl->line_coding.data_bits = line_coding.data_bits;
          p_ctrl->line_coding.stop_bits = line_coding.stop_bits;
          p_ctrl->line_coding.parity = line_coding.parity;
          CORE_EXIT_ATOMIC();
        }
      }
      break;

    case SL_USBD_CDC_REQ_GET_LINE_CODING:
      memcpy(&p_ctrl->req_buf_ptr[0], &p_ctrl->line_coding.baudrate, 4u);
      p_ctrl->req_buf_ptr[4] = p_ctrl->line_coding.stop_bits;
      p_ctrl->req_buf_ptr[5] = p_ctrl->line_coding.parity;
      p_ctrl->req_buf_ptr[6] = p_ctrl->line_coding.data_bits;
      status = sl_usbd_core_write_control_sync((void *)&p_ctrl->req_buf_ptr[0],
                                               7u,
                                               SLI_USBD_CDC_ACM_CTRL_REQ_TIMEOUT_mS,
                                               false,
                                               &xfer_len);

      if (status == SL_STATUS_OK) {
        valid = true;
      }
      break;

    case SL_USBD_CDC_REQ_SET_CTRL_LINE_STATE:
      event_state = (SL_IS_BIT_SET(p_setup_req->wValue, SLI_USBD_CDC_ACM_REQ_RTS) == true) ? SL_USBD_CDC_ACM_CTRL_RTS
                    : 0x00u;
      event_state |= (SL_IS_BIT_SET(p_setup_req->wValue, SLI_USBD_CDC_ACM_REQ_DTR) == true) ? SL_USBD_CDC_ACM_CTRL_DTR
                     : 0x00u;
      event_chngd = p_ctrl->line_ctrl
                    ^  event_state;

      if ((SL_IS_BIT_CLEAR(p_ctrl->line_ctrl, SL_USBD_CDC_ACM_CTRL_DTR) == true)
          && (SL_IS_BIT_SET(p_setup_req->wValue, SLI_USBD_CDC_ACM_REQ_DTR) == true)) {
        CORE_ENTER_ATOMIC();

        if (p_ctrl->line_state_sent == false) {
          p_ctrl->line_state_sent = true;
          p_ctrl->line_state_buf_ptr[SL_USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->line_state;
          CORE_EXIT_ATOMIC();

          sl_usbd_cdc_notify_host(p_ctrl->nbr,
                                  SLI_USBD_CDC_ACM_REQ_STATE,
                                  0u,
                                  &p_ctrl->line_state_buf_ptr[0],
                                  SLI_USBD_CDC_ACM_REQ_STATE_SIZE,
                                  &result);
        } else {
          CORE_EXIT_ATOMIC();
        }
      }

      if (event_chngd != 0x00u) {
        CORE_ENTER_ATOMIC();
        p_ctrl->line_ctrl |= (event_state & event_chngd);
        p_ctrl->line_ctrl &= ~(~(uint32_t)event_state & event_chngd);
        CORE_EXIT_ATOMIC();

        if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->line_control_changed != NULL) {
          p_ctrl->callbacks->line_control_changed(p_ctrl->nbr, event_state, event_chngd);
        }
      }
      valid = true;
      break;

    case SL_USBD_CDC_REQ_SEND_BREAK:                               // ------------- SEND_BREAK (see Note #1g) ------------
      if ((p_setup_req->wValue == 0x0000u) && (SL_IS_BIT_SET(p_ctrl->line_ctrl, SL_USBD_CDC_ACM_CTRL_BREAK))) {
        CORE_ENTER_ATOMIC();
        SL_CLEAR_BIT(p_ctrl->line_ctrl, (uint8_t)SL_USBD_CDC_ACM_CTRL_BREAK);
        CORE_EXIT_ATOMIC();

        if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->line_control_changed != NULL) {
          p_ctrl->callbacks->line_control_changed(p_ctrl->nbr, 0x00u, SL_USBD_CDC_ACM_CTRL_BREAK);
        }
      } else if ((p_setup_req->wValue != 0x0000u) && (SL_IS_BIT_CLEAR(p_ctrl->line_ctrl, SL_USBD_CDC_ACM_CTRL_BREAK))) {
        CORE_ENTER_ATOMIC();
        SL_SET_BIT(p_ctrl->line_ctrl, SL_USBD_CDC_ACM_CTRL_BREAK);
        CORE_EXIT_ATOMIC();

        if (p_ctrl->callbacks != NULL && p_ctrl->callbacks->line_control_changed != NULL) {
          p_ctrl->callbacks->line_control_changed(p_ctrl->nbr, SL_USBD_CDC_ACM_CTRL_BREAK, SL_USBD_CDC_ACM_CTRL_BREAK);
        }
      }
      valid = true;
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                       usbd_cdc_acm_notification_complete_cb()
 *
 * @brief    ACM subclass notification complete callback.
 *
 * @param    p_subclass_arg  Pointer to ACM subclass notification complete callback argument.
 *******************************************************************************************************/
static void usbd_cdc_acm_notification_complete_cb(void       *p_subclass_arg)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl;
  uint8_t           line_state_prev;
  uint8_t           event_chngd;
  bool          result;
  CORE_DECLARE_IRQ_STATE;

  p_ctrl = (sli_usbd_cdc_acm_ctrl_t *)p_subclass_arg;

  CORE_ENTER_ATOMIC();
  // Get prev state.
  line_state_prev = p_ctrl->line_state_buf_ptr[SL_USBD_CDC_NOTIFICATION_HEADER];

  if (SL_IS_ANY_BIT_SET(line_state_prev, SLI_USBD_CDC_ACM_EVENTS_IRRE)) {
    // Clr irregular events.
    line_state_prev &= ~(line_state_prev & SLI_USBD_CDC_ACM_EVENTS_IRRE);
  }

  event_chngd = line_state_prev ^ p_ctrl->line_state;

  if (event_chngd != 0x00u) {
    p_ctrl->line_state_buf_ptr[SL_USBD_CDC_NOTIFICATION_HEADER] = p_ctrl->line_state;
    p_ctrl->line_state &= ~(p_ctrl->line_state & SLI_USBD_CDC_ACM_EVENTS_IRRE);
    CORE_EXIT_ATOMIC();

    sl_usbd_cdc_notify_host(p_ctrl->nbr,
                            SLI_USBD_CDC_ACM_REQ_STATE,
                            0u,
                            p_ctrl->line_state_buf_ptr,
                            SLI_USBD_CDC_ACM_REQ_STATE_SIZE,
                            &result);
  } else {
    p_ctrl->line_state_sent = false;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                           usbd_cdc_acm_mgmt_interface_descriptor_cb()
 *
 * @brief    CDC Subclass interface descriptor callback.
 *
 * @param    p_subclass_arg      Pointer to subclass argument.
 *
 * @param    first_dci_if_nbr    Interface number of the first Data Class Interface following a
 *                               Communication Class Interface (see Note #1).
 *
 * @note     (1) The Call Management Functional Descriptor contains the field 'bDataInterface' which
 *               represents the interface number of Data Class interface (DCI) optionally used for call
 *               management. In case the Communication Class Interface is followed by several DCIs,
 *               the interface number set for 'bDataInterface' will be the first DCI. It is NOT
 *               possible to use another DCI for handling the call management.
 *
 * @internal
 * @note     (2) [INTERNAL] Table 4 of the PSTN specification describes the abstract control management
 *               functional descriptor. The 'bmCapabilities' (offset 3) field describes the requests
 *               supported by the device.
 *               @verbatim
 *                   BIT       STATE     DESCRIPTION
 *                   -------   -------    -------------
 *                   D0       '1'       Device supports the request combination of SET_COMM_FEATURE,
 *                                       CLR_COMM_FEATURE, and GET_COMM_FEATURE.
 *
 *                   D1       '1'       Device supports the request combination of SET_LINE_CODING,
 *                                       SET_CTRL_LINE_STATE, GET_LINE_CODING and the notification state.
 *
 *                   D2       '1'       Device supports the request SEND_BREAK.
 *
 *                   D3       '1'       Device supports the notification NETWORK_CONNECTION.
 *                                       Not required in ACM serial emulation subclass.
 *               @endverbatim
 *
 * @note     (3) [INTERNAL] Table 3 of the PSTN specification describes the call management functional
 *               descriptor. The 'bmCapabilities' (offset 3) field describes how call management is
 *               handled by the device.
 *               @verbatim
 *                   BIT       STATE     DESCRIPTION
 *                   -------   -------    -------------
 *                   D0       '0'       Device does not handle call management itself.
 *
 *                           '1'       Device handles call management itself.
 *
 *                   D1       '0'       Device send/receives call management information only over the
 *                                       communication class interface.
 *
 *                           '1'       Device can send/receive call management information over a data
 *                                       class interface.
 *               @endverbatim
 * @endinternal
 *******************************************************************************************************/
static void usbd_cdc_acm_mgmt_interface_descriptor_cb(void       *p_subclass_arg,
                                                      uint8_t    first_dci_if_nbr)
{
  sli_usbd_cdc_acm_ctrl_t *p_ctrl = (sli_usbd_cdc_acm_ctrl_t *)p_subclass_arg;

  // Build Abstract Control Mgmt Desc
  sl_usbd_core_write_08b_to_descriptor_buf(4u);                                       // Desc size.
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_TYPE_CS_IF);              // Desc type.
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_SUBTYPE_ACM);             // Desc subtype.
  // Dev request capabilities (see Note #2).
  sl_usbd_core_write_08b_to_descriptor_buf(0x01u
                                           | 0x02u
                                           | 0x04u);

  //  Build Call Mgmt Desc
  sl_usbd_core_write_08b_to_descriptor_buf(5u);                                       // Desc size.
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_TYPE_CS_IF);              // Desc type.
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_SUBTYPE_CALL_MGMT);       // Desc subtype.
  sl_usbd_core_write_08b_to_descriptor_buf(p_ctrl->call_mgmt_capabilities);           // Dev call mgmt capabilities (see Note #3).
  // IF nbr of data class IF optionally used for call mgmt.
  if (p_ctrl->call_mgmt_capabilities == SL_USBD_CDC_ACM_CALL_MGMT_DATA_OVER_DCI) {
    sl_usbd_core_write_08b_to_descriptor_buf(first_dci_if_nbr);                       // See Note #1.
  } else {
    sl_usbd_core_write_08b_to_descriptor_buf(0u);
  }
}

/****************************************************************************************************//**
 *                                       usbd_cdc_acm_get_mgmt_interface_descriptor_size()
 *
 * @brief    Retrieve the size of the CDC subclass interface descriptor.
 *
 * @param    p_subclass_arg  Pointer to subclass argument.
 *******************************************************************************************************/
static uint16_t usbd_cdc_acm_get_mgmt_interface_descriptor_size(void       *p_subclass_arg)
{
  (void)&p_subclass_arg;

  return (SLI_USBD_CDC_ACM_DESC_TOT_SIZE);
}
