/***************************************************************************//**
 * @file
 * @brief Internal APIs and defines for the Network Steering plugin.
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

#include "app/framework/plugin/network-steering/network-steering.h"

extern const char * emAfPluginNetworkSteeringStateNames[];
extern uint8_t emAfPluginNetworkSteeringTotalBeacons;
extern uint8_t emAfPluginNetworkSteeringJoinAttempts;

#ifdef UC_BUILD
#if (EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS != 1)
extern uint8_t emAfPluginNetworkSteeringPanIdIndex;
#endif // # EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
#else // UC_BUILD
#ifndef EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
extern uint8_t emAfPluginNetworkSteeringPanIdIndex;
#endif // # EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
#endif // UC_BUILD

uint8_t emAfPluginNetworkSteeringGetMaxPossiblePanIds(void);
void emAfPluginNetworkSteeringClearStoredPanIds(void);
uint16_t* emAfPluginNetworkSteeringGetStoredPanIdPointer(uint8_t index);

void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState);
uint8_t emAfPluginNetworkSteeringGetCurrentChannel();

extern EmberAfPluginNetworkSteeringJoiningState emAfPluginNetworkSteeringState;

#ifdef TRY_ALL_KEYS
  #define TRYING_ALL_KEYS                                                \
  (((emAfPluginNetworkSteeringState)                                     \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_USE_ALL_KEYS) \
   || ((emAfPluginNetworkSteeringState)                                  \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_USE_ALL_KEYS))
#else // TRY_ALL_KEYS
    #define TRYING_ALL_KEYS         false
#endif // TRY_ALL_KEYS

#define emAfPluginNetworkSteeringStateUsesInstallCodes()                      \
  (((emAfPluginNetworkSteeringState)                                          \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_INSTALL_CODE)      \
   || ((emAfPluginNetworkSteeringState)                                       \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_INSTALL_CODE) \
   || TRYING_ALL_KEYS)

#define emAfPluginNetworkSteeringStateUsesCentralizedKey()              \
  (((emAfPluginNetworkSteeringState)                                    \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_CENTRALIZED) \
   || ((emAfPluginNetworkSteeringState)                                 \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_CENTRALIZED))

#define emAfPluginNetworkSteeringStateUsesDistributedKey()              \
  (((emAfPluginNetworkSteeringState)                                    \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_DISTRIBUTED) \
   || ((emAfPluginNetworkSteeringState)                                 \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_DISTRIBUTED))

#define emAfPluginNetworkSteeringStateSetUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                   \
     |= EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)
#define emAfPluginNetworkSteeringStateUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                \
   & EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)
#define emAfPluginNetworkSteeringStateClearUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                     \
     &= ~EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)

#define emAfPluginNetworkSteeringStateSetVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                   \
     |= EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
#define emAfPluginNetworkSteeringStateVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                \
   & EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
#define emAfPluginNetworkSteeringStateClearVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                     \
     &= ~EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
