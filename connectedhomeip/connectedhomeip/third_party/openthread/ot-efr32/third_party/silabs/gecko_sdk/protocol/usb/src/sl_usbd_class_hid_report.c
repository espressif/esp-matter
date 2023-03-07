/***************************************************************************//**
 * @file
 * @brief USB Device Hid report
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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_status.h"
#include "sl_malloc.h"

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

/********************************************************************************************************
 *                                               HID DEFINES
 *******************************************************************************************************/

#define  SLI_USBD_HID_REPORT_ITEM_SIZE_MASK                 0x03u
#define  SLI_USBD_HID_REPORT_ITEM_TYPE_MASK                 0x0Cu
#define  SLI_USBD_HID_REPORT_ITEM_TAG_MASK                  0xF0u

#define  SLI_USBD_HID_IDLE_INFINITE                         0x00u
#define  SLI_USBD_HID_IDLE_ALL_REPORT                       0x00u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_hid_report_clear(sli_usbd_hid_report_t *p_report);

static sli_usbd_hid_report_id_t *usbd_hid_report_allocate_id(void);

static sli_usbd_hid_report_id_t *usbd_hid_report_get_id(sli_usbd_hid_report_t      *p_report,
                                                        sli_usbd_hid_report_type_t report_type,
                                                        uint8_t                    report_id);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialize HID report module
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_init(void)
{
  uint16_t ix;

  for (ix = 0u; ix < SL_USBD_HID_REPORT_ID_QUANTITY; ix++) {
    sli_usbd_hid_report_id_t *p_report_id = &usbd_hid_ptr->report_id_table[ix];

    p_report_id->id = 0u;
    p_report_id->size = 0u;
    p_report_id->data_ptr = NULL;
    p_report_id->next_ptr = NULL;

    p_report_id->class_nbr = SL_USBD_CLASS_NBR_NONE;
    p_report_id->idle_count = 0u;
    p_report_id->idle_rate = SLI_USBD_HID_IDLE_INFINITE;
    p_report_id->update = false;
    p_report_id->timer_next_ptr = NULL;
  }

  usbd_hid_ptr->report_id_table_index = SL_USBD_HID_REPORT_ID_QUANTITY;
  usbd_hid_ptr->report_id_timer_list = NULL;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Parse HID report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_parse(uint8_t                 class_nbr,
                                      const uint8_t           *p_report_data,
                                      uint16_t                report_data_len,
                                      sli_usbd_hid_report_t   *p_report)
{
  sli_usbd_hid_report_id_t   *p_report_id = NULL;
  sli_usbd_hid_report_item_t *p_item;
  uint8_t           item_tbl_size = 0u;
  uint8_t           col_nesting = 0u;
  uint8_t           report_type;

  p_item = usbd_hid_ptr->report_item_table;
  p_item->report_id = 0u;
  p_item->size = 0u;
  p_item->count = 0u;

  usbd_hid_report_clear(p_report);

  while (report_data_len > 0u) {
    uint8_t tag = *p_report_data;
    uint32_t data = 0u;

    p_report_data++;
    report_data_len--;

    switch (tag & SLI_USBD_HID_REPORT_ITEM_SIZE_MASK) {
      // Item size: 4 bytes.
      case 3u:
        if (report_data_len < 4u) {
          return SL_STATUS_INVALID_PARAMETER;
        }

        memcpy(&data, p_report_data, 4u);

        p_report_data += 4u;
        report_data_len -= 4u;
        break;

      // Item size: 2 bytes.
      case 2u:
        if (report_data_len < 2u) {
          return SL_STATUS_INVALID_PARAMETER;
        }
        memcpy(&data, p_report_data, 2u);

        p_report_data += 2u;
        report_data_len -= 2u;
        break;

      // Item size: 1 byte.
      case 1u:
        if (report_data_len < 1u) {
          return SL_STATUS_INVALID_PARAMETER;
        }

        data = *p_report_data;

        p_report_data++;
        report_data_len--;
        break;

      // Item size: 0 bytes.
      case 0u:
      default:
        break;
    }

    switch (tag & (SLI_USBD_HID_REPORT_ITEM_TYPE_MASK
                   | SLI_USBD_HID_REPORT_ITEM_TAG_MASK)) {
      case SL_USBD_HID_MAIN_INPUT:
        p_report_id = usbd_hid_report_get_id(p_report,
                                             SLI_USBD_HID_REPORT_TYPE_INPUT,
                                             p_item->report_id);
        if (p_report_id == NULL) {
          return SL_STATUS_ALLOCATION_FAILED;
        }

        p_report_id->size += p_item->count * p_item->size;
        p_report_id->class_nbr = class_nbr;

        if (p_report->max_input_report_size < p_report_id->size) {
          p_report->max_input_report_size = p_report_id->size;
        }
        break;

      case SL_USBD_HID_MAIN_OUTPUT:
        p_report_id = usbd_hid_report_get_id(p_report,
                                             SLI_USBD_HID_REPORT_TYPE_OUTPUT,
                                             p_item->report_id);
        if (p_report_id == NULL) {
          return SL_STATUS_ALLOCATION_FAILED;
        }

        p_report_id->size += p_item->count * p_item->size;
        p_report_id->class_nbr = class_nbr;

        if (p_report->max_output_report_size < p_report_id->size) {
          p_report->max_output_report_size = p_report_id->size;
        }
        break;

      case SL_USBD_HID_MAIN_FEATURE:
        p_report_id = usbd_hid_report_get_id(p_report,
                                             SLI_USBD_HID_REPORT_TYPE_FEATURE,
                                             p_item->report_id);
        if (p_report_id == NULL) {
          return SL_STATUS_ALLOCATION_FAILED;
        }

        p_report_id->size += p_item->count * p_item->size;
        p_report_id->class_nbr = class_nbr;

        if (p_report->max_feature_report_size < p_report_id->size) {
          p_report->max_feature_report_size = p_report_id->size;
        }
        break;

      case SL_USBD_HID_MAIN_COLLECTION:
        col_nesting++;
        break;

      case SL_USBD_HID_MAIN_ENDCOLLECTION:
        if (col_nesting == 0) {
          return SL_STATUS_INVALID_PARAMETER;
        }

        col_nesting--;
        break;

      case SL_USBD_HID_GLOBAL_REPORT_SIZE:
        p_item->size = data & 0xFFFFu;
        break;

      case SL_USBD_HID_GLOBAL_REPORT_COUNT:
        p_item->count = data & 0xFFFFu;
        break;

      case SL_USBD_HID_GLOBAL_REPORT_ID:
        p_item->report_id = data & 0xFFu;
        p_report->has_reports = true;
        break;

      case SL_USBD_HID_GLOBAL_PUSH:
#if (SL_USBD_HID_PUSH_POP_ITEM_QUANTITY > 0)
        if (item_tbl_size >= SL_USBD_HID_PUSH_POP_ITEM_QUANTITY) {
          return SL_STATUS_ALLOCATION_FAILED;
        }
#endif
        p_item = &usbd_hid_ptr->report_item_table[item_tbl_size + 1u];

        p_item->report_id = usbd_hid_ptr->report_item_table[item_tbl_size].report_id;
        p_item->size = usbd_hid_ptr->report_item_table[item_tbl_size].size;
        p_item->count = usbd_hid_ptr->report_item_table[item_tbl_size].count;

        item_tbl_size++;
        break;

      case SL_USBD_HID_GLOBAL_POP:
        if (item_tbl_size == 0u) {
          return SL_STATUS_INVALID_PARAMETER;
        }

        item_tbl_size--;
        p_item = &usbd_hid_ptr->report_item_table[item_tbl_size];
        break;

      case SL_USBD_HID_LOCAL_USAGE:
      case SL_USBD_HID_LOCAL_USAGE_MIN:
      case SL_USBD_HID_LOCAL_USAGE_MAX:
      case SL_USBD_HID_GLOBAL_USAGE_PAGE:
      case SL_USBD_HID_GLOBAL_LOG_MIN:
      case SL_USBD_HID_GLOBAL_LOG_MAX:
      case SL_USBD_HID_GLOBAL_PHY_MIN:
      case SL_USBD_HID_GLOBAL_PHY_MAX:
      case SL_USBD_HID_GLOBAL_UNIT_EXPONENT:
      case SL_USBD_HID_GLOBAL_UNIT:
      case SL_USBD_HID_LOCAL_DESIGNATOR_INDEX:
      case SL_USBD_HID_LOCAL_DESIGNATOR_MIN:
      case SL_USBD_HID_LOCAL_DESIGNATOR_MAX:
      case SL_USBD_HID_LOCAL_STRING_INDEX:
      case SL_USBD_HID_LOCAL_STRING_MIN:
      case SL_USBD_HID_LOCAL_STRING_MAX:
      case SL_USBD_HID_LOCAL_DELIMITER:
        break;

      default:
        return SL_STATUS_INVALID_PARAMETER;
    }
  }

  if (col_nesting != 0u) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Convert Report Size to Octets
  p_report->max_input_report_size += 7u;
  p_report->max_input_report_size /= 8u;

  p_report->max_output_report_size += 7u;
  p_report->max_output_report_size /= 8u;

  p_report->max_feature_report_size += 7u;
  p_report->max_feature_report_size /= 8u;

  // Reserve space for report id.
  if (p_report->has_reports == true) {
    if (p_report->max_input_report_size > 0u) {
      p_report->max_input_report_size++;
    }
    if (p_report->max_output_report_size > 0u) {
      p_report->max_output_report_size++;
    }
    if (p_report->max_feature_report_size > 0u) {
      p_report->max_feature_report_size++;
    }
  }

  if (p_report->max_output_report_size > 0u) {
    // Make sure the allocated size is multiple of 4 for alignment
    uint32_t alloc_size = SLI_USBD_ROUND_INC_UP_PWR2(p_report->max_output_report_size, 4);
    p_report->max_output_report_ptr = (uint8_t *)sl_malloc(alloc_size);
    if (p_report->max_output_report_ptr == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  if (p_report->max_feature_report_size > 0u) {
    // Make sure the allocated size is multiple of 4 for alignment
    uint32_t alloc_size = SLI_USBD_ROUND_INC_UP_PWR2(p_report->max_feature_report_size, 4);
    p_report->max_feature_report_ptr = (uint8_t *)sl_malloc(alloc_size);
    if (p_report->max_feature_report_ptr == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  for (report_type = 0; report_type < 3u; report_type++) {
    p_report_id = p_report->reports[report_type];
    while (p_report_id != NULL) {
      p_report_id->size += 7u;
      p_report_id->size /= 8u;

      if (p_report_id->size > 0u) {
        if (p_report->has_reports == true) {
          // Reserve space for report id.
          p_report_id->size++;
        }
        // Input reports use individual buf.
        if (report_type == 0u) {
          // Make sure the allocated size for data is multiple of 4 for alignment
          uint32_t alloc_size = SLI_USBD_ROUND_INC_UP_PWR2(p_report_id->size, 4);
          p_report_id->data_ptr = (uint8_t *)sl_malloc(alloc_size);
          if (p_report_id->data_ptr == NULL) {
            return SL_STATUS_ALLOCATION_FAILED;
          }
          // The first byte must be the report id.
          memset(&p_report_id->data_ptr[0u], 0, p_report_id->size);
          p_report_id->data_ptr[0u] = p_report_id->id;
        }
      }

      p_report_id = p_report_id->next_ptr;
    }
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Retrieve HID report length and pointer to its data area
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_get_info(const sli_usbd_hid_report_t *p_report,
                                         sli_usbd_hid_report_type_t  report_type,
                                         uint8_t                     report_id,
                                         uint8_t                     **p_buf,
                                         bool                        *p_is_largest,
                                         uint16_t                    *p_length)
{
  sli_usbd_hid_report_id_t *p_report_id;

  switch (report_type) {
    case SLI_USBD_HID_REPORT_TYPE_INPUT:
      p_report_id = p_report->reports[0u];
      break;

    case SLI_USBD_HID_REPORT_TYPE_OUTPUT:
      p_report_id = p_report->reports[1u];
      break;

    case SLI_USBD_HID_REPORT_TYPE_FEATURE:
      p_report_id = p_report->reports[2u];
      break;

    case SLI_USBD_HID_REPORT_TYPE_NONE:
    default:
      *p_length = 0;
      return SL_STATUS_INVALID_PARAMETER;
  }

  while (p_report_id != NULL) {
    if (p_report_id->id == report_id) {
      switch (report_type) {
        case SLI_USBD_HID_REPORT_TYPE_INPUT:
          if (p_buf != NULL) {
            *p_buf = p_report_id->data_ptr;
          }
          *p_is_largest = (p_report_id->size == p_report->max_input_report_size) ? true : false;
          break;

        case SLI_USBD_HID_REPORT_TYPE_OUTPUT:
          if (p_buf != NULL) {
            *p_buf = p_report->max_output_report_ptr;
          }
          *p_is_largest = false;
          break;

        case SLI_USBD_HID_REPORT_TYPE_FEATURE:
        default:
          if (p_buf != NULL) {
            *p_buf = p_report->max_feature_report_ptr;
          }
          *p_is_largest = (p_report_id->size == p_report->max_feature_report_size) ? true : false;
          break;
      }
      *p_length = p_report_id->size;
      return SL_STATUS_OK;
    }

    p_report_id = p_report_id->next_ptr;
  }

  *p_length = 0;
  return SL_STATUS_INVALID_PARAMETER;
}

/****************************************************************************************************//**
 * Process all periodic HID input reports
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_timer_task_handler(void)
{
  sli_usbd_hid_report_id_t *p_report_id;
  sli_usbd_hid_report_id_t *p_report_id_prev;
  bool        service;
  uint32_t         xfer_len;
  CORE_DECLARE_IRQ_STATE;

  p_report_id_prev = (sli_usbd_hid_report_id_t *)0;
  CORE_ENTER_ATOMIC();
  p_report_id = usbd_hid_ptr->report_id_timer_list;
  CORE_EXIT_ATOMIC();

  while (p_report_id != NULL) {
    service = false;

    CORE_ENTER_ATOMIC();
    if (p_report_id->idle_rate == SLI_USBD_HID_IDLE_INFINITE) {
      p_report_id->idle_count = 0u;
      p_report_id->update = false;

      if (p_report_id_prev == NULL) {
        usbd_hid_ptr->report_id_timer_list = p_report_id->timer_next_ptr;
      } else {
        p_report_id_prev->timer_next_ptr = p_report_id->timer_next_ptr;
      }

      p_report_id->timer_next_ptr = NULL;
    } else {
      if (p_report_id->update == true) {
        p_report_id->update = false;

        if (p_report_id->idle_count > 1) {
          p_report_id->idle_count = p_report_id->idle_rate;
        }
      }

      if (p_report_id->idle_count > 1) {
        p_report_id->idle_count--;
      } else {
        p_report_id->idle_count = p_report_id->idle_rate;
        service = true;
      }
    }
    CORE_EXIT_ATOMIC();

    if (service == true) {
      sl_usbd_hid_write_sync(p_report_id->class_nbr,
                             p_report_id->data_ptr,
                             p_report_id->size,
                             100,
                             &xfer_len);
    }

    p_report_id_prev = p_report_id;
    p_report_id = p_report_id->timer_next_ptr;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Retrieve HID input report idle rate
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_get_idle_rate(const sli_usbd_hid_report_t *p_report,
                                              uint8_t                     report_id,
                                              uint8_t                     *p_idle_rate)
{
  sli_usbd_hid_report_id_t *p_report_id;

  p_report_id = p_report->reports[0u];

  while (p_report_id != NULL) {
    if (p_report_id->id == report_id) {
      *p_idle_rate = p_report_id->idle_rate;
      return SL_STATUS_OK;
    }

    p_report_id = p_report_id->next_ptr;
  }

  *p_idle_rate = 0u;
  return SL_STATUS_INVALID_PARAMETER;
}

/****************************************************************************************************//**
 * Set HID input report idle rate
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_set_idle_rate(const sli_usbd_hid_report_t *p_report,
                                              uint8_t                     report_id,
                                              uint8_t                     idle_rate)
{
  sli_usbd_hid_report_id_t *p_report_id;
  sl_status_t          status;
  CORE_DECLARE_IRQ_STATE;

  p_report_id = p_report->reports[0u];

  status = SL_STATUS_INVALID_PARAMETER;

  while (p_report_id != NULL) {
    if ((p_report_id->id == report_id)
        || (report_id == SLI_USBD_HID_IDLE_ALL_REPORT)) {
      if (idle_rate != SLI_USBD_HID_IDLE_INFINITE) {
        CORE_ENTER_ATOMIC();
        // Add report id into timer list.
        if (p_report_id->idle_rate == SLI_USBD_HID_IDLE_INFINITE) {
          p_report_id->timer_next_ptr = usbd_hid_ptr->report_id_timer_list;
          usbd_hid_ptr->report_id_timer_list = p_report_id;
        }

        p_report_id->idle_rate = idle_rate;
        p_report_id->update = true;

        CORE_EXIT_ATOMIC();
      } else {
        CORE_ENTER_ATOMIC();
        p_report_id->idle_rate = SLI_USBD_HID_IDLE_INFINITE;
        CORE_EXIT_ATOMIC();
      }

      status = SL_STATUS_OK;

      if (report_id != SLI_USBD_HID_IDLE_ALL_REPORT) {
        return status;
      }
    }

    p_report_id = p_report_id->next_ptr;
  }

  return status;
}

/****************************************************************************************************//**
 * Remove all HID input report from periodic service list
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_remove_all_idle(const sli_usbd_hid_report_t *p_report)
{
  sli_usbd_hid_report_id_t *p_report_id;
  CORE_DECLARE_IRQ_STATE;

  p_report_id = p_report->reports[0u];

  while (p_report_id != NULL) {
    // Remove only reports present on timer list.
    CORE_ENTER_ATOMIC();
    if (p_report_id->timer_next_ptr != NULL) {
      p_report_id->idle_rate = SLI_USBD_HID_IDLE_INFINITE;
    }
    CORE_EXIT_ATOMIC();

    p_report_id = p_report_id->next_ptr;
  }

  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           usbd_hid_report_clear()
 *
 * @brief    Initialize HID report structure.
 *
 * @param    p_report    Pointer to HID report structure.
 *******************************************************************************************************/
static void usbd_hid_report_clear(sli_usbd_hid_report_t *p_report)
{
  p_report->has_reports = 0;
  p_report->max_input_report_size = 0;
  p_report->max_output_report_size = 0;
  p_report->max_feature_report_size = 0;

  p_report->max_output_report_ptr = NULL;
  p_report->max_feature_report_ptr = NULL;

  p_report->reports[0u] = NULL;
  p_report->reports[1u] = NULL;
  p_report->reports[2u] = NULL;
}

/****************************************************************************************************//**
 *                                           usbd_hid_report_allocate_id()
 *
 * @brief    Allocate an instance of HID report id structure.
 *
 * @return   Pointer to HID report id structure, if NO error(s).
 *
 *           Pointer to NULL,                    otherwise.
 *******************************************************************************************************/
static sli_usbd_hid_report_id_t *usbd_hid_report_allocate_id(void)
{
  sli_usbd_hid_report_id_t *p_report_id;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (usbd_hid_ptr->report_id_table_index == 0u) {
    CORE_EXIT_ATOMIC();
    return (NULL);
  }

  usbd_hid_ptr->report_id_table_index--;

  p_report_id = &usbd_hid_ptr->report_id_table[usbd_hid_ptr->report_id_table_index];
  CORE_EXIT_ATOMIC();

  return (p_report_id);
}

/****************************************************************************************************//**
 *                                           usbd_hid_report_get_id()
 *
 * @brief    Retrieve HID report id structure.
 *
 * @param    p_report        Pointer to HID report structure.
 *
 * @param    report_type     HID report type.
 *
 * @param    report_id       HID report id.
 *
 * @return   Pointer to HID report id structure, if NO error(s).
 *
 *           Pointer to NULL,                    otherwise.
 *
 * @note     (1) If HID report id structure is not available for the specific report type and id, an
 *               instance of HID report id structure is allocated and linked into the HID report
 *               structure.
 *******************************************************************************************************/
static sli_usbd_hid_report_id_t *usbd_hid_report_get_id(sli_usbd_hid_report_t      *p_report,
                                                        sli_usbd_hid_report_type_t report_type,
                                                        uint8_t                    report_id)
{
  sli_usbd_hid_report_id_t *p_report_id;
  sli_usbd_hid_report_id_t *p_report_id_prev;
  uint8_t         type;

  switch (report_type) {
    case SLI_USBD_HID_REPORT_TYPE_INPUT:
      type = 0u;
      break;

    case SLI_USBD_HID_REPORT_TYPE_OUTPUT:
      type = 1u;
      break;

    case SLI_USBD_HID_REPORT_TYPE_FEATURE:
      type = 2u;
      break;

    case SLI_USBD_HID_REPORT_TYPE_NONE:
    default:
      return (NULL);
  }

  if (p_report->has_reports == false) {
    if (report_id > 0) {
      return (NULL);
    }

    if (p_report->reports[type] == NULL) {
      p_report->reports[type] = usbd_hid_report_allocate_id();
      if (p_report->reports[type] == NULL) {
        return (NULL);
      }
    }

    p_report_id = p_report->reports[type];

    return (p_report_id);
  }

  p_report_id = p_report->reports[type];
  p_report_id_prev = NULL;
  while (p_report_id != NULL) {
    if (p_report_id->id == report_id) {
      return (p_report_id);
    }

    p_report_id_prev = p_report_id;
    p_report_id = p_report_id->next_ptr;
  }

  p_report_id = usbd_hid_report_allocate_id();
  if (p_report_id == NULL) {
    return (NULL);
  }

  p_report_id->id = report_id;

  if (p_report->reports[type] == NULL) {
    p_report->reports[type] = p_report_id;
  } else {
    p_report_id_prev->next_ptr = p_report_id;
  }

  return (p_report_id);
}
