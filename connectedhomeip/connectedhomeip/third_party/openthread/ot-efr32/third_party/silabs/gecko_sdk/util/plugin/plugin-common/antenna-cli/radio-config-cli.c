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
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

#define PHY_COUNT ((sizeof(phyNames) / sizeof(phyNames[0])) - 1)
static const char * const phyNames[] = {
  "IEEE802154_2P4_MODE_DEFAULT",
#if HAL_CONFIG || defined(EZSP_HOST)
  "IEEE802154_2P4_MODE_ANT_DIV",
  "IEEE802154_2P4_MODE_COEX",
  "IEEE802154_2P4_MODE_ANT_DIV_COEX",
  "IEEE802154_2P4_MODE_FEM",
  "IEEE802154_2P4_MODE_ANT_DIV_FEM",
  "IEEE802154_2P4_MODE_COEX_FEM",
  "IEEE802154_2P4_MODE_ANT_DIV_COEX_FEM",
#endif //HAL_CONFIG || defined(EZSP_HOST)
  "INVALID_PHY_SELECTION"
};

//-----------------------------------------------------------------------------
// Get Active Radio PHY
// Console Command : "plugin antenna get-active-phy"
// Console Response: "Active Radio PHY:<Active Radio PHY>"
void emberAfPluginGetActiveRadioPhy(void)
{
  uint8_t activePhy = 0;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(activePhy);

  ezspGetValue(EZSP_VALUE_ACTIVE_RADIO_CONFIG, &valueLength, &activePhy);
#elif !defined(EMBER_STACK_IP) && defined(HAL_CONFIG)//!EZSP_HOST
  extern uint8_t halGetActiveRadioConfig(void);
  activePhy = (uint8_t)halGetActiveRadioConfig();
#endif //EZSP_HOST

  if (activePhy >= PHY_COUNT) {
    activePhy = PHY_COUNT;
  }
  emberSerialPrintfLine(APP_SERIAL, "Active Radio PHY:%p", phyNames[activePhy]);
}
