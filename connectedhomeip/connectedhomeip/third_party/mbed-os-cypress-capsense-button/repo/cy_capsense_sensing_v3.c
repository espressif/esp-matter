/***************************************************************************//**
* \file cy_capsense_sensing_v3.c
* \version 3.0
*
* \brief
* This file contains the source of functions common for different scanning
* modes.
*
********************************************************************************
* \copyright
* Copyright 2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <stddef.h>
#include <string.h>
#include "cy_syslib.h"
#include "cy_sysclk.h"
#include "cy_gpio.h"
#include "cy_device_headers.h"
#include "cy_capsense_sensing_v3.h"
#include "cy_capsense_generator_v3.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_processing.h"
#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
    #include "cy_msc.h"
    #include "cy_dmac.h"
#endif

#if (defined(CY_IP_M0S8MSCV3))

/*******************************************************************************
* Local definition
*******************************************************************************/
#define CY_CAPSENSE_CSD_CCOMP_CALC_DIV      (4u * CY_CAPSENSE_PERCENTAGE_100)
#define CY_CAPSENSE_CSX_CCOMP_CALC_DIV      (8u * CY_CAPSENSE_PERCENTAGE_100)



/*******************************************************************************
* Constants
*******************************************************************************/
const cy_stc_msc_base_config_t cy_capsense_mscCfg = CY_CAPSENSE_MSC_CONFIG_DEFAULT;

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/
static void Cy_CapSense_SetModClkDivider(
                const cy_stc_capsense_context_t * context);

/** \} \endcond */


/*******************************************************************************
* Function Name: Cy_CapSense_ScanAllSlots
****************************************************************************//**
*
* Initiates the non-blocking scan of all slots. Scanning is
* initiated only if no scan is in progress. Scan finishing can be
* checked by the Cy_CapSense_IsBusy() function.
*
* This function initiates a scan only for the first slot for all channels
* and then exits. Scans for the remaining slots in the Interrupt-driven scan mode
* are initiated
* in the interrupt service routine (part of middleware) trigged at the end
* of each scan completion for each channel. If the syncMode field in the
* cy_stc_capsense_common_config_t structure is set to CY_CAPSENSE_SYNC_MODE_OFF,
* then the next slot scan for the channel with the fired interrupt,
* will start regardless of the another channel readiness for the next scan.
* If the syncMode field is set to CY_CAPSENSE_SYNC_INTERNAL (for single-chip projects)
* or to CY_CAPSENSE_SYNC_EXTERNAL (for multi-chip projects),
* then the next slot scan for the channel with the fired interrupt,
* will start in lockstep with another channels after they all are ready
* for the next scan (the next scan configuration is loaded into the channel MSC HW block).
* Scans for the remaining slots in CS-DMA scan mode are initiated
* by DMAC trigged at the end
* of each scan completion for each channel. The channel scan synchronization is
* performed as in Interrupt-driven scan mode. After all slots are scanned,
* the FRAME interrupt is fired and the interrupt service routine (part of middleware)
* updates the busy status.
*
* The status of the current scan should be
* checked using the Cy_CapSense_IsBusy() function, where there are separate busy bits  for each
* channel and the application program waits until all scans are finished
* prior to starting a next scan by using this function.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_HW_BUSY          - The HW is busy with the previous scan.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ScanAllSlots(
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context)
    {
        capStatus = Cy_CapSense_ScanSlots(0u, context->ptrCommonConfig->numSlots, context);
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ScanSlots
****************************************************************************//**
*
* Initiates the non-blocking scan of specified slots. Scanning is
* initiated only if no scan is in progress. Scan finishing can be
* checked by the Cy_CapSense_IsBusy() function.
*
* This function initiates a scan only for the first specified slot for all channels
* and then exits. Scans for the remaining slots in the interrupt-driven scan mode
* are initiated
* in the interrupt service routine (part of middleware) trigged at the end
* of each scan completion for each channel. If the syncMode field in the
* cy_stc_capsense_common_config_t structure is set to CY_CAPSENSE_SYNC_MODE_OFF,
* then the next slot scan for the channel with the fired interrupt,
* will start regardless of the another channel readiness for the next scan.
* If the syncMode field is set to CY_CAPSENSE_SYNC_INTERNAL (for single-chip projects)
* or to CY_CAPSENSE_SYNC_EXTERNAL (for multi-chip projects),
* then the next slot scan for the channel with the fired interrupt,
* will start in lockstep with another channels after they all are ready
* for the next scan.
* The scan for the remaining slots in CS-DMA scan mode are initiated
* by DMAC trigged at the end
* of each scan completion for each channel. The channel scan synchronization is
* performed as in Interrupt-driven scan mode. After all slots are scanned,
* the FRAME interrupt is fired and the interrupt service routine (part of middleware)
* updates the busy status.
*
* \param startSlotId
* The slot ID scan will be started from.
*
* \param numberSlots
* The number of slots will be scanned.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_HW_BUSY          - The HW is busy with the previous scan.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ScanSlots(
                uint32_t startSlotId,
                uint32_t numberSlots,
                cy_stc_capsense_context_t * context)
{
    uint32_t wdIndex;
    uint32_t snsIndex;
    uint32_t slotValue;
    uint32_t curChIndex;
    uint32_t curSlotIndex;
    const cy_stc_capsense_common_config_t * ptrCommonCfg = context->ptrCommonConfig;
    uint32_t * ptrSensorFrame;
    uint32_t scanSlotIndexValid;
    uint32_t lastSlot = startSlotId + numberSlots - 1u;
    uint32_t sensorFrame[CY_MSC_6_SNS_REGS] = {0u, 0u, 0u, 0u, 0u, 0u};
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if ((NULL != context) && (0u != numberSlots))
    {
        if (ptrCommonCfg->numSlots > lastSlot)
        {
            if (CY_CAPSENSE_NOT_BUSY != (context->ptrCommonContext->status & CY_CAPSENSE_BUSY_ALL_CH_MASK))
            {
                /* Previous widget is being scanned. Return error. */
                capStatus = CY_CAPSENSE_STATUS_HW_BUSY;
            }
            else
            {
                context->ptrInternalContext->slotIndex = (uint16_t)startSlotId;
                context->ptrInternalContext->currentSlotIndex = (uint16_t)startSlotId;
                context->ptrInternalContext->endSlotIndex = (uint16_t)lastSlot;

                for (curChIndex = 0u; curChIndex < ptrCommonCfg->numChannels; curChIndex++)
                {
                    context->ptrActiveScanSns[curChIndex].currentChannelSlotIndex = startSlotId;
                }

                /* Configure DMA resources for each channel */
                if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == ptrCommonCfg->scanningMode)
                {
                    for (curChIndex = 0u; curChIndex < ptrCommonCfg->numChannels; curChIndex++)
                    {
                        Cy_CapSense_ConfigureDmaResource(curChIndex, context);
                    }
                }

                if (CY_CAPSENSE_SCAN_MODE_INT_DRIVEN == ptrCommonCfg->scanningMode)
                {
                    /* Initiates the frame start for each channel in interrupt driven scan mode */
                    for (curChIndex = 0u; curChIndex < ptrCommonCfg->numChannels; curChIndex++)
                    {
                        Cy_MSC_WriteReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                         CY_MSC_REG_OFFSET_FRAME_CMD, MSC_FRAME_CMD_START_FRAME_Msk);
                    }
                }

                /* Initialize all enabled MSC channels for scan */
                for (curChIndex = 0u; curChIndex < ptrCommonCfg->numChannels; curChIndex++)
                {
                    Cy_CapSense_SetBusyFlags(curChIndex, context);
                    curSlotIndex = startSlotId + (curChIndex + ptrCommonCfg->channelOffset) * ptrCommonCfg->numSlots;
                    slotValue = context->ptrScanSlots[curSlotIndex].wdId;
                    if(CY_CAPSENSE_SLOT_EMPTY == slotValue)
                    {
                        /* CY_ID#XXXX */
                        scanSlotIndexValid = curSlotIndex;
                        if(ptrCommonCfg->numChannels > 1u)
                        {
                            if(curSlotIndex < ptrCommonCfg->numSlots)
                            {
                                scanSlotIndexValid += ptrCommonCfg->numSlots;
                            }
                            else
                            {
                                scanSlotIndexValid -= ptrCommonCfg->numSlots;
                            }
                        }
                        else
                        {
                            scanSlotIndexValid = curSlotIndex;
                        }
                    }
                    else if (CY_CAPSENSE_SLOT_SHIELD_ONLY <= slotValue)
                    {
                        scanSlotIndexValid = (context->ptrScanSlots[curSlotIndex].snsId * ptrCommonCfg->numSlots) +
                                    (curSlotIndex % ptrCommonCfg->numSlots);
                    }
                    else
                    {
                        scanSlotIndexValid = curSlotIndex;
                    }
                    /* Initializes for each channel the active sensor structure for the current sensor */
                    wdIndex = context->ptrScanSlots[scanSlotIndexValid].wdId;
                    snsIndex = context->ptrScanSlots[scanSlotIndexValid].snsId;
                    Cy_CapSense_InitActivePtr(curChIndex, wdIndex, snsIndex, context);
                    if (CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection)
                    {
                        Cy_CapSense_ConfigureAnalogMuxResource(curChIndex, context);
                        /* Getting the sensor frame configuration */
                        ptrSensorFrame = &sensorFrame[0u];
                        Cy_CapSense_GenerateSensorConfig(curChIndex, curSlotIndex, ptrSensorFrame, context);
                        /* Configure the last slot */
                        if (1u == numberSlots)
                        {
                            sensorFrame[CY_CAPSENSE_SNS_CTL_INDEX] |= MSC_SNS_CTL_LAST_Msk;
                        }
                        if (CY_MSC_5_SNS_REGS == context->ptrInternalContext->snsConfigSize)
                        {
                            ptrSensorFrame++;
                        }
                    }
                    else
                    {
                        /* Configure the last slot for each channel */
                        context->ptrSensorFrameContext[(lastSlot + 1u +
                            (curChIndex + ptrCommonCfg->channelOffset) * ptrCommonCfg->numSlots) *
                                context->ptrInternalContext->snsConfigSize - 1u] |= MSC_SNS_CTL_LAST_Msk;
                        ptrSensorFrame = &context->ptrSensorFrameContext[(startSlotId +
                            (curChIndex + ptrCommonCfg->channelOffset) * ptrCommonCfg->numSlots) *
                                context->ptrInternalContext->snsConfigSize];
                    }

                    if ((NULL != context->ptrInternalContext->ptrSSCallback) &&
                        ((curChIndex + ptrCommonCfg->channelOffset) == (ptrCommonCfg->numChannels - 1u)))
                    {
                        context->ptrInternalContext->ptrSSCallback((cy_stc_active_scan_sns_t *)&context->ptrActiveScanSns[0u]);
                    }

                    if (CY_CAPSENSE_SCAN_MODE_INT_DRIVEN == ptrCommonCfg->scanningMode)
                    {
                        Cy_MSC_ConfigureScan(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                             context->ptrInternalContext->snsConfigSize,
                                             ptrSensorFrame);
                    }
                }

                if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == ptrCommonCfg->scanningMode)
                {
                    /* Initiates the frame start for each channel in DMA driven scan mode */
                    for (curChIndex = 0u; curChIndex < ptrCommonCfg->numChannels; curChIndex++)
                    {
                        Cy_MSC_WriteReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                         CY_MSC_REG_OFFSET_FRAME_CMD, MSC_FRAME_CMD_START_FRAME_Msk);
                    }
                }

                capStatus = CY_CAPSENSE_STATUS_SUCCESS;
            }
        }
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ScanAllWidgets
****************************************************************************//**
*
* Initiates the non-blocking scan for all widgets/sensors. Scanning is
* initiated only if no scan is in progress. Scan finishing can be
* checked by the Cy_CapSense_IsBusy() function.
*
* The function is the wrapper for the Cy_CapSense_ScanAllSlots() function
* to provide the backward compatibility.
*
* \note
* The function operates only in single-channel projects for a while.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_HW_BUSY          - The HW is busy with the previous scan.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ScanAllWidgets(cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context)
    {
        if (1u >= context->ptrCommonConfig->numChannels)
        {
            capStatus = Cy_CapSense_ScanSlots(0u, context->ptrCommonConfig->numSlots, context);
        }
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ScanWidget
****************************************************************************//**
*
* Initiates the scanning of all sensors in the widget. Scanning is
* initiated only if no scan is in progress. Scan finishing can be
* checked by the Cy_CapSense_IsBusy() function.
*
* The function uses the Cy_CapSense_ScanSlots() function with the parameters of
* startSlotId and numberSlots retrieved from the firstSlotId and numSlots
* fields of the cy_stc_capsense_widget_config_t structure.
*
* \note
* The function operates only in single-channel projects for a while.

* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_HW_BUSY          - The HW is busy with the previous scan.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ScanWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context)
    {
        if (1u >= context->ptrCommonConfig->numChannels)
        {
            if (widgetId < context->ptrCommonConfig->numWd)
            {
                capStatus = Cy_CapSense_ScanSlots(context->ptrWdConfig[widgetId].firstSlotId,
                        context->ptrWdConfig[widgetId].numSlots, context);
            }
        }
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ScanSensor
****************************************************************************//**
*
* Initiates the scanning of the selected sensor in the widget. Scanning is
* initiated only if no scan is in progress. Scan finishing can be
* checked by the Cy_CapSense_IsBusy() function.
*
* \note
* The function is not available for a while.

* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param sensorId
* Specifies the ID number of the sensor within the widget. A macro for the
* sensor ID within a specified widget can be found in the cycfg_capsense.h
* file defined as CY_CAPSENSE_<WIDGET_NAME>_SNS<SENSOR_NUMBER>_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_HW_BUSY          - The HW is busy with the previous scan.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ScanSensor(
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context)
    {
        if (1u >= context->ptrCommonConfig->numChannels)
        {
            if (widgetId < context->ptrCommonConfig->numWd)
            {
                if (sensorId < context->ptrWdConfig[widgetId].numSns)
                {
                    capStatus = Cy_CapSense_ScanSlots((sensorId + context->ptrWdConfig->firstSlotId), 1u, context);
                }
            }
        }
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateAllSlots
****************************************************************************//**
*
* Executes the CapDAC calibration for all the sensors in all widgets in
* the project to defined target values.
*
* This function detects the sensing method used by each widget and performs
* a successive approximation search algorithm to find the appropriate reference
* and compensation CapDAC (if enabled) values for all sensors to make
* sensor raw counts closest to the defined value levels.
*
* This function can be used only if the Enable CapDAC auto-calibration parameter
* is enabled for CSD and/or CSX widgets.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM        - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_CALIBRATION_FAIL - The calibration failed if software
*                                         watchdog timeout occurred
*                                         during any calibration scan,
*                                         the scan was not completed, or
*                                         resulted raw counts
*                                         are outside the limits.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_CalibrateAllSlots(
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t calibStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    uint32_t curSlotIndex;
    uint32_t curWdIndex;
    uint32_t csdTarget;
    uint32_t csxTarget;

    if ((NULL != context) && (0u != context->ptrCommonConfig->numSlots))
    {
        calibStatus = CY_CAPSENSE_STATUS_SUCCESS;
        if (((context->ptrCommonConfig->csdEn == CY_CAPSENSE_ENABLE) &&
             (context->ptrCommonConfig->csdCdacAutocalEn == CY_CAPSENSE_ENABLE)) ||
             ((context->ptrCommonConfig->csxEn == CY_CAPSENSE_ENABLE) &&
              (context->ptrCommonConfig->csxCdacAutocalEn == CY_CAPSENSE_ENABLE)))
        {
            context->ptrCommonContext->status |= CY_CAPSENSE_BUSY_CALIBRATION;
            csdTarget = context->ptrCommonConfig->csdRawTarget;
            csxTarget = context->ptrCommonConfig->csxRawTarget;
            for (curSlotIndex = 0u; curSlotIndex < context->ptrCommonConfig->numSlots; curSlotIndex++)
            {
                /* Calibrate all sensors in slot */
                calibStatus = Cy_CapSense_CalibrateSlot(curSlotIndex, csdTarget, csxTarget, context);
                if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
                {
                    calibStatus = CY_CAPSENSE_STATUS_CALIBRATION_FAIL;
                    break;
                }
            }
            if (CY_CAPSENSE_STATUS_SUCCESS == calibStatus)
            {
                context->ptrCommonContext->status |= CY_CAPSENSE_BUSY_VERIFY_CALIBRATION;
                for (curWdIndex = 0u; curWdIndex < context->ptrCommonConfig->numWd; curWdIndex++)
                {
                    /* Normalize all widgets */
                    calibStatus |= Cy_CapSense_NormalizeCdac(curWdIndex, context);;
                }

                if ((context->ptrCommonConfig->csdCdacCompEn == CY_CAPSENSE_ENABLE) ||
                    (context->ptrCommonConfig->csxCdacCompEn == CY_CAPSENSE_ENABLE))
                {
                    for (curSlotIndex = 0u; curSlotIndex < context->ptrCommonConfig->numSlots; curSlotIndex++)
                    {
                        /* Calibrate all sensors in slot */
                        calibStatus = Cy_CapSense_CalibrateSlot(curSlotIndex, csdTarget, csxTarget, context);
                        if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
                        {
                            calibStatus = CY_CAPSENSE_STATUS_CALIBRATION_FAIL;
                            break;
                        }
                    }
                }

                for (curWdIndex = 0u; curWdIndex < context->ptrCommonConfig->numWd; curWdIndex++)
                {
                    /* Check calibration result */
                    calibStatus |= Cy_CapSense_VerifyCalibration(curWdIndex, csdTarget, csxTarget, context);
                }

                if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
                {
                    calibStatus = CY_CAPSENSE_STATUS_CALIBRATION_FAIL;
                }

                context->ptrCommonContext->status &= (uint32_t)~CY_CAPSENSE_BUSY_VERIFY_CALIBRATION;
            }
            context->ptrCommonContext->status &= (uint32_t)~CY_CAPSENSE_BUSY_CALIBRATION;
        }
    }

    return (calibStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateAllWidgets
****************************************************************************//**
*
* Calibrates CapDACs for all widgets.
*
* The function is the wrapper for the Cy_CapSense_CalibrateAllSlots() function
* to provide the backward compatibility.
*
* \note
* The function operates only in single-channel projects for a while.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM     - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_CALIBRATION_FAIL - The calibration failed if software
*                                         watchdog timeout occurred
*                                         during any calibration scan,
*                                         the scan was not completed, or
*                                         resulted raw counts
*                                         are outside the limits.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_CalibrateAllWidgets(
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t calibStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context)
    {
        if (1u >= context->ptrCommonConfig->numChannels)
        {
            calibStatus = Cy_CapSense_CalibrateAllSlots(context);
        }
    }

    return (calibStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateWidget
****************************************************************************//**
*
* Executes the CapDAC calibration for all the sensors in the specified widget
* to the default target value.
*
* This function performs exactly the same tasks as
* Cy_CapSense_CalibrateAllWidgets(), but only for a specified widget.
*
* \note
* The function operates only in single-channel projects for a while.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_PARAM     - The input parameter is invalid.
* - CY_CAPSENSE_STATUS_CALIBRATION_FAIL - The calibration failed if software
*                                         watchdog timeout occurred
*                                         during any calibration scan,
*                                         the scan was not completed, or
*                                         resulted raw counts
*                                         are outside the limits.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_CalibrateWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t calibStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    uint32_t curSlotIndex;
    uint32_t csdTarget;
    uint32_t csxTarget;
    uint32_t firstSlotIdx;
    uint32_t slotsNum;

    if (NULL != context)
    {
        if (1u == context->ptrCommonConfig->numChannels)
        {
            if (widgetId < context->ptrCommonConfig->numWd)
            {
                /* Add check for AutocalEn at this widget */
                context->ptrCommonContext->status |= CY_CAPSENSE_BUSY_CALIBRATION;
                csdTarget = context->ptrCommonConfig->csdRawTarget;
                csxTarget = context->ptrCommonConfig->csxRawTarget;
                firstSlotIdx = context->ptrWdConfig[widgetId].firstSlotId;
                slotsNum = firstSlotIdx + context->ptrWdConfig[widgetId].numSlots;

                for (curSlotIndex = firstSlotIdx; curSlotIndex < slotsNum; curSlotIndex++)
                {
                    /* Calibrate all sensors in slot */
                    calibStatus = Cy_CapSense_CalibrateSlot(curSlotIndex, csdTarget, csxTarget, context);
                    if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
                    {
                        break;
                    }
                }

                /* Normalize all widgets */
                calibStatus |= Cy_CapSense_NormalizeCdac(widgetId, context);
                context->ptrCommonContext->status &= (uint32_t)~CY_CAPSENSE_BUSY_CALIBRATION;

                /* Check calibration result */
                calibStatus |= Cy_CapSense_VerifyCalibration(widgetId, csdTarget, csxTarget, context);
            }
        }
    }

    return (calibStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_WatchdogCyclesNum
****************************************************************************//**
*
* Converts the specified time into number of CPU cycles.
*
* \param desiredTimeUs
* The time (delay) specified in us.
*
* \param cpuFreqMHz
* The CPU frequency in MHz.
*
* \param cyclesPerLoop
* The number of cycles per a loop.
*
*******************************************************************************/
uint32_t Cy_CapSense_WatchdogCyclesNum(
                uint32_t desiredTimeUs,
                uint32_t cpuFreqMHz,
                uint32_t cyclesPerLoop)
{
    uint32_t retVal;

    if(0uL != cyclesPerLoop)
    {
        retVal = (desiredTimeUs * cpuFreqMHz) / cyclesPerLoop;
    }
    else
    {
        retVal = 0xFFFFFFFFuL;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SsInitialize
****************************************************************************//**
*
* Performs the hardware and firmware initialization required for proper operation
* of the CapSense middleware. This function is called from
* the Cy_CapSense_Init() prior to calling any other function of the middleware.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return status
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_SsInitialize(cy_stc_capsense_context_t * context)
{
    uint32_t curChIndex;
    cy_en_msc_status_t mscStatus;
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    const cy_stc_capsense_common_config_t * ptrCommonCfg = context->ptrCommonConfig;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;
    uint32_t i;

    /* Configure the modulator clock and then start it */
    Cy_CapSense_SetModClkDivider(context);

    Cy_CapSense_SetIOsInDefaultState(context);
    Cy_CapSense_SetShieldPinsInDefaultState(context);

    /* Configuring the CIC filter BIT_FORMAT to UNSIGNED */
    context->ptrInternalContext->filterBitFormat = 0u;

    /* Initialize all enabled MSC channels for scan */
    for (curChIndex = 0u; curChIndex < context->ptrCommonConfig->numChannels; curChIndex++)
    {
        /* Generate base frame configurations for all enabled MSC channels */
        capStatus = Cy_CapSense_GenerateBaseConfig(curChIndex, context);

        if (CY_CAPSENSE_STATUS_SUCCESS != capStatus)
        {
            break;
        }

        /* Reset the sense method of all channels */
        context->ptrActiveScanSns[curChIndex].currentSenseMethod = CY_CAPSENSE_SENSING_METHOD_UNDEFINED;
        Cy_CapSense_SetCmodInDefaultState(curChIndex, context);

        /* Generates sensor frame configuration */
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection)
        {
            Cy_CapSense_GenerateAllSensorConfig(curChIndex,
                    &context->ptrSensorFrameContext[(context->ptrCommonConfig->numSlots *
                                                    (curChIndex + ptrCommonCfg->channelOffset)) *
                                                    context->ptrInternalContext->snsConfigSize], context);
        }
    }

    /* Assign the ISR for scan */
    context->ptrInternalContext->ptrISRCallback = &Cy_CapSense_ScanISR;

    /* Configure non-CapSense HW Resources */
    if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == ptrCommonCfg->scanningMode)
    {
        Cy_CapSense_InitializeDmaResource(context);
    }
    else if (CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection)
    {
        context->ptrInternalContext->csdInactiveSnsHsiom = HSIOM_SEL_GPIO;
        switch (ptrCommonCfg->csdInactiveSnsConnection)
        {
        case CY_CAPSENSE_SNS_CONNECTION_HIGHZ:
            context->ptrInternalContext->csdInactiveSnsDm = CY_GPIO_DM_ANALOG;
            break;
        case CY_CAPSENSE_SNS_CONNECTION_SHIELD:
            context->ptrInternalContext->csdInactiveSnsHsiom = HSIOM_SEL_CSD_SHIELD;
            if (CY_CAPSENSE_SHIELD_ACTIVE == context->ptrCommonConfig->csdShieldMode)
            {
                context->ptrInternalContext->csdInactiveSnsDm = CY_GPIO_DM_ANALOG;
            }
            else
            {
                context->ptrInternalContext->csdInactiveSnsDm = CY_GPIO_DM_STRONG;
            }
            break;
        default:
            /* CY_CAPSENSE_SNS_CONNECTION_GROUND */
            context->ptrInternalContext->csdInactiveSnsDm = CY_GPIO_DM_STRONG_IN_OFF;
            break;
        }
    }
    else
    {
        /* Interrupt Driven mode has nothing specific */
    }

    /* Find maximum raw count for each widget */
    ptrWdCfg = &context->ptrWdConfig[0u];
    for (i = 0u; i < context->ptrCommonConfig->numWd; i++)
    {
        if (CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
        {
            if(CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode)
            {
                ptrWdCfg->ptrWdContext->maxRawCount = ptrWdCfg->ptrWdContext->cicRate;
                ptrWdCfg->ptrWdContext->maxRawCount *= ptrWdCfg->ptrWdContext->cicRate;

                if(0u != context->ptrInternalContext->filterBitFormat)
                {
                    ptrWdCfg->ptrWdContext->maxRawCount <<= 1u;
                }

                ptrWdCfg->ptrWdContext->sigPFC = ptrWdCfg->ptrWdContext->maxRawCount;
            }
            else
            {
                ptrWdCfg->ptrWdContext->maxRawCount = Cy_CapSense_GetMaxRawCIC2(
                                                         ptrWdCfg->numChopCycles,
                                                         ptrWdCfg->ptrWdContext->numSubConversions,
                                                         ptrWdCfg->ptrWdContext->snsClk,
                                                         ptrWdCfg->ptrWdContext->cicRate);
                if((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) &&
                   ((CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdCfg->wdType) ||
                   (CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType)))
                {
                    ptrWdCfg->ptrWdContext->sigPFC = Cy_CapSense_GetMaxRawCIC2(
                                                                 ptrWdCfg->numChopCycles,
                                                                 ptrWdCfg->ptrWdContext->numSubConversions,
                                                                 ptrWdCfg->ptrWdContext->rowSnsClk,
                                                                 ptrWdCfg->ptrWdContext->cicRate);
                }
            }
        }
        else
        {
            ptrWdCfg->ptrWdContext->maxRawCount =  Cy_CapSense_GetMaxRawCIC1(
                                                               ptrWdCfg->numChopCycles,
                                                               ptrWdCfg->ptrWdContext->numSubConversions,
                                                               ptrWdCfg->ptrWdContext->snsClk,
                                                               context->ptrCommonConfig->numEpiCycles);
            if((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) &&
               ((CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdCfg->wdType) ||
               (CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType)))
            {
                ptrWdCfg->ptrWdContext->sigPFC =  Cy_CapSense_GetMaxRawCIC1(
                                                              ptrWdCfg->numChopCycles,
                                                              ptrWdCfg->ptrWdContext->numSubConversions,
                                                              ptrWdCfg->ptrWdContext->rowSnsClk,
                                                              context->ptrCommonConfig->numEpiCycles);

            }
        }

        ptrWdCfg++;
    }

    /* Call user's callback function if it is registered */
    if (NULL != context->ptrInternalContext->ptrEODsInitCallback)
    {
        context->ptrInternalContext->ptrEODsInitCallback(context);
    }

    if (CY_CAPSENSE_STATUS_SUCCESS == capStatus)
    {
        for (curChIndex = 0u; curChIndex < context->ptrCommonConfig->numChannels; curChIndex++)
        {
            /* MSCv3 IP Block Base Register Configuration */
            mscStatus = Cy_MSC_Configure(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                         &context->ptrBaseFrameContext[curChIndex],
                                         CY_MSC_CAPSENSE_KEY,
                                         ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscContext);
            if (CY_MSC_SUCCESS != mscStatus)
            {
                capStatus = CY_CAPSENSE_STATUS_HW_BUSY;
                break;
            }

            /* Clear all pending interrupts of the MSC HW block */
            Cy_MSC_WriteReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase, CY_MSC_REG_OFFSET_INTR,
                            CY_CAPSENSE_MSC_INTR_ALL_MSK);
            (void)Cy_MSC_ReadReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase, CY_MSC_REG_OFFSET_INTR);
        }
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_InterruptHandler
****************************************************************************//**
*
* Implements interrupt service routine for CapSense Middleware.
*
* The CSD / MSC HW block generates an interrupt at end of every sensor scan.
* The CapSense middleware uses this interrupt to implement a
* non-blocking sensor scan method, in which only the first sensor scan is
* initiated by the application program and subsequent sensor scans are
* initiated in the interrupt service routine as soon as the current scan
* is completed. The above stated interrupt service routine is implemented
* as a part of the CapSense middleware.
*
* The CapSense middleware does not initialize or modify the priority
* of interrupts. For the operation of middleware, the application program
* must configure MSC interrupt and assign interrupt vector to
* the Cy_CapSense_InterruptHandler() function. Refer to function
* usage example for details.
*
* Update doc including snippets for CSD & MSC around the MW.
*
* \param base
* The pointer to the base register address of the CSD HW block. A macro for
* the pointer can be found in cycfg_peripherals.h file defined as
* \<Csd_Personality_Name\>_HW. If no name is specified then the default name
* is used csd_\<Block_Number\>_csd_\<Block_Number\>_HW.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \note
* The calls of the Start Sample and End Of Scan callbacks
* (see the \ref group_capsense_callbacks section for details) are the part of the
* Cy_CapSense_InterruptHandler() routine and they lengthen its execution. These
* callbacks will lengthen the CSD ISR execution in case of a direct call of the
* Cy_CapSense_InterruptHandler() function from a CSD ISR.
*
* \funcusage
*
* An example of the ISR initialization:
*
* The CapSense_ISR_cfg variable should be declared by the application
* program according to the examples below:<br>
* For Core CM0+:
* \snippet capsense/snippet/main.c snippet_m0p_capsense_interrupt_source_declaration
*
* The CapSense interrupt handler should be defined by the application program
* according to the example below:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_IntHandler
*
* Then, the application program configures and enables the interrupt
* for each MSC HW block between calls of the Cy_CapSense_Init() and
* Cy_CapSense_Enable() functions:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Initialization
*
* The CY_MSC<X>_HW is the pointer to the base register address of
* the MSC_X HW block. A macro for the pointer is in the cycfg_peripherals.h
* file defined as \<Msc_Personality_Name\>_HW. If no name is specified,
* the default name msc_\<Block_Number\>_msc_\<Block_Number\>_HW is used.
*
*******************************************************************************/
void Cy_CapSense_InterruptHandler(const MSC_Type * base, cy_stc_capsense_context_t * context)
{
    (void)base;
    context->ptrInternalContext->ptrISRCallback((void *)context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ConnectSensor
****************************************************************************//**
*
* Connects a sensor to the specified channel MSC HW block.
*
* The function checks the widget type and performs connection of all sensor
* electrodes (sns for CSD widgets and Tx/Rx for CSX ones) including ganged.
* The sensor and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_ConnectSensor(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    /* Connect all current sensor's electrodes for scan */
    if (CY_CAPSENSE_CSD_GROUP ==
            context->ptrWdConfig[context->ptrActiveScanSns[chIndex].widgetIndex].senseGroup)
    {
        /* Connect all pins of the current sensor */
        Cy_CapSense_CsdConnectSns(chIndex, context);
    }
    if (CY_CAPSENSE_CSX_GROUP ==
            context->ptrWdConfig[context->ptrActiveScanSns[chIndex].widgetIndex].senseGroup)
    {
        /* Connect current sensor's Tx and Rx IOs for scan
        * and set flag to indicate that IOs should be disconnected */
        Cy_CapSense_CsxConnectTx(chIndex, context);
        Cy_CapSense_CsxConnectRx(chIndex, context);
    }
    context->ptrActiveScanSns[chIndex].connectedSnsState = CY_CAPSENSE_SNS_CONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DisconnectSensor
****************************************************************************//**
*
* Disconnects a sensor from the specified channel MSC HW block.
*
* The function checks the widget type and performs disconnection of all sensor
* electrodes (sns for CSD widgets and Tx/Rx for CSX ones) including ganged.
* The sensor and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DisconnectSensor(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    /* Connect all current sensor's electrodes for scan */
    if (CY_CAPSENSE_CSD_RM_SENSING_METHOD ==
            context->ptrWdConfig[context->ptrActiveScanSns[chIndex].widgetIndex].senseMethod)
    {
        /* Connect all pins of the current sensor */
        Cy_CapSense_CsdDisconnectSns(chIndex, context);
    }
    if (CY_CAPSENSE_CSX_RM_SENSING_METHOD ==
            context->ptrWdConfig[context->ptrActiveScanSns[chIndex].widgetIndex].senseMethod)
    {
        /* Disconnect current sensor's Tx and Rx IOs for scan
        * and set flag to indicate that IOs should be disconnected */
        Cy_CapSense_CsxDisconnectTx(chIndex, context);
        Cy_CapSense_CsxDisconnectRx(chIndex, context);
    }
    context->ptrActiveScanSns[chIndex].connectedSnsState = CY_CAPSENSE_SNS_DISCONNECTED;
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsdConnectSns
****************************************************************************//**
*
* Connects a CSD sensor to the specified channel MSC HW block.
*
* The function connects all the sensor pins including ganged.
* The sensor and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsdConnectSns(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_electrode_config_t * eltdPinCfg =
                          context->ptrActiveScanSns[chIndex].ptrEltdConfig;
    const cy_stc_capsense_pin_config_t * ptrActivePin = eltdPinCfg->ptrPin;
    uint32_t i;

    if((chIndex + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
    {
        /* Connect all pins of current sensors */
        for(i = 0u; i < eltdPinCfg->numPins; i++)
        {
            Cy_CapSense_SsConfigPinRegisters(ptrActivePin->pcPtr, (uint32_t)ptrActivePin->pinNumber,
                                                 CY_GPIO_DM_ANALOG, HSIOM_SEL_CSD_SENSE);
            ptrActivePin++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsdDisconnectSns
****************************************************************************//**
*
* Disconnects a CSD sensor from the specified channel MSC HW block.
*
* The function disconnects all the sensor pins including ganged.
* The sensor and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsdDisconnectSns(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_electrode_config_t * eltdPinCfg =
                          context->ptrActiveScanSns[chIndex].ptrEltdConfig;
    const cy_stc_capsense_pin_config_t * ptrActivePin = eltdPinCfg->ptrPin;

    uint32_t i;

    if ((chIndex + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
    {
        /* Disconnect all pins of the current sensor */
        for(i = 0u; i < eltdPinCfg->numPins; i++)
        {
            Cy_CapSense_SsConfigPinRegisters(ptrActivePin->pcPtr, (uint32_t)ptrActivePin->pinNumber,
                                             context->ptrInternalContext->csdInactiveSnsDm,
                                             context->ptrInternalContext->csdInactiveSnsHsiom);
            Cy_GPIO_Clr(ptrActivePin->pcPtr, (uint32_t)ptrActivePin->pinNumber);
            ptrActivePin++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsxConnectTx
****************************************************************************//**
*
* Connects a CSX sensor Tx electrode to the specified channel MSC HW block.
*
* The function connects all the Tx electrode pins including ganged.
* The electrode and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsxConnectTx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t i;
    uint32_t pinIndex;
    uint32_t eltdMptxOrder;
    uint32_t pattern;
    en_hsiom_sel_t hsiomState = HSIOM_SEL_CSD_SENSE;
    const cy_stc_capsense_electrode_config_t * ptrEltd;
    cy_stc_active_scan_sns_t * ptrActive = &context->ptrActiveScanSns[chIndex];
    uint32_t mptxOrd = context->ptrWdConfig[ptrActive->widgetIndex].mptxOrder;
    const cy_stc_capsense_pin_config_t * pinPointer;

    if (CY_CAPSENSE_MPTX_MIN_ORDER > mptxOrd)
    {
        ptrEltd = ptrActive->ptrTxConfig;
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrEltd->chId)
        {
            pinPointer = ptrActive->ptrTxConfig->ptrPin;
            for (pinIndex = ptrEltd->numPins; pinIndex-- > 0u;)
            {
                Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                    CY_GPIO_DM_STRONG_IN_OFF, hsiomState);
                pinPointer++;
            }
        }
    }
    else
    {
        /* Finds the index of the first TX electrode in the mptx group */
        eltdMptxOrder = ptrActive->sensorIndex % mptxOrd;
        i = ptrActive->sensorIndex - eltdMptxOrder;
        i = ptrActive->ptrWdConfig->numCols + (i % ptrActive->ptrWdConfig->numRows);
        ptrEltd = &ptrActive->ptrWdConfig->ptrEltdConfig[i];

        /* Finding the right vector / pattern for mptx operation */
        pattern = ptrActive->ptrWdConfig->ptrMptxTable->vector;
        if (0u != eltdMptxOrder)
        {
            pattern = (pattern >> eltdMptxOrder) | (pattern << (mptxOrd - eltdMptxOrder));
        }

        if (CY_CAPSENSE_MPTX_MAX_ORDER > mptxOrd)
        {
            pattern &= (1u << mptxOrd) - 1u;
        }

        /* Loop through all involved mptx TX electrodes, positive and negative */
        for (i = 0u; i < mptxOrd; i++)
        {
            if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrEltd->chId)
            {
                hsiomState = HSIOM_SEL_CSD_SENSE;
                if (0u == (pattern & 0x01u))
                {
                    hsiomState = HSIOM_SEL_CSD_SHIELD;
                }
                pinPointer = ptrEltd->ptrPin;
                /* Loop through all pads for this electrode (ganged sensor) */
                for (pinIndex = ptrEltd->numPins; pinIndex-- > 0u;)
                {
                    Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                        CY_GPIO_DM_STRONG_IN_OFF, hsiomState);
                    pinPointer++;
                }
            }
            pattern >>= 0x01u;
            ptrEltd++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsxConnectRx
****************************************************************************//**
*
* Connects a CSX sensor Rx electrode to the specified channel MSC HW block.
*
* The function connects all the Rx electrode pins including ganged.
* The electrode and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsxConnectRx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer =
                context->ptrActiveScanSns[chIndex].ptrRxConfig->ptrPin;

    if ((chIndex + context->ptrCommonConfig->channelOffset) == context->ptrActiveScanSns[chIndex].ptrRxConfig->chId)
    {
        for (pinIndex = context->ptrActiveScanSns[chIndex].ptrRxConfig->numPins;
             pinIndex-- > 0u;)
        {
            Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                                             CY_GPIO_DM_ANALOG, HSIOM_SEL_AMUXA);
            context->ptrActiveScanSns[chIndex].connectedSnsState = CY_CAPSENSE_SNS_CONNECTED;
            pinPointer++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsxDisconnectTx
****************************************************************************//**
*
* Disconnects a CSX sensor Tx electrode from the specified channel MSC HW block.
*
* The function disconnects all the Tx electrode pins including ganged.
* The electrode and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsxDisconnectTx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t i;
    uint32_t pinIndex;
    uint32_t eltdMptxOrder;
    const cy_stc_capsense_electrode_config_t * ptrEltd;
    cy_stc_active_scan_sns_t * ptrActive = &context->ptrActiveScanSns[chIndex];
    uint32_t mptxOrd = context->ptrWdConfig[ptrActive->widgetIndex].mptxOrder;
    const cy_stc_capsense_pin_config_t * pinPointer = ptrActive->ptrTxConfig->ptrPin;

    if (CY_CAPSENSE_MPTX_MIN_ORDER > mptxOrd)
    {
        ptrEltd = ptrActive->ptrTxConfig;
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrEltd->chId)
        {
            for (pinIndex = ptrEltd->numPins; pinIndex-- > 0u;)
            {
                Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                    CY_GPIO_DM_STRONG_IN_OFF, HSIOM_SEL_GPIO);
                Cy_GPIO_Clr(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber);
                pinPointer++;
            }
        }
    }
    else
    {
        /* Finds the index of the first TX electrode in the mptx group */
        eltdMptxOrder = ptrActive->sensorIndex % mptxOrd;
        i = ptrActive->sensorIndex - eltdMptxOrder;
        i = ptrActive->ptrWdConfig->numCols + (i % ptrActive->ptrWdConfig->numRows);
        ptrEltd = &ptrActive->ptrWdConfig->ptrEltdConfig[i];

        /* Loop through all involved mptx TX electrodes, positive and negative */
        for (i = 0u; i < mptxOrd; i++)
        {
            if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrEltd->chId)
            {
                pinPointer = ptrEltd->ptrPin;
                /* Loop through all pads for this electrode (ganged sensor) */
                for (pinIndex = ptrEltd->numPins; pinIndex-- > 0u;)
                {
                    Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                        CY_GPIO_DM_STRONG_IN_OFF, HSIOM_SEL_GPIO);
                    Cy_GPIO_Clr(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber);
                    pinPointer++;
                }
            }
            ptrEltd++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_CsxDisconnectRx
****************************************************************************//**
*
* Disconnects a CSX sensor Rx electrode from the specified channel MSC HW block.
*
* The function disconnects all the Rx electrode pins including ganged.
* The electrode and the current slot are specified in the cy_stc_active_scan_sns_t structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_CsxDisconnectRx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t pinIndex;
    const cy_stc_capsense_pin_config_t * pinPointer =
                context->ptrActiveScanSns[chIndex].ptrRxConfig->ptrPin;

    if ((chIndex + context->ptrCommonConfig->channelOffset) == context->ptrActiveScanSns[chIndex].ptrRxConfig->chId)
    {
        for (pinIndex = context->ptrActiveScanSns[chIndex].ptrRxConfig->numPins;
             pinIndex-- > 0u;)
        {
            Cy_CapSense_SsConfigPinRegisters(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber,
                                             CY_GPIO_DM_STRONG_IN_OFF, HSIOM_SEL_GPIO);
            Cy_GPIO_Clr(pinPointer->pcPtr, (uint32_t)pinPointer->pinNumber);
            pinPointer++;
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_ScanISR
****************************************************************************//**
*
* This is an internal ISR function to handle the MSC sensing method operation.
*
* \param capsenseContext
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_ScanISR(void * capsenseContext)
{
    uint32_t intrMask;
    uint32_t i;
    uint32_t tmpRawCount;
    uint32_t maxCount;
    uint32_t slotValue;
    uint32_t * ptrSensorFrame;
    uint32_t scanSlotIndexValid;
    uint32_t sensorFrame[CY_MSC_6_SNS_REGS] = {0u, 0u, 0u, 0u, 0u, 0u};

    cy_stc_capsense_context_t * cxt = (cy_stc_capsense_context_t *)capsenseContext;
    const cy_stc_capsense_common_config_t * ptrCommonCfg = cxt->ptrCommonConfig;
    cy_stc_active_scan_sns_t * ptrActive;
    cy_stc_capsense_internal_context_t * ptrIntrCxt = cxt->ptrInternalContext;

    uint32_t curChIndex = ptrCommonCfg->channelOffset + ptrCommonCfg->numChannels;
    uint32_t slotStcIndex;

    /* Define the interrupt scope */
    if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == ptrCommonCfg->scanningMode)
    {
        intrMask = MSC_INTR_MASK_FRAME_Msk;
    }
    else
    {
        intrMask = MSC_INTR_MASK_SCAN_Msk;
    }

    /* Check which channel have fired the interrupt */
    for (i = 0u; i < ptrCommonCfg->numChannels; i++)
    {
        if (0u != (Cy_MSC_ReadReg(ptrCommonCfg->ptrMscChConfig[i].ptrMscBase, CY_MSC_REG_OFFSET_INTR) & intrMask))
        {
            curChIndex = i;
            break;
        }
    }

    if (curChIndex < ptrCommonCfg->numChannels)
    {
        /* Clear all pending interrupts of the MSC HW block */
        Cy_MSC_WriteReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase, CY_MSC_REG_OFFSET_INTR,
                        CY_CAPSENSE_MSC_INTR_ALL_MSK);
        (void)Cy_MSC_ReadReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase, CY_MSC_REG_OFFSET_INTR);

        if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN != ptrCommonCfg->scanningMode)
        {
            slotStcIndex = (curChIndex + ptrCommonCfg->channelOffset) * ptrCommonCfg->numSlots + ptrIntrCxt->currentSlotIndex;
            if(CY_CAPSENSE_SLOT_SHIELD_ONLY > cxt->ptrScanSlots[slotStcIndex].wdId)
            {
                /* Read raw counts */
                tmpRawCount = Cy_MSC_ReadReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                            CY_MSC_REG_OFFSET_RESULT_FIFO_RD);
                /* Set the active scan pointer to the current channel */
                ptrActive = &(cxt->ptrActiveScanSns[curChIndex]);
                ptrActive->ptrSnsContext->status &= (uint8_t)~CY_CAPSENSE_SNS_OVERFLOW_MASK;
                if (MSC_RESULT_FIFO_RD_OVERFLOW_Msk == (tmpRawCount & MSC_RESULT_FIFO_RD_OVERFLOW_Msk))
                {
                    ptrActive->ptrSnsContext->status |= CY_CAPSENSE_SNS_OVERFLOW_MASK;
                }
                tmpRawCount &= MSC_RESULT_FIFO_RD_RAW_COUNT_Msk;

                if ((CY_CAPSENSE_CSD_GROUP == ptrActive->ptrWdConfig->senseGroup) &&
                    (ptrActive->ptrWdConfig->numCols <= ptrActive->sensorIndex))
                {
                    maxCount = (uint32_t) ptrActive->ptrWdContext->sigPFC;
                }
                else
                {
                    maxCount = (uint32_t) ptrActive->ptrWdContext->maxRawCount;
                }

                if(tmpRawCount > maxCount)
                {
                    tmpRawCount = maxCount;
                }
                if (CY_CAPSENSE_CSX_GROUP == ptrActive->currentSenseGroup)
                {
                    tmpRawCount = maxCount - tmpRawCount;
                }
                ptrActive->ptrSnsContext->raw = (uint16_t)tmpRawCount;
            }
            else
            {
                (void)Cy_MSC_ReadReg(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                                            CY_MSC_REG_OFFSET_RESULT_FIFO_RD);
            }

            if (cxt->ptrScanSlots[slotStcIndex].wdId != CY_CAPSENSE_SLOT_EMPTY)
            {
                /* Disconnect the scanned sensor */
                if ((CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection) &&
                    (CY_CAPSENSE_SLOT_SHIELD_ONLY != cxt->ptrScanSlots[slotStcIndex].wdId) &&
                    (CY_CAPSENSE_SLOT_EMPTY != cxt->ptrScanSlots[slotStcIndex].wdId))
                {
                    Cy_CapSense_DisconnectSensor(curChIndex, cxt);
                }
            }

            /* Check for the last slot, if not - start the next slot scan */
            if (ptrIntrCxt->currentSlotIndex != ptrIntrCxt->endSlotIndex)
            {
                /* Update active sensor structure */
                cxt->ptrActiveScanSns[curChIndex].currentChannelSlotIndex = ptrIntrCxt->currentSlotIndex + 1u;
                slotStcIndex++;

                slotValue = cxt->ptrScanSlots[slotStcIndex].wdId;
                if(CY_CAPSENSE_SLOT_EMPTY == slotValue)
                {
                    /* CY_ID#XXXX */
                    scanSlotIndexValid = slotStcIndex;
                    if(cxt->ptrCommonConfig->numChannels > 1u)
                    {
                        if(slotStcIndex < cxt->ptrCommonConfig->numSlots)
                        {
                            scanSlotIndexValid += cxt->ptrCommonConfig->numSlots;
                        }
                        else
                        {
                            scanSlotIndexValid -= cxt->ptrCommonConfig->numSlots;
                        }
                    }
                    else
                    {
                        scanSlotIndexValid = slotStcIndex;
                    }
                }
                else if (CY_CAPSENSE_SLOT_SHIELD_ONLY <= slotValue)
                {
                    scanSlotIndexValid = (cxt->ptrScanSlots[slotStcIndex].snsId * cxt->ptrCommonConfig->numSlots) +
                                (slotStcIndex % cxt->ptrCommonConfig->numSlots);
                }
                else
                {
                    scanSlotIndexValid = slotStcIndex;
                }

                /* Initialize next slot scan for the current channel */
                Cy_CapSense_InitActivePtr(curChIndex, cxt->ptrScanSlots[scanSlotIndexValid].wdId,
                            cxt->ptrScanSlots[scanSlotIndexValid].snsId, cxt);
                ptrSensorFrame = &cxt->ptrSensorFrameContext[((curChIndex * cxt->ptrCommonConfig->numSlots) +
                        cxt->ptrActiveScanSns[curChIndex].currentChannelSlotIndex) * ptrIntrCxt->snsConfigSize];
                if (CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection)
                {
                    Cy_CapSense_ConfigureAnalogMuxResource(curChIndex, cxt);
                    /* Getting the sensor frame configuration */
                    ptrSensorFrame = &sensorFrame[0u];
                    Cy_CapSense_GenerateSensorConfig(curChIndex, slotStcIndex, ptrSensorFrame, cxt);
                    /* Configure the last slot */
                    if (0u == (ptrIntrCxt->endSlotIndex - cxt->ptrActiveScanSns[curChIndex].currentChannelSlotIndex))
                    {
                        ptrSensorFrame[CY_CAPSENSE_SNS_CTL_INDEX] |= MSC_SNS_CTL_LAST_Msk;
                    }
                    if (CY_MSC_5_SNS_REGS == ptrIntrCxt->snsConfigSize)
                    {
                        ptrSensorFrame++;
                    }
                }

                if (1u == cxt->ptrCommonConfig->numChannels)
                {
                    ptrIntrCxt->currentSlotIndex++;
                }
                else
                {
                    for (i = 1u; i < cxt->ptrCommonConfig->numChannels; i++)
                    {
                        if (cxt->ptrActiveScanSns[0u].currentChannelSlotIndex != cxt->ptrActiveScanSns[i].currentChannelSlotIndex)
                        {
                            break;
                        }
                        ptrIntrCxt->currentSlotIndex++;
                    }
                }

                /* Start the next scan for the current channel */
                Cy_MSC_ConfigureScan(ptrCommonCfg->ptrMscChConfig[curChIndex].ptrMscBase,
                                     ptrIntrCxt->snsConfigSize,
                                     ptrSensorFrame);
            }
            else
            {
                Cy_CapSense_ClrBusyFlags(curChIndex, cxt);
            }
        }
        else
        {
            /* The WHOLE FRAME scan is finished for the current channel */
            Cy_CapSense_ClrBusyFlags(curChIndex, cxt);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_SwitchSensingMethod
****************************************************************************//**
*
* This function changes CapSense pin configuration based on the provided
* sensing method if Analog Mux mode is enabled.
*
* \param mode
* Specifies the scan mode:
* - CY_CAPSENSE_CSD_SENSING_METHOD
* - CY_CAPSENSE_CSX_RM_SENSING_METHOD
* - CY_CAPSENSE_CSD_RM_SENSING_METHOD
* - CY_CAPSENSE_SENSING_METHOD_UNDEFINED
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return status
* Returns the operation status:
* - CY_CAPSENSE_STATUS_SUCCESS - Indicates the successful mode switching.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_SwitchSensingMethod(
                uint8_t mode,
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_SUCCESS;

    if (context->ptrActiveScanSns[chIndex].currentSenseMethod != mode)
    {
        context->ptrActiveScanSns[chIndex].currentSenseMethod = mode;

        if (CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            Cy_CapSense_SetCmodInAmuxModeState(chIndex, context);

            /* Enable the specified mode */
            if (CY_CAPSENSE_CSX_RM_SENSING_METHOD == mode)
            {
                Cy_CapSense_SetIOsInDesiredState(CY_GPIO_DM_STRONG_IN_OFF, 0u, HSIOM_SEL_GPIO, chIndex, context);
            }
            else
            {
                Cy_CapSense_SetIOsInDesiredState(context->ptrInternalContext->csdInactiveSnsDm, 0u,
                context->ptrInternalContext->csdInactiveSnsHsiom, chIndex, context);
            }
        }
    }
    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetCmodInDefaultState
****************************************************************************//**
*
* Sets all available MSC Cmod pins connected via AMuxBus into a default state .
*
* Sets all external capacitors connected via AMuxBus into the default state:
* - HSIOM   - Disconnected, the GPIO mode.
* - DM      - High-Z Analog.
*
* Do not call this function directly from the application program.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetCmodInDefaultState(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    Cy_CapSense_SsConfigPinRegisters(
            context->ptrCommonConfig->ptrMscChConfig[chIndex].portCmod1,
            (uint32_t)context->ptrCommonConfig->ptrMscChConfig[chIndex].pinCmod1,
            CY_GPIO_DM_ANALOG, HSIOM_SEL_GPIO);
    Cy_CapSense_SsConfigPinRegisters(
            context->ptrCommonConfig->ptrMscChConfig[chIndex].portCmod2,
            (uint32_t)context->ptrCommonConfig->ptrMscChConfig[chIndex].pinCmod2,
            CY_GPIO_DM_ANALOG, HSIOM_SEL_GPIO);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetCmodInAmuxModeState
****************************************************************************//**
*
* Sets all available MSC Cmod pins connected via AMuxBus into a default state.
*
* Sets all external capacitors connected via AMuxBus into the default state:
* - HSIOM   - Disconnected, the GPIO mode.
* - DM      - High-Z Analog.
*
* Do not call this function directly from the application program.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetCmodInAmuxModeState(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t * ptrSwSelCfg = &context->ptrBaseFrameContext[chIndex].swSelGpio;
    uint32_t wdIndex = context->ptrActiveScanSns[chIndex].widgetIndex;

    *ptrSwSelCfg &= ~CY_CAPSENSE_CMOD_AMUX_MSK;
    if (CY_CAPSENSE_CSD_GROUP == context->ptrWdConfig[wdIndex].senseGroup)
    {
        *ptrSwSelCfg |= CY_CAPSENSE_FW_CMOD_AMUX_CSD_REG_SW_SEL_GPIO_VALUE;
    }
    else
    {
        *ptrSwSelCfg |= CY_CAPSENSE_FW_CMOD_AMUX_CSX_REG_SW_SEL_GPIO_VALUE;
    }

    Cy_MSC_WriteReg(context->ptrCommonConfig->ptrMscChConfig[chIndex].ptrMscBase,
                    CY_MSC_REG_OFFSET_SW_SEL_GPIO,
                    *ptrSwSelCfg);

    Cy_CapSense_SsConfigPinRegisters(
    context->ptrCommonConfig->ptrMscChConfig[chIndex].portCmod1,
    (uint32_t)context->ptrCommonConfig->ptrMscChConfig[chIndex].pinCmod1,
    CY_GPIO_DM_ANALOG, HSIOM_SEL_AMUXA);

    Cy_CapSense_SsConfigPinRegisters(
    context->ptrCommonConfig->ptrMscChConfig[chIndex].portCmod2,
    (uint32_t)context->ptrCommonConfig->ptrMscChConfig[chIndex].pinCmod2,
    CY_GPIO_DM_ANALOG, HSIOM_SEL_AMUXA);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetIOsInDesiredState
****************************************************************************//**
*
* Sets all CapSense pins into a desired state.
*
* Sets all the CSD/CSX IOs into a desired state.
* Default state:
* - HSIOM   - Disconnected, the GPIO mode.
* - DM      - Strong drive.
* - State   - Zero.
*
* Do not call this function directly from the application program.
*
* \param desiredDriveMode
* Specifies the desired pin control port (PC) configuration.
*
* \param desiredPinOutput
* Specifies the desired pin output data register (DR) state.
*
* \param desiredHsiom
* Specifies the desired pin HSIOM state.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetIOsInDesiredState(
                uint32_t desiredDriveMode,
                uint32_t desiredPinOutput,
                en_hsiom_sel_t desiredHsiom,
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context)
{
    uint32_t loopIndex;
    const cy_stc_capsense_pin_config_t * ptrPinCfg = context->ptrPinConfig;

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < context->ptrCommonConfig->numPin; loopIndex++)
    {
        if (chIndex == ptrPinCfg->chId)
        {
            Cy_CapSense_SsConfigPinRegisters(ptrPinCfg->pcPtr,
                (uint32_t)ptrPinCfg->pinNumber, desiredDriveMode, desiredHsiom);

            if (0u != desiredPinOutput)
            {
                Cy_GPIO_Set(ptrPinCfg->pcPtr, (uint32_t)ptrPinCfg->pinNumber);
            }
            else
            {
                Cy_GPIO_Clr(ptrPinCfg->pcPtr, (uint32_t)ptrPinCfg->pinNumber);
            }
        }
        /* Get next electrode */
        ptrPinCfg++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetIOsInDefaultState
****************************************************************************//**
*
* Sets all CapSense pins into a default state.
*
* Sets all the CSD/CSX IOs into a desired state.
* Default state:
* - HSIOM   - Disconnected, the GPIO mode.
* - DM      - Strong drive.
* - State   - Zero.
*
* Do not call this function directly from the application program.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetIOsInDefaultState(
                const cy_stc_capsense_context_t * context)
{
    uint32_t loopIndex;
    const cy_stc_capsense_pin_config_t * ptrPinCfg = context->ptrPinConfig;

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < context->ptrCommonConfig->numPin; loopIndex++)
    {
        Cy_CapSense_SsConfigPinRegisters(ptrPinCfg->pcPtr,
                (uint32_t)ptrPinCfg->pinNumber, CY_GPIO_DM_ANALOG, HSIOM_SEL_GPIO);
        Cy_GPIO_Clr(ptrPinCfg->pcPtr, (uint32_t)ptrPinCfg->pinNumber);
        ptrPinCfg++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetShieldPinsInDefaultState
****************************************************************************//**
*
* Sets all shield pins into a default state.
*
* Do not call this function directly from the application program.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetShieldPinsInDefaultState(
                const cy_stc_capsense_context_t * context)
{
    uint32_t loopIndex;
    const cy_stc_capsense_pin_config_t * ptrPinCfg = context->ptrShieldPinConfig;

    /* Loop through all shield electrodes */
    for (loopIndex = 0u; loopIndex < context->ptrCommonConfig->csdShieldNumPin; loopIndex++)
    {
        Cy_CapSense_SsConfigPinRegisters(ptrPinCfg->pcPtr,
                  (uint32_t)ptrPinCfg->pinNumber, CY_GPIO_DM_ANALOG, HSIOM_SEL_GPIO);

        Cy_GPIO_Clr(ptrPinCfg->pcPtr, (uint32_t)ptrPinCfg->pinNumber);
        /* Get next electrode */
        ptrPinCfg++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitActivePtr
****************************************************************************//**
*
* Initializes active scan sensor structure with all available
* pointers for further faster access to widget/sensor parameters.
*
* \param chIndex
* Specifies the channel ID number.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param sensorId
* Specifies the ID number of the sensor within the widget. A macro for the
* sensor ID within a specified widget can be found in the cycfg_capsense.h
* file defined as CY_CAPSENSE_<WIDGET_NAME>_SNS<SENSOR_NUMBER>_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitActivePtr(
                uint32_t chIndex,
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = &(context->ptrActiveScanSns[chIndex]);
    if(CY_CAPSENSE_SLOT_SHIELD_ONLY > widgetId)
    {
        Cy_CapSense_InitActivePtrWd(chIndex, widgetId, context);
        Cy_CapSense_InitActivePtrSns(chIndex, sensorId, context);
    }
    else
    {
        ptrActive->widgetIndex = (uint8_t)widgetId;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitActivePtrSns
****************************************************************************//**
*
* Initializes active scan sensor structure with pointers to sensor
* for further faster access to widget/sensor parameters.
*
* This function supposes that the Cy_CapSense_InitActivePtrWd() function
* is called before.
*
* \param chIndex
* The desired channel index.
*
* \param sensorId
* Specifies the ID number of the sensor within the widget. A macro for the
* sensor ID within a specified widget can be found in the cycfg_capsense.h
* file defined as CY_CAPSENSE_<WIDGET_NAME>_SNS<SENSOR_NUMBER>_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void  Cy_CapSense_InitActivePtrSns(
                uint32_t chIndex,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = &(context->ptrActiveScanSns[chIndex]);
    uint32_t numberRows;
    uint32_t numberCols;
    uint32_t widgetSenseGroup = ptrActive->currentSenseGroup;

    ptrActive->mfsChannelIndex = 0u;
    ptrActive->sensorIndex = (uint16_t)sensorId;
    ptrActive->ptrSnsContext = &ptrActive->ptrWdConfig->ptrSnsContext[sensorId];
    ptrActive->connectedSnsState = CY_CAPSENSE_SNS_DISCONNECTED;

    switch(widgetSenseGroup)
    {
        case (uint8_t)CY_CAPSENSE_CSD_GROUP:
            ptrActive->ptrEltdConfig = &ptrActive->ptrWdConfig->ptrEltdConfig[sensorId];
            break;
        case (uint8_t)CY_CAPSENSE_CSX_GROUP:
            numberRows = ptrActive->ptrWdConfig->numRows;
            numberCols = ptrActive->ptrWdConfig->numCols;
            ptrActive->rxIndex = (uint8_t)(sensorId / numberRows);
            ptrActive->txIndex = (uint8_t)(numberCols + (sensorId % numberRows));
            ptrActive->ptrRxConfig = &ptrActive->ptrWdConfig->ptrEltdConfig[ptrActive->rxIndex];
            ptrActive->ptrTxConfig = &ptrActive->ptrWdConfig->ptrEltdConfig[ptrActive->txIndex];
            break;
        default:
            CY_ASSERT(0 != 0);
            break;
    }
}

/*******************************************************************************
* Function Name: Cy_CapSense_InitActivePtrWd
****************************************************************************//**
*
* Initializes active scan sensor structure with pointers to current widget
* for further faster access to widget/sensor parameters.
*
* This function does not update pointers to current sensor and the
* Cy_CapSense_InitActivePtrSns() function should be called after current one.
*
* \param chIndex
* The desired channel index.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitActivePtrWd(
                uint32_t chIndex,
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_stc_active_scan_sns_t * ptrActive = &(context->ptrActiveScanSns[chIndex]);

    ptrActive->widgetIndex = (uint8_t)widgetId;
    ptrActive->ptrWdConfig = &context->ptrWdConfig[widgetId];
    ptrActive->ptrWdContext = ptrActive->ptrWdConfig->ptrWdContext;
    ptrActive->currentSenseGroup = ptrActive->ptrWdConfig->senseGroup;
}


/*******************************************************************************
* Function Name: Cy_CapSense_SsConfigPinRegisters
****************************************************************************//**
*
* Configures drive mode and HSIOM state of a desired pin. The action
* is performed inside the critical section.
*
* \param base
* The pointer to the pin's port register base address.
*
* \param pinNum
* Position of the pin bit-field within the port register.
*
* \param dm
* Specifies drive mode of the pin.
*
* \param hsiom
* Specifies HSIOM state of the pin.
*
*******************************************************************************/
void Cy_CapSense_SsConfigPinRegisters(
                GPIO_PRT_Type * base,
                uint32_t pinNum,
                uint32_t dm,
                en_hsiom_sel_t hsiom)
{
    uint32_t interruptState = Cy_SysLib_EnterCriticalSection();
    if (HSIOM_SEL_GPIO == hsiom)
    {
        Cy_GPIO_SetHSIOM(base, pinNum, hsiom);
        Cy_GPIO_SetDrivemode(base, pinNum, dm);
    }
    else
    {
        Cy_GPIO_SetDrivemode(base, pinNum, dm);
        Cy_GPIO_SetHSIOM(base, pinNum, hsiom);
    }
    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_CapSense_EnableShieldElectrodes
****************************************************************************//**
*
* This internal function initializes Shield Electrodes.
*
* The function sets the bit in the HSIOM register which enables the shield electrode
* functionality on the pin. The port and pin configurations are stored in
* the cy_capsense_shieldIoList structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_EnableShieldElectrodes(uint32_t chIndex, cy_stc_capsense_context_t * context)
{
    uint32_t shieldIndex;
    const cy_stc_capsense_pin_config_t * ptrShieldPin = context->ptrShieldPinConfig;
    uint32_t * ptrSwSelCfg = &context->ptrBaseFrameContext[chIndex].swSelGpio;

    *ptrSwSelCfg &= ~MSC_SW_SEL_GPIO_SW_CSD_SHIELD_Msk;
    if (CY_CAPSENSE_SHIELD_ACTIVE == context->ptrCommonConfig->csdShieldMode)
    {
        *ptrSwSelCfg |= CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_CSD_SHIELD_VALUE;
    }
    else
    {
        *ptrSwSelCfg |= CY_CAPSENSE_FW_SHIELD_PASSIVE_AMUX_REG_SW_CSD_SHIELD_VALUE;
    }

    Cy_MSC_WriteReg(context->ptrCommonConfig->ptrMscChConfig[chIndex].ptrMscBase,
                    CY_MSC_REG_OFFSET_SW_SEL_GPIO,
                    *ptrSwSelCfg);

    for (shieldIndex = 0u; shieldIndex < context->ptrCommonConfig->csdShieldNumPin; shieldIndex++)
    {
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrShieldPin->chId)
        {
            if (CY_CAPSENSE_SHIELD_ACTIVE == context->ptrCommonConfig->csdShieldMode)
              {
                  Cy_CapSense_SsConfigPinRegisters(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber,
                                                  CY_GPIO_DM_ANALOG, HSIOM_SEL_CSD_SHIELD);
              }
              else
              {
                  Cy_CapSense_SsConfigPinRegisters(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber,
                                                  CY_GPIO_DM_STRONG, HSIOM_SEL_CSD_SHIELD);
              }
        }
        ptrShieldPin++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DisableShieldElectrodes
****************************************************************************//**
*
* This internal function disables Shield Electrodes.
*
* The function resets the bit in the HSIOM register which disables the shield
* electrode functionality on the pin. The port and pin configurations are
* stored in the cy_capsense_shieldIoList structure.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DisableShieldElectrodes(uint32_t chIndex, cy_stc_capsense_context_t * context)
{
    uint32_t shieldIndex;
    const cy_stc_capsense_pin_config_t * ptrShieldPin = context->ptrShieldPinConfig;

    context->ptrBaseFrameContext[chIndex].swSelGpio &= ~MSC_SW_SEL_GPIO_SW_CSD_SHIELD_Msk;

    Cy_MSC_WriteReg(context->ptrCommonConfig->ptrMscChConfig[chIndex].ptrMscBase,
                    CY_MSC_REG_OFFSET_SW_SEL_GPIO,
                    context->ptrBaseFrameContext[chIndex].swSelGpio);

    for (shieldIndex = 0u; shieldIndex < context->ptrCommonConfig->csdShieldNumPin; shieldIndex++)
    {
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrShieldPin->chId)
        {
            Cy_CapSense_SsConfigPinRegisters(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber,
                                            CY_GPIO_DM_STRONG_IN_OFF, HSIOM_SEL_GPIO);
            Cy_GPIO_Clr(ptrShieldPin->pcPtr, (uint32_t)ptrShieldPin->pinNumber);
        }
        ptrShieldPin++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_IsBusy
****************************************************************************//**
*
* This function returns a status of the CapSense middleware whether a scan is
* currently in progress or not.
*
* If the middleware is busy, a new scan or setup widgets should not be initiated.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the middleware as a sum of the next masks :
* - CY_CAPSENSE_NOT_BUSY    - No scan is in progress and a next scan frame
*                             can be initiated.
* - CY_CAPSENSE_BUSY[x]     - The set [x] bit of the result means that
*                             the previously initiated scan for the [x] channel is
*                             in progress. The next scan frame cannot be started.
* - CY_CAPSENSE_BUSY_CALIBRATION - The auto-calibration is in progress.
*                                  The next scan frame cannot be started.
* - CY_CAPSENSE_BUSY_VERIFY_CALIBRATION - The auto-calibration verification is
*                                         in progress.
*                                         The next scan frame cannot be started.
*
*******************************************************************************/
uint32_t Cy_CapSense_IsBusy(const cy_stc_capsense_context_t * context)
{
    return (context->ptrCommonContext->status);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetBusyFlags
****************************************************************************//**
*
* Sets BUSY flags of the cy_capsense_context.status register specified
* by the flags parameter.
*
* This is an internal function. Do not call this function directly from
* the application program.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_SetBusyFlags(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    context->ptrCommonContext->status |= (((uint32_t)CY_CAPSENSE_BUSY_CH_MASK) << chIndex);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ClrBusyFlags
****************************************************************************//**
*
* Clears BUSY flags of the cy_capsense_context.status register specified
* by the flags parameter.
*
* This is an internal function. Do not call this function directly from
* the application program.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_ClrBusyFlags(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    cy_stc_capsense_internal_context_t * ptrIntrCxt;
    const cy_stc_capsense_common_config_t * ptrCommonCfg = context->ptrCommonConfig;
    uint32_t snsPos;
    uint32_t * framePtr;
    uint32_t snsCtlRegIndex;

    context->ptrCommonContext->status &= (uint32_t)(~(((uint32_t)CY_CAPSENSE_BUSY_CH_MASK) << chIndex));
    if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == ptrCommonCfg->sensorConnection)
    {
        ptrIntrCxt = context->ptrInternalContext;
        snsPos = ptrIntrCxt->endSlotIndex + (context->ptrCommonConfig->numSlots * chIndex);
        framePtr = &context->ptrSensorFrameContext[snsPos * context->ptrInternalContext->snsConfigSize];
        snsCtlRegIndex = (CY_MSC_6_SNS_REGS == context->ptrInternalContext->snsConfigSize) ? (CY_CAPSENSE_SNS_CTL_INDEX) :
                                                                                             (CY_CAPSENSE_SNS_CTL_INDEX - 1u);
        framePtr[snsCtlRegIndex] &= ~MSC_SNS_CTL_LAST_Msk;
    }

    /* Clear busy flag for the current channel */
    if (0u == context->ptrCommonContext->status)
    {
        /* Mark completion of scan cycle */
        context->ptrCommonContext->scanCounter++;
        if (NULL != context->ptrInternalContext->ptrEOSCallback)
        {
            context->ptrInternalContext->ptrEOSCallback((cy_stc_active_scan_sns_t *)&context->ptrActiveScanSns[0u]);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_ConfigureAnalogMuxResource
****************************************************************************//**
*
* Configures the Analog Mux connections for a specified channel.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of operation.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ConfigureAnalogMuxResource(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    uint32_t wdIndex = context->ptrActiveScanSns[chIndex].widgetIndex;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[wdIndex];

    capStatus = Cy_CapSense_SwitchSensingMethod(ptrWdCfg->senseMethod, chIndex, context);

    if (CY_CAPSENSE_SHIELD_DISABLED != context->ptrCommonConfig->csdShieldMode)
    {
        if (CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup)
        {
            Cy_CapSense_EnableShieldElectrodes(chIndex, context);
        }
        else
        {
            Cy_CapSense_DisableShieldElectrodes(chIndex, context);
        }
    }

    if((wdIndex != CY_CAPSENSE_SLOT_EMPTY) &&
       (wdIndex != CY_CAPSENSE_SLOT_SHIELD_ONLY))
    {
        /* Connect the widget first sensor electrodes */
        Cy_CapSense_ConnectSensor(chIndex, context);
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeDmaResource
****************************************************************************//**
*
* Initializes the DMAC channels in CS-DMA scanning mode.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of operation.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_InitializeDmaResource(
                cy_stc_capsense_context_t * context)
{
    cy_stc_msc_channel_config_t * ptrCurMscChCfg = context->ptrCommonConfig->ptrMscChConfig;
    uint32_t curChIndex;
    uint8_t wrIdx;
    uint8_t rdIdx;
    uint8_t wrChIdx;
    uint8_t rdChIdx;
    DMAC_Type * dmacBase;
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    if (NULL != context->ptrCommonConfig->ptrDmacBase)
    {
        dmacBase = context->ptrCommonConfig->ptrDmacBase;
        for (curChIndex = 0u; curChIndex < context->ptrCommonConfig->numChannels; curChIndex++)
        {
            ptrCurMscChCfg = &(context->ptrCommonConfig->ptrMscChConfig[curChIndex]);
            wrIdx = ptrCurMscChCfg->dmaWrChIndex;
            wrChIdx = ptrCurMscChCfg->dmaChainWrChIndex;
            rdIdx = ptrCurMscChCfg->dmaRdChIndex;
            rdChIdx = ptrCurMscChCfg->dmaChainRdChIndex;

            /* Perform the initial configuration of the WR channel of DMAC */
            Cy_DMAC_Descriptor_DeInit(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Descriptor_SetSrcIncrement(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Descriptor_SetDstIncrement(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Descriptor_SetDataSize(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_WORD);
            Cy_DMAC_Descriptor_SetSrcTransferSize(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetDstTransferSize(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetRetrigger(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_RETRIG_IM);
            Cy_DMAC_Descriptor_SetFlipping(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetTriggerType(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_SINGLE_DESCR);
            Cy_DMAC_Descriptor_SetCpltState(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetState(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Channel_SetPriority(dmacBase, wrIdx, 3u);
            Cy_DMAC_Descriptor_SetPreemptable(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Channel_SetCurrentDescriptor(dmacBase, wrIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Channel_Enable(dmacBase, wrIdx);

            /* Perform the initial configuration of the Chain WR channel of DMAC */
            Cy_DMAC_Descriptor_DeInit(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Descriptor_SetSrcIncrement(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Descriptor_SetDstIncrement(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetDataSize(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_WORD);
            Cy_DMAC_Descriptor_SetSrcTransferSize(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetDstTransferSize(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetRetrigger(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_RETRIG_IM);
            Cy_DMAC_Descriptor_SetFlipping(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetTriggerType(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_SINGLE_ELEMENT);
            Cy_DMAC_Descriptor_SetCpltState(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetState(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Channel_SetPriority(dmacBase, wrChIdx, 3u);
            Cy_DMAC_Descriptor_SetPreemptable(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Channel_SetCurrentDescriptor(dmacBase, wrChIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Channel_Enable(dmacBase, wrChIdx);

            /* Perform the initial configuration of the RD channel of DMAC */
            Cy_DMAC_Descriptor_DeInit(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Descriptor_SetSrcIncrement(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetDstIncrement(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetDataSize(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_HALFWORD);
            Cy_DMAC_Descriptor_SetSrcTransferSize(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetDstTransferSize(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_DATA);
            Cy_DMAC_Descriptor_SetRetrigger(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_RETRIG_IM);
            Cy_DMAC_Descriptor_SetFlipping(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetTriggerType(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_SINGLE_DESCR);
            Cy_DMAC_Descriptor_SetCpltState(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetState(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Channel_SetPriority(dmacBase, rdIdx, 3u);
            Cy_DMAC_Descriptor_SetPreemptable(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Channel_SetCurrentDescriptor(dmacBase, rdIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Channel_Enable(dmacBase, rdIdx);

            /* Perform the initial configuration of the Chain Read channel of DMAC */
            Cy_DMAC_Descriptor_DeInit(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Descriptor_SetSrcIncrement(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Descriptor_SetDstIncrement(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetDataSize(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_WORD);
            Cy_DMAC_Descriptor_SetSrcTransferSize(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetDstTransferSize(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_TRANSFER_SIZE_WORD);
            Cy_DMAC_Descriptor_SetRetrigger(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_RETRIG_IM);
            Cy_DMAC_Descriptor_SetFlipping(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetTriggerType(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, CY_DMAC_SINGLE_ELEMENT);
            Cy_DMAC_Descriptor_SetCpltState(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Descriptor_SetState(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, true);
            Cy_DMAC_Channel_SetPriority(dmacBase, rdChIdx, 3u);
            Cy_DMAC_Descriptor_SetPreemptable(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING, false);
            Cy_DMAC_Channel_SetCurrentDescriptor(dmacBase, rdChIdx, CY_DMAC_DESCRIPTOR_PING);
            Cy_DMAC_Enable(dmacBase);
        }

        capStatus = CY_CAPSENSE_STATUS_SUCCESS;

    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ConfigureDmaResource
****************************************************************************//**
*
* Configures DMAC channels prior to the scan.
*
* \param mscChIndex
* Index of the MSC channel to configure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of operation.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ConfigureDmaResource(
                uint32_t mscChIndex,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_SUCCESS;
    uint32_t snsConfigSize = context->ptrInternalContext->snsConfigSize;
    cy_stc_msc_channel_config_t * ptrCurMscChCfg = &(context->ptrCommonConfig->ptrMscChConfig[mscChIndex]);
    DMAC_Type * dmacBase = context->ptrCommonConfig->ptrDmacBase;
    MSC_Type * mscBase = ptrCurMscChCfg->ptrMscBase;
    uint8_t wrChIndex = ptrCurMscChCfg->dmaWrChIndex;
    uint8_t chainWrChIndex = ptrCurMscChCfg->dmaChainWrChIndex;
    uint8_t rdChIndex = ptrCurMscChCfg->dmaRdChIndex;
    uint8_t chainRdChIndex = ptrCurMscChCfg->dmaChainRdChIndex;
    uint32_t * ptrConfigReg;
    uint32_t currentSlotIndex = context->ptrInternalContext->currentSlotIndex;
    uint32_t endSlotIndex = context->ptrInternalContext->endSlotIndex;
    uint32_t currSlotPos = (mscChIndex + context->ptrCommonConfig->channelOffset) * context->ptrCommonConfig->numSlots;

    /* Check the number of registers in sensor configuration */
    if (CY_MSC_5_SNS_REGS == snsConfigSize)
    {
        ptrConfigReg = (uint32_t *)&(mscBase->SNS_SW_SEL_CSW_MASK1);
    }
    else
    {
        ptrConfigReg = (uint32_t *)&(mscBase->SNS_SW_SEL_CSW_MASK2);
    }

    /* Disable DMAC channels before configuring */
    Cy_DMAC_Channel_Disable(dmacBase, wrChIndex);
    Cy_DMAC_Channel_Disable(dmacBase, chainWrChIndex);
    Cy_DMAC_Channel_Disable(dmacBase, rdChIndex);
    Cy_DMAC_Channel_Disable(dmacBase, chainRdChIndex);

    /* Configure the destination and source addresses for the Write channel of DMAC */
    Cy_DMAC_Descriptor_SetDstAddress(dmacBase, wrChIndex, CY_DMAC_DESCRIPTOR_PING, ptrConfigReg);
    Cy_DMAC_Descriptor_SetDataCount(dmacBase, wrChIndex, CY_DMAC_DESCRIPTOR_PING,  context->ptrInternalContext->snsConfigSize);

    /* Configure the destination and source addresses for the Chain Write channel of DMAC */
    Cy_DMAC_Descriptor_SetSrcAddress(dmacBase, chainWrChIndex, CY_DMAC_DESCRIPTOR_PING, &context->ptrCommonConfig->ptrDmaWrChSnsCfgAddr[currSlotPos + currentSlotIndex]);
    Cy_DMAC_Descriptor_SetDstAddress(dmacBase, chainWrChIndex, CY_DMAC_DESCRIPTOR_PING, (void *) &(DMAC_DESCR_PING_SRC(dmacBase, wrChIndex)));
    Cy_DMAC_Descriptor_SetDataCount(dmacBase, chainWrChIndex, CY_DMAC_DESCRIPTOR_PING, (endSlotIndex - currentSlotIndex + 1u));

    /* Configure the destination and source addresses for the Read channel of DMAC */
    Cy_DMAC_Descriptor_SetSrcAddress(dmacBase, rdChIndex, CY_DMAC_DESCRIPTOR_PING, (uint32_t *)&(mscBase->RESULT_FIFO_RD));
    Cy_DMAC_Descriptor_SetDataCount(dmacBase, rdChIndex, CY_DMAC_DESCRIPTOR_PING, 1u);

    /* Configure the destination and source addresses for the Chain Read channel of DMAC */
    Cy_DMAC_Descriptor_SetSrcAddress(dmacBase, chainRdChIndex, CY_DMAC_DESCRIPTOR_PING, &context->ptrCommonConfig->ptrDmaRdChSnsCfgAddr[currSlotPos + currentSlotIndex]);
    Cy_DMAC_Descriptor_SetDstAddress(dmacBase, chainRdChIndex, CY_DMAC_DESCRIPTOR_PING, (void *) &(DMAC_DESCR_PING_DST(dmacBase, rdChIndex)));
    Cy_DMAC_Descriptor_SetDataCount(dmacBase, chainRdChIndex, CY_DMAC_DESCRIPTOR_PING, (endSlotIndex - currentSlotIndex + 1u));

    /* Enable DMAC channels to perform scanning */
    Cy_DMAC_Channel_Enable(dmacBase, wrChIndex);
    Cy_DMAC_Channel_Enable(dmacBase, chainWrChIndex);
    Cy_DMAC_Channel_Enable(dmacBase, rdChIndex);
    Cy_DMAC_Channel_Enable(dmacBase, chainRdChIndex);

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetModClkDivider
****************************************************************************//**
*
* Sets the modulator clock and then starts it.
*
* Do not call this function directly from the application program.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_SetModClkDivider(
                const cy_stc_capsense_context_t * context)
{
    uint32_t dividerIndex = (uint32_t)context->ptrCommonConfig->periDividerIndex;
    cy_en_divider_types_t dividerType = (cy_en_divider_types_t)context->ptrCommonConfig->periDividerType;
    uint32_t dividerValue = context->ptrCommonContext->modClk;

    if (0u == dividerValue)
    {
        dividerValue = 1u;
    }
    dividerValue--;
    (void)Cy_SysClk_PeriphDisableDivider(dividerType, dividerIndex);
    if ((CY_SYSCLK_DIV_8_BIT == dividerType) || (CY_SYSCLK_DIV_16_BIT == dividerType))
    {
        (void)Cy_SysClk_PeriphSetDivider(dividerType, dividerIndex, dividerValue);
    }
    else
    {
        (void)Cy_SysClk_PeriphSetFracDivider(dividerType, dividerIndex, dividerValue, 0u);
    }
    (void)Cy_SysClk_PeriphEnableDivider(dividerType, dividerIndex);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalibrateSlot
****************************************************************************//**
*
* Calibrates the CapDACs for selected slot to the specified target.
*
* \param scanSlotId
* The slot ID scan will be done for.
*
* \param csdTarget
* Specifies the calibration target in percentages of the maximum raw count for CSD sensing method.
*
* \param csxTarget
* Specifies the calibration target in percentages of the maximum raw count for CSX sensing method.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_CALIBRATION_FAIL - The calibration failed if software
*                                         watchdog timeout occurred
*                                         during any calibration scan,
*                                         the scan was not completed, or
*                                         resulted raw counts
*                                         are outside the limits.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_CalibrateSlot(
                uint32_t scanSlotId,
                uint32_t csdTarget,
                uint32_t csxTarget,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t calibStatus = CY_CAPSENSE_STATUS_SUCCESS;
    cy_stc_capsense_widget_config_t const * ptrWdCfg[CY_CAPSENSE_MAX_CH_NUM];
    const cy_stc_capsense_common_config_t * ptrCommCfg;
    uint32_t chIndex;
    uint32_t wdIndex;
    uint32_t snsIndex;
    uint32_t calibrationBitEn = 0u;
    uint32_t target;
    uint32_t maxRawTmp;
    uint32_t tempRefCdac[CY_CAPSENSE_MAX_CH_NUM];
    uint16_t * ptrRaw[CY_CAPSENSE_MAX_CH_NUM];
    uint32_t rawTarget[CY_CAPSENSE_MAX_CH_NUM];
    uint8_t calMask = (uint8_t)CY_CAPSENSE_CAL_MIDDLE_VALUE;

    ptrCommCfg = context->ptrCommonConfig;

    /* Populate all sensors in widget */
    for(chIndex = 0u; chIndex < ptrCommCfg->numChannels; chIndex++)
    {
        wdIndex = context->ptrScanSlots[scanSlotId + (ptrCommCfg->numSlots * chIndex)].wdId;
        if(CY_CAPSENSE_SLOT_SHIELD_ONLY > wdIndex)
        {
            snsIndex = context->ptrScanSlots[scanSlotId + (ptrCommCfg->numSlots * chIndex)].snsId;
            ptrWdCfg[chIndex] = &context->ptrWdConfig[wdIndex];
            /* Set widget pointer to NULL if auto-calibration disabled */
            if(!(((CY_CAPSENSE_DISABLE == ptrCommCfg->csdCdacAutocalEn) &&
                (CY_CAPSENSE_CSD_GROUP == ptrWdCfg[chIndex]->senseGroup)) ||
                ((CY_CAPSENSE_DISABLE == ptrCommCfg->csxCdacAutocalEn) &&
                (CY_CAPSENSE_CSX_GROUP == ptrWdCfg[chIndex]->senseGroup))))

            {
                calibrationBitEn |= 0x01 << chIndex;
                tempRefCdac[chIndex] = CY_CAPSENSE_CAL_MIDDLE_VALUE;
                Cy_CapSense_SetCdacs(tempRefCdac[chIndex], scanSlotId, chIndex, context);

                /* Get raw count target for proper sensing method */
                target = csdTarget;
                if(CY_CAPSENSE_CSX_GROUP == ptrWdCfg[chIndex]->senseGroup)
                {
                    if(CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode)
                    {
                        target = 100u - csxTarget;
                    }
                    else
                    {
                        target = csxTarget;
                    }
                }

                if (CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
                {
                    maxRawTmp = ptrWdCfg[chIndex]->ptrWdContext->cicRate;
                    maxRawTmp *= maxRawTmp;
                }
                else
                {
                    if ((CY_CAPSENSE_CSD_GROUP == ptrWdCfg[chIndex]->senseGroup) &&
                        (ptrWdCfg[chIndex]->numCols <= snsIndex))
                    {
                        maxRawTmp = (uint32_t)context->ptrWdContext[wdIndex].sigPFC;
                    }
                    else
                    {
                        maxRawTmp = (uint32_t)context->ptrWdContext[wdIndex].maxRawCount;
                    }
                }

                rawTarget[chIndex] = (uint16_t)((maxRawTmp * target) / CY_CAPSENSE_PERCENTAGE_100);
                ptrRaw[chIndex] = &ptrWdCfg[chIndex]->ptrSnsContext[snsIndex].raw;
            }
        }
    }

    /* Perform calibration for all sensors */
    do
    {
        /* Scan all sensors in slot */
        calibStatus |= Cy_CapSense_ScanSlots(scanSlotId, 1u, context);
        calibStatus |= Cy_CapSense_WaitEndScan(CY_CAPSENSE_CALIBRATION_TIMEOUT, context);
        if(CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
        {
            Cy_CapSense_PreProcessSlotCIC2Raw(scanSlotId, context);
        }
        if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
        {
            break;
        }

        for(chIndex = 0u; chIndex < ptrCommCfg->numChannels; chIndex++)
        {
            if(0u != (calibrationBitEn & (1u << chIndex)))
            {
                if((CY_CAPSENSE_CSD_GROUP == ptrWdCfg[chIndex]->senseGroup) ||
                   (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode))
                {
                    /* Update CDAC based on scan result */
                    if (*ptrRaw[chIndex] < rawTarget[chIndex])
                    {
                        tempRefCdac[chIndex] &= (uint8_t)(~calMask);
                    }
                }
                else
                {
                    /* Update CDAC based on scan result */
                    if (*ptrRaw[chIndex] > rawTarget[chIndex])
                    {
                        tempRefCdac[chIndex] &= (uint8_t)(~calMask);
                    }
                }

                tempRefCdac[chIndex] |= (uint8_t)(calMask >> 1u);
                if(0u == tempRefCdac[chIndex])
                {
                    tempRefCdac[chIndex] = 1u;
                }

                /* Store cdac value */
                Cy_CapSense_SetCdacs(tempRefCdac[chIndex], scanSlotId, chIndex, context);
            }
        }

        calMask >>= 1u;
    }
    while(calMask != 0u);

    for(chIndex = 0u; chIndex < ptrCommCfg->numChannels; chIndex++)
    {
        if(0u != (calibrationBitEn && (1u << chIndex)))
        {
            Cy_CapSense_SetCdacs(tempRefCdac[chIndex], scanSlotId, chIndex, context);
        }
    }
    /* Perform scan again to get real raw count if CDAC was changed last iteration */
    calibStatus |= Cy_CapSense_ScanSlots(scanSlotId, 1u, context);
    calibStatus |= Cy_CapSense_WaitEndScan(CY_CAPSENSE_CALIBRATION_TIMEOUT, context);
    if(CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
    {
        Cy_CapSense_PreProcessSlotCIC2Raw(scanSlotId, context);
    }

    if (CY_CAPSENSE_STATUS_SUCCESS != calibStatus)
    {
        calibStatus = CY_CAPSENSE_STATUS_CALIBRATION_FAIL;
    }

    return (calibStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_NormalizeCdac
****************************************************************************//**
*
* This function normalizes compensation CDAC for selected widgets.
*
* \param widgetId
* Specifies the desired widget ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_NormalizeCdac(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    cy_stc_capsense_widget_config_t const * ptrWdCfg;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    const cy_stc_capsense_common_config_t * ptrCommCfg;
    uint8_t maxRefCdac;
    uint8_t maxRowRefCdac;
    uint16_t snsIndex;
    uint32_t twoDimentionWidget = 0u;

    if (NULL != context)
    {
        ptrWdCfg = &context->ptrWdConfig[widgetId];
        if((ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP) &&
                                ((CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdCfg->wdType) ||
                                (CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType)))
        {
            twoDimentionWidget = 1u;
        }
        ptrCommCfg = context->ptrCommonConfig;

        if (((ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP) &&
                (ptrCommCfg->csdCdacAutocalEn == CY_CAPSENSE_ENABLE)) ||
            ((ptrWdCfg->senseGroup == CY_CAPSENSE_CSX_GROUP) &&
                (ptrCommCfg->csxCdacAutocalEn == CY_CAPSENSE_ENABLE)))
        {
            /* Find max refCDAC value for widget */
            maxRefCdac = 0u;
            ptrSnsCxt = &ptrWdCfg->ptrSnsContext[0];

            if(ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP)
            {
                for(snsIndex = 0u; snsIndex < ptrWdCfg->numCols; snsIndex++)
                {
                    if(ptrSnsCxt->cdacComp > maxRefCdac)
                    {
                        maxRefCdac = ptrSnsCxt->cdacComp;
                    }
                    ptrSnsCxt++;
                }
            }
            else
            {
                for(snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
                {
                    if(ptrSnsCxt->cdacComp > maxRefCdac)
                    {
                        maxRefCdac = ptrSnsCxt->cdacComp;
                    }
                    ptrSnsCxt++;
                }

            }


            if (0u != twoDimentionWidget)
            {
                ptrSnsCxt = &ptrWdCfg->ptrSnsContext[ptrWdCfg->numCols];
                maxRowRefCdac = 0u;
                /* Go through all columns of touchpad to find max refCDAC value */
                for(snsIndex = ptrWdCfg->numCols; snsIndex < ptrWdCfg->numSns; snsIndex++)
                {
                    if(ptrSnsCxt->cdacComp > maxRowRefCdac)
                    {
                        maxRowRefCdac = ptrSnsCxt->cdacComp;
                    }
                    ptrSnsCxt++;
                }
            }

            ptrWdCfg->ptrWdContext->cdacRef[0u] = maxRefCdac;
            if (ptrWdCfg->ptrWdContext->cdacRef[0u] == 0u)
            {
                ptrWdCfg->ptrWdContext->cdacRef[0u] = 1;
            }
            if (((ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP) &&
                    (ptrCommCfg->csdCdacCompEn == CY_CAPSENSE_ENABLE)) ||
                ((ptrWdCfg->senseGroup == CY_CAPSENSE_CSX_GROUP) &&
                    (ptrCommCfg->csxCdacCompEn == CY_CAPSENSE_ENABLE)))
            {
                /* Recalculate the cdacRef value to approx. 50% on RefCDAC and 50% on CompCDAC */
                ptrWdCfg->ptrWdContext->cdacRef[0u] = (maxRefCdac + 1u) >> 1u;

                if (0u == ptrWdCfg->ptrWdContext->cdacRef[0u])
                {
                    ptrWdCfg->ptrWdContext->cdacRef[0u] = 1u;
                }
            }

            ptrSnsCxt = &ptrWdCfg->ptrSnsContext[0u];
            for(snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
            {
                ptrSnsCxt->cdacComp = 0u;
                ptrSnsCxt++;
            }

            /* Go through all columns of touchpad to normalize them */
            if (0u != twoDimentionWidget)
            {
                /* Recalculate cdacRef value */
                ptrWdCfg->ptrWdContext->rowCdacRef[0u] = maxRowRefCdac;
                if (ptrWdCfg->ptrWdContext->rowCdacRef[0u] == 0u)
                {
                    ptrWdCfg->ptrWdContext->rowCdacRef[0u] = 1u;
                }
                if (((ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP) &&
                        (ptrCommCfg->csdCdacCompEn == CY_CAPSENSE_ENABLE)) ||
                    ((ptrWdCfg->senseGroup == CY_CAPSENSE_CSX_GROUP) &&
                        (ptrCommCfg->csxCdacCompEn == CY_CAPSENSE_ENABLE)))
                {
                    /* Recalculate cdacRef value */
                    ptrWdCfg->ptrWdContext->rowCdacRef[0u] = (maxRowRefCdac + 1u) >> 1u;
                    if (ptrWdCfg->ptrWdContext->rowCdacRef[0u] == 0u)
                    {
                        ptrWdCfg->ptrWdContext->rowCdacRef[0u] = 1;
                    }
                }

                ptrSnsCxt = &ptrWdCfg->ptrSnsContext[ptrWdCfg->numCols];
                for(snsIndex = ptrWdCfg->numCols; snsIndex < ptrWdCfg->numSns; snsIndex++)
                {
                    ptrSnsCxt->cdacComp = 0u;
                    ptrSnsCxt++;
                }
            }
        }

        capStatus = CY_CAPSENSE_STATUS_SUCCESS;
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_VerifyCalibration
****************************************************************************//**
*
* Verifies that the calibrated widgets meets the configured conditions.
*
* This function checks whether the raw count of each sensor of the specified widgets
* is within the raw count range defined by raw count target and +/- calibration
* error.
*
* \param wdId
* Specifies the desired widget ID.
*
* \param csdTarget
* Specifies the calibration target in percentages of the maximum raw count
* for CSD sensing method.
*
* \param csxTarget
* Specifies the calibration target in percentages of the maximum raw count
* for CSX sensing method.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_VerifyCalibration(
                uint32_t wdId,
                uint32_t csdTarget,
                uint32_t csxTarget,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t calibStatus = CY_CAPSENSE_STATUS_BAD_PARAM;
    cy_stc_capsense_widget_config_t const * ptrWdCfg;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    const cy_stc_capsense_common_config_t * ptrCommCfg;
    uint16_t snsIndex;
    uint32_t lowerLimit;
    uint32_t upperLimit;
    uint32_t lowerLimitCmp;
    uint32_t upperLimitCmp;
    uint32_t target;
    uint32_t tmpRawCount;
    uint32_t calibrationError;

    if (NULL != context)
    {
        calibStatus = CY_CAPSENSE_STATUS_SUCCESS;

        calibStatus |= Cy_CapSense_ScanAllSlots(context);
        calibStatus |= Cy_CapSense_WaitEndScan(CY_CAPSENSE_CALIBRATION_TIMEOUT, context);
        Cy_CapSense_PreProcessAllRaw(context);


        if (CY_CAPSENSE_STATUS_SUCCESS == calibStatus)
        {
            ptrWdCfg = &context->ptrWdConfig[wdId];
            ptrCommCfg = context->ptrCommonConfig;

            if (((ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP) &&
                    (ptrCommCfg->csdCdacAutocalEn == CY_CAPSENSE_ENABLE)) ||
                ((ptrWdCfg->senseGroup == CY_CAPSENSE_CSX_GROUP) &&
                    (ptrCommCfg->csxCdacAutocalEn == CY_CAPSENSE_ENABLE)))
            {
                lowerLimit = 0u;

                /* Calculate acceptable raw count range based on the resolution, target and error */
                if (CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup)
                {
                    calibrationError = ptrCommCfg->csdCalibrationError;
                    target = csdTarget;
                }
                else
                {
                    calibrationError = ptrCommCfg->csxCalibrationError;
                    target = csxTarget;
                }

                if (target > calibrationError)
                {
                    lowerLimit = target - calibrationError;
                }
                upperLimit = target + calibrationError;
                if (upperLimit > CY_CAPSENSE_PERCENTAGE_100)
                {
                    upperLimit = CY_CAPSENSE_PERCENTAGE_100;
                }

                ptrSnsCxt = &ptrWdCfg->ptrSnsContext[0u];
                for(snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
                {
                    if (CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
                    {
                        tmpRawCount = ptrWdCfg->ptrWdContext->cicRate;
                        tmpRawCount *= tmpRawCount;
                    }
                    else
                    {
                        if ((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) &&
                            (ptrWdCfg->numCols <= snsIndex))
                        {
                            tmpRawCount = (uint32_t)context->ptrWdContext[wdId].sigPFC;
                        }
                        else
                        {
                            tmpRawCount = (uint32_t)context->ptrWdContext[wdId].maxRawCount;
                        }
                    }

                    lowerLimitCmp = (tmpRawCount * lowerLimit) / CY_CAPSENSE_PERCENTAGE_100;
                    upperLimitCmp = (tmpRawCount * upperLimit) / CY_CAPSENSE_PERCENTAGE_100;

                    tmpRawCount = ptrSnsCxt->raw;
                    if ((tmpRawCount < lowerLimitCmp) || (tmpRawCount > upperLimitCmp))
                    {
                        calibStatus = CY_CAPSENSE_STATUS_CALIBRATION_FAIL;
                        break;
                    }
                    ptrSnsCxt++;
                }
            }
        }
    }

    return (calibStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_SetCdacs
****************************************************************************//**
*
* Sets the desired CDAC value into Compensation CDAC field to be used in
* scanning when CDAC auto-calibration in progress.
*
* \param cdac
* CDAC value.
*
* \param scanSlotId
* Slot index.
*
* \param chId
* Channel ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_TIMEOUT       - The software watchdog timeout occurred
*                                      during the scan, the scan was not completed.
*
*******************************************************************************/
void Cy_CapSense_SetCdacs(
                uint32_t cdac,
                uint32_t scanSlotId,
                uint32_t chId,
                cy_stc_capsense_context_t * context)
{
    uint32_t wdIndex;
    uint32_t snsIndex;
    uint32_t snsPos;
    uint32_t * currentFramePtr;
    uint32_t sensorCfgTmp[CY_MSC_6_SNS_REGS] = {0u, 0u, 0u, 0u, 0u, 0u};

    snsPos = scanSlotId + (context->ptrCommonConfig->numSlots * chId);
    wdIndex = context->ptrScanSlots[snsPos].wdId;
    snsIndex = context->ptrScanSlots[snsPos].snsId;

    if((CY_CAPSENSE_SLOT_SHIELD_ONLY != wdIndex) &&
       (CY_CAPSENSE_SLOT_TX_ONLY != wdIndex) &&
       (CY_CAPSENSE_SLOT_EMPTY != wdIndex))
    {
        /* Update structure */
        context->ptrWdConfig[wdIndex].ptrSnsContext[snsIndex].cdacComp = (uint8_t)cdac;
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            Cy_CapSense_GenerateCdacConfig(
                    snsPos,
                    &sensorCfgTmp[0u],
                    context);

            currentFramePtr = &context->ptrSensorFrameContext[snsPos * context->ptrInternalContext->snsConfigSize];
            /* Store results */
            if (CY_MSC_5_SNS_REGS == context->ptrInternalContext->snsConfigSize)
            {
                currentFramePtr[CY_CAPSENSE_SNS_CDAC_CTL_INDEX - 1u] = sensorCfgTmp[CY_CAPSENSE_SNS_CDAC_CTL_INDEX];
            }
            else
            {
                currentFramePtr[CY_CAPSENSE_SNS_CDAC_CTL_INDEX] = sensorCfgTmp[CY_CAPSENSE_SNS_CDAC_CTL_INDEX];
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_WaitEndScan
****************************************************************************//**
*
* Waits till end of scan or till the provided timeout.
*
* \param timeout
* Watchdog timeout in microseconds.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_TIMEOUT       - The software watchdog timeout occurred
*                                      during the scan, the scan was not completed.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_WaitEndScan(
                uint32_t timeout,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t status = CY_CAPSENSE_STATUS_BAD_PARAM;

    /* Approximate duration of Wait For Scan */
    uint32_t isBusyLoopDuration = 5uL;

    uint32_t cpuFreqMHz = context->ptrCommonConfig->cpuClkHz / CY_CAPSENSE_CONVERSION_MEGA;
    uint32_t watchdogCounter;

    if (NULL != context)
    {
        status = CY_CAPSENSE_STATUS_SUCCESS;

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = Cy_CapSense_WatchdogCyclesNum(timeout, cpuFreqMHz, isBusyLoopDuration);
        while(CY_CAPSENSE_NOT_BUSY != (context->ptrCommonContext->status & CY_CAPSENSE_BUSY_ALL_CH_MASK))
        {
            if(0uL == watchdogCounter)
            {
                status = CY_CAPSENSE_STATUS_TIMEOUT;
                break;
            }
            watchdogCounter--;
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: Cy_Capsense_SlotPinState
****************************************************************************//**
*
* Configures the desired electrode the the specified state by
* updating the CapSense configuration.
*
* This function changes / overwrites configuration of an electrode (several
* pins in case the electrode is ganged to more pins) with a state provided
* by pinState parameter. The function does this only for the mentioned slot ID.
* If a pin should have the desired state during several scans, the function
* should be called multiple times for the each desired slot.
*
* The re-configuration is possible when
* parameter Sensor connection method = CTRLMUX.
* If parameter Sensor connection method = AMUXBUS, then the function
* returns CY_CAPSENSE_STATUS_BAD_CONFIG. In next releases
* the Cy_CapSense_SetPinState() function will be provided for the AMUXBUS
* configuration.
*
* The function changes the configuration of an electrode without storing
* the default state. A user is responsible to keep the default state to
* revert to the default settings if needed. Also, the default settings
* can be configured again by calling Cy_CapSense_Enable() function that
* leads to repeating CapSense Data Structure initialization, repeating
* DAC auto-calibration and repeating baselinning.
*
* Using this function is not recommended. This function is used to
* implement only the user's specific use cases (as changing the CapSense
* default configuration).
*
* Call this function from CapSense Data Structure Initialization Callback
* ptrEODsInitCallback. For details of how to register callback see
* the \ref group_capsense_callbacks section. That avoids repeating of
* DAC auto-calibration and baselining since the callback is called after
* CapSense Data Structure initialization but before the first initialization
* scan.
*
* The function is a low-level function and does not perform verification of
* input parameters (like slot ID, pointers, etc.). For example,
* CY_CAPSENSE_CTRLMUX_STATE_SHIELD is not available if shield is not configured
* in the project.
*
* \param slotId
* The desired slot ID.
*
* \param ptrEltdCfg
* The pointer to an electrode the all pins states of which will be configured
* as pinState parameter.
*
* \param pinState
* The desired pins state for CSX widget electrodes:
* * CY_CAPSENSE_CTRLMUX_STATE_RX - Rx electrode.
* * CY_CAPSENSE_CTRLMUX_STATE_TX - Tx electrode.
* * CY_CAPSENSE_CTRLMUX_STATE_GND - Grounded.
* * CY_CAPSENSE_CTRLMUX_STATE_TX_NEGATIVE - Negative Tx electrode
*     (for multi-phase TX method).
* The desired pins state for CSD widget electrodes:
* * CY_CAPSENSE_CTRLMUX_STATE_SNS - Self-cap sensor.
* * CY_CAPSENSE_CTRLMUX_STATE_HIGH_Z - Unconnected (high-z).
* * CY_CAPSENSE_CTRLMUX_STATE_GND - Grounded.
* * CY_CAPSENSE_CTRLMUX_STATE_SHIELD - Shield is routed to the pin.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS          - The operation is performed successfully.
* - CY_CAPSENSE_STATUS_BAD_CONFIG       - The function does not suppose to be
*                                         called with the current CapSense
*                                         configuration.
*
*******************************************************************************/
cy_capsense_status_t Cy_Capsense_SlotPinState(
                uint32_t slotId,
                const cy_stc_capsense_electrode_config_t * ptrEltdCfg,
                uint32_t pinState,
                cy_stc_capsense_context_t * context)
{
    uint32_t i;
    uint32_t j;
    uint32_t mask = 0u;
    uint32_t numRegs = context->ptrInternalContext->snsConfigSize;
    uint32_t * ptrSnsFrm = &context->ptrSensorFrameContext[(ptrEltdCfg->chId *
            context->ptrCommonConfig->numSlots + slotId) * numRegs];
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_SUCCESS;

    for (i = 0u; i < ptrEltdCfg->numPins; i++)
    {
        mask |= 1u << ptrEltdCfg->ptrPin->padNumber;
    }

    i = 3u;
    if (CY_MSC_5_SNS_REGS >= numRegs)
    {
        i--;
    }
    for (j = 0u; j < i; j++)
    {
        ptrSnsFrm[j] &= ~mask;
        if (0u != (context->ptrInternalContext->mapPinState[pinState] & (1u << (i - 1u - j))))
        {
            ptrSnsFrm[j] |= mask;
        }
    }
    return (capStatus);
}


#endif /* CY_IP_M0S8MSCV3 */


/* [] END OF FILE */
