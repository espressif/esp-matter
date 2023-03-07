/***************************************************************************//**
 * @file app_process.c
 * @brief Top level application functions.
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
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Retrieve input character from VCOM port.
 ******************************************************************************/
static void app_iostream_usart_process_action(void);

/***************************************************************************//**
 * Print key storage.
 ******************************************************************************/
static void print_key_storage(void);

/***************************************************************************//**
 * Print asymmetric key size option.
 ******************************************************************************/
static void print_key_size_option(void);

/***************************************************************************//**
 * Print export public key.
 ******************************************************************************/
static void print_export_public(void);

/***************************************************************************//**
 * Get test key buffer pointer.
 *
 * @param private False for public key, true for private key.
 * @returns Returns pointer to test key buffer.
 ******************************************************************************/
static uint8_t * get_test_key_buf_ptr(bool private);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Space press status
static bool space_press;

/// Enter press status
static bool enter_press;

/// State machine state variable
static state_t app_state = PSA_CRYPTO_INIT;

/// String for example
static uint8_t example_string[] = "PSA Crypto Asymmetric Key Example";

/// Asymmetric key storage selection
static uint8_t asymmetric_key_storage_select;

static const char *asymmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED"
};

/// Asymmetric key ECC curve selection
static uint8_t asymmetric_key_curve_select;

static const char *asymmetric_key_curve_string[] = {
  "SECPxxxR1",
  "MONTGOMERY",
  "Ed25519"
};

static const psa_ecc_family_t asymmetric_key_curve[] = {
  PSA_ECC_FAMILY_SECP_R1,
  PSA_ECC_FAMILY_MONTGOMERY,
  PSA_ECC_FAMILY_TWISTED_EDWARDS
};

/// ECC SECPR1 key size selection
static uint8_t secpr1_key_size_select;

static const char *secpr1_key_size_string[] = {
  "SECP192R1",
  "SECP256R1",
  "SECP384R1",
  "SECP521R1",
};

static const size_t secpr1_key_size[] = {
  192,
  256,
  384,
  521
};

/// ECC Montgomery key size selection
static uint8_t montgomery_key_size_select;

static const char *montgomery_key_size_string[] = {
  "CURVE25519 (X25519)",
  "CURVE448 (X448)"
};

static const size_t montgomery_key_size[] = {
  255,
  448
};

/// Asymmetric key usage selection
static uint8_t asymmetric_key_usage_select;

static const char *asymmetric_key_usage_string[] = {
  "NON-EXPORTABLE & NON-COPYABLE",
  "EXPORTABLE",
  "COPYABLE"
};

static const psa_key_usage_t asymmetric_key_usage[] = {
  0,
  PSA_KEY_USAGE_EXPORT,
  PSA_KEY_USAGE_COPY
};

/// SECP192R1 key pair
static const uint8_t secp192r1_private[] = {
  0x4f, 0xc2, 0x13, 0x5e, 0x07, 0x5f, 0x69, 0xe5,
  0x72, 0x93, 0x08, 0xc6, 0x70, 0xa5, 0x97, 0xaf,
  0x3d, 0x3d, 0x65, 0x6d, 0x90, 0x1e, 0x06, 0xbd
};

static const uint8_t secp192r1_public[] = {
  0x04,         // Uncompressed point format
  0xe1, 0x8e, 0x21, 0xaa, 0x15, 0xd2, 0x7a, 0x71,
  0x3c, 0x9b, 0x5d, 0x8a, 0xb9, 0x27, 0x9a, 0xff,
  0xae, 0x18, 0x2c, 0x32, 0xc5, 0x8b, 0x9a, 0x71,
  0x65, 0xdd, 0x82, 0xc0, 0x5a, 0x3b, 0x84, 0xb1,
  0xac, 0x33, 0x5b, 0xf3, 0x42, 0x83, 0xce, 0x34,
  0xad, 0x5a, 0x9a, 0x3b, 0xe2, 0x68, 0x41, 0xbe
};

/// SECP256R1 key pair
static const uint8_t secp256r1_private[] = {
  0xac, 0x3f, 0x26, 0xd2, 0x0f, 0x24, 0x24, 0xe4,
  0xbb, 0x25, 0x6f, 0x30, 0x25, 0xe8, 0xd9, 0x75,
  0x9a, 0xaa, 0x06, 0x45, 0x1d, 0xe2, 0xdf, 0xdb,
  0x6e, 0x02, 0x7d, 0x72, 0x98, 0xcc, 0x1d, 0xb1
};

static const uint8_t secp256r1_public[] = {
  0x04,         // Uncompressed point format
  0xb1, 0xbc, 0x6f, 0x6f, 0xa5, 0x66, 0x40, 0xed,
  0x52, 0x2b, 0x2e, 0xe0, 0xf5, 0xb3, 0xcf, 0x7e,
  0x5d, 0x48, 0xf6, 0x0b, 0xe8, 0x14, 0x8f, 0x0d,
  0xc0, 0x84, 0x40, 0xf0, 0xa4, 0xe1, 0xdc, 0xa4,
  0x7c, 0x04, 0x11, 0x9e, 0xd6, 0xa1, 0xbe, 0x31,
  0xb7, 0x70, 0x7e, 0x5f, 0x9d, 0x00, 0x1a, 0x65,
  0x9a, 0x05, 0x10, 0x03, 0xe9, 0x5e, 0x1b, 0x93,
  0x6f, 0x05, 0xc3, 0x7e, 0xa7, 0x93, 0xad, 0x63
};

/// SECP384R1 key pair
static const uint8_t secp384r1_private[] = {
  0x3f, 0x5d, 0x8d, 0x9b, 0xe2, 0x80, 0xb5, 0x69,
  0x6c, 0xc5, 0xcc, 0x9f, 0x94, 0xcf, 0x8a, 0xf7,
  0xe6, 0xb6, 0x1d, 0xd6, 0x59, 0x2b, 0x2a, 0xb2,
  0xb3, 0xa4, 0xc6, 0x07, 0x45, 0x04, 0x17, 0xec,
  0x32, 0x7d, 0xcd, 0xca, 0xed, 0x7c, 0x10, 0x05,
  0x3d, 0x71, 0x9a, 0x05, 0x74, 0xf0, 0xa7, 0x6a
};

static const uint8_t secp384r1_public[] = {
  0x04,         // Uncompressed point format
  0xd9, 0xc6, 0x62, 0xb5, 0x0b, 0xa2, 0x9c, 0xa4,
  0x79, 0x90, 0x45, 0x0e, 0x04, 0x3a, 0xea, 0xf4,
  0xf0, 0xc6, 0x9b, 0x15, 0x67, 0x6d, 0x11, 0x2f,
  0x62, 0x2a, 0x71, 0xc9, 0x30, 0x59, 0xaf, 0x99,
  0x96, 0x91, 0xc5, 0x68, 0x0d, 0x2b, 0x44, 0xd1,
  0x11, 0x57, 0x9d, 0xb1, 0x2f, 0x4a, 0x41, 0x3a,
  0x2e, 0xd5, 0xc4, 0x5f, 0xcf, 0xb6, 0x7b, 0x5b,
  0x63, 0xe0, 0x0b, 0x91, 0xeb, 0xe5, 0x9d, 0x09,
  0xa6, 0xb1, 0xac, 0x2c, 0x0c, 0x42, 0x82, 0xaa,
  0x12, 0x31, 0x7e, 0xd5, 0x91, 0x4f, 0x99, 0x9b,
  0xc4, 0x88, 0xbb, 0x13, 0x2e, 0x83, 0x42, 0xcc,
  0x36, 0xf2, 0xca, 0x5e, 0x33, 0x79, 0xc7, 0x47
};

/// SECP521R1 key pair
static const uint8_t secp521r1_private[] = {
  0x01, 0xb1, 0xb6, 0xad, 0x07, 0xbb, 0x79, 0xe7,
  0x32, 0x0d, 0xa5, 0x98, 0x60, 0xea, 0x28, 0xe0,
  0x55, 0x28, 0x4f, 0x60, 0x58, 0xf2, 0x79, 0xde,
  0x66, 0x6e, 0x06, 0xd4, 0x35, 0xd2, 0xaf, 0x7b,
  0xda, 0x28, 0xd9, 0x9f, 0xa4, 0x7b, 0x7d, 0xd0,
  0x96, 0x3e, 0x16, 0xb0, 0x07, 0x30, 0x78, 0xee,
  0x8b, 0x8a, 0x38, 0xd9, 0x66, 0xa5, 0x82, 0xf4,
  0x6d, 0x19, 0xff, 0x95, 0xdf, 0x3a, 0xd9, 0x68,
  0x5a, 0xae
};

static const uint8_t secp521r1_public[] = {
  0x04,         // Uncompressed point format
  0x00, 0x1d, 0xe1, 0x42, 0xd5, 0x4f, 0x69, 0xeb,
  0x03, 0x8e, 0xe4, 0xb7, 0xaf, 0x9d, 0x3c, 0xa0,
  0x77, 0x36, 0xfd, 0x9c, 0xf7, 0x19, 0xeb, 0x35,
  0x4d, 0x69, 0x87, 0x9e, 0xe7, 0xf3, 0xc1, 0x36,
  0xfb, 0x0f, 0xbf, 0x9f, 0x08, 0xf8, 0x6b, 0xe5,
  0xfa, 0x12, 0x8e, 0xc1, 0xa0, 0x51, 0xd3, 0xe6,
  0xc6, 0x43, 0xe8, 0x5a, 0xda, 0x8f, 0xfa, 0xcf,
  0x36, 0x63, 0xc2, 0x60, 0xbd, 0x2c, 0x84, 0x4b,
  0x6f, 0x56, 0x00, 0xce, 0xe8, 0xe4, 0x8a, 0x9e,
  0x65, 0xd0, 0x9c, 0xad, 0xd8, 0x9f, 0x23, 0x5d,
  0xee, 0x05, 0xf3, 0xb8, 0xa6, 0x46, 0xbe, 0x71,
  0x5f, 0x1f, 0x67, 0xd5, 0xb4, 0x34, 0xe0, 0xff,
  0x23, 0xa1, 0xfc, 0x07, 0xef, 0x77, 0x40, 0x19,
  0x3e, 0x40, 0xee, 0xff, 0x6f, 0x3b, 0xcd, 0xfd,
  0x76, 0x5a, 0xa9, 0x15, 0x50, 0x33, 0x52, 0x4f,
  0xe4, 0xf2, 0x05, 0xf5, 0x44, 0x4e, 0x29, 0x2c,
  0x4c, 0x2f, 0x6a, 0xc1
};

/// X25519 key pair
static const uint8_t x25519_private[] = {
  0xe0, 0x95, 0x00, 0x32, 0xb1, 0xdf, 0x10, 0x08,
  0x38, 0xbc, 0x4d, 0x2d, 0x65, 0x5d, 0x07, 0x69,
  0xc9, 0xda, 0x4c, 0x2c, 0x48, 0xbc, 0x00, 0xa1,
  0x6c, 0x81, 0xe4, 0x6b, 0xdd, 0x40, 0xdd, 0x4a
};

static const uint8_t x25519_public[] = {
  0x67, 0x77, 0x63, 0x00, 0x29, 0xd6, 0x25, 0x11,
  0x23, 0x54, 0x4d, 0x70, 0x6d, 0x39, 0x4c, 0x6b,
  0x9b, 0x93, 0x43, 0x6b, 0x8b, 0xf2, 0xb3, 0x50,
  0x9f, 0xe2, 0xd7, 0x80, 0x81, 0x30, 0x37, 0x01
};

/// X448 key pair
static const uint8_t x448_private[] = {
  0x74, 0x18, 0x7f, 0xaf, 0xa1, 0xa6, 0x4a, 0x64,
  0xf1, 0x3f, 0x56, 0x7c, 0x06, 0x2a, 0x78, 0x36,
  0xb0, 0xff, 0x4b, 0x18, 0xcc, 0x52, 0x8e, 0x2d,
  0x91, 0x66, 0xbc, 0x49, 0x5c, 0x75, 0xdf, 0x3c,
  0x7d, 0x0d, 0xa1, 0xb6, 0xa7, 0xa4, 0x51, 0x1b,
  0x43, 0xd7, 0xb4, 0xe9, 0x21, 0xb8, 0x50, 0xbb,
  0x06, 0x3b, 0xf0, 0xfd, 0x76, 0xa0, 0x10, 0xb5
};

static const uint8_t x448_public[] = {
  0x03, 0x73, 0x17, 0x73, 0x2c, 0x78, 0xe2, 0x64,
  0xa2, 0x3c, 0x7d, 0x32, 0xbd, 0xc8, 0x2a, 0x8b,
  0x93, 0x03, 0x71, 0xf6, 0x3e, 0x7c, 0xe2, 0x20,
  0x53, 0x05, 0x9b, 0x17, 0x03, 0x0b, 0xf8, 0xc1,
  0x0c, 0xfa, 0x7e, 0xdc, 0xd3, 0xb8, 0xd7, 0x23,
  0x8f, 0x62, 0xf5, 0x41, 0x8d, 0xf2, 0x69, 0xb6,
  0xdc, 0x7c, 0x3c, 0x57, 0x4f, 0xd9, 0x34, 0xb1
};

/// Ed25519 key pair
static const uint8_t ed25519_private[] = {
  0x83, 0x3f, 0xe6, 0x24, 0x09, 0x23, 0x7b, 0x9d,
  0x62, 0xec, 0x77, 0x58, 0x75, 0x20, 0x91, 0x1e,
  0x9a, 0x75, 0x9c, 0xec, 0x1d, 0x19, 0x75, 0x5b,
  0x7d, 0xa9, 0x01, 0xb9, 0x6d, 0xca, 0x3d, 0x42
};

static const uint8_t ed25519_public[] = {
  0xec, 0x17, 0x2b, 0x93, 0xad, 0x5e, 0x56, 0x3b,
  0xf4, 0x93, 0x2c, 0x70, 0xe1, 0x24, 0x50, 0x34,
  0xc3, 0x54, 0x67, 0xef, 0x2e, 0xfd, 0x4d, 0x64,
  0xeb, 0xf8, 0x19, 0x68, 0x34, 0x67, 0xe2, 0xbf
};

/// Selected key size
static size_t selected_key_size;

/// Selected key string
static const char *selected_key_string;

/// Algorithm for asymmetric key
static psa_algorithm_t key_algo;

/// Key storage for public key import
static uint8_t key_storage_public;

/// Source key identifier
static psa_key_id_t source_key_id;

/// Copied key identifier
static psa_key_id_t copy_key_id;

/// Key process
static uint8_t key_process;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  psa_status_t ret = PSA_SUCCESS;

  // Retrieve input character from VCOM port
  app_iostream_usart_process_action();

  switch (app_state) {
    case PSA_CRYPTO_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . PSA Crypto initialization... ");
      if (init_psa_crypto() == PSA_SUCCESS) {
        print_key_storage();
      } else {
        app_state = PSA_CRYPTO_IDLE;
      }
      break;

    case SELECT_KEY_STORAGE:
      if (space_press) {
        space_press = false;
        asymmetric_key_storage_select++;
        if (asymmetric_key_storage_select > KEY_STORAGE_MAX) {
          asymmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
        printf("  + Current asymmetric key is a %s key.\n",
               asymmetric_key_storage_string[asymmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        key_storage_public = asymmetric_key_storage_select;
#if defined(SEMAILBOX_PRESENT)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("\n  . Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
        printf("  + Press SPACE to select ECC curve (%s or %s or %s), press "
               "ENTER to next option.\n",
               asymmetric_key_curve_string[0],
               asymmetric_key_curve_string[1],
               asymmetric_key_curve_string[2]);
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 1) {
          printf("\n  . Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        } else {
          printf("\n  . Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        }
        printf("  + Press SPACE to select ECC curve (%s or %s or %s), press "
               "ENTER to next option.\n",
               asymmetric_key_curve_string[0],
               montgomery_key_size_string[0],
               asymmetric_key_curve_string[2]);
#endif
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 0) {
          printf("\n  . Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        } else {
          printf("\n  . Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        }
        printf("  + Press SPACE to select ECC curve (%s or %s), press ENTER to "
               "next option.\n",
               asymmetric_key_curve_string[0],
               montgomery_key_size_string[0]);
#endif
        app_state = SELECT_KEY_CURVE;
      }
      break;

    case SELECT_KEY_CURVE:
      if (space_press) {
        space_press = false;
        asymmetric_key_curve_select++;
        if (asymmetric_key_curve_select > KEY_CURVE_MAX) {
          asymmetric_key_curve_select = 0;
        }
#if defined(SEMAILBOX_PRESENT)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 1) {
          printf("  + Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        } else {
          printf("  + Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        }
#endif
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 0) {
          printf("  + Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        } else {
          printf("  + Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        }
#endif
      }
      if (enter_press) {
        enter_press = false;
        print_key_size_option();
      }
      break;

    case SELECT_SECPR1_SIZE:
      if (space_press) {
        space_press = false;
        secpr1_key_size_select++;
        if (secpr1_key_size_select > SECPR1_SIZE_MAX) {
          secpr1_key_size_select = 0;
        }
        printf("  + Current %s key length is %d-bit (%s).\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select],
               secpr1_key_size[secpr1_key_size_select],
               secpr1_key_size_string[secpr1_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key_size = secpr1_key_size[secpr1_key_size_select];
        selected_key_string = secpr1_key_size_string[secpr1_key_size_select];
        printf("\n  . Current asymmetric key usage is %s.\n",
               asymmetric_key_usage_string[asymmetric_key_usage_select]);
        printf("  + Press SPACE to select key usage (%s or %s or %s), press "
               "ENTER to run.\n", asymmetric_key_usage_string[0],
               asymmetric_key_usage_string[1], asymmetric_key_usage_string[2]);
        app_state = SELECT_KEY_USAGE;
      }
      break;

    case SELECT_MONTGOMERY_SIZE:
      if (space_press) {
        space_press = false;
        montgomery_key_size_select++;
        if (montgomery_key_size_select > MONTGOMERY_SIZE_MAX) {
          montgomery_key_size_select = 0;
        }
        printf("  + Current %s key length is %d-bit (%s).\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select],
               montgomery_key_size[montgomery_key_size_select],
               montgomery_key_size_string[montgomery_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key_size = montgomery_key_size[montgomery_key_size_select];
        selected_key_string = montgomery_key_size_string[montgomery_key_size_select];
        printf("\n  . Current asymmetric key usage is %s.\n",
               asymmetric_key_usage_string[asymmetric_key_usage_select]);
        printf("  + Press SPACE to select key usage (%s or %s or %s), press "
               "ENTER to run.\n", asymmetric_key_usage_string[0],
               asymmetric_key_usage_string[1], asymmetric_key_usage_string[2]);
        app_state = SELECT_KEY_USAGE;
      }
      break;

    case SELECT_KEY_USAGE:
      if (space_press) {
        space_press = false;
        asymmetric_key_usage_select++;
        if (asymmetric_key_usage_select > KEY_USAGE_MAX) {
          asymmetric_key_usage_select = 0;
        }
        printf("  + Current asymmetric key usage is %s.\n",
               asymmetric_key_usage_string[asymmetric_key_usage_select]);
      }
      if (enter_press) {
        enter_press = false;
        // Wrapped key cannot copy to other location
        if ((asymmetric_key_usage[asymmetric_key_usage_select] & PSA_KEY_USAGE_COPY)
            && asymmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
          printf("\n  . Wrapped key should be NON-COPYABLE!\n");
          print_key_storage();
        } else {
          app_state = CREATE_KEY;
        }
      }
      break;

    case CREATE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Creating a %s (%d-bit) %s %s key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_usage_string[asymmetric_key_usage_select],
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            PERSISTENT_KEY_ID,
                            asymmetric_key_usage[asymmetric_key_usage_select],
                            key_algo) == PSA_SUCCESS) {
        // Free resources and save create key id to destroy
        reset_key_attr();
        source_key_id = get_key_id();
        key_process = 0;
        if (asymmetric_key_usage[asymmetric_key_usage_select]
            & PSA_KEY_USAGE_EXPORT) {
          // Export private key if key is exportable
          app_state = EXPORT_PRIVATE_KEY;
        } else {
          app_state = EXPORT_PUBLIC_KEY;
          print_export_public();
        }
      }
      break;

    case IMPORT_PRIVATE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Importing a %s (%d-bit) %s %s private key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_usage_string[asymmetric_key_usage_select],
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      // Setup buffer and length to import private key
      memcpy(get_key_buf_ptr(), get_test_key_buf_ptr(true),
             PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                                        selected_key_size));
      set_key_len(PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                                             selected_key_size));
      if (create_import_key(IMPORT_KEY_OFFSET + asymmetric_key_storage_select,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            PERSISTENT_KEY_ID,
                            asymmetric_key_usage[asymmetric_key_usage_select],
                            key_algo) == PSA_SUCCESS) {
        // Free resources and save import key id to destroy
        reset_key_attr();
        source_key_id = get_key_id();
        key_process = 1;
        if (asymmetric_key_usage[asymmetric_key_usage_select]
            & PSA_KEY_USAGE_EXPORT) {
          // Export private key if key is exportable
          app_state = EXPORT_PRIVATE_KEY;
        } else {
          app_state = EXPORT_PUBLIC_KEY;
          print_export_public();
        }
      }
      break;

    case EXPORT_PRIVATE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Exporting a private key of a %s (%d-bit) %s %s key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_usage_string[asymmetric_key_usage_select],
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      // Clear key buffer to export key
      memset(get_key_buf_ptr(), 0, KEY_BUF_SIZE);
      if (export_key() == PSA_SUCCESS) {
        if (key_process == 1) {
          printf("  + Comparing export private key with import private "
                 "key... ");
          if (memcmp(get_key_buf_ptr(), get_test_key_buf_ptr(true),
                     get_key_len()) == 0) {
            printf("OK\n");
            app_state = EXPORT_PUBLIC_KEY;
            print_export_public();
          } else {
            printf("Failed\n");
          }
        } else {
          // No comparison if from key creation
          app_state = EXPORT_PUBLIC_KEY;
          print_export_public();
        }
      }
      break;

    case EXPORT_PUBLIC_KEY:
      app_state = PSA_CRYPTO_EXIT;
      // Clear key buffer to export key
      memset(get_key_buf_ptr(), 0, KEY_BUF_SIZE);
      if (export_public_key() == PSA_SUCCESS) {
        switch (key_process) {
          case 0:       // From key creation
            app_state = DESTROY_KEY;
            if (asymmetric_key_usage[asymmetric_key_usage_select]
                & PSA_KEY_USAGE_COPY) {
              app_state = COPY_KEY;
            }
            break;

          case 1:       // From import private key
            printf("  + Comparing export public key with import public "
                   "key... ");
            if (memcmp(get_key_buf_ptr(), get_test_key_buf_ptr(false),
                       get_key_len()) == 0) {
              printf("OK\n");
              if (asymmetric_key_usage[asymmetric_key_usage_select]
                  & PSA_KEY_USAGE_COPY) {
                app_state = COPY_KEY;
              } else {
                app_state = DESTROY_KEY;
              }
            } else {
              printf("Failed\n");
            }
            break;

          case 2:       // From key creation
            app_state = DESTROY_KEY;
            break;

          case 3:       // From import private key
          case 4:       // From import public key
            printf("  + Comparing export public key with import public "
                   "key... ");
            if (memcmp(get_key_buf_ptr(), get_test_key_buf_ptr(false),
                       get_key_len()) == 0) {
              printf("OK\n");
              app_state = DESTROY_KEY;
            } else {
              printf("Failed\n");
            }
            break;

          default:
            break;
        }
      }
      break;

    case COPY_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Copying a %s (%d-bit) %s key... ", selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select ^ 0x01]);
      // Copy from create or import key and lifetime is storage ^= 0x01
      if ((asymmetric_key_storage_select ^ 0x01) == VOLATILE_PLAIN_KEY) {
        ret = copy_key(0, DEFAULT_KEY_USAGE, key_algo);
      } else {
        ret = copy_key(PERSISTENT_COPY_KEY_ID, DEFAULT_KEY_USAGE,
                       key_algo);
      }
      if (ret == PSA_SUCCESS) {
        // Free resources and save copied key id to destroy
        reset_key_attr();
        copy_key_id = get_key_id();
        app_state = EXPORT_PUBLIC_KEY;
        key_process += 2;
        printf("\n  . Exporting a public key of a %s (%d-bit) %s copied "
               "key... ", selected_key_string, selected_key_size,
               asymmetric_key_storage_string[key_storage_public ^ 0x01]);
      }
      break;

    case IMPORT_PUBLIC_KEY:
      app_state = PSA_CRYPTO_EXIT;
      // Setup buffer and length to import public key
      if (asymmetric_key_curve_select == 0) {
        memcpy(get_key_buf_ptr(), get_test_key_buf_ptr(false),
               PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(asymmetric_key_curve[asymmetric_key_curve_select]),
                                          selected_key_size));
        set_key_len(PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(asymmetric_key_curve[asymmetric_key_curve_select]),
                                               selected_key_size));
      } else {
        // PSA_EXPORT_KEY_OUTPUT_SIZE() doesn't support Montgomery or Ed25519 public key yet
        if ((selected_key_size % 8) != 0) {
          // X25519 or Ed25519
          memcpy(get_key_buf_ptr(), get_test_key_buf_ptr(false),
                 (selected_key_size + 1) / 8);
          set_key_len((selected_key_size + 1) / 8);
        } else {
          // X448
          memcpy(get_key_buf_ptr(), get_test_key_buf_ptr(false),
                 selected_key_size / 8);
          set_key_len(selected_key_size / 8);
        }
      }
      // Public key cannot be wrapped
      if (asymmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
        key_storage_public -= VOLATILE_WRAP_KEY;
      }
      printf("\n  . Importing a %s (%d-bit) %s %s public key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_usage_string[asymmetric_key_usage_select],
             asymmetric_key_storage_string[key_storage_public]);
      if (create_import_key(IMPORT_KEY_OFFSET + key_storage_public,
                            PSA_KEY_TYPE_ECC_PUBLIC_KEY(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            PERSISTENT_KEY_ID,
                            asymmetric_key_usage[asymmetric_key_usage_select],
                            key_algo) == PSA_SUCCESS) {
        // Free resources and save import key id to destroy
        reset_key_attr();
        source_key_id = get_key_id();
        // Public key is exportable
        app_state = EXPORT_PUBLIC_KEY;
        print_export_public();
        key_process = 4;
      }
      break;

    case DESTROY_KEY:
      app_state = PSA_CRYPTO_EXIT;
      set_key_id(source_key_id);                // Destroy create or import key
      printf("  + Destroying a %s (%d-bit) %s %s key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_usage_string[asymmetric_key_usage_select],
             asymmetric_key_storage_string[key_storage_public]);

      if (destroy_key() == PSA_SUCCESS) {
        source_key_id = 0;                      // Reset source key identifier
        switch (key_process) {
          case 0:       // From key creation
            app_state = IMPORT_PRIVATE_KEY;
            if (get_test_key_buf_ptr(true) == NULL) {
              print_key_storage();
            }
            break;

          case 1:       // From import private key
            app_state = IMPORT_PUBLIC_KEY;
            break;

          case 2:       // Copy from key creation
          case 3:       // Copy from import private key
            set_key_id(copy_key_id);            // Destroy copied key
            printf("  + Destroying a %s (%d-bit) %s copied key... ",
                   selected_key_string, selected_key_size,
                   asymmetric_key_storage_string[asymmetric_key_storage_select ^ 0x01]);
            if (destroy_key() == PSA_SUCCESS) {
              copy_key_id = 0;                  // Reset copied key identifier
              if (key_process == 2 && get_test_key_buf_ptr(true) != NULL) {
                app_state = IMPORT_PRIVATE_KEY;
              } else {
                print_key_storage();
              }
            }
            break;

          case 4:       // From import public key
            print_key_storage();
            break;

          default:
            break;
        }
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy the keys regardless they are valid or not
      set_key_id(source_key_id);
      printf("  + Destroying a key... ");
      destroy_key();
      set_key_id(copy_key_id);
      printf("  + Destroying a copied key... ");
      destroy_key();
      mbedtls_psa_crypto_free();
      app_state = PSA_CRYPTO_IDLE;
      break;

    case PSA_CRYPTO_IDLE:
    default:
      break;
  }
}

#if defined(MBEDTLS_ENTROPY_RAIL_PRESENT)
/***************************************************************************//**
 * It cancels the MCU to go to sleep when RAIL entropy is used.
 ******************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return false;
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Retrieve input character from VCOM port.
 ******************************************************************************/
static void app_iostream_usart_process_action(void)
{
  int8_t c;

  c = getchar();
  if (c > 0) {
    if (c == ' ') {
      space_press = true;
    }
    if (c == '\r') {
      enter_press = true;
    }
  }
}

/***************************************************************************//**
 * Print key storage.
 ******************************************************************************/
static void print_key_storage(void)
{
  // Check if NVM3 can release any out-of-date objects to free up memory.
  while (nvm3_repackNeeded(nvm3_defaultHandle)) {
    printf("\n  . Repacking NVM... ");
    if (nvm3_repack(nvm3_defaultHandle) == ECODE_NVM3_OK) {
      printf("OK\n");
    } else {
      printf("Failed\n");
    }
  }

  printf("\n  . Current asymmetric key is a %s key.\n",
         asymmetric_key_storage_string[asymmetric_key_storage_select]);
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s key, press ENTER to "
         "next option.\n",
         asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1],
         asymmetric_key_storage_string[2],
         asymmetric_key_storage_string[3]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1]);
#endif
  app_state = SELECT_KEY_STORAGE;
}

/***************************************************************************//**
 * Print asymmetric key size option.
 ******************************************************************************/
static void print_key_size_option(void)
{
  if (asymmetric_key_curve_select == 0) {
    key_algo = PSA_ALG_ECDSA_ANY;
    printf("\n  . Current %s key length is %d-bit (%s).\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           secpr1_key_size[secpr1_key_size_select],
           secpr1_key_size_string[secpr1_key_size_select]);
    printf("  + Press SPACE to select %s key length (%d or %d or %d or %d), "
           "press ENTER to next option.\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           secpr1_key_size[0], secpr1_key_size[1], secpr1_key_size[2],
           secpr1_key_size[3]);
    app_state = SELECT_SECPR1_SIZE;
  } else if (asymmetric_key_curve_select == 1) {
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    key_algo = PSA_ALG_ECDH;
    printf("\n  . Current %s key length is %d-bit (%s).\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           montgomery_key_size[montgomery_key_size_select],
           montgomery_key_size_string[montgomery_key_size_select]);
    printf("  + Press SPACE to select %s key length (%d or %d), press ENTER to "
           "next option.\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           montgomery_key_size[0], montgomery_key_size[1]);
    app_state = SELECT_MONTGOMERY_SIZE;
#else
    // No software fallback on X448, only X25519 is available
    selected_key_size = 255;
    selected_key_string = asymmetric_key_curve_string[asymmetric_key_curve_select];
    key_algo = PSA_ALG_ECDH;
    printf("\n  . Current asymmetric key usage is %s.\n",
           asymmetric_key_usage_string[asymmetric_key_usage_select]);
    printf("  + Press SPACE to select key usage (%s or %s or %s), press "
           "ENTER to run.\n", asymmetric_key_usage_string[0],
           asymmetric_key_usage_string[1], asymmetric_key_usage_string[2]);
    app_state = SELECT_KEY_USAGE;
#endif
  } else {
    // Ed25519 can only use for EdDSA
    selected_key_size = 255;
    selected_key_string = asymmetric_key_curve_string[asymmetric_key_curve_select];
    key_algo = PSA_ALG_PURE_EDDSA;
    printf("\n  . Current asymmetric key usage is %s.\n",
           asymmetric_key_usage_string[asymmetric_key_usage_select]);
    printf("  + Press SPACE to select key usage (%s or %s or %s), press "
           "ENTER to run.\n", asymmetric_key_usage_string[0],
           asymmetric_key_usage_string[1], asymmetric_key_usage_string[2]);
    app_state = SELECT_KEY_USAGE;
  }
}

/***************************************************************************//**
 * Print export public key.
 ******************************************************************************/
static void print_export_public(void)
{
  printf("\n  . Exporting a public key of a %s (%d-bit) %s %s key... ",
         selected_key_string, selected_key_size,
         asymmetric_key_usage_string[asymmetric_key_usage_select],
         asymmetric_key_storage_string[key_storage_public]);
}

/***************************************************************************//**
 * Get test key buffer pointer.
 ******************************************************************************/
static uint8_t * get_test_key_buf_ptr(bool private)
{
  switch (selected_key_size) {
    case 192:
      if (private) {
        return((uint8_t *)secp192r1_private);
      } else {
        return((uint8_t *)secp192r1_public);
      }

    case 256:
      if (private) {
        return((uint8_t *)secp256r1_private);
      } else {
        return((uint8_t *)secp256r1_public);
      }

    case 384:
      if (private) {
        return((uint8_t *)secp384r1_private);
      } else {
        return((uint8_t *)secp384r1_public);
      }

    case 521:
      if (private) {
        return((uint8_t *)secp521r1_private);
      } else {
        return((uint8_t *)secp521r1_public);
      }

    case 255:
      if (asymmetric_key_curve_select == 1) {
        if (private) {
          return((uint8_t *)x25519_private);
        } else {
          return((uint8_t *)x25519_public);
        }
      } else {
        if (private) {
          return((uint8_t *)ed25519_private);
        } else {
          return((uint8_t *)ed25519_public);
        }
      }

    case 448:
      if (private) {
        return((uint8_t *)x448_private);
      } else {
        return((uint8_t *)x448_public);
      }

    default:
      return NULL;
  }
}
