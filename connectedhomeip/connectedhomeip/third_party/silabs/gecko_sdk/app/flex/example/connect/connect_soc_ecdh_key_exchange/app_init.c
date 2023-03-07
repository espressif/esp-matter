/***************************************************************************//**
 * @file
 * @brief app_init.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdio.h>
#include "app_framework_common.h"
#include "app_log.h"
#include "app_process.h"

#include "sl_connect_ecdh_key_exchange.h"
// Ensure that psa is initialized corretly
#include "psa/crypto.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern EmberKeyData connect_network_key;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
void emberAfInitCallback(void)
{
  // Ensure that psa is initialized corretly
  psa_crypto_init();
  // set the default PAN ID, it can be changed with CLI
  sl_connect_ecdh_key_exchange_set_pan_id(SL_CONNECT_ECDH_KEY_EXCHANGE_DEFAULT_PAN_ID);
  // set the default communication channel, it can be changed with CLI
  emberSetRadioChannelExtended(emberGetDefaultChannel(), false);
  // set the default Tx power, it can be changed with CLI
  emberSetRadioPower(SL_CONNECT_ECDH_KEY_EXCHANGE_DEFAULT_TX_POWER, false);

  psa_status_t psa_status;
  if ((psa_status = sl_connect_ecdh_key_exchange_init()) != PSA_SUCCESS) {
    app_log_info("PSA init failed (status: %ld)\n", psa_status);
  } else {
    app_log_error("PSA init succeed\n");
  }

  psa_status = psa_generate_random(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: generate random network key succeed\n");
    app_log_info("PSA: random network key length: %u\n", EMBER_ENCRYPTION_KEY_SIZE);
    app_log_info("PSA: random network key: ");
    app_log_hexdump_info(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
    app_log_append_info("\n");
    if (set_security_key(connect_network_key.contents, (size_t)EMBER_ENCRYPTION_KEY_SIZE) == true) {
      app_log_info("Connect: set the random key as network key succeed\n");
    } else {
      app_log_error("Connect: set the random key as network key failed\n");
    }
  } else {
    app_log_error("PSA: generate random network key failed (status: %ld)\n", psa_status);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
