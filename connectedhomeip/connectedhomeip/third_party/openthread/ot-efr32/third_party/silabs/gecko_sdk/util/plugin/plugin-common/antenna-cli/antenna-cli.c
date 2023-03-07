/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#if !defined(EZSP_HOST)
#include "stack/include/ember.h"
#else
#include "stack/include/ember-types.h"
#endif

#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#ifndef EMBER_STACK_IP
#include "app/util/common/common.h"
#endif //EMBER_STACK_IP

#ifdef EZSP_HOST
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/framework/include/af-types.h"
#include "app/framework/util/af-main.h"
#endif //EZSP_HOST
#ifdef EMBER_AF_GENERATE_CLI
#include "app/framework/plugin/counters/counters.h"
#else //!EMBER_AF_GENERATE_CLI
#include "app/util/counters/counters.h"
#endif //EMBER_AF_GENERATE_CLI
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include "plugin/antenna/antenna.h"

//-----------------------------------------------------------------------------
// Get TX antenna mode (0-don't switch,1-primary,2-secondary,3-TX antenna diversity)
// Console Command : "plugin antenna get-antenna-mode"
// Console Response: "TX antenna mode:<antennaMode>"
void emberAfPluginAntennaGetAntennaTxMode(void)
{
  uint8_t antennaMode;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(antennaMode);

  ezspGetValue(EZSP_VALUE_ANTENNA_MODE, &valueLength, &antennaMode);
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  antennaMode = (uint8_t)halGetAntennaMode();
#endif //EZSP_HOST

  emberSerialPrintfLine(APP_SERIAL, "TX antenna mode:%d", antennaMode);
}

//-----------------------------------------------------------------------------
// Set TX antenna mode (0-don't switch,1-primary,2-secondary,3-TX antenna diversity)
// Console Command : "plugin antenna set-antenna-mode <antennaMode>"
// Console Response: none
void emberAfPluginAntennaSetAntennaTxMode(void)
{
  uint8_t antennaMode = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(antennaMode);

  emberAfSetEzspValue(EZSP_VALUE_ANTENNA_MODE,
                      valueLength,
                      &antennaMode,
                      "set TX antenna mode");
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  halSetAntennaMode(antennaMode);
#endif //EZSP_HOST
}
//-----------------------------------------------------------------------------
// Get RX antenna mode (0-don't switch,1-primary,2-secondary,3-RX antenna diversity)
// Console Command : "plugin antenna get-antenna-rx-mode"
// Console Response: "RX Antenna Mode: 0x<antennaMode>"
void emberAfPluginAntennaGetAntennaRxMode(void)
{
  uint8_t antennaMode;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(antennaMode);

  ezspGetValue(EZSP_VALUE_ANTENNA_RX_MODE, &valueLength, &antennaMode);
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  antennaMode = (uint8_t)halGetAntennaRxMode();
#endif //EZSP_HOST

  emberSerialPrintfLine(APP_SERIAL, "RX antenna mode:%d", antennaMode);
}

//-----------------------------------------------------------------------------
// Set RX antenna mode (0-don't switch,1-primary,2-secondary,3-RX antenna diversity)
// Console Command : "plugin antenna set-antenna-mode 0x<antennaMode>"
// Console Response: none
void emberAfPluginAntennaSetAntennaRxMode(void)
{
  uint32_t antennaMode = (uint32_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(antennaMode);

  emberAfSetEzspValue(EZSP_VALUE_ANTENNA_RX_MODE,
                      valueLength,
                      (uint8_t*)&antennaMode,
                      "set RX antenna mode");
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  halSetAntennaRxMode(antennaMode);
#endif //EZSP_HOST
};

//-----------------------------------------------------------------------------
// Get Active Radio PHY
// Console Command : "plugin antenna get-active-phy"
// Console Response: "Active Radio PHY:<Active Radio PHY>"
void emberAfPluginAntennaGetActivePhy(void)
{
  extern void emberAfPluginGetActiveRadioPhy(void);
  emberAfPluginGetActiveRadioPhy();
}
