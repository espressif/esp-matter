/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_DEFS.h
 *
 *  This file contains internal definitions of the GPHAL.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_GP_DEFS_H_
#define _HAL_GP_DEFS_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_arm.h"

#if defined(GP_HAL_DIVERSITY_INCLUDE_IPC)
#include "gpHal_kx_Ipc.h"
#endif //defined(GP_HAL_DIVERSITY_INCLUDE_IPC)

#if defined(GP_COMP_GPHAL_PBM)
#include "gpHal_kx_Pbm.h"
#endif //defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
#ifdef GP_COMP_GPHAL_MAC
#include "gpHal_kx_MAC.h"
#endif //GP_COMP_GPHAL_MAC
#ifdef GP_COMP_GPHAL_BLE
#include "gpHal_kx_BLE.h"
#endif //GP_COMP_GPHAL_BLE

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
#include "gpHal_kx_Fll.h"
#endif //GP_COMP_GPHAL_MAC || GP_COMP_GPHAL_BLE

#include "gpHal_Ble.h"

#include "gpHal_kx_ES.h"
#include "gpHal_kx_DP.h"
#include "gpHal_kx_Phy.h"

#include "gpLog.h"
#include "gpStat.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


//-------------------------------------------------------------------------------------------------------
//In total six channel indexes are available (0 -> 5). As it is possible to run three IEEE802.15.4 stacks, channel indexes 0, 1 and 2 can be taken
//by these stacks. BLE channel index is reserved on channel index 3. For ED scans, channel index 5 is used.
#define GPHAL_BLE_CHANNEL_IDX (3)
#define GPHAL_ZB_CHANNEL_IDX (0)
#define GPHAL_ED_SCAN_CHANNEL_IDX (5)

/* gpHalPhy_GetFemTxOptions parameter constants */
#define GPHAL_TX_OPTIONS_FOR_TRANSMIT   false
#define GPHAL_TX_OPTIONS_FOR_ACK        true

#ifndef GP_BSP_RX_ANTSEL_INT_DEFAULT
#if defined(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#define GP_BSP_RX_ANTSEL_INT_DEFAULT GP_HAL_ANT_TO_ANTSEL_INT(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#else
#define GP_BSP_RX_ANTSEL_INT_DEFAULT GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN
#endif
#endif


#ifndef GP_BSP_TX_ANTSEL_INT_DEFAULT
#if defined(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#define GP_BSP_TX_ANTSEL_INT_DEFAULT GP_HAL_ANT_TO_ANTSEL_INT(GP_HAL_DIVERSITY_SINGLE_ANTENNA)
#else
#define GP_BSP_TX_ANTSEL_INT_DEFAULT GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN
#endif
#endif

// Timeout on start
#ifndef GP_HAL_ACK_TIMEOUT_ON_START
#define GP_HAL_ACK_TIMEOUT_ON_START     (true)
#endif // GP_HAL_ACK_TIMEOUT_ON_START

// Default (imm-ack) rx window size [in symbols]
#ifndef GP_HAL_IMMACK_TIMEOUT
#define GP_HAL_IMMACK_TIMEOUT           (0x0F)
#endif // GP_HAL_IMMACK_TIMEOUT

// Using the max by HW supported value for the rx window [in symbols]
// Max rx ack frame size now becomes: 0x3f / 2 = 31.5 bytes, => 31 bytes
#ifndef GP_HAL_ENHACK_TIMEOUT
#define GP_HAL_ENHACK_TIMEOUT           (0x3F)
#endif // GP_HAL_ENHACK_TIMEOUT

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** PbmAddress is the base address of a pbm entry
 *  this macro checks if startOfData points to a byte in the FRAME area of the PBM entry.
 */
#define GP_HAL_IS_VALID_PBM_FRAME_PTR(PbmAddress, startOfData) GP_HAL_IS_VALID_PBM_FRAME_OFFSET((startOfData)-((PbmAddress)+GPHAL_REGISTER_PBM_FORMAT_T_FRAME_0))

#define GP_HAL_IS_DATA_CONFIRM_CALLBACK_REGISTERED()                            gpHal_DataConfirmCallback

#define GP_HAL_CB_DATA_CONFIRM(status, pdLoh, lastChannelUsed)                  gpHal_DataConfirmCallback(status, pdLoh, lastChannelUsed)

#define GP_HAL_IS_DATA_INDICATION_CALLBACK_REGISTERED()                         gpHal_DataIndicationCallback

#define GP_HAL_CB_DATA_INDICATION(pdLoh, rxInfo)                                gpHal_DataIndicationCallback(pdLoh, rxInfo)

#define GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED()                              gpHal_EDConfirmCallback

#define GP_HAL_CB_ED_CONFIRM(channelMask, protoED)                              gpHal_EDConfirmCallback(channelMask, protoED)

#define GP_HAL_IS_EXTERNAL_EVENT_CALLBACK_REGISTERED()                          gpHal_ExternalEventCallback

#define GP_HAL_CB_EXTERNAL_EVENT()                                              gpHal_ExternalEventCallback()

#define GP_HAL_IS_FRAME_QUEUED_CALLBACK_REGISTERED()                            gpHal_MacFrameQueued

#define GP_HAL_CB_FRAME_QUEUED()                                                gpHal_MacFrameQueued()

#define GP_HAL_IS_FRAME_UNQUEUED_CALLBACK_REGISTERED()                          gpHal_MacFrameUnqueued

#define GP_HAL_CB_FRAME_UNQUEUED()                                              gpHal_MacFrameUnqueued()

#define GP_HAL_ANT_TO_ANTSEL_INT(ant)                                           (1==(ant)?GP_WB_ENUM_ANTSEL_INT_USE_PORT_1:GP_WB_ENUM_ANTSEL_INT_USE_PORT_0)
#define GP_HAL_ANT_TO_ANTSEL_EXT(ant)                                           (1==(ant)?GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_1:GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_0)
//implies USE_PORT_FROM_DESIGN will be threaded as fixed antenna 0
#define GP_HAL_ANTSEL_INT_TO_ANT(antsel)                                        (GP_WB_ENUM_ANTSEL_INT_USE_PORT_1==(antsel)?1:0)
#define GP_HAL_ANTSEL_EXT_TO_ANT(antsel)                                        (GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_1==(antsel)?1:0)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/




/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_InitSleep(void);

//Component dependent ISR macro's
#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
void gpHal_PHYInterrupt(void);
void gpHal_RCIInterrupt(void);
void gpHal_ISR_RCIInterrupt(UInt16 highPrioRciPending);
#endif

#if defined(GP_COMP_GPHAL_BLE)
void gpHal_Handle_All_RCIAdvIndIRQ(void);
#endif

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
void gpHal_IPCInterrupt(void);
void gpHal_IpcGpm2XInterrupt(void);
void gpHal_ISR_IPCGPMInterrupt(UInt64 highPrioIpcPending);
#endif

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
void gpHal_ESInterrupt(void);
#endif

typedef struct gpHal_PbmCoexOptions_s {
    UInt8 coex_tx_prio;
    Bool coex_tx_req_en;
    UInt8 coex_ack_rx_prio;
    Bool coex_ack_rx_req_en;
    Bool coex_grant_aware_txpack;
} gpHal_PbmCoexOptions_t;

typedef struct gpHal_PbmTxOptions_s {
    Bool first_boff_is_0;
    Bool csma_ca_enable;
    Bool treat_csma_fail_as_no_ack;
    UInt8 vq_sel;
    Bool skip_cal_tx;
    UInt8 confirm_queue;
    Bool acked_mode;
    UInt8 antenna;
    Bool fcs_insert_dis;
    Bool stop_rx_window_on_full_pbm;
    Bool ed_scan;
    Bool rx_duration_valid;
    Bool start_rx_only_if_pending_bit_set;
    Bool channel_ch0a_en;
    Bool channel_ch0b_en;
    Bool channel_ch0c_en;
    Bool channel_ch1_en;
    UInt8 channel_idx;
    UInt8 tx_power_setting; //pa_ultralow<<7 | pa_low<<6 | tx_power
    UInt8 phy_mode;
    UInt8 antsel_int;
    UInt8 antsel_ext;
    Bool ext_pa_required;
    UInt8 min_be;
    UInt8 max_be;
    UInt8 max_csma_backoffs;
    UInt8 max_frame_retries;
    UInt8 num_channel_slots;
    UInt8 channel_ch0a;
    UInt8 channel_ch0b;
    UInt8 channel_ch0c;
    UInt8 channel_ch1;
    UInt8 rx_duration;
    UInt8 state;
    UInt8 return_code;
    UInt8 frame_ptr;
    UInt8 frame_len;

    gpHal_PbmCoexOptions_t coex_settings;
    gpHal_TxPower_t txPower;
} gpHal_PbmTxOptions_t;


void gpHalPhy_GetFemTxOptions(gpHal_TxPower_t txPower, gpHal_FllChannel_t fllChannel, gpHal_PbmTxOptions_t* pTxOptions, Bool forAck);

#ifdef GP_COMP_GPHAL_MAC
#if defined(GP_HAL_MAC_SW_CSMA_CA)
typedef struct __gpHalMac_CSMA_CA_state
{
    UInt8 pbmHandle;
    UInt8 minBe;
    UInt8 maxBe;
    UInt8 maxCsmaBackoffs;
    UInt8 backOffCount;
    UInt8 remainingFrameRetries;
    UInt8 initialMaxFrameRetries;
    gpPad_Handle_t padHandle;
    UInt8 minBeRetransmit;
    UInt8 maxBeRetransmit;
    Bool  useAdditionalRetransmitBackoff;
} gpHalMac_CSMA_CA_state_t;

gpHalMac_CSMA_CA_state_t* gpHalMac_Get_CSMA_CA_State(UInt8 pbmHandle);
void gpHalMac_Do_CCAFailRetry(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State);
void gpHalMac_Do_NoAckRetry(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State);
/** @brief Free and trigger next entry pending in SW CSMA CA queue */
void gpHalMac_Free_CSMA_CA_State(UInt8 pbmHandle);
#endif // GP_HAL_MAC_SW_CSMA_CA

extern gpHal_MacFrameQueued_t gpHal_MacFrameQueued;
extern gpHal_MacFrameUnqueued_t gpHal_MacFrameUnqueued;
#endif //GP_COMP_GPHAL_MAC

extern void gpHal_Scan_EDConfirm(UInt8 channel, UInt8 protoRssi);

void gpHal_InitScan(void);

#if defined(GP_COMP_GPHAL_BLE) || defined(GP_COMP_GPHAL_MAC)
void gpHal_PbmInit(void);
void gpHal_CalibrateFllChannel(UInt8 srcId, UInt8 fllChannel);
void gpHal_CalibrateFllChannelUnprotected(gpHal_ChannelSlotIdentifier_t slotId, UInt8 fllChannel);
#endif //defined(GP_COMP_GPHAL_BLE) || defined(GP_COMP_GPHAL_MAC)

#ifdef GP_COMP_GPHAL_BLE
void gpHal_RegisterBleCallbacks(gpHal_BleCallbacks_t* pCallbacks);

// callbacks that stay in gpHal layer
void gpHal_cbBleDataIndication(UInt8 pbmEntry);
void gpHal_cbBleDataConfirm(UInt8 pbmEntry);
void gpHal_cbBleAdvertisingIndication(UInt8 pbmEntry);
void gpHal_cbBleConnectionRequestIndication(UInt8 pbmEntry);
void gpHal_cbBleConnectionRequestConfirm(void);
void gpHal_cbBleEventProcessed(UInt8 eventId);
void gpHal_cbBleLastConnEventCountReached(UInt8 eventId);
void gpHal_cbBleScanRequestReceived(void);

void gpHal_BleCommonInit(void);
void gpHal_BleTestModeInit(void);
void gpHal_BleWlInit(void);
void gpHal_BleValidationInit(void);
void gpHal_BleDirectionFindingInit(void);
void gpHal_BlePreCalibrationInit(void);
void gpHal_BlePreCalibrateAllChannels(void);

void gpHal_BleRpa_Init(void);

Bool gpHal_BleTestModeIsActive(void);
void gpHal_BleSetClockAccuracy(UInt16 caPpm);

#endif //GP_COMP_GPHAL_BLE

void gpHal_cbEventTooLate(void);

UInt8 gpHalPhy_GetTxPowerSetting(gpHal_TxPower_t requested_txPower_dBm, Bool forAck);

void gpHalES_OscillatorBenchmarkDone_Handler(void);

#if defined(GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK)
gpHal_SleepClockMeasurementStatus_t gpHalEs_Get32kHzBenchmarkStatus(void);
#endif

#if defined(GP_COMP_GPHAL_MAC)
void gpHalMac_SetAckTransmitPower(gpHal_SourceIdentifier_t srcId);
#endif //defined(GP_COMP_GPHAL_MAC)

#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
UInt8 gpHalPhy_GetTxMode(gpHal_TxPower_t txPower);
#endif


typedef struct {
    gpHal_ContinuousWaveMode_t mode;
    UInt8 channel;
    Int8 txpower;
    gpHal_AntennaSelection_t antenna;
} gpHalPhy_CurrentCWModeSettings_t;

// Fetch the last used active CW settings
gpHalPhy_CurrentCWModeSettings_t *gpHal_GetCurrentContinuousWaveModeSettings(void);

#if defined(GP_DIVERSITY_GPHAL_TRIM_XTAL_32M)
void gpHal_Xtal32MHzTrimAlgorithmInit(void);
#endif

#ifndef GP_DIVERSITY_GPHAL_DISABLE_TRIM_VDD_RAM_TUNE
/* Initialize the VddRamTune tuning algorithm */
void gpHal_VddRamTuneTrimAlgoInit(void);

/* Restore the VddRamtune value last measured by algorithm*/
void gpHal_VddRamTuneTrimAlgoRestoreTuneValue(void);
#endif

#if defined(GP_COMP_GPHAL_MAC)
/** @typedef gpHal_RxModeConfig_t
 *  @brief   The gpHal_RxModeConfig_t typedef defines the structure used to pass Receiver Mode configurations. */
 typedef struct gpHal_RxModeConfig_s {
    gpHal_RxMode_t                  rxMode;
    gpHal_AntennaMode_t             antennaMode;
    gpHal_Channel_t                 channelList[GP_HAL_MAX_NUM_OF_SLOTS];
 } gpHal_RxModeConfig_t;

/** @brief This function sets a new receiver mode configuration.
 *
 *  This function sets the configuration to be applied for receiver (mode, channel(s) used, attenuation) during testing.
 *
 *  @param rxModeConfig             Reference to the storage location of new configuration info
 *
 *  @return void
 */
GP_API void gpHal_SetRxModeConfig(gpHal_RxModeConfig_t* rxModeConfig);


#endif //defined(GP_COMP_GPHAL_MAC)

#endif //_HAL_GP_DEFS_H_
