/// Copyright 2017 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "../../plugin-common/eeprom-powerdown/eeprom-powerdown.h"

void emberAfPluginEepromPowerdownInitCallback(void)
{
  emberEepromPowerDownInitCallback();
}
