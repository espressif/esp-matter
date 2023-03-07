/***************************************************************************//**
* \file cy_capsense_sensing_v3.h
* \version 3.0
*
* \brief
* This file provides the function prototypes specific to the scanning module.
*
********************************************************************************
* \copyright
* Copyright 2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_SENSING_V3_H)
#define CY_CAPSENSE_SENSING_V3_H

#include "cy_syslib.h"
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
    #include "cy_msc.h"
#endif

#if (defined(CY_IP_M0S8MSCV3))

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************/
/** \addtogroup group_capsense_high_level *//** \{ */
/******************************************************************************/

cy_capsense_status_t Cy_CapSense_ScanAllSlots(
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_ScanSlots(
                uint32_t startSlotId,
                uint32_t numberSlots,
                cy_stc_capsense_context_t * context);

cy_capsense_status_t Cy_CapSense_ScanAllWidgets(
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_ScanWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_ScanSensor(
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context);

uint32_t Cy_CapSense_IsBusy(
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InterruptHandler(
                const MSC_Type * base,
                cy_stc_capsense_context_t * context);

/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_low_level *//** \{ */
/******************************************************************************/

cy_capsense_status_t Cy_CapSense_CalibrateAllSlots(
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_CalibrateAllWidgets(
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_CalibrateWidget(
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_Capsense_SlotPinState(
                uint32_t slotId,
                const cy_stc_capsense_electrode_config_t * ptrEltdCfg,
                uint32_t pinState,
                cy_stc_capsense_context_t * context);
/** \} */

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal *//** \{ */
/******************************************************************************/

void Cy_CapSense_SetBusyFlags(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_ClrBusyFlags(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_SsInitialize(
                cy_stc_capsense_context_t * context);
uint32_t Cy_CapSense_WatchdogCyclesNum(
                uint32_t desiredTimeUs,
                uint32_t cpuFreqMHz,
                uint32_t cyclesPerLoop);
cy_capsense_status_t Cy_CapSense_ConfigureAnalogMuxResource(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_InitializeDmaResource(
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_ConfigureDmaResource(
                uint32_t mscChIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_ScanISR(void * capsenseContext);
cy_capsense_status_t Cy_CapSense_SwitchSensingMethod(
                uint8_t mode,
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_SetCmodInDefaultState(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_SetCmodInAmuxModeState(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_InitActivePtrSns(
                uint32_t chIndex,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_InitActivePtrWd(
                uint32_t chIndex,
                uint32_t widgetId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_InitActivePtr(
                uint32_t chIndex,
                uint32_t widgetId,
                uint32_t sensorId,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_SetIOsInDesiredState(
                uint32_t desiredDriveMode,
                uint32_t desiredPinOutput,
                en_hsiom_sel_t desiredHsiom,
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_SetIOsInDefaultState(
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_SsConfigPinRegisters(
                GPIO_PRT_Type * base,
                uint32_t pinNum,
                uint32_t dm,
                en_hsiom_sel_t hsiom);
void Cy_CapSense_ConnectSensor(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_DisconnectSensor(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_CsdConnectSns(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CsdDisconnectSns(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CsxConnectRx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CsxConnectTx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CsxDisconnectRx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_CsxDisconnectTx(
                uint32_t chIndex,
                const cy_stc_capsense_context_t * context);
void Cy_CapSense_EnableShieldElectrodes(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_DisableShieldElectrodes(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_CalibrateSlot(
                uint32_t scanSlotId,
                uint32_t csdTarget,
                uint32_t csxTarget,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_NormalizeCdac(
                uint32_t wdId,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_VerifyCalibration(
                uint32_t wdId,
                uint32_t csdTarget,
                uint32_t csxTarget,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_SetCdacs(
                uint32_t cdac,
                uint32_t scanSlotId,
                uint32_t chId,
                cy_stc_capsense_context_t * context);
cy_capsense_status_t Cy_CapSense_WaitEndScan(
                uint32_t timeout,
                cy_stc_capsense_context_t * context);
void Cy_CapSense_SetShieldPinsInDefaultState(
                const cy_stc_capsense_context_t * context);


/** \} \endcond */


/*******************************************************************************
* Local definition
*******************************************************************************/

#define CY_CAPSENSE_MSC_INTR_ALL_MSK                            (MSC_INTR_MASK_SUB_SAMPLE_Msk |\
                                                                 MSC_INTR_MASK_SAMPLE_Msk |\
                                                                 MSC_INTR_MASK_SCAN_Msk |\
                                                                 MSC_INTR_MASK_INIT_Msk |\
                                                                 MSC_INTR_MASK_FRAME_Msk |\
                                                                 MSC_INTR_MASK_CIC2_ERROR_Msk |\
                                                                 MSC_INTR_MASK_CONFIG_REQ_Msk |\
                                                                 MSC_INTR_MASK_FIFO_UNDERFLOW_Msk |\
                                                                 MSC_INTR_MASK_FIFO_OVERFLOW_Msk)


#define CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_CSD_SHIELD_VALUE    (0x00000040uL)
#define CY_CAPSENSE_FW_SHIELD_PASSIVE_AMUX_REG_SW_CSD_SHIELD_VALUE   (0x00000050uL)

#define CY_CAPSENSE_CALIBRATION_TIMEOUT                         (1000000uL)
#define CY_CAPSENSE_MAX_CH_NUM                                  (4u)
#define CY_CAPSENSE_CDAC_BITS_USED                              (8u)
#define CY_CAPSENSE_CAL_MIDDLE_VALUE                            (1u << (CY_CAPSENSE_CDAC_BITS_USED - 1u))

#define CY_CAPSENSE_CMOD_AMUX_MSK                               (MSC_SW_SEL_GPIO_SW_CSD_SENSE_Msk |\
                                                                 MSC_SW_SEL_GPIO_SW_CSD_MUTUAL_Msk |\
                                                                 MSC_SW_SEL_GPIO_SW_CSD_POLARITY_Msk |\
                                                                 MSC_SW_SEL_GPIO_SW_DSI_CMOD_Msk |\
                                                                 MSC_SW_SEL_GPIO_SW_DSI_CSH_TANK_Msk)

#define CY_CAPSENSE_FW_CMOD_AMUX_CSD_REG_SW_SEL_GPIO_VALUE      ((4u << MSC_SW_SEL_GPIO_SW_CSD_SENSE_Pos) |\
                                                                 (2u << MSC_SW_SEL_GPIO_SW_CSD_POLARITY_Pos) |\
                                                                 (3u << MSC_SW_SEL_GPIO_SW_DSI_CMOD_Pos) |\
                                                                 (3u << MSC_SW_SEL_GPIO_SW_DSI_CSH_TANK_Pos))

#define CY_CAPSENSE_FW_CMOD_AMUX_CSX_REG_SW_SEL_GPIO_VALUE      ((2u << MSC_SW_SEL_GPIO_SW_CSD_SENSE_Pos) |\
                                                                 (1u << MSC_SW_SEL_GPIO_SW_CSD_MUTUAL_Pos))

/*******************************************************************************
* Constant definition
*******************************************************************************/
/*
 *  Definition of the default configuration of the MSC HW registers that is
 *  intended to be used on the MSC HW block capturing stage.
 *  The configuration includes:
 *  1. Start of the analog settling process:
 *      - Enables the MSC HW block;
 *      - Enables all the sub-blocks of the MSC HW block;
 *      - Enables the Sense Modulator output;
 *  2. Clear all of the pending interrupt requests of the MSC HW block;
 *  3. Sets into default state the rest of the CSD HW block registers which are not related
 *     to actions #1 and #2.
*/

#define CY_CAPSENSE_MSC_CONFIG_DEFAULT                      \
    {                                                       \
        .ctl                = MSC_CTL_SENSE_EN_Msk |        \
                              MSC_CTL_MSCCMP_EN_Msk |       \
                              MSC_CTL_ENABLED_Msk,          \
        .spare              = 0x00u,                        \
        .scanCtl1           = 0x00u,                        \
        .scanCtl2           = 0x00u,                        \
        .initCtl1           = 0x00u,                        \
        .initCtl2           = 0x00u,                        \
        .initCtl3           = 0x00u,                        \
        .initCtl4           = 0x00u,                        \
        .senseDutyCtl       = 0x00u,                        \
        .sensePeriodCtl     = 0x00u,                        \
        .filterCtl          = 0x00u,                        \
        .ccompCdacCtl       = 0x00u,                        \
        .ditherCdacCtl      = 0x00u,                        \
        .cswCtl             = 0x00u,                        \
        .swSelGpio          = 0x00u,                        \
        .swSelCdacRe        = 0x00u,                        \
        .swSelCdacCo        = 0x00u,                        \
        .swSelCdacCf        = 0x00u,                        \
        .swSelCmod1         = 0x00u,                        \
        .swSelCmod2         = 0x00u,                        \
        .swSelCmod3         = 0x00u,                        \
        .swSelCmod4         = 0x00u,                        \
        .obsCtl             = 0x00u,                        \
        .status1            = 0x00u,                        \
        .status2            = 0x00u,                        \
        .status3            = 0x00u,                        \
        .resultFifoStatus   = 0x00u,                        \
        .resultFifoRd       = 0x00u,                        \
        .intr               = 0x00u,                        \
        .intrSet            = 0x00u,                        \
        .intrMask           = 0x00u,                        \
        .intrMasked         = 0x00u,                        \
        .frameCmd           = 0x00u,                        \
        {                                                   \
            .snsSwSelCswMask2   = 0x00u,                    \
            .snsSwSelCswMask1   = 0x00u,                    \
            .snsSwSelCswMask0   = 0x00u,                    \
            .snsScanCtl         = 0x00u,                    \
            .snsCdacCtl         = 0x00u,                    \
            .snsCtl             = 0x00u,                    \
        },                                                  \
        .swSelCsw[0u]       = 0x00,                         \
        .swSelCsw[1u]       = 0x00,                         \
        .swSelCsw[2u]       = 0x00,                         \
        .swSelCsw[3u]       = 0x00,                         \
        .swSelCsw[4u]       = 0x00,                         \
        .swSelCsw[5u]       = 0x00,                         \
        .swSelCsw[6u]       = 0x00,                         \
        .swSelCsw[7u]       = 0x00,                         \
        .swSelCsw[8u]       = 0x00,                         \
        .swSelCsw[9u]       = 0x00,                         \
        .swSelCsw[10u]      = 0x00,                         \
        .swSelCsw[11u]      = 0x00,                         \
        .swSelCsw[12u]      = 0x00,                         \
        .swSelCsw[13u]      = 0x00,                         \
        .swSelCsw[14u]      = 0x00,                         \
        .swSelCsw[15u]      = 0x00,                         \
        .swSelCsw[16u]      = 0x00,                         \
        .swSelCsw[17u]      = 0x00,                         \
        .swSelCsw[18u]      = 0x00,                         \
        .swSelCsw[19u]      = 0x00,                         \
        .swSelCsw[20u]      = 0x00,                         \
        .swSelCsw[21u]      = 0x00,                         \
        .swSelCsw[22u]      = 0x00,                         \
        .swSelCsw[23u]      = 0x00,                         \
        .swSelCsw[24u]      = 0x00,                         \
        .swSelCsw[25u]      = 0x00,                         \
        .swSelCsw[26u]      = 0x00,                         \
        .swSelCsw[27u]      = 0x00,                         \
        .swSelCsw[28u]      = 0x00,                         \
        .swSelCsw[29u]      = 0x00,                         \
        .swSelCsw[30u]      = 0x00,                         \
        .swSelCsw[31u]      = 0x00,                         \
        .swSelCswFunc[0u]   = 0x00,                         \
        .swSelCswFunc[1u]   = 0x00,                         \
        .swSelCswFunc[2u]   = 0x00,                         \
        .swSelCswFunc[3u]   = 0x00,                         \
        .swSelCswFunc[4u]   = 0x00,                         \
        .swSelCswFunc[5u]   = 0x00,                         \
        .swSelCswFunc[6u]   = 0x00,                         \
        .swSelCswFunc[7u]   = 0x00,                         \
        .mode[0u] =                                         \
        {                                                   \
            .senseDutyCtl   = 0x00,                         \
            .swSelCdacFl    = 0x00,                         \
            .swSelTop       = 0x00,                         \
            .swSelComp      = 0x00,                         \
            .swSelSh        = 0x00,                         \
        },                                                  \
        .mode[1u] =                                         \
        {                                                   \
            .senseDutyCtl   = 0x00,                         \
            .swSelCdacFl    = 0x00,                         \
            .swSelTop       = 0x00,                         \
            .swSelComp      = 0x00,                         \
            .swSelSh        = 0x00,                         \
        },                                                  \
        .mode[2u] =                                         \
        {                                                   \
            .senseDutyCtl   = 0x00,                         \
            .swSelCdacFl    = 0x00,                         \
            .swSelTop       = 0x00,                         \
            .swSelComp      = 0x00,                         \
            .swSelSh        = 0x00,                         \
        },                                                  \
        .mode[3u] =                                         \
        {                                                   \
            .senseDutyCtl   = 0x00,                         \
            .swSelCdacFl    = 0x00,                         \
            .swSelTop       = 0x00,                         \
            .swSelComp      = 0x00,                         \
            .swSelSh        = 0x00,                         \
        },                                                  \
    }

extern const cy_stc_msc_base_config_t cy_capsense_mscCfg;


#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_M0S8MSCV3 */

#endif /* CY_CAPSENSE_SENSING_V3_H */


/* [] END OF FILE */
