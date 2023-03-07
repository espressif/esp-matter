/***************************************************************************//**
 * @file
 * @brief USB Device Vendor Class
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

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_enum.h"
#include "sl_status.h"

#include "em_core.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_vendor.h"

#include "sli_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct sli_usbd_vendor_ctrl sli_usbd_vendor_ctrl_t;

/********************************************************************************************************
 *                                           VENDOR CLASS STATES
 *******************************************************************************************************/

SL_ENUM(sli_usbd_vendor_state_t) {
  SLI_USBD_VENDOR_STATE_NONE = 0,
  SLI_USBD_VENDOR_STATE_INIT,
  SLI_USBD_VENDOR_STATE_CONFIG
};

/********************************************************************************************************
 *                               VENDOR CLASS EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

// Vendor class comm info
typedef struct {
  // Ptr to ctrl info.
  sli_usbd_vendor_ctrl_t *ctrl_ptr;
  // Avail EP for comm: Bulk (and Intr)
  uint8_t       data_bulk_in_endpoint_address;
  uint8_t       data_bulk_out_endpoint_address;
  uint8_t       interrupt_in_endpoint_address;
  uint8_t       interrupt_out_endpoint_address;

  bool      data_bulk_in_active_transfer;
  bool      data_bulk_out_active_transfer;
  bool      interrupt_in_active_transfer;
  bool      interrupt_out_active_transfer;
} sli_usbd_vendor_comm_t;

// Vendor Class Ctrl Info
struct sli_usbd_vendor_ctrl {
  sli_usbd_vendor_state_t           state;                                ///< Vendor class state.
  uint8_t                           class_nbr;                            ///< Vendor class instance nbr.
  sli_usbd_vendor_comm_t            *comm_ptr;                            ///< Vendor class comm info ptr.
  bool                              interrupt_enable;                     ///< Intr IN & OUT EPs en/dis flag.
  uint16_t                          interrupt_interval;                   ///< Polling interval for intr IN & OUT EPs.
  sl_usbd_vendor_callbacks_t        *callback_ptr;                        ///< Ptr to app callback for vendor-specific req.
  sl_usbd_vendor_async_function_t   bulk_read_async_function;             ///< Ptr to callback  used for async comm.
  void                              *bulk_read_async_arg_ptr;             ///< Ptr to extra arg used for async comm.
  sl_usbd_vendor_async_function_t   bulk_write_async_function;            ///< Ptr to callback  used for async comm.
  void                              *bulk_write_async_arg_ptr;            ///< Ptr to extra arg used for async comm.
  sl_usbd_vendor_async_function_t   interrupt_read_async_function;        ///< Ptr to callback  used for async comm.
  void                              *interrupt_read_async_arg_ptr;        ///< Ptr to extra arg used for async comm.
  sl_usbd_vendor_async_function_t   interrupt_write_async_function;       ///< Ptr to callback  used for async comm.
  void                              *interrupt_write_async_arg_ptr;       ///< Ptr to extra arg used for async comm.
#if (USBD_CFG_MS_OS_DESC_EN == 1)                                         // Microsoft ext properties.
  sl_usbd_microsoft_ext_property_t  microsoft_ext_property_table[SL_USBD_VENDOR_MS_EXTENDED_PROPERTIES_QUANTITY];
  uint8_t                           interface_number;                     // Interface number associated with class instance
  uint8_t                           microsoft_ext_property_next;
#endif
};

// Vendor Class Root Struct
typedef struct {
  sli_usbd_vendor_ctrl_t  ctrl_table[SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY];                                        ///< Vendor class ctrl array.
  uint8_t                 ctrl_nbr_next;
  sli_usbd_vendor_comm_t  comm_tbl[SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY * SL_USBD_VENDOR_CONFIGURATION_QUANTITY];  ///< Vendor class comm array.
  uint8_t                 comm_nbr_next;
} sli_usbd_vendor_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static sli_usbd_vendor_t usbd_vendor;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_vendor_enable(uint8_t config_nbr,
                               void    *p_if_class_arg);

static void usbd_vendor_disable(uint8_t config_nbr,
                                void    *p_if_class_arg);

static bool usbd_vendor_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                        void                      *p_if_class_arg);

#if (USBD_CFG_MS_OS_DESC_EN == 1)
static uint8_t usbd_vendor_get_microsoft_compat_id(uint8_t *p_sub_compat_id_ix);

static uint8_t usbd_vendor_get_microsoft_ext_property_table(sl_usbd_microsoft_ext_property_t **pp_ext_property_tbl, uint8_t if_no);
#endif

static void usbd_vendor_read_bulk_async_complete(uint8_t      ep_addr,
                                                 void         *p_buf,
                                                 uint32_t     buf_len,
                                                 uint32_t     xfer_len,
                                                 void         *p_arg,
                                                 sl_status_t  status);

static void usbd_vendor_write_bulk_async_complete(uint8_t      ep_addr,
                                                  void         *p_buf,
                                                  uint32_t     buf_len,
                                                  uint32_t     xfer_len,
                                                  void         *p_arg,
                                                  sl_status_t  status);

static void usbd_vendor_read_interrupt_async_complete(uint8_t      ep_addr,
                                                      void         *p_buf,
                                                      uint32_t     buf_len,
                                                      uint32_t     xfer_len,
                                                      void         *p_arg,
                                                      sl_status_t  status);

static void usbd_vendor_write_interrupt_async_complete(uint8_t      ep_addr,
                                                       void         *p_buf,
                                                       uint32_t     buf_len,
                                                       uint32_t     xfer_len,
                                                       void         *p_arg,
                                                       sl_status_t  status);

/********************************************************************************************************
 *                                           VENDOR CLASS DRIVER
 *******************************************************************************************************/

static sl_usbd_class_driver_t usbd_vendor_driver = {
  usbd_vendor_enable,
  usbd_vendor_disable,
  NULL,                                                     // Vendor does NOT use alternate interface(s).
  NULL,
  NULL,                                                     // Vendor does NOT use functional EP desc.
  NULL,
  NULL,                                                     // Vendor does NOT use functional IF desc.
  NULL,
  NULL,                                                     // Vendor does NOT handle std req with IF recipient.
  NULL,                                                     // Vendor does NOT define class-specific req.
  usbd_vendor_request_handler,

#if (USBD_CFG_MS_OS_DESC_EN == 1)
  usbd_vendor_get_microsoft_compat_id,
  usbd_vendor_get_microsoft_ext_property_table,
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initializes the internal structures and variables used by the Vendor class
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_init(void)
{
  uint8_t       ix;
  sli_usbd_vendor_ctrl_t *p_ctrl;
  sli_usbd_vendor_comm_t *p_comm;

  usbd_vendor.ctrl_nbr_next = SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY;
  usbd_vendor.comm_nbr_next = SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY * SL_USBD_VENDOR_CONFIGURATION_QUANTITY;

  // Init vendor class struct.
  for (ix = 0u; ix < usbd_vendor.ctrl_nbr_next; ix++) {
    p_ctrl = &usbd_vendor.ctrl_table[ix];
    p_ctrl->state = SLI_USBD_VENDOR_STATE_NONE;
    p_ctrl->class_nbr = SL_USBD_CLASS_NBR_NONE;
    p_ctrl->comm_ptr = NULL;
    p_ctrl->interrupt_enable = false;
    p_ctrl->interrupt_interval = 0u;
    p_ctrl->callback_ptr = NULL;
    p_ctrl->bulk_read_async_function = NULL;
    p_ctrl->bulk_read_async_arg_ptr = NULL;
    p_ctrl->bulk_write_async_function = NULL;
    p_ctrl->bulk_write_async_arg_ptr = NULL;
    p_ctrl->interrupt_read_async_function = NULL;
    p_ctrl->interrupt_read_async_arg_ptr = NULL;
    p_ctrl->interrupt_write_async_function = NULL;
    p_ctrl->interrupt_write_async_arg_ptr = NULL;

#if (USBD_CFG_MS_OS_DESC_EN == 1)
    p_ctrl->microsoft_ext_property_next = 0u;

    memset(p_ctrl->microsoft_ext_property_table, 0, sizeof(sl_usbd_microsoft_ext_property_t) * SL_USBD_VENDOR_MS_EXTENDED_PROPERTIES_QUANTITY);
#endif
  }

  // Init vendor EP tbl.
  for (ix = 0u; ix < usbd_vendor.comm_nbr_next; ix++) {
    p_comm = &usbd_vendor.comm_tbl[ix];
    p_comm->ctrl_ptr = NULL;
    p_comm->data_bulk_in_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->data_bulk_out_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->interrupt_in_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->interrupt_out_endpoint_address = SL_USBD_ENDPOINT_ADDR_NONE;

    p_comm->data_bulk_in_active_transfer = false;
    p_comm->data_bulk_out_active_transfer = false;
    p_comm->interrupt_in_active_transfer = false;
    p_comm->interrupt_out_active_transfer = false;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a new instance of the Vendor class
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_create_instance(bool                        intr_en,
                                           uint16_t                    interval,
                                           sl_usbd_vendor_callbacks_t  *p_vendor_callbacks,
                                           uint8_t                     *p_class_nbr)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  uint8_t       vendor_class_nbr;
  CORE_DECLARE_IRQ_STATE;

  if (p_class_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (intr_en == true) {
    // interval must be a power of 2.
    if (SLI_USBD_IS_PWR2(interval) != true) {
      *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  CORE_ENTER_ATOMIC();
  // Chk if max nbr of instances reached.
  if (usbd_vendor.ctrl_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_class_nbr = SL_USBD_CLASS_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Next avail vendor class instance nbr.
  usbd_vendor.ctrl_nbr_next--;
  // Alloc new vendor class instance nbr.
  vendor_class_nbr = usbd_vendor.ctrl_nbr_next;
  CORE_EXIT_ATOMIC();

  p_ctrl = &usbd_vendor.ctrl_table[vendor_class_nbr];          // Get vendor class instance.
                                                               // Store vendor class instance info.
  p_ctrl->interrupt_enable = intr_en;                          // Intr EPs en/dis.
  p_ctrl->interrupt_interval = interval;                       // Polling interval for intr EPs.
  p_ctrl->callback_ptr = p_vendor_callbacks;                   // App callback for vendor-specific req.

  *p_class_nbr = vendor_class_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Creates a new instance of the Vendor class
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_add_to_configuration(uint8_t class_nbr,
                                                uint8_t config_nbr)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  sli_usbd_vendor_comm_t *p_comm;
  uint8_t       if_nbr;
  uint8_t       ep_addr;
  uint16_t       comm_nbr;
  uint16_t       intr_interval;
  sl_status_t        status;
  CORE_DECLARE_IRQ_STATE;

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get vendor class instance.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();

  if (usbd_vendor.comm_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Alloc new vendor class comm info nbr.
  comm_nbr = usbd_vendor.comm_nbr_next - 1u;
  // Next avail vendor class comm info nbr.
  usbd_vendor.comm_nbr_next--;
  CORE_EXIT_ATOMIC();

  // Get vendor class comm info.
  p_comm = &usbd_vendor.comm_tbl[comm_nbr];

  // Config Desc Construction
  // See Note #2.
  // Add vendor IF desc to config desc.
  status = sl_usbd_core_add_interface(config_nbr,
                                      &usbd_vendor_driver,
                                      (void *)p_comm,
                                      NULL,
                                      SL_USBD_CLASS_CODE_VENDOR_SPECIFIC,
                                      SL_USBD_SUBCLASS_CODE_VENDOR_SPECIFIC,
                                      SL_USBD_PROTOCOL_CODE_VENDOR_SPECIFIC,
                                      "Vendor-specific class",
                                      &if_nbr);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Microsoft ext properties.
#if (USBD_CFG_MS_OS_DESC_EN == 1)
  p_ctrl->interface_number = if_nbr;
#endif

  // Add bulk IN EP desc.
  status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                          if_nbr,
                                          0u,
                                          true,
                                          0u,
                                          &ep_addr);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Store bulk IN EP addr.
  p_comm->data_bulk_in_endpoint_address = ep_addr;

  // Add bulk OUT EP desc.
  status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                          if_nbr,
                                          0u,
                                          false,
                                          0u,
                                          &ep_addr);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Store bulk OUT EP addr.
  p_comm->data_bulk_out_endpoint_address = ep_addr;

  if (p_ctrl->interrupt_enable == true) {
    if (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
      // In FS, bInterval in frames.
      intr_interval = p_ctrl->interrupt_interval;
    } else {
      // In HS, bInterval in microframes.
      intr_interval = p_ctrl->interrupt_interval * 8u;
    }

    // Add intr IN EP desc.
    status = sl_usbd_core_add_interrupt_endpoint(config_nbr,
                                                 if_nbr,
                                                 0u,
                                                 true,
                                                 0u,
                                                 intr_interval,
                                                 &ep_addr);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Store intr IN EP addr.
    p_comm->interrupt_in_endpoint_address = ep_addr;

    // Add intr OUT EP desc.
    status = sl_usbd_core_add_interrupt_endpoint(config_nbr,
                                                 if_nbr,
                                                 0u,
                                                 false,
                                                 0u,
                                                 intr_interval,
                                                 &ep_addr);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Store intr OUT EP addr.
    p_comm->interrupt_out_endpoint_address = ep_addr;
  }
  // Store vendor class instance info.
  CORE_ENTER_ATOMIC();
  // Set class instance to init state.
  p_ctrl->state = SLI_USBD_VENDOR_STATE_INIT;
  p_ctrl->class_nbr = class_nbr;
  p_ctrl->comm_ptr = NULL;
  CORE_EXIT_ATOMIC();

  // Save ref to vendor class instance ctrl struct.
  p_comm->ctrl_ptr = p_ctrl;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the vendor class enable state
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_is_enabled(uint8_t  class_nbr,
                                      bool     *p_enabled)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  sl_usbd_device_state_t  state;
  sl_status_t             status;

  // TODO validate class_nbr
  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];
  // Get dev state.
  status = sl_usbd_core_get_device_state(&state);

  if ((status == SL_STATUS_OK)
      && (state == SL_USBD_DEVICE_STATE_CONFIGURED)
      && (p_ctrl->state == SLI_USBD_VENDOR_STATE_CONFIG)) {
    *p_enabled = true;
  } else {
    *p_enabled = false;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds a Microsoft OS extended property to this vendor class instance
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
sl_status_t sl_usbd_vendor_add_microsoft_ext_property(uint8_t        class_nbr,
                                                      uint8_t        property_type,
                                                      const uint8_t  *p_property_name,
                                                      uint16_t       property_name_len,
                                                      const uint8_t  *p_property,
                                                      uint32_t       property_len)
{
  uint8_t              ext_property_nbr;
  sli_usbd_vendor_ctrl_t        *p_ctrl;
  sl_usbd_microsoft_ext_property_t *p_ext_property;
  CORE_DECLARE_IRQ_STATE;

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!((property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_SZ)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_EXPAND_SZ)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_BINARY)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_LINK)
        || (property_type == SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_MULTI_SZ))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((p_property_name == NULL) && (property_name_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_property == NULL) && (property_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  ext_property_nbr = p_ctrl->microsoft_ext_property_next;

#if (RTOS_ARG_CHK_EXT_EN == 1)
  if (ext_property_nbr >= SL_USBD_VENDOR_MS_EXTENDED_PROPERTIES_QUANTITY) {
    CORE_EXIT_ATOMIC();

    return SL_STATUS_ALLOCATION_FAILED;
  }
#endif
  p_ctrl->microsoft_ext_property_next++;

  p_ext_property = &p_ctrl->microsoft_ext_property_table[ext_property_nbr];

  p_ext_property->property_type = property_type;
  p_ext_property->property_name_ptr = p_property_name;
  p_ext_property->property_name_len = property_name_len;
  p_ext_property->property_ptr = p_property;
  p_ext_property->property_len = property_len;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Receive the data from the host through the Bulk OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_bulk_sync(uint8_t    class_nbr,
                                          void       *p_buf,
                                          uint32_t   buf_len,
                                          uint16_t   timeout,
                                          uint32_t   *p_xfer_len)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  uint32_t       xfer_len;
  bool      conn;
  sl_status_t        status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0u)) {
    *p_xfer_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  status = sl_usbd_core_read_bulk_sync(p_ctrl->comm_ptr->data_bulk_out_endpoint_address,
                                       p_buf,
                                       buf_len,
                                       timeout,
                                       &xfer_len);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0;
    return status;
  }

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sends the data to host through Bulk IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_bulk_sync(uint8_t     class_nbr,
                                           void        *p_buf,
                                           uint32_t    buf_len,
                                           uint16_t    timeout,
                                           bool        end,
                                           uint32_t    *p_xfer_len)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  uint32_t               xfer_len;
  bool                   conn;
  sl_status_t            status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0u)) {
    *p_xfer_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  status = sl_usbd_core_write_bulk_sync(p_ctrl->comm_ptr->data_bulk_in_endpoint_address,
                                        p_buf,
                                        buf_len,
                                        timeout,
                                        end,
                                        &xfer_len);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0;
    return status;
  }

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Receive the data from the host through the Bulk OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_bulk_async(uint8_t                         class_nbr,
                                           void                            *p_buf,
                                           uint32_t                        buf_len,
                                           sl_usbd_vendor_async_function_t async_fnct,
                                           void                            *p_async_arg)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  bool      conn;
  sl_status_t        status;

  if ((p_buf == NULL) && (buf_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  p_ctrl->bulk_read_async_function = async_fnct;
  p_ctrl->bulk_read_async_arg_ptr = p_async_arg;

  // Check if another xfer is already in progress.
  if (p_ctrl->comm_ptr->data_bulk_out_active_transfer == false) {
    // Indicate that a xfer is in progress.
    p_ctrl->comm_ptr->data_bulk_out_active_transfer = true;
    status = sl_usbd_core_read_bulk_async(p_ctrl->comm_ptr->data_bulk_out_endpoint_address,
                                          p_buf,
                                          buf_len,
                                          usbd_vendor_read_bulk_async_complete,
                                          (void *)p_ctrl);
    if (status != SL_STATUS_OK) {
      p_ctrl->comm_ptr->data_bulk_out_active_transfer = false;
    }
  } else {
    status = SL_STATUS_NOT_READY;
  }

  return status;
}

/****************************************************************************************************//**
 * Sends the data to host through the Bulk IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_bulk_async(uint8_t                         class_nbr,
                                            void                            *p_buf,
                                            uint32_t                        buf_len,
                                            sl_usbd_vendor_async_function_t async_fnct,
                                            void                            *p_async_arg,
                                            bool                            end)
{
  sli_usbd_vendor_ctrl_t  *p_ctrl;
  bool                    conn;
  sl_status_t             status;

  if ((p_buf == NULL) && (buf_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  p_ctrl->bulk_write_async_function = async_fnct;
  p_ctrl->bulk_write_async_arg_ptr = p_async_arg;

  // Check if another xfer is already in progress.
  if (p_ctrl->comm_ptr->data_bulk_in_active_transfer == false) {
    // Indicate that a xfer is in progress.
    p_ctrl->comm_ptr->data_bulk_in_active_transfer = true;
    status = sl_usbd_core_write_bulk_async(p_ctrl->comm_ptr->data_bulk_in_endpoint_address,
                                           p_buf,
                                           buf_len,
                                           usbd_vendor_write_bulk_async_complete,
                                           (void *)p_ctrl,
                                           end);
    if (status != SL_STATUS_OK) {
      p_ctrl->comm_ptr->data_bulk_in_active_transfer = false;
    }
  } else {
    status = SL_STATUS_NOT_READY;
  }

  return status;
}

/****************************************************************************************************//**
 * Receives the data from the host through the Interrupt OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_interrupt_sync(uint8_t    class_nbr,
                                               void       *p_buf,
                                               uint32_t   buf_len,
                                               uint16_t   timeout,
                                               uint32_t   *p_xfer_len)
{
  sli_usbd_vendor_ctrl_t  *p_ctrl;
  uint32_t                xfer_len;
  bool                    conn;
  sl_status_t             status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0u)) {
    *p_xfer_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  status = sl_usbd_core_read_interrupt_sync(p_ctrl->comm_ptr->interrupt_out_endpoint_address,
                                            p_buf,
                                            buf_len,
                                            timeout,
                                            &xfer_len);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0;
    return status;
  }

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sends data to the host through the Interrupt IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_interrupt_sync(uint8_t   class_nbr,
                                                void      *p_buf,
                                                uint32_t  buf_len,
                                                uint16_t  timeout,
                                                bool      end,
                                                uint32_t  *p_xfer_len)
{
  sli_usbd_vendor_ctrl_t  *p_ctrl;
  uint32_t                xfer_len;
  bool                    conn;
  sl_status_t             status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if ((p_buf == NULL) && (buf_len != 0u)) {
    *p_xfer_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    *p_xfer_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  status = sl_usbd_core_write_interrupt_sync(p_ctrl->comm_ptr->interrupt_in_endpoint_address,
                                             p_buf,
                                             buf_len,
                                             timeout,
                                             end,
                                             &xfer_len);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0;
    return status;
  }

  *p_xfer_len = xfer_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Receives the data from the host through Interrupt OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_interrupt_async(uint8_t                         class_nbr,
                                                void                            *p_buf,
                                                uint32_t                        buf_len,
                                                sl_usbd_vendor_async_function_t async_fnct,
                                                void                            *p_async_arg)
{
  sli_usbd_vendor_ctrl_t  *p_ctrl;
  bool                    conn;
  sl_status_t             status;

  if ((p_buf == NULL) && (buf_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  p_ctrl->interrupt_read_async_function = async_fnct;
  p_ctrl->interrupt_read_async_arg_ptr = p_async_arg;

  // Check if another xfer is already in progress.
  if (p_ctrl->comm_ptr->interrupt_out_active_transfer == false) {
    // Indicate that a xfer is in progres.
    p_ctrl->comm_ptr->interrupt_out_active_transfer = true;
    status = sl_usbd_core_read_interrupt_async(p_ctrl->comm_ptr->interrupt_out_endpoint_address,
                                               p_buf,
                                               buf_len,
                                               usbd_vendor_read_interrupt_async_complete,
                                               (void *)p_ctrl);
    if (status != SL_STATUS_OK) {
      p_ctrl->comm_ptr->interrupt_out_active_transfer = false;
    }
  } else {
    status = SL_STATUS_NOT_READY;
  }

  return status;
}

/****************************************************************************************************//**
 * Sends the data to the host through the Interrupt IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_interrupt_async(uint8_t                          class_nbr,
                                                 void                             *p_buf,
                                                 uint32_t                         buf_len,
                                                 sl_usbd_vendor_async_function_t  async_fnct,
                                                 void                             *p_async_arg,
                                                 bool                             end)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  bool      conn;
  sl_status_t        status;

  if ((p_buf == NULL) && (buf_len != 0u)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_usbd_vendor_is_enabled(class_nbr, &conn);

  // Chk class state.
  if (conn != true) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get Vendor class instance ctrl struct.
  p_ctrl = &usbd_vendor.ctrl_table[class_nbr];

  p_ctrl->interrupt_write_async_function = async_fnct;
  p_ctrl->interrupt_write_async_arg_ptr = p_async_arg;

  // Check if another xfer is already in progress.
  if (p_ctrl->comm_ptr->interrupt_in_active_transfer == false) {
    // Indicate that a xfer is in progress.
    p_ctrl->comm_ptr->interrupt_in_active_transfer = true;
    status = sl_usbd_core_write_interrupt_async(p_ctrl->comm_ptr->interrupt_in_endpoint_address,
                                                p_buf,
                                                buf_len,
                                                usbd_vendor_write_interrupt_async_complete,
                                                (void *)p_ctrl,
                                                end);
    if (status != SL_STATUS_OK) {
      p_ctrl->comm_ptr->interrupt_in_active_transfer = false;
    }
  } else {
    status = SL_STATUS_NOT_READY;
  }

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          usbd_vendor_enable()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    config_nbr      Configuration index to add the interface to.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void usbd_vendor_enable(uint8_t config_nbr,
                               void    *p_if_class_arg)
{
  sli_usbd_vendor_comm_t *p_comm;
  sli_usbd_vendor_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_vendor_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  CORE_ENTER_ATOMIC();
  p_ctrl->comm_ptr = p_comm;
  p_ctrl->state = SLI_USBD_VENDOR_STATE_CONFIG;
  CORE_EXIT_ATOMIC();

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->enable != NULL) {
    p_ctrl->callback_ptr->enable(p_ctrl->class_nbr);
  }
}

/****************************************************************************************************//**
 *                                          usbd_vendor_disable()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    config_nbr      Configuration index to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument.
 *******************************************************************************************************/
static void usbd_vendor_disable(uint8_t config_nbr,
                                void    *p_if_class_arg)
{
  sli_usbd_vendor_comm_t *p_comm;
  sli_usbd_vendor_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_vendor_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  CORE_ENTER_ATOMIC();
  p_comm->ctrl_ptr->state = SLI_USBD_VENDOR_STATE_INIT;
  CORE_EXIT_ATOMIC();

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->disable != NULL) {
    p_ctrl->callback_ptr->disable(p_ctrl->class_nbr);
  }
}

/****************************************************************************************************//**
 *                                           usbd_vendor_request_handler()
 *
 * @brief    Process vendor-specific request.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument passed to sl_usbd_core_add_interface().
 *
 * @return   true,   if vendor-specific request successfully processed.
 *
 *           false, otherwise.
 *******************************************************************************************************/
static bool usbd_vendor_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                        void                      *p_if_class_arg)
{
  sli_usbd_vendor_comm_t *p_comm;
  sli_usbd_vendor_ctrl_t *p_ctrl;
  bool                    valid;

  p_comm = (sli_usbd_vendor_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  if (p_ctrl->callback_ptr != NULL && p_ctrl->callback_ptr->setup_req != NULL) {
    p_ctrl->callback_ptr->setup_req(p_ctrl->class_nbr, p_setup_req);
    valid = true;
  } else {
    valid = false;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                       usbd_vendor_get_microsoft_compat_id()
 *
 * @brief    Returns Microsoft descriptor compatible id.
 *
 * @param    p_sub_compat_id_ix  Pointer to the variable that will receive subcompatible id.
 *
 * @return   Compatible id.
 *
 *           false, otherwise.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
static uint8_t usbd_vendor_get_microsoft_compat_id(uint8_t *p_sub_compat_id_ix)
{
  *p_sub_compat_id_ix = SL_USBD_MICROSOFT_SUBCOMPAT_ID_NULL;

  return (SL_USBD_MICROSOFT_COMPAT_ID_WINUSB);
}
#endif

/****************************************************************************************************//**
 *                                   usbd_vendor_get_microsoft_ext_property_table()
 *
 * @brief    Returns Microsoft descriptor extended properties table.
 *
 * @param    pp_ext_property_tbl     Pointer to the variable that will receive the Microsoft extended
 *                                   properties table.
 *
 * @param    interface_number        Interface number
 *
 * @return   Number of Microsoft extended properties in table.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
static uint8_t usbd_vendor_get_microsoft_ext_property_table(sl_usbd_microsoft_ext_property_t **pp_ext_property_tbl,
                                                            uint8_t                          interface_number)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;

  for (int i = 0; i < SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY; i++) {
    // Get Vendor class instance ctrl struct.
    p_ctrl = &usbd_vendor.ctrl_table[i];
    if (p_ctrl->interface_number == interface_number) {
      *pp_ext_property_tbl = p_ctrl->microsoft_ext_property_table;
      // Only one extended property (GUID) supported.
      return (p_ctrl->microsoft_ext_property_next);
    }
  }
  // no extended properties table has been set
  return 0;
}
#endif

/****************************************************************************************************//**
 *                                           usbd_vendor_read_bulk_async_complete()
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
static void usbd_vendor_read_bulk_async_complete(uint8_t      ep_addr,
                                                 void         *p_buf,
                                                 uint32_t     buf_len,
                                                 uint32_t     xfer_len,
                                                 void         *p_arg,
                                                 sl_status_t  status)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  void             *p_callback_arg;

  (void)&ep_addr;

  // Get Vendor class instance ctrl struct.
  p_ctrl = (sli_usbd_vendor_ctrl_t *)p_arg;
  p_callback_arg = p_ctrl->bulk_read_async_arg_ptr;

  // Xfer finished, no more active xfer.
  p_ctrl->comm_ptr->data_bulk_out_active_transfer = false;
  // Call app callback to inform about xfer completion.
  p_ctrl->bulk_read_async_function(p_ctrl->class_nbr,
                                   p_buf,
                                   buf_len,
                                   xfer_len,
                                   p_callback_arg,
                                   status);
}

/****************************************************************************************************//**
 *                                           usbd_vendor_write_bulk_async_complete()
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
static void usbd_vendor_write_bulk_async_complete(uint8_t       ep_addr,
                                                  void          *p_buf,
                                                  uint32_t      buf_len,
                                                  uint32_t      xfer_len,
                                                  void          *p_arg,
                                                  sl_status_t   status)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  void             *p_callback_arg;

  (void)&ep_addr;

  // Get Vendor class instance ctrl struct.
  p_ctrl = (sli_usbd_vendor_ctrl_t *)p_arg;
  p_callback_arg = p_ctrl->bulk_write_async_arg_ptr;

  // Xfer finished, no more active xfer.
  p_ctrl->comm_ptr->data_bulk_in_active_transfer = false;
  // Call app callback to inform about xfer completion.
  p_ctrl->bulk_write_async_function(p_ctrl->class_nbr,
                                    p_buf,
                                    buf_len,
                                    xfer_len,
                                    p_callback_arg,
                                    status);
}

/****************************************************************************************************//**
 *                                       usbd_vendor_read_interrupt_async_complete()
 *
 * @brief    Inform the application about the Interrupt OUT transfer completion.
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
static void usbd_vendor_read_interrupt_async_complete(uint8_t       ep_addr,
                                                      void          *p_buf,
                                                      uint32_t      buf_len,
                                                      uint32_t      xfer_len,
                                                      void          *p_arg,
                                                      sl_status_t   status)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  void             *p_callback_arg;

  (void)&ep_addr;

  // Get Vendor class instance ctrl struct.
  p_ctrl = (sli_usbd_vendor_ctrl_t *)p_arg;
  p_callback_arg = p_ctrl->interrupt_read_async_arg_ptr;

  // Xfer finished, no more active xfer.
  p_ctrl->comm_ptr->interrupt_out_active_transfer = false;
  // Call app callback to inform about xfer completion.
  p_ctrl->interrupt_read_async_function(p_ctrl->class_nbr,
                                        p_buf,
                                        buf_len,
                                        xfer_len,
                                        p_callback_arg,
                                        status);
}

/****************************************************************************************************//**
 *                                       usbd_vendor_write_interrupt_async_complete()
 *
 * @brief    Inform the application about the Interrupt IN transfer completion.
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
static void usbd_vendor_write_interrupt_async_complete(uint8_t      ep_addr,
                                                       void         *p_buf,
                                                       uint32_t     buf_len,
                                                       uint32_t     xfer_len,
                                                       void         *p_arg,
                                                       sl_status_t  status)
{
  sli_usbd_vendor_ctrl_t *p_ctrl;
  void             *p_callback_arg;

  (void)&ep_addr;

  // Get Vendor class instance ctrl struct.
  p_ctrl = (sli_usbd_vendor_ctrl_t *)p_arg;
  p_callback_arg = p_ctrl->interrupt_write_async_arg_ptr;

  // Xfer finished, no more active xfer.
  p_ctrl->comm_ptr->interrupt_in_active_transfer = false;
  // Call app callback to inform about xfer completion.
  p_ctrl->interrupt_write_async_function(p_ctrl->class_nbr,
                                         p_buf,
                                         buf_len,
                                         xfer_len,
                                         p_callback_arg,
                                         status);
}
