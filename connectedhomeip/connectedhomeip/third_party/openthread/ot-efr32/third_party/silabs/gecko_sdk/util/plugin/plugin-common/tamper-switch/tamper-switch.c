// *****************************************************************************
// * tamper-switch.c
// *
// * This plugin uses the button-interface plugin to create a tamper switch.  It
// * will activate itself once it detects a very long press on the button, and
// * generate callbacks whenever it detects the device has been tampered with.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "hal/micro/micro.h"
#include EMBER_AF_API_BUTTON_INTERFACE
#include EMBER_AF_API_TAMPER_SWITCH

// This plugin has a hard requirement that button0 be used in button interface.
// As a result, we should absolutely generate an error if the BUTTON0 macro,
// which is critical for this plugin to work, is not defined.
#ifndef BUTTON0
#error "BUTTON0 must be defined for the tamper switch plugin to function!"
#endif

// ------------------------------------------------------------------------------
// Plugin private macros

#define TAMPER_SW_IS_ACTIVE_HI   EMBER_AF_PLUGIN_TAMPER_SWITCH_TAMPER_POLARITY

// state variables for whether or not the button is tracking tamper status yet
#define TAMPER_NOT_ACTIVE        0x00
#define TAMPER_ACTIVE            0x01

// ------------------------------------------------------------------------------
// Forward declaration of functions
void emberAfPluginTamperSwitchTamperActiveCallback(void);
void emberAfPluginTamperSwitchTamperAlarmCallback(void);

// ------------------------------------------------------------------------------
// Plugin private variables

// State variable for whether or not the pushbutton has entered "tamper" mode
static bool tamperState = TAMPER_NOT_ACTIVE;

// ------------------------------------------------------------------------------
// Plugin consumed callback implementations

// This function will be called on device init.  For now, all it needs to do is
// configure the button-interface button0 polarity to match the value specified
// by the user via plugin option
void emberAfPluginTamperSwitchInitCallback(void)
{
  halTamperSwitchInitialize();
}

// The button pressing callback should occur after the button has been held for
// four seconds.  At this point, it is safe to assume the contact switch is now
// inside its enclosure, so the button being released should be interpreted as
// the case being opened, which should trip the tamper alarm.
void emberAfPluginButtonInterfaceButton0PressingCallback(void)
{
  tamperState = TAMPER_ACTIVE;
  emberAfPluginTamperSwitchTamperActiveCallback();
}

// A long press on button 0 means the device was removed after being in its
// enclosure for more than 4 seconds.  This should be considered a tamper
// attempt, and the IAS Zone Server should be informed.
void emberAfPluginButtonInterfaceButton0PressedLongCallback(
  uint16_t button0TimePressed,
  bool     pressedAtReset)
{
  if (tamperState == TAMPER_ACTIVE) {
    emberAfPluginTamperSwitchTamperAlarmCallback();
    tamperState = TAMPER_NOT_ACTIVE;
  }
}

// ------------------------------------------------------------------------------
// Plugin public functions

void halTamperSwitchInitialize(void)
{
#if TAMPER_SW_IS_ACTIVE_HI
  halPluginButtonInterfaceSetButtonPolarity(
    BUTTON0,
    EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_HI);
#else
  halPluginButtonInterfaceSetButtonPolarity(
    BUTTON0,
    EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_LO);
#endif
}

// Disarm the tamper switch
void halTamperSwitchDisarm(void)
{
  tamperState = TAMPER_NOT_ACTIVE;
}

// Return the (polarity compensated) state of the button.
uint8_t halTamperSwitchGetValue(void)
{
  return(halPluginButtonInterfaceButtonPoll(BUTTON0));
}
