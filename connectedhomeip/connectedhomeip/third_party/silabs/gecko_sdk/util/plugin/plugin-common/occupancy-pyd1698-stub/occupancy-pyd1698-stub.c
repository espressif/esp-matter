//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include EMBER_AF_API_OCCUPANCY
#include "../occupancy-pyd1698/occupancy-pyd1698.h"

EmberEventControl emberAfPluginOccupancyPyd1698NotifyEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698InitEventControl;

void emberAfPluginOccupancyPyd1698InitEventHandler(void)
{
}

void emberAfPluginOccupancyPyd1698NotifyEventHandler(void)
{
}

void emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventHandler(void)
{
}

void emberAfPluginOccupancyPyd1698OccupancyTimeoutEventHandler(void)
{
}

void emberAfPluginOccupancyPyd1698InitCallback(void)
{
}

HalOccupancySensorType halOccupancyGetSensorType(void)
{
  return HAL_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC;
}

void halOccupancyPyd1698Read(HalPydInMsg_t *readMsg)
{
}

void halOccupancyPyd1698WriteConfiguration(HalPydCfg_t *cfgMsg)
{
}

void halOccupancyPyd1698GetCurrentConfiguration(HalPydCfg_t *config)
{
}
