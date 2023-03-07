/***************************************************************************//**
 * @file
 * @brief Common code between multi PAN and non multi PAN support, which
 *  includes single PAN and multi-network.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "stack/include/ember.h"
#include "event_queue/event-queue.h"
#include "multi-pan.h"

extern EmberEventQueue emStackEventQueue;
#ifdef EMBER_TEST
  #define testAssert assert
#else // EMBER_TEST
  #define testAssert(x) do {} while (0)
#endif // EMBER_TEST

extern EmberEvent emBeaconEvents[];
extern void emZigbeeBeaconEventHandler(EmberEvent *event);

extern EmberEvent emPermitJoiningEvents[];
extern void emZigbeePermitJoiningEventHandler(EmberEvent *event);

extern EmberEvent emNetworkManagementEvents[];
extern void emNetworkManagementEventHandler(EmberEvent *event);

#if !defined(SL_ZIGBEE_LEAF_STACK) || defined(CSL_SUPPORT)
extern EmberEvent emNeighborExchangeEvents[];
extern void emNeighborExchangeEventHandler(EmberEvent *event);
#endif

extern EmberEvent emRequestKeyEvents[];
extern void emRequestKeyEventHandler(EmberEvent *event);

#if !defined(SL_ZIGBEE_LEAF_STACK) || defined(CSL_SUPPORT)

extern EmberEvent emSendParentAnnounceEvents[];
extern void emSendParentAnnounceEventHandler(EmberEvent *event);

#endif

extern EmberEvent emTransientLinkKeyEvents[];
extern void emTransientLinkKeyEventHandler(EmberEvent *event);

extern EmberEvent emGpTxEvents[];
extern void emGpTxEventHandler(EmberEvent *event);

extern EmberEvent emSourceRouteUpdateEvents[];
extern void emSourceRouteUpdateEventHandler(EmberEvent *event);

extern EmberEvent emAllowTcRejoinsUsingWellKnownKeyEvents[];
extern void emAllowTcRejoinsUsingWellKnownKeyEventHandler(EmberEvent *event);

static void initializeForkedEvents(void)
{
  for (uint8_t j = 0; j < emNumMultiPanForks; j++) {
    emBeaconEvents[j].actions.queue = &emStackEventQueue;
    emBeaconEvents[j].actions.handler = emZigbeeBeaconEventHandler;
    emBeaconEvents[j].actions.marker = NULL;
    emBeaconEvents[j].actions.name = EVENT_NAME("Beacon");
    emBeaconEvents[j].next = NULL;

    emPermitJoiningEvents[j].actions.queue = &emStackEventQueue;
    emPermitJoiningEvents[j].actions.handler = emZigbeePermitJoiningEventHandler;
    emPermitJoiningEvents[j].actions.marker = NULL;
    emPermitJoiningEvents[j].actions.name =  EVENT_NAME("Permit Joining");
    emPermitJoiningEvents[j].next = NULL;

    emNetworkManagementEvents[j].actions.queue = &emStackEventQueue;
    emNetworkManagementEvents[j].actions.handler = emNetworkManagementEventHandler;
    emNetworkManagementEvents[j].actions.marker = NULL;
    emNetworkManagementEvents[j].actions.name =  EVENT_NAME("NWK mgm");
    emNetworkManagementEvents[j].next = NULL;

#if !defined(SL_ZIGBEE_LEAF_STACK) || defined(CSL_SUPPORT)
    emNeighborExchangeEvents[j].actions.queue = &emStackEventQueue;
    emNeighborExchangeEvents[j].actions.handler = emNeighborExchangeEventHandler;
    emNeighborExchangeEvents[j].actions.marker = NULL;
    emNeighborExchangeEvents[j].actions.name =  EVENT_NAME("Neighbor exg");
    emNeighborExchangeEvents[j].next = NULL;
#endif

    emRequestKeyEvents[j].actions.queue = &emStackEventQueue;
    emRequestKeyEvents[j].actions.handler = emRequestKeyEventHandler;
    emRequestKeyEvents[j].actions.marker = NULL;
    emRequestKeyEvents[j].actions.name =  EVENT_NAME("Req key");
    emRequestKeyEvents[j].next = NULL;

#if !defined(SL_ZIGBEE_LEAF_STACK) || defined(CSL_SUPPORT)
    emSendParentAnnounceEvents[j].actions.queue = &emStackEventQueue;
    emSendParentAnnounceEvents[j].actions.handler = emSendParentAnnounceEventHandler;
    emSendParentAnnounceEvents[j].actions.marker = NULL;
    emSendParentAnnounceEvents[j].actions.name =  EVENT_NAME("Parent announce");
    emSendParentAnnounceEvents[j].next = NULL;
#endif

    emTransientLinkKeyEvents[j].actions.queue = &emStackEventQueue;
    emTransientLinkKeyEvents[j].actions.handler = emTransientLinkKeyEventHandler;
    emTransientLinkKeyEvents[j].actions.marker = NULL;
    emTransientLinkKeyEvents[j].actions.name =  EVENT_NAME("Transient link key");
    emTransientLinkKeyEvents[j].next = NULL;

    emGpTxEvents[j].actions.queue = &emStackEventQueue;
    emGpTxEvents[j].actions.handler = emGpTxEventHandler;
    emGpTxEvents[j].actions.marker = NULL;
    emGpTxEvents[j].actions.name =  EVENT_NAME("GP TX");
    emGpTxEvents[j].next = NULL;

    emSourceRouteUpdateEvents[j].actions.queue = &emStackEventQueue;
    emSourceRouteUpdateEvents[j].actions.handler = emSourceRouteUpdateEventHandler;
    emSourceRouteUpdateEvents[j].actions.marker = NULL;
    emSourceRouteUpdateEvents[j].actions.name =  EVENT_NAME("Source route update");
    emSourceRouteUpdateEvents[j].next = NULL;

    emAllowTcRejoinsUsingWellKnownKeyEvents[j].actions.queue = &emStackEventQueue;
    emAllowTcRejoinsUsingWellKnownKeyEvents[j].actions.handler = emAllowTcRejoinsUsingWellKnownKeyEventHandler;
    emAllowTcRejoinsUsingWellKnownKeyEvents[j].actions.marker = NULL;
    emAllowTcRejoinsUsingWellKnownKeyEvents[j].actions.name =  EVENT_NAME("Allow TC rejoin");
    emAllowTcRejoinsUsingWellKnownKeyEvents[j].next = NULL;
  }
}

void emMultiPanInit(void)
{
  initializeForkedEvents();
}
