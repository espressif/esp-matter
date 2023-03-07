/***************************************************************************//**
 * @file
 * @brief A library for retrieving Ember stack counters over the air.
 *
 * The library allows the application to request counters from a remote
 * node.  The request specifies whether the remote node should clear
 * its counters after sending back the response.
 * The library also handles and responds to incoming counter requests appropriately.
 * A convenience method recognizes incoming responses.
 *
 * Summary of requirements:
 * - Include counters.c and counters-ota.c in the build on an EmberZNet processor.
 *   Include only counters-ota-host.c in the build on an EZSP host processor.
 * - Call ::emberIsIncomingCountersRequest() in the incoming message handler.
 * - Call ::emberIsIncomingCountersResponse() in the incoming message handler.
 * - Call ::emberIsOutgoingCountersResponse() in the message sent handler.
 *
 * The request and response messages use profile id EMBER_PRIVATE_PROFILE_ID,
 * and the following cluster ids:
 *
 * - EMBER_REPORT_COUNTERS_REQUEST
 * - EMBER_REPORT_AND_CLEAR_COUNTERS_REQUEST
 * - EMBER_REPORT_COUNTERS_RESPONSE
 * - EMBER_REPORT_AND_CLEAR_COUNTERS_RESPONSE
 *
 * The request has no payload.  The payload of the response is a list of the
 * non-zero counters.  Each entry in the list consists of the one-byte counter
 * id followed by a two-byte counter, low byte first.  If all entries do not
 * fit into a single payload, multiple response messages are sent.
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

#ifndef SILABS_PLUGIN_COUNTERS_OTA_H
#define SILABS_PLUGIN_COUNTERS_OTA_H

/**
 * @defgroup counters Counters
 * @ingroup component
 * @brief API and Callbacks for the Counters Component
 *
 * This component provides support for reading and
 * manipulating counters that record different events in the stack.
 *
 */

/**
 * @addtogroup counters
 * @{
 */

/** To ensure the counters response does not exceed the maximum
 * payload length, they are divided into separate messages if necessary.
 * The maximum length of 48 leaves sufficient room for headers, all
 * security modes, and source routing subframes if present.
 * It should not be necessary to change this value, but if you do,
 * make sure you understand the process.
 */
#define MAX_PAYLOAD_LENGTH 48

/**
 * @name API
 * @{
 */

/** Send an request to the specified destination to send back a report
 * of the non-zero counters.
 *
 * @param destination the address of the node to send the request to.
 * @param clearCounters whether or not the destination should reset its
 * counters to zero after successfully reporting them back to the requester.
 * Because of technical constraints, counters are always cleared
 * on a network coprocessor, such as the EM260.
 * @return EMBER_SUCCESS if the request was successfully submitted for
 * sending.  See ::emberSendUnicast() or ::ezspSendUnicast for possible
 * failure statuses.
 */
EmberStatus emberAfPluginCountersSendRequest(EmberNodeId destination,
                                             bool clearCounters);

/** Call this function at the beginning of
 * the incoming message handler.  It returns true if the incoming
 * message was a counter's request and should be ignored by
 * the rest of the incoming message handler.
 *
 * @param apsFrame the APS frame passed to the incoming message handler.
 * @param sender the node ID of the sender of the request.
 * @return true if the message was a counters request and should be
 * ignored by the rest of the incoming message handler.
 */
bool emberAfPluginCountersIsIncomingRequest(EmberApsFrame *apsFrame,
                                            EmberNodeId sender);

/** Call this function within the incoming
 * message handler to determine if the message is a counter's
 * response.  If so, it is up to the application to decode
 * the payload whose format is described above.
 *
 * @param apsFrame the APS frame passed to the incoming message handler.
 * @return true if the message is a counters response.
 */
bool emberAfPluginCountersIsIncomingResponse(EmberApsFrame *apsFrame);

/** Call this function at the beginning of the
 * message sent handler.  It returns true if the message was a
 * counters response and should be ignored by the rest of the handler.
 *
 * @param apsFrame the APS frame passed to the message sent handler.
 * @param status the status passed to the message sent handler.
 * @param return true if the message was a counters response and should
 * be ignored by the rest of the message sent handler.
 */
bool emberAfPluginCountersIsOutgoingResponse(EmberApsFrame *apsFrame,
                                             EmberStatus status);

/** @} */ // end of name APIs
/** @} */ // end of counters

#endif // SILABS_PLUGIN_COUNTERS_OTA_H
