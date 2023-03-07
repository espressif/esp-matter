/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_MAC.c
 *   This file contains the implementation of the MAC functions
 *
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpPd.h"
#include "gpSched.h"

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_Pbm.h"
#include "gpHal_MAC.h"
#include "gpHal_kx_Phy.h"
//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "gpAssert.h"
#include "gpLog.h"

#ifdef GP_COMP_TXMONITOR
#include "gpTxMonitor.h"
#endif //GP_COMP_TXMONITOR
#include "gpHal_RadioMgmt.h"
#include "gpRandom.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// The default threshold in dBm to be used when selecting a low or high ack Tx Power
#define GPHAL_DEFAULT_ACK_TX_POWER_SELECT_THRESHOLD    0

#define RX_LPL_SA_MASK     (0x1)
#define RX_LPL_AD_MASK     (0x2)
#define RX_MCH_SA_MASK     (0x4)
#define RX_MCH_AD_MASK     (0x8)

#define GP_HALMAC_RX_MODE_IS_LPL_AD(c)                      (!!(c & RX_LPL_AD_MASK))
#define GP_HALMAC_RX_MODE_IS_LPL_SA(c)                      (!!(c & RX_LPL_SA_MASK))
#define GP_HALMAC_RX_MODE_IS_MCH_SA(c)                      (!!(c & RX_MCH_SA_MASK))
#define GP_HALMAC_RX_MODE_IS_MCH_AD(c)                      (!!(c & RX_MCH_AD_MASK))

#define GP_HALMAC_RX_MODE_SET_LPL_AD(c)                     (c |= RX_LPL_AD_MASK)
#define GP_HALMAC_RX_MODE_SET_LPL_SA(c)                     (c |= RX_LPL_SA_MASK)
#define GP_HALMAC_RX_MODE_SET_MCH_SA(c)                     (c |= RX_MCH_SA_MASK)
#define GP_HALMAC_RX_MODE_SET_MCH_AD(c)                     (c |= RX_MCH_AD_MASK)

#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
#define SA_MODE()   true
#else
#define SA_MODE()   GP_BSP_HAS_DIFFERENTIAL_SINGLE_ANTENNA()
#endif

// Queue to be used for Rx channel calibration using PBM's
#define GPHAL_CALIBRATION_PBM_CONFIRM_QUEUE 2
#define GP_WB_READ_RCI_DATA_CNF_CALIBRATION_PBM() GP_WB_READ_RCI_DATA_CNF_2_PBM()

#define GP_HAL_ZB_RAW_VSIE_SIZE       0x10  // round vsie size (5) to next multiple of GP_WB_MAX_MEMBER_SIZE (8)
#define GPHAL_ZB_VSIE_POOLSIZE 2

/* compile time verification of info structures */
GP_COMPILE_TIME_VERIFY((GP_HAL_ZB_RAW_VSIE_SIZE % GP_WB_MAX_MEMBER_SIZE) == 0);
GP_COMPILE_TIME_VERIFY(GP_HAL_ZB_RAW_VSIE_SIZE >= GP_WB_ZB_RAWMODE_VSIE_SIZE);
#ifndef GP_COMP_CHIPEMU
COMPILER_ALIGNED(GP_WB_MAX_MEMBER_SIZE) UInt8 gpHal_ZB_RawMode_VsIE_Pool[GP_HAL_ZB_RAW_VSIE_SIZE*GPHAL_ZB_VSIE_POOLSIZE] LINKER_SECTION(".lower_ram_retain_gpmicro_accessible");
#endif // GP_COMP_CHIPEMU

#ifdef GP_COMP_CHIPEMU
extern UInt32 gpChipEmu_GetGpMicroStructZBRawVsIEPoolStart(UInt32 gp_mm_ram_linear_start);
#define GP_HAL_ZB_RAW_VSIE_POOL_START                  gpChipEmu_GetGpMicroStructZBRawVsIEPoolStart(GP_MM_RAM_LINEAR_START)
#else
#define GP_HAL_ZB_RAW_VSIE_POOL_START                  ((UIntPtr)&gpHal_ZB_RawMode_VsIE_Pool[0])
#endif // GP_COMP_CHIPEMU

#define GP_HAL_ZB_RAW_VSIE_POOL_INDEX_TO_ADDRESS(index) (GP_HAL_ZB_RAW_VSIE_POOL_START + index*GP_HAL_ZB_RAW_VSIE_SIZE)

static UInt8 gpHal_activeRawVsIeIndex;

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

// pan coordinator regmap field has different encoding: (1 = no pan coordinator, 2 = pan coordinator)
#define GP_HAL_PAN_COORDINATOR_TO_ADDRESSMAP_FIELD(panCoordinator)  (panCoordinator + 1)
#define GP_HAL_PAN_COORDINATOR_FROM_ADDRESSMAP_FIELD(field)         (field - 1)

#if defined(GP_HAL_MAC_SW_CSMA_CA)
// Check if current state is the first entry in the queue
#define GP_HAL_MAC_CSMA_FIRST_ENTRY(pCsmaState) (&gpHalMac_CSMA_CA_state[0] == (pCsmaState))
#endif //GP_HAL_MAC_SW_CSMA_CA

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Transmit power can vary for different channels [11-26]
gpHal_TxPower_t gpHal_pDefaultTransmitPowerTable[16] = {
    GPHAL_DEFAULT_TRANSMIT_POWER, //Channel 11
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER  //Channel 26
};

UInt8 gpHal_MacState=0x0;

// Max duration in us for transmitting a frame and receiving the corresponding ACK, without the CSMA time
static UInt32 gpHal_MacMaxTransferTime;

//Default set - keeping only for legacy set/gets - to be removed ?
static const ROM gpHal_PbmTxOptions_t gpHalMac_PbmTxOptionsDefault FLASH_PROGMEM =
{
    false,      //first_boff_is_0
    false,      //csma_ca_enable
    true,       //treat_csma_fail_as_no_ack - switched in FillInPbmOptions
    GP_WB_ENUM_PBM_VQ_UNTIMED, //vq_sel
    false,      //skip_cal_tx
    0,          //confirm_queue
    false,      //acked_mode
    0,          //antenna;
    false,      //fcs_insert_dis
    false,      //stop_rx_window_on_full_pbm
    false,      //ed_scan
    false,      //rx_duration_valid
    false,      //start_rx_only_if_pending_bit_set
    false,      //channel_ch0a_en
    false,      //channel_ch0b_en
    false,      //channel_ch0c_en
    false,      //channel_ch1_en
    0,          //channel_idx - only 0 used for now
    12,         //tx_power_setting
    0,          //phy_mode
    GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN,  //antsel_int
    GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN,  //antsel_ext
    false,      //ext_pa_required
    0x3,        //min_be
    0x5,        //max_be
    0x4,        //max_csma_backoffs;
    0x3,        //max_frame_retries;
    0x1,        //num_channel_slots;
    20-11,         //channel_ch0a;
    20-11,         //channel_ch0b;
    20-11,         //channel_ch0c;
    20-11,         //channel_ch1;
    0,          //rx_duration;
    7,          //state, ONLY DEBUG
    255,        //return_code, ONLY DEBUG
    0,          //frame_ptr
    0,          //frame_len
    {
    0,          //coex_tx_prio;
    0,          //coex_tx_req_en
    0,          //coex_ack_rx_prio
    0,          //coex_ack_rx_req_en
    0,          //coex_grant_aware_txpack
    },
    0,          //txPower
};


gpHal_TxPower_t gpHal_LastUsedTxPower;

static UInt8 gpHalMac_PbmHandleCalibration = 0xFF;

#if defined(GP_HAL_MAC_SW_CSMA_CA)
static gpHalMac_CSMA_CA_state_t gpHalMac_CSMA_CA_state[GPHAL_QTA_MAX_SIZE];
#endif // GP_HAL_MAC_SW_CSMA_CA

gpHal_RxModeConfig_t    gpHal_ConfiguredReceiverMode;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void gpHalMac_SetTxOptions(UInt8 pbmEntry, gpHal_PbmTxOptions_t* pTxOptions);
static void gpHalMac_ApplyAckTransmitSettings(gpHal_SourceIdentifier_t srcId, UInt8 channel);

static void gpHalMac_InitAckPbm(void);
static void gpHalMac_InitCalibrationPbm(void);
static void gpHalMac_InitMacAddress(void);


#if defined(GP_HAL_MAC_SW_CSMA_CA)
static void gpHalMac_CCARetry(void *pData);
static void gpHalMac_Trigger_CSMA_CA_State(void);
#endif // GP_HAL_MAC_SW_CSMA_CA

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

STATIC INLINE UInt8 gpHalMac_GetInternalRxChannel(gpHal_ChannelSlotIdentifier_t srcId)
{
    UInt8 channel=0xFF-11;
    switch (srcId)
    {
        case 0:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_0();
            break;
        case 1:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_1();
            break;
        case 2:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_2();
            break;
        case 3:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_3();
            break;
        case 4:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_4();
            break;
        case 5:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_5();
            break;
        default:
            GP_ASSERT_SYSTEM(false);
    }
    return channel+11;
}

void gpHalMac_SetTxOptions(UInt8 pbmEntry, gpHal_PbmTxOptions_t* pTxOptions)
{
    gpHal_Address_t optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmEntry);
    UInt8 tmp;

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0_TO_GP_FLOW_CTRL_0(tmp, pTxOptions->first_boff_is_0);
    GP_WB_SET_PBM_FORMAT_T_GP_CSMA_CA_ENABLE_TO_GP_FLOW_CTRL_0(tmp, pTxOptions->csma_ca_enable);
    GP_WB_SET_PBM_FORMAT_T_GP_TREAT_CSMA_FAIL_AS_NO_ACK_TO_GP_FLOW_CTRL_0(tmp, pTxOptions->treat_csma_fail_as_no_ack);
    GP_WB_SET_PBM_FORMAT_T_GP_VQ_SEL_TO_GP_FLOW_CTRL_0(tmp, pTxOptions->vq_sel);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_0(optsbase, tmp);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_GP_ACKED_MODE_TO_GP_FLOW_CTRL_1(tmp,pTxOptions->acked_mode);
    GP_WB_SET_PBM_FORMAT_T_GP_ANTENNA_TO_GP_FLOW_CTRL_1(tmp,pTxOptions->antenna);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_1(optsbase, tmp);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_IDX_TO_GP_FLOW_CTRL_2(tmp, pTxOptions->channel_idx);
    //GP_WB_SET_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET_TO_GP_FLOW_CTRL_2(tmp, 0);  //Will be overwritten in PollScenario
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN_TO_GP_FLOW_CTRL_2(tmp, pTxOptions->channel_ch0a_en);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN_TO_GP_FLOW_CTRL_2(tmp, pTxOptions->channel_ch0b_en);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN_TO_GP_FLOW_CTRL_2(tmp, pTxOptions->channel_ch0c_en);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_2(optsbase, tmp);

#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS) 
    tmp = 0x0;
#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
    GP_WB_SET_PBM_FORMAT_T_GP_MODE_CTRL_TO_EXTERNAL_PA_FEM_SETTINGS(tmp, pTxOptions->phy_mode);
#endif
    GP_WB_WRITE_PBM_FORMAT_T_EXTERNAL_PA_FEM_SETTINGS(optsbase, tmp);
#endif



    tmp=0x0;
    GP_WB_WRITE_PBM_FORMAT_T_PA_POWER_SETTINGS(optsbase, pTxOptions->tx_power_setting);

    GP_WB_WRITE_PBM_FORMAT_T_PA_POWER_TABLE_INDEX(optsbase, BLE_MGR_PA_POWER_TABLE_INDEX_INVALID);

    GP_WB_WRITE_PBM_FORMAT_T_ATTENUATOR_SEL(optsbase, GP_WB_ENUM_ATTENUATOR_SEL_NO_ATTENUATION);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_MIN_BE_TO_CSMA_CA_BE_SETTINGS(tmp, pTxOptions->min_be);
    GP_WB_SET_PBM_FORMAT_T_MAX_BE_TO_CSMA_CA_BE_SETTINGS(tmp, pTxOptions->max_be);
    GP_WB_WRITE_PBM_FORMAT_T_CSMA_CA_BE_SETTINGS(optsbase, tmp);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_MAX_CSMA_BACKOFFS_TO_CSMA_CA_BO_RETRY_SETTINGS(tmp, pTxOptions->max_csma_backoffs);
    GP_WB_SET_PBM_FORMAT_T_MAX_FRAME_RETRIES_TO_CSMA_CA_BO_RETRY_SETTINGS(tmp, pTxOptions->max_frame_retries);
    GP_WB_SET_PBM_FORMAT_T_NUM_CHANNEL_SLOTS_TO_CSMA_CA_BO_RETRY_SETTINGS(tmp, pTxOptions->num_channel_slots);

    GP_WB_WRITE_PBM_FORMAT_T_CSMA_CA_BO_RETRY_SETTINGS(optsbase, tmp);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0A_TO_GP_CHANNEL_CHANGE_0(tmp, pTxOptions->channel_ch0a);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0B_TO_GP_CHANNEL_CHANGE_0(tmp, pTxOptions->channel_ch0b);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CHANGE_0(optsbase, tmp);

    tmp = 0x0;
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0C_TO_GP_CHANNEL_CHANGE_1(tmp, pTxOptions->channel_ch0c);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH1_TO_GP_CHANNEL_CHANGE_1(tmp, pTxOptions->channel_ch1);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CHANGE_1(optsbase, tmp);
}

static void gpHalMac_PreparePollRequestPBM(gpHal_Address_t optsbase)
{
#ifdef GP_DIVERSITY_GPHAL_NO_FP_BIT_NEEDED_FOR_RESPONSE_WINDOW
    GP_WB_WRITE_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET(optsbase, false);
#else
    GP_WB_WRITE_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET(optsbase, true);
#endif // GP_DIVERSITY_GPHAL_NO_FP_BIT_NEEDED_FOR_RESPONSE_WINDOW
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION(optsbase, GPHAL_POLL_REQ_MAX_WAIT_TIME);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION_VALID(optsbase, true);
}

static void gpHalMac_PrepareTimedTxPBM(gpHal_Address_t optsbase)
{
    GP_WB_WRITE_PBM_FORMAT_T_GP_VQ_SEL(optsbase, GP_WB_ENUM_PBM_VQ_SCHED0);
}

void gpHal_CalibrateMacChannel(gpHal_ChannelSlotIdentifier_t srcId, UInt8 macChannel)
{
    UInt16 timedOut = false;
    gpHal_Address_t optsbase;
    Bool previouslyRadioClaimed = false;

    /* release any unhandled NRT claim/grant so that MAC calibration can proceed*/
    if (radioMgmt_IsRadioClaimed())
    {
        previouslyRadioClaimed = true;
        gpHalRadioMgmtAsynch_releaseRadio();
    }

    if (gpHalMac_PbmHandleCalibration == 0xFF)
    {
        gpHalMac_InitCalibrationPbm();
        GP_ASSERT_DEV_EXT(gpHalMac_PbmHandleCalibration != 0xFF);
    }

    optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHalMac_PbmHandleCalibration);

    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_IDX(optsbase, srcId);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN(optsbase, true);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(optsbase, macChannel-11);

    GP_WB_WRITE_QTA_PBEFE_DATA_REQ(gpHalMac_PbmHandleCalibration);
    __DSB();
    // Wait for PBM to be returned from specific queue used
    // BLE could claim the radio indefinitely, however in practice it should not be more than seconds
    GP_DO_WHILE_TIMEOUT(gpHalMac_PbmHandleCalibration != GP_WB_READ_RCI_DATA_CNF_CALIBRATION_PBM(),
                        0xFFFE<<5 /* largest value that fits in UInt16 counter */,
                        &timedOut);
    if (timedOut)
    {
        GP_LOG_PRINTF("gpHalMac_CalibrateChannel timeout! (Heavy BLE radio usage?)",0);
        GP_ASSERT_DEV_INT(false);
        while (gpHalMac_PbmHandleCalibration != GP_WB_READ_RCI_DATA_CNF_CALIBRATION_PBM())
        {
            HAL_SLEEP_UC_1US(100000);
        }
    }
    if (previouslyRadioClaimed)
    {
        gpHalRadioMgmtAsynch_claimRadio(NULL);
    }
}

static void gpHalMac_SetRxOnWhenIdle(gpHal_ChannelSlotIdentifier_t srcId, Bool flag)
{
    switch (srcId)
    {
    case 0:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH0(flag);
        break;
    case 1:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH1(flag);
        break;
    case 2:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH2(flag);
        break;
    case 3:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH3(flag);
        break;
    case 4:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH4(flag);
        break;
    case 5:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(flag);
        break;
    default:
        GP_ASSERT_SYSTEM(false);
    }
}

UInt8 gpHalMac_GetChannel_LowTXPower(UInt8 *low_channel)
{
    UInt8 tmp_channel;
    gpHal_TxPower_t lowest_txPower = 0;
    gpHal_TxPower_t tmp_txPower = 0;
    Bool notSet = true;
    UInt8 rxOn;
    UIntLoop idx;

    rxOn = GP_WB_READ_RIB_RX_ON_WHEN_IDLE_CH();
    for (idx=0; idx<6; idx++)
    {
        if (BIT_TST(rxOn, idx))
        {
            tmp_channel = gpHalMac_GetInternalRxChannel(idx);
            if (tmp_channel <= 26) /* skip BLE channels */
            {
                tmp_txPower = gpHal_GetDefaultTransmitPower(tmp_channel);
                if (notSet || tmp_txPower < lowest_txPower)
                {
                    lowest_txPower = tmp_txPower;
                    *low_channel = tmp_channel;
                    notSet=false;
                }
            }
        }
    }
    return rxOn;
}

void gpHalMac_RecalibrateChannels(void)
{
    UInt8 channel;
    UInt8 rxOn;
    UIntLoop idx;

    rxOn = GP_WB_READ_RIB_RX_ON_WHEN_IDLE_CH();

    for(idx=0; idx < GP_HAL_MAX_NUM_OF_SLOTS; idx++)
    {
        if (BIT_TST(rxOn, idx))
        {
            channel = gpHalMac_GetInternalRxChannel(idx);
            if (channel <= 26) {
                gpHal_CalibrateMacChannel(idx, channel);
            }
        }
    }
}

void gpHal_SetPipMode(Bool pipmode)
{
    {
        GP_WB_WRITE_RX_EN_PIP(pipmode);
    }
}


void gpHal_SetCCAThreshold(void)
{
    UInt8 protoEDThreshold = gpHal_CalculateProtoRSSI(GPHAL_CCA_THRESHOLD_VALUE);
    GP_WB_WRITE_PLME_CCA_ED_THRESH(protoEDThreshold);
}


//-------------------------------------------------------------------------------------------------------
//  TX Power
//-------------------------------------------------------------------------------------------------------

void gpHal_SetDefaultTransmitPowers(gpHal_TxPower_t* pDefaultTransmitPowerTable )
{
    GP_ASSERT_DEV_EXT(pDefaultTransmitPowerTable != NULL);
    MEMCPY(gpHal_pDefaultTransmitPowerTable,pDefaultTransmitPowerTable,sizeof(gpHal_pDefaultTransmitPowerTable));

    // Update ack transmit powers of both possible stacks
    gpHalMac_SetAckTransmitPower(gpHal_SourceIdentifier_0);
    gpHalMac_SetAckTransmitPower(gpHal_SourceIdentifier_1);
    gpHalMac_SetAckTransmitPower(gpHal_SourceIdentifier_2);
}


/**
 * @brief Find the channel with the lowest transmit power and use this channel power to set the acknowledgement transmit power
 * Also apply possible FEM settings.
 */
void gpHalMac_SetAckTransmitPower(gpHal_SourceIdentifier_t srcId)
{
    UInt8 channel;

    /* All RX channels share the same ACK PBM.
     * To be safe we choose the channel with the lowest transmit power.
     */
    if (!gpHalMac_GetChannel_LowTXPower(&channel))
    {
        // Not RX-ing on any channel.
        return;
    }

    gpHalMac_ApplyAckTransmitSettings(srcId, channel);
}


void gpHalMac_ApplyAckTransmitSettings(gpHal_SourceIdentifier_t srcId, UInt8 channel)
{
    gpHal_PbmTxOptions_t txOptions;

    gpHalPhy_GetFemTxOptions(gpHal_DefaultTransmitPower,GP_HAL_CONVERT_MACPHY_TO_FLL_CHANNEL(channel), &txOptions, GPHAL_TX_OPTIONS_FOR_ACK);



#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
    GP_WB_WRITE_MACFILT_ACK_MODE_CTRL(txOptions.phy_mode);
#endif

    //ACK_POWER_CFG_PAN settings exist out of
    //      ..000000  00   MACFILT_ACK_POWER_L         ack tx power if rssi is lower than threshold(thr), this is used when incomming frame does belongs to this pan/short/ch_idx combo
    //      .0......  00   MACFILT_ACK_PA_LOW_L
    //      0.......  00   MACFILT_ACK_PA_ULTRA_LOW_L
    //      ..000000  00   MACFILT_ACK_POWER_H         ack tx power if rssi is higher or equal to threshold(thr), this is used when incomming frame does belongs to this pan/short/ch_idx combo
    //      .0......  00   MACFILT_ACK_PA_LOW_H
    //      0.......  00   MACFILT_ACK_PA_ULTRA_LOW_H
    //      00000000  00   MACFILT_ACK_POWER_RSSI_THR  threshold which will be compared to the rssi of the incomming frame, this is used when incomming frame does belongs to this pan/short/ch_idx combo
    //      00000000  00   MACFILT_ACK_POWER_SPACING   this has no meaning, just a placeholder for this address so the table is correctly structured
    // each property can be access individually by the macro's in gpHal_kx_reg_prop.h
    // however, the tx_pa_power_settings already combines a (power, pa_low, pa_utltra_low) triplet, so doing some explicity combining instead

    // GP_WB_SET_MACFILT_ACK_POWER_L_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_SETTING(txOptions.tx_pa_power_settings));
    // GP_WB_SET_MACFILT_ACK_PA_LOW_L_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_PA_LOW(txOptions.tx_pa_power_settings));
    // GP_WB_SET_MACFILT_ACK_PA_ULTRA_LOW_L_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_PA_ULTRALOW(txOptions.tx_pa_power_settings));
    // GP_WB_SET_MACFILT_ACK_POWER_H_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_SETTING(txOptions.tx_pa_power_settings));
    // GP_WB_SET_MACFILT_ACK_PA_LOW_H_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_PA_LOW(txOptions.tx_pa_power_settings));
    // GP_WB_SET_MACFILT_ACK_PA_ULTRA_LOW_H_TO_ACK_POWER_CFG_PAN(tmp,GP_HAL_MAC_TX_POWER_PA_ULTRALOW(txOptions.tx_pa_power_settings));

    UInt32 tmp = ((UInt32)(txOptions.tx_power_setting) << 8) | txOptions.tx_power_setting;
    GP_WB_SET_MACFILT_ACK_POWER_RSSI_THR_TO_ACK_POWER_CFG_PAN(tmp, gpHal_CalculateProtoRSSI(GPHAL_DEFAULT_ACK_TX_POWER_SELECT_THRESHOLD));

    if(gpHal_SourceIdentifier_0 == srcId)
    {
        GP_WB_WRITE_MACFILT_ACK_POWER_CFG_PAN(tmp);
    }
    else if(gpHal_SourceIdentifier_1 == srcId)
    {
        GP_WB_WRITE_MACFILT_ACK_POWER_CFG_PAN_ALT_A(tmp);
    }
    else if(gpHal_SourceIdentifier_2 == srcId)
    {
        GP_WB_WRITE_MACFILT_ACK_POWER_CFG_PAN_ALT_B(tmp);
    }
    else if (gpHal_SourceIdentifier_Inv != srcId) //allow invalid to set general settings
    {
        GP_ASSERT_DEV_EXT(false);
    }

    /* Set the default Ack transmit power (in case of no match in MAC filter on short addr / pan ID / chIdx combination) */
    GP_WB_WRITE_MACFILT_ACK_POWER_CFG_DEFAULT(tmp);
}

void gpHalMac_InitAckPbm(void)
{
    if (!GP_HAL_MAC_STATE_GET_ACKPBM_INITIALIZED())
    {
        UInt8 pbmHandleAck;

        pbmHandleAck = gpHal_GetHandle(GP_HAL_RESERVED_PBM_SIZE);
        GP_WB_WRITE_RIB_ACK_PBM_PTR(pbmHandleAck);
        gpHalMac_ApplyAckTransmitSettings(gpHal_SourceIdentifier_Inv,11);
        GP_HAL_MAC_STATE_SET_ACKPBM_INITIALIZED();
    }
}

void gpHalMac_InitCalibrationPbm(void)
{
    gpHal_Address_t optsbase;

    // Claim the pbm to be used for changing/calibrating the Rx channel
    gpHalMac_PbmHandleCalibration = gpHal_GetHandle(GP_HAL_RESERVED_PBM_SIZE);

    GP_LOG_PRINTF("gpHalMac_PbmHandleCalibration: %d",0,gpHalMac_PbmHandleCalibration);

    // Global configuration of channel calibration PBM
    optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHalMac_PbmHandleCalibration);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CONFIRM_QUEUE(optsbase, GPHAL_CALIBRATION_PBM_CONFIRM_QUEUE);
    GP_WB_WRITE_PBM_FORMAT_T_GP_ACKED_MODE(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION_VALID(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_ED_SCAN(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_VQ_SEL(optsbase,GP_WB_ENUM_PBM_VQ_UNTIMED);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN(optsbase,0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN (optsbase,0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH1_EN(optsbase,0);
}

void gpHalMac_GetFactoryMacAddress(MACAddress_t* pAddress)
{
    const UInt8 noAddress[3] = {0, 0, 0};   //  Mac address only valid if top 3 bytes are not 0x00
    const MACAddress_t ffAddress = {0xFFFFFFFF, 0xFFFFFFFF};
    UInt64 deviceAddress;

    // Read from user license
    GP_HAL_READ_BYTE_STREAM(GP_MM_FLASH_ALT_START + 8, pAddress, sizeof(MACAddress_t));

    if((MEMCMP(pAddress, &ffAddress, sizeof(MACAddress_t)) != 0) &&
       (MEMCMP(&(((UInt8*)pAddress)[5]), noAddress, sizeof(noAddress)) != 0))
    {
        // MAC address in user page is set
        return;
    }

    // Read from user area in inf page
    deviceAddress = GP_WB_READ_NVR_USER_AREA_1();

    MEMCPY(pAddress, &deviceAddress, sizeof(MACAddress_t));

    if((MEMCMP(pAddress, &ffAddress, sizeof(MACAddress_t)) != 0) &&
       (MEMCMP(&(((UInt8*)pAddress)[5]), noAddress, sizeof(noAddress)) != 0))
    {
        // MAC address in user area of inf page is set
        return;
    }

    // Read from info page
    deviceAddress = GP_WB_READ_NVR_MAC_ADDRESSS();

    MEMCPY(pAddress, &deviceAddress, sizeof(MACAddress_t));

    RF_TO_HOST_UINT64(&pAddress);
}

void gpHalMac_InitMacAddress(void)
{
    UInt64Struct_t macAddr;
    UInt8 srcId;

    gpHalMac_GetFactoryMacAddress(&macAddr);

    for (srcId = 0; srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++srcId)
    {
        gpHal_SetExtendedAddress(&macAddr, srcId);
    }

    // Enable 3 slots in the PAN ID table.
    // Fill table with invalid entries.
    GP_WB_WRITE_RIB_DST_PAN_ID_TABLE_SIZE(GP_HAL_MAC_NUMBER_OF_RX_SRCIDS);
    for (srcId = 0; srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++srcId)
    {
        gpHal_SetPanId(0xffff, srcId);
        gpHal_SetShortAddress(0xffff, srcId);
    }
}

#if defined(GP_HAL_MAC_SW_CSMA_CA)
void gpHalMac_CCARetry(void *pData)
{
    gpHalMac_CSMA_CA_state_t *pCSMA_CA_State = (gpHalMac_CSMA_CA_state_t *)pData;
    gpHal_DataRequest_base(pCSMA_CA_State->pbmHandle);
}
#endif // GP_HAL_MAC_SW_CSMA_CA

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


//-------------------------------------------------------------------------------------------------------
//  INIT
//-------------------------------------------------------------------------------------------------------
void gpHalMac_InitRxConfig(void)
{
    UIntLoop i=0;

    for (i=0; i < GP_HAL_MAX_NUM_OF_SLOTS; ++i)
    {
        gpHal_ConfiguredReceiverMode.channelList[i] = GP_HAL_MULTICHANNEL_INVALID_CHANNEL;
    }
}

void gpHal_InitMAC(void)
{
    //Start with clean local MAC state
    GP_HAL_MAC_STATE_INIT();

    gpHalMac_InitAckPbm();

    gpHalMac_InitMacAddress();

    gpHalMac_InitRxConfig();

    //GP_WB_WRITE_PLME_CCA_ED_THRESH(GPHAL_CCA_THRESHOLD_VALUE);

    gpHal_SetCCAThreshold();


    // Start with invalid last used tx power (of -100 dBm)
    gpHal_LastUsedTxPower = gpHal_DefaultTransmitPower;

#if defined(GP_HAL_MAC_SW_CSMA_CA)
    //GP_WB_WRITE_PLME_CCA_ED_THRESH(0); // Always triggers CCA
    MEMSET(gpHalMac_CSMA_CA_state, 0, sizeof(gpHalMac_CSMA_CA_state));
    {
        UIntLoop idx;
        for (idx=0; idx<GPHAL_QTA_MAX_SIZE; idx++)
        {
                gpHalMac_CSMA_CA_state[idx].pbmHandle = GP_PBM_INVALID_HANDLE;
        }
    }
#endif // GP_HAL_MAC_SW_CSMA_CA


}


//-------------------------------------------------------------------------------------------------------
//  MAC FUNCTIONS
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//  MAC GENERAL FUNCTIONS
//-------------------------------------------------------------------------------------------------------

gpHal_TxPower_t gpHal_GetLastUsedTxPower(void)
{
    // Make sure we don't read an invalid tx power (at least one packet has to be sent)
    GP_ASSERT_DEV_INT(gpHal_LastUsedTxPower != gpHal_DefaultTransmitPower);
    return gpHal_LastUsedTxPower;
}

void gpHal_SetTimedMode (Bool timedMode)
{
    GP_ASSERT_SYSTEM(false);
    /* this function is obsolete and might be removed */
}

UInt8 gpHal_GetRxChannel(gpHal_SourceIdentifier_t srcId)
{
    GP_ASSERT_DEV_INT(srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS);
    return gpHalMac_GetInternalRxChannel(srcId);
}

gpHal_Result_t gpHal_GetRadioState(void)
{
    UInt8 radioState;
    radioState=GP_WB_READ_PLME_TRX_STATE_STATUS();

    switch(radioState)
    {
        case GP_WB_ENUM_PHY_STATE_TRX_OFF:
        {
            radioState = gpHal_ResultTrxOff;
            break;
        }
        case GP_WB_ENUM_PHY_STATE_TX_ON:
        {
            radioState = gpHal_ResultTxOn;
            break;
        }
        case GP_WB_ENUM_PHY_STATE_RX_ON:
        {
            radioState = gpHal_ResultRxOn;
            break;
        }
        default:
        {
            radioState = gpHal_ResultBusy;
            break;
        }
    }

    return radioState;
}


//-------------------------------------------------------------------------------------------------------
//  MAC TRANSMIT FUNCTIONS
//-------------------------------------------------------------------------------------------------------

static void gpHalMac_FillInTxOptions(UInt8 pbmHandle, gpPad_Attributes_t* pOptions, Bool ackRequest, UInt8 channelIndex)
{
    gpHal_PbmTxOptions_t txOptions;

    GP_LOG_PRINTF("ch:%u,%u,%u a:%u mBE:%u/%u mBO:%u mFR:%u Pow:%i C:%i",0,\
                                        pOptions->channels[0], pOptions->channels[1], pOptions->channels[2], \
                                        pOptions->antenna, \
                                        pOptions->maxBE, pOptions->minBE, \
                                        pOptions->maxCsmaBackoffs, pOptions->maxFrameRetries, \
                                        pOptions->txPower,
                                        pOptions->csma);
    //Set defaults
    MEMCPY_P(&txOptions, &gpHalMac_PbmTxOptionsDefault, sizeof(gpHal_PbmTxOptions_t));

    //Antenna selection
    txOptions.antenna = pOptions->antenna;

    //CSMA/CA settings
    if(pOptions->csma != gpHal_CollisionAvoidanceModeNoCCA)
    {
#ifdef GP_HAL_MAC_SW_CSMA_CA
        txOptions.min_be = 0;
        txOptions.max_be = 0;
        txOptions.max_csma_backoffs = 0;
#else
        //txOptions.max_frame_retries = pOptions->maxFrameRetries; //- retries will be set during channel config
        if(pOptions->csma == gpHal_CollisionAvoidanceModeCSMA)
        {
            txOptions.min_be = pOptions->minBE;
            txOptions.max_be = pOptions->maxBE;
            txOptions.max_csma_backoffs = pOptions->maxCsmaBackoffs;
        }
        else if (pOptions->csma == gpHal_CollisionAvoidanceModeCCA)
        {
            txOptions.min_be = 0;
            txOptions.max_be = 0;
            txOptions.max_csma_backoffs = 0;
        }
#endif //GP_HAL_MAC_SW_CSMA_CA
        txOptions.csma_ca_enable = true;
    }
    else
    {
        txOptions.csma_ca_enable = false;
    }

    //Write channel options
    txOptions.channel_idx = channelIndex; // Source ID

    GP_ASSERT_DEV_INT(pOptions->channels[0] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0a = pOptions->channels[0] - 11;

    //Using channel a for Tx channel selection
    txOptions.channel_ch0a_en = (pOptions->channels[0] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0b_en = (pOptions->channels[1] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0c_en = (pOptions->channels[2] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);

    /* Check that the channels are used from start to end, without gaps */
    if (pOptions->channels[0] == GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
    {
        GP_ASSERT_DEV_EXT(pOptions->channels[1] == GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    }
    if (pOptions->channels[1] == GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
    {
        GP_ASSERT_DEV_EXT(pOptions->channels[2] == GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    }

    txOptions.num_channel_slots = 0;

    UInt8 i;
    for (i=0; i<GP_PAD_MAX_CHANNELS; i++)
    {
        if(pOptions->channels[i] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
        {
            txOptions.num_channel_slots++;
        }
    }

    //Reset possible option
    txOptions.first_boff_is_0 = false;

    txOptions.acked_mode = ackRequest;
    if(!txOptions.channel_ch0b_en && !txOptions.channel_ch0c_en)
    {
        //Normal no-ack and csma fail results
        txOptions.treat_csma_fail_as_no_ack = false;
        if(txOptions.acked_mode)
        {
#ifdef GP_HAL_MAC_SW_CSMA_CA
            // SW CSMA handling retries - no fill-in required in PBM
            txOptions.max_frame_retries = 0;
#else
            //1 channel Acked Tx - MAC retries can be taken into account
            GP_ASSERT_DEV_INT(pOptions->maxFrameRetries <= GPHAL_MAX_HW_TX_RETRY_AMOUNT);
            txOptions.max_frame_retries = pOptions->maxFrameRetries;
#endif
        }
        else
        {
            //1 channel Un-acked Tx - no MAC retries - used for multi-channel behavior
            txOptions.max_frame_retries = 0;
        }
    }
    else if(txOptions.channel_ch0b_en && !txOptions.channel_ch0c_en)
    {
        //2 channel Tx - MAC retries ignored - used for multi-channel behavior
        txOptions.max_frame_retries = 1;
        txOptions.channel_ch0b = pOptions->channels[1] - 11; //Using channel info as storage
    }
    else if(txOptions.channel_ch0b_en && txOptions.channel_ch0c_en)
    {
        //3 channel Tx - MAC retries ignored - used for multi-channel behavior
        txOptions.max_frame_retries = 2;
        txOptions.channel_ch0b = pOptions->channels[1] - 11; //Using channel info as storage
        txOptions.channel_ch0c = pOptions->channels[2] - 11; //Using channel info as storage

        // setting FIRST Backoff is 0 gives 1 ms extra margin for a 3-channel mc retansmit.
        // it's only done if there are 3 channels in the list.
        // Measurement indicates that for a short (2byte) RF4CE data frame, the loop
        // decreases from 9.2ms to 8.2ms (as expected)
        txOptions.first_boff_is_0 = true;
    }


    // Save power setting for future reference
    gpHal_LastUsedTxPower = pOptions->txPower;
    txOptions.txPower     = pOptions->txPower;

    gpHalPhy_GetFemTxOptions(pOptions->txPower, GP_HAL_CONVERT_MACPHY_TO_FLL_CHANNEL(pOptions->channels[0]), &txOptions, GPHAL_TX_OPTIONS_FOR_TRANSMIT);


    //Write Tx options
    gpHalMac_SetTxOptions(pbmHandle, &txOptions);

}

void gpHal_DataRequest_base(UInt8 pbmHandle)
{
#ifdef GP_COMP_TXMONITOR
    gpTxMonitor_AnnounceTxStart();
#endif //GP_COMP_TXMONITOR

    GP_STAT_SAMPLE_TIME();
    //Queue the frame
    GP_WB_WRITE_QTA_PBEFE_DATA_REQ(pbmHandle);

    // Make sure we don't have overrun (indicates something is wrong in checking queue size before queueing)
    GP_ASSERT_DEV_EXT(!GP_WB_READ_QTA_UNMASKED_QUEUE_FULL_INTERRUPT());
}

gpHal_Result_t gpHal_DataRequest(gpHal_DataReqOptions_t *dataReqOptions, gpPad_Handle_t padHandle, gpPd_Loh_t pdLoh)
{
    UInt8 ackRequest;
    UInt8 pbmHandle;
    gpHal_Address_t optsbase;
#if defined(GP_HAL_MAC_SW_CSMA_CA)
    gpHalMac_CSMA_CA_state_t* pCsmaState;
#endif
    gpPad_Attributes_t padAttributes;

    GP_ASSERT_DEV_EXT(dataReqOptions);
    GP_ASSERT_DEV_EXT(dataReqOptions->srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS);

    // make pd offset valid
    pdLoh.offset &= 0x7f;

    if((pdLoh.length > GPHAL_MAX_15_4_PACKET_LENGTH_NO_FCS) || (pdLoh.length == 0))
    {
        GP_ASSERT_DEV_INT(false);
        return gpHal_ResultInvalidParameter;
    }

    // Check if there is room in the queue (which is always the case if the last queue entry is empty)
    if(GP_WB_READ_QTA_STATE_6() != GP_WB_ENUM_QTA_ENTRY_STATE_EMPTY)
    {
        return gpHal_ResultBusy;
    }

#if defined(GP_HAL_MAC_SW_CSMA_CA)
    pCsmaState = gpHalMac_Get_CSMA_CA_State(GP_PBM_INVALID_HANDLE);
    if (NULL == pCsmaState) /* CSMA/CA state full */
    {
        return gpHal_ResultBusy;
    }
    pCsmaState->padHandle = padHandle;
#endif

    //Data to PBM
    pbmHandle = gpPd_DataRequest(&pdLoh);
    if(!GP_HAL_CHECK_PBM_VALID(pbmHandle))
    {
        return gpHal_ResultBusy;
    }
    optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    //Check if Ack bit is set
    ackRequest = BIT_TST(gpPd_ReadByte(pdLoh.handle, pdLoh.offset), GPHAL_ACK_REQ_LSB);

    //Set basic settings to pbm
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, pdLoh.offset);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, pdLoh.length);

    //Set all pad settings to a certain PBM
    gpPad_GetAttributes(padHandle, &padAttributes);
    gpHalMac_FillInTxOptions(pbmHandle, &padAttributes, ackRequest, dataReqOptions->srcId);

    GP_WB_WRITE_PBM_FORMAT_T_ENC_ENABLE(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_ENC_DONE(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_CSL_IE_OFFSET(optsbase, 0);



    //Add any additional settings for a certain scenario
    switch(dataReqOptions->macScenario)
    {
        case gpHal_MacPollReq:
        {
            gpHalMac_PreparePollRequestPBM(optsbase);
            {
                UInt8 txchannels[GP_PAD_MAX_CHANNELS];
                gpPad_GetTxChannels(padHandle, txchannels);
                gpHalMac_ApplyAckTransmitSettings(dataReqOptions->srcId, txchannels[0]);
            }
            GP_WB_WRITE_PBM_FORMAT_T_GP_FCS_INSERT_DIS(optsbase, 0);
            break;
        }
        case gpHal_MacManualCrc_NoRetries:
        {
            GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(optsbase, 0);
            GP_WB_WRITE_PBM_FORMAT_T_GP_FCS_INSERT_DIS(optsbase, 1);
            break;
        }
        case gpHal_MacManualCrc:
        {
            GP_WB_WRITE_PBM_FORMAT_T_GP_FCS_INSERT_DIS(optsbase, 1);
            break;
        }
        default:
        {
            GP_WB_WRITE_PBM_FORMAT_T_GP_FCS_INSERT_DIS(optsbase, 0);
            break;
        }
    }


#if defined(GP_HAL_MAC_SW_CSMA_CA)
    // Software CSMA/CA cannot be done when a timed transmission is requested. PAD TxCsmaMode should be set to gpHal_CollisionAvoidanceModeNoCCA
    if (dataReqOptions->macScenario == gpHal_MacTimedTx)
    {
        // Put packet on timed TX queue; will be sent at next TXPacket event.
        // Only one timed queue ( GP_WB_ENUM_PBM_VQ_SCHED0  --> GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE0 )
        // Store the PBM associated to timed queue0, no timed tx should be scheduled before
        GP_ASSERT_DEV_INT(padAttributes.csma == gpHal_CollisionAvoidanceModeNoCCA);

        gpHalMac_PrepareTimedTxPBM(optsbase);
        gpHal_DataRequest_base(pbmHandle);

        // Timed Tx doesn't use CSMA-CA, nor does it use HW retries
        // So we set everything to zero, except the pbmHandle (for tracking)
        MEMSET(pCsmaState, 0x00, sizeof(gpHalMac_CSMA_CA_state_t));
        pCsmaState->pbmHandle = pbmHandle;
    }
    else
    {
        /* Save for software CSMA_CA */
        pCsmaState->pbmHandle = pbmHandle;

        pCsmaState->minBe = padAttributes.minBE;
        pCsmaState->maxBe = padAttributes.maxBE;
        pCsmaState->maxCsmaBackoffs = padAttributes.maxCsmaBackoffs;
        pCsmaState->backOffCount = 0;

        pCsmaState->minBeRetransmit = padAttributes.minBERetransmit;
        pCsmaState->maxBeRetransmit = padAttributes.maxBERetransmit;

        // The retries configuration used for both Ack-ed and Non-Ack-ed frames.
        // Because Non-Acked frames can also have tx retries in case the
        // do-tx-retry-on-cca-fail is active
        // Initialize remainingFrameRetries - will be decremented during Tx cycles
        pCsmaState->remainingFrameRetries = padAttributes.maxFrameRetries;
        // Store initial max value to report amount of retries
        pCsmaState->initialMaxFrameRetries = padAttributes.maxFrameRetries;
        if(ackRequest)
        {
            GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(optsbase,0);
        }

        /* Only trigger HW queueing on first CSMA CA state entry */
        if(GP_HAL_MAC_CSMA_FIRST_ENTRY(pCsmaState))
        {
            gpHalMac_Trigger_CSMA_CA_State();
        }
    }
#else
    if (dataReqOptions->macScenario == gpHal_MacTimedTx)
    {
        // Put packet on timed TX queue; will be sent at next TXPacket event.
        gpHalMac_PrepareTimedTxPBM(optsbase);
    }

    gpHal_DataRequest_base(pbmHandle);
#endif // GP_HAL_MAC_SW_CSMA_CA

    if (GP_HAL_IS_FRAME_QUEUED_CALLBACK_REGISTERED())
    {
        GP_HAL_CB_FRAME_QUEUED();
    }

    return gpHal_ResultSuccess;
}

//-------------------------------------------------------------------------------------------------------
//  MAC RECEIVE FUNCTIONS
//-------------------------------------------------------------------------------------------------------

// Return true if at least one channel is in RX mode.
STATIC INLINE Bool gpHalMac_GetInternalRxOnWhenIdle(void)
{
    UIntLoop srcId;
    for (srcId = 0; srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++srcId)
    {
        if (gpHal_ConfiguredReceiverMode.channelList[srcId] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
        {
            return true;
        }
    }
    return false;
}

static void gpHalMac_SwitchChannels(UInt8* channelList, Bool forceUpdate)
{
    UInt8 oldChannels;
    UInt8 changedChannels;
    UIntLoop slot;

    // Every source identifier uses up to two RX channel slots.
    // The mapping between source identifier and channel slot is static:
    //   srcId 0 uses channel slots 0 and 3,
    //   srcId 1 uses channel slots 1 and 4,
    //   srcId 2 uses channel slots 2 and 5.
    COMPILE_TIME_ASSERT(GP_HAL_MAC_NUMBER_OF_RX_SRCIDS <= GP_HAL_MAX_NUM_OF_SLOTS / 2);

    // Check which indices where enabled
    changedChannels = 0x0;
    oldChannels = 0x0;

    // verify last configured state, which is kept in static gpHal_ConfiguredReceiverMode
    for (slot = 0; slot < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++slot)
    {
        if (gpHal_ConfiguredReceiverMode.channelList[slot] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
        {
            BIT_SET(oldChannels,slot);
        }
        if (gpHal_ConfiguredReceiverMode.channelList[slot] != channelList[slot])
        {
            BIT_SET(changedChannels,slot);
        }
    }

    if (!forceUpdate && (!changedChannels))
    {
        return;
    }

    //For optimization register accesses
    gpHal_GoToSleepWhenIdle(false);

    for (slot = 0; slot < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++slot)
    {
        if (BIT_TST(changedChannels, slot)) //Change needed
        {
            UInt8 channel = channelList[slot];
            gpHal_ConfiguredReceiverMode.channelList[slot] = channel;

            if (channel != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
            {
                //Rx On
                gpHal_CalibrateMacChannel(slot, channel);
                gpHalMac_SetRxOnWhenIdle(slot, true);
                gpHalMac_SetAckTransmitPower(slot);
            }
            else
            {
                //Rx Off
                gpHalMac_SetRxOnWhenIdle(slot, false);
            }
        }
    }
    if (gpHalMac_GetInternalRxOnWhenIdle())
    {
        if (!GP_HAL_MAC_STATE_GET_RXON())
        {
            // Rx on when idle, but no old channels were enabled
            GP_HAL_MAC_STATE_SET_RXON();
            gpHal_GoToSleepWhenIdle(false);
        }
    }
    else
    {
        if (GP_HAL_MAC_STATE_GET_RXON())
        {
            //Re-enable sleep when last channel was disabled
            GP_HAL_MAC_STATE_CLR_RXON();
            gpHal_GoToSleepWhenIdle(true);
        }
    }

    //For optimization register accesses
    gpHal_GoToSleepWhenIdle(true);
}

void gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_t srcId, Bool flag, UInt8 channel)
{
    UInt8 currentChannel;

    GP_ASSERT_DEV_INT(srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS);

    currentChannel = gpHal_ConfiguredReceiverMode.channelList[srcId];

    // Only proceed when switching on/off or when changing an active channel number.
        /* Channel change */
    if ((flag && (channel != currentChannel)) ||
        /* State change on same channel */
       (flag != ((currentChannel != GP_HAL_MULTICHANNEL_INVALID_CHANNEL))))
    {
        UInt8 channelList[GP_HAL_MAX_NUM_OF_SLOTS];

        MEMCPY(channelList, gpHal_ConfiguredReceiverMode.channelList, GP_HAL_MAX_NUM_OF_SLOTS);
        channelList[srcId] = flag ? channel : GP_HAL_MULTICHANNEL_INVALID_CHANNEL;

        //No full configuration of Rx Modes needed - provide updated channel list
        gpHalMac_SwitchChannels(channelList, false);
    }
}

void gpHal_SetEnhAckVSIE(UInt8 vsIeLen, UInt8* pVsIeData)
{
    UInt8 nonActiveVsIe = gpHal_activeRawVsIeIndex == 0 ? 1 : 0;
    gpHal_Address_t new_ptr = (gpHal_Address_t)GP_HAL_ZB_RAW_VSIE_POOL_INDEX_TO_ADDRESS(nonActiveVsIe);

    if((vsIeLen > GP_WB_ZB_RAWMODE_VSIE_VSIEDATA_LEN) || ((vsIeLen != 0) && (pVsIeData == NULL)))
    {
        GP_ASSERT_DEV_INT(0);
        return;
    }

    // Update the VS IE data and length
    gpHal_WriteRegs(new_ptr + GP_WB_ZB_RAWMODE_VSIE_VSIEDATA_ADDRESS, pVsIeData, vsIeLen);
    GP_WB_WRITE_ZB_RAWMODE_VSIE_VSIELEN(new_ptr, vsIeLen);

    // make the updated vsie struct active by updating the pointer used by the RT.
    GP_WB_WRITE_MACFILT_RAWMODE_VSIE_PTR(new_ptr);
    gpHal_activeRawVsIeIndex = nonActiveVsIe;

}


void gpHal_SetPanId(UInt16 panId, gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_WB_WRITE_MACFILT_PAN_ID(panId);
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        GP_WB_WRITE_MACFILT_PAN_ID_ALT_A(panId);
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        GP_WB_WRITE_MACFILT_PAN_ID_ALT_B(panId);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
    }
}

UInt16 gpHal_GetPanId(gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        return GP_WB_READ_MACFILT_PAN_ID();
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        return GP_WB_READ_MACFILT_PAN_ID_ALT_A();
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        return GP_WB_READ_MACFILT_PAN_ID_ALT_B();
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
        return 0xFFFF;
    }
}

void gpHal_SetShortAddress(UInt16 shortAddress, gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_WB_WRITE_MACFILT_SHORT_ADDRESS(shortAddress);
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        GP_WB_WRITE_MACFILT_SHORT_ADDRESS_ALT_A(shortAddress);
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        GP_WB_WRITE_MACFILT_SHORT_ADDRESS_ALT_B(shortAddress);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
    }
}

UInt16 gpHal_GetShortAddress(gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        return GP_WB_READ_MACFILT_SHORT_ADDRESS();
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        return GP_WB_READ_MACFILT_SHORT_ADDRESS_ALT_A();
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        return GP_WB_READ_MACFILT_SHORT_ADDRESS_ALT_B();
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
        return 0xFFFF;
    }
}

void gpHal_SetExtendedAddress(MACAddress_t* pExtendedAddress, gpHal_SourceIdentifier_t srcId)
{
    HOST_TO_RF_UINT64(pExtendedAddress);
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_HAL_WRITE_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ADDRESS, pExtendedAddress,
                             sizeof(*pExtendedAddress));
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        GP_HAL_WRITE_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ALT_A_ADDRESS, pExtendedAddress,
                             sizeof(*pExtendedAddress));
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        GP_HAL_WRITE_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ALT_B_ADDRESS, pExtendedAddress,
                             sizeof(*pExtendedAddress));
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);
    }
    RF_TO_HOST_UINT64(pExtendedAddress);
}

void gpHal_GetExtendedAddress(MACAddress_t* pExtendedAddress, gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_HAL_READ_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ADDRESS , pExtendedAddress,
                            sizeof(*pExtendedAddress));
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        GP_HAL_READ_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ALT_A_ADDRESS , pExtendedAddress,
                            sizeof(*pExtendedAddress));
    }
    else if(srcId == gpHal_SourceIdentifier_2)
    {
        GP_HAL_READ_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ALT_B_ADDRESS , pExtendedAddress,
                            sizeof(*pExtendedAddress));
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);
    }
    RF_TO_HOST_UINT64(pExtendedAddress);
}

void gpHal_ResetExtendedAddress(gpHal_SourceIdentifier_t srcId)
{
    MACAddress_t macAddr;

    gpHalMac_GetFactoryMacAddress(&macAddr);
    gpHal_SetExtendedAddress(&macAddr, srcId);
}

void gpHal_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress)
{
}

void gpHal_SetCoordShortAddress(UInt16 shortCoordAddress)
{
}

void gpHal_SetPanCoordinator(Bool panCoordinator)
{
    GP_WB_WRITE_MACFILT_PAN_COORDINATOR(GP_HAL_PAN_COORDINATOR_TO_ADDRESSMAP_FIELD(panCoordinator));
}

Bool gpHal_GetPanCoordinator(void)
{
    return GP_HAL_PAN_COORDINATOR_FROM_ADDRESSMAP_FIELD(GP_WB_READ_MACFILT_PAN_COORDINATOR());
}

void gpHal_SetAddressRecognition(Bool enable, Bool panCoordinator)
{
    GP_WB_WRITE_MACFILT_DST_PAN_ID_CHECK_ON(enable);
    GP_WB_WRITE_MACFILT_DST_ADDR_CHECK_ON(enable);
    GP_WB_WRITE_MACFILT_PAN_COORDINATOR(GP_HAL_PAN_COORDINATOR_TO_ADDRESSMAP_FIELD(panCoordinator));
}

void gpHal_SetBeaconSrcPanChecking(Bool enable)
{
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON(enable);
}

Bool gpHal_GetBeaconSrcPanChecking(void)
{
    return GP_WB_READ_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON();
}

gpHal_Result_t gpHal_SetMacRxMode(Bool enableMultiStandard, Bool enableMultiChannel, Bool enableHighSensitivity)
{
    if(enableMultiStandard)
    {
        if (enableMultiChannel || enableHighSensitivity)
        {
            return gpHal_ResultInvalidParameter;
        }
        gpHal_ConfiguredReceiverMode.rxMode = gpHal_RxModeMultiStandard;
    }
    else if (enableMultiChannel)
    {
        if (enableHighSensitivity)
        {
            return gpHal_ResultInvalidParameter;
        }
        gpHal_ConfiguredReceiverMode.rxMode = gpHal_RxModeMultiChannel;
    }
    else if (enableHighSensitivity)
    {
        gpHal_ConfiguredReceiverMode.rxMode = gpHal_RxModeHighSensitivity; /* this is the "normal rx" mode in hardware */
    }
    else
    {
        gpHal_ConfiguredReceiverMode.rxMode = gpHal_RxModeLowPower;
    }
    gpHal_SetRxModeConfig(&gpHal_ConfiguredReceiverMode);
    return gpHal_ResultSuccess;
}

void gpHal_GetMacRxMode(Bool* enableMultiStandard, Bool* enableMultiChannel, Bool* enableHighSensitivity)
{
    *enableMultiStandard   = false;
    *enableMultiChannel    = false;
    *enableHighSensitivity = false;

    switch(gpHal_ConfiguredReceiverMode.rxMode)
    {
        case gpHal_RxModeMultiStandard:
        {
            *enableMultiStandard   = true;
            break;
        }
        case gpHal_RxModeMultiChannel:
        {
            *enableMultiChannel    = true;
            break;
        }
        case gpHal_RxModeHighSensitivity:
        {
            *enableHighSensitivity = true;
            break;
        }
        default:
        {
            break;
        }
    }
}

void gpHal_SetRxModeConfig(gpHal_RxModeConfig_t* receiverMode2Configure)
{
    UInt8 rapRxMode;

#ifdef GP_HAL_DIVERSITY_MULTISTANDARD_LISTENING_MODE
    //in case of multi standard, the supported 15.4 settings are limited
    // * only RAP_RX_MODE_M
    // * RAP_RX_MODE_M_AD will not be selected, independantly from BSP setting
    // * only 1 channel active channel allowed

    if(receiverMode2Configure->rxMode == gpHal_RxModeMultiStandard)
    {
        UInt8 channelCounter = 0;
        UInt8 slot;
        for (slot = 0; slot < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS; ++slot)
        {
            if (gpHal_ConfiguredReceiverMode.channelList[slot] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
            {
                channelCounter++;
                GP_ASSERT_SYSTEM(channelCounter <= 1);
            }
        }
        // MAC is active: enable multistandrad listening concurrent mode:
        // hw will automatically switch to concurrent mode when both 15.4 and BLE are scanning
        // (i.e. not for BLE window widening - of course)
        GP_WB_WRITE_PLME_RX_MULTI_STD_MODE(GP_WB_ENUM_MULTI_STD_MODE_ZB_BLE_CONCURRENT);
    }
    else
    {
        // Prioritize BLE
        GP_WB_WRITE_PLME_RX_MULTI_STD_MODE(GP_WB_ENUM_MULTI_STD_MODE_BLE_PRIORITY);
    }
#else
    // SW-8951: move the BLE PRIORITY setting to gpHal_Phy.c
    // Prioritize BLE
    GP_WB_WRITE_PLME_RX_MULTI_STD_MODE(GP_WB_ENUM_MULTI_STD_MODE_BLE_PRIORITY);
#endif // GP_HAL_DIVERSITY_MULTISTANDARD_LISTENING_MODE
    switch ((UInt8)receiverMode2Configure->rxMode)
    {
        case gpHal_RxModeHighSensitivity:
        {
            rapRxMode = RAP_RX_MODE_N;
            break;
        }
        case gpHal_RxModeLowPower:
        {
            rapRxMode = RAP_RX_MODE_L;
            break;
        }
        case gpHal_RxModeMultiChannel:
        {
            rapRxMode = RAP_RX_MODE_M;
            break;
        }
#ifdef GP_HAL_DIVERSITY_MULTISTANDARD_LISTENING_MODE
        case gpHal_RxModeMultiStandard:
        {
            rapRxMode = RAP_RX_MODE_MBXZB;
            break;
        }
#endif
        default:
        {
            GP_LOG_SYSTEM_PRINTF("UNSUPPORTED rxMode %02x",0,(UInt8)(receiverMode2Configure->rxMode));
            GP_ASSERT_DEV_EXT(false);
            return;
        }
    }


    if(receiverMode2Configure->rxMode != gpHal_RxModeMultiStandard)
    {
        //switch to RAP rxmode AD variant when not single antenna and AntennaDiversity is enabled
        if(!SA_MODE())
        {
            if(gpHal_GetRxAntennaDiversity())
            {
                COMPILE_TIME_ASSERT( (RAP_RX_MODE_N+1) == RAP_RX_MODE_N_AD );
                COMPILE_TIME_ASSERT( (RAP_RX_MODE_L+1) == RAP_RX_MODE_L_AD );
                COMPILE_TIME_ASSERT( (RAP_RX_MODE_M+1) == RAP_RX_MODE_M_AD );
                rapRxMode++;
            }
        }
    }

    //Disable sleep to optimize register access sequence
    gpHal_GoToSleepWhenIdle(false);

    rap_rx_zb_set_mode(rapRxMode);

    gpHalMac_SwitchChannels(receiverMode2Configure->channelList, false);

    // store the applied settings for retrieval with gpHalMac_GetRxMode()
    MEMCPY(&gpHal_ConfiguredReceiverMode, receiverMode2Configure, sizeof(gpHal_RxModeConfig_t));

    //Re-enable sleep after register access sequence
    gpHal_GoToSleepWhenIdle(true);
}

UInt8 gpHal_GetAvailableSrcIds(void)
{
    if((gpHal_ConfiguredReceiverMode.rxMode) == gpHal_RxModeMultiStandard)
    {
        return 1;
    }
    else
    {
        return GP_HAL_MAC_NUMBER_OF_RX_SRCIDS;
    }
}
/*****************************************************************************
 *                    kx Specific
 *****************************************************************************/

void gpHal_SetAutoAcknowledge(Bool flag)
{
    GP_WB_WRITE_MACFILT_ACK_REQUEST_ENABLES(flag?(0x40-1):0);
}

GP_API Bool gpHal_GetAutoAcknowledge(void)
{
    return (GP_WB_READ_MACFILT_ACK_REQUEST_ENABLES())?true:false;
}

UInt8 gpHal_ConvertProtoEDToProtoRSSI( UInt8 protoED )
{
    return protoED;
}

Bool gpHal_CheckNoLock(void)
{
    //Out of lock will be corrected by ISR
    //This should only return true in case of consistent out of lock
    return GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_LOCK_INTERRUPT();
}

void gpHal_SetPromiscuousMode(Bool flag)
{
    if (flag)
    {
        GP_WB_WRITE_MACFILT_ENABLE_SNIFFING(true);
        GP_WB_WRITE_MACFILT_ACK_REQUEST_ENABLES(0);
        //Update gpHal state:
        GP_HAL_MAC_STATE_SET_PROMISCUOUS_MODE();
    }
    else
    {
        GP_WB_WRITE_MACFILT_ENABLE_SNIFFING(false);
        GP_WB_WRITE_MACFILT_ACK_REQUEST_ENABLES(0x40-1);
        //Update gpHal state:
        GP_HAL_MAC_STATE_CLR_PROMISCUOUS_MODE();
    }
}

Bool gpHal_GetPromiscuousMode(void)
{
    return GP_HAL_MAC_STATE_GET_PROMISCUOUS_MODE();
}

#if defined(GP_HAL_MAC_SW_CSMA_CA)
//-------------------------------------------------------------------------------------------------------
// SW based CSMA CA handling
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/* Scheduled functions handling CSMA/CA algorithm*/
//-------------------------------------------------------------------------------------------------------
static UInt32 gpHalMac_DoCsmaCaBackoffScheduleCalculation(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State)
{
    UInt32 backOffTime_us;
    UInt32 backOffTime_max_us;
    UInt8 exponent;

    // calculate the exponent
    exponent = pCSMA_CA_State->minBe + pCSMA_CA_State->backOffCount;
    exponent = (exponent < pCSMA_CA_State->maxBe) ? exponent : pCSMA_CA_State->maxBe;

    if(exponent == 0)
    {
        backOffTime_us = 0;
    }
    else
    {
        // If exponent is non zero, calculate the backoff random delay
        UInt8 random;

        gpRandom_GetNewSequence(1, &random);
        backOffTime_max_us = 320UL*((1<<exponent) - 1);
        // Add random variation in backoff time
        backOffTime_us = ((backOffTime_max_us*random) >> 8);
    }
    return backOffTime_us;
}

static UInt32 gpHalMac_DoRetransmitBackoffScheduleCalculation(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State)
{
    UInt32 backOffTime_us;
    UInt32 backOffTime_max_us;
    UInt8 exponent;
    UInt8 retriesDone = pCSMA_CA_State->initialMaxFrameRetries - pCSMA_CA_State->remainingFrameRetries;

    // calculate the exponent
    exponent = pCSMA_CA_State->minBeRetransmit + retriesDone;
    exponent = (exponent < pCSMA_CA_State->maxBeRetransmit) ? exponent : pCSMA_CA_State->maxBeRetransmit;

    if(exponent == 0)
    {
        backOffTime_us = 0;
    }
    else
    {
        // If exponent is non zero, calculate the backoff random delay
        UInt8 random;

        gpRandom_GetNewSequence(1, &random);
        backOffTime_max_us = 320UL*((1<<exponent) - 1);
        // Add random variation in backoff time
        backOffTime_us = ((backOffTime_max_us*random) >> 8);
    }
    return backOffTime_us;
}

static void gpHalMac_DoBackoffScheduleCalculation(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State)
{
    UInt32 backOffTime_us;

    backOffTime_us = gpHalMac_DoCsmaCaBackoffScheduleCalculation(pCSMA_CA_State);

    if(pCSMA_CA_State->useAdditionalRetransmitBackoff)
    {
        backOffTime_us += gpHalMac_DoRetransmitBackoffScheduleCalculation(pCSMA_CA_State);
    }
    gpSched_ScheduleEventArg(backOffTime_us, gpHalMac_CCARetry, (void*)pCSMA_CA_State);
}

void gpHalMac_Do_CCAFailRetry(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State)
{
    //From CSMA-CA algorithm, first increase backoff counter, then calculate the delay
    pCSMA_CA_State->backOffCount++;

    //calculate the backoff delay and trigger retry
    gpHalMac_DoBackoffScheduleCalculation(pCSMA_CA_State);
}

void gpHalMac_Do_NoAckRetry(gpHalMac_CSMA_CA_state_t *pCSMA_CA_State)
{
    Bool csma_ca_enable;
    Bool first_boff_is_0;
    gpHal_Address_t pbmOptAddress;

    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pCSMA_CA_State->pbmHandle);
    csma_ca_enable = GP_WB_READ_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(pbmOptAddress);
    first_boff_is_0 = GP_WB_READ_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0(pbmOptAddress);

    //MAC retry, always clear the CCA backoff counts
    pCSMA_CA_State->backOffCount = 0;

    if (csma_ca_enable && !first_boff_is_0)
    {
        //Do not call CCA retry, otherwise backoff would already be increased
        gpHalMac_DoBackoffScheduleCalculation(pCSMA_CA_State);
    }
    else // Reschedule immediately
    {
        gpSched_ScheduleEventArg(0, gpHalMac_CCARetry, (void*)pCSMA_CA_State);
    }

    pCSMA_CA_State->remainingFrameRetries--;
}

//-------------------------------------------------------------------------------------------------------
/* State queue administration */
//-------------------------------------------------------------------------------------------------------

static void gpHalMac_ShiftQueue_CSMA_CA_State(void)
{
    UIntLoop i;
    //Shift to front and invalidate the end
    for(i = 0; i < GPHAL_QTA_MAX_SIZE-1; i++)
    {
        if(gpHalMac_CSMA_CA_state[i+1].pbmHandle == GP_PBM_INVALID_HANDLE)
        {
            //No need to shift further
            break;
        }
        MEMCPY(&gpHalMac_CSMA_CA_state[i], &gpHalMac_CSMA_CA_state[i+1], sizeof(gpHalMac_CSMA_CA_state_t));
    }
    //Invalidate last entry - if none were shifted first entry is invalidated
    gpHalMac_CSMA_CA_state[i].pbmHandle = GP_PBM_INVALID_HANDLE;
}

static void gpHalMac_Trigger_CSMA_CA_State(void)
{
    //Head of queue alyways holds active entry
    if (gpHalMac_CSMA_CA_state[0].pbmHandle != GP_PBM_INVALID_HANDLE)
    {
        gpHal_Address_t pbmOptAddress;
        pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHalMac_CSMA_CA_state[0].pbmHandle);

        //if first backoff is zero or if csma is not enabled for this packet
        if(GP_WB_READ_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0(pbmOptAddress) || (!GP_WB_READ_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(pbmOptAddress)))
        {
            gpHal_DataRequest_base(gpHalMac_CSMA_CA_state[0].pbmHandle);
        }
        else
        {
            gpHalMac_DoBackoffScheduleCalculation(&gpHalMac_CSMA_CA_state[0]);
        }
    }
}

/** Get new state entry - returns a pointer to a free space when called with GP_PBM_INVALID_HANDLE */
gpHalMac_CSMA_CA_state_t* gpHalMac_Get_CSMA_CA_State(UInt8 pbmHandle)
{
    UIntLoop idx;
    for (idx=0; idx<GPHAL_QTA_MAX_SIZE; ++idx)
    {
        if (gpHalMac_CSMA_CA_state[idx].pbmHandle == pbmHandle)
        {
            return &gpHalMac_CSMA_CA_state[idx];
        }
    }
    return NULL;
}

void gpHalMac_Free_CSMA_CA_State(UInt8 pbmHandle)
{
    //Only active entry should be freed
    GP_ASSERT_DEV_EXT(pbmHandle == gpHalMac_CSMA_CA_state[0].pbmHandle);

    //Pop out entry 0 - shift up rest of queue
    gpHalMac_ShiftQueue_CSMA_CA_State();
    //Re trigger queue if needed
    gpHalMac_Trigger_CSMA_CA_State();
}
#endif // GP_HAL_MAC_SW_CSMA_CA

Bool gpHal_IsMacQueueEmpty(void)
{
    if(GP_WB_ENUM_QTA_ENTRY_STATE_EMPTY == GP_WB_READ_QTA_STATE_0())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* Retrieve the default tx power given a IEEE 15.4 channel */
gpHal_TxPower_t gpHal_GetDefaultTransmitPower(gpHal_Channel_t channel)
{
    GP_ASSERT_DEV_INT((channel >= 11) && (channel <= 26));
    return gpHal_pDefaultTransmitPowerTable[channel-11];
}

void gpHal_MacSetMaxTransferTime(UInt32 MacMaxTransferTime)
{
    gpHal_MacMaxTransferTime = MacMaxTransferTime;
}

UInt32 gpHal_MacGetMaxTransferTime(void)
{
    return gpHal_MacMaxTransferTime;
}

void gpHal_MacEnableEnhancedFramePending(gpHal_SourceIdentifier_t srcId, Bool enable)
{
    GP_ASSERT_DEV_INT(srcId < GP_HAL_MAC_NUMBER_OF_RX_SRCIDS);
    GP_WB_WRITE_MACFILT_EVERY_FRAME_IS_DATAREQ(BM(srcId));
}

UInt8 gpHal_GetLastUsedChannel(UInt8 PBMentry)
{
    UInt8 channel = 0xFF; // invalid channel;
    if(gpPd_BufferTypeZigBee == gpPd_GetPdType(gpPd_GetPdFromPBM(PBMentry)))
    {
        gpHal_Address_t pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
        gpHal_SourceIdentifier_t srcId = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_IDX(pbmOptAddress);
        channel = gpHal_GetRxChannel(srcId);
    }
    return channel;
}

