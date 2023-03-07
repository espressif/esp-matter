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

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Key Derivation (HKDF and PBKDF2) Example";

/// Test vector for HKDF
/// Input key material
static const uint8_t hkdf_input_key_material[] = {
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};

/// Salt (optional)
static const uint8_t hkdf_salt[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c
};

/// Information (optional)
static const uint8_t hkdf_information[] = {
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9
};

/// Expect output key material (okm) without salt and information (SHA256)
static const uint8_t expect_sha256_no_salt_info_hkdf_okm[] = {
  0x8d, 0xa4, 0xe7, 0x75, 0xa5, 0x63, 0xc1, 0x8f,
  0x71, 0x5f, 0x80, 0x2a, 0x06, 0x3c, 0x5a, 0x31,
  0xb8, 0xa1, 0x1f, 0x5c, 0x5e, 0xe1, 0x87, 0x9e,
  0xc3, 0x45, 0x4e, 0x5f, 0x3c, 0x73, 0x8d, 0x2d,
  0x9d, 0x20, 0x13, 0x95, 0xfa, 0xa4, 0xb6, 0x1a,
  0x96, 0xc8
};

/// Expect output key material (okm) with salt and information (SHA256)
static const uint8_t expect_sha256_hkdf_okm[] = {
  0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a,
  0x90, 0x43, 0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a,
  0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c,
  0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf,
  0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18,
  0x58, 0x65
};

/// Expect output key material (okm) with salt and information (SHA512)
static const uint8_t expect_sha512_hkdf_okm[] = {
  0x83, 0x23, 0x90, 0x08, 0x6c, 0xda, 0x71, 0xfb,
  0x47, 0x62, 0x5b, 0xb5, 0xce, 0xb1, 0x68, 0xe4,
  0xc8, 0xe2, 0x6a, 0x1a, 0x16, 0xed, 0x34, 0xd9,
  0xfc, 0x7f, 0xe9, 0x2c, 0x14, 0x81, 0x57, 0x93,
  0x38, 0xda, 0x36, 0x2c, 0xb8, 0xd9, 0xf9, 0x25,
  0xd7, 0xcb
};

/// Test vector for PBKDF2
/// Input key material
static const uint8_t pbkdf2_input_key_material[] = "password";

/// Salt
static const uint8_t pbkdf2_salt[] = "salt";

/// Expect output key material (okm) with 1 iteration
static const uint8_t expect_iteration_1_pbkdf2_okm[] = {
  0x12, 0x0f, 0xb6, 0xcf, 0xfc, 0xf8, 0xb3, 0x2c,
  0x43, 0xe7, 0x22, 0x52, 0x56, 0xc4, 0xf8, 0x37,
  0xa8, 0x65, 0x48, 0xc9, 0x2c, 0xcc, 0x35, 0x48,
  0x08, 0x05, 0x98, 0x7c, 0xb7, 0x0b, 0xe1, 0x7b
};

/// Expect output key material (okm) with 2 iterations
static const uint8_t expect_iteration_2_pbkdf2_okm[] = {
  0xae, 0x4d, 0x0c, 0x95, 0xaf, 0x6b, 0x46, 0xd3,
  0x2d, 0x0a, 0xdf, 0xf9, 0x28, 0xf0, 0x6d, 0xd0,
  0x2a, 0x30, 0x3f, 0x8e, 0xf3, 0xc2, 0x51, 0xdf,
  0xd6, 0xe2, 0xd8, 0x5a, 0x95, 0x47, 0x4c, 0x43
};

/// Expect output key material (okm) with 4096 iterations
static const uint8_t expect_iteration_4096_pbkdf2_okm[] = {
  0xc5, 0xe4, 0x78, 0xd5, 0x92, 0x88, 0xc8, 0x41,
  0xaa, 0x53, 0x0d, 0xb6, 0x84, 0x5c, 0x4c, 0x8d,
  0x96, 0x28, 0x93, 0xa0, 0x01, 0xce, 0x4e, 0x11,
  0xa4, 0x96, 0x38, 0x73, 0xaa, 0x98, 0x13, 0x4a
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely.
 *****************************************************************************/
void app_process_action(void)
{
  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        app_state = HKDF_TEST_SETUP;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case HKDF_TEST_SETUP:
      set_input_key_material_ptr((uint8_t *)hkdf_input_key_material);
      set_salt_ptr((uint8_t *)hkdf_salt);
      set_information_ptr((uint8_t *)hkdf_information);
      set_input_key_material_len(sizeof(hkdf_input_key_material));
      set_output_key_material_len(sizeof(expect_sha256_hkdf_okm));
      app_state = HKDF_TEST1;
      break;

    case HKDF_TEST1:
      set_salt_len(0);
      set_information_len(0);
      app_state = SE_MANAGER_EXIT;
      printf("\n  . HKDF test with zero-length salt and information "
             "(SHA256)\n");
      printf("  + Deriving %d bytes HKDF symmetric key... ",
             sizeof(expect_sha256_no_salt_info_hkdf_okm));
      if (derive_hkdf_key(SL_SE_HASH_SHA256) == SL_STATUS_OK) {
        printf("  + Comparing generated HKDF key with expected data... ");
        if (memcmp(expect_sha256_no_salt_info_hkdf_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_sha256_no_salt_info_hkdf_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
          app_state = HKDF_TEST2;
        }
      }
      break;

    case HKDF_TEST2:
      set_salt_len(sizeof(hkdf_salt));
      set_information_len(sizeof(hkdf_information));

      app_state = SE_MANAGER_EXIT;
      printf("\n  . HKDF test with salt and information (SHA256)\n");
      printf("  + Deriving %d bytes HKDF symmetric key... ",
             sizeof(expect_sha256_hkdf_okm));
      if (derive_hkdf_key(SL_SE_HASH_SHA256) == SL_STATUS_OK) {
        printf("  + Comparing generated HKDF key with expected data... ");
        if (memcmp(expect_sha256_hkdf_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_sha256_hkdf_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
          app_state = HKDF_TEST3;
        }
      }
      break;

    case HKDF_TEST3:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . HKDF test with salt and information (SHA512)\n");
      printf("  + Deriving %d bytes HKDF symmetric key... ",
             sizeof(expect_sha512_hkdf_okm));
      if (derive_hkdf_key(SL_SE_HASH_SHA512) == SL_STATUS_OK) {
        printf("  + Comparing generated HKDF key with expected data... ");
        if (memcmp(expect_sha512_hkdf_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_sha512_hkdf_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
          app_state = PBKDF2_TEST_SETUP;
        }
      }
      break;

    case PBKDF2_TEST_SETUP:
      set_input_key_material_ptr((uint8_t *)pbkdf2_input_key_material);
      set_salt_ptr((uint8_t *)pbkdf2_salt);
      set_salt_len(strlen((char *)pbkdf2_salt));
      set_input_key_material_len(strlen((char *)pbkdf2_input_key_material));
      set_output_key_material_len(sizeof(expect_iteration_1_pbkdf2_okm));
      app_state = PBKDF2_TEST1;
      break;

    case PBKDF2_TEST1:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . PBKDF2 test with 1 iteration (SHA256)\n");
      printf("  + Deriving %d bytes PBKDF2 symmetric key... ",
             sizeof(expect_iteration_1_pbkdf2_okm));
      if (derive_pbkdf2_key(SL_SE_PRF_HMAC_SHA256, 1) == SL_STATUS_OK) {
        printf("  + Comparing generated PBKDF2 key with expected data... ");
        if (memcmp(expect_iteration_1_pbkdf2_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_iteration_1_pbkdf2_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
          app_state = PBKDF2_TEST2;
        }
      }
      break;

    case PBKDF2_TEST2:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . PBKDF2 test with 2 iterations (SHA256)\n");
      printf("  + Deriving %d bytes PBKDF2 symmetric key... ",
             sizeof(expect_iteration_2_pbkdf2_okm));
      if (derive_pbkdf2_key(SL_SE_PRF_HMAC_SHA256, 2) == SL_STATUS_OK) {
        printf("  + Comparing generated PBKDF2 key with expected data... ");
        if (memcmp(expect_iteration_2_pbkdf2_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_iteration_2_pbkdf2_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
          app_state = PBKDF2_TEST3;
        }
      }
      break;

    case PBKDF2_TEST3:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . PBKDF2 test with 4096 iterations (SHA256)\n");
      printf("  + Deriving %d bytes PBKDF2 symmetric key... ",
             sizeof(expect_iteration_4096_pbkdf2_okm));
      if (derive_pbkdf2_key(SL_SE_PRF_HMAC_SHA256, 4096) == SL_STATUS_OK) {
        printf("  + Comparing generated PBKDF2 key with expected data... ");
        if (memcmp(expect_iteration_4096_pbkdf2_okm,
                   get_output_key_material_ptr(),
                   sizeof(expect_iteration_4096_pbkdf2_okm)) != 0) {
          printf("Failed\n");
        } else {
          printf("OK\n");
        }
      }
      break;

    case SE_MANAGER_EXIT:
      printf("\n  . SE manager deinitialization... ");
      deinit_se_manager();
      app_state = SE_MANAGER_IDLE;
      break;

    case SE_MANAGER_IDLE:
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
