/*******************************************************************************
* @file  rsi_bt_sbc_codec.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020-2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file     rsi_bt_sbc_codec.c
 * @version  0.1
 * @date     28 Feb 2019
 *
 *
 *
 *  @brief : This file contains BT A2DP SBC encoding API's
 *
 *  @section Description  This file contains BT A2DP API's called from application
 *
 */
#include "rsi_driver.h"
#if RSI_BT_ENABLE
#include <stdint.h>
#include <string.h>
#include "sbc_encoder.h"

#include "rsi_bt_sbc_codec.h"
#include "rsi_bt_config.h"
#ifdef FRDM_K28F
#if ((BT_A2DP_SOURCE_SBC_CODEC || UNIFIED_PROTOCOL || COEX_MAX_APP || COEX_THROUGHPUT) || (defined LINUX_PLATFORM))
#if (defined LINUX_PLATFORM)
#define LOG_PRINT
#endif
#endif
#endif
#define BT_EVT_A2DP_PCM_DATA 1
#define BT_A2DP_MAX_DATA_PKT 32
#define BT_A2DP_EX_DATA_PKT  18 //Need some space to store the extra packets temporarily due to reconstruction
#define BT_ADDR_ARRAY_LEN    18
#define DEFAULT_MTU_SIZE     310
#define PKT_HEADER_AND_CRC   11 // 11 --> l2cap+controller header + crc
#define RTP_HEADER           13 // 13 --> rtp_header
#define SBC_HEADER_SIZE      4
#define SBC_SYNCWORD         0x9C

#define MAX_SBC_PKT_SIZE 1048

#define SBC_FRAME_SIZE sbc_frame_size_t
#define SKIP_SIZE      (SBC_FRAME_SIZE + RTP_HEADER + PKT_HEADER_AND_CRC)

#define HW_BUFFER_OVERFLOW 0x4057

typedef struct a2dp_info_s {
  uint16_t mtu;
  uint16_t event_map;
  SBC_ENC_PARAMS rsi_encoder;
  uint8_t addr_s[BT_ADDR_ARRAY_LEN];
} a2dp_info_t;

uint16_t pcm_offset;
static a2dp_info_t a2dp_info;
uint8_t sbc_frame_size_t;

#define PCM_AUDIO 1
#define SBC_AUDIO 2
#define MP3_AUDIO 3
#define AAC_AUDIO 4
uint32_t i_bytes_consumed;
uint32_t i_out_bytes;
#if (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
extern uint8_t mp3_data[MP3_INPUT_BUFFER_SIZE];
uint16_t mp3_cur_buf_len    = 0;
uint16_t mp3_offset         = 0;
uint16_t mp3_bytes_consumed = 0;
uint16_t mp3_frame_len      = 0;
int32_t mp3_err_code        = 0;
uint8_t pcm_data1[4700] __attribute__((aligned(4)));
#endif

#ifdef FRAME_WORK
uint8_t pcm_data_t[2560] __attribute__((aligned(4)));
uint8_t pcm_data_t[PCM_INPUT_BUFFER_SIZE] __attribute__((aligned(4)));
uint32_t pcm_cur_buf_len = 0;
uint8_t init_flag        = 0;
uint8_t send_flag        = 1;
rsi_sbc_queue_t rsi_sbc_queue;
uint16_t sbc_data_len_t[PRE_ENC_BUF_LEN] = { 0 };
uint16_t sbc_offset                      = 0;
uint8_t idx                              = 0;
uint16_t sbc_len                         = 0;
uint8_t flag;
uint8_t sbc_data_t[1100 * PRE_ENC_BUF_LEN];

typedef struct data_pkt_s {
  uint8_t used;
  uint8_t reserved;
  uint16_t data_len;
#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  uint8_t data[PCM_INPUT_BUFFER_SIZE];
#else
  uint8_t data[MP3_INPUT_BUFFER_SIZE];
#endif
  struct data_pkt_s *next;
} data_pkt_t;

data_pkt_t *p_pkt;
data_pkt_t *pcm_q_head;
data_pkt_t pcm_data_pool[BT_A2DP_MAX_DATA_PKT];
data_pkt_t *sbc_q_head;
data_pkt_t *sbc_pkt;
data_pkt_t *sbc_queue_head = NULL;
#endif

typedef struct data_pkt_s {
  uint8_t used;
  uint8_t pkt_cnt;
  uint16_t data_len;
  uint8_t data[MAX_SBC_PKT_SIZE];
  struct data_pkt_s *next;
} data_pkt_t;

typedef struct rsi_queue_s {
  uint32_t pkt_cnt;
  data_pkt_t *head;
  data_pkt_t *tail;
} rsi_queue_t;

rsi_queue_t rsi_sbc_queue;

data_pkt_t sbc_data_pool[BT_A2DP_MAX_DATA_PKT + BT_A2DP_EX_DATA_PKT];

rsi_mutex_handle_t sbc_mutex;

/**************************************************************************************/
/*==============================================*/
/**
 * @fn         void add_to_list(rsi_queue_t *rsi_queue, data_pkt_t *data_pkt)
 * @brief      Add the packet to the list
 * @param[in]  rsi_queue - pointer to q queue 
 * @param[in]  data_pkt -  poniter to a packet in queue 
 * @return     void           
 */
void add_to_list(rsi_queue_t *rsi_queue, data_pkt_t *data_pkt)
{
  rsi_queue->pkt_cnt++;
  if (rsi_queue->tail == NULL) {
    rsi_queue->head = data_pkt;
  } else {
    rsi_queue->tail->next = data_pkt;
  }
  rsi_queue->tail       = data_pkt;
  rsi_queue->tail->next = NULL;
  SL_PRINTF(SL_RSI_BT_ADD_PAKCET_TO_LIST, BLUETOOTH, LOG_INFO);
}

/**************************************************************************************/
/*==============================================*/
/**
 * @fn         void del_from_list(rsi_queue_t *rsi_queue)
 * @brief      Delete the packet from the list 
 * @param[in]  rsi_queue -  pointer to a queue
 * @return     void 	
 *             
 */
void del_from_list(rsi_queue_t *rsi_queue)
{
  rsi_queue->pkt_cnt--;
  rsi_queue->head = rsi_queue->head->next;
  if (rsi_queue->head == NULL) {
    rsi_queue->tail = NULL;
  }
  SL_PRINTF(SL_RSI_BT_DELETE_PAKCET_FROM_LIST, BLUETOOTH, LOG_INFO);
}

/**************************************************************************************/
/*==============================================*/
/**
 * @fn          static void free_data_pkt (data_pkt_t  *p_data_pool, data_pkt_t *p_data_pkt)
 * @brief       Free  "p_data_pkt"
 * @param[in]   p_data_pool - pointer to a data pool
 * @param[in]   p_data_pkt - pointer to a packet in queue
 * @param[out]  p_data_pool - pointer to a data pool
 * @return      void
 */
/***************************************************************************************/
static void free_data_pkt(data_pkt_t *p_data_pool, data_pkt_t *p_data_pkt)
{
  uint8_t ix = 0;
  if (p_data_pkt->used == 1) {
    for (ix = 0; ix < (BT_A2DP_MAX_DATA_PKT + BT_A2DP_EX_DATA_PKT); ix++) {
      if (&p_data_pool[ix] == p_data_pkt) {
        p_data_pool[ix].used = 0;
        return;
      }
    }
  }

  if (ix >= (BT_A2DP_MAX_DATA_PKT + BT_A2DP_EX_DATA_PKT)) {
    while (1)
      ;
  }
  return;
  SL_PRINTF(SL_RSI_BT_FREE_DATA_PACKET, BLUETOOTH, LOG_INFO);
}
/**************************************************************************************/
/*==============================================*/
/**
 * @fn          static data_pkt_t *alloc_data_pkt (data_pkt_t *p_data_pool)
 * @brief       Allocate data packet from pool
 * @param[in]   p_data_pool - pointer to a data pool
 * @param[out]  void 
 * @return      data pool
 * 
 */
/***************************************************************************************/
static data_pkt_t *alloc_data_pkt(data_pkt_t *p_data_pool)
{
  uint8_t ix = 0;
  for (ix = 0; ix < (BT_A2DP_MAX_DATA_PKT + BT_A2DP_EX_DATA_PKT); ix++) {
    if (p_data_pool[ix].used == 0) {
      p_data_pool[ix].used = 1;
      return (&p_data_pool[ix]);
    }
  }
  return NULL;
  SL_PRINTF(SL_RSI_BT_ALLOC_DATA_PACKET, BLUETOOTH, LOG_INFO);
}

/**************************************************************************************/
/*==============================================*/
/**
 * @fn          void bt_evt_a2dp_disconn(void)
 * @brief       Clear PCM Data Event on A2DP Disconnection 
 * @return      void
 * 
 */
/***************************************************************************************/
void bt_evt_a2dp_disconn(void)
{
  a2dp_info.mtu = DEFAULT_MTU_SIZE;
  SL_PRINTF(SL_RSI_BT_EVT_A2DP_DISCONNECT, BLUETOOTH, LOG_INFO);
}

/**************************************************************************************/
int16_t rsi_bt_app_send_sbc_data(void)
{
  data_pkt_t *sbc_head = NULL;

  int16_t send_sbc_cmd_status = RSI_SUCCESS;

  if (rsi_sbc_queue.pkt_cnt == 0) {
    return RSI_APP_ERR_A2DP_SBC_BUFF_UNDERFLOW;
  }

  rsi_mutex_lock(&sbc_mutex);
  sbc_head = rsi_sbc_queue.head;

  send_sbc_cmd_status = rsi_bt_a2dp_send_sbc_aac_data(a2dp_info.addr_s, sbc_head->data, sbc_head->data_len, 0);

  free_data_pkt((data_pkt_t *)&sbc_data_pool, sbc_head);
  del_from_list(&rsi_sbc_queue);
  rsi_mutex_unlock(&sbc_mutex);
  return send_sbc_cmd_status;
}
/*==============================================*/
/**
 * @fn          int16_t rsi_bt_a2dp_sbc_encode_task (uint8_t *pcm_data, uint16_t pcm_data_len, 
 *                                                   uint16_t *bytes_consumed)
 * @brief       Encode Raw Data
 * @param[in]   pcm_data -      Input raw audio data 
 * @param[in]   pcm_data_len -  Raw audio length in bytes
 * @param[out]  bytes_consumed - No of bytes consumed by encoder 
 * @return      0		-	Success \n
 *              Non-Zero Value	-	Failure
 */
/***************************************************************************************/
int16_t rsi_bt_a2dp_sbc_encode_task(uint8_t *pcm_data, uint16_t pcm_data_len, uint16_t *bytes_consumed)
{
  int16_t status = 0;
#if !(RSI_AUDIO_DATA_TYPE == PCM_AUDIO || RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  USED_PARAMETER(pcm_data_len); //This statement is added only to resolve compilation warning, value is unchanged
#endif
#if !(RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  USED_PARAMETER(bytes_consumed); //This statement is added only to resolve compilation warning, value is unchanged
#endif
  int16_t send_sbc_cmd_status = RSI_SUCCESS;
  data_pkt_t *sbc_pkt         = NULL;

  if (pcm_data == NULL) {
    return RSI_APP_ERR_INVALID_INPUT;
    SL_PRINTF(SL_RSI_APP_ERR_INVALID_INPUT, BLUETOOTH, LOG_ERROR);
  }

  pcm_offset = 0;

  //The extra packets generated during reconstruction must be sent out first
  while (rsi_sbc_queue.pkt_cnt >= BT_A2DP_MAX_DATA_PKT) {
    status = rsi_bt_app_send_sbc_data();
    if (status == HW_BUFFER_OVERFLOW) {
      break;
    }
  }

  sbc_pkt = (data_pkt_t *)alloc_data_pkt((data_pkt_t *)&sbc_data_pool);

  if (sbc_pkt == NULL) {
    return RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW;
    SL_PRINTF(SL_RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW, BLUETOOTH, LOG_ERROR);
  }

  sbc_pkt->data_len = 0;

#if (RSI_AUDIO_DATA_TYPE == PCM_AUDIO)
  while (pcm_offset < pcm_data_len) {
    a2dp_info.rsi_encoder.ps16PcmBuffer = (SINT16 *)(pcm_data + pcm_offset); //PCM Data Pointer
    a2dp_info.rsi_encoder.pu8Packet     = sbc_pkt->data + sbc_pkt->data_len; //SBC Data Pointer

    /* SBC Encoding */
    pcm_offset += SBC_Encoder(&(a2dp_info.rsi_encoder));

    sbc_pkt->data_len += a2dp_info.rsi_encoder.u16PacketLength;
    sbc_frame_size_t = a2dp_info.rsi_encoder.u16PacketLength;

    if (sbc_pkt->data_len > (a2dp_info.mtu - SKIP_SIZE)) {
      /* Reached MTU and add it to Queue */
      rsi_mutex_lock(&sbc_mutex);
      add_to_list(&rsi_sbc_queue, sbc_pkt);
      rsi_mutex_unlock(&sbc_mutex);
      break;
    }
  }

  if (rsi_sbc_queue.pkt_cnt >= BT_A2DP_MAX_DATA_PKT) {
    return RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW;
    SL_PRINTF(SL_RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW, BLUETOOTH, LOG_ERROR);
  }

#endif

#if (RSI_AUDIO_DATA_TYPE == MP3_AUDIO)
  memcpy(&mp3_data[mp3_cur_buf_len], pcm_data, pcm_data_len);

  mp3_cur_buf_len += pcm_data_len;

  /* Get MP3 Frame Length */
  mp3_frame_len = get_mp3_frame_len(mp3_data);

  while (mp3_frame_len < mp3_cur_buf_len) {
    if (mp3_frame_len == 0) {
      i_out_bytes      = 0;
      i_bytes_consumed = 0;
      goto PROCESS_PCM_DATA;
    }
    mp3_err_code = ia_mp3_dec_main_process(mp3_data, pcm_data1, mp3_frame_len);
    memcpy(&pcm_data_t[pcm_cur_buf_len], pcm_data1, i_out_bytes);
    pcm_cur_buf_len += i_out_bytes;
    mp3_offset += i_bytes_consumed;
PROCESS_PCM_DATA:
    while (pcm_offset < pcm_cur_buf_len) {
      init_flag = 0;
      /* Buffer upto MTU size */
      if (sbc_data_len_t[idx] >= (a2dp_info.mtu - SKIP_SIZE)) {
        if (idx < PRE_ENC_BUF_LEN - 1) {
          idx++;
        } else {
          if (send_sbc_cmd_status == RSI_APP_ERR_HW_BUFFER_OVERFLOW) {
            init_flag = 1;
            break;
          }
          idx        = 0;
          sbc_offset = 0;
          while ((idx < PRE_ENC_BUF_LEN) && (sbc_data_len_t[idx] >= (a2dp_info.mtu - SKIP_SIZE))) {
            if (sbc_data_t[0] != SBC_SYNCWORD) {
              LOG_PRINT("\n Invalid SBC Packet \n");
              SL_PRINTF(SL_RSI_INVALID_SBC_PACKET, BLUETOOTH, LOG_ERROR);
            }
            send_sbc_cmd_status =
              rsi_bt_a2dp_send_sbc_aac_data(a2dp_info.addr_s, sbc_data_t + sbc_offset, sbc_data_len_t[idx], 0);
            if (send_sbc_cmd_status == RSI_APP_ERR_A2DP_NOT_STREAMING) {
              continue;
            }
            sbc_offset += sbc_data_len_t[idx];
            sbc_data_len_t[idx] = 0;
            idx++;
            if (send_sbc_cmd_status == RSI_APP_ERR_HW_BUFFER_OVERFLOW) {
              /* Clear More Data Request */
              //rsi_app_async_event_map &= ~BIT(RSI_APP_EVENT_A2DP_MORE_DATA_REQ);

              memcpy(sbc_data_t, sbc_data_t + sbc_offset, sbc_len - sbc_offset);
              for (int i = 0, j = idx; (i < PRE_ENC_BUF_LEN) && (j < PRE_ENC_BUF_LEN); i++, j++) {
                sbc_data_len_t[i] = sbc_data_len_t[j];
                sbc_data_len_t[j] = 0;
              }
              idx = PRE_ENC_BUF_LEN - idx;
              sbc_len -= sbc_offset;
              goto ENCODE;
            }
          }
          idx = 0;
          sbc_len -= sbc_offset;
        }
      }
ENCODE:
      a2dp_info.rsi_encoder.ps16PcmBuffer = pcm_data_t + pcm_offset; //PCM Data Pointer
      a2dp_info.rsi_encoder.pu8Packet     = sbc_data_t + sbc_len;    //SBC Data Pointer

      /* SBC Encoding */
      pcm_offset += SBC_Encoder(&(a2dp_info.rsi_encoder));

      sbc_data_len_t[idx] += a2dp_info.rsi_encoder.u16PacketLength;
      sbc_frame_size_t = a2dp_info.rsi_encoder.u16PacketLength;
      sbc_len += sbc_frame_size_t;
    }

    /* Copy the remaining PCM Data input Buffer to Zero'th location of the same */
    if (pcm_offset != 0) {
      memmove(pcm_data_t, &pcm_data_t[pcm_offset], (pcm_cur_buf_len - pcm_offset));
    }
    pcm_cur_buf_len -= pcm_offset;
    pcm_offset = 0;

    if (mp3_offset != 0) {
      memmove(mp3_data, &mp3_data[mp3_offset], (mp3_cur_buf_len - mp3_offset));
    }
    mp3_cur_buf_len -= mp3_offset;
    mp3_bytes_consumed += i_bytes_consumed;
    mp3_frame_len = get_mp3_frame_len(mp3_data);
    mp3_offset    = 0;

    if (init_flag) {
      break;
    }

    if (mp3_frame_len == 0) {
      mp3_bytes_consumed = 0;
      break;
    }
  }

  *bytes_consumed    = mp3_bytes_consumed;
  mp3_offset         = 0;
  mp3_bytes_consumed = 0;
#endif
  return send_sbc_cmd_status;
}

/**************************************************************************************/
/*==============================================*/
/**
 * @fn          int16_t rsi_bt_cmd_a2dp_pcm_mp3_data (uint8_t *addr, uint8_t *pcm_data,
 *                                                    uint16_t pcm_data_len, uint16_t *bytes_consumed)
 * @brief       Set PCM Data event
 * @param[in]   addr - Remote BD Address 
 * @param[in]   pcm_data - PCM Data 
 * @param[in]   pcm_data_len - PCM Data Length
 * @param[in]   bytes_consumed - Bytes consumed
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
***************************************************************************************/
int16_t rsi_bt_cmd_a2dp_pcm_mp3_data(uint8_t *addr, uint8_t *pcm_data, uint16_t pcm_data_len, uint16_t *bytes_consumed)
{
  int16_t err = 0;

  memcpy(a2dp_info.addr_s, addr, BT_ADDR_ARRAY_LEN);

  err = rsi_bt_a2dp_sbc_encode_task(pcm_data, pcm_data_len, bytes_consumed);

  SL_PRINTF(SL_RSI_BT_CMD_A2DP_PCM_MP3_DATA, BLUETOOTH, LOG_INFO);

  return err;
}
/**************************************************************************************
 * @fn          int16_t rsi_bt_cmd_sbc_init (void) 
 * @brief       Initialize SBC with default configuration
 * @param[in]   void
 * @return      0		-	Success \n
 *              Non-Zero Value	-	Failure 
 * 
***************************************************************************************/
int16_t rsi_bt_cmd_sbc_init(void)
{
  int8_t status = RSI_SUCCESS;

  a2dp_info.rsi_encoder.s16SamplingFreq     = SBC_sf44100;
  a2dp_info.rsi_encoder.s16ChannelMode      = SBC_JOINT_STEREO;
  a2dp_info.rsi_encoder.s16NumOfSubBands    = SUB_BANDS_8;
  a2dp_info.rsi_encoder.s16NumOfBlocks      = SBC_BLOCK_3;
  a2dp_info.rsi_encoder.s16AllocationMethod = SBC_LOUDNESS;
  a2dp_info.rsi_encoder.s16BitPool          = 53;

  SBC_Encoder_Init(&(a2dp_info.rsi_encoder));
  SL_PRINTF(SL_RSI_BT_CMD_SBC_INIT_SUCCESS, BLUETOOTH, LOG_INFO);

  status = rsi_mutex_create(&sbc_mutex);
  if (status != RSI_ERROR_NONE) {
    LOG_PRINT("failed to create mutex object, error = %d \r\n", status);
  }
  return 0;
}
/**************************************************************************************
 * @fn          int16_t rsi_bt_cmd_sbc_reinit (rsi_bt_a2dp_sbc_codec_cap_t *sbc_cap) 
 * @brief       Reinit SBC with user configuration 
 * @param[in]   sbc_cap, a pointer to a sbc capability structure
 * @param[out]  None
 * @return      
*************************************************************************************/
typedef struct rsi_bt_a2dp_sbc_codec_cap_ss {
  uint8_t ChannelMode : 4;
  uint8_t Freq : 4;
  uint8_t AllocMethod : 2;
  uint8_t SubBands : 2;
  uint8_t BlockLength : 4;
  uint8_t MinBitPool;
  uint8_t MaxBitPool;
} rsi_bt_a2dp_sbc_codec_cap_tt;

int16_t rsi_bt_cmd_sbc_reinit(void *p_sbc_cap)
{
  rsi_bt_a2dp_sbc_codec_cap_tt *sbc_cap = (rsi_bt_a2dp_sbc_codec_cap_tt *)p_sbc_cap;

  /* Sampling Frequency */
  if (((uint32_t)sbc_cap->Freq) & SBC_SAMPLING_FREQ_48000)
    a2dp_info.rsi_encoder.s16SamplingFreq = SBC_sf48000;
  else if (((uint32_t)sbc_cap->Freq) & SBC_SAMPLING_FREQ_44100)
    a2dp_info.rsi_encoder.s16SamplingFreq = SBC_sf44100;
  else if (((uint32_t)sbc_cap->Freq) & SBC_SAMPLING_FREQ_32000)
    a2dp_info.rsi_encoder.s16SamplingFreq = SBC_sf32000;
  else if (((uint32_t)sbc_cap->Freq) & SBC_SAMPLING_FREQ_16000)
    a2dp_info.rsi_encoder.s16SamplingFreq = SBC_sf16000;
  else
    LOG_PRINT("\n Frequency Mismatch \n");
  SL_PRINTF(SL_RSI_BT_FREQUENCY_MISMATCH, BLUETOOTH, LOG_ERROR);

  /* Block Length */
  if (((uint32_t)sbc_cap->BlockLength) & SBC_BLOCK_LENGTH_16)
    a2dp_info.rsi_encoder.s16NumOfBlocks = SBC_BLOCK_3;
  else if (((uint32_t)sbc_cap->BlockLength) & SBC_BLOCK_LENGTH_12)
    a2dp_info.rsi_encoder.s16NumOfBlocks = SBC_BLOCK_2;
  else if (((uint32_t)sbc_cap->BlockLength) & SBC_BLOCK_LENGTH_8)
    a2dp_info.rsi_encoder.s16NumOfBlocks = SBC_BLOCK_1;
  else if (((uint32_t)sbc_cap->BlockLength) & SBC_BLOCK_LENGTH_4)
    a2dp_info.rsi_encoder.s16NumOfBlocks = SBC_BLOCK_0;
  else
    LOG_PRINT("\n Block Length Mismatch \n");
  SL_PRINTF(SL_RSI_BT_BLOCK_LENGTH_MISMATCH, BLUETOOTH, LOG_ERROR);

  /* SubBands */
  if (((uint32_t)sbc_cap->SubBands) & SBC_SUBBANDS_8)
    a2dp_info.rsi_encoder.s16NumOfSubBands = SUB_BANDS_8;
  else if (((uint32_t)sbc_cap->SubBands) & SBC_SUBBANDS_4)
    a2dp_info.rsi_encoder.s16NumOfSubBands = SUB_BANDS_4;
  else
    LOG_PRINT("\n Subbands Mismatch \n");
  SL_PRINTF(SL_RSI_BT_SUBBANDS_MISMATCH, BLUETOOTH, LOG_ERROR);

  /* Channel Mode */
  if (((uint32_t)sbc_cap->ChannelMode) & SBC_CHANNEL_MODE_JOINT_STEREO)
    a2dp_info.rsi_encoder.s16ChannelMode = SBC_JOINT_STEREO;
  else if (((uint32_t)sbc_cap->ChannelMode) & SBC_CHANNEL_MODE_STEREO)
    a2dp_info.rsi_encoder.s16ChannelMode = SBC_STEREO;
  else if (((uint32_t)sbc_cap->ChannelMode) & SBC_CHANNEL_MODE_DUAL_CHANNEL)
    a2dp_info.rsi_encoder.s16ChannelMode = SBC_DUAL;
  else if (((uint32_t)sbc_cap->ChannelMode) & SBC_CHANNEL_MODE_MONO)
    a2dp_info.rsi_encoder.s16ChannelMode = SBC_MONO;
  else
    LOG_PRINT("\n Channel Mode Mismatch \n");
  SL_PRINTF(SL_RSI_BT_CHANNEL_MODE_MISMATCH, BLUETOOTH, LOG_ERROR);

  /* Allocation Method */
  if (((uint32_t)sbc_cap->AllocMethod) & SBC_ALLOCATION_LOUDNESS)
    a2dp_info.rsi_encoder.s16AllocationMethod = SBC_LOUDNESS;
  else if (((uint32_t)sbc_cap->AllocMethod) & SBC_ALLOCATION_SNR)
    a2dp_info.rsi_encoder.s16AllocationMethod = SBC_SNR;
  else
    LOG_PRINT("\n Allocation Method Mismatch \n");
  SL_PRINTF(SL_RSI_BT_ALLOCATION_METHOD_MISMATCH, BLUETOOTH, LOG_ERROR);

  /* BitPool */
  a2dp_info.rsi_encoder.s16BitPool = (uint8_t)sbc_cap->MaxBitPool;

  /* Reinitialize SBC Encoder with new parameters */
  SBC_Encoder_Init(&(a2dp_info.rsi_encoder));

  return 0;
}
/*==============================================*/
int16_t rsi_bt_sbc_encode(uint8_t *remote_dev_addr,
                          uint8_t *pcm_mp3_data,
                          uint16_t pcm_mp3_data_len,
                          uint16_t *bytes_consumed)
{
  int16_t err = 0;

  memcpy(a2dp_info.addr_s, remote_dev_addr, BT_ADDR_ARRAY_LEN);

  err = rsi_bt_a2dp_sbc_encode_task(pcm_mp3_data, pcm_mp3_data_len, bytes_consumed);
  SL_PRINTF(SL_RSI_SBC_ENCODE, BLUETOOTH, LOG_INFO);

  return err;
}

/*==============================================*/
void reset_audio_params(void)
{
  while (rsi_sbc_queue.head != NULL) {
    free_data_pkt((data_pkt_t *)&sbc_data_pool, rsi_sbc_queue.head);
    del_from_list(&rsi_sbc_queue);
  }
  return;
}

/*==============================================*/
void rebuild_sbc_buffer(uint16_t prev_mtu, uint16_t curr_mtu)
{
  data_pkt_t *head            = rsi_sbc_queue.head;
  data_pkt_t *new_sbc_pkt     = NULL;
  uint8_t nmbr_of_queue_nodes = rsi_sbc_queue.pkt_cnt;
  uint8_t old_node_head_idx   = 0;
  uint8_t new_node_head_idx   = 0;
  uint8_t sbc_frame_size      = a2dp_info.rsi_encoder.u16PacketLength;
  uint8_t prev_sbc_frames_cnt = 0;
  uint8_t curr_sbc_frames_cnt = 0;

  /* Calculate the maximum number of SBC frames in each packet */
  if (sbc_frame_size != 0) {
    prev_sbc_frames_cnt = (uint8_t)((prev_mtu - PKT_HEADER_AND_CRC - RTP_HEADER) / sbc_frame_size);
    curr_sbc_frames_cnt = (uint8_t)((curr_mtu - PKT_HEADER_AND_CRC - RTP_HEADER) / sbc_frame_size);
  }

  if ((prev_sbc_frames_cnt != curr_sbc_frames_cnt) && (sbc_frame_size != 0)) {
    /* Allocate a data pkt to put the first packet rebuilt */
    new_sbc_pkt = (data_pkt_t *)alloc_data_pkt((data_pkt_t *)&sbc_data_pool);
    if (new_sbc_pkt == NULL) {
      return;
    }
    new_node_head_idx = 0;

    /* Loop for all Packets in Queue */
    while (nmbr_of_queue_nodes != 0) {
      if (new_node_head_idx < curr_sbc_frames_cnt) {
        /* Copy sbc frames over */
        memcpy((new_sbc_pkt->data + (new_node_head_idx * sbc_frame_size)),
               (head->data + (old_node_head_idx * sbc_frame_size)),
               sbc_frame_size * sizeof(uint8_t));
        new_node_head_idx++;
        old_node_head_idx++;
      }

      if (new_node_head_idx == curr_sbc_frames_cnt) {
        /* Reached New MTU, add to the list */
        new_node_head_idx     = 0;
        new_sbc_pkt->data_len = curr_sbc_frames_cnt * sbc_frame_size;
        add_to_list(&rsi_sbc_queue, new_sbc_pkt);

        /* Allocate another data pkt to store the next packet rebuilt */
        new_sbc_pkt = (data_pkt_t *)alloc_data_pkt((data_pkt_t *)&sbc_data_pool);
        if (new_sbc_pkt == NULL) {
          return;
        }
        new_node_head_idx = 0;
      }

      if (old_node_head_idx == prev_sbc_frames_cnt) {
        /* The data of this packet has been copied and can be released */
        nmbr_of_queue_nodes--;
        old_node_head_idx = 0;
        free_data_pkt((data_pkt_t *)&sbc_data_pool, rsi_sbc_queue.head);
        del_from_list(&rsi_sbc_queue);
        head = head->next;
      }
    } /* end of while */

    if (new_node_head_idx != 0) {
      /* The remaining sbc frames becomes the last packet */
      new_sbc_pkt->data_len = new_node_head_idx * sbc_frame_size;
      new_node_head_idx     = 0;
      add_to_list(&rsi_sbc_queue, new_sbc_pkt);
    } else {
      /* Free unused packet */
      free_data_pkt((data_pkt_t *)&sbc_data_pool, new_sbc_pkt);
    }
  }
  return;
}

/*==============================================*/
void update_modified_mtu_size(uint16_t rem_mtu_size)
{
  if (a2dp_info.mtu != rem_mtu_size) {
    rsi_mutex_lock(&sbc_mutex);
    /* Re-Build SBC Buffer */
    rebuild_sbc_buffer(a2dp_info.mtu, rem_mtu_size);
    rsi_mutex_unlock(&sbc_mutex);
  }

#ifdef USE_REM_MTU_SIZE_ONLY
  a2dp_info.mtu = rem_mtu_size;
#else
  a2dp_info.mtu = (rem_mtu_size > DEFAULT_MTU_SIZE) ? rem_mtu_size : DEFAULT_MTU_SIZE;
#endif
  return;
}

#endif
