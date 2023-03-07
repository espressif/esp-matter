/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   This file contains the implementation of the MAC scan functions
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

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MAC.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "hal.h"
#include "gpBsp.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#define GP_HAL_SYMBOL_DURATION 16
#define GP_HAL_NUMBER_OF_CHANNELS 16
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpHal_channelScan {
    UInt16 todoChannels;
    UInt16 scanMask;
    UInt8 pbmEdScan;
    UInt8 protoRssiLst[GP_HAL_NUMBER_OF_CHANNELS];
} gpHal_channelScan_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static gpHal_channelScan_t gpHal_channelScan;
extern gpHal_EDConfirmCallback_t gpHal_EDConfirmCallback;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void gpHalScan_doEmptyEDConfirm(void)
{
    if (GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
    {
        GP_HAL_CB_ED_CONFIRM(0, gpHal_channelScan.protoRssiLst);
    }
}

static void gpHalScan_ResetScanVariables(void)
{
    gpHal_channelScan.todoChannels = 0;
    gpHal_channelScan.scanMask = 0;
    gpHal_channelScan.pbmEdScan = GP_PBM_INVALID_HANDLE;
    MEMSET(gpHal_channelScan.protoRssiLst, 0, sizeof(gpHal_channelScan.protoRssiLst));
}

void gpHal_InitScan(void)
{
    gpHalScan_ResetScanVariables();
    gpHal_EDConfirmCallback = NULL;
}

//inspired on static void rap_format_t_reset_options(UInt16 optsbase)
static void gpHalScan_InitOptions(UInt8 pbmHandle, UInt32 timeUs)
{
    gpHal_Address_t optsbase;
    UInt8 u8 = 0;

    optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    GP_WB_SET_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CSMA_CA_ENABLE_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_TREAT_CSMA_FAIL_AS_NO_ACK_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_VQ_SEL_TO_GP_FLOW_CTRL_0(u8, GP_WB_ENUM_PBM_VQ_UNTIMED); //Selecting untimed queue
    GP_WB_SET_PBM_FORMAT_T_GP_SKIP_CAL_TX_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_0(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_CONFIRM_QUEUE_TO_GP_FLOW_CTRL_1(u8, 1); //RCI int Confirm 1
    GP_WB_SET_PBM_FORMAT_T_GP_ACKED_MODE_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_ANTENNA_TO_GP_FLOW_CTRL_1(u8, GP_WB_READ_RX_ANT_USE_ZB());
    GP_WB_SET_PBM_FORMAT_T_GP_FCS_INSERT_DIS_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_STOP_RX_WINDOW_ON_PBM_FULL_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_ED_SCAN_TO_GP_FLOW_CTRL_1(u8, true); //Enabling ED scan
    GP_WB_SET_PBM_FORMAT_T_GP_RX_DURATION_VALID_TO_GP_FLOW_CTRL_1(u8, true); //Enabling Rx window for scan
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_1(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_IDX_TO_GP_FLOW_CTRL_2(u8, GPHAL_ED_SCAN_CHANNEL_IDX);
    GP_WB_SET_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH1_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_2(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_MODE_CTRL_TO_EXTERNAL_PA_FEM_SETTINGS(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_EXT_ANTENNA_TO_EXTERNAL_PA_FEM_SETTINGS(u8, 0);
    GP_WB_WRITE_PBM_FORMAT_T_EXTERNAL_PA_FEM_SETTINGS(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_TX_POWER_TO_PA_POWER_SETTINGS(u8, 0); // +/-0dBm (+1 being added by channel compesation)
    GP_WB_WRITE_PBM_FORMAT_T_PA_POWER_SETTINGS(optsbase, u8);

    GP_WB_WRITE_PBM_FORMAT_T_RETURN_CODE(optsbase, 7);    //only for test and debug
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION(optsbase, timeUs/16);
}

static UInt8 gpHalScan_GetNextChannel(void)
{
    UInt16 channel;
    /* Get the next channel to be scanned */
    for (channel = 0; channel < GP_HAL_NUMBER_OF_CHANNELS; channel++)
    {
        if(BIT_TST(gpHal_channelScan.todoChannels, channel))
        {
            break;
        }
    }
    return (UInt8)channel;
}

static void gpHalScan_EDQueueNext(void)
{
    UInt8 channel;
    gpHal_Address_t pbmOtpAddress;

    /* Get the next channel to be scanned */
    channel = gpHalScan_GetNextChannel();
    pbmOtpAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHal_channelScan.pbmEdScan);

    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOtpAddress, channel);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN(pbmOtpAddress, 1);

    //Queue the frame
    GP_WB_WRITE_QTA_PBEFE_DATA_REQ(gpHal_channelScan.pbmEdScan);
}

void gpHal_Scan_EDConfirm(UInt8 channel, UInt8 protoRssi)
{
    gpHal_channelScan.protoRssiLst[channel] = protoRssi;        // store the result for previously scanned channel

    /* If this was set to 0 during interrupt we could get
     * new running EDRequests while EDConfirm was unfinished */
    GP_ASSERT_SYSTEM(gpHal_channelScan.todoChannels);

    BIT_SET(gpHal_channelScan.scanMask, channel); // mark a scanned channel
    BIT_CLR(gpHal_channelScan.todoChannels, channel); // and remove it from the list of channels to do (still)

    if (gpHal_channelScan.todoChannels)
    {
        /* Reuse the PBM */
        gpHalScan_EDQueueNext();
    }
    else
    {
        /* All channels scanned, PBM can be freed */
        gpHal_FreeHandle(gpHal_channelScan.pbmEdScan);
        if (GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_ED_CONFIRM(gpHal_channelScan.scanMask, gpHal_channelScan.protoRssiLst);
        }
        gpHalScan_ResetScanVariables();
    }
}


static void gpHalScan_Stop(void)
{
    UInt16 currentChannel; //16-bit for use in mask

    if (!gpHal_channelScan.todoChannels)
    {
        //No scan running
        return;
    }

    //Stop further scans
    //Next channel in todoChannels is the one currently scanned
    //Set todo to this last one only and let confirm come as usual
    currentChannel = gpHalScan_GetNextChannel();
    gpHal_channelScan.todoChannels = BM(currentChannel);
}

//-------------------------------------------------------------------------------------------------------
//  MAC ED REQUEST FUNCTION
//-------------------------------------------------------------------------------------------------------
gpHal_Result_t gpHal_EDRequest(UInt32 time_us , UInt16 channelMask )
{
    if(time_us == 0 && channelMask == 0x0)
    {
        gpHalScan_Stop();
        return gpHal_ResultSuccess;
    }

    if (gpHal_channelScan.todoChannels)   // still busy while new request is made ?
    {
        return gpHal_ResultBusy;
    }

    if (!channelMask) // if 0 channels to scan we schedule empty event, so that the expected flow still applies
    {
        gpHalScan_doEmptyEDConfirm();
        return gpHal_ResultSuccess;
    }

    if (time_us < 144)
    {
        time_us = 144;
    }
    time_us -= 16;

    gpHalScan_ResetScanVariables();
    gpHal_channelScan.pbmEdScan = gpHal_GetHandle(GPHAL_PBM_15_4_MAX_SIZE);
    if(!GP_HAL_CHECK_PBM_VALID(gpHal_channelScan.pbmEdScan))
    {
        return gpHal_ResultBusy;
    }
    gpHal_channelScan.todoChannels = channelMask;

    gpHalScan_InitOptions(gpHal_channelScan.pbmEdScan, time_us);

    gpHalScan_EDQueueNext();          // will start the loop over all channels that must be scanned
    return gpHal_ResultSuccess;
}

UInt8 gpHal_CalculateED( UInt8 protoED)
{
    UInt16 result;
    Int8 rssi;

    rssi = gpHal_CalculateRSSI(protoED);

    // Floor
    if(rssi < -92)
    {
        result = 0;
    }
    // Ceil
    else if(rssi > -42)
    {
        result = 0xFF;
    }
    // Range
    else
    {
        // Let result start on zero and multiply with 5.1 to map the 50 dB range to 0 -> 0xFF (255/50)
        result = (rssi + 92) * 51;
        result /= 10;
    }

    return (UInt8)result;
}

