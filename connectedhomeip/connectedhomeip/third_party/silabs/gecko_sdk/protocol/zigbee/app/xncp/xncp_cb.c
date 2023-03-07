/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "xncp.h"

/** @brief Enables or disables the processing of EZSP commands on the NCP.
 *
 * The application can use this callback to enable or
 * disable the processing of EZSP commands on the NCP. If this function returns
 * true, the frame will be handled by the NCP in a normal
 * manner. If this function returns false, the NCP will disregard this
 * command and respond to the HOST that it is an invalid frame.
 *
 * @param frameId The EZSP frame ID of the message.  Ver.: always
 * @param payloadLength The length of the payload of the EZSP message.
 *  Ver.: always
 * @param payload The EZSP message payload.  Ver.: always
 *
 * @return A ::bool value indicating whether or not the frame will
 * continue to be handled. If this function returns true,
 * the frame will be handled by the NCP in a normal manner. If this function
 * returns false, then the NCP will disregard this command and respond to the
 * HOST that it is an invalid frame.
 */
WEAK(bool emberAfPluginXncpPermitHostToNcpFrameCallback(uint16_t frameId,
                                                        uint8_t payloadLength,
                                                        uint8_t *payload))
{
  return true;
}

/** @brief Called when the NCP wants to inform the HOST of a callback
 * that has been called on the NCP.
 *
 * If this function returns false,
 * the NCP will not notify the HOST of the callback. If the function returns
 * true, then the NCP will continue scheduling the notification of the HOST
 * regarding the callback.
 *
 * @param frameId The frameId of the callback.  Ver.: always
 * @param payloadLength The length of the payload carried in the callback.
 *  Ver.: always
 * @param payload The actual payload carried by the callback.  Ver.: always
 *
 * @return A ::bool value. If the value is false, then
 * the NCP will not notify the HOST of the callback. If the return value is
 * true, the NCP will continue scheduling the notification of the HOST
 * regarding the callback.
 */
WEAK(bool emberAfPluginXncpPermitNcpToHostFrameCallback(uint16_t frameId,
                                                        uint8_t payloadLength,
                                                        uint8_t *payload))
{
  return true;
}

/** @brief Called when the NCP receives a custom EZSP message from the HOST.
 *
 * The message length and payload is passed to the callback in the first
 * two arguments. The implementation can then fill in the replyPayload and set
 * the replayPayloadLength to the number of bytes in the replyPayload.
 * See documentation for the function ezspCustomFrame on sending these messages
 * from the HOST.
 *
 * @param messageLength The length of the messagePayload.
 * @param messagePayload The custom message that was sent from the HOST.
 * Ver.: always
 * @param replyPayloadLength The length of the replyPayload.  This needs to be
 * set by the implementation for a properly formed response to be sent
 * back to the HOST. Ver.: always
 * @param replyPayload The custom message to send back to the HOST in respose
 * to the custom message. Ver.: always
 *
 * @return An ::EmberStatus indicating the result of the custom message
 * handling. This returned status is always the first byte of the EZSP
 * response.
 */
WEAK(EmberStatus emberAfPluginXncpIncomingCustomFrameCallback(uint8_t messageLength,
                                                              uint8_t *messagePayload,
                                                              uint8_t *replyPayloadLength,
                                                              uint8_t *replyPayload))
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

/** @brief Called when the NCP receives an incoming Zigbee PRO message.
 *
 * This handling is done before any other action is taken by the NCP on the
 * callback. If the function returns true, the
 * incoming message is done being handled, and the NCP does not need to record
 * the callback or report it to the HOST.
 *
 * @param type The ::EmberIncomingMessageType of the incoming message.
 *  Ver.: always
 * @param apsFrame The ::EmberApsFrame received in the incoming message.
 *  Ver.: always
 * @param message The payload of the incoming message, in the form of an
 * ::EmberMessageBuffer.  Ver.: always
 *
 * @return A ::bool value. If this value is true, the NCP will not handle
 * the incoming message any further and it will not report the incoming message
 * to the HOST. If the returned value is false, the NCP will continue
 * to process the incoming message and the callback will also be passed to the
 * HOST.
 */
WEAK(bool emberAfPluginXncpIncomingMessageCallback(EmberIncomingMessageType type,
                                                   EmberApsFrame *apsFrame,
                                                   EmberMessageBuffer message))
{
  return false;
}

/** @brief Gets XNCP information.
 *
 * This callback enables users to communicate the version number and
 * manufacturer ID of their NCP application to the framework. This information
 * is needed for the EZSP command frame called getXncpInfo. This callback will
 * be called when that frame is received so that the application can report
 * its version number and manufacturer ID to be sent back to the HOST.
 *
 * @param versionNumber The version number of the NCP application.
 * @param manufacturerId The manufacturer ID of the NCP application.
 */
WEAK(void emberAfPluginXncpGetXncpInformation(uint16_t *manufacturerId,
                                              uint16_t *versionNumber))
{
  *versionNumber = *manufacturerId = 0;
}
