/***************************************************************************//**
 * @file app_process.c
 * @brief Top level application functions.
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

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Attestation Example";

/// Space press status
static bool space_press;

/// Enter press status
static bool enter_press;

/// Used when selecting a valid nonce size for the IAT token
static int8_t selected_nonce = 2;

static const uint32_t iat_token_nonce_sizes[] = {
  SL_SE_ATTESTATION_CHALLENGE_SIZE_32,
  SL_SE_ATTESTATION_CHALLENGE_SIZE_48,
  SL_SE_ATTESTATION_CHALLENGE_SIZE_64
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely.
 *****************************************************************************/
void app_process_action(void)
{
  sl_status_t ret;
  uint8_t token_buf[ATTESTED_TOKEN_MAX_SIZE] = { 0 }; // Token buffer
  size_t token_size = 0; // Size of actual token in bytes

  // Retrieve input character from VCOM port
  app_iostream_usart_process_action();

  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("Initializing SE Manager...\n");

      if (init_se_manager() != SL_STATUS_OK) {
        printf("[ERROR] Failed to initialize SE Manager.\n");
        app_state = SE_MANAGER_EXIT;
        break;
      }

      printf("\nSelect nonce size for the IAT token (32, 48 or 64 bytes).\n");
      printf("Press SPACE to cycle through the options.\n");
      printf("Press ENTER to make a selection.\n");
      app_state = SELECT_NONCE_SIZE;
      break;

    case SELECT_NONCE_SIZE:
      if (space_press) {
        space_press = false;
        selected_nonce = (selected_nonce + 1) % 3;
        printf("    Current nonce size:  %" PRIu32 "\n",
               iat_token_nonce_sizes[selected_nonce]);
      }

      if (enter_press) {
        enter_press = false;
        printf("    Selected nonce size: %" PRIu32 "\n",
               iat_token_nonce_sizes[selected_nonce]);
        app_state = GET_IAT_TOKEN;
      }
      break;

    case GET_IAT_TOKEN:
      ret = get_attested_token(IAT_TOKEN,
                               iat_token_nonce_sizes[selected_nonce],
                               token_buf,
                               sizeof(token_buf),
                               &token_size);

      if (ret == SL_STATUS_OK) {
        printf("\nPSA IAT token\n");
        printf("=============\n");
        print_attestation_token(token_buf, token_size);
      } else {
        printf("[ERROR] Failed to get PSA IAT token.\n");
        sl_status_print(ret);
      }

      app_state = GET_CONFIG_TOKEN;
      break;

    case GET_CONFIG_TOKEN:
      /// Nonce/challenge size must be 32 for config tokens.
      ret = get_attested_token(CONFIG_TOKEN,
                               SL_SE_ATTESTATION_CHALLENGE_SIZE_32,
                               token_buf,
                               sizeof(token_buf),
                               &token_size);

      if (ret == SL_STATUS_OK) {
        printf("\nConfig token\n");
        printf("============\n");
        print_attestation_token(token_buf, token_size);
      } else {
        printf("[ERROR] Failed to get config token.\n");
        sl_status_print(ret);
      }

      app_state = SE_MANAGER_EXIT;
      break;

    case SE_MANAGER_EXIT:
      printf("\n\nExiting...\n");
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
