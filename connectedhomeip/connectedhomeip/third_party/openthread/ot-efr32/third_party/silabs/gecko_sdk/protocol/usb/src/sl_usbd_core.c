/***************************************************************************//**
 * @file
 * @brief USB Device Core Operations
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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_status.h"
#include "sl_string.h"

#include "em_core.h"

#include "sl_usbd_device_config.h"
#include "sl_usbd_core_config.h"
#include "sl_usbd_core.h"

#include "sli_usbd_core.h"
#include "sli_usbd_driver.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_LOG_BUS_CH                   (USBD, BUS)

/********************************************************************************************************
 *                                       OBJECTS TOTAL NUMBER DEFINES
 *******************************************************************************************************/

#define  SLI_USBD_CONFIG_NBR_TOT                      (255u - 1u)
#define  SLI_USBD_INTERFACE_NBR_TOT                   (255u - 1u)
#define  SLI_USBD_ALT_INTERFACE_NBR_TOT               (255u - 1u)
#define  SLI_USBD_INTERFACE_GROUP_NBR_TOT             (255u - 1u)
#define  SLI_USBD_ENDPOINT_NBR_TOT                    (255u - 1u)

/********************************************************************************************************
 *                                           PROTOCOL DEFINES
 *
 * Note(s) : (1) The descriptor buffer is used to send the device, configuration and string descriptors.
 *
 *               (a) The size of the descriptor buffer is set to 64 which is the maximum packet size
 *                   allowed by the USB specification for FS and HS devices.
 *
 *           (2) USB spec 2.0 (section 9.6.3), table 9-10 specify the bitmap for the configuration
 *               attributes.
 *
 *                   D7    Reserved (set to one)
 *                   D6    Self-powered
 *                   D5    Remote Wakeup
 *                   D4..0 Reserved (reset to zero)
 *******************************************************************************************************/

#define  SLI_USBD_DESC_BUF_LEN                                    64u               // See Note #1a.
#define  SLI_USBD_ENDPOINT_CTRL_ALLOC                             (0x01u | 0x02u)   // (BIT_00 | BIT_01)

#define  SLI_USBD_CONFIG_DESC_SELF_POWERED                        0x40u             // BIT_06   See Note #2.
#define  SLI_USBD_CONFIG_DESC_REMOTE_WAKEUP                       0x20u             // BIT_05
#define  SLI_USBD_CONFIG_DESC_RSVD_SET                            0x80u             // BIT_07

// Microsoft Desc Define
#define  SLI_USBD_MICROSOFT_DESC_COMPAT_ID_HDR_VER_1_0            0x0010u
#define  SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_HDR_VER_1_0       0x000Au
#define  SLI_USBD_MICROSOFT_DESC_VER_1_0                          0x0100u

#define  SLI_USBD_MICROSOFT_STR_LEN                               18u               // Length of MS OS string.
#define  SLI_USBD_MICROSOFT_STR_IX                                0xEEu             // Index  of MS OS string.

#define  SLI_USBD_MICROSOFT_DESC_COMPAT_ID_HDR_LEN                16u
#define  SLI_USBD_MICROSOFT_DESC_COMPAT_ID_SECTION_LEN            24u

#define  SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_HDR_LEN           10u
#define  SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_SECTION_HDR_LEN   8u

#define  SLI_USBD_MICROSOFT_FEATURE_COMPAT_ID                     0x0004u
#define  SLI_USBD_MICROSOFT_FEATURE_EXT_PROPERTIES                0x0005u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 *
 * Note(s) : (1) For more information, see "Extended Compat id OS Feature Descriptor Specification",
 *               Appendix A, available at http://msdn.microsoft.com/en-us/windows/hardware/gg463179.aspx.
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (USBD_CFG_MS_OS_DESC_EN == 1)
// Signature used in MS OS string desc.
static const char usbd_microsoft_signature_str[] = "MSFT100";

static const char usbd_microsoft_compat_id[][8u] = {
  { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
  { 'R', 'N', 'D', 'I', 'S', 0u, 0u, 0u },
  { 'P', 'T', 'P', 0u, 0u, 0u, 0u, 0u },
  { 'M', 'T', 'P', 0u, 0u, 0u, 0u, 0u },
  { 'X', 'U', 'S', 'B', '2', '0', 0u, 0u },
  { 'B', 'L', 'U', 'T', 'U', 'T', 'H', 0u },
  { 'W', 'I', 'N', 'U', 'S', 'B', 0u, 0u },
};

static const char usbd_microsoft_subcompat_id[][8u] = {
  { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
  { '1', '1', 0u, 0u, 0u, 0u, 0u, 0u },
  { '1', '2', 0u, 0u, 0u, 0u, 0u, 0u },
  { 'E', 'D', 'R', 0u, 0u, 0u, 0u, 0u },
};
#endif

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

static sli_usbd_t usb_device;

static uint32_t descriptor_buffer[SLI_USBD_DESC_BUF_LEN / 4u] = { 0 };
static uint32_t ctrl_status_buffer = 0;

sli_usbd_t *usbd_ptr = NULL;

#if (SLI_USBD_CFG_DBG_STATS_EN == 1)
sl_usbd_debug_device_stats_t usbd_debug_stats_device;
sl_usbd_debug_endpoint_stats_t  usbd_debug_stats_endpoint_table[32u];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static sl_status_t usbd_core_add_device(void);

// Standard Request Handlers
static void usbd_core_stdreq_handler(sli_usbd_device_t *p_dev);

static bool usbd_core_device_stdreq(sli_usbd_device_t *p_dev,
                                    uint8_t           request);

static bool usbd_core_interface_stdreq(sli_usbd_device_t *p_dev,
                                       uint8_t           request);

static bool usbd_core_endpoint_stdreq(const sli_usbd_device_t *p_dev,
                                      uint8_t                 request);

static bool usbd_core_class_stdreq(const sli_usbd_device_t *p_dev);

static bool usbd_core_vendor_stdreq(const sli_usbd_device_t *p_dev);

#if (USBD_CFG_MS_OS_DESC_EN == 1)
static bool usbd_core_microsoft_device_stdreq(const sli_usbd_device_t *p_dev);

static bool usbd_core_microsoft_interface_stdreq(const sli_usbd_device_t *p_dev);

static bool usbd_core_microsoft_ext_property_stdreq(const sli_usbd_device_t  *p_dev,
                                                    sli_usbd_configuration_t *p_config,
                                                    uint8_t                  if_nbr,
                                                    uint16_t                 len);
#endif

static bool usbd_core_get_descriptor_stdreq(sli_usbd_device_t *p_dev);

static void usbd_core_unset_configuration(sli_usbd_device_t *p_dev);

static sl_status_t usbd_core_set_configuration(sli_usbd_device_t *p_dev,
                                               uint8_t           config_nbr);

static sl_status_t usbd_core_send_device_descriptor(sli_usbd_device_t *p_dev,
                                                    bool              other,
                                                    uint16_t          req_len);

static sl_status_t usbd_core_send_configuration_descriptor(sli_usbd_device_t *p_dev,
                                                           uint8_t           config_nbr,
                                                           bool              other,
                                                           uint16_t          req_len);

#if (USBD_CFG_STR_EN == 1)
static sl_status_t usbd_core_send_string_descriptor(sli_usbd_device_t   *p_dev,
                                                    uint8_t             str_ix,
                                                    uint16_t            req_len);

static sl_status_t usbd_core_add_string(sli_usbd_device_t *p_dev,
                                        const char        *p_str);

static uint8_t usbd_core_get_string_index(const sli_usbd_device_t *p_dev,
                                          const char              *p_str);

static const char *usbd_core_get_string_descriptor(const sli_usbd_device_t *p_dev,
                                                   uint8_t                 str_nbr);
#endif

static void usbd_core_start_descriptor_write(sli_usbd_device_t *p_dev,
                                             uint16_t          req_len);

static sl_status_t usbd_core_stop_descriptor_write(sli_usbd_device_t *p_dev);

static void usbd_core_write_08b_to_descriptor_buf(sli_usbd_device_t *p_dev,
                                                  uint8_t           val);

static void usbd_core_write_16b_to_descriptor_buf(sli_usbd_device_t *p_dev,
                                                  uint16_t          val);

static void usbd_core_write_to_descriptor_buf(sli_usbd_device_t *p_dev,
                                              const uint8_t     *p_buf,
                                              uint16_t          len);

// USB Object Functions
static sli_usbd_configuration_t *usbd_core_get_configuration_structure(const sli_usbd_device_t *p_dev,
                                                                       uint8_t                 config_nbr);

static sli_usbd_interface_t *usbd_core_get_interface_structure(const sli_usbd_configuration_t *p_config,
                                                               uint8_t                        if_nbr);

static sli_usbd_alt_interface_t *usbd_core_get_alt_interface_structure(const sli_usbd_interface_t *p_if,
                                                                       uint8_t                    if_alt_nbr);

static sl_status_t usbd_core_open_alt_interface(sli_usbd_device_t              *p_dev,
                                                uint8_t                        if_nbr,
                                                const sli_usbd_alt_interface_t *p_if_alt);

static void usbd_core_close_alt_interface(sli_usbd_device_t              *p_dev,
                                          const sli_usbd_alt_interface_t *p_if_alt);

static sli_usbd_interface_group_t *usbd_core_get_interface_group_structure(const sli_usbd_configuration_t *p_config,
                                                                           uint8_t                        if_grp_nbr);

static void usbd_core_set_event(sli_usbd_event_code_t event);

static void usbd_core_process_event(sli_usbd_device_t     *p_dev,
                                    sli_usbd_event_code_t event);

static sl_status_t usbd_core_add_endpoint(uint8_t  config_nbr,
                                          uint8_t  if_nbr,
                                          uint8_t  if_alt_nbr,
                                          uint8_t  attrib,
                                          bool     dir_in,
                                          uint16_t max_pkt_len,
                                          uint8_t  interval,
                                          uint8_t  *p_ep_addr);

static sl_status_t usbd_core_allocate_endpoint(sl_usbd_device_speed_t   spd,
                                               uint8_t                  type,
                                               bool                     dir_in,
                                               uint16_t                 max_pkt_len,
                                               uint8_t                  if_alt_nbr,
                                               sli_usbd_endpoint_info_t *p_ep,
                                               uint32_t                 *p_alloc_bit_map,
                                               bool                     *p_alloc);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initializes the USB device stack
 *******************************************************************************************************/
sl_status_t sl_usbd_core_init(void)
{
  sli_usbd_t         *p_usbd;
  sli_usbd_device_t     *p_dev;
  sli_usbd_configuration_t  *p_config;
  sli_usbd_interface_t      *p_if;
  sli_usbd_alt_interface_t  *p_if_alt;
  sli_usbd_interface_group_t  *p_if_grp;
  sli_usbd_endpoint_info_t *p_ep;
  uint16_t   tbl_ix;
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  p_usbd = &usb_device;

  p_usbd->config_nbr_next = SL_USBD_CONFIGURATION_QUANTITY;
  p_usbd->interface_next = SL_USBD_INTERFACE_QUANTITY;
  p_usbd->alt_interface_nbr_next = SL_USBD_ALT_INTERFACE_QUANTITY;
#if (SL_USBD_INTERFACE_GROUP_QUANTITY > 0)
  p_usbd->interface_group_nbr_next = SL_USBD_INTERFACE_GROUP_QUANTITY;
#endif
  p_usbd->endpoint_info_nbr_next = SL_USBD_DESCRIPTOR_QUANTITY;
  p_usbd->std_req_timeout_ms = 5000u;

  CORE_ENTER_ATOMIC();
  usbd_ptr = p_usbd;
  CORE_EXIT_ATOMIC();

  // Device Initialization
  p_dev = &usbd_ptr->device;
  // Default dev addr.
  p_dev->address = 0u;
  p_dev->state = SL_USBD_DEVICE_STATE_NONE;
  p_dev->state_prev = SL_USBD_DEVICE_STATE_NONE;
  p_dev->conn_status = false;
  p_dev->speed = SL_USBD_DEVICE_SPEED_INVALID;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  // Init HS & FS cfg list
  memset(p_dev->config_fs_speed_table_ptrs, 0, sizeof(sli_usbd_configuration_t *) * SL_USBD_CONFIGURATION_QUANTITY);
#if (USBD_CFG_HS_EN == 1)
  memset(p_dev->config_hs_speed_table_ptrs, 0, sizeof(sli_usbd_configuration_t *) * SL_USBD_CONFIGURATION_QUANTITY);
#endif

#else
  // linked-list implementation.
  p_dev->config_fs_head_ptr = NULL;
  p_dev->config_fs_tail_ptr = NULL;
#if (USBD_CFG_HS_EN == 1)
  p_dev->config_hs_head_ptr = NULL;
  p_dev->config_hs_tail_ptr = NULL;
#endif
#endif

  p_dev->config_cur_ptr = NULL;
  p_dev->config_cur_nbr = SL_USBD_CONFIG_NBR_NONE;
  p_dev->config_fs_total_nbr = 0u;
#if (USBD_CFG_HS_EN == 1)
  p_dev->config_hs_total_nbr = 0u;
#endif

  memset(p_dev->endpoint_interface_table, 0, (SL_USBD_ENDPOINT_MAX_NBR * (sizeof(uint8_t))));

  // Alloc desc buf from heap.
  p_dev->desc_buf_ptr = (uint8_t *)descriptor_buffer;
  if (p_dev->desc_buf_ptr == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  memset(p_dev->desc_buf_ptr, 0, SLI_USBD_DESC_BUF_LEN);

  // Alloc ctrl status buf from heap.
  p_dev->ctrl_status_buf_ptr = (uint8_t *)&ctrl_status_buffer;
  if (p_dev->ctrl_status_buf_ptr == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  p_dev->actual_buf_ptr = p_dev->desc_buf_ptr;
  p_dev->desc_buf_index = 0u;
  p_dev->desc_buf_req_len = 0u;
  p_dev->desc_buf_max_len = SLI_USBD_DESC_BUF_LEN;
  p_dev->desc_buf_status_ptr = NULL;

#if (SL_USBD_STRING_QUANTITY > 0)
  memset(p_dev->str_descriptor_table, 0, SL_USBD_STRING_QUANTITY * sizeof(char *));
  p_dev->str_max_index = 0u;
#endif
#if (USBD_CFG_MS_OS_DESC_EN == 1)
  p_dev->str_microsoft_vendor_code = 0u;
#endif

  memset(&p_dev->setup_req, 0, sizeof(sl_usbd_setup_req_t));

  memset(&p_dev->setup_req_next, 0, sizeof(sl_usbd_setup_req_t));

  p_dev->endpoint_max_ctrl_pkt_size = 0u;
  p_dev->endpoint_max_phy_nbr = 0u;

  memset(p_dev->endpoint_interface_table, SL_USBD_INTERFACE_NBR_NONE, SL_USBD_ENDPOINT_MAX_NBR);

  p_dev->self_power = false;
  p_dev->remote_wakeup = 0u;

  SLI_USBD_DBG_STATS_DEV_RESET();

  // Configuration Table Initialization
  for (tbl_ix = 0u; tbl_ix < SL_USBD_CONFIGURATION_QUANTITY; tbl_ix++) {
    p_config = &usbd_ptr->config_table[tbl_ix];
    p_config->attrib = 0x00u;
    p_config->max_power = 0u;
    p_config->desc_len = 0u;

#if (USBD_CFG_STR_EN == 1)
    p_config->name_ptr = NULL;
#endif

// Init IF list
#if (USBD_CFG_OPTIMIZE_SPD == 1)
    // array implementation.
    memset(p_config->interface_table_ptrs, 0, sizeof(sli_usbd_interface_t *) * SL_USBD_INTERFACE_QUANTITY);

#if (SL_USBD_INTERFACE_GROUP_QUANTITY > 0u)
    memset(p_config->interface_group_table_ptrs, 0, sizeof(sli_usbd_interface_group_t*) * SL_USBD_INTERFACE_GROUP_QUANTITY);
#endif
#else
    // linked-list implementation.
    p_config->interface_head_ptr = NULL;
    p_config->interface_tail_ptr = NULL;
    p_config->interface_group_head_ptr = NULL;
    p_config->interface_group_tail_ptr = NULL;
    p_config->next_ptr = NULL;
#endif
    p_config->interface_nbr_total = 0u;
    p_config->interface_group_nbr_total = 0u;
    p_config->endpoint_alloc_map = 0x00u;
#if (USBD_CFG_HS_EN == 1)
    p_config->config_other_speed = SL_USBD_CONFIG_NBR_NONE;
#endif
  }
  // Interface Table Initialization
  for (tbl_ix = 0u; tbl_ix < SL_USBD_INTERFACE_QUANTITY; tbl_ix++) {
    p_if = &usbd_ptr->interface_table[tbl_ix];
    p_if->class_code = SL_USBD_CLASS_CODE_USE_IF_DESC;
    p_if->class_sub_code = SL_USBD_SUBCLASS_CODE_USE_IF_DESC;
    p_if->class_protocol_code = SL_USBD_PROTOCOL_CODE_USE_IF_DESC;
    p_if->class_driver_ptr = NULL;
    p_if->class_arg_ptr = NULL;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
    memset(p_if->alt_table_ptrs, 0, sizeof(sli_usbd_alt_interface_t *) * SL_USBD_ALT_INTERFACE_QUANTITY);
#else
    p_if->alt_head_ptr = NULL;
    p_if->alt_tail_ptr = NULL;
#endif
    p_if->alt_cur_ptr = NULL;
    p_if->alt_cur = SL_USBD_ALT_INTERFACE_NBR_NONE;
    p_if->alt_nbr_total = 0u;
    p_if->group_nbr = SL_USBD_INTERFACE_GROUP_NBR_NONE;
    p_if->endpoint_alloc_map = 0x00u;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
    p_if->next_ptr = NULL;
#endif
  }
  // Alternate Settings Table Initialization
  for (tbl_ix = 0u; tbl_ix < SL_USBD_ALT_INTERFACE_QUANTITY; tbl_ix++) {
    p_if_alt = &usbd_ptr->alt_interface_table[tbl_ix];
    p_if_alt->class_arg_ptr = NULL;
    p_if_alt->endpoint_alloc_map = 0x00u;
    p_if_alt->endpoint_nbr_total = 0u;
#if (USBD_CFG_STR_EN == 1)
    p_if_alt->name_ptr = NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 0)
    p_if_alt->next_ptr = NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 1)
    memset(p_if_alt->endpoint_table_ptrs, 0, SL_USBD_ENDPOINT_MAX_NBR * sizeof(sli_usbd_endpoint_info_t *));
    p_if_alt->endpoint_table_map = 0u;
#else
    p_if_alt->endpoint_head_ptr = NULL;
    p_if_alt->endpoint_tail_ptr = NULL;
#endif
  }

#if (SL_USBD_INTERFACE_GROUP_QUANTITY > 0u)
  // Interface Group Table Initialization
  for (tbl_ix = 0u; tbl_ix < SL_USBD_INTERFACE_GROUP_QUANTITY; tbl_ix++) {
    p_if_grp = &usbd_ptr->interface_group_table[tbl_ix];
    p_if_grp->class_code = SL_USBD_CLASS_CODE_USE_IF_DESC;
    p_if_grp->class_sub_code = SL_USBD_SUBCLASS_CODE_USE_IF_DESC;
    p_if_grp->class_protocol_code = SL_USBD_PROTOCOL_CODE_USE_IF_DESC;
    p_if_grp->interface_start = SL_USBD_INTERFACE_NBR_NONE;
    p_if_grp->interface_count = 0u;
#if (USBD_CFG_STR_EN == 1)
    p_if_grp->name_ptr = NULL;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 0)
    p_if_grp->next_ptr = NULL;
#endif
  }
#endif

  // Endpoint Information Table Initialization
  for (tbl_ix = 0u; tbl_ix < SL_USBD_DESCRIPTOR_QUANTITY; tbl_ix++) {
    p_ep = &usbd_ptr->endpoint_info_table[tbl_ix];
    p_ep->address = SL_USBD_ENDPOINT_NBR_NONE;
    p_ep->attrib = 0x00u;
    p_ep->interval = 0u;
    // Default sync addr is zero.
    p_ep->sync_addr = 0u;
    // Default feedback rate exponent is zero.
    p_ep->sync_refresh = 0u;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
    p_ep->next_ptr = NULL;
#endif
  }

#if (USBD_CFG_STR_EN == 1)
  usbd_ptr->str_quantity_per_device = SL_USBD_STRING_QUANTITY;
#endif

  status = sli_usbd_core_init_endpoint();
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = usbd_core_add_device();
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Starts the device stack
 *******************************************************************************************************/
sl_status_t sl_usbd_core_start_device(void)
{
  sli_usbd_device_t     *p_dev;
  bool   init;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    return SL_STATUS_INVALID_STATE;
  }

  init = false;

  // If dev not initialized, call dev drv 'Init()' function.
  if (p_dev->state == SL_USBD_DEVICE_STATE_NONE) {
    status = sli_usbd_driver_init();
    if (status != SL_STATUS_OK) {
      return status;
    }

    init = true;
  }

  status = sli_usbd_driver_start();

  if (init == true) {
    CORE_ENTER_ATOMIC();
    p_dev->state = SL_USBD_DEVICE_STATE_INIT;
    CORE_EXIT_ATOMIC();
  }

  return status;
}

/****************************************************************************************************//**
 * Stops the device stack
 *******************************************************************************************************/
sl_status_t sl_usbd_core_stop_device(void)
{
  sli_usbd_device_t     *p_dev;
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state == SL_USBD_DEVICE_STATE_NONE) {
    return SL_STATUS_INVALID_STATE;
  }

  // Close curr cfg.
  usbd_core_unset_configuration(p_dev);
  status = sli_usbd_driver_stop();

  CORE_ENTER_ATOMIC();
  // Re-init dev stack to 'INIT' state.
  p_dev->state = SL_USBD_DEVICE_STATE_INIT;
  p_dev->state_prev = SL_USBD_DEVICE_STATE_INIT;
  p_dev->conn_status = false;
  CORE_EXIT_ATOMIC();

  return status;
}

/****************************************************************************************************//**
 * Adds a configuration attribute to the device
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_configuration(uint8_t                  attrib,
                                           uint16_t                 max_pwr,
                                           sl_usbd_device_speed_t   spd,
                                           const char               *p_name,
                                           uint8_t                  *p_cfg_nbr)
{
  sli_usbd_device_t      *p_dev;
  sli_usbd_configuration_t   *p_config;
  uint8_t    config_tbl_ix;
  uint8_t    config_nbr;
  sl_usbd_device_speed_t  drv_spd;
  sl_status_t     status;
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_HS_EN == 0)
  (void)&spd;
  (void)&drv_spd;
#endif

  if (p_cfg_nbr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk max pwr (see Note #1).
  if (max_pwr > SL_USBD_MAX_BUS_PWR_LIMIT_mA) {
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return SL_STATUS_INVALID_STATE;
  }

// Chk if dev supports high spd.
#if (USBD_CFG_HS_EN == 1)
  sli_usbd_driver_get_speed(&drv_spd);
  if ((drv_spd != SL_USBD_DEVICE_SPEED_HIGH)
      && (spd == SL_USBD_DEVICE_SPEED_HIGH)) {
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

  CORE_ENTER_ATOMIC();
  // Chk if cfg is avail.
  if (usbd_ptr->config_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  config_tbl_ix = usbd_ptr->config_nbr_next - 1u;

#if (USBD_CFG_HS_EN == 1)
  // Add cfg to dev HS cfg.
  if (spd == SL_USBD_DEVICE_SPEED_HIGH) {
    config_nbr = p_dev->config_hs_total_nbr;
    // Chk cfg limit.
    if (config_nbr > SLI_USBD_CONFIG_NBR_TOT) {
      CORE_EXIT_ATOMIC();
      *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
      return SL_STATUS_ALLOCATION_FAILED;
    }
    p_dev->config_hs_total_nbr++;
  } else {
#endif
  // FS cfg.
  config_nbr = p_dev->config_fs_total_nbr;
  // Chk cfg limit.
  if (config_nbr > SLI_USBD_CONFIG_NBR_TOT) {
    CORE_EXIT_ATOMIC();
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  p_dev->config_fs_total_nbr++;
#if (USBD_CFG_HS_EN == 1)
}
#endif
  usbd_ptr->config_nbr_next--;

  // Configuration Structure Initialization
  p_config = &usbd_ptr->config_table[config_tbl_ix];

  // Link cfg into dev struct.
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  CORE_EXIT_ATOMIC();

#if (USBD_CFG_HS_EN == 1)
  if (spd == SL_USBD_DEVICE_SPEED_HIGH) {
    p_dev->config_hs_speed_table_ptrs[config_nbr] = p_config;
    // Set spd bit in cfg nbr.
    SL_SET_BIT(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_dev->config_fs_speed_table_ptrs[config_nbr] = p_config;
#if (USBD_CFG_HS_EN == 1)
}
#endif

#else
  p_config->next_ptr = NULL;

#if (USBD_CFG_HS_EN == 1)
  if (spd == SL_USBD_DEVICE_SPEED_HIGH) {
    // Link cfg in HS list.
    if (p_dev->config_hs_head_ptr == NULL) {
      p_dev->config_hs_head_ptr = p_config;
      p_dev->config_hs_tail_ptr = p_config;
    } else {
      p_dev->config_hs_tail_ptr->next_ptr = p_config;
      p_dev->config_hs_tail_ptr = p_config;
    }
    // Set spd bit in cfg nbr.
    SL_SET_BIT(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  // Link cfg in FS list.
  if (p_dev->config_fs_head_ptr == NULL) {
    p_dev->config_fs_head_ptr = p_config;
    p_dev->config_fs_tail_ptr = p_config;
  } else {
    p_dev->config_fs_tail_ptr->next_ptr = p_config;
    p_dev->config_fs_tail_ptr = p_config;
  }
#if (USBD_CFG_HS_EN == 1)
}
#endif
  CORE_EXIT_ATOMIC();
#endif

#if (USBD_CFG_STR_EN == 1)
  p_config->name_ptr = p_name;
#endif

  p_config->attrib = attrib;
  // Init EP alloc bitmap.
  p_config->endpoint_alloc_map = SLI_USBD_ENDPOINT_CTRL_ALLOC;
  p_config->max_power = max_pwr;
  // Init cfg desc len.
  p_config->desc_len = 0u;

#if (USBD_CFG_STR_EN == 1)
  // Add cfg string to dev.
  status = usbd_core_add_string(p_dev, p_name);
  if (status != SL_STATUS_OK) {
    *p_cfg_nbr = SL_USBD_CONFIG_NBR_NONE;
    return status;
  }
#else
  (void)&p_name;
#endif

  *p_cfg_nbr = config_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Associate a configuration with its alternative-speed counterpart
 *******************************************************************************************************/
#if (USBD_CFG_HS_EN == 1)
sl_status_t sl_usbd_core_associate_other_speed_configuration(uint8_t config_nbr,
                                                             uint8_t config_other)
{
  sli_usbd_device_t    *p_dev;
  sli_usbd_configuration_t *p_config;
  sli_usbd_configuration_t *p_config_other;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    return SL_STATUS_INVALID_STATE;
  }

  // Chk if both cfg are from same spd.
  if (((config_nbr & SL_USBD_CONFIG_NBR_SPD_BIT) ^ (config_other & SL_USBD_CONFIG_NBR_SPD_BIT)) != SL_USBD_CONFIG_NBR_SPD_BIT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_config_other = usbd_core_get_configuration_structure(p_dev, config_other);

  if (p_config_other == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if cfg already associated.
  if (!((p_config->config_other_speed == SL_USBD_CONFIG_NBR_NONE)
        && (p_config_other->config_other_speed == SL_USBD_CONFIG_NBR_NONE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p_config->config_other_speed = config_other;
  p_config_other->config_other_speed = config_nbr;

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Gets the current device state
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_state(sl_usbd_device_state_t *p_dev_state)
{
  sli_usbd_device_t *p_dev;

  if (p_dev_state == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  *p_dev_state = p_dev->state;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Get device speed
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_speed(sl_usbd_device_speed_t *p_dev_speed)
{
  sli_usbd_device_t *p_dev;

  if (p_dev_speed == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state == SL_USBD_DEVICE_STATE_NONE) {
    *p_dev_speed = SL_USBD_DEVICE_SPEED_INVALID;
    return SL_STATUS_INVALID_STATE;
  }

  *p_dev_speed = p_dev->speed;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sets the device's current power source
 *******************************************************************************************************/
sl_status_t sl_usbd_core_set_device_self_power(bool self_pwr)
{
  sli_usbd_device_t *p_dev;
  CORE_DECLARE_IRQ_STATE;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  CORE_ENTER_ATOMIC();
  p_dev->self_power = self_pwr;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Set the device's Microsoft vendor code
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
sl_status_t sl_usbd_core_set_device_microsoft_vendor_code(uint8_t vendor_code)
{
  sli_usbd_device_t *p_dev;
  CORE_DECLARE_IRQ_STATE;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  CORE_ENTER_ATOMIC();
  p_dev->str_microsoft_vendor_code = vendor_code;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Get device configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_configuration(sl_usbd_device_config_t **p_dev_cfg)
{
  sli_usbd_device_t *p_dev;

  if (p_dev_cfg == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  *p_dev_cfg = &p_dev->device_config;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the last frame number from the driver
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_frame_number(uint16_t *p_frame_nbr)
{
  sli_usbd_device_t     *p_dev;

  if (p_frame_nbr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state == SL_USBD_DEVICE_STATE_NONE) {
    *p_frame_nbr = 0;
    return SL_STATUS_INVALID_STATE;
  }

  sli_usbd_driver_get_frame_number(p_frame_nbr);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds an interface to a specific configuration
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interface(uint8_t                cfg_nbr,
                                       sl_usbd_class_driver_t *p_class_drv,
                                       void                   *p_if_class_arg,
                                       void                   *p_if_alt_class_arg,
                                       uint8_t                class_code,
                                       uint8_t                class_sub_code,
                                       uint8_t                class_protocol_code,
                                       const char             *p_name,
                                       uint8_t                *p_if_nbr)
{
  uint8_t  if_tbl_ix;
  uint8_t  if_nbr;
  uint8_t  if_alt_nbr;
  sli_usbd_device_t    *p_dev;
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t     *p_if;
  sli_usbd_alt_interface_t *p_if_alt;
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  if (p_if_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_class_drv == NULL) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_NULL_POINTER;
  }

  // Chk if interface_descriptor() & interface_get_descriptor_size() are present or not.
  if (!(((p_class_drv->interface_descriptor == NULL) && (p_class_drv->interface_get_descriptor_size == NULL))
        || ((p_class_drv->interface_descriptor != NULL) && (p_class_drv->interface_get_descriptor_size != NULL)))) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_NULL_POINTER;
  }

  // Chk if endpoint_descriptor() & endpoint_get_descriptor_size() are present or not.
  if (!(((p_class_drv->endpoint_descriptor == NULL) && (p_class_drv->endpoint_get_descriptor_size == NULL))
        || ((p_class_drv->endpoint_descriptor != NULL) && (p_class_drv->endpoint_get_descriptor_size != NULL)))) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_NULL_POINTER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE) && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_INVALID_STATE;
  }

  // Get cfg struct.
  p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr);

  if (p_config == NULL) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  // Chk if IF struct is avail.
  if (usbd_ptr->interface_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  if_tbl_ix = usbd_ptr->interface_next - 1u;

  // Chk if IF alt struct is avail.
  if (usbd_ptr->alt_interface_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  if_alt_nbr = usbd_ptr->alt_interface_nbr_next - 1u;

  // Get next IF nbr in cfg.
  if_nbr = p_config->interface_nbr_total;
  // Chk IF limit.
  if (if_nbr > SLI_USBD_INTERFACE_NBR_TOT) {
    CORE_EXIT_ATOMIC();
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_ptr->interface_next--;
  usbd_ptr->alt_interface_nbr_next--;
  p_config->interface_nbr_total++;

  p_if = &usbd_ptr->interface_table[if_tbl_ix];
  // Get IF alt struct (see Note #1).
  p_if_alt = &usbd_ptr->alt_interface_table[if_alt_nbr];

  // Link IF and alt setting.
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  CORE_EXIT_ATOMIC();

  p_config->interface_table_ptrs[if_nbr] = p_if;
  p_if->alt_table_ptrs[0u] = p_if_alt;
#else
  p_if->next_ptr = NULL;
  p_if_alt->next_ptr = NULL;
  p_if->alt_head_ptr = p_if_alt;
  p_if->alt_tail_ptr = p_if_alt;

  if (p_config->interface_head_ptr == NULL) {
    p_config->interface_head_ptr = p_if;
    p_config->interface_tail_ptr = p_if;
  } else {
    p_config->interface_tail_ptr->next_ptr = p_if;
    p_config->interface_tail_ptr = p_if;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_if->class_code = class_code;
  p_if->class_sub_code = class_sub_code;
  p_if->class_protocol_code = class_protocol_code;
  p_if->class_driver_ptr = p_class_drv;
  p_if->class_arg_ptr = p_if_class_arg;
  p_if->endpoint_alloc_map = SLI_USBD_ENDPOINT_CTRL_ALLOC;
  // Set curr alt setting.
  p_if->alt_cur_ptr = p_if_alt;
  p_if->alt_cur = 0u;
  p_if->alt_nbr_total = 1u;
  p_if_alt->endpoint_alloc_map = p_if->endpoint_alloc_map;
  p_if_alt->class_arg_ptr = p_if_alt_class_arg;

#if (USBD_CFG_STR_EN == 1)
  p_if_alt->name_ptr = p_name;
#endif

  // Add IF string to dev.
#if (USBD_CFG_STR_EN == 1)
  status = usbd_core_add_string(p_dev, p_name);
  if (status != SL_STATUS_OK) {
    *p_if_nbr = SL_USBD_INTERFACE_NBR_NONE;
    return status;
  }
#else
  (void)&p_name;
#endif

  *p_if_nbr = if_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Adds an alternate setting to a specific interface
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_alt_interface(uint8_t     config_nbr,
                                           uint8_t     if_nbr,
                                           void        *p_class_arg,
                                           const char  *p_name,
                                           uint8_t     *p_if_alt_nbr)
{
  sli_usbd_device_t    *p_dev;
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t     *p_if;
  sli_usbd_alt_interface_t *p_if_alt;
  uint8_t  if_alt_tbl_ix;
  uint8_t  if_alt_nbr;
  sl_status_t   status;
  CORE_DECLARE_IRQ_STATE;

  if (p_if_alt_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Get Object References

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Get config struct.
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    *p_if_alt_nbr = SL_USBD_ALT_INTERFACE_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get config struct.
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);

  if (p_if == NULL) {
    *p_if_alt_nbr = SL_USBD_ALT_INTERFACE_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  // Chk if next alt setting is avail.
  if (usbd_ptr->alt_interface_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_if_alt_nbr = SL_USBD_ALT_INTERFACE_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  if_alt_tbl_ix = usbd_ptr->alt_interface_nbr_next - 1u;

  // Chk if alt setting is avail.
  if_alt_nbr = p_if->alt_nbr_total;
  if (if_alt_nbr > SLI_USBD_ALT_INTERFACE_NBR_TOT) {
    CORE_EXIT_ATOMIC();
    *p_if_alt_nbr = SL_USBD_ALT_INTERFACE_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  usbd_ptr->alt_interface_nbr_next--;
  p_if->alt_nbr_total++;

  p_if_alt = &usbd_ptr->alt_interface_table[if_alt_tbl_ix];

  // Add alt setting to IF.
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  CORE_EXIT_ATOMIC();

  p_if->alt_table_ptrs[if_alt_nbr] = p_if_alt;
#else
  p_if_alt->next_ptr = NULL;

  p_if->alt_tail_ptr->next_ptr = p_if_alt;
  p_if->alt_tail_ptr = p_if_alt;
  CORE_EXIT_ATOMIC();
#endif

  p_if_alt->class_arg_ptr = p_class_arg;
  p_if_alt->endpoint_alloc_map = SLI_USBD_ENDPOINT_CTRL_ALLOC;

#if (USBD_CFG_STR_EN == 1)
  p_if_alt->name_ptr = p_name;
#endif

  p_if_alt->endpoint_alloc_map &= ~p_if->endpoint_alloc_map;
  p_if_alt->endpoint_alloc_map |= SLI_USBD_ENDPOINT_CTRL_ALLOC;

#if (USBD_CFG_STR_EN == 1)
  // Add alt setting string to dev.
  status = usbd_core_add_string(p_dev, p_name);
  if (status != SL_STATUS_OK) {
    *p_if_alt_nbr = SL_USBD_ALT_INTERFACE_NBR_NONE;
    return status;
  }
#else
  (void)&p_name;
#endif

  *p_if_alt_nbr = if_alt_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Creates an interface group
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interface_group(uint8_t      config_nbr,
                                             uint8_t      class_code,
                                             uint8_t      class_sub_code,
                                             uint8_t      class_protocol_code,
                                             uint8_t      if_start,
                                             uint8_t      if_cnt,
                                             const char   *p_name,
                                             uint8_t      *p_if_grp_num)
{
  sli_usbd_device_t    *p_dev;
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t     *p_if;
  sli_usbd_interface_group_t *p_if_grp;
  uint8_t  if_grp_tbl_ix;
  uint8_t  if_grp_nbr;
  uint8_t  if_nbr;
  uint8_t  if_end;
  sl_status_t   status;
  CORE_DECLARE_IRQ_STATE;

  if (p_if_grp_num == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (((uint16_t)(if_start) + (uint16_t)(if_cnt)) > (uint16_t)SLI_USBD_INTERFACE_NBR_TOT) {
    *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get Object References

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Get cfg struct.
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Verify that IFs do NOT belong to another group.
  for (if_nbr = 0u; if_nbr < if_cnt; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr + if_start);

    if (p_if == NULL) {
      *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
      return SL_STATUS_INVALID_PARAMETER;
    }

    if (p_if->group_nbr != SL_USBD_INTERFACE_GROUP_NBR_NONE) {
      *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
      return SL_STATUS_ALREADY_EXISTS;
    }
  }

  CORE_ENTER_ATOMIC();

  // Chk if IF grp is avail.
  if (usbd_ptr->interface_group_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  usbd_ptr->interface_group_nbr_next--;
  if_grp_tbl_ix = usbd_ptr->interface_group_nbr_next;

  p_if_grp = &usbd_ptr->interface_group_table[if_grp_tbl_ix];

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  if_grp_nbr = p_config->interface_group_nbr_total;
  p_config->interface_group_nbr_total++;
  CORE_EXIT_ATOMIC();

  p_config->interface_group_table_ptrs[if_grp_nbr] = p_if_grp;
#else
  p_if_grp->next_ptr = NULL;

  if_grp_nbr = p_config->interface_group_nbr_total;
  p_config->interface_group_nbr_total++;

  if (if_grp_nbr == 0u) {
    p_config->interface_group_head_ptr = p_if_grp;
    p_config->interface_group_tail_ptr = p_if_grp;
  } else {
    p_config->interface_group_tail_ptr->next_ptr = p_if_grp;
    p_config->interface_group_tail_ptr = p_if_grp;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_if_grp->class_code = class_code;
  p_if_grp->class_sub_code = class_sub_code;
  p_if_grp->class_protocol_code = class_protocol_code;
  p_if_grp->interface_start = if_start;
  p_if_grp->interface_count = if_cnt;

#if (USBD_CFG_STR_EN == 1)
  p_if_grp->name_ptr = p_name;
#endif

  if_end = if_cnt + if_start;
  for (if_nbr = if_start; if_nbr < if_end; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);

    if (p_if == NULL) {
      *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
      return SL_STATUS_INVALID_PARAMETER;
    }

    if (p_if->group_nbr != SL_USBD_INTERFACE_GROUP_NBR_NONE) {
      *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
      return SL_STATUS_ALREADY_EXISTS;
    }

    CORE_ENTER_ATOMIC();
    p_if->group_nbr = if_grp_nbr;
    CORE_EXIT_ATOMIC();
  }

#if (USBD_CFG_STR_EN == 1)
  // Add IF grp string to dev.
  status = usbd_core_add_string(p_dev, p_name);
  if (status != SL_STATUS_OK) {
    *p_if_grp_num = SL_USBD_INTERFACE_GROUP_NBR_NONE;
    return status;
  }
#else
  (void)&p_name;
#endif

  *p_if_grp_num = if_grp_nbr;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Gets the device descriptor
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_descriptor(uint8_t *p_buf,
                                               uint8_t max_len,
                                               uint8_t *p_desc_len)
{
  sli_usbd_device_t   *p_dev;
  sl_status_t   status;
  sl_status_t   local_status;

  local_status = SL_STATUS_OK;

  if (p_desc_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_buf == NULL) {
    *p_desc_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (max_len == 0) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state != SL_USBD_DEVICE_STATE_NONE) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  p_dev->actual_buf_ptr = p_buf;
  p_dev->desc_buf_max_len = max_len;
  p_dev->desc_buf_status_ptr = &local_status;

  status = usbd_core_send_device_descriptor(p_dev, false, max_len);

  p_dev->desc_buf_status_ptr = NULL;

  *p_desc_len = p_dev->desc_buf_index;
  return status;
}

/****************************************************************************************************//**
 * Gets a configuration descriptor
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_configuration_descriptor(uint8_t  *p_buf,
                                                      uint16_t max_len,
                                                      uint8_t  config_ix,
                                                      uint16_t *p_desc_len)
{
  sli_usbd_device_t   *p_dev;
  sl_status_t   local_status;
  sl_status_t   status;

  local_status = SL_STATUS_OK;

  if (p_desc_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_buf == NULL) {
    *p_desc_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (max_len == 0) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state != SL_USBD_DEVICE_STATE_NONE) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  p_dev->actual_buf_ptr = p_buf;
  p_dev->desc_buf_max_len = max_len;
  p_dev->desc_buf_status_ptr = &local_status;

  status = usbd_core_send_configuration_descriptor(p_dev, config_ix, false, max_len);

  p_dev->desc_buf_status_ptr = NULL;

  *p_desc_len = p_dev->desc_buf_index;
  return status;
}

/****************************************************************************************************//**
 * Get a string descriptor
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_get_string_descriptor(uint8_t *p_buf,
                                               uint8_t max_len,
                                               uint8_t str_ix,
                                               uint8_t *p_desc_len)
{
  sli_usbd_device_t   *p_dev;
  sl_status_t   local_status;
  sl_status_t   status;

  local_status = SL_STATUS_OK;

  if (p_desc_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (p_buf == NULL) {
    *p_desc_len = 0;
    return SL_STATUS_NULL_POINTER;
  }

  if (max_len == 0) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->state != SL_USBD_DEVICE_STATE_NONE) {
    *p_desc_len = 0;
    return SL_STATUS_INVALID_STATE;
  }

  p_dev->actual_buf_ptr = p_buf;
  p_dev->desc_buf_max_len = max_len;
  p_dev->desc_buf_status_ptr = &local_status;

  status = usbd_core_send_string_descriptor(p_dev, str_ix, max_len);

  p_dev->desc_buf_status_ptr = NULL;

  *p_desc_len = p_dev->desc_buf_index;
  return status;
}
#endif

/****************************************************************************************************//**
 * Add string to USB device
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_add_string(const char *p_str)
{
  sli_usbd_device_t  *p_dev;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    return SL_STATUS_INVALID_STATE;
  }

  return usbd_core_add_string(p_dev, p_str);
}
#endif

/****************************************************************************************************//**
 * Get string index corresponding to a given string
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_get_string_index(const char  *p_str,
                                          uint8_t     *p_str_ix)
{
  sli_usbd_device_t   *p_dev;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  *p_str_ix = usbd_core_get_string_index(p_dev, p_str);

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Writes an 8-bit value to the descriptor buffer
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_08b_to_descriptor_buf(uint8_t val)
{
  sli_usbd_device_t  *p_dev;
  sl_status_t  status;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  status = *(p_dev->desc_buf_status_ptr);

  if (status == SL_STATUS_OK) {
    usbd_core_write_to_descriptor_buf(p_dev, &val, 1u);
  }

  return status;
}

/****************************************************************************************************//**
 * Writes a 16-bit value in the descriptor buffer
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_16b_to_descriptor_buf(uint16_t val)
{
  sli_usbd_device_t *p_dev;
  sl_status_t  status;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  status = *(p_dev->desc_buf_status_ptr);

  if (status == SL_STATUS_OK) {
    uint8_t buf[2u];

    buf[0u] = (uint8_t)(val        & 0xFFu);
    buf[1u] = (uint8_t)((val >> 8u) & 0xFFu);

    usbd_core_write_to_descriptor_buf(p_dev, &buf[0u], 2u);
  }

  return status;
}

/****************************************************************************************************//**
 * Writes a 24-bit value to the descriptor buffer
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_24b_to_descriptor_buf(uint32_t val)
{
  sli_usbd_device_t *p_dev;
  sl_status_t  status;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  status = *(p_dev->desc_buf_status_ptr);

  if (status == SL_STATUS_OK) {
    uint8_t buf[3u];

    buf[0u] = (uint8_t)(val         & 0xFFu);
    buf[1u] = (uint8_t)((val >> 8u)  & 0xFFu);
    buf[2u] = (uint8_t)((val >> 16u) & 0xFFu);

    usbd_core_write_to_descriptor_buf(p_dev, &buf[0u], 3u);
  }

  return status;
}

/****************************************************************************************************//**
 * Writes a 24-bit value to the descriptor buffer
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_32b_to_descriptor_buf(uint32_t val)
{
  sli_usbd_device_t *p_dev;
  sl_status_t  status;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  status = *(p_dev->desc_buf_status_ptr);

  if (status == SL_STATUS_OK) {
    uint8_t buf[4u];

    buf[0u] = (uint8_t)(val         & 0xFFu);
    buf[1u] = (uint8_t)((val >> 8u) & 0xFFu);
    buf[2u] = (uint8_t)((val >> 16u) & 0xFFu);
    buf[3u] = (uint8_t)((val >> 24u) & 0xFFu);

    usbd_core_write_to_descriptor_buf(p_dev, &buf[0u], 4u);
  }

  return status;
}

/****************************************************************************************************//**
 * Writes a buffer into the descriptor buffer
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_buf_to_descriptor_buf(const uint8_t  *p_buf,
                                                     uint16_t       len)
{
  sli_usbd_device_t *p_dev;
  sl_status_t  status;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if ((p_buf == NULL) || (len == 0u)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = *(p_dev->desc_buf_status_ptr);

  if (status == SL_STATUS_OK) {
    usbd_core_write_to_descriptor_buf(p_dev, p_buf, len);
  }

  return status;
}

/****************************************************************************************************//**
 * Adds a bulk endpoint to alternate setting interface
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_bulk_endpoint(uint8_t   config_nbr,
                                           uint8_t   if_nbr,
                                           uint8_t   if_alt_nbr,
                                           bool      dir_in,
                                           uint16_t  max_pkt_len,
                                           uint8_t   *p_ep_addr)
{
  if (p_ep_addr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

#if (USBD_CFG_HS_EN == 1)
  // Chk EP size.
  // SL_USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  if (((max_pkt_len != 0u) && (max_pkt_len != 512u))
      && (SL_IS_BIT_SET(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT))) {
    *p_ep_addr = SL_USBD_ENDPOINT_ADDR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

#if (USBD_CFG_HS_EN == 1)
  if (((max_pkt_len != 0u)
       && (max_pkt_len != 8u)
       && (max_pkt_len != 16u)
       && (max_pkt_len != 32u)
       && (max_pkt_len != 64u))
      && (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true)) {
#else
  if ( (max_pkt_len != 0u)
       && (max_pkt_len != 8u)
       && (max_pkt_len != 16u)
       && (max_pkt_len != 32u)
       && (max_pkt_len != 64u)) {
#endif
    *p_ep_addr = SL_USBD_ENDPOINT_ADDR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  return usbd_core_add_endpoint(config_nbr,
                                if_nbr,
                                if_alt_nbr,
                                SL_USBD_ENDPOINT_TYPE_BULK,
                                dir_in,
                                max_pkt_len,
                                0u,
                                p_ep_addr);
}

/****************************************************************************************************//**
 * Adds an interrupt endpoint to an alternate setting interface
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interrupt_endpoint(uint8_t    config_nbr,
                                                uint8_t    if_nbr,
                                                uint8_t    if_alt_nbr,
                                                bool       dir_in,
                                                uint16_t   max_pkt_len,
                                                uint16_t   interval,
                                                uint8_t    *p_ep_addr)
{
  uint8_t interval_code;

  if (p_ep_addr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (interval == 0u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

#if (USBD_CFG_HS_EN == 1)
  // Full spd validation.
  // SL_USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  if (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
#endif
  if (max_pkt_len > 64u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (interval < 255u) {
    interval_code = (uint8_t)interval;
  } else {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
#if (USBD_CFG_HS_EN == 1)
} else {
  // High spd validation.
  if (((if_alt_nbr == 0u) && (max_pkt_len > 64u)) || (max_pkt_len > 1024u)) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // See Note #2.
  if (interval > SL_USBD_ENDPOINT_MAX_INTERVAL_VAL) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // interval must be a power of 2.
  if (SLI_USBD_IS_PWR2(interval) == false) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Compute bInterval exponent in 2^(bInterval-1).
  interval_code = (uint8_t)(32u - __CLZ(interval));

  if (interval_code > 16u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
}
#endif

  return usbd_core_add_endpoint(config_nbr,
                                if_nbr,
                                if_alt_nbr,
                                SL_USBD_ENDPOINT_TYPE_INTR,
                                dir_in,
                                max_pkt_len,
                                interval_code,
                                p_ep_addr);
}

/****************************************************************************************************//**
 * Adds an isochronous endpoint to alternate setting interface
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_add_isochronous_endpoint(uint8_t    config_nbr,
                                                  uint8_t    if_nbr,
                                                  uint8_t    if_alt_nbr,
                                                  bool       dir_in,
                                                  uint8_t    attrib,
                                                  uint16_t   max_pkt_len,
                                                  uint8_t    transaction_frame,
                                                  uint16_t   interval,
                                                  uint8_t    *p_ep_addr)
{
  uint16_t pkt_len;
  uint8_t interval_code;

  if (p_ep_addr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Chk if dflt IF setting with isoc EP max_pkt_len > 0.
  if ((if_alt_nbr == 0u) && (max_pkt_len > 0u)) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if sync & usage bits are used.
  if ((attrib & (uint8_t)(~(SL_USBD_ENDPOINT_TYPE_SYNC_MASK | SL_USBD_ENDPOINT_TYPE_USAGE_MASK))) != 0u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

#if (USBD_CFG_HS_EN == 1)
  // Full spd validation.
  // SL_USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  if (SL_IS_BIT_CLEAR(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
#endif
  if (max_pkt_len > 1023u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (transaction_frame != 1u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }
#if (USBD_CFG_HS_EN == 1)
} else {
  // High spd validation.
  switch (transaction_frame) {
    case 1u:
      if (max_pkt_len > 1024u) {
        *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case 2u:
      if ((max_pkt_len < 513u) || (max_pkt_len > 1024u)) {
        *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case 3u:
      if ((max_pkt_len < 683u) || (max_pkt_len > 1024u)) {
        *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    default:
      *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
      return SL_STATUS_INVALID_PARAMETER;
  }
}
#endif

  // Explicit feedback EP must be set to no sync.
  if (((attrib & SL_USBD_ENDPOINT_TYPE_USAGE_MASK) == SL_USBD_ENDPOINT_TYPE_USAGE_FEEDBACK)
      && ((attrib & SL_USBD_ENDPOINT_TYPE_SYNC_MASK) != SL_USBD_ENDPOINT_TYPE_SYNC_NONE)) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // See Note #3.
  if (interval > SL_USBD_ENDPOINT_MAX_INTERVAL_VAL) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // interval must be a power of 2.
  if (SLI_USBD_IS_PWR2(interval) == false) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Compute bInterval exponent in 2^(bInterval-1).
  interval_code = (uint8_t)(32u - __CLZ(interval));

  if (interval_code > 16u) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  pkt_len = (transaction_frame - 1u) << 11u | max_pkt_len;

  return usbd_core_add_endpoint(config_nbr,
                                if_nbr,
                                if_alt_nbr,
                                SL_USBD_ENDPOINT_TYPE_ISOC | attrib,
                                dir_in,
                                pkt_len,
                                interval_code,
                                p_ep_addr);
}
#endif

/****************************************************************************************************//**
 * Set synchronization feedback rate on synchronization isochronous endpoint
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_set_isochronous_endpoint_refresh_rate(uint8_t config_nbr,
                                                               uint8_t if_nbr,
                                                               uint8_t if_alt_nbr,
                                                               uint8_t synch_ep_addr,
                                                               uint8_t sync_refresh)
{
  sli_usbd_device_t     *p_dev;
  sli_usbd_configuration_t  *p_config;
  sli_usbd_interface_t      *p_if;
  sli_usbd_alt_interface_t  *p_if_alt;
  sli_usbd_endpoint_info_t *p_ep;
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  uint32_t ep_alloc_map;
#endif
  uint8_t  ep_nbr;
  bool found;
  CORE_DECLARE_IRQ_STATE;

  // See Note #3.
  if ((sync_refresh < 1u) || (sync_refresh > 9u)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get Object References

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get cfg struct.
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF struct.
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);

  if (p_if == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if audio class.
  if (p_if->class_code != SL_USBD_CLASS_CODE_AUDIO) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if audio class, version 1.0.
  if (p_if->class_protocol_code != 0u) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF alt setting struct.
  p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);

  if (p_if_alt == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  found = false;
  p_ep = NULL;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  ep_alloc_map = p_if_alt->endpoint_table_map;
  while ((ep_alloc_map != 0x00u)
         && (found != true)) {
    ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
    p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];

    if (p_ep->address == synch_ep_addr) {
      found = true;
    }

    SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
  }
#else
  p_ep = p_if_alt->endpoint_head_ptr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
    if (p_ep->address == synch_ep_addr) {
      found = true;
      break;
    }

    p_ep = p_ep->next_ptr;
  }
#endif

  if (found != true) {
    return SL_STATUS_INVALID_HANDLE;
  }
  // Chk EP type attrib.
  if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_ISOC) {
    return SL_STATUS_INVALID_HANDLE;
  }

  // Chk EP sync type attrib.
  switch (p_ep->attrib & SL_USBD_ENDPOINT_TYPE_SYNC_MASK) {
    case SL_USBD_ENDPOINT_TYPE_SYNC_NONE:
      break;

    case SL_USBD_ENDPOINT_TYPE_SYNC_ASYNC:
    case SL_USBD_ENDPOINT_TYPE_SYNC_ADAPTIVE:
    case SL_USBD_ENDPOINT_TYPE_SYNC_SYNC:
    default:
      return SL_STATUS_INVALID_HANDLE;
  }

  // Chk EP usage type attrib.
  switch (p_ep->attrib & SL_USBD_ENDPOINT_TYPE_USAGE_MASK) {
    case SL_USBD_ENDPOINT_TYPE_USAGE_FEEDBACK:
      // See Note #4.
      break;

    case SL_USBD_ENDPOINT_TYPE_USAGE_DATA:
    case SL_USBD_ENDPOINT_TYPE_USAGE_IMPLICIT_FEEDBACK:
    default:
      return SL_STATUS_INVALID_HANDLE;
  }

  // Chk associated sync EP addr.
  if (p_ep->sync_addr != 0u) {
    return SL_STATUS_INVALID_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  p_ep->sync_refresh = sync_refresh;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Associates synchronization endpoint to isochronous endpoint
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_set_isochronous_endpoint_sync_address(uint8_t config_nbr,
                                                               uint8_t if_nbr,
                                                               uint8_t if_alt_nbr,
                                                               uint8_t data_ep_addr,
                                                               uint8_t sync_addr)
{
  sli_usbd_device_t     *p_dev;
  sli_usbd_configuration_t  *p_config;
  sli_usbd_interface_t      *p_if;
  sli_usbd_alt_interface_t  *p_if_alt;
  sli_usbd_endpoint_info_t *p_ep;
  sli_usbd_endpoint_info_t *p_ep_isoc;
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  uint32_t ep_alloc_map;
#endif
  uint8_t  ep_nbr;
  bool found_ep;
  bool found_sync;
  CORE_DECLARE_IRQ_STATE;

  // Get Object References

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Chk curr dev state.
  if ((p_dev->state != SL_USBD_DEVICE_STATE_NONE)
      && (p_dev->state != SL_USBD_DEVICE_STATE_INIT)) {
    return SL_STATUS_INVALID_STATE;
  }

  // Get cfg struct.
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF struct.
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);

  if (p_if == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if audio class.
  if (p_if->class_code != SL_USBD_CLASS_CODE_AUDIO) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Chk if audio class, version 1.0.
  if (p_if->class_protocol_code != 0u) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF alt setting struct.
  p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);

  if (p_if_alt == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  found_ep = false;
  found_sync = false;
  p_ep_isoc = NULL;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  ep_alloc_map = p_if_alt->endpoint_table_map;
  while ((ep_alloc_map != 0x00u)
         && ((found_ep != true)
             || (found_sync != true))) {
    ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
    p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];

    if (p_ep->address == data_ep_addr) {
      found_ep = true;
      p_ep_isoc = p_ep;
    }

    if (p_ep->address == sync_addr) {
      found_sync = true;
    }

    SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
  }
#else
  p_ep = p_if_alt->endpoint_head_ptr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
    if (p_ep->address == data_ep_addr) {
      found_ep = true;
      p_ep_isoc = p_ep;
    }

    if (p_ep->address == sync_addr) {
      found_sync = true;
    }

    if ((found_ep == true)
        && (found_sync == true)) {
      break;
    }

    p_ep = p_ep->next_ptr;
  }
#endif

  if (found_ep != true) {
    return SL_STATUS_INVALID_HANDLE;
  }

  if (found_sync != true) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Chk EP type attrib.
  if ((p_ep_isoc->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_ISOC) {
    return SL_STATUS_INVALID_HANDLE;
  }

  // Chk EP sync type attrib.
  switch (p_ep_isoc->attrib & SL_USBD_ENDPOINT_TYPE_SYNC_MASK) {
    case SL_USBD_ENDPOINT_TYPE_SYNC_ASYNC:
      if (SL_USBD_ENDPOINT_IS_IN(p_ep_isoc->address) == true) {
        return SL_STATUS_INVALID_HANDLE;
      }
      break;

    case SL_USBD_ENDPOINT_TYPE_SYNC_ADAPTIVE:
      if (SL_USBD_ENDPOINT_IS_IN(p_ep_isoc->address) == false) {
        return SL_STATUS_INVALID_HANDLE;
      }
      break;

    case SL_USBD_ENDPOINT_TYPE_SYNC_NONE:
    case SL_USBD_ENDPOINT_TYPE_SYNC_SYNC:
    default:
      return SL_STATUS_INVALID_HANDLE;
  }

  // Chk EP usage type attrib.
  switch (p_ep_isoc->attrib & SL_USBD_ENDPOINT_TYPE_USAGE_MASK) {
    case SL_USBD_ENDPOINT_TYPE_USAGE_DATA:
      break;

    case SL_USBD_ENDPOINT_TYPE_USAGE_FEEDBACK:
    case SL_USBD_ENDPOINT_TYPE_USAGE_IMPLICIT_FEEDBACK:
    default:
      return SL_STATUS_INVALID_HANDLE;
  }

  // Refresh interval must be set to zero.
  if (p_ep_isoc->sync_refresh != 0u) {
    return SL_STATUS_INVALID_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  p_ep_isoc->sync_addr = sync_addr;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 * Gets the maximum physical endpoint number
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_max_phy_endpoint_number(uint8_t *p_ep_phy_nbr)
{
  sli_usbd_device_t *p_dev;

  if (p_ep_phy_nbr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  if (p_dev->endpoint_max_phy_nbr == 0u) {
    *p_ep_phy_nbr = SL_USBD_ENDPOINT_PHY_NONE;
  } else {
    *p_ep_phy_nbr = p_dev->endpoint_max_phy_nbr - 1u;
  }

  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Notifies the USB connection bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_connect_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_CONNECT);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notifies the USB disconnection bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_disconnect_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_DISCONNECT);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notifies the USB High-Speed bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_high_speed_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_HS);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notifies the USB reset bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_reset_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_RESET);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notifies the USB suspend bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_suspend_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_SUSPEND);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notifies the USB resume bus events to the device stack
 *******************************************************************************************************/
sl_status_t sli_usbd_core_resume_event(void)
{
  usbd_core_set_event(SLI_USBD_EVENT_BUS_RESUME);
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sends a USB setup event to the core task
 *******************************************************************************************************/
sl_status_t sli_usbd_core_setup_event(void *p_buf)
{
  sli_usbd_device_t *p_dev;
  sli_usbd_core_event_t core_event;
  uint8_t *p_buf_08;

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  SLI_USBD_LOG_VRB(("USBD: Setup Pkt"));

  p_buf_08 = (uint8_t *)p_buf;
  p_dev->setup_req_next.bmRequestType = p_buf_08[0u];
  p_dev->setup_req_next.bRequest = p_buf_08[1u];
  memcpy(&p_dev->setup_req_next.wValue, (p_buf_08 + 2u), 2u);
  memcpy(&p_dev->setup_req_next.wIndex, (p_buf_08 + 4u), 2u);
  memcpy(&p_dev->setup_req_next.wLength, (p_buf_08 + 6u), 2u);

  core_event.type = SLI_USBD_EVENT_SETUP;

  sli_usbd_core_os_put_core_event(&core_event);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Sends a USB endpoint event to the core task
 *******************************************************************************************************/
sl_status_t sli_usbd_core_endpoint_event(uint8_t      ep_addr,
                                         sl_status_t  status)
{
  sli_usbd_core_event_t core_event;

  core_event.type = SLI_USBD_EVENT_ENDPOINT;
  core_event.endpoint_address = ep_addr;
  core_event.status = status;

  // Queue core event.
  sli_usbd_core_os_put_core_event(&core_event);

  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               usbd_core_add_device()
 *
 * @brief    Adds a device to the stack and creates the default control endpoints.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static sl_status_t usbd_core_add_device(void)
{
  sli_usbd_device_t               *p_dev;
  uint8_t                         ep_phy_nbr;
  uint32_t                        ep_alloc_map;
  sli_usbd_endpoint_info_t        ep_info;
  bool                            alloc;
  sl_usbd_device_speed_t          drv_spd;
  sl_status_t                     status;

  sli_usbd_driver_get_speed(&drv_spd);

  // Initialize Device Structure
  p_dev = &usbd_ptr->device;
  p_dev->speed = drv_spd;

  // device configuration parameters from sl_usbd_device_config.h
  p_dev->device_config.product_id          = SL_USBD_DEVICE_PRODUCT_ID;
  p_dev->device_config.vendor_id           = SL_USBD_DEVICE_VENDOR_ID;
  p_dev->device_config.device_bcd          = SL_USBD_DEVICE_RELEASE_NUMBER;
  p_dev->device_config.manufacturer_str_ptr = SL_USBD_DEVICE_MANUFACTURER_STRING;
  p_dev->device_config.product_str_ptr      = SL_USBD_DEVICE_PRODUCT_STRING;
  p_dev->device_config.serial_nbr_str_ptr    = SL_USBD_DEVICE_SERIAL_NUMBER_STRING;
  p_dev->device_config.lang_id             = SL_USBD_DEVICE_LANGUAGE_ID;

  ep_alloc_map = 0x00u;

  // Alloc physical EP for ctrl OUT.
  usbd_core_allocate_endpoint(drv_spd,
                              SL_USBD_ENDPOINT_TYPE_CTRL,
                              false,
                              0u,
                              0u,
                              &ep_info,
                              &ep_alloc_map,
                              &alloc);

  if (alloc != true) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_info.address);
  ep_phy_nbr++;

  if (p_dev->endpoint_max_phy_nbr < ep_phy_nbr) {
    p_dev->endpoint_max_phy_nbr = ep_phy_nbr;
  }

  // Alloc physical EP for ctrl IN.
  usbd_core_allocate_endpoint(drv_spd,
                              SL_USBD_ENDPOINT_TYPE_CTRL,
                              true,
                              0u,
                              0u,
                              &ep_info,
                              &ep_alloc_map,
                              &alloc);

  if (alloc != true) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  p_dev->endpoint_max_ctrl_pkt_size = ep_info.max_pkt_size;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_info.address);
  ep_phy_nbr++;

  if (p_dev->endpoint_max_phy_nbr < ep_phy_nbr) {
    p_dev->endpoint_max_phy_nbr = ep_phy_nbr;
  }

#if (USBD_CFG_STR_EN == 1)
  // Add device configuration strings:

  // Manufacturer string.
  status = usbd_core_add_string(p_dev,
                                p_dev->device_config.manufacturer_str_ptr);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Product string.
  status = usbd_core_add_string(p_dev,
                                p_dev->device_config.product_str_ptr);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Serial number string.
  status = usbd_core_add_string(p_dev,
                                p_dev->device_config.serial_nbr_str_ptr);
  if (status != SL_STATUS_OK) {
    return status;
  }
#endif

  status = sli_usbd_core_os_create_task();

  return status;
}

/****************************************************************************************************//**
 *                                               usbd_core_add_endpoint()
 *
 * @brief    Add an endpoint to alternate setting interface.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    attrib          Endpoint's attributes.
 *
 * @param    dir_in          Endpoint Direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet size.
 *
 * @param    interval        interval for polling data transfers.
 *
 * @param    p_ep_addr       Pointer to the variable that will receive endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 *******************************************************************************************************/
static sl_status_t usbd_core_add_endpoint(uint8_t   config_nbr,
                                          uint8_t   if_nbr,
                                          uint8_t   if_alt_nbr,
                                          uint8_t   attrib,
                                          bool      dir_in,
                                          uint16_t  max_pkt_len,
                                          uint8_t   interval,
                                          uint8_t   *p_ep_addr)

{
  sli_usbd_device_t     *p_dev;
  sli_usbd_configuration_t  *p_config;
  sli_usbd_interface_t      *p_if;
  sli_usbd_alt_interface_t  *p_if_alt;
  sli_usbd_endpoint_info_t *p_ep;
  uint8_t   ep_type;
  uint32_t   ep_alloc_map;
  uint32_t   ep_alloc_map_clr;
  uint8_t   ep_nbr;
  uint8_t   ep_phy_nbr;
  sl_usbd_device_speed_t dev_spd;
  bool  alloc;
  CORE_DECLARE_IRQ_STATE;

  // Get Object References

  // Get dev struct.
  p_dev = &usbd_ptr->device;

  // Get cfg struct.
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);

  if (p_config == NULL) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF struct.
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);

  if (p_if == NULL) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Get IF alt setting struct.
  p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);

  if (p_if_alt == NULL) {
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();
  // Chk if EP is avail.
  if (usbd_ptr->endpoint_info_nbr_next == 0u) {
    CORE_EXIT_ATOMIC();
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_ALLOCATION_FAILED;
  }
  usbd_ptr->endpoint_info_nbr_next--;
  ep_nbr = usbd_ptr->endpoint_info_nbr_next;
  CORE_EXIT_ATOMIC();

  ep_type = attrib & SL_USBD_ENDPOINT_TYPE_MASK;
#if (USBD_CFG_HS_EN == 1)
  if (SL_IS_BIT_SET(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
    dev_spd = SL_USBD_DEVICE_SPEED_HIGH;
  } else {
#endif
  dev_spd = SL_USBD_DEVICE_SPEED_FULL;
#if (USBD_CFG_HS_EN == 1)
}
#endif

  p_ep = &usbd_ptr->endpoint_info_table[ep_nbr];
  p_ep->interval = interval;
  p_ep->attrib = attrib;
  // Default sync addr is zero.
  p_ep->sync_addr = 0u;
  // Default feedback rate exponent is zero.
  p_ep->sync_refresh = 0u;

  CORE_ENTER_ATOMIC();
  // Get cfg EP alloc bit map.
  ep_alloc_map = p_config->endpoint_alloc_map;
  // Clr EP already alloc'd in the IF.
  ep_alloc_map &= ~p_if->endpoint_alloc_map;
  ep_alloc_map |= p_if_alt->endpoint_alloc_map;

  ep_alloc_map_clr = ep_alloc_map;

  // Alloc physical EP.
  usbd_core_allocate_endpoint(dev_spd,
                              ep_type,
                              dir_in,
                              // Mask out transactions per microframe.
                              max_pkt_len & 0x7FF,
                              if_alt_nbr,
                              p_ep,
                              &ep_alloc_map,
                              &alloc);

  if (alloc != true) {
    CORE_EXIT_ATOMIC();
    *p_ep_addr = SL_USBD_ENDPOINT_NBR_NONE;
    return SL_STATUS_NOT_AVAILABLE;
  }

  // Set transactions per microframe.
  p_ep->max_pkt_size |= max_pkt_len & 0x1800;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);
  ep_phy_nbr++;

  if (p_dev->endpoint_max_phy_nbr < ep_phy_nbr) {
    p_dev->endpoint_max_phy_nbr = ep_phy_nbr;
  }

  p_if_alt->endpoint_alloc_map |= ep_alloc_map & ~ep_alloc_map_clr;
  p_if->endpoint_alloc_map |= p_if_alt->endpoint_alloc_map;
  p_config->endpoint_alloc_map |= p_if->endpoint_alloc_map;

  p_if_alt->endpoint_nbr_total++;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  ep_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);
  p_if_alt->endpoint_table_ptrs[ep_nbr] = p_ep;
  SL_SET_BIT(p_if_alt->endpoint_table_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
#else
  p_ep->next_ptr = NULL;
  if (p_if_alt->endpoint_head_ptr == NULL) {
    p_if_alt->endpoint_head_ptr = p_ep;
    p_if_alt->endpoint_tail_ptr = p_ep;
  } else {
    p_if_alt->endpoint_tail_ptr->next_ptr = p_ep;
    p_if_alt->endpoint_tail_ptr = p_ep;
  }
#endif
  CORE_EXIT_ATOMIC();

  *p_ep_addr = p_ep->address;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                               usbd_core_allocate_endpoint()
 *
 * @brief    Allocate a physical endpoint from the device controller.
 *
 * @param    spd                 Endpoint speed.
 *                                   - SL_USBD_DEVICE_SPEED_FULL   Endpoint is full-speed.
 *                                   - SL_USBD_DEVICE_SPEED_HIGH   Endpoint is high-speed.
 *
 * @param    type                Endpoint type.
 *                                   - SL_USBD_ENDPOINT_TYPE_CTRL  Control endpoint.
 *                                   - SL_USBD_ENDPOINT_TYPE_ISOC  Isochronous endpoint.
 *                                   - SL_USBD_ENDPOINT_TYPE_BULK  Bulk endpoint.
 *                                   - SL_USBD_ENDPOINT_TYPE_INTR  Interrupt endpoint.
 *
 * @param    dir_in              Endpoint direction.
 *                                   - true    IN  endpoint.
 *                                   - false   OUT endpoint.
 *
 * @param    max_pkt_len         Endpoint maximum packet size length.
 *
 * @param    if_alt_nbr          Alternate interface number containing the endpoint.
 *
 * @param    p_ep                Pointer to the variable that will receive the endpoint parameters.
 *
 * @param    ----                Argument validated in 'sl_usbd_core_add_device()' & 'usbd_core_add_endpoint()'
 *
 * @param    p_alloc_bit_map     Pointer to allocation table bit-map.
 *
 * @param    ---------------     Argument validated in 'sl_usbd_core_add_device()' & 'usbd_core_add_endpoint()'
 *
 * @param    p_alloc             Pointer to a boolean where function result will be returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) 'Universal Serial Bus Specification, Revision 2.0, April 27, 2000' Section 5.5.3
 *               @n
 *               "An endpoint for control transfers specifies the maximum data payload size that
 *               the endpoint can accept from or transmit to the bus. The allowable maximum control
 *               transfer data payload sizes for full-speed devices is 8, 16, 32, or 64 bytes; for
 *               high-speed devices, it is 64 bytes and for low-speed devices, it is 8 bytes."
 *               @n
 *               "All Host Controllers are required to have support for 8-, 16-, 32-, and 64-byte
 *               maximum data payload sizes for full-speed control endpoints, only 8-byte maximum
 *               data payload sizes for low-speed control endpoints, and only 64-byte maximum data
 *               payload size for high-speed control endpoints"
 *******************************************************************************************************/
static sl_status_t usbd_core_allocate_endpoint(sl_usbd_device_speed_t   spd,
                                               uint8_t                  type,
                                               bool                     dir_in,
                                               uint16_t                 max_pkt_len,
                                               uint8_t                  if_alt_nbr,
                                               sli_usbd_endpoint_info_t *p_ep,
                                               uint32_t                 *p_alloc_bit_map,
                                               bool                     *p_alloc)
{
  sli_usbd_driver_endpoint_info_t *p_ep_tbl;
  uint8_t       ep_tbl_ix;
  uint8_t       ep_attrib;
  uint8_t       ep_attrib_srch;
  uint8_t       ep_max_pkt_bits;
  uint16_t       ep_max_pkt;
  bool      ep_found;

#if (USBD_CFG_HS_EN == 0)
  (void)&spd;
  (void)&if_alt_nbr;
#endif

  if (dir_in == true) {
    ep_attrib_srch = SL_USBD_ENDPOINT_INFO_DIR_IN;
  } else {
    ep_attrib_srch = SL_USBD_ENDPOINT_INFO_DIR_OUT;
  }

  switch (type) {
    case SL_USBD_ENDPOINT_TYPE_CTRL:
      SL_SET_BIT(ep_attrib_srch, SL_USBD_ENDPOINT_INFO_TYPE_CTRL);
      break;

    case SL_USBD_ENDPOINT_TYPE_ISOC:
      SL_SET_BIT(ep_attrib_srch, SL_USBD_ENDPOINT_INFO_TYPE_ISOC);
      break;

    case SL_USBD_ENDPOINT_TYPE_BULK:
      SL_SET_BIT(ep_attrib_srch, SL_USBD_ENDPOINT_INFO_TYPE_BULK);
      break;

    case SL_USBD_ENDPOINT_TYPE_INTR:
      SL_SET_BIT(ep_attrib_srch, SL_USBD_ENDPOINT_INFO_TYPE_INTR);
      break;

    default:
      *p_alloc = false;
      return SL_STATUS_OK;
  }

  // Get ctrl EP info tbl.
  sli_usbd_driver_get_endpoint_info(&p_ep_tbl);
  // Get attrib for first entry.
  ep_attrib = p_ep_tbl->attrib;
  ep_tbl_ix = 0u;
  ep_found = false;

  // Search until last entry or EP found.
  while ((ep_attrib != 0x00u)
         && (ep_found == false)) {
    // Chk if EP not alloc'd and EP attrib match req'd attrib.
    if ((SL_IS_BIT_CLEAR(*p_alloc_bit_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_tbl_ix)) == true)
        && (SL_IS_BIT_SET(ep_attrib, ep_attrib_srch) == true)) {
      ep_max_pkt = p_ep_tbl[ep_tbl_ix].max_pkt_size;

      switch (type) {
        // Chk ctrl transfer pkt size constrains.
        case SL_USBD_ENDPOINT_TYPE_CTRL:
          ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 64u);
          // Get next power of 2.
          ep_max_pkt_bits = (uint8_t)(31u - __CLZ(ep_max_pkt));
          ep_max_pkt = SLI_USBD_SINGLE_BIT_MASK_16(ep_max_pkt_bits);
#if (USBD_CFG_HS_EN == 1)
          if ((spd == SL_USBD_DEVICE_SPEED_HIGH)
              && (ep_max_pkt != 64u)) {
            break;
          }

          if ((spd == SL_USBD_DEVICE_SPEED_HIGH)
              && (ep_max_pkt < 8u)) {
            break;
          }
#endif
          ep_found = true;
          break;

        case SL_USBD_ENDPOINT_TYPE_BULK:
#if (USBD_CFG_HS_EN == 1)
          // Max pkt size is 512 for bulk EP in HS.
          ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 512u);
          if ((spd == SL_USBD_DEVICE_SPEED_HIGH)
              && (ep_max_pkt == 512u)) {
            ep_found = true;
            break;
          }
#endif
          // Max pkt size is 64 for bulk EP in FS.
          ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 64u);
          ep_max_pkt_bits = (uint8_t)(31u - __CLZ(ep_max_pkt));
          ep_max_pkt = SLI_USBD_SINGLE_BIT_MASK_16(ep_max_pkt_bits);
#if (USBD_CFG_HS_EN == 1)
          if ((spd == SL_USBD_DEVICE_SPEED_HIGH)
              && (ep_max_pkt >= 8u)) {
            break;
          }
#endif
          ep_found = true;
          break;

        case SL_USBD_ENDPOINT_TYPE_ISOC:
#if (USBD_CFG_HS_EN == 1)
          if (spd == SL_USBD_DEVICE_SPEED_HIGH) {
            ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 1024u);
          } else {
#endif
          ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 1023u);
#if (USBD_CFG_HS_EN == 1)
      }
#endif

          if (max_pkt_len > 0u) {
            ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, max_pkt_len);
          }

          ep_found = true;
          break;

        case SL_USBD_ENDPOINT_TYPE_INTR:
#if (USBD_CFG_HS_EN == 1)
          if ((spd == SL_USBD_DEVICE_SPEED_HIGH)
              && (if_alt_nbr != 0u)) {
            // Dflt IF intr EP max pkt size limited to 64.
            ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 1024u);
          } else {
#endif
          ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, 64u);
#if (USBD_CFG_HS_EN == 1)
      }
#endif
          if (max_pkt_len > 0u) {
            ep_max_pkt = SLI_USBD_GET_MIN(ep_max_pkt, max_pkt_len);
          }

          ep_found = true;
          break;

        default:
          *p_alloc = false;
          return SL_STATUS_OK;
      }

      if ((ep_found == true)
          && ((max_pkt_len == ep_max_pkt)
              || (max_pkt_len == 0u))) {
        p_ep->max_pkt_size = ep_max_pkt;
        SL_SET_BIT(*p_alloc_bit_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_tbl_ix));
        p_ep->address = p_ep_tbl[ep_tbl_ix].nbr;
        if (dir_in == true) {
          // Add dir bit (IN EP).
          p_ep->address |= SL_USBD_ENDPOINT_DIR_IN;
        }
      } else {
        ep_found = false;
        ep_tbl_ix++;
        ep_attrib = p_ep_tbl[ep_tbl_ix].attrib;
      }
    } else {
      ep_tbl_ix++;
      ep_attrib = p_ep_tbl[ep_tbl_ix].attrib;
    }
  }

  if (ep_found == false) {
    *p_alloc = false;
    return SL_STATUS_OK;
  }

  *p_alloc = true;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                           usbd_core_stdreq_handler()
 *
 * @brief    Standard request process.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *******************************************************************************************************/
static void usbd_core_stdreq_handler(sli_usbd_device_t *p_dev)
{
  uint8_t recipient;
  uint8_t type;
  uint8_t request;
  bool valid;
  bool dev_to_host;
  sl_status_t   local_status;
  CORE_DECLARE_IRQ_STATE;

  local_status = SL_STATUS_OK;

#if (USBD_CFG_MS_OS_DESC_EN != 1)
  (void)&local_status;
#endif

  CORE_ENTER_ATOMIC();
  // Copy setup request.
  p_dev->setup_req.bmRequestType = p_dev->setup_req_next.bmRequestType;
  p_dev->setup_req.bRequest = p_dev->setup_req_next.bRequest;
  p_dev->setup_req.wValue = p_dev->setup_req_next.wValue;
  p_dev->setup_req.wIndex = p_dev->setup_req_next.wIndex;
  p_dev->setup_req.wLength = p_dev->setup_req_next.wLength;
  CORE_EXIT_ATOMIC();

  recipient = p_dev->setup_req.bmRequestType & SL_USBD_REQ_RECIPIENT_MASK;
  type = p_dev->setup_req.bmRequestType & SL_USBD_REQ_TYPE_MASK;
  request = p_dev->setup_req.bRequest;
  dev_to_host = SL_IS_BIT_SET(p_dev->setup_req.bmRequestType, SL_USBD_REQ_DIR_BIT);
  valid = false;

  switch (type) {
    case SL_USBD_REQ_TYPE_STANDARD:
      // Select req recipient:
      switch (recipient) {
        case SL_USBD_REQ_RECIPIENT_DEVICE:
          // Device.
          valid = usbd_core_device_stdreq(p_dev, request);
          break;

        case SL_USBD_REQ_RECIPIENT_INTERFACE:
          // Interface.
          valid = usbd_core_interface_stdreq(p_dev, request);
          break;

        case SL_USBD_REQ_RECIPIENT_ENDPOINT:
          // Endpoint.
          valid = usbd_core_endpoint_stdreq(p_dev, request);
          break;

        case SL_USBD_REQ_RECIPIENT_OTHER:
        // Not supported.
        default:
          break;
      }
      break;

    case SL_USBD_REQ_TYPE_CLASS:
      // Class-specific req.
      switch (recipient) {
        case SL_USBD_REQ_RECIPIENT_INTERFACE:
        case SL_USBD_REQ_RECIPIENT_ENDPOINT:
          // Class-specific req.
          valid = usbd_core_class_stdreq(p_dev);
          break;

        case SL_USBD_REQ_RECIPIENT_DEVICE:
        case SL_USBD_REQ_RECIPIENT_OTHER:
        default:
          break;
      }
      break;

    case SL_USBD_REQ_TYPE_VENDOR:
      switch (recipient) {
        case SL_USBD_REQ_RECIPIENT_INTERFACE:
#if (USBD_CFG_MS_OS_DESC_EN == 1)
          if (request == p_dev->str_microsoft_vendor_code) {
            p_dev->desc_buf_status_ptr = &local_status;
            // Microsoft OS descriptor req.
            valid = usbd_core_microsoft_interface_stdreq(p_dev);
            p_dev->desc_buf_status_ptr = NULL;
          } else {
            // Vendor-specific req.
            valid = usbd_core_vendor_stdreq(p_dev);
          }
          break;
#endif

        case SL_USBD_REQ_RECIPIENT_ENDPOINT:
          // Vendor-specific req.
          valid = usbd_core_vendor_stdreq(p_dev);
          break;

        case SL_USBD_REQ_RECIPIENT_DEVICE:
#if (USBD_CFG_MS_OS_DESC_EN == 1)
          if (request == p_dev->str_microsoft_vendor_code) {
            p_dev->desc_buf_status_ptr = &local_status;
            // Microsoft OS descriptor req.
            valid = usbd_core_microsoft_device_stdreq(p_dev);
            p_dev->desc_buf_status_ptr = NULL;
          }
#endif
          break;

        case SL_USBD_REQ_RECIPIENT_OTHER:
        default:
          break;
      }
      break;

    case SL_USBD_REQ_TYPE_RESERVED:
    default:
      break;
  }

  if (valid == false) {
    SLI_USBD_LOG_DBG(("USBD: Request Error"));
    sli_usbd_core_stall_control_endpoint();
  } else {
    uint32_t std_req_timeout;

    CORE_ENTER_ATOMIC();
    std_req_timeout = usbd_ptr->std_req_timeout_ms;
    CORE_EXIT_ATOMIC();

    if (dev_to_host == true) {
      SLI_USBD_LOG_VRB(("USBD: Rx Status"));
      sli_usbd_core_get_control_rx_status(std_req_timeout);
    } else {
      SLI_USBD_LOG_VRB(("USBD: Tx Status"));
      sli_usbd_core_get_control_tx_status(std_req_timeout);
    }
  }
}

/****************************************************************************************************//**
 *                                               usbd_core_device_stdreq()
 *
 * @brief    Process device standard request.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----    Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *
 * @note     (1) USB Spec 2.0, section 9.4.6 specifies the format of the SET_ADDRESS request. The
 *               SET_ADDRESS sets the device address for all future device access.
 *           - (a) The 'wValue' filed specify the device address to use for all subsequent accesses.
 *           - (b) If the specified device address is greater than 127 or if 'wIndex' or 'wLength'
 *                   are non-zero, the behavior of the device is not specified.
 *           - (c) If the device is in the default state and the address specified is non-zero,
 *                   the device shall enter the device address, otherwise the device remains in the
 *                   default state' (this is not an error condition).
 *           - (d) If the device is in the address state and the address specified is zero, then
 *                   the device shall enter the default state otherwise, the device remains in
 *                   the address state but uses the newly-specified address.
 *           - (e) Device behavior when the SET_ADDRESS request is received while the device is not
 *                   in the default or address state is not specified.
 *           - (f) USB Spec 2.0, section 9.2.6.3 specifies the maximum timeout for the SET_ADDRESS
 *                   request:
 *                   "After the reset/resume recovery interval, if a device receives a SetAddress()
 *                   request, the device must be able to complete processing of the request and be
 *                   able to successfully complete the Status stage of the request within 50 ms. In
 *                   the case of the SetAddress() request, the Status stage successfully completes
 *                   when the device sends the zero-length Status packet or when the device sees
 *                   the ACK in response to the Status stage data packet."
 *
 * @note     (2) USB Spec 2.0, section 9.4.7 specifies the format of the SET_CONFIGURATION request.
 *           - (a) The lower byte of 'wValue' field specifies the desired configuration.
 *           - (b) If 'wIndex', 'wLength', or the upper byte of wValue is non-zero, then the behavior
 *                   of this request is not specified.
 *           - (c) The configuration value must be zero or match a configuration value from a
 *                   configuration value from a configuration descriptor. If the configuration value
 *                   is zero, the device is place in its address state.
 *           - (d) Device behavior when this request is received while the device is in the Default
 *                   state is not specified.
 *           - (e) If device is in address state and the specified configuration value is zero,
 *                   then the device remains in the Address state. If the specified configuration value
 *                   matches the configuration value from a configuration descriptor, then that
 *                   configuration is selected and the device enters the Configured state. Otherwise,
 *                   the device responds with a Request Error.
 *           - (f) If the specified configuration value is zero, then the device enters the Address
 *                   state. If the specified configuration value matches the configuration value from a
 *                   configuration descriptor, then that configuration is selected and the device
 *                   remains in the Configured state. Otherwise, the device responds with a Request
 *                   Error.
 *
 * @note     (3) USB Spec 2.0, section 9.4.2 specifies the format of the GET_CONFIGURATION request.
 *           - (a) If 'wValue' or 'wIndex' are non-zero or 'wLength' is not '1', then the device
 *                   behavior is not specified.
 *           - (b) If the device is in default state, the device behavior is not specified.
 *           - (c) In address state a value of zero MUST be returned.
 *           - (d) In configured state, the non-zero bConfigurationValue of the current configuration
 *                   must be returned.
 *
 * @note     (4) USB Spec 2.0, section 9.4.5 specifies the format of the GET_STATUS request.
 *           - (a) If 'wValue' is non-zero or 'wLength is not equal to '2', or if wIndex is non-zero
 *                   then the behavior of the device is not specified.
 *           - (b) USB Spec 2, 0, figure 9-4 shows the format of information returned by the device
 *                   for a GET_STATUS request.
 *                   @verbatim
 *                   +====|====|====|====|====|====|====|========|=========+
 *                   | D0 | D1 | D2 | D3 | D4 | D3 | D2 |   D1   |    D0   |
 *                   |----------------------------------|--------|---------|
 *                   |     RESERVED (RESET TO ZERO)     | Remote |   Self  |
 *                   |                                  | Wakeup | Powered |
 *                   +==================================|========|=========+
 *                   @endverbatim
 *               - (1) The Self Powered field indicates whether the device is currently self-powered.
 *                       If D0 is reset to zero, the device is bus-powered. If D0 is set to one, the
 *                       device is self-powered. The Self Powered field may not be changed by the
 *                       SetFeature() or ClearFeature() requests.
 *               - (2) The Remote Wakeup field indicates whether the device is currently enabled to
 *                       request remote wakeup. The default mode for devices that support remote wakeup
 *                       is disabled. If D1 is reset to zero, the ability of the device to signal
 *                       remote wakeup is disabled. If D1 is set to one, the ability of the device to
 *                       signal remote wakeup is enabled. The Remote Wakeup field can be modified by
 *                       the SetFeature() and ClearFeature() requests using the DEVICE_REMOTE_WAKEUP
 *                       feature selector. This field is reset to zero when the device is reset.
 *
 * @note     (5) USB Spec 2.0, section 9.4.1/9.4.9 specifies the format of the CLEAR_FEATURE/SET_FEATURE
 *               request.
 *           - (a) If 'wLength' or 'wIndex' are non-zero, then the device behavior is not specified.
 *           - (b) The device CLEAR_FEATURE request is only valid when the device is in the
 *                   configured state.
 *******************************************************************************************************/
static bool usbd_core_device_stdreq(sli_usbd_device_t   *p_dev,
                                    uint8_t             request)
{
  bool valid;
  bool dev_to_host;
  uint8_t dev_addr;
  uint8_t cfg_nbr;
  uint32_t std_req_timeout;
  uint32_t xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = usbd_ptr->std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  SLI_USBD_DBG_STATS_DEV_INC(std_req_device_nbr);

  dev_to_host = SL_IS_BIT_SET(p_dev->setup_req.bmRequestType, SL_USBD_REQ_DIR_BIT);
  valid = false;

  switch (request) {
    case SL_USBD_REQ_GET_DESCRIPTOR:
      if (dev_to_host != true) {
        break;
      }

      valid = usbd_core_get_descriptor_stdreq(p_dev);
      break;

    case SL_USBD_REQ_SET_ADDRESS:
      if (dev_to_host != false) {
        break;
      }

      SLI_USBD_DBG_STATS_DEV_INC(std_req_set_address_nbr);

      // Get dev addr (see Note #1a).
      dev_addr = (uint8_t)(p_dev->setup_req.wValue
                           & 0xFFu);

      SLI_USBD_LOG_VRB(("USBD: Set Address to addr #", (u)dev_addr));

      // Validate request values. (see Note #1b).
      if ((dev_addr > 127u)
          || (p_dev->setup_req.wIndex != 0u)
          || (p_dev->setup_req.wLength != 0u)) {
        break;
      }

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_DEFAULT:
          if (dev_addr > 0u) {
            // See Note #1c.
            status = sli_usbd_driver_set_address(dev_addr);
            if (status != SL_STATUS_OK) {
              SLI_USBD_LOG_ERR(("USBD Set Address Failed"));
              break;
            }

            CORE_ENTER_ATOMIC();
            // Set dev in addressed state.
            p_dev->address = dev_addr;
            p_dev->state = SL_USBD_DEVICE_STATE_ADDRESSED;
            CORE_EXIT_ATOMIC();

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_ADDRESSED:
          if (dev_addr == 0u) {
            // See Note #1c. If dev addr is zero set addr in dev drv.
            status = sli_usbd_driver_set_address(0u);
            if (status != SL_STATUS_OK) {
              SLI_USBD_LOG_ERR(("USBD Set Address Failed"));
              break;
            }

            CORE_ENTER_ATOMIC();
            // Dev enters default state.
            p_dev->address = 0u;
            p_dev->state = SL_USBD_DEVICE_STATE_DEFAULT;
            CORE_EXIT_ATOMIC();

            valid = true;
          } else {
            // remains in addressed state and set new addr.
            status = sli_usbd_driver_set_address(dev_addr);
            if (status != SL_STATUS_OK) {
              SLI_USBD_LOG_ERR(("USBD Set Address Failed"));
              break;
            }

            CORE_ENTER_ATOMIC();
            p_dev->address = dev_addr;
            CORE_EXIT_ATOMIC();

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_CONFIGURED:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          SLI_USBD_LOG_ERR(("USBD Set Address Failed due to invalid device state."));
          break;
      }
      break;

    case SL_USBD_REQ_SET_CONFIGURATION:
      if (dev_to_host != false) {
        break;
      }

      SLI_USBD_DBG_STATS_DEV_INC(std_req_set_configuration_nbr);

      // Validate request values (see Note #2b).
      if (((p_dev->setup_req.wValue & 0xFF00u) != 0u)
          && (p_dev->setup_req.wIndex != 0u)
          && (p_dev->setup_req.wLength != 0u)) {
        break;
      }
      // Get cfg value.
      cfg_nbr = (uint8_t)(p_dev->setup_req.wValue & 0xFFu);
      SLI_USBD_LOG_VRB(("USBD: Set Configuration #", (u)cfg_nbr));

#if (USBD_CFG_HS_EN == 1)
      // Cfg value MUST exists.
      if ((cfg_nbr > p_dev->config_hs_total_nbr)
          && (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH)) {
        SLI_USBD_LOG_ERR(("  Set Configuration Cfg Invalid nbr"));
        break;
      }
#endif

      if ((cfg_nbr > p_dev->config_fs_total_nbr)
          && (p_dev->speed == SL_USBD_DEVICE_SPEED_FULL)) {
        SLI_USBD_LOG_ERR(("  Set Configuration Cfg Invalid nbr"));
        break;
      }

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_ADDRESSED:
          // See Note #2e.
          if (cfg_nbr > 0u) {
            // Open cfg.
            status = usbd_core_set_configuration(p_dev, (cfg_nbr - 1u));

            if (status != SL_STATUS_OK) {
              SLI_USBD_LOG_ERR(("USBD Set Configuration Failed. status = ", status));
              break;
            }

            valid = true;
          } else {
            // Remain in addressed state.
            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_CONFIGURED:
          // See Note #2f.
          if (cfg_nbr > 0u) {
            if (p_dev->config_cur_nbr == (cfg_nbr - 1u)) {
              valid = true;
              break;
            }

            // Close curr  cfg.
            usbd_core_unset_configuration(p_dev);

            // Open cfg.
            status = usbd_core_set_configuration(p_dev, (cfg_nbr - 1u));

            if (status != SL_STATUS_OK) {
              SLI_USBD_LOG_ERR(("USBD Set Configuration Failed. status = ", status));
              break;
            }

            valid = true;
          } else {
            // Close curr cfg.
            usbd_core_unset_configuration(p_dev);

            CORE_ENTER_ATOMIC();
            p_dev->state = SL_USBD_DEVICE_STATE_ADDRESSED;
            CORE_EXIT_ATOMIC();

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          SLI_USBD_LOG_ERR(("USBD Set Configuration Failed due to invalid device state."));
          break;
      }
      break;

    case SL_USBD_REQ_GET_CONFIGURATION:
      if (dev_to_host != true) {
        break;
      }

      // Validate request values (see Note #3a).
      if ((p_dev->setup_req.wLength != 1u)
          && (p_dev->setup_req.wIndex != 0u)
          && (p_dev->setup_req.wValue != 0u)) {
        break;
      }

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_ADDRESSED:
          // See Note #3b.
          cfg_nbr = 0u;
          SLI_USBD_LOG_VRB(("USBD: Get Configuration #", (u)cfg_nbr));

          // Uses Ctrl status buf to follow USB mem alignment.
          p_dev->ctrl_status_buf_ptr[0u] = cfg_nbr;

          status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                                   1u,
                                                   std_req_timeout,
                                                   false,
                                                   &xfer_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_CONFIGURED:
          // See Note #3c.
          if (p_dev->config_cur_ptr == NULL) {
            break;
          }

          cfg_nbr = p_dev->config_cur_nbr + 1u;
          SLI_USBD_LOG_VRB(("USBD: Get Configuration #", (u)cfg_nbr));

          // Uses Ctrl status buf to follow USB mem alignment.
          p_dev->ctrl_status_buf_ptr[0u] = cfg_nbr;

          status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                                   1u,
                                                   std_req_timeout,
                                                   false,
                                                   &xfer_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          SLI_USBD_LOG_ERR(("USBD Get Configuration Failed due to invalid device state."));
          break;
      }
      break;

    case SL_USBD_REQ_GET_STATUS:
      if (dev_to_host != true) {
        break;
      }

      // Validate request values (see Note #4a).
      if ((p_dev->setup_req.wLength != 2u)
          && (p_dev->setup_req.wIndex != 0u)
          && (p_dev->setup_req.wValue != 0u)) {
        break;
      }

      SLI_USBD_LOG_VRB(("USBD: Get Status (Device)"));
      p_dev->ctrl_status_buf_ptr[0u] = 0x00u;
      p_dev->ctrl_status_buf_ptr[1u] = 0x00u;

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_ADDRESSED:
          // See Note #4b.
          if (p_dev->self_power == true) {
            p_dev->ctrl_status_buf_ptr[0u] |= 0x01u; // BIT_00
          }
          if (p_dev->remote_wakeup == true) {
            p_dev->ctrl_status_buf_ptr[0u] |= 0x02u; // BIT_01
          }

          status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                                   2u,
                                                   std_req_timeout,
                                                   false,
                                                   &xfer_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_CONFIGURED:
          if (p_dev->config_cur_ptr != NULL) {
            if (SL_IS_BIT_SET(p_dev->config_cur_ptr->attrib, SL_USBD_DEV_ATTRIB_SELF_POWERED)) {
              p_dev->ctrl_status_buf_ptr[0u] |= 0x01u; // BIT_00
            }
            if (SL_IS_BIT_SET(p_dev->config_cur_ptr->attrib, SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP)) {
              p_dev->ctrl_status_buf_ptr[0u] |= 0x02u; // BIT_01
            }
          }

          status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                                   2u,
                                                   std_req_timeout,
                                                   false,
                                                   &xfer_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          SLI_USBD_LOG_ERR(("USBD Get Status (Device) Failed due to invalid device state."));
          break;
      }
      break;

    case SL_USBD_REQ_CLEAR_FEATURE:
    case SL_USBD_REQ_SET_FEATURE:
      if (dev_to_host != false) {
        break;
      }

      // Validate request values.
      if ((p_dev->setup_req.wLength != 0u)
          && (p_dev->setup_req.wIndex != 0u)) {
        break;
      }

      if (request == SL_USBD_REQ_CLEAR_FEATURE) {
        SLI_USBD_LOG_VRB(("USBD: Clear Feature (Device)"));
      } else {
        SLI_USBD_LOG_VRB(("USBD: Set Feature (Device)"));
      }

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_CONFIGURED:
          if (p_dev->config_cur_ptr == NULL) {
            break;
          }

          if ((p_dev->setup_req.wValue == SL_USBD_FEATURE_SEL_DEVICE_REMOTE_WAKEUP)
              && (SL_IS_BIT_SET(p_dev->config_cur_ptr->attrib, SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP))) {
            p_dev->remote_wakeup = (request == SL_USBD_REQ_CLEAR_FEATURE) ? 0u : 1u;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_ADDRESSED:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          if (request == SL_USBD_REQ_CLEAR_FEATURE) {
            SLI_USBD_LOG_ERR(("USBD Clear Feature (Device) Failed due to invalid device state."));
          } else {
            SLI_USBD_LOG_ERR(("USBD Set Feature (Device) Failed due to invalid device state."));
          }
          break;
      }
      break;

    default:
      break;
  }

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(std_req_device_stall_nbr, (valid == false));

  return (valid);
}

/****************************************************************************************************//**
 *                                               usbd_core_interface_stdreq()
 *
 * @brief    Process device standard request (Interface).
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *
 * @note     (1) USB Spec 2.0, section 9.4.10 specifies the format of the SET_INTERFACE request.
 *               This request allows the host to select an alternate setting for the specified
 *               interface:
 *           - (a) Some USB devices have configurations with interfaces that have mutually
 *                   exclusive settings.  This request allows the host to select the desired
 *                   alternate setting.  If a device only supports a default setting for the
 *                   specified interface, then a STALL may be returned in the Status stage of
 *                   the request. This request cannot be used to change the set of configured
 *                   interfaces (the SetConfiguration() request must be used instead).
 *           - (2) USB Spec 2.0, section 9.4.4 specifies the format of the GET_INTERFACE request.
 *               This request returns the selected alternate setting for the specified interface.
 *           - (a) If 'wValue' is non-zero or 'wLength' is not '1', then the device behavior is
 *                   not specified.
 *           - (b) The GET_INTERFACE request is only valid when the device is in the configured
 *                   state.
 *******************************************************************************************************/
static bool usbd_core_interface_stdreq(sli_usbd_device_t   *p_dev,
                                       uint8_t             request)
{
  uint32_t std_req_timeout;
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sli_usbd_alt_interface_t *p_if_alt;
  sl_usbd_class_driver_t *p_class_drv;
  uint8_t if_nbr;
  uint8_t if_alt_nbr;
  bool valid;
  bool dev_to_host;
  uint16_t req_len;
  uint32_t xfer_len;
  sl_status_t status;
  sl_status_t local_status;
  CORE_DECLARE_IRQ_STATE;

  local_status = SL_STATUS_OK;

  CORE_ENTER_ATOMIC();
  std_req_timeout = usbd_ptr->std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  SLI_USBD_DBG_STATS_DEV_INC(std_req_interface_nbr);

  p_config = p_dev->config_cur_ptr;
  if (p_config == NULL) {
    SLI_USBD_DBG_STATS_DEV_INC(std_req_interface_stall_nbr);
    return (false);
  }

  if_nbr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);
  if (p_if == NULL) {
    SLI_USBD_DBG_STATS_DEV_INC(std_req_interface_stall_nbr);
    return (false);
  }

  dev_to_host = SL_IS_BIT_SET(p_dev->setup_req.bmRequestType, SL_USBD_REQ_DIR_BIT);
  valid = false;

  switch (request) {
    case SL_USBD_REQ_GET_STATUS:
      if (dev_to_host != true) {
        break;
      }

      SLI_USBD_LOG_VRB(("USBD: Get Status (Interface) IF ", (u)if_nbr));

      if ((p_dev->state != SL_USBD_DEVICE_STATE_ADDRESSED)
          && (p_dev->state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
        break;
      }

      if ((p_dev->state == SL_USBD_DEVICE_STATE_ADDRESSED)
          && (if_nbr != 0u)) {
        break;
      }

      p_dev->ctrl_status_buf_ptr[0u] = 0x00u;

      status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                               1u,
                                               std_req_timeout,
                                               false,
                                               &xfer_len);
      if (status != SL_STATUS_OK) {
        break;
      }

      valid = true;
      break;

    case SL_USBD_REQ_CLEAR_FEATURE:
    case SL_USBD_REQ_SET_FEATURE:
      if (dev_to_host != false) {
        break;
      }

      if (request == SL_USBD_REQ_CLEAR_FEATURE) {
        SLI_USBD_LOG_VRB(("USBD: Clear Feature (Interface) IF ", (u)if_nbr));
      } else {
        SLI_USBD_LOG_VRB(("USBD: Set Feature (Interface) IF ", (u)if_nbr));
      }

      if ((p_dev->state != SL_USBD_DEVICE_STATE_ADDRESSED)
          && (p_dev->state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
        break;
      }

      if ((p_dev->state == SL_USBD_DEVICE_STATE_ADDRESSED)
          && (if_nbr != 0u)) {
        break;
      }

      valid = true;
      break;

    case SL_USBD_REQ_GET_DESCRIPTOR:
      if (dev_to_host != true) {
        break;
      }

      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (Interface) IF ", (u)if_nbr));

      p_class_drv = p_if->class_driver_ptr;
      if (p_class_drv->interface_req == NULL) {
        break;
      }

      req_len = p_dev->setup_req.wLength;
      usbd_core_start_descriptor_write(p_dev, req_len);

      p_dev->desc_buf_status_ptr = &local_status;

      valid = p_class_drv->interface_req(&p_dev->setup_req,
                                         p_if->class_arg_ptr);
      if (valid == true) {
        status = usbd_core_stop_descriptor_write(p_dev);
        if (status != SL_STATUS_OK) {
          valid = false;
        }
      }
      p_dev->desc_buf_status_ptr = NULL;
      break;

    case SL_USBD_REQ_GET_INTERFACE:
      if (dev_to_host != true) {
        break;
      }

      if (p_dev->state != SL_USBD_DEVICE_STATE_CONFIGURED) {
        break;
      }

      p_dev->ctrl_status_buf_ptr[0u] = p_if->alt_cur;

      SLI_USBD_LOG_VRB(("USBD: Get Interface IF ", (u)if_nbr, " Alt ", (u)p_if->alt_cur));

      status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                               1u,
                                               std_req_timeout,
                                               false,
                                               &xfer_len);
      if (status != SL_STATUS_OK) {
        break;
      }

      valid = true;
      break;

    case SL_USBD_REQ_SET_INTERFACE:
      if (dev_to_host != false) {
        break;
      }

      if (p_dev->state != SL_USBD_DEVICE_STATE_CONFIGURED) {
        break;
      }
      // Get IF alt setting nbr.
      if_alt_nbr = (uint8_t)(p_dev->setup_req.wValue  & 0xFFu);
      p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);

      SLI_USBD_LOG_VRB(("USBD: Set Interface IF ", (u)if_nbr, " Alt ", (u)if_alt_nbr));

      if (p_if_alt == NULL) {
        SLI_USBD_LOG_ERR(("USBD: Set Interface Invalid Alt IF"));
        break;
      }

      // If alt setting is the same as the cur one, no further processing is needed.
      if (p_if_alt == p_if->alt_cur_ptr) {
        valid = true;
        break;
      }

      // Close the cur alt setting.
      usbd_core_close_alt_interface(p_dev, p_if->alt_cur_ptr);

      // Open the new alt setting.
      status = usbd_core_open_alt_interface(p_dev, if_nbr, p_if_alt);
      // Re-open curr IF alt setting, in case it fails.
      if (status != SL_STATUS_OK) {
        status = usbd_core_open_alt_interface(p_dev, p_if->alt_cur, p_if->alt_cur_ptr);
        break;
      }

      CORE_ENTER_ATOMIC();
      // Set IF alt setting.
      p_if->alt_cur_ptr = p_if_alt;
      p_if->alt_cur = if_alt_nbr;
      CORE_EXIT_ATOMIC();

      // Notify class that IF or alt IF has been updated.
      if (p_if->class_driver_ptr->alt_settings_update != NULL) {
        p_if->class_driver_ptr->alt_settings_update(p_dev->config_cur_nbr,
                                                    if_nbr,
                                                    if_alt_nbr,
                                                    p_if->class_arg_ptr,
                                                    p_if_alt->class_arg_ptr);
      }

      valid = true;
      break;

    default:
      p_class_drv = p_if->class_driver_ptr;
      if (p_class_drv->interface_req == NULL) {
        break;
      }

      valid = p_class_drv->interface_req(&p_dev->setup_req,
                                         p_if->class_arg_ptr);
      break;
  }

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(std_req_interface_stall_nbr, (valid == false));

  return (valid);
}

/****************************************************************************************************//**
 *                                               usbd_core_endpoint_stdreq()
 *
 * @brief    Process device standard request (Endpoint).
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    request     USB device request.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *******************************************************************************************************/
static bool usbd_core_endpoint_stdreq(const sli_usbd_device_t *p_dev,
                                      uint8_t                 request)
{
  uint32_t std_req_timeout;
  sli_usbd_interface_t *p_if;
  sli_usbd_alt_interface_t *p_alt_if;
  bool ep_is_stall;
  uint8_t if_nbr;
  uint8_t ep_addr;
  uint8_t ep_phy_nbr;
  bool valid;
  bool dev_to_host;
  uint8_t feature;
  uint32_t xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = usbd_ptr->std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  SLI_USBD_DBG_STATS_DEV_INC(std_req_endpoint_nbr);

  ep_addr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
  feature = (uint8_t)(p_dev->setup_req.wValue & 0xFFu);
  dev_to_host = SL_IS_BIT_SET(p_dev->setup_req.bmRequestType, SL_USBD_REQ_DIR_BIT);
  valid = false;

  switch (request) {
    case SL_USBD_REQ_CLEAR_FEATURE:
    case SL_USBD_REQ_SET_FEATURE:
      if (dev_to_host != false) {
        break;
      }

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_ADDRESSED:
          if (((ep_addr == 0x80u)
               || (ep_addr == 0x00u))
              && (feature == SL_USBD_FEATURE_SEL_ENDPOINT_HALT)) {
            if (request == SL_USBD_REQ_CLEAR_FEATURE) {
              SLI_USBD_LOG_VRB(("USBD: Clear Feature (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));

              status = sl_usbd_core_stall_endpoint(ep_addr, false);
              if (status != SL_STATUS_OK) {
                break;
              }
            } else {
              SLI_USBD_LOG_VRB(("USBD: Set Feature (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));

              status = sl_usbd_core_stall_endpoint(ep_addr, true);
              if (status != SL_STATUS_OK) {
                break;
              }
            }

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_CONFIGURED:
          if (feature == SL_USBD_FEATURE_SEL_ENDPOINT_HALT) {
            if (request == SL_USBD_REQ_CLEAR_FEATURE) {
              SLI_USBD_LOG_VRB(("USBD: Clear Feature (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));

              status = sl_usbd_core_stall_endpoint(ep_addr, false);
              if (status != SL_STATUS_OK) {
                break;
              }
            } else {
              SLI_USBD_LOG_VRB(("USBD: Set Feature (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));

              status = sl_usbd_core_stall_endpoint(ep_addr, true);
              if (status != SL_STATUS_OK) {
                break;
              }
            }

            ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
            if_nbr = p_dev->endpoint_interface_table[ep_phy_nbr];
            p_if = usbd_core_get_interface_structure(p_dev->config_cur_ptr, if_nbr);
            p_alt_if = p_if->alt_cur_ptr;

            // Notify class that EP state has been updated.
            if (p_if->class_driver_ptr->endpoint_state_update != NULL) {
              p_if->class_driver_ptr->endpoint_state_update(p_dev->config_cur_nbr,
                                                            if_nbr,
                                                            p_if->alt_cur,
                                                            ep_addr,
                                                            p_if->class_arg_ptr,
                                                            p_alt_if->class_arg_ptr);
            }

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          break;
      }
      break;

    case SL_USBD_REQ_GET_STATUS:
      if (dev_to_host != true) {
        break;
      }

      p_dev->ctrl_status_buf_ptr[0u] = 0x00u;
      p_dev->ctrl_status_buf_ptr[1u] = 0x00u;

      switch (p_dev->state) {
        case SL_USBD_DEVICE_STATE_ADDRESSED:
          if ((ep_addr == 0x80u)
              || (ep_addr == 0x00u)) {
            SLI_USBD_LOG_VRB(("USBD: Get Status (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));
            sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
            if (ep_is_stall == true) {
              p_dev->ctrl_status_buf_ptr[0u] = 0x01u; // BIT_00
              p_dev->ctrl_status_buf_ptr[1u] = 0x00u;
            }

            status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0u],
                                                     2u,
                                                     std_req_timeout,
                                                     false,
                                                     &xfer_len);
            if (status != SL_STATUS_OK) {
              break;
            }

            valid = true;
          }
          break;

        case SL_USBD_DEVICE_STATE_CONFIGURED:
          SLI_USBD_LOG_VRB(("USBD: Get Status (EP)(STALL) for EP 0x", (X)SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)));
          sl_usbd_core_is_endpoint_stalled(ep_addr, &ep_is_stall);
          if (ep_is_stall == true) {
            p_dev->ctrl_status_buf_ptr[0u] = 0x01u; // BIT_00
            p_dev->ctrl_status_buf_ptr[1u] = 0x00u;
          }

          status = sl_usbd_core_write_control_sync((void *)&p_dev->ctrl_status_buf_ptr[0],
                                                   2u,
                                                   std_req_timeout,
                                                   false,
                                                   &xfer_len);
          if (status != SL_STATUS_OK) {
            break;
          }

          valid = true;
          break;

        case SL_USBD_DEVICE_STATE_NONE:
        case SL_USBD_DEVICE_STATE_INIT:
        case SL_USBD_DEVICE_STATE_ATTACHED:
        case SL_USBD_DEVICE_STATE_DEFAULT:
        case SL_USBD_DEVICE_STATE_SUSPENDED:
        default:
          break;
      }
      break;

    default:
      break;
  }

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(std_req_endpoint_stall_nbr, (valid == false));

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_core_class_stdreq()
 *
 * @brief    Class standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *******************************************************************************************************/
static bool usbd_core_class_stdreq(const sli_usbd_device_t *p_dev)
{
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sl_usbd_class_driver_t *p_class_drv;
  uint8_t recipient;
  uint8_t if_nbr;
  uint8_t ep_addr;
  uint8_t ep_phy_nbr;
  bool valid;

  SLI_USBD_DBG_STATS_DEV_INC(std_req_class_nbr);

  p_config = p_dev->config_cur_ptr;
  if (p_config == NULL) {
    return (false);
  }

  recipient = p_dev->setup_req.bmRequestType & SL_USBD_REQ_RECIPIENT_MASK;

  if (recipient == SL_USBD_REQ_RECIPIENT_INTERFACE) {
    if_nbr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
  } else {
    ep_addr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
    if_nbr = p_dev->endpoint_interface_table[ep_phy_nbr];
  }

  p_if = usbd_core_get_interface_structure(p_config, if_nbr);
  if (p_if == NULL) {
    SLI_USBD_DBG_STATS_DEV_INC(std_req_class_stall_nbr);
    return (false);
  }

  p_class_drv = p_if->class_driver_ptr;
  if (p_class_drv->class_req == NULL) {
    SLI_USBD_DBG_STATS_DEV_INC(std_req_class_stall_nbr);
    return (false);
  }

  valid = p_class_drv->class_req(&p_dev->setup_req,
                                 p_if->class_arg_ptr);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(std_req_class_stall_nbr, (valid == false));

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_core_vendor_stdreq()
 *
 * @brief    Vendor standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *******************************************************************************************************/
static bool usbd_core_vendor_stdreq(const sli_usbd_device_t *p_dev)
{
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sl_usbd_class_driver_t *p_class_drv;
  uint8_t recipient;
  uint8_t if_nbr;
  uint8_t ep_addr;
  uint8_t ep_phy_nbr;
  bool valid;

  p_config = p_dev->config_cur_ptr;
  if (p_config == NULL) {
    return (false);
  }

  recipient = p_dev->setup_req.bmRequestType & SL_USBD_REQ_RECIPIENT_MASK;

  if (recipient == SL_USBD_REQ_RECIPIENT_INTERFACE) {
    if_nbr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
  } else {
    ep_addr = (uint8_t)(p_dev->setup_req.wIndex & 0xFFu);
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
    if_nbr = p_dev->endpoint_interface_table[ep_phy_nbr];
  }

  p_if = usbd_core_get_interface_structure(p_config, if_nbr);
  if (p_if == NULL) {
    return (false);
  }

  p_class_drv = p_if->class_driver_ptr;
  if (p_class_drv->vendor_req == NULL) {
    return (false);
  }

  valid = p_class_drv->vendor_req(&p_dev->setup_req,
                                  p_if->class_arg_ptr);

  return (valid);
}

/****************************************************************************************************//**
 *                                           usbd_core_microsoft_device_stdreq()
 *
 * @brief    Microsoft descriptor request handler (when recipient is device).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
static bool usbd_core_microsoft_device_stdreq(const sli_usbd_device_t *p_dev)
{
  bool valid;
  uint8_t if_nbr;
  uint8_t max_if;
  uint8_t if_ix;
  uint8_t compat_id_ix;
  uint8_t subcompat_id_ix;
  uint8_t section_cnt;
  uint16_t feature;
  uint16_t len;
  uint8_t cfg_nbr = 0u;
  uint32_t desc_len;
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sl_usbd_class_driver_t *p_class_drv;
  sl_status_t status;

  valid = false;
  feature = p_dev->setup_req.wIndex;
  if_nbr = (uint8_t)(p_dev->setup_req.wValue & 0xFFu);
  len = p_dev->setup_req.wLength;

  // Use 1st cfg as Microsoft doesn't specify cfg in setup pkt.
#if (USBD_CFG_HS_EN == 1)
  if (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH) {
    p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr | SL_USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr);
#if (USBD_CFG_HS_EN == 1)
}
#endif
  if (p_config == NULL) {
    return (false);
  }

  switch (feature) {
    case SLI_USBD_MICROSOFT_FEATURE_COMPAT_ID:
      // See note (1).
      // Send Desc Header
      // Compute length of descriptor.
      desc_len = SLI_USBD_MICROSOFT_DESC_COMPAT_ID_HDR_LEN;
      section_cnt = 0u;
      if (if_nbr == 0u) {
        // If req IF == 0, sends all dev compat IDs.
        max_if = p_config->interface_nbr_total;
      } else {
        max_if = if_nbr + 1u;
      }

      for (if_ix = if_nbr; if_ix < max_if; if_ix++) {
        p_if = usbd_core_get_interface_structure(p_config, if_ix);
        p_class_drv = p_if->class_driver_ptr;
        if (p_class_drv->microsoft_get_compat_id != NULL) {
          compat_id_ix = p_class_drv->microsoft_get_compat_id(&subcompat_id_ix);
          if (compat_id_ix != SL_USBD_MICROSOFT_COMPAT_ID_NONE) {
            desc_len += SLI_USBD_MICROSOFT_DESC_COMPAT_ID_SECTION_LEN;
            section_cnt++;
          }
        }
      }

      // Wr desc hdr.
      usbd_core_start_descriptor_write((sli_usbd_device_t *)p_dev, desc_len);

      sl_usbd_core_write_32b_to_descriptor_buf(desc_len);
      sl_usbd_core_write_16b_to_descriptor_buf(SLI_USBD_MICROSOFT_DESC_VER_1_0);
      sl_usbd_core_write_16b_to_descriptor_buf(feature);
      sl_usbd_core_write_08b_to_descriptor_buf(section_cnt);
      // Add 7 null bytes (reserved).
      sl_usbd_core_write_32b_to_descriptor_buf(0u);
      sl_usbd_core_write_16b_to_descriptor_buf(0u);
      sl_usbd_core_write_08b_to_descriptor_buf(0u);

      // Send Desc Sections
      if (len != SLI_USBD_MICROSOFT_DESC_COMPAT_ID_HDR_VER_1_0) {
        // If req len = version, only send desc hdr.
        for (if_ix = if_nbr; if_ix < max_if; if_ix++) {
          p_if = usbd_core_get_interface_structure(p_config, if_ix);
          if (p_if->class_driver_ptr->microsoft_get_compat_id != NULL) {
            compat_id_ix = p_if->class_driver_ptr->microsoft_get_compat_id(&subcompat_id_ix);
            if (compat_id_ix != SL_USBD_MICROSOFT_COMPAT_ID_NONE) {
              sl_usbd_core_write_08b_to_descriptor_buf(if_ix);
              sl_usbd_core_write_08b_to_descriptor_buf(0x01u);

              sl_usbd_core_write_buf_to_descriptor_buf((uint8_t *)usbd_microsoft_compat_id[compat_id_ix],
                                                       8u);

              sl_usbd_core_write_buf_to_descriptor_buf((uint8_t *)usbd_microsoft_subcompat_id[subcompat_id_ix],
                                                       8u);

              // Add 6 null bytes (reserved).
              sl_usbd_core_write_32b_to_descriptor_buf(0u);
              sl_usbd_core_write_16b_to_descriptor_buf(0u);
            }
          }
        }
      }

      if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
        status = usbd_core_stop_descriptor_write((sli_usbd_device_t *)p_dev);
        *(p_dev->desc_buf_status_ptr) = status;
      }

      if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
        valid = true;
      }
      break;

    case SLI_USBD_MICROSOFT_FEATURE_EXT_PROPERTIES:
      valid = usbd_core_microsoft_ext_property_stdreq(p_dev,
                                                      p_config,
                                                      if_nbr,
                                                      len);
      break;

    default:
      break;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           usbd_core_microsoft_interface_stdreq()
 *
 * @brief    Microsoft descriptor request handler (when recipient is interface).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
static bool usbd_core_microsoft_interface_stdreq(const sli_usbd_device_t *p_dev)
{
  bool valid = false;
  uint8_t if_nbr = (uint8_t)(p_dev->setup_req.wValue & 0xFFu);
  uint16_t feature = p_dev->setup_req.wIndex;
  uint16_t len = p_dev->setup_req.wLength;
  sli_usbd_configuration_t *p_config;

  // Use 1st cfg as Microsoft doesn't specify cfg in setup pkt.
#if (USBD_CFG_HS_EN == 1)
  if (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH) {
    p_config = usbd_core_get_configuration_structure(p_dev, 0u | SL_USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = usbd_core_get_configuration_structure(p_dev, 0u);
#if (USBD_CFG_HS_EN == 1)
}
#endif
  if (p_config == NULL) {
    return (false);
  }

  switch (feature) {
    case SLI_USBD_MICROSOFT_FEATURE_EXT_PROPERTIES:
      valid = usbd_core_microsoft_ext_property_stdreq(p_dev,
                                                      p_config,
                                                      if_nbr,
                                                      len);
      break;

    default:
      break;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           usbd_core_microsoft_ext_property_stdreq()
 *
 * @brief    Microsoft descriptor request handler (when recipient is interface).
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    if_nbr  Interface number.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    len     Length of descriptor as requested by host.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *
 * @note     (1) For more information on Microsoft OS decriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) Page feature is not supported so Microsoft OS descriptors have their length limited
 *               to 64Kbytes.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
static bool usbd_core_microsoft_ext_property_stdreq(const sli_usbd_device_t     *p_dev,
                                                    sli_usbd_configuration_t    *p_config,
                                                    uint8_t                     if_nbr,
                                                    uint16_t                    len)
{
  bool valid = false;
  uint8_t section_cnt = 0u;
  uint8_t ext_property_cnt;
  uint8_t ext_property_ix;
  uint32_t desc_len = SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_HDR_LEN;
  sli_usbd_interface_t *p_if;
  sl_usbd_class_driver_t *p_class_drv;
  sl_usbd_microsoft_ext_property_t *p_ext_property;
  sl_status_t status;

  // Send Desc Header
  // Compute length of descriptor.
  p_if = usbd_core_get_interface_structure(p_config, if_nbr);
  p_class_drv = p_if->class_driver_ptr;

  if (p_class_drv->microsoft_get_ext_property_table != NULL) {
    ext_property_cnt = p_class_drv->microsoft_get_ext_property_table(&p_ext_property, if_nbr);
    for (ext_property_ix = 0u; ext_property_ix < ext_property_cnt; ext_property_ix++) {
      desc_len += SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_SECTION_HDR_LEN;
      desc_len += p_ext_property->property_name_len;
      desc_len += p_ext_property->property_len;
      desc_len += 6u;

      section_cnt++;
      p_ext_property++;
    }
  }

  usbd_core_start_descriptor_write((sli_usbd_device_t *)p_dev, desc_len);

  sl_usbd_core_write_32b_to_descriptor_buf(desc_len);
  sl_usbd_core_write_16b_to_descriptor_buf(SLI_USBD_MICROSOFT_DESC_VER_1_0);
  sl_usbd_core_write_16b_to_descriptor_buf(SLI_USBD_MICROSOFT_FEATURE_EXT_PROPERTIES);
  sl_usbd_core_write_16b_to_descriptor_buf(section_cnt);

  // Send Desc Sections
  // If req len = version, only send desc hdr.
  if ((len != SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_HDR_VER_1_0)
      && (p_class_drv->microsoft_get_ext_property_table != NULL)) {
    ext_property_cnt = p_class_drv->microsoft_get_ext_property_table(&p_ext_property, if_nbr);
    for (ext_property_ix = 0u; ext_property_ix < ext_property_cnt; ext_property_ix++) {
      // Compute desc section len.
      desc_len = SLI_USBD_MICROSOFT_DESC_EXT_PROPERTIES_SECTION_HDR_LEN;
      desc_len += p_ext_property->property_name_len;
      desc_len += p_ext_property->property_len;
      desc_len += 6u;

      // Wr desc section.
      sl_usbd_core_write_32b_to_descriptor_buf(desc_len);
      sl_usbd_core_write_32b_to_descriptor_buf(p_ext_property->property_type);

      sl_usbd_core_write_16b_to_descriptor_buf(p_ext_property->property_name_len);
      sl_usbd_core_write_buf_to_descriptor_buf((uint8_t *)p_ext_property->property_name_ptr,
                                               p_ext_property->property_name_len);

      sl_usbd_core_write_32b_to_descriptor_buf(p_ext_property->property_len);
      sl_usbd_core_write_buf_to_descriptor_buf((uint8_t *)p_ext_property->property_ptr,
                                               p_ext_property->property_len);

      p_ext_property++;
    }
  }

  if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
    status = usbd_core_stop_descriptor_write((sli_usbd_device_t *)p_dev);
    *(p_dev->desc_buf_status_ptr) = status;
  }

  if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
    valid = true;
  }

  return (valid);
}
#endif

/****************************************************************************************************//**
 *                                           usbd_core_get_descriptor_stdreq()
 *
 * @brief    GET_DESCRIPTOR standard request handler.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   true,   if no error(s) occurred and request is supported.
 *           false, if any errors are returned.
 *******************************************************************************************************/
static bool usbd_core_get_descriptor_stdreq(sli_usbd_device_t *p_dev)
{
  uint8_t desc_type;
  uint8_t desc_ix;
  uint16_t req_len;
  bool valid;
  sl_status_t local_status;
  sl_status_t status;
#if (USBD_CFG_HS_EN == 1)
  sl_usbd_device_speed_t drv_spd;
#endif

  local_status = SL_STATUS_OK;

  desc_type = (uint8_t)((p_dev->setup_req.wValue >> 8u) & 0xFFu);
  desc_ix = (uint8_t)(p_dev->setup_req.wValue        & 0xFFu);
  valid = false;
  req_len = p_dev->setup_req.wLength;
  // Set the desc buf as the current buf.
  p_dev->actual_buf_ptr = p_dev->desc_buf_ptr;
  // Set the max len for the desc buf.
  p_dev->desc_buf_max_len = SLI_USBD_DESC_BUF_LEN;
  p_dev->desc_buf_status_ptr = &local_status;

  switch (desc_type) {
    case SL_USBD_DESC_TYPE_DEVICE:
      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (Device)"));
      status = usbd_core_send_device_descriptor(p_dev, false, req_len);
      if (status == SL_STATUS_OK) {
        valid = true;
      }
      break;

    case SL_USBD_DESC_TYPE_CONFIGURATION:
      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (Configuration) ix #", (u)desc_ix));
      status = usbd_core_send_configuration_descriptor(p_dev, desc_ix, false, req_len);
      if (status != SL_STATUS_OK) {
        SLI_USBD_LOG_ERR(("USBD Get Descriptor (Configuration) Failed. status = ", status));
      } else {
        valid = true;
      }
      break;

    case SL_USBD_DESC_TYPE_STRING:
      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (String) ix #", (u)desc_ix));
#if (USBD_CFG_STR_EN == 1)
      status = usbd_core_send_string_descriptor(p_dev, desc_ix, req_len);
      if (status == SL_STATUS_OK) {
        valid = true;
      }
#endif
      break;

    case SL_USBD_DESC_TYPE_DEVICE_QUALIFIER:
      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (Device Qualifier)"));
#if (USBD_CFG_HS_EN == 1)
      sli_usbd_driver_get_speed(&drv_spd);

      // Chk if dev only supports FS.
      if (drv_spd == SL_USBD_DEVICE_SPEED_FULL) {
        break;
      }

      status = usbd_core_send_device_descriptor(p_dev, true, req_len);

      if (status == SL_STATUS_OK) {
        valid = true;
      }
#endif
      break;

    case SL_USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      SLI_USBD_LOG_VRB(("USBD: Get Descriptor (Other Speed)"));
#if (USBD_CFG_HS_EN == 1)
      sli_usbd_driver_get_speed(&drv_spd);

      if (drv_spd == SL_USBD_DEVICE_SPEED_FULL) {
        break;
      }

      status = usbd_core_send_configuration_descriptor(p_dev, desc_ix, true, req_len);

      if (status == SL_STATUS_OK) {
        valid = true;
      }
#endif
      break;

    default:
      break;
  }

  p_dev->desc_buf_status_ptr = NULL;
  return (valid);
}

/****************************************************************************************************//**
 *                                               usbd_core_unset_configuration()
 *
 * @brief    Close current device configuration.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue and
 *                   'sl_usbd_core_stop_device()' function.
 *******************************************************************************************************/
static void usbd_core_unset_configuration(sli_usbd_device_t *p_dev)
{
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sli_usbd_alt_interface_t *p_if_alt;
  uint8_t if_nbr;
  CORE_DECLARE_IRQ_STATE;

  p_config = p_dev->config_cur_ptr;
  if (p_config == NULL) {
    return;
  }

  // Notify app about clr cfg.
  sl_usbd_on_config_event(SL_USBD_EVENT_CONFIG_UNSET, p_dev->config_cur_nbr);

  for (if_nbr = 0u; if_nbr < p_config->interface_nbr_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    if (p_if == NULL) {
      return;
    }

    p_if_alt = p_if->alt_cur_ptr;
    if (p_if_alt == NULL) {
      return;
    }

    if (p_if->class_driver_ptr->disable != NULL) {
      // Notify class that cfg is not active.
      p_if->class_driver_ptr->disable(p_dev->config_cur_nbr,
                                      p_if->class_arg_ptr);
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->state = SL_USBD_DEVICE_STATE_ADDRESSED;
  CORE_EXIT_ATOMIC();

  for (if_nbr = 0u; if_nbr < p_config->interface_nbr_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    if (p_if == NULL) {
      return;
    }

    p_if_alt = p_if->alt_cur_ptr;
    if (p_if_alt == NULL) {
      return;
    }

    usbd_core_close_alt_interface(p_dev, p_if_alt);

    p_if_alt = usbd_core_get_alt_interface_structure(p_if, 0u);

    CORE_ENTER_ATOMIC();
    p_if->alt_cur_ptr = p_if_alt;
    p_if->alt_cur = 0u;
    CORE_EXIT_ATOMIC();
  }

  CORE_ENTER_ATOMIC();
  p_dev->config_cur_ptr = NULL;
  p_dev->config_cur_nbr = SL_USBD_CONFIG_NBR_NONE;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               usbd_core_set_configuration()
 *
 * @brief    Open specified configuration.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    config_nbr  Configuration number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static sl_status_t usbd_core_set_configuration(sli_usbd_device_t   *p_dev,
                                               uint8_t             config_nbr)

{
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sli_usbd_alt_interface_t *p_if_alt;
  uint8_t if_nbr;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_HS_EN == 1)
  if (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH) {
    p_config = usbd_core_get_configuration_structure(p_dev, config_nbr | SL_USBD_CONFIG_NBR_SPD_BIT);
  } else {
#endif
  p_config = usbd_core_get_configuration_structure(p_dev, config_nbr);
#if (USBD_CFG_HS_EN == 1)
}
#endif

  if (p_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  for (if_nbr = 0u; if_nbr < p_config->interface_nbr_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    if (p_if == NULL) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    p_if_alt = p_if->alt_cur_ptr;
    if (p_if_alt == NULL) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    status = usbd_core_open_alt_interface(p_dev, if_nbr, p_if_alt);

    if (status != SL_STATUS_OK) {
      return status;
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->config_cur_ptr = p_config;
  p_dev->config_cur_nbr = config_nbr;
  CORE_EXIT_ATOMIC();

  CORE_ENTER_ATOMIC();
  p_dev->state = SL_USBD_DEVICE_STATE_CONFIGURED;
  CORE_EXIT_ATOMIC();

  for (if_nbr = 0u; if_nbr < p_config->interface_nbr_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    if (p_if == NULL) {
      return SL_STATUS_INVALID_PARAMETER;
    } else {
      if (p_if->class_driver_ptr->enable != NULL) {
        // Notify class that cfg is active.
        p_if->class_driver_ptr->enable(config_nbr,
                                       p_if->class_arg_ptr);
      }
    }
  }

  // Notify app about set cfg.
  sl_usbd_on_config_event(SL_USBD_EVENT_CONFIG_SET, config_nbr);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                           usbd_core_send_device_descriptor()
 *
 * @brief    Send device configuration descriptor.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    other       Other speed configuration :
 *                           - true     Current speed.
 *                           - false      Other operational speed.
 *
 * @param    req_len     Requested length by the host.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB Spec 2.0 table 9-8 describes the standard device descriptor.
 *
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               | Offset |        Field       |  size |   Value  |            Description            |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    0   | bLength            |   1   | Number   | size of this descriptor           |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    1   | bDescriptorType    |   1   | Const    | DEVICE Descriptor type            |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    2   | bcdUSB             |   2   | BCD USB  | Specification release number      |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    4   | bDeviceClass       |   1   | Class    | Class code.                       |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    5   | bDeviceSubClass    |   1   | SubClass | Subclass code.                    |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    6   | bDeviceProtocol    |   1   | protocol | protocol code.                    |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    7   | bMaxPacketSize0    |   1   | Number   | Max packet size for EP zero       |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |    8   | idVendor           |   2   | id       | Vendor  id                        |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   10   | idProduct          |   2   | id       | Product id                        |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   12   | bcdDevice          |   2   | BCD      | Dev release number in BCD format  |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   14   | iManufacturer      |   1   | Index    | Index of manufacturer string      |
 *               +--------+--------------------+-------+----------+-----------------------------------+
 *               |   15   | iProduct           |   1   | Index    | Index of product string           |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *               |   16   | iSerialNumber      |   1   | Index    | Index of serial number string     |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *               |   17   | bNumConfigurations |   1   |  Number  | Number of possible configurations |
 *               +--------|--------------------|-------|----------|-----------------------------------+
 *
 *           ) To enable host to identify devices that use the Interface Association descriptor the
 *           device descriptor should contain the following values.
 *******************************************************************************************************/
static sl_status_t usbd_core_send_device_descriptor(sli_usbd_device_t    *p_dev,
                                                    bool                 other,
                                                    uint16_t             req_len)
{
  sli_usbd_configuration_t *p_config;
  bool if_grp_en;
  uint8_t cfg_nbr;
  uint8_t cfg_nbr_spd;
  uint8_t cfg_nbr_total;
  sl_usbd_device_speed_t drv_spd;
#if (USBD_CFG_STR_EN == 1)
  uint8_t str_ix;
#endif

#if (USBD_CFG_HS_EN == 0)
  (void)&other;
  (void)&drv_spd;
#endif

  if_grp_en = false;

#if (USBD_CFG_HS_EN == 1)
  if (other == false) {
#endif
  usbd_core_start_descriptor_write(p_dev, req_len);
  // Desc len.
  usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_DEV);
  // Dev desc type.
  usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_DEVICE);
  // USB spec release nbr in BCD fmt (2.00).
  usbd_core_write_16b_to_descriptor_buf(p_dev, 0x200u);

#if (USBD_CFG_HS_EN == 1)
  if (p_dev->speed == SL_USBD_DEVICE_SPEED_FULL) {
#endif
  cfg_nbr_spd = 0x00u;
  cfg_nbr_total = p_dev->config_fs_total_nbr;
#if (USBD_CFG_HS_EN == 1)
} else {
  cfg_nbr_spd = SL_USBD_CONFIG_NBR_SPD_BIT;
  cfg_nbr_total = p_dev->config_hs_total_nbr;
}
#endif

  cfg_nbr = 0u;
  while ((cfg_nbr < cfg_nbr_total)
         && (if_grp_en == false)) {
    p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr | cfg_nbr_spd);
    if (p_config != NULL) {
      if (p_config->interface_group_nbr_total > 0u) {
        if_grp_en = true;
      }
    }

    cfg_nbr++;
  }

  if (if_grp_en == false) {
    // Dev class is specified in IF desc.
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_CLASS_CODE_USE_IF_DESC);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_SUBCLASS_CODE_USE_IF_DESC);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_PROTOCOL_CODE_USE_IF_DESC);
  } else {
    // Multi-Interface function dev class.
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_CLASS_CODE_MISCELLANEOUS);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_SUBCLASS_CODE_USE_COMMON_CLASS);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_PROTOCOL_CODE_USE_IAD);
  }
  // Set max pkt size for ctrl EP.
  usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)p_dev->endpoint_max_ctrl_pkt_size);
  // Set vendor id, product id and dev id.
  usbd_core_write_16b_to_descriptor_buf(p_dev, p_dev->device_config.vendor_id);
  usbd_core_write_16b_to_descriptor_buf(p_dev, p_dev->device_config.product_id);
  usbd_core_write_16b_to_descriptor_buf(p_dev, p_dev->device_config.device_bcd);

#if (USBD_CFG_STR_EN == 1)
  str_ix = usbd_core_get_string_index(p_dev, p_dev->device_config.manufacturer_str_ptr);
  usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
  str_ix = usbd_core_get_string_index(p_dev, p_dev->device_config.product_str_ptr);
  usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
  str_ix = usbd_core_get_string_index(p_dev, p_dev->device_config.serial_nbr_str_ptr);
  usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
#else
  usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
  usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
  usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
#endif
  usbd_core_write_08b_to_descriptor_buf(p_dev, cfg_nbr_total);

#if (USBD_CFG_HS_EN == 1)
} else {
  sli_usbd_driver_get_speed(&drv_spd);
  if (drv_spd != SL_USBD_DEVICE_SPEED_HIGH) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  usbd_core_start_descriptor_write(p_dev, req_len);
  // Desc len.
  usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_DEV_QUAL);
  usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_DEVICE_QUALIFIER);
  // USB spec release nbr in BCD fmt (2.00).
  usbd_core_write_16b_to_descriptor_buf(p_dev, 0x200u);

  if (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH) {
    cfg_nbr_spd = 0x00u;
    cfg_nbr_total = p_dev->config_fs_total_nbr;
  } else {
    cfg_nbr_spd = SL_USBD_CONFIG_NBR_SPD_BIT;
    cfg_nbr_total = p_dev->config_hs_total_nbr;
  }

  cfg_nbr = 0u;
  while ((cfg_nbr < cfg_nbr_total)
         && (if_grp_en == false)) {
    p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr | cfg_nbr_spd);
    if (p_config != NULL) {
      if (p_config->interface_group_nbr_total > 0u) {
        if_grp_en = true;
      }
      cfg_nbr++;
    }
  }
  if (if_grp_en == false) {
    // Dev class is specified in IF desc.
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_CLASS_CODE_USE_IF_DESC);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_SUBCLASS_CODE_USE_IF_DESC);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_PROTOCOL_CODE_USE_IF_DESC);
  } else {
    // Multi-Interface function dev class.
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_CLASS_CODE_MISCELLANEOUS);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_SUBCLASS_CODE_USE_COMMON_CLASS);
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_PROTOCOL_CODE_USE_IAD);
  }
  // Set max pkt size for ctrl EP.
  usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)p_dev->endpoint_max_ctrl_pkt_size);
  usbd_core_write_08b_to_descriptor_buf(p_dev, cfg_nbr_total);
  usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
}
#endif

  return usbd_core_stop_descriptor_write(p_dev);
}

/****************************************************************************************************//**
 *                                           usbd_core_send_configuration_descriptor()
 *
 * @brief    Send configuration descriptor.
 *
 * @param    p_dev       Pointer to device struct.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    cfg_nbr     Configuration number.
 *
 * @param    other       Other speed configuration :
 *                       false      Descriptor is build for the current speed.
 *                       true     Descriptor is build for the  other  speed.
 *
 * @param    req_len     Requested length by the host.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static sl_status_t usbd_core_send_configuration_descriptor(sli_usbd_device_t    *p_dev,
                                                           uint8_t              config_nbr,
                                                           bool                 other,
                                                           uint16_t             req_len)
{
  sli_usbd_configuration_t *p_config;
  sli_usbd_interface_t *p_if;
  sli_usbd_endpoint_info_t *p_ep;
  sli_usbd_alt_interface_t *p_if_alt;
  sli_usbd_interface_group_t *p_if_grp;
  sl_usbd_class_driver_t *p_if_drv;
  uint8_t cfg_nbr_cur;
  uint8_t ep_nbr;
  uint8_t if_nbr;
  uint8_t if_total;
  uint8_t if_grp_cur;
  uint8_t if_alt_nbr;
  uint8_t attrib;
#if (USBD_CFG_STR_EN == 1)
  uint8_t str_ix;
#endif
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  uint32_t ep_alloc_map;
#endif

#if (USBD_CFG_HS_EN == 1)
  if (p_dev->speed == SL_USBD_DEVICE_SPEED_HIGH) {
    cfg_nbr_cur = config_nbr | SL_USBD_CONFIG_NBR_SPD_BIT;
  } else {
#endif
  cfg_nbr_cur = config_nbr;
#if (USBD_CFG_HS_EN == 1)
}
#endif

  p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr_cur);

  if (p_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if (USBD_CFG_HS_EN == 1)
  // other will always be false when HS is disabled.
  if (other == true) {
    if (p_config->config_other_speed == SL_USBD_CONFIG_NBR_NONE) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    cfg_nbr_cur = p_config->config_other_speed;

    // Retrieve cfg struct for other spd.
    p_config = usbd_core_get_configuration_structure(p_dev, cfg_nbr_cur);
    if (p_config == NULL) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
#endif

  // Init cfg desc len.
  p_config->desc_len = SL_USBD_DESC_LEN_CFG;

  // Build Configuration Descriptor
  usbd_core_start_descriptor_write(p_dev, req_len);

  // Desc len.
  usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_CFG);
  // Desc type.
  if (other == true) {
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION);
  } else {
    usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_CONFIGURATION);
  }

  if_total = p_config->interface_nbr_total;
  if_grp_cur = SL_USBD_INTERFACE_GROUP_NBR_NONE;

  for (if_nbr = 0u; if_nbr < if_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    p_if_drv = p_if->class_driver_ptr;

    if ((p_if->group_nbr != if_grp_cur)
        && (p_if->group_nbr != SL_USBD_INTERFACE_GROUP_NBR_NONE)) {
      // Add IF assoc desc len.
      p_config->desc_len += SL_USBD_DESC_LEN_IF_ASSOCIATION;
      if_grp_cur = p_if->group_nbr;
    }

    p_config->desc_len += (SL_USBD_DESC_LEN_IF * p_if->alt_nbr_total);

    for (if_alt_nbr = 0u; if_alt_nbr < p_if->alt_nbr_total; if_alt_nbr++) {
      p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);
      p_config->desc_len += (SL_USBD_DESC_LEN_EP * p_if_alt->endpoint_nbr_total);

      // Add IF functional desc len.
      if (p_if_drv->interface_get_descriptor_size != NULL) {
        p_config->desc_len += p_if_drv->interface_get_descriptor_size(cfg_nbr_cur,
                                                                      if_nbr,
                                                                      if_alt_nbr,
                                                                      p_if->class_arg_ptr,
                                                                      p_if_alt->class_arg_ptr);
      }

#if (USBD_CFG_OPTIMIZE_SPD == 1)
      ep_alloc_map = p_if_alt->endpoint_table_map;
      while (ep_alloc_map != 0x00u) {
        ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
        p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];

        // Add EP functional desc len.
        if (p_if_drv->endpoint_get_descriptor_size != NULL) {
          p_config->desc_len += p_if_drv->endpoint_get_descriptor_size(cfg_nbr_cur,
                                                                       if_nbr,
                                                                       if_alt_nbr,
                                                                       p_ep->address,
                                                                       p_if->class_arg_ptr,
                                                                       p_if_alt->class_arg_ptr);
        }

        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          p_config->desc_len += 2u;
        }

        SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
      }
#else
      p_ep = p_if_alt->endpoint_head_ptr;

      for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
        if (p_if_drv->endpoint_get_descriptor_size != NULL) {
          p_config->desc_len += p_if_drv->endpoint_get_descriptor_size(cfg_nbr_cur,
                                                                       if_nbr,
                                                                       if_alt_nbr,
                                                                       p_ep->address,
                                                                       p_if->class_arg_ptr,
                                                                       p_if_alt->class_arg_ptr);
        }

        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          p_config->desc_len += 2u;
        }

        p_ep = p_ep->next_ptr;
      }
#endif
    }
  }

  // Build Cfg Desc
  // Desc len.
  usbd_core_write_16b_to_descriptor_buf(p_dev, p_config->desc_len);
  // nbr of IF.
  usbd_core_write_08b_to_descriptor_buf(p_dev, p_config->interface_nbr_total);
  // Cfg ix.
  usbd_core_write_08b_to_descriptor_buf(p_dev, config_nbr + 1u);

#if (USBD_CFG_STR_EN == 1)
  // Add str ix.
  str_ix = usbd_core_get_string_index(p_dev, p_config->name_ptr);
  usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
#else
  usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
#endif

  attrib = SLI_USBD_CONFIG_DESC_RSVD_SET;
  if (SL_IS_BIT_SET(p_config->attrib, SL_USBD_DEV_ATTRIB_SELF_POWERED)) {
    SL_SET_BIT(attrib, SLI_USBD_CONFIG_DESC_SELF_POWERED);
  }
  if (SL_IS_BIT_SET(p_config->attrib, SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP)) {
    SL_SET_BIT(attrib, SLI_USBD_CONFIG_DESC_REMOTE_WAKEUP);
  }
  usbd_core_write_08b_to_descriptor_buf(p_dev, attrib);
  usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)((p_config->max_power + 1u) / 2u));

  // Build Interface Descriptor
  if_total = p_config->interface_nbr_total;
  if_grp_cur = SL_USBD_INTERFACE_GROUP_NBR_NONE;

  for (if_nbr = 0u; if_nbr < if_total; if_nbr++) {
    p_if = usbd_core_get_interface_structure(p_config, if_nbr);
    p_if_drv = p_if->class_driver_ptr;

    if ((p_if->group_nbr != if_grp_cur)
        && (p_if->group_nbr != SL_USBD_INTERFACE_GROUP_NBR_NONE)) {
      // Add IF assoc desc (IAD).
      p_if_grp = usbd_core_get_interface_group_structure(p_config, p_if->group_nbr);

      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_IF_ASSOCIATION);
      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_IAD);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_grp->interface_start);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_grp->interface_count);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_grp->class_code);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_grp->class_sub_code);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_grp->class_protocol_code);

#if (USBD_CFG_STR_EN == 1)
      str_ix = usbd_core_get_string_index(p_dev, p_if_grp->name_ptr);
      usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
#else
      usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
#endif

      if_grp_cur = p_if->group_nbr;
    }
    // Add IF/alt settings desc.
    for (if_alt_nbr = 0u; if_alt_nbr < p_if->alt_nbr_total; if_alt_nbr++) {
      p_if_alt = usbd_core_get_alt_interface_structure(p_if, if_alt_nbr);

      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_IF);
      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_INTERFACE);
      usbd_core_write_08b_to_descriptor_buf(p_dev, if_nbr);
      usbd_core_write_08b_to_descriptor_buf(p_dev, if_alt_nbr);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if_alt->endpoint_nbr_total);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if->class_code);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if->class_sub_code);
      usbd_core_write_08b_to_descriptor_buf(p_dev, p_if->class_protocol_code);

#if (USBD_CFG_STR_EN == 1)
      str_ix = usbd_core_get_string_index(p_dev, p_if_alt->name_ptr);
      usbd_core_write_08b_to_descriptor_buf(p_dev, str_ix);
#else
      usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
#endif

      if (p_if_drv->interface_descriptor != NULL) {
        // Add class specific IF desc.
        p_if_drv->interface_descriptor(cfg_nbr_cur,
                                       if_nbr,
                                       if_alt_nbr,
                                       p_if->class_arg_ptr,
                                       p_if_alt->class_arg_ptr);
      }
      // Build EP Desc
#if (USBD_CFG_OPTIMIZE_SPD == 1)
      ep_alloc_map = p_if_alt->endpoint_table_map;
      while (ep_alloc_map != 0x00u) {
        ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
        p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];

        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_EP + 2u);
        } else {
          usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_EP);
        }

        usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_ENDPOINT);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->address);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->attrib);
        usbd_core_write_16b_to_descriptor_buf(p_dev, p_ep->max_pkt_size);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->interval);

        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->sync_refresh);
          usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->sync_addr);
        }

        if (p_if_drv->endpoint_descriptor != NULL) {
          // Add class specific EP desc.
          p_if_drv->endpoint_descriptor(cfg_nbr_cur,
                                        if_nbr,
                                        if_alt_nbr,
                                        p_ep->address,
                                        p_if->class_arg_ptr,
                                        p_if_alt->class_arg_ptr);
        }

        SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
      }
#else
      p_ep = p_if_alt->endpoint_head_ptr;

      for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_EP + 2u);
        } else {
          usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_LEN_EP);
        }

        usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_ENDPOINT);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->address);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->attrib);
        usbd_core_write_16b_to_descriptor_buf(p_dev, p_ep->max_pkt_size);
        usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->interval);

        if ((p_if->class_code == SL_USBD_CLASS_CODE_AUDIO)
            && (p_if->class_protocol_code == 0u)
            && (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC)
                || ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_INTR))) {
          // EP desc on audio class v1.0 has 2 additional fields.
          usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->sync_refresh);
          usbd_core_write_08b_to_descriptor_buf(p_dev, p_ep->sync_addr);
        }

        if (p_if_drv->endpoint_descriptor != NULL) {
          // Add class specific EP desc.
          p_if_drv->endpoint_descriptor(cfg_nbr_cur,
                                        if_nbr,
                                        if_alt_nbr,
                                        p_ep->address,
                                        p_if->class_arg_ptr,
                                        p_if_alt->class_arg_ptr);
        }

        p_ep = p_ep->next_ptr;
      }
#endif
    }
  }

  return usbd_core_stop_descriptor_write(p_dev);
}

/****************************************************************************************************//**
 *                                           usbd_core_send_string_descriptor()
 *
 * @brief    Send string descriptor.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    str_ix      String index.
 *
 * @param    req_len     Requested length by the host.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == 1)
static sl_status_t usbd_core_send_string_descriptor(sli_usbd_device_t   *p_dev,
                                                    uint8_t             str_ix,
                                                    uint16_t            req_len)
{
  const char *p_str;
  size_t len;
#if (USBD_CFG_MS_OS_DESC_EN == 1)
  uint8_t ix;
#endif

  usbd_core_start_descriptor_write(p_dev, req_len);

  switch (str_ix) {
    case 0u:
      usbd_core_write_08b_to_descriptor_buf(p_dev, 4u);
      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_STRING);
      usbd_core_write_16b_to_descriptor_buf(p_dev, p_dev->device_config.lang_id);
      break;

#if (USBD_CFG_MS_OS_DESC_EN == 1)
    case SLI_USBD_MICROSOFT_STR_IX:
      usbd_core_write_08b_to_descriptor_buf(p_dev, SLI_USBD_MICROSOFT_STR_LEN);
      usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_STRING);

      for (ix = 0u; ix < 7u; ix++) {
        usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)usbd_microsoft_signature_str[ix]);
        usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
      }

      usbd_core_write_08b_to_descriptor_buf(p_dev, p_dev->str_microsoft_vendor_code);
      usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);
      break;
#endif

    default:
      p_str = usbd_core_get_string_descriptor(p_dev, str_ix - 1u);
      if (p_str != NULL) {
        len = sl_strlen((char *)p_str);
        len = (2u * len) + 2u;
        len = SLI_USBD_GET_MIN(len, 255u);
        len = len - (len % 2u);

        usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)len);
        usbd_core_write_08b_to_descriptor_buf(p_dev, SL_USBD_DESC_TYPE_STRING);

        while (*p_str != '\0') {
          usbd_core_write_08b_to_descriptor_buf(p_dev, (uint8_t)*p_str);
          usbd_core_write_08b_to_descriptor_buf(p_dev, 0u);

          p_str++;
        }
      } else {
        return SL_STATUS_NULL_POINTER;
      }
      break;
  }

  return usbd_core_stop_descriptor_write(p_dev);
}
#endif

/****************************************************************************************************//**
 *                                           usbd_core_start_descriptor_write()
 *
 * @brief    Start write operation in the descriptor buffer.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    req_len     Requested length by the host.
 *******************************************************************************************************/
static void usbd_core_start_descriptor_write(sli_usbd_device_t   *p_dev,
                                             uint16_t            req_len)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_dev->desc_buf_index = 0u;
  p_dev->desc_buf_req_len = req_len;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               usbd_core_stop_descriptor_write()
 *
 * @brief    Stop write operation in the descriptor buffer.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----   Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function might be called in two contexts: when a Get Descriptor standard request
 *               is received, or when a driver supporting standard request auto-reply queries the
 *               device, a configuration or a string descriptor. The descriptor needs to be sent on
 *               control endpoint 0 only if this function is called for a Get Descriptor standard
 *               request. If the function is called when a driver needs the descriptor, nothing has to
 *               be done.
 *******************************************************************************************************/
static sl_status_t usbd_core_stop_descriptor_write(sli_usbd_device_t *p_dev)
{
  uint32_t xfer_len;
  sl_status_t status;

  status = SL_STATUS_OK;

  // See Note #1.
  if (p_dev->actual_buf_ptr == p_dev->desc_buf_ptr) {
    if (p_dev->desc_buf_index > 0u) {
      uint32_t std_req_timeout;
      CORE_DECLARE_IRQ_STATE;

      CORE_ENTER_ATOMIC();
      std_req_timeout = usbd_ptr->std_req_timeout_ms;
      CORE_EXIT_ATOMIC();

      status = sl_usbd_core_write_control_sync(&p_dev->desc_buf_ptr[0u],
                                               (uint32_t)p_dev->desc_buf_index,
                                               std_req_timeout,
                                               (p_dev->desc_buf_req_len > 0u) ? true : false,
                                               &xfer_len);
    }
  }

  return status;
}

/****************************************************************************************************//**
 *                                           usbd_core_write_08b_to_descriptor_buf()
 *
 * @brief    Write 8-bit value in the descriptor buffer.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    val     8-bit value.
 *******************************************************************************************************/
static void usbd_core_write_08b_to_descriptor_buf(sli_usbd_device_t   *p_dev,
                                                  uint8_t             val)
{
  if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
    usbd_core_write_to_descriptor_buf(p_dev, &val, 1u);
  }
}

/****************************************************************************************************//**
 *                                               sl_usbd_core_write_16b_to_descriptor_buf()
 *
 * @brief    Write 16-bit value in the descriptor buffer.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    val     16-bit value.
 *******************************************************************************************************/
static void usbd_core_write_16b_to_descriptor_buf(sli_usbd_device_t   *p_dev,
                                                  uint16_t            val)
{
  if (*(p_dev->desc_buf_status_ptr) == SL_STATUS_OK) {
    uint8_t buf[2u];

    buf[0u] = (uint8_t)(val        & 0xFFu);
    buf[1u] = (uint8_t)((val >> 8u) & 0xFFu);

    usbd_core_write_to_descriptor_buf(p_dev, &buf[0u], 2u);
  }
}

/****************************************************************************************************//**
 *                                               usbd_core_write_to_descriptor_buf()
 *
 * @brief    USB device configuration write request.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    p_buf   Pointer to data buffer.
 *
 * @param    len     Buffer length.
 *
 * @note     (1) This function might be called in two contexts: when a Get Descriptor standard request
 *               is received, or when a driver supporting standard request auto-reply queries the
 *               device, a configuration or a string descriptor. In the Get Descriptor standard
 *               request case, if the buffer is full, a transfer on control endpoint 0 is done, before
 *               resuming to fill the buffer. In the case of a driver supporting standard request
 *               auto-reply, if the buffer is full, an error is set and the function exits.
 *
 * @note     (2) If an error is reported by sl_usbd_core_write_control_sync() during the construction of the descriptor,
 *               this pointer will store the error code, stop the rest of the data phase, skip the
 *               status phase and ensure that the control endpoint 0 is stalled to notify the host
 *               that an error has occurred.
 *******************************************************************************************************/
static void usbd_core_write_to_descriptor_buf(sli_usbd_device_t         *p_dev,
                                              const uint8_t             *p_buf,
                                              uint16_t                  len)
{
  uint8_t *p_desc;
  uint8_t buf_cur_ix;
  uint16_t len_req;
  uint32_t xfer_len;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  p_desc = p_dev->actual_buf_ptr;
  buf_cur_ix = p_dev->desc_buf_index;
  len_req = p_dev->desc_buf_req_len;
  status = SL_STATUS_OK;

  while ((len_req != 0u)
         && (len != 0u)) {
    if (buf_cur_ix >= p_dev->desc_buf_max_len) {
      // Send data in response to std req. See Note #1.
      if (p_dev->actual_buf_ptr == p_dev->desc_buf_ptr) {
        uint32_t std_req_timeout;

        CORE_ENTER_ATOMIC();
        std_req_timeout = usbd_ptr->std_req_timeout_ms;
        CORE_EXIT_ATOMIC();

        buf_cur_ix = 0u;
        status = sl_usbd_core_write_control_sync(&p_dev->desc_buf_ptr[0u],
                                                 SLI_USBD_DESC_BUF_LEN,
                                                 std_req_timeout,
                                                 false,
                                                 &xfer_len);
        if (status != SL_STATUS_OK) {
          break;
        }
      } else {
        // Buf provided by driver is too small. See Note #1.
        len_req = 0u;
        status = SL_STATUS_ALLOCATION_FAILED;
      }
    } else {
      p_desc[buf_cur_ix] = *p_buf;
      p_buf++;
      len--;
      len_req--;
      buf_cur_ix++;
    }
  }

  CORE_ENTER_ATOMIC();
  p_dev->desc_buf_index = buf_cur_ix;
  p_dev->desc_buf_req_len = len_req;
  if (p_dev->desc_buf_status_ptr != NULL) {
    // See Note #2.
    *(p_dev->desc_buf_status_ptr) = status;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           usbd_core_get_configuration_structure()
 *
 * @brief    Get configuration structure.
 *
 * @param    p_dev       Pointer to device struct.
 *
 * @param    config_nbr  Configuration number.
 *
 * @return   Pointer to configuration structure, if no errors are returned.
 *
 *           Pointer to NULL,                    if any errors are returned.
 *******************************************************************************************************/
static sli_usbd_configuration_t *usbd_core_get_configuration_structure(const sli_usbd_device_t *p_dev,
                                                                       uint8_t                 config_nbr)
{
  sli_usbd_configuration_t *p_config;
  uint8_t cfg_val;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  uint8_t config_ix;
#endif

#if (USBD_CFG_HS_EN == 1)
  // SL_USBD_CONFIG_NBR_SPD_BIT will always be clear in FS.
  cfg_val = config_nbr & (uint8_t)(~SL_USBD_CONFIG_NBR_SPD_BIT);
#else
  cfg_val = config_nbr;
#endif

#if (USBD_CFG_OPTIMIZE_SPD == 1)  // Array implementation.
#if (USBD_CFG_HS_EN == 1)
  if (SL_IS_BIT_SET(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT) == true) {
    // Chk if cfg nbr is valid.
    if (cfg_val >= p_dev->config_hs_total_nbr) {
      return (NULL);
    }
    // Get HS cfg struct.
    p_config = p_dev->config_hs_speed_table_ptrs[cfg_val];
  } else {
#endif
  // Chk if cfg nbr is valid.
  if (cfg_val >= p_dev->config_fs_total_nbr) {
    return (NULL);
  }
  // Get FS cfg struct.
  p_config = p_dev->config_fs_speed_table_ptrs[cfg_val];
#if (USBD_CFG_HS_EN == 1)
}
#endif
#else  // Linked-list implementation.
#if (USBD_CFG_HS_EN == 1)
  if (SL_IS_BIT_SET(config_nbr, SL_USBD_CONFIG_NBR_SPD_BIT)) {
    // Chk if cfg nbr is valid.
    if (cfg_val >= p_dev->config_hs_total_nbr) {
      return (NULL);
    }
    p_config = p_dev->config_hs_head_ptr;
  } else {
#endif
  // Chk if cfg nbr is valid.
  if (cfg_val >= p_dev->config_fs_total_nbr) {
    return (NULL);
  }
  p_config = p_dev->config_fs_head_ptr;
#if (USBD_CFG_HS_EN == 1)
}
#endif

  // Iterate thru list until to get cfg struct.
  for (config_ix = 0u; config_ix < cfg_val; config_ix++) {
    p_config = p_config->next_ptr;
  }
#endif

  return (p_config);
}

/****************************************************************************************************//**
 *                                               usbd_core_set_event()
 *
 * @brief    Send an event to the core task.
 *
 * @param    event   Event code :
 *                   SLI_USBD_EVENT_BUS_RESET    reset.
 *                   SLI_USBD_EVENT_BUS_SUSPEND  suspend.
 *                   SLI_USBD_EVENT_BUS_RESUME   resume.
 *                   SLI_USBD_EVENT_BUS_CONNECT     Connect.
 *                   SLI_USBD_EVENT_BUS_DISCONNECT  Disconnect.
 *                   SLI_USBD_EVENT_BUS_HS       High speed.
 *                   SLI_USBD_EVENT_ENDPOINT           Endpoint.
 *                   SLI_USBD_EVENT_SETUP        Setup.
 *******************************************************************************************************/
static void usbd_core_set_event(sli_usbd_event_code_t event)
{
  sli_usbd_core_event_t core_event;

  core_event.type = event;
  core_event.status = SL_STATUS_OK;

  sli_usbd_core_os_put_core_event(&core_event);
}

/****************************************************************************************************//**
 * Processes all core events and core operations
 *******************************************************************************************************/
void sli_usbd_core_task_handler(void)
{
  sli_usbd_core_event_t core_event;
  sli_usbd_device_t *p_dev;
  uint8_t ep_addr;
  sli_usbd_event_code_t event;
  sl_status_t xfer_status;
  sl_status_t status;

#if SL_USBD_AUTO_START_USB_DEVICE == 1
  sl_usbd_core_start_device();
#endif

  // event loop
  while (true) {
    // Wait for an event.
    status = sli_usbd_core_os_get_core_event((void *) &core_event);
    if (status == SL_STATUS_OK) {
      event = core_event.type;
      p_dev = &usbd_ptr->device;

      // Decode event.
      switch (event) {
        // Bus Events
        case SLI_USBD_EVENT_BUS_RESET:
        case SLI_USBD_EVENT_BUS_RESUME:
        case SLI_USBD_EVENT_BUS_CONNECT:
        case SLI_USBD_EVENT_BUS_HS:
        case SLI_USBD_EVENT_BUS_SUSPEND:
        case SLI_USBD_EVENT_BUS_DISCONNECT:
          usbd_core_process_event(p_dev, event);
          break;

        // Endpoint Events
        case SLI_USBD_EVENT_ENDPOINT:
          if (p_dev->state == SL_USBD_DEVICE_STATE_SUSPENDED) {
            p_dev->state = p_dev->state_prev;
          }
          ep_addr = core_event.endpoint_address;
          xfer_status = core_event.status;
          sl_usbd_core_endpoint_transfer_async(ep_addr, xfer_status);
          break;

        // Setup Events
        case SLI_USBD_EVENT_SETUP:
          SLI_USBD_DBG_STATS_DEV_INC(device_setup_event_nbr);
          if (p_dev->state == SL_USBD_DEVICE_STATE_SUSPENDED) {
            p_dev->state = p_dev->state_prev;
          }
          usbd_core_stdreq_handler(p_dev);
          break;

        default:
          break;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           usbd_core_process_event()
 *
 * @brief    Processes bus related events.
 *
 * @param    p_dev   Pointer to USB device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    event   Bus related events :
 *                   SLI_USBD_EVENT_BUS_RESET    reset.
 *                   SLI_USBD_EVENT_BUS_SUSPEND  suspend.
 *                   SLI_USBD_EVENT_BUS_RESUME   resume.
 *                   SLI_USBD_EVENT_BUS_CONNECT     Connect.
 *                   SLI_USBD_EVENT_BUS_DISCONNECT  Disconnect.
 *                   SLI_USBD_EVENT_BUS_HS       High speed.
 *
 * @note     (1) This prevents a suspend event to overwrite the internal status with a suspend state in
 *               the case of multiple suspend events in a row.
 *
 * @note     (2) USB Spec 2.0 section 9.1.1.6 states "When suspended, the USB device maintains any
 *               internal status, including its address and configuration."
 *
 * @note     (3) A suspend event is usually followed by a resume event when the bus activity comes back.
 *               But in some cases, after a suspend event, a reset event can be notified to the Core
 *               before a resume event. Thus, the internal state of the device should not be changed
 *               to the previous one.
 *******************************************************************************************************/
static void usbd_core_process_event(sli_usbd_device_t        *p_dev,
                                    sli_usbd_event_code_t    event)
{
  CORE_DECLARE_IRQ_STATE;

  switch (event) {
    case SLI_USBD_EVENT_BUS_RESET:
      SLI_USBD_DBG_STATS_DEV_INC(device_reset_event_nbr);
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: reset"));

      CORE_ENTER_ATOMIC();
      if (p_dev->conn_status == false) {
        p_dev->conn_status = true;
        CORE_EXIT_ATOMIC();

        // Call application connect callback.
        sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_CONNECT);
      } else {
        CORE_EXIT_ATOMIC();
      }

      // Close ctrl EP.
      sli_usbd_core_close_control_endpoint();

      if (p_dev->config_cur_nbr != SL_USBD_CONFIG_NBR_NONE) {
        // Close curr cfg.
        usbd_core_unset_configuration(p_dev);
      }

      // Open ctrl EP.
      sli_usbd_core_open_control_endpoint(p_dev->endpoint_max_ctrl_pkt_size);

      // Set dev in default state, reset dev speed.
      CORE_ENTER_ATOMIC();
      p_dev->address = 0u;
      p_dev->state = SL_USBD_DEVICE_STATE_DEFAULT;
      p_dev->speed = SL_USBD_DEVICE_SPEED_FULL;
      CORE_EXIT_ATOMIC();

      // Call application reset callback.
      sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_RESET);

      break;

    case SLI_USBD_EVENT_BUS_SUSPEND:
      SLI_USBD_DBG_STATS_DEV_INC(device_suspend_event_nbr);
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: suspend"));

      CORE_ENTER_ATOMIC();
      // See Note #1.
      if (p_dev->state != SL_USBD_DEVICE_STATE_SUSPENDED) {
        // Save cur state (see Note #2).
        p_dev->state_prev = p_dev->state;
      }
      // Set  suspended state.
      p_dev->state = SL_USBD_DEVICE_STATE_SUSPENDED;
      CORE_EXIT_ATOMIC();

      // Call application suspend callback.
      sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_SUSPEND);
      break;

    case SLI_USBD_EVENT_BUS_RESUME:
      SLI_USBD_DBG_STATS_DEV_INC(device_resume_event_nbr);
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: resume"));

      CORE_ENTER_ATOMIC();
      // See Note #3.
      if (p_dev->state == SL_USBD_DEVICE_STATE_SUSPENDED) {
        // Restore prev state.
        p_dev->state = p_dev->state_prev;
      }
      CORE_EXIT_ATOMIC();

      // Call application resume callback.
      sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_RESUME);

      break;

    case SLI_USBD_EVENT_BUS_CONNECT:
      SLI_USBD_DBG_STATS_DEV_INC(device_conn_event_nbr);
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: Connect"));

      CORE_ENTER_ATOMIC();
      // Set attached state.
      p_dev->state = SL_USBD_DEVICE_STATE_ATTACHED;
      p_dev->conn_status = true;
      CORE_EXIT_ATOMIC();

      // Call application connect callback.
      sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_CONNECT);

      break;

    case SLI_USBD_EVENT_BUS_DISCONNECT:
      SLI_USBD_DBG_STATS_DEV_INC(device_disconnect_event_nbr);
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: Disconnect"));

      // Close ctrl EP.
      sli_usbd_core_close_control_endpoint();

      if (p_dev->config_cur_nbr != SL_USBD_CONFIG_NBR_NONE) {
        // Close curr cfg.
        usbd_core_unset_configuration(p_dev);
      }

      CORE_ENTER_ATOMIC();
      // Set default address.
      p_dev->address = 0u;
      // Dev is not attached.
      p_dev->state = SL_USBD_DEVICE_STATE_INIT;
      // No active cfg.
      p_dev->config_cur_nbr = SL_USBD_CONFIG_NBR_NONE;
      p_dev->conn_status = false;
      CORE_EXIT_ATOMIC();

      // Call application disconnect callback.
      sl_usbd_on_bus_event(SL_USBD_EVENT_BUS_DISCONNECT);

      break;

    case SLI_USBD_EVENT_BUS_HS:
      SLI_USBD_LOG_VRB_TO(SLI_USBD_LOG_BUS_CH, ("USBD Bus: High Speed detection"));
#if (USBD_CFG_HS_EN == 1)
      CORE_ENTER_ATOMIC();
      p_dev->speed = SL_USBD_DEVICE_SPEED_HIGH;
      if (p_dev->state == SL_USBD_DEVICE_STATE_SUSPENDED) {
        p_dev->state = p_dev->state_prev;
      }
      CORE_EXIT_ATOMIC();
#endif
      break;

    case SLI_USBD_EVENT_ENDPOINT:
    case SLI_USBD_EVENT_SETUP:
    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                               usbd_core_get_interface_structure()
 *
 * @brief    Gets the interface structure.
 *
 * @param    p_cfg   Pointer to configuration structure.
 *
 * @param    if_nbr  Interface number.
 *
 * @return   Pointer to interface structure, if no errors are returned.
 *
 *           Pointer to NULL,                if any errors are returned.
 *******************************************************************************************************/
static sli_usbd_interface_t *usbd_core_get_interface_structure(const sli_usbd_configuration_t *p_config,
                                                               uint8_t                        if_nbr)
{
  sli_usbd_interface_t *p_if;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  uint8_t if_ix;
#endif

  // Chk if IF nbr is valid.
  if (if_nbr >= p_config->interface_nbr_total) {
    return (NULL);
  }

  // Get IF struct.
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  p_if = p_config->interface_table_ptrs[if_nbr];
#else
  p_if = p_config->interface_head_ptr;

  for (if_ix = 0u; if_ix < if_nbr; if_ix++) {
    p_if = p_if->next_ptr;
  }
#endif

  return (p_if);
}

/****************************************************************************************************//**
 *                                           usbd_core_get_alt_interface_structure()
 *
 * @brief    Gets the alternate setting interface structure.
 *
 * @param    p_if        Pointer to interface structure.
 *
 * @param    if_alt_nbr  Alternate setting interface number.
 *
 * @return   Pointer to alternate setting interface structure, if no errors are returned.
 *
 *           Pointer to NULL,                                  if any errors are returned.
 *******************************************************************************************************/
static sli_usbd_alt_interface_t *usbd_core_get_alt_interface_structure(const sli_usbd_interface_t *p_if,
                                                                       uint8_t                    if_alt_nbr)
{
  sli_usbd_alt_interface_t *p_if_alt;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  uint8_t if_alt_ix;
#endif

  // Chk alt setting nbr.
  if (if_alt_nbr >= p_if->alt_nbr_total) {
    return (NULL);
  }

  // Get alt IF struct.
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  p_if_alt = p_if->alt_table_ptrs[if_alt_nbr];
#else
  p_if_alt = p_if->alt_head_ptr;

  for (if_alt_ix = 0u; if_alt_ix < if_alt_nbr; if_alt_ix++) {
    p_if_alt = p_if_alt->next_ptr;
  }
#endif

  return (p_if_alt);
}

/****************************************************************************************************//**
 *                                               usbd_core_open_alt_interface()
 *
 * @brief    Opens all endpoints from the specified alternate setting.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----     Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    if_nbr      Interface number.
 *
 * @param    p_if_alt    Pointer to alternate setting interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static sl_status_t usbd_core_open_alt_interface(sli_usbd_device_t              *p_dev,
                                                uint8_t                        if_nbr,
                                                const sli_usbd_alt_interface_t *p_if_alt)
{
  uint8_t ep_nbr;
  uint8_t ep_phy_nbr;
  bool valid;
  sli_usbd_endpoint_info_t *p_ep;
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  uint32_t ep_alloc_map;
#endif
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  valid = true;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  ep_alloc_map = p_if_alt->endpoint_table_map;
  while (ep_alloc_map != 0x00u) {
    ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
    p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);

    CORE_ENTER_ATOMIC();
    p_dev->endpoint_interface_table[ep_phy_nbr] = if_nbr;
    CORE_EXIT_ATOMIC();

    status = sli_usbd_core_open_endpoint(p_ep->address,
                                         p_ep->max_pkt_size,
                                         p_ep->attrib,
                                         p_ep->interval);
    if (status != SL_STATUS_OK) {
      valid = false;
      break;
    }

    SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
  }
#else
  p_ep = p_if_alt->endpoint_head_ptr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);

    CORE_ENTER_ATOMIC();
    p_dev->endpoint_interface_table[ep_phy_nbr] = if_nbr;
    CORE_EXIT_ATOMIC();

    status = sli_usbd_core_open_endpoint(p_ep->address,
                                         p_ep->max_pkt_size,
                                         p_ep->attrib,
                                         p_ep->interval);
    if (status != SL_STATUS_OK) {
      valid = false;
      break;
    }

    p_ep = p_ep->next_ptr;
  }
#endif

  if (valid == true) {
    status = SL_STATUS_OK;
  } else {
    usbd_core_close_alt_interface(p_dev, p_if_alt);
  }

  return status;
}

/****************************************************************************************************//**
 *                                           usbd_core_close_alt_interface()
 *
 * @brief    Closes all endpoints from the specified alternate setting.
 *
 * @param    p_dev       Pointer to USB device.
 *
 * @param    -----       Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    p_if_alt    Pointer to alternate setting interface.
 *******************************************************************************************************/
static void usbd_core_close_alt_interface(sli_usbd_device_t              *p_dev,
                                          const sli_usbd_alt_interface_t *p_if_alt)
{
  uint8_t ep_nbr;
  uint8_t ep_phy_nbr;
  sli_usbd_endpoint_info_t *p_ep;
#if (USBD_CFG_OPTIMIZE_SPD == 1)
  uint32_t ep_alloc_map;
#endif
  CORE_DECLARE_IRQ_STATE;

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  ep_alloc_map = p_if_alt->endpoint_table_map;
  while (ep_alloc_map != 0x00u) {
    ep_nbr = (uint8_t)__CLZ(__RBIT(ep_alloc_map));
    p_ep = p_if_alt->endpoint_table_ptrs[ep_nbr];
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);

    CORE_ENTER_ATOMIC();
    p_dev->endpoint_interface_table[ep_phy_nbr] = SL_USBD_INTERFACE_NBR_NONE;
    CORE_EXIT_ATOMIC();

    sli_usbd_core_close_endpoint(p_ep->address);

    SL_CLEAR_BIT(ep_alloc_map, SLI_USBD_SINGLE_BIT_MASK_32(ep_nbr));
  }
#else
  p_ep = p_if_alt->endpoint_head_ptr;

  for (ep_nbr = 0u; ep_nbr < p_if_alt->endpoint_nbr_total; ep_nbr++) {
    ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(p_ep->address);

    CORE_ENTER_ATOMIC();
    p_dev->endpoint_interface_table[ep_phy_nbr] = SL_USBD_INTERFACE_NBR_NONE;
    CORE_EXIT_ATOMIC();

    sli_usbd_core_close_endpoint(p_ep->address);

    p_ep = p_ep->next_ptr;
  }
#endif
}

/****************************************************************************************************//**
 *                                           usbd_core_get_interface_group_structure()
 *
 * @brief    Gets the interface group structure.
 *
 * @param    p_cfg       Pointer to configuration structure.
 *
 * @param    if_grp_nbr  Interface number.
 *
 * @return   Pointer to interface group structure, if no errors are returned.
 *
 *           Pointer to NULL,                      if any errors are returned.
 *******************************************************************************************************/
static sli_usbd_interface_group_t *usbd_core_get_interface_group_structure(const sli_usbd_configuration_t *p_config,
                                                                           uint8_t                        if_grp_nbr)
{
  sli_usbd_interface_group_t *p_if_grp;
#if (USBD_CFG_OPTIMIZE_SPD == 0)
  uint8_t if_grp_ix;
#endif

#if (USBD_CFG_OPTIMIZE_SPD == 1)
  p_if_grp = p_config->interface_group_table_ptrs[if_grp_nbr];
#else
  p_if_grp = p_config->interface_group_head_ptr;

  for (if_grp_ix = 0u; if_grp_ix < if_grp_nbr; if_grp_ix++) {
    p_if_grp = p_if_grp->next_ptr;
  }
#endif

  return (p_if_grp);
}

/****************************************************************************************************//**
 *                                               usbd_core_add_string()
 *
 * @brief    Adds the string to the USB device.
 *
 * @param    p_dev   Pointer to device structure.
 *
 * @param    -----   Argument validated in the caller(s).
 *
 * @param    p_str   Pointer to string to add (see Note #1).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB spec 2.0 chapter 9.5 states "Where appropriate, descriptors contain references
 *               to string descriptors that provide displayable information describing a descriptor
 *               in human-readable form. The inclusion of string descriptors is optional.  However,
 *               the reference fields within descriptors are mandatory. If a device does not support
 *               string descriptors, string reference fields must be reset to zero to indicate no
 *               string descriptor is available.
 *
 *               Since string descriptors are optional, 'p_str' could be a NULL pointer.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == 1)
static sl_status_t usbd_core_add_string(sli_usbd_device_t       *p_dev,
                                        const char              *p_str)
{
  uint8_t str_ix;
  CORE_DECLARE_IRQ_STATE;

  // Return if NULL ptr.
  if (p_str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  for (str_ix = 0u; str_ix < p_dev->str_max_index; str_ix++) {
    if (p_str == p_dev->str_descriptor_table[str_ix]) {
      // Str already stored in tbl.
      return SL_STATUS_OK;
    }
  }

  CORE_ENTER_ATOMIC();
  // Get curr str tbl ix.
  str_ix = p_dev->str_max_index;

  // Chk if str can be stored in tbl.
  if (str_ix >= usbd_ptr->str_quantity_per_device) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  p_dev->str_descriptor_table[str_ix] = (char *)p_str;
  p_dev->str_max_index++;
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}
#endif

/****************************************************************************************************//**
 *                                               usbd_core_get_string_descriptor()
 *
 * @brief    Gets the string pointer.
 *
 * @param    p_dev       Pointer to device.
 *
 * @param    -----    Argument validate by the caller(s).
 *
 * @param    str_nbr     Number of the string to obtain.
 *
 * @return   Pointer to requested string, if no errors are returned.
 *
 *           Pointer to NULL,             if any errors are returned.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == 1)
static const char *usbd_core_get_string_descriptor(const sli_usbd_device_t *p_dev,
                                                   uint8_t                 str_nbr)
{
  const char *p_str;

  if (str_nbr > p_dev->str_max_index) {
    return (NULL);
  }

  p_str = p_dev->str_descriptor_table[str_nbr];
  return (p_str);
}
#endif

/****************************************************************************************************//**
 *                                           usbd_core_get_string_index()
 *
 * @brief    Get string index.
 *
 * @param    p_dev   Pointer to device.
 *
 * @param    -----  Argument validated in 'USBD_DevSetupPkt()' before posting the event to queue.
 *
 * @param    p_str   Pointer to string.
 *
 * @return   String index.
 *******************************************************************************************************/

#if (USBD_CFG_STR_EN == 1)
static uint8_t usbd_core_get_string_index(const sli_usbd_device_t *p_dev,
                                          const char              *p_str)
{
  uint8_t str_ix;

  // Return if a NULL pointer.
  if (p_str == NULL) {
    return (0u);
  }

  for (str_ix = 0u; str_ix < p_dev->str_max_index; str_ix++) {
    if (p_str == p_dev->str_descriptor_table[str_ix]) {
      // Str already stored in tbl.
      return (str_ix + 1u);
    }
  }

  return (0u);
}
#endif
