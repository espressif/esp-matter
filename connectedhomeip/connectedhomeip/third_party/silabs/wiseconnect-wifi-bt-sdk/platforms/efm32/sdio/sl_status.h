/**************************************************************************//**
 * Copyright 2018, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_STATUS_H
#define SL_STATUS_H

#include <stdint.h>

/*******************************************************************************
 ***************************   SPACE DEFINES   *********************************
 ******************************************************************************/

#define SL_STATUS_SPACE_MASK              ((sl_status_t)0xFF00)

#define SL_STATUS_GENERIC_SPACE           ((sl_status_t)0x0000)
#define SL_STATUS_WIFI_SPACE              ((sl_status_t)0x0B00)

/*******************************************************************************
 ***************************   STATUS DEFINES   ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************   GENERIC ERRORS   ********************************
 ******************************************************************************/

#define SL_STATUS_OK    ((sl_status_t)0x0000)  ///< No error.
#define SL_STATUS_FAIL  ((sl_status_t)0x0001)  ///< Generic error.

// State Errors
#define SL_STATUS_INVALID_STATE         ((sl_status_t)0x0002)  ///< Generic invalid state error.
#define SL_STATUS_NOT_READY             ((sl_status_t)0x0003)  ///< Module is not ready for requested operation.
#define SL_STATUS_BUSY                  ((sl_status_t)0x0004)  ///< Module is busy and cannot carry out requested operation.
#define SL_STATUS_IN_PROGRESS           ((sl_status_t)0x0005)  ///< Operation is in progress and not yet complete (pass or fail).
#define SL_STATUS_ABORT                 ((sl_status_t)0x0006)  ///< Operation aborted.
#define SL_STATUS_TIMEOUT               ((sl_status_t)0x0007)  ///< Operation timed out.
#define SL_STATUS_PERMISSION            ((sl_status_t)0x0008)  ///< Operation not allowed per permissions.
#define SL_STATUS_WOULD_BLOCK           ((sl_status_t)0x0009)  ///< Non-blocking operation would block.
#define SL_STATUS_IDLE                  ((sl_status_t)0x000A)  ///< Operation/module is Idle, cannot carry requested operation.
#define SL_STATUS_IS_WAITING            ((sl_status_t)0x000B)  ///< Operation cannot be done while construct is waiting.
#define SL_STATUS_NONE_WAITING          ((sl_status_t)0x000C)  ///< No task/construct waiting/pending for that action/event.
#define SL_STATUS_SUSPENDED             ((sl_status_t)0x000D)  ///< Operation cannot be done while construct is suspended.
#define SL_STATUS_NOT_AVAILABLE         ((sl_status_t)0x000E)  ///< Feature not available due to software configuration.
#define SL_STATUS_NOT_SUPPORTED         ((sl_status_t)0x000F)  ///< Feature not supported.
#define SL_STATUS_INITIALIZATION        ((sl_status_t)0x0010)  ///< Initialization failed.
#define SL_STATUS_NOT_INITIALIZED       ((sl_status_t)0x0011)  ///< Module has not been initialized.
#define SL_STATUS_ALREADY_INITIALIZED   ((sl_status_t)0x0012)  ///< Module has already been initialized.
#define SL_STATUS_DELETED               ((sl_status_t)0x0013)  ///< Object/construct has been deleted.
#define SL_STATUS_ISR                   ((sl_status_t)0x0014)  ///< Illegal call from ISR.
#define SL_STATUS_NETWORK_UP            ((sl_status_t)0x0015)  ///< Illegal call because network is up.
#define SL_STATUS_NETWORK_DOWN          ((sl_status_t)0x0016)  ///< Illegal call because network is down.
#define SL_STATUS_NOT_JOINED            ((sl_status_t)0x0017)  ///< Failure due to not being joined in a network.
#define SL_STATUS_NO_BEACONS            ((sl_status_t)0x0018)  ///< Invalid operation as there are no beacons.

// Allocation/ownership Errors
#define SL_STATUS_ALLOCATION_FAILED   ((sl_status_t)0x0019)  ///< Generic allocation error.
#define SL_STATUS_NO_MORE_RESOURCE    ((sl_status_t)0x001A)  ///< No more resource available to perform the operation.
#define SL_STATUS_EMPTY               ((sl_status_t)0x001B)  ///< Item/list/queue is empty.
#define SL_STATUS_FULL                ((sl_status_t)0x001C)  ///< Item/list/queue is full.
#define SL_STATUS_WOULD_OVERFLOW      ((sl_status_t)0x001D)  ///< Item would overflow.
#define SL_STATUS_HAS_OVERFLOWED      ((sl_status_t)0x001E)  ///< Item/list/queue has been overflowed.
#define SL_STATUS_OWNERSHIP           ((sl_status_t)0x001F)  ///< Generic ownership error.
#define SL_STATUS_IS_OWNER            ((sl_status_t)0x0020)  ///< Already/still owning resource.

// Invalid Parameters Errors
#define SL_STATUS_INVALID_PARAMETER       ((sl_status_t)0x0021)  ///< Generic invalid argument or consequence of invalid argument.
#define SL_STATUS_NULL_POINTER            ((sl_status_t)0x0022)  ///< Invalid null pointer received as argument.
#define SL_STATUS_INVALID_CONFIGURATION   ((sl_status_t)0x0023)  ///< Invalid configuration provided.
#define SL_STATUS_INVALID_MODE            ((sl_status_t)0x0024)  ///< Invalid mode.
#define SL_STATUS_INVALID_HANDLE          ((sl_status_t)0x0025)  ///< Invalid handle.
#define SL_STATUS_INVALID_TYPE            ((sl_status_t)0x0026)  ///< Invalid type for operation.
#define SL_STATUS_INVALID_INDEX           ((sl_status_t)0x0027)  ///< Invalid index.
#define SL_STATUS_INVALID_RANGE           ((sl_status_t)0x0028)  ///< Invalid range.
#define SL_STATUS_INVALID_KEY             ((sl_status_t)0x0029)  ///< Invalid key.
#define SL_STATUS_INVALID_CREDENTIALS     ((sl_status_t)0x002A)  ///< Invalid credentials.
#define SL_STATUS_INVALID_COUNT           ((sl_status_t)0x002B)  ///< Invalid count.
#define SL_STATUS_NOT_FOUND               ((sl_status_t)0x002C)  ///< Item could not be found.
#define SL_STATUS_ALREADY_EXISTS          ((sl_status_t)0x002D)  ///< Item already exists.

// IO/Communication Errors
#define SL_STATUS_IO                    ((sl_status_t)0x002E)  ///< Generic I/O failure.
#define SL_STATUS_IO_TIMEOUT            ((sl_status_t)0x002F)  ///< I/O failure due to timeout.
#define SL_STATUS_TRANSMIT              ((sl_status_t)0x0030)  ///< Generic transmission error.
#define SL_STATUS_TRANSMIT_UNDERFLOW    ((sl_status_t)0x0031)  ///< Transmit underflowed.
#define SL_STATUS_TRANSMIT_INCOMPLETE   ((sl_status_t)0x0032)  ///< Transmit is incomplete.
#define SL_STATUS_TRANSMIT_BUSY         ((sl_status_t)0x0033)  ///< Transmit is busy.
#define SL_STATUS_RECEIVE               ((sl_status_t)0x0034)  ///< Generic reception error.
#define SL_STATUS_OBJECT_READ           ((sl_status_t)0x0035)  ///< Failed to read on/via given object.
#define SL_STATUS_OBJECT_WRITE          ((sl_status_t)0x0036)  ///< Failed to write on/via given object.
#define SL_STATUS_MESSAGE_TOO_LONG      ((sl_status_t)0x0037)  ///< Message is too long.

// EEPROM/Flash Errors
#define SL_STATUS_EEPROM_MFG_VERSION_MISMATCH     ((sl_status_t)0x0038)  ///<
#define SL_STATUS_EEPROM_STACK_VERSION_MISMATCH   ((sl_status_t)0x0039)  ///<
#define SL_STATUS_FLASH_WRITE_INHIBITED           ((sl_status_t)0x003A)  ///< Flash write is inhibited.
#define SL_STATUS_FLASH_VERIFY_FAILED             ((sl_status_t)0x003B)  ///< Flash verification failed.
#define SL_STATUS_FLASH_PROGRAM_FAILED            ((sl_status_t)0x003C)  ///< Flash programming failed.
#define SL_STATUS_FLASH_ERASE_FAILED              ((sl_status_t)0x003D)  ///< Flash erase failed.

// MAC Errors
#define SL_STATUS_MAC_NO_DATA                   ((sl_status_t)0x003E)  ///<
#define SL_STATUS_MAC_NO_ACK_RECEIVED           ((sl_status_t)0x003F)  ///<
#define SL_STATUS_MAC_INDIRECT_TIMEOUT          ((sl_status_t)0x0040)  ///<
#define SL_STATUS_MAC_UNKNOWN_HEADER_TYPE       ((sl_status_t)0x0041)  ///<
#define SL_STATUS_MAC_ACK_HEADER_TYPE           ((sl_status_t)0x0042)  ///<
#define SL_STATUS_MAC_COMMAND_TRANSMIT_FAILURE  ((sl_status_t)0x0043)  ///<

// CLI_STORAGE Errors
#define SL_STATUS_CLI_STORAGE_NVM_OPEN_ERROR    ((sl_status_t)0x0044)  ///< Error in open NVM

// Security status codes
#define SL_STATUS_SECURITY_IMAGE_CHECKSUM_ERROR ((sl_status_t)0x0045)  ///< Image checksum is not valid.
#define SL_STATUS_SECURITY_DECRYPT_ERROR        ((sl_status_t)0x0046)  ///< Decryption failed

// Command status codes
#define SL_STATUS_COMMAND_IS_INVALID            ((sl_status_t)0x0047)  ///< Command was not recognized
#define SL_STATUS_COMMAND_TOO_LONG              ((sl_status_t)0x0048)  ///< Command maximum length exceeded
#define SL_STATUS_COMMAND_INCOMPLETE            ((sl_status_t)0x0049)  ///< Data received does not form a complete command

/*******************************************************************************
 ********************************   WIFI ERRORS   ******************************
 ******************************************************************************/

#define SL_STATUS_WIFI_INVALID_KEY                         ((sl_status_t)0x0B01)  ///< Invalid firmware keyset
#define SL_STATUS_WIFI_FIRMWARE_DOWNLOAD_TIMEOUT           ((sl_status_t)0x0B02)  ///< The firmware download took too long
#define SL_STATUS_WIFI_UNSUPPORTED_MESSAGE_ID              ((sl_status_t)0x0B03)  ///< Unknown request ID or wrong interface ID used
#define SL_STATUS_WIFI_WARNING                             ((sl_status_t)0x0B04)  ///< The request is successful but some parameters have been ignored
#define SL_STATUS_WIFI_NO_PACKET_TO_RECEIVE                ((sl_status_t)0x0B05)  ///< No Packets waiting to be received
#define SL_STATUS_WIFI_SLEEP_GRANTED                       ((sl_status_t)0x0B08)  ///< The sleep mode is granted
#define SL_STATUS_WIFI_SLEEP_NOT_GRANTED                   ((sl_status_t)0x0B09)  ///< The WFx does not go back to sleep
#define SL_STATUS_WIFI_SECURE_LINK_MAC_KEY_ERROR           ((sl_status_t)0x0B10)  ///< The SecureLink MAC key was not found
#define SL_STATUS_WIFI_SECURE_LINK_MAC_KEY_ALREADY_BURNED  ((sl_status_t)0x0B11)  ///< The SecureLink MAC key is already installed in OTP
#define SL_STATUS_WIFI_SECURE_LINK_RAM_MODE_NOT_ALLOWED    ((sl_status_t)0x0B12)  ///< The SecureLink MAC key cannot be installed in RAM
#define SL_STATUS_WIFI_SECURE_LINK_FAILED_UNKNOWN_MODE     ((sl_status_t)0x0B13)  ///< The SecureLink MAC key installation failed
#define SL_STATUS_WIFI_SECURE_LINK_EXCHANGE_FAILED         ((sl_status_t)0x0B14)  ///< SecureLink key (re)negotiation failed
#define SL_STATUS_WIFI_WRONG_STATE                         ((sl_status_t)0x0B18)  ///< The device is in an inappropriate state to perform the request
#define SL_STATUS_WIFI_CHANNEL_NOT_ALLOWED                 ((sl_status_t)0x0B19)  ///< The request failed due to regulatory limitations
#define SL_STATUS_WIFI_NO_MATCHING_AP                      ((sl_status_t)0x0B1A)  ///< The connection request failed because no suitable AP was found
#define SL_STATUS_WIFI_CONNECTION_ABORTED                  ((sl_status_t)0x0B1B)  ///< The connection request was aborted by host
#define SL_STATUS_WIFI_CONNECTION_TIMEOUT                  ((sl_status_t)0x0B1C)  ///< The connection request failed because of a timeout
#define SL_STATUS_WIFI_CONNECTION_REJECTED_BY_AP           ((sl_status_t)0x0B1D)  ///< The connection request failed because the AP rejected the device
#define SL_STATUS_WIFI_CONNECTION_AUTH_FAILURE             ((sl_status_t)0x0B1E)  ///< The connection request failed because the WPA handshake did not complete successfully
#define SL_STATUS_WIFI_RETRY_EXCEEDED                      ((sl_status_t)0x0B1F)  ///< The request failed because the retry limit was exceeded
#define SL_STATUS_WIFI_TX_LIFETIME_EXCEEDED                ((sl_status_t)0x0B20)  ///< The request failed because the MSDU life time was exceeded

/*******************************************************************************
 ********************************   DATA TYPES   *******************************
 ******************************************************************************/

typedef uint32_t sl_status_t;

/*******************************************************************************
 ********************************   FUNCTIONS   ********************************
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// Placeholder for future string functions.

#ifdef __cplusplus
}
#endif

#endif /* SL_STATUS_H */