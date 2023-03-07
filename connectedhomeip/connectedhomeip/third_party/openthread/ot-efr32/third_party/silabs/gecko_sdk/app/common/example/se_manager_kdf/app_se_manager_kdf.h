/***************************************************************************//**
 * @file app_se_manager_kdf.h
 * @brief SE manager KDF functions.
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
#ifndef APP_SE_MANAGER_KDF_H
#define APP_SE_MANAGER_KDF_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_key_derivation.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Input key material size
#define INPUT_KEY_SIZE  (32)

/// Output key material size
#define OUTPUT_KEY_SIZE (64)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get output key material pointer.
 *
 * @returns Returns pointer to output key material.
 ******************************************************************************/
uint8_t * get_output_key_material_ptr(void);

/***************************************************************************//**
 * Set pointer to input key material.
 *
 * @param ptr The pointer to input key material.
 ******************************************************************************/
void set_input_key_material_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set pointer to salt.
 *
 * @param ptr The pointer to salt.
 ******************************************************************************/
void set_salt_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set pointer to information.
 *
 * @param ptr The pointer to information.
 ******************************************************************************/
void set_information_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set input key material length.
 *
 * @param length The length of the input key material.
 ******************************************************************************/
void set_input_key_material_len(uint8_t length);

/***************************************************************************//**
 * Set salt length.
 *
 * @param length The length of the salt.
 ******************************************************************************/
void set_salt_len(uint8_t length);

/***************************************************************************//**
 * Set information length.
 *
 * @param length The length of the information.
 ******************************************************************************/
void set_information_len(uint8_t length);

/***************************************************************************//**
 * Set output key material length.
 *
 * @param length The length of the output key material.
 ******************************************************************************/
void set_output_key_material_len(uint8_t length);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Denitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Derive a pseudo random key from the input key material using HKDF.
 *
 * @param hash_type The Hash algorithm to be used.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t derive_hkdf_key(sl_se_hash_type_t hash_type);

/***************************************************************************//**
 * Derive a pseudo random key from the input key material using PBKDF2.
 *
 * @param prf The underlying psuedorandom function to use in the algorithm.
 * @param iterations The number of iterations, up to 16384.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t derive_pbkdf2_key(sl_se_pbkdf2_prf_type_t prf, size_t iterations);

#endif  // APP_SE_MANAGER_KDF_H
