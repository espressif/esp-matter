/***************************************************************************//**
 * @file app_se_manager_key_provisioning.h
 * @brief SE manager key provisioning functions.
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
#ifndef APP_SE_MANAGER_KEY_PROVISIONING_H
#define APP_SE_MANAGER_KEY_PROVISIONING_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_internal_keys.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Public key size
#define PUB_KEY_SIZE    (64)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get version data.
 *
 * @returns Returns the version.
 ******************************************************************************/
uint32_t get_version(void);

/***************************************************************************//**
 * Get SE OTP configuration buffer pointer.
 *
 * @returns Pointer to SE OTP configuration buffer.
 ******************************************************************************/
sl_se_otp_init_t * get_se_otp_conf_buf_ptr(void);

/***************************************************************************//**
 * Set pointer to AES or public key buffer.
 *
 * @param ptr The pointer to public key buffer.
 ******************************************************************************/
void set_key_buf_ptr(uint8_t *ptr);

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
 * Get the SE firmware version.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_version(void);

/***************************************************************************//**
 * Get SE OTP configuration.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_otp_conf(void);

/***************************************************************************//**
 * Read public key from SE OTP.
 *
 * @param key_id The key type (sign or command) to read from SE OTP.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_public_key(sl_se_device_key_type_t key_id);

/***************************************************************************//**
 * Program public key to the SE OTP.
 *
 * @param key_id The key type (sign or command) to program to SE OTP.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t program_public_key(sl_se_device_key_type_t key_id);

/***************************************************************************//**
 * Initialize SE OTP configuration.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_otp_conf(void);

#if !defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Program 128-bit AES key to the SE OTP.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t program_aes_key(void);

/***************************************************************************//**
 * Encrypt message with AES ECB.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_ecb(void);
#endif

#if defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Get executed command from command context.
 *
 * @returns The command in command context.
 ******************************************************************************/
uint32_t get_executed_command(void);

/***************************************************************************//**
 * Check if any command was executed.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t check_executed_command(void);

/***************************************************************************//**
 * Get the executed command response.
 *
 * @param buf Pointer to the response buffer.
 * @param buf_len The size of the response buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_executed_command_response(uint8_t *buf, uint32_t buf_len);
#endif

#endif  // APP_SE_MANAGER_KEY_PROVISIONING_H
