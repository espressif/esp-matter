// Copyright 2015 Silicon Laboratories, Inc.                                *80*
//
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"

// ------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginPowerMeterCs5463InitEventControl;
EmberEventControl emberAfPluginPowerMeterCs5463ReadEventControl;

// ------------------------------------------------------------------------------
// Plugin private event handlers
void emberAfPluginPowerMeterCs5463InitEventHandler(void)
{
}

void emberAfPluginPowerMeterCs5463ReadEventHandler(void)
{
}

// ------------------------------------------------------------------------------
// Plugin defined callbacks
// The init callback, which will be called by the framework on init.
void emberAfPluginPowerMeterCs5463InitCallback(void)
{
}

void halPowerMeterInit(void)
{
}

uint32_t halGetVrmsMilliV(void)
{
  return 0;
}

uint32_t halGetCrmsMilliA(void)
{
  return 0;
}

uint32_t halGetApparentPowerMilliW(void)
{
  return 0;
}

int32_t halGetActivePowerMilliW(void)
{
  return 0;
}

int8_t halGetPowerFactor(void)
{
  return 0;
}

bool halPowerMeterCalibrateCurrentGain(uint16_t referenceCurrentMa)
{
  return true;
}

void halSetCurrentGain(uint16_t currentGain)
{
}

int16_t halGetPowerMeterTempCentiC(void)
{
  return 0;
}

uint8_t halGetPowerMeterStatus(void)
{
  return 0;
}
