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

/***************************************************************************//**
 * Print asymmetric key algorithm option.
 ******************************************************************************/
static void print_key_algo_option(void);

/***************************************************************************//**
 * Print selected asymmetric key algorithm for ECDH client.
 ******************************************************************************/
static void print_key_algo_client(void);

/***************************************************************************//**
 * Print selected asymmetric key algorithm for ECDH server.
 ******************************************************************************/
static void print_key_algo_server(void);

/***************************************************************************//**
 * Print ECDH shared secret.
 *
 * @param client True for client, false for server.
 ******************************************************************************/
static void print_shared_secret(bool client);

/***************************************************************************//**
 * Compare shared secret from client and server.
 ******************************************************************************/
static void compare_shared_secret(void);

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
static uint8_t example_string[] = "SE Manager Key Agreement (ECDH) Example";

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Asymmetric key type (plain or wrap or volatile)
static uint8_t asymmetric_key_type_select;

static const char *asymmetric_key_type_string[] = {
  "plaintext",
  "wrapped",
  "volatile"
};
#endif

/// Asymmetric key algorithm selection
static uint8_t asymmetric_key_algo_select;

static const char *asymmetric_key_algo_string[] = {
  "ECC Weierstrass Prime",
  "ECC Montgomery"
};

/// ECC Montgomery key selection for ECDH
static uint8_t ecc_montgomery_key_select;

static const char *ecc_montgomery_key_string[] = {
  "ECC X25519",
  "ECC X448"
};

static const sl_se_key_type_t ecc_montgomery_key[] = {
  SL_SE_KEY_TYPE_ECC_X25519,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SL_SE_KEY_TYPE_ECC_X448
#endif
};

/// ECC Weierstrass Prime key selection
static uint8_t ecc_weierstrass_prime_key_select;

static const char *ecc_weierstrass_prime_key_string[] = {
  "ECC P192",
  "ECC P256",
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  "ECC P384",
  "ECC P521",
  "ECC Custom (secp256k1 in this example)"
#endif
};

static const sl_se_key_type_t ecc_weierstrass_prime_key[] = {
  SL_SE_KEY_TYPE_ECC_P192,
  SL_SE_KEY_TYPE_ECC_P256,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SL_SE_KEY_TYPE_ECC_P384,
  SL_SE_KEY_TYPE_ECC_P521,
  SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM
#endif
};

/// Selected key to run the example
static sl_se_key_type_t selected_key;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
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

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SELECT_KEY_TYPE:
      if (space_press) {
        space_press = false;
        asymmetric_key_type_select++;
        if (asymmetric_key_type_select == 3) {
          asymmetric_key_type_select = 0;
        }
        printf("  + Current asymmetric key is a %s key.\n",
               asymmetric_key_type_string[asymmetric_key_type_select]);
      }
      if (enter_press) {
        enter_press = false;
        printf("\n  . Current asymmetric key algorithm is %s.\n",
               asymmetric_key_algo_string[asymmetric_key_algo_select]);
        printf("  + Press SPACE to select asymmetric key algorithm (%s/%s), press"
               " ENTER to next option.\n",
               asymmetric_key_algo_string[0],
               asymmetric_key_algo_string[1]);
        app_state = SELECT_KEY_ALGO;
      }
      break;
#endif

    case SELECT_KEY_ALGO:
      if (space_press) {
        space_press = false;
        asymmetric_key_algo_select++;
        if (asymmetric_key_algo_select == 2) {
          asymmetric_key_algo_select = 0;
        }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Current asymmetric key algorithm is %s.\n",
               asymmetric_key_algo_string[asymmetric_key_algo_select]);
#else
        if (asymmetric_key_algo_select == 0) {
          printf("  + Current asymmetric key algorithm is %s.\n",
                 asymmetric_key_algo_string[asymmetric_key_algo_select]);
        } else {
          printf("  + Current asymmetric key algorithm is %s.\n",
                 ecc_montgomery_key_string[0]);
        }
#endif
      }
      if (enter_press) {
        enter_press = false;
        print_key_algo_option();
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SELECT_MONTGOMERY_KEY:
      if (space_press) {
        space_press = false;
        ecc_montgomery_key_select++;
        if (ecc_montgomery_key_select == 2) {
          ecc_montgomery_key_select = 0;
        }
        printf("  + Current ECC Montgomery key is %s.\n",
               ecc_montgomery_key_string[ecc_montgomery_key_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key = ecc_montgomery_key[ecc_montgomery_key_select];
        print_key_algo_client();
      }
      break;
#endif

    case SELECT_WEIERSTRASS_KEY:
      if (space_press) {
        space_press = false;
        ecc_weierstrass_prime_key_select++;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        if (ecc_weierstrass_prime_key_select == 5) {
#else
        if (ecc_weierstrass_prime_key_select == 2) {
#endif
          ecc_weierstrass_prime_key_select = 0;
        }
        printf("  + Current ECC Weierstrass Prime key is %s.\n",
               ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key = ecc_weierstrass_prime_key[ecc_weierstrass_prime_key_select];
        print_key_algo_client();
      }
      break;

    case CREATE_CLIENT_PLAIN_KEY:
      printf("  + Generate a client plain asymmetric key... ");
      if (create_plain_asymmetric_key(selected_key, true) == SL_STATUS_OK) {
        app_state = EXPORT_CLIENT_PLAIN_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_CLIENT_PLAIN_PUBLIC_KEY:
      printf("  + Export a client public key from plain asymmetric key... ");
      if (export_asymmetric_pubkey_from_plain(selected_key, true)
          == SL_STATUS_OK) {
        print_key_algo_server();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_SERVER_PLAIN_KEY:
      printf("  + Generate a server plain asymmetric key... ");
      if (create_plain_asymmetric_key(selected_key, false) == SL_STATUS_OK) {
        app_state = EXPORT_SERVER_PLAIN_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_SERVER_PLAIN_PUBLIC_KEY:
      printf("  + Export a server public key from plain asymmetric key... ");
      if (export_asymmetric_pubkey_from_plain(selected_key, false)
          == SL_STATUS_OK) {
        app_state = COMPUTE_CLIENT_SHARED_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case CREATE_CLIENT_WRAP_KEY:
      printf("  + Generate a client non-exportable wrapped asymmetric key... ");
      if (create_wrap_asymmetric_key(selected_key, true) == SL_STATUS_OK) {
        app_state = EXPORT_CLIENT_WRAP_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_CLIENT_WRAP_PUBLIC_KEY:
      printf("  + Export a client public key from wrapped asymmetric key... ");
      if (export_asymmetric_pubkey_from_wrap(selected_key, true)
          == SL_STATUS_OK) {
        print_key_algo_server();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_CLIENT_VOLATILE_KEY:
      printf("  + Generate a client non-exportable asymmetric key into a "
             "volatile SE key slot... ");
      if (create_volatile_asymmetric_key(selected_key, true) == SL_STATUS_OK) {
        app_state = EXPORT_CLIENT_VOLATILE_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_CLIENT_VOLATILE_PUBLIC_KEY:
      printf("  + Export a client public key from a volatile SE key slot... ");
      if (export_asymmetric_pubkey_from_volatile(selected_key, true)
          == SL_STATUS_OK) {
        print_key_algo_server();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DELETE_CLIENT_VOLATILE_KEY:
      printf("  + Delete a client non-exportable asymmetric key in a volatile "
             "SE key slot... ");
      if (delete_volatile_asymmetric_key(selected_key, true) == SL_STATUS_OK) {
        app_state = DELETE_SERVER_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_SERVER_WRAP_KEY:
      printf("  + Generate a server non-exportable wrapped asymmetric key... ");
      if (create_wrap_asymmetric_key(selected_key, false) == SL_STATUS_OK) {
        app_state = EXPORT_SERVER_WRAP_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_SERVER_WRAP_PUBLIC_KEY:
      printf("  + Export a server public key from wrapped asymmetric key... ");
      if (export_asymmetric_pubkey_from_wrap(selected_key, false)
          == SL_STATUS_OK) {
        app_state = COMPUTE_CLIENT_SHARED_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_SERVER_VOLATILE_KEY:
      printf("  + Generate a server non-exportable asymmetric key into a "
             "volatile SE key slot... ");
      if (create_volatile_asymmetric_key(selected_key, false) == SL_STATUS_OK) {
        app_state = EXPORT_SERVER_VOLATILE_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_SERVER_VOLATILE_PUBLIC_KEY:
      printf("  + Export a server public key from a volatile SE key slot... ");
      if (export_asymmetric_pubkey_from_volatile(selected_key, false)
          == SL_STATUS_OK) {
        app_state = COMPUTE_CLIENT_SHARED_SECRET;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DELETE_SERVER_VOLATILE_KEY:
      printf("  + Delete a server non-exportable asymmetric key in a volatile "
             "SE key slot... ");
      if (delete_volatile_asymmetric_key(selected_key, false) == SL_STATUS_OK) {
        print_startup();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

    case COMPUTE_CLIENT_SHARED_SECRET:
      print_shared_secret(true);
      break;

    case COMPUTE_SERVER_SHARED_SECRET:
      print_shared_secret(false);
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
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("\n  . Current asymmetric key is a %s key.\n",
         asymmetric_key_type_string[asymmetric_key_type_select]);
  printf("  + Press SPACE to select a %s or %s or %s key, press ENTER to next "
         "option.\n",
         asymmetric_key_type_string[0],
         asymmetric_key_type_string[1],
         asymmetric_key_type_string[2]);
  app_state = SELECT_KEY_TYPE;
#else
  printf("\n  . Current asymmetric key algorithm is %s.\n",
         asymmetric_key_algo_string[asymmetric_key_algo_select]);
  printf("  + Press SPACE to select asymmetric key algorithm (%s/%s), press"
         " ENTER to next option or run if %s is selected.\n",
         asymmetric_key_algo_string[0],
         ecc_montgomery_key_string[0],
         ecc_montgomery_key_string[0]);
  app_state = SELECT_KEY_ALGO;
#endif
}

/***************************************************************************//**
 * Print asymmetric key algorithm option.
 ******************************************************************************/
static void print_key_algo_option(void)
{
  if (asymmetric_key_algo_select == 0) {
    printf("\n  . Current ECC Weierstrass Prime key is %s.\n",
           ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    printf("  + Press SPACE to select ECC Weierstrass Prime key (%s/%s/%s/%s/%s),"
           " press ENTER to run.\n",
           ecc_weierstrass_prime_key_string[0],
           ecc_weierstrass_prime_key_string[1],
           ecc_weierstrass_prime_key_string[2],
           ecc_weierstrass_prime_key_string[3],
           ecc_weierstrass_prime_key_string[4]);
#else
    printf("  + Press SPACE to select ECC Weierstrass Prime key (%s/%s),"
           " press ENTER to run.\n",
           ecc_weierstrass_prime_key_string[0],
           ecc_weierstrass_prime_key_string[1]);
#endif
    app_state = SELECT_WEIERSTRASS_KEY;
  } else {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    printf("\n  . Current ECC Montgomery key is %s.\n",
           ecc_montgomery_key_string[ecc_montgomery_key_select]);
    printf("  + Press SPACE to select ECC Montgomery key (%s/%s), press ENTER to "
           "run.\n",
           ecc_montgomery_key_string[0],
           ecc_montgomery_key_string[1]);
    app_state = SELECT_MONTGOMERY_KEY;
#else
    selected_key = ecc_montgomery_key[0];
    print_key_algo_client();
#endif
  }
}

/***************************************************************************//**
 * Print selected asymmetric key algorithm for ECDH client.
 ******************************************************************************/
static void print_key_algo_client(void)
{
  printf("\n  . ECDH Client");
  printf("\n  + %s",
         asymmetric_key_algo_string[asymmetric_key_algo_select]);
  if (asymmetric_key_algo_select == 0) {
    printf(" - %s \n",
           ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  } else {
    printf(" - %s \n", ecc_montgomery_key_string[ecc_montgomery_key_select]);
  }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (asymmetric_key_type_select == 0) {
    app_state = CREATE_CLIENT_PLAIN_KEY;
  } else if (asymmetric_key_type_select == 1) {
    app_state = CREATE_CLIENT_WRAP_KEY;
  } else {
    app_state = CREATE_CLIENT_VOLATILE_KEY;
  }
#else
  app_state = CREATE_CLIENT_PLAIN_KEY;
#endif
}

/***************************************************************************//**
 * Print selected asymmetric key algorithm for ECDH server.
 ******************************************************************************/
static void print_key_algo_server(void)
{
  printf("\n  . ECDH Server");
  printf("\n  + %s",
         asymmetric_key_algo_string[asymmetric_key_algo_select]);
  if (asymmetric_key_algo_select == 0) {
    printf(" - %s \n",
           ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  } else {
    printf(" - %s \n", ecc_montgomery_key_string[ecc_montgomery_key_select]);
  }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (asymmetric_key_type_select == 0) {
    app_state = CREATE_SERVER_PLAIN_KEY;
  } else if (asymmetric_key_type_select == 1) {
    app_state = CREATE_SERVER_WRAP_KEY;
  } else {
    app_state = CREATE_SERVER_VOLATILE_KEY;
  }
#else
  app_state = CREATE_SERVER_PLAIN_KEY;
#endif
}

/***************************************************************************//**
 * Print ECDH shared secret.
 ******************************************************************************/
static void print_shared_secret(bool client)
{
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  sl_status_t ret;

  if (client) {
    memset(get_client_shared_secret_buf_ptr(), 0, SHARED_SECRET_SIZE);
    memset(get_server_shared_secret_buf_ptr(), 0, SHARED_SECRET_SIZE);
    printf("\n  . Compute ECDH shared secret");
    printf("\n  + %s",
           asymmetric_key_algo_string[asymmetric_key_algo_select]);
    if (asymmetric_key_algo_select == 0) {
      printf(" - %s \n",
             ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
    } else {
      printf(" - %s \n", ecc_montgomery_key_string[ecc_montgomery_key_select]);
    }
    printf("  + Compute client shared secret with %s key... ",
           asymmetric_key_type_string[asymmetric_key_type_select]);
  } else {
    printf("  + Compute server shared secret with %s key... ",
           asymmetric_key_type_string[asymmetric_key_type_select]);
  }

  if (asymmetric_key_type_select == 0) {
    ret = compute_plain_shared_secret(selected_key, client);
  } else if (asymmetric_key_type_select == 1) {
    ret = compute_wrap_shared_secret(selected_key, client);
  } else {
    ret = compute_volatile_shared_secret(selected_key, client);
  }

  if (ret == SL_STATUS_OK) {
    if (client) {
      app_state = COMPUTE_SERVER_SHARED_SECRET;
    } else {
      compare_shared_secret();
    }
  } else {
    app_state = SE_MANAGER_EXIT;
  }
#else
  if (client) {
    printf("\n  . Compute ECDH shared secret");
    printf("\n  + %s",
           asymmetric_key_algo_string[asymmetric_key_algo_select]);
    if (asymmetric_key_algo_select == 0) {
      printf(" - %s \n",
             ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
    } else {
      printf(" - %s \n", ecc_montgomery_key_string[ecc_montgomery_key_select]);
    }
    printf("  + Compute client shared secret with plaintext key... ");
    if (compute_plain_shared_secret(selected_key, client) == SL_STATUS_OK) {
      app_state = COMPUTE_SERVER_SHARED_SECRET;
    } else {
      app_state = SE_MANAGER_EXIT;
    }
  } else {
    printf("  + Compute server shared secret with plaintext key... ");
    if (compute_plain_shared_secret(selected_key, client) == SL_STATUS_OK) {
      compare_shared_secret();
    } else {
      app_state = SE_MANAGER_EXIT;
    }
  }
#endif
}

/***************************************************************************//**
 * Compare shared secret from client and server.
 ******************************************************************************/
static void compare_shared_secret(void)
{
  printf("  + Compare shared secret from client and server... ");
  if (memcmp(get_client_shared_secret_buf_ptr(),
             get_server_shared_secret_buf_ptr(),
             get_shared_secret_length(selected_key)) == 0) {
    printf("OK\n");
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (asymmetric_key_type_select == 2) {
      app_state = DELETE_CLIENT_VOLATILE_KEY;
    } else {
      print_startup();
    }
#else
    print_startup();
#endif
  } else {
    printf("Failed\n");
    app_state = SE_MANAGER_EXIT;
  }
}
