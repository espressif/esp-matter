/***************************************************************************//**
 * @file
 * @brief USB Device Msc Class
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_MSC_MODULE

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_core.h"

#include "sl_bit.h"
#include "sl_enum.h"
#include "sl_status.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_msc.h"
#include "sli_usbd_class_msc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_MSC_SIG_CBW                          0x43425355
#define  SLI_USBD_MSC_SIG_CSW                          0x53425355

#define  SLI_USBD_MSC_DEV_STR_LEN                      12u

#define  SLI_USBD_MSC_CTRL_REQ_TIMEOUT_mS              5000u

#define  SLI_USBD_MSC_COM_NBR_MAX                      (USBD_MSC_CFG_MAX_NBR_DEV * USBD_MSC_CFG_MAX_NBR_CFG)

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUESTS
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 3.
 *
 *           (2) The 'bRequest' field of a class-specific setup request may contain one of these values.
 *
 *           (3) The mass storage reset request is "used to reset the mass storage device and its
 *               associated interface".  The setup request packet will consist of :
 *
 *               (a) bmRequestType = 00100001b (class, interface, host-to-device)
 *               (b) bRequest      =     0xFF
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0000
 *
 *           (4) The get max lun is used to determine the number of lun's supported by the device.  The
 *               setup request packet will consist of :
 *
 *               (a) bmRequestType = 10100001b (class, interface, device-to-host)
 *               (b) bRequest      =     0xFE
 *               (c) wValue        =   0x0000
 *               (d) wIndex        = Interface number
 *               (e) wLength       =   0x0001
 *******************************************************************************************************/

#define  SLI_USBD_MSC_REQ_MASS_STORAGE_RESET                 0xFF   // See Notes #3.
#define  SLI_USBD_MSC_REQ_GET_MAX_LUN                        0xFE   // See Notes #4.

/********************************************************************************************************
 *                                       COMMAND BLOCK FLAG VALUES
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1.
 *
 *           (2) The 'flags' field of a command block wrapper may contain one of these values.
 *******************************************************************************************************/

#define  SLI_USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE          0x00
#define  SLI_USBD_MSC_BMCBWFLAGS_DIR_DEVICE_TO_HOST          0x80

/********************************************************************************************************
 *                                       COMMAND BLOCK STATUS VALUES
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.3, Table 5.3.
 *
 *           (2) The 'status' field of a command status wrapper may contain one of these values.
 *******************************************************************************************************/

#define  SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED                  0x00
#define  SLI_USBD_MSC_BCSWSTATUS_CMD_FAILED                  0x01
#define  SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR                 0x02

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

sli_usbd_msc_t  usbd_msc_obj = { 0 };
sli_usbd_msc_t *usbd_msc_ptr = NULL;

/********************************************************************************************************
 *********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_msc_enable(uint8_t config_nbr,
                            void    *p_if_class_arg);

static void usbd_msc_disable(uint8_t config_nbr,
                             void    *p_if_class_arg);

static bool usbd_msc_request(const sl_usbd_setup_req_t *p_setup_req,
                             void                      *p_if_class_arg);

static void usbd_msc_read_cbw(sli_usbd_msc_ctrl_t *p_ctrl,
                              sli_usbd_msc_comm_t *p_comm);

static bool usbd_msc_verify_cbw(const sli_usbd_msc_ctrl_t *p_ctrl,
                                sli_usbd_msc_comm_t       *p_comm,
                                void                      *p_cbw_buf,
                                uint32_t                  cbw_len);

static void usbd_msc_parse_cbw(sli_usbd_msc_cbw_t *p_cbw,
                               void               *p_buf_src);

static void usbd_msc_write_csw(sli_usbd_msc_ctrl_t *p_ctrl,
                               sli_usbd_msc_comm_t *p_comm);

static void usbd_msc_format_csw(const sli_usbd_msc_csw_t *p_csw,
                                void                     *p_buf_dest);

static void usbd_msc_process_command(sli_usbd_msc_ctrl_t *p_ctrl,
                                     sli_usbd_msc_comm_t *p_comm);

static bool usbd_msc_verify_response(sli_usbd_msc_comm_t *p_comm,
                                     uint32_t            data_len,
                                     uint8_t             data_dir);

static void usbd_msc_read_from_host(sli_usbd_msc_ctrl_t *p_ctrl,
                                    sli_usbd_msc_comm_t *p_comm);

static void usbd_msc_write_to_host(sli_usbd_msc_ctrl_t *p_ctrl,
                                   sli_usbd_msc_comm_t *p_comm);

static void usbd_msc_read_from_device(sli_usbd_msc_ctrl_t *p_ctrl,
                                      sli_usbd_msc_comm_t *p_comm);

static void usbd_msc_write_to_device(const sli_usbd_msc_ctrl_t *p_ctrl,
                                     sli_usbd_msc_comm_t       *p_comm,
                                     void                      *p_buf,
                                     uint32_t                  xfer_len);

static void usbd_msc_update_endpoint_state(uint8_t config_nbr,
                                           uint8_t if_nbr,
                                           uint8_t if_alt_nbr,
                                           uint8_t ep_addr,
                                           void    *p_if_class_arg,
                                           void    *p_if_alt_class_arg);

/********************************************************************************************************
 *                                               MSC CLASS DRIVER
 *******************************************************************************************************/

sl_usbd_class_driver_t usbd_msc_driver = {
  usbd_msc_enable,
  usbd_msc_disable,
  NULL,                                                     // MSC does NOT use alternate IF(s).
  usbd_msc_update_endpoint_state,
  NULL,                                                     // MSC does NOT use IF functional desc.
  NULL,
  NULL,                                                     // MSC does NOT use EP functional desc.
  NULL,
  NULL,                                                     // MSC does NOT handle std req with IF recipient.
  usbd_msc_request,
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
 * Initializes internal structures and variables used by the MSC Class
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_init(void)
{
  uint8_t    ix;
  sli_usbd_msc_ctrl_t *p_ctrl;
  sli_usbd_msc_comm_t *p_comm;
  sl_status_t   status;
  CORE_DECLARE_IRQ_STATE;

  usbd_msc_obj.next_ctrl_nbr      = SL_USBD_MSC_CLASS_INSTANCE_QUANTITY;
  usbd_msc_obj.next_comm_nbr      = SL_USBD_MSC_CLASS_INSTANCE_QUANTITY * SL_USBD_MSC_CONFIGURATION_QUANTITY;
  usbd_msc_obj.class_instance_qty = SL_USBD_MSC_CLASS_INSTANCE_QUANTITY;
  usbd_msc_obj.lun_qty            = SL_USBD_MSC_LUN_QUANTITY;

  // init MSC OS layer.
  status = sli_usbd_msc_os_init();

  if (status != SL_STATUS_OK) {
    return status;
  }

  CORE_ENTER_ATOMIC();
  usbd_msc_ptr = &usbd_msc_obj;
  CORE_EXIT_ATOMIC();

  // init MSC class struct.
  for (ix = 0u; ix < SL_USBD_MSC_CLASS_INSTANCE_QUANTITY; ix++) {
    p_ctrl = &usbd_msc_ptr->ctrl_table[ix];

    p_ctrl->state = SLI_USBD_MSC_STATE_NONE;
    p_ctrl->comm_ptr = NULL;
    p_ctrl->max_lun = 0u;

    memset((void *)p_ctrl->cbw_buffer, 0, SLI_USBD_MSC_LEN_CBW);
    memset((void *)p_ctrl->csw_buffer, 0, SLI_USBD_MSC_LEN_CSW);
    memset((void *)p_ctrl->data_buffer, 0, SL_USBD_MSC_DATA_BUFFER_SIZE);

    p_comm = &usbd_msc_ptr->comm_table[ix];

    p_comm->data_Bulk_in_ep_addr = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->data_Bulk_out_ep_addr = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->ctrl_ptr = NULL;

    p_comm->cbw.signature = 0u;
    p_comm->cbw.tag = 0u;
    p_comm->cbw.data_transfer_length = 0u;
    p_comm->cbw.flags = 0u;
    p_comm->cbw.lun = 0u;
    p_comm->cbw.cbw_length = 0u;

    memset((void *)p_comm->cbw.cb, 0, sizeof(p_comm->cbw.cb));

    p_comm->csw.signature = 0u;
    p_comm->csw.tag = 0u;
    p_comm->csw.data_residue = 0u;
    p_comm->csw.status = 0u;

    p_comm->stall = false;
    p_comm->bytes_to_xfer = 0u;
    p_comm->wr_buffer_ptr = NULL;
    p_comm->wr_buffer_length = 0u;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a new instance of the Mass Storage Class
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_create_instance(uint8_t                       subclass,
                                        uint32_t                      msc_task_stack_size,
                                        osPriority_t                  msc_task_priority,
                                        sl_usbd_msc_subclass_driver_t *p_subclass_drv,
                                        void                          *p_subclass_arg,
                                        uint8_t                       *p_class_nbr)
{
  uint8_t    msc_nbr;
  sli_usbd_msc_ctrl_t *p_ctrl;
  sl_status_t   status;
  CORE_DECLARE_IRQ_STATE;

  if (p_subclass_drv == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_subclass_drv->enable == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_subclass_drv->disable == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_subclass_drv->process_command == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_subclass_drv->read_data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_subclass_drv->write_data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_class_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (msc_task_stack_size == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_msc_ptr->next_ctrl_nbr == 0u) {
    CORE_EXIT_ATOMIC();
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_msc_ptr->next_ctrl_nbr--;
  // Alloc new MSC class.
  msc_nbr = usbd_msc_ptr->next_ctrl_nbr;
  CORE_EXIT_ATOMIC();

  status = sli_usbd_msc_os_create_task(msc_nbr,
                                       msc_task_stack_size,
                                       msc_task_priority);
  if (status != SL_STATUS_OK) {
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return status;
  }

  p_ctrl = &usbd_msc_ptr->ctrl_table[msc_nbr];

  p_ctrl->class_nbr         = msc_nbr;
  p_ctrl->subclass_code     = subclass;
  // only MSC BBB protocol is supported
  p_ctrl->subclass_protocol = SL_USBD_MSC_PROTOCOL_CODE_BULK_ONLY;
  p_ctrl->subclass_driver   = p_subclass_drv;
  p_ctrl->subclass_arg      = p_subclass_arg;

  *p_class_nbr = msc_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds an existing MSC instance to the specified configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr)
{
  sli_usbd_msc_ctrl_t  *p_ctrl;
  sli_usbd_msc_comm_t  *p_comm;
  sl_usbd_device_config_t   *p_dev_cfg;
  const char *p_str;
  uint16_t     str_len;
  uint8_t     if_nbr;
  uint8_t     ep_addr;
  uint16_t     comm_nbr;
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  if (class_nbr >= usbd_msc_ptr->class_instance_qty) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sl_usbd_core_get_device_configuration(&p_dev_cfg);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_dev_cfg->serial_nbr_str_ptr != NULL) {
    p_str = &p_dev_cfg->serial_nbr_str_ptr[0];
    str_len = 0u;
    while ((*p_str != (char)'\0')
           && (p_str != (const char *)  0)) {
      if ((SLI_USBD_ASCII_IS_DIG_HEX(*p_str) == false)
          // Serial nbr must be a hex string.
          || ((SLI_USBD_ASCII_IS_ALPHA(*p_str) == true)
              // Make sure that if A-F values are present they are lower-case.
              && (SLI_USBD_ASCII_IS_LOWER(*p_str) == true))) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      p_str++;
      str_len++;
    }

    if (str_len < SLI_USBD_MSC_DEV_STR_LEN) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_msc_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();

  if (usbd_msc_ptr->next_comm_nbr == 0u) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_msc_ptr->next_comm_nbr--;
  comm_nbr = usbd_msc_ptr->next_comm_nbr;
  CORE_EXIT_ATOMIC();

  p_comm = &usbd_msc_ptr->comm_table[comm_nbr];

  status = sl_usbd_core_add_interface(config_nbr,
                                      &usbd_msc_driver,
                                      (void *)p_comm,
                                      NULL,
                                      SL_USBD_CLASS_CODE_MASS_STORAGE,
                                      p_ctrl->subclass_code,
                                      p_ctrl->subclass_protocol,
                                      "USB Mass Storage Interface",
                                      &if_nbr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Add bulk-IN EP desc.
  status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                          if_nbr,
                                          0u,
                                          true,
                                          0u,
                                          &ep_addr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Store bulk-IN EP address.
  p_comm->data_Bulk_in_ep_addr = ep_addr;

  // Add bulk-OUT EP desc.
  status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                          if_nbr,
                                          0u,
                                          false,
                                          0u,
                                          &ep_addr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Store bulk-OUT EP address.
  p_comm->data_Bulk_out_ep_addr = ep_addr;

  CORE_ENTER_ATOMIC();
  // Set class instance to init state.
  p_ctrl->state = SLI_USBD_MSC_STATE_INIT;
  p_ctrl->comm_ptr = NULL;
  CORE_EXIT_ATOMIC();

  p_comm->ctrl_ptr = p_ctrl;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds an MSC logical unit to the MSC interface
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_lun_add(uint8_t         class_nbr,
                                void            *p_lu_data,
                                uint8_t         *p_lu_nbr)
{
  uint8_t    max_lun;
  sli_usbd_msc_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  if (p_lu_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= usbd_msc_ptr->class_instance_qty) {
    *p_lu_nbr = SL_USBD_MSC_LU_NBR_INVALID;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_msc_ptr->ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  max_lun = p_ctrl->max_lun;

  if (max_lun >= usbd_msc_ptr->lun_qty) {
    CORE_EXIT_ATOMIC();

    *p_lu_nbr = SL_USBD_MSC_LU_NBR_INVALID;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  p_ctrl->max_lun++;
  CORE_EXIT_ATOMIC();

  p_ctrl->lun_data_ptr_table[max_lun] = p_lu_data;

  *p_lu_nbr = max_lun;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the MSC enable state of the device
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_is_enabled(uint8_t  class_nbr,
                                   bool     *p_enabled)
{
  sli_usbd_msc_ctrl_t          *p_ctrl;
  sl_usbd_device_state_t state;
  sl_status_t            status;

  if (p_enabled == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= usbd_msc_ptr->class_instance_qty) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_msc_ptr->ctrl_table[class_nbr];

  if (p_ctrl->comm_ptr == NULL) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev state.
  status = sl_usbd_core_get_device_state(&state);

  if (status != SL_STATUS_OK) {
    *p_enabled = false;
    return SL_STATUS_INVALID_STATE;
  }

  // Return true if dev state config & MSC state config.
  if ((state == SL_USBD_DEVICE_STATE_CONFIGURED) && (p_ctrl->state == SLI_USBD_MSC_STATE_CONFIG)) {
    *p_enabled = true;
  } else {
    *p_enabled = false;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * This function is used to handle MSC transfers
 *******************************************************************************************************/
void sli_usbd_msc_task_handler(uint8_t class_nbr)
{
  bool   conn;
  sli_usbd_msc_ctrl_t *p_ctrl = &usbd_msc_ptr->ctrl_table[class_nbr];
  sli_usbd_msc_comm_t *p_comm;

  while (true) {
    sl_usbd_msc_is_enabled(class_nbr, &conn);
    if (conn != true) {
      // Wait till MSC state and dev state is connected.
      sli_usbd_msc_os_pend_connect_signal(class_nbr, 0u);
    }

    p_comm = p_ctrl->comm_ptr;
    if (p_comm == NULL) {
      continue;
    }

    switch (p_comm->next_comm_state) {
      case SLI_USBD_MSC_COMM_STATE_CBW:
        // Receive cbw state
        usbd_msc_read_cbw(p_ctrl,
                          p_comm);
        break;

      case SLI_USBD_MSC_COMM_STATE_DATA:
        // Data Transport State
        usbd_msc_process_command(p_ctrl,
                                 p_comm);
        break;

      case SLI_USBD_MSC_COMM_STATE_CSW:
        // Transmit csw State
        usbd_msc_write_csw(p_ctrl,
                           p_comm);
        break;

      case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY:
      case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:
      case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:
        // Wait on sem for Reset Recovery to complete.
        sli_usbd_msc_os_pend_comm_signal(class_nbr, 0u);
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CBW;

        break;

      case SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL:
        // Wait on sem for clear bulk-IN stall to complete.
        sli_usbd_msc_os_pend_comm_signal(class_nbr, 0u);
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
        break;

      case SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL:
        // Wait on sem for clear bulk-OUT stall to complete.
        sli_usbd_msc_os_pend_comm_signal(class_nbr, 0u);
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
        break;

      case SLI_USBD_MSC_COMM_STATE_NONE:
      default:
        break;
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                              usbd_msc_enable()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    config_nbr      Configuration index to add the interface to.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void usbd_msc_enable(uint8_t config_nbr,
                            void    *p_if_class_arg)
{
  sli_usbd_msc_comm_t *p_comm;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;
  p_comm = (sli_usbd_msc_comm_t *)p_if_class_arg;

  CORE_ENTER_ATOMIC();
  p_comm->ctrl_ptr->comm_ptr = p_comm;
  // Set initial MSC state to config.
  p_comm->ctrl_ptr->state = SLI_USBD_MSC_STATE_CONFIG;
  // Set initial MSC comm state to rx cbw.
  p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CBW;
  CORE_EXIT_ATOMIC();

  p_comm->ctrl_ptr->subclass_driver->enable(p_comm->ctrl_ptr->class_nbr,
                                            p_comm->ctrl_ptr->subclass_arg);

  sli_usbd_msc_os_post_connect_signal(p_comm->ctrl_ptr->class_nbr);

  SLI_USBD_LOG_VRB(("USBD MSC: Conn"));
}

/****************************************************************************************************//**
 *                                           usbd_msc_disable()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    config_nbr      Configuration index to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void usbd_msc_disable(uint8_t config_nbr,
                             void    *p_if_class_arg)
{
  sli_usbd_msc_comm_t       *p_comm;
  sli_usbd_msc_comm_state_t comm_state;
  bool                post_signal;
  uint8_t             class_nbr;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;
  p_comm = (sli_usbd_msc_comm_t *)p_if_class_arg;
  class_nbr = p_comm->ctrl_ptr->class_nbr;

  CORE_ENTER_ATOMIC();
  comm_state = p_comm->next_comm_state;
  switch (comm_state) {
    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY:
    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:
    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:
    case SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL:
    case SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL:
      post_signal = true;
      break;

    case SLI_USBD_MSC_COMM_STATE_NONE:
    case SLI_USBD_MSC_COMM_STATE_CBW:
    case SLI_USBD_MSC_COMM_STATE_DATA:
    case SLI_USBD_MSC_COMM_STATE_CSW:
    default:
      post_signal = false;
      break;
  }
  p_comm->ctrl_ptr->comm_ptr = NULL;
  // Set MSC state to init.
  p_comm->ctrl_ptr->state = SLI_USBD_MSC_STATE_INIT;
  // Set MSC comm state to none.
  p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_NONE;
  CORE_EXIT_ATOMIC();

  if (post_signal == true) {
    sli_usbd_msc_os_post_comm_signal(class_nbr);                // Post sem to notify waiting task if comm
  }                                                             // is in reset recovery and bulk-IN or bulk-OUT
                                                                // stall states.

  p_comm->ctrl_ptr->subclass_driver->disable(p_comm->ctrl_ptr->class_nbr,
                                             p_comm->ctrl_ptr->subclass_arg);

  SLI_USBD_LOG_VRB(("USBD MSC: Disconn"));
}

/****************************************************************************************************//**
 *                                           usbd_msc_update_endpoint_state()
 *
 * @brief    Notify class that endpoint state has been updated.
 *
 * @param    config_nbr          Configuration ix to add the interface.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    ep_addr             Endpoint address.
 *
 * @param    p_if_class_arg      Pointer to class argument specific to interface.
 *
 * @param    p_if_alt_class_arg  Pointer to class argument specific to alternate interface.
 *
 * @note     (1) For Reset Recovery, the host shall issue:
 *
 *           - (a) a Bulk-Only Mass Storage Reset
 *           - (b) a Clear Feature HALT to the Bulk-In endpoint or Bulk-Out endpoint.
 *           - (c) a Clear Feature HALT to the complement Bulk-Out endpoint or Bulk-In endpoint.
 *******************************************************************************************************/
static void usbd_msc_update_endpoint_state(uint8_t config_nbr,
                                           uint8_t if_nbr,
                                           uint8_t if_alt_nbr,
                                           uint8_t ep_addr,
                                           void    *p_if_class_arg,
                                           void    *p_if_alt_class_arg)
{
  sli_usbd_msc_comm_t *p_comm = (sli_usbd_msc_comm_t *)p_if_class_arg;
  bool          ep_is_stall;
  bool          ep_in_is_stall;
  bool          ep_out_is_stall;
  uint8_t       class_nbr = p_comm->ctrl_ptr->class_nbr;
  sl_status_t   status;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  switch (p_comm->next_comm_state) {
    case SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL:
      // Verify correct EP addr.
      if (ep_addr == p_comm->data_Bulk_in_ep_addr) {
        sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
        // Verify that EP is unstalled.
        if (ep_is_stall == false) {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Bulk IN stall, Signal"));
          // Post sem to notify waiting task.
          sli_usbd_msc_os_post_comm_signal(class_nbr);
        } else {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEp Bulk In stall, EP not stalled"));
        }
      } else {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Bulk IN stall, invalid endpoint"));
      }
      break;

    case SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL:
      // Verify correct EP addr.
      if (ep_addr == p_comm->data_Bulk_out_ep_addr) {
        sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
        // Verify that EP is unstalled.
        if (ep_is_stall == false) {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Bulk OUT stall, Signal"));
          // Post sem to notify waiting task.
          sli_usbd_msc_os_post_comm_signal(class_nbr);
        } else {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Bulk OUT stall, EP not stalled"));
        }
      } else {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Bulk OUT stall, invalid endpoint"));
      }
      break;

    // RX cbw / TX csw State
    case SLI_USBD_MSC_COMM_STATE_CBW:
    case SLI_USBD_MSC_COMM_STATE_CSW:
    case SLI_USBD_MSC_COMM_STATE_DATA:
      SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Rx cbw / Process Data / Tx csw, skip"));
      break;

    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL:
      // Verify correct EP addr.
      if (ep_addr == p_comm->data_Bulk_in_ep_addr) {
        sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
        // Verify that EP is unstalled.
        if (ep_is_stall == false) {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP In Cleared, stall again"));
          sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
        } else {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP In Stalled"));
        }
      } else {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, invalid endpoint"));
      }

      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL;
      break;

    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL:
      // Verify correct EP addr.
      if (ep_addr == p_comm->data_Bulk_out_ep_addr) {
        sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
        // Verify that EP is unstalled.
        if (ep_is_stall == false) {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP OUT Cleared, stall again"));
          sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
        } else {
          SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, EP OUT Stalled"));
        }
      } else {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, invalid endpoint"));
      }

      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY;
      break;

    case SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY:
      status = sl_usbd_core_is_endpoint_stalled(p_comm->data_Bulk_in_ep_addr, &ep_in_is_stall);
      // Check stall condition of bulk-IN EP.
      if (status != SL_STATUS_OK) {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, IsStalled failed"));
      }

      ep_out_is_stall = sl_usbd_core_is_endpoint_stalled(p_comm->data_Bulk_out_ep_addr, &ep_in_is_stall);
      // Check stall condition of bulk-OUT EP.
      if (status != SL_STATUS_OK) {
        SLI_USBD_LOG_ERR(("USBD MSC: UpdateEP Reset Recovery, IsStalled failed"));
      }

      if ((ep_in_is_stall == false) && (ep_out_is_stall == false)) {
        SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Reset Recovery, Signal"));
        sli_usbd_msc_os_post_comm_signal(class_nbr);
      } else {
        SLI_USBD_LOG_DBG(("USBD MSC: UpdateEP Reset Recovery, MSC Reset, Clear Stalled"));
      }
      break;

    case SLI_USBD_MSC_COMM_STATE_NONE:
    default:
      SLI_USBD_LOG_VRB(("USBD MSC: UpdateEP Invalid state, stall IN/OUT"));
      // Clr stall unexpected here.
      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, false);
      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, false);
      break;
  }
}

/****************************************************************************************************//**
 *                                           usbd_msc_request()
 *
 * @brief    Process class-specific request.
 *
 * @param    p_setup_req     Pointer to SETUP request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument provided when calling USBD_IF_Add().
 *
 * @return   true, if class request was successful
 *
 *           false,  if the class request was not successful.
 *
 * @note     (1) The Mass Storage Reset class request is used to reset the device and its associated
 *               interface. This request readies the device for the next cbw from the host. The host
 *               sends this request via the control endpoint to the device. The device shall preserve
 *               the value of its bulk data toggle bits and endpoint stall conditions despite the
 *               Bulk-Only Mass Storage Reset. The device shall NAK the status stage of the device
 *               request until the Bulk-Only Mass Storage Reset is complete.
 *
 * @note     (2) The Get Max lun class request is used to determine the number of logical units supported
 *               by the device. The device shall return one byte of data that contains the maximum lun
 *               supported by the device.
 *******************************************************************************************************/
static bool usbd_msc_request(const sl_usbd_setup_req_t *p_setup_req,
                             void                      *p_if_class_arg)
{
  uint8_t       request;
  bool          valid;
  sli_usbd_msc_ctrl_t *p_ctrl;
  sli_usbd_msc_comm_t *p_comm;
  uint32_t      xfer_len;
  sl_status_t   status;

  request = p_setup_req->bRequest;
  p_comm = (sli_usbd_msc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;
  valid = false;

  switch (request) {
    case SLI_USBD_MSC_REQ_MASS_STORAGE_RESET:
      if ((p_setup_req->wValue == 0u)
          && (p_setup_req->wLength == 0u)) {
        SLI_USBD_LOG_VRB(("USBD MSC: Class Mass Storage Reset, stall IN/OUT"));
        status = sl_usbd_core_abort_endpoint(p_comm->data_Bulk_in_ep_addr);
        if (status != SL_STATUS_OK) {
          SLI_USBD_LOG_ERR(("USBD MSC: Class Mass Storage Reset, EP IN Abort failed"));
        }

        status = sl_usbd_core_abort_endpoint(p_comm->data_Bulk_out_ep_addr);
        if (status != SL_STATUS_OK) {
          SLI_USBD_LOG_ERR(("USBD MSC: Class Mass Storage Reset, EP OUT Abort failed"));
        }

        if (status == SL_STATUS_OK) {
          valid = true;
        }
      }
      break;

    case SLI_USBD_MSC_REQ_GET_MAX_LUN:
      if ((p_setup_req->wValue == 0u)
          && (p_setup_req->wLength == 1u)) {
        SLI_USBD_LOG_VRB(("USBD MSC: Class Get Max Lun"));
        if (p_comm->ctrl_ptr->max_lun > 0u) {
          // Store max lun info.
          p_ctrl->ctrl_status_buffer = p_comm->ctrl_ptr->max_lun - 1;
          // Tx max lun info through ctrl EP.
          status = sl_usbd_core_write_control_sync((void *)&p_ctrl->ctrl_status_buffer,
                                                   1u,
                                                   SLI_USBD_MSC_CTRL_REQ_TIMEOUT_mS,
                                                   false,
                                                   &xfer_len);
          if ((status == SL_STATUS_OK) && (p_comm->ctrl_ptr->state == SLI_USBD_MSC_STATE_CONFIG)) {
            valid = true;
          }
        }
      }
      break;

    default:
      break;
  }

  return (valid);
}

/****************************************************************************************************//***
 *                                           usbd_msc_verify_cbw()
 *
 * @brief    Parses and verify the cbw sent by the Host.
 *
 * @param    p_ctrl      Pointer to MSC instance control structure.
 *
 * @param    p_comm      Pointer to MSC comm structure.
 *
 * @param    p_cbw_buf   Pointer to the raw cbw buffer.
 *
 * @param    cbw_len     Length of the raw cbw buffer.
 *
 * @return   true,     if cbw is valid.
 *           false,   if cbw is invalid.
 *
 * @note     (1) The device performs two verifications on every cbw received. First is that the cbw
 *               is valid. Second is that the cbw is meaningful.
 *
 *           - (a) The device shall consider a cbw valid when:
 *               - (1) The cbw was received after the device had sent a csw or after a reset.
 *               - (2) The cbw is 31 (1Fh) bytes in length.
 *               - (3) The signature is equal to 43425355h.
 *
 *           - (b) The device shall consider a cbw meaningful when:
 *               - (1) No reserve bits are set.
 *               - (2) lun contains a valid lun supported by the device.
 *               - (3) both cCBWCBLength and the content of the cb are in accordance with
 *                       bInterfaceSubClass
 *******************************************************************************************************/
static bool usbd_msc_verify_cbw(const sli_usbd_msc_ctrl_t *p_ctrl,
                                sli_usbd_msc_comm_t       *p_comm,
                                void                      *p_cbw_buf,
                                uint32_t                  cbw_len)
{
  if (cbw_len != SLI_USBD_MSC_LEN_CBW) {
    // See note #1a2.
    SLI_USBD_LOG_ERR(("USBD MSC: Verify cbw, invalid length"));
    return (false);
  }

  // Parse the raw buffer into cbw struct.
  usbd_msc_parse_cbw(&p_comm->cbw, p_cbw_buf);

  if (p_comm->cbw.signature != SLI_USBD_MSC_SIG_CBW) {
    // See note #1a3.
    SLI_USBD_LOG_ERR(("USBD MSC: Verify cbw, invalid signature"));
    return (false);
  }

  if (((p_comm->cbw.lun    & 0xF0u) > 0u)
      || ((p_comm->cbw.cbw_length & 0xE0u) > 0u)
      || (p_comm->cbw.lun >= p_ctrl->max_lun)) {
    // See note #1b.
    SLI_USBD_LOG_ERR(("USBD MSC: Verify cbw, invalid lun/reserved bits"));
    return (false);
  }

  return (true);
}

/****************************************************************************************************//***
 *                                           usbd_msc_verify_response()
 *
 * @brief    Check the data transfer conditions of host and device and set the csw status field.
 *
 * @param    p_comm      Pointer to MSC comm structure.
 *
 * @param    data_len    The length of the response the device intends to transfer.
 *
 * @param    data_dir    The data transfer direction.
 *
 * @return   true,     cbw satisfies one of the thirteen cases.
 *           false,   Mismatch between direction indicated by cbw and device command.
 *
 * @note     (1) "USB Mass Storage Class - Bulk Only Transport", Revision 1.0, Section 6.7, lists
 *               the thirteen cases of host expectation & device intent with descriptions of the
 *               appropriate action.
 *******************************************************************************************************/
static bool usbd_msc_verify_response(sli_usbd_msc_comm_t *p_comm,
                                     uint32_t            data_len,
                                     uint8_t             data_dir)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_comm->stall = false;
  // Check for host and device dirs.
  if ((data_len) && (p_comm->cbw.flags != data_dir)) {
    // Case  8: Hi <> Do || 10: Hn <> Di.
    p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    CORE_EXIT_ATOMIC();

    return (false);
  }

  // Hn Cases
  if (p_comm->cbw.data_transfer_length == 0u) {
    if (data_len == 0u) {
      // Case  1: Hn = Dn.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED;
    } else {
      // Case  2: Hn < Di || 3: Hn < Do.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    }
    CORE_EXIT_ATOMIC();

    return (true);
  }
  //  Hi Cases
  if (p_comm->cbw.flags == SLI_USBD_MSC_BMCBWFLAGS_DIR_DEVICE_TO_HOST) {
    if (data_len == 0u) {
      // Case  4: Hi >  Dn.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_FAILED;
      p_comm->stall = true;
    } else if (p_comm->cbw.data_transfer_length > data_len) {
      // Case  5: Hi >  Di.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED;
      p_comm->stall = true;
    } else if (p_comm->cbw.data_transfer_length < data_len) {
      // Case  7: Hi <  Di.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    } else {
      // Case  6: Hi == Di.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED;
    }
    CORE_EXIT_ATOMIC();

    return (true);
  }
  // Ho Cases:
  if (p_comm->cbw.flags == SLI_USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
    if (p_comm->cbw.data_transfer_length > data_len) {
      //  Case 9: Ho > Dn || 11: Ho >  Do.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED;
      p_comm->stall = true;
    } else if (p_comm->cbw.data_transfer_length < data_len) {
      // Case 13: Ho <  Do.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR;
    } else {
      // Case 12: Ho ==  Do.
      p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_PASSED;
    }
    CORE_EXIT_ATOMIC();

    return (true);
  }
  CORE_EXIT_ATOMIC();

  return (true);
}

/****************************************************************************************************//***
 *                                           usbd_msc_process_command()
 *
 * @brief    Process the cbw sent by the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_process_command(sli_usbd_msc_ctrl_t *p_ctrl,
                                     sli_usbd_msc_comm_t *p_comm)
{
  uint8_t     lun;
  uint32_t    data_len;
  uint8_t     data_dir;
  bool        result;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  lun = p_comm->cbw.lun;

  status = p_comm->ctrl_ptr->subclass_driver->process_command(p_ctrl->class_nbr,
                                                              p_ctrl->lun_data_ptr_table[lun],     // Send the cb to subclass dev.
                                                              p_comm->cbw.cb,
                                                              (uint8_t *)p_ctrl->data_buffer,
                                                              &data_len,
                                                              &data_dir,
                                                              p_ctrl->subclass_arg);
  if (status == SL_STATUS_OK) {
    // Verify data xfer conditions.
    result = usbd_msc_verify_response(p_comm, data_len, data_dir);
    if (result == false) {
      status = SL_STATUS_FAIL;
    }
  } else {
    // Set csw field in preparation to be returned to host.
    p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_FAILED;
  }

  if (status == SL_STATUS_OK) {
    p_comm->bytes_to_xfer = SLI_USBD_GET_MIN(p_comm->cbw.data_transfer_length, data_len);
    if (p_comm->bytes_to_xfer > 0u) {
      // Host expects data and device has data.
      if (p_comm->cbw.flags == SLI_USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
        // Rx data from host on bulk-OUT.
        usbd_msc_read_from_host(p_ctrl, p_comm);
      } else {
        // Tx data to host on bulk-IN.
        usbd_msc_write_to_host(p_ctrl, p_comm);
      }
    } else {
      if (p_comm->stall) {
        // Host expects data and but dev has NO data.
        if (p_comm->cbw.flags == SLI_USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
          CORE_ENTER_ATOMIC();
          p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL;
          CORE_EXIT_ATOMIC();

          sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
        } else {
          // Direction from dev to the host.
          CORE_ENTER_ATOMIC();
          p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL;
          CORE_EXIT_ATOMIC();

          sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
        }
      } else {
        // Host expects NO data.
        CORE_ENTER_ATOMIC();
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
        CORE_EXIT_ATOMIC();
      }
    }
  } else {
    // cmd failed.
    if (p_comm->cbw.data_transfer_length == 0u) {
      // If no data stage send csw to host.
      CORE_ENTER_ATOMIC();
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
      CORE_EXIT_ATOMIC();
    } else if (p_comm->cbw.flags == SLI_USBD_MSC_BMCBWFLAGS_DIR_HOST_TO_DEVICE) {
      // If data stage is dost to dev, stall OUT pipe and send csw.
      CORE_ENTER_ATOMIC();
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
    } else {
      // If data stage is dev to host, stall IN pipe and wait for clear stall.
      CORE_ENTER_ATOMIC();
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL;
      CORE_EXIT_ATOMIC();

      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
    }
  }
}

/****************************************************************************************************//***
 *                                           usbd_msc_write_to_host()
 *
 * @brief    Reads data from the MSC device and write it to host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_write_to_host(sli_usbd_msc_ctrl_t *p_ctrl,
                                   sli_usbd_msc_comm_t *p_comm)
{
  uint32_t buf_len;
  CORE_DECLARE_IRQ_STATE;

  buf_len = SLI_USBD_GET_MIN(p_comm->bytes_to_xfer, SL_USBD_MSC_DATA_BUFFER_SIZE);

  while (buf_len > 0u) {
    usbd_msc_read_from_device(p_ctrl, p_comm);

    // Update remaining bytes to transmit.
    p_comm->bytes_to_xfer -= buf_len;
    // Update csw data residue field.
    p_comm->csw.data_residue -= buf_len;
    buf_len = SLI_USBD_GET_MIN(p_comm->bytes_to_xfer, SL_USBD_MSC_DATA_BUFFER_SIZE);
  }

  if (p_comm->stall == true) {
    p_comm->stall = false;

    CORE_ENTER_ATOMIC();
    // Set the next state to bulk-IN stall.
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
  } else {
    CORE_ENTER_ATOMIC();
    // Set the next state to tx csw.
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//***
 *                                           usbd_msc_read_from_device()
 *
 * @brief    Reads data from the MSC devoce and transmits data to the host. csw will be transmitted
 *           after the data completion stage.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_read_from_device(sli_usbd_msc_ctrl_t *p_ctrl,
                                      sli_usbd_msc_comm_t *p_comm)
{
  uint32_t    buf_len;
  uint8_t     lun;
  uint32_t    xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  buf_len = SLI_USBD_GET_MIN(p_comm->bytes_to_xfer, SL_USBD_MSC_DATA_BUFFER_SIZE);
  lun = p_comm->cbw.lun;
  CORE_EXIT_ATOMIC();

  // read data from the device.
  status = p_comm->ctrl_ptr->subclass_driver->read_data(p_ctrl->class_nbr,
                                                        p_ctrl->lun_data_ptr_table[lun],
                                                        p_comm->cbw.cb[0u],
                                                        (uint8_t *)p_ctrl->data_buffer,
                                                        buf_len,
                                                        p_ctrl->subclass_arg);
  if (status != SL_STATUS_OK) {
    p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_FAILED;
    goto end_fail;
  }

  // Tx data to the host.
  status = sl_usbd_core_write_bulk_sync(p_comm->data_Bulk_in_ep_addr,
                                        (void *)p_ctrl->data_buffer,
                                        buf_len,
                                        0u,
                                        false,
                                        &xfer_len);
  if (status != SL_STATUS_OK) {
    goto end_fail;
  }

  return;

  end_fail:
  p_comm->stall = true;
}

/****************************************************************************************************//***
 *                                           usbd_msc_write_to_device()
 *
 * @brief    Save the buffer and buffer length and write data to the MSC device.
 *
 * @param    p_ctrl      Pointer to MSC instance control structure.
 *
 * @param    p_comm      Pointer to MSC comm structure.
 *
 * @param    p_buf       Pointer to the data buffer.
 *
 * @param    xfer_len    Length of the data buffer.
 *******************************************************************************************************/
static void usbd_msc_write_to_device(const sli_usbd_msc_ctrl_t *p_ctrl,
                                     sli_usbd_msc_comm_t       *p_comm,
                                     void                      *p_buf,
                                     uint32_t                  xfer_len)
{
  sl_status_t status;
  uint8_t   lun;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_comm->wr_buffer_ptr = p_buf;
  p_comm->wr_buffer_length = xfer_len;
  CORE_EXIT_ATOMIC();

  lun = p_comm->cbw.lun;

  // write data to device.
  status = p_comm->ctrl_ptr->subclass_driver->write_data(p_ctrl->class_nbr,
                                                         p_ctrl->lun_data_ptr_table[lun],
                                                         p_comm->cbw.cb[0u],
                                                         p_comm->wr_buffer_ptr,
                                                         p_comm->wr_buffer_length,
                                                         p_ctrl->subclass_arg);
  if (status != SL_STATUS_OK) {
    CORE_ENTER_ATOMIC();
    // Enter bulk-OUT stall state.
    p_comm->csw.status = SLI_USBD_MSC_BCSWSTATUS_CMD_FAILED;
    CORE_EXIT_ATOMIC();

    SLI_USBD_LOG_ERR(("USBD MSC: Wr to device, Stall OUT"));

    p_comm->stall = true;
  }
}

/****************************************************************************************************//***
 *                                           usbd_msc_read_from_host()
 *
 * @brief    It receives data from the host. After the data stage is complete, csw is sent.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_read_from_host(sli_usbd_msc_ctrl_t *p_ctrl,
                                    sli_usbd_msc_comm_t *p_comm)
{
  uint32_t  buf_len;
  uint32_t    xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  buf_len = SLI_USBD_GET_MIN(p_comm->bytes_to_xfer, SL_USBD_MSC_DATA_BUFFER_SIZE);
  CORE_EXIT_ATOMIC();

  while (buf_len > 0) {
    SLI_USBD_LOG_VRB(("USBD MSC: Rx Data Len:", (u)buf_len));
    // Rx data from host on bulk-OUT pipe
    status = sl_usbd_core_read_bulk_sync(p_comm->data_Bulk_out_ep_addr,
                                         (void *)p_ctrl->data_buffer,
                                         buf_len,
                                         0u,
                                         &xfer_len);
    if (status != SL_STATUS_OK) {
      CORE_ENTER_ATOMIC();
      // Enter reset recovery state if err.
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      SLI_USBD_LOG_ERR(("USBD MSC: Rx Data, stall OUT"));

      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
      return;
    } else {
      // Process rx data if no err.
      usbd_msc_write_to_device(p_ctrl,
                               p_comm,
                               (void *)p_ctrl->data_buffer,
                               xfer_len);
      p_comm->bytes_to_xfer -= xfer_len;
      p_comm->csw.data_residue -= xfer_len;
      buf_len = SLI_USBD_GET_MIN(p_comm->bytes_to_xfer, SL_USBD_MSC_DATA_BUFFER_SIZE);
    }
  }

  if (p_comm->stall) {
    CORE_ENTER_ATOMIC();
    p_comm->stall = false;
    // Enter bulk-OUT stall state.
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL;
    CORE_EXIT_ATOMIC();

    SLI_USBD_LOG_ERR(("USBD MSC: Rx Data, stall OUT"));
    sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
    return;
  }

  CORE_ENTER_ATOMIC();
  // Enter tx csw state.
  p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CSW;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//***
 *                                               usbd_msc_write_csw()
 *
 * @brief    Send status csw to the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_write_csw(sli_usbd_msc_ctrl_t *p_ctrl,
                               sli_usbd_msc_comm_t *p_comm)
{
  uint32_t    xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  // Set csw signature and rcvd tag from cbw.
  p_comm->csw.signature = SLI_USBD_MSC_SIG_CSW;
  p_comm->csw.tag = p_comm->cbw.tag;

  // write csw to raw buf.
  usbd_msc_format_csw(&p_comm->csw,
                      (void *)p_ctrl->csw_buffer);
  CORE_EXIT_ATOMIC();
  // Tx csw to host through bulk-IN pipe.
  status = sl_usbd_core_write_bulk_sync(p_comm->data_Bulk_in_ep_addr,
                                        (void *)p_ctrl->csw_buffer,
                                        SLI_USBD_MSC_LEN_CSW,
                                        0,
                                        false,
                                        &xfer_len);
  // Enter reset recovery state.
  if (status != SL_STATUS_OK) {
    CORE_ENTER_ATOMIC();
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    SLI_USBD_LOG_ERR(("USBD MSC: TxCSW, stall IN"));
    sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
  } else if (p_comm->csw.status == SLI_USBD_MSC_BCSWSTATUS_PHASE_ERROR) {
    CORE_ENTER_ATOMIC();
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY;
    CORE_EXIT_ATOMIC();
  } else {
    CORE_ENTER_ATOMIC();
    // Enter rx cbw state.
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CBW;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//***
 *                                               usbd_msc_read_cbw()
 *
 * @brief    Receive cbw from the host.
 *
 * @param    p_ctrl  Pointer to MSC instance control structure.
 *
 * @param    p_comm  Pointer to MSC comm structure.
 *******************************************************************************************************/
static void usbd_msc_read_cbw(sli_usbd_msc_ctrl_t *p_ctrl,
                              sli_usbd_msc_comm_t *p_comm)
{
  bool cbw_ok;
  uint32_t    xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  // Rx cbw and returns xfer_len upon success.
  status = sl_usbd_core_read_bulk_sync(p_comm->data_Bulk_out_ep_addr,
                                       (void *)p_ctrl->cbw_buffer,
                                       SLI_USBD_MSC_LEN_CBW,
                                       0,
                                       &xfer_len);
  SLI_USBD_LOG_VRB(("USBD MSC: RxCBW with len=", (u)xfer_len));

  switch (status) {
    case SL_STATUS_OK:
      break;

    case SL_STATUS_ABORT:
    case SL_STATUS_INVALID_STATE:
      CORE_ENTER_ATOMIC();
      if (p_ctrl->state == SLI_USBD_MSC_STATE_CONFIG) {
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CBW;
      }
      CORE_EXIT_ATOMIC();
      SLI_USBD_LOG_ERR(("USBD MSC: RxCBW, OS Abort"));
      break;

    case SL_STATUS_TIMEOUT:
      CORE_ENTER_ATOMIC();
      if (p_ctrl->state == SLI_USBD_MSC_STATE_CONFIG) {
        p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_CBW;
      }
      CORE_EXIT_ATOMIC();
      SLI_USBD_LOG_ERR(("USBD MSC: RxCBW, OS Timeout"));
      break;

    case SL_STATUS_RECEIVE:
    case SL_STATUS_WOULD_OVERFLOW:
      CORE_ENTER_ATOMIC();
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL;
      CORE_EXIT_ATOMIC();

      SLI_USBD_LOG_ERR(("USBD MSC: RxCBW, stall IN/OUT"));
      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
      break;

    case SL_STATUS_FAIL:
    case SL_STATUS_NULL_POINTER:
    case SL_STATUS_NOT_READY:
    case SL_STATUS_FULL:
    default:
      CORE_ENTER_ATOMIC();
      p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL;
      CORE_EXIT_ATOMIC();

      SLI_USBD_LOG_ERR(("USBD MSC: RxCBW, stall OUT"));
      sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);
      break;
  }
  if (status != SL_STATUS_OK) {
    return;
  }

  // Verify that cbw is valid and meaningful.
  cbw_ok = usbd_msc_verify_cbw(p_ctrl,
                               p_comm,
                               (void *)p_ctrl->cbw_buffer,
                               xfer_len);
  if (cbw_ok != true) {
    // Enter reset recovery state.
    CORE_ENTER_ATOMIC();
    p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL;
    CORE_EXIT_ATOMIC();

    SLI_USBD_LOG_ERR(("USBD MSC: RxCBW, stall IN/OUT"));

    sl_usbd_core_stall_endpoint(p_comm->data_Bulk_in_ep_addr, true);
    sl_usbd_core_stall_endpoint(p_comm->data_Bulk_out_ep_addr, true);

    return;
  }

  CORE_ENTER_ATOMIC();
  // Host expected transfer length.
  p_comm->csw.data_residue = p_comm->cbw.data_transfer_length;
  p_comm->bytes_to_xfer = 0u;
  // Enter data transport state.
  p_comm->next_comm_state = SLI_USBD_MSC_COMM_STATE_DATA;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           usbd_msc_parse_cbw()
 *
 * @brief    Parse cbw into cbw structure.
 *
 * @param    p_cbw       Variable that will hold the cbw parsed in this function.
 *
 * @param    p_buf_src   Pointer to buffer that holds cbw.
 *******************************************************************************************************/
static void usbd_msc_parse_cbw(sli_usbd_msc_cbw_t *p_cbw,
                               void               *p_buf_src)
{
  uint8_t *p_buf_src_08;

  p_buf_src_08 = (uint8_t *)p_buf_src;

  memcpy(&p_cbw->signature, (void *)((uint8_t *)p_buf_src_08 +  0u), 4u);
  memcpy(&p_cbw->tag, (void *)((uint8_t *)p_buf_src_08 +  4u), 4u);
  memcpy(&p_cbw->data_transfer_length, (void *)((uint8_t *)p_buf_src_08 +  8u), 4u);
  p_cbw->flags = p_buf_src_08[12u];
  p_cbw->lun = p_buf_src_08[13u];
  p_cbw->cbw_length = p_buf_src_08[14u];

  memcpy((void *)&p_cbw->cb[0u],
         (void *)&p_buf_src_08[15u],
         16u);
}

/****************************************************************************************************//**
 *                                           usbd_msc_format_csw()
 *
 * @brief    Format csw from csw structure.
 *
 * @param    p_csw       Variable holds the csw information.
 *
 * @param    p_buf_dest  Pointer to buffer that will hold csw.
 *******************************************************************************************************/
static void usbd_msc_format_csw(const sli_usbd_msc_csw_t *p_csw,
                                void                     *p_buf_dest)
{
  uint8_t *p_buf_dest_08;

  p_buf_dest_08 = (uint8_t *)p_buf_dest;

  memcpy((void *)((uint8_t *)p_buf_dest_08 + 0u), &p_csw->signature, 4u);
  memcpy((void *)((uint8_t *)p_buf_dest_08 + 4u), &p_csw->tag, 4u);
  memcpy((void *)((uint8_t *)p_buf_dest_08 + 8u), &p_csw->data_residue, 4u);

  p_buf_dest_08[12] = p_csw->status;
}
