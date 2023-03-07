/***************************************************************************//**
 * @file
 * @brief Return-code definitions for EmberZNet stack API functions.
 *
 * See @ref status_codes for documentation.
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

/**
 * @addtogroup status_codes
 *
 * Many EmberZNet API functions return an ::EmberStatus value to indicate
 * the success or failure of the call.
 * Return codes are one byte long.
 * This page documents the possible status codes and their meanings.
 *
 * See error-def.h for source code.
 *
 * See also error.h for information on how the values for the return codes are
 * built up from these definitions.
 * The file error-def.h is separated from error.h because utilities will use this file
 * to parse the return codes.
 *
 * @note Do not include error-def.h directly. It is included by
 * error.h inside an enumeration typedef, which is in turn included by ember.h.
 *
 * @{
 */

#ifndef SILABS_ERROR_DEF_H
#define SILABS_ERROR_DEF_H

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberStatus
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
/**
 * @name Generic Messages
 * These messages are system wide.
 */
//@{

/**
 * @brief The generic "no error" message.
 */
  EMBER_SUCCESS = 0x00,

/**
 * @brief The generic "fatal error" message.
 */
  EMBER_ERR_FATAL = 0x01,

/**
 * @brief An invalid value was passed as an argument to a function.
 */
  EMBER_BAD_ARGUMENT = 0x02,

/**
 * @brief The requested information was not found.
 */
  EMBER_NOT_FOUND = 0x03,

/**
 * @brief The manufacturing and stack token format in non-volatile memory
 * is different than what the stack expects (returned at initialization).
 */
  EMBER_EEPROM_MFG_STACK_VERSION_MISMATCH = 0x04,

/**
 * @brief The static memory definitions in ember-static-memory.h
 * are incompatible with this stack version.
 */
  EMBER_INCOMPATIBLE_STATIC_MEMORY_DEFINITIONS = 0x05,

/**
 * @brief The manufacturing token format in non-volatile memory is
 * different than what the stack expects (returned at initialization).
 */
  EMBER_EEPROM_MFG_VERSION_MISMATCH = 0x06,

/**
 * @brief The stack token format in non-volatile memory is different
 * than what the stack expects (returned at initialization).
 */
  EMBER_EEPROM_STACK_VERSION_MISMATCH = 0x07,

//@} // END Generic Messages

/**
 * @name Packet Buffer Module Errors
 */
//@{

/**
 * @brief There are no more buffers.
 */
  EMBER_NO_BUFFERS = 0x18,

/**
 * @brief Packet is dropped by packet-handoff callbacks.
 */
  EMBER_PACKET_HANDOFF_DROP_PACKET = 0x19,

//@} / END Packet Buffer Module Errors

/**
 * @name Serial Manager Errors
 */
//@{

/**
 * @brief Specifies an invalid baud rate.
 */
  EMBER_SERIAL_INVALID_BAUD_RATE = 0x20,

/**
 * @brief Specifies an invalid serial port.
 */
  EMBER_SERIAL_INVALID_PORT = 0x21,

/**
 * @brief Tried to send too much data.
 */
  EMBER_SERIAL_TX_OVERFLOW = 0x22,

/**
 * @brief There wasn't enough space to store a received character
 * and the character was dropped.
 */
  EMBER_SERIAL_RX_OVERFLOW = 0x23,

/**
 * @brief Detected a UART framing error.
 */
  EMBER_SERIAL_RX_FRAME_ERROR = 0x24,

/**
 * @brief Detected a UART parity error.
 */
  EMBER_SERIAL_RX_PARITY_ERROR = 0x25,

/**
 * @brief There is no received data to process.
 */
  EMBER_SERIAL_RX_EMPTY = 0x26,

/**
 * @brief The receive interrupt was not handled in time and a
 * character was dropped.
 */
  EMBER_SERIAL_RX_OVERRUN_ERROR = 0x27,

//@}

/**
 * @name MAC Errors
 */
//@{

/**
 * @brief The MAC transmit queue is full.
 */
  EMBER_MAC_TRANSMIT_QUEUE_FULL = 0x39,
// Internal

/**
 * @brief MAC header FCF error on receive.
 */
  EMBER_MAC_UNKNOWN_HEADER_TYPE = 0x3A,

/**
 * @brief MAC ACK header received.
 */
  EMBER_MAC_ACK_HEADER_TYPE = 0x3B,

/**
 * @brief The MAC can't complete this task because it is scanning.
 */
  EMBER_MAC_SCANNING = 0x3D,

/**
 * @brief No pending data exists for a data poll.
 */
  EMBER_MAC_NO_DATA = 0x31,

/**
 * @brief Attempts to scan when joined to a network.
 */
  EMBER_MAC_JOINED_NETWORK = 0x32,

/**
 * @brief Scan duration must be 0 to 14 inclusive. Tried to
 * scan with an incorrect duration value.
 */
  EMBER_MAC_BAD_SCAN_DURATION = 0x33,

/**
 * @brief emberStartScan was called with an incorrect scan type.
 */
  EMBER_MAC_INCORRECT_SCAN_TYPE = 0x34,

/**
 * @brief emberStartScan was called with an invalid channel mask.
 */
  EMBER_MAC_INVALID_CHANNEL_MASK = 0x35,

/**
 * @brief Failed to scan the current channel because
 * the relevant MAC command could not be transmitted.
 */
  EMBER_MAC_COMMAND_TRANSMIT_FAILURE = 0x36,

/**
 * @brief An ACK was expected following the transmission but
 * the MAC level ACK was never received.
 */
  EMBER_MAC_NO_ACK_RECEIVED = 0x40,

/**
 * @brief MAC failed to transmit a message because it could not successfully
 * perform a radio network switch.
 */
  EMBER_MAC_RADIO_NETWORK_SWITCH_FAILED = 0x41,

/**
 * @brief An indirect data message timed out before a poll requested it.
 */
  EMBER_MAC_INDIRECT_TIMEOUT = 0x42,

//@}

/**
 * @name  Simulated EEPROM Errors
 */
//@{

/**
 * @brief The Simulated EEPROM is telling the application that
 * at least one flash page to be erased. The GREEN status means the
 * current page has not filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * The application should call the function ::halSimEepromErasePage()
 * when it can to erase a page.
 */
  EMBER_SIM_EEPROM_ERASE_PAGE_GREEN = 0x43,

/**
 * @brief The Simulated EEPROM is telling the application that
 * at least one flash page must be erased.  The RED status means the
 * current page has filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * Due to the shrinking availability of write space, data could
 * be lost. The application must call the function ::halSimEepromErasePage()
 * as soon as possible to erase a page.
 */
  EMBER_SIM_EEPROM_ERASE_PAGE_RED = 0x44,

/**
 * @brief The Simulated EEPROM has run out of room to write new data
 * and the data trying to be set has been lost. This error code is the
 * result of ignoring the ::SIM_EEPROM_ERASE_PAGE_RED error code.
 *
 * The application must call the function ::halSimEepromErasePage() to make room for
 * any further calls to set a token.
 */
  EMBER_SIM_EEPROM_FULL = 0x45,

//  Errors 46 and 47 are now defined below in the
//    flash error block  (was attempting to prevent renumbering).

/**
 * @brief Attempt 1 to initialize the Simulated EEPROM has failed.
 *
 * This failure means the information already stored in the Flash (or a lack
 * thereof), is fatally incompatible with the token information compiled
 * into the code image being run.
 */
  EMBER_SIM_EEPROM_INIT_1_FAILED = 0x48,

/**
 * @brief Attempt 2 to initialize the Simulated EEPROM has failed.
 *
 * This failure means Attempt 1 failed, and the token system failed to
 * properly reload default tokens and reset the Simulated EEPROM.
 */
  EMBER_SIM_EEPROM_INIT_2_FAILED = 0x49,

/**
 * @brief Attempt 3 to initialize the Simulated EEPROM has failed.
 *
 * This failure means one or both of the tokens ::TOKEN_MFG_NVDATA_VERSION or
 * ::TOKEN_STACK_NVDATA_VERSION were incorrect and the token system failed to
 * properly reload default tokens and reset the Simulated EEPROM.
 */
  EMBER_SIM_EEPROM_INIT_3_FAILED = 0x4A,

/**
 * @brief The Simulated EEPROM is repairing itself.
 *
 * While there's nothing for an app to do when the SimEE is going to
 * repair itself (SimEE has to be fully functional for the rest of the
 * system to work), alert the application to the fact that repair
 * is occurring.  There are debugging scenarios where an app might want
 * to know that repair is happening, such as monitoring frequency.
 * @note  Common situations will trigger an expected repair, such as
 *        using an erased chip or changing token definitions.
 */
  EMBER_SIM_EEPROM_REPAIRING = 0x4D,

//@}

/**
 * @name  Flash Errors
 */
//@{

/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash. The target memory attempting to be programmed is already programmed.
 * The flash write routines were asked to flip a bit from a 0 to 1, which is
 * physically impossible and the write was therefore inhibited. The data in
 * the Flash cannot be trusted after this error.
 */
  EMBER_ERR_FLASH_WRITE_INHIBITED = 0x46,

/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash and the write verification has failed.  Data in the Flash
 * cannot be trusted after this error and it is possible this error is the
 * result of exceeding the life cycles of the Flash.
 */
  EMBER_ERR_FLASH_VERIFY_FAILED = 0x47,

/**
 * @description A fatal error has occurred while trying to write data to the
 * Flash possibly due to write protection or an invalid address. Data in
 * the Flash cannot be trusted after this error and it is possible this error
 * is the result of exceeding the life cycles of the Flash.
 */
  EMBER_ERR_FLASH_PROG_FAIL = 0x4B,

/**
 * @description A fatal error has occurred while trying to erase the Flash possibly
 * due to write protection. Data in the Flash cannot be trusted after
 * this error and it is possible this error is the result of exceeding the
 * life cycles of the Flash.
 */
  EMBER_ERR_FLASH_ERASE_FAIL = 0x4C,

//@}

/**
 * @name  Bootloader Errors
 */
//@{

/**
 * @brief The bootloader received an invalid message (failed attempt
 * to go into bootloader).
 */
  EMBER_ERR_BOOTLOADER_TRAP_TABLE_BAD = 0x58,

/**
 * @brief The bootloader received an invalid message (failed attempt to go
 * into the bootloader).
 */
  EMBER_ERR_BOOTLOADER_TRAP_UNKNOWN = 0x59,

/**
 * @brief The bootloader cannot complete the bootload operation because
 * either an image was not found or the image exceeded memory bounds.
 */
  EMBER_ERR_BOOTLOADER_NO_IMAGE = 0x05A,

//@}

/**
 * @name  Transport Errors
 */
//@{

/**
 * @brief The APS layer attempted to send or deliver a message
 * and failed.
 */
  EMBER_DELIVERY_FAILED = 0x66,

/**
 * @brief This binding index is out of range for the current binding table.
 */
  EMBER_BINDING_INDEX_OUT_OF_RANGE = 0x69,

/**
 * @brief This address table index is out of range for the current
 * address table.
 */
  EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE = 0x6A,

/**
 * @brief An invalid binding table index was given to a function.
 */
  EMBER_INVALID_BINDING_INDEX = 0x6C,

/**
 * @brief The API call is not allowed given the current state of the
 * stack.
 */
  EMBER_INVALID_CALL = 0x70,

/**
 * @brief The link cost to a node is not known.
 */
  EMBER_COST_NOT_KNOWN = 0x71,

/**
 * @brief The maximum number of in-flight messages  = i.e.,
 * ::EMBER_APS_UNICAST_MESSAGE_COUNT, has been reached.
 */
  EMBER_MAX_MESSAGE_LIMIT_REACHED = 0x72,

/**
 * @brief The message to be transmitted is too big to fit into a
 * single over-the-air packet.
 */
  EMBER_MESSAGE_TOO_LONG = 0x74,

/**
 * @brief The application is trying to delete or overwrite a binding
 * that is in use.
 */
  EMBER_BINDING_IS_ACTIVE = 0x75,

/**
 * @brief The application is trying to overwrite an address table entry
 * that is in use.
 */
  EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE = 0x76,

/**
 * @brief An attempt was made to transmit during the suspend period.
 */
  EMBER_TRANSMISSION_SUSPENDED = 0x77,

//@}
//

/**
 * @name  Green Power status codes
 */
//@{

/**
 * @brief Security match.
 */
  EMBER_MATCH = 0x78,
/**
 * @brief Drop frame.
 */
  EMBER_DROP_FRAME = 0x79,
/**
 * @brief
 */
  EMBER_PASS_UNPROCESSED = 0x7A,
/**
 * @brief
 */
  EMBER_TX_THEN_DROP = 0x7B,
/**
 * @brief
 */
  EMBER_NO_SECURITY = 0x7C,
/**
 * @brief
 */
  EMBER_COUNTER_FAILURE = 0x7D,
/**
 * @brief
 */
  EMBER_AUTH_FAILURE = 0x7E,
/**
 * @brief
 */
  EMBER_UNPROCESSED = 0x7F,

//@}
//

/**
 * @name  HAL Module Errors
 */
//@{

/**
 * @brief The conversion is complete.
 */
  EMBER_ADC_CONVERSION_DONE = 0x80,

/**
 * @brief The conversion cannot be done because a request is being
 * processed.
 */
  EMBER_ADC_CONVERSION_BUSY = 0x81,

/**
 * @brief The conversion is deferred until the current request has been
 * processed.
 */
  EMBER_ADC_CONVERSION_DEFERRED = 0x82,

/**
 * @brief No results are pending.
 */
  EMBER_ADC_NO_CONVERSION_PENDING = 0x84,

/**
 * @brief Sleeping (for a duration) has been abnormally interrupted
 * and exited prematurely.
 */
  EMBER_SLEEP_INTERRUPTED = 0x85,

//@}

/**
 * @name  PHY Errors
 */
//@{

/**
 * @brief The transmit attempt failed because the radio scheduler could not find
 * a slot to transmit this packet in or a higher priority event interrupted it.
 */
  EMBER_PHY_TX_SCHED_FAIL = 0x87,

/**
 * @brief The transmit hardware buffer underflowed.
 */
  EMBER_PHY_TX_UNDERFLOW = 0x88,

/**
 * @brief The transmit hardware did not finish transmitting a packet.
 */
  EMBER_PHY_TX_INCOMPLETE = 0x89,

/**
 * @brief An unsupported channel setting was specified.
 */
  EMBER_PHY_INVALID_CHANNEL = 0x8A,

/**
 * @brief An unsupported power setting was specified.
 */
  EMBER_PHY_INVALID_POWER = 0x8B,

/**
 * @brief The requested operation cannot be completed because the radio
 * is currently busy, either transmitting a packet or performing calibration.
 */
  EMBER_PHY_TX_BUSY = 0x8C,

/**
 * @brief The transmit attempt failed because all CCA attempts indicated that
 * the channel was busy.
 */
  EMBER_PHY_TX_CCA_FAIL = 0x8D,

/**
 * @brief The transmit attempt was blocked from going over the air. Typically
 * this is due to the Radio Hold Off (RHO) or Coexistence plugins as they can
 * prevent transmits based on external signals.
 */
  EMBER_PHY_TX_BLOCKED = 0x8E,

/**
 * @brief The expected ACK was received after the last transmission.
 */
  EMBER_PHY_ACK_RECEIVED = 0x8F,

//@}

/**
 * @name  Return Codes Passed to emberStackStatusHandler()
 * See also ::emberStackStatusHandler = ,.
 */
//@{

/**
 * @brief The stack software has completed initialization and is ready
 * to send and receive packets over the air.
 */
  EMBER_NETWORK_UP = 0x90,

/**
 * @brief The network is not operating.
 */
  EMBER_NETWORK_DOWN = 0x91,

/**
 * @brief An attempt to join a network failed.
 */
  EMBER_JOIN_FAILED = 0x94,

/**
 * @brief After moving, a mobile node's attempt to re-establish contact
 * with the network failed.
 */
  EMBER_MOVE_FAILED = 0x96,

/**
 * @brief An attempt to join as a router failed due to a Zigbee
 * versus Zigbee Pro incompatibility.  Zigbee devices joining Zigbee Pro networks
 * (or vice versa) must join as End Devices, not Routers.
 */
  EMBER_CANNOT_JOIN_AS_ROUTER = 0x98,

/** @brief The local node ID has changed. The application can get the new
 *  node ID by calling ::emberGetNodeId().
 */
  EMBER_NODE_ID_CHANGED = 0x99,

/** @brief The local PAN ID has changed. The application can get the new PAN
 *  ID by calling ::emberGetPanId().
 */
  EMBER_PAN_ID_CHANGED = 0x9A,

/** @brief The channel has changed.
 */
  EMBER_CHANNEL_CHANGED = 0x9B,

/** @brief The network has been opened for joining.
 */
  EMBER_NETWORK_OPENED = 0x9C,

/** @brief The network has been closed for joining.
 */
  EMBER_NETWORK_CLOSED = 0x9D,

/** @brief An attempt to join or rejoin the network failed because
 *  no router beacons could be heard by the joining node.
 */
  EMBER_NO_BEACONS = 0xAB,

/** @brief An attempt was made to join a Secured Network using a pre-configured
 *  key, but the Trust Center sent back a Network Key in-the-clear when
 *  an encrypted Network Key was required. (::EMBER_REQUIRE_ENCRYPTED_KEY).
 */
  EMBER_RECEIVED_KEY_IN_THE_CLEAR = 0xAC,

/** @brief An attempt was made to join a Secured Network, but the device did
 *  not receive a Network Key.
 */
  EMBER_NO_NETWORK_KEY_RECEIVED = 0xAD,

/** @brief After a device joined a Secured Network, a Link Key was requested
 *  (::EMBER_GET_LINK_KEY_WHEN_JOINING) but no response was ever received.
 */
  EMBER_NO_LINK_KEY_RECEIVED = 0xAE,

/** @brief An attempt was made to join a Secured Network without a
 *  pre-configured key, but the Trust Center sent encrypted data using a
 *  pre-configured key.
 */
  EMBER_PRECONFIGURED_KEY_REQUIRED = 0xAF,

//@}

/**
 * @name  Security Errors
 */
/**
 * @brief The passed key data is not valid. A key of all zeros or
 *   all F's are reserved values and cannot be used.
 */
  EMBER_KEY_INVALID = 0xB2,

/**
 * @brief The chosen security level (the value of ::EMBER_SECURITY_LEVEL)
 *  is not supported by the stack.
 */
  EMBER_INVALID_SECURITY_LEVEL = 0x95,

/**
 * @brief An error occurred when trying to encrypt at the APS Level.
 *
 * To APS encrypt an outgoing packet, the sender
 * needs to know the EUI64 of the destination. This error occurs because
 * the EUI64 of the destination can't be determined from
 * the short address (no entry in the neighbor, child, binding
 * or address tables).
 *
 * Every time this error code is seen, note that the stack initiates an
 * IEEE address discovery request behind the scenes. Responses
 * to the request are stored in the trust center cache portion of the
 * address table. Note that you need at least 1 entry allocated for
 * TC cache in the address table plugin. Depending on the available rows in
 * the table, newly discovered addresses may replace old ones. The address
 * table plugin is enabled by default on the host. If you are using an SoC
 * platform, please be sure to add the address table plugin.
 *
 * When customers choose to send APS messages by using short addresses,
 * they should incorporate a retry mechanism and try again, no sooner than
 * 2 seconds later, to resend the APS message. If the app always
 * receives 0xBE (EMBER_IEEE_ADDR_DISCOVERY_IN_PROGRESS) after
 * multiple retries, that might indicate that:
 * a) destination node is not on the network
 * b) there are problems with the health of the network
 * c) there may not be any space set aside in the address table for
 *    the newly discovered address - this can be rectified by reserving
 *    more entries for the trust center cache in the address table plugin
 */
  EMBER_IEEE_ADDRESS_DISCOVERY_IN_PROGRESS = 0xBE,
/**
 * @brief An error occurred when trying to encrypt at the APS Level.
 *
 * This error occurs either because the long address of the recipient can't be
 * determined from the short address (no entry in the binding table)
 * or there is no link key entry in the table associated with the destination,
 * or there was a failure to load the correct key into the encryption core.
 */
  EMBER_APS_ENCRYPTION_ERROR = 0xA6,

/** @brief There was an attempt to form or join a network with security
 * without calling ::emberSetInitialSecurityState() first.
 */
  EMBER_SECURITY_STATE_NOT_SET = 0xA8,

/** @brief There was an attempt to set an entry in the key table using
 *   an invalid long address. Invalid addresses include:
 *    - The local device's IEEE address
 *    - Trust Center's IEEE address
 *    - An existing table entry's IEEE address
 *    - An address consisting of all zeros or all F's
 */
  EMBER_KEY_TABLE_INVALID_ADDRESS = 0xB3,

/** @brief There was an attempt to set a security configuration that
 *   is not valid given the other security settings.
 */
  EMBER_SECURITY_CONFIGURATION_INVALID = 0xB7,

/** @brief There was an attempt to broadcast a key switch too quickly after
 *    broadcasting the next network key. The Trust Center must wait at
 *    least a period equal to the broadcast timeout so that all routers
 *    have a chance to receive the broadcast of the new network key.
 */
  EMBER_TOO_SOON_FOR_SWITCH_KEY = 0xB8,

/** @brief The received signature corresponding to the message that was passed
    to the CBKE Library failed verification and is not valid.
 */
  EMBER_SIGNATURE_VERIFY_FAILURE = 0xB9,

/** @brief The message could not be sent because the link key corresponding
    to the destination is not authorized for use in APS data messages.
    APS Commands (sent by the stack) are allowed. To use it for encryption
    of APS data messages it must be authorized using a key agreement protocol
    (such as CBKE).
 */
  EMBER_KEY_NOT_AUTHORIZED = 0xBB,

/** @brief The security data provided was not valid, or an
 *    integrity check failed.
 */
  EMBER_SECURITY_DATA_INVALID = 0xBD,

//@}

/**
 * @name  Miscellaneous Network Errors
 */
//@{

/**
 * @brief The node has not joined a network.
 */
  EMBER_NOT_JOINED = 0x93,

/**
 * @brief A message cannot be sent because the network is currently
 *  overloaded.
 */
  EMBER_NETWORK_BUSY = 0xA1,

/**
 * @brief The application tried to send a message using an
 * endpoint that it has not defined.
 */
  EMBER_INVALID_ENDPOINT = 0xA3,

/**
 * @brief The application tried to use a binding that has been remotely
 * modified and the change has not yet been reported to the application.
 */
  EMBER_BINDING_HAS_CHANGED = 0xA4,

/**
 * @brief An attempt to generate random bytes failed because of
 * insufficient random data from the radio.
 */
  EMBER_INSUFFICIENT_RANDOM_DATA = 0xA5,

/** A Zigbee route error command frame was received indicating
 * that a source routed message from this node failed en route.
 */
  EMBER_SOURCE_ROUTE_FAILURE = 0xA9,

/** A Zigbee route error command frame was received indicating
 * that a message sent to this node along a many-to-one route
 * failed en route. The route error frame was delivered by
 * an ad-hoc search for a functioning route.
 */
  EMBER_MANY_TO_ONE_ROUTE_FAILURE = 0xAA,

//@}

/**
 * @name  Miscellaneous Utility Errors
 */
//@{

/**
 * @brief A critical and fatal error indicating that the version of the
 * stack trying to run does not match with the chip it's running on. The
 * software (stack) on the chip must be replaced with software
 * compatible with the chip.
 */
  EMBER_STACK_AND_HARDWARE_MISMATCH = 0xB0,

/**
 * @brief An index was passed into the function that was larger
 * than the valid range.
 */
  EMBER_INDEX_OUT_OF_RANGE = 0xB1,

/**
 * @brief There are no empty entries left in the table.
 */
  EMBER_TABLE_FULL = 0xB4,

/**
 * @brief The requested table entry has been erased and contains
 *   no valid data.
 */
  EMBER_TABLE_ENTRY_ERASED = 0xB6,

/**
 * @brief The requested function cannot be executed because
 *   the library that contains the necessary functionality is not present.
 */
  EMBER_LIBRARY_NOT_PRESENT = 0xB5,

/**
 * @brief The stack accepted the command and is currently processing
 * the request.  The results will be returned via an appropriate handler.
 */
  EMBER_OPERATION_IN_PROGRESS = 0xBA,

/**
 * @brief The EUI of the Trust center has changed due to a successful rejoin.
 *   The device may need to perform other authentication to verify the new TC
 *   is authorized to take over.
 */
  EMBER_TRUST_CENTER_EUI_HAS_CHANGED = 0xBC,

//@}

/**
 * @name  NVM3 Token Errors
 */
//@{

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as no valid NVM3 page was found.
 */
  EMBER_NVM3_TOKEN_NO_VALID_PAGES = 0xC0,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance was already opened with other parameters.
 */
  EMBER_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS = 0xC1,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance is not aligned properly in memory.
 */
  EMBER_NVM3_ERR_ALIGNMENT_INVALID = 0xC2,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the size of the NVM3 instance is too small.
 */
  EMBER_NVM3_ERR_SIZE_TOO_SMALL = 0xC3,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *    as the NVM3 page size is not supported.
 */
  EMBER_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED = 0xC4,

/**
 * @brief NVM3 is telling the application that there was an error initializing
 * some of the tokens.
 */
  EMBER_NVM3_ERR_TOKEN_INIT = 0xC5,

/**
 * @brief NVM3 is telling the application there has been an error when
 * attempting to upgrade SimEE tokens.
 */
  EMBER_NVM3_ERR_UPGRADE = 0xC6,

/**
 * @brief NVM3 is telling the application that there has been an unknown
 * error.
 */
  EMBER_NVM3_ERR_UNKNOWN = 0xC7,

//@}

/**
 * @name  Application Errors
 * These error codes are available for application use.
 */
//@{

/**
 * @brief This error is reserved for customer application use.
 *  This will never be returned from any portion of the network stack or HAL.
 */
  EMBER_APPLICATION_ERROR_0 = 0xF0,
  EMBER_APPLICATION_ERROR_1 = 0xF1,
  EMBER_APPLICATION_ERROR_2 = 0xF2,
  EMBER_APPLICATION_ERROR_3 = 0xF3,
  EMBER_APPLICATION_ERROR_4 = 0xF4,
  EMBER_APPLICATION_ERROR_5 = 0xF5,
  EMBER_APPLICATION_ERROR_6 = 0xF6,
  EMBER_APPLICATION_ERROR_7 = 0xF7,
  EMBER_APPLICATION_ERROR_8 = 0xF8,
  EMBER_APPLICATION_ERROR_9 = 0xF9,
  EMBER_APPLICATION_ERROR_10 = 0xFA,
  EMBER_APPLICATION_ERROR_11 = 0xFB,
  EMBER_APPLICATION_ERROR_12 = 0xFC,
  EMBER_APPLICATION_ERROR_13 = 0xFD,
  EMBER_APPLICATION_ERROR_14 = 0xFE,
  EMBER_APPLICATION_ERROR_15 = 0xFF,
};

//@} // END name group

/** @} END addtogroup */

#endif // SILABS_ERROR_DEF_H
