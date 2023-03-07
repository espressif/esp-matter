/***************************************************************************//**
 * @file
 * @brief It provides APIs to interface between alternate MAC and stack.
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

#ifndef SILABS_ALTERNATE_MAC_H_
#define SILABS_ALTERNATE_MAC_H_

/**
 * @brief The 8-bit identifier to uniquely identify the interface.
 */
typedef uint8_t EmberMacInterfaceId;

/**
 * @brief The page number for Alternate MAC interface.
 */
#define EMBER_ALT_MAC_PAGE            25

/**
 * @brief The Alternate MAC channel mask that can be used during channel
 * operations (such as scans).
 */
#define EMBER_ALT_MAC_CHANNEL_MASK    0x00000001UL

/**
 * @brief Stack uses the predefined radio channel for its radio interface while
 * associating over alternate MAC interface. So when a device joins
 * a zigbee network over alternate MAC, it configure its radio network
 * on this channel.
 */
#define DEFAULT_RADIO_CHANNEL_SELECTED   20

/**
 * @brief Scan duration over alternate MAC interface.
 */
#define EMBER_ALT_MAC_ACTIVE_SCAN_DURATION_IN_MS  500

/**
 * @brief Received packet max buffer size.
 */
#define EMBER_ALT_MAC_RECEIVE_BUFFER_SIZE 125

// Transmit and Receive Zigbee MAC data frame format:
// ------------------------------------------------------------------------------------
// |  Frame Control  |  Seq No |  PanId  |  Dest  |   Src   |        MAC PAYLOAD      |
// ------------------------------------------------------------------------------------
// Alternate MAC implementer should add CRC or something similar if needed
// while transmitting a packet and must pass CRC, PAN ID and destination address
// of a received packet before calling emberAltMacReceiveCallback().

/**
 * @brief Defines MAC transmit complete parameters.
 */
typedef struct {
  /* MAC interface ID */
  EmberMacInterfaceId macInterfaceId;
  /* To identify a last outgoing flat packet */
  uint8_t tag;
  /* Transmitted packet status*/
  EmberStatus status;
} EmberMacTransmitComplete;

/**
 * @brief Defines MAC transmit parameters.
 */
typedef struct {
  /* Length of outgoing flat packet */
  uint8_t length;
  /* Outgoing flat packet */
  const uint8_t* outgoingFlatPacket;
  /* Transmit complete info */
  EmberMacTransmitComplete txCompleteInfo;
} EmberMacTransmit;

/**
 * @brief Defines MAC transmit callback signature to register while
 * initializing the alternate MAC interface.
 */
typedef void (*MacTransmitCallback) (EmberMacTransmit);

/**
 * @brief Defines alternate MAC configuration parameters.
 */
typedef struct {
  /* Scan duration over alternate MAC */
  uint16_t scanDuration;
  /* To register the transmit callback.
   * Called when there is packet to transmit */
  MacTransmitCallback macTransmit;
} EmberAltMacConfig;

/**
 * @brief Defines MAC receive parameters.
 */
typedef struct {
  /* To indicate MAC interface */
  EmberMacInterfaceId macInterfaceId;
  /* Length of received packet */
  uint8_t length;
  /* Received flat packet */
  const uint8_t* incomingFlatPacket;
} EmberMacReceive;

/** @brief Initializes alternate MAC interface for communication.
 *        This should be called from main initialization.
 *
 *  @params *config Pointer to alternate MAC configuration(s).
 *  - ::scanDuration scan duration in milliseconds for alternate MAC.
 *  - ::macTransmit Register the transmit callback. It gets called
 *                  when there is a packet to transmit over alternate
 *                  MAC interface.
 *
 *  @return EmberMacInterfaceId Returns MAC interface Id. This needs
 *                  to provide with other MAC APIs to communicate
 *                  with Zigbee stack.
 *
 *  Note: Assertion happens if
 *          - EmberAltMacConfig or MacTransmitCallback is NULL.
 */
EmberMacInterfaceId emberAltMacInit(const EmberAltMacConfig* config);

/** @brief Informs stack about the status of last transmitted
 *        packet. MacInterfaceId and tag should be matched with
 *        what was provided in MacTransmitCallback. This callback needs
 *        to call on every transmitted packet in order to get next packet
 *        to transmit if any.
 *
 *  @param macTransmitComplete Transmit complete status.
 *  - :: macInterfaceId Alternate MAC interface ID.
 *  - :: tag  Provided tag from transmit callback.
 *  - :: Status Status of last transmitted packet.
 *
 *  @return An ::EmberStatus value
 *  - ::EMBER_SUCESS
 *  - ::EMBER_ERR_FATAL if
 *          - Alternate MAC interface is not initialized.
 *
 *  Note: Assertion happens if
 *          - Alternate MAC interface ID or tag does not match.
 *          - It is being called more than ones for the same packet.
 */
EmberStatus emberAltMacTransmitCompleteCallback(EmberMacTransmitComplete macTransmitComplete);

/** @brief This should call when alternate MAC receives a Zigbee packet
 *         to pass it to the stack.
 *
 *  @params macReceive Received packet information.
 *  - :: macInterfaceId Alternate MAC interface ID.
 *  - :: Length Length of received packet.
 *  - :: *incomingFlatPacket Pointer to received packet. See above
 *                           for the data frame format.
 *
 *  @return An ::EmberStatus value
 *  - ::EMBER_SUCESS
 *  - ::EMBER_ERR_FATAL if
 *          - Alternate MAC interface is not initialized.
 *          - MAC interface Id is not matched with provided during initialization.
 *          - Received packet length is more than EMBER_ALT_MAC_RECEIVE_BUFFER_SIZE.
 *          - There is no buffer left to store a packet.
 *
 *  Note: Assertion happens if
 *          - IncomingFlatPacket pointer is NULL.
 *          - Received packet length in zero or more than EMBER_ALT_MAC_RECEIVE_BUFFER_SIZE.
 */
EmberStatus emberAltMacReceiveCallback(EmberMacReceive macReceive);

#endif /* SILABS_ALTERNATE_MAC_H_ */
