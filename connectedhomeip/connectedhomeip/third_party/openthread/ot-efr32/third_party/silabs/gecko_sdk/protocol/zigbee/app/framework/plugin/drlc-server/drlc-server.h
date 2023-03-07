/***************************************************************************//**
 * @file
 * @brief  *
 * This is an implementation of a DRLC server, which is intended
 * for testing only. Generally, it is expected that the DRLC
 * server will act as a pass through for DRLC events and will not maintain
 * a list of scheduled events of its own. However, for testing purposes
 * it is highly useful to have the server store scheduled events so that
 * - minus a backend - the ESP can respond to get scheduled events
 * commands.
 *
 * Several things about
 * the code make it incomplete as an actual DRLC server table:
 *
 * 1. Scheduled events are not stored in persistent memory, which they
 *    would have to be if this were intended for an ESP which for some
 *    reason were storing events.
 *
 * Note: The EmberAfLoadControlEvent source field is used by the plugin as a
 * status field.  If the first byte is set to 0xFF, the event is
 * considered inactive.  If it is set to 0x00, the event is considered
 * active.  Other bytes are used internally.
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

#include "../../include/af-types.h"

/**
 * @defgroup drlc-server DRLC Server
 * @ingroup component cluster
 * @brief API and Callbacks for the DRLC Cluster Server Component
 *
 * Silicon Labs implementation of the Demand Response Load Control (DRLC)
 * cluster server. For more information about the component code, see the
 * comments in app/framework/plugin/drlc-server/drlc-server.h. This component
 * requires extending to manage the event table (adding/removing events). It
 * simply serves up the events that are entered into the table.
 *
 */

/**
 * @addtogroup drlc-server
 * @{
 */

#ifndef EMBER_AF_PLUGIN_DRLC_SERVER_SCHEDULED_EVENT_TABLE_SIZE
  #define  EMBER_AF_PLUGIN_DRLC_SERVER_SCHEDULED_EVENT_TABLE_SIZE 2
#endif //EMBER_AF_PLUGIN_DRLC_SERVER_SCHEDULED_EVENT_TABLE_SIZE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// used by the CLI to clear all scheduled events in the DRLC server
void emAfClearScheduledLoadControlEvents(uint8_t endpoint);

// Retrieves the load control event at the given index.
EmberStatus emAfGetScheduledLoadControlEvent(uint8_t endpoint,
                                             uint8_t index,
                                             EmberAfLoadControlEvent *event);

// Sets the load control event at the given index.  The first byte of the
// source field should be set to 0x00 for active or 0xFF for inactive.
EmberStatus emAfSetScheduledLoadControlEvent(uint8_t endpoint,
                                             uint8_t index,
                                             const EmberAfLoadControlEvent *event);

void emAfPluginDrlcServerPrintInfo(uint8_t endpoint);

void emAfPluginDrlcServerSlceMessage(EmberNodeId nodeId,
                                     uint8_t srcEndpoint,
                                     uint8_t dstEndpoint,
                                     uint8_t index);

// These internal functions help maintain event order, which is sorted by
// start time and issuer event ID. This is needed when responding to
// GetScheduledEvents with startTime and/or issuerEventId specified, where
// the returned events must be sent back in order
uint8_t findPlacementInEventOrderToLoadControlIndex(uint8_t ep, uint8_t index);
void updateEventOrderToLoadControlIndex(uint8_t ep,
                                        uint8_t index,
                                        bool eventAdded,
                                        bool isNewEvent);
void clearEventOrderToLoadControlIndex(uint8_t ep);

#endif //DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of drlc-server
