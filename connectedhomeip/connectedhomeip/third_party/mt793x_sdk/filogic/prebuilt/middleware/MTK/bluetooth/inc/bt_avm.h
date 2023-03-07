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

#ifndef __BT_AVM_H__
#define __BT_AVM_H__

#include "bt_platform.h"
#include "bt_system.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN

#define BT_AVM_TYPE_CALL (0x00)
#define BT_AVM_TYPE_A2DP (0x01)

typedef uint8_t bt_avm_audio_type_t;  /**< Define the bt audio type. */

#define BT_AVM_ROLE_NORMAL  (0x00)
#define BT_AVM_ROLE_AWS_MCE_AGENT (0x01)
#define BT_AVM_ROLE_AWS_MCE_PARTNER (0x02)
#define BT_AVM_ROLE_AWS_MCE_CLINET  (0x03)
typedef uint8_t bt_avm_role_t;  /**< Define the bt audio type. */

#define BT_AVM_CIRTICAL_DATA_TX_RESULT   (0x00) /**< Param ref #bt_avm_critial_tx_result_t */
#define BT_AVM_CIRTICAL_DATA_RX_IND      (0x01) /**< Param ref #bt_avm_critial_data_t */
typedef uint8_t bt_avm_critial_data_event_t;  /**< Define the ciritial data callback event type. */

/**
 * @brief The event report to user
 */
#define BT_AVM_DECODE_NOTIFICATION_IND    (BT_MODULE_AVM | 0x0000)    /**< A  
 * notification that DSP should start decode media data, with #bt_avm_decode_notify_ind_t as the payload in the callback function. */
#define BT_AVM_MEDIA_DATA_RECEIVED_IND    (BT_MODULE_AVM | 0x0001)   /**< A 
 * notification that the media packet is received, with #bt_avm_a2dp_media_info_t as the payload in the callback function. */
#define BT_AVM_SET_LOCAL_ASI_FLAG     (BT_MODULE_AVM | 0x0002)   /**< A 
 * notification that the reinitial sync request is received, with #bt_avm_a2dp_resync_ind_t as the payload in the callback function. */
#define BT_AVM_SEQUENCE_NUM_WRAP_COUNT_IND    (BT_MODULE_AVM | 0x0004)   /**< A 
 * notification that sequence number wrap happens, with #bt_avm_a2dp_sn_wrap_count_t as the payload in the callback function. */

/**
 * @brief    Bluetooth share buffer set to controller module, this structure is
 */
typedef struct {
    uint32_t a2dp_address;      /**< A2DP media packet buffering pointer.*/
    uint32_t sco_up_address;    /**< eSCO/SCO up-link packet buffering pointer.*/
    uint32_t sco_dl_address;    /**< eSCO/SCO down-link packet buffering pointer.*/
    uint32_t clock_mapping_address; /**< Audio clock and BT clock mapping buffer pointer. */
} bt_avm_share_buffer_info_t;

/**
 * @brief    Bluetooth share buffer set to controller module, this structure is
 */
typedef struct {
    uint32_t music_dl_address;      /**< music down-link media packet buffering pointer.*/
    uint32_t voice_up_address;      /**< voice up-link media packet buffering pointer.*/
    uint32_t sco_up_address;        /**< eSCO/SCO up-link packet buffering pointer.*/
    uint32_t sco_dl_address;        /**< eSCO/SCO down-link packet buffering pointer.*/
    uint32_t clock_mapping_address; /**< Audio clock and BT clock mapping buffer pointer. */
    uint32_t reserve;               /**< Reserved for future.
                                                                 bit0: 1 means ull_server (bt dongle), 0 means ull_client.
                                                                 bit1~bit2:0 means headset client, 1 means earbuds client.
                                                                 bit16~bit23:the length of each downlink packet.
                                                                 bit24~bit31:the length of each uplink packet.*/
} bt_avm_ext_share_buffer_info_t;

typedef struct {
    uint32_t ul_address;    /**< LE audio up-link packet buffering pointer.*/
    uint32_t dl_address;    /**< LE audio down-link packet buffering pointer.*/
} bt_leaudio_buffer_set_t;

/**
 * @brief    Bluetooth LE audio buffer set to controller module, this structure is
 */
BT_PACKED(typedef struct {
    uint16_t count;
    bt_leaudio_buffer_set_t buffer[0];
}) bt_avm_leaudio_buffer_info_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_AVM_DECODE_NOTIFICATION_IND event..
 * @{
 */
typedef struct {
    uint32_t timestamp; /**< The timestamp for audio packet.*/
    bt_clock_t clock;   /**< The bt clock when audio packet is received.*/
    uint16_t sequence_num; /** < The sequence number for related audio packet. */
} bt_avm_a2dp_packet_info_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_AVM_DECODE_NOTIFICATION_IND event..
 * @{
 */
typedef struct {
    uint32_t handle;            /**< bt gap connection handle.*/
    bt_avm_a2dp_packet_info_t packet_info[2];   /**< The timestamp for first and second packet.*/
} bt_avm_decode_notify_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_AVM_MEDIA_DATA_RECEIVED_IND event..
 * @{
 */
typedef struct {
    uint32_t gap_handle;    /**< bt gap connection handle.*/
    uint32_t asi;           /**< the audio sample instant playing.*/
    bt_clock_t clock;       /**< the BT clock when the asi is playing>.*/
    uint32_t ratio;          /**< the ratio when the asi is playing>.*/
    uint32_t samples;       /**< the samples per a2dp packet when the asi is playing>.*/
} bt_avm_a2dp_media_info_t;


/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_AVM_MEDIA_DATA_RECEIVED_IND event..
 * @{
 */
typedef struct {
    uint32_t handle;    /**< bt gap connection handle.*/
    uint8_t local_asi_flag; /**< The flag which is used to indicate whether or not controller adopts local asi.*/
} bt_avm_a2dp_local_asi_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_AVM_SEQUENCE_NUM_WRAP_COUNT_IND event..
 * @{
 */
typedef struct {
    uint32_t gap_handle;    /**< bt gap connection handle.*/
    uint8_t wrap_count;           /**< the times that sn wrap happens.*/
} bt_avm_a2dp_sn_wrap_count_t;

typedef void (*critial_data_callback)(bt_avm_critial_data_event_t event, void *data);

/**
 * @brief    User critical data for transmission, this structure is
 */
typedef struct {
    uint8_t     seq;         /**< Sequence number for data transmission.*/
    uint8_t     length;      /**< The data length.*/
    uint8_t    *data;        /**< Raw data.*/
    bool        is_le_data; /**< ture:BLE type data false :EDR type data.*/
} bt_avm_critial_data_t;

/**
 * @brief    User critical data send result, this structure is
 */
typedef struct {
    uint8_t     status;      /**< Tx status. 0: success, 1: timeout, 2: abandon*/
    uint8_t     seq;         /**< Sequence number for data transmission.*/
} bt_avm_critial_tx_result_t;

/**
 * @}
 */



/**
 * @brief     This function sets buffer to controller after #BT_POWER_ON_CNF,
 *            which is accessed by host and controller used to update clock offset for each acl link.
 * @param[in] buffer is a buffer pointer for host and controller access.
 * @param[in] link_num is acl link num.
 * @return    #BT_STATUS_SUCCESS,  sets clock offset share buffer successfully.
 *            #BT_STATUS_FAIL, sets clock offset share buffer failed.
 */
bt_status_t bt_avm_set_clock_offset_share_buffer(uint8_t *buffer, uint8_t link_num);

/**
 * @brief     This function sets buffer to controller after #BT_POWER_ON_CNF, which is used by controler and DSP.
 * @param[in] buffer_info is a buffer set, including A2DP buffer, SCO/eSCO uplink buffer and down-link buffer.
 * @return    #BT_STATUS_SUCCESS,  sets share buffer successfully.
 *            #BT_STATUS_FAIL, sets share buffer failed.
 */
bt_status_t bt_avm_set_share_buffer(bt_avm_share_buffer_info_t *buffer_info);

/**
 * @brief     This function sets buffer to controller after #BT_POWER_ON_CNF, which is used by controller and DSP.
 * @param[in] buffer_info is a buffer set, including music uplink and downlink buffer, SCO/eSCO uplink buffer and down-link buffer.
 * @return    #BT_STATUS_SUCCESS,  sets share buffer successfully.
 *            #BT_STATUS_FAIL, sets share buffer failed.
 */
bt_status_t bt_avm_set_ext_share_buffer(bt_avm_ext_share_buffer_info_t *buffer_info);

/**
 * @brief     This function sets buffer to controller after #BT_POWER_ON_CNF, which is used by controller and DSP.
 * @param[in] buffer_info is a buffer set, including LE audio uplink buffer and down-link buffer.
 * @return    #BT_STATUS_SUCCESS,  sets share buffer successfully.
 *            #BT_STATUS_FAIL, sets share buffer failed.
 */
bt_status_t bt_avm_set_leaudio_buffer(bt_avm_leaudio_buffer_info_t *buffer_info);

/**
* @brief               This function sets the condition to controller where application will be notified if media packet condition is met.
* @param[in] condition is media packet node which needs be released after it is consumed.
* @return              #BT_STATUS_SUCCESS,  sets a2dp notify condition successfully.
*                      #BT_STATUS_FAIL, sets a2dp notify condition failed.
*/
bt_status_t bt_avm_set_a2dp_notify_condition(uint32_t gap_handle, uint32_t condition);

/**
* @brief               This function sets the Bluetooth clock to controller when the audio sys should be opened.
* @param[in] gap_handle is the gap connection handle for current link.
* @param[in] type       is the media type.
* @param[in] bt_clock   is the Bluetooth clock when the audio sys should be opened.
* @return              #BT_STATUS_SUCCESS,  sets audio tracking time successfully.
*                      #BT_STATUS_FAIL, sets audio tracking time failed.
*/
bt_status_t bt_avm_set_audio_tracking_time(uint32_t gap_handle, bt_avm_audio_type_t type, bt_clock_t *bt_clock);


/**
* @brief               This function get the pointer to the clock offset for specified link.
* @param[in] gap_handle is the gap connection handle for current link.
* @return              the pointer to corresponding buffer.
*                      NULL if the ACL link with the gap_handle is not existed.
*/
const void *bt_avm_get_clock_offset_address(uint32_t gap_handle);

/**
* @brief               This function tell the Controller that the host is ready
*                      to play a2dp, and ask it not to notify the media packet received.
* @param[in] bt_avm_a2dp_media_info_t is playing media information structured
*                      in #bt_avm_a2dp_media_info_t.
* @return              #BT_STATUS_SUCCESS,  sets audio tracking time successfully.
*                      #BT_STATUS_FAIL, sets audio tracking time failed.
*/
bt_status_t bt_avm_play_a2dp(bt_avm_a2dp_media_info_t *play_info);

/**
* @brief               This function set the music enabled flag to BT controller.
* @param[in] handle is the gap connection handle for Agent or Single headset, or the
*                      AWS connection handle for Partner and Client role.
* @param[in] role is the role type of the BT AVM.
* @param[in] enable is flag indicating the music status of enabled status.
* @return              #BT_STATUS_SUCCESS,  updates the slope successfully.
*                      #BT_STATUS_FAIL, updates the slope failed.
*/
bt_status_t bt_avm_set_music_enable(uint32_t handle, bt_avm_role_t role, bool enable);

/**
* @brief               This function set the ull music enabled flag to BT controller.
* @param[in] handle is the gap connection handle for Agent or Single headset, or the
*                      AWS connection handle for Partner and Client role.
* @param[in] role is the role type of the BT AVM.
* @param[in] enable is flag indicating the music status of enabled status.
* @param[in] dl_latency is indicating the music downlink latency to be set.
* @param[in] ul_latency is indicating the voice uplink latency to be set.
* @return              #BT_STATUS_SUCCESS,  updates the slope successfully.
*                      #BT_STATUS_FAIL, updates the slope failed.
*/
bt_status_t bt_avm_set_ull_gaming_mode_enable(uint32_t handle, bt_avm_role_t role, bool enable, uint16_t dl_latency, uint16_t ul_latency);

/**
* @brief               This function requests BT Controller to enable or disable
*                      the wideband scan on AWS_MCE partner side.
* @param[in] enable is the flag, false-disable, true-enable.
* @return              None.
*/
void bt_avm_set_wide_band_scan_flag(bool enable);


uint32_t bt_avm_get_gpt_count(bt_bd_addr_t addr);

void bt_avm_clear_gpt_count(bt_bd_addr_t addr);

void bt_avm_unlock_bt_sleep(void);

/**
* @brief               this function request enable flushable data send/receive
*                      Only can be enable once time.
* @param[in] max_length is maximum length user can be send.
* @param[in] cb is callback function.
* @return              #BT_STATUS_SUCCESS, init success.
*                      #BT_STATUS_FAIL, init failed.
*/
bt_status_t bt_avm_critical_data_init(uint8_t max_length, critial_data_callback cb);

/**
* @brief               this function send data to peer device with a flush timeout
*                      It's not reliable.
* @param[in] gap_handle is the gap connection handle for Agent or Single headset.
* @param[flush_timeout] is flush timeout. Suggestion > 20ms
                        Uint: ms
                        Range: 0 ~ 0xFFFF; 0: means no flush timeout
* @param[data] is the user data to be send.
* @return              #BT_STATUS_SUCCESS, send success.
*                      #BT_STATUS_FAIL, send failed.
*/
bt_status_t bt_avm_critical_data_send(uint32_t gap_handle, uint16_t flush_timeout, bt_avm_critial_data_t *data);


/**
* @brief            This function get the number of unsent BLE packets in the controller.
* @param[in] conn_handle  The connection handle of the BLE.
* @return                 The number of unsent BLE packets.
*/
uint16_t bt_avm_get_acl_queue_entries(bt_handle_t conn_handle);

/**
 * @brief     Tis function sets dongle mode to controller.
 * @param[in] mode is indicating the dongle mode.
 * @return    #BT_STATUS_SUCCESS, set dongle mode successfully.
 *                #BT_STATUS_FAIL,set dongle mode failed.
 */
bt_status_t bt_avm_set_dongle_mode(uint8_t mode);

BT_EXTERN_C_END

#endif


