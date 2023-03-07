/***************************************************************************//**
 * @file app_se_manager_tamper.h
 * @brief SE manager tamper functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef APP_SE_MANAGER_TAMPER_H
#define APP_SE_MANAGER_TAMPER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "em_prs.h"
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// PRS channels for tamper
#define TAMPER_INT_PRS_CH       (6)
#define TAMPER_CNT_PRS_CH       (6)
#define HW_RST_TAMPER_PRS_CH    (7)
#define SW_RST_TAMPER_PRS_CH    (8)

/// GPIO for PRS
#define PRS_PB0_PORT            (SL_SIMPLE_BUTTON_GET_PORT(sl_button_btn0. \
                                                           context))
#define PRS_PB0_PIN             (SL_SIMPLE_BUTTON_GET_PIN(sl_button_btn0. \
                                                          context))
#define PRS_PB1_PORT            (SL_SIMPLE_BUTTON_GET_PORT(sl_button_btn1. \
                                                           context))
#define PRS_PB1_PIN             (SL_SIMPLE_BUTTON_GET_PIN(sl_button_btn1. \
                                                          context))

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Enable tamper interrupt.
 ******************************************************************************/
void enable_tamper_int(void);

/***************************************************************************//**
 * Get tamper interrupt status.
 *
 * @returns Returns true if tamper interrupt was triggered.
 ******************************************************************************/
bool get_tamper_int_status(void);

/***************************************************************************//**
 * Get reset cause buffer pointer.
 *
 * @returns Returns pointer to reset cause buffer.
 ******************************************************************************/
uint32_t * get_rst_cause_buf_ptr(void);

/***************************************************************************//**
 * Get SE status buffer pointer.
 *
 * @returns Returns pointer to SE status buffer.
 ******************************************************************************/
sl_se_status_t * get_se_status_buf_ptr(void);

/***************************************************************************//**
 * Get SE OTP configuration buffer pointer.
 *
 * @returns Returns pointer to SE OTP configuration buffer.
 ******************************************************************************/
sl_se_otp_init_t * get_se_otp_conf_buf_ptr(void);

/***************************************************************************//**
 * Initialize PRS for tamper.
 ******************************************************************************/
void init_tamper_prs(void);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Deinitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Get EMU->RSTCAUSE after a tamper reset.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_reset_cause(void);

/***************************************************************************//**
 * Get SE status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_status(void);

/***************************************************************************//**
 * Get SE OTP configuration.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_otp_conf(void);

/***************************************************************************//**
 * Initialize SE OTP configuration.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_otp_conf(void);

#endif  // APP_SE_MANAGER_TAMPER_H
