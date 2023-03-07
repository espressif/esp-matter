#ifndef RSI_BT_SBC_APIS_H
#define RSI_BT_SBC_APIS_H

#define SBC_SAMPLING_FREQ_16000 (1 << 3)
#define SBC_SAMPLING_FREQ_32000 (1 << 2)
#define SBC_SAMPLING_FREQ_44100 (1 << 1)
#define SBC_SAMPLING_FREQ_48000 (1 << 0)

#define SBC_CHANNEL_MODE_MONO         (1 << 3)
#define SBC_CHANNEL_MODE_DUAL_CHANNEL (1 << 2)
#define SBC_CHANNEL_MODE_STEREO       (1 << 1)
#define SBC_CHANNEL_MODE_JOINT_STEREO (1 << 0)

#define SBC_BLOCK_LENGTH_4  (1 << 3)
#define SBC_BLOCK_LENGTH_8  (1 << 2)
#define SBC_BLOCK_LENGTH_12 (1 << 1)
#define SBC_BLOCK_LENGTH_16 (1 << 0)

#define SBC_SUBBANDS_4 (1 << 1)
#define SBC_SUBBANDS_8 (1 << 0)

#define SBC_ALLOCATION_SNR      (1 << 1)
#define SBC_ALLOCATION_LOUDNESS (1 << 0)

#define SBC_MAX_BITPOOL 53
#define SBC_MIN_BITPOOL 2

/***********************************************************************************************************************************************/
// Error codes
/***********************************************************************************************************************************************/

#define RSI_APP_ERR_A2DP_SBC_BUFF_OVERFLOW  0x00A1
#define RSI_APP_ERR_A2DP_SBC_BUFF_UNDERFLOW 0x00A2
#define RSI_APP_ERR_INVALID_INPUT           0x00A3

void bt_evt_a2dp_more_data(void);
void bt_evt_a2dp_start(uint16_t rem_mtu_size);
void bt_evt_a2dp_config(void);
void bt_evt_a2dp_open(void);
void bt_evt_a2dp_conn(void);
void bt_evt_a2dp_disconn(void);
int16_t rsi_bt_cmd_sbc_init(void);
int16_t rsi_bt_cmd_sbc_reinit(void *sbc_cap);
int16_t rsi_bt_a2dp_sbc_encode_task(uint8_t *pcm_data, uint16_t pcm_data_len, uint16_t *bytes_consumed);
int16_t rsi_bt_cmd_a2dp_pcm_mp3_data(uint8_t *addr, uint8_t *pcm_data, uint16_t pcm_data_len, uint16_t *bytes_consumed);
int16_t rsi_bt_send_sbc_data(void);

#endif
