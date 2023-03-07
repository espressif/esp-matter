/***************************************************************************//**
 * @file
 * @brief Code for multi PAN support.
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

#ifndef SILABS_MULTI_PAN_H
#define SILABS_MULTI_PAN_H

#include "stack/include/library.h"
#include "event_queue/event-queue.h"
#include "stack/include/ember-types.h"

extern uint8_t emNumMultiPanForks;
extern const EmberLibraryStatus emMultiPanLibraryStatus;

void emNetworkPanInfoDataInit(uint8_t nwkIndex, EmberNetworkInfo *curNetwork);
void emMultiPanInit(void);

#ifdef EMBER_MULTI_NETWORK_STRIPPED
#define emGetNetworkIndexForForkedGlobal()  0
#define emGetForkedEvent(x)  (x)
// void(control) to make compiler happy for unused warning.
#define emGetNetworkIndexFromEvent(event, list) EMBER_NULL_NETWORK_INDEX
#define emSetChildTableForCurrentNetwork()
#define emIsPanIdInUse(panId) 0
#define LOOP_NUM_MULTI_PAN_FORKS(body) \
  do {                                 \
    uint8_t nwkIndex = 0;              \
    { body }                           \
  } while (0)
#else // !EMBER_MULTI_NETWORK_STRIPPED
void emGetChildTokenForCurrentNetwork(void *data,
                                      uint16_t tokenAddress,
                                      uint8_t childIndex);

void emSetChildTokenForCurrentNetwork(uint16_t tokenAddress,
                                      uint8_t childIndex,
                                      void *data);

void emSetChildTableForCurrentNetwork(void);

// For a forked PAN event, gets the correct event control pointer based on the
// current network index. This should be used when arming an event to fire. The
// correct event control must be armed so that the event handler knows which
// network index is the right one.
// NOTE: not all events are forked. See multi-pan-common.c for a list of those
// which are. This API should only be called on those lists of events. In
// multi-PAN, events are forked so that the handlers can tell which network
// index armed the event (the triggering EmberEventControl pointer is checked in
// the handler. There is only one handler for all forked events.)
EmberEventControl* emGetForkedEventControl(EmberEventControl *list);

EmberEvent* emGetForkedEvent(EmberEvent *list);
uint8_t emGetNetworkIndexFromEvent(EmberEvent *event,
                                   EmberEvent *list);

// When dereferencing forked PAN globals, we need to read and write to the
// correct index. For multi PAN, this is the current network index returned by
// emGetCurrentNetworkIndex. For multi network, there's only one copy of the
// data, so we always return index 0.
uint8_t emGetNetworkIndexForForkedGlobal(void);

// For a forked PAN event, gets the network index that this event control
// maps to. This should be called in an event handler, so that the code can
// determine which network index is the right one.
uint8_t emGetNetworkIndexFromEventControl(EmberEventControl *control,
                                          EmberEventControl *list);

// This api can be used to check given panId is used by other multi PANs.
// This can be used to restrict using same PAN while forming or setting new
// PAN during PAN conflict resolution.
bool emIsPanIdInUse(EmberPanId panId);

#define LOOP_NUM_MULTI_PAN_FORKS(body) \
  for (uint8_t nwkIndex = 0;           \
       nwkIndex < emNumMultiPanForks;  \
       nwkIndex++)                     \
  { body }
#endif // EMBER_MULTI_NETWORK_STRIPPED
#endif // SILABS_MULTI_PAN_H
