/******************************************************************************

 @file  hci.c

 @brief This file contains the Host Controller Interface (HCI) API.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "hci_tl.h"
#include "hci_data.h"
#include "hci_event.h"
#include "hci.h"

#include "ll_ae.h"

#if defined( CC26XX ) || defined( CC13XX )
#include "ll_config.h"
#endif // CC26XX/CC13XX

#include "rom_jt.h"
#include "ecc_rom.h"

#include <ti/drivers/rf/RF.h>
#include "rf_api.h"
//
extern RF_Handle rfHandle;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// HCI Version and Revision
#if defined( CC26XX ) || defined( CC13XX )
  #if defined(BLE_V50_FEATURES)
  #ifdef QUAL_TEST
    #define HCI_VERSION                              0x0A    // BT Core Specification V5.1
  #else
    #define HCI_VERSION                              0x09    // BT Core Specification V5.0
  #endif
  #elif defined(CTRL_V42_CONFIG)
    #define HCI_VERSION                              0x08    // BT Core Specification V4.2
  #elif defined(CTRL_V41_CONFIG)
    #define HCI_VERSION                              0x07    // BT Core Specification V4.1
  #else // CTRL_V40_CONFIG
    #define HCI_VERSION                              0x06    // BT Core Specification V4.0
  #endif
#else // !CC26XX
  #define HCI_VERSION                                0x06    // BT Core Specification V4.0
#endif // CC26XX/CC13XX

// Major Version (8 bits) . Minor Version (4 bits) . SubMinor Version (4 bits)
#if defined( CC26X2 ) || defined(CC13X2) || defined(CC13X2P)
#define HCI_REVISION                                 0x0224  // HCI Version BLE5 2.2.4
#elif defined( CC26XX )
#define HCI_REVISION                                 0x0111  // HCI Version BLE5 1.1.1
#else // !CC26X2 && !CC13X2 && !CC26XX && !CC13XX
#define HCI_REVISION                                 0x0141  // HCI Version 1.4.1
#endif // CC26X2/CC13X2/CC26XX/CC13XX

// Internal Only Status Values
#define HCI_STATUS_WARNING_FLAG_UNCHANGED            LL_STATUS_WARNING_FLAG_UNCHANGED

// Parameter Limits
#define HCI_ADV_NONCONN_INTERVAL_MIN                 LL_ADV_NONCONN_INTERVAL_MIN
#define HCI_ADV_NONCONN_INTERVAL_MAX                 LL_ADV_NONCONN_INTERVAL_MAX
#define HCI_ADV_CONN_INTERVAL_MIN                    LL_ADV_CONN_INTERVAL_MIN
#define HCI_ADV_CONN_INTERVAL_MAX                    LL_ADV_CONN_INTERVAL_MAX
#define HCI_SCAN_INTERVAL_MIN                        LL_SCAN_INTERVAL_MIN
#define HCI_SCAN_INTERVAL_MAX                        LL_SCAN_INTERVAL_MAX
#define HCI_SCAN_WINDOW_MIN                          LL_SCAN_WINDOW_MIN
#define HCI_SCAN_WINDOW_MAX                          LL_SCAN_WINDOW_MAX
#define HCI_CONN_INTERVAL_MIN                        LL_CONN_INTERVAL_MIN
#define HCI_CONN_INTERVAL_MAX                        LL_CONN_INTERVAL_MAX
#define HCI_CONN_TIMEOUT_MIN                         LL_CONN_TIMEOUT_MIN
#define HCI_CONN_TIMEOUT_MAX                         LL_CONN_TIMEOUT_MAX
#define HCI_SLAVE_LATENCY_MIN                        LL_SLAVE_LATENCY_MIN
#define HCI_SLAVE_LATENCY_MAX                        LL_SLAVE_LATENCY_MAX

/*
** Bluetooth Supported Commands
** Core Specification, Vol. 2, Part C, Section 3.3
*/

// Local Supported Feature Set
// Note: Bit 5 in byte 4 is LE supported feature bit.
//       Bit 6 in byte 4 is BR/EDR not supported feature bit.
#define LOCAL_SUPPORTED_FEATURE_SET_BYTE_4           0x60

/*
** Bluetooth Supported Commands
** Core Specification, Vol. 2, Part E, Section 6.27
*/

// Local Supported Commands
#define SUPPORTED_COMMAND_LEN                                                       64

// No Command Supported in Byte
#define SUPPORTED_CMD_NONE                                                          0x00

// Byte 0
#define SUPPORTED_CMD_INQUIRY                                                       0x01
#define SUPPORTED_CMD_INQUIRE_CANCEL                                                0x02
#define SUPPORTED_CMD_PERIODIC_INQUIRY_MODE                                         0x04
#define SUPPORTED_CMD_EXIT_PERIODIC_INQUIRY_MODE                                    0x08
#define SUPPORTED_CMD_CREATE_CONNECTION                                             0x10
#define SUPPORTED_CMD_DISCONNECT                                                    0x20
#define SUPPORTED_CMD_ADD_SCO_CONNECTION                                            0x40  // deprecated
#define SUPPORTED_CMD_CREATE_CONNECTION_CANCEL                                      0x80
// Byte 1
#define SUPPORTED_CMD_ACCEPT_CONNECTION_REQUEST                                     0x01
#define SUPPORTED_CMD_REJECT_CONNECTION_REQUEST                                     0x02
#define SUPPORTED_CMD_LINK_KEY_REQUEST_REPLY                                        0x04
#define SUPPORTED_CMD_LINK_KEY_REQUEST_NEG_REPLY                                    0x08
#define SUPPORTED_CMD_PIN_CODE_REQUEST_REPLY                                        0x10
#define SUPPORTED_CMD_PIN_CODE_REQUEST_NEG_REPLY                                    0x20
#define SUPPORTED_CMD_CHANGE_CONNECTION_PACKET_TYPE                                 0x40
#define SUPPORTED_CMD_AUTHENTICATION_REQUESTED                                      0x80
// Byte 2
#define SUPPORTED_CMD_SET_CONNECTION_ENCRYPTION                                     0x01
#define SUPPORTED_CMD_CHANGE_CONNECTION_LINK_KEY                                    0x02
#define SUPPORTED_CMD_MASTER_LINK_KEY                                               0x04
#define SUPPORTED_CMD_REMOTE_NAME_REQUEST                                           0x08
#define SUPPORTED_CMD_REMOTE_NAME_REQUEST_CANCEL                                    0x10
#define SUPPORTED_CMD_READ_REMOTE_SUPPORTED_FEATURES                                0x20
#define SUPPORTED_CMD_READ_REMOTE_EXTENDED_FEATURES                                 0x40
#define SUPPORTED_CMD_READ_REMOTE_VERSION_INFO                                      0x80
// Byte 3
#define SUPPORTED_CMD_READ_CLOCK_OFFSET                                             0x01
#define SUPPORTED_CMD_READ_LMP_HANDLE                                               0x02
#define SUPPORTED_CMD_RESERVED01                                                    0x04
#define SUPPORTED_CMD_RESERVED02                                                    0x08
#define SUPPORTED_CMD_RESERVED03                                                    0x10
#define SUPPORTED_CMD_RESERVED04                                                    0x20
#define SUPPORTED_CMD_RESERVED05                                                    0x40
#define SUPPORTED_CMD_RESERVED06                                                    0x80
// Byte 4
#define SUPPORTED_CMD_RESERVED07                                                    0x01
#define SUPPORTED_CMD_HOLD_MODE                                                     0x02
#define SUPPORTED_CMD_SNIFF_MODE                                                    0x04
#define SUPPORTED_CMD_EXIT_SNIFF_MODE                                               0x08
#define SUPPORTED_CMD_PARK_STATE                                                    0x10
#define SUPPORTED_CMD_EXIT_PARK_STATE                                               0x20
#define SUPPORTED_CMD_QOS_SETUP                                                     0x40
#define SUPPORTED_CMD_ROLE_DISCOVERY                                                0x80
// Byte 5
#define SUPPORTED_CMD_SWITCH_ROLE                                                   0x01
#define SUPPORTED_CMD_READ_LINK_POLICY_SETTINGS                                     0x02
#define SUPPORTED_CMD_WRITE_LINK_POLICY_SETTINGS                                    0x04
#define SUPPORTED_CMD_READ_DEFAULT_LINK_POLICY_SETTINGS                             0x08
#define SUPPORTED_CMD_WRITE_DEFAULT_LINK_POLICY_SETTINGS                            0x10
#define SUPPORTED_CMD_FLOW_SPECIFICATION                                            0x20
#define SUPPORTED_CMD_SET_EVENT_MASK                                                0x40
#define SUPPORTED_CMD_RESET                                                         0x80
// Byte 6
#define SUPPORTED_CMD_SET_EVENT_FILTER                                              0x01
#define SUPPORTED_CMD_FLUSH                                                         0x02
#define SUPPORTED_CMD_READ_PIN_TYPE                                                 0x04
#define SUPPORTED_CMD_WRITE_PIN_TYPE                                                0x08
#define SUPPORTED_CMD_CREATE_NEW_UNIT_KEY                                           0x10
#define SUPPORTED_CMD_READ_STORED_LINK_KEY                                          0x20
#define SUPPORTED_CMD_WRITE_STORED_LINK_KEY                                         0x40
#define SUPPORTED_CMD_DELETE_STORED_LINK_KEY                                        0x80
// Byte 7
#define SUPPORTED_CMD_WRITE_LOCAL_NAME                                              0x01
#define SUPPORTED_CMD_READ_LOCAL_NAME                                               0x02
#define SUPPORTED_CMD_READ_CONNECTION_ACCEPT_TIME                                   0x04
#define SUPPORTED_CMD_WRITE_CONNECTION_ACCEPT_TIME                                  0x08
#define SUPPORTED_CMD_READ_PAGE_TIMEOUT                                             0x10
#define SUPPORTED_CMD_WRITE_PAGE_TIMEOUT                                            0x20
#define SUPPORTED_CMD_READ_SCAN_ENABLE                                              0x40
#define SUPPORTED_CMD_WRITE_SCAN_ENABLE                                             0x80
// Byte 8
#define SUPPORTED_CMD_READ_PAGE_SCAN_ACTIVITY                                       0x01
#define SUPPORTED_CMD_WRITE_PAGE_SCAN_ACTIVITY                                      0x02
#define SUPPORTED_CMD_READ_INQUIRY_SCAN_ACTIVITY                                    0x04
#define SUPPORTED_CMD_WRITE_INQUIRY_SCAN_ACTIVITY                                   0x08
#define SUPPORTED_CMD_READ_AUTHENTICATION_ENABLE                                    0x10
#define SUPPORTED_CMD_WRITE_AUTHENTICATION_ENABLE                                   0x20
#define SUPPORTED_CMD_READ_ENCRYPTION_MODE                                          0x40  // deprecated
#define SUPPORTED_CMD_WRITE_ENCRYPTION_MODE                                         0x80  // deprecated
// Byte 9
#define SUPPORTED_CMD_READ_CLASS_OF_DEVICE                                          0x01
#define SUPPORTED_CMD_WRITE_CLASS_OF_DEVICE                                         0x02
#define SUPPORTED_CMD_READ_VOICE_SETTING                                            0x04
#define SUPPORTED_CMD_WRITE_VOICE_SETTING                                           0x08
#define SUPPORTED_CMD_READ_AUTOMATIC_FLUSH_TIMEOUT                                  0x10
#define SUPPORTED_CMD_WRITE_AUTOMATIC_FLUSH_TIMEOUT                                 0x20
#define SUPPORTED_CMD_READ_NUMBER_BROADCAST_RETRANSMISSIONS                         0x40
#define SUPPORTED_CMD_WRITE_NUMBER_BROADCAST_RETRANSMISSIONS                        0x80
// Byte 10
#define SUPPORTED_CMD_READ_HOLD_MODE_ACTIVITY                                       0x01
#define SUPPORTED_CMD_WRITE_HOLD_MODE_ACTIVITY                                      0x02
#define SUPPORTED_CMD_READ_TRANSMIT_POWER_LEVEL                                     0x04
#define SUPPORTED_CMD_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE                          0x08
#define SUPPORTED_CMD_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE                         0x10
#define SUPPORTED_CMD_SET_CONTROLLER_TO_HOST_FLOW_CONTROL                           0x20
#define SUPPORTED_CMD_HOST_BUFFER_SIZE                                              0x40
#define SUPPORTED_CMD_HOST_NUMBER_OF_COMPLETED_PACKETS                              0x80
// Byte 11
#define SUPPORTED_CMD_READ_LINK_SUPERVISION_TIMEOUT                                 0x01
#define SUPPORTED_CMD_WRITE_LINK_SUPERVISION_TIMEOUT                                0x02
#define SUPPORTED_CMD_READ_NUMBER_OF_SUPPORTED_IAC                                  0x04
#define SUPPORTED_CMD_READ_CURRENT_IAC_LAP                                          0x08
#define SUPPORTED_CMD_WRITE_CURRENT_IAC_LAP                                         0x10
#define SUPPORTED_CMD_READ_PAGE_SCAN_MODE_PERIOD                                    0x20  // deprecated
#define SUPPORTED_CMD_WRITE_PAGE_SCAN_MODE_PERIOD                                   0x40  // deprecated
#define SUPPORTED_CMD_READ_PAGE_SCAN_MODE                                           0x80  // deprecated
// Byte 12
#define SUPPORTED_CMD_WRITE_PAGE_SCAN_MODE                                          0x01  // deprecated
#define SUPPORTED_CMD_SET_AFH_HOST_CHANNEL_CLASSIFICATION                           0x02
#define SUPPORTED_CMD_RESERVED08                                                    0x04
#define SUPPORTED_CMD_RESERVED09                                                    0x08
#define SUPPORTED_CMD_READ_INQUIRY_SCAN_TYPE                                        0x10
#define SUPPORTED_CMD_WRITE_INQUIRY_SCAN_TYPE                                       0x20
#define SUPPORTED_CMD_READ_INQUIRY_MODE                                             0x40
#define SUPPORTED_CMD_WRITE_INQUIRY_MODE                                            0x80
// Byte 13
#define SUPPORTED_CMD_READ_PAGE_SCAN_TYPE                                           0x01
#define SUPPORTED_CMD_WRITE_PAGE_SCAN_TYPE                                          0x02
#define SUPPORTED_CMD_READ_AFH_CHANNEL_ASSESSMENT_MODE                              0x04
#define SUPPORTED_CMD_WRITE_AFH_CHANNEL_ASSESSMENT_MODE                             0x08
#define SUPPORTED_CMD_RESERVED10                                                    0x10
#define SUPPORTED_CMD_RESERVED11                                                    0x20
#define SUPPORTED_CMD_RESERVED12                                                    0x40
#define SUPPORTED_CMD_RESERVED13                                                    0x80
// Byte 14
#define SUPPORTED_CMD_RESERVED14                                                    0x01
#define SUPPORTED_CMD_RESERVED15                                                    0x02
#define SUPPORTED_CMD_RESERVED16                                                    0x04
#define SUPPORTED_CMD_READ_LOCAL_VERSION_INFORMATION                                0x08
#define SUPPORTED_CMD_RESERVED17                                                    0x10
#define SUPPORTED_CMD_READ_LOCAL_SUPPORTED_FEATURES                                 0x20
#define SUPPORTED_CMD_READ_LOCAL_EXTENDED_FEATURES                                  0x40
#define SUPPORTED_CMD_READ_BUFFER_SIZE                                              0x80
// Byte 15
#define SUPPORTED_CMD_READ_COUNTRY_CODE                                             0x01  // deprecated
#define SUPPORTED_CMD_READ_BDADDR                                                   0x02
#define SUPPORTED_CMD_READ_FAILED_CONTACT_COUNTER                                   0x04
#define SUPPORTED_CMD_RESET_FAILED_CONTACT_COUNTER                                  0x08
#define SUPPORTED_CMD_READ_LINK_QUALITY                                             0x10
#define SUPPORTED_CMD_READ_RSSI                                                     0x20
#define SUPPORTED_CMD_READ_AFH_CHANNEL_MAP                                          0x40
#define SUPPORTED_CMD_READ_CLOCK                                                    0x80
// Byte 16
#define SUPPORTED_CMD_READ_LOOPBACK_MODE                                            0x01
#define SUPPORTED_CMD_WRITE_LOOPBACK_MODE                                           0x02
#define SUPPORTED_CMD_ENABLE_DEVICE_UNDER_TEST_MODE                                 0x04
#define SUPPORTED_CMD_SETUP_SYNCHRONOUS_CONNECTION_REQUEST                          0x08
#define SUPPORTED_CMD_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST                         0x10
#define SUPPORTED_CMD_REJECT_SYNCHRONOUS_CONNECTION_REQUEST                         0x20
#define SUPPORTED_CMD_RESERVED18                                                    0x40
#define SUPPORTED_CMD_RESERVED19                                                    0x80
// Byte 17
#define SUPPORTED_CMD_READ_EXTENDED_INQUIRY_RESPONSE                                0x01
#define SUPPORTED_CMD_WRITE_EXTENDED_INQUIRY_RESPONSE                               0x02
#define SUPPORTED_CMD_REFRESH_ENCRYPTION_KEY                                        0x04
#define SUPPORTED_CMD_RESERVED20                                                    0x08
#define SUPPORTED_CMD_SNIFF_SUBRATING                                               0x10
#define SUPPORTED_CMD_READ_SIMPLE_PAIRING_MODE                                      0x20
#define SUPPORTED_CMD_WRITE_SIMPLE_PAIRING_MODE                                     0x40
#define SUPPORTED_CMD_READ_LOCAL_OOB_DATA                                           0x80
// Byte 18
#define SUPPORTED_CMD_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL                    0x01
#define SUPPORTED_CMD_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL                            0x02
#define SUPPORTED_CMD_READ_DEFAULT_ERRONEOUS_DATA_REPORTING                         0x04
#define SUPPORTED_CMD_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING                        0x08
#define SUPPORTED_CMD_RESERVED21                                                    0x10
#define SUPPORTED_CMD_RESERVED22                                                    0x20
#define SUPPORTED_CMD_RESERVED23                                                    0x40
#define SUPPORTED_CMD_IO_CAPABILITY_REQUEST_REPLY                                   0x80
// Byte 19
#define SUPPORTED_CMD_USER_CONFIRMATION_REQUEST_REPLY                               0x01
#define SUPPORTED_CMD_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY                      0x02
#define SUPPORTED_CMD_USER_PASSKEY_REQUEST_REPLY                                    0x04
#define SUPPORTED_CMD_USER_PASSKEY_REQUEST_NEGATIVE_REPLY                           0x08
#define SUPPORTED_CMD_REMOTE_OOB_DATA_REQUEST_REPLY                                 0x10
#define SUPPORTED_CMD_WRITE_SIMPLE_PAIRING_DEBUG_MODE                               0x20
#define SUPPORTED_CMD_ENHANCED_FLUSH                                                0x40
#define SUPPORTED_CMD_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY                        0x80
// Byte 20
#define SUPPORTED_CMD_RESERVED24                                                    0x01
#define SUPPORTED_CMD_RESERVED25                                                    0x02
#define SUPPORTED_CMD_SEND_KEYPRESS_NOTIFICATION                                    0x04
#define SUPPORTED_CMD_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY                          0x08
#define SUPPORTED_CMD_READ_ENCRYPTION_KEY_SIZE                                      0x10
#define SUPPORTED_CMD_RESERVED26                                                    0x20
#define SUPPORTED_CMD_RESERVED27                                                    0x40
#define SUPPORTED_CMD_RESERVED28                                                    0x80
// Byte 21
#define SUPPORTED_CMD_CREATE_PHYSICAL_LINK                                          0x01
#define SUPPORTED_CMD_ACCEPT_PHYSICAL_LINK                                          0x02
#define SUPPORTED_CMD_DISCONNECT_PHYSICAL_LINK                                      0x04
#define SUPPORTED_CMD_CREATE_LOGICAL_LINK                                           0x08
#define SUPPORTED_CMD_ACCEPT_LOGICAL_LINK                                           0x10
#define SUPPORTED_CMD_DISCONNECT_LOGICAL_LINK                                       0x20
#define SUPPORTED_CMD_LOGICAL_LINK_CANCEL                                           0x40
#define SUPPORTED_CMD_FLOW_SPEC_MDOIFY                                              0x80
// Byte 22
#define SUPPORTED_CMD_READ_LOGICAL_LINK_ACCEPT_TIMEOUT                              0x01
#define SUPPORTED_CMD_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT                             0x02
#define SUPPORTED_CMD_SET_EVENT_MASK_PAGE_2                                         0x04
#define SUPPORTED_CMD_READ_LOCATION_DATA                                            0x08
#define SUPPORTED_CMD_WRITE_LOCATION_DATA                                           0x10
#define SUPPORTED_CMD_READ_LOCAL_AMP_INFO                                           0x20
#define SUPPORTED_CMD_READ_LOCAL_AMP_ASSOC                                          0x40
#define SUPPORTED_CMD_WRITE_LOCAL_AMP_ASSOC                                         0x80
// Byte 23
#define SUPPORTED_CMD_READ_FLOW_CONTROL_MODE                                        0x01
#define SUPPORTED_CMD_WRITE_FLOW_CONTROL_MODE                                       0x02
#define SUPPORTED_CMD_READ_DATA_BLOCK_SIZE                                          0x04
#define SUPPORTED_CMD_RESERVED29                                                    0x08
#define SUPPORTED_CMD_RESERVED30                                                    0x10
#define SUPPORTED_CMD_ENABLE_AMP_RECEIVER_REPORTS                                   0x20
#define SUPPORTED_CMD_AMP_TEST_END                                                  0x40
#define SUPPORTED_CMD_AMP_TEST                                                      0x80
// Byte 24
#define SUPPORTED_CMD_READ_ENHANCED_TRANSMIT_POWER_LEVEL                            0x01
#define SUPPORTED_CMD_RESERVED31                                                    0x02
#define SUPPORTED_CMD_READ_BEST_EFFORT_FLUSH_TIMEOUT                                0x04
#define SUPPORTED_CMD_WRITE_BEST_EFFORT_FLUSH_TIMEOUT                               0x08
#define SUPPORTED_CMD_SHORT_RANGE_MODE                                              0x10
#define SUPPORTED_CMD_READ_LE_HOST_SUPPORT                                          0x20
#define SUPPORTED_CMD_WRITE_LE_HOST_SUPPORT                                         0x40
#define SUPPORTED_CMD_RESERVED32                                                    0x80
// Byte 25
#define SUPPORTED_CMD_LE_SET_EVENT_MASK                                             0x01
#define SUPPORTED_CMD_LE_READ_BUFFER_SIZE                                           0x02
#define SUPPORTED_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES                              0x04
#define SUPPORTED_CMD_RESERVED33                                                    0x08
#define SUPPORTED_CMD_LE_SET_RANDOM_ADDRESS                                         0x10
#define SUPPORTED_CMD_LE_SET_ADVERTISING_PARAMETERS                                 0x20
#define SUPPORTED_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER                          0x40
#define SUPPORTED_CMD_LE_SET_ADVERTISING_DATA                                       0x80
// Byte 26
#define SUPPORTED_CMD_LE_SET_SCAN_RESPONSE_DATA                                     0x01
#define SUPPORTED_CMD_LE_SET_ADVERTISE_ENABLE                                       0x02
#define SUPPORTED_CMD_LE_SET_SCAN_PARAMETERS                                        0x04
#define SUPPORTED_CMD_LE_SET_SCAN_ENABLE                                            0x08
#define SUPPORTED_CMD_LE_CREATE_CONNECTION                                          0x10
#define SUPPORTED_CMD_LE_CREATE_CONNECTION_CANCEL                                   0x20
#define SUPPORTED_CMD_LE_READ_WHITE_LIST_SIZE                                       0x40
#define SUPPORTED_CMD_LE_CLEAR_WHITE_LIST                                           0x80
// Byte 27
#define SUPPORTED_CMD_LE_ADD_DEVICE_TO_WHITE_LIST                                   0x01
#define SUPPORTED_CMD_LE_REMOVE_DEVICE_FROM_WHITE_LIST                              0x02
#define SUPPORTED_CMD_LE_CONNECTION_UPDATE                                          0x04
#define SUPPORTED_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION                            0x08
#define SUPPORTED_CMD_LE_READ_CHANNEL_MAP                                           0x10
#define SUPPORTED_CMD_LE_READ_REMOTE_USED_FEATURES                                  0x20
#define SUPPORTED_CMD_LE_ENCRYPT                                                    0x40
#define SUPPORTED_CMD_LE_RAND                                                       0x80
// Byte 28
#define SUPPORTED_CMD_LE_START_ENCRYPTION                                           0x01
#define SUPPORTED_CMD_LE_LONG_TERM_KEY_REQUEST_REPLY                                0x02
#define SUPPORTED_CMD_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY                       0x04
#define SUPPORTED_CMD_LE_READ_SUPPORTED_STATES                                      0x08
#define SUPPORTED_CMD_LE_RECEIVER_TEST                                              0x10
#define SUPPORTED_CMD_LE_TRANSMITTER_TEST                                           0x20
#define SUPPORTED_CMD_LE_TEST_END                                                   0x40
#define SUPPORTED_CMD_RESERVED34                                                    0x80
// Byte 29
#define SUPPORTED_CMD_RESERVED35                                                    0x01
#define SUPPORTED_CMD_RESERVED36                                                    0x02
#define SUPPORTED_CMD_RESERVED37                                                    0x04
#define SUPPORTED_CMD_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION                         0x08
#define SUPPORTED_CMD_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION                        0x10
#define SUPPORTED_CMD_READ_LOCAL_SUPPORTED_CODECS                                   0x20
#define SUPPORTED_CMD_SET_MWS_CHANNEL_PARAMETERS_COMMAND                            0x40
#define SUPPORTED_CMD_SET_EXTERNAL_FRAME_CONFIGURATION_COMMAND                      0x80
// Byte 30
#define SUPPORTED_CMD_SET_MWS_SIGNALING_COMMAND                                     0x01
#define SUPPORTED_CMD_SET_TRANSPORT_LAYER_COMMAND                                   0x02
#define SUPPORTED_CMD_SET_MWS_SCAN_FREQUENCY_TABLE_COMMAND                          0x04
#define SUPPORTED_CMD_GET_TRANSPORT_LAYER_CONFIGURATION_COMMAND                     0x08
#define SUPPORTED_CMD_SET_MWS_PATTERN_CONFIGURATION_COMMAND                         0x10
#define SUPPORTED_CMD_SET_TRIGGERED_CLOCK_CAPTURE                                   0x20
#define SUPPORTED_CMD_TRUNCATED_PAGE                                                0x40
#define SUPPORTED_CMD_TRUNCATED_PAGE_CANCEL                                         0x80
// Byte 31
#define SUPPORTED_CMD_SET_CONNECTIONLESS_SLAVE_BROADCAST                            0x01
#define SUPPORTED_CMD_SET_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE                    0x02
#define SUPPORTED_CMD_START_SYNCHRONIZATION_TRAIN                                   0x04
#define SUPPORTED_CMD_RECEIVE_SYNCHRONIZATION_TRAIN                                 0x08
#define SUPPORTED_CMD_SET_RESERVED_LT_ADDR                                          0x10
#define SUPPORTED_CMD_DELETE_RESERVED_LT_ADDR                                       0x20
#define SUPPORTED_CMD_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA                       0x40
#define SUPPORTED_CMD_READ_SYNCHRONIZATION_TRAIN_PARAMETERS                         0x80
// Byte 32
#define SUPPORTED_CMD_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS                        0x01
#define SUPPORTED_CMD_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY                        0x02
#define SUPPORTED_CMD_READ_SECURE_CONNECTIONS_HOST_SUPPORT                          0x04
#define SUPPORTED_CMD_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT                         0x08
#define SUPPORTED_CMD_READ_AUTHENTICATED_PAYLOAD_TIMEOUT                            0x10
#define SUPPORTED_CMD_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT                           0x20
#define SUPPORTED_CMD_READ_LOCAL_OOB_EXTENDED_DATA                                  0x40
#define SUPPORTED_CMD_WRITE_SECURE_CONNECTIONS_TEST_MODE                            0x80
// Byte 33
#define SUPPORTED_CMD_READ_EXTENDED_PAGE_TIMEOUT                                    0x01
#define SUPPORTED_CMD_WRITE_EXTENDED_PAGE_TIMEOUT                                   0x02
#define SUPPORTED_CMD_READ_EXTENDED_INQUIRY_LENGTH                                  0x04
#define SUPPORTED_CMD_WRITE_EXTENDED_INQUIRY_LENGTH                                 0x08
#define SUPPORTED_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND          0x10
#define SUPPORTED_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND 0x20
#define SUPPORTED_CMD_SET_DATA_LENGTH                                               0x40
#define SUPPORTED_CMD_READ_SUGGESTED_DEFAULT_DATA_LENGTH                            0x80
// Byte 34
#define SUPPORTED_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH                        0x01
#define SUPPORTED_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY                                 0x02
#define SUPPORTED_CMD_LE_GENERATE_DH_KEY                                            0x04
#define SUPPORTED_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST                               0x08
#define SUPPORTED_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                          0x10
#define SUPPORTED_CMD_LE_CLEAR_RESOLVING_LIST                                       0x20
#define SUPPORTED_CMD_LE_READ_RESOLVING_LIST                                        0x40
#define SUPPORTED_CMD_LE_READ_PEER_RESOLVABLE_ADDRESS                               0x80
// Byte 35
#define SUPPORTED_CMD_LE_READ_LOCAL_RESOLVABLE_ADDRESS                              0x01
#define SUPPORTED_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE                              0x02
#define SUPPORTED_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT                     0x04
#define SUPPORTED_CMD_LE_READ_MAXIMUM_DATA_LENGTH                                   0x08
#define SUPPORTED_CMD_LE_READ_PHY                                                   0x10
#define SUPPORTED_CMD_LE_SET_DEFAULT_PHY                                            0x20
#define SUPPORTED_CMD_LE_SET_PHY                                                    0x40
#define SUPPORTED_CMD_LE_ENHANCED_RECEIVER_TEST                                     0x80
// Byte 36
#define SUPPORTED_CMD_LE_ENHANCED_TRANSMITTER_TEST                                  0x01
#define SUPPORTED_CMD_LE_SET_ADVVERTISING_SET_RANDOM_ADDRESS                        0x02
#define SUPPORTED_CMD_LE_SET_EXTENDED_ADVVERTISING_PARAMETERS                       0x04
#define SUPPORTED_CMD_LE_SET_EXTENDED_ADVERTISING_DATA                              0x08
#define SUPPORTED_CMD_LE_SET_EXTENDED_SCAN_RESPONSE_DATA                            0x10
#define SUPPORTED_CMD_LE_SET_EXTENDED_ADVERTISING_ENABLE                            0x20
#define SUPPORTED_CMD_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH                       0x40
#define SUPPORTED_CMD_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS                  0x80
// Byte 37
#define SUPPORTED_CMD_LE_REMOVE_ADVERTISING_SET                                     0x01
#define SUPPORTED_CMD_LE_CLEAR_ADVERTISING_SETS                                     0x02
#define SUPPORTED_CMD_LE_SET_PERIODIC_ADVERTISING_PARAMETERS                        0x04
#define SUPPORTED_CMD_LE_SET_PERIODIC_ADVERTISING_DATA                              0x08
#define SUPPORTED_CMD_LE_SET_PERIODIC_ADVERTISING_ENABLE                            0x10
#define SUPPORTED_CMD_LE_SET_EXTENDED_SCAN_PARAMETERS                               0x20
#define SUPPORTED_CMD_LE_SET_EXTENDED_SCAN_EANBLE                                   0x40
#define SUPPORTED_CMD_LE_EXTENDED_CREATE_CONNECTION_COMMAND                         0x80
// Byte 38
#define SUPPORTED_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC                           0x01
#define SUPPORTED_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL                    0x02
#define SUPPORTED_CMD_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC                        0x04
#define SUPPORTED_CMD_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST                     0x08
#define SUPPORTED_CMD_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST                0x10
#define SUPPORTED_CMD_LE_CLEAR_PERIODIC_ADVERTISER_LIST                             0x20
#define SUPPORTED_CMD_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE                         0x40
#define SUPPORTED_CMD_LE_READ_TRANSMIT_POWER                                        0x80
// Byte 39
#define SUPPORTED_CMD_LE_READ_RF_PATH_COMPENSATION                                  0x01
#define SUPPORTED_CMD_LE_WRITE_RF_PATH_COMPENSATION                                 0x02
#define SUPPORTED_CMD_LE_SET_PRIVACY_MODE                                           0x04
#define SUPPORTED_CMD_RESERVED38                                                    0x08
#define SUPPORTED_CMD_RESERVED39                                                    0x10
#define SUPPORTED_CMD_RESERVED40                                                    0x20
#define SUPPORTED_CMD_RESERVED41                                                    0x40
#define SUPPORTED_CMD_RESERVED42                                                    0x80
// Byte 40 - Byte 63
#define SUPPORTED_CMD_RESERVED43                                                    0x01
#define SUPPORTED_CMD_RESERVED44                                                    0x02
#define SUPPORTED_CMD_RESERVED45                                                    0x04
#define SUPPORTED_CMD_RESERVED46                                                    0x08
#define SUPPORTED_CMD_RESERVED47                                                    0x10
#define SUPPORTED_CMD_RESERVED48                                                    0x20
#define SUPPORTED_CMD_RESERVED49                                                    0x40
#define SUPPORTED_CMD_RESERVED50                                                    0x80

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_0  //
///////////////////////////////

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  #define SUPPORTED_COMMAND_BYTE_0                   (SUPPORTED_CMD_DISCONNECT)
#else  // !INIT_CFG
  #define SUPPORTED_COMMAND_BYTE_0                   (SUPPORTED_CMD_NONE)
#endif  // INIT_CFG

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_2  //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_2                     (SUPPORTED_CMD_READ_REMOTE_VERSION_INFO)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_5  //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_5                     (SUPPORTED_CMD_RESET |                                   \
                                                      SUPPORTED_CMD_SET_EVENT_MASK)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_10 //
// SUPPORTED_COMMAND_BYTE_15 //
///////////////////////////////

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  #define SUPPORTED_COMMAND_BYTE_10                  (SUPPORTED_CMD_HOST_NUMBER_OF_COMPLETED_PACKETS    |     \
                                                      SUPPORTED_CMD_HOST_BUFFER_SIZE                    |     \
                                                      SUPPORTED_CMD_SET_CONTROLLER_TO_HOST_FLOW_CONTROL |     \
                                                      SUPPORTED_CMD_READ_TRANSMIT_POWER_LEVEL )

  #define SUPPORTED_COMMAND_BYTE_15                  (SUPPORTED_CMD_READ_RSSI |                               \
                                                      SUPPORTED_CMD_READ_BDADDR)

#else  // !ADV_CONN_CFG && !INIT_CFG
  #define SUPPORTED_COMMAND_BYTE_10                  (SUPPORTED_CMD_HOST_NUMBER_OF_COMPLETED_PACKETS |        \
                                                      SUPPORTED_CMD_HOST_BUFFER_SIZE                 |        \
                                                      SUPPORTED_CMD_SET_CONTROLLER_TO_HOST_FLOW_CONTROL)

  #define SUPPORTED_COMMAND_BYTE_15                  (SUPPORTED_CMD_READ_BDADDR)
#endif  // ADV_CONN_CFG | INIT_CFG

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_14 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_14                    (SUPPORTED_CMD_READ_LOCAL_SUPPORTED_FEATURES |           \
                                                      SUPPORTED_CMD_READ_LOCAL_VERSION_INFORMATION)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_22 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_22                    (SUPPORTED_CMD_SET_EVENT_MASK_PAGE_2)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
#define BYTE_25_ADV                                  (SUPPORTED_CMD_LE_SET_ADVERTISING_DATA              |    \
                                                      SUPPORTED_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER |    \
                                                      SUPPORTED_CMD_LE_SET_ADVERTISING_PARAMETERS)

#define BYTE_26_ADV                                  (SUPPORTED_CMD_LE_SET_ADVERTISE_ENABLE |                 \
                                                      SUPPORTED_CMD_LE_SET_SCAN_RESPONSE_DATA)
#else  // !ADV_NCONN_CFG && !ADV_CONN_CFG
#define BYTE_25_ADV                                  (SUPPORTED_CMD_NONE)
#define BYTE_26_ADV                                  (SUPPORTED_CMD_NONE)
#endif  // ADV_NCONN_CFG | ADV_CONN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
#define BYTE_26_SCAN                                 (SUPPORTED_CMD_LE_SET_SCAN_ENABLE |                      \
                                                      SUPPORTED_CMD_LE_SET_SCAN_PARAMETERS)
#else  // !SCAN_CFG
#define BYTE_26_SCAN                                 (SUPPORTED_CMD_NONE)
#endif  // SCAN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
#define BYTE_27_ADV                                  (SUPPORTED_CMD_LE_READ_CHANNEL_MAP  |                    \
                                                      SUPPORTED_CMD_LE_CONNECTION_UPDATE |                    \
                                                      SUPPORTED_CMD_LE_READ_REMOTE_USED_FEATURES)

#define BYTE_28_ADV                                  (SUPPORTED_CMD_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY | \
                                                      SUPPORTED_CMD_LE_LONG_TERM_KEY_REQUEST_REPLY)
#else  // ADV_CONN_CFG
#define BYTE_27_ADV                                  (SUPPORTED_CMD_NONE)
#define BYTE_28_ADV                                  (SUPPORTED_CMD_NONE)
#endif  // ADV_NCONN_CFG | ADV_CONN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
#define BYTE_26_INIT                                 (SUPPORTED_CMD_LE_CREATE_CONNECTION_CANCEL |             \
                                                      SUPPORTED_CMD_LE_CREATE_CONNECTION)

#define BYTE_27_INIT                                 (SUPPORTED_CMD_LE_ENCRYPT                         |      \
                                                      SUPPORTED_CMD_LE_READ_CHANNEL_MAP                |      \
                                                      SUPPORTED_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION |      \
                                                      SUPPORTED_CMD_LE_CONNECTION_UPDATE)

#define BYTE_28_INIT                                 (SUPPORTED_CMD_LE_START_ENCRYPTION)

#else  //!INIT_CFG

#define BYTE_26_INIT                                 (SUPPORTED_CMD_NONE)
#define BYTE_27_INIT                                 (SUPPORTED_CMD_NONE)
#define BYTE_28_INIT                                 (SUPPORTED_CMD_NONE)

#endif  // INIT_CFG

#define BYTE_25_COMMON                               (SUPPORTED_CMD_LE_SET_RANDOM_ADDRESS            |        \
                                                      SUPPORTED_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES |        \
                                                      SUPPORTED_CMD_LE_READ_BUFFER_SIZE              |        \
                                                      SUPPORTED_CMD_LE_SET_EVENT_MASK)

#define BYTE_26_COMMON                               (SUPPORTED_CMD_LE_CLEAR_WHITE_LIST     |                 \
                                                      SUPPORTED_CMD_LE_READ_WHITE_LIST_SIZE)

#define BYTE_27_COMMON                               (SUPPORTED_CMD_LE_RAND                          |        \
                                                      SUPPORTED_CMD_LE_READ_REMOTE_USED_FEATURES     |        \
                                                      SUPPORTED_CMD_LE_REMOVE_DEVICE_FROM_WHITE_LIST |        \
                                                      SUPPORTED_CMD_LE_ADD_DEVICE_TO_WHITE_LIST)

#define BYTE_28_COMMON                               (SUPPORTED_CMD_LE_TEST_END         |                     \
                                                      SUPPORTED_CMD_LE_TRANSMITTER_TEST |                     \
                                                      SUPPORTED_CMD_LE_RECEIVER_TEST    |                     \
                                                      SUPPORTED_CMD_LE_READ_SUPPORTED_STATES)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_25 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_25                    (BYTE_25_COMMON |                                        \
                                                      BYTE_25_ADV)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_26 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_26                    (BYTE_26_COMMON |                                        \
                                                      BYTE_26_ADV    |                                        \
                                                      BYTE_26_SCAN   |                                        \
                                                      BYTE_26_INIT)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_27 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_27                    (BYTE_27_COMMON |                                        \
                                                      BYTE_27_ADV    |                                        \
                                                      BYTE_27_INIT)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_28 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_28                    (BYTE_28_COMMON |                                        \
                                                      BYTE_28_ADV    |                                        \
                                                      BYTE_28_INIT)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_32 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_32                  (SUPPORTED_CMD_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT |     \
                                                    SUPPORTED_CMD_READ_AUTHENTICATED_PAYLOAD_TIMEOUT)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_33 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_33                    (SUPPORTED_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND |  \
                                                      SUPPORTED_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND          |  \
                                                      SUPPORTED_CMD_SET_DATA_LENGTH                                               |  \
                                                      SUPPORTED_CMD_READ_SUGGESTED_DEFAULT_DATA_LENGTH)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_34 //
///////////////////////////////

#define SUPPORTED_COMMAND_BYTE_34                  (SUPPORTED_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH | \
                                                    SUPPORTED_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY          | \
                                                    SUPPORTED_CMD_LE_GENERATE_DH_KEY                     | \
                                                    SUPPORTED_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST        | \
                                                    SUPPORTED_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST   | \
                                                    SUPPORTED_CMD_LE_CLEAR_RESOLVING_LIST                | \
                                                    SUPPORTED_CMD_LE_READ_RESOLVING_LIST                 | \
                                                    SUPPORTED_CMD_LE_READ_PEER_RESOLVABLE_ADDRESS)

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_35 //
///////////////////////////////

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  #define SUPPORTED_COMMAND_BYTE_35                (SUPPORTED_CMD_LE_READ_LOCAL_RESOLVABLE_ADDRESS          | \
                                                    SUPPORTED_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE          | \
                                                    SUPPORTED_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT | \
                                                    SUPPORTED_CMD_LE_READ_MAXIMUM_DATA_LENGTH               | \
                                                    SUPPORTED_CMD_LE_READ_PHY                               | \
                                                    SUPPORTED_CMD_LE_SET_DEFAULT_PHY                        | \
                                                    SUPPORTED_CMD_LE_SET_PHY                                | \
                                                    SUPPORTED_CMD_LE_ENHANCED_RECEIVER_TEST)
#else // !(PHY_2MBPS_CFG | PHY_LR_CFG)
  #define SUPPORTED_COMMAND_BYTE_35                (SUPPORTED_CMD_LE_READ_LOCAL_RESOLVABLE_ADDRESS          | \
                                                    SUPPORTED_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE          | \
                                                    SUPPORTED_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT | \
                                                    SUPPORTED_CMD_LE_READ_MAXIMUM_DATA_LENGTH)
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_36 //
///////////////////////////////

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  #define SUPPORTED_COMMAND_BYTE_36                  (SUPPORTED_CMD_LE_ENHANCED_TRANSMITTER_TEST)
#else // !(PHY_2MBPS_CFG | PHY_LR_CFG)
  #define SUPPORTED_COMMAND_BYTE_36                  (SUPPORTED_CMD_NONE)
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

///////////////////////////////
// SUPPORTED_COMMAND_BYTE_39 //
///////////////////////////////
#define SUPPORTED_COMMAND_BYTE_39                  (SUPPORTED_CMD_LE_SET_PRIVACY_MODE)

/*******************************************************************************
 * TYPEDEFS
 */

typedef const uint8 supportedCmdsTable_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

// Note: There are 64 bytes of supported commands per the Core spec. One
//       additional byte is added as a status when the table is returned
//       as a parameter for event generation.
supportedCmdsTable_t supportedCmdsTable[SUPPORTED_COMMAND_LEN+1] =
{
  // used to hold status when returned as an event parameter
  HCI_SUCCESS,
  // supported commands
  SUPPORTED_COMMAND_BYTE_0,
  0x00,
  SUPPORTED_COMMAND_BYTE_2,
  0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_5,
  0x00, 0x00, 0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_10,
  0x00, 0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_14,
  SUPPORTED_COMMAND_BYTE_15,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_22,
  0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_25,
  SUPPORTED_COMMAND_BYTE_26,
  SUPPORTED_COMMAND_BYTE_27,
  SUPPORTED_COMMAND_BYTE_28,
  0x00, 0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_32,
  SUPPORTED_COMMAND_BYTE_33,
  SUPPORTED_COMMAND_BYTE_34,
  SUPPORTED_COMMAND_BYTE_35,
  SUPPORTED_COMMAND_BYTE_36,
  0x00, 0x00,
  SUPPORTED_COMMAND_BYTE_39,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

/*******************************************************************************
 * GLOBAL VARIABLES
 */

uint8  hciPTMenabled;
uint8  ctrlToHostEnable;
uint16 numHostBufs;

/*******************************************************************************
 * HCI API
 */

extern uint8  ctrlToHostEnable;
extern uint16 numHostBufs;

/*
** Buffer Management
*/


/*******************************************************************************
 * This API is used to allocate memory using buffer management.
 *
 * Public function defined in hci.h.
 */
void *HCI_bm_alloc( uint16 size )
{
  return( MAP_LL_TX_bm_alloc( size ) );
}


/*******************************************************************************
 * This API is used to check that the connection time parameters are valid.
 *
 * Public function defined in hci.h.
 */
uint8 HCI_ValidConnTimeParams( uint16 connIntervalMin,
                               uint16 connIntervalMax,
                               uint16 connLatency,
                               uint16 connTimeout )
{
  return( !LL_INVALID_CONN_TIME_PARAM( connIntervalMin,
                                       connIntervalMax,
                                       connLatency,
                                       connTimeout )                          &&
          !LL_INVALID_CONN_TIME_PARAM_COMBO( connIntervalMax,
                                             connLatency,
                                             connTimeout )
#if defined( CC26XX ) || defined( CC13XX )
                                                          );
#else // CC254x
                                                                              &&
          LL_ValidConnTimeParams( connIntervalMin,
                                  connIntervalMax ) == HCI_SUCCESS );
#endif // CC26XX/CC13XX
}


/*
** Data
*/

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This API is used to send a ACL data packet over a connection.
 *
 * NOTE: L2CAP is affected by this routine's status values as it must remap
 *       them to Host status values. If any additional status values are added
 *       and/or changed in this routine, a TI stack engineer must be notified!
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SendDataPkt( uint16  connHandle,
                             uint8   pbFlag,
                             uint16  pktLen,
                             uint8  *pData )
{
  hciStatus_t hciStatus;

  // various checks
  if ( hciPTMenabled == TRUE )
  {
    // not allowed command during PTM
    hciStatus = HCI_ERROR_CODE_CONTROLLER_BUSY;
  }
  else if ( (pktLen == 0) || (pData == NULL) || ((connHandle & 0xFF00) != 0) )
  {
    // bad packet length, bad pointer, or bad connection handle
    // Note: The TI LE only supports a eight bit connection handle, so check to
    //       be sure something isn't erroneously mapped to a valid connection
    //       handle (e.g. 0x0100 -> 0x0000).
    hciStatus = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }
  else // packet is okay
  {
    // attempt to send the packet
    // Note: A return of HCI_SUCCESS from this routine indicates that either
    //       the data was transmitted and freed, or it is still in use
    //       (i.e. queued).
    hciStatus = MAP_LL_TxData( connHandle, pData, pktLen, pbFlag );
  }

  return( hciStatus );
}
#endif // ADV_CONN_CFG | INIT_CFG

/*
** Link Control Commands
*/

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This BT API is used to terminate a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_DisconnectCmd( uint16 connHandle,
                               uint8  reason )

{
  MAP_HCI_CommandStatusEvent( MAP_LL_Disconnect(connHandle, reason),
                              HCI_DISCONNECT );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This BT API is used to request version information from the the remote
 * device in a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadRemoteVersionInfoCmd( uint16 connHandle )
{
  hciStatus_t status;

  MAP_HCI_CommandStatusEvent( HCI_SUCCESS, HCI_READ_REMOTE_VERSION_INFO );

  status = MAP_LL_ReadRemoteVersionInfo( connHandle );

  // check if something went wrong
  // Note: If success is returned, then Command Complete is handled by Callback.
  if ( status != HCI_SUCCESS )
  {
    MAP_HCI_CommandCompleteEvent( HCI_READ_REMOTE_VERSION_INFO,
                                  sizeof(status),
                                  &status );
  }

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*
** Controller and Baseband Commands
*/

/*******************************************************************************
 * This BT API is used to set the HCI event mask, which is used to determine
 * which events are supported.
 *
 * Note: The global pHciEvtMask is used for BT events. A different global is
 *       used for LE events: bleEvtMask.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SetEventMaskCmd( uint8 *pMask )
{
  hciStatus_t status;

  // check parameters
  if( pMask != NULL )
  {
    (void)MAP_osal_memcpy( pHciEvtMask, pMask, B_EVENT_MASK_LEN );

    status = HCI_SUCCESS;
  }
  else // bad parameters
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }

  MAP_HCI_CommandCompleteEvent( HCI_SET_EVENT_MASK, sizeof(status), &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This BT API is used to set the HCI event mask page 2, which is used to
 * determine which events are supported.
 *
 * Note: The global pHciEvtMask2 is used for BT events. A different global is
 *       used for LE events: pBleEvtMask.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SetEventMaskPage2Cmd( uint8 *pMask )
{
  hciStatus_t status;

  // check parameters
  if( pMask != NULL )
  {
    (void)MAP_osal_memcpy( pHciEvtMask2, pMask, B_EVENT_MASK_LEN );

    status = HCI_SUCCESS;
  }
  else // bad parameters
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }

  MAP_HCI_CommandCompleteEvent( HCI_SET_EVENT_MASK_PAGE_2, sizeof(status), &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 *
 * This BT API is used to reset the Link Layer.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ResetCmd( void )
{
  hciStatus_t status;

  // reset the Link Layer
  status = MAP_LL_Reset();

  // reset the Bluetooth and the BLE event mask bits
  MAP_hciInitEventMasks();

  // initialize Controller to Host flow control flag and counter
  ctrlToHostEnable = FALSE;
  numHostBufs      = 0;

  // complete the command
  MAP_HCI_CommandCompleteEvent( HCI_RESET, sizeof(status), &status);

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 *
 * This BT API is used to read the transmit power level.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadTransmitPowerLevelCmd( uint16 connHandle,
                                           uint8  txPwrType )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  // 3: Transmit Power Level
  uint8 rtnParam[4];

  rtnParam[0] = MAP_LL_ReadTxPowerLevel( connHandle,
                                         txPwrType,
                                         (int8 *)&(rtnParam[3]) );

  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_READ_TRANSMIT_POWER,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 *
 * This BT API is used by the Host to turn flow control on or off for data sent
 * from the Controller to Host.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_SetControllerToHostFlowCtrlCmd( uint8 flowControlEnable )
{
  hciStatus_t status = HCI_SUCCESS;

  // check parameters
  if ( (flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_OFF)              ||
       (flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_ON_SYNCH_OFF) ||
       (flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_OFF_SYNCH_ON) ||
       (flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_ON_SYNCH_ON) )
  {
    // check the parameter
    if( flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_OFF )
    {
      // disable flow control
      ctrlToHostEnable = FALSE;
    }
    else if ( flowControlEnable == HCI_CTRL_TO_HOST_FLOW_CTRL_ACL_ON_SYNCH_OFF )
    {
      // enable flow control
      ctrlToHostEnable = TRUE;
    }
    else // other two combinations not supported
    {
      // so indidicate
      status = HCI_ERROR_CODE_UNSUPPORTED_FEATURE_PARAM_VALUE;
    }
  }
  else // bad parameters
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }

  MAP_HCI_CommandCompleteEvent( HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL,
                                sizeof(status),
                                &status);

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 *
 * This BT API is used by the Host to notify the Controller of the maximum size
 * ACL buffer the Controller can send to the Host.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_HostBufferSizeCmd( uint16 hostAclPktLen,
                                   uint8  hostSyncPktLen,
                                   uint16 hostTotalNumAclPkts,
                                   uint16 hostTotalNumSyncPkts )
{
  hciStatus_t status;

  // unused input parameter; PC-Lint error 715.
  (void)hostSyncPktLen;
  (void)hostAclPktLen;
  (void)hostTotalNumSyncPkts;

  // check parameters
  // Note: Only Number of ACL Packets is supported. The rest of the parameters
  //       are ignored for now.
  if ( hostTotalNumAclPkts == 0 )
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }
  else // parameter okay
  {
    status = HCI_SUCCESS;

    // so save in a counter
    numHostBufs = hostTotalNumAclPkts;
  }

  MAP_HCI_CommandCompleteEvent( HCI_HOST_BUFFER_SIZE, sizeof(status), &status );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This BT API is used by the Host to notify the Controller of the number of
 * HCI data packets that have been completed for each connection handle since
 * this command was previously sent to the controller.
 *
 * Note: It is assumed that there will be at most only one handle. Even if more
 *       than one handle is provided, the Controller does not track Host buffers
 *       as a function of connection handles (and isn't required to do so).
 * Note: The connection handle is not used.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_HostNumCompletedPktCmd( uint8   numHandles,
                                        uint16 *connHandles,
                                        uint16 *numCompletedPkts )
{
  // check parameters
  if ( (numHandles != 0) && (connHandles != NULL) &&
       ((numCompletedPkts != NULL) && (*numCompletedPkts != 0)) )
  {
    // check if flow control is enabled
    if ( ctrlToHostEnable == TRUE )
    {
      // check if the number of Host buffers was previously exhausted
      if ( numHostBufs == 0 )
      {
        // yes, so disable LL Rx flow control
        (void)MAP_LL_CtrlToHostFlowControl( LL_DISABLE_RX_FLOW_CONTROL );
      }

      for (uint8 i=0; i<numHandles; i++)
      {
        // host is indicating it has freed one or more buffers
        // Note: It is assumed that the Host will only free one buffer at a time,
        //       and in any case, number of Host buffers are not tracked as a
        //       function of connection handles.
        // Note: No checks are made to ensure the specified connection handles
        //       are valid or active.
        numHostBufs += numCompletedPkts[i*2];
      }
    }

    // Note: The specification indicates that no event is normally returned.
  }
  else // bad parameters
  {
    hciStatus_t status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;

    // Note: The specification indicates that no event is normally returned,
    //       except if there are invalid parameters.
    MAP_HCI_CommandCompleteEvent( HCI_HOST_NUM_COMPLETED_PACKETS,
                                  sizeof(status),
                                  &status);
  }

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*
** Information Parameters
*/

/*******************************************************************************
 * This BT API is used to read the local version information.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalVersionInfoCmd( void )
{
  // 0: Status
  // 1: HCI Version Number
  // 2: HCI Revision Number LSB
  // 3: HCI Revision Number MSB
  // 4: Version Number
  // 5: Connection Handle LSB
  // 6: Connection Handle MSB
  // 7: LL Subversion Number LSB
  // 8: LL Subversion Number MSB
  uint8  rtnParam[9];
  uint8  version;
  uint16 comID;
  uint16 subverNum;

  // status
  rtnParam[0] = MAP_LL_ReadLocalVersionInfo( &version,
                                             &comID,
                                             &subverNum );

  // HCI version and revision
  rtnParam[1] = HCI_VERSION;
  rtnParam[2] = LO_UINT16( HCI_REVISION );
  rtnParam[3] = HI_UINT16( HCI_REVISION );

  // LL version, manufacturer name, LL subversion
  rtnParam[4] = version;
  rtnParam[5] = LO_UINT16( comID );
  rtnParam[6] = HI_UINT16( comID );
  rtnParam[7] = LO_UINT16( subverNum );
  rtnParam[8] = HI_UINT16( subverNum );

  MAP_HCI_CommandCompleteEvent( HCI_READ_LOCAL_VERSION_INFO,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This BT API is used to read the locally supported commands.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalSupportedCommandsCmd( void )
{
  // 0:     Status (HCI_SUCCESS)
  // 1..64: Supported Commands
  MAP_HCI_CommandCompleteEvent( HCI_READ_LOCAL_SUPPORTED_COMMANDS,
                                SUPPORTED_COMMAND_LEN+1,
                                (uint8 *)supportedCmdsTable );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This BT API is used to read the locally supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalSupportedFeaturesCmd( void )
{
  // 0:    Status
  // 1..8: Supported Features
  uint8 rtnParam[9] = {HCI_SUCCESS, 0, 0, 0, 0, 0, 0, 0, 0};

  // set byte 4 of the feature list, which is the only byte that matters
  rtnParam[5] = LOCAL_SUPPORTED_FEATURE_SET_BYTE_4;

  MAP_HCI_CommandCompleteEvent( HCI_READ_LOCAL_SUPPORTED_FEATURES,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This BT API is used to read this device's BLE address (BDADDR).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadBDADDRCmd( void )
{
  // 0:    Status
  // 1..6: BDADDR
  uint8 rtnParam[7];

  // status
  rtnParam[0] = MAP_LL_ReadBDADDR( &(rtnParam[1]) );

  MAP_HCI_CommandCompleteEvent( HCI_READ_BDADDR, sizeof(rtnParam), rtnParam );

  return( HCI_SUCCESS );
}

/*
** Status Parameters
*/

/*******************************************************************************
 * This BT API is used to read the RSSI.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadRssiCmd( uint16 connHandle )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  // 3: RSSI
  uint8 rtnParam[4];

  // status
  rtnParam[0] = MAP_LL_ReadRssi( connHandle,
                                 (int8 *)&(rtnParam[3]) );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle);
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_READ_RSSI, sizeof(rtnParam), rtnParam );

  return( HCI_SUCCESS );
}


/*
** HCI Low Energy Commands
*/

/*******************************************************************************
 * This LE API is used to set the HCI LE event mask, which is used to determine
 * which LE events are supported.
 *
 * Note: The global pBleEvtMask is used for LE events. A different global is
 *       used for BT events: pHciEvtMask.
 *
 * Public function defined in hci.h.
 * This BT API is used to read the local version information.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetEventMaskCmd( uint8 *pEventMask )
{
  hciStatus_t status;

  // check parameters
  if ( pEventMask != NULL )
  {
    // set the BLE event mask
    for ( uint8 i=0; i<B_EVENT_MASK_LEN; i++ )
    {
      pBleEvtMask[i] = pEventMask[i];
    }

    status = HCI_SUCCESS;
  }
  else // bad parameters
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_EVENT_MASK,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used by the Host to determine the maximum ACL data packet
 * size allowed by the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadBufSizeCmd( void )
{
  // 0: Status
  // 1: Data Packet Length LSB
  // 2: Data Packet Length MSB
  // 3: Buffer Size
  uint8 rtnParam[4];

  // status
  rtnParam[0] = HCI_SUCCESS;

#if defined( CC26XX ) || defined( CC13XX )
  // data packet length
  rtnParam[1] = LO_UINT16( maximumPduSize );
  rtnParam[2] = HI_UINT16( maximumPduSize );

  // number of data packets allowed by Controller
  rtnParam[3] = maxNumTxDataBufs;

#else // CC254x
  // data packet length
  rtnParam[1] = LO_UINT16( HCI_DATA_MAX_DATA_LENGTH );
  rtnParam[2] = HI_UINT16( HCI_DATA_MAX_DATA_LENGTH );

  // number of data packets allowed by Controller
  rtnParam[3] = HCI_MAX_NUM_DATA_BUFFERS;

#endif // CC26XX/CC13XX

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_BUFFER_SIZE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to read the LE locally supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadLocalSupportedFeaturesCmd( void )
{
  // 0:    Status
  // 1..8: Local Supported Features
  uint8 rtnParam[9];

  rtnParam[0] = MAP_LL_ReadLocalSupportedFeatures( &(rtnParam[1]) );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_LOCAL_SUPPORTED_FEATURES,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to set this device's Random address.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetRandomAddressCmd( uint8 *pRandAddr )
{
  hciStatus_t status;

  // check parameters
  if ( pRandAddr != NULL )
  {
    status = MAP_LL_SetRandomAddress( pRandAddr );
  }
  else // bad parameters
  {
    status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
  }

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_RANDOM_ADDR,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * This LE API is used to set the Advertising parameters.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetAdvParamCmd( uint16  advIntervalMin,
                                   uint16  advIntervalMax,
                                   uint8   advType,
                                   uint8   ownAddrType,
                                   uint8   directAddrType,
                                   uint8  *directAddr,
                                   uint8   advChannelMap,
                                   uint8   advFilterPolicy )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetAdvParam( advIntervalMin,
                               advIntervalMax,
                               advType,
                               ownAddrType,
                               directAddrType,
                               directAddr,
                               advChannelMap,
                               advFilterPolicy );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_ADV_PARAM, sizeof(status), &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // ADV_NCONN_CFG | ADV_CONN_CFG


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * This LE API is used to set the Advertising data.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetAdvDataCmd( uint8  dataLen,
                                  uint8 *pData )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetAdvData( dataLen, pData );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_ADV_DATA, sizeof(status), &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}

/*******************************************************************************
 * This LE API is used to set the Advertising data.
 *
 * Public function defined in hci.h.
 */
 hciStatus_t HCI_LE_SetExtAdvData( aeSetDataCmd_t *pCmdParams )
{
  hciStatus_t status;

  status = LE_SetExtAdvData(pCmdParams);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_ADV_DATA, sizeof(status), &status );

  return status;
}

/*******************************************************************************
 * This LE API is used to set the Advertising Scan Response data.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetExtScanRspData( aeSetDataCmd_t *pCmdParams )
{
  hciStatus_t status;

  status = LE_SetExtScanRspData(pCmdParams);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_SCAN_RSP_DATA, sizeof(status), &status );

  return status;
}

hciStatus_t HCI_LE_SetAdvStatus( aeEnableCmd_t *pCmdParams )
{
  hciStatus_t status = LE_SetExtAdvEnable(pCmdParams);

  if( pCmdParams->enable == LL_ADV_MODE_ON )
  {
    MAP_HCI_CommandCompleteEvent( HCI_LE_MAKE_DISCOVERABLE_DONE, sizeof(status), &status );
  }
  else
  {
    MAP_HCI_CommandCompleteEvent( HCI_LE_END_DISCOVERABLE_DONE, sizeof(status), &status );
  }

  return status;
}
#endif // ADV_NCONN_CFG | ADV_CONN_CFG

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * This LE API is used to set the Advertising Scan Response data.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetScanRspDataCmd( uint8  dataLen,
                                      uint8 *pData )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetScanRspData( dataLen,
                                  pData );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_SCAN_RSP_DATA,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // ADV_NCONN_CFG | ADV_CONN_CFG


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * This LE API is used to turn Advertising on or off.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetAdvEnableCmd( uint8 advEnable )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetAdvControl( advEnable );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_ADV_ENABLE,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // ADV_NCONN_CFG | ADV_CONN_CFG


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * This LE API is used to read transmit power when Advertising.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadAdvChanTxPowerCmd( void )
{
  // 0: Status
  // 1: Advertising Transmit Power
  uint8 rtnParam[2];

  // status
  rtnParam[0] = MAP_LL_ReadAdvChanTxPower( (int8*)&(rtnParam[1]) );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_ADV_CHANNEL_TX_POWER,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_NCONN_CFG | ADV_CONN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * This LE API is used to set the Scan parameters.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetScanParamCmd( uint8  scanType,
                                    uint16 scanInterval,
                                    uint16 scanWindow,
                                    uint8  ownAddrType,
                                    uint8  filterPolicy )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetScanParam( scanType,
                                scanInterval,
                                scanWindow,
                                ownAddrType,
                                filterPolicy );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_SCAN_PARAM,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // SCAN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * This LE API is used to turn Scanning on or off.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetScanEnableCmd( uint8 scanEnable,
                                     uint8 filterDuplicates )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_SetScanControl( scanEnable,
                                  filterDuplicates );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_SCAN_ENABLE,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // SCAN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * This LE API is used to create a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_CreateConnCmd( uint16  scanInterval,
                                  uint16  scanWindow,
                                  uint8   initFilterPolicy,
                                  uint8   addrTypePeer,
                                  uint8  *peerAddr,
                                  uint8   ownAddrType,
                                  uint16  connIntervalMin,
                                  uint16  connIntervalMax,
                                  uint16  connLatency,
                                  uint16  connTimeout,
                                  uint16  minLen,
                                  uint16  maxLen )
{
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
  return( LL_STATUS_ERROR_COMMAND_DISALLOWED );
#else // !AE_CFG
  hciStatus_t status;

  status = MAP_LL_CreateConn( scanInterval,
                              scanWindow,
                              initFilterPolicy,
                              addrTypePeer,
                              peerAddr,
                              ownAddrType,
                              connIntervalMin,
                              connIntervalMax,
                              connLatency,
                              connTimeout,
                              minLen,
                              maxLen );

  MAP_HCI_CommandStatusEvent( status, HCI_LE_CREATE_CONNECTION );

  return( HCI_SUCCESS );
#endif // AE_CFG
}
#endif // INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
/*******************************************************************************
 * This LE API is used to cancel a create connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_CreateConnCancelCmd( void )
{
  hciStatus_t status;

  status = MAP_LL_CreateConnCancel();

  MAP_HCI_CommandCompleteEvent( HCI_LE_CREATE_CONNECTION_CANCEL,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // INIT_CFG


/*******************************************************************************
 * This LE API is used to read the total number of white list entries that can
 * be stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadWhiteListSizeCmd( void )
{
  // 0: Status
  // 1: White List Size
  uint8 rtnParam[2];

  rtnParam[0] = MAP_LL_ReadWlSize( &(rtnParam[1]) );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_WHITE_LIST_SIZE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to clear the white list.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ClearWhiteListCmd( void )
{
  hciStatus_t status;

  status = MAP_LL_ClearWhiteList();

  MAP_HCI_CommandCompleteEvent( HCI_LE_CLEAR_WHITE_LIST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to add a white list entry.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_AddWhiteListCmd( uint8  addrType,
                                    uint8 *devAddr )
{
  hciStatus_t status;

  status = MAP_LL_AddWhiteListDevice( devAddr,
                                      addrType );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ADD_WHITE_LIST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to remove a white list entry.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoveWhiteListCmd( uint8 addrType,
                                       uint8 *devAddr )
{
  hciStatus_t status;

  status = MAP_LL_RemoveWhiteListDevice( devAddr,
                                         addrType );

  MAP_HCI_CommandCompleteEvent( HCI_LE_REMOVE_WHITE_LIST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to update the connection parameters.
 *
 * Public function defined in hci.h.
 */
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
hciStatus_t HCI_LE_ConnUpdateCmd( uint16 connHandle,
                                  uint16 connIntervalMin,
                                  uint16 connIntervalMax,
                                  uint16 connLatency,
                                  uint16 connTimeout,
                                  uint16 minLen,
                                  uint16 maxLen )
{
  hciStatus_t status;

#if defined(CC26XX) || (!defined(CC26XX) && (CTRL_CONFIG & INIT_CFG)) ||       \
    defined(CC13XX) || (!defined(CC13XX) && (CTRL_CONFIG & INIT_CFG))
  status = MAP_LL_ConnUpdate( connHandle,
                              connIntervalMin,
                              connIntervalMax,
                              connLatency,
                              connTimeout,
                              minLen,
                              maxLen );

#else // !CC26XX/!13XX & CTRL_CONIFG=ADV_CONN_CFG
  status = HCI_ERROR_CODE_CMD_DISALLOWED;
#endif // CC26XX/13XX || (!CC26XX/!13XX & INIT_CFG)

  MAP_HCI_CommandStatusEvent( status, HCI_LE_CONNECTION_UPDATE );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to update the current data channel map.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetHostChanClassificationCmd( uint8 *chanMap )
{
  hciStatus_t status = LL_STATUS_ERROR_FEATURE_NOT_SUPPORTED;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  status = LL_SetSecAdvChanMap( chanMap );
#endif

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  status = MAP_LL_ChanMapUpdate( chanMap , maxNumConns );
#endif
  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // INIT_CFG

/*******************************************************************************
 * This EXT API is used to update the default channel map.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetHostDefChanClassificationCmd( uint8 *chanMap )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_HOST_DEF_CHANNEL_CLASSIFICATION_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_HOST_DEF_CHANNEL_CLASSIFICATION_EVENT );
  rtnParam[2] = LL_STATUS_ERROR_FEATURE_NOT_SUPPORTED;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  rtnParam[2] = LL_SetDefChanMap( chanMap );
#endif

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_HOST_DEFAULT_CHANNEL_CLASSIFICATION,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This EXT API is used to update the channel map of a specific connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetHostConnChanClassificationCmd( uint8 *chanMap , uint16 connID )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_HOST_CONN_CHANNEL_CLASSIFICATION_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_HOST_CONN_CHANNEL_CLASSIFICATION_EVENT );
  rtnParam[2] = LL_STATUS_ERROR_FEATURE_NOT_SUPPORTED;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  rtnParam[2] = MAP_LL_ChanMapUpdate( chanMap,  connID);
#endif

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_HOST_CONNECTION_CHANNEL_CLASSIFICATION,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to read a connection's data channel map.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadChannelMapCmd( uint16 connHandle )
{
  // 0:    Status
  // 1:    Connection Handle LSB
  // 2:    Connection Handle MSB
  // 3..7: Channel Map (LSB to MSB)
  uint8 rtnParam[8];

  rtnParam[0] = MAP_LL_ReadChanMap(  connHandle,
                                    &(rtnParam[3]) );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_CHANNEL_MAP,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to read the remote device's used features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadRemoteUsedFeaturesCmd( uint16 connHandle )
{
  hciStatus_t status;

  MAP_HCI_CommandStatusEvent( HCI_SUCCESS, HCI_LE_READ_REMOTE_USED_FEATURES );

  status = MAP_LL_ReadRemoteUsedFeatures( connHandle );

  // check if something went wrong
  // Note: If success is returned, then Command Complete is handled by Callback.
  if ( status != HCI_SUCCESS )
  {
    MAP_HCI_CommandCompleteEvent( HCI_LE_READ_REMOTE_USED_FEATURES,
                                  sizeof(status),
                                  &status );
  }

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This LE API is used to perform an encryption using AES128.
 *
 * Note: Input parameters are ordered LSB..MSB. This is done this way to be
 *       consistent with the transport layer, which is required to send the
 *       key and plaintext (which is given in MSO..LSO order) in LSO..MSO
 *       order. This means any direct function call to this routine must
 *       ensure the byte order is the same.
 *
 * Note: The byte reversing was originally done in hciLEEncrypt, which is now
 *       defunct (i.e. bypassed by ICall interface directly to this function).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EncryptCmd( uint8 *key,
                               uint8 *plainText )
{
  // 0:     Status
  // 1..16: Ciphertext Data
  uint8 rtnParam[17];

  // reverse byte order of key to MSO..LSO, as required by FIPS.
  MAP_HCI_ReverseBytes( &key[0], KEYLEN );

  // reverse byte order of plaintext to MSO..LSO, as required by FIPS.
  MAP_HCI_ReverseBytes( &plainText[0], KEYLEN );

  rtnParam[0] = MAP_LL_Encrypt(  key,
                                 plainText,
                                &rtnParam[1] );

  // check for success
  if ( rtnParam[0] == LL_STATUS_SUCCESS )
  {
    // reverse byte order of ciphertext to LSO..MSO for transport layer
    MAP_HCI_ReverseBytes( &rtnParam[1], KEYLEN );

    MAP_HCI_CommandCompleteEvent( HCI_LE_ENCRYPT, sizeof(rtnParam), rtnParam );
  }
  else // error
  {
    MAP_HCI_CommandCompleteEvent( HCI_LE_ENCRYPT, sizeof(uint8), rtnParam );
  }

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to generate a random number.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RandCmd( void )
{
  // 0:    Status
  // 1..8: Random Bytes
  uint8 rtnParam[B_RANDOM_NUM_SIZE+1];

  rtnParam[0] = MAP_LL_Rand( &rtnParam[1], B_RANDOM_NUM_SIZE );

  // check if the operation has been completed; if not, then it has been delayed
  // until a current radio operation completes as the radio is needed to
  // generate a true random number, or there was some kind of error
  if ( rtnParam[0] != LL_STATUS_ERROR_DUE_TO_DELAYED_RESOURCES )
  {
    // check if the operation was okay
    if ( rtnParam[0] == LL_STATUS_SUCCESS )
    {
      MAP_HCI_CommandCompleteEvent( HCI_LE_RAND,
                                    B_RANDOM_NUM_SIZE+1,
                                    rtnParam );
    }
    else // an error occurred
    {
      MAP_HCI_CommandCompleteEvent( HCI_LE_RAND, sizeof(uint8), rtnParam );
    }
  }

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * This LE API is used to start encryption in a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_StartEncyptCmd( uint16  connHandle,
                                   uint8  *random,
                                   uint8  *encDiv,
                                   uint8  *ltk )
{
  hciStatus_t status;

  status = MAP_LL_StartEncrypt( connHandle,
                                random,
                                encDiv,
                                ltk );

  MAP_HCI_CommandStatusEvent( status, HCI_LE_START_ENCRYPTION );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This LE API is used by the Host to send to the Controller a positive LTK
 * reply.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_LtkReqReplyCmd( uint16  connHandle,
                                   uint8  *ltk )
{
  // 0: Status
  // 1: Connection Handle (LSB)
  // 2: Connection Handle (MSB)
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_EncLtkReply( connHandle, ltk );
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_LTK_REQ_REPLY,
                                sizeof(rtnParam),
                                rtnParam );

  return ( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used by the Host to send to the Controller a negative LTK
 * reply.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_LtkReqNegReplyCmd( uint16 connHandle )
{
  // 0: Status
  // 1: Connection Handle (LSB)
  // 2: Connection Handle (MSB)
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_EncLtkNegReply( connHandle );
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_LTK_REQ_NEG_REPLY,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // INIT_CFG || ADV_CONN_CFG

/*******************************************************************************
 * This LE API is used to read the Controller's supported states.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadSupportedStatesCmd( void )
{
  // 0:    Status
  // 1..8: Supported States
  uint8 rtnParam[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  rtnParam[0] = MAP_LL_ReadSupportedStates( &rtnParam[1] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_SUPPORTED_STATES,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI API is used to get the Auhenticated Payload Timeout (APTO) value for
 * this connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadAuthPayloadTimeoutCmd( uint16  connHandle,
                                           uint16 *apto )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  // 3: APTO LSB
  // 4: APTO MSB
  uint8 rtnParam[5];

  rtnParam[0] = MAP_LL_ReadAuthPayloadTimeout( connHandle,
                                               (uint16 *)&rtnParam[3] );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_READ_AUTH_PAYLOAD_TIMEOUT,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI API is used to set the Auhenticated Payload Timeout (APTO) value for
 * this connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_WriteAuthPayloadTimeoutCmd( uint16 connHandle,
                                            uint16 aptoValue )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_WriteAuthPayloadTimeout( connHandle,
                                                aptoValue );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_WRITE_AUTH_PAYLOAD_TIMEOUT,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This LE API is used to positively reply to the HCI LE Remote Connection
 * Parameter Request event from the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoteConnParamReqReplyCmd( uint16 connHandle,
                                               uint16 connIntervalMin,
                                               uint16 connIntervalMax,
                                               uint16 connLatency,
                                               uint16 connTimeout,
                                               uint16 minLen,
                                               uint16 maxLen )
{
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_RemoteConnParamReqReply( connHandle,
                                                connIntervalMin,
                                                connIntervalMax,
                                                connLatency,
                                                connTimeout,
                                                minLen,
                                                maxLen );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_REMOTE_CONN_PARAM_REQ_REPLY,
                                sizeof(rtnParam),
                                rtnParam );


  return( HCI_SUCCESS );
#else
  return( HCI_ERROR_CODE_UNKNOWN_HCI_CMD );
#endif // (ADV_CONN_CFG | INIT_CFG)
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to negatively reply to the HCI LE Remote Connection
 * Parameter Request event from the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoteConnParamReqNegReplyCmd( uint16 connHandle,
                                                  uint8  reason )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_RemoteConnParamReqNegReply( connHandle,
                                                   reason );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_REMOTE_CONN_PARAM_REQ_NEG_REPLY,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


// V4.2 - Extended Data Length

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to set the maximum transmission packet size and the
 * maximum packet transmission time for the connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetDataLenCmd( uint16 connHandle,
                                  uint16 txOctets,
                                  uint16 txTime)
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LL_SetDataLen( connHandle,
                                   txOctets,
                                   txTime );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_DATA_LENGTH,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to read the default maximum trasmit packet size and
 * the default maximum packet transmit time to be used for new connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadSuggestedDefaultDataLenCmd( void )
{
  // 0: Status
  // 1: Initial Max Tx Octets LSB
  // 2: Initial Max Tx Octets MSB
  // 3: Initial Max Tx Time LSB
  // 4: Initial Max Tx Time MSB
  uint8 rtnParam[5];

  rtnParam[0] = MAP_LL_ReadDefaultDataLen( (uint16 *)&rtnParam[1],
                                           (uint16 *)&rtnParam[3] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to set the default maximum transmit packet size and the
 * default maximum transmit time to be used for new connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_WriteSuggestedDefaultDataLenCmd( uint16 txOctets,
                                                    uint16 txTime )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_WriteDefaultDataLen( txOctets,
                                            txTime );

  MAP_HCI_CommandCompleteEvent( HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to read the maximum supported transmit and receive
 * payload octets and packet duration times.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadMaxDataLenCmd( void )
{
  // 0: Status
  // 1: Supported Max Tx Octets LSB
  // 2: Supported Max Tx Octets MSB
  // 3: Supported Max Tx Time LSB
  // 4: Supported Max Tx Time MSB
  // 5: Supported Max Rx Octets LSB
  // 6: Supported Max Rx Octets MSB
  // 7: Supported Max Rx Time LSB
  // 8: Supported Max Rx Time MSB
  uint8 rtnParam[9];

  rtnParam[0] = MAP_LL_ReadMaxDataLen( (uint16 *)&rtnParam[1],
                                       (uint16 *)&rtnParam[3],
                                       (uint16 *)&rtnParam[5],
                                       (uint16 *)&rtnParam[7] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_MAX_DATA_LENGTH,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)


// V4.2 - Privacy 1.2

/*******************************************************************************
 * This LE API is used to add one device to the list of address translations
 * used to resolve Resolvable Private Addresses in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_AddDeviceToResolvingListCmd( uint8  peerIdAddrType,
                                                uint8 *peerIdAddr,
                                                uint8 *peerIRK,
                                                uint8 *localIRK )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_AddDeviceToResolvingList( peerIdAddrType,
                                                 peerIdAddr,
                                                 peerIRK,
                                                 localIRK );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to remove one device fromthe list of address
 * translations used to resolve Resolvable Private Addresses in the
 * Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoveDeviceFromResolvingListCmd( uint8  peerIdAddrType,
                                                     uint8 *peerIdAddr )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_RemoveDeviceFromResolvingList( peerIdAddrType,
                                                      peerIdAddr );

  MAP_HCI_CommandCompleteEvent( HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to remove all devices from the list of address
 * translations used to resolve Resolvable Private addresses in the
 * Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ClearResolvingListCmd( void )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_ClearResolvingList();

  MAP_HCI_CommandCompleteEvent( HCI_LE_CLEAR_RESOLVING_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to read the total number of address translation
 * entries in the resolving list that can be stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadResolvingListSizeCmd( void )
{
  // 0: Status
  // 1: Resolving List Size
  uint8 rtnParam[2];

  rtnParam[0] = MAP_LL_ReadResolvingListSize( &rtnParam[1] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_RESOLVING_LIST_SIZE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to get the current peer Resolvable Private Address
 * being used for the corresponding peer Public or Random (Static)
 * Identity Address.
 *
 * Note: The peer's Resolvable Private Address being used may change after
 *       this command is called.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadPeerResolvableAddressCmd( uint8  peerIdAddrType,
                                                 uint8 *peerIdAddr )
{
  // 0:    Status
  // 1..6: Peer Resolvable Address
  uint8 rtnParam[7];

  rtnParam[0] = MAP_LL_ReadPeerResolvableAddress(  peerIdAddrType,
                                                   peerIdAddr,
                                                  &rtnParam[1] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_PEER_RESOLVABLE_ADDRESS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to get the current local Resolvable Private Address
 * being used for the corresponding local Public or Random (Static)
 * Identity Address.
 *
 * Note: The local Resolvable Private Address being used may change after
 *       this command is called.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadLocalResolvableAddressCmd( uint8  localIdAddrType,
                                                  uint8 *localIdAddr )
{
  // 0:    Status
  // 1..6: Local Resolvable Address
  uint8 rtnParam[7];

  rtnParam[0] = MAP_LL_ReadLocalResolvableAddress(  localIdAddrType,
                                                    localIdAddr,
                                                   &rtnParam[1] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to enable resolution of Resolvable Private Addresses
 * in the Controller. This causes the Controller to use the resolving
 * list whenever the Controller receives a local or peer Resolvable Private
 * Address.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetAddressResolutionEnableCmd( uint8 addrResolutionEnable )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_SetAddressResolutionEnable( addrResolutionEnable );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to set the length of time the Controller uses a
 * Resolvable Private Address before a new Resolvable Private Address is
 * generated and starts being used. Note that this timeout applies to all
 * addresses generated by the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetResolvablePrivateAddressTimeoutCmd( uint16 rpaTimeout )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_SetResolvablePrivateAddressTimeout( rpaTimeout );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is called by the HCI to set the length of time the Controller
 * uses a Resolvable Private Address before a new Resolvable Private Address is
 * generated and starts being used. Note that this timeout applies to all
 * addresses generated by the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPrivacyModeCmd( uint8  peerIdAddrType,
                                      uint8 *peerIdAddr,
                                      uint8  privacyMode )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LL_SetPrivacyMode( peerIdAddrType,
                                       peerIdAddr,
                                       privacyMode );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_PRIVACY_MODE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


// V4.2 - Secure Connections

/*******************************************************************************
 * This LE API is used to read the local P-256 public key from the Controller.
 * The Controller shall generate a new P-256 public/private key pair upon
 * receipt of this command.
 *
 * Note: Generates LE Read Local P256 Public Key Complete event.
 *
 * WARNING: THIS ROUTINE WILL TIE UP THE LL FOR ABOUT 160ms!
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadLocalP256PublicKeyCmd( void )
{
  // send Command Status first
  // Note: Just indicate success here, and if there's an ECC error, it will be
  //       reflected in the Complete event status.
  MAP_HCI_CommandStatusEvent( HCI_SUCCESS,
                              HCI_LE_READ_LOCAL_P256_PUBLIC_KEY );

  // generate the Public P256 key
  MAP_LL_ReadLocalP256PublicKeyCmd();

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to initiate the generation of a Diffie-hellman key in
 * the Controller for use over the LE transport. This command takes the remote
 * P-256 public key as input. The Diffie-Hellman key generation uses the
 * private key generated by LE_Read_Local_P256_Public_Key command.
 *
 * Note: Generates LE DHKey Generation Complete event.
 *
 * WARNING: THIS ROUTINE WILL TIE UP THE LL FOR ABOUT 160ms!
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_GenerateDHKeyCmd( uint8 *publicKey )
{
  // send Command Status first
  // Note: Just indicate success here, and if there's an ECC error, it will be
  //       reflected in the Complete event status.
  MAP_HCI_CommandStatusEvent( HCI_SUCCESS,
                              HCI_LE_GENERATE_DHKEY );

  // generate the Public P256 key
  MAP_LL_GenerateDHKeyCmd( publicKey );

  return( HCI_SUCCESS );
}


// V5.0 - 2M and Coded PHY

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/**
 * This LE API is used to Read the current transmitter and receiver PHY.
 *
 * Sends hciEvt_CmdComplete_t with cmdOpcode HCI_LE_READ_PHY
 *       and Return Parameters of -   0: Status
 *                                    1: Connection Handle LSB
 *                                    2: Connection Handle MSB
 *                                    3: Tx PHY
 *                                    4: Rx PHY
 *
 * @param connHandle Connection handle.
 *
 * @return HCI_SUCCESS
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadPhyCmd( uint16 connHandle )
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  // 3: Tx PHY
  // 4: Rx PHY
  uint8 rtnParam[5];

  rtnParam[0] = MAP_LL_ReadPhy( connHandle,
                                &rtnParam[3],
                                &rtnParam[4] );

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_PHY,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API allows the Host to specify its preferred values for the
 * transmitter and receiver PHY to be used for all subsequent connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetDefaultPhyCmd( uint8 allPhys,
                                     uint8 txPhy,
                                     uint8 rxPhy )
{
  hciStatus_t status;

  status = MAP_LL_SetDefaultPhy( allPhys,
                                 txPhy,
                                 rxPhy );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_DEFAULT_PHY,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to request a change to the transmitter and receiver PHY
 * for a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPhyCmd( uint16 connHandle,
                              uint8  allPhys,
                              uint8  txPhy,
                              uint8  rxPhy,
                              uint16 phyOpts )
{
  hciStatus_t status;

  status = MAP_LL_SetPhy( connHandle,
                          allPhys,
                          txPhy,
                          rxPhy,
                          phyOpts );

  // check if the LL determined that neither tx or rx PHY will change as a
  // result of this command
  if ( status == LL_STATUS_ERROR_REPEATED_ATTEMPTS )
  {
    uint8 tx, rx;

    // get the current tx/rx phy
    MAP_LL_ReadPhy( connHandle,
                    &tx,
                    &rx );

    // issue a successful command status
    MAP_HCI_CommandStatusEvent( HCI_SUCCESS,
                                HCI_LE_SET_PHY );

    // issue a phy change update event
    MAP_LL_PhyUpdateCompleteEventCback( HCI_SUCCESS,
                                        connHandle,
                                        tx,
                                        rx );
  }
  else if ( status == LL_STATUS_ERROR_TRANSACTION_COLLISION )
  {
    // issue a successful command status
    MAP_HCI_CommandStatusEvent( HCI_SUCCESS,
                                HCI_LE_SET_PHY );

    MAP_LL_PhyUpdateCompleteEventCback( LL_STATUS_ERROR_TRANSACTION_COLLISION,
                                        connHandle,
                                        0,
                                        0 );
  }
  else // send command status as is
  {
    MAP_HCI_CommandStatusEvent( status,
                                HCI_LE_SET_PHY );
  }

  return( HCI_SUCCESS );
}
#endif // (PHY_2MBPS_CFG | PHY_LR_CFG) & (ADV_CONN_CFG | INIT_CFG)


/*******************************************************************************
 * This LE API is used to start the transmit Direct Test Mode test.
 * The Controller shall transmit at maximum power.
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_TransmitterTestCmd( uint8 txChan,
                                       uint8 dataLen,
                                       uint8 payloadType )
{
  hciStatus_t status;

#if defined( CC26XX ) || defined( CC13XX )
  status = MAP_LL_DirectTestTxTest( txChan,
                                    dataLen,
                                    payloadType,
                                    LL_DTM_TX_1_MBPS );
#else // !CC26XX/CC13XX
  status = MAP_LL_DirectTestTxTest( txChan,
                                    dataLen,
                                    payloadType );
#endif // CC26XX/CC13XX

  MAP_HCI_CommandCompleteEvent( HCI_LE_TRANSMITTER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This LE API is used to start the receiver Direct Test Mode test.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReceiverTestCmd( uint8 rxChan )
{
  hciStatus_t status;

#if defined( CC26XX ) || defined( CC13XX )
  status = MAP_LL_DirectTestRxTest( rxChan,
                                    LL_DTM_TX_1_MBPS );
#else // !CC26XX/CC13XX
  status = MAP_LL_DirectTestRxTest( rxChan );
#endif // CC26XX/CC13XX

  MAP_HCI_CommandCompleteEvent( HCI_LE_RECEIVER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * This LE API is used to start a test where the DUT receives reference packets
 * at a fixed interval. The tester generates the test reference packets.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EnhancedRxTestCmd( uint8 rxChan,
                                      uint8 rxPhy,
                                      uint8 modIndex )
{
  hciStatus_t status;

  status = MAP_LL_EnhancedRxTest( rxChan,
                                  rxPhy,
                                  modIndex );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ENHANCED_RECEIVER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * This LE API is used to start a test where the DUT generates test reference
 * packets at a fixed interval. The Controller shall transmit at maximum power.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EnhancedTxTestCmd( uint8 txChan,
                                      uint8 payloadLen,
                                      uint8 payloadType,
                                      uint8 txPhy )
{
  hciStatus_t status;

  status = MAP_LL_EnhancedTxTest( txChan,
                                  payloadLen,
                                  payloadType,
                                  txPhy );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ENHANCED_TRANSMITTER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * This LE API is used to start a test where the DUT receives reference packets
 * at a fixed interval. The tester generates the test reference packets.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EnhancedCteRxTestCmd( uint8 rxChan,
                                         uint8 rxPhy,
                                         uint8 modIndex,
                                         uint8 expectedCteLength,
                                         uint8 expectedCteType,
                                         uint8 slotDurations,
                                         uint8 length,
                                         uint8 *pAntenna)
{
  hciStatus_t status;

  status = MAP_LL_EnhancedCteRxTest( rxChan,
                                     rxPhy,
                                     modIndex,
                                     expectedCteLength,
                                     expectedCteType,
                                     slotDurations,
                                     length,
                                     pAntenna );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ENHANCED_CTE_RECEIVER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG


#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
/*******************************************************************************
 * This LE API is used to start a test where the DUT generates test reference
 * packets at a fixed interval. The Controller shall transmit at maximum power.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_EnhancedCteTxTestCmd( uint8 txChan,
                                         uint8 payloadLen,
                                         uint8 payloadType,
                                         uint8 txPhy,
                                         uint8 cteLength,
                                         uint8 cteType,
                                         uint8 length,
                                         uint8 *pAntenna)
{
  hciStatus_t status;

  status = MAP_LL_EnhancedCteTxTest( txChan,
                                     payloadLen,
                                     payloadType,
                                     txPhy,
                                     cteLength,
                                     cteType,
                                     length,
                                     pAntenna );

  MAP_HCI_CommandCompleteEvent( HCI_LE_ENHANCED_CTE_TRANSMITTER_TEST,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

/*******************************************************************************
 * This LE API is used to end the Direct Test Mode test.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_TestEndCmd( void )
{
  hciStatus_t status = MAP_LL_DirectTestEnd();

  if ( status != HCI_SUCCESS )
  {
    // 0:    Status
    // 1..2: Number of Packets (for Receive DTM only)
    uint8 rtnParam[3];

    rtnParam[0] = HCI_ERROR_CODE_CMD_DISALLOWED;

    // not valid if LL API failed; otherwise values returned by a
    // MAP_LL_DirectTestEndDoneCback event
    rtnParam[1] = 0;
    rtnParam[2] = 0;

    MAP_HCI_CommandCompleteEvent( HCI_LE_TEST_END,
                                  sizeof(rtnParam),
                                  rtnParam );
  }

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This LE API is used to read the min/max Tx power.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadTxPowerCmd( void )
{
  // 0: Status
  // 1: Minimum Tx Power
  // 2: Maximum Tx Power
  uint8 rtnParam[3];

  rtnParam[0] = MAP_LE_ReadTxPowerCmd( (int8 *)&rtnParam[1],
                                       (int8 *)&rtnParam[2] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_TX_POWER,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This function is used to read the RF Path Compensation Values (in 0.1 dBm)
 * parameter used in the Tx Power Level and RSSI calculation.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadRfPathCompCmd( void )
{
  // 0: Status
  // 1: RF Tx Path Compensation LSB
  // 2: RF Tx Path Compensation MSB
  // 3: RF Rx Path Compensation LSB
  // 4: RF Rx Path Compensation MSB
  uint8 rtnParam[5];

  rtnParam[0] = MAP_LE_ReadRfPathCompCmd( (int16 *)&rtnParam[1],
                                          (int16 *)&rtnParam[3] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_RF_PATH_COMPENSATION,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This function is used to indicate the RF path gain or loss (in 0.1 dBm)
 * between the RF transceiver and the antenna contributed by intermediate
 * components. A positive value means a net RF path gain and a negative value
 * means a net RF path loss.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_WriteRfPathCompCmd( int16 txPathParam,
                                       int16 rxPathParam )
{
  // 0: Status
  uint8 rtnParam[1];

  rtnParam[0] = MAP_LE_WriteRfPathCompCmd( txPathParam,
                                           rxPathParam );

  MAP_HCI_CommandCompleteEvent( HCI_LE_WRITE_RF_PATH_COMPENSATION,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_CONN_CFG) || (CTRL_CONFIG & INIT_CFG))
/*******************************************************************************
 * Used to enable or disable sampling received Constant Tone Extension fields on a
 * connection and to set the antenna switching pattern and switching and sampling slot 
 * durations to be used.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionCteReceiveParamsCmd( uint16 connHandle,
                                                     uint8 samplingEnable,
                                                     uint8 slotDurations,
                                                     uint8 length,
                                                     uint8 *pAntenna)
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  // status
  rtnParam[0] = MAP_LL_SetConnectionCteReceiveParams(connHandle,
                                                     samplingEnable,
                                                     slotDurations,
                                                     length,
                                                     pAntenna);

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * Set the antenna switching pattern and permitted Constant Tone Extension types used 
 * for transmitting Constant Tone Extensions requested by the peer device on a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionCteTransmitParamsCmd( uint16 connHandle,
                                                      uint8 types,
                                                      uint8 length,
                                                      uint8 *pAntenna)
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  // status
  rtnParam[0] = MAP_LL_SetConnectionCteTransmitParams(connHandle,
                                                      types,
                                                      length,
                                                      pAntenna);

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Start or stop initiating the CTE Request procedure on a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionCteRequestEnableCmd( uint16 connHandle,
                                                     uint8 enable,
                                                     uint16 interval,
                                                     uint8 length,
                                                     uint8 type)
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  // status
  rtnParam[0] = MAP_LL_SetConnectionCteRequestEnable(connHandle,
                                                     enable,
                                                     interval,
                                                     length,
                                                     type);

  // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTION_CTE_REQUEST_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * Set a respond to LL_CTE_REQ PDUs with LL_CTE_RSP PDUs on a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionCteResponseEnableCmd( uint16 connHandle,
                                                      uint8 enable)
{
  // 0: Status
  // 1: Connection Handle LSB
  // 2: Connection Handle MSB
  uint8 rtnParam[3];

  // status
  rtnParam[0] = MAP_LL_SetConnectionCteResponseEnable(connHandle,
                                                      enable);

    // connection handle
  rtnParam[1] = LO_UINT16( connHandle );
  rtnParam[2] = HI_UINT16( connHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTION_CTE_RESPONSE_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Set a respond to LL_CTE_REQ PDUs with LL_CTE_RSP PDUs on a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadAntennaInformationCmd( void )
{
  // 0: Status
  // 1: supported sample rates
  // 2: Number of antennas
  // 3: Max length of antenna switching pattern
  // 4: Max CTE length
  uint8 rtnParam[5];

  // status
  rtnParam[0] = MAP_LL_ReadAntennaInformation(&rtnParam[1],
                                              &rtnParam[2],
                                              &rtnParam[3],
                                              &rtnParam[4]);

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_ANTENNA_INFORMATION,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)

#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
/*******************************************************************************
 * Used by the Host to set the advertiser parameters for periodic advertising.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPeriodicAdvParamsCmd( uint8 advHandle,
                                            uint16 periodicAdvIntervalMin,
                                            uint16 periodicAdvIntervalMax,
                                            uint16 periodicAdvProp )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetPeriodicAdvParams(advHandle,
                                            periodicAdvIntervalMin,
                                            periodicAdvIntervalMax,
                                            periodicAdvProp);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_PERIODIC_ADV_PARAMETERS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used to set the advertiser data used in periodic advertising PDUs.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPeriodicAdvDataCmd( uint8 advHandle,
                                          uint8 operation,
                                          uint8 dataLength,
                                          uint8 *data )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetPeriodicAdvData(advHandle,
                                          operation,
                                          dataLength,
                                          data);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_PERIODIC_ADV_DATA,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used to request the advertiser to enable or disable
 * the periodic advertising for the advertising set
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPeriodicAdvEnableCmd( uint8 enable,
                                            uint8 advHandle )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetPeriodicAdvEnable(enable,
                                            advHandle);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_PERIODIC_ADV_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used by the Host to set the type, length, and antenna switching pattern
 * for the transmission of Constant Tone Extensions in any periodic advertising.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionlessCteTransmitParamsCmd( uint8 advHandle,
                                                          uint8 cteLen,
                                                          uint8 cteType,
                                                          uint8 cteCount,
                                                          uint8 length,
                                                          uint8 *pAntenna)
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetConnectionlessCteTransmitParams(advHandle,
                                                          cteLen,
                                                          cteType,
                                                          cteCount,
                                                          length,
                                                          pAntenna);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMS,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used by the Host to request that the Controller enables or disables the use
 * of Constant Tone Extensions in any periodic advertising.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionlessCteTransmitEnableCmd( uint8 advHandle,
                                                          uint8 enable)
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetConnectionlessCteTransmitEnable(advHandle,
                                                          enable);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

#endif

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * Used a scanner to synchronize with a periodic advertising train from
 * an advertiser and begin receiving periodic advertising packets.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_PeriodicAdvCreateSyncCmd( uint8  options,
                                             uint8  advSID,
                                             uint8  advAddrType,
                                             uint8  *advAddress,
                                             uint16 skip,
                                             uint16 syncTimeout,
                                             uint8  syncCteType )
{
  hciStatus_t status;

  // status
  status = MAP_LE_PeriodicAdvCreateSync( options,
                                     advSID,
                                     advAddrType,
                                     advAddress,
                                     skip,
                                     syncTimeout,
                                     syncCteType );

  MAP_HCI_CommandStatusEvent( status, HCI_LE_PERIODIC_ADV_CREATE_SYNC );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to cancel the HCI_LE_Periodic_Advertising_Create_Sync
 * command while it is pending.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_PeriodicAdvCreateSyncCancelCmd( void )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_PeriodicAdvCreateSyncCancel();

  MAP_HCI_CommandCompleteEvent( HCI_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to stop reception of the periodic advertising
 * train identified by the syncHandle parameter.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_PeriodicAdvTerminateSyncCmd( uint16 syncHandle )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_PeriodicAdvTerminateSync(syncHandle);

  MAP_HCI_CommandCompleteEvent( HCI_LE_PERIODIC_ADV_TERMINATE_SYNC,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to add an entry, consisting of a single device address
 * and SID, to the Periodic Advertiser list stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_AddDeviceToPeriodicAdvListCmd( uint8 advAddrType,
                                                  uint8 *advAddress,
                                                  uint8 advSID )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_AddDeviceToPeriodicAdvList(advAddrType,
                                              advAddress,
                                              advSID);

  MAP_HCI_CommandCompleteEvent( HCI_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to remove one entry from the list of Periodic Advertisers
 * stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_RemoveDeviceFromPeriodicAdvListCmd( uint8 advAddrType,
                                                       uint8 *advAddress,
                                                       uint8 advSID )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_RemoveDeviceFromPeriodicAdvList(advAddrType,
                                                   advAddress,
                                                   advSID);

  MAP_HCI_CommandCompleteEvent( HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to remove all entries from the list of Periodic
 * Advertisers in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ClearPeriodicAdvListCmd( void )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_ClearPeriodicAdvList();

  MAP_HCI_CommandCompleteEvent( HCI_LE_CLEAR_PERIODIC_ADV_LIST,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to read the total number of Periodic Advertiser
 * list entries that can be stored in the Controller.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_ReadPeriodicAdvListSizeCmd( void )
{
  // 0: Status
  // 1: List Size
  uint8 rtnParam[2];

  rtnParam[0] = MAP_LE_ReadPeriodicAdvListSize( &rtnParam[1] );

  MAP_HCI_CommandCompleteEvent( HCI_LE_READ_PERIODIC_ADV_LIST_SIZE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used a scanner to enable or disable reports for the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetPeriodicAdvReceiveEnableCmd( uint16 syncHandle,
                                                   uint8  enable )
{
  // 0: Status
  uint8 rtnParam[1];

  // status
  rtnParam[0] = MAP_LE_SetPeriodicAdvReceiveEnable(syncHandle,
                                               enable);

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_PERIODIC_ADV_RECEIVE_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * Used by the Host to request that the Controller enables or disables capturing
 * IQ samples from the CTE of periodic advertising packets in the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_LE_SetConnectionlessIqSamplingEnableCmd( uint16 syncHandle,
                                                         uint8 samplingEnable,
                                                         uint8 slotDurations,
                                                         uint8 maxSampledCtes,
                                                         uint8 length,
                                                         uint8 *pAntenna)
{
  // 0: Status
  // 1: Periodic Scan Handle LSB
  // 2: Periodic Scan Handle MSB
  uint8 rtnParam[3];

  // status
  rtnParam[0] = MAP_LE_SetConnectionlessIqSamplingEnable(syncHandle,
                                                         samplingEnable,
                                                         slotDurations,
                                                         maxSampledCtes,
                                                         length,
                                                         pAntenna);

  // periodic scan handle
  rtnParam[1] = LO_UINT16( syncHandle );
  rtnParam[2] = HI_UINT16( syncHandle );

  MAP_HCI_CommandCompleteEvent( HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE,
                                sizeof(rtnParam),
                                rtnParam );

  return( HCI_SUCCESS );
}
#endif

/*
** HCI Vendor Specific Comamnds: Link Layer Extensions
*/


/*******************************************************************************
 * This HCI Extension API is used to set the receiver gain.
 *
 * Note: If the LL can not perform the command immediately, the HCI will be
 *       notified by a corresonding LL callback.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetRxGainCmd( uint8 rxGain )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];
  uint8 cmdComplete = TRUE;

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_RX_GAIN_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_RX_GAIN_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetRxGain( rxGain, &cmdComplete );

  // check if the command was performed, or if it was delayed
  // Note: If delayed, a callback will be generated by the LL.
  if ( cmdComplete == TRUE )
  {
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_RX_GAIN,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to set the transmit power.
 *
 * Note: If the LL can not perform the command immediately, the HCI will be
 *       notified by a corresonding LL callback.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetTxPowerCmd( uint8 txPower )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];
  uint8 cmdComplete = TRUE;

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_TX_POWER_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_TX_POWER_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetTxPower( txPower, &cmdComplete );

  // check if the command was performed, or if it was delayed
  // Note: If delayed, a callback will be generated by the LL.
  if ( cmdComplete == TRUE )
  {
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_TX_POWER,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to set whether a connection will be limited
 * to one packet per event.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_OnePktPerEvtCmd( uint8 control )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_ONE_PKT_PER_EVT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_ONE_PKT_PER_EVT_EVENT );
  rtnParam[2] = MAP_LL_EXT_OnePacketPerEvent( control );

  // check if LL indicates the internal state of this feature is not being
  // changed by this command
  // Note: This is an internal status that only exists between the LL and HCI.
  //       It is being used here to basically suppress unnecessary events,
  //       allowing the application to repeatedly call this API without
  //       resulting in excessive events flooding the system.
  if ( rtnParam[2] != HCI_STATUS_WARNING_FLAG_UNCHANGED )
  {
    // the internal state of this feature has changed, so return event
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_ONE_PKT_PER_EVT,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This HCI Extension API is used to set whether the system clock will be
 * divided when the MCU is halted.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ClkDivOnHaltCmd( uint8 control )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_CLK_DIVIDE_ON_HALT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_CLK_DIVIDE_ON_HALT_EVENT );
  rtnParam[2] = MAP_LL_EXT_ClkDivOnHalt( control );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_CLK_DIVIDE_ON_HALT,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to indicate to the Controller whether or not
 * the Host will be using the NV memory during BLE operations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DeclareNvUsageCmd( uint8 mode )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_DECLARE_NV_USAGE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_DECLARE_NV_USAGE_EVENT );
  rtnParam[2] = MAP_LL_EXT_DeclareNvUsage( mode );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_DECLARE_NV_USAGE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to decrypt encrypted data using AES128.
 *
 * Note: Input parameters are ordered LSB..MSB. This is done this way to be
 *       consistent with the transport layer, which is required to send the
 *       key and plaintext (which is given in MSO..LSO order) in LSO..MSO
 *       order. This means any direct function call to this routine must
 *       ensure the byte order is the same.
 *
 * Note: The byte reversing was originally done in hciExtDecrypt, which is now
 *       defunct (i.e. bypassed by ICall interface directly to this function).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DecryptCmd( uint8 *key,
                                uint8 *encText )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  // 3..18: Plaintext Data
  uint8 rtnParam[19];

  rtnParam[0] = LO_UINT16( HCI_EXT_DECRYPT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_DECRYPT_EVENT );

  // reverse byte order of key to MSO..LSO, as required by FIPS.
  MAP_HCI_ReverseBytes( &key[0], KEYLEN );

  // reverse byte order of ciphertext to MSO..LSO, as required by FIPS.
  MAP_HCI_ReverseBytes( &encText[0], KEYLEN );

  rtnParam[2] = MAP_LL_EXT_Decrypt(  key,
                                     encText,
                                    &rtnParam[3] );

  // check if okay
  if ( rtnParam[2] == LL_STATUS_SUCCESS )
  {
    // reverse byte order of plaintext to LSO..MSO for transport layer
    MAP_HCI_ReverseBytes( &rtnParam[3], KEYLEN );

    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_DECRYPT,
                                               sizeof(rtnParam),
                                               rtnParam );
  }
  else // bad parameters
  {
    rtnParam[2] = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;

    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_DECRYPT,
                                               sizeof(uint8)+2,
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to write this devie's supported features.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetLocalSupportedFeaturesCmd( uint8 *localFeatures )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetLocalSupportedFeatures( localFeatures );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * This HCI Extension API is used to set whether transmit data is sent as soon
 * as possible even when slave latency is used.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetFastTxResponseTimeCmd( uint8 control )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_FAST_TX_RESP_TIME_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_FAST_TX_RESP_TIME_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetFastTxResponseTime( control );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_FAST_TX_RESP_TIME,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
/*******************************************************************************
 * This HCI Extension API is used to enable or disable suspending slave
 * latency.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetSlaveLatencyOverrideCmd( uint8 control )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_OVERRIDE_SL_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_OVERRIDE_SL_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetSlaveLatencyOverride( control );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_OVERRIDE_SL,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG


/*******************************************************************************
 * This API is used start a continuous transmitter modem test, using either
 * a modulated or unmodulated carrier wave tone, at the frequency that
 * corresponds to the specified RF channel. Use HCI_EXT_EndModemTest command
 * to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTest command!
 * Note: The BLE device will transmit at maximum power.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemTestTxCmd( uint8 cwMode,
                                    uint8 txChan )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_MODEM_TEST_TX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_MODEM_TEST_TX_EVENT );
  rtnParam[2] = MAP_LL_EXT_ModemTestTx( cwMode, txChan );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_MODEM_TEST_TX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used to start a continuous transmitter direct test mode test
 * using a modulated carrier wave and transmitting a 37 byte packet of
 * Pseudo-Random 9-bit data. A packet is transmitted on a different frequency
 * (linearly stepping through all RF channels 0..39) every 625us. Use
 * HCI_EXT_EndModemTest to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTest command!
 * Note: The BLE device will transmit at maximum power.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemHopTestTxCmd( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_MODEM_HOP_TEST_TX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_MODEM_HOP_TEST_TX_EVENT );
  rtnParam[2] = MAP_LL_EXT_ModemHopTestTx();

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_MODEM_HOP_TEST_TX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used to start a continuous receiver modem test using a modulated
 * carrier wave tone, at the frequency that corresponds to the specific RF
 * channel. Any received data is discarded. Receiver gain may be adjusted using
 * the HCI_EXT_SetRxGain command. RSSI may be read during this test by using the
 * HCI_ReadRssi command. Use the HCI_EXT_EndModemTest command to end the test.
 *
 * Note: A Controller reset will be issued by LL_EXT_EndModemTest!
 * Note: The BLE device will transmit at maximum power.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ModemTestRxCmd( uint8 rxChan )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_MODEM_TEST_RX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_MODEM_TEST_RX_EVENT );
  rtnParam[2] = MAP_LL_EXT_ModemTestRx( rxChan );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_MODEM_TEST_RX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used start the enhanced BLE5 continuous transmitter modem test,
 * using either a modulated or unmodulated carrier wave tone, at the frequency
 * that corresponds to the specified RF channel, for a given PHY (1M, 2M,
 * Coded S2, or Coded S8). Use LL_EXT_EndModemTest to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTest command!
 * Note: The BLE device will transmit at the current TX power setting.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EnhancedModemTestTxCmd( uint8 cwMode,
                                            uint8 rfPhy,
                                            uint8 rfChan )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_ENHANCED_MODEM_TEST_TX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_ENHANCED_MODEM_TEST_TX_EVENT );
  rtnParam[2] = MAP_LL_EXT_EnhancedModemTestTx( cwMode,
                                                rfPhy,
                                                rfChan );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_ENHANCED_MODEM_TEST_TX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used to start the enhanced continuous transmitter direct test
 * mode test using a modulated carrier wave. A test reference data packet is
 * transmitted on a different frequency (linearly stepping through all RF
 * channels 0..39), for a given PHY (1M, 2M, Coded S2, or Coded S8), every
 * period (depending on the payload length, as given Vol. 6, Part F,
 * section 4.1.6). Use LL_EXT_EndModemTest to end the test.
 *
 * Note: A Controller reset will be issued by the HCI_EXT_EndModemTest command!
 * Note: The BLE device will transmit at the current TX power setting.
 * Note: This API can be used to verify this device meets Japan's TELEC
 *       regulations.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EnhancedModemHopTestTxCmd( uint8 payloadLen,
                                               uint8 payloadType,
                                               uint8 rfPhy )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX_EVENT );
  rtnParam[2] = MAP_LL_EXT_EnhancedModemHopTestTx( payloadLen,
                                                   payloadType,
                                                   rfPhy );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used to start the enhanced BLE5 continuous receiver modem test
 * using a modulated carrier wave tone, at the frequency that corresponds to the
 * specific RF channel, for a given PHY (1M, 2M, Coded S2, or Coded S8). Any
 * received data is discarded. RSSI may be read during this test by using the
 * LL_ReadRssi command. Use LL_EXT_EndModemTest command to end the test.
 *
 * Note: A Controller reset will be issued by LL_EXT_EndModemTest!
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EnhancedModemTestRxCmd( uint8 rfPhy,
                                            uint8 rfChan )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_ENHANCED_MODEM_TEST_RX_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_ENHANCED_MODEM_TEST_RX_EVENT );
  rtnParam[2] = MAP_LL_EXT_EnhancedModemTestRx( rfPhy,
                                                rfChan );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_ENHANCED_MODEM_TEST_RX,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This API is used to shutdown a modem test. A complete Controller reset will
 * take place.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EndModemTestCmd( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_END_MODEM_TEST_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_END_MODEM_TEST_EVENT );
  rtnParam[2] = MAP_LL_EXT_EndModemTest();

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_END_MODEM_TEST,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This API is used to set this device's BLE address (BDADDR).
 *
 * Note: This command is only allowed when the device's state is Standby.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetBDADDRCmd( uint8 *bdAddr )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_BDADDR_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_BDADDR_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetBDADDR( bdAddr );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_BDADDR,
                                             sizeof(rtnParam),
                                             rtnParam );

  // notify the HCI Test Application (if there is one) that BDADDR changed
  if ( hciTestTaskID )
  {
    (void)MAP_osal_set_event( hciTestTaskID,
                          HCI_BDADDR_UPDATED_EVENT );
  }

  return( HCI_SUCCESS );
}

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG)
/*******************************************************************************
 * This API is used to set the advertiser's virtual public address.
 *
 * Note: This command is only allowed when the advertise set is not active,
 *      address type is public and its event type is Legacy
 *      Non-Connectable and Non-Scanable
 *
 * Public function defined in hci.h.
 */

hciStatus_t HCI_EXT_SetVirtualAdvAddrCmd( uint8 advHandle,
                                          uint8 *bdAddr )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_VIRTUAL_ADV_ADDRESS_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_VIRTUAL_ADV_ADDRESS_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetVirtualAdvAddr( advHandle,
                                                bdAddr );
#ifndef HOST_CONFIG
  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_LE_SET_EXT_VIRTUAL_ADV_ADDRESS,
                                             sizeof(rtnParam),
                                             rtnParam );
  return( HCI_SUCCESS );
#else // used from host-test / app.
  return (rtnParam[2]);
#endif
}
#endif

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This API is used to set this device's Sleep Clock Accuracy value.
 *
 * Note: For a slave device, this value is directly used, but only
 *       if power management is enabled. For a master device, this
 *       value is converted into one of eight ordinal values
 *       representing a SCA range, as specified in Table 2.2,
 *       Vol. 6, Part B, Section 2.3.3.1 of the Core specification.
 *
 * Note: This command is only allowed when the device is not in a connection.
 *
 * Note: The device's SCA value remains unaffected by a HCI_Reset.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetSCACmd( uint16 scaInPPM )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_SCA_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_SCA_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetSCA( scaInPPM );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_SCA,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This HCI Extension API is used to enable Production Test Mode.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_EnablePTMCmd( void )
{
  // stop everything before entering PTM
  MAP_HCI_ResetCmd();

  // set global for runtime check
  hciPTMenabled = TRUE;

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to set frequency tuning up or down.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetFreqTuneCmd( uint8 step )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_FREQ_TUNE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_FREQ_TUNE_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetFreqTune( step );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_FREQ_TUNE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to save the frequency tuning value.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SaveFreqTuneCmd( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SAVE_FREQ_TUNE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SAVE_FREQ_TUNE_EVENT );
  rtnParam[2] = MAP_LL_EXT_SaveFreqTune();

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SAVE_FREQ_TUNE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to set the max TX power for Direct Test Mode.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetMaxDtmTxPowerCmd( uint8 txPower )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_MAX_DTM_TX_POWER_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_MAX_DTM_TX_POWER_EVENT );
  rtnParam[2] = MAP_LL_EXT_SetMaxDtmTxPower( txPower );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_MAX_DTM_TX_POWER,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to configure and map a CC254x I/O Port as a
 * General Purpose I/O (GPIO) output signal that reflects the Power Management
 * state of the CC254x device. The GPIO output will be High on Wake, and Low
 * upon entering Sleep. This feature can be disabled by specifying
 * HCI_EXT_PM_IO_PORT_NONE for the ioPort (ioPin is then ignored). The system
 * default value upon hardware reset is disabled.
 *
 * Note: Only Pins 0, 3 and 4 are valid for Port 2 since Pins 1 and 2 are mapped
 *       to debugger signals DD and DC.
 *
 * Note: Port/Pin signal change will only occur when Power Savings is enabled.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_MapPmIoPortCmd( uint8 ioPort, uint8 ioPin )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_MAP_PM_IO_PORT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_MAP_PM_IO_PORT_EVENT );
  rtnParam[2] = MAP_LL_EXT_MapPmIoPort( ioPort, ioPin );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_MAP_PM_IO_PORT,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to terminate a connection immediately without
 * following normal BLE disconnect control procedure.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DisconnectImmedCmd( uint16 connHandle )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_DISCONNECT_IMMED_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_DISCONNECT_IMMED_EVENT );
  rtnParam[2] = MAP_LL_EXT_DisconnectImmed( connHandle );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_DISCONNECT_IMMED,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to Reset or Read the Packet Error Rate data
 * for a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_PacketErrorRateCmd( uint16 connHandle, uint8 command )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  // 3: Command
  uint8 rtnParam[4];

  rtnParam[0] = LO_UINT16( HCI_EXT_PER_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_PER_EVENT );
  rtnParam[2] = MAP_LL_EXT_PacketErrorRate( connHandle, command );
  rtnParam[3] = command;

  // check if it is okay to complete this event now or later
  if ( (command == HCI_EXT_PER_RESET) || (rtnParam[2] != LL_STATUS_SUCCESS) )
  {
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_PER,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to start or end Packet Error Rate by Frequency
 * counter accumulation for a connection.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_PERbyChanCmd( uint16 connHandle, perByChan_t *perByChan )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_PER_BY_CHAN_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_PER_BY_CHAN_EVENT );
  rtnParam[2] = MAP_LL_EXT_PERbyChan( connHandle, perByChan );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_PER_BY_CHAN,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 * This HCI Extension API is used to Extend Rf Range.
 *
 * Note: If the LL can not perform the command immediately, the HCI will be
 *       notified by a corresonding LL callback.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ExtendRfRangeCmd( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];
  uint8 cmdComplete = TRUE;

  rtnParam[0] = LO_UINT16( HCI_EXT_EXTEND_RF_RANGE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_EXTEND_RF_RANGE_EVENT );
  rtnParam[2] = MAP_LL_EXT_ExtendRfRange( &cmdComplete );

  // check if the command was performed, or if it was delayed
  // Note: If delayed, a callback will be generated by the LL.
  if ( cmdComplete == TRUE )
  {
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_EXTEND_RF_RANGE,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to enable or disable HALT during RF.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_HaltDuringRfCmd( uint8 mode )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_HALT_DURING_RF_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_HALT_DURING_RF_EVENT );
  rtnParam[2] = MAP_LL_EXT_HaltDuringRf( mode );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_HALT_DURING_RF,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to set a user revision number or read
 * the build revision number (combined user/system build number).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_BuildRevisionCmd( uint8 mode, uint16 userRevNum )
{
  // check input parameter that doesn't require vendor specific event
  if ( mode == HCI_EXT_SET_USER_REVISION )
  {
    // save the user's revision number
    // Note: No vendor specific event is generated as this is intended to be
    //       called from the target build. Otherwise, an asynchronous event
    //       would unexpectedly be received by the Host processor.
    return( MAP_LL_EXT_BuildRevision( mode, userRevNum, NULL ) );
  }
  else // vendor specific event required
  {
    // 0: Event Opcode (LSB)
    // 1: Event Opcode (MSB)
    // 2: Status
    // 3..6: Build Revision (combined user+system)
    uint8 rtnParam[7];

    rtnParam[0] = LO_UINT16( HCI_EXT_BUILD_REVISION_EVENT );
    rtnParam[1] = HI_UINT16( HCI_EXT_BUILD_REVISION_EVENT );
    rtnParam[2] = MAP_LL_EXT_BuildRevision( mode, userRevNum, &rtnParam[3] );

    // check for error
    if ( rtnParam[2] != LL_STATUS_SUCCESS )
    {
      // clear build revision
      *((uint32 *)&rtnParam[3]) = 0;
    }

    // return event
    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_BUILD_REVISION,
                                               sizeof(rtnParam),
                                               rtnParam );
  }

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to set the sleep delay.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_DelaySleepCmd( uint16 delay )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_DELAY_SLEEP_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_DELAY_SLEEP_EVENT );
  rtnParam[2] = HCI_ERROR_CODE_CMD_DISALLOWED;

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_DELAY_SLEEP,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


/*******************************************************************************
 * This HCI Extension API is used to issue a soft or hard system reset.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ResetSystemCmd( uint8 mode )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_RESET_SYSTEM_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_RESET_SYSTEM_EVENT );

#if defined( CC26XX ) || defined( CC13XX )
  if (mode == HCI_EXT_RESET_SYSTEM_HARD)
  {
    rtnParam[2] = MAP_LL_EXT_ResetSystem( mode );
  }
  else // HCI_EXT_RESET_SYSTEM_SOFT not working
  {
    rtnParam[2] = HCI_ERROR_CODE_UNSUPPORTED_FEATURE_PARAM_VALUE;
  }
#else // !CC26XX
  rtnParam[2] = MAP_LL_EXT_ResetSystem( mode );
#endif // CC26XX/CC13XX

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_RESET_SYSTEM,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to enable or disable overlapped processing.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_OverlappedProcessingCmd( uint8 mode )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_OVERLAPPED_PROCESSING_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_OVERLAPPED_PROCESSING_EVENT );
  rtnParam[2] = MAP_LL_EXT_OverlappedProcessing( mode );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_OVERLAPPED_PROCESSING,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This HCI Extension API is used to set the minimum number of completed packets
 * which must be met before a Number of Completed Packets event is returned. If
 * the limit is not reach by the end of the connection event, then a Number of
 * Completed Packets event will be returned (if non-zero) based on the
 * flushOnEvt flag.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_NumComplPktsLimitCmd( uint8 limit,
                                          uint8 flushOnEvt )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_NUM_COMPLETED_PKTS_LIMIT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_NUM_COMPLETED_PKTS_LIMIT_EVENT );
  rtnParam[2] = MAP_LL_EXT_NumComplPktsLimit( limit, flushOnEvt );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_NUM_COMPLETED_PKTS_LIMIT,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This API is used to get connection related information, which includes the
 * number of allocated connections, the number of active connections, and for
 * each active connection, the connection ID, the connection role (Master or
 * Slave), the peer address and peer address type. The number of allocated
 * connections is based on a default build value that can be changed using
 * MAX_NUM_BLE_CONNS. The number of active connections refers to active BLE
 * connections.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_GetConnInfoCmd( uint8         *numAllocConns,
                                    uint8         *numActiveConns,
                                    hciConnInfo_t *activeConnInfo )
{
  // check if all pointers are invalid
  if ( (numAllocConns  == NULL) &&
       (numActiveConns == NULL) &&
       (activeConnInfo == NULL) )
  {
    // 0:  Event Opcode (LSB)
    // 1:  Event Opcode (MSB)
    // 2:  Status
    // 3:  Number Allocated Connections
    // 4:  Number Active Connections
    // For each active connection:
    // 5:  Connection ID
    // 6:  Connection Role
    // 7:  Peer Device Address
    // 13: Peer Device Address Type
    uint8  size;
    uint8  numConns;
    uint8 *connInfo;

    // they are, so this command originated from the transport layer

    // get number of active connections
    MAP_LL_GetNumActiveConns( &numConns );

    // allocate memory based on number of active connections
    // Note: The value of 5 is derived from the sizes of opcode (2), status (1),
    //       numAllocConns (1), and numActiveConns (1).
    size     = (numConns * sizeof(hciConnInfo_t)) + 5;
    connInfo = MAP_osal_mem_alloc( size );

    // check if we have the memory
    if ( connInfo != NULL )
    {
      connInfo[0] = LO_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
      connInfo[1] = HI_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
      connInfo[2] = HCI_SUCCESS;

      // Note: Currently, this function always returns SUCCESS.
      (void)MAP_LL_EXT_GetConnInfo( &connInfo[3],
                                    &connInfo[4],
                                    (numConns==0)?NULL:&connInfo[5] );

      MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_CONNECTION_INFO,
                                                 size,
                                                 connInfo );

      MAP_osal_mem_free( (void *)connInfo );
    }
    else // out of memory
    {
      // 0:  Event Opcode (LSB)
      // 1:  Event Opcode (MSB)
      // 2:  Status
      uint8 rtnParam[3];

      rtnParam[0] = LO_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
      rtnParam[1] = HI_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
      rtnParam[2] = HCI_ERROR_CODE_MEM_CAP_EXCEEDED;

      MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_CONNECTION_INFO,
                                                 sizeof(rtnParam),
                                                 rtnParam );
    }

    return( HCI_SUCCESS );
  }
  else // called directly (one or more pointers are not NULL)
  {
    // 0:  Event Opcode (LSB)
    // 1:  Event Opcode (MSB)
    // 2:  Status
    uint8 rtnParam[3];

    // one or more pointers provided by user
    // Note: Only non-Null pointers are populated.
    (void)MAP_LL_EXT_GetConnInfo( numAllocConns,
                                  numActiveConns,
                                  (uint8 *)activeConnInfo );

    rtnParam[0] = LO_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
    rtnParam[1] = HI_UINT16( HCI_EXT_GET_CONNECTION_INFO_EVENT );
    rtnParam[2] = HCI_SUCCESS;

    MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_CONNECTION_INFO,
                                               sizeof(rtnParam),
                                               rtnParam );

    return( HCI_SUCCESS );
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This LE API is used to set the maximum supported Tx and Rx Octets (in bytes)
 * and Time (in us).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetMaxDataLenCmd( uint16 txOctets,
                                      uint16 txTime,
                                      uint16 rxOctets,
                                      uint16 rxTime )
{
  // 0:  Event Opcode (LSB)
  // 1:  Event Opcode (MSB)
  // 2:  Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_MAX_DATA_LENGTH_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_MAX_DATA_LENGTH_EVENT );

  rtnParam[2] = MAP_LL_EXT_SetMaxDataLen( txOctets,
                                          txTime,
                                          rxOctets,
                                          rxTime );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_MAX_DATA_LENGTH,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // (ADV_CONN_CFG | INIT_CFG)

/*******************************************************************************
 * This HCI Extension API is used to set the transmit power.
 *
 * Note: If the LL can not perform the command immediately, the HCI will be
 *       notified by a corresonding LL callback.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetDtmTxPktCntCmd( uint16 txPktCnt )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_DTM_TX_PKT_CNT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_DTM_TX_PKT_CNT_EVENT );

  rtnParam[2] = MAP_LL_EXT_SetDtmTxPktCnt( txPktCnt );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_DTM_TX_PKT_CNT,
                                             sizeof(rtnParam),
                                             rtnParam );
  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This BT API is used to read this device's Random address (BDADDR).
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_ReadRandAddrCmd( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  // 3..8: Random Address
  uint8 rtnParam[9];

  rtnParam[0] = LO_UINT16( HCI_EXT_READ_RAND_ADDR_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_READ_RAND_ADDR_EVENT );

  // status
  rtnParam[2] = MAP_LL_EXT_ReadRandomAddress( &(rtnParam[3]) );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_READ_RAND_ADDR,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This API is used to set pin as output and initialize it.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetPinOutputCmd( uint8 dio,
                                     uint8 value )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_PIN_OUTPUT_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_PIN_OUTPUT_EVENT );

  // status
  rtnParam[2] = LL_EXT_SetPinOutput( dio, value );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_PIN_OUTPUT,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This API is used to set CTE accuracy for 1M and 2M PHY per connection handle (0x0XXX)
 * or per periodic advertising train handle (0x1XXX)
 * sample rate range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * sample size range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetLocationingAccuracyCmd( uint16 handle,
                                               uint8  sampleRate1M,
                                               uint8  sampleSize1M,
                                               uint8  sampleRate2M,
                                               uint8  sampleSize2M,
                                               uint8  sampleCtrl)
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_LOCATIONING_ACCURACY_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_LOCATIONING_ACCURACY_EVENT );

  // status
  rtnParam[2] = MAP_LL_EXT_SetLocationingAccuracy( handle,
                                                   sampleRate1M,
                                                   sampleSize1M,
                                                   sampleRate2M,
                                                   sampleSize2M,
                                                   sampleCtrl);

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_LOCATIONING_ACCURACY,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This HCI EXTENSION API is used to get connection information needed to track
 * an active BLE connection on the device calling this function.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_GetActiveConnInfoCmd( uint8 connId, hciActiveConnInfo_t *activeConnInfo)
{
  uint8  size;
  uint8 *defaultConnInfo;

  if(activeConnInfo == NULL)
  {
    // allocate memory based on number of active connections
    // Note: The value of 3 is derived from the sizes of opcode (2) and status (1).
    size     = (sizeof(hciActiveConnInfo_t)) + 3;
    defaultConnInfo = MAP_osal_mem_alloc( size );

    // check if we have the memory
    if ( defaultConnInfo != NULL )
    {
      defaultConnInfo[0] = LO_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );
      defaultConnInfo[1] = HI_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );

      // Note: Currently, this function always returns SUCCESS.
      defaultConnInfo[2] = MAP_LL_EXT_GetActiveConnInfo( connId,&defaultConnInfo[3] );

      HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_ACTIVE_CONNECTION_INFO,
                                             size,
                                             defaultConnInfo );

      MAP_osal_mem_free( (void *)defaultConnInfo );
    }
    else // out of memory
    {
      // 0:  Event Opcode (LSB)
      // 1:  Event Opcode (MSB)
      // 2:  Status
      uint8 rtnParam[3];

      rtnParam[0] = LO_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );
      rtnParam[1] = HI_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );
      rtnParam[2] = HCI_ERROR_CODE_MEM_CAP_EXCEEDED;

      HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_ACTIVE_CONNECTION_INFO,
                                             sizeof(rtnParam),
                                             rtnParam );
    }
  }
  else // called directly (pointer is not NULL)
  {
    // 0:  Event Opcode (LSB)
    // 1:  Event Opcode (MSB)
    // 2:  Status
    uint8 rtnParam[3];

    // pointer provided by user
    rtnParam[2] = MAP_LL_EXT_GetActiveConnInfo( connId, (uint8 *)activeConnInfo );

    rtnParam[0] = LO_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );
    rtnParam[1] = HI_UINT16( HCI_EXT_GET_ACTIVE_CONNECTION_INFO_EVENT );

    HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_GET_ACTIVE_CONNECTION_INFO,
                                           sizeof(rtnParam),
                                           rtnParam );
  }

  return( HCI_SUCCESS );
}

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*******************************************************************************
 * This HCI EXTENSION API is used to set the scan channels mapping
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetExtScanChannels( uint8 extScanChannelsMap )
{
  hciStatus_t status;

  status = MAP_LL_EXT_SetExtScanChannels( extScanChannelsMap );

  MAP_HCI_CommandCompleteEvent( HCI_EXT_SET_SCAN_CHAN,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}
#endif // CTRL_CONFIG & SCAN_CFG

/*******************************************************************************
 * This HCI EXTENSION API is used to set the QOS Parameters.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetQOSParameters( uint8  taskType,
                                      uint8  paramType,
                                      uint32 paramVal,
                                      uint16 taskHandle)
{
  hciStatus_t status;

  status = MAP_LL_EXT_SetQOSParameters( taskType,
                                        paramType,
                                        paramVal,
                                        taskHandle );

  MAP_HCI_CommandCompleteEvent( HCI_EXT_SET_QOS_PARAMETERS,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This HCI EXTENSION API is used to set the default QOS Parameters.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_SetQOSDefaultParameters(uint32 paramDefaultVal,
                                            uint8  paramType,
                                            uint8  taskType)
{
  hciStatus_t status;

  status = MAP_LL_EXT_SetQOSDefaultParameters( paramDefaultVal,
                                               paramType,
                                               taskType);

  MAP_HCI_CommandCompleteEvent( HCI_EXT_SET_QOS_DEFAULT_PARAMETERS,
                                sizeof(status),
                                &status );

  return( HCI_SUCCESS );
}

/*******************************************************************************
 * This API is used to enable or disable the Coex feature.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_CoexEnableCmd( uint8 enable )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_COEX_ENABLE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_COEX_ENABLE_EVENT );

  // status
  rtnParam[2] = LL_EXT_CoexEnable( enable );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_COEX_ENABLE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}

#ifdef LL_TEST_MODE
/*******************************************************************************
 * This HCI Extension API is used send a LL Test Mode test case.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_EXT_LLTestModeCmd( uint8 testCase )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_LL_TEST_MODE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_LL_TEST_MODE_EVENT );

  // Note: This function will never reside in ROM, so no MAP_ required.
  rtnParam[2] = LL_EXT_LLTestMode( testCase );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_LL_TEST_MODE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return( HCI_SUCCESS );
}
#endif // LL_TEST_MODE


/*
** LL Callback Functions
*/

/*******************************************************************************
 * This LL command Callback is used by the LL to notify the HCI that the LE
 * RAND command has been completed.
 *
 * Note: The length is always given by B_RANDOM_NUM_SIZE.
 *
 * Public function defined in hci.h.
 */
void LL_RandCback( uint8 *randData )
{
  // 0:    Status
  // 1..8: Random Bytes
  uint8 rtnParam[B_RANDOM_NUM_SIZE+1];

  rtnParam[0] = LL_STATUS_SUCCESS;

  // copy random data block
  (void)MAP_osal_memcpy( &rtnParam[1], randData, B_RANDOM_NUM_SIZE );

  MAP_HCI_CommandCompleteEvent( HCI_LE_RAND, B_RANDOM_NUM_SIZE+1, rtnParam );

  return;
}


/*******************************************************************************
 * This LL Extension command Callback is used by the LL to notify the HCI that
 * the set RX gain command has been completed.
 *
 * Public function defined in hci.h.
 */
void LL_EXT_SetRxGainCback( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_RX_GAIN_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_RX_GAIN_EVENT );
  rtnParam[2] = HCI_SUCCESS;

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_RX_GAIN,
                                             sizeof(rtnParam),
                                             rtnParam );

  return;
}


/*******************************************************************************
 * This LL Extension command Callback is used by the LL to notify the HCI that
 * the set TX power command has been completed.
 *
 * Public function defined in hci.h.
 */
void LL_EXT_SetTxPowerCback( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_SET_TX_POWER_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_SET_TX_POWER_EVENT );
  rtnParam[2] = HCI_SUCCESS;

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_SET_TX_POWER,
                                             sizeof(rtnParam),
                                             rtnParam );

  return;
}


/*******************************************************************************
 * This LL Extension command Callback is used by the LL to notify the HCI that
 * the Packet Error Rate Read has been completed.
 *
 * Note: The counters are only 16 bits. At the shortest connection
 *       interval, this provides a bit over 8 minutes of data.
 *
 * Public function defined in hci.h.
 */
void LL_EXT_PacketErrorRateCback( uint16 numPkts,
                                  uint16 numCrcErr,
                                  uint16 numEvents,
                                  uint16 numMissedEvts )
{
  // 0:  Event Opcode (LSB)
  // 1:  Event Opcode (MSB)
  // 2:  Status
  // 3:  Command
  // 4:  Number of Packets (LSB)
  // 5:  Number of Packets (MSB)
  // 6:  Number of CRC Errors (LSB)
  // 7:  Number of CRC Errors (MSB)
  // 8:  Number of Events (LSB)
  // 9:  Number of Events (MSB)
  // 10: Number of Missed Events(LSB)
  // 11: Number of Missed Events (MSB)
  uint8 rtnParam[12];

  rtnParam[0]  = LO_UINT16( HCI_EXT_PER_EVENT );
  rtnParam[1]  = HI_UINT16( HCI_EXT_PER_EVENT );
  rtnParam[2]  = HCI_SUCCESS;
  rtnParam[3]  = HCI_EXT_PER_READ;
  rtnParam[4]  = LO_UINT16( numPkts );
  rtnParam[5]  = HI_UINT16( numPkts );
  rtnParam[6]  = LO_UINT16( numCrcErr );
  rtnParam[7]  = HI_UINT16( numCrcErr );
  rtnParam[8]  = LO_UINT16( numEvents );
  rtnParam[9]  = HI_UINT16( numEvents );
  rtnParam[10] = LO_UINT16( numMissedEvts );
  rtnParam[11] = HI_UINT16( numMissedEvts );

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_PER,
                                             sizeof(rtnParam),
                                             rtnParam );

  return;
}

/*******************************************************************************
 * This LL Extension command Callback is used by the LL to notify the HCI that
 * the Extend Rf Range command has been completed.
 *
 * Public function defined in hci.h.
 */
void LL_EXT_ExtendRfRangeCback( void )
{
  // 0: Event Opcode (LSB)
  // 1: Event Opcode (MSB)
  // 2: Status
  uint8 rtnParam[3];

  rtnParam[0] = LO_UINT16( HCI_EXT_EXTEND_RF_RANGE_EVENT );
  rtnParam[1] = HI_UINT16( HCI_EXT_EXTEND_RF_RANGE_EVENT );
  rtnParam[2] = HCI_SUCCESS;

  MAP_HCI_VendorSpecifcCommandCompleteEvent( HCI_EXT_EXTEND_RF_RANGE,
                                             sizeof(rtnParam),
                                             rtnParam );

  return;
}

/***************************************************************************************************
 */
