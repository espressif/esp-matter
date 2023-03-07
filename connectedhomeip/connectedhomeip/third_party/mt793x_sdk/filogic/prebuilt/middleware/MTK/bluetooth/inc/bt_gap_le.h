/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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

#ifndef __BT_GAP_LE_H__
#define __BT_GAP_LE_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGAP GAP
 * @{
 * This section introduces the Generic Access Profile (GAP) APIs including terms and acronyms, supported features, details on how to use this driver, GAP function groups, enums, structures and functions.
 * For more information about GAP, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 3, part C]</a>.
 * This section includes details on both Classic Bluetooth and Bluetooth Low Energy modules.
 * @addtogroup GAP_LE Bluetooth Low Energy
 * @{
 * This section defines the Low Energy (LE) GAP confirmation and indication macros, structures and API prototypes.
 * It defines the generic LE procedures related to device discovery and LE link connectivity. Applications can be developed to configure the controller and control the Bluetooth devices operating in idle, advertising,
 * scanning, initiating and connected modes.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GAP                        | Generic Access Profile. This profile defines the generic procedures related to discovery of Bluetooth enabled devices and link management aspects of connecting to the Bluetooth enabled devices. It also defines procedures related to the use of different security levels. |
 * |\b HCI                        | Host Controller Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#HCI">Wikipedia</a>. |
 * |\b LE                         | Low Energy. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth_low_energy">Wikipedia</a>. |
 * |\b Out-of-Band                | An association mode, primarily designed for scenarios where an Out-of-Band mechanism is used to discover devices and to exchange or transfer cryptographic numbers used in the pairing process. |
 * |\b RSSI                       | Received Signal Strength Indication. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">Wikipedia</a>. |
 * |\b ADV                        | Advertising. A device sends data in either non-connectable undirected or scannable undirected advertising events. |
 * |\b CSRK                       | Connection Signature Resolving Key. A 128-bit key used to sign data and verify signatures on the receiving device. |
 * |\b EDIV                       | Encrypted Diversifier. A 16-bit stored value used to identify the LTK distributed during LE legacy pairing. |
 * |\b GATT                       | Generic Attribute Profile. A service framework using the Attribute Protocol for discovering services, and for reading and writing characteristic values on a peer device. |
 * |\b IRK                        | Identity Resolving Key. A 128-bit key used to generate and resolve random addresses. |
 * |\b LTK                        | Long Term Key. A 128-bit key used to generate the contributory session key for an encrypted connection. |
 * |\b TK                         | Temporary Key. A 128-bit temporary key used in the pairing process to generate short term key. |
 * |\b RAND                       | Random Number. A 64-bit stored valued used to identify the LTK distributed during LE legacy pairing. |
 * |\b SMP                        | Security Manager (SM) Protocol defines the protocol and behavior to manage pairing, authentication and encryption between low energy devices. |
 * |\b I/O \b Capability          | Input/Output Capability. It is used in pairing feature exchange process to determine which pairing method shall be used. |
 * |\b MITM                       | Man-in-the-middle Protection. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Man-in-the-middle_attack">Wikipedia</a>. |
 * |\b LESC                       | Bluetooth LE Secure Connection. It is a new pairing procedures and algorithms defined in <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2</a> |
 * |\b RPA                       | Resolvable Private Address. A resolvable private address may be resolved by the corresponding device's IRK. If a resolvable private address is resolved, the device can associate this address with the peer device. |
 *
 * @section bt_gap_le_api_usage How to use this module
 *
 * - The application calls functions #bt_gap_le_set_advertising(), #bt_gap_le_set_scan() and #bt_gap_le_connect().
 * - Then it receives a GAP confirmation or indication. The GAP confirmation or indication is used to notify the application that the process is complete or the indication is received.
 *   BT_GAP_LE_XXX_CNF confirms that the process completed successfully (the HCI received the related HCI command).
 *   BT_GAP_LE_XXX_IND confirms that a corresponding indicator is received.
 * - The GAP confirmation and indication structures provide related information to the application.
 *   The application receives related data through the event parameters.
 *   In addition, a user-defined event callback is implemented to apply actions after receiving the confirmations or indications.
 *  - Sample code:
 *      @code
 *          // Start advertising from the application, so other devices can scan for the advertisement.
 *          void user_application_start_advertising()
 *          {
 *              bt_app_advertising = true;
 *              bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
 *                  .advertising_interval_min = 0x0800,
 *                  .advertising_interval_max = 0x0800,
 *                  .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
 *                  .advertising_channel_map = 7,
 *                  .advertising_filter_policy = 0
 *              };
 *              adv_enable.advertising_enable = BT_HCI_ENABLE;
 *              bt_gap_le_set_advertising(&adv_enable, &adv_para, NULL, NULL);
 *          }
 *
 *          // Connect to a device from the application.
 *          void user_application_connect(bt_addr_type_t type, bt_bd_addr_ptr_t address)
 *          {
 *              bt_hci_cmd_le_create_connection_t connect_para = {
 *                      .le_scan_interval = 0x0010,
 *                      .le_scan_window = 0x0010,
 *                      .initiator_filter_policy = BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS,
 *                      .peer_address = {
 *                      .type = BT_ADDR_PUBLIC,
 *                  },
 *                  .own_address_type = BT_ADDR_PUBLIC,
 *                  .conn_interval_min = 0x0006,
 *                  .conn_interval_max = 0x0320,
 *                  .conn_latency = 0x0000,
 *                  .supervision_timeout = 0x07d0,
 *                  .minimum_ce_length = 0x0000,
 *                  .maximum_ce_length = 0x0190,
 *              };
 *              connect_para.peer_address.type = lt_addr_type;
 *              memcpy(connect_para.peer_address.addr, lt_addr, sizeof(lt_addr));
 *              bt_gap_le_connect(&connect_para);
 *          }
 *
 *          // A user-defined static callback for the application to listen to the event.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_LE_ADVERTISING_REPORT_IND:
 *                      const bt_gap_le_advertising_report_ind_t *report = (bt_gap_le_advertising_report_ind_t *)p;
 *                      if (status == BT_STATUS_SUCCESS) {
 *                          BT_LOGI("APP", "BT_GAP_LE_ADVERTISING_REPORT_IND Successful");
 *                      } else {
 *                          BT_LOGI("APP", "BT_GAP_LE_ADVERTISING_REPORT_IND Failed");
 *                      }
 *                      BT_LOGI("APP", "========================================");
 *                      BT_LOGI("APP", "Address:\t%s", bt_debug_addr2str(&report->address));
 *                      BT_LOGI("APP", "Event Type:\t%s", get_event_type(report->event_type));
 *                      uint8_t count, ad_data_len, ad_data_type, ad_data_idx;
 *                      count = 0;
 *                      uint8_t buff[100] = {0};
 *                      while (count < report->length_data) {
 *                          ad_data_len = report->data[count];
 *                          // Handle error for data length over 30 bytes.
 *                          if (ad_data_len >= 0x1F) {
 *                              BT_LOGI("APP", "Advertising Data Length Error");
 *                              break;
 *                          }
 *                          ad_data_type = report->data[count+1];
 *                          count+=2;
 *
 *                          if (ad_data_type == BT_GAP_LE_AD_TYPE_FLAG) {
 *                              if (report->data[count] & BT_GAP_LE_AD_FLAG_LIMITED_DISCOVERABLE) {
 *                                  BT_LOGI("APP", "Advertising Flags:\tLE Limited Discoverable Mode");
 *                              } else if (report->data[count] & BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE) {
 *                                  BT_LOGI("APP", "Advertising Flags:\tLE General Discoverable Mode");
 *                              } else {
 *                                  BT_LOGI("APP", "Advertising Flags:\tUnknown: 0x%02x", report->data[count]);
 *                              }
 *                              count+=(ad_data_len-1);
 *                          } else if (ad_data_type == BT_GAP_LE_AD_TYPE_NAME_COMPLETE) {
 *                              for (ad_data_idx=0; ad_data_idx<(ad_data_len-1); ad_data_idx++, count++) {
 *                                  buff[ad_data_idx] = report->data[count];
 *                              }
 *                              BT_LOGI("APP", "Complete Name:\t%s", buff);
 *                          } else {
 *                              count+=(ad_data_len-1);
 *                          }
 *                      }
 *                      printf("[I][APP] RAW DATA=0x");
 *                      for (count = 0; count<report->length_data; count++) {
 *                          printf("%02x", report->data[count]);
 *                      }
 *                      printf("\n");
 *                      BT_LOGI("APP", "========================================");
 *                      break;
 *                  case BT_GAP_LE_CONNECT_IND:
 *                      const bt_hci_subevt_le_connection_complete_t *connect_complete = ((bt_gap_le_connection_ind_t *)buff)->connection_complete;
 *                      BT_LOGI("APP", "connection handle=0x%04x", connect_complete->connection_handle);
 *                      if (connect_complete->role == BT_ROLE_MASTER) {
 *                          BT_LOGI("APP", "role=Master");
 *                      } else {
 *                          BT_LOGI("APP", "role=Slave");
 *                      }
 *                      if (connect_complete->peer_address_type) {
 *                          BT_LOGI("APP", "peer address:%02X:%02X:%02X:%02X:%02X:%02X(Random Device Address)\n", connect_complete->peer_address[5],
 *                              connect_complete->peer_address[4], connect_complete->peer_address[3], connect_complete->peer_address[2], connect_complete->peer_address[1], connect_complete->peer_address[0]);
 *                      } else {
 *                          BT_LOGI("APP", "peer address:%02X:%02X:%02X:%02X:%02X:%02X(Public Device Address)\n", connect_complete->peer_address[5],
 *                              connect_complete->peer_address[4], connect_complete->peer_address[3], connect_complete->peer_address[2], connect_complete->peer_address[1], connect_complete->peer_address[0]);
 *                      }
 *                      // The connection is established, you can now send data.
 *                      break;
 *                  case BT_GAP_LE_DISCONNECT_CNF:
 *                      // Indicate the status of executing the bt_gap_le_disconnect() API.
 *                      if (status == BT_STATUS_SUCCESS) {
 *                          BT_LOGI("APP", "BT_GAP_LE_DISCONNECT_CNF Successful");
 *                      } else {
 *                          BT_LOGI("APP", "BT_GAP_LE_DISCONNECT_CNF Failed");
 *                      }
 *                      break;
 *                  case BT_GAP_LE_DISCONNECT_IND:
 *                      bt_hci_evt_disconnect_complete_t *disconnect_complete;
 *                      disconnect_complete = (bt_hci_evt_disconnect_complete_t*) buff;
 *                      // Handle the disconnection event and release the disconnected link record.
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

#include "bt_type.h"
#include "bt_system.h"
#include "bt_hci.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup Bluetoothhbif_gap_define Define
 * @{
 */

#define BT_GAP_LE_SMP_DISPLAY_ONLY                     0x00       /**< The device has the ability to display or communicate a 6 digit decimal number,
                                                                       but does not have the ability to indicate 'yes' or 'no'. */
#define BT_GAP_LE_SMP_DISPLAY_YES_NO                   0x01       /**< The device has the ability to display or communicate a 6 digit decimal number.
                                                                       Also, the device has at least two buttons, one of which maps to 'yes' and the other to 'no';
                                                                       or the device has a mechanism whereby the user can indicate either 'yes' or 'no'. */
#define BT_GAP_LE_SMP_KEYBOARD_ONLY                    0x02       /**< The device has a numeric keyboard that can input the numbers '0' through '9' and a confirmation.
                                                                       But, the device does not have the ability to display or communicate a 6 digit decimal number. */
#define BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT               0x03       /**< The device does not have the ability to indicate 'yes' or 'no', and does not have the ability to
                                                                       display or communicate a 6 digit decimal number. */
#define BT_GAP_LE_SMP_KEYBOARD_DISPLAY                 0x04       /**< The device has a numeric keyboard that can input the numbers '0' to '9' and a confirmation.
                                                                       And the device has the ability to display or communicate a 6 digit decimal number. */
#define BT_GAP_LE_SMP_IO_CAPABILITY_NUM                0x05       /**< Total number of I/O capability types. */
typedef uint8_t bt_gap_le_smp_io_capability_t;                    /**< Device I/O capability. */

#define BT_GAP_LE_SMP_OOB_DATA_NOT_PRESENTED           0x00       /**< Did not receive Out-of-Band data. */
#define BT_GAP_LE_SMP_OOB_DATA_PRESENTED               0x01       /**< Received Out-of-Band data. */
typedef uint8_t bt_gap_le_smp_oob_data_flag_t;                    /**< Indicates if the Out-of-Band data is received or not. */

#define BT_GAP_LE_SMP_AUTH_REQ_NO_BONDING              0x00        /**< Does not save the IRK, LTK and CSRK. */
#define BT_GAP_LE_SMP_AUTH_REQ_BONDING                 (0x01 << 0) /**< Saves the IRK, LTK and CSRK. */
#define BT_GAP_LE_SMP_AUTH_REQ_MITM                    (0x01 << 2) /**< Requires the MITM protection. */
#define BT_GAP_LE_SMP_AUTH_REQ_SECURE_CONNECTION       (0x01 << 3) /**< Requires LE secure connection. */
#define BT_GAP_LE_SMP_AUTH_REQ_SUPPORT_H7              (0x01 << 5) /**< Local support H7. */
typedef uint8_t bt_gap_le_smp_auth_req_t;                          /**< Authentication requirement. */

#define BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY            (0x01 << 0) /**< Distribute LTK. */
#define BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY             (0x01 << 1) /**< Distribute IRK. */
#define BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN              (0x01 << 2) /**< Distribute CSRK. */
#define BT_GAP_LE_SMP_KEY_DISTRIBUTE_LINK              (0x01 << 3) /**< Distribute CTKD. */
typedef uint8_t bt_gap_le_smp_key_distribute_t;                    /**< The key to distribute. */

#define BT_GAP_LE_SMP_JUST_WORKS_MASK          (0x1 << 0)  /**< Using Just Works method. */
#define BT_GAP_LE_SMP_AUTHENTICATION_MASK      BT_GAP_LE_SECURITY_AUTHENTICATION_MASK /**< Using Authentication (MITM protection). */
#define BT_GAP_LE_SMP_PASSKEY_INPUT_MASK       (0x1 << 2)  /**< Using Passkey Input method. */
#define BT_GAP_LE_SMP_PASSKEY_DISPLAY_MASK     (0x1 << 3)  /**< Using Passkey Display method. */
#define BT_GAP_LE_SMP_LESC_MASK                BT_GAP_LE_SECURITY_LESC_MASK           /**< Using LE Secure Connection. */
#define BT_GAP_LE_SMP_PASSKEY_MASK             (0x1 << 5)  /**< Using Passkey method. */
#define BT_GAP_LE_SMP_OOB_MASK                 (0x1 << 6)  /**< Using Out-of-Band method. */
#define BT_GAP_LE_SMP_NUMERIC_COMPARISON_MASK  (0x1 << 7)  /**< Using Numeric Comparison method. */

#define BT_GAP_LE_SMP_JUST_WORKS       BT_GAP_LE_SMP_JUST_WORKS_MASK  /**< Using Legacy Just Works method. */
#define BT_GAP_LE_SMP_PASSKEY_INPUT    (BT_GAP_LE_SMP_PASSKEY_MASK | BT_GAP_LE_SMP_PASSKEY_INPUT_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK)    /**< Using Legacy Passkey Input method. */
#define BT_GAP_LE_SMP_PASSKEY_DISPLAY  (BT_GAP_LE_SMP_PASSKEY_MASK | BT_GAP_LE_SMP_PASSKEY_DISPLAY_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK)  /**< Using Legacy Passkey Display method. */
#define BT_GAP_LE_SMP_OOB              (BT_GAP_LE_SMP_OOB_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK)  /**< Using Legacy Out-of-Band method. */

#define BT_GAP_LE_SMP_LESC_JUST_WORKS      (BT_GAP_LE_SMP_JUST_WORKS_MASK | BT_GAP_LE_SMP_LESC_MASK) /**< Using LESC Just Works method. */
#define BT_GAP_LE_SMP_LESC_PASSKEY_INPUT   (BT_GAP_LE_SMP_PASSKEY_MASK | BT_GAP_LE_SMP_PASSKEY_INPUT_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK | BT_GAP_LE_SMP_LESC_MASK)    /**< Using LESC Passkey Input method. */
#define BT_GAP_LE_SMP_LESC_PASSKEY_DISPLAY (BT_GAP_LE_SMP_PASSKEY_MASK | BT_GAP_LE_SMP_PASSKEY_DISPLAY_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK | BT_GAP_LE_SMP_LESC_MASK)  /**< Using LESC Passkey Display method. */
#define BT_GAP_LE_SMP_LESC_OOB             (BT_GAP_LE_SMP_OOB_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK | BT_GAP_LE_SMP_LESC_MASK)   /**< Using LESC Out-of-Band method. */
#define BT_GAP_LE_SMP_LESC_NUMERIC_COMPARISON (BT_GAP_LE_SMP_NUMERIC_COMPARISON_MASK | BT_GAP_LE_SMP_AUTHENTICATION_MASK | BT_GAP_LE_SMP_LESC_MASK) /**< Using LESC Numeric Comparison method. */
typedef uint8_t bt_gap_le_smp_pairing_method_t;    /**< The final pairing method. */


#define BT_GAP_LE_SET_RANDOM_ADDRESS_CNF           (BT_MODULE_GAP | 0x0003)    /**< Set random address confirmation with NULL payload. */
#define BT_GAP_LE_SET_WHITE_LIST_CNF               (BT_MODULE_GAP | 0x0004)    /**< Set white list confirmation with NULL payload. */
#define BT_GAP_LE_SET_ADVERTISING_IND              (BT_MODULE_GAP | 0x0005)    /**< Set advertising indication for #BT_HCI_STATUS_DIRECTED_ADVERTISING_TIMEOUT with NULL payload. */
#define BT_GAP_LE_SET_ADVERTISING_CNF              (BT_MODULE_GAP | 0x0006)    /**< Set advertising confirmation with NULL payload. */
#define BT_GAP_LE_SET_SCAN_CNF                     (BT_MODULE_GAP | 0x0007)    /**< Set scan confirmation with NULL payload. */
#define BT_GAP_LE_ADVERTISING_REPORT_IND           (BT_MODULE_GAP | 0x0008)    /**< Advertising report indication with #bt_gap_le_advertising_report_ind_t. */
#define BT_GAP_LE_CONNECT_CNF                      (BT_MODULE_GAP | 0x0009)    /**< Connect confirmation with NULL payload. */
#define BT_GAP_LE_CONNECT_IND                      (BT_MODULE_GAP | 0x000a)    /**< Connect indication with #bt_gap_le_connection_ind_t, indicates a connection is created. */
#define BT_GAP_LE_CONNECT_CANCEL_CNF               (BT_MODULE_GAP | 0x000b)    /**< Connect cancel confirmation with NULL payload. */
#define BT_GAP_LE_DISCONNECT_CNF                   (BT_MODULE_GAP | 0x000c)    /**< Disconnect confirmation with NULL payload. */
#define BT_GAP_LE_DISCONNECT_IND                   (BT_MODULE_GAP | 0x000d)    /**< Disconnect indication with #bt_gap_le_disconnect_ind_t as the payload, indicates a connection is disconnected. */
#define BT_GAP_LE_CONNECTION_UPDATE_CNF            (BT_MODULE_GAP | 0x000e)    /**< Connection update confirmation with NULL payload. */
#define BT_GAP_LE_CONNECTION_UPDATE_IND            (BT_MODULE_GAP | 0x000f)    /**< Connection update indication with #bt_gap_le_connection_update_ind_t. */
#define BT_GAP_LE_READ_RSSI_CNF                    (BT_MODULE_GAP | 0x0011)    /**< Read RSSI confirmation with #bt_hci_evt_cc_read_rssi_t. */
#define BT_GAP_LE_UPDATE_DATA_LENGTH_CNF           (BT_MODULE_GAP | 0x0012)    /**< Update data length confirmation with #bt_hci_evt_cc_le_set_data_length_t. */
#define BT_GAP_LE_BONDING_REPLY_REQ_IND            (BT_MODULE_GAP | 0x0015)    /**< Bonding reply request indication with #bt_gap_le_bonding_reply_req_ind_t, indicates that the application should reply the passkey or Out-of-Band data by calling #bt_gap_le_bonding_reply(). The passkey or Out-of-Band data should not be NULL. */
#define BT_GAP_LE_BONDING_COMPLETE_IND             (BT_MODULE_GAP | 0x0016)    /**< Bonding complete indication with #bt_gap_le_bonding_complete_ind_t, indicates the pairing procedure is complete. */
#define BT_GAP_LE_SET_RESOLVING_LIST_CNF           (BT_MODULE_GAP | 0x0017)    /**< Set (Add one/ Delete one/ Clear all) resolving list confirmation with NULL payload. */
#define BT_GAP_LE_SET_ADDRESS_RESOLUTION_ENABLE_CNF           (BT_MODULE_GAP | 0x0018) /**< Set (Enable/Disable) address resolution confirmation with NULL payload. */
#define BT_GAP_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_CNF  (BT_MODULE_GAP | 0x0019) /**< Set resolvable private address timeout confirmation with NULL payload. */
#define BT_GAP_LE_SET_TX_POWER_CNF                 (BT_MODULE_GAP | 0x0020)    /**< Set TX power confirmation with NULL payload. */
#ifdef __BT_MULTI_ADV__
#define BT_GAP_LE_START_MULTIPLE_ADVERTISING_CNF   (BT_MODULE_GAP | 0x0021)    /**< The confirmation event of starting a multi-advertising instance, with #bt_gap_le_start_multiple_advertising_cnf_t as the payload. */
#define BT_GAP_LE_STOP_MULTIPLE_ADVERTISING_CNF    (BT_MODULE_GAP | 0x0022)    /**< The confirmation event of stopping a multi-advertising instance, with #bt_gap_le_stop_multiple_advertising_cnf_t as the payload. */
#endif
#define BT_GAP_LE_READ_PHY_CNF                     (BT_MODULE_GAP | 0x0023)    /**< Current phy indication with #bt_hci_evt_cc_le_read_phy_t as the payload, indicates read phy complete. */
#define BT_GAP_LE_SET_PHY_CNF                      (BT_MODULE_GAP | 0x0024)    /**< Current phy indication with NULL payload. */
#define BT_GAP_LE_PHY_UPDATE_IND                   (BT_MODULE_GAP | 0x0025)    /**< Current phy indication with #bt_gap_le_phy_update_ind_t as the payload, indicates phy update complete. */
#define BT_GAP_LE_EXT_ADVERTISING_REPORT_IND       (BT_MODULE_GAP | 0x0026)    /**< Advertising report indication with #bt_gap_le_ext_advertising_report_ind_t. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_IND (BT_MODULE_GAP | 0x0027)    /**< Periodic Advertising sync established indication with #bt_gap_le_periodic_advertising_sync_established_ind_t. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_REPORT_IND  (BT_MODULE_GAP | 0x0028)    /**< Periodic Advertising report indication with #bt_gap_le_periodic_advertising_report_ind_t. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_LOST_IND        (BT_MODULE_GAP | 0x0029)    /**< Periodic Advertising sync lost indication with #bt_gap_le_periodic_advertising_sync_lost_ind_t. */
#define BT_GAP_LE_SCAN_TIMEOUT_IND                 (BT_MODULE_GAP | 0x002A)    /**< The event indicates that scanning has ended because the duration has expired, Please refer to the #bt_gap_le_set_extended_scan(). */
#define BT_GAP_LE_ADVERTISING_SET_TERMINATED_IND   (BT_MODULE_GAP | 0x002B)    /**< Advertising set terminated indication with #bt_gap_le_advertising_set_terminated_ind_t. */
#define BT_GAP_LE_SCAN_REQUEST_RECEIVED_IND        (BT_MODULE_GAP | 0x002C)    /**< Scan request received indication with #bt_gap_le_scan_request_received_ind_t. */
#define BT_GAP_LE_CHANNEL_SELECTION_ALGORITHM_IND  (BT_MODULE_GAP | 0x002D)    /**< Channel selection algorithm indication with #bt_gap_le_channel_selection_algorithm_ind_t. */
#define BT_GAP_LE_SET_PERIODIC_ADVERTISER_LIST_CNF (BT_MODULE_GAP | 0x002E)    /**< Set (Add one/ Delete one/ Clear all) periodic advertiser list confirmation with NULL payload. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CNF          (BT_MODULE_GAP | 0x002F)    /**< Periodic advertising create sync confirmation with NULL payload. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_CNF   (BT_MODULE_GAP | 0x0030)    /**< Periodic advertising create sync cancel confirmation with NULL payload. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_CNF       (BT_MODULE_GAP | 0x0031)    /**< Terminate eriodic advertising confirmation with NULL payload. */
#define BT_GAP_LE_SET_EXTENDED_SCAN_CNF            (BT_MODULE_GAP | 0x0032)    /**< Set extended scan confirmation with NULL payload. */
#define BT_GAP_LE_REMOVE_ADVERTISING_CNF           (BT_MODULE_GAP | 0x0033)    /**< Remove advertising confirmation with #bt_gap_le_remove_advertising_cnf_t. */
#define BT_GAP_LE_ENABLE_EXTENDED_ADVERTISING_CNF  (BT_MODULE_GAP | 0x0034)    /**< Enable extended advertising confirmation with #bt_gap_le_enable_extended_advertising_cnf_t. */
#define BT_GAP_LE_CONFIG_EXTENDED_ADVERTISING_CNF  (BT_MODULE_GAP | 0x0035)    /**< Config extended advertising confirmation with #bt_gap_le_config_extended_advertising_cnf_t. */
#define BT_GAP_LE_ENABLE_PERIODIC_ADVERTISING_CNF  (BT_MODULE_GAP | 0x0036)    /**< Enable periodic advertising confirmation with #bt_gap_le_enable_periodic_advertising_cnf_t. */
#define BT_GAP_LE_CONFIG_PERIODIC_ADVERTISING_CNF  (BT_MODULE_GAP | 0x0037)    /**< Config periodic advertising confirmation with #bt_gap_le_config_periodic_advertising_cnf_t. */
#define BT_GAP_LE_RPA_ROTAION_IND                  (BT_MODULE_GAP | 0x0038)    /**< RPA rotaion with #bt_gap_le_rpa_rotation_ind_t. */
#define BT_GAP_LE_HANDLE_UPDATE                    (BT_MODULE_GAP | 0x0039)    /**< After RHO done the hci handle will be updated with #bt_gap_le_handle_update_t */

#define BT_GAP_LE_SET_PRIVACY_MODE_CNF             (BT_MODULE_GAP | 0x0060)    /**< Set privacy mode confirmation with NULL payload. */

#ifdef __MTK_MULTI_ADV__
#define BT_GAP_LE_SETPARA_MULTIPLE_ADVERTISING_CNF (BT_MODULE_GAP | 0x0080)    /**< The confirmation event of setting parameter for a multi-advertising instance. */
#define BT_GAP_LE_SETDATA_MULTIPLE_ADVERTISING_CNF (BT_MODULE_GAP | 0x0081)    /**< The confirmation event of setting data for a multi-advertising instance. */
#define BT_GAP_LE_SCANDATA_MULTIPLE_ADVERTISING_CNF (BT_MODULE_GAP | 0x0082)    /**< The confirmation event of setting scan response data for a multi-advertising instance. */
#define BT_GAP_LE_READ_VENDOR_CAPABILITIES_CNF     (BT_MODULE_GAP | 0x0083)    /**< This event indicates the vendor capabilities defined by Google */
#endif

#ifdef BT_BQB
#define BT_GAP_LE_BQB_DISCONNECT_REQ_IND           (BT_MODULE_GAP | 0xFFFF)    /**< Disconnect request indication. Only used on the Bluetooth Qualification Body (BQB) test to request the application to disconnect. */
#endif

/**
 * @brief Advertising report event type.
 */
#define BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_IND          0x00 /**< Connectable undirected advertising indication. */
#define BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_DIRECT_IND   0x01 /**< Connectable directed advertising indication. */
#define BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_SCAN_IND     0x02 /**< Scannable undirected advertising indication. */
#define BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_NONCONN_IND  0x03 /**< Non connectable undirected advertising indication. */
#define BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_SCAN_RSP     0x04 /**< Scan response. */
typedef uint8_t bt_gap_le_adv_report_evt_type_t;            /**< The type of advertising report event. */

/**
 * @brief Extended advertising report data status type.
 */
#define BT_GAP_LE_EXT_ADV_REPORT_DATA_STATUS_COMPLETE       0x00    /**< Indicate the data complete. */
#define BT_GAP_LE_EXT_ADV_REPORT_DATA_STATUS_INCOMPLETE     0x01    /**< Indicate the data incomplete, more data to come. */
#define BT_GAP_LE_EXT_ADV_REPORT_DATA_STATUS_TRUNCATED      0x02    /**< Indicate the data incomplete, data truncated, no more data to come. */
typedef uint8_t bt_gap_le_ext_adv_report_data_status_t;   /**< The type of extended advertising report data status. */

/**
 * @brief Extended advertising report event type.
 *  Bit 0 used to indicate whether the advertising is connectable.
 *  Bit 1 used to indicate whether the advertising is scannable.
 *  Bit 2 used to indicate whether the advertising is directed.
 *  Bit 3 used to indicate whether the advertising report is a scan response information.
 *  Bit 4 used to indicate whether the advertising report is a legacy advertising PDU.
 *  Bit 5-6 used to indicate the data status from the advertising report, please refer #bt_gap_le_ext_adv_report_data_status_t .
 */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_CONNECTABLE           0x0001  /**< Connectable advertising indication. */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_SCANNABLE             0x0002  /**< Scannable advertising indication. */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED              0x0004  /**< Directed advertising indication. */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_SCAN_RSP              0x0008  /**< Scan response indication. */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_LEGACY_PDU            0x0010  /**< Use legacy advertising pdu indication. */
#define BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DATA_STATUS           0x0060  /**< The data status value mask, use the #BT_GAP_LE_EXT_ADV_REPORT_DATA_STATUS_MASK to get the data status from report event type. */
typedef uint16_t bt_gap_le_ext_adv_report_evt_mask_t;   /**< The type of extended advertising report event. */

#define BT_GAP_LE_EXT_ADV_REPORT_DATA_STATUS_MASK(evt_type)    ((evt_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DATA_STATUS) >> 5)   /**< The macro function to get the value of #bt_gap_le_ext_adv_report_data_status_t from #bt_gap_le_ext_adv_report_evt_mask_t. */

/**
 *  @brief Data types used for the advertising data, please refer to <a href="https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile">Assigned number for the Generic Access Profile</a>.
 */
#define BT_GAP_LE_AD_TYPE_FLAG                     0x01 /**< Flags */
#define BT_GAP_LE_AD_TYPE_16_BIT_UUID_PART         0x02 /**< Incomplete list of the 16-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE     0x03 /**< Complete list of the 16-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_32_BIT_UUID_PART         0x04 /**< Incomplete list of the 32-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_32_BIT_UUID_COMPLETE     0x05 /**< Complete list of the 32-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_128_BIT_UUID_PART        0x06 /**< Incomplete list of the 128-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_128_BIT_UUID_COMPLETE    0x07 /**< Complete list of the 128-bit service class UUIDs. */
#define BT_GAP_LE_AD_TYPE_NAME_SHORT               0x08 /**< Shortened local name. */
#define BT_GAP_LE_AD_TYPE_NAME_COMPLETE            0x09 /**< Complete local name. */
#define BT_GAP_LE_AD_TYPE_TX_POWER                 0x0A /**< Transmit power level. */
#define BT_GAP_LE_AD_TYPE_COD                      0x0D /**< Class of the device. */
#define BT_GAP_LE_AD_TYPE_SM_TK                    0x10 /**< Security manager's TK value. */
#define BT_GAP_LE_AD_TYPE_SM_OOB_FLAG              0x11 /**< Security manager's Out-of-Band flags. */
#define BT_GAP_LE_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE  0x12 /**< Slave connection interval range. */
#define BT_GAP_LE_AD_TYPE_16_BIT_SOLICITATION_UUID 0x14 /**< List of the 16-bit service solicitation UUIDs. */
#define BT_GAP_LE_AD_TYPE_128_BIT_SOLICITATION_UUID        0x15 /**< List of the 128-bit service solicitation UUIDs. */
#define BT_GAP_LE_AD_TYPE_SERVICE_DATA             0x16 /**< Service data. */
#define BT_GAP_LE_AD_TYPE_16_BIT_UUID_DATA         0x16 /**< Service data with 16-bit UUID. */
#define BT_GAP_LE_AD_TYPE_PUBLIC_TARGET_ADDRESS    0x17 /**< Public target address. */
#define BT_GAP_LE_AD_TYPE_RANDOM_TARGET_ADDRESS    0x18 /**< Random target address. */
#define BT_GAP_LE_AD_TYPE_APPEARANCE               0x19 /**< Appearance. */
#define BT_GAP_LE_AD_TYPE_ADV_INTERVAL             0x1A /**< Advertising interval. */
#define BT_GAP_LE_AD_TYPE_LE_BT_DEVICE_ADDRESS     0x1B /**< Bluetooth LE device address. */
#define BT_GAP_LE_AD_TYPE_LE_ROLE                  0x1C /**< LE role. */
#define BT_GAP_LE_AD_TYPE_32_BIT_SOLICITATION_UUID 0x1F /**< List of 32-bit service solicitation UUIDs. */
#define BT_GAP_LE_AD_TYPE_32_BIT_UUID_DATA         0x20 /**< Service data - 32-bit UUID. */
#define BT_GAP_LE_AD_TYPE_128_BIT_UUID_DATA        0x21 /**< Service data - 128-bit UUID. */
#define BT_GAP_LE_AD_TYPE_MANUFACTURER_SPECIFIC    0xFF /**< Manufacturer specific data. */

#define BT_GAP_LE_AD_FLAG_LIMITED_DISCOVERABLE     (0x01 << 0) /**< LE limited discoverable mode. */
#define BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE     (0x01 << 1) /**< LE general discoverable mode. */
#define BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED     (0x01 << 2) /**< BR/EDR not supported. */

#define BT_GAP_LE_DIRECT_ADDR_TYPE_INVALID         0           /**< For #bt_gap_le_advertising_report_ind_t. */

#define BT_GAP_LE_SECURITY_UNAUTH_MASK             0x0         /**< The key is unauthenticated. */
#define BT_GAP_LE_SECURITY_AUTHORIZATION_MASK      (0x1 << 0)  /**< The key is authorized. */
#define BT_GAP_LE_SECURITY_AUTHENTICATION_MASK     (0x1 << 1)  /**< The key is authenticated. */
#define BT_GAP_LE_SECURITY_ENCRYPTION_MASK         (0x1 << 2)  /**< The LTK is available. */
#define BT_GAP_LE_SECURITY_SIGN_MASK               (0x1 << 3)  /**< The CSRK is available. */
#define BT_GAP_LE_SECURITY_LESC_MASK               (0x1 << 4)  /**< Use LE secure connection. */
#define BT_GAP_LE_SECURITY_BONDED_MASK             (0x1 << 5)  /**< Bonded mask. */
typedef uint8_t bt_gap_le_security_mode_t;                     /**< The security mode of the device. */

#define BT_GAP_LE_CLEAR_WHITE_LIST         0x2010                  /**< Clear the white list. */
#define BT_GAP_LE_ADD_TO_WHITE_LIST        0x2011                  /**< Add to the white list. */
#define BT_GAP_LE_REMOVE_FROM_WHITE_LIST   0x2012                  /**< Remove from the white list. */
typedef uint32_t bt_gap_le_set_white_list_op_t;                    /**< The operation type to the white list. */

#define BT_GAP_LE_ADD_TO_PERIODIC_ADV_LIST          0x2047  /**< Operation type: add a device to the periodic advertiser list with the #bt_hci_cmd_le_add_device_to_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_REMOVE_FROM_PERIODIC_ADV_LIST     0x2048  /**< Operation type: remove a device from the periodic advertiser list with the #bt_hci_cmd_le_remove_device_from_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_CLEAR_PERIODIC_ADV_LIST           0x2049  /**< Operation type: clean the periodic advertiser list with NULL type parameters. */
typedef uint16_t bt_gap_le_set_periodic_adv_list_op_t;  /**< Operation type of the set periodic advertiser list. */

#define BT_GAP_LE_ADD_TO_RESOLVING_LIST                     0x2027  /**< Operation type: add a device to the resolving list with the #bt_hci_cmd_le_add_device_to_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_REMOVE_FROM_RESOLVING_LIST                0x2028  /**< Operation type: remove a device from the resolving list with the #bt_hci_cmd_le_remove_device_from_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_CLEAR_RESOLVING_LIST                      0x2029  /**< Operation type: clean the resolving list with NULL type parameters. */
typedef uint32_t bt_gap_le_set_resolving_list_op_t;             /**< Operation type of the set resolving list. */

#define BT_GAP_LE_ADVERTISING_HANDLE_MIN    0x01                /**< The minimum value of advertising handle. */
#define BT_GAP_LE_ADVERTISING_HANDLE_MAX    0xEF                /**< The maximum value of advertising handle. */
typedef uint8_t bt_gap_le_advertising_handle_t;             /**< The type of advertising handle. */

/**
 * @brief LE security modes.
 */
#define BT_GAP_LE_SECURITY_MODE1_1 (BT_GAP_LE_SECURITY_UNAUTH_MASK)    /**< Mode 1 level 1, no security. */
#define BT_GAP_LE_SECURITY_MODE1_2 (BT_GAP_LE_SECURITY_UNAUTH_MASK | BT_GAP_LE_SECURITY_ENCRYPTION_MASK) /**< Mode 1 level 2, link encryption by unauthenticated key. */
#define BT_GAP_LE_SECURITY_MODE1_3 (BT_GAP_LE_SECURITY_AUTHENTICATION_MASK | BT_GAP_LE_SECURITY_ENCRYPTION_MASK)  /**< Mode 1 level 3, link encryption by authenticated key. */
#define BT_GAP_LE_SECURITY_MODE1_4 (BT_GAP_LE_SECURITY_LESC_MASK | BT_GAP_LE_SECURITY_AUTHENTICATION_MASK | BT_GAP_LE_SECURITY_ENCRYPTION_MASK) /**< Mode 1 level 4, link encryption by authenticated LE secure connection key. */
#define BT_GAP_LE_SECURITY_MODE2_1 (BT_GAP_LE_SECURITY_UNAUTH_MASK | BT_GAP_LE_SECURITY_SIGN_MASK) /**< Mode 2 level 1, Data signed by unauthenticated key. */
#define BT_GAP_LE_SECURITY_MODE2_2 (BT_GAP_LE_SECURITY_AUTHENTICATION_MASK | BT_GAP_LE_SECURITY_SIGN_MASK) /**< Mode 2 level 2, Data signed by authenticated key. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_gap_struct Struct
 * @{
 */

/**
 * @brief For more information about the pairing configuration, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [Vol 3, Part H] Section 3.5.1</a>.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_smp_io_capability_t      io_capability;                  /**< Device I/O Capability. */
    bt_gap_le_smp_oob_data_flag_t      oob_data_flag;                  /**< Device Out-of-Band Capability. */
    bt_gap_le_smp_auth_req_t           auth_req;                       /**< Device Authentication Requirement. */
    uint8_t                            maximum_encryption_key_size;    /**< The maximum size of an encryption key. The recommended value is 16. */
    bt_gap_le_smp_key_distribute_t     initiator_key_distribution;     /**< The key sent from a Central device. */
    bt_gap_le_smp_key_distribute_t     responder_key_distribution;     /**< The key sent from a Peripheral device. */
}) bt_gap_le_smp_pairing_config_t;

/**
 *  @brief Bonding information.
 */
BT_PACKED(
typedef struct {
    bt_key_t    ltk;          /**< Long term key. */
}) bt_gap_le_encryption_info_t;

/**
 *  @brief Master identification.
 */
BT_PACKED(
typedef struct {
    uint16_t    ediv;         /**< Encrypted diversifier. */
    uint8_t     rand[8];      /**< 64-bit random number. */
}) bt_gap_le_master_identification_t;

/**
 *  @brief Identity information.
 */
BT_PACKED(
typedef struct {
    bt_key_t    irk;          /**< Identity resolving key. */
}) bt_gap_le_identity_info_t;

/**
 *  @brief set/remove/clear resolving list confirmation.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_set_resolving_list_op_t    resolving_list_confirm;          /**< Set/remove/clear resolving list confirmation. */
}) bt_gap_le_resolving_list_confirm_t;

/**
 *  @brief Identity address information.
 */
BT_PACKED(
typedef struct {
    bt_addr_t   address;       /**< Identity resolving address. */
}) bt_gap_le_identity_address_info_t;

/**
 *  @brief Signing information.
 */
BT_PACKED(
typedef struct {
    bt_key_t    csrk;          /**< Connection signature resolving key. */
}) bt_gap_le_signing_info_t;

/**
 *  @brief LE local key information.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_encryption_info_t         encryption_info;    /**< Encryption information. */
    bt_gap_le_master_identification_t   master_id;          /**< Master identification. */
    bt_gap_le_identity_info_t           identity_info;      /**< Identity information. */
    bt_gap_le_signing_info_t            signing_info;       /**< Signing information. */
}) bt_gap_le_local_key_t;

/**
 *  @brief LE bonding information structure.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_local_key_t               local_key;          /**< Local key. */
    bt_gap_le_encryption_info_t         encryption_info; /**< Encryption information. */
    bt_gap_le_master_identification_t   master_id;       /**< Master identification. */
    bt_gap_le_identity_info_t           identity_info;   /**< Identity information. */
    bt_gap_le_identity_address_info_t   identity_addr;      /**< Identity address. */
    bt_gap_le_signing_info_t            signing_info;    /**< Signing information. */
    uint32_t                            sign_counter;       /**< Sign counter. */
    uint32_t                            gattc_sign_counter; /**< Sign counter of the GATT client. */
    bt_gap_le_security_mode_t           key_security_mode;  /**< Security mode. */
    uint8_t                             key_size;           /**< Key size. */
}) bt_gap_le_bonding_info_t;

typedef void bt_gap_le_set_advertising_cnf_t;    /**< Advertising confirmation. */


/**
 *  @brief Indication to request the local configuration.
 */
typedef struct {
    bt_gap_le_local_key_t      *local_key_req; /**< Set a pointer to the local key. */
    bool                  sc_only_mode_req;    /**< Set this variable to enable the secure connection only mode. */
} bt_gap_le_local_config_req_ind_t;

/**
 *  @brief Indication to request bonding information.
 */
typedef struct {
    const bt_addr_t            remote_addr;       /**< The address of a remote device to be bonded. */
    bt_gap_le_bonding_info_t   *bonding_info_req; /**< Set a pointer to the connection bonding information. */
} bt_gap_le_bonding_info_req_ind_t;

/**
 *  @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_CONNECT_IND event.
 *         Connection indication, an event sent to the application once the connection is established.
 *         Application may send data after receiving the connection indication successfully.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;     /**< Connection handle. */
    bt_role_t         role;                  /**< Role of the local device. */
    bt_addr_t         peer_addr;             /**< Address information of the remote device. */
    bt_bd_addr_t      local_resolvable_private_address; /**< Resolvable private address used by the local device.
                                                           It is only valid when its own address type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY. */
    bt_bd_addr_t      peer_resolvable_private_address; /**< Resolvable private address used by the remote device.
                                                          It is only valid when the peer address type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY. */
    uint16_t          conn_interval;         /**< Connection interval on this connection. */
    uint16_t          conn_latency;          /**< Connection latency on this connection. */
    uint16_t          supervision_timeout;   /**< Supervision timeout on this connection. */
    uint8_t           master_clock_accuracy; /**< Clock accuracy in master. */
    bt_addr_t         local_addr;            /**< Address information of the local device. */
}) bt_gap_le_connection_ind_t;

/**
 *  @brief Received connection update parameters from the remote device.
 */
typedef struct {
    uint16_t        interval_min;            /**< Range[6..0x190], Time = N * 1.25 msec. */
    uint16_t        interval_max;            /**< Range[6..0x190], Time = N * 1.25 msec. */
    uint16_t        slave_latency;           /**< Range[0..0x1F3]. */
    uint16_t        timeout_multiplier;      /**< Range[0x0A..0xC80], Time = N * 10 msec. */
} bt_gap_le_connection_update_param_t;

/**
 *  @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_CONNECTION_UPDATE_IND event.
 *         Connection update indication, an event sent to the application after
 *         the controller has already updated the connection parameters to
 *         indicate the connection is updated.
 */
typedef struct {
    bt_handle_t     conn_handle;         /**< Connection handle. */
    uint16_t        conn_interval;       /**< Updated connection interval. */
    uint16_t        conn_latency;        /**< Updated connection latency. */
    uint16_t        supervision_timeout; /**< Updated supervision timeout. */
} bt_gap_le_connection_update_ind_t;

typedef bt_hci_evt_disconnect_complete_t bt_gap_le_disconnect_ind_t;  /**< This structure defines the associated parameter type in the callback for #BT_GAP_LE_DISCONNECT_IND event. */

/**
 *  @brief Bonding start indication. Application should set the pairing_config_req variable.
 */
typedef struct {
    const bt_handle_t                  handle;                     /**< Connection handle. */
    const uint16_t                     is_previously_bonded;       /**< Informs whether the remote device has been paired before. */
    bt_gap_le_smp_pairing_config_t     pairing_config_req;         /**< Requested pairing configuration. */
} bt_gap_le_bonding_start_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_BONDING_COMPLETE_IND event.
 */
typedef struct {
    bt_handle_t     handle; /**< Connection handle. */
} bt_gap_le_bonding_complete_ind_t;

#ifdef __BT_MULTI_ADV__
/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_START_MULTIPLE_ADVERTISING_CNF event.
 */
typedef struct {
    uint8_t    instance;          /**< The multi-advertising instance. */
} bt_gap_le_start_multiple_advertising_cnf_t;

typedef bt_gap_le_start_multiple_advertising_cnf_t bt_gap_le_stop_multiple_advertising_cnf_t; /**< This structure defines the associated parameter type in the callback for #BT_GAP_LE_STOP_MULTIPLE_ADVERTISING_CNF event. */
#endif

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_BONDING_REPLY_REQ_IND event.
 */
typedef struct {
    bt_handle_t                    handle;              /**< Connection handle. */
    bt_gap_le_smp_pairing_method_t method;              /**< The pairing method. */
    uint32_t                       passkey_display: 20; /**< The passkey to be displayed on the screen. */
} bt_gap_le_bonding_reply_req_ind_t;

/**
 *  @brief Bond reply.
 */
typedef struct {
    union {
        bt_key_t    oob_data;    /**< The received Out-of-Band data. */
        uint32_t    passkey: 20; /**< The entered passkey. */
        bool   nc_value_matched; /**< Set to true, if the displayed value is a match when applying a numeric comparison. */
    };
} bt_gap_le_bonding_reply_t;

/**
 *  @brief Temp key info.
 */
typedef struct {
    bt_key_t    tk;                      /**< Local key. */
} bt_gap_le_temp_key_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_ADVERTISING_REPORT_IND event.
 *         Advertising report indication, an event sent to the application after
 *         receiving the advertising report from the remote device.
 *         Enable the scan to receive an advertising report.
 *         Please refer to the #bt_gap_le_set_scan() API.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_adv_report_evt_type_t event_type;                         /**< Event type. */
    bt_addr_t       address;                                            /**< The address of the advertiser sending an advertisement. */
    bt_addr_t       direct_address;                                     /**< The address to send the directed advertisement. */
    int8_t          rssi;                                               /**< The RSSI value. */
    uint8_t         data_length;                                        /**< Length of the data. */
    uint8_t         data[BT_HCI_LE_ADVERTISING_DATA_LENGTH_MAXIMUM];    /**< Content of the advertisement data. */
}) bt_gap_le_advertising_report_ind_t;

/**
 * @brief This structure defines the detail information about le connection.
 */
typedef struct {
    bt_handle_t       connection_handle;     /**< Connection handle. */
    bt_role_t         role;                  /**< Role of the local device. */
    bt_addr_t         peer_addr;             /**< Address information of the remote device. */
    bt_addr_t         local_resolvable_private_address; /**< Resolvable private address used by the local device.
                                                           It is only valid when its own address type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY. */
    bt_addr_t         peer_resolvable_private_address; /**< Resolvable private address used by the remote device.
                                                          It is only valid when the peer address type is #BT_ADDR_PUBLIC_IDENTITY or #BT_ADDR_RANDOM_IDENTITY. */
    bt_addr_t         local_addr;            /**< Address information of the local device. */
} bt_gap_le_connection_information_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_PHY_UPDATE_IND event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t         handle;     /**< Connection handle. */
    bt_hci_le_phy_t     tx;         /**< The transmitter PHY of the connection. */
    bt_hci_le_phy_t     rx;         /**< The recevier PHy of connection. */
}) bt_gap_le_phy_update_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_EXT_ADVERTISING_REPORT_IND event.
 *         Extended advertising report indication, an event sent to the application after
 *         receiving the extended advertising report from the remote device.
 *         Enable the extended scan to receive an extended advertising report.
 *         Please refer to the #bt_gap_le_set_extended_scan() API.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_ext_adv_report_evt_mask_t     event_type;                     /**< The advertising event type. */
    bt_addr_t                               address;                        /**< The address of the advertiser sending an advertisement. */
    bt_hci_le_adv_phy_t                     primary_phy;                    /**< The Advertiser PHY on primary advertising channel. */
    bt_hci_le_adv_phy_t                     secondary_phy;                  /**< The Advertiser PHY on secondary advertising channel. */
    uint8_t                                 advertising_SID;                /**< The value of the advertising SID subfield in the ADI field of the PDU. */
    int8_t                                  tx_power;                       /**< The Tx power value, 0x7F means the Tx power information not available. */
    int8_t                                  rssi;                           /**< The RSSI value, 0x7F means the RSSI information not available. */
    uint16_t                                periodic_advertising_interval;  /**< The interval of the periodic advertising, 0 means no periodic advertising. */
    bt_addr_t                               direct_address;                 /**< The address to send the directed advertisement. */
    uint8_t                                 data_length;                    /**< Length of the data. */
    bt_data_t                               data;                           /**< Content of the advertisement data. */
}) bt_gap_le_ext_advertising_report_ind_t;

/**
 *  @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_IND event.
 *         Periodic advertising sync established indication, an event sent to the application once the periodic advertising sync is established.
 *         Application may send data periodically after receiving the periodic advertising sync established indication successfully.
 */
BT_PACKED(
typedef struct {
    bt_handle_t         sync_handle;                    /**< Sync handle. */
    uint8_t             advertising_SID;                /**< The value of advertising SID subfield in the ADI filed of the PDU. */
    bt_addr_type_t      advertiser_address_type;        /**< Advertiser Address Type. */
    bt_bd_addr_t        advertiser_address;             /**< Address of the advertising device. */
    bt_hci_le_adv_phy_t advertiser_phy;                 /**< The advertiser PHY value. */
    uint16_t            periodic_advertising_interval;  /**< The interval of the periodic advertising. */
    uint8_t             advertiser_clock_accuracy;      /**< The advertiser clock accuracy. */
}) bt_gap_le_periodic_advertising_sync_established_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_PERIODIC_ADVERTISING_REPORT_IND event.
 *         Periodic advertising report indication, an event sent to the application after
 *         receiving the periodic advertising report from the remote device.
 *         Create a periodic advertising sync to receive a periodic advertising report.
 *         Please refer to the bt_gap_le_periodic_advertising_create_sync() API.
 */
BT_PACKED(
typedef struct {
    bt_handle_t                             sync_handle;    /**< Sync handle. */
    int8_t                                  tx_power;       /**< The Tx power value, 0x7F means the Tx power information not available. */
    int8_t                                  rssi;           /**< The RSSI value, 0x7F means the RSSI information not available. */
    uint8_t                                 unused;         /**< The unused value. */
    bt_gap_le_ext_adv_report_data_status_t  data_status;    /**< The data status of the report event. */
    uint8_t                                 data_length;    /**< The periodic advertising data length. */
    bt_data_t                               data;           /**< Content of the periodic advertisement data. */
}) bt_gap_le_periodic_advertising_report_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_LOST_IND event.
 *         Periodic advertising sync lost indication, an event sent to the application when there
 *         has not received a periodic advertising packet within the timeout period.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       sync_handle;          /**< Sync handle. */
}) bt_gap_le_periodic_advertising_sync_lost_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_ADVERTISING_SET_TERMINATED_IND event.
 *         Advertising set terminated indication, an event sent to the application indicates the
 *         controller has terminated the advertising.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_advertising_handle_t  handle;                                     /**< The termincated advertising handle. */
    uint16_t                        connection_handle;                          /**< The connection handle which indicate the advertising terminated due to a connection had been established by the advertising. */
    uint8_t                         num_completed_extended_advertising_events;  /**< The number of completed extended advertising events transmitted by the controller. */
}) bt_gap_le_advertising_set_terminated_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_SCAN_REQUEST_RECEIVED_IND event.
 *         Scan request received indication, an event sent to the application when there
 *         has received a scan request from remote device.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_advertising_handle_t  handle;             /**< Advertising handle. */
    bt_addr_t                       scanner_address;    /**< The scanner's address.*/
}) bt_gap_le_scan_request_received_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_CHANNEL_SELECTION_ALGORITHM_IND event.
 *         Channel selection algorithm indication, an event sent to the application indicates
 *         which channel selection algorithm is used.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;            /**< Connection handle. */
    uint8_t           channel_selection_algorithm;  /**< Used channel selection algorithm in controller, 0x00 means LE Channel Selection Algorithm #1 is used, 0x02 means LE Channel Selection Algorithm #2 is used. */
}) bt_gap_le_channel_selection_algorithm_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_ENABLE_EXTENDED_ADVERTISING_CNF event.
 */
typedef struct {
    bt_gap_le_advertising_handle_t  handle; /**< Advertising handle. */
    bt_hci_enable_t enable;                 /**< Extended advertising enable, #BT_HCI_ENABLE means extended advertising enabled. */
} bt_gap_le_enable_extended_advertising_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_CONFIG_EXTENDED_ADVERTISING_CNF event.
 */
typedef struct {
    bt_gap_le_advertising_handle_t  handle; /**< Advertising handle. */
    int8_t      selected_tx_power;          /**< Current selected Tx power by controller. */
} bt_gap_le_config_extended_advertising_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_ENABLE_PERIODIC_ADVERTISING_CNF event.
 */
typedef struct {
    bt_gap_le_advertising_handle_t  handle; /**< Advertising handle. */
    bt_hci_enable_t enable;                 /**< Periodic advertising enable, #BT_HCI_ENABLE means periodic advertising enabled. */
} bt_gap_le_enable_periodic_advertising_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_CONFIG_PERIODIC_ADVERTISING_CNF event.
 */
typedef struct {
    bt_gap_le_advertising_handle_t  handle; /**< Advertising handle. */
} bt_gap_le_config_periodic_advertising_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_REMOVE_ADVERTISING_CNF event.
 */
typedef struct {
    bt_gap_le_advertising_handle_t  handle; /**< Advertising handle. */
    bt_hci_enable_t clear_all;              /**< All advertising handle cleared flag, #BT_HCI_ENABLE means all advertising handle had cleared. */
} bt_gap_le_remove_advertising_cnf_t;

/**
 *  @brief      LE set extended advertising data command.
 */
typedef struct {
    uint16_t     data_length;           /**< Length of the extended advertising data. For the maximum advertising data length, please use the API #bt_gap_le_get_max_adv_length. */
    uint8_t     *data;                  /**< Extended advertising data. */
    uint8_t      fragment_preference;   /**< 0x00 means controller may fragment all host advertising data, 0x01 means controller should not fragment or should minimize fragmentation of host advertising data. All other values reserved. */
} bt_gap_le_set_ext_advertising_data_t;

/**
 *  @brief      LE set extended scan response data command.
 */
typedef bt_gap_le_set_ext_advertising_data_t bt_gap_le_set_ext_scan_response_data_t;

/**
 *  @brief      LE set periodic advertising data command.
 */
typedef struct {
    uint16_t     data_length;           /**< The length of periodic advertising data. For the maximum advertising data length, please use the API #bt_gap_le_get_max_adv_length. */
    uint8_t     *data;                  /**< Periodic advertising data. */
} bt_gap_le_set_periodic_advertising_data_t;

/**
 *  @brief This structure defines the parameter in the callback for #BT_GAP_LE_RPA_ROTAION_IND event.
 */
BT_PACKED(
typedef struct {
    bt_gap_le_advertising_handle_t      handle;     /**< Advertising handle. */
    bt_bd_addr_t                        address;    /**< The current advertising rpa address. */
}) bt_gap_le_rpa_rotation_ind_t;

/**
 *  @brief This structure defines the parameter #BT_GAP_LE_HANDLE_UPDATE event.
 */
typedef struct {
    uint8_t num;                    /**< link number. */
    bt_handle_t *old_handles;       /**< The list for old handle. */
    bt_handle_t *new_handles;       /**< The list for new handle. */
} bt_gap_le_handle_update_t;

/**
 * @}
 */

/**
 * @brief     This function sets a random address. The random address has to be configured before advertising, scanning or initiating.
 *            The application receives the #BT_GAP_LE_SET_RANDOM_ADDRESS_CNF event after the set random address command is complete.
 * @param[in] random_addr   is a pointer to a given random address(6 bytes). It should not be NULL.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_random_address(bt_bd_addr_ptr_t random_addr);

/**
 * @brief     This function gets the random address.
 * @return    The random address.
 */
bt_bd_addr_ptr_t bt_gap_le_get_random_address(void);

/**
 * @brief      This function gets the connection information according to the connection handle.
 * @param[in]  conn_handle         is the connection handle.
 * @param[out] conn_info           is the connection information.
 * @return     #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_get_connection_information(bt_handle_t conn_handle, bt_gap_le_connection_information_t *conn_info);

/**
 * @brief     This function adds, removes or clears a device from the white list.
 *            The application receives the #BT_GAP_LE_SET_WHITE_LIST_CNF event after the adding, removing or clearing command is complete.
 * @param[in] op            is the method to operate the white list. Please refer to #bt_gap_le_set_white_list_op_t.
 * @param[in] address       is the address with a type to be set.
 *                          It should not be NULL when using #BT_GAP_LE_ADD_TO_WHITE_LIST or #BT_GAP_LE_REMOVE_FROM_WHITE_LIST.
 *                          And it should be NULL when using the #BT_GAP_LE_CLEAR_WHITE_LIST operation.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_white_list(bt_gap_le_set_white_list_op_t op, const bt_addr_t *address);

/**
 * @brief     This function adds, removes or clears a device from the resolving list.
 *            The application receives the #BT_GAP_LE_SET_RESOLVING_LIST_CNF event after the adding, removing or clearing operation is complete.
 * @param[in] op            is the method to operate the resolving list.
 *                          The op could be #BT_GAP_LE_ADD_TO_RESOLVING_LIST, #BT_GAP_LE_REMOVE_FROM_RESOLVING_LIST or #BT_GAP_LE_CLEAR_RESOLVING_LIST.
 *                          For #BT_GAP_LE_ADD_TO_RESOLVING_LIST, the user should create a #bt_hci_cmd_le_add_device_to_resolving_list_t and pass the address to the device parameter.
 *                          For #BT_GAP_LE_REMOVE_FROM_RESOLVING_LIST, the user should create a #bt_hci_cmd_le_remove_device_from_resolving_list_t and pass the address to the device parameter.
 *                          For #BT_GAP_LE_CLEAR_RESOLVING_LIST, the user should set NULL as the device parameter.
 * @param[in] device        is the device to be added or removed.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_resolving_list(bt_gap_le_set_resolving_list_op_t op, const void *device);

/**
 * @brief     This function enables or disables the address resolution. The application receives the #BT_GAP_LE_SET_ADDRESS_RESOLUTION_ENABLE_CNF event after this command is complete.
 * @param[in] enable         is a flag to specify whether the address resolution in the controller should be disabled (0) or enabled (1).
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_address_resolution_enable(bool enable);

/**
 * @brief     This function sets the resolvable private address timeout value. The application receives the #BT_GAP_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_CNF event after this command is complete.
 * @param[in] timeout        is the resolvable private address timeout value.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_resolvable_private_address_timeout(uint32_t timeout);

/**
 * @brief     This function sets the advertising.
 *            Commands will be sent in the sequence of advertising parameter, advertising data, scan response data and advertising enable.
 *            The stack will not set advertising parameters if the advertising parameter is NULL.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the #BT_GAP_LE_SET_ADVERTISING_CNF event after the enable command is complete.
 * @param[in] enable        is a switch for the advertiser, cannot be NULL.
 * @param[in] param         is the advertising parameter.
 * @param[in] data          is the advertising data.
 * @param[in] scan_rsp      is the scan response data.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_advertising(
    const bt_hci_cmd_le_set_advertising_enable_t *enable,
    const bt_hci_cmd_le_set_advertising_parameters_t *param,
    const bt_hci_cmd_le_set_advertising_data_t *data,
    const bt_hci_cmd_le_set_scan_response_data_t *scan_rsp);

//__MTK_COMMON__
/**
 * @brief     This function sets the advertising.
 *            Commands will be sent in the sequence of advertising parameter, advertising data, scan response data and advertising enable.
 *            The stack will not set advertising parameters if the advertising parameter is NULL.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the #BT_GAP_LE_SET_ADVERTISING_CNF event after the enable command is complete.
 * @param[in] enable        is a switch for the advertiser, cannot be NULL.
 * @param[in] param         is the advertising parameter.
 * @param[in] data          is the advertising data.
 * @param[in] scan_rsp      is the scan response data.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_advertising_single(
    const bt_hci_cmd_le_set_advertising_enable_t *enable,
    const bt_hci_cmd_le_set_advertising_parameters_t *param,
    const bt_hci_cmd_le_set_advertising_data_t *data,
    const bt_hci_cmd_le_set_scan_response_data_t *scan_rsp);

/**
 * @brief     This function sets the scan. Commands will be sent in the sequence of scan parameter and scan enable.
 *            The stack will not set the scanning parameter if the scan parameter is NULL.
 *            The application receives the #BT_GAP_LE_SET_SCAN_CNF event after the enable command is complete and
 *            #BT_GAP_LE_ADVERTISING_REPORT_IND after the remote device sends the advertising packet.
 * @param[in] enable        is a switch for the scanner, cannot be NULL.
 * @param[in] param         is a pointer to the scan parameters or NULL if the user needs to disable the scan.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_scan(const bt_hci_cmd_le_set_scan_enable_t *enable, const bt_hci_cmd_le_set_scan_parameters_t *param);

/**
 * @brief     This function creates the link layer connection. The application receives the #BT_GAP_LE_CONNECT_CNF event after the create connection command is complete.
 *            Then the application receives the #BT_GAP_LE_CONNECT_IND event after the LE read remote used features complete event is received or the disconnection complete event is received.
 * @param[in] param         is the connection parameter.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_connect(const bt_hci_cmd_le_create_connection_t *param);

/**
 * @brief     This function cancels the link layer connection. The operation is issued only after creating a connection and before receiving the #BT_GAP_LE_CONNECT_IND event.
 *            The application receives the #BT_GAP_LE_CONNECT_CANCEL_CNF event after the cancel connection command is complete.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_cancel_connection(void);

/**
 * @brief     This function disconnects the link layer connection.
 *            The application receives the #BT_GAP_LE_DISCONNECT_CNF event after the disconnect command is complete and
 *            the #BT_GAP_LE_DISCONNECT_IND event after the disconnection complete event is received.
 * @param[in] param         is the parameter of disconnection request.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_disconnect(const bt_hci_cmd_disconnect_t *param);

/**
 * @brief     This function updates the connection parameter. The stack will send a request to the controller to update LE connection parameter if the stack is the master.
 *            The stack will send an updating request to the remote device if the stack is the slave.
 *            The application receives the #BT_GAP_LE_CONNECTION_UPDATE_CNF event after the HCI LE connection update command is complete or the L2CAP connection parameter update response is received.
 *            Then the application receives the #BT_GAP_LE_CONNECTION_UPDATE_IND event after the connection update complete event is received.
 * @param[in] param         is the connection update parameter.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_update_connection_parameter(const bt_hci_cmd_le_connection_update_t *param);

/**
 * @brief     This function starts the pairing procedure.
 *            If the device acts as the master and needs (re-)bonding, it will start the SM.
 *            If the device acts as the master and was bonded before, it will start the encryption.
 *            If the device acts as the slave, it will send SM security request.
 *            The #bt_gap_le_get_pairing_config() callback is invoked after the pairing procedure starts and the application receives
 *            the #BT_GAP_LE_BONDING_COMPLETE_IND event after the pairing is complete.
 *            The #BT_GAP_LE_BONDING_REPLY_REQ_IND event may be received to request the passkey or Out-of-Band
 *            data for pairing.
 * @param[in] handle         is the handle of the connection to bond.
 * @param[in] pairing_config is the pairing configuration to be used to bond with the remote device.
 * @return    #BT_STATUS_SUCCESS, the bonding procedure started successfully.
 *            #BT_STATUS_FAIL, the SM session is busy.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_bond(uint32_t handle, bt_gap_le_smp_pairing_config_t const *const pairing_config);

/**
 * @brief     This function replies to the #BT_GAP_LE_BONDING_REPLY_REQ_IND message.
 *            Reply oob_data for Out-of-Band or passkey for Passkey Input pairing method.
 * @param[in] handle   is the handle of the connection you want reply to.
 * @param[in] rsp      is the TK value (oob_data or passkey).
 * @return    #BT_STATUS_SUCCESS, the application replied with the Out-of-Band data or passkey successfully.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_bonding_reply(uint32_t handle, bt_gap_le_bonding_reply_t const *const rsp);

/**
 * @brief     This function reads the RSSI value of the specific connection. The application receives the #BT_GAP_LE_READ_RSSI_CNF event after the read RSSI command is complete.
 * @param[in] handle        is the connection handle.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_read_rssi(const bt_hci_cmd_read_rssi_t *handle);

/**
 * @brief     This function updates the data length used for a given connection. The application receives the #BT_GAP_LE_UPDATE_DATA_LENGTH_CNF event after the 'set data length command' is complete.
 * @param[in] param         is a pointer to the parameters of set data length, including connection handle, TX octets and TX time.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_update_data_length(const bt_hci_cmd_le_set_data_length_t *param);

/**
 * @brief     This function sets the radio transmission power for a given connection. The application receives the #BT_GAP_LE_SET_TX_POWER_CNF event after the set TX power command is complete.
 * @param[in] param         is a pointer to a structure that specifies the parameters for TX power.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_set_tx_power(const bt_hci_cmd_le_set_tx_power_t *param);

/**
 * @brief     This is a user-defined API that returns the local configuration.
 * @return    A pointer to the local configuration containing the default local key and secure connection mode flag. The pointer should not be NULL and should point to a global variable.
 */
bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void);

/**
 * @brief     This is a user-defined API that returns the special and new local keys. Because of the new local keys, the values of LTK, EDIV, Rand and CSRK should be regenerated each time before they are distributed.
 * @return    A pointer to the local key. The pointer cannot be NULL. It's recommended to use a global variable to store the local key.
 */
bt_gap_le_local_key_t *bt_gap_le_get_local_key(void);

/**
 * @brief   This is a user-defined API that returns the bonding information.
 * @param[in] remote_addr The address of a remote device to be bonded.
 * @return                A pointer to the connection bonding information. The pointer should not be NULL and should point to a global variable.
 */
bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);

/**
 * @brief   This is a user-defined API that gets the pairing configuration.
 * @param[in] ind         A pointer to the bonding start indication structure. The pairing_config_req inside the structure should be a global variable.
 * @return    #BT_STATUS_SUCCESS, if the pairing configuration was set successfully.
 *            #BT_STATUS_OUT_OF_MEMORY.
 */
bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);

/**
 * @brief   This is a user-defined API that returns whether to accept connection parameter update request.
 * @param[in]     handle                  is the handle of Bluetooth LE connection.
 * @param[in,out] connection_parameter    is the connection parameter of the request.
 * @return                                true, if the request is accepted, false otherwise.
 */
bool bt_gap_le_is_connection_update_request_accepted(bt_handle_t handle, bt_gap_le_connection_update_param_t *connection_parameter);

#ifdef __BT_MULTI_ADV__
/**
 * @brief     This function stops a multi-advertising instance.
 *            The application receives the #BT_GAP_LE_STOP_MULTIPLE_ADVERTISING_CNF event after the advertising disable command (the last command) is complete.
 *            It is supported on the MT7697 chip only, and requires linking the 'libble_multi_adv_xxx.a' library.
 * @param[in] instance      is the instance of multi-advertising to be stopped.
 * @return                  #BT_STATUS_SUCCESS, the operations are sent successfully.
 *                          #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                          #BT_STATUS_OUT_OF_MEMORY, the operation failed, out of memory.
 */
bt_status_t bt_gap_le_stop_multiple_advertising(uint8_t instance);

/**
 * @brief     This function starts a multi-advertising instance.
 *            Commands will be sent in the sequence of advertising parameter, advertising data, scan response data and advertising enable.
 *            The application receives the #BT_GAP_LE_START_MULTIPLE_ADVERTISING_CNF event after the advertising enable command (the last command) is complete.
 *            It is supported on the MT7697 chip only, and requires linking the 'libble_multi_adv.a' library.
 * @param[in] instance      is the instance of multi-advertising to start with a range from 1 to (#bt_gap_le_get_max_multiple_advertising_instances() - 1).
 * @param[in] tx_power      is the transmit power of multi-advertising with a range from -70 dBm to 20 dBm.
 * @param[in] address       is the multi-advertising address.
 * @param[in] param         is the multi-advertising parameter, the usage is the same as for #bt_gap_le_set_advertising()
 *                          except advertising_type should not be #BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED, #BT_HCI_ADV_TYPE_CONNECTABLE_DIRECTED_HIGH and #BT_HCI_ADV_TYPE_CONNECTABLE_DIRECTED_LOW.
 * @param[in] data          is the multi-advertising data, the usage is the same as for #bt_gap_le_set_advertising().
 * @param[in] scan_rsp      is the scan response data, the usage is the same as for #bt_gap_le_set_advertising().
 * @return                  #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                          #BT_STATUS_OUT_OF_MEMORY, the operation failed, out of memory.
 */
bt_status_t bt_gap_le_start_multiple_advertising(
    uint8_t instance,
    int8_t  tx_power,
    bt_bd_addr_ptr_t address,
    const bt_hci_cmd_le_set_advertising_parameters_t *param,
    const bt_hci_cmd_le_set_advertising_data_t *data,
    const bt_hci_cmd_le_set_scan_response_data_t *scan_rsp);

/**
 * @brief     This function gets the maximum number of supported multi-advertising instances.
 *            It is supported on the MT7697 chip only, and requires linking the 'libble_multi_adv.a' library.
 * @return    The maximum number of supported multi-advertising instances.
 */
uint8_t bt_gap_le_get_max_multiple_advertising_instances(void);
#endif

/**
 * @brief     This function gets the maximum advertising data length support by controller.
 * @return    The maximum advertising data length.
 */
uint32_t bt_gap_le_get_max_adv_length(void);

/**
 * @brief     This function reads the current transmitter PHY and receiver PHY of the specific connection. The application receives the event #BT_GAP_LE_READ_PHY_CNF event after the read PHY command is complete.
 * @param[in] conn_handle        is the connection handle.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing, please retry after the event #BT_GAP_LE_READ_PHY_CNF had been received.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_get_phy(bt_handle_t conn_handle);

/**
 * @brief     This function sets the preferred phy of the specific connection. The application receives the event #BT_GAP_LE_SET_PHY_CNF event after the set PHY command is complete.
 * @param[in] conn_handle        is the connection handle.
 * @param[in] phy            is the PHY preferences value.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing, please retry after the event #BT_GAP_LE_SET_PHY_CNF had been received.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_set_phy(bt_handle_t conn_handle, const bt_hci_le_set_phy_t *phy);

/**
 * @brief     This function enables or disables the indicated extended advertising.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the event #BT_GAP_LE_ENABLE_EXTENDED_ADVERTISING_CNF event after the enable command is complete.
 * @param[in] handle        is the advertising handle which is indicated when call #bt_gap_le_config_extended_advertising.
 * @param[in] enable        indicate enable or disable an advertising, it can trigger again to change advertising timing even if advertising is enabled.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_ENABLE_EXTENDED_ADVERTISING_CNF had been received, or check the parameters are valid.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_enable_extended_advertising(
    bt_gap_le_advertising_handle_t handle,
    const bt_hci_le_set_ext_advertising_enable_t *enable);

/**
 * @brief     This function creates or configures or changes an extended advertising indicated by handle.
 *            Commands will be sent in the sequence of extended advertising parameter, advertising set random address, extended advertising data and extended scan response data.
 *            The extended advertising parameters are mandatory if the advertising indicated by the handle had not been created before.
 *            A new advertising handle will be created if the handle had not been created before.
 *            The stack will not apply advertising set extended advertising parameter if extended advertising parameters are NULL.
 *            The stack will not apply advertising set random address if random address is NULL.
 *            The stack will not apply extended advertising data if extended advertising data is NULL.
 *            The stack will not apply extended scan response data if extended  scan response data is NULL.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application will receive the event #BT_GAP_LE_CONFIG_EXTENDED_ADVERTISING_CNF after the config command is complete.
 * @param[in] handle        is the extended advertising handle.
 * @param[in] random_addr        is the extended advertising random address,  it can't change when advertising is enabled.
 * @param[in] param        is the extended advertising parameter, it can't change when advertising is enabled.
 * @param[in] data        is the extended advertising data, it can change again even if the advertising is enabled.
 * @param[in] scan_rsp        is the extended scan response data, it can change again even if the advertising is enabled.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_CONFIG_EXTENDED_ADVERTISING_CNF had been received, or check the parameters are valid.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_config_extended_advertising(
    bt_gap_le_advertising_handle_t handle,
    const bt_bd_addr_ptr_t random_addr,
    const bt_hci_le_set_ext_advertising_parameters_t *param,
    const bt_gap_le_set_ext_advertising_data_t *data,
    const bt_gap_le_set_ext_scan_response_data_t *scan_rsp);

/**
 * @brief     This function removes or clears the extended advertising from stack context.
 *            The handle will be invalid if clear all advertising flag is enabled.
 *            If the advertising handle is enabled by the API #bt_gap_le_enable_extended_advertising(), the event #BT_GAP_LE_REMOVE_ADVERTISING_CNF will be with error code command disallowed.
 *            Only if all the advertising handles are disabled, the clear all flag can be set as enable, or the event #BT_GAP_LE_REMOVE_ADVERTISING_CNF will received with error code command disallowed.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the #BT_GAP_LE_REMOVE_ADVERTISING_CNF event after the remove extended advertising command is complete.
 * @param[in] handle        is the advertising handle which is indicated when call #bt_gap_le_config_extended_advertising.
 * @param[in] clear_all     is the flag to clear all advertising handle.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_REMOVE_ADVERTISING_CNF or check the parameters are valid.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_remove_extended_advertising(
    bt_gap_le_advertising_handle_t handle, bt_hci_enable_t clear_all);

/**
 * @brief     This function sets the extended scan. Commands will be sent in the sequence of scan parameter and scan enable.
 *            The stack will not set the scanning parameter if the scan parameter is NULL.
 *            The application receives the #BT_GAP_LE_SET_EXTENDED_SCAN_CNF event after the enable command is complete and
 *            #BT_GAP_LE_EXT_ADVERTISING_REPORT_IND after the remote device sends the advertising packet.
 * @param[in] params      is a pointer to the extended scan parameters, it can be NULL
 * @param[in] enable     is a switch for the scanner to enable or disable or change the timing of scanning, it can be NULL.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *               #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_SET_EXTENDED_SCAN_CNF or check the parameters are valid.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_set_extended_scan(
    const bt_hci_le_set_ext_scan_parameters_t *params,
    const bt_hci_cmd_le_set_extended_scan_enable_t *enable);

/**
 * @brief     This function enables or disables the indicated periodic advertising.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the event #BT_GAP_LE_ENABLE_PERIODIC_ADVERTISING_CNF event after the enable command is complete.
 * @param[in] handle        is the advertising handle which is indicated when call #bt_gap_le_config_periodic_advertising.
 * @param[in] enable        indicates whether advertising enabled or disabled.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation is not supported on this version.
 *            #BT_STATUS_FAIL, the operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_ENABLE_PERIODIC_ADVERTISING_CNF had been received, or check the parameters are valid.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_enable_periodic_advertising(bt_gap_le_advertising_handle_t handle, bt_hci_enable_t enable);

/**
 * @brief     This function sets the parameters and data of the indicated periodic advertising.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the event #BT_GAP_LE_CONFIG_PERIODIC_ADVERTISING_CNF event after the command is complete.
 * @param[in] handle        is the periodic advertising handle which is indicated when calling #bt_gap_le_config_periodic_advertising.
 * @param[in] params        is the periodic advertising parameters #bt_hci_le_set_periodic_advertising_parameters_t.
 * @param[in] data          is the periodic advertising data #bt_gap_le_set_periodic_advertising_data_t.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation is not supported on this version.
 *            #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_CONFIG_PERIODIC_ADVERTISING_CNF had been received, or check the parameters are valid.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_config_periodic_advertising(bt_gap_le_advertising_handle_t handle,
                                                  const bt_hci_le_set_periodic_advertising_parameters_t *params,
                                                  const bt_gap_le_set_periodic_advertising_data_t *data);
/**
 * @brief     This function sets the periodic advertising list by #bt_gap_le_set_periodic_adv_list_op_t.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 *            The application receives the event #BT_GAP_LE_SET_PERIODIC_ADVERTISER_LIST_CNF event after the command is complete.
 * @param[in] op        is the periodic advertising operation #bt_gap_le_set_periodic_adv_list_op_t.
 * @param[in] param     is the periodic advertising parameters.
 *                           if the op is #BT_GAP_LE_ADD_TO_PERIODIC_ADV_LIST,the paramaters is bt_hci_cmd_le_add_device_to_resolving_list_t;
 *                           if the op is #BT_GAP_LE_REMOVE_FROM_PERIODIC_ADV_LIST, the parameters are bt_hci_cmd_le_remove_device_from_resolving_list_t;
 *                           if the op is #BT_GAP_LE_CLEAR_PERIODIC_ADV_LIST, the parameters are NULL.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation is not supported on this version.
 *            #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_SET_PERIODIC_ADVERTISER_LIST_CNF had been received, or check the parameters are valid.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_set_periodic_advertiser_list(bt_gap_le_set_periodic_adv_list_op_t op, const void *param);

/**
 * @brief     This function sets the privacy mode by #bt_hci_cmd_le_set_privacy_mode_t.
 *            If #BT_STATUS_OUT_OF_MEMORY is returned, application should send fewer commands at a time.
 * @param[in] param     is the privacy mode parameters, it contains the peer identity address,the peer identity address type and the privacy mode.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation is not supported on this version.
 *            #BT_STATUS_FAIL, an identical operation is executing or the parameters are invalid, please retry after the event #BT_GAP_LE_SET_PERIODIC_ADVERTISER_LIST_CNF had been received, or check the parameters are valid.
 */
bt_status_t bt_gap_le_set_privacy_mode(bt_hci_cmd_le_set_privacy_mode_t *param);

/**
 * @brief     This function is used to synchronize with a periodic advertising train from an advertiser and begin receiving periodic advertising packets.
 *            The application receives the event #BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CNF event after the command is complete.
 * @param[in] param     is the periodic advertising parameters #bt_hci_cmd_le_periodic_advertising_create_sync_t .
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *            #BT_STATUS_FAIL, an identical operation is executing or the parameter is invalid, please retry after the event #BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CNF had been received, or check the parameter is valid.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_periodic_advertising_create_sync(const bt_hci_cmd_le_periodic_advertising_create_sync_t *param);

/**
 * @brief     This function is used to cancel the #bt_gap_le_periodic_advertising_create_sync while it is pending.
 *            The application receives the event #BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_CNF event after the command is complete.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *            #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *            #BT_STATUS_FAIL, an identical operation is executing, please retry after the event #BT_GAP_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_CNF had been received.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */

bt_status_t bt_gap_le_periodic_advertising_create_sync_cancel(void);

/**
 * @brief     This is a user-defined API that gets the pairing temp key or ra.
 * @param[in] connection_handle   is the connection handle.
 * @param[in] tk_info             is the temp key save buffer, the pointer cannot be NULL.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_gap_le_get_temp_key(bt_handle_t connection_handle, bt_gap_le_temp_key_t *tk_info);

//__MTK_COMMON__
/**
 * @brief     This function is used to stop reception of the periodic advertising train identified by the Sync_Handle parameter.
 * @param[in] sync_handle    identify the periodic advertising train
 * @return    #BT_STATUS_SUCCESS, terminate successfully
 *            #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 *            #BT_STATUS_FAIL, the opeation is excuting failed or the parameters are invalid
 */
bt_status_t bt_gap_le_periodic_advertising_terminate_sync(bt_handle_t sync_handle);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif

