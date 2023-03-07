/***************************************************************************//**
* \file cy_capsense_processing.c
* \version 3.0
*
* \brief
* This file provides the source code for the Data Processing module functions.
* The Data Processing module is responsible for the low-level raw count
* processing provided by the sensing module, maintaining baseline and
* difference values and performing high-level widget processing like
* updating button status or calculating slider position.
*
********************************************************************************
* \copyright
* Copyright 2018-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "cy_syslib.h"
#include <stddef.h>
#include <string.h>
#include "cy_capsense_common.h"
#include "cy_capsense_processing.h"
#include "cy_capsense_filter.h"
#include "cy_capsense_lib.h"
#include "cy_capsense_centroid.h"

#if (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3))

/*******************************************************************************
* Local definition
*******************************************************************************/
/* Raw data normalization and scaling */
#define CY_CAPSENSE_SCALING_SHIFT              (15)
#define CY_CAPSENSE_MAX_TX_PATTERN_NUM         (32)

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/
#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
static uint16_t Cy_CapSense_ConvertToUint16(int32_t sample);
#endif /* CY_CAPSENSE_PLATFORM_BLOCK_MSCV3 */
/** \} \endcond */


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeAllStatuses
****************************************************************************//**
*
* Performs initialization of all statuses and related modules including
* debounce counters and touch positions of all the widgets.
*
* The initialization includes the following tasks:
* * Reset the debounce counters of all the widgets.
* * Reset the number of touches.
* * Reset the position filter history for slider and touchpad widgets.
* * Clear all status of widgets and sensors.
* * Enable all the widgets.
*
* Calling this function is accompanied by
* * Cy_CapSense_InitializeAllBaselines().
* * Cy_CapSense_InitializeAllFilters().
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeAllStatuses(const cy_stc_capsense_context_t * context)
{
    uint32_t widgetId;

    for(widgetId = context->ptrCommonConfig->numWd; widgetId-- > 0u;)
    {
        Cy_CapSense_InitializeWidgetStatus(widgetId, context);
        Cy_CapSense_InitializeWidgetGestures(widgetId, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeWidgetStatus
****************************************************************************//**
*
* Performs initialization of all statuses, debounce counters, and touch positions
* of the specified widget.
*
* The initialization includes:
* * Resets the debounce counter of the widget.
* * Resets the number of touches.
* * Resets the position filter history for slider and touchpad widgets.
* * Clears widget and sensor statuses.
* * Enables the widget.
*
* The Button and Matrix Button widgets have individual debounce counters per
* sensor for the CSD widgets and per node for the CSX widgets.
*
* The Slider and Touchpad widgets have a single debounce counter per widget.
*
* The Proximity widget has two debounce counters per sensor. One is for the
* proximity event and the second is for the touch event.
*
* All debounce counters during initialization are set to the value of the
* onDebounce widget parameter.
*
* Calling this function is accompanied by
* * Cy_CapSense_InitializeWidgetBaseline().
* * Cy_CapSense_InitializeWidgetFilter().
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeWidgetStatus(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t filterSize;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdCfg->ptrSnsContext;
    uint32_t snsNumber = ptrWdCfg->numSns;
    cy_stc_capsense_position_t * ptrHistory;

    /* Clear widget statuses (Non active, Not disabled, Working) */
    ptrWdCxt->status &= (uint8_t)~(CY_CAPSENSE_WD_ACTIVE_MASK |
                                   CY_CAPSENSE_WD_DISABLE_MASK |
                                   CY_CAPSENSE_WD_WORKING_MASK);
    /* Clear sensor status */
    for (snsIndex = snsNumber; snsIndex-- >0u;)
    {
        ptrSnsCxt->status &= (uint8_t)~(CY_CAPSENSE_SNS_TOUCH_STATUS_MASK | CY_CAPSENSE_SNS_PROX_STATUS_MASK);
        ptrSnsCxt++;
    }

    /* Reset debounce counters */
    switch (ptrWdCfg->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
            /* Each button requires one debounce counter */
            (void)memset(ptrWdCfg->ptrDebounceArr, (int32_t)ptrWdCxt->onDebounce, (size_t)snsNumber);
            break;
        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            /* Each widget requires one debounce counter */
            if (ptrWdCfg->senseGroup == CY_CAPSENSE_CSD_GROUP)
            {
                *(ptrWdCfg->ptrDebounceArr) = ptrWdCxt->onDebounce;
            }
            else
            {
                /*
                * CSX Touchpad has debounce located in another place. Moreover,
                * debounce counter is initialized at ID assignment, so no need
                * to do it here.
                */
            }
            break;
        case (uint8_t)CY_CAPSENSE_WD_PROXIMITY_E:
            /* Proximity widgets have 2 debounce counters per sensor (for touch and prox detection) */
            (void)memset(ptrWdCfg->ptrDebounceArr, (int32_t)ptrWdCxt->onDebounce, (size_t)(snsNumber << 1u));
            break;
        default:
            break;
    }

    /* Reset touch numbers */
    switch (ptrWdCfg->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
            /* Clean number of touches */
            ptrWdCxt->wdTouch.numPosition = CY_CAPSENSE_POSITION_NONE;
            if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
            {
                ptrWdCfg->ptrPosFilterHistory->numPosition = CY_CAPSENSE_POSITION_NONE;
            }
            break;
        default:
            break;
    }

    /* Reset ballistic displacement */
    if (0u != (ptrWdCfg->centroidConfig & CY_CAPSENSE_CENTROID_BALLISTIC_MASK))
    {
        ptrWdCxt->xDelta = 0;
        ptrWdCxt->yDelta = 0;
        ptrWdCfg->ptrBallisticContext->oldTouchNumber = 0u;
    }

    /* Reset touch history */
    if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        switch (ptrWdCfg->wdType)
        {
            case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
                /* Clean position filter history */
                if (ptrWdCfg->senseGroup == CY_CAPSENSE_CSX_GROUP)
                {
                    /* Reset all history IDs to undefined state */
                    ptrHistory = ptrWdCfg->ptrPosFilterHistory->ptrPosition;
                    filterSize = (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_SIZE_MASK) >> CY_CAPSENSE_POSITION_FILTERS_SIZE_OFFSET;
                    for (snsIndex = 0u; snsIndex < CY_CAPSENSE_MAX_CENTROIDS; snsIndex++)
                    {
                        ptrHistory->id = CY_CAPSENSE_CSX_TOUCHPAD_ID_UNDEFINED;
                        ptrHistory += filterSize;
                    }
                }
                break;
            default:
                break;
        }
        /* Init Adaptive IIR filter */
        if (0u != (ptrWdCfg->posFilterConfig & CY_CAPSENSE_POSITION_AIIR_MASK))
        {
            Cy_CapSense_AdaptiveFilterInitialize_Lib_Call(&ptrWdCfg->aiirConfig,
                                                          ptrWdCfg->ptrPosFilterHistory->ptrPosition,
                                                          context);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InitializeWidgetGestures
****************************************************************************//**
*
* Performs initialization of all gestures for the specified widget.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InitializeWidgetGestures(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;

    if (((uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E == ptrWdCfg->wdType) ||
        ((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType))
    {
        if (NULL != ptrWdCfg->ptrGestureConfig)
        {
            if (0u != (ptrWdCfg->ptrGestureConfig->gestureEnableMask & CY_CAPSENSE_GESTURE_ALL_GESTURES_MASK))
            {
                ptrWdCxt->gestureDetected = 0u;
                ptrWdCxt->gestureDirection = 0u;
                Cy_CapSense_Gesture_ResetState(ptrWdCfg->ptrGestureContext);
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DecodeWidgetGestures
****************************************************************************//**
*
* Performs decoding of all gestures for the specified widget.
*
* This function should be called by application program only after all sensors
* are scanned and all data processing is executed using
* Cy_CapSense_ProcessAllWidgets() or Cy_CapSense_ProcessWidget() functions
* for the widget. Calling this function multiple times without a new sensor
* scan and process causes unexpected behavior.
*
* \note The function (Gesture detection functionality) requires a timestamp
* for its operation. The timestamp should be initialized and maintained
* in the application program prior to calling this function. See the
* descriptions of the Cy_CapSense_SetGestureTimestamp() and
* Cy_CapSense_IncrementGestureTimestamp() functions for details.
*
* \param widgetId
* Specifies the ID number of the widget. A macro for the widget ID can be found
* in the cycfg_capsense.h file defined as CY_CAPSENSE_<WIDGET_NAME>_WDGT_ID.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the detected Gesture mask and direction of detected gestures.
* The same information is stored in ptrWdContext->gestureDetected and
* ptrWdContext->gestureDirection registers. Corresponding macros could be found
* \ref group_capsense_macros_gesture.
* * bit[0..15] - detected gesture masks gesture
*   * bit[0] - one-finger single click gesture
*   * bit[1] - one-finger double click gesture
*   * bit[2] - one-finger click and drag gesture
*   * bit[3] - two-finger single click gesture
*   * bit[4] - one-finger scroll gesture
*   * bit[5] - two-finger scroll gesture
*   * bit[6] - one-finger edge swipe
*   * bit[7] - one-finger flick
*   * bit[8] - one-finger rotate
*   * bit[9] - two-finger zoom
*   * bit[13] - touchdown event
*   * bit[14] - liftoff event
* * bit[16..31] - gesture direction if detected
*    * bit[0..1] - direction of one-finger scroll gesture
*    * bit[2..3] - direction of two-finger scroll gesture
*    * bit[4..5] - direction of one-finger edge swipe gesture
*    * bit[6] - direction of one-finger rotate gesture
*    * bit[7] - direction of two-finger zoom gesture
*    * bit[8..10] - direction of one-finger flick gesture
*
* \funcusage
*
* An example of gesture decoding:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Gesture
*
* An example of gesture status parsing:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Gesture_Macro
*
*******************************************************************************/
uint32_t Cy_CapSense_DecodeWidgetGestures(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context)
{
    uint32_t gestureStatus = 0u;
    uint32_t posIndex;
    uint32_t positionNum;
    const cy_stc_capsense_widget_config_t * ptrWdCfg = &context->ptrWdConfig[widgetId];
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdCfg->ptrWdContext;
    cy_stc_capsense_gesture_position_t position[CY_CAPSENSE_MAX_CENTROIDS];

    if (((uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E == ptrWdCfg->wdType) ||
        ((uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType))
    {
        if (NULL != ptrWdCfg->ptrGestureConfig)
        {
            if (0u != (ptrWdCfg->ptrGestureConfig->gestureEnableMask & CY_CAPSENSE_GESTURE_ALL_GESTURES_MASK))
            {
                positionNum = ptrWdCxt->wdTouch.numPosition;
                if (positionNum > CY_CAPSENSE_MAX_CENTROIDS)
                {
                    positionNum = 0u;
                }
                for (posIndex = 0u; posIndex < positionNum; posIndex++)
                {
                    position[posIndex].x = ptrWdCxt->wdTouch.ptrPosition[posIndex].x;
                    position[posIndex].y = ptrWdCxt->wdTouch.ptrPosition[posIndex].y;
                }
                Cy_CapSense_Gesture_Decode(context->ptrCommonContext->timestamp, (uint32_t)ptrWdCxt->wdTouch.numPosition,
                        &position[0u], ptrWdCfg->ptrGestureConfig, ptrWdCfg->ptrGestureContext);
                ptrWdCxt->gestureDetected = ptrWdCfg->ptrGestureContext->detected;
                ptrWdCxt->gestureDirection = ptrWdCfg->ptrGestureContext->direction;
                gestureStatus = (uint32_t)ptrWdCxt->gestureDetected | ((uint32_t)ptrWdCxt->gestureDirection << CY_CAPSENSE_GESTURE_DIRECTION_OFFSET);
            }
        }
    }
    return gestureStatus;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetRawCounts
****************************************************************************//**
*
* Performs default processing of the raw counts of the specified CSX widget.
*
* The processing includes the following tasks:
* - Run Filters.
* - Update Baselines.
* - Update Differences.
* The same process is applied to all the sensors of the specified widget.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified widget processing operation:
* - Zero - if operation was successfully completed;
* - Non-zero - if baseline processing of any sensor of the specified widget
* failed. The result is concatenated with the index of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsxWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = CY_CAPSENSE_STATUS_SUCCESS;
    uint32_t snsIndex;
    uint32_t snsHistorySize;
    uint32_t freqChIndex;
    uint32_t freqChNumber;
    uint16_t * ptrHistoryCh;
    uint16_t * ptrHistorySns;
    uint16_t * ptrBslnInvCh = ptrWdConfig->ptrBslnInv;
    uint16_t * ptrBslnInvSns;
    uint8_t * ptrHistoryLowCh = NULL;
    uint8_t * ptrHistoryLowSns = NULL;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtCh;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtSns;

    snsHistorySize = (uint32_t)ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK;
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;

    ptrSnsCxtCh = &ptrWdConfig->ptrSnsContext[0u];
    ptrHistoryCh = &ptrWdConfig->ptrRawFilterHistory[0u];
    if(CY_CAPSENSE_IIR_FILTER_PERFORMANCE == (ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK))
    {
        ptrHistoryLowCh = &ptrWdConfig->ptrRawFilterHistoryLow[0u];
    }

    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        ptrSnsCxtSns = ptrSnsCxtCh;
        ptrHistorySns = ptrHistoryCh;
        ptrHistoryLowSns = ptrHistoryLowCh;
        ptrBslnInvSns = ptrBslnInvCh;
        for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
        {
            Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                         ptrSnsCxtSns,
                                                         ptrHistorySns,
                                                         ptrHistoryLowSns,
                                                         context);
            result |= Cy_CapSense_FtUpdateBaseline(ptrWdConfig->ptrWdContext, ptrSnsCxtSns, ptrBslnInvSns, context);
            Cy_CapSense_DpUpdateDifferences(ptrWdConfig->ptrWdContext, ptrSnsCxtSns);
            ptrSnsCxtSns++;
            ptrBslnInvSns++;
            ptrHistorySns += snsHistorySize;
            if(NULL != ptrHistoryLowSns)
            {
                ptrHistoryLowSns++;
            }
        }

        ptrSnsCxtCh += context->ptrCommonConfig->numSns;
        ptrBslnInvCh += context->ptrCommonConfig->numSns;
        ptrHistoryCh += context->ptrCommonConfig->numSns * snsHistorySize;
        if(NULL != ptrHistoryLowCh)
        {
            ptrHistoryLowCh += context->ptrCommonConfig->numSns;
        }
    }

    if(CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn)
    {
        ptrSnsCxtSns = ptrWdConfig->ptrSnsContext;
        for (snsIndex = ptrWdConfig->numSns; snsIndex-- > 0u;)
        {
            Cy_CapSense_RunMfsFiltering(ptrSnsCxtSns, context);
            ptrSnsCxtSns++;
        }
    }

    return result;
}

/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxWidgetStatus
****************************************************************************//**
*
* Updates the status of the CSX widget in the Data Structure.
*
* This function determines the type of widget and runs the appropriate function
* that implements the status update algorithm for this type of widget.
*
* When the widget-specific processing completes this function updates the
* sensor and widget status registers in the data structure.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    switch (ptrWdConfig->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
            Cy_CapSense_DpProcessButton_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            Cy_CapSense_DpProcessCsxTouchpad_Call(ptrWdConfig, context);
            break;

    default:
        CY_ASSERT(0 != 0);
        break;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxSensorRawCountsExt
****************************************************************************//**
*
* Performs customized processing of the CSX sensor raw counts.
*
* If all bits are set at once, the default processing order will take place.
* For a custom order, this function can be called multiple times and execute
* one task at a time.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param ptrSnsContext
* The pointer to the sensor context structure.
*
* \param ptrSnsRawHistory
* The pointer to the filter history.
*
* \param ptrSnsRawHistoryLow
* The pointer to the extended filter history.
*
* \param mode
* The bit-mask with the data processing tasks to be executed.
* The mode parameters can take the following values:
* - CY_CAPSENSE_PROCESS_FILTER     (0x01) Run Firmware Filter
* - CY_CAPSENSE_PROCESS_BASELINE   (0x02) Update Baselines
* - CY_CAPSENSE_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
* - CY_CAPSENSE_PROCESS_ALL               Execute all tasks
*
* \param ptrSnsBslnInv
* The pointer to the sensor baseline inversion used for BIST if enabled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified sensor processing operation:
* - CY_CAPSENSE_STATUS_SUCCESS if operation was successfully completed;
* - Non-zero - if baseline processing of any
* sensor of the specified widget failed. The result is concatenated with the index
* of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsxSensorRawCountsExt(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                uint32_t mode,
                uint16_t * ptrSnsBslnInv,
                const cy_stc_capsense_context_t * context)
{
    uint32_t  result = CY_CAPSENSE_STATUS_SUCCESS;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    if (0u != (mode & CY_CAPSENSE_PROCESS_FILTER))
    {
        Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                     ptrSnsContext,
                                                     ptrSnsRawHistory,
                                                     ptrSnsRawHistoryLow,
                                                     context);

    }

    if (0u != (mode & CY_CAPSENSE_PROCESS_BASELINE))
    {
        result = Cy_CapSense_FtUpdateBaseline(ptrWdCxt, ptrSnsContext, ptrSnsBslnInv, context);
    }
    if (0u != (mode & CY_CAPSENSE_PROCESS_DIFFCOUNTS))
    {
        Cy_CapSense_DpUpdateDifferences(ptrWdCxt, ptrSnsContext);
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetRawCounts
****************************************************************************//**
*
* Performs default processing of the raw counts of the specified CSD widget.
*
* The processing includes the following tasks:
* - Run Filters.
* - Update Baselines.
* - Update Differences.
* The same process is applied to all the sensors of the specified widget.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified widget processing operation:
* - Zero - if operation was successfully completed.
* - Non-zero - if baseline processing of any sensor of the specified widget
* failed. The result is concatenated with the index of failed sensor.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsdWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t result = CY_CAPSENSE_STATUS_SUCCESS;
    uint32_t snsIndex;
    uint32_t snsHistorySize;
    uint32_t freqChIndex;
    uint32_t freqChNumber;
    uint16_t * ptrHistoryCh;
    uint16_t * ptrHistorySns;
    uint16_t * ptrBslnInvCh = ptrWdConfig->ptrBslnInv;
    uint16_t * ptrBslnInvSns;
    uint8_t * ptrHistoryLowCh = NULL;
    uint8_t * ptrHistoryLowSns = NULL;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtCh;
    cy_stc_capsense_sensor_context_t * ptrSnsCxtSns;

    #if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
        cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNEHistory = ptrWdConfig->ptrNoiseEnvelope;
    #endif

    snsHistorySize = (uint32_t)ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK;
    freqChNumber = (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn) ? 3u : 1u;

    ptrSnsCxtCh = &ptrWdConfig->ptrSnsContext[0u];
    ptrHistoryCh = &ptrWdConfig->ptrRawFilterHistory[0u];
    if(CY_CAPSENSE_IIR_FILTER_PERFORMANCE == (ptrWdConfig->rawFilterConfig & CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK))
    {
        ptrHistoryLowCh = &ptrWdConfig->ptrRawFilterHistoryLow[0u];
    }

    for(freqChIndex = 0u; freqChIndex < freqChNumber; freqChIndex++)
    {
        ptrSnsCxtSns = ptrSnsCxtCh;
        ptrHistorySns = ptrHistoryCh;
        ptrBslnInvSns = ptrBslnInvCh;
        ptrHistoryLowSns = ptrHistoryLowCh;
        for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
        {
            Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig,
                                                         ptrSnsCxtSns,
                                                         ptrHistorySns,
                                                         ptrHistoryLowSns,
                                                         context);
            #if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
                /* Run auto-tuning activities */
                if (CY_CAPSENSE_CSD_SS_HWTH_EN == context->ptrCommonConfig->csdAutotuneEn)
                {
                    Cy_CapSense_RunNoiseEnvelope_Lib_Call(ptrSnsCxtSns->raw,
                                                          ptrWdConfig->ptrWdContext->sigPFC,
                                                          ptrNEHistory,
                                                          context);
                    Cy_CapSense_DpUpdateThresholds_Call(ptrWdConfig->ptrWdContext, ptrNEHistory, snsIndex, context);
                    if ((uint8_t)CY_CAPSENSE_WD_PROXIMITY_E == ptrWdConfig->wdType)
                    {
                        ptrWdConfig->ptrWdContext->proxTh = (uint16_t)(((uint32_t)ptrWdConfig->ptrWdContext->fingerTh *
                            context->ptrCommonConfig->proxTouchCoeff) / CY_CAPSENSE_PERCENTAGE_100);
                    }
                    ptrNEHistory++;
                }
            #endif

            result |= Cy_CapSense_FtUpdateBaseline(ptrWdConfig->ptrWdContext, ptrSnsCxtSns, ptrBslnInvSns, context);
            Cy_CapSense_DpUpdateDifferences(ptrWdConfig->ptrWdContext, ptrSnsCxtSns);

            ptrSnsCxtSns++;
            ptrBslnInvSns++;
            ptrHistorySns += snsHistorySize;
            if(NULL != ptrHistoryLowSns)
            {
                ptrHistoryLowSns++;
            }

        }

        ptrSnsCxtCh += context->ptrCommonConfig->numSns;
        ptrBslnInvCh += context->ptrCommonConfig->numSns;
        ptrHistoryCh += context->ptrCommonConfig->numSns * snsHistorySize;
        if(NULL != ptrHistoryLowCh)
        {
            ptrHistoryLowCh += context->ptrCommonConfig->numSns;
        }

    }

    if(CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mfsEn)
    {
        ptrSnsCxtSns = ptrWdConfig->ptrSnsContext;
        for (snsIndex = ptrWdConfig->numSns; snsIndex-- > 0u;)
        {
            Cy_CapSense_RunMfsFiltering(ptrSnsCxtSns, context);
            ptrSnsCxtSns++;
        }
    }

    return result;

}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdSensorRawCountsExt
****************************************************************************//**
*
* Performs customized processing of the CSX sensor raw counts.
*
* If all bits are set at once, the default processing order will take place.
* For a custom order, this function can be called multiple times and execute
* one task at a time.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param ptrSnsContext
* The pointer to the sensor context structure.
*
* \param ptrSnsRawHistory
* The pointer to the filter history.
*
* \param ptrSnsRawHistoryLow
* The pointer to the extended filter history.
*
* \param mode
* The bit-mask with the data processing tasks to be executed.
* The mode parameters can take the following values:
* - CY_CAPSENSE_PROCESS_FILTER     (0x01) Run Firmware Filter
* - CY_CAPSENSE_PROCESS_BASELINE   (0x02) Update Baselines
* - CY_CAPSENSE_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
* - CY_CAPSENSE_PROCESS_CALC_NOISE (0x08) Calculate Noise (only if FW Tuning is enabled)
* - CY_CAPSENSE_PROCESS_THRESHOLDS (0x10) Update Thresholds (only if FW Tuning is enabled)
* - CY_CAPSENSE_PROCESS_ALL               Execute all tasks
*
* \param ptrBslnInvSns
* The pointer to the sensor baseline inversion used for BIST if enabled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the specified sensor processing operation:
* - CY_CAPSENSE_STATUS_SUCCESS if operation was successfully completed.
* - CY_CAPSENSE_STATUS_BAD_DATA if baseline processing of any sensor of the specified widget
*   failed.
*
*******************************************************************************/
uint32_t Cy_CapSense_DpProcessCsdSensorRawCountsExt(
                    const cy_stc_capsense_widget_config_t * ptrWdConfig,
                    cy_stc_capsense_sensor_context_t * ptrSnsContext,
                    uint16_t * ptrSnsRawHistory,
                    uint8_t * ptrSnsRawHistoryLow,
                    uint32_t mode,
                    uint16_t * ptrBslnInvSns,
                    const cy_stc_capsense_context_t * context)
{
    uint32_t  result = CY_CAPSENSE_STATUS_SUCCESS;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    if (0u != (mode & CY_CAPSENSE_PROCESS_FILTER))
    {
        Cy_CapSense_FtRunEnabledFiltersInternal_Call(ptrWdConfig, ptrSnsContext,
                                                     ptrSnsRawHistory, ptrSnsRawHistoryLow,
                                                     context);
    }

    if (0u != (mode & CY_CAPSENSE_PROCESS_BASELINE))
    {
        result = Cy_CapSense_FtUpdateBaseline(ptrWdCxt, ptrSnsContext, ptrBslnInvSns, context);
    }
    if (0u != (mode & CY_CAPSENSE_PROCESS_DIFFCOUNTS))
    {
        Cy_CapSense_DpUpdateDifferences(ptrWdCxt, ptrSnsContext);
    }

    return result;
}

/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdWidgetStatus
****************************************************************************//**
*
* Updates the status of the CSD widget in the Data Structure.
*
* This function determines the type of widget and runs the appropriate function
* that implements the status update algorithm for this type of widget.
*
* When the widget-specific processing completes this function updates the
* sensor and widget status registers in the data structure.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context)
{
    switch (ptrWdConfig->wdType)
    {
        case (uint8_t)CY_CAPSENSE_WD_BUTTON_E:
            Cy_CapSense_DpProcessButton_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
        case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
            Cy_CapSense_DpProcessSlider_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E:
            Cy_CapSense_DpProcessCsdMatrix_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_TOUCHPAD_E:
            Cy_CapSense_DpProcessCsdTouchpad_Call(ptrWdConfig, context);
            break;

        case (uint8_t)CY_CAPSENSE_WD_PROXIMITY_E:
            Cy_CapSense_DpProcessProximity_Call(ptrWdConfig, context);
            break;

        default:
            break;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpUpdateThresholds
****************************************************************************//**
*
* Updates noise and finger thresholds for a specified widget.
*
* This algorithm is a part of SmartSense feature.
*
* \param ptrWdContext
* The pointer to the widget context structure.
*
* \param ptrNoiseEnvelope
* The pointer to the noise-envelope history structure.
*
* \param startFlag
* The flag indicates when a new widget is processed.
*
*******************************************************************************/
void Cy_CapSense_DpUpdateThresholds(
                cy_stc_capsense_widget_context_t * ptrWdContext,
                const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                uint32_t startFlag)
{
    cy_stc_capsense_smartsense_update_thresholds_t thresholds;

    /* Calculate Thresholds */
    thresholds.fingerTh = ptrWdContext->fingerTh;
    Cy_CapSense_UpdateThresholds_Lib(ptrNoiseEnvelope, &thresholds, ptrWdContext->sigPFC, startFlag);

    /* Update CapSense context */
    ptrWdContext->fingerTh = thresholds.fingerTh;
    ptrWdContext->noiseTh = thresholds.noiseTh;
    ptrWdContext->nNoiseTh = thresholds.nNoiseTh;
    ptrWdContext->hysteresis = thresholds.hysteresis;
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpUpdateDifferences
****************************************************************************//**
*
* Calculates new difference values.
*
* This function calculates the difference between the baseline and raw counts.
* If the difference is positive (raw > baseline), and higher than the widget
* noise threshold, it is saved into the data structure for further processing.
* Otherwise the difference is set to zero. The final difference value is saved
* with the subtracted noise threshold value.
*
* \param ptrWdContext
* The pointer to the widget context structure.
*
* \param ptrSnsContext
* The pointer to the sensor context structure.
*
*******************************************************************************/
void Cy_CapSense_DpUpdateDifferences(
                const cy_stc_capsense_widget_context_t * ptrWdContext,
                cy_stc_capsense_sensor_context_t * ptrSnsContext)
{
    ptrSnsContext->diff = 0u;
    if (ptrSnsContext->raw > (ptrSnsContext->bsln + ptrWdContext->noiseTh))
    {
        ptrSnsContext->diff = ptrSnsContext->raw - ptrSnsContext->bsln;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessButton
****************************************************************************//**
*
* Processes the status of the Button widget.
*
* This function processes the status of the CSD/CSX Button widget and
* CSX Matrix Button widget. It applies the hysteresis and debounce algorithm
* to each sensor difference value. This function is expected to be called
* after each new widget scan. If it is called multiple times for the same
* scan data, the debounce algorithm will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessButton(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t activeCount = 0u;
    uint32_t startIndex = 0u;
    uint32_t touchTh;

    (void) context;

    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all widget's sensors */
    for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
    {
        /* Calculate touch threshold based on current sensor state */
        touchTh = (0u == ptrSnsCxt->status) ?
                  ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
                  ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= touchTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status = 0u;
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
            activeCount++;
            startIndex = snsIndex;
        }

        /* Update widget status */
        if (0u != ptrSnsCxt->status)
        {
            ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
        }

        ptrSnsCxt++;
        ptrDebounceCnt++;
    }
    /* Update position struct */
    if (((uint8_t)CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdConfig->wdType) &&
        (CY_CAPSENSE_CSX_GROUP == ptrWdConfig->senseGroup))
    {
        ptrWdCxt->wdTouch.numPosition = (uint8_t)activeCount;
        ptrWdCxt->wdTouch.ptrPosition->id = (uint16_t)startIndex;
        ptrWdCxt->wdTouch.ptrPosition->x = (uint16_t)(startIndex / ptrWdConfig->numRows);
        ptrWdCxt->wdTouch.ptrPosition->y = (uint16_t)(startIndex % ptrWdConfig->numRows);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessProximity
****************************************************************************//**
*
* Processes the status of the Proximity widget.
*
* This function processes the status of the CSD Proximity widget. It applies the
* hysteresis and debounce algorithm to each sensor difference value.
* The proximity and touch events are considered independently so debounce and
* hysteresis are also applied independently.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
*******************************************************************************/
void Cy_CapSense_DpProcessProximity(const cy_stc_capsense_widget_config_t * ptrWdConfig)
{
    uint32_t snsTh;
    uint32_t snsIndex;
    uint32_t snsStMask;

    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;

    /* Reset widget status */
    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all sensor's status bits */
    for (snsIndex = 0u; snsIndex < ((uint32_t)ptrWdConfig->numSns << 1u); snsIndex++)
    {
        /*
        * Proximity - odd; Touch - even. Example:
        * Bit 0 -> touch event
        * Bit 1 -> proximity event
        */
        snsTh = ptrWdCxt->proxTh;
        snsStMask = CY_CAPSENSE_SNS_PROX_STATUS_MASK;
        if (0u == (snsIndex & 0x01u))
        {
            snsTh = ptrWdCxt->fingerTh;
            snsStMask = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
        }
        /* Calculate threshold based on current sensor state */
        snsTh = (0u == (snsStMask & ptrSnsCxt->status)) ?
                  (snsTh + ptrWdCxt->hysteresis) :
                  (snsTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= snsTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status &= (uint8_t)(~snsStMask);
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status |= (uint8_t)snsStMask;
        }

        /* Update widget status */
        if (0u != (ptrSnsCxt->status & (uint8_t)snsStMask))
        {
            ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
        }

        if (0u != (snsIndex & 0x01u))
        {
            ptrSnsCxt++;
        }
        ptrDebounceCnt++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessSlider
****************************************************************************//**
*
* Processes the status of the Linear/Radial Slider widget.
* The process involves running the Linear/Radial centroid algorithm.
*
* It applies the hysteresis and debounce algorithm to the widget ignoring
* the individual states of the sensors.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data, the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessSlider(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;
    uint32_t wdActive = 0u;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    uint32_t sensorNumber = ptrWdConfig->numSns;
    cy_stc_capsense_position_t newPosition[CY_CAPSENSE_MAX_CENTROIDS];
    cy_stc_capsense_touch_t newTouch = {&newPosition[0u], CY_CAPSENSE_POSITION_NONE};

    /* Calculate touch threshold based on current slider state */
    touchTh = (0u == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK)) ?
              ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
              ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

    if (0u < (*ptrDebounceCnt))
    {
        /* Decrement debounce counter */
        (*ptrDebounceCnt)--;
    }

    /* Check new widget activity */
    for (snsIndex = sensorNumber; snsIndex-- > 0u;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActive |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }

    /* No touch detected */
    if (0u == wdActive)
    {
        /* Reset debounce counter */
        (*ptrDebounceCnt) = ptrWdCxt->onDebounce;
        ptrWdCxt->status &= (uint8_t)(~CY_CAPSENSE_WD_ACTIVE_MASK);
    }

    if (0u == (*ptrDebounceCnt))
    {
        /* New touch detected or touch still exists from previous cycle */
        ptrWdCxt->status |= CY_CAPSENSE_WD_ACTIVE_MASK;
    }
    else
    {
        if (0u != wdActive)
        {
            /* Clear sensor state if activity was detected but debounce was not passed */
            ptrSnsCxt = ptrWdConfig->ptrSnsContext;

            for (snsIndex = sensorNumber; snsIndex-- > 0u;)
            {
                ptrSnsCxt->status = 0u;
                ptrSnsCxt++;
            }
        }
    }

    /* Centroid processing */
    if (CY_CAPSENSE_WD_ACTIVE_MASK == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK))
    {
        switch (ptrWdConfig->wdType)
        {
            case (uint8_t)CY_CAPSENSE_WD_RADIAL_SLIDER_E:
                Cy_CapSense_DpCentroidRadial(&newTouch, ptrWdConfig);
                break;

            case (uint8_t)CY_CAPSENSE_WD_LINEAR_SLIDER_E:
                if ((0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_DIPLEXING_MASK)))
                {
                    /* Run local maximum search for diplexed slider */
                    Cy_CapSense_DpCentroidDiplex(&newTouch, ptrWdConfig);
                }
                else
                {
                    Cy_CapSense_DpCentroidLinear(&newTouch, ptrWdConfig);
                }
                break;

            default:
                break;
        }
    }

    /* Position filtering */
    if (0u != (ptrWdConfig->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        Cy_CapSense_ProcessPositionFilters_Call(&newTouch, ptrWdConfig, context);
    }
    /* Copy positions into public structure */
    ptrWdConfig->ptrWdContext->wdTouch.numPosition = newTouch.numPosition;
    for (snsIndex = 0u; snsIndex < newTouch.numPosition; snsIndex++)
    {
        ptrWdConfig->ptrWdContext->wdTouch.ptrPosition[snsIndex] = newTouch.ptrPosition[snsIndex];
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdMatrix
****************************************************************************//**
*
* Processes the status of the CSD Matrix Button widget.
*
* This function processes the status of the CSD Matrix Button widget.
* It applies the hysteresis and debounce algorithm to each sensor value.
*
* Then the function analyzes how many row and column sensors are active.
* If only one per row and one per column, the function considers this as
* a valid touch and updates the corresponding Data Structure registers
* with the button id and active row and column sensors.
*
* If multiple sensors are active in row or column sensors, this function sets
* the corresponding registers to the CY_CAPSENSE_POSITION_MULTIPLE
* value that indicates that it is not possible to detect the touched button id.
*
* This function is expected to be called after each new widget scan. If it is
* called multiple times for the same scan data, the debounce algorithm
* will not work properly.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdMatrix(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;

    (void) context;

    uint32_t colNumber = ptrWdConfig->numCols;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    cy_stc_capsense_position_t * ptrSnsTouch = ptrWdCxt->wdTouch.ptrPosition;

    uint32_t numActiveRows = 0u;
    uint32_t numActiveCols = 0u;

    uint32_t activeRow = 0u;
    uint32_t activeCol = 0u;

    /* Reset status */
    ptrWdCxt->status &= (uint8_t)~CY_CAPSENSE_WD_ACTIVE_MASK;

    /* Go through all widget's sensors */
    for (snsIndex = 0u; snsIndex < ptrWdConfig->numSns; snsIndex++)
    {
        /* Calculate touch threshold based on current sensor state */
        touchTh = (0u == ptrSnsCxt->status) ?
                  ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
                  ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* No touch */
        if (ptrSnsCxt->diff <= touchTh)
        {
            /* Reset debounce counter */
            *ptrDebounceCnt = ptrWdCxt->onDebounce;
            ptrSnsCxt->status = 0u;
        }

        /* New touch or touch still exists */
        if (0u == (*ptrDebounceCnt))
        {
            ptrSnsCxt->status = CY_CAPSENSE_SNS_TOUCH_STATUS_MASK;
        }

        /* Update information about active row/col sensors */
        if (0u != ptrSnsCxt->status)
        {
            if (snsIndex < colNumber)
            {
                numActiveCols++;
                activeCol = snsIndex;
            }
            else
            {
                numActiveRows++;
                activeRow = snsIndex - colNumber;
            }
        }

        ptrSnsCxt++;
        ptrDebounceCnt++;
    }

    /*
    * Number of touches (numActiveCols * numActiveRows):
    * 0 -> No touch
    * 1 -> One touch
    * 2+ -> Multiple touches
    */

    ptrWdCxt->wdTouch.numPosition = (uint8_t)(numActiveCols * numActiveRows);
    if (ptrWdCxt->wdTouch.numPosition > CY_CAPSENSE_POSITION_ONE)
    {
        ptrWdCxt->wdTouch.numPosition = CY_CAPSENSE_POSITION_MULTIPLE;
    }

    if (CY_CAPSENSE_POSITION_ONE == ptrWdCxt->wdTouch.numPosition)
    {
        ptrSnsTouch->x = (uint16_t)activeCol;
        ptrSnsTouch->y = (uint16_t)activeRow;
        ptrSnsTouch->z = 0u;
        ptrSnsTouch->id = (uint16_t)((activeRow * colNumber) + activeCol);
    }

    /* Update widget status if any activity is detected (even non-valid) */
    if ((0u != numActiveRows) || (0u != numActiveCols))
    {
        ptrWdCxt->status |= (uint8_t)CY_CAPSENSE_WD_ACTIVE_MASK;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsdTouchpad
****************************************************************************//**
*
* Processes status of the CSD Touchpad widget. This includes running
* Centroid algorithm and updating status in the Data Structure registers.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsdTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint32_t touchTh;
    uint32_t wdActiveCol = 0uL;
    uint32_t wdActiveRow = 0uL;
    uint8_t * ptrDebounceCnt = ptrWdConfig->ptrDebounceArr;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt = ptrWdConfig->ptrSnsContext;
    cy_stc_capsense_widget_context_t * ptrWdCxt = ptrWdConfig->ptrWdContext;
    uint32_t sensorNumber = ptrWdConfig->numSns;
    uint32_t colNumber = ptrWdConfig->numCols;
    uint32_t rowNumber = ptrWdConfig->numRows;
    cy_stc_capsense_position_t newPosition[CY_CAPSENSE_MAX_CENTROIDS];
    cy_stc_capsense_touch_t newTouch = {&newPosition[0u], CY_CAPSENSE_POSITION_NONE};
    cy_stc_capsense_ballistic_delta_t delta;

    /* Calculate touch threshold based on current sensor state */
    touchTh = (0u == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK)) ?
              ((uint32_t)ptrWdCxt->fingerTh + ptrWdCxt->hysteresis) :
              ((uint32_t)ptrWdCxt->fingerTh - ptrWdCxt->hysteresis);

    if (0u < (*ptrDebounceCnt))
    {
        /* Decrement debounce counter */
        (*ptrDebounceCnt)--;
    }

    /* Widget is considered as active if at least one sensor is active on both axes */
    for (snsIndex = colNumber; snsIndex-- > 0uL;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActiveCol |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }
    for (snsIndex = rowNumber; snsIndex-- > 0uL;)
    {
        ptrSnsCxt->status = (touchTh < ptrSnsCxt->diff) ? CY_CAPSENSE_SNS_TOUCH_STATUS_MASK : 0u;
        wdActiveRow |= ptrSnsCxt->status;
        ptrSnsCxt++;
    }

    /* No touch detected */
    if ((0uL == wdActiveCol) || (0uL == wdActiveRow))
    {
        /* Reset debounce counter */
        (*ptrDebounceCnt) = ptrWdCxt->onDebounce;
        ptrWdCxt->status &= (uint8_t)(~CY_CAPSENSE_WD_ACTIVE_MASK);
    }
    if (0u == (*ptrDebounceCnt))
    {
        /* New touch detected or touch still exists from previous cycle */
        ptrWdCxt->status |= CY_CAPSENSE_WD_ACTIVE_MASK;
    }
    else
    {
        if ((0uL != wdActiveCol) && (0uL != wdActiveRow))
        {
            /* Clear sensor state if activity was detected but debounce was not passed */
            ptrSnsCxt = ptrWdConfig->ptrSnsContext;
            for (snsIndex = sensorNumber; snsIndex-- > 0uL;)
            {
                ptrSnsCxt->status = 0u;
                ptrSnsCxt++;
            }
        }
    }

    /* If widget is still active after debounce run the centroid algorithm */
    if (CY_CAPSENSE_WD_ACTIVE_MASK == (ptrWdCxt->status & CY_CAPSENSE_WD_ACTIVE_MASK))
    {
        /* 3x3 CSD Touchpad processing */
        if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_3X3_MASK))
        {
            /* Centroid processing */
            Cy_CapSense_DpCentroidTouchpad(&newTouch, ptrWdConfig);
        }
        /* 5x5 Advanced CSD Touchpad processing */
        if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_5X5_MASK))
        {
            /* Centroid processing */
            Cy_CapSense_DpAdvancedCentroidTouchpad_Call(&newTouch, ptrWdConfig, context);
        }
    }

    /* Position filtering */
    if (0u != (ptrWdConfig->posFilterConfig & CY_CAPSENSE_POSITION_FILTERS_MASK))
    {
        Cy_CapSense_ProcessPositionFilters_Call(&newTouch, ptrWdConfig, context);
    }

    /* Copy positions into public structure */
    ptrWdCxt->wdTouch.numPosition = newTouch.numPosition;
    if (CY_CAPSENSE_POSITION_MULTIPLE != ptrWdCxt->wdTouch.numPosition)
    {
        for (snsIndex = 0u; snsIndex < newTouch.numPosition; snsIndex++)
        {
            ptrWdCxt->wdTouch.ptrPosition[snsIndex] = newTouch.ptrPosition[snsIndex];
        }
    }

    /* Ballistic Multiplier Filtering */
    if (0u != (ptrWdConfig->centroidConfig & CY_CAPSENSE_CENTROID_BALLISTIC_MASK))
    {
        Cy_CapSense_BallisticMultiplier_Lib_Call(&ptrWdConfig->ballisticConfig,
                                                 &ptrWdCxt->wdTouch,
                                                 &delta,
                                                 context->ptrCommonContext->timestamp,
                                                 ptrWdConfig->ptrBallisticContext,
                                                 context);
        ptrWdCxt->xDelta = delta.deltaX;
        ptrWdCxt->yDelta = delta.deltaY;
    }

}


/*******************************************************************************
* Function Name: Cy_CapSense_DpProcessCsxTouchpad
****************************************************************************//**
*
* Processes the status of the CSX Touchpad widget.
* The process involves running the 3x3 centroid algorithm with the
* tracking of finger id.
*
* \param ptrWdConfig
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_DpProcessCsxTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context)
{
    (void)context;
    /* Check whether sensors are active and located all local maxima */
    Cy_CapSense_DpFindLocalMaxDd(ptrWdConfig);
    /* Calculate centroid position for all found local maxima */
    Cy_CapSense_DpCalcTouchPadCentroid(ptrWdConfig);
    /* Identify all touches and assign them correct ID based on historical data */
    Cy_CapSense_DpTouchTracking(ptrWdConfig);
    /* Filter the position data and write it into data structure */
    Cy_CapSense_DpFilterTouchRecord(ptrWdConfig, context);
}


/*******************************************************************************
* Function Name: Cy_CapSense_RunMfsFiltering
****************************************************************************//**
*
* Selects the median difference signal when the multi-frequency scan is enabled.
*
* \param ptrSnsContext
* The pointer to the widget configuration structure.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_RunMfsFiltering(
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                const cy_stc_capsense_context_t * context)
{
    ptrSnsContext->diff = (uint16_t)Cy_CapSense_FtMedian((uint32_t)ptrSnsContext->diff,
                  (uint32_t)ptrSnsContext[CY_CAPSENSE_MFS_CH1_INDEX * context->ptrCommonConfig->numSns].diff,
                  (uint32_t)ptrSnsContext[CY_CAPSENSE_MFS_CH2_INDEX * context->ptrCommonConfig->numSns].diff);
}


#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
/******************************************************************************
* Function Name: Cy_CapSense_ConvertToUint16
******************************************************************************
*
* Converts input value to uint16 range
*
* \param sample
* The sample is a value of int32.
*
* \return
* Returns the sample converted to uint16(0,65535)
*
*****************************************************************************/
static uint16_t Cy_CapSense_ConvertToUint16(int32_t sample)
{
    if (0 > sample)
    {
        sample = 0;
    }

    if ((int32_t)UINT16_MAX < sample)
    {
        sample = (int32_t)UINT16_MAX;
    }

    return (uint16_t)(sample);
}


/*******************************************************************************
* Function Name: Cy_CapSense_ProcessWidgetMptxDeconvolution
****************************************************************************//**
*
* Performs raw count deconvolution for the specified CSX widget when
* Multi-phase Tx is enabled.
*
* This function decodes raw counts received after scanning into normal view by
* perfoming deconvolution algorithm. If the function is called for a widget with
* disabled Multi-phase Tx, the function returns CY_CAPSENSE_STATUS_BAD_DATA.
*
* No need to call this function from application layer since the
* Cy_CapSense_ProcessAllWidgets() and Cy_CapSense_ProcessWidget() functions calls
* deconvolution automatically.
*
* DAC auto-calibration when enabled performs sensor auto-calibration without
* performing deconvolution.
* The deconvolution algorithm for even number of TX electrodes decreases raw count
* level twice (keeping the signal on the same level).
*
* \param widgetId
* Specifies the ID number of the widget.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the processing operation.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_ProcessWidgetMptxDeconvolution(uint32_t widgetId, cy_stc_capsense_context_t * context)
{
    const cy_stc_capsense_widget_config_t * ptrWdCfg;
    uint32_t idx;
    uint32_t sumIdx;
    uint32_t ceIdx;
    uint32_t rotIdx;
    /* Order of multi-TX sequence */
    uint32_t mptxOrderLocal;
    int32_t localBuf[CY_CAPSENSE_MPTX_MAX_ORDER];
    int16_t deconvCoefRot[CY_CAPSENSE_MAX_TX_PATTERN_NUM * 2];
    int32_t accum;
    cy_capsense_status_t result = CY_CAPSENSE_STATUS_BAD_DATA;

    ptrWdCfg = &context->ptrWdConfig[widgetId];
    mptxOrderLocal = ptrWdCfg->mptxOrder;
    if (mptxOrderLocal >= CY_CAPSENSE_MPTX_MIN_ORDER)
    {
        result = CY_CAPSENSE_STATUS_SUCCESS;
        memcpy(&deconvCoefRot[0], (const void *)&ptrWdCfg->ptrMptxTable->deconvCoef[0u], mptxOrderLocal * 2);
        memcpy(&deconvCoefRot[mptxOrderLocal], (const void *)&ptrWdCfg->ptrMptxTable->deconvCoef[0u], mptxOrderLocal * 2);

        ceIdx = ptrWdCfg->numRows * ptrWdCfg->numCols;
        while (ceIdx >= mptxOrderLocal)
        {
            ceIdx -= mptxOrderLocal;
            /* Copy vector formed by RX[rxIdx] and TX[txIdx..txIdx+mptxOrderLocal]
            * from sensors to localBuf */
            idx = mptxOrderLocal;
            while (idx--)
            {
                localBuf[idx] = (int32_t)ptrWdCfg->ptrSnsContext[idx + ceIdx].raw;
            }

            /* Multiply vector stored in localBuf by the matrix of deconvolution coefficients. */
            idx = mptxOrderLocal;
            while (idx--)
            {
                accum = 0;
                rotIdx = idx + mptxOrderLocal - 1u;
                sumIdx = mptxOrderLocal;
                while (sumIdx--)
                {
                    accum += localBuf[sumIdx] * deconvCoefRot[rotIdx];
                    rotIdx--;
                }
                /* Shift the result in such a way that guarantees no overflow */
                accum >>= CY_CAPSENSE_SCALING_SHIFT;
                /* Convert the result to unsigned 16 bit and store in the target buffer */
                ptrWdCfg->ptrSnsContext[idx + ceIdx].raw = Cy_CapSense_ConvertToUint16(accum);
            }
        }
    }
    return result;
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessAllRaw
****************************************************************************//**
*
* This internal function executes the pre-processing of
* raw data for all widgets produced by the fifth CapSense HW generation.
* The pre-processing routine implements the following operations:
* - Executes the CIC2 post-processing if the filter mode is set to CIC2.
* - Executes the raw data inversion for the CSX sensors in CS-DMA scan mode.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessAllRaw(
        const cy_stc_capsense_context_t * context)
{
    uint32_t i;

    for(i = 0u; i < context->ptrCommonConfig->numWd; i++)
    {
        Cy_CapSense_PreProcessWidgetRaw(i, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessWidgetRaw
****************************************************************************//**
*
* This internal function executes the pre-processing
* of raw data for specified widgets produced by the fifth CapSense HW generation.
* The pre-processing routine implements the following operations:
* - Executes the CIC2 post-processing if the filter mode is set to CIC2.
* - Executes the raw data inversion for the CSX sensors in CS-DMA scan mode.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessWidgetRaw(
        uint32_t widgetId,
        const cy_stc_capsense_context_t * context)
{
    if(CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
    {
        Cy_CapSense_PreProcessWidgetCIC2Raw(widgetId, context);
    }

    if((CY_CAPSENSE_CSX_GROUP == context->ptrWdConfig[widgetId].senseGroup) &&
       (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode))
    {
        Cy_CapSense_InvertWidgetRaw(widgetId, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessSnsRaw
****************************************************************************//**
*
* This internal function executes the pre-processing
* of raw data for specified sensors produced by the fifth CapSense HW generation.
* The pre-processing routine implements the following operations:
* - Executes the CIC2 post-processing if the filter mode is set to CIC2.
* - Executes the raw data inversion for the CSX sensors in CS-DMA scan mode.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param sensorId
* The sensor ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessSnsRaw(
        uint32_t widgetId,
        uint32_t sensorId,
        const cy_stc_capsense_context_t * context)
{
    if(CY_CAPSENSE_CIC2_FILTER == context->ptrCommonConfig->cicFilterMode)
    {
        Cy_CapSense_PreProcessSnsCIC2Raw(widgetId, sensorId, context);
    }

    if((CY_CAPSENSE_CSX_GROUP == context->ptrWdConfig[widgetId].senseGroup) &&
       (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode))

    {
        Cy_CapSense_InvertSnsRaw(widgetId, sensorId, context);
    }

}


/*******************************************************************************
* Function Name: Cy_CapSense_InvertWidgetRaw
****************************************************************************//**
*
* This internal function executes the raw data inversion for specified
* widgets. Raw data is inverted relative to the theoretical MAX value.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InvertWidgetRaw(
        uint32_t widgetId,
        const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    ptrWdCfg = &context->ptrWdConfig[widgetId];

    for(snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
    {
        Cy_CapSense_InvertSnsRaw(widgetId, snsIndex, context);
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_InvertSnsRaw
****************************************************************************//**
*
* This internal function executes the raw data inversion for specified
* sensors. The raw data is inverted relative to the theoretical MAX value.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param sensorId
* The sensor ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_InvertSnsRaw(
        uint32_t widgetId,
        uint32_t sensorId,
        const cy_stc_capsense_context_t * context)
{
    uint32_t maxCount;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    ptrWdCfg = &context->ptrWdConfig[widgetId];
    ptrSnsCxt = &ptrWdCfg->ptrSnsContext[sensorId];

    maxCount = ptrWdCfg->ptrWdContext->maxRawCount;
    if(ptrSnsCxt->raw > maxCount)
    {
        ptrSnsCxt->raw = maxCount;
    }
    ptrSnsCxt->raw = maxCount - ptrSnsCxt->raw;
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessWidgetCIC2Raw
****************************************************************************//**
*
* This internal function executes the pre-processing
* of raw data, for specified widgets produced by the fifth CapSense HW generation with the
* filter mode set to CIC2.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessWidgetCIC2Raw(
        uint32_t widgetId,
        const cy_stc_capsense_context_t * context)
{
    uint32_t snsIndex;
    uint16_t cicRateSqr;
    uint8_t cic2HwDividerCol;
    uint8_t cic2SamplesCol;
    uint8_t cic2HwDividerRow;
    uint8_t cic2SamplesRow;
    uint8_t wdTwoDimensionCsd;

    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    ptrWdCfg = &context->ptrWdConfig[widgetId];

    if((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) &&
       ((CY_CAPSENSE_WD_MATRIX_BUTTON_E == ptrWdCfg->wdType) ||
        (CY_CAPSENSE_WD_TOUCHPAD_E == ptrWdCfg->wdType)))
    {
        wdTwoDimensionCsd = 1u;
    }
    else
    {
        wdTwoDimensionCsd = 0u;
    }

    cicRateSqr = ptrWdCfg->ptrWdContext->cicRate;
    cicRateSqr *= cicRateSqr;

    cic2SamplesCol = Cy_CapSense_GetCIC2SamplesNum(
                                     ptrWdCfg->numChopCycles,
                                     ptrWdCfg->ptrWdContext->numSubConversions,
                                     ptrWdCfg->ptrWdContext->snsClk,
                                     ptrWdCfg->ptrWdContext->cicRate);
    cic2HwDividerCol = Cy_CapSense_GetCIC2HwDivider(cic2SamplesCol);

    if(1u == wdTwoDimensionCsd)
    {
        cic2SamplesRow = Cy_CapSense_GetCIC2SamplesNum(
                                         ptrWdCfg->numChopCycles,
                                         ptrWdCfg->ptrWdContext->numSubConversions,
                                         ptrWdCfg->ptrWdContext->rowSnsClk,
                                         ptrWdCfg->ptrWdContext->cicRate);
        cic2HwDividerRow = Cy_CapSense_GetCIC2HwDivider(cic2SamplesRow);
    }

    ptrSnsCxt = &ptrWdCfg->ptrSnsContext[0u];
    for(snsIndex = 0u; snsIndex < ptrWdCfg->numSns; snsIndex++)
    {
        if((1u == wdTwoDimensionCsd) &&
           (ptrWdCfg->numCols <= snsIndex))
        {
            ptrSnsCxt->raw = Cy_CapSense_PreProcessCIC2Data(
                                             cic2SamplesRow,
                                             cic2HwDividerRow,
                                             cicRateSqr,
                                             context->ptrInternalContext->filterBitFormat,
                                             ptrSnsCxt->raw);
        }
        else
        {
            ptrSnsCxt->raw = Cy_CapSense_PreProcessCIC2Data(
                                             cic2SamplesCol,
                                             cic2HwDividerCol,
                                             cicRateSqr,
                                             context->ptrInternalContext->filterBitFormat,
                                             ptrSnsCxt->raw);

        }
        ptrSnsCxt++;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessSnsCIC2Raw
****************************************************************************//**
*
* This internal function executes the pre-processing
* of raw data for specified widgets produced by the fifth CapSense HW generation, with the
* filter mode set to CIC2.
*
* \param widgetId
* The widget ID, for which the post-processing should be executed.
*
* \param sensorId
* The sensor ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessSnsCIC2Raw(
        uint32_t widgetId,
        uint32_t sensorId,
        const cy_stc_capsense_context_t * context)
{

    uint32_t tmpVal;
    uint32_t cicRateSqr;
    uint32_t cic2HwDivider;
    uint32_t cic2SamplesNum;
    cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;

    ptrWdCfg = &context->ptrWdConfig[widgetId];
    ptrSnsCxt = &ptrWdCfg->ptrSnsContext[sensorId];

    cicRateSqr = ptrWdCfg->ptrWdContext->cicRate;
    cicRateSqr *= cicRateSqr;

    if ((CY_CAPSENSE_CSD_GROUP == context->ptrWdConfig[widgetId].senseGroup) &&
        (context->ptrWdConfig[widgetId].numCols <= sensorId))
    {
        tmpVal = context->ptrWdContext[widgetId].rowSnsClk;
    }
    else
    {
        tmpVal = context->ptrWdContext[widgetId].snsClk;
    }

    cic2SamplesNum = Cy_CapSense_GetCIC2SamplesNum(
                                     ptrWdCfg->numChopCycles,
                                     ptrWdCfg->ptrWdContext->numSubConversions,
                                     tmpVal,
                                     ptrWdCfg->ptrWdContext->cicRate);

    cic2HwDivider = Cy_CapSense_GetCIC2HwDivider(cic2SamplesNum);
    ptrSnsCxt->raw = Cy_CapSense_PreProcessCIC2Data(
                                     cic2SamplesNum,
                                     cic2HwDivider,
                                     cicRateSqr,
                                     context->ptrInternalContext->filterBitFormat,
                                     ptrSnsCxt->raw);
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessSlotCIC2Raw
****************************************************************************//**
*
* This internal function executes the post-processing of
* raw data for specified slots produced by the fifth CapSense HW genetation with the
* filter mode set to CIC2.
*
* \param slotId
* The slot ID, for which the post-processing should be executed.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
void Cy_CapSense_PreProcessSlotCIC2Raw(
        uint32_t slotId,
        const cy_stc_capsense_context_t * context)
{
    uint32_t chNumber;
    uint32_t curChIndex;
    uint32_t slotStcIndex;
    const cy_stc_capsense_scan_slot_t * slotPtr;

    chNumber = context->ptrCommonConfig->numChannels;
    for(curChIndex = 0u; curChIndex < chNumber; curChIndex++)
    {
        slotStcIndex = curChIndex * context->ptrCommonConfig->numSlots + slotId;
        slotPtr = &context->ptrScanSlots[slotStcIndex];
        if((slotPtr->wdId != CY_CAPSENSE_SLOT_SHIELD_ONLY) &&
           (slotPtr->wdId != CY_CAPSENSE_SLOT_TX_ONLY) &&
           (slotPtr->wdId != CY_CAPSENSE_SLOT_EMPTY))
        {
            Cy_CapSense_PreProcessSnsCIC2Raw(slotPtr->wdId, slotPtr->snsId, context);
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_GetCIC2SamplesNum
****************************************************************************//**
*
* This internal function determines the number CIC2 samples that will be
* accumulated for specified sensing parameters.
*
* \param convsNumber
* The number of conversions.
*
* \param subConvsNumber
* The number of sub-conversions.
*
* \param snsClkDivider
* The divider value for the sense clock.
*
* \param cicRate
* The filter decimation rate.
*
* \return
* The number of CIC2 samples.
*
*******************************************************************************/
uint32_t Cy_CapSense_GetCIC2SamplesNum(
            uint32_t convsNumber,
            uint32_t subConvsNumber,
            uint32_t snsClkDivider,
            uint32_t cicRate)
{
    uint32_t tmpVal;

    tmpVal = (snsClkDivider * subConvsNumber / cicRate) - 1u;
    tmpVal *= convsNumber;

    return(tmpVal);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GetCIC2HwDivider
****************************************************************************//**
*
* This internal function determines the value of the divider that will be
* applied to the data, accumulated by the CIC2 HW for the specified number of
* samples.
*
* \param cic2SamplesNum
* The number of CIC2 samples for the specified sensing parameters. This value
* can be obtained by using the Cy_CapSense_GetCIC2SamplesNum function.
*
* \return
* The CIC2 HW divider value.
*
*******************************************************************************/
uint32_t Cy_CapSense_GetCIC2HwDivider(
            uint32_t cic2SamplesNum)
{
    uint32_t tmpVal;

    if(cic2SamplesNum > 8u)
    {
        tmpVal = 16u;
    }
    else if(cic2SamplesNum > 4u)
    {
        tmpVal = 8u;
    }
    else if(cic2SamplesNum > 2u)
    {
        tmpVal = 4u;
    }
    else if(cic2SamplesNum > 1u)
    {
        tmpVal = 2u;
    }
    else
    {
        tmpVal = 1u;
    }

    return(tmpVal);
}


/*******************************************************************************
* Function Name: Cy_CapSense_PreProcessCIC2Data
****************************************************************************//**
*
* This internal function executes the post-processing of raw data
* produced by the fifth CapSense HW generation with the filter mode set
* to CIC2.
*
* \param cic2SamplesNum
* The number of CIC2 samples for specified sensing parameters. This value
* can be obtained by using the Cy_CapSense_GetCIC2SamplesNum function.
*
* \param cic2HwDivider
* The the value of the divider to be applied to the data accumulated by
* the CIC2 HW for the specified sensing parameters. This value can be obtained
* by using the Cy_CapSense_GetCIC2HwDivider function.
*
* \param cicRateSqr
* The squared value of the filter decimation rate.
*
* \param rawDataIn
* The input raw-data value.
*
* \param bitFormat
* Determines the CIC2 filtering mode. 0-unsigned mode. 1-signed mode.
*
* \return
* Final raw data, ready to be used by the sensor/widget processing routines.
*
*******************************************************************************/
uint32_t Cy_CapSense_PreProcessCIC2Data(
            uint32_t cic2SamplesNum,
            uint32_t cic2HwDivider,
            uint32_t cicRateSqr,
            uint32_t bitFormat,
            uint32_t rawDataIn)
{
    uint32_t tmpVal;

    tmpVal = rawDataIn;
    if(0u != bitFormat)
    {
        tmpVal = (tmpVal * cic2HwDivider) / cic2SamplesNum;
        tmpVal += (cicRateSqr);
    }
    else
    {
        tmpVal = (tmpVal * cic2HwDivider) / cic2SamplesNum;
    }

    return (tmpVal);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GetMaxRawCIC1
****************************************************************************//**
*
* This internal function determines the MAX raw count value that can be obtained
* from the fifth CapSense HW generation if the filter mode is set to CIC1.
*
* \param convsNumber
* The number of conversions.
*
* \param subConvsNumber
* The number of sub-conversions.
*
* \param snsClkDivider
* The divider value for the sense clock.
*
* \param numEpiCycles
* Number of ModClk cycles to be run during epilogue.
*
* \return
* Theoretical MAX raw count.
*
*******************************************************************************/
uint32_t Cy_CapSense_GetMaxRawCIC1(
            uint32_t convsNumber,
            uint32_t subConvsNumber,
            uint32_t snsClkDivider,
            uint32_t numEpiCycles)
{
    uint32_t maxRawTmp;

    maxRawTmp = (numEpiCycles * convsNumber - 1u);
    maxRawTmp += (subConvsNumber * convsNumber * snsClkDivider);
    maxRawTmp = (maxRawTmp > MSC_STATUS1_RAW_COUNT_Msk) ?
                 ((uint16_t)MSC_STATUS1_RAW_COUNT_Msk) : maxRawTmp;

    return (maxRawTmp);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GetMaxRawCIC2
****************************************************************************//**
*
* This internal function determines the MAX raw count value that can be obtained
* from the fifth CapSense HW generation if the filter mode is set to CIC2.
*
* \param convsNumber
* The number of conversions.
*
* \param subConvsNumber
* The number of sub-conversions.
*
* \param snsClkDivider
* The divider value for the sense clock.
*
* \param cicRate
* The filter decimation rate.
*
* \return
* Theoretical MAX raw count.
*
*******************************************************************************/
uint32_t Cy_CapSense_GetMaxRawCIC2(
            uint32_t convsNumber,
            uint32_t subConvsNumber,
            uint32_t snsClkDivider,
            uint32_t cicRate)
{
    uint32_t maxRawTmp;
    uint32_t cic2SamplesNum;
    uint32_t cic2HwDivider;

    cic2SamplesNum = Cy_CapSense_GetCIC2SamplesNum(convsNumber,
                                                   subConvsNumber,
                                                   snsClkDivider,
                                                   cicRate);
    cic2HwDivider = Cy_CapSense_GetCIC2HwDivider(cic2SamplesNum);
    maxRawTmp = (cicRate * cicRate * cic2SamplesNum) / cic2HwDivider;

    maxRawTmp = (maxRawTmp > MSC_STATUS1_RAW_COUNT_Msk) ?
                 ((uint16_t)MSC_STATUS1_RAW_COUNT_Msk) : maxRawTmp;

    return (maxRawTmp);
}

#endif /* CY_CAPSENSE_PLATFORM_BLOCK_MSCV3 */

#endif /* (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3)) */


/* [] END OF FILE */
