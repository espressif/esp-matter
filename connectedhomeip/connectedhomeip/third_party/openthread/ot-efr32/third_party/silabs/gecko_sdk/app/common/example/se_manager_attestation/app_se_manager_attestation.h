/***************************************************************************//**
 * @file app_se_manager_attestation.h
 * @brief SE manager attestation functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_SE_MANAGER_ATTESTATION_H
#define APP_SE_MANAGER_ATTESTATION_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_attestation.h"
#include "sl_se_manager_entropy.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define WORD_SIZE   (4U)
#define ATTESTED_TOKEN_MAX_SIZE   (1024U)

typedef enum {
  IAT_TOKEN,
  CONFIG_TOKEN
} token_t;

typedef enum {
  CLAIM_ARM_PSA_PROFILE_ID              = -75000,
  CLAIM_ARM_PSA_PARTITION_ID            = -75001,
  CLAIM_ARM_PSA_LIFECYCLE               = -75002,
  CLAIM_ARM_PSA_IMPLEMENTATION_ID       = -75003,
  CLAIM_ARM_PSA_BOOT_SEED               = -75004,
  CLAIM_ARM_PSA_SW_COMPONENTS           = -75006,
  CLAIM_ARM_PSA_NONCE                   = -75008,
  CLAIM_ARM_PSA_IETF_EAT_UEID           = -75009,
  CLAIM_SE_STATUS                       = -76000,
  CLAIM_OTP_CONFIG                      = -76001,
  CLAIM_MCU_BOOT_KEY                    = -76002,
  CLAIM_MCU_AUTH_KEY                    = -76003,
  CLAIM_CURRENT_APPLIED_TAMPER_SETTINGS = -76004
} attestation_claim_id_t;

typedef enum {
  CBOR_UNSIGNED_INT,
  CBOR_NEGATIVE_INT,
  CBOR_BYTE_STR,
  CBOR_TEXT_STR,
  CBOR_ARRAY,
  CBOR_MAP,
  CBOR_TAG,
  CBOR_SIMPLE_FLOAT
} cbor_major_type_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * @brief
 *   Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * @brief
 *   Denitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * @brief
 *   Get attested token of a given type (PSA IAT or config token).
 *
 * @param[in] token_type
 *   Type of token (IAT_TOKEN or CONFIG_TOKEN).
 *
 * @param[in] nonce_size
 *   Size of randomly generated nonce/challenge in bytes.
 *
 * @param[out] token_buf
 *   Buffer in which the token will be stored.
 *
 * @param[in] token_buf
 *   Size of token buffer in bytes. Must be padded to word alignment.
 *
 * @param[out] token_size
 *   Size of the token in bytes.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_attested_token(token_t token_type,
                               size_t nonce_size,
                               uint8_t *token_buf,
                               size_t token_buf_size,
                               size_t *token_size);

/***************************************************************************//**
 * @brief
 *   Print attestation token and claims in both raw and human-readable format.
 *
 * @param[in] token_data
 *   Buffer containing attestation token.
 *
 * @param[in] token_size
 *   Size of attestation token in bytes.

 * @returns void.
 ******************************************************************************/
void print_attestation_token(uint8_t *token_data, size_t token_size);

/***************************************************************************//**
 * @brief
 *   Parse and pretty-print claims from attestation token.
 *
 * @param[in] token_data
 *   Buffer containing CBOR-encoded token.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t token_claims_pretty_print(uint8_t *token_data);

/***************************************************************************//**
 * @brief
 *   Parse and pretty-print CBOR data.
 *
 * @param[in] cbor_data
 *   Buffer containing CBOR data.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t cbor_pretty_print(uint8_t *cbor_data);

#endif  // APP_SE_MANAGER_ATTESTATION_H
