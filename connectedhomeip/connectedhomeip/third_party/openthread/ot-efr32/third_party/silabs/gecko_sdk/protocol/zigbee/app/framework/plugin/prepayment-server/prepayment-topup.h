/***************************************************************************//**
 * @file
 * @brief TopUp APIs and defines for the Prepayment Server plugin.
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

#ifndef SILABS_PREPAYMENT_TOP_UP_H
#define SILABS_PREPAYMENT_TOP_UP_H

/**
 * @brief Formats top up payload data needed by the Publish Top Up Log command.
 * @param nodeId The address of the destination node to whom the Publish Top Up
 * Log command should be sent.
 * @param srcEndpoint The endpoint of the sender that supports the prepayment cluster.
 * @param dstEndpoint The endpoint on the destination node that supports the prepayment client.
 * @param latestEndTime The latest top up time of top up records to be returned in the
 * Publish Top Up Log command(s).
 * @param numberOfRecords The maximum number of records to return in the Publish
 * Top Up Log command(s).
 *
 **/
void emberAfPluginSendPublishTopUpLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                      uint32_t latestEndTime, uint8_t numberOfRecords);

/**
 * @brief Sends a Publish Top Up Log command.
 * @param nodeId The address of the destination node to whom the Publish Top Up
 * Log command should be sent.
 * @param srcEndpoint The endpoint of the sender that supports the prepayment cluster.
 * @param dstEndpoint The endpoint on the destination node that supports the prepayment client.
 * @param commandIndex Indicates which payload fragment is being sent.  Set to 0xFE
 * if this is the last expected command.
 * @param totalNumberOfCommands If the payload does not fit into 1 command,
 * this indicates the total number of sub-commands in the message.
 * @param topUpPayload An array of Top Up Payload structures, containing the payload
 * that should be included in the Publish Top Up Log command.
 *
 **/
void emberAfPluginPrepaymentServerPublishTopUpLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                  uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                  TopUpPayload *topUpPayload);

/**
 * @name Callbacks
 *  @{
 */

/** @brief Consumer Top Up
 *
 * This function is called when a Consumer Top Up command is received. This
 * callback must evaluate the command data and determine if it is a valid Top
 * Up command.
 *
 * @param originatingDevice   Ver.: always
 * @param topUpCode   Ver.: always
 */
bool emberAfPluginPrepaymentServerConsumerTopUpCallback(uint8_t originatingDevice,
                                                        uint8_t *topUpCode);
/** @} */ // end of Callbacks

#endif  // #ifndef _PREPAYMENT_TOP_UP_H_
