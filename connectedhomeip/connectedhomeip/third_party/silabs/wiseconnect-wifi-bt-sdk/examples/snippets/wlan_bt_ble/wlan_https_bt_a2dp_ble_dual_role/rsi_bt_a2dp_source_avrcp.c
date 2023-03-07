/*******************************************************************************
* @file  rsi_bt_a2dp_source_avrcp_DEMO_57.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file    rsi_bt_a2dp_source_avrcp_DEMO_57.c
 * @version 0.1
 * @date    01 Feb 2020
 *
 *
 *
 *  @brief : This file contains example application for BT Classic A2DP Source and AVRCP target
 *
 *  @section Description  This application serves as BT Classic A2DP Source and AVRCP target
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

#include <rsi_common_app.h>
//#if COEX_MAX_APP
//! BT include file to refer BT APIs
#include <rsi_driver.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_apis.h>
#include <rsi_bt.h>
#include <stdio.h>
#ifdef FRDM_K28F
#include "fsl_lptmr.h"
#endif
#ifdef RSI_M4_INTERFACE
#include <rsi_board.h>
#endif
#if (RSI_AUDIO_DATA_SRC == ARRAY)
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
#include "pcm_data_buff.h"
#endif
#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
//! SD card include files
#include "ff.h"
#include "diskio.h"
#include "fsl_debug_console.h"
#endif

#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if INQUIRY_ENABLE
static uint8_t inq_responses_count;
uint8_t rsi_inq_resp_list[MAX_NO_OF_RESPONSES][RSI_DEV_ADDR_LEN] = { 0 };
uint8_t rsi_inq_resp_list_name_length[MAX_NAME_LENGTH]           = { 0 };
static uint8_t inq_resp_name_length_index;
#endif
#endif
#include "rsi_bt_sbc_codec.h"
#include "rsi_bt_config.h"
#include "rsi_common_config.h"
/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define RSI_DID_SPEC_ID          0x0200
#define RSI_DID_VENDOR_ID        0x0201
#define RSI_DID_PRODUCT_ID       0x0202
#define RSI_DID_VERSION          0x0203
#define RSI_DID_PRIMARY_RECOED   0x0001
#define RSI_DID_VENDOR_ID_SOURCE 0x0002

#define BT_AVRCP_MEDIA_ATTR_ID_MAX_COUNT 0x01

#define BT_AVRCP_MEDIA_ATTR_ID_MEDIA_TITLE     0x01
#define BT_AVRCP_MEDIA_ATTR_ID_ARTIST_NAME     0x02
#define BT_AVRCP_MEDIA_ATTR_ID_ALBUM_NAME      0x03
#define BT_AVRCP_MEDIA_ATTR_ID_TRACK_NUM       0x04
#define BT_AVRCP_MEDIA_ATTR_ID_NUM_OF_TRACK    0x05
#define BT_AVRCP_MEDIA_ATTR_ID_GENRE           0x06
#define BT_AVRCP_MEDIA_ATTR_ID_PLAYING_TIME_MS 0x07

#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED            0x01
#define AVRCP_EVENT_TRACK_CHANGED                      0x02
#define AVRCP_EVENT_TRACK_REACHED_END                  0x03
#define AVRCP_EVENT_TRACK_REACHED_START                0x04
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED               0x05
#define AVRCP_EVENT_BATT_STATUS_CHANGED                0x06
#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED              0x07
#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED 0x08
#define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED        0x09
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED          0x0a
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED           0x0b
#define AVRCP_EVENT_UIDS_CHANGED                       0x0c
#define AVRCP_EVENT_VOLUME_CHANGED                     0x0d

#define PLAYER_STATUS_STOP     0x00
#define PLAYER_STATUS_PLAY     0x01
#define PLAYER_STATUS_PAUSE    0x02
#define PLAYER_STATUS_FWD_SEEK 0x03
#define PLAYER_STATUS_REV_SEEK 0x04
#define PLAYER_STATUS_ERROR    0xFF

#define AVRCP_CAP_ID_COMPANY_ID       0x2
#define AVRCP_CAP_ID_EVENTS_SUPPORTED 0x3

#define SAMPLE_ARTIST_NAME "SAMPLE_ARTIST_NAME"
#define SAMPLE_ALBUM_NAME  "SAMPLE_ALBUM_NAME"
#define SAMPLE_GENRE       "SAMPLE_GENRE"

#define PKT_HEADER_AND_CRC 11 // 11 --> l2cap+controller header + crc
#define RTP_HEADER         13 // 13 --> rtp_header

uint8_t *rsi_itoa(uint32_t val, uint8_t *str);
#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
#define SONG_LIST_MAX_COUNT 3
#define AUDIO_FILE_PATH     "/sbc_1.sbc"
#define AUDIO_FILE_PATH_2   "/sbc_2.sbc"
#define AUDIO_FILE_PATH_3   "/sbc_3.sbc"
#define ELEMENT_READ_SIZE   1
#define RSI_SBC_DATA_SIZE   1100
/* sampling frequency */
#define SBC_FREQ_16000 0x00
#define SBC_FREQ_32000 0x01
#define SBC_FREQ_44100 0x02
#define SBC_FREQ_48000 0x03

/* blocks */
#define SBC_BLK_4  0x00
#define SBC_BLK_8  0x01
#define SBC_BLK_12 0x02
#define SBC_BLK_16 0x03

/* channel mode */
#define SBC_MODE_MONO         0x00
#define SBC_MODE_DUAL_CHANNEL 0x01
#define SBC_MODE_STEREO       0x02
#define SBC_MODE_JOINT_STEREO 0x03

/* allocation method */
#define SBC_AM_LOUDNESS 0x00
#define SBC_AM_SNR      0x01

/* subbands */
#define SBC_SB_4 0x00
#define SBC_SB_8 0x01
//! SBC_FRAME Mapping Headers
#define SBC_HEADER_SIZE 4
#define SBC_FRAME_SIZE  sbc_frame_size
#define SKIP_SIZE       (SBC_FRAME_SIZE + RTP_HEADER + PKT_HEADER_AND_CRC)
#define SBC_SYNCWORD    0x9C
#if RSI_BT_MEMORY_STATS
chip_bt_buffers_stats_t rsi_chip_bt_buffers_stats;
#endif
enum {
  MONO         = SBC_MODE_MONO,
  DUAL_CHANNEL = SBC_MODE_DUAL_CHANNEL,
  STEREO       = SBC_MODE_STEREO,
  JOINT_STEREO = SBC_MODE_JOINT_STEREO
} mode;

#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
#define SONG_LIST_MAX_COUNT 3
#define AUDIO_FILE_PATH     "/pcm.wav"
#define AUDIO_FILE_PATH_2   "/pcm.wav"
#define AUDIO_FILE_PATH_3   "/pcm.wav"
#define RSI_PCM_DATA_SIZE   512
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
#define SONG_LIST_MAX_COUNT 3
#define AUDIO_FILE_PATH     "/mp3.mp3"
#define AUDIO_FILE_PATH_2   "/mp3.mp3"
#define AUDIO_FILE_PATH_3   "/mp3.mp3"
#define RSI_MP3_DATA_SIZE   512 * 20
#define RMOVE_ID3_TAG
#endif
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
#define ELEMENT_READ_SIZE 1
#endif
#define DEFAULT_MTU_SIZE 310
/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
uint8_t play_pause_count;
uint8_t dual_pair;
uint16_t sample_freq;
uint16_t rem_mtu_size                           = DEFAULT_MTU_SIZE;
uint8_t app_song_list[SONG_LIST_MAX_COUNT][128] = { AUDIO_FILE_PATH, AUDIO_FILE_PATH_2, AUDIO_FILE_PATH_3 };
uint8_t app_song_list_idx;
uint8_t local_dev_role          = 0xff;
volatile uint32_t aud_pkts_sent = 0;
uint16_t NbrBytesReqd;
uint8_t glbl_play_status = PLAYER_STATUS_PLAY;

static uint32_t rsi_app_async_event_map  = 0;
static uint32_t rsi_app_async_event_map2 = 0;
static uint8_t str_conn_bd_addr[BD_ADDR_ARRAY_LEN];
static uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN]  = { 0 };
static uint8_t remote_dev_addr1[RSI_DEV_ADDR_LEN] = { 0 };
static uint8_t local_dev_addr[RSI_DEV_ADDR_LEN]   = { 0 };
static uint8_t linkkey[RSI_LINKKEY_REPLY_SIZE];
static uint8_t dev_mode;
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO) || (RSI_AUDIO_DATA_TYPE == MP3_AUDIO))
uint8_t first_buff_overflow = 1;
uint8_t underflow           = 0;
uint16_t overflow_count = 0, bt_disable_triggered = 0;
static uint32_t bytes_to_read;
static uint8_t frame_size;
static uint8_t no_of_sbc_frames;
static uint8_t no_of_channels;
static uint8_t no_of_subbands;
static uint8_t no_of_blocks;
#endif
#if RUN_TIME_PKT_TYPE_CHANGE
uint16_t headset_max_mtu_size = 0;
#endif

static rsi_bt_event_avrcp_notify_t avrcp_notify;
rsi_bt_rsp_avrcp_get_capabilities_t get_cap_resp;
rsi_bt_rsp_avrcp_remote_version_t avrcp_version;
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO) || (RSI_AUDIO_DATA_TYPE == MP3_AUDIO))
static rsi_bt_a2dp_sbc_codec_cap_t set_sbc_cap;
static rsi_bt_resp_a2dp_get_config_t sbc_resp_cap;
#endif
//! Structure variables
app_state_t app_state                          = 0;
static rsi_bt_resp_get_local_name_t local_name = { 0 };

uint8_t powersave_command_given = 0;
#if RSI_BT_AVDTP_STATS
#define AVDTP_STATS_ENABLE  1
#define AVDTP_STATS_DISABLE 0
#define AVDTP_STATS_UPDATE_RATE \
  30000 //! This will be considered as millisec. Minimum Time will be fine tunes by running all the possible modes.
#if BT_BR_EDR_ADAPTIVE
uint16_t first_pkt_type;
uint16_t second_pkt_type;
uint16_t is_first_pkt_type_set  = 1;
uint16_t is_second_pkt_type_set = 0;
#define PACKET_ERROR_RATE_THRESHOLD 50 //! Range should be 0 to 100%, Considering 50% as threshold for now
#endif
rsi_bt_event_avdtp_stats_t avdtp_stats;
uint32_t packet_error;
uint32_t packet_sent;
uint32_t packet_error_rate;
uint32_t ack_count;
uint32_t actual_tx_count;
#endif
#if RSI_BT_AUTO_RATE
rsi_bt_event_ar_stats_t ar_stats;
#endif

#if RSI_PACKET_CHANGE_INDICATION
rsi_bt_event_pkt_change_t rsi_bt_pkt_change;
#endif

#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
uint16_t pkt_size;
uint8_t sbc_frames;
uint32_t sbc_frame_size = 0;
uint8_t sbc_data[RSI_SBC_DATA_SIZE];

#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
uint16_t pkt_size;
uint8_t pcm_data[RSI_PCM_DATA_SIZE];
uint8_t sbc_frames;
uint32_t sbc_frame_size = 0;

#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
uint16_t pkt_size;
uint8_t mp3_data[MP3_INPUT_BUFFER_SIZE];

//! TAG is always present at the beggining of a ID3V2 MP3 file
//! Constant size 10 bytes
typedef struct {
  uint8_t id[3];      //"ID3"
  uint8_t version[2]; // $04 00
  uint8_t flags;      // %abcd0000
  uint32_t size;      //4 * %0xxxxxxx
} __attribute__((__packed__)) ID3TAG;
#endif

#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
/* File object */
FIL g_fileObject_sd;
uint8_t file_idx_sd = 0;
uint8_t first_read  = 1;
/* Read buffer */
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
static uint8_t g_bufferRead[PCM_INPUT_BUFFER_SIZE];
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
static uint8_t g_bufferRead[MP3_INPUT_BUFFER_SIZE];
#else
static uint8_t g_bufferRead[512];
#endif
#endif

#if (RSI_BT_DUAL_PAIR_TEST == 1)
uint8_t dual_pair_flag;
#endif
/* As per garmin requirment*/
rsi_bt_a2dp_sbc_codec_cap_t audio_codec_cap = { SBC_CHANNEL_MODE_JOINT_STEREO,
                                                SBC_SAMPLING_FREQ_44100,
                                                SBC_ALLOCATION_LOUDNESS, //| SBC_ALLOCATION_SNR,
                                                SBC_SUBBANDS_8,
                                                SBC_BLOCK_LENGTH_16,
                                                SBC_MIN_BITPOOL,
                                                35 }; //SBC_MAX_BITPOOL };
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_semaphore_handle_t bt_app_sem, bt_inquiry_sem;
#if WLAN_SYNC_REQ
extern rsi_semaphore_handle_t sync_coex_bt_sem;
#endif
#if WLAN_TRANSIENT_CASE
extern rsi_semaphore_handle_t wlan_sync_coex_bt_sem;
bool bt_thread_in_acceptor_connection = false, bt_thread_as_pagescan = false;
extern uint32_t disable_factor_count;
#endif
extern bool rsi_ble_running, rsi_ant_running, rsi_wlan_running, powersave_cmd_given;
extern rsi_mutex_handle_t power_cmd_mutex;
/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

#if RPS_LINK_KEY_SAVE
extern bool rsi_link_key_remove(dev_address_type addr1);
extern bool rsi_link_key_add(rsi_bt_event_user_linkkey_save_t *link_key_update);
extern bool rsi_link_key_read(rsi_bt_event_user_linkkey_save_t *peer_link_key);
extern void rsi_init_link_key_list(void);
#endif
/*=======================================================================*/
//   ! FUNCTION PROTOTYPES
/*=======================================================================*/
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO) || (RSI_AUDIO_DATA_TYPE == MP3_AUDIO))
int16_t read_pcm_data_from_sd(void);
#endif
/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

/*==============================================*/
/**
 * @fn         rsi_bt_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BT initialization.
 */
static void rsi_bt_app_init_events()
{
  rsi_app_async_event_map  = 0;
  rsi_app_async_event_map2 = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_bt_app_set_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_app_async_event_map |= BIT(event_num);
  } else {
    rsi_app_async_event_map2 |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&bt_app_sem);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_bt_app_clear_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_app_async_event_map &= ~BIT(event_num);
  } else {
    rsi_app_async_event_map2 &= ~BIT((event_num - 32));
  }
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_bt_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix <= 31) {
      if (rsi_app_async_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (rsi_app_async_event_map2 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn         switch_proto_async
 * @brief      invoked when bt switching is enabled
 * @param[in]  none
 * @return     none
 * @section description
 * This function returns the highest priority event among all the set events
 */
void switch_proto_async(uint16_t mode, uint8_t *bt_disabled_status)
{
  //These statements are added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(mode);
  UNUSED_PARAMETER(bt_disabled_status);
  LOG_PRINT("\n IN ASYNC \n");

#if TEST_CASE_7
  if (*bt_disabled_status == RSI_SUCCESS) {
    rsi_bt_app_set_event(RSI_APP_EVENT_BT_DISABLED);
  } else {
    LOG_PRINT("Event Received and its status = %x :\r\n", *bt_disabled_status);
  }
#endif
}

#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if INQUIRY_ENABLE
/*==============================================*/
/**
 * @fn         rsi_bt_inq_response
 * @brief      invoked when bt inquiry response is received
 * @param[out] status - status of inquiry
 * 				resp_event - bt inquiry response
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote bt inquiry response is received
 */
void rsi_bt_inq_response(uint16_t status, rsi_bt_event_inquiry_response_t *resp_event)
{
  uint8_t tmp_str_addr[BD_ADDR_ARRAY_LEN] = { 0 };
  if (status != RSI_APP_ERR_NONE) {
    return;
  }
  rsi_6byte_dev_address_to_ascii(tmp_str_addr, resp_event->dev_addr);
#if BT_EIR_FRIENDLY_NAME_TEST
  memcpy(&l_resp_event[inq_responses_count++], resp_event, sizeof(rsi_bt_event_inquiry_response_t));
  LOG_PRINT("\r\n inq_type : %d, Bdaddr : %s, class : %d%d%d, rssi : %d, name : %s ",
            resp_event->inquiry_type,
            tmp_str_addr,
            resp_event->cod[0],
            resp_event->cod[1],
            resp_event->cod[2],
            resp_event->rssi,
            (resp_event->name_length) ? resp_event->remote_device_name : NULL);
#else
  memcpy(&rsi_inq_resp_list[inq_responses_count][0],
         rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr1, (int8_t *)tmp_str_addr),
         RSI_DEV_ADDR_LEN);
  rsi_inq_resp_list_name_length[inq_responses_count++] = resp_event->name_length;
  LOG_PRINT("\r\n inq_type : %d, Bdaddr : %s, class : %d%d%d, rssi : %d, name : %s ",
            resp_event->inquiry_type,
            tmp_str_addr,
            resp_event->cod[0],
            resp_event->cod[1],
            resp_event->cod[2],
            resp_event->rssi,
            (resp_event->name_length) ? resp_event->remote_device_name : NULL);
#endif
}

/*==============================================*/
/**
 * @fn         rsi_bt_inquiry_complete
 * @brief      invoked when bt inquiry is completed
 * @param[out] status - status of inquiry
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote bd addresses inquiry is completed
 */
void rsi_bt_inquiry_complete(uint16_t status)
{
  if (status != RSI_APP_ERR_NONE) {
    LOG_PRINT("\r\n Inq complete error with status : %x", status);
  }
  LOG_PRINT("\r\n Inq completed \n");
  rsi_bt_app_set_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
}

/*==============================================*/
/**
 * @fn         rsi_remote_name_response
 * @brief      invoked when remote name request is called
 * @param[out] status - status of remote name request
 * 				remote_name_response_event - remote name response.
 * @param[out] none
 * @return     none.
 * @section description
 * This callback function invoked when remote name request initiated in application
 */
void rsi_remote_name_response(uint16_t status, rsi_bt_event_remote_device_name_t *remote_name_response_event)
{
  uint8_t tmp_str_addr[BD_ADDR_ARRAY_LEN] = { 0 };
  if (status != RSI_APP_ERR_NONE) {
    LOG_PRINT("\r\n Remote name response error with status : %x", status);
  } else {
    LOG_PRINT("\r\n remote name response received \n");
    rsi_6byte_dev_address_to_ascii(tmp_str_addr, remote_name_response_event->dev_addr);
    LOG_PRINT("\r\n Bdaddr : %s, name_length : %d, name : %s\n ",
              tmp_str_addr,
              remote_name_response_event->name_length,
              remote_name_response_event->remote_device_name);
  }
  inq_resp_name_length_index++;
  rsi_bt_app_set_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
}
#endif
#endif
/*==============================================*/
/**
 * @fn         rsi_bt_app_on_connection_initiated
 * @brief      invoked when before connection complete event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] conn_event, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */

void rsi_bt_app_on_connection_initiated(uint16_t resp_status, rsi_bt_event_connection_initiated_t *conn_event)
{

  if (resp_status == RSI_APP_ERR_NONE) {
    if (conn_event->conn_initiated == 1) {
      rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTION_INITIATED);
      LOG_PRINT("\n connection initiated by headset \n");
    }
  }
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_conn
 * @brief      invoked when connection complete event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] conn_event, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */

void rsi_bt_app_on_conn(uint16_t resp_status, rsi_bt_event_bond_t *conn_event)
{
  if (resp_status == RSI_APP_ERR_NONE) {
    app_state |= (1 << CONNECTED);
    rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
  } else if (resp_status == RSI_APP_ERR_ACL_CONN_ALREADY_EXISTS) {
    app_state |= (1 << CONNECTED);
    //app_state |= (1 << AUTHENTICATED);
    rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
  } else {
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
  memcpy((int8_t *)remote_dev_addr, conn_event->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on conn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, conn_event->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_pincode_req
 * @brief      invoked when pincode request event is received
 * @param[out] user_pincode_request, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pincode request from remote device
 */
void rsi_bt_app_on_pincode_req(uint16_t resp_status, rsi_bt_event_user_pincode_request_t *user_pincode_request)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_PINCODE_REQ);
  memcpy((int8_t *)remote_dev_addr, user_pincode_request->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on pincode: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_pincode_request->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_linkkey_req
 * @brief      invoked when linkkey request event is received
 * @param[out] user_linkkey_req, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey request from remote device
 */
void rsi_bt_app_on_linkkey_req(uint16_t resp_status, rsi_bt_event_user_linkkey_request_t *user_linkkey_req)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_REQ);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_req->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on linkkey req: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_req->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_linkkey_save
 * @brief      invoked when linkkey save event is received
 * @param[out] user_linkkey_req, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey save from local device
 */
void rsi_bt_app_on_linkkey_save(uint16_t resp_status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_SAVE);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_save->dev_addr, RSI_DEV_ADDR_LEN);
  memcpy(user_linkkey_save->local_dev_addr, local_dev_addr, RSI_DEV_ADDR_LEN);
#if RPS_LINK_KEY_SAVE
  rsi_link_key_add((rsi_bt_event_user_linkkey_save_t *)user_linkkey_save);
#else
  memcpy(linkkey, user_linkkey_save->linkKey, RSI_LINKKEY_REPLY_SIZE);
#endif
  LOG_PRINT("on linkkey save: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_save->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_auth_complete
 * @brief      invoked when authentication event is received
 * @param[out] resp_status, authentication status
 * @param[out] auth_complete, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pairing status and remote device information
 */
void rsi_bt_app_on_auth_complete(uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete)
{
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AUTH_COMPLT);
    app_state |= (1 << AUTHENTICATED);
  } else {
    //! remove link key from flash(SD card)
#if RPS_LINK_KEY_SAVE
    rsi_link_key_remove(auth_complete->dev_addr);
#else
    memset(linkkey, 0, RSI_LINKKEY_REPLY_SIZE);
#endif
  }
  memcpy((int8_t *)remote_dev_addr, auth_complete->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on auth complete: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, auth_complete->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_unbond_status
 * @brief      invoked when disconnect event is received
 * @param[out] resp_status, disconnect status/error
 * @param[out] bt_disconnected, disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the disconnected device information
 */
void rsi_bt_app_on_unbond_status(uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status)
{

  rsi_bt_app_set_event(RSI_APP_EVENT_UNBOND_STATUS);
  memcpy((int8_t *)remote_dev_addr, unbond_status->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on unbond status: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, unbond_status->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_disconn
 * @brief      invoked when disconnect event is received
 * @param[out] resp_status, disconnect status/error
 * @param[out] bt_disconnected, disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the disconnected device information
 */
void rsi_bt_app_on_disconn(uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnected)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  memcpy((int8_t *)remote_dev_addr, bt_disconnected->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on disconn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, bt_disconnected->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_confirm_request
 * @brief      invoked when confirmation request event is received
 * @param[out] confirmation request,confirmation request to remote device  
 * @return     none.
 * @section description
 * This callback function indicates the confirmation request event
 */
void rsi_bt_on_confirm_request(uint16_t resp_status,
                               rsi_bt_event_user_confirmation_request_t *user_confirmation_request)
{
  LOG_PRINT("\r\n data: %d", user_confirmation_request->confirmation_value);
  rsi_bt_app_set_event(RSI_APP_EVENT_CONFIRM_REQUEST);
  LOG_PRINT("\r\n on confirm req: bd addr %s, resp status 0x%x\r\n", str_conn_bd_addr, resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_mode_change
 * @brief      invoked when mode chande event is received
 * @param[out] mode change,mode change request to remote device
 * @return     none.
 * @section description
 * This callback function indicates the mode change event
 */
void rsi_bt_on_mode_change(uint16_t resp_status, rsi_bt_event_mode_change_t *mode_change)
{
  memcpy((int8_t *)remote_dev_addr, mode_change->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("mode_change_event: str_conn_bd_addr: %s, resp_status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, mode_change->dev_addr),
            resp_status);
  LOG_PRINT("mode : %d \r\n", mode_change->current_mode);
  dev_mode = mode_change->current_mode;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_a2dp_connect
 * @brief      invoked when a2dp profile connected event is received
 * @param[out] a2dp_connect, a2dp connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp connected remote device information
 */
void rsi_bt_app_on_a2dp_connect(uint16_t resp_status, rsi_bt_event_a2dp_connect_t *a2dp_connect)
{
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_CONN);
    app_state |= (1 << A2DP_CONNECTED);
    memcpy((int8_t *)remote_dev_addr, a2dp_connect->dev_addr, RSI_DEV_ADDR_LEN);
  }
  LOG_PRINT("on a2dp conn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, a2dp_connect->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_a2dp_disconnect
 * @brief      invoked when a2dp profile disconnected event is received
 * @param[out] a2dp_disconnect, a2dp disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp disconnected remote device information
 */
void rsi_bt_app_on_a2dp_disconnect(uint16_t resp_status, rsi_bt_event_a2dp_disconnect_t *a2dp_disconnect)
{
  if (resp_status == 0) {
    app_state &= ~(1 << A2DP_CONNECTED);
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_DISCONN);
  }
  memcpy((int8_t *)remote_dev_addr, a2dp_disconnect->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on a2dp disconn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, a2dp_disconnect->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_a2dp_configure
 * @brief      invoked when a2dp profile configured event is received
 * @param[out] a2dp_connect, a2dp configured remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp configured remote device information
 */
void rsi_bt_app_on_a2dp_configure(uint16_t resp_status, rsi_bt_event_a2dp_configure_t *a2dp_configure)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_CONFIGURE);
  memcpy((int8_t *)remote_dev_addr, a2dp_configure->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on a2dp configure: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, a2dp_configure->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_a2dp_more_data_req
 * @brief      invoked when a2dp profile configured event is received
 * @param[out] a2dp_connect, a2dp configured remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp configured remote device information
 */
void rsi_bt_on_a2dp_more_data_req(uint16_t resp_status, rsi_bt_event_a2dp_more_data_req_t *a2dp_more_data_req)
{
  if (resp_status == RSI_SUCCESS) {
    if (app_state & (1 << A2DP_STREAM_START)) {
      rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
      memcpy((int8_t *)remote_dev_addr, a2dp_more_data_req->dev_addr, RSI_DEV_ADDR_LEN);
      /* No. of SBC bytes required to receive more data request
			 * Ex1:for 2DH5 Packet (MTU=672) for Bitpool=53, 595 (5*119) is the maximum SBC bytes can be sent to RS9116
			 *     then this number will be 595*3 (Assuming RS9116 stack has 3 buffers available)
			 * Ex2:for 3DH5 Packet (MTU=1021) for Bitpool=53, 952 (8*119) is the maximum SBC bytes can be sent to RS9116
			 *     then this number will be 952*5 (Assuming RS9116 stack has 5 buffers available)
			 */
      NbrBytesReqd = a2dp_more_data_req->NbrBytesReqd;
    }
  }
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_a2dp_open
 * @brief      invoked when a2dp profile open event is received
 * @param[out] a2dp_open, a2dp open remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp open remote device information
 */
void rsi_bt_app_on_a2dp_open(uint16_t resp_status, rsi_bt_event_a2dp_open_t *a2dp_oen)
{
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_OPEN);
    app_state |= (1 << A2DP_STREAM_OPEN);
    memcpy((int8_t *)remote_dev_addr, a2dp_oen->dev_addr, RSI_DEV_ADDR_LEN);
  }
  LOG_PRINT("on a2dp open: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, a2dp_oen->dev_addr),
            resp_status);
}
/**
 * @fn         rsi_bt_app_on_a2dp_start
 * @brief      invoked when a2dp profile start event is received
 * @param[out] a2dp_start, a2dp start remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp start remote device information
 */
void rsi_bt_app_on_a2dp_start(uint16_t resp_status, rsi_bt_event_a2dp_start_t *a2dp_start)
{
  uint32_t freq;

  if (resp_status == RSI_SUCCESS) {
    sample_freq = freq = a2dp_start->sample_freq;
#if (BT_BDR_MODE == 1)
    rem_mtu_size = DEFAULT_MTU_SIZE;
#else
    rem_mtu_size = (a2dp_start->rem_mtu_size > DEFAULT_MTU_SIZE) ? (a2dp_start->rem_mtu_size) : DEFAULT_MTU_SIZE;

#endif
#if RUN_TIME_PKT_TYPE_CHANGE
    headset_max_mtu_size = a2dp_start->rem_mtu_size;
#endif
#if !(A2DP_BT_ONLY_CONNECTION)
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_START);
    app_state |= (1 << A2DP_STREAM_START);
#endif
  }
  update_modified_mtu_size(rem_mtu_size);
  LOG_PRINT("\r\non a2dp start: bd addr %s, resp status 0x%x\r\n", str_conn_bd_addr, resp_status);
  LOG_PRINT("\r\nSampling_Freq: %d, MTU: %d\r\n", freq, rem_mtu_size);
}

/**
 * @fn         rsi_bt_app_on_a2dp_suspend
 * @brief      invoked when a2dp profile suspend event is received
 * @param[out] a2dp_suspend, a2dp suspend remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp suspend remote device information
 */
void rsi_bt_app_on_a2dp_suspend(uint16_t resp_status, rsi_bt_event_a2dp_suspend_t *a2dp_suspend)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(a2dp_suspend);
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_SUSPEND);
  }

  LOG_PRINT("\r\n on a2dp suspend: bd addr %s, resp status 0x%x\r\n", str_conn_bd_addr, resp_status);
}

/**
 * @fn         rsi_bt_app_a2dp_pause
 * @brief      API to initiate a2dp stream pause
 * @param[out] none
 * @return     none.
 * @section description
 * This function request to susped the a2dp stream
 */
void rsi_bt_app_a2dp_pause()
{
  int32_t status = 0;
  status         = rsi_bt_a2dp_suspend(remote_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n A2DP suspend req: bd addr %s, status err 0x%x\r\n", str_conn_bd_addr, status);
  } else {
    app_state &= ~(1 << A2DP_STREAM_START);
    rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
    rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
#endif
  }
}

/**
 * @fn         rsi_bt_app_a2dp_resume
 * @brief      API to initiate a2dp stream resume
 * @param[out] none
 * @return     none.
 * @section description
 * This function request to susped the a2dp stream
 */
void rsi_bt_app_a2dp_resume()
{
  int32_t status = 0;
#if !(A2DP_BT_ONLY_CONNECTION)
  status = rsi_bt_a2dp_start((uint8_t *)remote_dev_addr);
#endif
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n A2DP start req: bd addr %s, status err 0x%x\r\n", str_conn_bd_addr, status);
  } else {
    /*
		 * 0x00 Active Mode.
		 * 0x01 Hold Mode.
		 * 0x02 Sniff Mode. */
    if (dev_mode == 0x02) {
      rsi_bt_sniff_exit_mode(remote_dev_addr);
    }
    app_state |= (1 << A2DP_STREAM_START);
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
  }
}

/**
 * @fn         rsi_bt_app_on_a2dp_reconfig
 * @brief      invoked when a2dp profile reconfig event is received
 * @param[out] a2dp_suspend, a2dp suspend remote device information
 * @return     none.
 * @section description
 * This callback function indicates the a2dp suspend remote device information
 */
void rsi_bt_app_on_a2dp_reconfig(uint16_t resp_status, rsi_bt_event_a2dp_reconfig_t *a2dp_reconfig)
{
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_RECONFIG);
  }

  LOG_PRINT("\r\n on a2dp reconfig: bd addr %s, reconfig status 0x%x\r\n",
            str_conn_bd_addr,
            a2dp_reconfig->resp_status);
}
/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_connect
 * @brief       invoked when avrcp connected event is received
 * @param[out] avrcp_conn, remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp connected event
 */
void rsi_bt_app_on_avrcp_connect(uint16_t resp_status, rsi_bt_event_avrcp_connect_t *avrcp_connect)
{

  if (resp_status == 0) {
    app_state |= (1 << AVRCP_CONNECTED);
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_CONN);
  }
  memcpy(remote_dev_addr, avrcp_connect->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp conn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_connect->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_disconnect
 * @brief       invoked when avrcp disconnected event is received
 * @param[out] avrcp_disconn, remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp disconnected event
 */
void rsi_bt_app_on_avrcp_disconnect(uint16_t resp_status, rsi_bt_event_avrcp_disconnect_t *avrcp_disconn)
{
  if (resp_status == 0) {
    app_state &= ~(1 << AVRCP_CONNECTED);
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_DISCONN);
  }
  memcpy(remote_dev_addr, avrcp_disconn->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp disconn: bd addr %s, resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_disconn->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_play
 * @brief      invoked when avrcp previous event is play 
 * @param[out] avrcp play,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp play event
 */
void rsi_bt_app_on_avrcp_play(uint16_t resp_status, rsi_bt_event_avrcp_play_t *avrcp_play)
{
  if (avrcp_play->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_PLAY);
  }
  memcpy(remote_dev_addr, avrcp_play->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp play: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_play->dev_addr),
            resp_status,
            avrcp_play->status_flag);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_pause
 * @brief      invoked when avrcp pause event is received
 * @param[out] avrcp pause,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp pause event
 */
void rsi_bt_app_on_avrcp_pause(uint16_t resp_status, rsi_bt_event_avrcp_pause_t *avrcp_pause)
{
  if (avrcp_pause->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_PAUSE);
  }
  memcpy(remote_dev_addr, avrcp_pause->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp pause: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_pause->dev_addr),
            resp_status,
            avrcp_pause->status_flag);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_next
 * @brief      invoked when avrcp next event is received
 * @param[out] avrcp pause,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp next event
 */
void rsi_bt_app_on_avrcp_next(uint16_t resp_status, rsi_bt_event_avrcp_next_t *avrcp_next)
{
  if (avrcp_next->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_NEXT);
  }
  memcpy(remote_dev_addr, avrcp_next->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp next: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_next->dev_addr),
            resp_status,
            avrcp_next->status_flag);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_previous
 * @brief      invoked when avrcp previous event is received
 * @param[out] avrcp pause,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp previous event
 */
void rsi_bt_app_on_avrcp_previous(uint16_t resp_status, rsi_bt_event_avrcp_previous_t *avrcp_prev)
{
  if (avrcp_prev->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_PREVIOUS);
  }
  memcpy(remote_dev_addr, avrcp_prev->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp previous: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_prev->dev_addr),
            resp_status,
            avrcp_prev->status_flag);
}
/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_vol_up
 * @brief      invoked when avrcp volume up event is received
 * @param[out] avrcp volume up,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp volume up event
 */
void rsi_bt_app_on_avrcp_vol_up(uint16_t resp_status, rsi_bt_event_avrcp_vol_up_t *avrcp_vol_up)
{
  if (avrcp_vol_up->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_VOL_UP);
  }
  memcpy(remote_dev_addr, avrcp_vol_up->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp volume up: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_vol_up->dev_addr),
            resp_status,
            avrcp_vol_up->status_flag);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_avrcp_vol_down
 * @brief      invoked when avrcp volume down event is received
 * @param[out] avrcp volume down,remote device bdaddress
 * @return     none.
 * @section description
 * This callback function indicates the avrcp volume down event
 */
void rsi_bt_app_on_avrcp_vol_down(uint16_t resp_status, rsi_bt_event_avrcp_vol_down_t *avrcp_vol_down)
{
  if (avrcp_vol_down->status_flag == RSI_BUTTON_RELEASED) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_VOL_DOWN);
  }
  memcpy(remote_dev_addr, avrcp_vol_down->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp volume down: bd addr %s, resp status 0x%x status_flag 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, avrcp_vol_down->dev_addr),
            resp_status,
            avrcp_vol_down->status_flag);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_avrcp_notify_event
 * @brief      invoked when avrcp notification event is received
 * @param[out] avrcp p_notify, player notication event related info
 * @return     none.
 * @section description
 * This callback function indicates the avrcp notification event
 */

void rsi_bt_on_avrcp_notify_event(uint16_t resp_status, rsi_bt_event_avrcp_notify_t *p_notify)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_NOTIFY);
  memcpy(&avrcp_notify, p_notify, sizeof(rsi_bt_event_avrcp_notify_t));
  memcpy(remote_dev_addr, p_notify->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on avrcp notify: bd addr %s, Event_ID: %d resp status 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, p_notify->dev_addr),
            avrcp_notify.notify_val.eventid,
            resp_status);
}
/*==============================================*/
/**
 * @fn         rsi_bt_on_avrcp_get_cap_event
 * @brief      invoked when AVRCP get capabilities event is received
 * @param[out] resp_status, event status
 * @param[out] ssp_complete, secure simple pairing requested remote device information
 * @return     none.
 * @section description
 * This callback function indicates the ssp confirm event
 */
uint8_t app_cap_type;
uint8_t app_att_id;
rsi_bt_event_avrcp_ele_att_t app_song_atts;
rsi_bt_event_avrcp_get_cur_att_val_t app_att_list;
rsi_bt_event_avrcp_get_cur_att_val_t app_att_text_list;
rsi_bt_event_avrcp_vals_text_req_t app_vals_list;

void rsi_bt_on_avrcp_get_cap_event(uint8_t *bd_addr, uint8_t cap_type)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(bd_addr);
  app_cap_type = cap_type;
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_GET_CAP);
  LOG_PRINT("\r\nRSI_APP_EVENT_AVRCP_GET_CAP: cap_type: %d\r\n", cap_type);
  return;
}

void rsi_bt_on_avrcp_get_app_supp_atts_event(uint8_t *bd_addr)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(bd_addr);
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_GET_ATTS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_GET_ATTS\r\n");
  return;
}

void rsi_bt_on_avrcp_get_app_suup_att_vals_event(uint8_t *bd_addr, uint8_t att_id)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(bd_addr);
  app_att_id = att_id;
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_GET_ATT_VALS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_GET_ATT_VALS: att_id: %d\r\n", att_id);
  return;
}

void rsi_bt_on_avrcp_get_app_cur_att_val_event(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_list)
{
  uint8_t ix;

  memcpy(&app_att_list, p_att_list, sizeof(rsi_bt_event_avrcp_get_cur_att_val_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_CUR_ATT_VALS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_CUR_ATT_VALS: nbr_atts: %d, atts: ", p_att_list->nbr_atts);
  for (ix = 0; ix < p_att_list->nbr_atts; ix++) {
    LOG_PRINT("%d, ", p_att_list->atts);
  }
  LOG_PRINT("\r\n");

  return;
}

void rsi_bt_on_avrcp_set_app_cur_att_val_event(rsi_bt_event_avrcp_set_att_val_t *p_att_list)
{
  uint8_t ix;

  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_SET_ATT_VALS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_CUR_ATT_VALS: nbr_atts:%d\r\n", p_att_list->nbr_atts);
  for (ix = 0; ix < p_att_list->nbr_atts; ix++) {
    LOG_PRINT("attid: %d - attvalue: %d \r\n", p_att_list->att_list[ix].attid, p_att_list->att_list[ix].attvalue);
  }
  LOG_PRINT("\r\n");
  return;
}

void rsi_bt_on_avrcp_get_app_att_text_event(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_text_list)
{
  uint8_t ix;

  memcpy(&app_att_text_list, p_att_text_list, sizeof(rsi_bt_event_avrcp_get_cur_att_val_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_ATT_TEXT);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_ATT_TEXT: nbr_atts: %d, atts: ", p_att_text_list->nbr_atts);
  for (ix = 0; ix < p_att_text_list->nbr_atts; ix++) {
    LOG_PRINT("%d, ", p_att_text_list->atts[ix]);
  }
  LOG_PRINT("\r\n");
  return;
}

void rsi_bt_on_avrcp_get_app_att_vals_text_event(rsi_bt_event_avrcp_vals_text_req_t *p_vals_list)
{
  uint8_t ix;

  memcpy(&app_vals_list, p_vals_list, sizeof(rsi_bt_event_avrcp_vals_text_req_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_ATT_VALS_TEXT);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_ATT_VALS_TEXT: att_id: %d, nbr_vals:%d, vals:",
            p_vals_list->att_id,
            p_vals_list->nbr_vals);
  for (ix = 0; ix < p_vals_list->nbr_vals; ix++) {
    LOG_PRINT("%d, ", p_vals_list->vals);
  }
  LOG_PRINT("\r\n");
  return;
}
#if 0

void rsi_bt_on_avrcp_supp_char_sets_event(
		rsi_bt_event_avrcp_char_sets_t *p_char_sets) {
	uint8_t ix;

	rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_CHAR_SET);
	LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_CHAR_SET: nbr_sets:%d, sets: ",
			p_char_sets->nbr_sets);
	for (ix = 0; ix < p_char_sets->nbr_sets; ix++) {
		LOG_PRINT("%d, ", p_char_sets->char_sets[ix]);
	}
	LOG_PRINT("\r\n");
	return;
}

void rsi_bt_on_avrcp_batt_status_event(uint8_t *bd_addr, uint8_t batt_status) {
	rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_BATT_STATUS);
	LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_BATT_STATUS: batt_status: %d\r\n",
			batt_status);
	return;
}
#endif

void rsi_bt_on_avrcp_get_song_atts_event(rsi_bt_event_avrcp_ele_att_t *p_song_atts)
{
  uint8_t ix;

  memcpy(&app_song_atts, p_song_atts, sizeof(rsi_bt_event_avrcp_ele_att_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_SONG_ATTS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_SONG_ATTS: nbr_atts: %d, att_list:", p_song_atts->nbr_atts);
  for (ix = 0; ix < p_song_atts->nbr_atts; ix++) {
    LOG_PRINT("%d, ", p_song_atts->ele_atts[ix]);
  }
  LOG_PRINT("\r\n");

  return;
}
void rsi_bt_on_avrcp_get_play_status_event(uint8_t *bd_addr)
{
  //This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(bd_addr);
  rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_PLAY_STATUS);
  LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_PLAY_STATUS: \r\n");
  return;
}

#define MAX_REG_EVENTS 10
uint8_t app_event_id[MAX_REG_EVENTS];
uint8_t app_reg_events;
void rsi_bt_on_avrcp_reg_notify_event(uint8_t *bd_addr, uint8_t event_id)
{
  UNUSED_PARAMETER(bd_addr);
  if (app_reg_events < MAX_REG_EVENTS) {
    app_event_id[app_reg_events++] = event_id;
    rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_REG_EVENT);
    LOG_PRINT("\r\n RSI_APP_EVENT_AVRCP_REG_EVENT: event_id: %d\r\n", event_id);
  } else {
    LOG_PRINT("\r\n app_reg_events: %d crossed MAX_REG_EVENTS\r\n", app_reg_events);
  }
  return;
}

uint8_t abs_vol;
void rsi_bt_on_avrcp_set_abs_vol_event(rsi_bt_event_avrcp_set_abs_vol_t *p_abs_vol)
{
  uint8_t abs_vol_per;
  //rsi_bt_app_set_event (RSI_APP_EVENT_AVRCP_SET_ABS_VOL);
  memcpy(remote_dev_addr, p_abs_vol->dev_addr, RSI_DEV_ADDR_LEN);
  abs_vol = p_abs_vol->abs_vol;
  /* The value 0x0 corresponds to 0%.
	 * The value 0x7F corresponds to 100%. */
  abs_vol_per = (abs_vol * 100) / 0x7f;

  LOG_PRINT("on avrcp set abs vol: bd addr %s, Vol %d%\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, p_abs_vol->dev_addr),
            abs_vol_per);

  return;
}
#if RSI_BT_MEMORY_STATS
/*==============================================*/
/**
 * @fn         rsi_bt_on_chip_memory_status_event
 * @brief      its invoked when memory stats events are received.
 * @param[in]  resp_status, chip_bt_buffers_stats_t structure parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when memory stats events are received
 */
void rsi_bt_on_chip_memory_status_event(uint16_t resp_status, chip_bt_buffers_stats_t *chip_bt_buffers_stats)
{
  memcpy(&rsi_chip_bt_buffers_stats, chip_bt_buffers_stats, sizeof(chip_bt_buffers_stats_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_MEMORY_STATS_RECEIVED);
}
#endif

#if RSI_BT_AVDTP_STATS
/*==============================================*/
/**
 * @fn         rsi_bt_on_avdtp_stats_event
 * @brief      invoked when AVDTP stats event is received
 * @param[out] avdtp_stats, avdtp stats information
 * @return     none.
 * @section description
 * This callback function indicates avdtp stats details in BT device.
 */
void rsi_bt_on_avdtp_stats_event(uint16_t resp_status, rsi_bt_event_avdtp_stats_t *bt_avdtp_stats)
{
  memcpy(&avdtp_stats, bt_avdtp_stats, sizeof(rsi_bt_event_avdtp_stats_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AVDTP_STATS);
  //LOG_PRINT ("AVDTP STATS Received:\r\n");
}
#endif
#if RSI_BT_AUTO_RATE
void rsi_bt_on_ar_stats_event(uint16_t resp_status, rsi_bt_event_ar_stats_t *bt_ar_stats)
{
  memcpy(&ar_stats, bt_ar_stats, sizeof(rsi_bt_event_ar_stats_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_AR_STATS);
}
#endif
#if RSI_PACKET_CHANGE_INDICATION
void rsi_bt_pkt_change_stats_event(uint16_t resp_status, rsi_bt_event_pkt_change_t *bt_pkt_change_stats)
{
  memcpy(&rsi_bt_pkt_change, bt_pkt_change_stats, sizeof(rsi_bt_event_pkt_change_t));
  rsi_bt_app_set_event(RSI_APP_EVENT_PACKET_CHANGE);
  //LOG_PRINT ("\nPacket Change Received:\r\n");
}
#endif

#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
int32_t sbc_calc_frame_len(uint8_t *data)
{
  int32_t ret = 0;
  uint8_t block_mode, mode, subband_mode, subbands, channels, blocks, joint, bitpool;

  if (data[0] != SBC_SYNCWORD)
    return -2;

  block_mode = (data[1] >> 4) & 0x03;

  switch (block_mode) {
    case SBC_BLK_4:
      blocks = 4;
      break;
    case SBC_BLK_8:
      blocks = 8;
      break;
    case SBC_BLK_12:
      blocks = 12;
      break;
    case SBC_BLK_16:
      blocks = 16;
      break;
  }

  mode = (data[1] >> 2) & 0x03;

  switch (mode) {
    case MONO:
      channels = 1;
      break;
    case DUAL_CHANNEL: /* fall-through */
    case STEREO:
    case JOINT_STEREO:
      channels = 2;
      break;
  }

  subband_mode = (data[1] & 0x01);
  subbands     = subband_mode ? 8 : 4;

  bitpool = data[2];

  if ((mode == MONO || mode == DUAL_CHANNEL) && bitpool > 16 * subbands)
    return -4;

  if ((mode == STEREO || mode == JOINT_STEREO) && bitpool > 32 * subbands)
    return -4;

  joint = mode == SBC_MODE_JOINT_STEREO ? 1 : 0;

  ret = 4 + (4 * subbands * channels) / 8;
  /* This term is not always evenly divide so we round it up */
  if (channels == 1)
    ret += ((blocks * channels * bitpool) + 7) / 8;
  else
    ret += (((joint ? subbands : 0) + blocks * bitpool) + 7) / 8;

  return ret;
}

#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
int16_t read_sbc_data()
#else
int16_t read_sbc_data(FILE *fp)
#endif
{
  FRESULT error;
  uint8_t p_sbc_data[RSI_SBC_DATA_SIZE];
  UINT bytesRead;
  pkt_size   = 0;
  sbc_frames = 0;

  while (pkt_size < (rem_mtu_size - (SKIP_SIZE))) {
    //! read file 1 byte
#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
    error = f_read(&g_fileObject_sd, &p_sbc_data[pkt_size], 4, &bytesRead);
    if (error) {
      PRINTF("\r\n Read file failed. \r\n");
      return error;
    } else {
      if (bytesRead < SBC_HEADER_SIZE) {
        return RSI_APP_ERR_AUDIO_EOF;
      } else {
        pkt_size += bytesRead;
      }
    }
#else
    pkt_size += fread(&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, 4, fp);
#endif
    //! check SW == 0x9C
    if (p_sbc_data[0] == SBC_SYNCWORD) {
      sbc_frame_size = sbc_calc_frame_len(&p_sbc_data[0]);
      sbc_frames     = (rem_mtu_size - PKT_HEADER_AND_CRC - RTP_HEADER) / sbc_frame_size;
      //! read file <size-1>
#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
      //rsi_hal_set_gpio(RSI_HAL_RESET_PIN);
      error = f_read(&g_fileObject_sd, &p_sbc_data[pkt_size], ((sbc_frames * sbc_frame_size) - 4), &bytesRead);
      //rsi_hal_clear_gpio(RSI_HAL_RESET_PIN);
      if (error) {
        PRINTF("\r\n Read file failed. \r\n");
        return error;
      } else {
        if (bytesRead < (sbc_frame_size - SBC_HEADER_SIZE)) {
          return RSI_APP_ERR_AUDIO_EOF;
        } else {
          pkt_size += bytesRead;
        }
      }
#else
      pkt_size += fread(&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, (sbc_frame_size - 4), fp);
#endif
    } else {
      //! error ...
      LOG_PRINT(" fread err\n\n");
      return RSI_APP_ERR_AUDIO_EOF;
    }
    //sbc_frames++;
  }
  memset(sbc_data, 0, sizeof(sbc_data));
  memcpy(sbc_data, p_sbc_data, pkt_size);
#if 0
	while (pkt_size < (rem_mtu_size - (SKIP_SIZE))) {
		//! read file 1 byte
#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
		error = f_read(&g_fileObject_sd, &p_sbc_data[pkt_size], 4, &bytesRead);
		if (error) {
			PRINTF("\r\n Read file failed. \r\n");
			return error;
		} else {
			if (bytesRead < SBC_HEADER_SIZE) {
				return RSI_APP_ERR_AUDIO_EOF;
			} else {
				pkt_size += bytesRead;
			}
		}
#else
		pkt_size += fread (&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, 4, fp);
#endif
		//! check SW == 0x9C
		if (p_sbc_data[0] == SBC_SYNCWORD) {
			sbc_frame_size = sbc_calc_frame_len(&p_sbc_data[0]);
			//! read file <size-1>
#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
			error = f_read(&g_fileObject_sd, &p_sbc_data[pkt_size],
					(sbc_frame_size - 4), &bytesRead);
			if (error) {
				PRINTF("\r\n Read file failed. \r\n");
				return error;
			} else {
				if (bytesRead < (sbc_frame_size - SBC_HEADER_SIZE)) {
					return RSI_APP_ERR_AUDIO_EOF;
				} else {
					pkt_size += bytesRead;
				}
			}
#else
			pkt_size += fread (&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, (sbc_frame_size - 4), fp);
#endif
		} else {
			//! error ...
			LOG_PRINT("\r\n fread err\n\n");
			return RSI_APP_ERR_AUDIO_EOF;
		}
		sbc_frames++;
	}
	memset(sbc_data, 0, sizeof(sbc_data));
	memcpy(sbc_data, p_sbc_data, pkt_size);
#endif
  return 0;
}
#endif
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
int16_t open_audio_file(FILE **fp, uint8_t *file_name)
{
  *fp = fopen(file_name, "rb");
  if (*fp == NULL) {
    LOG_PRINT("\r\n file open fail\r\n");
    return 1;
  }
  LOG_PRINT("\r\n file opend fp: 0x%x\r\n", *fp);
  fseek(*fp, 0, SEEK_SET);

  return 0;
}

int16_t close_audio_file(FILE **fp)
{
  if (*fp == NULL) {
    LOG_PRINT("\r\n fp invalid\r\n");
    return 1;
  }

  fclose(*fp);
  LOG_PRINT("\r\n file closed fp: 0x%x\r\n", *fp);

  return 0;
}

#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
int32_t sbc_calc_frame_len(uint8_t *data)
{
  int32_t ret = 0;
  uint8_t block_mode, mode, subband_mode, subbands, channels, blocks, joint, bitpool;

  if (data[0] != SBC_SYNCWORD)
    return -2;

  block_mode = (data[1] >> 4) & 0x03;

  switch (block_mode) {
    case SBC_BLK_4:
      blocks = 4;
      break;
    case SBC_BLK_8:
      blocks = 8;
      break;
    case SBC_BLK_12:
      blocks = 12;
      break;
    case SBC_BLK_16:
      blocks = 16;
      break;
  }

  mode = (data[1] >> 2) & 0x03;

  switch (mode) {
    case MONO:
      channels = 1;
      break;
    case DUAL_CHANNEL: /* fall-through */
    case STEREO:
    case JOINT_STEREO:
      channels = 2;
      break;
  }

  subband_mode = (data[1] & 0x01);
  subbands     = subband_mode ? 8 : 4;

  bitpool = data[2];

  if ((mode == MONO || mode == DUAL_CHANNEL) && bitpool > 16 * subbands)
    return -4;

  if ((mode == STEREO || mode == JOINT_STEREO) && bitpool > 32 * subbands)
    return -4;

  joint = mode == SBC_MODE_JOINT_STEREO ? 1 : 0;

  ret = 4 + (4 * subbands * channels) / 8;
  /* This term is not always evenly divide so we round it up */
  if (channels == 1)
    ret += ((blocks * channels * bitpool) + 7) / 8;
  else
    ret += (((joint ? subbands : 0) + blocks * bitpool) + 7) / 8;

  return ret;
}

int16_t read_sbc_data(FILE *fp)
{
  int16_t fread_resp                    = 0;
  uint8_t p_sbc_data[RSI_SBC_DATA_SIZE] = { 0 };

  pkt_size   = 0;
  sbc_frames = 0;

  while (pkt_size < (rem_mtu_size - SKIP_SIZE)) {
    //! read file 1 byte
    fread_resp = fread(&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, SBC_HEADER_SIZE, fp);
    if (fread_resp < SBC_HEADER_SIZE) {
      //! error ...
      LOG_PRINT("\r\n fread err\n\n");
      return RSI_APP_ERR_AUDIO_EOF;
    } else {
      pkt_size += fread_resp;
    }

    //! check SW == 0x9C
    if (p_sbc_data[0] == SBC_SYNCWORD) {
      //! read file <size-1>
      sbc_frame_size = sbc_calc_frame_len(&p_sbc_data[0]);
      fread_resp     = fread(&p_sbc_data[pkt_size], ELEMENT_READ_SIZE, (sbc_frame_size - SBC_HEADER_SIZE), fp);
      if (fread_resp < (sbc_frame_size - SBC_HEADER_SIZE)) {
        //! error ...
        LOG_PRINT("\r\n fread err\n\n");
        return RSI_APP_ERR_AUDIO_EOF;
      } else {
        pkt_size += fread_resp;
      }
    } else {
      //! error ...
      LOG_PRINT("\r\n fread err\n\n");
      return RSI_APP_ERR_AUDIO_EOF;
    }
    sbc_frames++;
  }
  memset(sbc_data, 0, sizeof(sbc_data));
  memcpy(sbc_data, p_sbc_data, pkt_size);

  return 0;
}

#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
int16_t read_pcm_data(FILE *fp)
{
  pkt_size = fread(pcm_data, ELEMENT_READ_SIZE, sizeof(pcm_data), fp);
  if (pkt_size < 512) {
    return RSI_APP_ERR_AUDIO_EOF;
  }

  return 0;
}

#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)

unsigned int unsynchsafe(uint32_t be_in)
{
  unsigned int out = 0ul, mask = 0x7F000000ul;
  unsigned int in = 0ul;

  /* be_in is now big endian */
  /* convert to little endian */
  in = ((be_in >> 24) | ((be_in >> 8) & 0xFF00ul) | ((be_in << 8) & 0xFF0000ul) | (be_in << 24));

  while (mask) {
    out >>= 1;
    out |= (in & mask);
    mask >>= 8;
  }

  return out;
}

//! Makes sure the file is supported and return the correct size
int mp3Header(FILE *media, ID3TAG *tag)
{
  unsigned int tag_size;

  fread(tag, sizeof(ID3TAG), 1, media);

  if (memcmp((tag->id), "ID3", 3)) {
    return -1;
  }

  tag_size = unsynchsafe(tag->size);
  LOG_PRINT("\r\n tag_size = %x\n", tag_size);
  return tag_size;
}

void rsi_mp3_remove_id3_tag(FILE *fp)
{
  unsigned int id3_size, offset;
  ID3TAG mp3_tag;

  rewind(fp);
  id3_size = mp3Header(fp, &mp3_tag);
  LOG_PRINT("\r\n id3_size = %x\n", id3_size);

  if (mp3_tag.id[0] == 'I' && mp3_tag.id[1] == 'D' && mp3_tag.id[2] == '3') {
    fseek(fp, id3_size, SEEK_CUR);
    LOG_PRINT("\r\nid3 matched\n");
  } else {
    LOG_PRINT("\r\nid3 did not matched\n");
  }
  return;
}

int16_t read_mp3_data(FILE *fp)
{
  pkt_size = fread(mp3_data, 1, sizeof(mp3_data), fp);
  // LOG_PRINT ("\r\nmp3 pkt len read: %d\r\n", pkt_size);
  if (pkt_size == 0) {
    return RSI_APP_ERR_AUDIO_EOF;
  }

  return 0;
}
#endif
#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
int16_t open_audio_file_from_sd()
{
  FRESULT error;
#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
  // Open sbc audio file
  error = f_open(&g_fileObject_sd,
                 (const TCHAR *)_T(app_song_list[app_song_list_idx]),
                 (FA_WRITE | FA_READ | FA_OPEN_EXISTING));

  if (error) {
    PRINTF("\r\n Open existing file failed\r\n");
    return error;
  }
#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)

  // Open pcm audio file
  error = f_open(&g_fileObject_sd,
                 (const TCHAR *)_T(app_song_list[app_song_list_idx]),
                 (FA_WRITE | FA_READ | FA_OPEN_EXISTING));
  if (error) {
    PRINTF("\r\n Open existing file failed\r\n");
    return error;
  }
#endif

#if (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  // Open mp3 audio file
  error = f_open(&g_fileObject_sd,
                 (const TCHAR *)_T(app_song_list[app_song_list_idx]),
                 (FA_WRITE | FA_READ | FA_OPEN_EXISTING));
  if (error) {
    PRINTF("\r\n Open existing file failed\r\n");
    return error;
  }
#endif
  /* Move the file pointer */
  error = f_lseek(&g_fileObject_sd, 0U);
  if (error) {
    PRINTF("\r\n Set file pointer position failed. \r\n");
    return error;
  }

  return 0;
}
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
#if (!TA_BASED_ENCODER)
int32_t rsi_bt_app_sbc_encode()
{
  int16_t err             = 0;
  uint16_t bytes_consumed = 0;

#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    err = rsi_initiate_power_save();
    if (err != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return err;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif

  err = read_pcm_data_from_sd();
  if (err) {
    return err;
  }

  err = rsi_bt_sbc_encode(remote_dev_addr, g_bufferRead, bytes_to_read, &bytes_consumed);

  if (err)
    return err;

  return 0;
}

#endif
int16_t read_pcm_data_from_sd()
{
  uint32_t error = 0;
  UINT bytesRead;

  memset(g_bufferRead, '\0', sizeof(g_bufferRead));

#if (!TA_BASED_ENCODER)
  error = f_read(&g_fileObject_sd, g_bufferRead, bytes_to_read, &bytesRead);
#else
  if (first_read) {
    error = f_read(&g_fileObject_sd, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
  } else {
    error = f_read(&g_fileObject_sd, g_bufferRead, bytes_to_read, &bytesRead);
  }
#endif

  if (error) {
    PRINTF("\r\n Read file failed. \r\n");
    return error;
  } else {
    if (bytesRead == 0) {
      return RSI_APP_ERR_AUDIO_EOF;
    }
  }

  return 0;
}
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
int16_t read_mp3_data_from_sd()
{
  uint32_t error = 0;
  UINT bytesRead;
reread:
  memset(g_bufferRead, '\0', sizeof(g_bufferRead));
  if (first_read) {
    error = f_read(&g_fileObject_sd, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
  } else {
    error = f_read(&g_fileObject_sd, g_bufferRead, bytes_to_read, &bytesRead);
  }

  if (error) {
    PRINTF("\r\n Read file failed. \r\n");
    return error;
  } else {
    if (bytesRead == 0) {
      return RSI_APP_ERR_AUDIO_EOF;
    }
  }
  //if (bytesRead == 0) {
  //	f_lseek(&g_fileObject_sd, 0U);
  //	goto reread;
  //}
  return 0;
}
#elif (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
int16_t read_sbc_data_from_sd()
{
  uint32_t error = 0;
  UINT bytesRead;
reread:
  memset(g_bufferRead, '\0', sizeof(g_bufferRead));
  error = f_read(&g_fileObject_sd, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
  if (error) {
    PRINTF("\r\n Read file failed. \r\n");
    return error;
  }

  if (bytesRead == 0) {
    f_lseek(&g_fileObject_sd, 0U);
    goto reread;
  }
  return 0;
}
#endif

#elif (RSI_AUDIO_DATA_SRC == ARRAY)

#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
#if (!TA_BASED_ENCODER)
static uint32_t pcm_offset = 0;
int32_t rsi_bt_app_sbc_encode()
{
  int16_t err             = 0;
  uint16_t bytes_consumed = 0;

#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    err = rsi_initiate_power_save();
    if (err != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return err;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif

  if (pcm_offset + bytes_to_read <= sizeof(pcm_data_buff)) {
    uint8_t *pcm_data_ptr = pcm_data_buff + pcm_offset;
    err                   = rsi_bt_sbc_encode(remote_dev_addr, pcm_data_ptr, bytes_to_read, &bytes_consumed);
    pcm_offset += bytes_to_read;

    if (pcm_offset >= sizeof(pcm_data_buff)) //check whether offset reaches to end of the array
    {
      pcm_offset = 0;
    }
  } else { //skip the last chunk since it is less than bytes_to_read
    pcm_offset = 0;
  }
  if (err)
    return err;

  return 0;
}
#endif //RSI_AUDIO_DATA_TYPE
#endif //TA_BASED_ENCODER

#endif //RSI_AUDIO_DATA_SRC

int32_t calculate_sbc_frame_size(rsi_bt_a2dp_sbc_codec_cap_t *sbc_cap)
{
  int32_t ret = 0;
  uint8_t block_mode, mode, subband_mode, subbands, channels, blocks, joint, bitpool;

  block_mode = sbc_cap->BlockLength;
  switch (block_mode) {
    case SBC_BLOCK_LENGTH_4:
      blocks = 4;
      break;
    case SBC_BLOCK_LENGTH_8:
      blocks = 8;
      break;
    case SBC_BLOCK_LENGTH_12:
      blocks = 12;
      break;
    case SBC_BLOCK_LENGTH_16:
      blocks = 16;
      break;
  }

  mode = sbc_cap->ChannelMode;
  switch (mode) {
    case SBC_CHANNEL_MODE_MONO:
      channels = 1;
      break;
    case SBC_CHANNEL_MODE_DUAL_CHANNEL: /* fall-through */
    case SBC_CHANNEL_MODE_STEREO:
    case SBC_CHANNEL_MODE_JOINT_STEREO:
      channels = 2;
      break;
  }

  subband_mode = sbc_cap->SubBands;
  switch (subband_mode) {
    case SBC_SUBBANDS_4:
      subbands = 4;
      break;
    case SBC_SUBBANDS_8:
      subbands = 8;
      break;
  }

  bitpool = sbc_cap->MaxBitPool;

  if ((mode == SBC_CHANNEL_MODE_MONO || mode == SBC_CHANNEL_MODE_DUAL_CHANNEL) && (bitpool > (16 * subbands)))
    return -4;

  if ((mode == SBC_CHANNEL_MODE_STEREO || mode == SBC_CHANNEL_MODE_JOINT_STEREO) && (bitpool > (32 * subbands)))
    return -4;

  joint = mode == SBC_CHANNEL_MODE_JOINT_STEREO ? 1 : 0;

  ret = 4 + (4 * subbands * channels) / 8;
  /* This term is not always evenly divide so we round it up */
  if (channels == 1)
    ret += ((blocks * channels * bitpool) + 7) / 8;
  else
    ret += (((joint ? subbands : 0) + blocks * bitpool) + 7) / 8;

  return ret;
}

int16_t send_audio_data(FILE *fp)
{
//This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
#if !(RSI_AUDIO_DATA_SRC == BIN_FILE)
  UNUSED_PARAMETER(fp);
#endif

#if ENABLE_POWER_SAVE
  uint32_t status = 0;
#endif
  int16_t err = 0;
#if RSI_AUDIO_DATA_TYPE != SBC_AUDIO
  uint16_t bytes_consumed = 0;
#endif
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)

#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
  err = read_sbc_data(fp);
  if (err) {
    return err;
  }

  err = rsi_bt_a2dp_send_sbc_aac_data(remote_dev_addr, sbc_data, pkt_size, 0);
#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  err = read_pcm_data(fp);

  if (err)
    return err;
#if (A2DP_BURST_MODE == 1)
  if (pkt_size >= 512)
#endif
    err = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr, pcm_data, pkt_size, RSI_AUDIO_DATA_TYPE, &bytes_consumed);

#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  err = read_mp3_data(fp);
  if (err)
    return err;
  err = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr, mp3_data, pkt_size, RSI_AUDIO_DATA_TYPE, &bytes_consumed);

#endif

#elif (RSI_AUDIO_DATA_SRC == ARRAY)

#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  static int16_t ix = 0;

  err = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr,
                                      pcm_data_buff + (ix * RSI_PCM_DATA_SIZE),
                                      RSI_PCM_DATA_SIZE,
                                      RSI_AUDIO_DATA_TYPE);
  ix  = (ix > 587) ? 0 : (ix + 1);

#endif

#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return status;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif
  err = read_sbc_data();
  if (err)
    return err;

  err = rsi_bt_a2dp_send_sbc_aac_data(remote_dev_addr, sbc_data, pkt_size, 0);
#elif (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return status;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif
  err = read_pcm_data_from_sd();

  if (err)
    return err;

  if (first_read) {
#if TA_BASED_ENCODER
    err = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr, g_bufferRead, sizeof(g_bufferRead), RSI_AUDIO_DATA_TYPE);
#else
    err           = rsi_bt_sbc_encode(remote_dev_addr, g_bufferRead, sizeof(g_bufferRead), &bytes_consumed);
    bytes_to_read = bytes_consumed;
    first_read    = 0;
#endif
  } else {
#if TA_BASED_ENCODER
    err = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr, g_bufferRead, sizeof(g_bufferRead), RSI_AUDIO_DATA_TYPE);
#else
    err           = rsi_bt_sbc_encode(remote_dev_addr, g_bufferRead, bytes_to_read, &bytes_consumed);
    bytes_to_read = bytes_consumed;
#endif
  }
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
#if ENABLE_POWER_SAVE
  rsi_mutex_lock(&power_cmd_mutex);
  if (!powersave_cmd_given) {
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("failed to keep module in power save \r\n");
      return status;
    }
    powersave_cmd_given = true;
  }
  rsi_mutex_unlock(&power_cmd_mutex);
#endif
  err = read_mp3_data_from_sd();

  if (err)
    return err;
  if (first_read) {
    err           = rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr,
                                        g_bufferRead,
                                        sizeof(g_bufferRead),
                                        RSI_AUDIO_DATA_TYPE,
                                        &bytes_consumed);
    bytes_to_read = bytes_consumed;
    first_read    = 0;
  } else {
    err =
      rsi_bt_a2dp_send_pcm_mp3_data(remote_dev_addr, g_bufferRead, bytes_to_read, RSI_AUDIO_DATA_TYPE, &bytes_consumed);
    bytes_to_read = bytes_consumed;
  }
#endif
#endif

#if (A2DP_BURST_MODE == 0)
#ifdef __linux__
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  usleep(2200);
#elif (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
  usleep(3000);
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  usleep(12000);
#endif
#elif (defined RSI_M4_INTERFACE)
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  rsi_delay_ms(3);
#elif (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
  rsi_delay_ms(3);
#elif (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  rsi_delay_ms(12);
#endif
#endif

  return 0;
#else
  return err;
#endif
}
#if 0
uint8_t set_cnt = 0;
uint8_t reset_cnt = 0;
uint8_t gpio_value = 0;
void gpio_test()
{
	uint8_t ix = 0;
	while (ix < 10)
	{

		GPIO_WritePinOutput(BOARD_INITPINS_PS_OUT_GPIO, BOARD_INITPINS_PS_OUT_PIN, 1);
		rsi_delay_ms(1);
		gpio_value = GPIO_ReadPinInput(BOARD_INITPINS_PS_IN_GPIO, BOARD_INITPINS_PS_IN_PIN);
		if (gpio_value)
			set_cnt++;
		rsi_delay_ms(1);
		GPIO_WritePinOutput(BOARD_INITPINS_PS_OUT_GPIO, BOARD_INITPINS_PS_OUT_PIN, 0);
		rsi_delay_ms(1);
		gpio_value = GPIO_ReadPinInput(BOARD_INITPINS_PS_IN_GPIO, BOARD_INITPINS_PS_IN_PIN);
		if (!gpio_value)
			reset_cnt++;
		rsi_delay_ms(1);
		ix++;
	}
}
#endif

/*==============================================*/
/**
 * @fn         rsi_bt_a2dp_source_sbc_codec
 * @brief      Tests the BT Classic A2DP Source.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the A2DP Source.
 */
int32_t rsi_bt_a2dp_source_sbc_codec(void)
{
  int32_t status                         = 0;
  uint8_t str_bd_addr[BD_ADDR_ARRAY_LEN] = { 0 };
  uint8_t eir_data[64]                   = { 2, 1, 0 };

#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
  status = open_audio_file(&fp, app_song_list[app_song_list_idx]);
  if (status != RSI_SUCCESS) {
    return 0;
  }

#ifdef RMOVE_ID3_TAG
  rsi_mp3_remove_id3_tag(fp);
  LOG_PRINT("mp3 data without id3 tag\n");
#endif

#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
  status = open_audio_file_from_sd();
  if (status)
    return 0;

#else

#if (RSI_AUDIO_DATA_TYPE != PCM_AUDIO)
  LOG_PRINT("audio type combination not supported\n");
  return 0;
#endif

#endif

#ifdef RSI_UART_FLOW_CTRL_ENABLE
  rsi_cmd_uart_flow_ctrl(RSI_ENABLE);
#endif
#if RPS_LINK_KEY_SAVE
  rsi_init_link_key_list();
#endif

  //! BT register GAP callbacks:
  rsi_bt_gap_register_callbacks(NULL, //role_change
                                rsi_bt_app_on_conn,
                                rsi_bt_app_on_unbond_status,
                                rsi_bt_app_on_disconn,
#if ((RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE) && (INQUIRY_ENABLE))
                                rsi_bt_inq_response,      //NULL, //scan_resp
                                rsi_remote_name_response, //NULL, //remote_name_req
#else
                                NULL,
                                NULL,
#endif
                                NULL,                      //rsi_bt_on_passkey_display,//passkey_display
                                NULL,                      //remote_name_req+cancel
                                rsi_bt_on_confirm_request, //confirm req
                                rsi_bt_app_on_pincode_req,
                                NULL, //rsi_bt_on_passkey_request,//passkey request
#if ((RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE) && (INQUIRY_ENABLE))
                                rsi_bt_inquiry_complete, //NULL, //inquiry complete
#else
                                NULL,
#endif
                                rsi_bt_app_on_auth_complete,
                                rsi_bt_app_on_linkkey_req, //linkkey request
                                NULL,                      //rsi_bt_on_ssp_complete,//ssp coplete
                                rsi_bt_app_on_linkkey_save,
                                NULL, //get services
                                NULL,
                                rsi_bt_on_mode_change,
                                NULL,
                                rsi_bt_app_on_connection_initiated); //search service

  //! initialize the event map
  rsi_bt_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(local_dev_addr);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_bd_address: %s\r\n", rsi_6byte_dev_address_to_ascii(str_bd_addr, local_dev_addr));

  //! set the local device name
  status = rsi_bt_set_local_name(RSI_BT_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&local_name);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_name: %s\r\n", local_name.name);

  //! prepare Extended Response Data
  eir_data[3] = strlen(RSI_BT_LOCAL_NAME) + 1;
  eir_data[4] = 9;
  strcpy((char *)&eir_data[5], RSI_BT_LOCAL_NAME);
  //! set eir data
  status = rsi_bt_set_eir_data(eir_data, strlen(RSI_BT_LOCAL_NAME) + 5);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n set eir data filed, status =0x%x \n");
    return status;
  }
#if (RSI_APP_AVDTP_ROLE != INITIATOR_ROLE)
#if 0
	//! start the discover mode
	status = rsi_bt_start_discoverable();
	if(status != RSI_SUCCESS)
	{
		return status;
	}
#endif
  //Adding Device Identification
  status = rsi_bt_add_device_id(RSI_DID_SPEC_ID,
                                RSI_DID_VENDOR_ID,
                                RSI_DID_PRODUCT_ID,
                                RSI_DID_VERSION,
                                RSI_DID_PRIMARY_RECOED,
                                RSI_DID_VENDOR_ID_SOURCE);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! start the connectability mode
  status = rsi_bt_set_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#if WLAN_TRANSIENT_CASE
  bt_thread_as_pagescan = true;
#endif
#else
  //! stop the discover mode
  status = rsi_bt_stop_discoverable();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! stop the connectability mode
  status = rsi_bt_set_non_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  //! start the ssp mode
  status = rsi_bt_set_ssp_mode(1, 1);
  if (status != RSI_SUCCESS) {
    return status;
  }
  //! initialize the A2DP profile
  status = rsi_bt_a2dp_init(&audio_codec_cap);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //!Initialize host based encode
  status = rsi_bt_cmd_sbc_init();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! register the A2DP profile callback's
  rsi_bt_a2dp_register_callbacks(rsi_bt_app_on_a2dp_connect,
                                 rsi_bt_app_on_a2dp_disconnect,
                                 rsi_bt_app_on_a2dp_configure,
                                 rsi_bt_app_on_a2dp_open,
                                 rsi_bt_app_on_a2dp_start,
                                 rsi_bt_app_on_a2dp_suspend,
                                 NULL, //rsi_bt_app_on_a2dp_abort,
                                 NULL, //rsi_bt_app_on_a2dp_close,
                                 NULL, //rsi_bt_on_a2dp_encode_data,
                                 NULL, //rsi_bt_on_a2dp_pcm_data
                                 rsi_bt_on_a2dp_more_data_req,
                                 rsi_bt_app_on_a2dp_reconfig);

  //! initialize the AVRCP profile
  status = rsi_bt_avrcp_init(NULL);
  if (status != RSI_SUCCESS) {
    return status;
  }

#if AVRCP_PROFILE_ENABLE
  rsi_bt_avrcp_register_callbacks(rsi_bt_app_on_avrcp_connect,
                                  rsi_bt_app_on_avrcp_disconnect,
                                  rsi_bt_app_on_avrcp_play,
                                  rsi_bt_app_on_avrcp_pause,
                                  NULL,
                                  rsi_bt_app_on_avrcp_next,
                                  rsi_bt_app_on_avrcp_previous,
                                  rsi_bt_app_on_avrcp_vol_up,
                                  rsi_bt_app_on_avrcp_vol_down,
                                  NULL,
                                  rsi_bt_on_avrcp_notify_event);

  rsi_bt_avrcp_target_register_callbacks(rsi_bt_on_avrcp_get_cap_event,
                                         rsi_bt_on_avrcp_get_app_supp_atts_event,
                                         rsi_bt_on_avrcp_get_app_suup_att_vals_event,
                                         rsi_bt_on_avrcp_get_app_cur_att_val_event,
                                         rsi_bt_on_avrcp_set_app_cur_att_val_event,
                                         rsi_bt_on_avrcp_get_app_att_text_event,
                                         rsi_bt_on_avrcp_get_app_att_vals_text_event,
                                         NULL, //rsi_bt_on_avrcp_supp_char_sets_event,
                                         NULL, //rsi_bt_on_avrcp_batt_status_event,
                                         rsi_bt_on_avrcp_get_song_atts_event,
                                         rsi_bt_on_avrcp_get_play_status_event,
                                         rsi_bt_on_avrcp_reg_notify_event,
                                         rsi_bt_on_avrcp_set_abs_vol_event,
                                         NULL,
                                         NULL,
                                         NULL);

#endif

#if RSI_BT_AVDTP_STATS
  //! BT AVDTP Stats callback
  rsi_bt_avdtp_events_register_callbacks(rsi_bt_on_avdtp_stats_event);

  //! set AVDTP Stats enable in controller, for receiving AVDTP Stats
  status = rsi_bt_vendor_avdtp_stats_enable(AVDTP_STATS_ENABLE, AVDTP_STATS_UPDATE_RATE);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
#if RSI_BT_AUTO_RATE
  rsi_bt_ar_events_register_callbacks(rsi_bt_on_ar_stats_event);
  status = rsi_bt_vendor_ar_enable(RSI_BT_AUTO_RATE_ENABLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("AR Enable failed\n");
    return status;
  }
#endif

#if RSI_PACKET_CHANGE_INDICATION
  //! BT PKT CHNAGE Stats callback
  rsi_bt_pkt_change_events_register_callbacks(rsi_bt_pkt_change_stats_event);
#endif

#if RSI_BT_MEMORY_STATS
  //! Registering Memory stats Callback
  rsi_bt_on_chip_memory_status_callbacks_register(rsi_bt_on_chip_memory_status_event);

  //! enabling Memory stats
  rsi_memory_stats_enable(BT_STATS, STATS_ENABLE, MEMORY_STATS_INTERVAL);
#endif

#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if INQUIRY_ENABLE
  //! start bt inquiry after 5sec of ble and ant activities
  if (rsi_ble_running || rsi_ant_running) {
    rsi_semaphore_wait(&bt_inquiry_sem, 0);

    //! wait for 5sec before starting bt inquiry
    rsi_delay_ms(5000);
  }

  inq_responses_count = 0;
  //! Start inquiry
  status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
  if (status != 0) {
    LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
  }
  LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
#if RUN_TIME_BT_DISABLE
#if ERROR_TEST_CASE
  //! disable the bt protocol
  status = rsi_switch_proto(0, switch_proto_async);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n failed to disable bt protocol %d\r\n", status);
    //return status;
  } else {
    LOG_PRINT("\r\n disable bt protocol triggered successfully \r\n");
  }
#endif
#endif
#if INQUIRY_CONNECTION_SIMULTANEOUS
  status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, RSI_BT_REMOTE_BD_ADDR));
  if (status != RSI_SUCCESS) {
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
#endif
#else
  status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, RSI_BT_REMOTE_BD_ADDR));
  if (status != RSI_SUCCESS) {
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
#endif
#endif

  return 0;
}
#if RUN_TIME_PKT_TYPE_CHANGE
uint16_t determine_mtu_size_based_on_packet_type(uint32_t pkt_type)
{

  uint16_t size = DEFAULT_MTU_SIZE;      //! Default.
  if (pkt_type == PTYPE_1MBPS_MODE_ONLY) //! BR
  {
    size = DEFAULT_MTU_SIZE;
  } else //! EDR
  {
    if (pkt_type == PTYPE_2DH3_MAY_BE_USED) {
      size = BT_2DH3_PAYLOAD_MAX_LEN;
    } else if (pkt_type == PTYPE_2DH5_MAY_BE_USED) {
      size = BT_2DH5_PAYLOAD_MAX_LEN;
    } else if (pkt_type == PTYPE_3DH3_MAY_BE_USED) {
      size = BT_3DH3_PAYLOAD_MAX_LEN;
    } else if (pkt_type == PTYPE_3DH5_MAY_BE_USED) {
      size = BT_3DH5_PAYLOAD_MAX_LEN;
    } else {
      LOG_PRINT("wrong packet type given\n");
    }
  }

  if (size > headset_max_mtu_size) //! if size is more than headset mtu size, return headset mtu size
  {
    size = headset_max_mtu_size;
  }
  return size;
}
#endif

/*==============================================*/
/**
 * @fn         rsi_app_bt_task_WIFI_6
 * @brief      this function is used to handle the BT events.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is used to check the bt events and process the events
 */
int32_t rsi_bt_app_task()
{
  int32_t status         = 0;
  int32_t temp_event_map = 0;
  uint8_t ix;
#if (RSI_AUDIO_DATA_TYPE == SBC_AUDIO)
  FILE *fp = NULL;
#endif
  uint32_t file_size = 0;

#if RUN_TIME_BT_DISABLE
#if TEST_CASE_6
  if (!powersave_command_given) {
    LOG_PRINT("\n Triggering the Sleep:debug \n");
    status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    powersave_command_given = 1;
  }
  LOG_PRINT("\n in deepsleep mode \n");
  //! wait for 30sec
  rsi_delay_ms(30000);
#endif
#endif
//! FIXME: Workaround
//! Wait till WLAN got connected and ip done state.
#if WLAN_SYNC_REQ
  if (rsi_wlan_running) {
    LOG_PRINT("Waiting BT, Wlan to unblock\n");
    rsi_semaphore_wait(&sync_coex_bt_sem, 0);
  }
#endif
  //! enable the bt protocol
  status = rsi_switch_proto(1, NULL);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n failed to enable bt protocol \r\n");
    //return status;
  }

#if RUN_TIME_BT_DISABLE
#if TEST_CASE_5 //! Added as per gamin applied case2.
  if (!powersave_command_given) {
    LOG_PRINT("\n Triggering the Sleep:debug \n");
    status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    powersave_command_given = 1;
  }
  LOG_PRINT("\n in deepsleep mode \n");
  //! wait for 30sec
  //rsi_delay_ms(30000);
#endif
#if (TEST_CASE_1 || TEST_CASE_5 || TEST_CASE_4 || TEST_CASE_6)
  rsi_delay_ms(10000);
  //! disable the bt protocol
  status = rsi_switch_proto(0, switch_proto_async); //! Make sure without enable, disable call should not be there.
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n failed to disable bt protocol \r\n");
    //return status;
  } else {
    LOG_PRINT("\r\n disable bt protocol triggered successfully \r\n");
  }
#if TEST_CASE_4
  rsi_delay_ms(10000);
  if (!powersave_command_given) {
    LOG_PRINT("\n Triggering the Sleep:debug \n");
    status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }
    powersave_command_given = 1;
  }
  LOG_PRINT("\n in deepsleep mode \n");
  //! wait for 30sec
  //rsi_delay_ms(30000);
#endif
#if ENABLE_BACK_AFTER_10SEC
  rsi_delay_ms(10000);
  //! enable the bt protocol
  status = rsi_switch_proto(1, NULL);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n failed to enable bt protocol \r\n");
    //return status;
  } else {
    LOG_PRINT("\r\n enable bt protocol triggered successfully \r\n");
  }
#endif
#endif
#endif
#if ((TEST_CASE_1 || TEST_CASE_5 || TEST_CASE_4 || TEST_CASE_6) && ENABLE_BACK_AFTER_10SEC)
  //! BT A2DP Initialization
  status = rsi_bt_a2dp_source_sbc_codec();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n bt_a2dp init failed \r\n");
    return status;
  }
#else
#if !TEST_CASE_1
  //! BT A2DP Initialization
  status = rsi_bt_a2dp_source_sbc_codec();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n bt_a2dp init failed \r\n");
    return status;
  }
#endif
#endif
  while (1) {
    //! Application main loop
#if WLAN_TRANSIENT_CASE
    if (disable_factor_count == DISABLE_ITER_COUNT) {
      if (bt_thread_in_acceptor_connection == true) {

        //! Trigger disconnect.
        status = rsi_bt_avrcp_disconn(remote_dev_addr);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n AVRCP Disconn Failed: %x\r\n", status);
        } else {
          LOG_PRINT("\r\n AVRCP Disconn Successful\r\n");
        }

        status = rsi_bt_a2dp_disconnect(remote_dev_addr);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n A2DP Disconn Failed: %x\r\n", status);
        } else {
          LOG_PRINT("\r\n A2DP Disconn Successful\r\n");
        }
        status = rsi_bt_disconnect(remote_dev_addr);

        //status = rsi_bt_disconnect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr,RSI_BT_REMOTE_BD_ADDR));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
        } else {
          LOG_PRINT("\r\n BT_Disconnect Successful \n");
        }
      } else if (bt_thread_as_pagescan == true) {
        //! stop the connectability mode
        status = rsi_bt_set_non_connectable();
        if (status != RSI_SUCCESS) {
          printf("bt acceptor pgscan disabled failed\n");
          //return status;
        }
        printf("bt acceptor pgscan disabled\n");
        rsi_semaphore_post(&wlan_sync_coex_bt_sem);
      }
    }
#endif
    //! checking for received events
    temp_event_map = rsi_bt_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      //! wait on events
      rsi_semaphore_wait(&bt_app_sem, 0);
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {

#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
      case RSI_APP_EVENT_CONNECT: {

        //! clear the connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONNECT);

#if INQUIRY_ENABLE
#if !INQUIRY_CONNECTION_SIMULTANEOUS
        bool bt_device_found = false;
        //! Check in all devices, whether the intended slave is present or not.
        for (int i = 0; i < inq_responses_count; i++) {
          if (memcmp(&rsi_inq_resp_list[i][0],
                     rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr1, RSI_BT_REMOTE_BD_ADDR),
                     RSI_DEV_ADDR_LEN)
              == 0) {
            bt_device_found = true;
            break;
          }
#if RSI_BT_DUAL_PAIR_TEST
          if (memcmp(&rsi_inq_resp_list[i][0],
                     rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr1, RSI_BT_REMOTE_BD_ADDR_2),
                     RSI_DEV_ADDR_LEN)
              == 0) {
            bt_device_found = true;
            break;
          }
#endif
        }
#if RUN_TIME_BT_DISABLE
#if (TEST_CASE_2 || TEST_CASE_7)
        rsi_delay_ms(10000);

        //! disable the bt protocol
        status = rsi_switch_proto(0, switch_proto_async);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n failed to disable bt protocol \r\n");
          //return status;
        } else {
          LOG_PRINT("\r\n disable bt protocol triggered successfully \r\n");
        }
#if ENABLE_BACK_AFTER_10SEC
        rsi_delay_ms(10000);
        //! enable the bt protocol
        status = rsi_switch_proto(1, NULL);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n failed to enable bt protocol \r\n");
          //return status;
        } else {
          LOG_PRINT("\r\n enable bt protocol triggered successfully \r\n");
        }
#endif
#endif
#endif
#if (TEST_CASE_2 && ENABLE_BACK_AFTER_10SEC)
        {
          inq_responses_count = 0;
          //! Start inquiry
          status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
          if (status != 0) {
            LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
          }
          LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
        }
#else
#if !(TEST_CASE_2 || TEST_CASE_7)
        if (bt_device_found) {
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
          status = rsi_bt_connect(remote_dev_addr1);
          if (status != RSI_SUCCESS) {
            rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
          }
#endif
        } else {
          inq_responses_count        = 0;
          inq_resp_name_length_index = 0;
          //! Start inquiry
          status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
          if (status != 0) {
            LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
          }
          LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
        }
#endif
#endif
        break;
#endif
#endif
      }

#endif
      case RSI_APP_EVENT_CONNECTION_INITIATED: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONNECTION_INITIATED);
      } break;
      case RSI_APP_EVENT_REMOTE_NAME_REQ: {
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if INQUIRY_ENABLE
        uint8_t inq_triggered = 0;
        rsi_bt_app_clear_event(RSI_APP_EVENT_REMOTE_NAME_REQ);
        //To remove the warning statement with no effect [-Werror=unused-value]
        for (; inq_resp_name_length_index < inq_responses_count; inq_resp_name_length_index++) {
          if (!rsi_inq_resp_list_name_length[inq_resp_name_length_index]) {
            inq_triggered = 1;
            status        = rsi_bt_remote_name_request_async(&rsi_inq_resp_list[inq_resp_name_length_index][0], NULL);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n status= %x Remote name request failed \n", status);
            } else {
              LOG_PRINT("\r\n Remote Name Requested Succesfully, Response will come asynchronously\n");
            }
            break;
          }
        }
#endif
#endif
#if BT_EIR_FRIENDLY_NAME_TEST
        rsi_bt_app_set_event(RSI_APP_EVENT_CONNECT);
#else
#if (!(INQUIRY_CONNECTION_SIMULTANEOUS))
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if INQUIRY_ENABLE
        if (!inq_triggered)
#endif
#endif
          rsi_bt_app_set_event(RSI_APP_EVENT_CONNECT);
#endif
#endif
      } break;

      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

#if (RSI_APP_AVDTP_ROLE == ACCEPTOR_ROLE)
        //! stop the connectability mode
        LOG_PRINT("\r\nStopping Connectability Mode\n");
        status = rsi_bt_set_non_connectable();
        if (status != RSI_SUCCESS) {
          return status;
        }

        /* Initiate Role Switch to Become Master */
        status = rsi_bt_get_local_device_role(remote_dev_addr, &local_dev_role);
        if (local_dev_role != RSI_MASTER_ROLE) {
          rsi_bt_set_local_device_role(remote_dev_addr, RSI_MASTER_ROLE, &local_dev_role);
        }
#endif
#if WLAN_TRANSIENT_CASE
        bt_thread_in_acceptor_connection = true;
        bt_thread_as_pagescan            = false;
#endif
        //! clear the connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONNECTED);
        if (app_state & (1 << CONNECTED)) {
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
          //rsi_delay_ms(500);
          /* RSC-10334: Initiate A2DP Connection as soon as we get BOND Response */
          if (!(app_state & (1 << A2DP_CONNECTED))) {
            status = rsi_bt_a2dp_connect(remote_dev_addr);
            if (status != RSI_SUCCESS) {
              if (status == RSI_APP_ERR_SOCK_DISCONN) {
                rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
                app_state = 0;
              } else if (status == RSI_APP_ERR_A2DP_CONN_ALRDY_EXISTS) {
              } else if (status == RSI_APP_ERR_BT_SDP_RECORDS_NOT_FOUND) {
                LOG_PRINT("\n SDP Records not found and hence disconnect\n");
                status = rsi_bt_disconnect(remote_dev_addr);
                if (status != RSI_SUCCESS) {
                  LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
                } else {
                  LOG_PRINT("\r\n BT_Disconnect Successful \n");
                }
              } else
                rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);

              LOG_PRINT("\r\n a2dp conn resp status 0x%04x\n", status);
            }
          } else {
            LOG_PRINT("\r\n a2dp connection initiated by headset \n");
          }
#endif
        }
      } break;

      case RSI_APP_EVENT_LINKKEY_REQ: {
        //! linkkey request event

        //! clear the linkkey request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_REQ);
#if RPS_LINK_KEY_SAVE
        rsi_bt_event_user_linkkey_save_t peer_link;
        //rsi_ascii_dev_address_to_6bytes_rev(peer_link.dev_addr,(int8_t *) str_conn_bd_addr);
        memcpy(peer_link.dev_addr, remote_dev_addr, RSI_DEV_ADDR_LEN);
        memcpy(peer_link.local_dev_addr, local_dev_addr, RSI_DEV_ADDR_LEN);
        if (rsi_link_key_read(&peer_link)) {
          memcpy(linkkey, peer_link.linkKey, RSI_LINK_KEY_LEN);
        } else {
          memset(linkkey, 0, RSI_LINK_KEY_LEN);
        }
#endif
        if ((linkkey[0] == 0) && (linkkey[15] == 0)) {
          LOG_PRINT("\r\n linkkey request negative reply\r\n");
          status = rsi_bt_linkkey_request_reply(remote_dev_addr, NULL, 0);
        } else {
          LOG_PRINT("\r\n linkkey request positive reply\r\n");
          status = rsi_bt_linkkey_request_reply(remote_dev_addr, linkkey, 1);
        }
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" rsi_bt_linkkey_request_reply failed 0x%x\n", status);
        }
#if 0
        for(int i = 0; i < RSI_LINK_KEY_LEN; i++) {
            LOG_PRINT( "0x%02x ", linkkey[i]);
        }
        LOG_PRINT("\n");
#endif
      } break;

      case RSI_APP_EVENT_PINCODE_REQ: {
        //! pincode request event
        uint8_t *pin_code = (uint8_t *)PIN_CODE;

        //! clear the pincode request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PINCODE_REQ);

        //! sending the pincode requet reply
        status = rsi_bt_pincode_request_reply(remote_dev_addr, pin_code, 1);
        if (status != RSI_SUCCESS) {
          //return status;
        }
      } break;

      case RSI_APP_EVENT_LINKKEY_SAVE: {
        //! linkkey save event

        //! clear the linkkey save event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_SAVE);
#if 0
        for(int i = 0; i < RSI_LINK_KEY_LEN; i++) {
            LOG_PRINT( "0x%02x ", linkkey[i]);
        }
        LOG_PRINT("\n");
#endif
      } break;
      case RSI_APP_EVENT_AUTH_COMPLT: {
        //! authentication complete event

        //! clear the authentication complete event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_AUTH_COMPLT);
        if ((app_state & (1 << CONNECTED)) && (app_state & (1 << AUTHENTICATED))) {
#if 0
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
          rsi_delay_ms(500);
          /*RSC-8818: While in Reconnection, When DUT Initiates PHY Level Connection to the Remote Device,
	   * Some Devices like Samsung Level U Headset are initiating A2DP Profile Level Connection immediatley
	   * after authetication completes. Therefore, if the A2DP_CONNECTED state sets in app_state, DUT shouldn't 
	   * initiate A2DP Profile Conection one more time*/
          if (!(app_state & (1 << A2DP_CONNECTED))) {
            status = rsi_bt_a2dp_connect(remote_dev_addr);
            if (status != RSI_SUCCESS) {
              if (status == RSI_APP_ERR_SOCK_DISCONN) {
                rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
                app_state = 0;
              } else if (status == RSI_APP_ERR_A2DP_CONN_ALRDY_EXISTS) {
	      } else if (status == RSI_APP_ERR_BT_SDP_RECORDS_NOT_FOUND) {
		      LOG_PRINT("\n SDP Records not found and hence disconnect\n");
		      status = rsi_bt_disconnect(remote_dev_addr);
		      if (status != RSI_SUCCESS) {
			      LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
		      } else {
			      LOG_PRINT("\r\n BT_Disconnect Successful \n");
		      } 
              } else
                rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);

              LOG_PRINT("\r\n a2dp conn resp status 0x%04x\n", status);
            }
          } else {
            LOG_PRINT("\r\n a2dp connection initiated by headset \n");
          }
#endif
#endif
        }
      } break;

      case RSI_APP_EVENT_UNBOND_STATUS: {
        //! remote device unbond event

        //! clear the unbond event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_UNBOND_STATUS);

        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
        rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);

        app_state = 0;
      } break;

      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnect event
#if WLAN_TRANSIENT_CASE
        //! clear the disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);

        rsi_semaphore_post(&wlan_sync_coex_bt_sem);

        bt_thread_in_acceptor_connection = false;
        //bt_thread_as_pagescan = true;
        printf("bt acceptor disconnect done\n");
        rsi_semaphore_wait(&sync_coex_bt_sem, 0);
#endif
#if (RSI_APP_AVDTP_ROLE == ACCEPTOR_ROLE)
        //! set the connectability mode
        LOG_PRINT("\r\nSetting Connectable Mode\n");
        status = rsi_bt_set_connectable();
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif

        //! clear the disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
        app_state        = 0;
        play_pause_count = 0;
        rem_mtu_size     = DEFAULT_MTU_SIZE;
        glbl_play_status = PLAYER_STATUS_PLAY;
        app_reg_events   = 0;
#if (RSI_BT_DUAL_PAIR_TEST == 1)
        if (dual_pair == 1) {
          dual_pair = 0;
        } else {
          dual_pair = 1;
        }
#endif

#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
#if RUN_TIME_BT_DISABLE
#if TEST_CASE_3
        if (!bt_disable_triggered) {
#endif
#endif
          LOG_PRINT("\r\n looking for device\n");
#if (RSI_BT_DUAL_PAIR_TEST == 1)
          if (dual_pair == 1) {
            status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, RSI_BT_REMOTE_BD_ADDR));
          } else {
            status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, RSI_BT_REMOTE_BD_ADDR_2));
          }
#else
        status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, RSI_BT_REMOTE_BD_ADDR));
#endif
          if (status != RSI_SUCCESS) {
            if (status == RSI_APP_ERR_LIMITED_RESOURCE) {
              LOG_PRINT("\r\n Retry for conn !!\n");
              rsi_delay_ms(5);
              rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
            } else if (status == RSI_APP_ERR_ACL_CONN_ALREADY_EXISTS) {
              app_state |= (1 << CONNECTED);
              //app_state |= (1 << AUTHENTICATED);
              rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
              LOG_PRINT("\r\n Already conn \n");
            } else
              rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
          } else {
            LOG_PRINT("\r\n reconnect status = %x \n", status);
          }
#if RUN_TIME_BT_DISABLE
#if TEST_CASE_3
        }

#endif
#endif
#endif
      } break;
      case RSI_APP_EVENT_CONFIRM_REQUEST: {
        //! confirm req event

        //! clear the ssp confirmation event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONFIRM_REQUEST);

        rsi_bt_accept_ssp_confirm(remote_dev_addr);
      } break;

      case RSI_APP_EVENT_A2DP_CONN: {
        //! a2dp connected event

        //! clear the a2dp connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_CONN);
      } break;

      case RSI_APP_EVENT_A2DP_DISCONN: {
        //! a2dp disconnected event
        //! clear the a2dp disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_DISCONN);
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
        reset_audio_params();
#endif
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
        rsi_delay_ms(500);
        if ((app_state & (1 << CONNECTED)) && (app_state & (1 << AUTHENTICATED))
            && (!(app_state & (1 << A2DP_STREAM_START)))) {
          LOG_PRINT("trying for a2dp reconn\n");
          status = rsi_bt_a2dp_connect(remote_dev_addr);
          if (status != RSI_SUCCESS) {
            if (status == RSI_APP_ERR_SOCK_DISCONN) {
              rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
              app_state = 0;
            } else if (status == RSI_APP_ERR_A2DP_CONN_ALRDY_EXISTS) {
            } else if (status == RSI_APP_ERR_BT_SDP_RECORDS_NOT_FOUND) {
              LOG_PRINT("\n SDP Records not found and hence disconnect\n");
              status = rsi_bt_disconnect(remote_dev_addr);
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
              } else {
                LOG_PRINT("\r\n BT_Disconnect Successful \n");
              }
            } else
              rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
            LOG_PRINT("\r\na2dp conn resp status 0x%04x\r\n", status);
          }
        }
#endif
      } break;

      case RSI_APP_EVENT_A2DP_CONFIGURE: {
        //! a2dp configured event
        //! clear the a2dp configured event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_CONFIGURE);
      } break;

      case RSI_APP_EVENT_A2DP_OPEN: {
        //! a2dp open event

        //! clear the a2dp open event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_OPEN);
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO) || (RSI_AUDIO_DATA_TYPE == MP3_AUDIO))
        /* Get Config Command */
        rsi_bt_a2dp_get_config(remote_dev_addr, &sbc_resp_cap);
        LOG_PRINT("\r\nOLD_CONFIG: ChannelMode=%d | Freq=%d | ",
                  sbc_resp_cap.sbc_cap.ChannelMode,
                  sbc_resp_cap.sbc_cap.Freq);
        LOG_PRINT("AllocMethod=%d | SubBands=%d | ", sbc_resp_cap.sbc_cap.AllocMethod, sbc_resp_cap.sbc_cap.SubBands);
        LOG_PRINT("MinBitPool=%d | MaxBitPool=%d | ", sbc_resp_cap.sbc_cap.MinBitPool, sbc_resp_cap.sbc_cap.MaxBitPool);
        LOG_PRINT("BlockLength=%d\n", sbc_resp_cap.sbc_cap.BlockLength);
        memcpy(&set_sbc_cap, &sbc_resp_cap.sbc_cap, sizeof(set_sbc_cap));
        /* Use Macros from rsi_sbc_codec.h */
        set_sbc_cap.Freq        = SBC_SAMPLING_FREQ_44100;
        set_sbc_cap.ChannelMode = SBC_CHANNEL_MODE_JOINT_STEREO;
        set_sbc_cap.AllocMethod = SBC_ALLOCATION_LOUDNESS;
        set_sbc_cap.SubBands    = SBC_SUBBANDS_8;
        set_sbc_cap.BlockLength = SBC_BLOCK_LENGTH_16;
        set_sbc_cap.MaxBitPool  = 35;

        rsi_delay_ms(500);
        status = rsi_bt_a2dp_set_config(remote_dev_addr, &set_sbc_cap, NULL);
        if (status == RSI_APP_ERR_A2DP_SBC_SAME_CODEC_PARAMS) {
          if (!(app_state & (1 << A2DP_STREAM_START))) {
#if !(A2DP_BT_ONLY_CONNECTION)
            rsi_delay_ms(1000);
            status = rsi_bt_a2dp_start(remote_dev_addr);
#endif
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\na2dp start resp %x\n", status);
            }
          }
        }
#else

        if (!(app_state & (1 << A2DP_STREAM_START))) {
#if !(A2DP_BT_ONLY_CONNECTION)
          rsi_delay_ms(1000);
          status = rsi_bt_a2dp_start(remote_dev_addr);
#endif
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\na2dp start resp %x\n", status);
          }
        }
#endif
      } break;
      case RSI_APP_EVENT_A2DP_START: {
        //! a2dp start event
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO) || (RSI_AUDIO_DATA_TYPE == MP3_AUDIO))
        /* Get Config Command */
        rsi_bt_a2dp_get_config(remote_dev_addr, &sbc_resp_cap);
        LOG_PRINT("\r\nNEW_CONFIG: ChannelMode=%d | Freq=%d | ",
                  sbc_resp_cap.sbc_cap.ChannelMode,
                  sbc_resp_cap.sbc_cap.Freq);
        LOG_PRINT("AllocMethod=%d | SubBands=%d | ", sbc_resp_cap.sbc_cap.AllocMethod, sbc_resp_cap.sbc_cap.SubBands);
        LOG_PRINT("MinBitPool=%d | MaxBitPool=%d | ", sbc_resp_cap.sbc_cap.MinBitPool, sbc_resp_cap.sbc_cap.MaxBitPool);
        LOG_PRINT("BlockLength=%d\n", sbc_resp_cap.sbc_cap.BlockLength);
        /* ReInit SBC */
        memcpy(&set_sbc_cap, &sbc_resp_cap.sbc_cap, sizeof(rsi_bt_a2dp_sbc_codec_cap_t));
        rsi_bt_cmd_sbc_reinit((void *)&set_sbc_cap);
#if (!TA_BASED_ENCODER)
        /* Get Number of Channels */
        if (set_sbc_cap.ChannelMode == SBC_CHANNEL_MODE_MONO)
          no_of_channels = 1;
        else
          no_of_channels = 2;

        /* Get Number of Blocks */
        if (set_sbc_cap.BlockLength == SBC_BLOCK_LENGTH_4)
          no_of_blocks = 4;
        else if (set_sbc_cap.BlockLength == SBC_BLOCK_LENGTH_8)
          no_of_blocks = 8;
        else if (set_sbc_cap.BlockLength == SBC_BLOCK_LENGTH_12)
          no_of_blocks = 12;
        else if (set_sbc_cap.BlockLength == SBC_BLOCK_LENGTH_16)
          no_of_blocks = 16;

        /* Get Number of Subbands */
        if (set_sbc_cap.SubBands == SBC_SUBBANDS_4)
          no_of_subbands = 4;
        else
          no_of_subbands = 8;

        /* Calculate PCM bytes to read according to SBC codec config */
        frame_size          = calculate_sbc_frame_size((rsi_bt_a2dp_sbc_codec_cap_t *)&(set_sbc_cap));
        no_of_sbc_frames    = (rem_mtu_size - PKT_HEADER_AND_CRC - RTP_HEADER) / frame_size;
        bytes_to_read       = no_of_sbc_frames * (no_of_blocks * no_of_subbands * no_of_channels * 2);
        first_buff_overflow = 1;
#endif
#endif
#ifdef RSI_BT_DYNAMIC_PWR_INDEX
        status = rsi_bt_vendor_dynamic_pwr(RSI_ENABLE, remote_dev_addr, GAIN_INDEX_BR, GAIN_INDEX_2M, GAIN_INDEX_3M);
#endif
        //! clear the a2dp start event.
        status = rsi_bt_get_local_device_role(remote_dev_addr, &local_dev_role);
        if (local_dev_role == RSI_MASTER_ROLE) {
          LOG_PRINT("\nIn MASTER Role\n");
        } else {
          LOG_PRINT("\nIn SLAVE Role\n");
        }

#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
        rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
#else
        rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#endif
        /* Skullcandy Ink'd headset is initiating AVRCP conn even after we initiate avrcp conn req, 
					 Due to that, headset is not able to initiate play/pause events on time.
					 So maintaining enough time for remote device to initiate AVRCP conn since as source we should initiate AVRCP with least priority*/
        rsi_delay_ms(1000);
        if (!(app_state & (1 << AVRCP_CONNECTED))) {
          LOG_PRINT("\nInitiating AVRCP Connection\n");
          status = rsi_bt_avrcp_conn(remote_dev_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\navrcp conn resp status 0x%04x\r\n", status);
            if (status == RSI_APP_ERR_SOCK_TIMEOUT) {
              break;
            }
          }
        }
        LOG_PRINT("\r\nA2DP Send Audio\n");

        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_START);

#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
        reset_audio_params();
#endif
      } break;
      case RSI_APP_EVENT_A2DP_SUSPEND: {
        //! a2dp suspend event
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SUSPEND);
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
#endif
        LOG_PRINT("\r\n a2dp suspend\n");

      } break;
      case RSI_APP_EVENT_A2DP_RECONFIG: {
        //! a2dp reconfig event
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_RECONFIG);

        if (!(app_state & (1 << A2DP_STREAM_START))) {
#if !(A2DP_BT_ONLY_CONNECTION)
          rsi_delay_ms(1000);
          status = rsi_bt_a2dp_start(remote_dev_addr);
#endif
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n a2dp start resp %x\n", status);
          }
        }
      } break;

      case RSI_APP_EVENT_A2DP_MORE_DATA_REQ: {
        //! a2dp more data event
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER) //Host Based Encoding
        if (!(app_state & (1 << CONNECTED)) && !(app_state & (1 << A2DP_CONNECTED))) {
          rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
          rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
          break;
        }
        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);

        status = rsi_bt_app_send_sbc_data();

        rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);

        if (status == RSI_SUCCESS) {
          rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
        } else if (status == RSI_APP_ERR_A2DP_SBC_BUFF_UNDERFLOW) {
          underflow = 1;
        } else if (status == RSI_APP_ERR_HW_BUFFER_OVERFLOW) {
          overflow_count++;
#if RUN_TIME_BT_DISABLE
#if TEST_CASE_3
          if (overflow_count == 200) {
            overflow_count = 0;

            status = rsi_bt_avrcp_disconn(remote_dev_addr);

            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n AVRCP Disconn Failed: %x\r\n", status);
            } else {
              LOG_PRINT("\r\n AVRCP Disconn Successful\r\n");
            }

            status = rsi_bt_a2dp_disconnect(remote_dev_addr);

            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n A2DP Disconn Failed: %x\r\n", status);
            } else {
              LOG_PRINT("\r\n A2DP Disconn Successful\r\n");
            }
            status = rsi_bt_disconnect(remote_dev_addr);

            //status = rsi_bt_disconnect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr,RSI_BT_REMOTE_BD_ADDR));
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
            } else {
              LOG_PRINT("\r\n BT_Disconnect Successful \n");

              rsi_delay_ms(10000);
              //! Disable BT here!
              //! disable the bt protocol
              status = rsi_switch_proto(0, switch_proto_async);
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\r\n failed to disable bt protocol %d\r\n", status);
                //return status;
              } else {
                LOG_PRINT("\r\n disable bt protocol triggered successfully \r\n");
                bt_disable_triggered = 1;
              }
              //! Wait some time and Enable back. and set the event.
#if ENABLE_BACK_AFTER_10SEC
              rsi_delay_ms(5000);
              //! enable the bt protocol
              status = rsi_switch_proto(1, NULL);
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\r\n failed to enable bt protocol \r\n");
                //return status;
              } else {
                LOG_PRINT("\r\n enable bt protocol triggered successfully \r\n");
                bt_disable_triggered = 0;
                //overflow_count = 0;
                app_state = 0;
                rsi_delay_ms(5000);
                //rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);

                /* Re-Initializing A2DP */
                status = rsi_bt_a2dp_init(&audio_codec_cap);
                if (status != RSI_SUCCESS) {
                  LOG_PRINT("\r\n RE-ENABLE: A2DP INIT FAILED \r\n");
                  return status;
                } else {
                  LOG_PRINT("\r\n RE-ENABLE A2DP INIT SUCCESS \r\n");
                }

                //! initialize the AVRCP profile
                status = rsi_bt_avrcp_init(NULL);
                if (status != RSI_SUCCESS) {
                  LOG_PRINT("\r\n RE-ENABLE: AVRCP INIT FAILED \r\n");
                  return status;
                } else {
                  LOG_PRINT("\r\n RE-ENABLE AVRCP INIT SUCCESS \r\n");
                }
              }
#endif
            }
          }
#endif
#endif
          //Do Nothing
        } else if (status == RSI_APP_ERR_HW_BUFFER_OVERFLOW_TIMEOUT) {
          LOG_PRINT("\r\n Buffer Overflow Timeout: 0x%x \n", status);
          rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
        } else if (status == RSI_APP_ERR_A2DP_INVALID_SOCKET) {
          LOG_PRINT("\r\n Invalid Socket Error: 0x%x\n", status);
          /*This invalid socket error comes when there is no l2cap cfg response from remote device for stream socket.
            i.e, A2DP stream socket conn is not completed although A2DP Conn socket is completed.*/
          LOG_PRINT("\r\n Initiating profile level reconn: \r\n");
          status = rsi_bt_a2dp_disconnect(remote_dev_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n A2DP Disconn Failed: %x\r\n", status);
          }
          app_state &= ~(1 << A2DP_STREAM_START);
          break;
        } else {
          first_buff_overflow = 1;
          LOG_PRINT("\r\n send SBC command status: 0x%x \n", status);
        }

#else
        if (!(app_state & (1 << CONNECTED)) && !(app_state & (1 << A2DP_CONNECTED))) {
          rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
          break;
        }
        //			if ((play_pause_count % 2) == 1)
        //				break;

#if A2DP_BURST_MODE
        aud_pkts_sent = 0;
        do {
#endif
          rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
          //! Send audio data
          status = send_audio_data(fp);

          if (status == RSI_APP_ERR_AUDIO_EOF) {
            //reset_audio_params ();
            //! close current and open next song
            if (app_song_list_idx == (SONG_LIST_MAX_COUNT - 1))
              app_song_list_idx = 0;
            else
              app_song_list_idx++;
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
            close_audio_file(&fp);

            status = open_audio_file(&fp, app_song_list[app_song_list_idx]);
            if (status != RSI_SUCCESS) {
              return 0;
            }

#ifdef RMOVE_ID3_TAG
            rsi_mp3_remove_id3_tag(fp);
            LOG_PRINT("\r\n mp3 data without id3 tag\n");
#endif

#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
          status = f_close(&g_fileObject_sd);

          status = open_audio_file_from_sd();
          if (status != RSI_SUCCESS) {
            return 0;
          }
#endif
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
          } else if (status == RSI_SUCCESS) {
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
          } else if (status == RSI_APP_ERR_A2DP_NOT_STREAMING) {
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
            LOG_PRINT("A2DP not streaming: 0x%x \r\n", status);
          } else if (status == RSI_APP_ERR_A2DP_INVALID_SOCKET) {
            LOG_PRINT("\r\n Invalid Socket Error: 0x%x\n", status);
            /*This invalid socket error comes when there is no l2cap cfg response from remote device for stream socket.
            i.e, A2DP stream socket conn is not completed although A2DP Conn socket is completed.*/
            LOG_PRINT("\r\n Initiating profile level reconn: \r\n");
            status = rsi_bt_a2dp_disconnect(remote_dev_addr);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n A2DP Disconn Failed: 0x%x\r\n", status);
            }
            app_state &= ~(1 << A2DP_STREAM_START);
            break;
          }
#if A2DP_BURST_MODE
          aud_pkts_sent++;
        } while (aud_pkts_sent < A2DP_BURST_SIZE);
#endif
#endif
      } break;
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO && !TA_BASED_ENCODER)
      case RSI_APP_EVENT_A2DP_SBC_ENCODE: {

        rsi_bt_app_clear_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);

        status = rsi_bt_app_sbc_encode();

        if (status == RSI_APP_ERR_AUDIO_EOF) {
          //reset_audio_params (); //! wlan_bt glitch.
          //! close current and open next song
          if (app_song_list_idx == (SONG_LIST_MAX_COUNT - 1))
            app_song_list_idx = 0;
          else
            app_song_list_idx++;
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
          close_audio_file(&fp);

          status = open_audio_file(&fp, app_song_list[app_song_list_idx]);
          if (status != RSI_SUCCESS) {
            return 0;
          }

#ifdef RMOVE_ID3_TAG
          rsi_mp3_remove_id3_tag(fp);
          LOG_PRINT("\n mp3 data without id3 tag\n");
#endif

#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
          status = f_close(&g_fileObject_sd);

          status = open_audio_file_from_sd();
          if (status != RSI_SUCCESS) {
            return 0;
          }
#endif
          rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
        } else if ((status == RSI_SUCCESS) || (status == RSI_APP_ERR_INVALID_INPUT)) {
          rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_SBC_ENCODE);
          if (underflow) {
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
            underflow = 0;
          }
        } else if (status == RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW) {
          if (first_buff_overflow) {
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
            first_buff_overflow = 0;
          }
        }
      } break;
#endif
      case RSI_APP_EVENT_AVRCP_CONN: {
        //! avrcp conn event

        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_CONN);
#if (RSI_APP_AVDTP_ROLE != ACCEPTOR_ROLE)
        if (!(app_state & (1 << A2DP_CONNECTED))) {
          rsi_delay_ms(500);
          status = rsi_bt_a2dp_connect(remote_dev_addr);
          if (status != RSI_SUCCESS) {
            if (status == RSI_APP_ERR_SOCK_DISCONN) {
              rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
              app_state = 0;
            } else if (status == RSI_APP_ERR_A2DP_CONN_ALRDY_EXISTS) {
            } else if (status == RSI_APP_ERR_BT_SDP_RECORDS_NOT_FOUND) {
              LOG_PRINT("\n SDP Records not found and hence disconnect\n");
              status = rsi_bt_disconnect(remote_dev_addr);
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\r\n BT_Disconnect status: 0x%x \n", status);
              } else {
                LOG_PRINT("\r\n BT_Disconnect Successful \n");
              }
            } else
              rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
            LOG_PRINT("\r\n a2dp conn resp status 0x%04x\r\n", status);
          }
        } else if (app_state & (1 << A2DP_STREAM_START)) {
          rsi_delay_ms(500);
        } else
          LOG_PRINT("\r\nwaiting for a2dp config\n");
#endif
        rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_GET_REM_CAP);
      } break;

      case RSI_APP_EVENT_AVRCP_DISCONN: {
        //! avrcp disconn event

        //! clear the avrcp disconn event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_DISCONN);
        app_reg_events = 0;
      } break;

      case RSI_APP_EVENT_AVRCP_PAUSE: {
        notify_val_t notify = { 0 };
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_PAUSE);
        LOG_PRINT("\r\n avrcp pause\r\n");
        glbl_play_status = PLAYER_STATUS_PAUSE;
        for (int i = 0; i < MAX_REG_EVENTS; i++) {
          if ((app_reg_events != 0) && (app_event_id[i] == AVRCP_EVENT_PLAYBACK_STATUS_CHANGED)) {
            notify.player_status = glbl_play_status;
            rsi_bt_avrcp_notify(remote_dev_addr, 1, &notify);
            app_event_id[i] = 0;
            app_reg_events--;
            break;
          }
        }
        rsi_bt_app_a2dp_pause();
      } break;
      case RSI_APP_EVENT_AVRCP_PLAY: {
        notify_val_t notify = { 0 };
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_PLAY);
        LOG_PRINT("\r\n avrcp play\r\n");
        if (!(app_state & (1 << A2DP_STREAM_START)) && (glbl_play_status == PLAYER_STATUS_PAUSE)) {
          glbl_play_status = PLAYER_STATUS_PLAY;
          for (int i = 0; i < MAX_REG_EVENTS; i++) {
            if ((app_reg_events != 0) && (app_event_id[i] == AVRCP_EVENT_PLAYBACK_STATUS_CHANGED)) {
              notify.player_status = glbl_play_status;
              rsi_bt_avrcp_notify(remote_dev_addr, 1, &notify);
              app_event_id[i] = 0;
              app_reg_events--;
              break;
            }
          }
#if !(A2DP_BT_ONLY_CONNECTION)
          rsi_bt_app_a2dp_resume();
#endif
        }
      } break;

      case RSI_APP_EVENT_AVRCP_NEXT:
      case RSI_APP_EVENT_AVRCP_PREVIOUS: {
        //! clear the avrcp next/prev receive event.
        if (temp_event_map == RSI_APP_EVENT_AVRCP_NEXT) {
          LOG_PRINT("\r\n avrcp next\r\n");
          rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_NEXT);
          if (app_song_list_idx == (SONG_LIST_MAX_COUNT - 1))
            app_song_list_idx = 0;
          else
            app_song_list_idx++;

        } else if (temp_event_map == RSI_APP_EVENT_AVRCP_PREVIOUS) {
          LOG_PRINT("\r\n avrcp previous\r\n");
          rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_PREVIOUS);
          if (app_song_list_idx == 0)
            app_song_list_idx = (SONG_LIST_MAX_COUNT - 1);
          else
            app_song_list_idx--;
        }
        rsi_bt_app_a2dp_resume();
        // Reset audio paramters
        reset_audio_params();

        //! close current song and open next song
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
        close_audio_file(&fp);
        status = open_audio_file(&fp, app_song_list[app_song_list_idx]);

        if (status != RSI_SUCCESS) {
          return 0;
        }

#ifdef RMOVE_ID3_TAG
        rsi_mp3_remove_id3_tag(fp);
        LOG_PRINT("\r\n mp3 data withou id3 tag\n");
#endif
#elif (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
        status = f_close(&g_fileObject_sd);
        status = open_audio_file_from_sd();
        if (status != RSI_SUCCESS) {
          return 0;
        }
#endif
        notify_val_t notify = { 0 };
        glbl_play_status    = PLAYER_STATUS_PLAY;
        for (int i = 0; i < MAX_REG_EVENTS; i++) {
          if ((app_reg_events != 0) && (app_event_id[i] == AVRCP_EVENT_PLAYBACK_STATUS_CHANGED)) {
            notify.player_status = glbl_play_status;
            rsi_bt_avrcp_notify(remote_dev_addr, 1, &notify);
            app_event_id[i] = 0;
            app_reg_events--;
          }
        }
      } break;
      case RSI_APP_EVENT_AVRCP_VOL_UP: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_VOL_UP);
      } break;
      case RSI_APP_EVENT_AVRCP_VOL_DOWN: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_VOL_DOWN);
      } break;
      case RSI_APP_EVENT_AVRCP_GET_CAP: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_GET_CAP);
#if 1
        uint32_t cap[3] = { 0 };
        uint8_t cap_cnt = 0;
        switch (app_cap_type) {
          case AVRCP_CAP_ID_COMPANY_ID:
            cap[cap_cnt++] = 0x1958;
            break;
          case AVRCP_CAP_ID_EVENTS_SUPPORTED:
            cap[cap_cnt++] = AVRCP_EVENT_PLAYBACK_STATUS_CHANGED;
            cap[cap_cnt++] = AVRCP_EVENT_TRACK_CHANGED;
            cap[cap_cnt++] = AVRCP_EVENT_VOLUME_CHANGED;
            break;
        }
        rsi_bt_avrcp_cap_resp(remote_dev_addr, app_cap_type, cap_cnt, cap);
#endif
      } break;
      case RSI_APP_EVENT_AVRCP_GET_REM_CAP: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_GET_REM_CAP);
        get_cap_resp = (rsi_bt_rsp_avrcp_get_capabilities_t){ 0 };
        status       = rsi_bt_avrcp_get_capabilities(remote_dev_addr, AVRCP_CAP_ID_EVENTS_SUPPORTED, &get_cap_resp);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nAVRCP get_capabilities: status 0x%x\r\n", status);
        } else {
          LOG_PRINT("\nAVRCP get_cap resp nbr_ids: %d\n", get_cap_resp.nbr_ids);
        }
        if (app_state & (1 << AVRCP_CONNECTED)) {
          uint8_t playback_status = 0;
#if 0 //for Sennheiser(Momentum Tw) headset is rejecting this PLAYBACK_STATUS_CHANGED notification, hence we are not receiving pause/play
          status =
            rsi_bt_avrcp_reg_notification(remote_dev_addr, AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, &playback_status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("AVRCP Register Notification: status: 0x%x\r\n", status);
          }
          rsi_delay_ms(5);
#endif
          status = rsi_bt_avrcp_reg_notification(remote_dev_addr, AVRCP_EVENT_VOLUME_CHANGED, &abs_vol);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("AVRCP Register Notification: status: 0x%x\r\n", status);
          }
          uint8_t abs_vol_per = (abs_vol * 100) / 0x7f;
          LOG_PRINT("Absolute Volume: %d% (0x%x)\r\n", abs_vol_per, abs_vol);
        }

        rsi_bt_app_set_event(RSI_APP_EVENT_AVRCP_GET_REM_VER);
      } break;
      case RSI_APP_EVENT_AVRCP_NOTIFY: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_NOTIFY);

        status = rsi_bt_avrcp_reg_notification(remote_dev_addr, avrcp_notify.notify_val.eventid, &abs_vol);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Avrcp reg notification cmd status: 0x%x\r\n", status);
          break;
        }

        //LOG_PRINT ("notify event id: %d \r\n", avrcp_notify.notify_val.eventid);
        switch (avrcp_notify.notify_val.eventid) {
          case AVRCP_EVENT_VOLUME_CHANGED: {
            uint8_t abs_vol_per;
            uint8_t abs_vol = avrcp_notify.notify_val.notify_val.abs_vol;
            abs_vol_per     = (abs_vol * 100) / 0x7f;
            LOG_PRINT("Absolute Volume: %d% (0x%x)\r\n", abs_vol_per, abs_vol);
          } break;
        }
      } break;
      case RSI_APP_EVENT_AVRCP_GET_REM_VER: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_GET_REM_VER);
        avrcp_version = (rsi_bt_rsp_avrcp_remote_version_t){ 0 };
        status        = rsi_bt_avrcp_get_remote_version(remote_dev_addr, &avrcp_version);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nAVRCP get_remote_version: status 0x%x\r\n", status);
        }
        LOG_PRINT("\r\nAVRCP remote version: %d.%d\n",
                  ((avrcp_version.version >> 8) & 0xFF),
                  ((avrcp_version.version) & 0xFF));
      } break;
      case RSI_APP_EVENT_AVRCP_GET_ATTS: {
        uint8_t atts[5] = { 1 };

        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_GET_ATTS);
        rsi_bt_avrcp_att_list_resp(remote_dev_addr, 1, atts);
      } break;

      case RSI_APP_EVENT_AVRCP_GET_ATT_VALS: {
        uint8_t vals[5] = { 0, 1 };
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_GET_ATT_VALS);
        rsi_bt_avrcp_att_val_list_resp(remote_dev_addr, 2, vals);
      } break;

      case RSI_APP_EVENT_AVRCP_CUR_ATT_VALS: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_CUR_ATT_VALS);
      } break;

      case RSI_APP_EVENT_AVRCP_SET_ATT_VALS: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_SET_ATT_VALS);
        rsi_bt_avrcp_set_cur_att_val_resp(remote_dev_addr, 0);
      } break;

      case RSI_APP_EVENT_AVRCP_ATT_TEXT: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_ATT_TEXT);
      } break;

      case RSI_APP_EVENT_AVRCP_ATT_VALS_TEXT: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_ATT_VALS_TEXT);
      } break;
#if 0
		case RSI_APP_EVENT_AVRCP_CHAR_SET:
			rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_CHAR_SET);
			rsi_bt_avrcp_char_set_status_resp(remote_dev_addr, 0);
			break;

		case RSI_APP_EVENT_AVRCP_BATT_STATUS:
			rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_BATT_STATUS);
			rsi_bt_avrcp_batt_status_resp(remote_dev_addr, 0);
			break;
#endif
      case RSI_APP_EVENT_AVRCP_SONG_ATTS: {
        attr_list_t attr_list[BT_AVRCP_MEDIA_ATTR_ID_MAX_COUNT] = { 0 };

        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_SONG_ATTS);

        for (ix = 0; ix < app_song_atts.nbr_atts; ix++) {
          attr_list[ix].id = (uint8_t)(app_song_atts.ele_atts[ix]);
          switch (app_song_atts.ele_atts[ix]) {
            case BT_AVRCP_MEDIA_ATTR_ID_MEDIA_TITLE:
#if (RSI_AUDIO_DATA_SRC == BIN_FILE)
              attr_list[ix].attr_len = strlen(app_song_list[app_song_list_idx]);
              memcpy(&attr_list[ix].attr_val[0], app_song_list[app_song_list_idx], attr_list[ix].attr_len);
              attr_list[ix].attr_len = 400;
#endif
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_ARTIST_NAME:
              attr_list[ix].attr_len = strlen(SAMPLE_ARTIST_NAME);
              memcpy(attr_list[ix].attr_val, SAMPLE_ARTIST_NAME, attr_list[ix].attr_len);
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_ALBUM_NAME:
              attr_list[ix].attr_len = strlen(SAMPLE_ALBUM_NAME);
              memcpy(attr_list[ix].attr_val, SAMPLE_ALBUM_NAME, attr_list[ix].attr_len);
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_TRACK_NUM:
              rsi_itoa((uint32_t)app_song_list_idx, (uint8_t *)attr_list[ix].attr_val);
              attr_list[ix].attr_len = strlen((const char *)attr_list[ix].attr_val);
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_NUM_OF_TRACK:
              rsi_itoa(SONG_LIST_MAX_COUNT, attr_list[ix].attr_val);
              attr_list[ix].attr_len = strlen((const char *)attr_list[ix].attr_val);
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_GENRE:
              attr_list[ix].attr_len = strlen((const char *)SAMPLE_GENRE);
              memcpy(attr_list[ix].attr_val, SAMPLE_GENRE, strlen(SAMPLE_GENRE));
              break;

            case BT_AVRCP_MEDIA_ATTR_ID_PLAYING_TIME_MS:
              rsi_itoa(((file_size / PCM_DATA_LEN_PER_MS) * 1000), (uint8_t *)attr_list[ix].attr_val);
              attr_list[ix].attr_len = strlen((const char *)attr_list[ix].attr_val);
              break;
          }
        }
        rsi_bt_avrcp_ele_att_resp(remote_dev_addr, app_song_atts.nbr_atts, attr_list);
      } break;

      case RSI_APP_EVENT_AVRCP_PLAY_STATUS: {
        uint32_t pos, size;
        uint8_t play_status;

        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_PLAY_STATUS);
#if (RSI_AUDIO_DATA_SRC == SD_BIN_FILE)
        pos = f_tell(&g_fileObject_sd);
#endif
        size        = file_size;
        play_status = glbl_play_status;
        rsi_bt_avrcp_play_status_resp(remote_dev_addr,
                                      play_status,
                                      ((size / PCM_DATA_LEN_PER_MS) * 1000),
                                      ((pos / PCM_DATA_LEN_PER_MS) * 1000));
      } break;

      case RSI_APP_EVENT_AVRCP_REG_EVENT: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVRCP_REG_EVENT);
        for (int i = 0; i < MAX_REG_EVENTS; i++) {
          if (app_event_id[i] == AVRCP_EVENT_TRACK_CHANGED) {
            uint8_t event_data[8] = { 0 };
            rsi_bt_avrcp_reg_notify_resp(remote_dev_addr, AVRCP_EVENT_TRACK_CHANGED, sizeof(event_data), event_data);
            break;
          }
          if (app_event_id[i] == AVRCP_EVENT_PLAYBACK_STATUS_CHANGED) {
            uint8_t event_data = glbl_play_status;
            LOG_PRINT("rsi_bt_avrcp_reg_notify_resp, glbl_play_status: %d\r\n", glbl_play_status);
            rsi_bt_avrcp_reg_notify_resp(remote_dev_addr,
                                         AVRCP_EVENT_PLAYBACK_STATUS_CHANGED,
                                         sizeof(event_data),
                                         &event_data);
            break;
          }
        }
      } break;
#if RSI_BT_AUTO_RATE
      case RSI_APP_EVENT_AR_STATS: {
        LOG_PRINT("\n\nReceived AR stats ---> PreviousState: %d\n", ar_stats.current_state);
        LOG_PRINT("3M-Fail:%d, 3M-Pass:%d, 3M-PER:%d\n",
                  ar_stats.fail_count_3m,
                  ar_stats.pass_count_3m,
                  (ar_stats.fail_count_3m * 100) / (ar_stats.pass_count_3m + ar_stats.fail_count_3m));
        LOG_PRINT("2M-Fail:%d, 2M-Pass:%d, 2M-PER:%d\n",
                  ar_stats.fail_count_2m,
                  ar_stats.pass_count_2m,
                  (ar_stats.fail_count_2m * 100) / (ar_stats.pass_count_2m + ar_stats.fail_count_2m));
        rsi_bt_app_clear_event(RSI_APP_EVENT_AR_STATS);
        switch (ar_stats.reason_code) {
          case _3M_STATE_CONTINUOUS_FAIL_THRESHOLD_REACHED:
            LOG_PRINT("!!!_3M_STATE_CONTINUOUS_FAIL_THRESHOLD_REACHED\n");
            break;
          case _3M_STATE_PER_THRESHOLD_REACHED:
            LOG_PRINT("!!!_3M_STATE_PER_THRESHOLD_REACHED\n");
            break;
          case _2M_STATE_PASS_THRESHOLD_REACHED:
            LOG_PRINT("!!!_2M_STATE_PASS_THRESHOLD_REACHED\n");
            break;
          case _2M_STATE_CONTINUOUS_PASS_THRESHOLD_REACHED:
            LOG_PRINT("!!!_2M_STATE_CONTINUOUS_PASS_THRESHOLD_REACHED\n");
            break;
          case _BT_AR_STATS_PERIODIC:
            LOG_PRINT("_BT_AR_STATS_PERIODIC\n\n");
            break;
        }
        break;
      }
#endif

#if RSI_BT_AVDTP_STATS
      case RSI_APP_EVENT_AVDTP_STATS: {

        LOG_PRINT("Clock:0x%x, TxPoll:%d, TxNull:%d, RxPoll:%d, RxNull:%d\n",
                  avdtp_stats.clock,
                  avdtp_stats.tx_poll,
                  avdtp_stats.tx_null,
                  avdtp_stats.rx_poll,
                  avdtp_stats.rx_null);
        LOG_PRINT("dm1_tx:%d, dm1_rx:%d, dm3_tx:%d, dm3_rx:%d, dm5_tx:%d, dm5_rx:%d\n",
                  avdtp_stats.dm1_pkt[0],
                  avdtp_stats.dm1_pkt[1],
                  avdtp_stats.dm3_pkt[0],
                  avdtp_stats.dm3_pkt[1],
                  avdtp_stats.dm5_pkt[0],
                  avdtp_stats.dm5_pkt[1]);
        LOG_PRINT("dh1_tx:%d, dh1_rx:%d, dh3_tx:%d, dh3_rx:%d, dh5_tx:%d, dh5_rx:%d\n",
                  avdtp_stats.dh1_pkt[0],
                  avdtp_stats.dh1_pkt[1],
                  avdtp_stats.dh3_pkt[0],
                  avdtp_stats.dh3_pkt[1],
                  avdtp_stats.dh5_pkt[0],
                  avdtp_stats.dh5_pkt[1]);
        LOG_PRINT("2dh1_tx:%d, 2dh1_rx:%d, 2dh3_tx:%d, 2dh3_rx:%d, 2dh5_tx:%d, 2dh5_rx:%d\n",
                  avdtp_stats.edr_2dh1_pkt[0],
                  avdtp_stats.edr_2dh1_pkt[1],
                  avdtp_stats.edr_2dh3_pkt[0],
                  avdtp_stats.edr_2dh3_pkt[1],
                  avdtp_stats.edr_2dh5_pkt[0],
                  avdtp_stats.edr_2dh5_pkt[1]);
        LOG_PRINT("3dh1_tx:%d, 3dh1_rx:%d, 3dh3_tx:%d, 3dh3_rx:%d, 3dh5_tx:%d, 3dh5_rx:%d\n",
                  avdtp_stats.edr_3dh1_pkt[0],
                  avdtp_stats.edr_3dh1_pkt[1],
                  avdtp_stats.edr_3dh3_pkt[0],
                  avdtp_stats.edr_3dh3_pkt[1],
                  avdtp_stats.edr_3dh5_pkt[0],
                  avdtp_stats.edr_3dh5_pkt[1]);
        LOG_PRINT("CRC_Fail:%d, Retries:%d, HDR_Err:%d, Seq_Rep:%d\n",
                  avdtp_stats.no_of_crc_fail,
                  avdtp_stats.no_of_retries,
                  avdtp_stats.no_of_hdr_err,
                  avdtp_stats.no_of_seq_repeat);
        LOG_PRINT("ACK_DELAY- LT_10:%d, LT_20:%d, LT_30:%d, LT_40:%d, LT_50:%d, LT_60:%d, LT_70:%d, LT_80:%d, "
                  "LT_90:%d, LT_100:%d, GT_100:%d \n",
                  avdtp_stats.ack_delay[0],
                  avdtp_stats.ack_delay[1],
                  avdtp_stats.ack_delay[2],
                  avdtp_stats.ack_delay[3],
                  avdtp_stats.ack_delay[4],
                  avdtp_stats.ack_delay[5],
                  avdtp_stats.ack_delay[6],
                  avdtp_stats.ack_delay[7],
                  avdtp_stats.ack_delay[8],
                  avdtp_stats.ack_delay[9],
                  avdtp_stats.ack_delay[10]);

        packet_error = avdtp_stats.no_of_crc_fail + avdtp_stats.no_of_retries + avdtp_stats.no_of_hdr_err;
        packet_sent  = avdtp_stats.tx_poll + avdtp_stats.tx_null + avdtp_stats.dm1_pkt[0] + avdtp_stats.dm3_pkt[0]
                      + avdtp_stats.dm5_pkt[0] + avdtp_stats.edr_2dh1_pkt[0] + avdtp_stats.edr_2dh3_pkt[0]
                      + avdtp_stats.edr_2dh5_pkt[0] + avdtp_stats.edr_3dh1_pkt[0] + avdtp_stats.edr_3dh3_pkt[0]
                      + avdtp_stats.edr_3dh5_pkt[0];
        if (packet_error != 0) {
          packet_error_rate = (packet_error * 100) / (packet_sent);
        } else {
          packet_error_rate = 0;
        }
        ack_count = avdtp_stats.ack_delay[0] + avdtp_stats.ack_delay[1] + avdtp_stats.ack_delay[2]
                    + avdtp_stats.ack_delay[3] + avdtp_stats.ack_delay[4] + avdtp_stats.ack_delay[5]
                    + avdtp_stats.ack_delay[6] + avdtp_stats.ack_delay[7] + avdtp_stats.ack_delay[8]
                    + avdtp_stats.ack_delay[9] + avdtp_stats.ack_delay[10];

        actual_tx_count = avdtp_stats.dm1_pkt[0] + avdtp_stats.dm3_pkt[0] + avdtp_stats.dm5_pkt[0]
                          + avdtp_stats.edr_2dh1_pkt[0] + avdtp_stats.edr_2dh3_pkt[0] + avdtp_stats.edr_2dh5_pkt[0]
                          + avdtp_stats.edr_3dh1_pkt[0] + avdtp_stats.edr_3dh3_pkt[0] + avdtp_stats.edr_3dh5_pkt[0]
                          - avdtp_stats.no_of_retries;

        LOG_PRINT("AVDTP STATS: packet_errors: %d & Total Packets Sent: %d \n", packet_error, packet_sent);
        LOG_PRINT("AVDTP_STATS:packet_error_rate %d\n", packet_error_rate);
        LOG_PRINT("AVDTP_STATS: Actual TX pkts: %d Total Ack count: %d \n\n", actual_tx_count, ack_count);
#if BT_BR_EDR_ADAPTIVE
        first_pkt_type  = PTYPE_2DH3_MAY_BE_USED;
        second_pkt_type = PTYPE_2DH5_MAY_BE_USED;
        if (packet_error_rate >= PACKET_ERROR_RATE_THRESHOLD && is_first_pkt_type_set) {
          is_first_pkt_type_set = 0;
          status                = rsi_bt_change_pkt_type(remote_dev_addr, first_pkt_type);
          if (status == RSI_SUCCESS) {
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO))
#if RUN_TIME_PKT_TYPE_CHANGE
            //! Do MTU Update.
            rem_mtu_size = determine_mtu_size_based_on_packet_type(first_pkt_type);
            LOG_PRINT("first_pkt_type: pkt_error_rate %d%% changed pkt type 0x%x \n",
                      packet_error_rate,
                      first_pkt_type);
            no_of_sbc_frames = (rem_mtu_size - PKT_HEADER_AND_CRC - RTP_HEADER) / frame_size;
            bytes_to_read    = no_of_sbc_frames * 512;
            update_modified_mtu_size(rem_mtu_size); //update to encoder task
            //FIXME: Need to check
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#endif
#endif
            is_second_pkt_type_set = 1;
          }

        } else if (packet_error_rate < PACKET_ERROR_RATE_THRESHOLD && is_second_pkt_type_set) {
          is_second_pkt_type_set = 0;
          status                 = rsi_bt_change_pkt_type(remote_dev_addr, second_pkt_type);
          if (status == RSI_SUCCESS) {
#if ((RSI_AUDIO_DATA_TYPE == PCM_AUDIO))
#if RUN_TIME_PKT_TYPE_CHANGE
            //! Do MTU Update.
            rem_mtu_size = determine_mtu_size_based_on_packet_type(second_pkt_type);
            LOG_PRINT("second_pkt_type: pkt_error_rate %d%% changed pkt type 0x%x \n",
                      packet_error_rate,
                      second_pkt_type);
            no_of_sbc_frames = (rem_mtu_size - PKT_HEADER_AND_CRC - RTP_HEADER) / frame_size;
            bytes_to_read    = no_of_sbc_frames * 512;
            update_modified_mtu_size(rem_mtu_size); //update to encoder task
            //FIXME: Need to check
            rsi_bt_app_set_event(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);
#endif
#endif
            is_first_pkt_type_set = 1;
          }
        }
#endif
        rsi_bt_app_clear_event(RSI_APP_EVENT_AVDTP_STATS);
      } break;
#endif

#if RSI_BT_MEMORY_STATS
      case RSI_APP_EVENT_MEMORY_STATS_RECEIVED: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_MEMORY_STATS_RECEIVED);
        LOG_PRINT("\r\nMemory Utilization Stats  \r\n");
        LOG_PRINT("Common Pool Utilization: %d percent  \r\n", rsi_chip_bt_buffers_stats.cp_buffer_utilization);
        LOG_PRINT("ACL Pool Utilization: %d percent \r\n", rsi_chip_bt_buffers_stats.acl_buffer_utilization);
      } break;
#endif

#if RSI_PACKET_CHANGE_INDICATION
      case RSI_APP_EVENT_PACKET_CHANGE: {
        rsi_bt_app_clear_event(RSI_APP_EVENT_PACKET_CHANGE);
        LOG_PRINT("\nPkt change indication: Changed Packet BitMap: 0x%x  \r\n", rsi_bt_pkt_change.pkt_type);
      } break;
#endif
#if RUN_TIME_BT_DISABLE
      case RSI_APP_EVENT_BT_DISABLED:
        //LOG_PRINT ("\r\nBT Disabled Successfully\r\n");
        //! clear the connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_BT_DISABLED);
#if TEST_CASE_7
        {
          //! enable the bt protocol
          status = rsi_switch_proto(1, NULL);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n failed to enable bt protocol \r\n");
            //return status;
          } else {
            LOG_PRINT("\r\n enable bt protocol triggered successfully \r\n");
          }

          inq_responses_count = 0;
          //! Start inquiry
          status = rsi_bt_inquiry(INQUIRY_TYPE, INQUIRY_DURATION, MAX_NO_OF_RESPONSES);
          if (status != 0) {
            LOG_PRINT("\r\n Inquiry started failed with status : %x \n", status);
          }
          LOG_PRINT("\r\n Inquiry started with duration : %d \n", INQUIRY_DURATION);
        }
#endif
        break;
#endif
    }
  }
  return 0;
}
//#endif
