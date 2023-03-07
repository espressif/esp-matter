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
static uint8_t example_string[] = "PSA Crypto Symmetric Key Example";

/// Symmetric key storage selection
static uint8_t symmetric_key_storage_select;

static const char *symmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED"
};

/// Symmetric key size selection
static uint8_t symmetric_key_size_select;

static const size_t symmetric_key_size[] = {
  128,
  192,
  256
};

/// Symmetric key usage selection
static uint8_t symmetric_key_usage_select;

static const char *symmetric_key_usage_string[] = {
  "NON-EXPORTABLE & NON-COPYABLE",
  "EXPORTABLE",
  "COPYABLE"
};

static const psa_key_usage_t symmetric_key_usage[] = {
  0,
  PSA_KEY_USAGE_EXPORT,
  PSA_KEY_USAGE_COPY
};

/// The 128-bit or 256-bit key for import
static const uint8_t key_import[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

/// Source key identifier
static psa_key_id_t source_key_id;

/// Copied key identifier
static psa_key_id_t copy_key_id;

/// Key process
static uint32_t key_process;

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
        symmetric_key_storage_select++;
        if (symmetric_key_storage_select > KEY_STORAGE_MAX) {
          symmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
        printf("  + Current symmetric key is a %s key.\n",
               symmetric_key_storage_string[symmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        printf("\n  . Current symmetric key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
        printf("  + Press SPACE to select symmetric key length (%d or %d or "
               "%d), press ENTER to next option.\n", symmetric_key_size[0],
               symmetric_key_size[1], symmetric_key_size[2]);
        app_state = SELECT_KEY_SIZE;
      }
      break;

    case SELECT_KEY_SIZE:
      if (space_press) {
        space_press = false;
        symmetric_key_size_select++;
        if (symmetric_key_size_select > KEY_SIZE_MAX) {
          symmetric_key_size_select = 0;
        }
        printf("  + Current symmetric key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        key_process = SYM_KEY_ALGO;
        if ((key_process == PSA_ALG_STREAM_CIPHER || key_process == PSA_ALG_CHACHA20_POLY1305)
            && (symmetric_key_size_select != KEY_SIZE_MAX)) {
          printf("  + CHACHA20 algorithm can only use 256-bit key, press SPACE "
                 "to select the correct key length.\n");
          break;
        }
        printf("\n  . Current symmetric key usage is %s.\n",
               symmetric_key_usage_string[symmetric_key_usage_select]);
        printf("  + Press SPACE to select key usage (%s or %s or %s), press "
               "ENTER to run.\n", symmetric_key_usage_string[0],
               symmetric_key_usage_string[1], symmetric_key_usage_string[2]);
        app_state = SELECT_KEY_USAGE;
      }
      break;

    case SELECT_KEY_USAGE:
      if (space_press) {
        space_press = false;
        symmetric_key_usage_select++;
        if (symmetric_key_usage_select > KEY_USAGE_MAX) {
          symmetric_key_usage_select = 0;
        }
        printf("  + Current symmetric key usage is %s.\n",
               symmetric_key_usage_string[symmetric_key_usage_select]);
      }
      if (enter_press) {
        enter_press = false;
        // Wrapped key cannot copy to other location
        if ((symmetric_key_usage[symmetric_key_usage_select] & PSA_KEY_USAGE_COPY)
            && symmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
          printf("\n  . Wrapped key should be NON-COPYABLE!\n");
          print_key_storage();
        } else {
          app_state = CREATE_KEY;
        }
      }
      break;

    case CREATE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Creating a %d-bit %s %s key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_usage_string[symmetric_key_usage_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      if (create_import_key(CREATE_KEY_OFFSET + symmetric_key_storage_select,
                            PSA_KEY_TYPE_NONE,
                            symmetric_key_size[symmetric_key_size_select],
                            PERSISTENT_KEY_ID,
                            symmetric_key_usage[symmetric_key_usage_select],
                            SYM_KEY_ALGO) == PSA_SUCCESS) {
        // Free resources and save create key id to destroy
        reset_key_attr();
        source_key_id = get_key_id();
        app_state = DESTROY_KEY;
        key_process = 0;
        if (symmetric_key_usage[symmetric_key_usage_select]
            & PSA_KEY_USAGE_EXPORT) {
          app_state = EXPORT_KEY;
          break;
        }
        if (symmetric_key_usage[symmetric_key_usage_select]
            & PSA_KEY_USAGE_COPY) {
          app_state = COPY_KEY;
        }
      }
      break;

    case IMPORT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Importing a %d-bit %s %s key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_usage_string[symmetric_key_usage_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      // Setup buffer and length for key import
      memcpy(get_key_buf_ptr(), key_import,
             symmetric_key_size[symmetric_key_size_select] / 8);
      set_key_len(symmetric_key_size[symmetric_key_size_select] / 8);
      if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                            PSA_KEY_TYPE_NONE,
                            symmetric_key_size[symmetric_key_size_select],
                            PERSISTENT_KEY_ID,
                            symmetric_key_usage[symmetric_key_usage_select],
                            SYM_KEY_ALGO) == PSA_SUCCESS) {
        // Free resources and save import key id to destroy
        reset_key_attr();
        source_key_id = get_key_id();
        app_state = DESTROY_KEY;
        key_process = 1;
        if (symmetric_key_usage[symmetric_key_usage_select]
            & PSA_KEY_USAGE_EXPORT) {
          app_state = EXPORT_KEY;
          break;
        }
        if (symmetric_key_usage[symmetric_key_usage_select]
            & PSA_KEY_USAGE_COPY) {
          app_state = COPY_KEY;
        }
      }
      break;

    case EXPORT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Exporting a %d-bit %s %s key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_usage_string[symmetric_key_usage_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      // Clear key buffer
      memset(get_key_buf_ptr(), 0,
             symmetric_key_size[symmetric_key_size_select] / 8);
      // Export from create or import key
      if (export_key() == PSA_SUCCESS) {
        if (key_process == 0) {
          app_state = DESTROY_KEY;
        } else {
          printf("  + Comparing export key with import key... ");
          if (memcmp(get_key_buf_ptr(), key_import, get_key_len())
              == 0) {
            printf("OK\n");
            app_state = DESTROY_KEY;
          } else {
            printf("Failed\n");
          }
        }
      }
      break;

    case COPY_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Copying a %d-bit %s key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select ^ 0x01]);
      // Copy from create or import key and lifetime is storage ^= 0x01
      if ((symmetric_key_storage_select ^ 0x01) == VOLATILE_PLAIN_KEY) {
        ret = copy_key(0, DEFAULT_KEY_USAGE, SYM_KEY_ALGO);
      } else {
        ret = copy_key(PERSISTENT_COPY_KEY_ID, DEFAULT_KEY_USAGE,
                       SYM_KEY_ALGO);
      }
      if (ret == PSA_SUCCESS) {
        // Free resources and save copied key id to destroy
        reset_key_attr();
        copy_key_id = get_key_id();
        app_state = DESTROY_KEY;
        key_process += 2;
      }
      break;

    case DESTROY_KEY:
      app_state = PSA_CRYPTO_EXIT;
      set_key_id(source_key_id);        // Destroy create or import key
      printf("  + Destroying a %d-bit %s %s key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_usage_string[symmetric_key_usage_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      if (destroy_key() == PSA_SUCCESS) {
        source_key_id = 0;              // Reset source key identifier
        if (key_process == 0) {
          app_state = IMPORT_KEY;       // From key creation
        } else if (key_process == 1) {
          // Import key is a non-exportable and non-copyable key
          print_key_storage();
        } else {
          // Destroy copied key
          set_key_id(copy_key_id);
          printf("  + Destroying a %d-bit %s copied key... ",
                 symmetric_key_size[symmetric_key_size_select],
                 symmetric_key_storage_string[symmetric_key_storage_select ^ 0x01]);
          if (destroy_key() == PSA_SUCCESS) {
            copy_key_id = 0;            // Reset copied key identifier
            if (key_process == 2) {
              app_state = IMPORT_KEY;   // From key creation
            } else {
              // Import key is copyable
              print_key_storage();
            }
          }
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

  printf("\n  . Current symmetric key is a %s key.\n",
         symmetric_key_storage_string[symmetric_key_storage_select]);
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s key, press ENTER to "
         "next option.\n",
         symmetric_key_storage_string[0],
         symmetric_key_storage_string[1],
         symmetric_key_storage_string[2],
         symmetric_key_storage_string[3]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", symmetric_key_storage_string[0],
         symmetric_key_storage_string[1]);
#endif
  app_state = SELECT_KEY_STORAGE;
}
