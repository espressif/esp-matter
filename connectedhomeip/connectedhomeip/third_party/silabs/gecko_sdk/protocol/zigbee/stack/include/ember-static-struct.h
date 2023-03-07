/***************************************************************************//**
 * @file
 * @brief Type definitions for customer configurable memory allocations
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

#ifndef SILABS_EMBER_STATIC_STRUCT_H
#define SILABS_EMBER_STATIC_STRUCT_H

#include "mac-child.h" // unified-mac

// This file is used in conjunction with ember-static-memory.h
//  to provide typedefs for structures allocated there.

// #########################################################
// # Application developers should not modify any portion  #
// #  of this file.  Doing so may lead to mysterious bugs. #
// #########################################################

// Neighbors

typedef struct {
  uint16_t data0[2];
  uint8_t  data1[12];
} EmNeighborTableEntry;

// Routing

typedef struct {
  uint16_t data0[2];
  uint8_t data1[3];
} EmRouteTableEntry;

typedef struct {
  uint16_t source;
  uint16_t sender;
  uint8_t  id;
  uint8_t  forwardRoutingCost;
  uint8_t  quarterSecondsToLive;
  uint8_t  routeTableIndex;
  uint8_t  networkIndex;
} EmDiscoveryTableEntry;

typedef struct {
  uint16_t source;
  uint8_t  sequence;
  uint8_t  numAcks;
  uint32_t neighborBitmask;
} EmBroadcastTableEntry;

typedef struct {
  uint16_t data0;
  uint8_t data1;
  uint16_t data2[2];
} EmRetryQueueEntry;

typedef struct {
  uint16_t data0;
  uint8_t data1;
  uint16_t data2[3];
} EmStoreAndForwardQueueEntry;

// APS

typedef struct {
  uint16_t data0;
  uint8_t data1[8];
  uint8_t data2;
} EmAddressTableEntry;

typedef struct {
  uint16_t data0[2];
  uint8_t data1[3];
} EmApsUnicastMessageData;

typedef struct {
  uint8_t identifier[8];
  uint8_t key[16];
  uint8_t info;
} EmKeyTableEntry;

// Note:
// For single network case:
//  - EmberNetworkInfo and EmPanInfo: Single instance present.
// For multi network case:
//  - EmberNetworkInfo: Instance present for each network.
//  - EmPanInfo: Single Instance present.
// For multi PAN case"
//  - EmberNetworkInfo and EmPanInfo: Instance present for each network.

typedef struct {
  // Child Table Info
  sl_mac_child_entry_t *childTable;
  uint16_t *childStatus;
  uint32_t *childTimers;
  uint16_t *childLqi;

  // Routing Table Info
  uint8_t *routeRecordTable;

  // Child Table End Device Info
  uint8_t *endDeviceTimeout;
  uint8_t endDeviceChildCount;

  // Used for sending link status more quickly at startup
  // and only used by routers.
  uint8_t fastLinkStatusCount;

  // Broadcast Table Info
  uint8_t broadcastHead;
  uint8_t inInitialBroadcastTimeout;
  uint32_t broadcastAgeCutoffIndexes;
  EmBroadcastTableEntry *broadcastTable;

  // Association info
  bool permitJoining;
  bool macPermitAssociation;
  bool allowRejoinsWithWellKnownKey;

  // Parent announcement
  uint8_t parentAnnounceIndex;
  uint8_t totalInitialChildren;

  // PanId conflict info
  EmberPanId newPanId;
} EmPanInfo;

// Network general info

typedef struct {
  uint8_t stackProfile;
  //uint16_t parentId;
  //uint8_t parentEui64[8];
  uint8_t nodeType;
  uint8_t zigbeeState;
  uint8_t dynamicCapabilities;
  //uint8_t radioChannel;
  //int8_t radioPower;
  //uint16_t localNodeId;
  //uint16_t localPanId;

  uint8_t zigbeeNetworkSecurityLevel;
  uint32_t securityStateBitmask;
  //uint8_t macDataSequenceNumber;
  uint8_t zigbeeSequenceNumber;
  uint8_t apsSequenceNumber;

  // Network security stuff
  uint8_t securityKeySequenceNumber;
  uint32_t nextNwkFrameCounter;

  // APS security stuff
  uint32_t incomingTcLinkKeyFrameCounter;

  // Neighbor table
  EmNeighborTableEntry* neighborTable;
  uint8_t neighborTableSize;
  uint8_t neighborCount;

  // Incoming frame counters table
  uint32_t* frameCounters;

  // The number of ticks since our last successful poll.  Ticks are
  // in seconds.
  uint32_t ticksSinceLastPoll; // for timing out our parent
  uint32_t msSinceLastPoll;   // for APS retry timeout adjustment

  // Transmission statistics that are reported in NWK_UPDATE_RESPONSE ZDO
  // messages.
  uint16_t unicastTxAttempts;
  uint16_t unicastTxFailures;
  uint16_t parentNwkInformation;

  // Child aging stuff
  //----------------------------------------------------------------
  // The last time we updated the child timers for each unit.
  // The milliseconds needs to be larger because we use it on children, who
  // may go a long time between calls to emberTick().
  uint16_t lastChildAgeTimeSeconds;
  uint32_t lastChildAgeTimeMs;

  // PAN info contains collection of elements needed to accommodate
  // single network, multi network and multi PAN use cases.
  EmPanInfo* panInfoData;
} EmberNetworkInfo;

#endif // SILABS_EMBER_STATIC_STRUCT_H
