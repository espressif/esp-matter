/***************************************************************************//**
 * @file
 * @brief Programmable NCP code.
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

#ifndef SILABS_PLUGIN_XNCP_H
#define SILABS_PLUGIN_XNCP_H

#include PLATFORM_HEADER

/**
 * @addtogroup xncp
 *
 * The XNCP functionality provides a way for an NCP application to send
 * and receive custom EZSP frames to and from a HOST. This gives users the
 * ability to develop custom serial protocols between a HOST and the NCP.
 *
 * An NCP application can use the API ::emberAfPluginXncpSendCustomEzspMessage
 * to send custom EZSP messages to the HOST. The message will be sent
 * to the HOST in an asynchronous manner, but the application can use the
 * ::EmberStatus return byte from the API to tell if their message was
 * successfully scheduled.
 *
 * An NCP application wishing to receive and handle custom EZSP frames from
 * a HOST should make use of the callbacks provided by this module. The
 * two most important callbacks to an NCP application will most likely be
 * ::emberAfPluginXncpGetXncpInformation and
 * ::emberAfPluginXncpIncomingCustomFrameCallback. Users will want to
 * implement the former of these two callbacks to declare the manufacturer ID
 * and version of their NCP application. The second callback will need to be
 * implemented for the NCP application to process custom EZSP frames
 * coming from the HOST. Using this callback, the message can be processed and
 * the response can be written. Upon return of this function, the custom
 * response will be sent back to the HOST.
 *
 * @{
 */

/** @brief Sends a custom EZSP message.
 *
 * Sends a custom EZSP message payload of length to the HOST.
 *
 * @param length The length of the custom EZSP message.  Ver.: always
 * @param payload The custom EZSP message itself.  Ver.: always
 *
 * @return An ::EmberStatus value describing the result of sending the custom
 * EZSP frame to the HOST.
 */
EmberStatus emberAfPluginXncpSendCustomEzspMessage(uint8_t length, uint8_t *payload);

// @} END addtogroup

#endif /* SILABS_PLUGIN_XNCP_H */
