/****************************************************************************
FILE NAME
    atvv.h

DESCRIPTION
    google voice over ble service.
    <<Google Voice over BLE spec 1.0>>

****************************************************************************/

#ifndef _BLE_GVOB_H_
#define _BLE_GVOB_H_


#define ATVV_SERVICE_UUID  BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xAB5E0001, 0x5A21, 0x4F05, 0xBC7D, 0xAF01F617B664))
#define ATVV_CHAR_TX           BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xAB5E0002, 0x5A21, 0x4F05, 0xBC7D, 0xAF01F617B664))
#define ATVV_CHAR_AUDIO    BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xAB5E0003, 0x5A21, 0x4F05, 0xBC7D, 0xAF01F617B664))
#define ATVV_CHAR_CTL         BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xAB5E0004, 0x5A21, 0x4F05, 0xBC7D, 0xAF01F617B664))

#define ATVV_CHAR_TX_ATTR_VAL_INDEX          (2)
#define ATVV_CHAR_AUDIO_ATTR_VAL_INDEX   (4)
#define ATVV_CHAR_CTL_ATTR_VAL_INDEX        (7)


//ATV ---> Remote
#define GET_CAPS                0x0A
#define MIC_OPEN                0x0C
#define MIC_CLOSE               0x0D
#define MIC_EXTEND            0x0E

//Remote ---> ATV
#define AUDIO_STOP            0x00
#define AUDIO_START          0x04
#define START_SEARCH        0x08
#define AUDIO_SYNC            0x0A
#define CAPS_RESP              0x0B
#define MIC_OPEN_ERROR    0x0C

#define START_REASON_MIC_OPEN  0x00
#define START_REASON_PTT  0x01
#define START_REASON_HTT  0x03

#define STOP_REASON_MIC_CLOSE       0x00
#define STOP_REASON_HTT                   0x02
#define STOP_REASON_AUDIO_START   0x04
#define STOP_REASON_TIME_OUT         0x08
#define STOP_REASON_DIS_NOTIFY      0x10

void atvv_init(void);
void set_codecs_mode(uint8_t used);
void set_assis_mode(uint8_t mode);
int audio_search(void);
int audio_start(uint8_t reason, uint8_t stream);
int audio_stop(uint8_t reason);
int audio_transfer(uint8_t *buf, uint16_t len);

#endif 
