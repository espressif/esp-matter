/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gphal_ISR.c
 *
 *  The file contains the interrupt handling code
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
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_Pbm.h"
#include "gpHal_kx_Fll.h"
#include "gpHal_Statistics.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"          //Containing all uC dependent implementations
#include "gpHal_reg.h"
#include "gpAssert.h"

#ifdef GP_COMP_TXMONITOR
#include "gpTxMonitor.h"
#endif //GP_COMP_TXMONITOR

#include "gpSched.h"

#if defined(GP_DIVERSITY_FREERTOS) && defined(GP_COMP_GPHAL_BLE)
#include "hal_BleFreeRTOS.h"
#endif //(GP_DIVERSITY_FREERTOS) && (GP_COMP_GPHAL_BLE)

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_MAC
//RF  callbacks
static gpHal_DataConfirmCallback_t          gpHal_DataConfirmCallback       = NULL;
static gpHal_DataIndicationCallback_t       gpHal_DataIndicationCallback    = NULL;
gpHal_EDConfirmCallback_t                   gpHal_EDConfirmCallback         = NULL;

//MAC callbacks
gpHal_MacFrameQueued_t                      gpHal_MacFrameQueued = NULL;
gpHal_MacFrameUnqueued_t                    gpHal_MacFrameUnqueued = NULL;
#endif //GP_COMP_GPHAL_MAC

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
static gpHal_AbsoluteEventCallback_t        gpHal_AbsoluteEventCallback[GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS] = { NULL, };
#endif

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
static gpHal_ExternalEventCallback_t        gpHal_ExternalEventCallback = NULL;
#endif


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_MAC
static void gpHal_SwitchChannel(gpHal_Address_t pbmOptAddress,UInt8 retries);
static void gpHal_DataConfirmInterrupt(UInt8 PBMentry);
#endif

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_MAC

static void gpHal_SwitchChannel(gpHal_Address_t pbmOptAddress,UInt8 retries)
{
    //Using A/B/C PBM settings to keep track of SW multi-channel behavior
    //A and B to be looped minimal
    UInt8 channelB = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0B(pbmOptAddress);

    //Only shift through enabled channels - fi just doing SW MAC retries - only channel A enabled
    if(GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN(pbmOptAddress))
    {
        //Move to channel B -> A
        GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOptAddress,channelB);
        if((retries > 1)
                && GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN(pbmOptAddress)) //A -> B -> C still to be looped, move C -> B
        {
            UInt8 channelC = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0C(pbmOptAddress);
            //Move to channel C -> B
            GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0B(pbmOptAddress,channelC);
        }
    }
}

void gpHal_DataConfirmInterrupt(UInt8 PBMentry)
{
    UInt8 result;
    UInt8 ackRequest;
    gpPd_Loh_t pdLoh;
    UInt8 retries;
#if defined(GP_HAL_MAC_SW_CSMA_CA)
    gpHalMac_CSMA_CA_state_t* pCsmaState;
#endif

    gpHal_Address_t pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    // Fetch status
    result = GP_WB_READ_PBM_FORMAT_T_RETURN_CODE(pbmOptAddress);

    //Fetch Ack from options
    ackRequest = GP_WB_READ_PBM_FORMAT_T_GP_ACKED_MODE(pbmOptAddress);

#if defined(GP_HAL_MAC_SW_CSMA_CA)
    gpHalMac_CSMA_CA_state_t *pCSMA_CA_State = gpHalMac_Get_CSMA_CA_State(PBMentry);
    if (result == GP_WB_ENUM_PBM_RETURN_CODE_CCA_FAILURE)
    {
        GP_ASSERT_DEV_INT(pCSMA_CA_State);
        gpHal_StatisticsMacCounters.ccaFails++;
        if (pCSMA_CA_State->backOffCount < pCSMA_CA_State->maxCsmaBackoffs)
        {
            gpHalMac_Do_CCAFailRetry(pCSMA_CA_State);
            return;
        }
    }

    pCsmaState = gpHalMac_Get_CSMA_CA_State(PBMentry);
    if((ackRequest && (result == GP_WB_ENUM_PBM_RETURN_CODE_NO_ACK)) ||
       ((gpPad_CheckPadValid(pCsmaState->padHandle)==gpPad_ResultValidHandle) 
        && gpPad_GetRetransmitOnCcaFail(pCsmaState->padHandle) && (result == GP_WB_ENUM_PBM_RETURN_CODE_CCA_FAILURE)))
    {
        GP_ASSERT_DEV_INT(pCSMA_CA_State);

        if (ackRequest && (result == GP_WB_ENUM_PBM_RETURN_CODE_NO_ACK) && 
            (gpPad_CheckPadValid(pCsmaState->padHandle)==gpPad_ResultValidHandle))
        {
            pCsmaState->useAdditionalRetransmitBackoff = gpPad_GetRetransmitRandomBackoff(pCsmaState->padHandle);
        }
        else
        {
            pCsmaState->useAdditionalRetransmitBackoff = false;
        }

        if(0 < pCSMA_CA_State->remainingFrameRetries)
        {
            gpHal_SwitchChannel(pbmOptAddress, pCSMA_CA_State->remainingFrameRetries);
            gpHalMac_Do_NoAckRetry(pCSMA_CA_State);
            return;
        }
    }
#endif // GP_HAL_MAC_SW_CSMA_CA

    //SW or HW multichannel ? - SW only channel A enabled
    retries = GP_WB_READ_PBM_FORMAT_T_MAX_FRAME_RETRIES(pbmOptAddress);
    if(!ackRequest && retries) //More channels remaining
    {
        gpHal_SwitchChannel(pbmOptAddress,retries);
        retries--;
        GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(pbmOptAddress, retries);
        gpHal_DataRequest_base(PBMentry);
    }
    else
    {
        if(GP_HAL_IS_DATA_CONFIRM_CALLBACK_REGISTERED())
        {
            UInt8 lastChannelUsed;
            UInt8 offset, length;
            gpHal_Result_t halResult;
            gpHal_SourceIdentifier_t srcId;

            //Get result of TX
            length = GP_WB_READ_PBM_FORMAT_T_FRAME_LEN(pbmOptAddress);
            offset = GP_WB_READ_PBM_FORMAT_T_FRAME_PTR(pbmOptAddress);
            /*
             * For compatilibity with k7b and prior.
             */
            halResult = gpHal_ResultSuccess;
            switch(result)
            {
                case GP_WB_ENUM_PBM_RETURN_CODE_TX_SUCCESS:
                    gpHal_StatisticsMacCounters.successTx++;
                    halResult = gpHal_ResultSuccess;    // k7b:GP_WB_ENUM_PBM_RESULT_SUCCESS
                    break;
                case GP_WB_ENUM_PBM_RETURN_CODE_CCA_FAILURE:
                    gpHal_StatisticsMacCounters.failTxChannelAccess++;
                    halResult = gpHal_ResultCCAFailure; // k7b:GP_WB_ENUM_PBM_RESULT_CHANNELACCESSFAILURE
                    break;
                case GP_WB_ENUM_PBM_RETURN_CODE_NO_ACK:
                    gpHal_StatisticsMacCounters.failTxNoAck++;
                    halResult = gpHal_ResultNoAck;      // k7b:GP_WB_ENUM_PBM_RESULT_NOACK
                    break;
                case GP_WB_ENUM_PBM_RETURN_CODE_TX_ABORTED:
                    gpHal_StatisticsMacCounters.failTxChannelAccess++;
                    halResult = gpHal_ResultGrantTimeout;      // k7b:GP_WB_ENUM_PBM_RESULT_NOACK
                    break;
                default:
                    GP_ASSERT_SYSTEM(false);
            }

            //Fetch the channel the packet was sent on
            srcId = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_IDX(pbmOptAddress);
            lastChannelUsed = gpHal_GetRxChannel(srcId);

#if defined(GP_HAL_MAC_SW_CSMA_CA)
            // No SW CSMA/CA follow up if timed transmission, check PBM options for queue type UNTIMED
            if( GP_WB_READ_PBM_FORMAT_T_GP_VQ_SEL(pbmOptAddress) == GP_WB_ENUM_PBM_VQ_UNTIMED)
            {
                const UInt8 retriesDone = pCSMA_CA_State->initialMaxFrameRetries - pCSMA_CA_State->remainingFrameRetries;

                //Update retries in PBM options directly - state will be freed now
                GP_WB_WRITE_PBM_FORMAT_T_TX_RETRY_EXTENDED(pbmOptAddress, retriesDone);

                // Free and retrigger queue
                gpHalMac_Free_CSMA_CA_State(PBMentry);
            }
            else
            {
                // In case of timed tx, the csma-ca state still needs to be cleared
                gpHalMac_Free_CSMA_CA_State(PBMentry);
            }
#endif // GP_HAL_MAC_SW_CSMA_CA
            gpHal_StatisticsMacCounters.txRetries += GP_WB_READ_PBM_FORMAT_T_TX_RETRY_EXTENDED(pbmOptAddress);

            gpPd_cbDataConfirm(PBMentry, offset, length, &pdLoh);

#ifdef GP_COMP_TXMONITOR
            if (gpHal_ResultSuccess == halResult)
            {
                gpTxMonitor_AnnounceTxFinished();
            }
#endif //GP_COMP_TXMONITOR

            if (GP_HAL_IS_FRAME_UNQUEUED_CALLBACK_REGISTERED())
            {
                GP_HAL_CB_FRAME_UNQUEUED();
            }

            GP_HAL_CB_DATA_CONFIRM(halResult, pdLoh, lastChannelUsed);
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
}
#endif //GP_COMP_GPHAL_MAC

#ifdef GP_COMP_GPHAL_BLE
static Bool gpHal_RCIAdvIndIRQ_Handler(UInt16 rciPending)
{
    if (rciPending & GP_WB_RCI_UNMASKED_BLE_ADV_IND_INTERRUPT_MASK)
    {
        gpHal_cbBleAdvertisingIndication(GP_WB_READ_RCI_BLE_ADV_IND_PBM());
#ifdef GP_COMP_TXMONITOR
        gpTxMonitor_AnnounceTxFinished();
#endif
        return true;
    }
    return false;
}
#endif /* GP_COMP_GPHAL_BLE */

//-------------------------------------------------------------------------------------------------------
//  ISR handlers (polled)
//-------------------------------------------------------------------------------------------------------

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

//-------------------------------------------------------------------------------------------------------
//  RCI INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_BLE
void gpHal_Handle_All_RCIAdvIndIRQ(void)
{
    UInt16 rciPending;
    //Using unmasked version - masks are disabled here in internal version
    rciPending = GP_WB_READ_RCI_UNMASKED_INTERRUPTS();

    while (gpHal_RCIAdvIndIRQ_Handler(rciPending))
    {
        rciPending = GP_WB_READ_RCI_UNMASKED_INTERRUPTS();
    }
}
#endif /* GP_COMP_GPHAL_BLE */


// Check LP mask versus implemented handlers
#ifdef GP_COMP_GPHAL_MAC
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_DATA_CNF_0_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_DATA_CNF_1_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_DATA_IND_INTERRUPT_MASK);
#endif
#ifdef GP_COMP_GPHAL_BLE
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_BLE_DATA_CNF_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_BLE_DATA_IND_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_LP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_BLE_ADV_IND_INTERRUPT_MASK);
#endif //GP_COMP_GPHAL_BLE

void gpHal_RCIInterrupt(void)
{
#ifdef GP_COMP_GPHAL_MAC
    UInt8 PBMentry;
    gpHal_Address_t pbmOptAddress;
#endif //GP_COMP_GPHAL_MAC

    UInt16 rciPending;

    GP_STAT_SAMPLE_TIME();

    //Using unmasked version - masks are disabled here in internal version
    rciPending = GP_WB_READ_RCI_UNMASKED_INTERRUPTS();

    if(rciPending == 0)
    {
        // We entered this function, but no interrupt is pending.
        // Handling is done by ISR routine
        return;
    }

    rciPending &= GPHAL_ISR_RCI_LP_ISR_UNMASKED;

    // After masking, we should always have a valid interrupt pending
    GP_ASSERT_SYSTEM(rciPending != 0);

#ifdef GP_COMP_GPHAL_MAC
    if( rciPending & GP_WB_RCI_UNMASKED_DATA_CNF_0_INTERRUPT_MASK)
    {
        PBMentry = GP_WB_READ_RCI_DATA_CNF_0_PBM();
        GP_ASSERT_SYSTEM(GP_HAL_CHECK_PBM_VALID(PBMentry));
        gpHal_DataConfirmInterrupt(PBMentry);
    }
    else if( rciPending & GP_WB_RCI_UNMASKED_DATA_CNF_1_INTERRUPT_MASK)
    {
        //Get result
        if(GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
        {
            UInt8 ed;
            UInt8 scanChannel;
            PBMentry = GP_WB_READ_RCI_DATA_CNF_1_PBM();
            GP_ASSERT_SYSTEM(GP_HAL_CHECK_PBM_VALID(PBMentry));
            pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

            scanChannel = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOptAddress);
            ed = GP_WB_READ_PBM_FORMAT_T_ED_RESULT(pbmOptAddress);

            /* PBM is freed here. */
            gpHal_Scan_EDConfirm(scanChannel, ed);
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
    else if( rciPending & GP_WB_RCI_UNMASKED_DATA_IND_INTERRUPT_MASK)
    {
        if(GP_HAL_IS_DATA_INDICATION_CALLBACK_REGISTERED())
        {
            gpPd_Loh_t pdLoh;
            UInt8 length, offset  = 0;
            gpHal_RxInfo_t rxInfo;

            //Packet has been received, HW has received and notified SW with interrupt
            gpHal_StatisticsMacCounters.totalRx++;

            MEMSET(&rxInfo, 0, sizeof(rxInfo));

            //Shutdown pending Rx windows
            GP_WB_RIB_STOP_RX_ACK_WINDOW();

            PBMentry = GP_WB_READ_RCI_DATA_IND_PBM();
            GP_ASSERT_SYSTEM(PBMentry <  GPHAL_NUMBER_OF_PBMS_USED);
            pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
            offset = GP_WB_READ_PBM_FORMAT_R_FRAME_PTR(pbmOptAddress);
            length = GP_WB_READ_PBM_FORMAT_R_FRAME_LEN(pbmOptAddress);

            if (length > GPHAL_MAX_15_4_PACKET_LENGTH)
            {
                length = GPHAL_MAX_15_4_PACKET_LENGTH;
            }

            gpPd_DataIndication(PBMentry, offset, length, &pdLoh, gpPd_BufferTypeZigBee);
            if (GP_PD_INVALID_HANDLE != pdLoh.handle)
            {
                //Fill in already fetched info by gpPd for compatibility
                rxInfo.rxChannel = gpPd_GetRxChannel(pdLoh.handle);
                GP_HAL_CB_DATA_INDICATION(pdLoh, &rxInfo);
            }
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
    else
#endif //GP_COMP_GPHAL_MAC

#ifdef GP_COMP_GPHAL_BLE
        if(rciPending & GP_WB_RCI_UNMASKED_BLE_DATA_CNF_INTERRUPT_MASK) // BLE TxDataConf
        {
#ifdef GP_COMP_TXMONITOR
            gpTxMonitor_AnnounceTxFinished();
#endif
            gpHal_cbBleDataConfirm(GP_WB_READ_RCI_BLE_DATA_CNF_PBM());
        }
        else if(rciPending & GP_WB_RCI_UNMASKED_BLE_DATA_IND_INTERRUPT_MASK) // BLE RxDataInd
        {
            gpHal_cbBleDataIndication(GP_WB_READ_RCI_BLE_DATA_IND_PBM());
        }
        else if(gpHal_RCIAdvIndIRQ_Handler(rciPending)) //GP_WB_RCI_UNMASKED_BLE_ADV_IND_INTERRUPT_MASK
        {
            // nothing more
        }
        else
#endif // GP_COMP_GPHAL_BLE
        {
            //GP_LOG_SYSTEM_PRINTF("RciPending: %x",0,rciPending);
            GP_ASSERT_SYSTEM(false);
        }
}

//-------------------------------------------------------------------------------------------------------
//  PHY INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------

void gpHal_PHYInterrupt(void)
{
    GP_LOG_PRINTF("PHY INT",0);

    gpHal_FllHandleInterrupts();
}

#endif //defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

//-------------------------------------------------------------------------------------------------------
//  ES INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)

void gpHal_ESInterrupt(void)
{
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT)

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
    if (GP_WB_READ_INT_CTRL_MASKED_ES_EXTERNAL_EVENT_INTERRUPT())
    {
        GP_WB_ES_CLR_EXTERNAL_EVENT_INTERRUPT();   //Clear interrupt
        if(GP_HAL_IS_EXTERNAL_EVENT_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_EXTERNAL_EVENT();
        }
        return;
    }
    else if (GP_WB_READ_INT_CTRL_MASKED_ES_COMP_EVENT_INTERRUPTS())
    {
        GP_WB_WRITE_ES_CLR_COMP_EVENT_INTERRUPTS(0x0F); //Clear interrupt
        if(GP_HAL_IS_EXTERNAL_EVENT_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_EXTERNAL_EVENT();
        }
        return;
    }
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

    //No relative event may be pending
    GP_ASSERT_SYSTEM(0 == GP_WB_READ_INT_CTRL_MASKED_ES_RELATIVE_EVENT_INTERRUPT());
#else
    //Check
    GP_ASSERT_SYSTEM(0 == GP_WB_READ_INT_CTRL_MASKED_ES_EXTERNAL_EVENT_INTERRUPT());
#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT)

    if (GP_WB_READ_INT_CTRL_MASKED_ES_OSCILLATOR_BENCHMARK_DONE_INTERRUPT())
    {
        GP_WB_ES_CLR_OSCILLATOR_BENCHMARK_DONE_INTERRUPT();
        gpHalES_OscillatorBenchmarkDone_Handler();
        return;
    }

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
    {
        UInt16 maskedEventInterrupts      = 0;
        UInt16 maskedEventInterruptsCheck = 0;
        UInt8  interruptIndex             = 0;

        //Look for Absolute interrupts active
        maskedEventInterrupts = GP_WB_READ_ES_UNMASKED_EVENT_INTERRUPTS();

        GP_ASSERT_SYSTEM(maskedEventInterrupts != 0);
        //Minimum one absolute event should be triggered - relative + external events are no longer a source of the ES interrupt
        if (maskedEventInterrupts == 0)
        {
            return;
        }

        for (interruptIndex = 0; interruptIndex <  GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS; interruptIndex++)
        {
            UInt8 eventResult;

            maskedEventInterruptsCheck = ((UInt16) 1 << interruptIndex);

            if ((maskedEventInterrupts & maskedEventInterruptsCheck) == 0)
            {
                continue;
            }

            eventResult = GP_WB_READ_EVENT_RESULT_FIELD(GPHAL_ES_EVENT_NR_TO_START_OFFSET(interruptIndex));

            if((eventResult == GPHAL_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE) || (eventResult == GPHAL_ENUM_EVENT_RESULT_MISSED_TOO_LATE))
            {
                // If an event was too late, update the symbol startup time
                gpHal_cbEventTooLate();
            }

            if (gpHal_AbsoluteEventCallback[interruptIndex])
            {
                gpHal_AbsoluteEventCallback[interruptIndex]();
            }
            // Clear interrupt - calculation cost is larger then a direct 16-bit access
            GP_WB_WRITE_ES_CLR_EVENT_INTERRUPTS(maskedEventInterruptsCheck);

            // Stop handling event interrupts to give possible PIO interrupt priority
            break;
        }
    }
#else
    //All the interrupts should be serviced by now.
    //If no absolute events are used, the ES interrupt should be handled by relative+external code.
    GP_ASSERT_SYSTEM(false);
#endif //GP_COMP_GPHAL_ES_ABS_EVENT
}

#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)

//-------------------------------------------------------------------------------------------------------
//  IPC INTERRUPT HANDLERS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_BLE
// Check LP mask versus implementation of handlers
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_UNEXPECTED_COND_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_EVENT_PROCESSED_INTERRUPTS_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_LAST_CONN_EVENT_CNT_INTERRUPTS_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_STAT_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_SCAN_REQ_RX_INTERRUPT_MASK);
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED & GP_WB_IPC_UNMASKED_GPM2X_TX_POWER_MEAS_DONE_INTERRUPT_MASK);
#endif //GP_COMP_GPHAL_BLE

void gpHal_IpcGpm2XInterrupt(void)
{
    UInt64 unmaskedIpcPending = 0;

    unmaskedIpcPending = GP_WB_READ_IPC_UNMASKED_GPM2X_INTERRUPTS();
    if(unmaskedIpcPending == 0)
    {
        // We entered this function, but no interrupt is pending.
        // Handling is done by ISR routine
        return;
    }

    // only set the ones we are interested in
    unmaskedIpcPending &= GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED;

    // After masking, we should always have a valid interrupt pending
    GP_ASSERT_SYSTEM(unmaskedIpcPending != 0);

#ifdef GP_COMP_GPHAL_BLE
    if (unmaskedIpcPending & GP_WB_IPC_UNMASKED_GPM2X_UNEXPECTED_COND_INTERRUPT_MASK)
    {
#ifdef GP_DIVERSITY_LOG
        UInt8 cond = GP_WB_READ_U8(BLE_MGR_UNEXPECTED_COND_REG_ADDRESS);
        UInt8 arg = GP_WB_READ_U8(BLE_MGR_UNEXPECTED_COND_ARG0_REG_ADDRESS);

        GP_LOG_SYSTEM_PRINTF("Unexpected cond: %u (arg: %u)",0, cond, arg);
#endif //GP_DIVERSITY_LOG
        GP_WB_IPC_CLR_GPM2X_UNEXPECTED_COND_INTERRUPT();
        GP_ASSERT_SYSTEM(false);
    }
    else if(unmaskedIpcPending & (GP_WB_IPC_UNMASKED_GPM2X_EVENT_PROCESSED_INTERRUPTS_MASK))
    {
        UIntLoop i;
        UInt16 events = GP_WB_GET_IPC_UNMASKED_GPM2X_EVENT_PROCESSED_INTERRUPTS_FROM_UNMASKED_GPM2X_INTERRUPTS(unmaskedIpcPending);

        for(i = 0; i < 16; i++)
        {
            if(BIT_TST(events,i))
            {
                //GP_LOG_SYSTEM_PRINTF("Event %i interrupt",0,i);
                GP_WB_WRITE_IPC_CLR_GPM2X_EVENT_PROCESSED_INTERRUPTS(1 << i);
                gpHal_cbBleEventProcessed(i);
            }
        }
    }
    else if(unmaskedIpcPending & GP_WB_IPC_UNMASKED_GPM2X_LAST_CONN_EVENT_CNT_INTERRUPTS_MASK)
    {
        UIntLoop i;
        UInt16 events = GP_WB_GET_IPC_UNMASKED_GPM2X_LAST_CONN_EVENT_CNT_INTERRUPTS_FROM_UNMASKED_GPM2X_INTERRUPTS(unmaskedIpcPending);

        for(i = 0; i < 16; i++)
        {
            if(BIT_TST(events,i))
            {
                //GP_LOG_SYSTEM_PRINTF("Event %i interrupt",0,i);
                GP_WB_WRITE_IPC_CLR_GPM2X_LAST_CONN_EVENT_CNT_INTERRUPTS(1 << i);
                gpHal_cbBleLastConnEventCountReached(i);
            }
        }
    }
    else if(unmaskedIpcPending & GP_WB_IPC_UNMASKED_GPM2X_STAT_INTERRUPT_MASK)
    {
        UInt8 statType;

        statType = GP_WB_READ_U8(BLE_MGR_STAT_TYPE_REG_ADDRESS);

        if(statType == BLE_MGR_STAT_TYPE_ES_TRIGGER_TOO_LATE)
        {
            // The ble ev mgr reported an event that was handled too late
            GP_WB_IPC_CLR_GPM2X_STAT_INTERRUPT();
            gpHal_cbEventTooLate();
        }
    }
    else if(unmaskedIpcPending & GP_WB_IPC_UNMASKED_GPM2X_SCAN_REQ_RX_INTERRUPT_MASK)
    {
        GP_WB_IPC_CLR_GPM2X_SCAN_REQ_RX_INTERRUPT();

        gpHal_cbBleScanRequestReceived();
    }
    else if(unmaskedIpcPending & GP_WB_IPC_UNMASKED_GPM2X_TX_POWER_MEAS_DONE_INTERRUPT_MASK)
    {
        GP_WB_IPC_CLR_GPM2X_TX_POWER_MEAS_DONE_INTERRUPT();
    }
    else
    {
        GP_ASSERT_DEV_INT(false);
    }
#endif // GP_COMP_GPHAL_BLE
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  REGISTER CALLBACK FUNCTIONS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_MAC

//RF register callbacks
void gpHal_RegisterDataConfirmCallback(gpHal_DataConfirmCallback_t callback)
{
    gpHal_DataConfirmCallback=callback;
}

void gpHal_RegisterDataIndicationCallback(gpHal_DataIndicationCallback_t callback)
{
    gpHal_DataIndicationCallback=callback;
}

void gpHal_RegisterEDConfirmCallback(gpHal_EDConfirmCallback_t callback)
{
    gpHal_EDConfirmCallback=callback;
}

//MAC callbacks
void gpHal_RegisterMacFrameQueuedCallback(gpHal_MacFrameQueued_t callback)
{
    gpHal_MacFrameQueued = callback;
}

void gpHal_RegisterMacFrameUnqueuedCallback(gpHal_MacFrameUnqueued_t callback)
{
    gpHal_MacFrameUnqueued = callback;
}
#endif //GP_COMP_GPHAL_MAC

//ES register callbacks
#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
void gpHal_RegisterAbsoluteEventCallback(gpHal_AbsoluteEventCallback_t callback, UInt8 eventNbr)
{
    GP_ASSERT_DEV_EXT(eventNbr < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS);
    gpHal_AbsoluteEventCallback[eventNbr]=callback;
}

#endif //GP_COMP_GPHAL_ES_ABS_EVENT

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
gpHal_ExternalEventCallback_t gpHal_RegisterExternalEventCallback(gpHal_ExternalEventCallback_t callback)
{
    gpHal_ExternalEventCallback_t oldCb = gpHal_ExternalEventCallback;

    gpHal_ExternalEventCallback = callback;

    return oldCb;
}
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

//-------------------------------------------------------------------------------------------------------
//  ISR handlers (called directly from ISR)
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//  RCI INTERRUPT HANDLERS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_BLE
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_RCI_HP_ISR_UNMASKED & GP_WB_RCI_UNMASKED_BLE_CONN_REQ_IND_INTERRUPT_MASK);
#endif //GP_COMP_GPHAL_BLE
void gpHal_ISR_RCIInterrupt(UInt16 highPrioRciPending)
{
    GP_STAT_SAMPLE_TIME();

#ifdef GP_COMP_GPHAL_BLE
    if(highPrioRciPending & GP_WB_RCI_UNMASKED_BLE_CONN_REQ_IND_INTERRUPT_MASK) // Rx_ConnectReqInd_Irq
    {
#ifdef GP_DIVERSITY_FREERTOS
        hal_BleIsrRciDefer(GP_BLE_TASK_EVENT_RCI_CONN_REQ, GP_WB_READ_RCI_BLE_CONN_REQ_IND_PBM());
#else
        gpHal_cbBleConnectionRequestIndication(GP_WB_READ_RCI_BLE_CONN_REQ_IND_PBM());
#endif //GP_DIVERSITY_FREERTOS
    }
#endif // GP_COMP_GPHAL_BLE
}

//-------------------------------------------------------------------------------------------------------
//  IPC INTERRUPT HANDLERS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_BLE
GP_COMPILE_TIME_VERIFY(GPHAL_ISR_IPC_GPM_HP_ISR_MASK & GP_WB_IPC_UNMASKED_GPM2X_CONN_REQ_TX_INTERRUPT_MASK);
#endif //GP_COMP_GPHAL_BLE
void gpHal_ISR_IPCGPMInterrupt(UInt64 highPrioIpcPending)
{
#ifdef GP_COMP_GPHAL_BLE
    // Connect request transmitted
    if(highPrioIpcPending & GP_WB_IPC_UNMASKED_GPM2X_CONN_REQ_TX_INTERRUPT_MASK)
    {
        // Clear interrupt
        GP_WB_IPC_CLR_GPM2X_CONN_REQ_TX_INTERRUPT();
#ifdef GP_DIVERSITY_FREERTOS
        hal_BleIsrIPCGPMDefer();
#else
        gpHal_cbBleConnectionRequestConfirm();
#endif //GP_DIVERSITY_FREERTOS
    }
#endif // GP_COMP_GPHAL_BLE
}
