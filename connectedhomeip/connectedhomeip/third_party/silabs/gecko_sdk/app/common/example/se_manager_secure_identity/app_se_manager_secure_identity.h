/***************************************************************************//**
 * @file app_se_manager_secure_identity.h
 * @brief SE manager secure identity functions.
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
#ifndef APP_SE_MANAGER_SECURE_IDENTITY_H
#define APP_SE_MANAGER_SECURE_IDENTITY_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_entropy.h"
#include "sl_se_manager_internal_keys.h"
#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_signature.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Certificate buffer size
#define CERT_SIZE       (512)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get certificate size.
 *
 * @param cert_type Type of the certificate, either batch or device.
 * @returns Returns certificate size, 0 for invalid certificate type.
 ******************************************************************************/
uint32_t get_cert_size(uint8_t cert_type);

/***************************************************************************//**
 * Get challenge buffer pointer.
 *
 * @returns Returns pointer to challenge buffer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void);

/***************************************************************************//**
 * Get certificate buffer pointer.
 *
 * @returns Returns pointer to the certificate buffer.
 ******************************************************************************/
uint8_t * get_cert_buf_ptr(void);

/***************************************************************************//**
 * Get public key buffer pointer.
 *
 * @returns Returns pointer to the public key buffer.
 ******************************************************************************/
uint8_t * get_pub_device_key_buf_ptr(void);

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
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Read size of stored certificates in the SE.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_cert_size(void);

/***************************************************************************//**
 * Read stored certificates in SE.
 *
 * @param cert_type Type of the certificate, either batch or device.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_cert_data(uint8_t cert_type);

/***************************************************************************//**
 * Sign challenge with private device key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sign_challenge(void);

/***************************************************************************//**
 * Get on-chip public device key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_public_device_key(void);

/***************************************************************************//**
 * Verify signature with on-chip public device key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t verify_signature_local(void);

/***************************************************************************//**
 * Verify signature with public device key in device certificate.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t verify_signature_remote(void);

#endif  // APP_SE_MANAGER_SECURE_IDENTITY_H
