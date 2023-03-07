/***************************************************************************//**
 * @file app_se_manager_attestation.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_attestation.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * @brief
 *   Parse CBOR major type and argument.
 *
 * @param[in] cbor_data
 *   Buffer containing CBOR data.
 *
 * @param[out] maj_type
 *   Pointer to uint8_t for storing major type.
 *
 * @param[out] arg
 *   Pointer to uint64_t for storing argument.
 *
 * @param[out] nbytes_read
 *   Number of bytes read and parsed from buffer.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t _cbor_parse_maj_and_arg(uint8_t *cbor_data,
                                           uint8_t *maj_type,
                                           uint64_t *arg,
                                           size_t *nbytes_read);

/***************************************************************************//**
 * @brief
 *   Parse and optionally pretty-print CBOR data.
 *
 * @param[in] cbor_data
 *   Buffer containing CBOR data.
 *
 * @param[out] nbytes_read
 *   Number of bytes read and parsed from buffer.
 *
 * @param[in] print_flag
 *   If set, print data while parsing.
 *
 * @param[in] indent_lvl
 *   Indentation level used when printing.
 *
 * @param[in] indent_width
 *   Indentation width. Number of whitespace characters per indentation level.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t _cbor_parse(uint8_t *cbor_data,
                               size_t *nbytes_read,
                               bool print_flag,
                               uint8_t indent_lvl,
                               uint8_t indent_width);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Get attested token of given type.
 ******************************************************************************/
sl_status_t get_attested_token(token_t token_type,
                               size_t nonce_size,
                               uint8_t *token_buf,
                               size_t token_buf_size,
                               size_t *token_size)
{
  sl_se_command_context_t cmd_ctx = { 0 }; // SE Manager command context
  uint8_t nonce[64] = { 0 }; /// Nonce/challenge, to be randomly generated.
  sl_status_t ret;

  /// Initialize Secure Element command context
  if (sl_se_init_command_context(&cmd_ctx) != SL_STATUS_OK) {
    printf("[ERROR] Failed to initialize SE command context.\n");
    return SL_STATUS_FAIL;
  }

  /// Generate random nonce
  ret = sl_se_get_random(&cmd_ctx, &nonce, nonce_size);
  sl_se_deinit_command_context(&cmd_ctx);

  if (ret != SL_STATUS_OK) {
    printf("[ERROR] Failed to generate random nonce.\n");
    return ret;
  }

  /// Get attested token
  switch (token_type) {
    case IAT_TOKEN:
      printf("\nCalling sl_se_attestation_get_psa_iat_token...\n");
      print_error_cycle( /// Macro that returns sl_status_t
        sl_se_attestation_get_psa_iat_token(&cmd_ctx,
                                            nonce,
                                            nonce_size,
                                            token_buf,
                                            token_buf_size,
                                            token_size), &cmd_ctx);
      break;

    case CONFIG_TOKEN:
      printf("\nCalling sl_se_attestation_get_config_token...\n");
      print_error_cycle( /// Macro that returns sl_status_t
        sl_se_attestation_get_config_token(&cmd_ctx,
                                           nonce,
                                           nonce_size,
                                           token_buf,
                                           token_buf_size,
                                           token_size), &cmd_ctx);
      break;

    default:
      printf("[ERROR] Unknown token type.\n");
      return SL_STATUS_INVALID_TYPE;
  }
}

/***************************************************************************//**
 * Pretty-print contents of attestation token.
 ******************************************************************************/
void print_attestation_token(uint8_t *token_data, size_t token_size)
{
  sl_status_t ret;

  printf("-------------------------------------------------------------------");
  printf("\nRaw token:\n");
  for (size_t i = 0; i < token_size; i++) {
    printf("%02x", token_data[i]);
  }
  printf("\n\n");

  printf("COSE_Sign1 structure:\n");
  ret = cbor_pretty_print(token_data);

  if (ret == SL_STATUS_NOT_SUPPORTED) {
    printf("[ERROR] CBOR: Not well-formed / implemented.\n");
    return;
  }

  printf("-------------------------------------------------------------------");
  ret = token_claims_pretty_print(token_data);
  printf("-------------------------------------------------------------------");
  printf("\n");

  if (ret == SL_STATUS_INVALID_TYPE) {
    printf("[ERROR] Invalid COSE_Sign1 structure.\n");
  }
}

/***************************************************************************//**
 * Pretty-print CBOR data. Wrapper for _cbor_parse with print_flag set.
 ******************************************************************************/
sl_status_t cbor_pretty_print(uint8_t *cbor_data)
{
  size_t nbytes_read = 0;
  return _cbor_parse(cbor_data, &nbytes_read, true, 0, 2);
}

/***************************************************************************//**
 * Pretty-print claims from attestation token.
 ******************************************************************************/
sl_status_t token_claims_pretty_print(uint8_t *token_data)
{
  uint8_t maj_type;
  uint8_t *data_ptr;
  uint64_t arg;
  uint32_t nclaims;
  int64_t claim_id;
  size_t item_size;
  sl_status_t ret;

  if (token_data[0] != 0xd2 || token_data[1] != 0x84) {
    return SL_STATUS_INVALID_TYPE;
  }

  data_ptr = token_data + 2;

  /// Skip protected header parameters
  ret = _cbor_parse(data_ptr, &item_size, false, 0, 0);
  if (ret != SL_STATUS_OK) {
    return ret;
  }
  data_ptr += item_size;

  /// Skip unprotected header parameters
  ret = _cbor_parse(data_ptr, &item_size, false, 0, 0);
  if (ret != SL_STATUS_OK) {
    return ret;
  }
  data_ptr += item_size;

  /// Unwrap byte string containing CBOR-encoded claims
  ret = _cbor_parse_maj_and_arg(data_ptr, &maj_type, &arg, &item_size);
  if (ret != SL_STATUS_OK) {
    return ret;
  }
  data_ptr += item_size;

  if (maj_type != CBOR_BYTE_STR) {
    return SL_STATUS_INVALID_TYPE;
  }

  printf("\nToken claims:\n");
  cbor_pretty_print(data_ptr);

  /// Parse and pretty-print the map data structure containing the claims
  printf("\nList of claims printed with human-friendly names:");
  ret = _cbor_parse_maj_and_arg(data_ptr, &maj_type, &arg, &item_size);
  if (ret != SL_STATUS_OK) {
    return ret;
  }
  data_ptr += item_size;

  if (maj_type != CBOR_MAP) {
    return SL_STATUS_INVALID_TYPE;
  }

  nclaims = arg;
  for (uint32_t i = 0; i < nclaims; i++) {
    /// Parse claim ID integer
    ret = _cbor_parse_maj_and_arg(data_ptr, &maj_type, &arg, &item_size);
    if (ret != SL_STATUS_OK) {
      return ret;
    }
    data_ptr += item_size;

    if (maj_type == CBOR_UNSIGNED_INT) {
      claim_id = arg;
    } else if (maj_type == CBOR_NEGATIVE_INT) {
      claim_id = -(arg + 1);
    } else {
      return SL_STATUS_INVALID_TYPE;
    }

    /// Parse and pretty-print claim
    printf("\n  ");
    switch (claim_id) {
      case CLAIM_ARM_PSA_PROFILE_ID:
        printf("ARM PSA Profile ID");
        break;

      case CLAIM_ARM_PSA_PARTITION_ID:
        printf("ARM PSA Partition ID");
        break;

      case CLAIM_ARM_PSA_LIFECYCLE:
        printf("ARM PSA Lifecycle");
        break;

      case CLAIM_ARM_PSA_IMPLEMENTATION_ID:
        printf("ARM PSA Implementation ID");
        break;

      case CLAIM_ARM_PSA_BOOT_SEED:
        printf("ARM PSA Boot seed");
        break;

      case CLAIM_ARM_PSA_SW_COMPONENTS:
        printf("ARM PSA Software components");
        break;

      case CLAIM_ARM_PSA_NONCE:
        printf("ARM PSA Nonce");
        break;

      case CLAIM_ARM_PSA_IETF_EAT_UEID:
        printf("ARM PSA / IETF EAT UEID");
        break;

      case CLAIM_SE_STATUS:
        printf("SE Status");
        break;

      case CLAIM_OTP_CONFIG:
        printf("OTP Configuration");
        break;

      case CLAIM_MCU_BOOT_KEY:
        printf("OTP MCU Boot key");
        break;

      case CLAIM_MCU_AUTH_KEY:
        printf("OTP MCU Auth key");
        break;

      case CLAIM_CURRENT_APPLIED_TAMPER_SETTINGS:
        printf("Current applied tamper settings");
        break;

      default:
        printf("(UNKNOWN CLAIM)");
        break;
    }

    printf(", Claim ID: %" PRIi64 "\n", claim_id);

    ret = _cbor_parse(data_ptr, &item_size, true, 1, 2);
    if (ret != SL_STATUS_OK) {
      return ret;
    }
    data_ptr += item_size;
  }

  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Parse CBOR major type and argument.
 ******************************************************************************/
static sl_status_t _cbor_parse_maj_and_arg(uint8_t *cbor_data,
                                           uint8_t *maj_type,
                                           uint64_t *arg,
                                           size_t *nbytes_read)
{
  uint8_t add_info, arg_nbytes;
  uint8_t *data_ptr;

  data_ptr = cbor_data;
  *maj_type = *data_ptr >> 5; /// High-order 3 bits
  add_info = *data_ptr & 0x1f; /// Low-order 5 bits
  data_ptr++;

  *arg = 0;
  arg_nbytes = 0;

  if (add_info < 24) {
    *arg = add_info;
  } else if (add_info == 24) {
    arg_nbytes = 1;
  } else if (add_info == 25) {
    arg_nbytes = 2;
  } else if (add_info == 26) {
    arg_nbytes = 4;
  } else if (add_info == 27) {
    arg_nbytes = 8;
  } else {
    return SL_STATUS_NOT_SUPPORTED;
  }

  while (0 < arg_nbytes--) { /// Read argument in network byte order
    *arg = (*arg << 8) | *(data_ptr++);
  }

  *nbytes_read = data_ptr - cbor_data;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Parse and optionally pretty-print CBOR data.
 ******************************************************************************/
static sl_status_t _cbor_parse(uint8_t *cbor_data,
                               size_t *nbytes_read,
                               bool print_flag,
                               uint8_t indent_lvl,
                               uint8_t indent_width)
{
  uint8_t maj_type;
  uint8_t *data_ptr;
  uint64_t arg;
  size_t item_size, nitems_in_queue;
  sl_status_t ret;

  data_ptr = cbor_data;

  /// Parse major type and argument of current CBOR item
  ret = _cbor_parse_maj_and_arg(data_ptr, &maj_type, &arg, &item_size);
  if (ret != SL_STATUS_OK) {
    return ret;
  }
  data_ptr += item_size;

  if (print_flag) {
    printf("%*s%02x", indent_lvl * indent_width, "", cbor_data[0]);
  }

  nitems_in_queue = 0;

  switch (maj_type) {
    case CBOR_UNSIGNED_INT:
      if (print_flag) {
        printf("    ; int(%" PRIu64 ")\n", arg);
      }
      break;

    case CBOR_NEGATIVE_INT:
      if (print_flag) {
        printf("    ; int(%" PRIi64 ")\n", -(arg + 1));
      }
      break;

    case CBOR_BYTE_STR:
      if (print_flag) {
        printf("    ; byte_str(%" PRIu64 ")\n", arg);
        printf("%*s", (indent_lvl + 1) * indent_width, "");
        for (uint64_t i = 0; i < arg; i++) {
          printf("%02x", *(data_ptr++));
        }
        printf("\n");
      } else {
        data_ptr += arg;
      }
      break;

    case CBOR_TEXT_STR:
      if (print_flag) {
        printf("    ; text_str(%" PRIu64 ")\n", arg);
        printf("%*s\"", (indent_lvl + 1) * indent_width, "");
        for (uint64_t i = 0; i < arg; i++) {
          printf("%c", *(data_ptr++));
        }
        printf("\"\n");
      } else {
        data_ptr += arg;
      }
      break;

    case CBOR_ARRAY:
      if (print_flag) {
        printf("    ; array(%" PRIu64 ")\n", arg);
      }
      nitems_in_queue = arg;
      break;

    case CBOR_MAP:
      if (print_flag) {
        printf("    ; map(%" PRIu64 ")\n", arg);
      }
      nitems_in_queue = 2 * arg;
      break;

    case CBOR_TAG:
      if (print_flag) {
        printf("    ; tag(%" PRIu64 ")\n", arg);
      }
      nitems_in_queue = 1;
      break;

    case CBOR_SIMPLE_FLOAT:

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }

  /// Parse queued items from CBOR array, map or tag types.
  while (nitems_in_queue--) {
    ret = _cbor_parse(data_ptr,
                      &item_size,
                      print_flag,
                      indent_lvl + 1,
                      indent_width);

    if (ret != SL_STATUS_OK) {
      return ret;
    }

    data_ptr += item_size;
  }

  *nbytes_read = data_ptr - cbor_data;

  return SL_STATUS_OK;
}
