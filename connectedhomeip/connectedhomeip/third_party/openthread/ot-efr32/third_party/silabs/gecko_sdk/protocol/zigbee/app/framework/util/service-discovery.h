/***************************************************************************//**
 * @file
 * @brief Interface for initiating and processing ZDO service discovery
 * (match descriptor) requests and response.
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

#ifndef SILABS_SERVICE_DISCOVERY_H
#define SILABS_SERVICE_DISCOVERY_H

#include "../include/af.h"
#define EMBER_SUPPORTED_SERVICE_DISCOVERY_STATES_PER_NETWORK 4u
// Internal Functions
void emAfServiceDiscoveryMatched(EmberNodeId nodeId,
                                 uint8_t endpointListLength,
                                 uint8_t* endpointList);

// Platform specific call to send a match descriptor request.
EmberStatus emAfSendMatchDescriptor(EmberNodeId target,
                                    EmberAfProfileId profileId,
                                    EmberAfClusterId clusterId,
                                    bool serverCluster);

bool emAfServiceDiscoveryIncoming(EmberNodeId sender,
                                  EmberApsFrame* apsFrame,
                                  const uint8_t* message,
                                  uint16_t length);

void emAfServiceDiscoveryComplete(uint8_t networkIndex, uint8_t sequenceNumber);

#ifndef EMBER_AF_DISCOVERY_TIMEOUT_QS
  #define EMBER_AF_DISCOVERY_TIMEOUT_QS (2u * 4u)
#endif

#ifdef UC_BUILD

extern sl_zigbee_event_t emAfServiceDiscoveryEvents[EMBER_SUPPORTED_SERVICE_DISCOVERY_STATES_PER_NETWORK][EMBER_SUPPORTED_NETWORKS];
void emAfServiceDiscoveryTimeoutHandler(sl_zigbee_event_t *event);

#else // !UC_BUILD

extern EmberEventControl emAfServiceDiscoveryEventControls[EMBER_SUPPORTED_SERVICE_DISCOVERY_STATES_PER_NETWORK][EMBER_SUPPORTED_NETWORKS];
void emAfServiceDiscoveryTimeoutHandler(EmberEventControl *control);

#if EMBER_SUPPORTED_NETWORKS == 1
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc Request State 0 NWK 0",             \
  "Svc Disc Request State 1 NWK 0",             \
  "Svc Disc Request State 2 NWK 0",             \
  "Svc Disc Request State 3 NWK 0",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                                           \
  { &emAfServiceDiscoveryEventControls[0][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler },
#elif EMBER_SUPPORTED_NETWORKS == 2
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc Request State 0 NWK 0",             \
  "Svc Disc Request State 1 NWK 0",             \
  "Svc Disc Request State 2 NWK 0",             \
  "Svc Disc Request State 3 NWK 0",             \
  "Svc Disc Request State 0 NWK 1",             \
  "Svc Disc Request State 1 NWK 1",             \
  "Svc Disc Request State 2 NWK 1",             \
  "Svc Disc Request State 3 NWK 1",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                                           \
  { &emAfServiceDiscoveryEventControls[0][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler },
#elif EMBER_SUPPORTED_NETWORKS == 3
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc Request State 0 NWK 0",             \
  "Svc Disc Request State 1 NWK 0",             \
  "Svc Disc Request State 2 NWK 0",             \
  "Svc Disc Request State 3 NWK 0",             \
  "Svc Disc Request State 0 NWK 1",             \
  "Svc Disc Request State 1 NWK 1",             \
  "Svc Disc Request State 2 NWK 1",             \
  "Svc Disc Request State 3 NWK 1",             \
  "Svc Disc Request State 0 NWK 2",             \
  "Svc Disc Request State 1 NWK 2",             \
  "Svc Disc Request State 2 NWK 2",             \
  "Svc Disc Request State 3 NWK 2",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                                           \
  { &emAfServiceDiscoveryEventControls[0][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler },
#elif EMBER_SUPPORTED_NETWORKS == 4
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc Request State 0 NWK 0",             \
  "Svc Disc Request State 1 NWK 0",             \
  "Svc Disc Request State 2 NWK 0",             \
  "Svc Disc Request State 3 NWK 0",             \
  "Svc Disc Request State 0 NWK 1",             \
  "Svc Disc Request State 1 NWK 1",             \
  "Svc Disc Request State 2 NWK 1",             \
  "Svc Disc Request State 3 NWK 1",             \
  "Svc Disc Request State 0 NWK 2",             \
  "Svc Disc Request State 1 NWK 2",             \
  "Svc Disc Request State 2 NWK 2",             \
  "Svc Disc Request State 3 NWK 2",             \
  "Svc Disc Request State 0 NWK 3",             \
  "Svc Disc Request State 1 NWK 3",             \
  "Svc Disc Request State 2 NWK 3",             \
  "Svc Disc Request State 3 NWK 3",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                                           \
  { &emAfServiceDiscoveryEventControls[0][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][0], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][1], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][2], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[0][3], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[1][3], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[2][3], (void(*)(void))emAfServiceDiscoveryTimeoutHandler }, \
  { &emAfServiceDiscoveryEventControls[3][3], (void(*)(void))emAfServiceDiscoveryTimeoutHandler },
#else
  #error
#endif

#endif // !UC_BUILD
#endif // SILABS_SERVICE_DISCOVERY_H
