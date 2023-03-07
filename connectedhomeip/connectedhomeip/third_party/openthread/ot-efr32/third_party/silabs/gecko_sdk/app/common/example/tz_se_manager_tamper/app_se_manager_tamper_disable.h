/***************************************************************************//**
 * @file app_se_manager_tamper_disable.h
 * @brief SE manager tamper disable functions.
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
#ifndef APP_SE_MANAGER_TAMPER_DISABLE_H
#define APP_SE_MANAGER_TAMPER_DISABLE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Mask to disable tamper signals
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
#define TAMPER_DISABLE_MASK     (0x00fa0000)
#else
#define TAMPER_DISABLE_MASK     (0xf2000000)
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get serial number buffer pointer.
 *
 * @returns Returns pointer to serial number buffer.
 ******************************************************************************/
uint8_t * get_serialno_buf_ptr(void);

/***************************************************************************//**
 * Get challenge buffer pointer.
 *
 * @returns Returns pointer to challenge buffer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void);

/***************************************************************************//**
 * Get public command key buffer pointer.
 *
 * @returns Returns pointer to public command key buffer.
 ******************************************************************************/
uint8_t * get_pubcmdkey_buf_ptr(void);

/***************************************************************************//**
 * Read public command key from SE OTP.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_public_command_key(void);

/***************************************************************************//**
 * Read the serial number of the SE and save it to access certificate.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_serial_number(void);

/***************************************************************************//**
 * Request challenge from the SE and save it to challenge response.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t request_challenge(void);

/***************************************************************************//**
 * Use a tamper disable token to disable tamper signals.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t use_tamper_disable_token(void);

/***************************************************************************//**
 * Roll challenge to invalidate the current tamper disable token.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t roll_challenge(void);

#endif  // APP_SE_MANAGER_TAMPER_DISABLE_H
