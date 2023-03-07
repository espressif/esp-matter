/***************************************************************************//**
 * @file
 * @brief USB Device Hid Class
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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    SLI_USBD_HID_MODULE

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "sl_bit.h"
#include "sl_status.h"

#include "em_core.h"

#include "sl_usbd_core.h"
#include "sli_usbd_core.h"

#include "sl_usbd_class_hid.h"
#include "sli_usbd_class_hid.h"
#include "sli_usbd_class_hid_os.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS                  5000u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC LOCAL DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.1 for more details about class-descriptor types.
 *
 *               (a) The 'wValue' field of Get_Descriptor Request specifies the Descriptor type in the
 *                   high byte.
 *
 *           (2) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about class-specific requests.
 *
 *               (a) The 'bRequest' field of SETUP packet may contain one of these values.
 *******************************************************************************************************/

#define  SLI_USBD_HID_DESC_LEN                                 6u

// Class-Specific Desc (see Note #1)
#define  SLI_USBD_HID_DESC_TYPE_HID                         0x21
#define  SLI_USBD_HID_DESC_TYPE_REPORT                      0x22
#define  SLI_USBD_HID_DESC_TYPE_PHYSICAL                    0x23
// Class-Specific Req (see Note #2)
#define  SLI_USBD_HID_REQ_GET_REPORT                        0x01
#define  SLI_USBD_HID_REQ_GET_IDLE                          0x02
#define  SLI_USBD_HID_REQ_GET_PROTOCOL                      0x03
#define  SLI_USBD_HID_REQ_SET_REPORT                        0x09
#define  SLI_USBD_HID_REQ_SET_IDLE                          0x0A
#define  SLI_USBD_HID_REQ_SET_PROTOCOL                      0x0B

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static sli_usbd_hid_t usbd_hid_obj;
sli_usbd_hid_t *usbd_hid_ptr = NULL;

static uint32_t ctrl_status_buffer;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_hid_enable(uint8_t config_nbr,
                            void    *p_if_class_arg);

static void usbd_hid_disable(uint8_t config_nbr,
                             void    *p_if_class_arg);

static void usbd_hid_update_alt_setting(uint8_t config_nbr,
                                        uint8_t if_nbr,
                                        uint8_t if_alt_nbr,
                                        void    *p_if_class_arg,
                                        void    *p_if_alt_class_arg);

static void usbd_hid_update_endpoint_state(uint8_t config_nbr,
                                           uint8_t if_nbr,
                                           uint8_t if_alt_nbr,
                                           uint8_t ep_addr,
                                           void    *p_if_class_arg,
                                           void    *p_if_alt_class_arg);

static void usbd_hid_interface_descriptor_cb(uint8_t config_nbr,
                                             uint8_t if_nbr,
                                             uint8_t if_alt_nbr,
                                             void    *p_if_class_arg,
                                             void    *p_if_alt_class_arg);

static void usbd_hid_interface_descriptor_handler(void *p_if_class_arg);

static uint16_t usbd_hid_get_interface_descriptor_size(uint8_t config_nbr,
                                                       uint8_t if_nbr,
                                                       uint8_t if_alt_nbr,
                                                       void    *p_if_class_arg,
                                                       void    *p_if_alt_class_arg);

static bool usbd_hid_interface_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                               void                      *p_if_class_arg);

static bool usbd_hid_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                           void                      *p_if_class_arg);

static void usbd_hid_write_async_complete(uint8_t      ep_addr,
                                          void         *p_buf,
                                          uint32_t     buf_len,
                                          uint32_t     xfer_len,
                                          void         *p_arg,
                                          sl_status_t  status);

static void usbd_hid_write_sync_complete(uint8_t      ep_addr,
                                         void         *p_buf,
                                         uint32_t     buf_len,
                                         uint32_t     xfer_len,
                                         void         *p_arg,
                                         sl_status_t  status);

static void usbd_hid_read_async_complete(uint8_t      ep_addr,
                                         void         *p_buf,
                                         uint32_t     buf_len,
                                         uint32_t     xfer_len,
                                         void         *p_arg,
                                         sl_status_t  status);

static void usbd_hid_output_data_complete(uint8_t      class_nbr,
                                          void         *p_buf,
                                          uint32_t     buf_len,
                                          uint32_t     xfer_len,
                                          void         *p_arg,
                                          sl_status_t  status);

/********************************************************************************************************
 *                                           HID CLASS DRIVER
 *******************************************************************************************************/

static sl_usbd_class_driver_t usbd_hid_driver = {
  usbd_hid_enable,
  usbd_hid_disable,
  usbd_hid_update_alt_setting,
  usbd_hid_update_endpoint_state,
  usbd_hid_interface_descriptor_cb,
  usbd_hid_get_interface_descriptor_size,
  NULL,
  NULL,
  usbd_hid_interface_request_handler,
  usbd_hid_class_request_handler,
  NULL,

#if (USBD_CFG_MS_OS_DESC_EN == 1)
  NULL,
  NULL
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialize HID class
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_init(void)
{
  uint8_t    ix;
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  sl_status_t     status;

  usbd_hid_ptr = &usbd_hid_obj;

  status = sli_usbd_hid_os_init();
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sli_usbd_hid_report_init();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Init HID class struct.
  for (ix = 0u; ix < SL_USBD_HID_CLASS_INSTANCE_QUANTITY; ix++) {
    p_ctrl = &usbd_hid_ptr->ctrl_table[ix];
    p_ctrl->class_nbr = SL_USBD_CLASS_NBR_NONE;
    p_ctrl->state = SLI_USBD_HID_STATE_NONE;
    p_ctrl->comm_ptr = NULL;
    p_ctrl->subclass_code = SL_USBD_HID_SUBCLASS_NONE;
    p_ctrl->protocol_code = SL_USBD_HID_PROTOCOL_NONE;
    p_ctrl->country_code = SL_USBD_HID_COUNTRY_CODE_NOT_SUPPORTED;
    p_ctrl->report_descriptor_ptr = NULL;
    p_ctrl->report_descriptor_len = 0u;
    p_ctrl->phy_descriptor_ptr = NULL;
    p_ctrl->phy_descriptor_len = 0u;
    p_ctrl->interval_in = 0u;
    p_ctrl->interval_out = 0u;
    p_ctrl->ctrl_read_enable = true;
    p_ctrl->callback_ptr = NULL;
    p_ctrl->interrupt_write_async_function = NULL;
    p_ctrl->interrupt_write_async_arg_ptr = NULL;
    p_ctrl->interrupt_read_async_function = NULL;
    p_ctrl->interrupt_read_async_arg_ptr = NULL;

    p_ctrl->ctrl_status_buf_ptr = (uint8_t *)&ctrl_status_buffer;
    if (p_ctrl->ctrl_status_buf_ptr == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  // Init HID EP tbl.
  for (ix = 0u; ix < SL_USBD_HID_CLASS_INSTANCE_QUANTITY * SL_USBD_HID_CONFIGURATION_QUANTITY; ix++) {
    p_comm = &usbd_hid_ptr->comm_tbl[ix];

    p_comm->ctrl_ptr = NULL;
    p_comm->data_interrupt_in_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->data_interrupt_out_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->data_interrupt_out_active_transfer = false;
  }

  usbd_hid_ptr->ctrl_nbr_next = SL_USBD_HID_CLASS_INSTANCE_QUANTITY;
  usbd_hid_ptr->comm_nbr_next = SL_USBD_HID_CLASS_INSTANCE_QUANTITY * SL_USBD_HID_CONFIGURATION_QUANTITY;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a new instance of the HID class
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_create_instance(uint8_t                     subclass,
                                        uint8_t                     protocol,
                                        sl_usbd_hid_country_code_t  country_code,
                                        uint16_t                    interval_in,
                                        uint16_t                    interval_out,
                                        bool                        ctrl_rd_en,
                                        sl_usbd_hid_callbacks_t     *p_hid_callbacks,
                                        uint8_t                     *p_class_nbr)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  uint8_t    class_nbr;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if (p_class_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // interval for intr IN must be a power of 2.
  if (!SLI_USBD_IS_PWR2(interval_in)) {
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // interval for intr OUT must be a power of 2.
  if (!((ctrl_rd_en == true) || (SLI_USBD_IS_PWR2(interval_out) == true))) {
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_hid_ptr->ctrl_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_hid_ptr->ctrl_nbr_next--;
  // Alloc new HID class instance.
  class_nbr = usbd_hid_ptr->ctrl_nbr_next;
  CORE_EXIT_ATOMIC();

  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  p_ctrl->subclass_code = subclass;
  p_ctrl->protocol_code = protocol;
  p_ctrl->country_code = country_code;
  p_ctrl->interval_in = interval_in;
  p_ctrl->interval_out = interval_out;
  p_ctrl->ctrl_read_enable = ctrl_rd_en;
  p_ctrl->callback_ptr = p_hid_callbacks;

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->get_report_desc != NULL) {
    p_ctrl->callback_ptr->get_report_desc(class_nbr,
                                          &p_ctrl->report_descriptor_ptr,
                                          &p_ctrl->report_descriptor_len);
  }

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->get_phy_desc != NULL) {
    p_ctrl->callback_ptr->get_phy_desc(class_nbr,
                                       &p_ctrl->phy_descriptor_ptr,
                                       &p_ctrl->phy_descriptor_len);
  }

  status = sli_usbd_hid_report_parse(class_nbr,
                                     p_ctrl->report_descriptor_ptr,
                                     p_ctrl->report_descriptor_len,
                                     &p_ctrl->report);

  if (status != SL_STATUS_OK) {
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return status;
  }

  *p_class_nbr = class_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds the HID class instance to the USB device configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint8_t    if_nbr;
  uint8_t    ep_addr;
  uint16_t    comm_nbr;
  uint16_t    interval_in;
  uint16_t    interval_out;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();

  if (usbd_hid_ptr->comm_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_hid_ptr->comm_nbr_next--;
  // Alloc new HID class comm info.
  comm_nbr = usbd_hid_ptr->comm_nbr_next;
  CORE_EXIT_ATOMIC();

  p_comm = &usbd_hid_ptr->comm_tbl[comm_nbr];

  // Config Desc Construction
  // See Note #2.

  // Add HID IF desc to config desc.
  status = sl_usbd_core_add_interface(config_nbr,
                                      &usbd_hid_driver,
                                      (void *)p_comm,
                                      (void *)0,
                                      SL_USBD_CLASS_CODE_HID,
                                      p_ctrl->subclass_code,
                                      p_ctrl->protocol_code,
                                      "HID Class",
                                      &if_nbr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
    // In FS, bInterval in frames.
    interval_in = p_ctrl->interval_in;
    interval_out = p_ctrl->interval_out;
  } else {
    // In HS, bInterval in microframes.
    interval_in = p_ctrl->interval_in * 8u;
    interval_out = p_ctrl->interval_out * 8u;
  }

  // Add intr IN EP desc.
  status = sl_usbd_core_add_interrupt_endpoint(config_nbr,
                                               if_nbr,
                                               0u,
                                               true,
                                               0u,
                                               interval_in,
                                               &ep_addr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Store intr IN EP addr.
  p_comm->data_interrupt_in_endpoint_address = ep_addr;

  if (p_ctrl->ctrl_read_enable == false) {
    // Add intr OUT EP desc.
    status = sl_usbd_core_add_interrupt_endpoint(config_nbr,
                                                 if_nbr,
                                                 0u,
                                                 false,
                                                 0u,
                                                 interval_out,
                                                 &ep_addr);

    if (status != SL_STATUS_OK) {
      return status;
    }
  } else {
    ep_addr = SL_USBD_ENDPOINT_ADDR_NONE;
  }

  // Store intr OUT EP addr.
  p_comm->data_interrupt_out_endpoint_address = ep_addr;

  // Store HID class instance info.
  CORE_ENTER_ATOMIC();
  p_ctrl->class_nbr = class_nbr;
  p_ctrl->state = SLI_USBD_HID_STATE_INIT;
  CORE_EXIT_ATOMIC();

  p_comm->ctrl_ptr = p_ctrl;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the HID class enable state
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_is_enabled(uint8_t  class_nbr,
                                   bool     *p_enabled)
{
  sli_usbd_hid_ctrl_t  *p_ctrl;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  if (p_enabled == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];
  // Get dev state.
  status = sl_usbd_core_get_device_state(&state);

  if ((status == SL_STATUS_OK)
      && (state == SL_USBD_DEVICE_STATE_CONFIGURED)
      && (p_ctrl->state == SLI_USBD_HID_STATE_CONFIG)) {
    *p_enabled = true;
    return SL_STATUS_OK;
  } else {
    *p_enabled = false;
    return status;
  }
}

/****************************************************************************************************//**
 * Receives  data from the host through the Interrupt OUT endpoint. This function is blocking.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_read_sync(uint8_t    class_nbr,
                                  void       *p_buf,
                                  uint32_t   buf_len,
                                  uint16_t   timeout,
                                  uint32_t   *p_xfer_len)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint32_t    xfer_len;
  bool   conn;
  sl_status_t     status;
  sl_status_t     local_status;
  CORE_DECLARE_IRQ_STATE;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0)) {
    *p_xfer_len = 0u;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->comm_ptr;
  sl_usbd_hid_is_enabled(class_nbr, &conn);
  CORE_EXIT_ATOMIC();

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Use SET_REPORT to rx data instead.
  if (p_ctrl->ctrl_read_enable == true) {
    status = sli_usbd_hid_os_lock_output(class_nbr);

    if (status != SL_STATUS_OK) {
      *p_xfer_len = 0u;
      return status;
    }

    if (p_ctrl->is_rx == true) {
      sli_usbd_hid_os_unlock_output(class_nbr);
      *p_xfer_len = 0u;
      return SL_STATUS_FAIL;
    }
    p_ctrl->is_rx = true;
    p_ctrl->rx_buf_len = buf_len;
    p_ctrl->rx_buf_ptr = (uint8_t *)p_buf;
    // Save app rx callback.
    p_ctrl->interrupt_read_async_function = usbd_hid_output_data_complete;
    p_ctrl->interrupt_read_async_arg_ptr = &xfer_len;

    sli_usbd_hid_os_unlock_output(class_nbr);
    local_status = sli_usbd_hid_os_pend_output(class_nbr, timeout);
    status = sli_usbd_hid_os_lock_output(class_nbr);

    if (status != SL_STATUS_OK) {
      *p_xfer_len = 0u;
      return status;
    }

    status = local_status;

    if (status == SL_STATUS_TIMEOUT) {
      p_ctrl->rx_buf_len = 0u;
      p_ctrl->rx_buf_ptr = (uint8_t *)0;

      p_ctrl->interrupt_read_async_function = 0;
      p_ctrl->interrupt_read_async_arg_ptr = 0;

      p_ctrl->is_rx = false;

      sli_usbd_hid_os_unlock_output(class_nbr);

      *p_xfer_len = 0u;
      return status;
    }

    sli_usbd_hid_os_unlock_output(class_nbr);

    if (status != SL_STATUS_OK) {
      *p_xfer_len = 0u;
      return status;
    }

    *p_xfer_len = xfer_len;
    return SL_STATUS_OK;
  }

  // Intr Out Comm
  status = sl_usbd_core_read_interrupt_sync(p_comm->data_interrupt_out_endpoint_address,
                                            p_buf,
                                            buf_len,
                                            timeout,
                                            &xfer_len);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Receives  data from the host through the Interrupt OUT endpoint. This function is non-blocking.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_read_async(uint8_t                      class_nbr,
                                   void                         *p_buf,
                                   uint32_t                     buf_len,
                                   sl_usbd_hid_async_function_t async_fnct,
                                   void                         *p_async_arg)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  bool   conn;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if ((p_buf == NULL) && (buf_len != 0)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->comm_ptr;
  sl_usbd_hid_is_enabled(class_nbr, &conn);
  CORE_EXIT_ATOMIC();

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Use SET_REPORT to rx data instead.
  if (p_ctrl->ctrl_read_enable == true) {
    status = sli_usbd_hid_os_lock_output(class_nbr);

    if (status != SL_STATUS_OK) {
      return status;
    }

    if (p_ctrl->is_rx == true) {
      sli_usbd_hid_os_unlock_output(class_nbr);
      return SL_STATUS_FAIL;
    }

    p_ctrl->is_rx = true;
    p_ctrl->rx_buf_len = buf_len;
    p_ctrl->rx_buf_ptr = (uint8_t *)p_buf;
    // Save app rx callback.
    p_ctrl->interrupt_read_async_function = async_fnct;
    p_ctrl->interrupt_read_async_arg_ptr = p_async_arg;

    sli_usbd_hid_os_unlock_output(class_nbr);
  } else {
    // Save app rx callback.
    CORE_ENTER_ATOMIC();
    p_ctrl->interrupt_read_async_function = async_fnct;
    p_ctrl->interrupt_read_async_arg_ptr = p_async_arg;
    // Intr Out Comm
    // Check if another xfer is already in progress.
    if (p_comm->data_interrupt_out_active_transfer == false) {
      // Indicate that a xfer is in progress.
      p_comm->data_interrupt_out_active_transfer = true;
      CORE_EXIT_ATOMIC();

      status = sl_usbd_core_read_interrupt_async(p_comm->data_interrupt_out_endpoint_address,
                                                 p_buf,
                                                 buf_len,
                                                 usbd_hid_read_async_complete,
                                                 (void *)p_comm);

      if (status != SL_STATUS_OK) {
        CORE_ENTER_ATOMIC();
        p_comm->data_interrupt_out_active_transfer = false;
        CORE_EXIT_ATOMIC();
      }
    } else {
      CORE_ENTER_ATOMIC();
      return SL_STATUS_NOT_READY;
    }
  }

  return status;
}

/****************************************************************************************************//**
 * Sends data to the host through the Interrupt IN endpoint. This function is blocking.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_write_sync(uint8_t    class_nbr,
                                   void       *p_buf,
                                   uint32_t   buf_len,
                                   uint16_t   timeout,
                                   uint32_t   *p_xfer_len)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint32_t    xfer_len;
  uint8_t    report_id;
  uint8_t    *p_buf_data;
  uint8_t    *p_buf_report;
  uint16_t    report_len;
  bool   is_largest;
  bool   conn;
  bool   eot;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0)) {
    *p_xfer_len = 0u;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->comm_ptr;
  sl_usbd_hid_is_enabled(class_nbr, &conn);
  CORE_EXIT_ATOMIC();

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Store Report Data
  if (buf_len > 0) {
    p_buf_data = (uint8_t *)p_buf;
    if (p_ctrl->report.has_reports == true) {
      report_id = p_buf_data[0];
    } else {
      report_id = 0;
    }

    status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                          SLI_USBD_HID_REPORT_TYPE_INPUT,
                                          report_id,
                                          &p_buf_report,
                                          &is_largest,
                                          &report_len);

    if (status != SL_STATUS_OK) {
      *p_xfer_len = 0u;
      return status;
    }

    if (report_len > buf_len) {
      *p_xfer_len = 0u;
      return SL_STATUS_INVALID_PARAMETER;
    }

    eot = !is_largest;
  } else {
    p_buf_data = (uint8_t *)0;
    p_buf_report = (uint8_t *)0;
    report_len = 0;
    eot = true;
  }

  status = sli_usbd_hid_os_lock_tx(class_nbr);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  // Intr In Comm
  status = sl_usbd_core_write_interrupt_async(p_comm->data_interrupt_in_endpoint_address,
                                              p_buf,
                                              buf_len,
                                              usbd_hid_write_sync_complete,
                                              (void *)p_comm,
                                              eot);

  if (status != SL_STATUS_OK) {
    sli_usbd_hid_os_unlock_tx(class_nbr);
    *p_xfer_len = 0u;
    return status;
  }

  // Defer copy while transmitting.
  if (buf_len > 0) {
    status = sli_usbd_hid_os_lock_input(class_nbr);

    if (status != SL_STATUS_OK) {
      sli_usbd_hid_os_unlock_tx(class_nbr);
      *p_xfer_len = 0u;
      return status;
    }

    memcpy(&p_buf_report[0], &p_buf_data[0], report_len);
    sli_usbd_hid_os_unlock_input(class_nbr);
  }

  status = sli_usbd_hid_os_pend_input(class_nbr, timeout);

  if (status != SL_STATUS_OK) {
    if (status == SL_STATUS_TIMEOUT) {
      status = sl_usbd_core_abort_endpoint(p_comm->data_interrupt_in_endpoint_address);
    }

    sli_usbd_hid_os_unlock_tx(class_nbr);

    *p_xfer_len = 0u;
    return status;
  }

  xfer_len = p_ctrl->data_interrupt_in_transfer_len;

  sli_usbd_hid_os_unlock_tx(class_nbr);

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sends data to the host through the Interrupt IN endpoint. This function is non-blocking.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_write_async(uint8_t                      class_nbr,
                                    void                         *p_buf,
                                    uint32_t                     buf_len,
                                    sl_usbd_hid_async_function_t async_fnct,
                                    void                         *p_async_arg)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint8_t    report_id;
  uint8_t    *p_buf_data;
  uint8_t    *p_buf_report;
  uint16_t    report_len;
  bool   is_largest;
  bool   conn;
  bool   eot;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if ((p_buf == NULL) && (buf_len != 0)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_HID_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (class_nbr < usbd_hid_ptr->ctrl_nbr_next) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get HID class instance.
  p_ctrl = &usbd_hid_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  p_comm = p_ctrl->comm_ptr;
  sl_usbd_hid_is_enabled(class_nbr, &conn);
  CORE_EXIT_ATOMIC();

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Store Report Data
  if (buf_len > 0) {
    p_buf_data = (uint8_t *)p_buf;

    if (p_ctrl->report.has_reports == true) {
      report_id = p_buf_data[0];
    } else {
      report_id = 0;
    }

    status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                          SLI_USBD_HID_REPORT_TYPE_INPUT,
                                          report_id,
                                          &p_buf_report,
                                          &is_largest,
                                          &report_len);

    if (status != SL_STATUS_OK) {
      return status;
    }

    if (report_len > buf_len) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    eot = !is_largest;
  } else {
    p_buf_data = (uint8_t *)0;
    p_buf_report = (uint8_t *)0;
    report_len = 0;
    eot = true;
  }

  status = sli_usbd_hid_os_lock_tx(class_nbr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Save app rx callback.
  p_ctrl->interrupt_write_async_function = async_fnct;
  p_ctrl->interrupt_write_async_arg_ptr = p_async_arg;

  // Intr In Comm
  status = sl_usbd_core_write_interrupt_async(p_comm->data_interrupt_in_endpoint_address,
                                              p_buf,
                                              buf_len,
                                              usbd_hid_write_async_complete,
                                              (void *)p_comm,
                                              eot);

  if (status != SL_STATUS_OK) {
    sli_usbd_hid_os_unlock_tx(class_nbr);
    return status;
  }

  // Defer copy while transmitting.
  if (buf_len > 0) {
    status = sli_usbd_hid_os_lock_input(class_nbr);

    if (status != SL_STATUS_OK) {
      sli_usbd_hid_os_unlock_tx(class_nbr);
      return status;
    }

    memcpy(&p_buf_report[0], &p_buf_data[0], report_len);

    sli_usbd_hid_os_unlock_input(class_nbr);
  }

  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               usbd_hid_enable()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void usbd_hid_enable(uint8_t  config_nbr,
                            void     *p_if_class_arg)
{
  sli_usbd_hid_comm_t *p_comm;
  sli_usbd_hid_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  CORE_ENTER_ATOMIC();
  p_ctrl->comm_ptr = p_comm;
  p_ctrl->state = SLI_USBD_HID_STATE_CONFIG;
  CORE_EXIT_ATOMIC();

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->enable != NULL) {
    p_ctrl->callback_ptr->enable(p_ctrl->class_nbr);
  }
}

/****************************************************************************************************//**
 *                                           usbd_hid_disable()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void usbd_hid_disable(uint8_t    config_nbr,
                             void       *p_if_class_arg)
{
  sli_usbd_hid_comm_t *p_comm;
  sli_usbd_hid_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  sli_usbd_hid_report_remove_all_idle(&p_ctrl->report);

  CORE_ENTER_ATOMIC();
  p_ctrl->comm_ptr = (sli_usbd_hid_comm_t *)0;
  p_ctrl->state = SLI_USBD_HID_STATE_INIT;
  if (p_ctrl->is_rx == true) {
    CORE_EXIT_ATOMIC();

    p_ctrl->interrupt_read_async_function(p_ctrl->class_nbr,
                                          p_ctrl->rx_buf_ptr,
                                          p_ctrl->rx_buf_len,
                                          0,
                                          (void *)p_ctrl->interrupt_read_async_arg_ptr,
                                          SL_STATUS_ABORT);

    CORE_ENTER_ATOMIC();
    p_ctrl->is_rx = false;
    p_ctrl->rx_buf_len = 0;
    p_ctrl->rx_buf_ptr = (uint8_t *)0;

    p_ctrl->interrupt_read_async_function = 0;
    p_ctrl->interrupt_read_async_arg_ptr = 0;
    CORE_EXIT_ATOMIC();
  } else {
    CORE_EXIT_ATOMIC();
  }

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->disable != NULL) {
    p_ctrl->callback_ptr->disable(p_ctrl->class_nbr);
  }
}

/****************************************************************************************************//**
 *                                       usbd_hid_update_alt_setting()
 *
 * @brief    Notify class that interface alternate setting has been updated.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *******************************************************************************************************/
static void usbd_hid_update_alt_setting(uint8_t    config_nbr,
                                        uint8_t    if_nbr,
                                        uint8_t    if_alt_nbr,
                                        void       *p_if_class_arg,
                                        void       *p_if_alt_class_arg)
{
  sli_usbd_hid_comm_t *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  CORE_ENTER_ATOMIC();
  p_comm->ctrl_ptr->comm_ptr = p_comm;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 *                                       usbd_hid_update_endpoint_state()
 *
 * Description : Notify class that endpoint state has been updated.
 *
 * Argument(s) : config_nbr          Configuration number.
 *
 *               if_nbr              Interface number.
 *
 *               if_alt_nbr          Interface alternate setting number.
 *
 *               ep_addr             Endpoint address.
 *
 *               p_if_class_arg      Pointer to class argument specific to interface.
 *
 *               p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) EP state may have changed, it can be checked through sl_usbd_core_is_endpoint_stalled().
 *******************************************************************************************************/
static void usbd_hid_update_endpoint_state(uint8_t    config_nbr,
                                           uint8_t    if_nbr,
                                           uint8_t    if_alt_nbr,
                                           uint8_t    ep_addr,
                                           void       *p_if_class_arg,
                                           void       *p_if_alt_class_arg)

{
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&ep_addr;
  (void)&p_if_class_arg;
  (void)&p_if_alt_class_arg;
}

/****************************************************************************************************//**
 *                                           usbd_hid_interface_descriptor_cb()
 *
 * @brief    Class interface descriptor callback.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *******************************************************************************************************/
static void usbd_hid_interface_descriptor_cb(uint8_t    config_nbr,
                                             uint8_t    if_nbr,
                                             uint8_t    if_alt_nbr,
                                             void       *p_if_class_arg,
                                             void       *p_if_alt_class_arg)

{
  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  usbd_hid_interface_descriptor_handler(p_if_class_arg);
}

/****************************************************************************************************//**
 *                                           usbd_hid_interface_descriptor_handler()
 *
 * @brief    Class interface descriptor callback handler.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void usbd_hid_interface_descriptor_handler(void       *p_if_class_arg)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint8_t    nbr_desc;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  nbr_desc = 0u;
  if (p_ctrl->report_descriptor_len > 0u) {
    nbr_desc++;
  }
  if (p_ctrl->phy_descriptor_len > 0u) {
    nbr_desc++;
  }

  sl_usbd_core_write_08b_to_descriptor_buf(SLI_USBD_HID_DESC_LEN + nbr_desc * 3u);
  sl_usbd_core_write_08b_to_descriptor_buf(SLI_USBD_HID_DESC_TYPE_HID);
  sl_usbd_core_write_16b_to_descriptor_buf(0x0111);
  sl_usbd_core_write_08b_to_descriptor_buf((uint8_t)p_ctrl->country_code);
  sl_usbd_core_write_08b_to_descriptor_buf(nbr_desc);

  if (p_ctrl->report_descriptor_len > 0u) {
    sl_usbd_core_write_08b_to_descriptor_buf(SLI_USBD_HID_DESC_TYPE_REPORT);
    sl_usbd_core_write_16b_to_descriptor_buf(p_ctrl->report_descriptor_len);
  }
  if (p_ctrl->phy_descriptor_len > 0u) {
    sl_usbd_core_write_08b_to_descriptor_buf(SLI_USBD_HID_DESC_TYPE_PHYSICAL);
    sl_usbd_core_write_16b_to_descriptor_buf(p_ctrl->phy_descriptor_len);
  }
}

/****************************************************************************************************//**
 *                                           usbd_hid_get_interface_descriptor_size()
 *
 * @brief    Retrieve the size of the class interface descriptor.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * @return   size of the class interface descriptor.
 *******************************************************************************************************/
static uint16_t usbd_hid_get_interface_descriptor_size(uint8_t    config_nbr,
                                                       uint8_t    if_nbr,
                                                       uint8_t    if_alt_nbr,
                                                       void       *p_if_class_arg,
                                                       void       *p_if_alt_class_arg)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint8_t    nbr_desc;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  nbr_desc = 0u;
  if (p_ctrl->report_descriptor_len > 0u) {
    nbr_desc++;
  }
  if (p_ctrl->phy_descriptor_len > 0u) {
    nbr_desc++;
  }

  return (SLI_USBD_HID_DESC_LEN + nbr_desc * 3u);
}

/****************************************************************************************************//**
 *                                               usbd_hid_interface_request_handler()
 *
 * @brief    Process interface requests.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   true,   if NO error(s) occurred and request is supported.
 *
 *           false, if any errors are returned.
 *
 * @note     (1) HID class supports 3 class-specific descriptors:
 *
 *           - (a) HID      descriptor (mandatory)
 *
 *           - (a) report   descriptor (mandatory)
 *
 *           - (a) Physical descriptor (optional)
 *
 *               HID descriptor is sent to Host as part of the Configuration descriptor. report and
 *               Physical descriptors are retrieved by the host using a GET_DESCRIPTOR standard request
 *               with the recipient being the interface. The way to get report and Physical descriptors
 *               is specific to the HID class. The HID class specification indicates in the section
 *               '7.1 Standard Requests':
 *
 *               "The HID class uses the standard request Get_Descriptor as described in the USB Specification.
 *               When a Get_Descriptor(Configuration) request is issued, it returns the Configuration descriptor,
 *               all Interface descriptors, all Endpoint descriptors, and the HID descriptor for each interface.
 *               It shall not return the String descriptor, HID report descriptor or any of the
 *               optional HID class descriptors."
 *
 * @note     (2) The HID descriptor identifies the length and type of subordinate descriptors for a
 *               device.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.1 for more details about HID descriptor.
 *
 * @note     (3) The report descriptor is made up of items that provide information about the device.
 *               The first part of an item contains three fields: item type, item tag, and item size.
 *               Together these fields identify the kind of information the item provides.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2 for more details about report descriptor.
 *
 * @note     (4) A Physical Descriptor is a data structure that provides information about the
 *               specific part or parts of the human body that are activating a control or controls.
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.3 for more details about Physical descriptor.
 *
 *           - (a) Descriptor set 0 is a special descriptor set that specifies the number of additional
 *                   descriptor sets, and also the number of Physical Descriptors in each set.
 *******************************************************************************************************/
static bool usbd_hid_interface_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                               void                      *p_if_class_arg)
{
  sli_usbd_hid_comm_t *p_comm;
  bool   valid;
  bool   dev_to_host;
  uint8_t    desc_type;
  uint8_t    w_value;
  uint16_t    desc_len;
  uint32_t    desc_offset;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  valid = false;

  dev_to_host = SL_IS_BIT_SET(p_setup_req->bmRequestType, SL_USBD_REQ_DIR_BIT);

  // Get Descriptor (see Note #1)
  if (p_setup_req->bRequest == SL_USBD_REQ_GET_DESCRIPTOR) {
    if (dev_to_host != true) {
      return (valid);
    }
    // Get desc type.
    desc_type = (uint8_t)((p_setup_req->wValue >> 8u) & 0xFFu);
    w_value = (uint8_t) (p_setup_req->wValue        & 0xFFu);

    switch (desc_type) {
      case SLI_USBD_HID_DESC_TYPE_HID:
        if (w_value != 0) {
          break;
        }

        usbd_hid_interface_descriptor_handler(p_if_class_arg);
        valid = true;
        break;

      case SLI_USBD_HID_DESC_TYPE_REPORT:
        if (w_value != 0) {
          break;
        }

        if (p_comm->ctrl_ptr->report_descriptor_len > 0) {
          sl_usbd_core_write_buf_to_descriptor_buf(p_comm->ctrl_ptr->report_descriptor_ptr,
                                                   p_comm->ctrl_ptr->report_descriptor_len);
          valid = true;
        }
        break;

      case SLI_USBD_HID_DESC_TYPE_PHYSICAL:
        if (p_comm->ctrl_ptr->phy_descriptor_len < 3) {
          break;
        }

        if (w_value > 0) {
          memcpy(&desc_len, (p_comm->ctrl_ptr->phy_descriptor_ptr + 1), 2u);
          desc_offset = desc_len * (w_value - 1) + 3;

          if (p_comm->ctrl_ptr->phy_descriptor_len < (desc_offset + desc_len)) {
            break;
          }

          sl_usbd_core_write_buf_to_descriptor_buf(&p_comm->ctrl_ptr->phy_descriptor_ptr[desc_offset],
                                                   desc_len);
        } else {
          // See Note #4a.
          sl_usbd_core_write_buf_to_descriptor_buf(p_comm->ctrl_ptr->phy_descriptor_ptr,
                                                   3);
        }

        valid = true;
        break;

      // Other desc type not supported.
      default:
        break;
    }
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_hid_class_request_handler()
 *
 * @brief    Process class-specific requests.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   true,   if NO error(s) occurred and request is supported.
 *
 *           false, if any errors are returned.
 *
 * @note     (1) HID defines the following class-specific requests :
 *
 *           - (a) GET_REPORT   allows the host to receive a report.
 *
 *           - (b) SET_REPORT   allows the host to send a report to the device, possibly setting
 *                               the state of input, output, or feature controls.
 *
 *           - (c) GET_IDLE     reads the current idle rate for a particular Input report.
 *
 *           - (d) SET_IDLE     silences a particular report on the Interrupt In pipe until a
 *                               new event occurs or the specified amount of time passes.
 *
 *           - (e) GET_PROTOCOL reads which protocol is currently active (either the boot
 *                               protocol or the report protocol).
 *
 *               - (1) protocol is either BOOT or REPORT.
 *
 *           - (f) SET_PROTOCOL switches between the boot protocol and the report protocol (or
 *                               vice versa).
 *
 *               See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about Class-Specific Requests.
 *******************************************************************************************************/
static bool usbd_hid_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                           void                      *p_if_class_arg)
{
  sli_usbd_hid_comm_t     *p_comm;
  sli_usbd_hid_ctrl_t     *p_ctrl;
  bool       valid;
  bool       dev_to_host;
  uint8_t        idle_rate;
  uint8_t        report_id;
  uint8_t        report_type;
  uint16_t        report_len;
  uint8_t        protocol;
  uint8_t        *p_buf;
  uint32_t        buf_len;
  uint16_t        req_len;
  uint8_t        w_value;
  bool       is_largest;
  sl_usbd_hid_callbacks_t *p_callback;
  uint32_t        xfer_len;
  sl_status_t         status;

  p_comm = (sli_usbd_hid_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;
  valid = false;
  p_callback = p_ctrl->callback_ptr;

  dev_to_host = SL_IS_BIT_SET(p_setup_req->bmRequestType, SL_USBD_REQ_DIR_BIT);

  // See Note #1.
  switch (p_setup_req->bRequest) {
    case SLI_USBD_HID_REQ_GET_REPORT:
      if (dev_to_host != true) {
        break;
      }

      report_type = (uint8_t)((p_setup_req->wValue >> 8u) & 0xFFu);
      report_id = (uint8_t)(p_setup_req->wValue        & 0xFFu);

      switch (report_type) {
        case 1:
          // Input report.
          status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                                SLI_USBD_HID_REPORT_TYPE_INPUT,
                                                report_id,
                                                &p_buf,
                                                &is_largest,
                                                &report_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          // Get min between req len & report size.
          buf_len = SLI_USBD_GET_MIN(p_setup_req->wLength, report_len);

          status = sli_usbd_hid_os_lock_input(p_ctrl->class_nbr);

          if (status != SL_STATUS_OK) {
            break;
          }

          // Send report to host.
          status = sl_usbd_core_write_control_sync((void *)p_buf,
                                                   buf_len,
                                                   SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS,
                                                   false,
                                                   &xfer_len);

          if (status == SL_STATUS_OK) {
            valid = true;
          }

          sli_usbd_hid_os_unlock_input(p_ctrl->class_nbr);
          break;

        case 3:
          // Feature report.
          if ((p_callback == (sl_usbd_hid_callbacks_t *)0)
              || (p_callback->get_feature_report == NULL)) {
            break;                                              // Stall request.
          }

          status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                                SLI_USBD_HID_REPORT_TYPE_FEATURE,
                                                report_id,
                                                &p_buf,
                                                &is_largest,
                                                &report_len);

          if (status != SL_STATUS_OK) {
            break;
          }

          // Get min between req len & report size.
          req_len = SLI_USBD_GET_MIN(p_setup_req->wLength, report_len);
          p_callback->get_feature_report(p_ctrl->class_nbr,
                                         report_id,
                                         p_buf,
                                         req_len);

          // Send report to host.
          status = sl_usbd_core_write_control_sync((void *)p_buf,
                                                   req_len,
                                                   SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS,
                                                   false,
                                                   &xfer_len);

          if (status == SL_STATUS_OK) {
            valid = true;
          }
          break;

        case 0:
        default:
          break;
      }
      break;

    case SLI_USBD_HID_REQ_SET_REPORT:
      if (dev_to_host != false) {
        break;
      }

      report_type = (uint8_t)((p_setup_req->wValue >> 8u) & 0xFFu);
      report_id = (uint8_t)(p_setup_req->wValue        & 0xFFu);

      switch (report_type) {
        case 2:
          // Output report.
          if ((p_callback == (sl_usbd_hid_callbacks_t *)0)
              || (p_callback->set_output_report == NULL)) {
            break;                                              // Stall request.
          }

          status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                                SLI_USBD_HID_REPORT_TYPE_OUTPUT,
                                                report_id,
                                                &p_buf,
                                                &is_largest,
                                                &report_len);

          if (status != SL_STATUS_OK) {
            break;
          }

          status = sli_usbd_hid_os_lock_output(p_ctrl->class_nbr);

          if (status != SL_STATUS_OK) {
            break;
          }

          if (p_setup_req->wLength > report_len) {
            sli_usbd_hid_os_unlock_output(p_ctrl->class_nbr);
            break;
          }

          // Receive report from host.
          status = sl_usbd_core_read_control_sync((void *)p_buf,
                                                  report_len,
                                                  SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS,
                                                  &xfer_len);

          if (status != SL_STATUS_OK) {
            sli_usbd_hid_os_unlock_output(p_ctrl->class_nbr);
            break;
          }

          sli_usbd_hid_os_unlock_output(p_ctrl->class_nbr);

          p_callback->set_output_report(p_ctrl->class_nbr,
                                        report_id,
                                        p_buf,
                                        report_len);

          valid = true;
          break;

        case 3:
          // Feature report.
          if ((p_callback == (sl_usbd_hid_callbacks_t *)0)
              || (p_callback->set_feature_report == NULL)) {
            break;                                              // Stall request.
          }

          status = sli_usbd_hid_report_get_info(&p_ctrl->report,
                                                SLI_USBD_HID_REPORT_TYPE_FEATURE,
                                                report_id,
                                                &p_buf,
                                                &is_largest,
                                                &report_len);

          if (status != SL_STATUS_OK) {
            break;
          }

          if (p_setup_req->wLength != report_len) {
            break;
          }

          // Receive report from host.
          status = sl_usbd_core_read_control_sync((void *)p_buf,
                                                  report_len,
                                                  0,
                                                  &xfer_len);

          if (status != SL_STATUS_OK) {
            break;
          }

          p_callback->set_feature_report(p_ctrl->class_nbr,
                                         report_id,
                                         p_buf,
                                         report_len);

          valid = true;

          break;

        case 0:
        default:
          break;
      }
      break;

    case SLI_USBD_HID_REQ_GET_IDLE:
      if (dev_to_host != true) {
        break;
      }

      w_value = (uint8_t)((p_setup_req->wValue >> 8u) & 0xFFu);

      // Chk if setup req is valid.
      if ((p_setup_req->wLength == 1)
          && (w_value == 0)) {
        report_id = (uint8_t)(p_setup_req->wValue & 0xFFu);

        // Get idle rate (duration).
        status = sli_usbd_hid_report_get_idle_rate(&p_ctrl->report,
                                                   report_id,
                                                   &p_ctrl->ctrl_status_buf_ptr[0]);

        if (status != SL_STATUS_OK) {
          break;
        }

        status = sl_usbd_core_write_control_sync((void *)&p_ctrl->ctrl_status_buf_ptr[0],
                                                 1u,
                                                 SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS,
                                                 false,
                                                 &xfer_len);

        if (status == SL_STATUS_OK) {
          valid = true;
        }
      }
      break;

    case SLI_USBD_HID_REQ_SET_IDLE:
      if (dev_to_host != false) {
        break;
      }

      // Chk if setup req is valid.
      if (p_setup_req->wLength > 0) {
        break;
      }

      idle_rate = (uint8_t)((p_setup_req->wValue >> 8u) & 0xFFu);
      report_id = (uint8_t)(p_setup_req->wValue        & 0xFFu);

      status = sli_usbd_hid_report_set_idle_rate(&p_ctrl->report,
                                                 report_id,
                                                 idle_rate);

      if (status == SL_STATUS_OK) {
        valid = true;
      }
      break;

    case SLI_USBD_HID_REQ_GET_PROTOCOL:
      if ((p_callback == (sl_usbd_hid_callbacks_t *)0)
          || (p_callback->get_protocol == NULL)) {
        break;                                                  // Stall request.
      }

      if (dev_to_host != true) {
        break;
      }

      // Chk if setup req is valid.
      if ((p_setup_req->wLength != 1)
          || (p_setup_req->wValue != 0)) {
        break;
      }

      //                                                           Get currently active protocol from app.
      p_callback->get_protocol(p_ctrl->class_nbr, &p_ctrl->ctrl_status_buf_ptr[0]);

      // Send active protocol to host (see Note #1e1).
      status = sl_usbd_core_write_control_sync((void *)&p_ctrl->ctrl_status_buf_ptr[0],
                                               1u,
                                               SLI_USBD_HID_CTRL_REQ_TIMEOUT_mS,
                                               false,
                                               &xfer_len);

      if (status == SL_STATUS_OK) {
        valid = true;
      }
      break;

    case SLI_USBD_HID_REQ_SET_PROTOCOL:
      if ((p_callback == (sl_usbd_hid_callbacks_t *)0)
          || (p_callback->set_protocol == NULL)) {
        break;                                                  // Stall request.
      }

      if (dev_to_host != false) {
        break;
      }

      // Chk if setup req is valid.
      if ((p_setup_req->wLength != 0)
          || ((p_setup_req->wValue != 0)
              && (p_setup_req->wValue != 1))) {
        break;
      }

      protocol = (uint8_t)p_setup_req->wValue;

      //                                                           Set new protocol.
      p_callback->set_protocol(p_ctrl->class_nbr, protocol);

      valid = true;
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_hid_output_data_complete()
 *
 * @brief    Inform the class about the set report transfer completion on control endpoint.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    status      Transfer status.
 *******************************************************************************************************/
static void usbd_hid_output_data_complete(uint8_t      class_nbr,
                                          void         *p_buf,
                                          uint32_t     buf_len,
                                          uint32_t     xfer_len,
                                          void         *p_arg,
                                          sl_status_t  status)
{
  uint32_t *p_xfer_len;

  (void)&class_nbr;
  (void)&p_buf;
  (void)&buf_len;

  p_xfer_len = (uint32_t *)p_arg;

  if (status == SL_STATUS_OK) {
    *p_xfer_len = xfer_len;
    sli_usbd_hid_os_post_output(class_nbr);
  } else {
    *p_xfer_len = 0;
    sli_usbd_hid_os_pend_abort_output(class_nbr);
  }
}

/****************************************************************************************************//**
 *                                           usbd_hid_read_async_complete()
 *
 * @brief    Inform the application about the Bulk OUT transfer completion.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length.
 *
 * @param    xfer_len    Number of octets received.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    status      Transfer status: success or error.
 *******************************************************************************************************/
static void usbd_hid_read_async_complete(uint8_t      ep_addr,
                                         void         *p_buf,
                                         uint32_t     buf_len,
                                         uint32_t     xfer_len,
                                         void         *p_arg,
                                         sl_status_t  status)
{
  sli_usbd_hid_comm_t       *p_comm;
  sli_usbd_hid_ctrl_t       *p_ctrl;
  sl_usbd_hid_async_function_t fnct;
  void                *p_fnct_arg;

  (void)&ep_addr;

  p_comm = (sli_usbd_hid_comm_t *)p_arg;
  p_ctrl = p_comm->ctrl_ptr;
  fnct = p_ctrl->interrupt_read_async_function;
  p_fnct_arg = p_ctrl->interrupt_read_async_arg_ptr;

  // Xfer finished, no more active xfer.
  p_comm->data_interrupt_out_active_transfer = false;

  // Notify app about xfer completion.
  fnct(p_ctrl->class_nbr,
       p_buf,
       buf_len,
       xfer_len,
       p_fnct_arg,
       status);
}

/****************************************************************************************************//**
 *                                           usbd_hid_write_async_complete()
 *
 * @brief    Inform the application about the Bulk IN transfer completion.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length.
 *
 * @param    xfer_len    Number of octets sent.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    status      Transfer status: success or error.
 *******************************************************************************************************/
static void usbd_hid_write_async_complete(uint8_t      ep_addr,
                                          void         *p_buf,
                                          uint32_t     buf_len,
                                          uint32_t     xfer_len,
                                          void         *p_arg,
                                          sl_status_t  status)
{
  sli_usbd_hid_comm_t *p_comm;
  sli_usbd_hid_ctrl_t *p_ctrl;

  (void)&ep_addr;

  p_comm = (sli_usbd_hid_comm_t *)p_arg;
  p_ctrl = (sli_usbd_hid_ctrl_t *)p_comm->ctrl_ptr;

  sli_usbd_hid_os_unlock_tx(p_ctrl->class_nbr);

  // Notify app about xfer completion.
  p_ctrl->interrupt_write_async_function(p_ctrl->class_nbr,
                                         p_buf,
                                         buf_len,
                                         xfer_len,
                                         p_ctrl->interrupt_write_async_arg_ptr,
                                         status);
}

/****************************************************************************************************//**
 *                                           usbd_hid_write_sync_complete()
 *
 * @brief    Inform the class about the Bulk IN transfer completion.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length.
 *
 * @param    xfer_len    Number of octets sent.
 *
 * @param    p_arg       Additional argument provided by application.
 *
 * @param    status      Transfer status: success or error.
 *******************************************************************************************************/
static void usbd_hid_write_sync_complete(uint8_t      ep_addr,
                                         void         *p_buf,
                                         uint32_t     buf_len,
                                         uint32_t     xfer_len,
                                         void         *p_arg,
                                         sl_status_t  status)
{
  sli_usbd_hid_ctrl_t *p_ctrl;
  sli_usbd_hid_comm_t *p_comm;
  uint8_t    class_nbr;

  (void)&ep_addr;
  (void)&p_buf;
  (void)&buf_len;

  p_comm = (sli_usbd_hid_comm_t *)p_arg;
  p_ctrl = p_comm->ctrl_ptr;
  class_nbr = p_ctrl->class_nbr;

  if (status == SL_STATUS_OK) {
    p_ctrl->data_interrupt_in_transfer_len = xfer_len;
    sli_usbd_hid_os_post_input(class_nbr);
  } else {
    p_ctrl->data_interrupt_in_transfer_len = 0;
    sli_usbd_hid_os_pend_abort_input(class_nbr);
  }
}
