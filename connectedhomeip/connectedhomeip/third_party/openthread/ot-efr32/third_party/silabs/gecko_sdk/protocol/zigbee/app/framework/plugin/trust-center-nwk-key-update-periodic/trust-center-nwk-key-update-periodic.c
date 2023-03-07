/***************************************************************************//**
 * @file
 * @brief Functionality for periodically updating the NWK key.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"

// *****************************************************************************
// Globals
#ifdef UC_BUILD

#include "sl_component_catalog.h"
#include "trust-center-nwk-key-update-periodic-config.h"
sl_zigbee_event_t emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEvent;
#define myEvent (&emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEvent)
void emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler(SLXU_UC_EVENT);

#else // !UC_BUILD

#ifdef EMBER_AF_PLUGIN_TEST_HARNESS
#define SL_CATALOG_ZIGBEE_TEST_HARNESS_PRESENT
#endif

EmberEventControl emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventControl;
#define myEvent emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventControl

// App builder settings
#define KEY_UPDATE_PERIOD EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_KEY_UPDATE_PERIOD
#define KEY_UPDATE_UNITS EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_KEY_UPDATE_UNITS

#define DAYS    0
#define MINUTES 1
#define HOURS   2

// All numbers in minutes
#if KEY_UPDATE_UNITS == DAYS
  #define KEY_UPDATE_MULTIPLIER (24 * 60)
  #define KEY_UPDATE_UNITS_STRING "days"
#elif KEY_UPDATE_UNITS == HOURS
  #define KEY_UPDATE_MULTIPLIER (60)
  #define KEY_UPDATE_UNITS_STRING "hours"
#elif KEY_UPDATE_UNITS == MINUTES
  #define KEY_UPDATE_MULTIPLIER (1)
  #define KEY_UPDATE_UNITS_STRING "minutes"
#else
  #error "Error: Unknown value for EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_KEY_UPDATE_UNITS"
#endif

#define KEY_UPDATE_PERIOD_MINUTES (KEY_UPDATE_PERIOD * KEY_UPDATE_MULTIPLIER)
#endif // UC_BUILD

// All delays are converted into milliseconds. Ensure that the converted millisecond value
// is under EMBER_MAX_EVENT_DELAY_MS
#if KEY_UPDATE_PERIOD_MINUTES > (EMBER_MAX_EVENT_DELAY_MS / 60000)
  #error "Error: Configured key update period is too large"
#endif

// *****************************************************************************

extern EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);

static void scheduleNextUpdate(void)
{
  slxu_zigbee_event_set_delay_minutes(myEvent, KEY_UPDATE_PERIOD_MINUTES);
  emberAfAppPrintln("Next NWK key update in %d minutes",
                    KEY_UPDATE_PERIOD_MINUTES);
}

void emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler(SLXU_UC_EVENT)
{
  // A very simple approach is to always kick off the NWK key update
  // assuming it will take much longer to complete than the period
  // at which we initiate.  If something went wrong and the NWK key update
  // didn't complete, a subsequent key update attempt may help recover this and
  // finish the previous one.  This also handles the reboot case where
  // the TC may have rebooted while in the middle of the key update.

  EmberStatus status = emberAfTrustCenterStartNetworkKeyUpdate();
  emberAfAppPrintln("Starting NWK Key update, status: 0x%X", status);
  scheduleNextUpdate();
}

#ifdef UC_BUILD

void emberAfPluginTrustCenterNwkKeyUpdatePeriodicInitCallback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(myEvent,
                             emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_DONE:
    {
      // Even though we might not be on a network at this point,
      // we want to schedule the process for sending key updates from
      // now on.

      // However for the test-harness, we manually manipulate the process
      // so we don't want it to automatically kick off.
#if !defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_PRESENT)
      scheduleNextUpdate();
#endif
    }
  }
}

#else // !UC_BUILD

void emberAfPluginTrustCenterNwkKeyUpdatePeriodicInitCallback(void)
{
  // Even though we might not be on a network at this point,
  // we want to schedule the process for sending key updates from
  // now on.

  // However for the test-harness, we manually manipulate the process
  // so we don't want it to automatically kick off.
#if !defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_PRESENT)
  scheduleNextUpdate();
#endif
}

#endif // UC_BUILD
