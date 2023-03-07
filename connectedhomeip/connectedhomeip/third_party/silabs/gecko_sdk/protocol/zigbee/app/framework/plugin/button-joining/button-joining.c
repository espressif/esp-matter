/***************************************************************************//**
 * @file
 * @brief Routines for forming/joining using the hardware buttons.
 *   button 0: if not joined: FORM if the device is capable of forming
 *             (a.k.a. a coordinator).  Otherwise form a network.
 *             if joined: broadcast ZDO MGMT Permit Join in network.
 *             Hold for 5 seconds and release: leave network
 *   button 1: Unused (Callback executed)
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
#include "button-joining.h"

//------------------------------------------------------------------------------
// Forward Declaration

#ifdef UC_BUILD
#include "button-joining-config.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_NETWORK_CREATOR_SECURITY_PRESENT
#include "network-creator-security.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_NETWORK_CREATOR_PRESENT
#include "network-creator.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_NETWORK_STEERING_PRESENT
#include "network-steering.h"
#endif
sl_zigbee_event_t emberAfPluginButtonJoiningButton0Event;
#define buttonEvent0 (&emberAfPluginButtonJoiningButton0Event)
void emberAfPluginButtonJoiningButton0EventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginButtonJoiningButton1Event;
#define buttonEvent1 (&emberAfPluginButtonJoiningButton1Event)
void emberAfPluginButtonJoiningButton1EventHandler(SLXU_UC_EVENT);
#define BUTTON0         0
#define BUTTON1         1
#else // !UC_BUILD
#include "app/framework/util/af-main.h"
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY
#include "app/framework/plugin/network-creator-security/network-creator-security.h"
#define SL_CATALOG_ZIGBEE_NETWORK_CREATOR_SECURITY_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR
#include "app/framework/plugin/network-creator/network-creator.h"
#define SL_CATALOG_ZIGBEE_NETWORK_CREATOR_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_STEERING
#include "app/framework/plugin/network-steering/network-steering.h"
#define SL_CATALOG_ZIGBEE_NETWORK_STEERING_PRESENT
#endif
EmberEventControl emberAfPluginButtonJoiningButton0EventControl;
EmberEventControl emberAfPluginButtonJoiningButton1EventControl;
#define buttonEvent0 emberAfPluginButtonJoiningButton0EventControl
#define buttonEvent1 emberAfPluginButtonJoiningButton1EventControl
#endif // UC_BUILD

static bool buttonPress(uint8_t button, uint8_t state);
static uint32_t  buttonPressDurationMs = 0;

#define BUTTON_HOLD_DURATION_MS 5000

//------------------------------------------------------------------------------
// Globals

#define PERMIT_JOIN_TIMEOUT EMBER_AF_PLUGIN_BUTTON_JOINING_PERMIT_JOIN_TIMEOUT

//------------------------------------------------------------------------------

// Event init only.
void emAfPluginButtonJoiningInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  slxu_zigbee_event_init(&emberAfPluginButtonJoiningButton0Event,
                         emberAfPluginButtonJoiningButton0EventHandler);
  slxu_zigbee_event_init(&emberAfPluginButtonJoiningButton1Event,
                         emberAfPluginButtonJoiningButton1EventHandler);
}

void emberAfPluginButtonJoiningButton0EventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(buttonEvent0);

  if (buttonPressDurationMs >= BUTTON_HOLD_DURATION_MS) {
    emberAfCorePrintln("Leaving network due to button press.");
    emberLeaveNetwork();
    return;
  }

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
#ifdef SL_CATALOG_ZIGBEE_NETWORK_CREATOR_SECURITY_PRESENT
    // The Network Creator Security plugin Open process is more comprehensive
    // and also takes care of broadcasting pjoin
    emberAfPluginNetworkCreatorSecurityOpenNetwork();
#else
    emberAfBroadcastPermitJoin(PERMIT_JOIN_TIMEOUT);
#endif // SL_CATALOG_ZIGBEE_NETWORK_CREATOR_SECURITY_PRESENT
  } else if (emberAfNetworkState() == EMBER_NO_NETWORK) {
#ifdef EMBER_AF_HAS_COORDINATOR_NETWORK
    emberAfCorePrintln("%p: nwk down: do form", "button0");
    // Use Z3.0 network formation if we have the plugin for it; else, fall back
    // to legacy / custom formation methods
#ifdef SL_CATALOG_ZIGBEE_NETWORK_CREATOR_PRESENT
    emberAfPluginNetworkCreatorStart(true);
#else // !SL_CATALOG_ZIGBEE_NETWORK_CREATOR_PRESENT
    emberAfFindUnusedPanIdAndForm();
#endif // SL_CATALOG_ZIGBEE_NETWORK_CREATOR_PRESENT
#else // !EMBER_AF_HAS_COORDINATOR_NETWORK
    // Use Z3.0 network steering if we have the plugin for it; else, fall back
    // to legacy / custom joining method
    emberAfCorePrintln("%p: nwk down: do join", "button0");
#ifdef SL_CATALOG_ZIGBEE_NETWORK_STEERING_PRESENT
    emberAfPluginNetworkSteeringStart();
#else // !SL_CATALOG_ZIGBEE_NETWORK_STEERING_PRESENT
    emberAfStartSearchForJoinableNetwork();
#endif // SL_CATALOG_ZIGBEE_NETWORK_STEERING_PRESENT
#endif // EMBER_AF_HAS_COORDINATOR_NETWORK
  } else {
    // MISRA requires ..else if.. to have terminating else.
  }
}

void emberAfPluginButtonJoiningButton1EventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(buttonEvent1);
  emberAfPluginButtonJoiningButtonEventCallback(1, // button 1 is pressed
                                                buttonPressDurationMs);
}

void emberAfPluginButtonJoiningPressButton(uint8_t button)
{
  // We don't bother to check the button press both times
  // since the only reason it fails is invalid button.
  // No point in doing that twice.
  bool result = buttonPress(button, BUTTON_PRESSED);
  buttonPress(button, BUTTON_RELEASED);
  if (!result) {
    emberAfCorePrintln("Invalid button %d", button);
  }
}

// ISR context functions!!!

// WARNING: these functions are in ISR so we must do minimal
// processing and not make any blocking calls (like printf)
// or calls that take a long time.

static bool buttonPress(uint8_t button, uint8_t state)
{
  // ISR CONTEXT!!!
  static uint32_t timeMs;
  #ifdef UC_BUILD
  sl_zigbee_event_t *eventControl;
  if (button == BUTTON0) {
    eventControl = buttonEvent0;
  } else if (button == BUTTON1) {
    eventControl = buttonEvent1;
  } else {
    return false;
  }

  #else
  EmberEventControl* event;

  if (button == BUTTON0) {
    event = &buttonEvent0;
  } else if (button == BUTTON1) {
    event = &buttonEvent1;
  } else {
    return false;
  }

  EmberEventControl eventControl = *event;
  #endif
  if (state == BUTTON_PRESSED) {
    buttonPressDurationMs = 0;
    timeMs = halCommonGetInt32uMillisecondTick();
  } else {
    buttonPressDurationMs = elapsedTimeInt32u(timeMs, halCommonGetInt32uMillisecondTick());
    slxu_zigbee_event_set_active(eventControl);
  }

  return true;
}

void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{
  // ISR CONTEXT!!!
  buttonPress(button, state);
}
