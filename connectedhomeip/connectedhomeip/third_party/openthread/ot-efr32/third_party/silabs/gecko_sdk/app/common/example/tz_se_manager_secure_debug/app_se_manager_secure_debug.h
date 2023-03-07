/***************************************************************************//**
 * @file app_se_manager_secure_debug.h
 * @brief SE manager secure debug functions.
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
#ifndef APP_SE_MANAGER_SECURE_DEBUG_H
#define APP_SE_MANAGER_SECURE_DEBUG_H

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

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get SE status buffer pointer.
 *
 * @returns Returns pointer to SE status buffer.
 ******************************************************************************/
sl_se_status_t * get_se_status_buf_ptr(void);

/***************************************************************************//**
 * Get debug lock status buffer pointer.
 *
 * @returns Returns pointer to debug lock status buffer.
 ******************************************************************************/
sl_se_debug_status_t * get_debug_lock_status_buf_ptr(void);

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
 * Get SE status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_status(void);

/***************************************************************************//**
 * Get debug lock status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_debug_lock_status(void);

/***************************************************************************//**
 * Read public command key from SE OTP.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_public_command_key(void);

/***************************************************************************//**
 * Enable the debug lock.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t enable_debug_lock(void);

/***************************************************************************//**
 * Perform a device mass erase and debug unlock.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_debug_lock(void);

/***************************************************************************//**
 * Enable the secure debug.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t enable_secure_debug(void);

/***************************************************************************//**
 * Disable the secure debug.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_secure_debug(void);

/***************************************************************************//**
 * Disable the device erase.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_device_erase(void);

/***************************************************************************//**
 * Set debug options.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t set_debug_option(void);

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
 * Use an unlock token to unlock the device.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t use_unlock_token(void);

/***************************************************************************//**
 * Roll challenge to invalidate the current unlock token.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t roll_challenge(void);

#endif  // APP_SE_MANAGER_SECURE_DEBUG_H
