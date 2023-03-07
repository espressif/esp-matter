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
#include "app_log.h"
#include "sl_app_common.h"
#include "sl_si70xx.h"
#include "sl_i2cspm_instances.h"
#include "sl_sleeptimer.h"
#include "app_process.h"
#include "app_init.h"
#include "app_framework_common.h"
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
extern EmberKeyData security_key;
/// Connect security key id
extern psa_key_id_t security_key_id;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
* Application framework init callback
******************************************************************************/
void emberAfInitCallback(void)
{
  EmberStatus status;
  uint8_t device_id = 0;

  // Ensure that psa is initialized corretly
  psa_crypto_init();

  // init temperature sensor
  if (!sl_si70xx_present(sl_i2cspm_sensor, SI7021_ADDR, &device_id)) {
    // wait a bit before re-trying
    // the si7021 sensor can take up to 80 ms (25 ms @25 deg C) to start up
    sl_sleeptimer_delay_millisecond(80);
    // init temperature sensor (2nd attempt)
    if (!sl_si70xx_present(sl_i2cspm_sensor, SI7021_ADDR, &device_id)) {
      app_log_error("Failed to initialize temperature sensor!\n");
    }
  }

  emberAfAllocateEvent(&report_control, &report_handler);
  // CLI info message
  app_log_info("\nSensor\n");

  if (security_key_id == 0) {
    // Initialize the security key to the default key prior to commissioning the node.
    set_security_key(security_key.contents, (size_t)EMBER_ENCRYPTION_KEY_SIZE);
  }

  status = emberNetworkInit();
  app_log_info("Network status 0x%02X\n", status);

#if defined(EMBER_AF_PLUGIN_BLE)
  bleConnectionInfoTableInit();
#endif
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
