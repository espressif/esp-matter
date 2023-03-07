/* Copyright Statement:
 *
 * (C) 2020 MediaTek Inc. All rights reserved.
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

#ifndef __BT_GAP_LE_AUDIO_H__
#define __BT_GAP_LE_AUDIO_H__

#ifdef BT_LE_AUDIO_ENABLE
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGAP GAP
 * @{
 * This section introduces the Generic Access Profile (GAP) APIs including terms and acronyms, supported features, details on how to use this driver, GAP function groups, enums, structures and functions.
 * For more information about GAP, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 3, part C]</a>.
 * This section includes details on both Classic Bluetooth and Bluetooth Low Energy modules.
 * @addtogroup GAP_LE_Audio Bluetooth Low Energy Audio
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
 * |\b CIS                        | Connected Isochronous Stream. A logical transport that enables connected devices to transfer isochronous data in either direction. |
 * |\b CIG                        | Connected Isochronous Groups. A group contain multiple CIS instances. |
 * |\b BIS                        | Broadcast Isochronous Stream. A connectionless isochronous communication. |
 * |\b BIG                        | Broadcast Isochronous Groups. A group contain multiple BIS instances. |
 *
 * @section bt_gap_le_audio_api_usage How to use this module
 *
 * - The application calls functions #bt_gap_le_create_cis(), #bt_gap_le_set_cig_parameters() and #bt_gap_le_setup_iso_data_path().
 * - The application shows how to setup CIG and create a CIS connection.
 *  - Sample code:
 *      @code
 *          // Setup CIG parameters for future CIS connection.
 *          void user_application_set_cig()
 *          {
 *              bt_gap_le_cis_param_t p_cis[1] = {0};
 *
 *              p_cis[0].cis_id = 0x00;
 *              p_cis[0].rtn_m_to_s = 0x02;
 *              p_cis[0].rtn_s_to_m = 0x02;
 *              p_cis[0].max_sdu_m_to_s = 120;
 *              p_cis[0].max_sdu_s_to_m = 0;
 *              p_cis[0].phy_m_to_s = 0x02;
 *              p_cis[0].phy_s_to_m = 0x02;
 *
 *              bt_gap_le_set_cig_params_t param = {
 *                  .cig_id = 0x01,
 *                  .sdu_interval_m_to_s = 0x002710,
 *                  .sdu_interval_s_to_m = 0x002710,
 *                  .sca = 0x00,
 *                  .packing = 0x00,
 *                  .framing = 0x00,
 *                  .max_transport_latency_m_to_s = 0x000A,
 *                  .max_transport_latency_s_to_m = 0x000A,
 *                  .cis_list = p_cis,
 *                  .cis_count = 1,
 *              };
 *              bt_gap_le_set_cig_parameters(&param);
 *          }
 *
 *          // A user-defined static callback for the application to listen to the event.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_LE_SET_CIG_PARAMETERS_CNF: {
 *                      bt_gap_le_set_cig_params_cnf_t *cnf = (bt_gap_le_set_cig_params_cnf_t *)buff;
 *                      printf("BT_GAP_LE_SET_CIG_PARAMETERS_CNF state:%x ", cnf->status);
 *
 *                      bt_gap_le_cis_set_t hdl_list[1] = {0x00};
 *                      hdl_list[0].acl_connection_handle = g_acl_connection_handle;    // pre-saved acl connection handle
 *                      hdl_list[0].cis_connection_handle = cnf->cis_connection_handle[0];
 *                      bt_gap_le_create_cis_t param = {
 *                          .cis_count = 0x01,
 *                          .cis_list = hdl_list,
 *                      };
 *                      status = bt_gap_le_create_cis(&param);
 *                      break;
 *                  }
 *
 *                  case BT_GAP_LE_CIS_ESTABLISHED_IND: {
 *                      bt_gap_le_cis_established_ind_t *ind = (bt_gap_le_cis_established_ind_t *)buff;
 *                      printf("CIS_ESTABLISHED_IND connection_handle:%x", ind->connection_handle);
 *
 *                      bt_gap_le_setup_iso_data_path_t param = {
 *                          .handle = ind->connection_handle,
 *                          .direction = BT_GAP_LE_ISO_DATA_PATH_DIRECTION_OUTPUT,
 *                          .data_path_id = 1, // use share buffer #1,
 *                          .codec_format = 0x00,
 *                          .company_id = 0x0000,
 *                          .vendor_codec_id = 0x0000,
 *                          .controller_delay = 0,
 *                          .codec_configuration_length = 0,
 *                          .codec_configuration = NULL
 *                      };
 *                      bt_gap_le_setup_iso_data_path(&param);
 *                      break;
 *                  }
 *
 *                  case BT_GAP_LE_SETUP_ISO_DATA_PATH_CNF: {
 *                      bt_hci_le_setup_iso_data_path_t *cnf = (bt_hci_le_setup_iso_data_path_t *)buff;
 *                      printf("BT_GAP_LE_SETUP_ISO_DATA_PATH_CNF");
 *                      break;
 *                  }
 *
 *                  case BT_GAP_LE_CIS_TERMINATED_IND: {
 *                      bt_gap_le_cis_terminated_ind_t* ind = (bt_gap_le_cis_terminated_ind_t*) buff;
 *                      printf("BT_GAP_LE_CIS_TERMINATED_IND, conn_hadl:%x", ind->connection_handle);
 *                      break;
 *                  }
 *
 *                  case BT_GAP_LE_REMOVE_CIG_CNF: {
 *                      printf("BT_GAP_LE_REMOVE_CIG_CNF");
 *                      break;
 *                  }
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */
#include "bt_gap_le.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup Bluetoothhbif_gap_leaudio_define Define
 * @{
 */
#define BT_GAP_LE_CIS_ESTABLISHED_IND              (BT_MODULE_GAP | 0x0040)    /**< CIS established indication with #bt_gap_le_cis_established_ind_t. */
#define BT_GAP_LE_CIS_TERMINATED_IND               (BT_MODULE_GAP | 0x0041)    /**< CIS terminated indication with #bt_gap_le_cis_terminated_ind_t. */
#define BT_GAP_LE_CIS_REQUEST_IND                  (BT_MODULE_GAP | 0x0042)    /**< CIS request indication with #bt_gap_le_cis_request_ind_t. */
#define BT_GAP_LE_SETUP_ISO_DATA_PATH_CNF          (BT_MODULE_GAP | 0x0043)    /**< Setup ISO data path confirmation with #bt_gap_le_setup_iso_data_path_cnf_t. */
#define BT_GAP_LE_REMOVE_ISO_DATA_PATH_CNF         (BT_MODULE_GAP | 0x0044)    /**< Remove ISO data path confirmation with #bt_gap_le_remove_iso_data_path_cnf_t. */
#define BT_GAP_LE_CIS_CONNECTION_CNF               (BT_MODULE_GAP | 0x0045)    /**< Create CIS connection confirmation with NULL payload. */
#define BT_GAP_LE_SET_CIG_PARAMETERS_CNF           (BT_MODULE_GAP | 0x0046)    /**< Set CIG parameters confirmation with #bt_gap_le_set_cig_params_cnf_t. */
#define BT_GAP_LE_REMOVE_CIG_CNF                   (BT_MODULE_GAP | 0x0047)    /**< Remove CIG confirmation with #bt_gap_le_remove_cig_cnf_t. */
#define BT_GAP_LE_CREATE_BIG_CNF                   (BT_MODULE_GAP | 0x0048)    /**< Create BIG confirmation with NULL payload */
#define BT_GAP_LE_TERMINATE_BIG_CNF                (BT_MODULE_GAP | 0x0049)    /**< Terminate BIG confirmation with NULL payload */
#define BT_GAP_LE_BIG_CREATE_SYNC_CNF              (BT_MODULE_GAP | 0x004a)    /**< Create BIG sync confirmation with NULL payload */
#define BT_GAP_LE_BIG_TERMINATE_SYNC_CNF           (BT_MODULE_GAP | 0x004b)    /**< Terminate BIG sync confirmation with NULL payload */
#define BT_GAP_LE_BIG_ESTABLISHED_IND              (BT_MODULE_GAP | 0x004c)    /**< BIG established indication with #bt_gap_le_big_established_ind_t */
#define BT_GAP_LE_BIG_TERMINATED_IND               (BT_MODULE_GAP | 0x004d)    /**< BIG terminated indication with #bt_gap_le_big_terminated_ind_t */
#define BT_GAP_LE_BIG_SYNC_ESTABLISHED_IND         (BT_MODULE_GAP | 0x004e)    /**< BIG sync established indication with #bt_gap_le_big_sync_established_ind_t*/
#define BT_GAP_LE_BIG_SYNC_LOST_IND                (BT_MODULE_GAP | 0x004f)    /**< BIG sync lost indication with #bt_gap_le_big_sync_lost_ind_t */
#define BT_GAP_LE_BIGINFO_ADV_REPORT_IND           (BT_MODULE_GAP | 0x0050)    /**< BIG info advertising report indication with #bt_gap_le_biginfo_advertising_report_t */
#define BT_GAP_LE_SET_CIG_PARAMETERS_TEST_CNF      (BT_MODULE_GAP | 0x005a)    /**< Set CIG parameters confirmation with #bt_gap_le_set_cig_params_test_cnf_t. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_IND           (BT_MODULE_GAP | 0x005b)    /**< Receive periodic advertising transfer with #bt_gap_le_periodic_advertising_sync_transfer_received_ind_t. */
#define BT_GAP_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_CNF               (BT_MODULE_GAP | 0x005c)    /**< Set periodic advertising receive enable with NULL payload. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_CNF                    (BT_MODULE_GAP | 0x005d)    /**< Set periodic advertising sync transfer with #bt_gap_le_periodic_advertising_sync_transfer_t. */
#define BT_GAP_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_CNF                (BT_MODULE_GAP | 0x005e)    /**< Periodic advertisin set info transfer with #bt_gap_le_periodic_advertising_set_info_transfer_t. */
#define BT_GAP_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_CNF     (BT_MODULE_GAP | 0x005f)    /**< Set periodic advertisin sync transfer parameters with #bt_gap_le_set_periodic_advertising_sync_transfer_parameters_t. */

#define BT_GAP_LE_CA_251_TO_500_PPM    0x00    /**< 251 ppm to 500 ppm. */
#define BT_GAP_LE_CA_151_TO_250_PPM    0x01    /**< 151 ppm to 250 ppm. */
#define BT_GAP_LE_CA_101_TO_150_PPM    0x02    /**< 101 ppm to 150 ppm. */
#define BT_GAP_LE_CA_76_TO_100_PPM     0x03    /**< 76 ppm to 100 ppm. */
#define BT_GAP_LE_CA_51_TO_75_PPM      0x04    /**< 51 ppm to 75 ppm. */
#define BT_GAP_LE_CA_31_TO_50_PPM      0x05    /**< 31 ppm to 50 ppm. */
#define BT_GAP_LE_CA_21_TO_30_PPM      0x06    /**< 21 ppm to 30 ppm. */
#define BT_GAP_LE_CA_0_TO_20_PPM       0x07    /**< 0 ppm to 20 ppm. */
typedef uint8_t bt_gap_le_clock_accuracy_t;         /**< Sleep clock accuracy modes. */

#define BT_GAP_LE_CIG_PACKING_SEQUENTIAL            0x00    /**< The subevents are arranged in sequential arrangement. */
#define BT_GAP_LE_CIG_PACKING_INTERLEAVED           0x01    /**< The subevents are arranged in interleaved arrangement. */
typedef uint8_t bt_gap_le_cig_packing_t;            /**< Method of arranging subevents of multiple CISes. */

#define BT_GAP_LE_CIG_FRAMING_UNFRAMED              0x00    /**< The CIS Data PDUs is unframed. */
#define BT_GAP_LE_CIG_FRAMING_FRAMED                0x01    /**< The CIS Data PDUs is framed. */
typedef uint8_t bt_gap_le_cig_framing_t;            /**< Format of CIS Data PDUs. */

#define BT_GAP_LE_ISO_DATA_PATH_DIRECTION_INPUT     0x00    /**< Input. */
#define BT_GAP_LE_ISO_DATA_PATH_DIRECTION_OUTPUT    0x01    /**< Output. */
typedef uint8_t bt_gap_le_iso_data_path_direction_t;            /**< ISO data path direction. */

#define BT_GAP_LE_ISO_DATA_PATH_ID_HCI              0x00    /**< HCI. */
#define BT_GAP_LE_ISO_DATA_PATH_ID_DISABLE          0xFF    /**< Disable. */
typedef uint8_t bt_gap_le_iso_data_path_id_t;               /**< ISO data path ID. */

#define BT_GAP_LE_REPLY_CIS_ACTION_ACCEPT           0x00    /**< Action accept. */
#define BT_GAP_LE_REPLY_CIS_ACTION_REJECT           0x01    /**< Action reject. */
typedef uint8_t bt_gap_le_reply_cis_action_t;               /**< Reply CIS action. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_gap_leaudio_struct Struct
 * @{
 */

typedef bt_hci_evt_disconnect_complete_t bt_gap_le_cis_terminate_ind_t;  /**< This structure defines the associated parameter type in the callback for #BT_GAP_LE_CIS_TERMINATED_IND event. */

/**
 *  @brief      Reply CIS Request command.
 */
typedef struct {
    bt_gap_le_reply_cis_action_t action;            /**< The action for replying CIS request. */
    union {
        bt_hci_le_accept_cis_request_params_t accept;   /**< The accept parameters. */
        bt_hci_le_reject_cis_request_params_t reject;   /**< The reject parameters. */
    };
} bt_gap_le_reply_cis_request_t;

/**
 *  @brief      LE Setup ISO data path command.
 */
BT_PACKED(
typedef struct {
    bt_handle_t handle;                                 /**< Connection handle of the CIS or BIS. Range: 0x0000 to 0x0EFF */
    bt_gap_le_iso_data_path_direction_t direction;      /**< The direction for which the data path is being configured. */
    bt_gap_le_iso_data_path_id_t data_path_id;          /**< The data transport path used. When set to a value in the range 0x01 to 0xFE, the data path shall use a vendor-specific transport interface */
    uint8_t codec_format;                               /**< The coding format used over the air.*/
    uint16_t company_id;                                /**< Company ID, see Assigned Numbers for Company Identifier. Shall be ignored if coding format is not 0xFF.*/
    uint16_t vendor_codec_id;                           /**< Vendor-defined codec ID. Shall be ignored if octet 0 is not 0xFF.*/
    uint32_t controller_delay : 24;                     /**< Controller delay in microseconds Range: 0x000000 to 0x3D0900 Time range: 0 s to 4 s */
    uint8_t codec_configuration_length;                 /**< Length of codec configuration. */
    uint8_t *codec_configuration;                       /**< Codec-specific configuration data. */
}) bt_gap_le_setup_iso_data_path_t;

/**
 *  @brief      LE Remove ISO data path command.
 */
typedef bt_hci_le_remove_iso_data_path_params_t bt_gap_le_remove_iso_data_path_t;

/**
 *  @brief      LE Setup CIG command CIS parameters.
 */
BT_PACKED(
typedef struct {
    uint8_t cis_id;             /**< identifies a CIS and is set by the master's Host and passed to the slave's Host through the Link Layers during the process of establishing a CIS. */
    uint16_t max_sdu_m_to_s;    /**< The maximum size of an SDU from the master's Host. */
    uint16_t max_sdu_s_to_m;    /**< The maximum size of an SDU from the slave's Host. */
    uint8_t phy_m_to_s;         /**< identifies which PHY to use for transmission from the master to the slave. */
    uint8_t phy_s_to_m;         /**< identifies which PHY to use for transmission from the slave to the master. */
    uint8_t rtn_m_to_s;         /**< The maximum number of times that every CIS Data PDU should be retransmitted from the master to slave before being acknowledged. */
    uint8_t rtn_s_to_m;         /**< The maximum number of times that every CIS Data PDU should be retransmitted from the slave to master before being acknowledged. */
}) bt_gap_le_cis_params_t;

/**
 *  @brief      LE Setup CIG parameters command.
 */
BT_PACKED(
typedef struct {
    uint32_t cig_id : 8;                                /**< identifies a CIG. This parameter is allocated by the master's Host and passed to the slave's Host through the Link Layers during the process of establishing a CIS. */
    uint32_t sdu_interval_m_to_s : 24;                  /**< The interval, in microseconds, of periodic SDUs. (0x000FF to 0xFFFFF) */
    uint32_t sdu_interval_s_to_m : 24;                  /**< The interval, in microseconds, of periodic SDUs. (0x000FF to 0xFFFFF) */
    uint32_t sca : 8;                                   /**< The sleep clock accuracy of all the slaves that will participate in the CIG. */
    bt_gap_le_cig_packing_t packing;                    /**< The preferred method of arranging subevents of multiple CISes. */
    bt_gap_le_cig_framing_t framing;                    /**< The format of CIS Data PDUs for all the CISes. */
    uint16_t max_transport_latency_m_to_s;              /**< Maximum time, in milliseconds, for an SDU to be transported from the master Controller to slave Controller. */
    uint16_t max_transport_latency_s_to_m;              /**< Maximum time, in milliseconds, for an SDU to be transported from the slave Controller to master Controller. */
    uint8_t cis_count;                                  /**< Total number of CISes in the CIG being added or modified. The maximum value is limited to 16. */
    bt_gap_le_cis_params_t *cis_list;                   /**< The parameters of each CIS in the CIG. */
}) bt_gap_le_set_cig_params_t;

/**
 *  @brief      LE Setup CIS parameters test command.
 */
BT_PACKED(
typedef struct {
    uint8_t cis_id;             /**< identifies a CIS and is set by the master's Host and passed to the slave's Host through the Link Layers during the process of establishing a CIS. */
    uint8_t nse;                /**< The maximum number of subevents in each CIS event. */
    uint16_t max_sdu_m_to_s;    /**< The maximum size of an SDU from the master's Host. */
    uint16_t max_sdu_s_to_m;    /**< The maximum size of an SDU from the slave's Host. */
    uint16_t max_pdu_m_to_s;    /**< The maximum size, in octets, of the payload from master Link Layer to slave Link Layer. */
    uint16_t max_pdu_s_to_m;    /**< The maximum size, in octets, of the payload from slave Link Layer to master Link Layer. */
    uint8_t phy_m_to_s;         /**< identifies which PHY to use for transmission from the master to the slave. */
    uint8_t phy_s_to_m;         /**< identifies which PHY to use for transmission from the slave to the master. */
    uint8_t bn_m_to_s;          /**< The burst number for master to slave. */
    uint8_t bn_s_to_m;          /**< The burst number for slave to master. */
}) bt_gap_le_cis_params_test_t;

/**
 *  @brief      LE Setup CIG test command CIS parameters.
 */
BT_PACKED(
typedef struct {
    uint32_t cig_id : 8;                                /**< identifies a CIG. This parameter is allocated by the master's Host and passed to the slave's Host through the Link Layers during the process of establishing a CIS. */
    uint32_t sdu_interval_m_to_s : 24;                  /**< The interval, in microseconds, of periodic SDUs. (0x000FF to 0xFFFFF) */
    uint32_t sdu_interval_s_to_m : 24;                  /**< The interval, in microseconds, of periodic SDUs. (0x000FF to 0xFFFFF) */
    uint32_t ft_m_to_s : 8;                             /**< The flush timeout in multiples of ISO_Interval for each payload sent from the master to slave. (0x01 to 0xFF) */
    uint32_t ft_s_to_m : 8;                             /**< The flush timeout in multiples of ISO_Interval for each payload sent from the slave to master. (0x01 to 0xFF) */
    uint32_t iso_interval : 16;                         /**< Time between consecutive CIS anchor points. (0x0004 to 0x0C80) Time = N * 1.25 ms (5 ms to 4 s) */
    uint32_t sca : 8;                                   /**< The worst-case sleep clock accuracy of all the slaves that will participate in the CIG. */
    uint8_t packing;                                    /**< The preferred method of arranging subevents of multiple CISes. */
    uint8_t framing;                                    /**< The format of CIS Data PDUs for all the CISes. */
    uint8_t cis_count;                                  /**< Total number of CISes in the CIG being added or modified. The maximum value is limited to 16. */
    bt_gap_le_cis_params_test_t *cis_list;              /**< The test parameters of each CIS in the CIG. */
}) bt_gap_le_set_cig_params_test_t;

/**
 *  @brief      CIS set parameters.
 */
BT_PACKED(
typedef struct {
    bt_handle_t cis_connection_handle;                  /**< The connection handle of the CIS to be established. */
    bt_handle_t acl_connection_handle;                  /**< The connection handle of the ACL connection associated with each CIS to be established. */
}) bt_gap_le_cis_set_t;

/**
 *  @brief      LE create CIS command.
 */
typedef struct {
    uint8_t cis_count;                                  /**< The total number of CISes established by this command. */
    bt_gap_le_cis_set_t *cis_list;                      /**< The connection handle list of the CIS to be established. */
} bt_gap_le_create_cis_t;

/**
 *  @brief      LE remove CIG command.
 */
BT_PACKED(
typedef struct {
    uint8_t cig_id;    /**< identifies a CIG. */
}) bt_gap_le_remove_cig_t;

/**
 * @brief       LE BIG create sync.
 */
BT_PACKED(
typedef struct {
    uint8_t         big_handle;                         /**< The handle of BIG. */
    uint16_t        sync_handle;                        /**< The advertising handle. */
    uint8_t         encryption;                         /**< identifies the encryption mode of the BIG. */
    bt_key_t        broadcast_code;                     /**< a 16-octet field that is used to generate the Session key to encrypt or decrypt payloads of an encrypted BIS. */
    uint8_t         maximum_subevents;                  /**< The maximum number of subevents that a Controller should use to receive data payloads in each interval for a BIS. */
    uint16_t        big_sync_timeout;                   /**< The maximum permitted time between successful receptions of BIS PDUs. */
    uint8_t         num_of_bis;                         /**< The number of BISes indices specified in the BIS_list parameter. */
    uint8_t         *bis_list;                          /**< a list of indices corresponding to BISes in the synchronized BIG. */
}) bt_gap_le_big_create_sync_t;

/**
 * @brief       LE CIS established event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                           /**< The status of the CIS. */
    bt_handle_t       connection_handle;                /**< Connection handle of the CIS. */
    uint32_t          cig_sync_delay : 24;              /**< The maximum time, in microseconds, for transmission of PDUs of all CISes in a CIG in an isochronous interval. */
    uint32_t          cis_sync_delay : 24;              /**< The maximum time, in microseconds, for transmission of PDUs of a CIS in an isochronous interval. */
    uint32_t          transport_latency_m_to_s : 24;    /**< The maximum time, in microseconds, for transmission of SDUs of all CISes in a CIG from slave to master. */
    uint32_t          transport_latency_s_to_m : 24;    /**< The maximum time, in microseconds, for transmission of SDUs of all CISes in a CIG from master to slave. */
    uint8_t           phy_m_to_s;                       /**< The transmitter PHY of packets used from the slave to master. */
    uint8_t           phy_s_to_m;                       /**< The transmitter PHY of packets used from the slave to master. */
}) bt_gap_le_cis_established_ind_t;

/**
 * @brief       LE CIS terminated event.
 */
typedef bt_hci_evt_disconnect_complete_t bt_gap_le_cis_terminated_ind_t;

/**
 * @brief       LE CIS request event.
 */
BT_PACKED(
typedef struct {
    bt_handle_t       acl_connection_handle;            /**< Connection handle of the ACL. */
    bt_handle_t       cis_connection_handle;            /**< Connection handle of the CIS. */
    uint8_t           cig_id;                           /**< CIG ID. */
    uint8_t           cis_id;                           /**< CIS ID. */
}) bt_gap_le_cis_request_ind_t;

/**
 * @brief       LE create BIG complete event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                           /**< The status of the BIG. */
    uint8_t           big_handle;                       /**< The identifier of a BIG. (Range: 0x01 to 0xEF)*/
    uint32_t          big_sync_delay : 24;              /**< The maximum time in microseconds for transmission of PDUs of all BISes in a BIG event. (Range: 0x0000EA to 0x7FFFFF) */
    uint32_t          transport_latency_big : 24;       /**< The maximum delay time, in microseconds, for transmission of SDUs of all BISes in a BIG event. (Range: 0x0000EA to 0x7FFFFF) */
    uint8_t           phy;                              /**< The PHY used to create the BIG. */
    uint8_t           nse;                              /**< The number of subevents in each BIS event in the BIG. (Range: 0x01 to 0x1E)*/
    uint8_t           bn;                               /**< The number of new payloads in each BIS event. (Range: 0x01 to 0x07)*/
    uint8_t           pto;                              /**< Offset used for pre-transmissions. (Range: 0x01 to 0x0F) */
    uint8_t           irc;                              /**< The number of times a payload is transmitted in a BIS event. (Range: 0x01 to 0x0F)*/
    uint16_t          max_pdu;                          /**< Maximum size, in octets, of the payload. (Range: 0x0000 to 0x00FB)*/
    uint16_t          iso_interval;                     /**< The time between two consecutive BIG anchor points. (Range: 0x0004 to 0x0C80, Time = N * 1.25 ms, Time Range: 5 ms to 4 s.)*/
    uint8_t           num_bis;                          /**< Total number of BISes in the BIG. (Range: 0x01 to 0x0F)*/
    bt_handle_t       connection_handle_list[0];        /**< The connection handles of the BISes in the BIG.*/
}) bt_gap_le_big_established_ind_t;

/**
 * @brief       LE terminate BIG complete event.
 */
BT_PACKED(
typedef struct {
    uint8_t           big_handle;                       /**< The identifier of a BIG. (Range: 0x01 to 0xEF)*/
    bt_hci_disconnect_reason_t  reason;                 /**< The reason for BIG termination. */
}) bt_gap_le_big_terminated_ind_t;

/**
 * @brief       LE BIG sync established event.
 */
BT_PACKED(
typedef struct {
    uint8_t           status;                           /**< The status of the BIG sync. */
    uint8_t           big_handle;                       /**< The identifier of a BIG. (Range: 0x01 to 0xEF)*/
    uint32_t          transport_latency_big : 24;       /**< The maximum delay time, in microseconds, for transmission of SDUs of all BISes in a BIG event. (Range: 0x0000EA to 0x7FFFFF) */
    uint32_t          nse : 8;                          /**< The number of subevents in each BIS event in the BIG. (Range: 0x01 to 0x1E)*/
    uint8_t           bn;                               /**< The number of new payloads in each BIS event. (Range: 0x01 to 0x07)*/
    uint8_t           pto;                              /**< Offset used for pre-transmissions. (Range: 0x01 to 0x0F) */
    uint8_t           irc;                              /**< The number of times a payload is transmitted in a BIS event. (Range: 0x01 to 0x0F)*/
    uint16_t          max_pdu;                          /**< Maximum size, in octets, of the payload. (Range: 0x0000 to 0x00FB)*/
    uint16_t          iso_interval;                     /**< The time between two consecutive BIG anchor points. (Range: 0x0004 to 0x0C80, Time = N * 1.25 ms, Time Range: 5 ms to 4 s.)*/
    uint8_t           num_bis;                          /**< Total number of BISes in the BIG. (Range: 0x01 to 0x0F)*/
    bt_handle_t       connection_handle_list[0];        /**< The connection handles of the BISes in the BIG.*/
}) bt_gap_le_big_sync_established_ind_t;

/**
 * @brief       LE BIG sync lost event.
 */
BT_PACKED(
typedef struct {
    uint8_t           big_handle;                       /**< The identifier of a BIG. */
    bt_hci_disconnect_reason_t  reason;                 /**< The reason that synchronization to BIG is terminated. */
}) bt_gap_le_big_sync_lost_ind_t;

/**
 *  @brief      LE BIGInfo advertising report event.
 */
BT_PACKED(
typedef struct {
    uint16_t          sync_handle;                      /**< Sync handle */
    uint8_t           num_bis;                          /**< Total number of BISes in the BIG. (Range: 0x01 to 0x0F)*/
    uint8_t           nse;                              /**< The number of subevents in each BIS event in the BIG. (Range: 0x01 to 0x1E)*/
    uint16_t          iso_interval;                     /**< The time between two consecutive BIG anchor points. (Range: 0x0004 to 0x0C80, Time = N * 1.25 ms, Time Range: 5 ms to 4 s.)*/
    uint8_t           bn;                               /**< The number of new payloads in each BIS event. (Range: 0x01 to 0x07)*/
    uint8_t           pto;                              /**< Offset used for pre-transmissions. (Range: 0x01 to 0x0F) */
    uint8_t           irc;                              /**< The number of times a payload is transmitted in a BIS event. (Range: 0x01 to 0x0F)*/
    uint16_t          max_pdu;                          /**< Maximum size, in octets, of the payload. (Range: 0x0000 to 0x00FB)*/
    uint32_t          sdu_interval : 24;                /**< Value of the SDU_Interval subfield of the BIGInfo field. (Range: 0x0000FF to 0x0FFFFF)*/
    uint16_t          max_sdu;                          /**< Value of the Max_SDU subfield of the BIGInfo field in the Advertising PDU *(Range: 0x0000 to 0x0FFF)*/
    uint8_t           phy;                              /**< The PHY used to create the BIG. */
    bt_gap_le_cig_framing_t framing;                    /**< Identifies the format for sending BIS Data PDUs. */
    uint8_t           encryption;                       /**< Identifies the encryption mode of the BISes. */
}) bt_gap_le_biginfo_advertising_report_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_TRANSFERT_RECEIVED_IND event.
 *         Periodic advertising transfer received indication, an event sent to indicate there is information from the device referred to by the Connection_Handle parameter
           and either successfully synchronized to the periodic advertising train or timed out while attempting to synchronize..
 */
BT_PACKED(
typedef struct {
    uint8_t status;
    bt_handle_t handle;
    uint16_t service_data;
    bt_handle_t sync_handle;
    uint8_t advertising_sid;
    bt_addr_type_t adv_address_type;
    bt_bd_addr_t adv_address;
    bt_hci_le_adv_phy_t adv_phy;
    uint16_t periodic_advertising_interval;
    bt_gap_le_clock_accuracy_t advertiser_clock_accuracy;
}) bt_gap_le_periodic_advertising_sync_transfer_received_ind_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_SETUP_ISO_DATA_PATH_CNF event.
 */
typedef bt_hci_le_setup_iso_data_path_t bt_gap_le_setup_iso_data_path_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_REMOVE_ISO_DATA_PATH_CNF event.
 */
typedef bt_hci_le_remove_iso_data_path_t bt_gap_le_remove_iso_data_path_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_SET_CIG_PARAMETERS_CNF event.
 */
typedef bt_hci_le_set_cig_params_t bt_gap_le_set_cig_params_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_SET_CIG_PARAMETERS_TEST_CNF event.
 */
typedef bt_hci_le_set_cig_params_t bt_gap_le_set_cig_params_test_cnf_t;

/**
 * @brief This structure defines the associated parameter type in the callback for #BT_GAP_LE_REMOVE_CIG_CNF event.
 */
typedef bt_hci_le_remove_cig_t bt_gap_le_remove_cig_cnf_t;

/**
 *  @brief  This structure defines the associated parameter for #BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_CNF.
 */
BT_PACKED(
typedef struct {
    bt_hci_status_t   status;            /**< Status */
    bt_handle_t       handle;            /**< Connection handle */
}) bt_gap_le_periodic_advertising_sync_transfer_t;

/**
 *  @brief  This structure defines the associated parameter for #BT_GAP_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_CNF.
 */
typedef bt_gap_le_periodic_advertising_sync_transfer_t bt_gap_le_periodic_advertising_set_info_transfer_t;

/**
 *  @brief  This structure defines the associated parameter for #BT_GAP_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_CNF.
 */
typedef bt_gap_le_periodic_advertising_sync_transfer_t bt_gap_le_set_periodic_advertising_sync_transfer_parameters_t;

/**
 * @}
 */

/**
 * @brief     This function replies to the received CIS request.
 *            The application receives the #BT_GAP_LE_CIS_ESTABLISHED_IND event when the CIS connection is established.
 * @param[in] params      is a pointer to the reply paramters
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_reply_cis_request(
    const bt_gap_le_reply_cis_request_t *params);

/**
 * @brief     This function sets up the ISO data path.
 *            The application receives the #BT_GAP_LE_SETUP_ISO_DATA_PATH_CNF event after the command is complete.
 * @param[in] params      is a pointer to the setup paramters
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_setup_iso_data_path(
    const bt_gap_le_setup_iso_data_path_t *params);

/**
 * @brief     This function removes ISO data path.
 *            The application receives the #BT_GAP_LE_REMOVE_ISO_DATA_PATH_CNF event after the command is complete.
 * @param[in] params      is a pointer to the remove paramters
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_remove_iso_data_path(
    const bt_gap_le_remove_iso_data_path_t *params);

/**
 * @brief     This function sets the CIG parameters.
 *            The application receives the #BT_GAP_LE_SET_CIG_PARAMETERS_CNF event after the command is complete.
 * @param[in] params      is a pointer to the CIG paramters
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_set_cig_parameters(
    const bt_gap_le_set_cig_params_t *params);

/**
 * @brief     This function set CIG test parameters.
 *            The application receives the #BT_GAP_LE_SET_CIG_PARAMETERS_TEST_CNF event after the command is complete.
 * @param[in] params      is a pointer to the CIG test paramters
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_set_cig_parameters_test(
    const bt_gap_le_set_cig_params_test_t *params);

/**
 * @brief     This function creates the CIS connection. The application receives the #BT_GAP_LE_CIS_CONNECTION_CNF event after the create connection command is complete.
 *            Then the application receives the #BT_GAP_LE_CIS_ESTABLISHED_IND event after CIS connection established.
 * @param[in] params      is a pointer to the paramters for CIS
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_create_cis(
    const bt_gap_le_create_cis_t *params);

/**
 * @brief     This function creates a BIG with one or more BISes. The application receives the #BT_GAP_LE_CREATE_BIG_CNF event after the create BIG command is complete.
 *            Then the application receives the #BT_GAP_LE_BIG_ESTABLISHED_IND event after the BIG is created.
 * @param[in] params      is a pointer to the paramters for creating a BIG
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_create_big(
    const bt_hci_le_create_big_t *params);

/**
 * @brief     This function terminates a BIG. The application receives the #BT_GAP_LE_TERMINATE_BIG_CNF event after the terminate BIG command is complete.
 *            Then the application receives the #BT_GAP_LE_BIG_TERMINATED_IND event after the transmission of all the BISes in the BIG are terminated.
 * @param[in] params      is a pointer to the paramters for terminating a BIG
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_terminate_big(
    const bt_hci_le_terminate_big_t *params);

/**
 * @brief     This function starts to synchronize to a BIG described in the specified periodic advertising train.
              The application receives the #BT_GAP_LE_BIG_CREATE_SYNC_CNF event after the create BIG sync command is complete.
 *            Then the application receives the #BT_GAP_LE_BIG_SYNC_ESTABLISHED_IND event after the BIG sync is established.
 * @param[in] params      is a pointer to the paramters for starting a BIG sync
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_big_create_sync(
    const bt_gap_le_big_create_sync_t *params);

/**
 * @brief     This function terminates the synchronization to a BIG described in the specified periodic advertising train.
              The application receives the #BT_GAP_LE_BIG_TERMINATE_SYNC_CNF event after the terminate BIG sync command is complete.
 *            Then the application receives the #BT_GAP_LE_BIG_SYNC_LOST_IND event after the BIG sync is lost.
 * @param[in] params      is a pointer to the paramters for terminating a BIG sync
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_big_terminate_sync(
    const bt_hci_le_big_terminate_sync_t *params);


/**
 * @brief     This function enables or disables reports for the periodic advertising train identified by the Sync_Handle parameter.
              The application receives the #BT_GAP_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_CNF event after the command is complete.
 * @param[in] params      is a pointer to the paramters for setting periodic advertising receive enable.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_periodic_advertising_set_receive_enable(
    const bt_hci_le_periodic_advrtising_receive_enable_t *params);
/**
 * @brief     This function instructs the Controller to send synchronization information about the periodic advertising train
              identified by the Sync_Handle parameter to a connected device.
              The application receives the #BT_GAP_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_CNF event after the sync transfer command is complete.
 * @param[in] params      is a pointer to the paramters for periodic advertising sync transfer.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_periodic_advertising_sync_transfer(
    const bt_hci_le_periodic_advrtising_sync_transfer_t *params);

/**
 * @brief     This function instructs the Controller to send synchronization information about the periodic advertising in an advertising set to a connected device.
              The application receives the #BT_GAP_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_CNF event after the set info transfer command is complete.
 * @param[in] params      is a pointer to the paramters for periodic advertising set info transfer.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_periodic_advertising_set_info_transfer(
    const bt_hci_le_periodic_advrtising_set_info_transfer_t *params);

/**
 * @brief     This function specify how the Controller will process periodic advertising synchronization information received from the device identified by the
              Connection_Handle parameter.
              The application receives the #BT_GAP_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_CNF event after the set info transfer command is complete.
 * @param[in] params      is a pointer to the paramters for setting periodic advertising transfer.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_UNSUPPORTED, the operation not supported on this version.
 */
bt_status_t bt_gap_le_set_periodic_advertising_sync_transfer_parameters(
    const bt_hci_le_set_periodic_advrtising_sync_transfer_params_t *params);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif
#endif

