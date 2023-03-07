/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/**
 * @addtogroup status_codes
 * @brief Return-code definitions for Connect stack API functions.
 *
 * Many Connect API functions return an ::EmberStatus value to indicate
 * the success or failure of the call.
 *
 * Return codes are one byte long.
 *
 * This page documents the possible status codes and their meanings.
 *
 * See error-def.h for source code.
 *
 * See also error.h for information on how the values for the return codes are
 * built up from these definitions.
 * The file error-def.h is separated from error.h because utilities will use
 * this file to parse the return codes.
 *
 * @note Do not include error-def.h directly. It is included by
 * error.h inside an enum typedef, which is in turn included by ember.h.
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
 * @brief The manufacturing and stack token format in non-volatile memory
 * is different than what the stack expects (returned at initialization).
 */
  EMBER_EEPROM_MFG_STACK_VERSION_MISMATCH = 0x04,

/**
 * @brief The API call is not allowed given the current state of the
 * stack.
 */
  EMBER_INVALID_CALL = 0x70,

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
 * @brief There are no more buffers (either in the stack heap or the queue used
 * by the associated module, such as indirect queue).
 */
  EMBER_NO_BUFFERS = 0x18,

//@} / END Packet Buffer Module Errors

/**
 * @name Serial Manager Errors
 */
//@{

/**
 * @brief Specified an invalid baud rate.
 */
  EMBER_SERIAL_INVALID_BAUD_RATE = 0x20,

/**
 * @brief Specified an invalid serial port.
 */
  EMBER_SERIAL_INVALID_PORT = 0x21,

/**
 * @brief Tried to send too much data.
 */
  EMBER_SERIAL_TX_OVERFLOW = 0x22,

/**
 * @brief There was not enough space to store a received character
 * and some characters were dropped.
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
 * @brief The receive interrupt was not handled in time and some
 * characters were dropped.
 */
  EMBER_SERIAL_RX_OVERRUN_ERROR = 0x27,

//@}

/**
 * @name MAC Errors
 */
//@{

/**
 * @brief No pending data exists for device doing a data poll.
 */
  EMBER_MAC_NO_DATA = 0x31,

/**
 * @brief The frequency hopping client failed the frequency hopping
 * synchronization procedure. It timed out trying to reach the frequency hopping
 * server.
 */
  EMBER_MAC_SYNC_TIMEOUT = 0x33,

/**
 * @brief The frequency hopping client failed the frequency hopping
 * synchronization procedure. The server is currently using a different seed.
 */
  EMBER_MAC_SYNC_WRONG_SEED = 0x34,

/**
 * @brief MAC security operation failed.
 */
  EMBER_MAC_SECURITY_FAILED = 0x35,

/**
 * @brief Transmission failed: the destination node does not appear in the
 * neighbor or child tables.
 */
  EMBER_MAC_UNKNOWN_DESTINATION = 0x37,

/**
 * @brief Transmission failed: the local node does not support security or a
 * secured transmission has been requested to a child that does not support
 * security.
 */
  EMBER_MAC_SECURITY_NOT_SUPPORTED = 0x38,

/**
 * @brief The MAC transmit queue is full.
 */
  EMBER_MAC_TRANSMIT_QUEUE_FULL = 0x39,
// Internal

/**
 * @brief MAC ACK header received.
 */
  EMBER_MAC_ACK_HEADER_TYPE = 0x3B,

/**
 * @brief The MAC can't complete this task because it is scanning.
 */
  EMBER_MAC_SCANNING = 0x3D,

/**
 * @brief The requested operation cannot be completed because MAC is
 * currently busy performing a high-priority task.
 */
  EMBER_MAC_BUSY = 0x3E,

/**
 * @brief Expected to receive an ACK following the transmission, but
 * the MAC level ACK was never received.
 */
  EMBER_MAC_NO_ACK_RECEIVED = 0x40,

/**
 * @brief Indirect data message timed out before polled.
 */
  EMBER_MAC_INDIRECT_TIMEOUT = 0x41,

/**
 * @brief Transmission failed: the indirect message was purged because the
 * destination child has been removed or updated, or because
 * @ref emberPurgeIndirectMessages() was called.
 */
  EMBER_MAC_INDIRECT_MESSAGE_PURGED = 0x42,

//@}

/**
 * @name  Simulated EEPROM Errors
 */
//@{

/**
 * @brief The Simulated EEPROM is telling the application that there
 * is at least one flash page to be erased.  The GREEN status means the
 * current page has not filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * The application should call the function ::halSimEepromErasePage() when it
 * can to erase a page.
 *
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_SIM_EEPROM_ERASE_PAGE_GREEN = 0x43,

/**
 * @brief The Simulated EEPROM is telling the application that there
 * is at least one flash page to be erased. The RED status means the
 * current page has filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * Because of the shrinking availability of write space, there is a danger of
 * data loss.  The application must call the function ::halSimEepromErasePage()
 * as soon as possible to erase a page.
 *
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_SIM_EEPROM_ERASE_PAGE_RED = 0x44,

/**
 * @brief The Simulated EEPROM has run out of room to write any new data
 * and the data trying to be set has been lost. This error code is the
 * result of ignoring the ::EMBER_SIM_EEPROM_ERASE_PAGE_RED error code.
 *
 * The application must call the function ::halSimEepromErasePage() to make room
 * for any further calls to set a token.
 *
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_SIM_EEPROM_FULL = 0x45,

//  Errors 46 and 47 are now defined below in the
//    flash error block (was attempting to prevent renumbering)

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
 * system to work), alert the application to the fact that repairing
 * is occurring.  There are debugging scenarios where an app might want
 * to know that repairing is happening; such as monitoring frequency.
 * @note  Common situations will trigger an expected repair, such as
 *        using an erased chip or changing token definitions.
 *
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_SIM_EEPROM_REPAIRING = 0x4D,

//@}

/**
 * @name  Flash Errors
 */
//@{

/**
 * @brief A fatal error has occurred while trying to write data to the
 * flash.  The target memory attempting to be programmed is already programmed.
 * The flash write routines were asked to flip a bit from a 0 to 1, which is
 * physically impossible and the write was therefore inhibited.  The data in
 * the flash cannot be trusted after this error.
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_ERR_FLASH_WRITE_INHIBITED = 0x46,

/**
 * @brief A fatal error has occurred while trying to write data to the
 * flash and the write verification has failed.  The data in the flash
 * cannot be trusted after this error, and it is possible this error is the
 * result of exceeding the life cycles of the flash.
 * @warning This status is generated in @ref halSimEepromCallback(), and handled
 * correctly in the default implementation.
 */
  EMBER_ERR_FLASH_VERIFY_FAILED = 0x47,

/**
 * @brief A fatal error has occurred while trying to write data to the
 * flash, possibly due to write protection or an invalid address.  The data in
 * the flash cannot be trusted after this error, and it is possible this error
 * is the result of exceeding the life cycles of the flash.
 */
  EMBER_ERR_FLASH_PROG_FAIL = 0x4B,

//@}

/**
 * @name  Transport Errors
 */
//@{

/**
 * @brief The message to be transmitted is too big to fit into a
 * single over-the-air packet.
 */
  EMBER_MESSAGE_TOO_LONG = 0x74,

//@}

/**
 * @name  HAL Module Errors
 */
//@{

/**
 * @brief Conversion is complete.
 */
  EMBER_ADC_CONVERSION_DONE = 0x80,

/**
 * @brief Conversion cannot be done because a request is being
 * processed.
 */
  EMBER_ADC_CONVERSION_BUSY = 0x81,

/**
 * @brief Conversion is deferred until the current request has been
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
 * @brief The expected ACK was received after the last transmission.
 */
  EMBER_PHY_ACK_RECEIVED = 0x8F,

//@}

/**
 * @name  Return Codes Passed to emberStackStatusHandler()
 * See also ::emberStackStatusHandler().
 */
//@{

/**
 * @brief The stack software has completed initialization required to be in a
 * network and is ready to send and receive packets over the air.
 * @sa emberStackStatusHandler()
 */
  EMBER_NETWORK_UP = 0x90,

/**
 * @brief The network is not operating.
 * @sa emberStackStatusHandler()
 */
  EMBER_NETWORK_DOWN = 0x91,

/**
 * @brief The node failed to initiate the scanning process during the joining
 * process.
 * @sa emberStackStatusHandler()
 */
  EMBER_JOIN_SCAN_FAILED = 0x92,

/**
 * @brief An attempt to join a network failed.
 * @sa emberStackStatusHandler()
 */
  EMBER_JOIN_FAILED = 0x94,

/**
 * @brief An attempt to join a network was rejected.
 * @sa emberStackStatusHandler()
 */
  EMBER_JOIN_DENIED = 0x95,

/**
 * @brief The node timed out waiting for a response during the joining process.
 * @sa emberStackStatusHandler()
 */
  EMBER_JOIN_TIMEOUT = 0x96,

/** @brief An attempt to join or rejoin the network failed because
 *  no valid beacons was received by the joining node.
 * @sa emberStackStatusHandler()
 */
  EMBER_NO_VALID_BEACONS = 0xAB,

//@}

/**
 * @name  Miscellaneous Network Errors
 */
//@{

/** @brief The security data provided was not valid, or an
 *    integrity check failed.
 */
  EMBER_SECURITY_DATA_INVALID = 0xBD,

/**
 * @brief The node has not joined a network. Returned by @ref emberNetworkInit()
 * if there was no connection data saved in tokens.
 */
  EMBER_NOT_JOINED = 0x93,

//@}

/**
 * @name  Miscellaneous Utility Errors
 */
//@{

/**
 * @brief There are no empty entries left in the table.
 */
  EMBER_TABLE_FULL = 0xB4,

/**
 * @brief The requested function cannot be executed because
 * the library (plugin) that contains the necessary functionality is not present.
 */
  EMBER_LIBRARY_NOT_PRESENT = 0xB5,

/**
 * @brief The requested NodeId has not been found in the child list or
 * grandchildren list (start network only).
 */
  EMBER_CHILD_NOT_FOUND = 0xB6,

//@}

/**
 * @name  NVM3 Token Errors
 */
//@{

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
