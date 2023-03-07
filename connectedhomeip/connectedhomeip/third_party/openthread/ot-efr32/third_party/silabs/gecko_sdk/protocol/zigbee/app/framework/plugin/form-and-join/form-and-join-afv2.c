/***************************************************************************//**
 * @file
 * @brief This provides a compatibility layer between the legacy form and join code
 * and the App Framework v2 code.  The legacy form and join code is agnostic
 * of the Application Framework and thus we cannot put lots of App Framework
 * specific code in there.
 * More specifically, the sensor/sink app doesn't use the framework and thus
 * cannot compile form-and-join code with Afv2 references in it.
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
#include "app/util/common/form-and-join.h"

// ****************************************************************************
// Globals
#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginFormAndJoinCleanupEvent;
#define cleanupEvent (&emberAfPluginFormAndJoinCleanupEvent)
void emberAfPluginFormAndJoinCleanupEventHandler(SLXU_UC_EVENT);
#else
EmberEventControl emberAfPluginFormAndJoinCleanupEventControl;
#define cleanupEvent emberAfPluginFormAndJoinCleanupEventControl
#endif

// ****************************************************************************
// Forward Declarations

// ****************************************************************************

void emberAfUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel)
{
  // It is not necessary to wrap these in push/pop calls because they
  // are executing in the context of a stack callback, which already has done
  // a push.
  emberAfPluginFormAndJoinUnusedPanIdFoundCallback(panId, channel);
}

void emberJoinableNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                                      uint8_t lqi,
                                      int8_t rssi)
{
  emberAfPluginFormAndJoinNetworkFoundCallback(networkFound, lqi, rssi);
}

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#if (!defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)              \
  && !defined(SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_NETWORK_PRESENT) \
  && !defined(EZSP_HOST))
#define PERFORM_JOIN_CLEANUP
#endif
#else // !UC_BUILD
#if !defined(EMBER_AF_DISABLE_FORM_AND_JOIN_TICK) && !defined(EZSP_HOST)
#define PERFORM_JOIN_CLEANUP
#endif
#endif // UC_BUILD

void emberAfPluginFormAndJoinCleanupEventHandler(SLXU_UC_EVENT)
{
  // This takes a bit of explaining.
  // Prior to this release the form-and-join library was not a plugin and was
  // hardcoded in the project templates, thus there was no way to turn it off.
  // We needed to be able to enable/disable it and so we made it into a plugin.

  // The Network Find was another plugin layered on top of the form-and-join library.
  // Any application that used form-and-join without the network-find plugin
  // would need to properly cleanup the form-and-join code.  On the SOC this
  // was done automatically by a timer that fired after 30 seconds, assuming
  // you called emberFormAndJoinTick() regularly.  On the host, there was nno
  // timer and thus an explicit call to emberFormAndJoinCleanup() was required.

  // Now if the network-find plugin was enabled it had its own cleanup
  // (that also called emberFormAndJoinCleanup()), therefore it was
  // unnecessary to schedule an event.

  // To maintain backwards compatibility we will run the cleanup code
  // only on SOC when it is not disabled by another piece of code
  // (e.g. network-find plugin)
#ifdef PERFORM_JOIN_CLEANUP
  emberFormAndJoinCleanup(EMBER_SUCCESS);
#else
  // This is a workaround put in place to handle bug EMAPPFWKV2-1379.  There are
  // certain circumstances in which the CleanupEvent is set active while the
  // above #if conditions are not met.  This results in the eventHandler taking
  // no action, leaving the event in an active state.  The event then has no
  // means by which it can be made inactive, so the scheduler indefinitely
  // calls the empty event, which prevents the device from sleeping.
  slxu_zigbee_event_set_inactive(cleanupEvent);
#endif
}

void formAndJoinMarkBuffers(void);
void emberAfPluginFormAndJoinMarker(void)
{
#ifndef EZSP_HOST
  formAndJoinMarkBuffers();
#endif
}

void emAfPluginFormAndJoinInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  slxu_zigbee_event_init(cleanupEvent,
                         emberAfPluginFormAndJoinCleanupEventHandler);
}
