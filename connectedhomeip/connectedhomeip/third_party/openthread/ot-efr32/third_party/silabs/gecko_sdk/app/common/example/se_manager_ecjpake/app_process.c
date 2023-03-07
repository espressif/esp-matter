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
static uint8_t example_string[] = "SE Manager Key Agreement (ECJPAKE) Example";

/// Pre-shared secret ("threadjpaketest") for client and server
static const uint8_t password[] = {
  0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70,
  0x61, 0x6b, 0x65, 0x74, 0x65, 0x73, 0x74
};

/// Pre-master secret buffer copy
static uint8_t pre_master_secret_buf_copy[PRE_MASTER_SECRET_SIZE];

/// Pre-master secret length copy
static size_t pre_master_secret_len_copy;

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
        set_pre_shared_secret_ptr((uint8_t *)password);
        set_pre_shared_secret_len(sizeof(password));
        app_state = SETUP_CLIENT;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case SETUP_CLIENT:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Client setup\n");
      printf("  + Initialize an ECJPAKE client context... ");
      if (init_ecjpake_client() != SL_STATUS_OK) {
        break;
      }

      printf("  + Set up an ECJPAKE client context... ");
      if (setup_ecjpake_client() != SL_STATUS_OK) {
        break;
      }

      printf("  + Check an ECJPAKE client context... ");
      if (check_client_ctx() != SL_STATUS_OK) {
        break;
      }

      app_state = SETUP_SERVER;
      break;

    case SETUP_SERVER:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Server setup\n");
      printf("  + Initialize an ECJPAKE server context... ");
      if (init_ecjpake_server() != SL_STATUS_OK) {
        break;
      }

      printf("  + Set up an ECJPAKE server context... ");
      if (setup_ecjpake_server() != SL_STATUS_OK) {
        break;
      }

      printf("  + Check an ECJPAKE server context... ");
      if (check_server_ctx() != SL_STATUS_OK) {
        break;
      }

      app_state = WRITE_CLIENT_ROUND_ONE;
      break;

    case WRITE_CLIENT_ROUND_ONE:
      printf("\n  . Start the ECJPAKE handshake process\n");
      printf("  + Generate and write the 1st round client message... ");
      if (write_client_round_one() == SL_STATUS_OK) {
        app_state = READ_SERVER_ROUND_ONE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case READ_SERVER_ROUND_ONE:
      printf("  + Read and process the 1st round server message... ");
      if (read_server_round_one() == SL_STATUS_OK) {
        app_state = WRITE_SERVER_ROUND_ONE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WRITE_SERVER_ROUND_ONE:
      printf("  + Generate and write the 1st round server message... ");
      if (write_server_round_one() == SL_STATUS_OK) {
        app_state = READ_CLIENT_ROUND_ONE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case READ_CLIENT_ROUND_ONE:
      printf("  + Read and process the 1st round client message... ");
      if (read_client_round_one() == SL_STATUS_OK) {
        app_state = WRITE_SERVER_ROUND_TWO;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WRITE_SERVER_ROUND_TWO:
      printf("  + Generate and write the 2nd round server message... ");
      if (write_server_round_two() == SL_STATUS_OK) {
        app_state = READ_CLIENT_ROUND_TWO;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case READ_CLIENT_ROUND_TWO:
      printf("  + Read and process the 2nd round client message... ");
      if (read_client_round_two() == SL_STATUS_OK) {
        app_state = DERIVE_CLIENT_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DERIVE_CLIENT_SECRET:
      printf("  + Derive the client secret... ");
      if (derive_client_secret() == SL_STATUS_OK) {
        pre_master_secret_len_copy = get_pre_master_secret_len();
        memcpy(pre_master_secret_buf_copy, get_pre_master_secret_buf_ptr(),
               pre_master_secret_len_copy);
        app_state = WRITE_CLIENT_ROUND_TWO;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WRITE_CLIENT_ROUND_TWO:
      printf("  + Generate and write the 2nd round client message... ");
      if (write_client_round_two() == SL_STATUS_OK) {
        app_state = READ_SERVER_ROUND_TWO;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case READ_SERVER_ROUND_TWO:
      printf("  + Read and process the 2nd round server message... ");
      if (read_server_round_two() == SL_STATUS_OK) {
        app_state = DERIVE_SERVER_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DERIVE_SERVER_SECRET:
      printf("  + Derive the server secret... ");
      if (derive_server_secret() == SL_STATUS_OK) {
        app_state = COMPARE_SHARED_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case COMPARE_SHARED_SECRET:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Complete the ECJPAKE handshake process\n");
      printf("  + Clear an ECJPAKE client context... ");
      if (clear_client_ctx() != SL_STATUS_OK) {
        break;
      }

      printf("  + Clear an ECJPAKE server context... ");
      if (clear_server_ctx() != SL_STATUS_OK) {
        break;
      }

      printf("  + Compare client and server derived secrets... ");
      if (get_pre_master_secret_len() != pre_master_secret_len_copy) {
        printf("Failed\n");
        break;
      }
      if (memcmp(pre_master_secret_buf_copy, get_pre_master_secret_buf_ptr(),
                 pre_master_secret_len_copy) != 0) {
        printf("Failed\n");
      } else {
        printf("OK\n");
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
