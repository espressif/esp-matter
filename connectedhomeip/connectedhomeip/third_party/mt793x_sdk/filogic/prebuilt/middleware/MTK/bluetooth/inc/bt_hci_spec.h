/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
/* MediaTek restricted information */

#ifndef __BT_HCI_PACKET_SPEC_H__
#define __BT_HCI_PACKET_SPEC_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup HCI
 * @{
 * The header file includes the definition in HCI spec.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b ACL                        | The LE asynchronous connection logical transport (LE ACL).              |
 * |                              | It is used to carry LL and L2CAP control signaling and best effort asynchronous user data. |
 * |\b BD                         | Bluetooth Device.
 * |\b DHKey                      | Diffie Hellman key.                                                     |
 * |\b HCI                        | Host Controller Interface. The interface between host stack and controller. |
 * |\b IRK                        | Identity Resolving Key. A 128-bit key used to generate and resolve random addresses. |
 * |\b LMP                        | Link Manager Protocol. For LM peer to peer communication.               |
 * |\b MIC                        | Message Integrity Check is used to authenticate the data PDU.           |
 * |\b PDU                        | Protocol Data Units. A message.                                         |
 * |\b QOS                        | Quality of service.                                                     |
 * |\b SCO                        | Synchronous logical link.                                               |
 *
 * @section bt_hci_spec_api_usage How to use this module
 *
 * - Please reference bt_gap_le.h to use the API to finish your function.
 *   You may find the related HCI command definitions or structure definitions in this header file.
 * - Request example
 *  - Sample code:
 *      @code
 *          bt_hci_cmd_read_rssi_t read_rssi = {
 *              .handle = 0x0200,
 *          };
 *
 *          bt_gap_le_read_rssi(&read_rssi);
 *      @endcode
 *
 * - Confirmation Example
 *  - Sample code:
 *      @code
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch(msg)
 *              {
 *                  case BT_GAP_LE_READ_RSSI_CNF:
 *                      const bt_hci_evt_cc_read_rssi_t *rssi = (bt_hci_evt_cc_read_rssi_t *)buff;
 *                      BT_LOGI("APP", "connection handle=0x%04x", rssi->handle);
 *                      if (rssi->rssi == 127) {
 *                          BT_LOGI("APP", "rssi cannot be read");
 *                      } else {
 *                          if ((rssi->rssi>>7)>0) {
 *                              BT_LOGI("APP", "rssi=%ddBm", ((~rssi->rssi)&0xFF)+0x01);
 *                          } else {
 *                              BT_LOGI("APP", "rssi=%ddBm", rssi->rssi);
 *                          }
 *                      }
 *                      break;
 *                  // You can add other cases in this.
 *              }
 *          }
 *      @endcode
 *
 */

#include "bt_type.h"
#include "bt_hci.h"
/**
 * @defgroup hbif_hci_spec_define Define
 * @{
 */

/**
 * @brief HCI packet indicator. Refer to the Bluetooth core specifications version 4.2 [Vol 4, Part A] UART Transport Layer, CH2 PROTOCOL.
 */
#define BT_UART_CMD     0x01
#define BT_UART_ACL     0x02
#define BT_UART_EVT     0x04
#define BT_UART_ISO     0x05
typedef uint8_t bt_uart_t;

/**
 * @brief Packet boundary flag. Refer to the Bluetooth core specifications version 4.2 [Vol 2, Part E] Host Controller Interface Functional Specification, 5.4.2 HCI ACL Data Packets.
 */
#define BT_HCI_PB_FIRST_NONFLUSH        0x00
#define BT_HCI_PB_CONTINUING            0x01
#define BT_HCI_PB_FIRST_FLUSH           0x02
#define BT_HCI_PB_COMPLETE              0x03

/* Broadcast flag. */
#define BT_HCI_BC_NO_BROADCAST          0x00

/**
 * @brief Below is defined in Refer to the Bluetooth core specifications version 4.2 [Vol 2, Part E] Host Controller Interface Functional Specification.
 */

/**
 * @brief Link Control Commands, reference: 7.1 LINK CONTROL COMMANDS.
 */
#define BT_HCI_CMD_INQUIRY                                  0x0401
#define BT_HCI_CMD_INQUIRY_CANCEL                           0x0402
#define BT_HCI_CMD_CREATE_CONNECTION                        0x0405
#define BT_HCI_CMD_DISCONNECT                               0x0406
#define BT_HCI_CMD_SCO_DISCONNECT                           0x0406 /* humming bird adding for disconnecting sco */
#define BT_HCI_CMD_CREATE_CONNECTION_CANCEL                 0x0408 /* 1.2 */
#define BT_HCI_CMD_ACCEPT_CON_REQ                           0x0409
#define BT_HCI_CMD_REJECT_CON_REQ                           0x040A
#define BT_HCI_CMD_LINK_KEY_REQ_REPL                        0x040B
#define BT_HCI_CMD_LINK_KEY_REQ_NEG_REPL                    0x040C
#define BT_HCI_CMD_PIN_CODE_REQ_REPL                        0x040D
#define BT_HCI_CMD_PIN_CODE_REQ_NEG_REPL                    0x040E
#define BT_HCI_CMD_CHNG_CONN_PACKET_TYPE                    0x040F
#define BT_HCI_CMD_AUTH_REQ                                 0x0411
#define BT_HCI_CMD_SET_CONN_ENCRYPT                         0x0413
#define BT_HCI_CMD_REM_NAME_REQ                             0x0419
#define BT_HCI_CMD_REM_NAME_REQ_CANCEL                      0x041A /* 1.2 */
#define BT_HCI_CMD_READ_REMOTE_FEATURES                     0x041B
#define BT_HCI_CMD_READ_REMOTE_EXT_FEATURES                 0x041C /* 1.2 */
#define BT_HCI_CMD_READ_REMOTE_VERSION                      0x041D
#define BT_HCI_CMD_READ_CLOCK_OFFSET                        0x041F
#define BT_HCI_CMD_SETUP_SYNC_CONNECTION                    0x0428 /* 1.2 */
#define BT_HCI_CMD_ACCEPT_SYNC_CON_REQ                      0x0429 /* 1.2 */
#define BT_HCI_CMD_REJECT_SYNC_CON_REQ                      0x042A /* 1.2 */
#define BT_HCI_CMD_IO_CAPABILITY_REPL                       0x042B /* 2.1 */
#define BT_HCI_CMD_USER_CONFIRM_REPL                        0x042C /* 2.1 */
#define BT_HCI_CMD_USER_CONFIRM_NEG_REPL                    0x042D /* 2.1 */
#define BT_HCI_CMD_USER_PASSKEY_REQ_REPL                    0x042E /* 2.1 */
#define BT_HCI_CMD_USER_PASSKEY_REQ_NEG_REPL                0x042F /* 2.1 */
#define BT_HCI_CMD_USER_OOB_DATA_REQ_REPL                   0x0430 /* 2.1 */
#define BT_HCI_CMD_USER_OOB_DATA_REQ_NEG_REPL               0x0433 /* 2.1 */
#define BT_HCI_CMD_IO_CAPABILITY_NEG_REPL                   0x0434 /* 2.1 */
#define BT_HCI_CMD_ENHANCED_SETUP_SYNC_CONNECTION           0x043D /* CSA2 */
#define BT_HCI_CMD_ENHANCED_ACCEPT_SYNC_CONNECTION          0x043E /* CSA2 */
#define BT_HCI_CMD_OOB_EXT_DATA_REQUEST_REPL                0x0445

/**
 * @brief Link Policy Commands, reference: 7.2 LINK POLICY COMMANDS.
 */
#define BT_HCI_CMD_HOLD_MODE                                0x0801
#define BT_HCI_CMD_SNIFF_MODE                               0x0803
#define BT_HCI_CMD_EXIT_SNIFF_MODE                          0x0804
#define BT_HCI_CMD_PARK_MODE                                0x0805
#define BT_HCI_CMD_EXIT_PARK_MODE                           0x0806
#define BT_HCI_CMD_QOS_SETUP                                0x0807
#define BT_HCI_CMD_ROLE_DISCOVERY                           0x0809
#define BT_HCI_CMD_SWITCH_ROLE                              0x080B
#define BT_HCI_CMD_READ_LINK_POLICY                         0x080C
#define BT_HCI_CMD_WRITE_LINK_POLICY                        0x080D
#define BT_HCI_CMD_READ_DEFAULT_LINK_POLICY                 0x080E /* 1.2 */
#define BT_HCI_CMD_WRITE_DEFAULT_LINK_POLICY                0x080F /* 1.2 */
#define BT_HCI_CMD_FLOW_SPECIFICATION                       0x0810 /* 1.2 */
#define BT_HCI_CMD_SNIFF_SUBRATING                          0x0811 /* 2.1 */

/**
 * @brief Controller & Baseband Commands, reference: 7.3 CONTROLLER & BASEBAND COMMANDS.
 */
#define BT_HCI_CMD_SET_EVENT_MASK                 0x0C01
#define BT_HCI_CMD_RESET                          0x0C03
#define BT_HCI_CMD_EVENT_FILTER                   0x0C05
#define BT_HCI_CMD_FLUSH                          0x0C08
#define BT_HCI_CMD_READ_PIN_TYPE                  0x0C09
#define BT_HCI_CMD_WRITE_PIN_TYPE                 0x0C0A
#define BT_HCI_CMD_CREATE_NEW_UNIT_KEY            0x0C0B
#define BT_HCI_CMD_READ_STORED_LINK_KEY           0x0C0D
#define BT_HCI_CMD_WRITE_STORED_LINK_KEY          0x0C11
#define BT_HCI_CMD_DEL_STORED_LINK_KEY            0x0C12
#define BT_HCI_CMD_CHNG_LOCAL_NAME                0x0C13
#define BT_HCI_CMD_READ_LOCAL_NAME                0x0C14
#define BT_HCI_CMD_READ_CONN_ACCEPT_TIMEOUT       0x0C15
#define BT_HCI_CMD_WRITE_CONN_ACCEPT_TIMEOUT      0x0C16
#define BT_HCI_CMD_READ_PAGE_TIMEOUT              0x0C17
#define BT_HCI_CMD_WRITE_PAGE_TIMEOUT             0x0C18
#define BT_HCI_CMD_READ_SCAN_ENABLE               0x0C19
#define BT_HCI_CMD_WRITE_SCAN_ENABLE              0x0C1A
#define BT_HCI_CMD_READ_PAGE_SCAN_ACTIVITY        0x0C1B
#define BT_HCI_CMD_WRITE_PAGE_SCAN_ACTIVITY       0x0C1C
#define BT_HCI_CMD_READ_INQ_SCAN_ACTIVITY         0x0C1D
#define BT_HCI_CMD_WRITE_INQ_SCAN_ACTIVITY        0x0C1E
#define BT_HCI_CMD_READ_AUTH_ENABLE               0x0C1F
#define BT_HCI_CMD_WRITE_AUTH_ENABLE              0x0C20
#define BT_HCI_CMD_READ_CLASS_OF_DEVICE           0x0C23
#define BT_HCI_CMD_WRITE_CLASS_OF_DEVICE          0x0C24
#define BT_HCI_CMD_READ_VOICE_SETTING             0x0C25
#define BT_HCI_CMD_WRITE_VOICE_SETTING            0x0C26
#define BT_HCI_CMD_READ_AUTO_FLUSH_TIMEOUT        0x0C27
#define BT_HCI_CMD_WRITE_AUTO_FLUSH_TIMEOUT       0x0C28
#define BT_HCI_CMD_READ_NUM_BC_RETRANSMIT         0x0C29
#define BT_HCI_CMD_WRITE_NUM_BC_RETRANSMIT        0x0C2A
#define BT_HCI_CMD_READ_HOLD_MODE_ACTIVITY        0x0C2B
#define BT_HCI_CMD_WRITE_HOLD_MODE_ACTIVITY       0x0C2C
#define BT_HCI_CMD_READ_XMIT_POWER_LEVEL          0x0C2D
#define BT_HCI_CMD_READ_SCO_FC_ENABLE             0x0C2E
#define BT_HCI_CMD_WRITE_SCO_FC_ENABLE            0x0C2F
#define BT_HCI_CMD_SET_CONTROLLER_TO_HOST_FLOW_CONTROL    0x0C31
#define BT_HCI_CMD_HOST_BUFFER_SIZE               0x0C33
#define BT_HCI_CMD_HOST_NUMBER_OF_COMPLETED_PACKETS     0x0C35
#define BT_HCI_CMD_READ_LINK_SUPERV_TIMEOUT       0x0C36
#define BT_HCI_CMD_WRITE_LINK_SUPERV_TIMEOUT      0x0C37
#define BT_HCI_CMD_READ_NUM_IAC                   0x0C38
#define BT_HCI_CMD_READ_CURRENT_IAC_LAP           0x0C39
#define BT_HCI_CMD_WRITE_CURRENT_IAC_LAP          0x0C3A
#define BT_HCI_CMD_READ_PAGE_SCAN_PERIOD_MODE     0x0C3B
#define BT_HCI_CMD_WRITE_PAGE_SCAN_PERIOD_MODE    0x0C3C
#define BT_HCI_CMD_READ_PAGE_SCAN_MODE            0x0C3D /* Not in 1.2 */
#define BT_HCI_CMD_WRITE_PAGE_SCAN_MODE           0x0C3E /* Not in 1.2 */
#define BT_HCI_CMD_SET_AFH_HOST_CHAN_CLASS        0x0C3F /* 1.2 */
#define BT_HCI_CMD_READ_INQ_SCAN_TYPE             0x0C42 /* 1.2 */
#define BT_HCI_CMD_WRITE_INQ_SCAN_TYPE            0x0C43 /* 1.2 */
#define BT_HCI_CMD_READ_INQ_MODE                  0x0C44 /* 1.2 */
#define BT_HCI_CMD_WRITE_INQ_MODE                 0x0C45 /* 1.2 */
#define BT_HCI_CMD_READ_PAGE_SCAN_TYPE            0x0C46 /* 1.2 */
#define BT_HCI_CMD_WRITE_PAGE_SCAN_TYPE           0x0C47 /* 1.2 */
#define BT_HCI_CMD_READ_AFH_CHAN_ASSESS_MODE      0x0C48 /* 1.2 */
#define BT_HCI_CMD_WRITE_AFH_CHAN_ASSESS_MODE     0x0C49 /* 1.2 */
#define BT_HCI_CMD_READ_ANONYMITY_MODE            0x0C4A /* 1.2 */
#define BT_HCI_CMD_WRITE_ANONYMITY_MODE           0x0C4B /* 1.2 */
#define BT_HCI_CMD_READ_ALIAS_AUTH_ENABLE         0x0C4C /* 1.2 */
#define BT_HCI_CMD_WRITE_ALIAS_AUTH_ENABLE        0x0C4D /* 1.2 */
#define BT_HCI_CMD_READ_ANON_ADDR_CHNG_PARM       0x0C4E /* 1.2 */
#define BT_HCI_CMD_WRITE_ANON_ADDR_CHNG_PARM      0x0C4F /* 1.2 */
#define BT_HCI_CMD_RESET_FIXED_ADDR_COUNTER       0x0C50 /* 1.2 */
#define BT_HCI_CMD_WRITE_EXT_INQUIRY_RESPONSE     0x0C52 /* 2.1 */
#define BT_HCI_CMD_WRITE_SIMPLE_PAIRING_MODE      0x0C56 /* 2.1 */
#define BT_HCI_CMD_READ_INQUIRY_RESPONSE_TX_PWR   0x0C58 /* 2.1 */
#define BT_HCI_CMD_WRITE_INQUIRY_TX_PWR           0x0C59 /* 2.1 */
#define BT_HCI_CMD_KEY_PRESS_NOTIFICATION            0x0C60 /* 2.1 */ // add for passkey entry
#define BT_HCI_CMD_READ_INQ_RSP_XMIT_POWER_LEVEL   0x0C58
#define BT_HCI_CMD_SET_EVENT_MASK_PAGE_2 =         0x0C63
#define BT_HCI_CMD_SET_RESERVED_LT_ADDR =          0x0C74
#define BT_HCI_CMD_DELETE_RESERVED_LT_ADDR         0x0C75 //TODO
#define BT_HCI_CMD_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA   0x0C76
#define BT_HCI_CMD_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS   0x0C78
#define BT_HCI_CMD_WRITE_FLOW_CONTROL_MODE           0x0C67 /* 3.0+HS */
#define BT_HCI_CMD_READ_LE_HOST_SUPPORT              0x0C6C /* 4.0 */
#define BT_HCI_CMD_WRITE_LE_HOST_SUPPORT             0x0C6D /* 4.0 */
#define BT_HCI_CMD_WRITE_SECURE_CONNECTION_HOST_SUPPORT          0x0C7A /* 4.1 Secure Connection */
#define BT_HCI_CMD_WRITE_AUTHENTICATED_TIMEOUT_COMMAND           0x0C7C /* 4.1 Secure Connection */

/**
 * @brief Informational Parameters, reference: 7.4 INFORMATIONAL PARAMETERS.
 */
#define BT_HCI_CMD_READ_LOCAL_VERSION_INFORMATION            0x1001
#define BT_HCI_CMD_READ_LOCAL_SUPPORTED_COMMANDS             0x1002
#define BT_HCI_CMD_READ_LOCAL_SUPPORTED_FEATURES             0x1003
#define BT_HCI_CMD_READ_BUFFER_SIZE                          0x1005
#define BT_HCI_CMD_READ_BD_ADDR                              0x1009

/**
 * @brief Satus Parameters, reference: 7.5 STATUS PARAMETERS.
 */
#define BT_HCI_CMD_READ_RSSI                                 0x1405
#define BT_HCI_CMD_READ_ENCRYPTION_KEY_SIZE                  0x1408

/**
 * @brief Testing Commands, reference: 7.6 TESTING COMMANDS.
 */
#define BT_HCI_CMD_READ_LOOPBACK_MODE                        0x1801
#define BT_HCI_CMD_WRITE_LOOPBACK_MODE                       0x1802
#define BT_HCI_CMD_ENABLE_DUT                                0x1803
#define BT_HCI_CMD_WRITE_SSP_DEBUG_MODE                      0x1804

/**
 * @brief Controller Commands, reference: 7.8 LE CONTROLLER COMMANDS.
 */
#define BT_HCI_CMD_LE_SET_EVENT_MASK                         0x2001
#define BT_HCI_CMD_LE_READ_BUFFER_SIZE                       0x2002
#define BT_HCI_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES          0x2003
#define BT_HCI_CMD_LE_SET_RANDOM_ADDRESS                     0x2005
#define BT_HCI_CMD_LE_SET_ADVERTISING_PARAMETERS             0x2006
#define BT_HCI_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER      0x2007
#define BT_HCI_CMD_LE_SET_ADVERTISING_DATA                   0x2008
#define BT_HCI_CMD_LE_SET_SCAN_RESPONSE_DATA                 0x2009
#define BT_HCI_CMD_LE_SET_ADVERTISING_ENABLE                 0x200A
#define BT_HCI_CMD_LE_SET_SCAN_PARAMETERS                    0x200B
#define BT_HCI_CMD_LE_SET_SCAN_ENABLE                        0x200C
#define BT_HCI_CMD_LE_CREATE_CONNECTION                      0x200D
#define BT_HCI_CMD_LE_CREATE_CONNECTION_CANCEL               0x200E
#define BT_HCI_CMD_LE_READ_WHITE_LIST_SIZE                   0x200F
#define BT_HCI_CMD_LE_CLEAR_WHITE_LIST                       0x2010
#define BT_HCI_CMD_LE_ADD_DEVICE_TO_WHITE_LIST               0x2011
#define BT_HCI_CMD_LE_REMOVE_DEVICE_FROM_WHITE_LIST          0x2012
#define BT_HCI_CMD_LE_CONNECTION_UPDATE                      0x2013
#define BT_HCI_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION        0x2014
#define BT_HCI_CMD_LE_READ_CHANNEL_MAP                       0x2015
#define BT_HCI_CMD_LE_READ_REMOTE_USED_FEATURES              0x2016
#define BT_HCI_CMD_LE_ENCRYPT                                0x2017
#define BT_HCI_CMD_LE_RAND                                   0x2018
#define BT_HCI_CMD_LE_START_ENCRYPTION                       0x2019
#define BT_HCI_CMD_LE_LONG_TERM_KEY_REQUEST_REPLY            0x201A
#define BT_HCI_CMD_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY   0x201B
#define BT_HCI_CMD_LE_READ_SUPPORTED_STATES                  0x201C
#define BT_HCI_CMD_LE_RECIEVER_TEST                          0x201D
#define BT_HCI_CMD_LE_TRANSMITTER_TEST                       0x201E
#define BT_HCI_CMD_LE_TEST_END                               0x201F
#define BT_HCI_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY            0x2020
#define BT_HCI_CMD_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY   0x2021
#define BT_HCI_CMD_LE_SET_DATA_LENGTH                        0x2022
#define BT_HCI_CMD_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH     0x2023
#define BT_HCI_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH    0x2024
#define BT_HCI_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY             0x2025
#define BT_HCI_CMD_LE_GENERATE_DHKEY                         0x2026
#define BT_HCI_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST           0x2027
#define BT_HCI_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST      0x2028
#define BT_HCI_CMD_LE_CLEAR_RESOLVING_LIST                   0x2029
#define BT_HCI_CMD_LE_READ_RESOLVING_LIST_SIZE               0x202A
#define BT_HCI_CMD_LE_READ_PEER_RESOLVABLE_ADDRESS           0x202B
#define BT_HCI_CMD_LE_READ_LOCAL_RESOLABLE_ADDRESS           0x202C
#define BT_HCI_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE          0x202D
#define BT_HCI_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT 0x202E
#define BT_HCI_CMD_LE_READ_MAXIMUM_DATA_LENGTH               0x202F

/* spec 5.0 new LE hci cmd */
#define BT_HCI_CMD_LE_READ_PHY                               0x2030
#define BT_HCI_CMD_LE_SET_DEFAULT_PHY                        0x2031
#define BT_HCI_CMD_LE_SET_PHY                                0x2032
#define BT_HCI_CMD_LE_ENHANCED_RECEIVER_TEST                 0x2033
#define BT_HCI_CMD_LE_ENHANCED_TRANSMITTER_TEST              0x2034
#define BT_HCI_CMD_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS     0x2035
#define BT_HCI_CMD_LE_SET_EXTENDED_ADVERTISING_PARAMETERS    0x2036
#define BT_HCI_CMD_LE_SET_EXTENDED_ADVERTISING_DATA          0x2037
#define BT_HCI_CMD_LE_SET_EXTENDED_SCAN_RESPONSE_DATA        0x2038
#define BT_HCI_CMD_LE_SET_EXTENDED_ADVERTISING_ENABLE        0x2039
#define BT_HCI_CMD_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH   0x203A
#define BT_HCI_CMD_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS 0x203B
#define BT_HCI_CMD_LE_REMOVE_ADVERTISING_SET                 0x203C
#define BT_HCI_CMD_LE_CLEAR_ADVERTISING_SETS                 0x203D
#define BT_HCI_CMD_LE_SET_PERIODIC_ADVERTISING_PARAMETERS    0x203E
#define BT_HCI_CMD_LE_SET_PERIODIC_ADVERTISING_DATA          0x203F
#define BT_HCI_CMD_LE_SET_PERIODIC_ADVERTISING_ENABLE        0x2040
#define BT_HCI_CMD_LE_SET_EXTENDED_SCAN_PARAMETERS           0x2041
#define BT_HCI_CMD_LE_SET_EXTENDED_SCAN_ENABLE               0x2042
#define BT_HCI_CMD_LE_EXTENDED_CREATE_CONNECTION             0x2043
#define BT_HCI_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC       0x2044
#define BT_HCI_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL   0x2045
#define BT_HCI_CMD_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC    0x2046
#define BT_HCI_CMD_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST 0x2047
#define BT_HCI_CMD_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST   0x2048
#define BT_HCI_CMD_LE_CLEAR_PERIODIC_ADVERTISER_LIST         0x2049
#define BT_HCI_CMD_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE     0x204A
#define BT_HCI_CMD_LE_SET_PRIVACY_MODE                       0x204E
#define BT_HCI_CMD_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE    0x2059
#define BT_HCI_CMD_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER     0x205A
#define BT_HCI_CMD_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER    0x205B
#define BT_HCI_CMD_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS     0x205C
#define BT_HCI_CMD_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS     0x205D

/* spec 5.2 new LE hci cmd Isochronous channels */
#define BT_HCI_CMD_LE_READ_BUFFER_SIZE_V2                    0x2060
#define BT_HCI_CMD_LE_READ_ISO_TX_SYNC                       0x2061
#define BT_HCI_CMD_LE_SET_CIG_PARAMETERS                     0x2062
#define BT_HCI_CMD_LE_SET_CIG_PARAMETERS_TEST                0x2063
#define BT_HCI_CMD_LE_CREATE_CIS                             0x2064
#define BT_HCI_CMD_LE_REMOVE_CIG                             0x2065
#define BT_HCI_CMD_LE_ACCEPT_CIS_REQUEST                     0x2066
#define BT_HCI_CMD_LE_REJECT_CIS_REQUEST                     0x2067
#define BT_HCI_CMD_LE_CREATE_BIG                             0x2068
#define BT_HCI_CMD_LE_CREATE_BIG_TEST                        0x2069
#define BT_HCI_CMD_LE_TERMINATE_BIG                          0x206A
#define BT_HCI_CMD_LE_BIG_CREATE_SYNC                        0x206B
#define BT_HCI_CMD_LE_BIG_TERMINATE_SYNC                     0x206C
#define BT_HCI_CMD_LE_REQUEST_PEER_SCA                       0x206D
#define BT_HCI_CMD_LE_SETUP_ISO_DATA_PATH                    0x206E
#define BT_HCI_CMD_LE_REMOVE_ISO_DATA_PATH                   0x206F
#define BT_HCI_CMD_LE_ISO_TRANSMIT_TEST                      0x2070
#define BT_HCI_CMD_LE_ISO_RECEIVE_TEST                       0x2071
#define BT_HCI_CMD_LE_ISO_READ_TEST_COUNTER                  0x2072
#define BT_HCI_CMD_LE_ISO_TEST_END                           0x2073
//#ifdef __MTK_COMMON__
#define BT_HCI_CMD_LE_SET_HOST_FEAUTRE                       0x2074
//#endif

/* Proprietary. */
#define BT_HCI_CMD_SET_BD_ADDR                               0xFC1A
#define BT_HCI_CMD_SET_LMP_EXTENDED_FEATURE                  0xFC1F
#define BT_HCI_CMD_READ_PROPRIETARY_EXTENDED_FEATURES        0xFC54
#define BT_HCI_CMD_WRITE_PROPRIETARY_EXTENDED_FEATURES       0xFC55
#define BT_HCI_CMD_SET_CONN_TX_POWER_LEVEL                   0xFC79
#define BT_HCI_CMD_ADVANCE_SET_CONN_TX_POWER_LEVEL           0xFDBB
#ifdef __BT_HB_ENABLE_SLEEP_MODE__
#define BT_HCI_CMD_SET_SLEEP_PARAM                           0xFC7A
#endif //__BT_HB_ENABLE_SLEEP_MODE__
#define BT_HCI_CMD_SET_FIRMWARE_LOG                          0xFCF1
#define BT_HCI_CMD_LE_SET_TX_POWER                           0xFC17
#define BT_HCI_SET_PINCODE_RSP_TIMEOUT                       0xFC66
#define BT_HCI_CMD_SET_SCO_STATUS                            0xFD80
#define BT_HCI_CMD_SWITCH_FW_PACKET_LOST_LOG                 0xFD90
#define BT_HCI_CMD_READ_TX_POWER_SETTINGS                    0xFDA0  //return [Rcal, init tx, Max tx]
#define BT_HCI_CMD_READ_RAW_RSSI                             0xFC61
#define BT_HCI_CMD_WRITE_FEATURE                             0xFC67
#define BT_HCI_CMD_SET_KEY_SIZE                              0xFC3C
#define BT_HCI_CMD_SET_KEY_SIZE_RANGE                        0xFC76
#define BT_HCI_CMD_GET_KEY_SIZE_RANGE                        0xFC3D
#define BT_HCI_CMD_VENDOR_LMP_REJECT_1M                      0xFC04
#define BT_HCI_CMD_VENDOR_LOWPOWER_SET_SYNCDROP              0xFC10
#define BT_HCI_CMD_VENDOR_SET_EARBUDS_PLUS_MODE              0xFC86

/* Google. */
#define BT_HCI_CMD_VENDOR_CAPABILITY                         0xFD53
#define BT_HCI_CMD_MULTIPLE_ADVERTISING                      0xFD54
#define BT_HCI_CMD_LE_SET_MULTI_ADV_PARAMETERS               0x01
#define BT_HCI_CMD_LE_SET_MULTI_ADV_DATA                     0x02
#define BT_HCI_CMD_LE_SET_MULTI_SCAN_RESP_DATA               0x03
#define BT_HCI_CMD_LE_SET_MULTI_ADV_RANDOM_ADDR              0x04
#define BT_HCI_CMD_LE_SET_MULTI_ADV_ENABLE                   0x05

/* aws oh */
#define BT_HCI_CMD_VENDOR_SET_AWS_AUD_ENHANCE_MODE           0xFDB2 /* AWS Enhancement */
#define BT_HCI_CMD_VENDOR_AWS_MCE_PREPARE_RHO                0xFDBA /* AWS_MCE PREPARE ROLE HANDOWER */
#define BT_HCI_CMD_VENDOR_AWS_MCE_RHO_APPLY_FINISHED         0xFDD2
#define BT_HCI_CMD_VENDOR_AWS_MCE_STOP_RHO                   0xFDD3
#define BT_HCI_CMD_VENDOR_AWS_MCE_DISALLOW_RHO_LE_LINK       0xFDD4
/*aws wb scan calibration paramters*/
#define BT_HCI_CMD_VENDOR_AWS_MCE_WB_SCAN_CALIB              0xFDE1

/* avm direction */
#define BT_HCI_CMD_VENDOR_SET_MEDIA_CHANNEL                  0xFDC1 /* Set A2DP media channel */
#define BT_HCI_CMD_VENDOR_SET_MEDIA_NOTIFY_CONDITION         0xFDC2
#define BT_HCI_CMD_VENDOR_SET_CLOCK_OFFSET_BUFFER            0xFDC3
#define BT_HCI_CMD_VENDOR_TRACKING_AUDSYS                    0xFDC4
#define BT_HCI_CMD_VENDOR_SET_SHARE_BUFFER                   0xFDC5
#define BT_HCI_CMD_VENDOR_SET_AUDIO_PLAY_EN                  0xFDC6
#define BT_HCI_CMD_VENDOR_SET_LEAUDIO_BUFFER                 0xFDC7  /* Set LE audio buffer for controller */
#define BT_HCI_CMD_VENDOR_SET_FEC_BUFFER                     0xFDC8
#define BT_HCI_CMD_VENDOR_SET_INQUIRY_DUPLICATE_FILTER       0xFDC9
#define BT_HCI_CMD_VENDOR_SET_EXT_SHARE_BUFFER               0xFDCA
#define BT_HCI_CMD_VENDOR_SET_ULL_GAMING_MODE                0xFDCB
#define BT_HCI_CMD_VENDOR_SET_DONGLE_MODE                    0xFDCC

/* bt driver notify controller to up to 64M */
#define BT_HCI_CMD_VENDOR_NOTIFY_CONTROLER_UP_to_64M         0xFC96

/* validate public key in host*/
#define BT_HCI_CMD_VENDOR_VALIDATE_PUBLIC_KEY                0xFDD1

/* get RF information */
#define BT_HCI_CMD_VENDOR_GET_RF_INFORMATION                 0xFDF1

/*for ull service*/
#define BT_HCI_CMD_VENDOR_SET_AIR_CIG_PARAMS                 0xFE00
#define BT_HCI_CMD_VENDOR_CREATE_AIR_CIS                     0xFE01
#define BT_HCI_CMD_VENDOR_REMOVE_AIR_CIG                     0xFE02
#define BT_HCI_CMD_VENDOR_ACCEPT_AIR_CIS_REQUEST             0xFE03
#define BT_HCI_CMD_VENDOR_REJECT_AIR_CIS_REQUEST             0xFE04
#define BT_HCI_CMD_VENDOR_SETUP_AIR_ISO_DATA_PATH            0xFE05
#define BT_HCI_CMD_VENDOR_REMOVE_AIR_ISO_DATA_PATH           0xFE06
#define BT_HCI_CMD_VENDOR_DISCONNECT_AIR_CIS                 0xFE07
#define BT_HCI_CMD_VENDOR_CHANGE_AIR_PARAMS                  0xFE08
#define BT_HCI_CMD_VENDOR_ACTIVIATE_UPLINK                   0xFE09
#define BT_HCI_CMD_VENDOR_UNMUTE_AIR_CIS                     0xFE0A
#define BT_HCI_CMD_VENDOR_SET_AIR_PARAMS_TABLE               0xFE0B

typedef uint16_t bt_hci_cmd_op_t;

/**
 * @brief HCI event code, reference: 7.7 EVENTS.
 */
#define BT_HCI_EVT_INQUIRY_COMPLETE                        0x01
#define BT_HCI_EVT_INQUIRY_RESULT                          0x02
#define BT_HCI_EVT_CONNECT_COMPLETE                        0x03
#define BT_HCI_EVT_CONNECT_REQUEST                         0x04
#define BT_HCI_EVT_DISCONNECTION_COMPLETE                  0x05
#define BT_HCI_EVT_AUTH_COMPLETE                           0x06
#define BT_HCI_EVT_REMOTE_NAME_REQ_COMPLETE                0x07
#define BT_HCI_EVT_ENCRYPTION_CHANGE                       0x08
#define BT_HCI_EVT_CHNG_CONN_LINK_KEY_COMPLETE             0x09
#define BT_HCI_EVT_MASTER_LINK_KEY_COMPLETE                0x0A
#define BT_HCI_EVT_READ_REMOTE_FEATURES_COMPLETE           0x0B
#define BT_HCI_EVT_READ_REMOTE_VERSION_COMPLETE            0x0C
#define BT_HCI_EVT_QOS_SETUP_COMPLETE                      0x0D
#define BT_HCI_EVT_COMMAND_COMPLETE                        0x0E
#define BT_HCI_EVT_COMMAND_STATUS                          0x0F
#define BT_HCI_EVT_HARDWARE_ERROR                          0x10
#define BT_HCI_EVT_FLUSH_OCCURRED                          0x11
#define BT_HCI_EVT_ROLE_CHANGE                             0x12
#define BT_HCI_EVT_NUMBER_OF_COMPLETED_PACKETS             0x13
#define BT_HCI_EVT_MODE_CHNG                               0x14
#define BT_HCI_EVT_RETURN_LINK_KEYS                        0x15
#define BT_HCI_EVT_PIN_CODE_REQ                            0x16
#define BT_HCI_EVT_LINK_KEY_REQ                            0x17   /* bt_bd_addr_t, bt_key_t */
#define BT_HCI_EVT_LINK_KEY_NOTIFY                         0x18
#define BT_HCI_EVT_LOOPBACK_COMMAND                        0x19
#define BT_HCI_EVT_DATA_BUFFER_OVERFLOW                    0x1A
#define BT_HCI_EVT_MAX_SLOTS_CHNG                          0x1B
#define BT_HCI_EVT_READ_CLOCK_OFFSET_COMPLETE              0x1C
#define BT_HCI_EVT_CONN_PACKET_TYPE_CHNG                   0x1D
#define BT_HCI_EVT_QOS_VIOLATION                           0x1E
#define BT_HCI_EVT_PAGE_SCAN_MODE_CHANGE                   0x1F /* Not in 1.2 */
#define BT_HCI_EVT_PAGE_SCAN_REPETITION_MODE               0x20
#define BT_HCI_EVT_FLOW_SPECIFICATION_COMPLETE             0x21 /* 1.2 */
#define BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI                0x22 /* 1.2 */
#define BT_HCI_EVT_READ_REMOTE_EXT_FEAT_COMPLETE           0x23 /* 1.2 */
#define BT_HCI_EVT_FIXED_ADDRESS                           0x24 /* 1.2 */
#define BT_HCI_EVT_ALIAS_ADDRESS                           0x25 /* 1.2 */
#define BT_HCI_EVT_GENERATE_ALIAS_REQ                      0x26 /* 1.2 */
#define BT_HCI_EVT_ACTIVE_ADDRESS                          0x27 /* 1.2 */
#define BT_HCI_EVT_ALLOW_PRIVATE_PAIRING                   0x28 /* 1.2 */
#define BT_HCI_EVT_ALIAS_ADDRESS_REQ                       0x29 /* 1.2 */
#define BT_HCI_EVT_ALIAS_NOT_RECOGNIZED                    0x2A /* 1.2 */
#define BT_HCI_EVT_FIXED_ADDRESS_ATTEMPT                   0x2B /* 1.2 */
#define BT_HCI_EVT_SYNC_CONNECT_COMPLETE                   0x2C /* 1.2 */
#define BT_HCI_EVT_SYNC_CONN_CHANGED                       0x2D /* 1.2 */
#define BT_HCI_EVT_SNIFF_SUBRATING_EVENT                   0x2E /* Posh_assert */
#define BT_HCI_EVT_INQUIRY_RESULT_WITH_EIR                 0x2F /* 2.1 */
#define BT_HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE         0x30 /* Posh_assert */
#define BT_HCI_EVT_IO_CAPABILITY_REQUEST                   0x31 /* 2.1 */
#define BT_HCI_EVT_IO_CAPABILITY_RESPONSE                  0x32 /* 2.1 */
#define BT_HCI_EVT_USER_CONFIRM_REQUSEST                   0x33 /* 2.1 */
#define BT_HCI_EVT_USER_PASSKEY_REQUEST_EVENT              0x34 /* Posh_assert */
#define BT_HCI_EVT_REMOTE_OOB_DATA_REQUEST_EVENT           0x35 /* Posh_assert */
#define BT_HCI_EVT_SIMPLE_PAIRING_COMPLETE                 0x36 /* 2.1 */
#define BT_HCI_EVT_LINK_SUPERVISION_TIMEOUT_CHNG           0x38 /* 2.1 */
#define BT_HCI_EVT_ENHANCED_FLUSH_COMPLETE                 0x39 /* 2.1 */
#define BT_HCI_EVT_USER_PASSKEY_NOTIFICATION               0x3B /* 2.1 */
#define BT_HCI_EVT_USER_KEYPRESS                           0x3C /* 2.1 */
#define BT_HCI_EVT_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT    0x3D    /* Posh_assert */
#define BT_HCI_EVT_LE_META_EVENT                           0x3E /* 4.0 */
#define BT_HCI_EVT_PHYSICAL_LINK_COMPLETE                  0x40 /* 3.0+HS */
#define BT_HCI_EVT_CHANNEL_SELECTED                        0x41 /* 3.0+HS */
#define BT_HCI_EVT_DISCONNECT_PHYSICAL_LINK                0x42 /* 3.0+HS */
#define BT_HCI_EVT_PHYSICAL_LINK_EARLY_WARNING             0x43 /* 3.0+HS */
#define BT_HCI_EVT_PHYSICAL_LINK_RECOVERY                  0x44 /* 3.0+HS */
#define BT_HCI_EVT_LOGICAL_LINK_COMPLETE                   0x45 /* 3.0+HS */
#define BT_HCI_EVT_DISCONNECT_LOGICAL_LINK                 0x46 /* 3.0+HS */
#define BT_HCI_EVT_NUM_OF_COMPLETE_DATA_BLOCKS             0x48 /* 3.0+HS */
#define BT_HCI_EVT_AMP_STATUS_CHANGE                       0x4D /* 3.0+HS */
#define BT_HCI_EVT_TRIGGERED_CLOCK_CAPTURE                 0x4E
#define BT_HCI_EVT_SYNCHRONIZATION_TRAIN_COMPLETE          0x4F
#define BT_HCI_EVT_SYNCHRONIZATION_TRAIN_RECEIVED          0x50
#define BT_HCI_EVT_CONECTIONLESS_SLAVE_BROADCAST_RECEIVED  0x51
#define BT_HCI_EVT_CONECTIONLESS_SLAVE_BROADCAST_TIMEOUT   0x52
#define BT_HCI_EVT_TRUNCATED_PAGE_COMPLETE                 0x53
#define BT_HCI_EVT_SLAVE_PAGE_RESPONSE_TIMEOUT             0x54
#define BT_HCI_EVT_CONNECTIONLESS_SLAVE_BROADCAST_CHANNEL_MAP_CHANGE  0x55
#define BT_HCI_EVT_INQUIRY_RESPONSE_NOTIFICATION           0x56
#define BT_HCI_EVT_AUTHENTICATED_PAYLOAD_TIMEOUT           0x57 /* BT4.1 */

#define BT_HCI_EVT_VENDOR_DUT_ACTIVE                       0xF5
#define BT_HCI_EVT_VENDOR_BT_POWER_OFF                     0xF6
#define BT_HCI_EVT_LE_VENDOR_EVENT                         0xFF
#define BT_HCI_EVT_MTK_ASSERT_EVENT                        0xFE

#define BT_HCI_EVT_VENDOR_SEQUENCE_NUM_WRAP_COUNT           0xE0
#define BT_HCI_EVT_VENDOR_MEDIA_PACKET_DONE                 0xEA
#define BT_HCI_EVT_VENDOR_CALL_ANCHOR_POINT                 0xEB
#define BT_HCI_EVT_VENDOR_MEDIA_DATA_RECEIVED               0xEB
#define BT_HCI_EVT_VENDOR_SET_LOCAL_ASI_FLAG                0xEC
#define BT_HCI_EVT_VENDOR_AWS_PLUS_MODE_CHANGED             0xED
#define BT_HCI_EVT_VENDOR_RHO_NEW_LINK_INFO                 0xEE
#define BT_HCI_EVT_VENDOR_RHO_LE_NEW_LINK_INFO              0xEF
#define BT_HCI_EVT_VENDOR_AWS_PREPARE_RHO_READY             0xFC
#define BT_HCI_EVT_VENDOR_ESCO_CONNECTION_PENDING           0xD0

/* validate public key in host*/
#define BT_HCI_EVT_VENDOR_VALIDATE_PUBLIC_KEY               0xF1

#define BT_HCI_EVT_VENDOR_RPA_ROTATION_EVENT                0xF6

#define BT_HCI_EVT_VENDOR_AGNET_MODE_CHANGED                0xF8
#define BT_HCI_EVT_VENDOR_EDR_ENABLE_EVENT                  0x02
#define BT_HCI_EVT_VENDOR_AWS_READY_EVENT                   0x03

typedef uint8_t bt_hci_evt_op_t;

/**
 * @brief CI subevent code, reference: 7.7 EVENTS.
 */
#define BT_HCI_SUBEVT_LE_CONNECTION_COMPLETE                 0x01
#define BT_HCI_SUBEVT_LE_ADVERTISING_REPORT                  0x02
#define BT_HCI_SUBEVT_LE_CONNECTION_UPDATE_COMPLETE          0x03
#define BT_HCI_SUBEVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE  0x04
#define BT_HCI_SUBEVT_LE_LONG_TERM_KEY_REQUEST               0x05
#define BT_HCI_SUBEVT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST 0x06
#define BT_HCI_SUBEVT_LE_DATA_LENGTH_CHANGE                  0x07
#define BT_HCI_SUBEVT_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE 0x08
#define BT_HCI_SUBEVT_LE_GENERATE_DHKEY_COMPLETE             0x09
#define BT_HCI_SUBEVT_LE_ENHANCED_CONNECTION_COMPLETE        0x0A
#define BT_HCI_SUBEVT_LE_DIRECT_ADVERTISING_REPORT           0x0B
#define BT_HCI_SUBEVT_LE_PHY_UPDATE_COMPLETE                 0x0C
#define BT_HCI_SUBEVT_LE_EXTENDED_ADVERTISING_REPORT         0x0D
#define BT_HCI_SUBEVT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED  0x0E
#define BT_HCI_SUBEVT_LE_PERIODIC_ADVERTISING_REPORT         0x0F
#define BT_HCI_SUBEVT_LE_PERIODIC_ADVERTISING_SYNC_LOST      0x10
#define BT_HCI_SUBEVT_LE_SCAN_TIMEOUT                        0x11
#define BT_HCI_SUBEVT_LE_ADVERTISING_SET_TERMINATED          0x12
#define BT_HCI_SUBEVT_LE_SCAN_REQUEST_RECEIVED               0x13
#define BT_HCI_SUBEVT_LE_CHANNEL_SELECTION_ALGORITHM         0x14
#define BT_HCI_SUBEVT_LE_CONNECTIONLESS_IQ_REPORT            0x15
#define BT_HCI_SUBEVT_LE_CONNECTION_IQ_REPORT                0x16
#define BT_HCI_SUBEVT_LE_CTE_REQUEST_FAILED                  0x17
#define BT_HCI_SUBEVT_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED    0x18
#define BT_HCI_SUBEVT_LE_CIS_ESTABLISHED                     0x19
#define BT_HCI_SUBEVT_LE_CIS_REQUEST                         0x1A
#define BT_HCI_SUBEVT_LE_CREATE_BIG_COMPLETE                 0x1B
#define BT_HCI_SUBEVT_LE_TERMINATE_BIG_COMPLETE              0x1C
#define BT_HCI_SUBEVT_LE_BIG_SYNC_ESTABLISHED                0x1D
#define BT_HCI_SUBEVT_LE_BIG_SYNC_LOST                       0x1E
#define BT_HCI_SUBEVT_LE_REQUEST_PEER_SCA_COMPLETE           0x1F
#define BT_HCI_SUBEVT_LE_PATH_LOSS_THRESHOLD                 0x20
#define BT_HCI_SUBEVT_LE_TRANSMIT_POWER_REPORTING            0x21
#define BT_HCI_SUBEVT_LE_BIGINFO_ADVERTISING_REPORT          0x22

/*for ull service*/
#define BT_HCI_SUBEVT_LE_VENDOR_AIR_CIS_REQUEST              0xF1
#define BT_HCI_SUBEVT_LE_VENDOR_AIR_CIS_ESTABLISHED          0xF2
#define BT_HCI_SUBEVT_LE_VENDOR_AIR_CIS_DISCONNECT_COMPLETE  0xF3
#define BT_HCI_SUBEVT_LE_VENDOR_AIR_CIG_PARAMS_CHANGED_EVENT      0xF4
#define BT_HCI_SUBEVT_LE_VENDOR_AIR_CIS_UPLINK_ACTIVIATED_EVENT    0xF5


typedef uint8_t bt_hci_subevt_op_t;

/**
 * @}
 */

/**
 * @defgroup hbif_hci_spec_struct Struct
 * @{
 */

/**
 * @brief Below is defined in the Bluetooth core specifications version 4.2 [Vol 2, Part E] Host Controller Interface Functional Specification,
 *        CH7 HCI COMMANDS AND EVENTS.
 */

/**
 * @brief Controller & Baseband Command Structure, reference: 7.3 CONTROLLER & BASEBAND COMMANDS.
 */
/**
 *  @brief      Host number of completed packets command.
 */
BT_PACKED(
typedef struct {
    uint8_t           number_of_handles;             /**< Number of connection handles */
    /* Below is temporary code as number_of_handles = 1 */
    bt_handle_t       connection_handle;             /**< Connection handle */
    uint16_t          host_num_of_completed_packets; /**< Number of completed data packets for the associated connection handle */
}) bt_hci_cmd_host_number_of_completed_packets_t;



/* LE Controller Command, reference: 7.8 LE CONTROLLER COMMANDS. */
/**
 *  @brief      LE set event mask command.
 */
BT_PACKED(
typedef struct {
    uint8_t           le_event_mask[8]; /**< LE event mask */
}) bt_hci_cmd_le_set_event_mask_t;

//#ifdef __MTK_COMMON__
/**
 *  @brief      LE set host feature
 */
BT_PACKED(
typedef struct {
    uint8_t           bit_num;  /**< LE bit position in the feature set */
    uint8_t           bit_val;  /**< the value of bit position */
}) bt_hci_cmd_le_set_host_feature_t;
//#endif

/**
 *  @brief      Set event mask command.
 */
BT_PACKED(
typedef struct {
    uint8_t           event_mask[8]; /**< Event mask. */
}) bt_hci_cmd_set_event_mask_t;

/**
 *  @brief      Set sco active command data.
 */
BT_PACKED(
typedef struct {
    bt_handle_t           handle; /**< Event mask. */
    uint8_t               active;
}) bt_hci_cmd_set_sco_active_t;

/**
 *  @brief      Set controller to host flow control command.
 */
BT_PACKED(
typedef struct {
    bt_hci_enable_t   flow_control_enable; /**< Flow control enable. */
}) bt_hci_cmd_set_controller_to_host_flow_control_t;

/**
 *  @brief      Host buffer size command.
 */
BT_PACKED(
typedef struct {
    uint16_t          host_acl_data_packet_length;             /**< Maximum length of each ACL data packet. */
    uint8_t           host_synchronous_data_packet_length;     /**< Maximum length of each synchronous data packet. */
    uint16_t          host_total_num_acl_data_packets;         /**< Total number of ACL data packets that can be stored in Host. */
    uint16_t          host_total_num_synchronous_data_packets; /**< Total number of synchronous data packets that can be stored in Host. */
}) bt_hci_cmd_host_buffer_size_t;

/**
 *  @brief      LE set random address command.
 */
BT_PACKED(
typedef struct {
    bt_bd_addr_t      random_address; /**< Random device address */
}) bt_hci_cmd_le_set_random_address_t;

/**
 *  @brief      LE read remote used features command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
}) bt_hci_cmd_le_read_remote_used_features_t;

/**
 *  @brief      LE encrypt command. please refer to 7.8.22 LE Encrypt Command.
 */
BT_PACKED(
typedef struct {
    bt_key_t          key;                /**< 128 bit key for the encryption of the data given in the command */
    bt_key_t          plaintext_data;     /**< 128 bit data block that is requested to be encrypted */
}) bt_hci_cmd_le_encrypt_t;

/**
 *  @brief      LE start encryption command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;     /**< Connection handle to be used to identify a connection */
    uint8_t           random_number[8];      /**< 64 bit random number */
    uint16_t          encrypted_diversifier; /**< 16 bit encrypted diversifier */
    bt_key_t          long_term_key;         /**< 128 bit long term key */
}) bt_hci_cmd_le_start_encryption_t;

/**
 *  @brief      LE long term key request reply command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
    bt_key_t          long_term_key;     /**< 128 bit long term key */
}) bt_hci_cmd_le_long_term_key_request_reply_t;

/**
 *  @brief      LE long term key request negative reply command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
}) bt_hci_cmd_le_long_term_key_request_negative_reply_t;

/**
 *  @brief      LE remote connection paramter request negative reply command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
    uint8_t           reason;            /**< reason */
}) bt_hci_cmd_le_remote_connection_parameter_request_negative_reply_t;

/**
 *  @brief      LE write suggested default data length command.
 */
BT_PACKED(
typedef struct {
    uint16_t          suggested_max_tx_octets; /**< Suggested maximum tx octets */
    uint16_t          suggested_max_tx_time;   /**< Suggested maximum tx time */
}) bt_hci_cmd_le_write_suggested_default_data_length_t;

/**
 *  @brief      LE generate dhkey command.
 */
BT_PACKED(
typedef struct {
    uint8_t           remote_p256_public_key[64]; /**< Remote p256 public key */
}) bt_hci_cmd_le_generate_dhkey_t;


/**
 *  @brief      Connection oriented command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       handle; /**< Handle */
    bt_data_t         data;   /**< Data */
}) bt_hci_cmd_connection_oriented_t;

/**
 *  @brief      Read automatic flush timeout command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       handle;          /**< Handle */
}) bt_hci_cmd_read_automatic_flush_timeout_t;

/**
 *  @brief      Write automatic flush timeout command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       handle;          /**< Handle */
    uint16_t          flush_timeout;   /**< Data */
}) bt_hci_cmd_write_automatic_flush_timeout_t;

/**
 *  @brief      Change the packet type command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       handle;          /**< Handle */
    uint16_t          type;     /**< packet type */
}) bt_hci_cmd_change_packet_type_t;


/**
 *  @brief      LE set multiple advertising random address command.
 */
BT_PACKED(
typedef struct {
    bt_bd_addr_t      random_address;    /**< Random device address. */
    uint8_t           instance;          /**< Advertising instance. */
}) bt_hci_cmd_le_set_multi_advertising_random_address_t;

/**
 *  @brief      LE set multiple advertising parameters command.
 */
BT_PACKED(
typedef struct {
    uint16_t          advertising_interval_min;   /**< Minimum advertising interval. The range is from 0x0020 to 0x4000. */
    uint16_t          advertising_interval_max;   /**< Maximum advertising interval. The range is from 0x0020 to 0x4000, and should be greater than the minimum advertising interval. */
    bt_hci_advertising_type_t   advertising_type; /**< Advertising type. */
    bt_addr_t         own_address;                /**< Local device address. */
    bt_addr_t         peer_address;               /**< Peer address. */
    uint8_t           advertising_channel_map;    /**< Advertising channel map. */
    uint8_t           advertising_filter_policy;  /**< Advertising filter policy. */
    uint8_t           instance;                   /**< Advertising instance. */
    int8_t            tx_power;                   /**< Advertising TX power, Range[-70..20]. */
}) bt_hci_cmd_le_set_multi_advertising_parameters_t;

/**
 *  @brief      LE set multiple advertising data command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_data_length; /**< Length of the advertising data. */
    uint8_t           advertising_data[31];    /**< Advertising data. */
    uint8_t           instance;                /**< Advertising instance. */
}) bt_hci_cmd_le_set_multi_advertising_data_t;

/**
 *  @brief      LE set multiple scan response data command.
 */
BT_PACKED(
typedef struct {
    uint8_t           scan_response_data_length; /**< Length of the scan response data. */
    uint8_t           scan_response_data[31];    /**< Scan response data. */
    uint8_t           instance;                  /**< Advertising instance. */
}) bt_hci_cmd_le_set_multi_scan_response_data_t;

/**
 *  @brief      LE set multiple advertising enable command.
 */
BT_PACKED(
typedef struct {
    bt_hci_enable_t   advertising_enable;      /**< Enable or disable advertising. */
    uint8_t           instance;                /**< Advertising instance. */
}) bt_hci_cmd_le_set_multi_advertising_enable_t;

/**
 *  @brief      LE read phy command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;    /**< Connection handle. */
}) bt_hci_cmd_le_read_phy_t;

/**
 *  @brief      LE set phy command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;    /**< Connection handle. */
    uint8_t           all_phys;             /**< All phys. */
    bt_hci_le_set_phy_t     phy_params;     /**< phy params. */
}) bt_hci_cmd_le_set_phy_t;

/**
 *  @brief      LE set advertising set random address command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;   /**< Advertising handle. */
    bt_bd_addr_t      advertising_random_address;   /**< Advertising random address. */
}) bt_hci_cmd_le_set_advertising_set_random_address_t;

/**
 *  @brief      LE set extended advertising parameters command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;   /**< Advertising handle. */
    bt_hci_le_set_ext_advertising_parameters_t  adv_params;
}) bt_hci_cmd_le_set_extended_advertising_parameters_t;

/**
 *  @brief      LE set extended advertising data command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;   /**< Advertising handle. */
    uint8_t           operation;
    uint8_t           fragment_preference;
    uint8_t           advertising_data_length;    /**< Length of the advertising data. */
    bt_data_t         advertising_data;         /**< Advertising data. */
}) bt_hci_cmd_le_set_extended_advertising_data_t;

/**
 *  @brief      LE set extended scan response data command.
 */
typedef bt_hci_cmd_le_set_extended_advertising_data_t bt_hci_cmd_le_set_extended_scan_response_data_t;

/**
 *  @brief      LE set extended advertising enable command.
 */
BT_PACKED(
typedef struct {
    uint8_t           enable;
    uint8_t           number_of_sets;
    uint8_t           advertising_handle;         /**< Advertsing handle. */
    uint16_t          duration;
    uint8_t           max_ext_advertising_evts;
}) bt_hci_cmd_le_set_extended_advertising_enable_t;

/**
 *  @brief      LE set remove advertising set command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;         /**< Advertsing handle. */
}) bt_hci_cmd_le_remove_advertising_set_t;

/**
 *  @brief      LE set periodic advertising parameters command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;         /**< Advertsing handle. */
    uint16_t          periodic_advertising_interval_min;
    uint16_t          periodic_advertising_interval_max;
    uint16_t          periodic_advertising_properties;
}) bt_hci_cmd_le_set_periodic_advertising_parameters_t;

/**
 *  @brief      LE set periodic advertising data command.
 */
BT_PACKED(
typedef struct {
    uint8_t           advertising_handle;         /**< Advertsing handle. */
    uint8_t           operation;
    uint8_t           advertising_data_length;    /**< Length of the advertising data. */
    bt_data_t         advertising_data;           /**< Advertising data. */
}) bt_hci_cmd_le_set_periodic_advertising_data_t;

/**
 *  @brief      LE set periodic advertising enable command.
 */
BT_PACKED(
typedef struct {
    uint8_t           enable;
    uint8_t           advertising_handle;         /**< Advertsing handle. */
}) bt_hci_cmd_le_set_periodic_advertising_enable_t;

/**
 *  @brief      LE set extended create connection command.
 */
BT_PACKED(
typedef struct {
    bt_hci_conn_filter_type_t   initiator_filter_policy; /**< Initiator filter policy. */
    bt_addr_type_t    own_address_type;                  /**< Address type of the local device. */
    bt_addr_t         peer_address;                      /**< The address of the peer device. */
    uint8_t           initiating_phys;                   /**< Initiating phys. */
    uint16_t          scan_interval;                     /**< Scan interval. The range is from 0x0004 to 0x4000. */
    uint16_t          scan_window;                       /**< Scan window. The range is from 0x0004 to 0x4000. */
    uint16_t          conn_interval_min;                 /**< The minimum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_interval_max;                 /**< The maximum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_latency;                      /**< Slave latency for the connection. The range is from 0x0000 to 0x01F3. */
    uint16_t          supervision_timeout;               /**< Supervision timeout for the LE link. The range is from 0x000A to 0x0C80. */
    uint16_t          minimum_ce_length;                 /**< The minimum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
    uint16_t          maximum_ce_length;                 /**< The maximum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
}) bt_hci_cmd_le_set_extended_create_connection_t;

/**
 *  @brief      LE periodic advertising terminate sync command.
 */
BT_PACKED(
typedef struct {
    uint16_t          sync_handle;
}) bt_hci_cmd_le_periodic_advertising_terminate_sync_t;

/**
 *  @brief      LE set extended scan parameters command.
 */
BT_PACKED(
typedef struct {
    uint8_t           own_address_type;
    uint8_t           scanning_filter_policy;
    uint8_t           scanning_phys;
    le_ext_scan_item_t scan_items;
}) bt_hci_cmd_le_set_extended_scan_parameters_t;

/**
 *  @brief      LE extended create connection command.
 */
BT_PACKED(
typedef struct {
    bt_hci_conn_filter_type_t   initiator_filter_policy; /**< Initiator filter policy. */
    bt_addr_type_t    own_address_type;                  /**< Address type of the local device. */
    bt_addr_t         peer_address;                      /**< The address of the peer device. */
    bt_hci_le_phy_t   initiating_phys;                   /**< The phys. */
    bt_hci_le_ext_connection_params_t conn_items[3];     /**< The parameters array, the number is decided by how many bits be seted to one in initiating_phys. */
}) bt_hci_cmd_le_extended_create_connection_t;

/* Proprietary. */

/**
 *  @brief      Set bd addr Command.
 */
BT_PACKED(
typedef struct {
    bt_bd_addr_t      public_address; /**< Public address */
}) bt_hci_cmd_set_bd_addr_t;

/**
 * @brief Below command complete events only have status parameter, so define a general structure for common use.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status; /**< Status */
}) bt_hci_evt_cc_general_t;

typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_set_event_mask_t;
#ifdef __BT_HB_ENABLE_SLEEP_MODE__
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_enable_sleep_mode_t;
#endif //__BT_HB_ENABLE_SLEEP_MODE__
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_reset_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_host_buffer_size_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_set_controller_to_host_flow_control_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_set_bd_addr_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_event_mask_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_random_address_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_advertising_parameters_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_advertising_data_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_scan_response_data_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_advertise_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_scan_parameters_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_scan_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_create_connection_cancel_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_clear_white_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_add_device_to_white_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_remove_device_from_white_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_write_suggested_default_data_length_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_add_device_to_resolving_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_remove_device_from_resolving_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_clear_resolving_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_address_resolution_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_resolvable_private_address_timeout_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_phy_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_advertising_set_random_address_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_advertising_data_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_extended_scan_response_data_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_extended_advertising_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_remove_advertising_set_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_clear_advertising_sets_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_periodic_advertising_parameters_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_periodic_advertising_data_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_periodic_advertising_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_extended_scan_parameters_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_extended_scan_enable_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_periodic_advertising_create_sync_cancel_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_periodic_terminate_sync_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_add_device_to_periodic_advertiser_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_remove_device_from_periodic_advertiser_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_clear_periodic_advertiser_list_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_write_rf_path_compensation_t;
typedef bt_hci_evt_cc_general_t bt_hci_evt_cc_le_set_privacy_mode_t;

/**
 * @brief Command Complete Event - Informational Parameters, reference: 7.4 INFORMATIONAL PARAMETER.
 */
/**
 *  @brief      Read local version information command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;             /**< Status */
    uint8_t           hci_version;        /**< HCI version */
    uint16_t          hci_revision;       /**< HCI reversion */
    uint8_t           lmp_pal_version;    /**< Version of the current LMP or PAL in the controller */
    uint16_t          manufacturer_name;  /**< Manufacture Name */
    uint16_t          lmp_pal_subversion; /**< Subversion of the current LMP or PAL in the controller */
}) bt_hci_evt_cc_read_local_version_information_t;

/**
 *  @brief      Read local supported commands command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                 /**< Status */
    uint8_t           supported_commands[64]; /**< Bit mask list of supported commands */
}) bt_hci_evt_cc_read_local_supported_commands_t;

/**
 *  @brief      Read local supported features command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;          /**< Status */
    uint8_t           lmp_features[8]; /**< Bit mask list of lmp features */
}) bt_hci_evt_cc_read_local_supported_features_t;

/**
 *  @brief      Read buffer size command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                                /**< Status */
    uint16_t          hc_acl_data_packet_length;             /**< Maximum length of each ACL data packet */
    uint8_t           hc_synchronous_data_packet_length;     /**< Maximum length of each synchronous data packet */
    uint16_t          hc_total_num_acl_data_packets;         /**< Total number of ACL data packets that can be stored in controller */
    uint16_t          hc_total_num_synchronous_data_packets; /**< Total number of synchronous data packets that can be stored in controller */
}) bt_hci_evt_cc_read_buffer_size_t;

/**
 *  @brief      Read bd addr command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;  /**< Status */
    bt_bd_addr_t      bd_addr; /**< BD address */
}) bt_hci_evt_cc_read_bd_addr_t;

/**
 *  @brief      LE read buffer size command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                       /**< Status */
    uint16_t          hc_le_data_packet_length;     /**< Maximum length of each ACL data packet */
    uint8_t           hc_total_num_le_data_packets; /**< Total number of ACL data packets that can be stored in controller */
}) bt_hci_evt_cc_le_read_buffer_size_t;

// #ifdef __MTK_COMMON__
/**
 *  @brief      LE read buffer size v2 command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                        /**< Status */
    uint16_t          hc_le_acl_data_pkt_len;        /**< Maximum length of each ACL data packet */
    uint8_t           hc_total_num_le_acl_data_pkts; /**< Total number of ACL data packets that can be stored in controller */
    uint16_t          hc_iso_data_pkt_len;           /**< Maximum length of each ISO data packet */
    uint8_t           hc_total_num_iso_data_pkts;    /**< Total number of ISO data packets that can be stored in controller */
}) bt_hci_evt_cc_le_read_buffer_size_v2_t;
// #endif

/**
 *  @brief      LE read local supported features command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;         /**< Status */
    uint8_t           le_features[8]; /**< Bit mask list of le features, please refer to core spec [Vol 6] Part B, Section 4.6. FEATURE SUPPORT. */
}) bt_hci_evt_cc_le_read_local_supported_features_t;

/**
 *  @brief      LE read white list size command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;          /**< Status */
    uint8_t           white_list_size; /**< Size of white list */
}) bt_hci_evt_cc_le_read_white_list_size_t;

/**
 *  @brief      LE encrypt command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;         /**< Status */
    bt_key_t          encrypted_data; /**< Encrypted Data */
}) bt_hci_evt_cc_le_encrypt_t;

/**
 *  @brief      LE rand command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;           /**< Status */
    uint8_t           random_number[8]; /**< Generated 64-bit random number */
}) bt_hci_evt_cc_le_rand_t;

/**
 *  @brief      LE long term key request negative reply command complete event.
 */
typedef bt_hci_evt_cc_le_long_term_key_request_reply_t
bt_hci_evt_cc_le_long_term_key_request_negative_reply_t;
/**
 *  @brief      LE remote connection parameter request reply command complete event.
 */
typedef bt_hci_evt_cc_le_long_term_key_request_reply_t
bt_hci_evt_cc_le_remote_connection_parameter_request_reply_t;
/**
 *  @brief      LE remote connection parameter request negative reply command complete event.
 */
typedef bt_hci_evt_cc_le_long_term_key_request_reply_t
bt_hci_evt_cc_le_remote_connection_parameter_request_negative_reply_t;

/**
 *  @brief      LE read suggested default data length command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                  /**< Status */
    uint16_t          suggested_max_tx_octets; /**< Suggested maximum number of payload octets for transmission */
    uint16_t          suggested_max_tx_time;   /**< Suggested maximum tx time */
}) bt_hci_evt_cc_le_read_suggested_default_data_length_t;

/**
 *  @brief      LE read resolving list size command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;              /**< Status */
    uint8_t           resolving_list_size; /**< Size of resolving list */
}) bt_hci_evt_cc_le_read_resolving_list_size_t;

/**
 *  @brief      LE read peer resolvable address command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                  /**< Status */
    bt_bd_addr_t      peer_resolvable_address; /**< Resolvable address of the remote device */
}) bt_hci_evt_cc_le_read_peer_resolvable_address_t;

/**
 *  @brief      LE read local resolvable address command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status */
    bt_bd_addr_t      local_resolvable_address; /**< Resolvable address of the local device */
}) bt_hci_evt_cc_le_read_local_resolvable_address_t;

/**
 *  @brief      LE read maximum data length command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                  /**< Status */
    uint16_t          supported_max_tx_octets; /**< Supported maximum number of payload octets for transmission */
    uint16_t          supported_max_tx_time;   /**< Local controller supported maximum tx time in microseconds */
    uint16_t          supported_max_rx_octets; /**< Supported maximum number of payload octets for reception */
    uint16_t          supported_max_rx_time;   /**< Local controller supported maximum rx time in microseconds */
}) bt_hci_evt_cc_le_read_maximum_data_length_t;

/**
 *  @brief      LE set multiple advertising command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status */
    uint8_t           multi_adv_opcode;         /**< Multiple advertising opcode */
}) bt_hci_evt_cc_le_set_multi_adv_t;

/**
 *  @brief      LE set extended advertising parameters command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status. */
    int8_t            selected_tx_power;        /**< The transmit power selected by controller. */
}) bt_hci_evt_cc_le_set_extended_advertising_parameters_t;

/**
 *  @brief      LE read maximum advertising data length command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status. */
    uint16_t          maximum_advertising_data_length;  /**< Maximum supported advertising data length. */
}) bt_hci_evt_cc_le_read_maximum_advertising_data_length_t;

/**
 *  @brief      LE read number of supported advertising sets command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status. */
    uint8_t           num_supported_advertising_sets;  /**< Number of advertising sets supported at the same time. */
}) bt_hci_evt_cc_le_read_number_of_supported_advertising_sets_t;

/**
 *  @brief      LE read periodic advertiser list size command complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                   /**< Status. */
    uint8_t           periodic_advertiser_list_size;  /**< Number of periodic advertiser list size. */
}) bt_hci_evt_cc_le_read_periodic_advertiser_list_size_t;

/**
 * @brief LE Meta Event, reference: 7.7.65 LE Meta Event.
 */
/**
 *  @brief      LE connection complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                /**< Status */
    bt_handle_t       connection_handle;     /**< Connection handle */
    bt_role_t         role;                  /**< Role */
    bt_addr_type_t    peer_address_type;     /**< Address type of the remote device */
    bt_bd_addr_t      peer_address;          /**< Remote device address */
    uint16_t          conn_interval;         /**< Connection interval used on this connection, range: 0x0006 to 0x0C80 */
    uint16_t          conn_latency;          /**< Slave latency for the connection, range: 0x0000 to 0x01F3 */
    uint16_t          supervision_timeout;   /**< Supervision timeout, range: 0x000A to 0x0C80 */
    uint8_t           master_clock_accuracy; /**< Master clock accuracy */
}) bt_hci_subevt_le_connection_complete_t;

/**
 *  @brief      LE advertising report event.
 */
BT_PACKED(
typedef struct {
    uint8_t           num_reports;  /**< Number of responses in event */
    /* Below is temporary code as num_reports = 1 */
    uint8_t           event_type;   /**< Event type */
    bt_addr_type_t    address_type; /**< Address type */
    bt_bd_addr_t      address;      /**< Address */
    uint8_t           length_data;  /**< Length of data */
    uint8_t           data[0x1F];   /**< Data */
    int8_t            rssi;         /**< RSSI */
}) bt_hci_subevt_le_advertising_report_t;

/**
 *  @brief      LE connection update complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;              /**< Status */
    bt_handle_t       connection_handle;   /**< Connection handle */
    uint16_t          conn_interval;       /**< Connection interval used on this connection, range: 0x0006 to 0x0C80 */
    uint16_t          conn_latency;        /**< Slave latency for the connection, range: 0x0000 to 0x01F3 */
    uint16_t          supervision_timeout; /**< Supervision timeout, range: 0x000A to 0x0C80 */
}) bt_hci_subevt_le_connection_update_complete_t;

/**
 *  @brief      Le read remote used features complete event
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;            /**< Status */
    bt_handle_t       connection_handle; /**< Connection handle */
    uint8_t           le_features[8];    /**< Bit mask list of used le features, please refer to core spec [Vol 6] Part B, Section 4.6 FEATURE SUPPORT. */
}) bt_hci_subevt_le_read_remote_used_features_complete_t;

/**
 *  @brief      LE long term key request event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;     /**< Connection handle */
    uint8_t           random_number[8];      /**< 64-bit random number */
    uint16_t          encrypted_diversifier; /**< 16-bit encrypted diversifier */
}) bt_hci_subevt_le_long_term_key_request_t;

/**
 *  @brief      LE remote connection parameter request event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
    uint16_t          interval_min;      /**< Minimum connection interval, range: 0x0006 to 0x0C80 */
    uint16_t          interval_max;      /**< Maximum connection interval, range: 0x0006 to 0x0C80 */
    uint16_t          latency;           /**< Maximum allowed slave latency, range: 0x0000 to 0x01F3 */
    uint16_t          timeout;           /**< Supervision timeout, range: 0x000A to 0x0C80 */
}) bt_hci_subevt_le_remote_connection_parameter_request_t;

/**
 *  @brief      LE data length change event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
    uint16_t          max_tx_octets;     /**< Maximum tx octets */
    uint16_t          max_tx_time;       /**< Maximum tx time */
    uint16_t          max_rx_octets;     /**< Maximum rx octets */
    uint16_t          max_rx_time;       /**< Maximum rx time */
}) bt_hci_subevt_le_data_length_change_t;

/**
 *  @brief      LE read local p256 public key complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                    /**< Status */
    uint8_t           local_p256_public_key[64]; /**< p256 public key in local device */
}) bt_hci_subevt_le_read_local_p256_public_key_complete_t;

/**
 *  @brief      LE generate DHKey complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;    /**< Status */
    uint8_t           dhkey[32]; /**< Generated diffie hellman key */
}) bt_hci_subevt_le_generate_dhkey_complete_t;

/**
 *  @brief      LE enhanced connection complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                /**< Status */
    bt_handle_t       connection_handle;     /**< Connection handle */
    bt_role_t         role;                  /**< Role of the local device */
    bt_addr_type_t    peer_address_type;     /**< Address type of the remote device */
    bt_bd_addr_t      peer_address;          /**< Remote device address */
    /**< Resolvable private address being used by the local device.
       It is only valid when own address type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY */
    bt_bd_addr_t      local_resolvable_private_address;
    /**< Resolvable private address being used by the remote device.
       It is only valid when #peer_address_type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY */
    bt_bd_addr_t      peer_resolvable_private_address;
    uint16_t          conn_interval;         /**< Connection interval used on this connection */
    uint16_t          conn_latency;          /**< Connection latency used on this connection */
    uint16_t          supervision_timeout;   /**< Supervision timeout used on this connection */
    uint8_t           master_clock_accuracy; /**< Clock accuracy in master */
}) bt_hci_subevt_le_enhanced_connection_complete_t;

/**
 *  @brief      LE direct advertising report event.
 */
BT_PACKED(
typedef struct {
    uint8_t           num_reports;         /**< Number of responses in event */
    /* Below is temporary code as num_reports = 1 */
    uint8_t           event_type;          /**< Event type */
    bt_addr_type_t    address_type;        /**< Address Type */
    bt_bd_addr_t      address;             /**< Address of the advertising device */
    bt_addr_type_t    direct_address_type; /**< Address Type (only random device address) */
    bt_bd_addr_t      direct_address;      /**< Random device address */
    int8_t            rssi;                /**< RSSI */
}) bt_hci_subevt_le_direct_advertising_report_t;

/**
 *  @brief      LE phy update complete event.
 */
typedef bt_hci_evt_cc_le_read_phy_t bt_hci_subevt_le_phy_update_complete_t;

/**
 *  @brief      LE extended advertising report event.
 */
BT_PACKED(
typedef struct {
    uint8_t           num_reports;         /**< Number of responses in event */
    /* Below is temporary code as num_reports = 1 */
    uint16_t          event_type;          /**< Event type */
    bt_addr_type_t    address_type;        /**< Address Type */
    bt_bd_addr_t      address;             /**< Address of the advertising device */
    uint8_t           primary_phy;
    uint8_t           secondary_phy;
    uint8_t           advertising_sid;
    int8_t            tx_power;
    int8_t            rssi;                /**< RSSI */
    uint16_t          periodic_advertising_interval;
    bt_addr_type_t    direct_address_type; /**< Address Type (only random device address) */
    bt_bd_addr_t      direct_address;      /**< Random device address */
    uint8_t           data_length;
    bt_data_t         data;
}) bt_hci_subevt_le_extended_advertising_report_t;

/**
 *  @brief      LE periodic advertising sync established event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                   /**< Status */
    uint16_t          sync_handle;              /**< Sync handle */
    uint8_t           advertising_sid;
    bt_addr_type_t    advertiser_address_type;  /**< Advertiser Address Type */
    bt_bd_addr_t      advertiser_address;       /**< Address of the advertising device */
    uint8_t           advertiser_phy;
    uint16_t          periodic_advertising_interval;
    uint8_t           advertiser_clock_accuracy;
}) bt_hci_subevt_le_periodic_advertising_sync_established_t;

/**
 *  @brief      LE periodic advertising report event.
 */
BT_PACKED(
typedef struct {
    uint16_t          sync_handle;         /**< Sync handle */
    int8_t            tx_power;
    int8_t            rssi;
    uint8_t           unused;
    uint8_t           data_status;
    uint8_t           data_length;
    bt_data_t         data;
}) bt_hci_subevt_le_periodic_advertising_report_t;

/**
 *  @brief      LE advertising set terminated event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                   /**< Status */
    uint8_t           advertising_handle;       /**< Advertising handle */
    uint16_t          connection_handle;        /**< Connection handle which is valid when advertising ends because a connection was created */
    uint8_t           num_completed_extended_advertising_events;
}) bt_hci_subevt_le_advertising_set_terminated_t;

#ifdef BT_LE_AUDIO_ENABLE
/**
 * @brief       LE CIS established event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                           /**< The status of the CIS. */
    bt_handle_t       connection_handle;                /**< Connection handle of the CIS. */
    uint32_t          cig_sync_delay : 24;              /**< The maximum time, in microseconds, for transmission of PDUs of all CISes in a CIS in an isochronous interval. */
    uint32_t          cis_sync_delay : 24;              /**< The maximum time, in microseconds, for transmission of PDUs of a CIS in an isochronous interval. */
    uint32_t          transport_latency_m_to_s : 24;    /**< The maximum time, in microseconds, for transmission of PDUs of a CIS in an isochronous interval. */
    uint32_t          transport_latency_s_to_m : 24;    /**< The maximum time, in microseconds, for transmission of SDUs of all CISes from master to slave. */
    uint8_t           phy_m_to_s;                       /**< The transmitter PHY of packets used from the slave to master. */
    uint8_t           phy_s_to_m;                       /**< The transmitter PHY of packets used from the slave to master. */
}) bt_hci_subevt_le_cis_established_t;

/**
 * @brief       LE CIS request event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       acl_connection_handle;            /**< Connection handle of the ACL. */
    bt_handle_t       cis_connection_handle;            /**< Connection handle of the CIS. */
    uint8_t           cig_id;                           /**< CIG ID. */
    uint8_t           cis_id;                           /**< CIS ID. */
}) bt_hci_subevt_le_cis_request_t;
#endif

/**
 *  @brief      Encryption change event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;             /**< Status */
    bt_handle_t       connection_handle;  /**< Connection handle */
    bt_hci_enable_t   encryption_enabled; /**< Enable or disable */
}) bt_hci_evt_encryption_change_t;

#ifdef BT_LE_AUDIO_ENABLE
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;             /**< Status */
    bt_handle_t       connection_handle;  /**< Connection handle */
    uint16_t          packet_sequence_number; /**< Enable or disable */
    uint32_t          time_stamp;
    uint8_t           time_offset[3];
}) bt_hci_evt_cc_le_read_iso_tx_sync_t;
#endif

/**
 *  @brief      Command complete event.
 */
BT_PACKED(
typedef struct {
    uint8_t           num_hci_command_packets; /**< Number of HCI command packets which are allowed to be sent to the controller */
    bt_hci_cmd_op_t   cmd_code;                /**< Command code */
    void              *data;                   /**< Return parameters for the command specified in #cmd_code */
}) bt_hci_evt_command_complete_t;

/**
 *  @brief      Flush occurred event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle */
}) bt_hci_evt_flush_occurred_t;

/**
 *  @brief      Command status event.
 *  @note       Command status event is used to indicate that the command described by
 *              the #cmd_code has been received, and that the controller is currently
 *              performing the task for this command.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;                  /**< Status */
    uint8_t           num_hci_command_packets; /**< Number of HCI command packets which are allowed to be sent to the controller */
    bt_hci_cmd_op_t   cmd_code;                /**< Command code */
}) bt_hci_evt_command_status_t;

/**
 *  @brief      Hardware error event.
 */
BT_PACKED(
typedef struct {
    uint8_t           hardware_code; /**< Hardware code to indicate hardware problem */
}) bt_hci_evt_hardware_error_t;

/**
 *  @brief      Number of completed packets event.
 */
BT_PACKED(
typedef struct {
    uint8_t           number_of_handles;           /**< The number of connection handles */
    /* Below is temporary code as number_of_handles = 1 */
    bt_handle_t       connection_handle;           /**< Connection handle */
    uint16_t          hc_num_of_completed_packets; /**< Number of HCI Data Packets that have been completed for the associated #connection_handle */
}) bt_hci_evt_number_of_completed_packets_t;

/**
 *  @brief      Number of completed packets event extension, when connection handle is not only one.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;           /**< Connection handle */
    uint16_t          hc_num_of_completed_packets; /**< Number of HCI Data Packets that have been completed for the associated #connection_handle */
}) bt_hci_evt_number_of_completed_packets_ext_t;

/**
 *  @brief      Encryption key refresh complete event.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;            /**< Status */
    bt_handle_t       connection_handle; /**< Connection handle */
}) bt_hci_evt_encryption_key_refresh_complete_t;

/* HCI Packet Structure. */
/**
 *  @brief      LE meta event format.
 */
BT_PACKED(
typedef struct {
    bt_hci_subevt_op_t  subevt_code; /**< Subevent code */
    bt_data_t           param;       /**< Parameters */
}) bt_hci_evt_le_meta_t;

/* HCI Packet Structure. */
/**
 *  @brief      MTK controller assert event format.
 */
BT_PACKED(
typedef struct {
    uint8_t             param[3];    /**< param */
    uint16_t            line;        /**< file line */
    bt_data_t           path;        /**< file path */
}) bt_hci_mtk_assert_t;

/**
 *  @brief      HCI command packet. Please refer to core spec 5.4.1 HCI Command Packet.
 */
BT_PACKED(
typedef struct {
    bt_hci_cmd_op_t     cmd_code; /**< Command code */
    uint8_t             length;   /**< Length of all of the parameters contained in this packet */
    bt_data_t           param;     /**< Parameters */
}) bt_hci_packet_cmd_t;

/**
 *  @brief      HCI event packet. Please refer to core spec 5.4.4 HCI Event Packet.
 */
BT_PACKED(
typedef struct {
    bt_hci_evt_op_t     evt_code; /**< Event code */
    uint8_t             length;   /**< Length of all of the parameters contained in this packet */
    bt_data_t           param;    /**< Parameters */
}) bt_hci_packet_evt_t;

/**
 *  @brief      HCI ACL packet. Please refer to core spec, Please refer to core spec 5.4.2 HCI ACL Data Packets.
 */
BT_PACKED(
typedef struct {
    uint16_t    handle: 12; /**< Handle */
    uint16_t    pb_flag: 2; /**< Packet boundary flag */
    uint16_t    bc_flag: 2; /**< Broadcast flag */
    uint16_t    length;     /**< Data total length */
    bt_data_t   data;       /**< Data */
}) bt_hci_packet_acl_t;

#ifdef BT_LE_AUDIO_ENABLE
/**
 *  @brief      HCI iso data packet. Please refer to core spec.
 */
BT_PACKED(
typedef struct {
    uint16_t handle: 12;    /**< Handle */
    uint16_t pb_flag: 2;    /**< Packet boundary flag */
    uint16_t ts_flag: 1;    /**< time stamp flag */
    uint16_t reserve: 1;    /**< reserve for feture */
    uint16_t length:  14;   /**< Data total length */
    uint16_t len_reserve: 2;/**< reserve for feture */
    bt_data_t data;         /**< Data */
}) bt_hci_packet_iso_t;
#endif

/**
 *  @brief      LE HCI packet.
 */
BT_PACKED(
typedef struct {
    bt_uart_t   indicator;                  /**< Indicate packet is CMD, ACL or EVT */
    BT_PACKED(
    union {
        bt_hci_packet_acl_t acl; /**< ACL packet */
        bt_hci_packet_cmd_t cmd; /**< Command packet */
        bt_hci_packet_evt_t evt; /**< Event packet */
#ifdef BT_LE_AUDIO_ENABLE
        bt_hci_packet_iso_t iso; /**< ISO packet */
#endif
    });
}) bt_hci_le_packet_t;

#ifdef __BT_HB_ENABLE_SLEEP_MODE__
/**
 *  @brief      Set sleep mode param
 */
BT_PACKED(
typedef struct {
    uint8_t mode;               /**< Set current project sleep mode */
    uint16_t duration;          /**< Controller can enter sleep when Controller send the last hci packet after the duration */
    uint16_t host_duration;     /**< Host can enter sleep when host received the last hci packet after the duration */
    uint8_t wakeup_pin;         /**< Set current project wakeup host pin.(Combo chip not support) */
    uint8_t time_comp;          /**< Time compensation for sleep */
}) bt_hci_cmd_set_sleep_param_t;
#endif //__BT_HB_ENABLE_SLEEP_MODE__

#ifdef __BT_HB_DUO__
/**
 *  @brief      BT HCI packet.
 */
#define BT_HCI_PACKET_HEADER \
    bt_linknode_t node;\
    uint16_t    packet_length;\
    uint16_t    offset;\
    void*       callback;                   /*tx done callback*/ \
    uint32_t    time_stamp;                 /*reserve for time stamp*/ \
    uint8_t     ref_count;\
    uint8_t     is_rx;                        /*is rx packet*/ \
    uint8_t     reserve

BT_PACKED(
typedef struct {
    BT_HCI_PACKET_HEADER;
    bt_uart_t   indicator;                  /**< Indicate packet is CMD, ACL or EVT */
    BT_PACKED(
    union {
        bt_hci_packet_acl_t acl;            /**< ACL packet */
        bt_hci_packet_cmd_t cmd;            /**< Command packet */
        bt_hci_packet_evt_t evt;            /**< Event packet */
#ifdef BT_LE_AUDIO_ENABLE
        bt_hci_packet_iso_t iso;            /**< ISO packet */
#endif
    });
}) bt_hci_packet_t;

BT_PACKED(
typedef struct {
    BT_HCI_PACKET_HEADER;
}) bt_hci_packet_linknode_t;

typedef void (*bt_hci_tx_done_callback)(bt_hci_packet_t *packet, bt_status_t status);

#else
typedef bt_linknode_t bt_hci_packet_linknode_t;
#endif

/**
 * @}
 */

/**
 * @}
 * @}
 */

#endif /* __BT_HCI_PACKET_SPEC_H__ */
