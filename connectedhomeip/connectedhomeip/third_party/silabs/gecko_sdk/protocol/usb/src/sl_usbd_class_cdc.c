/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (Cdc)
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

/****************************************************************************************************//**
 * @note   (1) This implementation is compliant with the CDC specification revision 1.2
 *             errata 1. November 3, 2010.
 *******************************************************************************************************/

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
#include "sl_usbd_class_cdc.h"

#include "sli_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CDC FUNCTIONAL DESCRIPTOR SIZE
 *******************************************************************************************************/

#define  SLI_USBD_CDC_DESC_SIZE_HEADER                       5u     // Header functional desc size.
#define  SLI_USBD_CDC_DESC_SIZE_UNION_MIN                    4u     // Min size of union functional desc.

/********************************************************************************************************
 *                                       CDC TOTAL NUMBER DEFINES
 *******************************************************************************************************/

#define  SLI_USBD_CDC_NBR_TOTAL                     (255u - 1u)
#define  SLI_USBD_CDC_DATA_IF_NBR_TOTAL             (255u - 1u)

/********************************************************************************************************
*                        MAXIMUM NUMBER OF COMMUNICATION/DATA IF EP STRUCTURES
********************************************************************************************************/

// Max nbr of comm struct.
#define  SLI_USBD_CDC_COMM_NBR_MAX                  (SL_USBD_CDC_CLASS_INSTANCE_QUANTITY \
                                                     * SL_USBD_CDC_CONFIGURATION_QUANTITY)

// Max nbr of data IF EP struct.
#define  SLI_USBD_CDC_DATA_IF_EP_NBR_MAX            (SL_USBD_CDC_CLASS_INSTANCE_QUANTITY   \
                                                     * SL_USBD_CDC_DATA_INTERFACE_QUANTITY \
                                                     * SL_USBD_CDC_CONFIGURATION_QUANTITY)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CDC STATE DATA TYPE
 *******************************************************************************************************/

SL_ENUM(sli_usbd_cdc_state_t) {
  SLI_USBD_CDC_STATE_NONE = 0,
  SLI_USBD_CDC_STATE_INIT,
  SLI_USBD_CDC_STATE_CONFIG
};

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct sli_usbd_cdc_comm sli_usbd_cdc_ctrl_t;
typedef struct sli_usbd_cdc_data_interface sli_usbd_cdc_data_interface_t;

/********************************************************************************************************
 *                                       CDC DATA IF CLASS DATA TYPE
 *
 * Note(s) : (1) USB CDC specification specifies that the type of endpoints belonging to a Data IF are
 *               restricted to being either isochronous or bulk, and are expected to exist in pairs of
 *               the same type (one IN and one OUT).
 *******************************************************************************************************/

typedef struct {
  uint8_t data_in;
  uint8_t data_out;
} sli_usbd_cdc_data_interface_endpoint_t;

struct sli_usbd_cdc_data_interface {
  uint8_t       interface_nbr;                               ///< Data IF nbr.
  uint8_t       protocol;                                    ///< Data IF protocol.
  bool      isochronous_enable;                              ///< EP isochronous enable.
  sli_usbd_cdc_data_interface_t *next_ptr;                   ///< Next data IF.
};

/****************************************************************************************************//**
 *                                   CDC COMMUNICATION IF DATA TYPE
 *
 * @note     (1) A CDC device consists in one communication IF, and multiple data IFs (optional).
 *                   @verbatim
 *                       +-----IFs----+-------EPs-------+
 *                       |  COMM_IF   |  CTRL           | <--------  Mgmt   Requests.
 *                       |            |  INTR (Optional)| ---------> Events Notifications.
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (0)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (0)
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (1)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (1)
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (2)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (2)
 *                       +------------+-----------------+
 *                                   .
 *                                   .
 *                                   .
 *                       +------------+-----------------+
 *                       |  DATA_IF   |  BULK/ISOC IN   | ---------> Data Tx (n - 1)
 *                       |            |  BULK/ISOC OUT  | <--------- Data Rx (n - 1)
 *                       +------------+-----------------+
 *                   @endverbatim
 *               - (a)  The communication IF may have an optional notification element. Notifications are
 *                       sent using a interrupt endpoint.
 *               - (b)  The communication IF structure contains a link list of Data IFs.
 *
 *******************************************************************************************************/
/// CDC Data Class Interface Communication
typedef struct {
  uint8_t               cci_interface_nbr;                          ///< Comm Class IF nbr.
  sli_usbd_cdc_ctrl_t   *ctrl_ptr;                                  ///< Ptr to ctrl info.
  uint8_t               notify_in;                                  ///< Notification EP (see note #1a).
  uint16_t              data_interface_endpoint_index;              ///< Start ix of data IFs EP information.
  bool                  notify_in_active_transfer;
} sli_usbd_cdc_comm_t;

/// CDC Class Control Information
struct sli_usbd_cdc_comm {
  sli_usbd_cdc_state_t            state;                            ///< CDC state.
  bool                            notify_enable;                    ///< CDC mgmt element notifications enable.
  uint16_t                        notify_interval;                  ///< CDC mgmt element notifications interval.
  uint8_t                         data_interface_nbr;               ///< Number of data IFs.
  sli_usbd_cdc_data_interface_t   *data_interface_head_ptr;         ///< Data IFs list head ptr. (see note #1b)
  sli_usbd_cdc_data_interface_t   *data_interface_tail_ptr;         ///< Data IFs list tail ptr.
  uint8_t                         subclass_code;                    ///< CDC subclass code.
  uint8_t                         subclass_protocol;                ///< CDC subclass protocol.
  sl_usbd_cdc_subclass_driver_t   *subclass_driver_ptr;             ///< CDC subclass drv.
  void                            *subclass_arg;                    ///< CDC subclass drv argument.
  sli_usbd_cdc_comm_t             *comm_ptr;                        ///< CDC comm information ptr.
};

/*
 ********************************************************************************************************
 *                                       CDC BASE CLASS ROOT STRUCT
 *******************************************************************************************************/
/// CDC Base Class Root Structure
typedef struct {
  sli_usbd_cdc_ctrl_t ctrl_table[SL_USBD_CDC_CLASS_INSTANCE_QUANTITY];                                    ///< Ctrl struct table.
  uint8_t             ctrl_nbr_next;                                                                      ///< Next ctrl struct index.

  sli_usbd_cdc_comm_t comm_tbl[SLI_USBD_CDC_COMM_NBR_MAX];                                                ///< Comm struct table.
  uint16_t            comm_nbr_next;                                                                      ///< Next comm struct index.

  sli_usbd_cdc_data_interface_t data_interface_table[SL_USBD_CDC_DATA_INTERFACE_QUANTITY];                ///< Data interface struct table.
  uint8_t                       data_interface_nbr_next;                                                  ///< Next data interface struct index.

  sli_usbd_cdc_data_interface_endpoint_t data_interface_endpoint_table[SLI_USBD_CDC_DATA_IF_EP_NBR_MAX];  ///< Data interface endpoint struct table.
  uint16_t                               data_interface_endpoint_nbr_next;                                ///< Next data interface endpoint struct index.
  uint16_t                               data_interface_endpoint_quantity;                                ///< Quantity of data interface endpoint.
} sli_usbd_cdc_t;

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

sli_usbd_cdc_t usbd_cdc_obj;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_cdc_enable(uint8_t config_nbr,
                            void    *p_if_class_arg);

static void usbd_cdc_disable(uint8_t config_nbr,
                             void    *p_if_class_arg);

static void usbd_cdc_comm_interface_descriptor_cb(uint8_t config_nbr,
                                                  uint8_t if_nbr,
                                                  uint8_t if_alt_nbr,
                                                  void    *p_if_class_arg,
                                                  void    *p_if_alt_class_arg);

static uint16_t usbd_cdc_get_comm_interface_descriptor_size(uint8_t config_nbr,
                                                            uint8_t if_nbr,
                                                            uint8_t if_alt_nbr,
                                                            void    *p_if_class_arg,
                                                            void    *p_if_alt_class_arg);

static bool usbd_cdc_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                           void                      *p_if_class_arg);

static void usbd_cdc_notification_complete_cb(uint8_t      ep_addr,
                                              void         *p_buf,
                                              uint32_t     buf_len,
                                              uint32_t     xfer_len,
                                              void         *p_arg,
                                              sl_status_t  status);

/********************************************************************************************************
 *                                           CDC CLASS DRIVERS
 *******************************************************************************************************/
static sl_usbd_class_driver_t usbd_cdc_comm_driver = {
  usbd_cdc_enable,
  usbd_cdc_disable,
  NULL,
  NULL,
  usbd_cdc_comm_interface_descriptor_cb,
  usbd_cdc_get_comm_interface_descriptor_size,
  NULL,
  NULL,
  NULL,
  usbd_cdc_class_request_handler,
  NULL,

#if (USBD_CFG_MS_OS_DESC_EN == 1)
  NULL,
  NULL
#endif
};

static sl_usbd_class_driver_t usbd_cdc_data_driver = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,

#if (USBD_CFG_MS_OS_DESC_EN == 1)
  NULL,
  NULL
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           APPLICATION FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initializes CDC class
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_init(void)
{
  uint8_t          ix;
  sli_usbd_cdc_ctrl_t       *p_ctrl;
  sli_usbd_cdc_comm_t       *p_comm;
  sli_usbd_cdc_data_interface_t    *p_data_if;
  sli_usbd_cdc_data_interface_endpoint_t *p_data_ep;

  usbd_cdc_obj.ctrl_nbr_next = SL_USBD_CDC_CLASS_INSTANCE_QUANTITY;
  usbd_cdc_obj.comm_nbr_next = SLI_USBD_CDC_COMM_NBR_MAX;
  usbd_cdc_obj.data_interface_nbr_next = SL_USBD_CDC_DATA_INTERFACE_QUANTITY;
  usbd_cdc_obj.data_interface_endpoint_nbr_next = 0u;
  usbd_cdc_obj.data_interface_endpoint_quantity = SLI_USBD_CDC_DATA_IF_EP_NBR_MAX;

  // Init CDC ctrl tbl.
  for (ix = 0u; ix < usbd_cdc_obj.ctrl_nbr_next; ix++) {
    p_ctrl = &usbd_cdc_obj.ctrl_table[ix];
    p_ctrl->state = SLI_USBD_CDC_STATE_NONE;
    p_ctrl->notify_enable = false;
    p_ctrl->notify_interval = 0u;
    p_ctrl->data_interface_nbr = 0u;
    p_ctrl->data_interface_head_ptr = NULL;
    p_ctrl->data_interface_tail_ptr = NULL;
    p_ctrl->subclass_code = SL_USBD_CDC_SUBCLASS_RSVD;
    p_ctrl->subclass_protocol = SL_USBD_CDC_COMM_PROTOCOL_NONE;
    p_ctrl->subclass_driver_ptr = NULL;
    p_ctrl->subclass_arg = NULL;
    p_ctrl->comm_ptr = NULL;
  }

  // Init CDC comm tbl.
  for (ix = 0u; ix < usbd_cdc_obj.comm_nbr_next; ix++) {
    p_comm = &usbd_cdc_obj.comm_tbl[ix];
    p_comm->ctrl_ptr = NULL;
    p_comm->notify_in = SL_USBD_ENDPOINT_ADDR_NONE;
    p_comm->data_interface_endpoint_index = 0u;
    p_comm->cci_interface_nbr = SL_USBD_INTERFACE_NBR_NONE;
    p_comm->notify_in_active_transfer = false;
  }

  // Init CDC data IF tbl.
  for (ix = 0u; ix < usbd_cdc_obj.data_interface_nbr_next; ix++) {
    p_data_if = &usbd_cdc_obj.data_interface_table[ix];
    p_data_if->protocol = SL_USBD_CDC_DATA_PROTOCOL_NONE;
    p_data_if->isochronous_enable = false;
    p_data_if->next_ptr = NULL;
    p_data_if->interface_nbr = SL_USBD_INTERFACE_NBR_NONE;
  }

  // Init CDC data IF EP tbl.
  for (ix = 0u; ix < usbd_cdc_obj.data_interface_endpoint_nbr_next; ix++) {
    p_data_ep = &usbd_cdc_obj.data_interface_endpoint_table[ix];
    p_data_ep->data_in = SL_USBD_ENDPOINT_ADDR_NONE;
    p_data_ep->data_out = SL_USBD_ENDPOINT_ADDR_NONE;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *  Creates a new instance of the CDC class
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_create_instance(uint8_t                       subclass,
                                        sl_usbd_cdc_subclass_driver_t *p_subclass_drv,
                                        void                          *p_subclass_arg,
                                        uint8_t                       protocol,
                                        bool                          notify_en,
                                        uint16_t                      notify_interval,
                                        uint8_t                       *p_class_nbr)
{
  uint8_t    cdc_nbr;
  sli_usbd_cdc_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  if (p_class_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // interval must be a power of 2.
  if ((notify_en == true) && (SLI_USBD_IS_PWR2(notify_interval) != true)) {
    *p_class_nbr = SL_USBD_CDC_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_cdc_obj.ctrl_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_class_nbr = SL_USBD_CDC_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_cdc_obj.ctrl_nbr_next--;
  // Alloc new CDC class.
  cdc_nbr = usbd_cdc_obj.ctrl_nbr_next;
  CORE_EXIT_ATOMIC();

  // Get & init CDC struct.
  p_ctrl = &usbd_cdc_obj.ctrl_table[cdc_nbr];

  p_ctrl->subclass_code = subclass;
  p_ctrl->subclass_protocol = protocol;
  p_ctrl->notify_enable = notify_en;
  p_ctrl->notify_interval = notify_interval;
  p_ctrl->subclass_driver_ptr = p_subclass_drv;
  p_ctrl->subclass_arg = p_subclass_arg;

  *p_class_nbr = cdc_nbr;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *  Adds a CDC instance to the USB device configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr)
{
  sli_usbd_cdc_ctrl_t       *p_ctrl;
  sli_usbd_cdc_comm_t       *p_comm;
  sli_usbd_cdc_data_interface_endpoint_t *p_data_ep;
  sli_usbd_cdc_data_interface_t    *p_data_if;
  uint8_t          if_nbr;
  uint8_t          if_grb_nbr;
  uint8_t          ep_addr;
  uint16_t          comm_nbr;
  uint16_t          data_if_nbr_cur = 0u;
  uint16_t          data_if_nbr_end = 0u;
  uint16_t          data_if_ix;
  uint16_t          notify_interval;
  sl_status_t           status;
  CORE_DECLARE_IRQ_STATE;

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];

  CORE_ENTER_ATOMIC();
  if (usbd_cdc_obj.comm_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_cdc_obj.comm_nbr_next--;
  // Alloc CDC class comm info.
  comm_nbr = usbd_cdc_obj.comm_nbr_next;

  p_comm = &usbd_cdc_obj.comm_tbl[comm_nbr];

  if (p_ctrl->data_interface_nbr > 0u) {
    // Alloc data IFs EP struct.
    data_if_nbr_cur = usbd_cdc_obj.data_interface_endpoint_nbr_next;
    data_if_nbr_end = data_if_nbr_cur + p_ctrl->data_interface_nbr;
    if (data_if_nbr_end > usbd_cdc_obj.data_interface_endpoint_quantity) {
      CORE_EXIT_ATOMIC();
      return SL_STATUS_ALLOCATION_FAILED;
    }

    usbd_cdc_obj.data_interface_endpoint_nbr_next = data_if_nbr_end;
  }
  CORE_EXIT_ATOMIC();

  // Add CDC comm IF to config.
  status = sl_usbd_core_add_interface(config_nbr,
                                      &usbd_cdc_comm_driver,
                                      (void *)p_comm,
                                      NULL,
                                      SL_USBD_CLASS_CODE_CDC_CONTROL,
                                      p_ctrl->subclass_code,
                                      p_ctrl->subclass_protocol,
                                      "CDC Comm Interface",
                                      &if_nbr);

  if (status != SL_STATUS_OK) {
    return status;
  }

  p_comm->cci_interface_nbr = if_nbr;

  if (p_ctrl->notify_enable == true) {
    if (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
      // In FS, bInterval in frames.
      notify_interval = p_ctrl->notify_interval;
    } else {
      // In HS, bInterval in microframes.
      notify_interval = p_ctrl->notify_interval * 8u;
    }

    // Add interrupt (IN) EP for notifications.
    status = sl_usbd_core_add_interrupt_endpoint(config_nbr,
                                                 if_nbr,
                                                 0u,
                                                 true,
                                                 0u,
                                                 notify_interval,
                                                 &ep_addr);

    if (status != SL_STATUS_OK) {
      return status;
    }

    p_comm->notify_in = ep_addr;
  }

  if (p_ctrl->data_interface_nbr > 0u) {
    // Add CDC data IFs to config.
    p_comm->data_interface_endpoint_index = data_if_nbr_cur;
    p_data_if = p_ctrl->data_interface_head_ptr;

    for (data_if_ix = data_if_nbr_cur; data_if_ix < data_if_nbr_end; data_if_ix++) {
      p_data_ep = &usbd_cdc_obj.data_interface_endpoint_table[data_if_ix];

      // Add CDC data IF to config.
      status = sl_usbd_core_add_interface(config_nbr,
                                          &usbd_cdc_data_driver,
                                          (void *)p_comm,
                                          NULL,
                                          SL_USBD_CLASS_CODE_CDC_DATA,
                                          SL_USBD_SUBCLASS_CODE_USE_IF_DESC,
                                          p_data_if->protocol,
                                          "CDC Data Interface",
                                          &if_nbr);

      if (status != SL_STATUS_OK) {
        return status;
      }

      p_data_if->interface_nbr = if_nbr;

      if (p_data_if->isochronous_enable == false) {
        // Add Bulk IN EP.
        status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                                if_nbr,
                                                0u,
                                                true,
                                                0u,
                                                &ep_addr);

        if (status != SL_STATUS_OK) {
          return status;
        }

        p_data_ep->data_in = ep_addr;
        // Add Bulk OUT EP.
        status = sl_usbd_core_add_bulk_endpoint(config_nbr,
                                                if_nbr,
                                                0u,
                                                false,
                                                0u,
                                                &ep_addr);

        if (status != SL_STATUS_OK) {
          return status;
        }

        p_data_ep->data_out = ep_addr;
        p_data_if = p_data_if->next_ptr;
        // Group comm IF with data IFs.
        status = sl_usbd_core_add_interface_group(config_nbr,
                                                  SL_USBD_CLASS_CODE_CDC_CONTROL,
                                                  p_ctrl->subclass_code,
                                                  p_ctrl->subclass_protocol,
                                                  p_comm->cci_interface_nbr,
                                                  p_ctrl->data_interface_nbr + 1u,
                                                  "CDC Device",
                                                  &if_grb_nbr);

        if (status != SL_STATUS_OK) {
          return status;
        }
      }
    }
  }

  p_comm->ctrl_ptr = p_ctrl;

  CORE_ENTER_ATOMIC();
  p_ctrl->state = SLI_USBD_CDC_STATE_INIT;
  p_ctrl->comm_ptr = NULL;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *  Gets the CDC class enable state
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_is_enabled(uint8_t  class_nbr,
                                     bool     *p_enabled)
{
  sli_usbd_cdc_ctrl_t  *p_ctrl;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    *p_enabled = false;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];

  if (p_ctrl->comm_ptr == NULL) {
    *p_enabled = false;
    return SL_STATUS_NULL_POINTER;
  }

  status = sl_usbd_core_get_device_state(&state);

  if ((status == SL_STATUS_OK)
      && (state == SL_USBD_DEVICE_STATE_CONFIGURED)
      && (p_ctrl->state == SLI_USBD_CDC_STATE_CONFIG)) {
    *p_enabled = true;
    return SL_STATUS_OK;
  }

  *p_enabled = false;
  return status;
}

/****************************************************************************************************//**
 *  Adds a data interface class to the CDC communication interface class
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_add_data_interface(uint8_t  class_nbr,
                                           bool     isoc_en,
                                           uint8_t  protocol,
                                           uint8_t  *p_if_nbr)
{
  sli_usbd_cdc_ctrl_t    *p_ctrl;
  sli_usbd_cdc_data_interface_t *p_data_if;
  uint16_t       data_if_ix;
  uint8_t       data_if_nbr;
  CORE_DECLARE_IRQ_STATE;

  if (p_if_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    *p_if_nbr = SL_USBD_CDC_DATA_IF_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check 'isoc_en' argument (see Note #1) .
  if (isoc_en != false) {
    *p_if_nbr = SL_USBD_CDC_DATA_IF_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  if (usbd_cdc_obj.data_interface_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_if_nbr = SL_USBD_CDC_DATA_IF_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_cdc_obj.data_interface_nbr_next--;
  data_if_ix = usbd_cdc_obj.data_interface_nbr_next;
  CORE_EXIT_ATOMIC();

  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];
  p_data_if = &usbd_cdc_obj.data_interface_table[data_if_ix];
  data_if_nbr = p_ctrl->data_interface_nbr;

  if (data_if_nbr == SLI_USBD_CDC_DATA_IF_NBR_TOTAL) {
    CORE_EXIT_ATOMIC();
    *p_if_nbr = SL_USBD_CDC_DATA_IF_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Add data IF in the list.
  if (p_ctrl->data_interface_head_ptr == NULL) {
    p_ctrl->data_interface_head_ptr = p_data_if;
  } else {
    p_ctrl->data_interface_tail_ptr->next_ptr = p_data_if;
  }
  p_ctrl->data_interface_tail_ptr = p_data_if;
  p_ctrl->data_interface_nbr++;

  CORE_EXIT_ATOMIC();

  p_data_if->protocol = protocol;
  p_data_if->isochronous_enable = isoc_en;
  p_data_if->next_ptr = NULL;

  *p_if_nbr = data_if_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *  Receives data on the CDC data interface
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_read_data(uint8_t  class_nbr,
                                  uint8_t  data_if_nbr,
                                  uint8_t  *p_buf,
                                  uint32_t buf_len,
                                  uint16_t timeout,
                                  uint32_t *p_xfer_len)
{
  sli_usbd_cdc_ctrl_t       *p_ctrl;
  sli_usbd_cdc_comm_t       *p_comm;
  sli_usbd_cdc_data_interface_t    *p_data_if;
  sli_usbd_cdc_data_interface_endpoint_t *p_data_ep;
  uint32_t          xfer_len;
  uint16_t          data_if_ix;
  sl_status_t           status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];

  // Transfers are only valid in config state.
  if (p_ctrl->state != SLI_USBD_CDC_STATE_CONFIG) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Check 'data_if_nbr' is valid.
  if (data_if_nbr >= p_ctrl->data_interface_nbr) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_comm = p_ctrl->comm_ptr;
  p_data_if = p_ctrl->data_interface_head_ptr;
  // Find data IF struct.
  for (data_if_ix = 0u; data_if_ix < data_if_nbr; data_if_ix++) {
    p_data_if = p_data_if->next_ptr;
  }

  data_if_ix = p_comm->data_interface_endpoint_index + data_if_nbr;
  p_data_ep = &usbd_cdc_obj.data_interface_endpoint_table[data_if_ix];
  xfer_len = 0u;

  if (p_data_if->isochronous_enable == false) {
    status = sl_usbd_core_read_bulk_sync(p_data_ep->data_out,
                                         p_buf,
                                         buf_len,
                                         timeout,
                                         &xfer_len);
  } else {
    *p_xfer_len = 0u;
    return SL_STATUS_NOT_SUPPORTED;
  }

  *p_xfer_len = xfer_len;
  return status;
}

/****************************************************************************************************//**
 *  Sends data on the CDC data interface
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_write_data(uint8_t  class_nbr,
                                   uint8_t  data_if_nbr,
                                   uint8_t  *p_buf,
                                   uint32_t buf_len,
                                   uint16_t timeout,
                                   uint32_t *p_xfer_len)
{
  sli_usbd_cdc_ctrl_t       *p_ctrl;
  sli_usbd_cdc_comm_t       *p_comm;
  sli_usbd_cdc_data_interface_t    *p_data_if;
  sli_usbd_cdc_data_interface_endpoint_t *p_data_ep;
  uint32_t          xfer_len;
  uint16_t          data_if_ix;
  sl_status_t           status;

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];

  // Transfers are only valid in config state.
  if (p_ctrl->state != SLI_USBD_CDC_STATE_CONFIG) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }
  // Check 'data_if_nbr' is valid.
  if (data_if_nbr >= p_ctrl->data_interface_nbr) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_comm = p_ctrl->comm_ptr;
  p_data_if = p_ctrl->data_interface_head_ptr;
  // Find data IF struct.
  for (data_if_ix = 0u; data_if_ix < data_if_nbr; data_if_ix++) {
    p_data_if = p_data_if->next_ptr;
  }

  data_if_ix = p_comm->data_interface_endpoint_index + data_if_nbr;
  p_data_ep = &usbd_cdc_obj.data_interface_endpoint_table[data_if_ix];
  xfer_len = 0u;

  if (p_data_if->isochronous_enable == false) {
    status = sl_usbd_core_write_bulk_sync(p_data_ep->data_in,
                                          p_buf,
                                          buf_len,
                                          timeout,
                                          true,
                                          &xfer_len);
  } else {
    *p_xfer_len = 0u;
    return SL_STATUS_NOT_SUPPORTED;
  }

  *p_xfer_len = xfer_len;
  return status;
}

/****************************************************************************************************//**
 *  Sends a communication interface class notification to the host
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_notify_host(uint8_t   class_nbr,
                                    uint8_t   notification,
                                    uint16_t  value,
                                    uint8_t   *p_buf,
                                    uint16_t  data_len,
                                    bool      *p_result)
{
  sli_usbd_cdc_comm_t *p_comm;
  sli_usbd_cdc_ctrl_t *p_ctrl;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  if (p_result == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (class_nbr >= SL_USBD_CDC_CLASS_INSTANCE_QUANTITY) {
    *p_result = false;
    return SL_STATUS_INVALID_PARAMETER;
  }
  p_ctrl = &usbd_cdc_obj.ctrl_table[class_nbr];

  // Transfers are only valid in config state.
  if (p_ctrl->state != SLI_USBD_CDC_STATE_CONFIG) {
    *p_result = false;
    return SL_STATUS_INVALID_STATE;
  }
  p_comm = p_ctrl->comm_ptr;

  p_buf[0] = (1u)                                               // Recipient : Interface.
             | (1u << 5u)                                       // type      : Class.
             |  0x80u; // BIT_07                                // Direction : Device to host.
  p_buf[1] = notification;
  p_buf[4] = p_comm->cci_interface_nbr;
  p_buf[5] = 0u;
  memcpy(&p_buf[2], &value, 2u);
  memcpy(&p_buf[6], &data_len, 2u);

  CORE_ENTER_ATOMIC();
  // Check if another xfer is already in progress.
  if (p_comm->notify_in_active_transfer == false) {
    // Indicate that a xfer is in progress.
    p_comm->notify_in_active_transfer = true;
    CORE_EXIT_ATOMIC();
    status = sl_usbd_core_write_interrupt_async(p_comm->notify_in,
                                                p_buf,
                                                (uint32_t)data_len + SL_USBD_CDC_NOTIFICATION_HEADER,
                                                usbd_cdc_notification_complete_cb,
                                                (void *)p_comm,
                                                true);
    if (status != SL_STATUS_OK) {
      CORE_ENTER_ATOMIC();
      p_comm->notify_in_active_transfer = false;
      CORE_EXIT_ATOMIC();

      *p_result = false;
      return status;
    }
  } else {
    CORE_EXIT_ATOMIC();
    *p_result = false;
    return SL_STATUS_NOT_READY;
  }

  *p_result = true;
  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               usbd_cdc_enable()
 *
 * @brief    Notify class that configuration is active.
 *
 * @param    config_nbr      Configuration ix to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void usbd_cdc_enable(uint8_t  config_nbr,
                            void     *p_if_class_arg)
{
  sli_usbd_cdc_comm_t *p_comm;
  sli_usbd_cdc_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_cdc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  CORE_ENTER_ATOMIC();
  p_ctrl->comm_ptr = p_comm;
  p_ctrl->state = SLI_USBD_CDC_STATE_CONFIG;
  CORE_EXIT_ATOMIC();

  p_ctrl->subclass_driver_ptr->enable(p_ctrl->subclass_arg);
}

/****************************************************************************************************//**
 *                                           usbd_cdc_disable()
 *
 * @brief    Notify class that configuration is not active.
 *
 * @param    config_nbr      Configuration ix to add the interface.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *******************************************************************************************************/
static void usbd_cdc_disable(uint8_t    config_nbr,
                                void       *p_if_class_arg)
{
  sli_usbd_cdc_comm_t *p_comm;
  sli_usbd_cdc_ctrl_t *p_ctrl;
  CORE_DECLARE_IRQ_STATE;

  (void)&config_nbr;

  p_comm = (sli_usbd_cdc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  CORE_ENTER_ATOMIC();
  p_ctrl->comm_ptr = (sli_usbd_cdc_comm_t *)0;
  p_ctrl->state = SLI_USBD_CDC_STATE_INIT;
  CORE_EXIT_ATOMIC();

  p_ctrl->subclass_driver_ptr->disable(p_ctrl->subclass_arg);
}

/****************************************************************************************************//**
 *                                           usbd_cdc_class_request_handler()
 *
 * @brief    Class request handler.
 *
 * @param    p_setup_req     Pointer to setup request structure.
 *
 * @param    p_if_class_arg  Pointer to class argument specific to interface.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *
 *           false, if any errors returned.
 *******************************************************************************************************/
static bool usbd_cdc_class_request_handler(const sl_usbd_setup_req_t *p_setup_req,
                                           void                      *p_if_class_arg)
{
  sli_usbd_cdc_ctrl_t         *p_ctrl;
  sli_usbd_cdc_comm_t         *p_comm;
  sl_usbd_cdc_subclass_driver_t *p_drv;
  bool           valid;

  p_comm = (sli_usbd_cdc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;
  p_drv = p_ctrl->subclass_driver_ptr;
  valid = true;

  if (p_drv->mgmt_req != NULL) {
    // Call subclass-specific management request handler.
    valid = p_drv->mgmt_req(p_setup_req,
                            p_ctrl->subclass_arg);
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_cdc_notification_complete_cb()
 *
 * @brief    CDC notification complete callback.
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
static void usbd_cdc_notification_complete_cb(uint8_t      ep_addr,
                                              void         *p_buf,
                                              uint32_t     buf_len,
                                              uint32_t     xfer_len,
                                              void         *p_arg,
                                              sl_status_t  status)
{
  sli_usbd_cdc_ctrl_t         *p_ctrl;
  sli_usbd_cdc_comm_t         *p_comm;
  sl_usbd_cdc_subclass_driver_t *p_drv;

  (void)&ep_addr;
  (void)&p_buf;
  (void)&buf_len;
  (void)&xfer_len;
  (void)&status;

  p_comm = (sli_usbd_cdc_comm_t *)p_arg;
  p_ctrl = p_comm->ctrl_ptr;
  p_drv = p_ctrl->subclass_driver_ptr;

  // Xfer finished, no more active xfer.
  p_comm->notify_in_active_transfer = false;
  if (p_drv->notify_cmpl != NULL) {
    p_drv->notify_cmpl(p_ctrl->subclass_arg);
  }
}

/****************************************************************************************************//**
 *                                           usbd_cdc_comm_interface_descriptor_cb()
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
static void usbd_cdc_comm_interface_descriptor_cb(uint8_t    config_nbr,
                                                  uint8_t    if_nbr,
                                                  uint8_t    if_alt_nbr,
                                                  void       *p_if_class_arg,
                                                  void       *p_if_alt_class_arg)
{
  sli_usbd_cdc_ctrl_t         *p_ctrl;
  sli_usbd_cdc_comm_t         *p_comm;
  sl_usbd_cdc_subclass_driver_t *p_drv;
  sli_usbd_cdc_data_interface_t      *p_data_if;
  uint8_t            desc_size;
  uint8_t            data_if_nbr;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (sli_usbd_cdc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;
  // Build header descriptor
  sl_usbd_core_write_08b_to_descriptor_buf(SLI_USBD_CDC_DESC_SIZE_HEADER);
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_TYPE_CS_IF);
  sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_SUBTYPE_HEADER);
  // CDC release number (1.20) in BCD fmt.
  sl_usbd_core_write_16b_to_descriptor_buf(0x0120u);

  // Build union IF descriptor
  if (p_ctrl->data_interface_nbr > 0u) {
    desc_size = SLI_USBD_CDC_DESC_SIZE_UNION_MIN + p_ctrl->data_interface_nbr;

    sl_usbd_core_write_08b_to_descriptor_buf(desc_size);
    sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_TYPE_CS_IF);
    sl_usbd_core_write_08b_to_descriptor_buf(SL_USBD_CDC_DESC_SUBTYPE_UNION);
    sl_usbd_core_write_08b_to_descriptor_buf(p_comm->cci_interface_nbr);

    // Add all subordinate data IFs.
    p_data_if = p_ctrl->data_interface_head_ptr;

    for (data_if_nbr = 0u; data_if_nbr < p_ctrl->data_interface_nbr; data_if_nbr++) {
      sl_usbd_core_write_08b_to_descriptor_buf(p_data_if->interface_nbr);
      p_data_if = p_data_if->next_ptr;
    }
  }

  p_drv = p_ctrl->subclass_driver_ptr;

  if (p_drv->fnct_descriptor != NULL) {
    // Call subclass-specific functional descriptor.
    p_drv->fnct_descriptor(p_ctrl->subclass_arg,
                           p_ctrl->data_interface_head_ptr->interface_nbr);
  }
}

/****************************************************************************************************//**
 *                                       usbd_cdc_get_comm_interface_descriptor_size()
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
static uint16_t usbd_cdc_get_comm_interface_descriptor_size(uint8_t    config_nbr,
                                                            uint8_t    if_nbr,
                                                            uint8_t    if_alt_nbr,
                                                            void       *p_if_class_arg,
                                                            void       *p_if_alt_class_arg)
{
  sli_usbd_cdc_ctrl_t         *p_ctrl;
  sli_usbd_cdc_comm_t         *p_comm;
  sl_usbd_cdc_subclass_driver_t *p_drv;
  uint16_t            desc_size;

  (void)&config_nbr;
  (void)&if_nbr;
  (void)&if_alt_nbr;
  (void)&p_if_alt_class_arg;

  p_comm = (sli_usbd_cdc_comm_t *)p_if_class_arg;
  p_ctrl = p_comm->ctrl_ptr;

  desc_size = SLI_USBD_CDC_DESC_SIZE_HEADER;

  if (p_ctrl->data_interface_nbr > 0u) {
    desc_size += SLI_USBD_CDC_DESC_SIZE_UNION_MIN;
    desc_size += p_ctrl->data_interface_nbr;
  }

  p_drv = p_ctrl->subclass_driver_ptr;

  if (p_drv->fnct_get_descriptor_size != NULL) {
    desc_size += p_drv->fnct_get_descriptor_size(p_ctrl->subclass_arg);
  }

  return (desc_size);
}
