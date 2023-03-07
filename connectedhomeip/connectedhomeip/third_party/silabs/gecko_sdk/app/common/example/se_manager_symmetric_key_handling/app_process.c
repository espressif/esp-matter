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
 * Print startup selection.
 ******************************************************************************/
static void print_startup(void);

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
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Symmetric Key Handling Example";

/// Symmetric key size selection
static uint8_t symmetric_key_size_select;

static const char *symmetric_key_size_string[] = {
  "Custom (28 bytes in this example)",
  "AES-128",
  "AES-192",
  "AES-256"
};

static const sl_se_key_type_t symmetric_key_size[] = {
  SL_SE_KEY_TYPE_SYMMETRIC,
  SL_SE_KEY_TYPE_AES_128,
  SL_SE_KEY_TYPE_AES_192,
  SL_SE_KEY_TYPE_AES_256
};

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Buffer for symmetric key verification
static uint8_t copy_buf[KEY_BUF_SIZE];
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*****************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  // Retrieve input character from VCOM port
  app_iostream_usart_process_action();

  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        print_startup();
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case SELECT_KEY_LENGTH:
      if (space_press) {
        space_press = false;
        symmetric_key_size_select++;
        if (symmetric_key_size_select == 4) {
          symmetric_key_size_select = 0;
        }
        printf("  + Current symmetric key length is %s.\n",
               symmetric_key_size_string[symmetric_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        app_state = CREATE_PLAIN_KEY;
      }
      break;

    case CREATE_PLAIN_KEY:
      printf("\n  . Plain symmetric key\n");
      printf("  + Generate a plain symmetric key... ");
      if (create_plain_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        app_state = IMPORT_PLAIN_KEY;
#else
        print_startup();
#endif
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case IMPORT_PLAIN_KEY:
      printf("\n  . Import and export symmetric key\n");
      printf("  + Import a plain symmetric key from buffer into an exportable "
             "wrapped key... ");
      memcpy(copy_buf, get_symmetric_key_buf_ptr(), KEY_BUF_SIZE);
      if (import_plain_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = EXPORT_WRAP_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_WRAP_KEY:
      printf("  + Export a wrapped symmetric key into a plain key buffer... ");
      if (export_wrap_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = COMPARE_EXPORT_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case COMPARE_EXPORT_KEY:
      printf("  + Compare export of symmetric key with import one... ");
      int16_t temp;
      if (symmetric_key_size[symmetric_key_size_select] == 0) {
        temp = memcmp(copy_buf, get_symmetric_key_buf_ptr(), CUSTOM_KEY_SIZE);
      } else {
        temp = memcmp(copy_buf, get_symmetric_key_buf_ptr(),
                      symmetric_key_size[symmetric_key_size_select]);
      }
      if (temp == 0) {
        printf("OK\n");
        app_state = CREATE_WRAP_KEY;
      } else {
        printf("Failed\n");
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_WRAP_KEY:
      printf("\n  . Wrapped symmetric key\n");
      printf("  + Generate a non-exportable wrapped symmetric key... ");
      if (create_wrap_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = CREATE_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_VOLATILE_KEY:
      printf("\n  . Volatile symmetric key\n");
      printf("  + Generate a non-exportable symmetric key into a volatile SE "
             "key slot... ");
      if (create_volatile_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = DELETE_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DELETE_VOLATILE_KEY:
      printf("  + Delete a non-exportable symmetric key in a volatile SE key"
             " slot... ");
      if (delete_volatile_symmetric_key(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = TRANSFER_WRAP_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case TRANSFER_WRAP_KEY:
      printf("\n  . Transfer symmetric key\n");
      printf("  + Transfer a non-exportable wrapped symmetric key into a "
             "volatile SE key slot... ");
      if (transfer_wrap_symmetric_key_to_volatile(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        app_state = TRANSFER_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case TRANSFER_VOLATILE_KEY:
      printf("  + Transfer a non-exportable symmetric key in a volatile SE key"
             " slot into a wrapped key buffer... ");
      if (transfer_volatile_symmetric_key_to_wrap(symmetric_key_size[symmetric_key_size_select])
          == SL_STATUS_OK) {
        printf("  + Delete a non-exportable symmetric key in a volatile SE key"
               " slot... ");
        if (delete_volatile_symmetric_key(symmetric_key_size[symmetric_key_size_select])
            == SL_STATUS_OK) {
          print_startup();
        } else {
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

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
 * Print startup selection.
 ******************************************************************************/
static void print_startup(void)
{
  printf("\n  . Symmetric key handling");
  printf("\n  + Current symmetric key length is %s.\n",
         symmetric_key_size_string[symmetric_key_size_select]);
  printf("  + Press SPACE to select symmetric key length (%s/%s/%s/%s), press"
         " ENTER to run.\n",
         symmetric_key_size_string[0],
         symmetric_key_size_string[1],
         symmetric_key_size_string[2],
         symmetric_key_size_string[3]);
  app_state = SELECT_KEY_LENGTH;
}
