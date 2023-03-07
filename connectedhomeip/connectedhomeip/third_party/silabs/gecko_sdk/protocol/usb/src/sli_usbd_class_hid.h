/***************************************************************************//**
 * @file
 * @brief USB Device Hid Private
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

#ifndef  SLI_USBD_CLASS_HID_H
#define  SLI_USBD_CLASS_HID_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_enum.h"

#include "sl_usbd_class_hid.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

SL_ENUM(sli_usbd_hid_report_type_t) {
  SLI_USBD_HID_REPORT_TYPE_NONE = 0,
  SLI_USBD_HID_REPORT_TYPE_INPUT,
  SLI_USBD_HID_REPORT_TYPE_OUTPUT,
  SLI_USBD_HID_REPORT_TYPE_FEATURE
};

typedef struct sli_usbd_hid_report_id sli_usbd_hid_report_id_t;

struct sli_usbd_hid_report_id {
  uint8_t         id;
  uint16_t         size;
  uint8_t         *data_ptr;
  sli_usbd_hid_report_id_t *next_ptr;

  uint8_t         class_nbr;
  uint8_t         idle_count;
  uint8_t         idle_rate;
  bool        update;
  sli_usbd_hid_report_id_t *timer_next_ptr;
};

typedef struct {
  bool        has_reports;
  uint16_t         max_input_report_size;
  uint16_t         max_feature_report_size;
  uint8_t         *max_feature_report_ptr;
  uint16_t         max_output_report_size;
  uint8_t         *max_output_report_ptr;
  // Index 0: Input reports; 1: Output; 2: Feature.
  sli_usbd_hid_report_id_t *reports[3];
} sli_usbd_hid_report_t;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct sli_usbd_hid_ctrl sli_usbd_hid_ctrl_t;

/********************************************************************************************************
 *                                           HID CLASS STATES
 *******************************************************************************************************/

SL_ENUM(sli_usbd_hid_state_t) {
  SLI_USBD_HID_STATE_NONE = 0,
  SLI_USBD_HID_STATE_INIT,
  SLI_USBD_HID_STATE_CONFIG
};

/********************************************************************************************************
 *                                   HID CLASS EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

// HID Class Comm Info
typedef struct {
  // Pointer to control information.
  sli_usbd_hid_ctrl_t *ctrl_ptr;
  // Avail EP for comm: Intr
  uint8_t             data_interrupt_in_endpoint_address;
  uint8_t             data_interrupt_out_endpoint_address;
  bool                data_interrupt_out_active_transfer;
} sli_usbd_hid_comm_t;

// HID Class Ctrl Info
struct sli_usbd_hid_ctrl {
  uint8_t                         class_nbr;                                // Class nbr.
  sli_usbd_hid_state_t            state;                                    // HID class state.
  sli_usbd_hid_comm_t             *comm_ptr;                                // HID class comm info ptr.

  sl_usbd_hid_async_function_t    interrupt_read_async_function;            // Ptr to async comm callback and arg.
  void                            *interrupt_read_async_arg_ptr;
  sl_usbd_hid_async_function_t    interrupt_write_async_function;
  void                            *interrupt_write_async_arg_ptr;
  uint32_t                        data_interrupt_in_transfer_len;

  uint8_t                         subclass_code;
  uint8_t                         protocol_code;
  sl_usbd_hid_country_code_t      country_code;
  sli_usbd_hid_report_t           report;
  const uint8_t                   *report_descriptor_ptr;
  uint16_t                        report_descriptor_len;
  const uint8_t                   *phy_descriptor_ptr;
  uint16_t                        phy_descriptor_len;
  uint16_t                        interval_in;
  uint16_t                        interval_out;
  bool                            ctrl_read_enable;                         // En rd operations thru ctrl xfer.
  sl_usbd_hid_callbacks_t         *callback_ptr;                            // Ptr to class-specific desc and req callbacks.
  uint8_t                         *rx_buf_ptr;
  uint32_t                        rx_buf_len;
  bool                            is_rx;

  uint8_t                         *ctrl_status_buf_ptr;                      // Buf used for ctrl status xfers.
};

// Report Item
typedef struct {
  uint8_t report_id;
  uint16_t size;
  uint16_t count;
} sli_usbd_hid_report_item_t;

// HID Root Structure
typedef struct {
  // HID class instance control structures.
  sli_usbd_hid_ctrl_t         ctrl_table[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
  uint8_t                     ctrl_nbr_next;

  // HID class comm array.
  sli_usbd_hid_comm_t         comm_tbl[SL_USBD_HID_CLASS_INSTANCE_QUANTITY * SL_USBD_HID_CONFIGURATION_QUANTITY];
  uint8_t                     comm_nbr_next;

  // report item tables.
  sli_usbd_hid_report_id_t    report_id_table[SL_USBD_HID_REPORT_ID_QUANTITY];

  uint16_t                    report_id_table_index;
  sli_usbd_hid_report_id_t    *report_id_timer_list;

  sli_usbd_hid_report_item_t  report_item_table[SL_USBD_HID_PUSH_POP_ITEM_QUANTITY + 1];
} sli_usbd_hid_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern sli_usbd_hid_t          *usbd_hid_ptr;

/********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_hid_report_init()
 *
 * @brief    Initialize HID report module.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_init(void);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_report_parse()
 *
 * @brief    Parse HID report.
 *
 * @param    class_nbr           Class instance number.
 *
 * @param    p_report_data       Pointer to HID report descriptor.
 *
 * @param    report_data_len     Length of  HID report descriptor.
 *
 * @param    p_report            _data       Pointer to HID report descriptor.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_parse(uint8_t                class_nbr,
                                      const uint8_t          *p_report_data,
                                      uint16_t               report_data_len,
                                      sli_usbd_hid_report_t  *p_report);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_report_get_info()
 *
 * @brief    Retrieve HID report length and pointer to its data area.
 *
 * @param    p_report        Pointer to HID report structure.
 *
 * @param    report_type     HID report type.
 *
 * @param    report_id       HID report id.
 *
 * @param    p_buf           Pointer to variable that will receive the pointer to the HID report data area.
 *
 * @param    p_is_largest    Pointer to variable that will receive the indication that the HID input or
 *                           feature report is the largest report from the list of input or feature
 *                           reports.
 *
 * @param    p_length        Pointer to variable that will receive the length of HID report, in octets.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_get_info(const sli_usbd_hid_report_t *p_report,
                                         sli_usbd_hid_report_type_t  report_type,
                                         uint8_t                     report_id,
                                         uint8_t                     **p_buf,
                                         bool                        *p_is_largest,
                                         uint16_t                    *p_length);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_report_get_idle_rate()
 *
 * @brief    Retrieve HID input report idle rate.
 *
 * @param    p_report    Pointer to HID report structure.
 *
 * @param    report_id   HID report id.
 *
 * @param    p_idle_rate Pointer to variable that will be set to idle rate.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) idle rate is in 4 millisecond units.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_get_idle_rate(const sli_usbd_hid_report_t *p_report,
                                              uint8_t                     report_id,
                                              uint8_t                     *p_idle_rate);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_report_set_idle_rate()
 *
 * @brief    Set HID input report idle rate.
 *
 * @param    p_report    Pointer to HID report structure.
 *
 * @param    report_id   HID report id.
 *
 * @param    idle_rate   report idle rate.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) idle rate is in 4 millisecond units.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_set_idle_rate(const sli_usbd_hid_report_t *p_report,
                                              uint8_t                     report_id,
                                              uint8_t                     idle_rate);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_report_remove_all_idle()
 *
 * @brief    Remove all HID input report from periodic service list.
 *
 * @param    p_report    Pointer to HID report structure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_remove_all_idle(const sli_usbd_hid_report_t *p_report);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_report_timer_task_handler()
 *
 * @brief    Process all periodic HID input reports.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_report_timer_task_handler(void);

/****************************************************************************************************//**
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
