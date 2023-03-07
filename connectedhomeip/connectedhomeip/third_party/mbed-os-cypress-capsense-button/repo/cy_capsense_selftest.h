/***************************************************************************//**
* \file cy_capsense_selftest.h
* \version 3.0
*
* \brief
* This file provides the function prototypes of the BIST module.
*
********************************************************************************
* \copyright
* Copyright 2019-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_SELFTEST_H)
#define CY_CAPSENSE_SELFTEST_H

#include "cy_syslib.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"

#if (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2))

#if defined(__cplusplus)
extern "C" {
#endif

/* Sense Clock frequency in Hz for sensors capacitance measurement. */
#define CY_CAPSENSE_BIST_ELTD_CAP_SNSCLK_DEFAULT      (375000uL)
/* The default maximum possible external capacitor charge/discharge time in microseconds */
#define CY_CAPSENSE_BIST_CAP_SETTLING_TIME_DEFAULT    (2u)

/* The all available tests mask for the Cy_CapSense_RunSelfTest() function */
#define CY_CAPSENSE_BIST_RUN_AVAILABLE_SELF_TEST_MASK (CY_CAPSENSE_BIST_CRC_WDGT | \
                                                       CY_CAPSENSE_BIST_SNS_INTEGRITY | \
                                                       CY_CAPSENSE_BIST_SNS_CAP | \
                                                       CY_CAPSENSE_BIST_SHIELD_CAP | \
                                                       CY_CAPSENSE_BIST_EXTERNAL_CAP |\
                                                       CY_CAPSENSE_BIST_VDDA)


/*******************************************************************************
* CapSense BIST Enumerated Types
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_enums *//** \{ */
/******************************************************************************/

/** Defines IDs of the external capacitors */
typedef enum
{
    CY_CAPSENSE_BIST_CMOD_ID_E          = 0x00u,                /**< ID for Cmod external capacitor */
    CY_CAPSENSE_BIST_CINTA_ID_E         = 0x01u,                /**< ID for CintA external capacitor */
    CY_CAPSENSE_BIST_CINTB_ID_E         = 0x02u,                /**< ID for CintB external capacitor */
    CY_CAPSENSE_BIST_CSH_ID_E           = 0x03u,                /**< ID for Csh external capacitor */
} cy_en_capsense_bist_external_cap_id_t;

/** \} */


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_high_level *//** \{ */
/******************************************************************************/

cy_en_capsense_bist_status_t Cy_CapSense_RunSelfTest(
                uint32_t testEnMask,
                cy_stc_capsense_context_t * context);

/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_low_level *//** \{ */
/******************************************************************************/

cy_en_capsense_bist_status_t Cy_CapSense_CheckCRCWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_CheckIntegritySensorRawcount(
                uint32_t widgetId,
                uint32_t sensorId,
                uint16_t rawcountHighLimit,
                uint16_t rawcountLowLimit,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_CheckIntegritySensorBaseline(
                uint32_t widgetId,
                uint32_t sensorId,
                uint16_t baselineHighLimit,
                uint16_t baselineLowLimit,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_CheckIntegritySensorPins(
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_MeasureCapacitanceSensor(
                uint32_t widgetId,
                uint32_t sensorId,
                uint32_t * ptrValue,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_MeasureCapacitanceShield(
                uint32_t * ptrValue,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_MeasureCapacitanceCap(
                cy_en_capsense_bist_external_cap_id_t integrationCapId,
                uint32_t * ptrValue,
                uint32_t maxCapacitance,
                cy_stc_capsense_context_t * context);
cy_en_capsense_bist_status_t Cy_CapSense_MeasureVdda(
                uint32_t * ptrValue,
                cy_stc_capsense_context_t * context);

/** \} */

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

cy_en_capsense_bist_status_t Cy_CapSense_MeasureCapacitanceSensorExt(
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_bist_custom_parameters_t * ptrScanConfig,
                uint32_t * ptrValue,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_BistInitialize(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_BistDisableMode(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_BistDsInitialize(
                cy_stc_capsense_context_t * context);
void Cy_CapSense_UpdateCrcWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);


/** \} \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2)) */

#endif /* CY_CAPSENSE_SELFTEST_H */


/* [] END OF FILE */
