//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"

// Events
EmberEventControl emberAfPluginBatteryMonitorReadADCEventControl;

uint16_t halGetBatteryVoltageMilliV(void)
{
  return 0;
}

void halBatteryMonitorInitialize(void)
{
}

void emberAfPluginBatteryMonitorReadADCEventHandler(void)
{
}

void emberAfPluginBatteryMonitorInitCallback(void)
{
}
