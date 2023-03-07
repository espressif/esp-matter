/***************************************************************************//**
 * @file
 * @brief See @ref znet_bootload for documentation.
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

#ifndef SILABS_BOOTLOAD_H
#define SILABS_BOOTLOAD_H

/**
 * @addtogroup znet_bootload
 * @brief EmberZNet bootload API.
 *
 * See bootload.h for source code.
 * @{
 */

/** @brief Transmit the given bootload message to a neighboring
 * node using a specific 802.15.4 header that allows the EmberZNet stack
 * and the Ember bootloader to recognize the message but will not
 * interfere with other Zigbee stacks.
 *
 * @param broadcast  If true, the destination address and PAN ID are
 *     both set to the broadcast address.
 * @param destEui64  The EUI64 of the target node. Ignored if
 *     the broadcast field is set to true.
 * @param message    The bootloader message to send.
 *
 * @return ::EMBER_SUCCESS if the message was successfully submitted to
 * the transmit queue, and ::EMBER_ERR_FATAL otherwise.
 */
EmberStatus emberSendBootloadMessage(bool broadcast,
                                     EmberEUI64 destEui64,
                                     EmberMessageBuffer message);

/**@brief Invoked by the EmberZNet stack when a
 * bootload message is received. If the application includes
 * ::emberIncomingBootloadMessageHandler(),
 * it must define EMBER_APPLICATION_HAS_BOOTLOAD_HANDLERS in its
 * CONFIGURATION_HEADER.
 *
 * @param longId   The EUI64 of the sending node.
 * @param message  The bootload message that was sent.
 */
void emberIncomingBootloadMessageHandler(EmberEUI64 longId,
                                         EmberMessageBuffer message);

/**@brief Invoked by the EmberZNet stack when the
 * MAC has finished transmitting a bootload message. If the
 * application includes this callback, it must define
 * EMBER_APPLICATION_HAS_BOOTLOAD_HANDLERS in its CONFIGURATION_HEADER.
 *
 * @param message  The message that was sent.
 * @param status   ::EMBER_SUCCESS if the transmission was successful,
 *    or ::EMBER_DELIVERY_FAILED if not.
 */
void emberBootloadTransmitCompleteHandler(EmberMessageBuffer message,
                                          EmberStatus status);

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file bootload.h is new and is described in @ref znet_bootload.
 *
 * - <b>Items moved from ember.h</b>
 *   - emberBootloadTransmitCompleteHandler()
 *   - emberIncomingBootloadMessageHandler()
 *   - emberSendBootloadMessage()
 *   .
 * .
 * HIDDEN -->
 */

#endif // SILABS_BOOTLOAD_H
