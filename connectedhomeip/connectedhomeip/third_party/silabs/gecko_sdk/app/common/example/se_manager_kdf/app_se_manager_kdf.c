/***************************************************************************//**
 * @file app_se_manager_kdf.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_kdf.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Command context
static sl_se_command_context_t cmd_ctx;

/// Pointer to salt (HKDF and PBKDF2)
static uint8_t *salt;

/// Pointer to information (HKDF)
static uint8_t *information;

/// Output key material buffer (HKDF and PBKDF2)
static uint8_t output_key_material[OUTPUT_KEY_SIZE];

/// Key descriptor pointing to an input key buffer (HKDF and PBKDF2)
static sl_se_key_descriptor_t input_key_desc = {
  .type = SL_SE_KEY_TYPE_SYMMETRIC,
  .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
  .storage.location.buffer.size = INPUT_KEY_SIZE
};

/// Key descriptor pointing to an output key buffer (HKDF and PBKDF2)
static sl_se_key_descriptor_t output_key_desc = {
  .type = SL_SE_KEY_TYPE_SYMMETRIC,
  .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
  .storage.location.buffer.pointer = output_key_material,
  .storage.location.buffer.size = sizeof(output_key_material)
};

/// Salt length (HKDF and PBKDF2)
static size_t salt_len;

/// Information length (HKDF)
static size_t information_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get output key material pointer.
 ******************************************************************************/
uint8_t * get_output_key_material_ptr(void)
{
  return(output_key_material);
}

/***************************************************************************//**
 * Set pointer to input key material.
 ******************************************************************************/
void set_input_key_material_ptr(uint8_t *ptr)
{
  input_key_desc.storage.location.buffer.pointer = ptr;
}

/***************************************************************************//**
 * Set pointer to salt.
 ******************************************************************************/
void set_salt_ptr(uint8_t *ptr)
{
  salt = ptr;
}

/***************************************************************************//**
 * Set pointer to information.
 ******************************************************************************/
void set_information_ptr(uint8_t *ptr)
{
  information = ptr;
}

/***************************************************************************//**
 * Set input key material length.
 ******************************************************************************/
void set_input_key_material_len(uint8_t length)
{
  input_key_desc.size = length;
}

/***************************************************************************//**
 * Set salt length.
 ******************************************************************************/
void set_salt_len(uint8_t length)
{
  salt_len = length;
}

/***************************************************************************//**
 * Set information length.
 ******************************************************************************/
void set_information_len(uint8_t length)
{
  information_len = length;
}

/***************************************************************************//**
 * Set output key material length.
 ******************************************************************************/
void set_output_key_material_len(uint8_t length)
{
  output_key_desc.size = length;
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Denitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Derive a pseudo random key from the input key material using HKDF.
 ******************************************************************************/
sl_status_t derive_hkdf_key(sl_se_hash_type_t hash_type)
{
  print_error_cycle(sl_se_derive_key_hkdf(&cmd_ctx,
                                          &input_key_desc,
                                          hash_type,
                                          salt,
                                          salt_len,
                                          information,
                                          information_len,
                                          &output_key_desc), &cmd_ctx);
}

/***************************************************************************//**
 * Derive a pseudo random key from the input key material using PBKDF2.
 ******************************************************************************/
sl_status_t derive_pbkdf2_key(sl_se_pbkdf2_prf_type_t prf, size_t iterations)
{
  print_error_cycle(sl_se_derive_key_pbkdf2(&cmd_ctx,
                                            &input_key_desc,
                                            prf,
                                            salt,
                                            salt_len,
                                            iterations,
                                            &output_key_desc), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
