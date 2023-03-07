#ifndef HW_CONSTANTS_H
#define HW_CONSTANTS_H

// some magic numbers from hardware
#define BLE_PRE_DET_DUR     (40)
#define BLE_HDR_PRE_DET_DUR (30)
#define BLE_LR_PRE_DET_DUR  (150)

#define BLE_RX_TIMESTAMP_OFFSET (-94)
#define BLE_HDR_RX_TIMESTAMP_OFFSET (-51)
#define BLE_LR125_RX_TIMESTAMP_OFFSET (-686)
#define BLE_LR500_RX_TIMESTAMP_OFFSET (-494)
#define BLE_TX_TIMESTAMP_OFFSET (16)
#define BLE_RX_FRAME_LENGTH_CORRECTION (1) // what was this agian??
#define BLE_TX_FRAME_LENGTH_CORRECTION (0)

#define ZB_RX_TIMESTAMP_OFFSET  (-9-16-192)
#define ZB_TX_TIMESTAMP_OFFSET  (16)
#define ZB_RX_FRAME_LENGTH_CORRECTION (0)
#define ZB_TX_FRAME_LENGTH_CORRECTION (0)

// The time needed by HW to be able to receive a packet after RX is set on. Resolution 1 us
#define T_OFF2RX 50

#define BLE_MAX_VALIDATION_START_IDX (20)
#define BLE_HDR_MAX_VALIDATION_START_IDX (18)

// some timings in us (microseconds)
#define T_TX_PAEN_DELAY     16 //((GP_WB_READ_TX_PAEN_VAR_DELAY_CRS()+1) * 16)
// #define T_CAL_DELAY         ((GP_WB_READ_RIB_CAL_DELAY()+1) * 16
#define T_OFF2TX_DELAY      32 //((GP_WB_READ_RIB_OFF2TX_DELAY()+1) * 16)

#endif
