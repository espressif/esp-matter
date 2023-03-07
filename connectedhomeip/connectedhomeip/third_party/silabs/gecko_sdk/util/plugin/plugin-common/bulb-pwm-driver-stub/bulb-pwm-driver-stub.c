// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BULB_PWM_DRIVER
#include EMBER_AF_API_DEBUG_PRINT

// Stub versious of the frequency and ticks per microsecond so we can test
// upper level logic in simulation.
#define TICKS_PER_PWM_PERIOD  6000
#define TICKS_PER_MICROSECOND 6

void halBulbPwmDriverBlinkInit(void);

void halBulbPwmDriverInitialize(void)
{
  halBulbPwmDriverBlinkInit();
}

uint16_t halBulbPwmDriverTicksPerPeriod(void)
{
  return TICKS_PER_PWM_PERIOD;
}

uint16_t halBulbPwmDriverTicksPerMicrosecond(void)
{
  return TICKS_PER_MICROSECOND;
}

void halBulbPwmDriverSetPwmLevel(uint16_t value, uint8_t pwm)
{
  emberAfCorePrintln("BULB-PWM-DRIVER-STUB PWM DRIVE:  %d %d", pwm, value);
}

// Required for AFv2 support
void emberAfPluginBulbPwmDriverInitCallback()
{
  halBulbPwmDriverInitialize();
}

void halBulbPwmDriverStatusLedOn(void)
{
  emberAfCorePrintln("PWM STATUS ON");
}

void halBulbPwmDriverStatusLedOff(void)
{
  emberAfCorePrintln("PWM STATUS OFF");
}
