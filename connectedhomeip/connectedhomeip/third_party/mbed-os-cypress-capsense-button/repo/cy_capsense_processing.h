/***************************************************************************//**
* \file cy_capsense_processing.h
* \version 3.0
*
* \brief
* This file provides the function prototypes for the Data Processing module.
* The Data Processing module is responsible for the low level raw counts
* processing provided by the sensing module, maintaining baseline and
* difference values and performing high-level widget processing like updating
* button status or calculating slider position.
*
********************************************************************************
* \copyright
* Copyright 2018-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_PROCESSING_H)
#define CY_CAPSENSE_PROCESSING_H

#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_lib.h"

#if (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3))


#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_high_level *//** \{ */
/******************************************************************************/
uint32_t Cy_CapSense_DecodeWidgetGestures(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_low_level *//** \{ */
/******************************************************************************/
void Cy_CapSense_InitializeWidgetGestures(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitializeAllStatuses(const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitializeWidgetStatus(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
/** \} */

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/
void Cy_CapSense_DpProcessButton(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpProcessCsxTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpProcessProximity(
                cy_stc_capsense_widget_config_t const * ptrWdConfig);

void Cy_CapSense_DpProcessCsdTouchpad(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpProcessSlider(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_DpProcessCsdMatrix(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpProcessCsdWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_DpProcessCsdWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_DpProcessCsdSensorRawCountsExt(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                uint32_t mode,
                uint16_t * ptrBslnInvSns,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpProcessCsxWidgetStatus(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_DpProcessCsxWidgetRawCounts(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                const cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_DpProcessCsxSensorRawCountsExt(
                const cy_stc_capsense_widget_config_t * ptrWdConfig,
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                uint16_t * ptrSnsRawHistory,
                uint8_t * ptrSnsRawHistoryLow,
                uint32_t mode,
                uint16_t * ptrSnsBslnInv,
                const cy_stc_capsense_context_t * context);

void Cy_CapSense_DpUpdateDifferences(
                const cy_stc_capsense_widget_context_t * ptrWdContext,
                cy_stc_capsense_sensor_context_t * ptrSnsContext);

void Cy_CapSense_DpUpdateThresholds(
                cy_stc_capsense_widget_context_t * ptrWdContext,
                const cy_stc_capsense_smartsense_csd_noise_envelope_t * ptrNoiseEnvelope,
                uint32_t startFlag);

void Cy_CapSense_RunMfsFiltering(
                cy_stc_capsense_sensor_context_t * ptrSnsContext,
                const cy_stc_capsense_context_t * context);

#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
cy_capsense_status_t Cy_CapSense_ProcessWidgetMptxDeconvolution(
                uint32_t wdId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessAllRaw(
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessWidgetRaw(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessSnsRaw(
                uint32_t widgetId,
                uint32_t sensorId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InvertWidgetRaw(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InvertSnsRaw(
                uint32_t widgetId,
                uint32_t sensorId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessWidgetCIC2Raw(
                uint32_t widgetId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessSnsCIC2Raw(
                uint32_t widgetId,
                uint32_t sensorId,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_PreProcessSlotCIC2Raw(
                uint32_t slotId,
                const cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_GetCIC2SamplesNum(
                uint32_t convsNumber,
                uint32_t subConvsNumber,
                uint32_t snsClkDivider,
                uint32_t cicRate);
uint32_t Cy_CapSense_GetCIC2HwDivider(
                uint32_t cic2SamplesNum);
uint32_t Cy_CapSense_PreProcessCIC2Data(
                uint32_t cic2SamplesNum,
                uint32_t cic2HwDivider,
                uint32_t cicRateSqr,
                uint32_t bitFormat,
                uint32_t rawDataIn);
uint32_t Cy_CapSense_GetMaxRawCIC1(
                uint32_t convsNumber,
                uint32_t subConvsNumber,
                uint32_t snsClkDivider,
                uint32_t numEpiCycles);
uint32_t Cy_CapSense_GetMaxRawCIC2(
                uint32_t convsNumber,
                uint32_t subConvsNumber,
                uint32_t snsClkDivider,
                uint32_t cicRate);


#endif /* CY_CAPSENSE_PLATFORM_BLOCK_MSCV3 */
/** \} \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3)) */

#endif /* CY_CAPSENSE_PROCESSING_H */


/* [] END OF FILE */
