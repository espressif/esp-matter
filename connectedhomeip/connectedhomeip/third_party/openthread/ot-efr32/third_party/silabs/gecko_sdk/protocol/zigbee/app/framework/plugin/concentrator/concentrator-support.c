/***************************************************************************//**
 * @file
 * @brief Code common to SOC and host to handle periodically broadcasting
 * many-to-one route requests (MTORRs).
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

// *****************************************************************************
// * concentrator-support.c
// *
// * Code common to SOC and host to handle periodically broadcasting
// * many-to-one route requests (MTORRs).
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "stack/include/zigbee-device-stack.h"
#include "concentrator-support.h"
#include "stack/include/source-route.h"

// *****************************************************************************
// Globals

#ifdef UC_BUILD
#include "concentrator-config.h"
#define emberIncomingRouteErrorHandler emAfPluginConcentratorIncomingRouteErrorCallback
// TODO: It appears that the "update" event is not used at all in the code.
// Once we clean up the UC_BUILD we should just remove it completely.
#else // !UC_BUILD
EmberEventControl emberAfPluginConcentratorUpdateEventControl;
#define myEvent emberAfPluginConcentratorUpdateEventControl
#endif // UC_BUILD

#define MIN_QS (EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS << 2)
#define MAX_QS (EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS << 2)

#if (MIN_QS > MAX_QS)
  #error "Minimum broadcast time must be less than max (EMBER_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS < EMBER_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS)"
#endif

// Handy values to make the code more readable.

typedef enum {
  USE_MIN_TIME = 0,
  USE_MAX_TIME = 1,
  USE_ADDRESS_DISCOVERY_TIME = 2,
} QueueDelay;

#ifndef EMBER_AF_HAS_ROUTER_NETWORK
  #error "Concentrator support only allowed on routers and coordinators."
#endif

extern uint8_t emSupportedNetworks;

//This is used to store the sourceRouteOverhead to our last sender
//It defaults to 0xFF if no valid sourceRoute is found. When available, it
//is used once to prevent the overhead of calling ezspGetSourceRouteOverhead()
//and cleared subsequently.
#if defined(EZSP_HOST)
static EmberNodeId targetIds[EMBER_SUPPORTED_NETWORKS];
#define targetId targetIds[emberGetCurrentNetwork()]
static uint8_t sourceRouteOverheads[EMBER_SUPPORTED_NETWORKS];
#define sourceRouteOverhead sourceRouteOverheads[emberGetCurrentNetwork()]
#endif

// EMINSIGHT-2484 - allow applications to set whether or not they want
// routers to send mtorrs.
EmberAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehaviors[EMBER_SUPPORTED_NETWORKS];

// *****************************************************************************
// Functions

void emberAfPluginConcentratorInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  for (uint8_t i = 0; i < emSupportedNetworks; i++) {
#if defined(EZSP_HOST)
    targetIds[i] = EMBER_UNKNOWN_NODE_ID;
    sourceRouteOverheads[i] = EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN;
#endif // EZSP_HOST
    emAfPluginConcentratorRouterBehaviors[i] = EMBER_AF_PLUGIN_CONCENTRATOR_DEFAULT_ROUTER_BEHAVIOR;
  }
}

uint32_t emberAfPluginConcentratorQueueDiscovery(void)
{
  return emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_RESCHEDULE);
}

void emberAfPluginConcentratorMessageSentCallback(EmberOutgoingMessageType type,
                                                  uint16_t indexOrDestination,
                                                  EmberApsFrame *apsFrame,
                                                  EmberStatus status,
                                                  uint16_t messageLength,
                                                  uint8_t *messageContents)
{
  // Handling of message sent failure is now done in the source route library.
  // the callback however, is left here in case there is still consumer side code depening on it
}

// We only store one valid overhead for one destination. We don't want to overwrite that with
// an invalid source route to another destination. We do however want to invalidate an
// overhead to our destination if it is now unknown.
void emberAfSetSourceRouteOverheadCallback(EmberNodeId destination, uint8_t overhead)
{
  #if defined(EZSP_HOST)
  (void)emberSetCurrentNetwork(emberGetCallbackNetwork());

  if (!(destination != targetId && overhead == EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN)) {
    targetId = destination;
    sourceRouteOverhead = overhead;
  }
  #endif
}

// In an effort to reduce the traffic between the host and NCP, for each incoming message,
// the sourceRouteOverhead to that particular destination is sent from the NCP to the host
// as a part of the incomingMessageHandler(). This information is cached and can be used
// once to calculate the MaximumPayload() to that same destination. It is invalidated after
// one use.
uint8_t emberAfGetSourceRouteOverheadCallback(EmberNodeId destination)
{
#if defined(EZSP_HOST)
  // While this function is named .*Callback, it is not always called in the
  // context of an app framework callback, so we do not need to call
  // emberSetCurrentNetwork(emberGetCallbackNetwork()) before referencing
  // targetId and sourceRouteOverhead

  if (targetId == destination && sourceRouteOverhead != EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN) {
    emberAfDebugPrintln("ValidSourceRouteFound %u ", sourceRouteOverhead);
    return sourceRouteOverhead;
  } else {
    return ezspGetSourceRouteOverhead(destination);
  }
#else
  return emberGetSourceRouteOverhead(destination);
#endif
}

void emberIncomingRouteErrorHandler(EmberStatus status, EmberNodeId target)
{
}

void ezspIncomingRouteErrorHandler(EmberStatus status, EmberNodeId target)
{
}

void emberAfPluginConcentratorStackStatusCallback(EmberStatus status)
{
  (void)emberSetCurrentNetwork(emberGetCallbackNetwork());

  EmberNodeType nodeType;
  if (status == EMBER_NETWORK_DOWN
      && !emberStackIsPerformingRejoin()) {
    //now we clear/init the source route table everytime the network is up , therefore we do clear the source route table on rejoin.
  } else if (status == EMBER_NETWORK_UP) {
    if ((emAfPluginConcentratorRouterBehavior == EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL)
        || (emberAfGetNodeType(&nodeType) == EMBER_SUCCESS
            && nodeType == EMBER_COORDINATOR)) {
      emberSetConcentrator(true,
                           EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE,
                           EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS,
                           EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS,
                           EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD,
                           EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD,
                           EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS
                           );
    }
  }
}
