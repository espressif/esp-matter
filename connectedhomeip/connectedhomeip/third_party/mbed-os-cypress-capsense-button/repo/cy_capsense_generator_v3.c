/***************************************************************************//**
* \file cy_capsense_generator_v3.c
* \version 3.0
*
* \brief
* This file contains the source of functions common for register map
* generator module.
*
********************************************************************************
* \copyright
* Copyright 2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include <string.h>
#include "cy_capsense_common.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_generator_v3.h"
#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
    #include "cy_msc.h"
#endif

#if (defined(CY_IP_M0S8MSCV3))

/*******************************************************************************
* Local definition
*******************************************************************************/
#define CY_CAPSENSE_LFSR_BITS_5B                  (3u)

#define CY_CAPSENSE_MSC0_CMOD1PADD_PIN            (2u)
#define CY_CAPSENSE_MSC1_CMOD1PADD_PIN            (1u)

#define CY_CAPSENSE_CMOD12_PAIR_SELECTION         (0u)
#define CY_CAPSENSE_CMOD34_PAIR_SELECTION         (1u)


/*******************************************************************************
* Constants
*******************************************************************************/
const cy_stc_msc_base_config_t cy_capsense_smTemplate = CY_CAPSENSE_SENSING_METHOD_BASE_TEMPLATE;


/*******************************************************************************
* Internal function prototypes
*******************************************************************************/
static void Cy_CapSense_CalculateMaskRegisters(
                uint32_t mask,
                uint32_t funcState,
                uint32_t * ptrCfg);

static void Cy_CapSense_GenerateCtrlMuxSwControl(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context);

static void Cy_CapSense_ApplyShieldConfig(
                cy_stc_msc_mode_config_t * ptrBaseCfgMode,
                cy_stc_capsense_context_t * context);

static void Cy_CapSense_ConfigSensorClock(
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context);

cy_capsense_status_t Cy_CapSense_GenerateSensorConfigValid(
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context);


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateBaseConfig
****************************************************************************//**
*
* Generates the configuration for all registers that have to be configured
* one-time to initialize the MSC block.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_GenerateBaseConfig(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    uint32_t i;
    uint32_t snsMethod;
    uint32_t idCounter;
    uint8_t * ptrMapping;
    cy_stc_msc_mode_config_t * ptrBaseCfgMode;
    uint32_t cdacDitherEnabled;
    uint32_t snsMethodInternal;
    uint32_t swTemplateIndex;

    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_SUCCESS;

    cy_stc_msc_base_config_t * ptrBaseCfg;
    const cy_stc_msc_base_config_t * ptrTemplateCfg;
    cy_stc_msc_channel_config_t * ptrMscChConfig = &context->ptrCommonConfig->ptrMscChConfig[chIndex];

    /*
     * BASE CONFIGURATION
     */
    ptrBaseCfg = &context->ptrBaseFrameContext[chIndex];
    ptrTemplateCfg = &cy_capsense_smTemplate;

    /* Copies template of a base config */
    ptrBaseCfg->ctl = ptrTemplateCfg->ctl;
    if(context->ptrCommonContext->modClk > 1u)
    {
        ptrBaseCfg->ctl &= (~MSC_CTL_CLK_MSC_RATIO_Msk);
    }
    else
    {
        ptrBaseCfg->ctl |= MSC_CTL_CLK_MSC_RATIO_Msk;
    }

    ptrBaseCfg->spare = ptrTemplateCfg->spare;
    ptrBaseCfg->scanCtl1 = ptrTemplateCfg->scanCtl1;
    ptrBaseCfg->scanCtl2 = ptrTemplateCfg->scanCtl2;
    ptrBaseCfg->initCtl1 = ptrTemplateCfg->initCtl1;
    ptrBaseCfg->initCtl2 = ptrTemplateCfg->initCtl2;
    ptrBaseCfg->initCtl3 = ptrTemplateCfg->initCtl3;
    ptrBaseCfg->initCtl4 = ptrTemplateCfg->initCtl4;
    ptrBaseCfg->senseDutyCtl = ptrTemplateCfg->senseDutyCtl;
    ptrBaseCfg->sensePeriodCtl = ptrTemplateCfg->sensePeriodCtl;
    ptrBaseCfg->filterCtl = ptrTemplateCfg->filterCtl;
    ptrBaseCfg->ccompCdacCtl = ptrTemplateCfg->ccompCdacCtl;
    ptrBaseCfg->ditherCdacCtl = ptrTemplateCfg->ditherCdacCtl;
    ptrBaseCfg->cswCtl = ptrTemplateCfg->cswCtl;
    ptrBaseCfg->swSelGpio = ptrTemplateCfg->swSelGpio;
    ptrBaseCfg->swSelCdacRe = ptrTemplateCfg->swSelCdacRe;
    ptrBaseCfg->swSelCdacCo = ptrTemplateCfg->swSelCdacCo;
    ptrBaseCfg->swSelCdacCf = ptrTemplateCfg->swSelCdacCf;
    ptrBaseCfg->swSelCmod1 = ptrTemplateCfg->swSelCmod1;
    ptrBaseCfg->swSelCmod2 = ptrTemplateCfg->swSelCmod2;
    ptrBaseCfg->swSelCmod3 = ptrTemplateCfg->swSelCmod3;
    ptrBaseCfg->swSelCmod4 = ptrTemplateCfg->swSelCmod4;

    if((CY_CAPSENSE_MSC0_CMOD1PADD_PIN != ptrMscChConfig->pinCmod1) &&
       (CY_CAPSENSE_MSC1_CMOD1PADD_PIN != ptrMscChConfig->pinCmod1))
    {
        ptrBaseCfg->swSelCmod1 = 0u;
        ptrBaseCfg->swSelCmod2 = 0u;
    }
    else
    {
        ptrBaseCfg->swSelCmod3 = 0u;
        ptrBaseCfg->swSelCmod4 = 0u;
    }

    ptrBaseCfg->obsCtl = ptrTemplateCfg->obsCtl;
    ptrBaseCfg->intr = ptrTemplateCfg->intr;
    ptrBaseCfg->intrSet = ptrTemplateCfg->intrSet;
    ptrBaseCfg->intrMask = ptrTemplateCfg->intrMask;

    /* Set the scanning mode */
    ptrBaseCfg->ctl |= ((uint32_t)context->ptrCommonConfig->scanningMode) << MSC_CTL_OPERATING_MODE_Pos;

    /* Frame is not started */
    ptrBaseCfg->frameCmd = 0x00u;

    /* Clear Structures */
    (void)memset(&ptrBaseCfg->sensorConfig, 0, sizeof(cy_stc_msc_mode_config_t));
    (void)memset(&ptrBaseCfg->swSelCsw[0u], 0, CY_MSC_CSW_NUM * CY_CAPSENSE_BYTE_IN_32_BIT);
    (void)memset(&ptrBaseCfg->swSelCswFunc[0u], 0, CY_MSC_CSW_FUNC_NUM * CY_CAPSENSE_BYTE_IN_32_BIT);
    (void)memset(&ptrBaseCfg->mode, 0, sizeof(cy_stc_msc_mode_config_t) * CY_CAPSENSE_BYTE_IN_32_BIT);

    /*
     * MODE CONFIGURATION
     */
    idCounter = 0u;
    ptrMapping = &context->ptrInternalContext->mapSenseMethod[0u];
    for (i = 0u; i < CY_CAPSENSE_REG_MODE_NUMBER; i++)
    {
        ptrMapping[i] = CY_CAPSENSE_REG_MODE_UNDEFINED;
    }
    if (0u == context->ptrCommonConfig->numWd)
    {
        capStatus |= CY_CAPSENSE_STATUS_BAD_CONFIG;
    }
    /* For each widget */
    for (i = 0u; i < context->ptrCommonConfig->numWd; i++)
    {
        snsMethod = context->ptrWdConfig[i].senseMethod;
        swTemplateIndex = (CY_CAPSENSE_CSD_RM_SENSING_METHOD == snsMethod) ? CY_CAPSENSE_CSD_RM_SENSING_METHOD_INDEX :
                                                                             CY_CAPSENSE_CSX_RM_SENSING_METHOD_INDEX;
        cdacDitherEnabled = context->ptrWdContext[i].cdacDitherEn;

        /* Calculate REG_MODE value */
        snsMethodInternal = snsMethod - 1u;

        if (CY_CAPSENSE_ENABLE == cdacDitherEnabled)
        {
            /* Shift sense mode to cdac dither enabled modes */
            snsMethodInternal += CY_CAPSENSE_REG_MODE_BASE_NUMBER;
        }

        /* If a method is a new method then register the method */
        if (ptrMapping[snsMethodInternal] == CY_CAPSENSE_REG_MODE_UNDEFINED)
        {
            /* Checks for overflow */
            if (CY_MSC_MODES_NUM <= idCounter)
            {
                capStatus |= CY_CAPSENSE_STATUS_CONFIG_OVERFLOW;
                break;
            }
            else
            {
                ptrMapping[snsMethodInternal] = (uint8_t)idCounter;
                ptrBaseCfg->mode[idCounter] = ptrTemplateCfg->mode[swTemplateIndex];
                ptrBaseCfg->mode[idCounter].swSelCdacFl = 0u;
                if (CY_CAPSENSE_ENABLE == cdacDitherEnabled)
                {
                    if (CY_CAPSENSE_REG_MODE_CSD_RM_DITHERING == snsMethodInternal)
                    {
                        ptrBaseCfg->mode[idCounter].swSelCdacFl = CY_CAPSENSE_CSD_FW_CAPDAC_DITHERING_REG_SW_SEL_CDAC_FL_VALUE;
                    }
                    else
                    {
                        ptrBaseCfg->mode[idCounter].swSelCdacFl = CY_CAPSENSE_CSX_FW_CAPDAC_DITHERING_REG_SW_SEL_CDAC_FL_VALUE;
                    }
                }

                ptrBaseCfg->mode[idCounter].swSelComp &= (~MSC_MODE_SW_SEL_COMP_CPCS1_Msk);
                ptrBaseCfg->mode[idCounter].swSelComp &= (~MSC_MODE_SW_SEL_COMP_CMCS2_Msk);
                ptrBaseCfg->mode[idCounter].swSelComp &= (~MSC_MODE_SW_SEL_COMP_CPCS3_Msk);
                ptrBaseCfg->mode[idCounter].swSelComp &= (~MSC_MODE_SW_SEL_COMP_CMCS4_Msk);
                ptrBaseCfg->mode[idCounter].swSelSh &= (~MSC_MODE_SW_SEL_SH_C1SHG_Msk);
                ptrBaseCfg->mode[idCounter].swSelSh &= (~MSC_MODE_SW_SEL_SH_C3SHG_Msk);

                if((CY_CAPSENSE_MSC0_CMOD1PADD_PIN == ptrMscChConfig->pinCmod1) ||
                   (CY_CAPSENSE_MSC1_CMOD1PADD_PIN == ptrMscChConfig->pinCmod1))
                {
                    ptrBaseCfg->mode[idCounter].swSelComp |= MSC_MODE_SW_SEL_COMP_CPCS1_Msk;
                    ptrBaseCfg->mode[idCounter].swSelComp |= MSC_MODE_SW_SEL_COMP_CMCS2_Msk;
                    ptrBaseCfg->mode[idCounter].swSelSh |= MSC_MODE_SW_SEL_SH_C1SHG_Msk;
                }
                else
                {
                    ptrBaseCfg->mode[idCounter].swSelComp |= MSC_MODE_SW_SEL_COMP_CPCS3_Msk;
                    ptrBaseCfg->mode[idCounter].swSelComp |= MSC_MODE_SW_SEL_COMP_CMCS4_Msk;
                    ptrBaseCfg->mode[idCounter].swSelSh |= MSC_MODE_SW_SEL_SH_C3SHG_Msk;
                }

                if(CY_CAPSENSE_CSX_RM_SENSING_METHOD == snsMethod)
                {
                    if(CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
                    {
                        ptrBaseCfg->mode[idCounter].swSelTop = CY_CAPSENSE_CSX_FW_AMUX_MODE_SW_SEL_TOP_VALUE;
                    }
                    else
                    {
                        ptrBaseCfg->mode[idCounter].swSelTop = CY_CAPSENSE_CSX_FW_CTLMUX_MODE_SW_SEL_TOP_VALUE;
                    }
                }

                idCounter++;
            }
        }
    }
    context->ptrInternalContext->numSenseMethod = idCounter;

    /*
     * PINS CONFIGURATION
     */
    idCounter = 0u;
    ptrMapping = &context->ptrInternalContext->mapPinState[0u];
    for (i = 0u; i < CY_CAPSENSE_PIN_STATE_NUMBER; i++)
    {
        ptrMapping[i] = 0xFFu;
    }

    if (0u != context->ptrCommonConfig->csxEn)
    {
        /* CY_CAPSENSE_CTRLMUX_STATE_RX */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_RX] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW0_RX_INDEX];
        idCounter++;
        /* CY_CAPSENSE_CTRLMUX_STATE_TX */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_TX] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW1_TX_INDEX];
        idCounter++;
        /* CY_CAPSENSE_CTRLMUX_STATE_GND */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW4_GND_INDEX];
        idCounter++;
        /* CY_CAPSENSE_CTRLMUX_STATE_TX_NEGATIVE */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_TX_NEGATIVE] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW2_NEG_TX_INDEX];
        idCounter++;
    }

    if (0u != context->ptrCommonConfig->csdEn)
    {
        /* CY_CAPSENSE_CTRLMUX_STATE_SNS */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SNS] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW3_SNS_INDEX];
        idCounter++;
        /* CY_CAPSENSE_CTRLMUX_STATE_HIGH_Z */
        ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_HIGH_Z] = idCounter;
        ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW5_HIGH_Z_INDEX];
        idCounter++;
        /* CY_CAPSENSE_SNS_CONNECTION_GROUND */
        if (ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND] == 0xFF)
        {
            ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND] = idCounter;
            ptrBaseCfg->swSelCswFunc[idCounter] = ptrTemplateCfg->swSelCsw[CY_CAPSENSE_SM_REG_SW_SEL_CSW4_GND_INDEX];
            idCounter++;
        }
        /* CY_CAPSENSE_CTRLMUX_STATE_SHIELD */
        if (CY_CAPSENSE_SHIELD_PASSIVE == context->ptrCommonConfig->csdShieldMode)
        {
            ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SHIELD] = idCounter;
            ptrBaseCfg->swSelCswFunc[idCounter] = CY_CAPSENSE_FW_SHIELD_PASSIVE_CTRLMUX_REG_SW_SEL_CSW_VALUE;
            idCounter++;
        }
        else
        {
            ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SHIELD] = idCounter;
            ptrBaseCfg->swSelCswFunc[idCounter] = CY_CAPSENSE_FW_SHIELD_ACTIVE_CTRLMUX_REG_SW_SEL_CSW_VALUE;
            idCounter++;
        }

    }
    if (0u == idCounter)
    {
        capStatus |= CY_CAPSENSE_STATUS_BAD_CONFIG;
    }
    else
    {
        context->ptrInternalContext->numFunc = idCounter;
        context->ptrInternalContext->snsConfigSize = CY_MSC_5_SNS_REGS;
        if (CY_CAPSENSE_PIN_STATE_FUNC_LIMIT < idCounter)
        {
            context->ptrInternalContext->snsConfigSize = CY_MSC_6_SNS_REGS;
        }
    }

    /*
     * CONFIGURATION FROM CAPSENSE DATA STRUCTURE
     */

    /* Initialize interrupts for all enabled MSC channels */
    if (CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN == context->ptrCommonConfig->scanningMode)
    {
        ptrBaseCfg->intrMask |= MSC_INTR_MASK_FRAME_Msk;
    }
    else
    {
        ptrBaseCfg->intrMask |= MSC_INTR_MASK_SCAN_Msk;
    }

    /* Shielding is available in CSD only */
    if ((CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdEn) &&
        (CY_CAPSENSE_SHIELD_DISABLED != context->ptrCommonConfig->csdShieldMode))
    {
        ptrMapping = &context->ptrInternalContext->mapSenseMethod[0u];
        if(CY_CAPSENSE_REG_MODE_UNDEFINED != ptrMapping[CY_CAPSENSE_REG_MODE_CSD_RM])
        {
            ptrBaseCfgMode = &ptrBaseCfg->mode[ptrMapping[CY_CAPSENSE_REG_MODE_CSD_RM]];
            Cy_CapSense_ApplyShieldConfig(ptrBaseCfgMode, context);
        }

        if(CY_CAPSENSE_REG_MODE_UNDEFINED != ptrMapping[CY_CAPSENSE_REG_MODE_CSD_RM_DITHERING])
        {
            ptrBaseCfgMode = &ptrBaseCfg->mode[ptrMapping[CY_CAPSENSE_REG_MODE_CSD_RM_DITHERING]];
            Cy_CapSense_ApplyShieldConfig(ptrBaseCfgMode, context);
        }
    }

    /* Generating the common configuration for the Compensation CDAC */
    ptrBaseCfg->ccompCdacCtl &= ~CY_CAPSENSE_CCOMP_CDAC_CTL_MASK;
    ptrBaseCfg->swSelCdacCo &= ~CY_CAPSENSE_CCOMP_CDAC_SW_SEL_MASK;
    if ((CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csdCdacCompEn) ||
        (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->csxCdacCompEn))
    {
        ptrBaseCfg->ccompCdacCtl |= CY_CAPSENSE_ENABLE_CCOMP_CDAC_CTL_SEL_VALUE;
        ptrBaseCfg->swSelCdacCo |= CY_CAPSENSE_ENABLE_CCOMP_CDAC_SW_SEL_VALUE;
    }

    /* Generating the common configuration for the CIC2 Filter */
    ptrBaseCfg->filterCtl = 0uL;
    ptrBaseCfg->filterCtl |= (uint32_t)context->ptrCommonConfig->cicFilterMode << MSC_FILTER_CTL_FILTER_MODE_Pos;

    /* Generating the common configuration for the control mux switch control */
    if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
    {
        Cy_CapSense_GenerateCtrlMuxSwControl(chIndex, context);
    }

    /* Generating the common configuration for the dithering CapDAC */
    ptrBaseCfg->ditherCdacCtl = (uint32_t)context->ptrCommonContext->cdacDitherSeed |
                                ((uint32_t)context->ptrCommonContext->cdacDitherPoly << MSC_DITHER_CDAC_CTL_LFSR_POLY_FL_Pos);

    /* Configures synchronization signals */

    /* Sync_Clock */
    ptrBaseCfg->ctl &= ~MSC_CTL_CLK_SYNC_EN_Msk;
    /* If SYNC_CLK is enabled, generates CLK_SYNC_EN only for the Master channel */
    if ((((uint32_t)context->ptrCommonConfig->masterChannelId) == (chIndex + context->ptrCommonConfig->channelOffset)) &&
        (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->syncClockEn))
    {
        ptrBaseCfg->ctl |= (uint32_t)context->ptrCommonConfig->syncClockEn << MSC_CTL_CLK_SYNC_EN_Pos;
    }
    /* Frame Start */
    ptrBaseCfg->ctl &= ~MSC_CTL_EXT_FRAME_START_EN_Msk;
    /*
     * If EXT_FRAME_START is enabled, generates EXT_FRAME_START_EN for all channels,
     * except of the Master channel which is generating FRAME_START for all another channels
     */
    if ((((uint32_t)context->ptrCommonConfig->masterChannelId) == (chIndex + context->ptrCommonConfig->channelOffset)) &&
        (CY_CAPSENSE_ENABLE == context->ptrCommonConfig->syncFrameStartEn))
    {
        ptrBaseCfg->ctl |= (uint32_t)context->ptrCommonConfig->syncFrameStartEn << MSC_CTL_EXT_FRAME_START_EN_Pos;
    }
    /* Generating the multi-channel mode for each channel with updating the sensor control registers */
    if (0u != context->ptrCommonConfig->syncMode)
    {
        Cy_MSC_WriteReg(context->ptrCommonConfig->ptrMscChConfig[chIndex].ptrMscBase,
                        CY_MSC_REG_OFFSET_SNS_CTL,
                        (context->ptrCommonConfig->syncMode << MSC_SNS_CTL_MULTI_CH_MODE_Pos));
    }

    /* Generating the common configuration for the clock dithering */
    ptrBaseCfg->sensePeriodCtl = ((uint32_t)context->ptrCommonContext->lfsrPoly << MSC_SENSE_PERIOD_CTL_LFSR_POLY_Pos) |
                                 ((uint32_t)context->ptrCommonContext->lfsrScale << MSC_SENSE_PERIOD_CTL_LFSR_SCALE_Pos);

    /* Generating the common configuration for the number of the auto-resampling cycles and the counter behaviour when the
     * RAW_COUNT exceeds 0xFFFF
     */
    ptrBaseCfg->scanCtl1 &= ~MSC_SCAN_CTL1_NUM_AUTO_RESAMPLE_Msk;
    ptrBaseCfg->scanCtl1 &= ~MSC_SCAN_CTL1_RAW_COUNT_MODE_Msk;
    ptrBaseCfg->scanCtl1 |= (uint32_t)context->ptrCommonConfig->numBadScans << MSC_SCAN_CTL1_NUM_AUTO_RESAMPLE_Pos;
    ptrBaseCfg->scanCtl1 |= (uint32_t)context->ptrCommonConfig->counterMode << MSC_SCAN_CTL1_RAW_COUNT_MODE_Pos;

    /* Generating the common configuration for the number of epilogue cycles */
    ptrBaseCfg->scanCtl2 &= ~MSC_SCAN_CTL2_NUM_EPI_CYCLES_Msk;
    if (0u < context->ptrCommonConfig->numEpiCycles)
    {
        ptrBaseCfg->scanCtl2 |= (uint32_t)context->ptrCommonConfig->numEpiCycles << MSC_SCAN_CTL2_NUM_EPI_CYCLES_Pos;
    }
    else
    {
        ptrBaseCfg->scanCtl2 |= (uint32_t)(1uL << MSC_SCAN_CTL2_NUM_EPI_CYCLES_Pos);
    }

    /* Generating the common configuration for the system level chopping */
    ptrBaseCfg->scanCtl2 &= ~MSC_SCAN_CTL2_CHOP_POL_Msk;
    ptrBaseCfg->scanCtl2 |= (uint32_t)context->ptrCommonConfig->chopPolarity << MSC_SCAN_CTL2_CHOP_POL_Pos;

    /* Generating the common configuration for the coarse initialization and coarse short phase */
    ptrBaseCfg->initCtl1 &= ~MSC_INIT_CTL1_NUM_INIT_CMOD_12_RAIL_CYCLES_Msk;
    ptrBaseCfg->initCtl1 &= ~MSC_INIT_CTL1_NUM_INIT_CMOD_12_SHORT_CYCLES_Msk;
    ptrBaseCfg->initCtl2 &= ~MSC_INIT_CTL2_NUM_INIT_CMOD_34_RAIL_CYCLES_Msk;
    ptrBaseCfg->initCtl2 &= ~MSC_INIT_CTL2_NUM_INIT_CMOD_34_SHORT_CYCLES_Msk;

    if((CY_CAPSENSE_MSC0_CMOD1PADD_PIN == ptrMscChConfig->pinCmod1) ||
       (CY_CAPSENSE_MSC1_CMOD1PADD_PIN == ptrMscChConfig->pinCmod1))
    {
        ptrBaseCfg->initCtl1 |= (uint32_t)context->ptrCommonConfig->numCoarseInitChargeCycles << MSC_INIT_CTL1_NUM_INIT_CMOD_12_RAIL_CYCLES_Pos;
        ptrBaseCfg->initCtl1 |= (uint32_t)context->ptrCommonConfig->numCoarseInitSettleCycles << MSC_INIT_CTL1_NUM_INIT_CMOD_12_SHORT_CYCLES_Pos;
        ptrBaseCfg->initCtl3 |= CY_CAPSENSE_CMOD12_PAIR_SELECTION << MSC_INIT_CTL3_CMOD_SEL_Pos;
    }
    else
    {
        ptrBaseCfg->initCtl2 |= (uint32_t)context->ptrCommonConfig->numCoarseInitChargeCycles << MSC_INIT_CTL2_NUM_INIT_CMOD_34_RAIL_CYCLES_Pos;
        ptrBaseCfg->initCtl2 |= (uint32_t)context->ptrCommonConfig->numCoarseInitSettleCycles << MSC_INIT_CTL2_NUM_INIT_CMOD_34_SHORT_CYCLES_Pos;
        ptrBaseCfg->initCtl3 |= CY_CAPSENSE_CMOD34_PAIR_SELECTION << MSC_INIT_CTL3_CMOD_SEL_Pos;
    }

    /* Generating the common configuration for the number of sub-conversions to be run during PRO_DUMMY and PRO_WAIT phases. */
    ptrBaseCfg->initCtl4 &= ~MSC_INIT_CTL4_NUM_PRO_DUMMY_SUB_CONVS_Msk;
    ptrBaseCfg->initCtl4 &= ~MSC_INIT_CTL4_NUM_PRO_WAIT_CYCLES_Msk;
    ptrBaseCfg->initCtl4 |= (uint32_t)context->ptrCommonContext->numFineInitCycles << MSC_INIT_CTL4_NUM_PRO_DUMMY_SUB_CONVS_Pos;
    ptrBaseCfg->initCtl4 |= (uint32_t)context->ptrCommonContext->numFineInitWaitCycles << MSC_INIT_CTL4_NUM_PRO_WAIT_CYCLES_Pos;

    if ((1u < context->ptrCommonConfig->numChips) ||
        (1u < context->ptrCommonConfig->numChannels))
    {
        ptrBaseCfg->initCtl3 &= ~MSC_INIT_CTL3_NUM_PRO_OFFSET_TRIPS_Msk;
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateSensorConfigValid
****************************************************************************//**
*
* Generates the configuration for registers that have to be configured to start
* a scan for a single sensor.
*
* \param scanSlot
* Slot to scan
*
* \param ptrSensorCfg
* Specifies the pointer to the sensor configuration to be filled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_GenerateSensorConfigValid(
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context)
{
    uint32_t wdIndex;
    uint32_t widgetSenseMethod;
    uint32_t snsMethodInternal;
    cy_capsense_status_t capStatus = CY_CAPSENSE_BAD_PARAM_E;

    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] = 0u;
    ptrSensorCfg[CY_CAPSENSE_SNS_CDAC_CTL_INDEX] = 0u;
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] = 0u;

    /* Sensor clock configuration */
    Cy_CapSense_ConfigSensorClock(scanSlot, &ptrSensorCfg[0u], context);

    /* CapDAC configuration */
    capStatus = Cy_CapSense_GenerateCdacConfig(scanSlot, &ptrSensorCfg[0u], context);

    /* CIC2 filter control */
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] &= ~MSC_SNS_CTL_DECIM_RATE_Msk;
    wdIndex = context->ptrScanSlots[scanSlot].wdId;
    if (0u != context->ptrCommonConfig->cicFilterMode)
    {
        ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= ((uint32_t)context->ptrWdContext[wdIndex].cicRate - 1u) << MSC_SNS_CTL_DECIM_RATE_Pos;
    }

    /* Update widget sense method */
    snsMethodInternal = context->ptrWdConfig[wdIndex].senseMethod;
    snsMethodInternal -= 1u;
    if (CY_CAPSENSE_ENABLE == context->ptrWdContext[wdIndex].cdacDitherEn)
    {
        /* Shift sense mode to cdac dither enabled modes */
        snsMethodInternal += CY_CAPSENSE_REG_MODE_BASE_NUMBER;
    }
    widgetSenseMethod = context->ptrInternalContext->mapSenseMethod[snsMethodInternal];
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] &= ~MSC_SNS_CTL_SENSE_MODE_SEL_Msk;
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= widgetSenseMethod << MSC_SNS_CTL_SENSE_MODE_SEL_Pos;

    /* Multi-channel mode */
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] &= ~MSC_SNS_CTL_MULTI_CH_MODE_Msk;
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= (uint32_t)context->ptrCommonConfig->syncMode << MSC_SNS_CTL_MULTI_CH_MODE_Pos;

    /* System level chopping */
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] &= ~MSC_SNS_SCAN_CTL_NUM_CONV_Msk;
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] |= ((uint32_t)context->ptrWdConfig[wdIndex].numChopCycles - 1u) << MSC_SNS_SCAN_CTL_NUM_CONV_Pos;

    /* Number of sub-conversions */
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] &= ~MSC_SNS_SCAN_CTL_NUM_SUB_CONVS_Msk;
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] |= (context->ptrWdContext[wdIndex].numSubConversions - 1u) << MSC_SNS_SCAN_CTL_NUM_SUB_CONVS_Pos;

    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] &= ~(MSC_SNS_SCAN_CTL_INIT_BYPASS_Msk);
    if((CY_CAPSENSE_ENABLE == context->ptrWdContext[wdIndex].coarseInitBypassEn) &&
       ((scanSlot % context->ptrCommonConfig->numSlots) != context->ptrWdConfig[wdIndex].firstSlotId))
    {
        ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] |= (MSC_SNS_SCAN_CTL_INIT_BYPASS_Msk);
    }

    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= (MSC_SNS_CTL_VALID_Msk);
    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= (MSC_SNS_CTL_START_SCAN_Msk);

   return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateSensorConfig
****************************************************************************//**
*
* Generates configuration to configure registers to
* the scan of the single sensor in the specified slot of the specified channel.
*
* \param chIndex
* The specified channel index.
*
* \param scanSlot
* The specified slot index.
*
* \param ptrSensorCfg
* Specifies the pointer to the sensor configuration to be filled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_GenerateSensorConfig(
                uint32_t chIndex,
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context)
{
    uint32_t wdIndex;
    uint32_t slotIndex = scanSlot;
    uint32_t nextChIndex;

    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_BAD_PARAM;

    wdIndex = context->ptrScanSlots[scanSlot].wdId;

    if((wdIndex == CY_CAPSENSE_SLOT_EMPTY) ||
       (wdIndex == CY_CAPSENSE_SLOT_SHIELD_ONLY) ||
       (wdIndex == CY_CAPSENSE_SLOT_TX_ONLY))
    {
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            if (chIndex == 0u)
            {
                slotIndex = scanSlot + context->ptrCommonConfig->numSlots;
            }
            else
            {
                slotIndex = scanSlot - context->ptrCommonConfig->numSlots;
            }
        }
        else
        {
            nextChIndex = (chIndex == 0u ? 1u : 0u);
            slotIndex = context->ptrActiveScanSns[chIndex].currentChannelSlotIndex + nextChIndex * context->ptrCommonConfig->numSlots;
        }

        /* Generate proper config for sensor */
        capStatus = Cy_CapSense_GenerateSensorConfigValid(slotIndex, ptrSensorCfg, context);
    }
    else
    {
        /* Generate proper config for sensor */
        capStatus = Cy_CapSense_GenerateSensorConfigValid(slotIndex, ptrSensorCfg, context);
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateAllSensorConfig
****************************************************************************//**
*
* Generates configuration to configure registers to start
* a scan for all sensors of the specified channel.
*
* \param chId
* Channel ID
*
* \param ptrSensorCfg
* Specifies the pointer to the sensor configuration to be filled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_GenerateAllSensorConfig(
                uint32_t chId,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context)
{
    uint32_t i = 0u;
    uint32_t j = 0u;
    uint32_t scanSlotIndex;
    uint32_t scanSlotIndexValid;
    uint8_t * ptrMapping;
    uint32_t snsMask;
    uint32_t snsMaskNegative;
    uint32_t snsMaskInactive = 0u;
    uint32_t snsFuncState;
    uint32_t snsFuncStateNegative;
    uint32_t snsFuncStateSelfCap;
    uint32_t snsFuncStateMutualCap;
    uint32_t snsIndex;
    uint32_t wdIndex;
    uint32_t slotValue;
    uint32_t widgetSenseGroup;
    uint32_t pattern;
    const cy_stc_capsense_electrode_config_t * eltdPinCfg;
    cy_capsense_status_t capStatus = CY_CAPSENSE_STATUS_SUCCESS;
    uint32_t sensorCfgTmp[CY_MSC_6_SNS_REGS] = {0u, 0u, 0u, 0u, 0u, 0u};

    /* Get mask for pins in control mux switch registers */
    for (i = 0u; i < context->ptrCommonConfig->numPin; i++)
    {
        if ((chId + context->ptrCommonConfig->channelOffset) == context->ptrPinConfig[i].chId)
        {
            snsMaskInactive |= (0x01u << context->ptrPinConfig[i].padNumber);
        }
    }

    ptrMapping = &context->ptrInternalContext->mapPinState[0u];
    /* Define mutual cap pin state */
    snsFuncStateMutualCap = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND];
    /* Define self cap pin state */
    snsFuncStateSelfCap = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND];
    switch (context->ptrCommonConfig->csdInactiveSnsConnection)
    {
        case CY_CAPSENSE_SNS_CONNECTION_HIGHZ:
            snsFuncStateSelfCap = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_HIGH_Z];
            break;
        case CY_CAPSENSE_SNS_CONNECTION_SHIELD:
            snsFuncStateSelfCap = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SHIELD];
            break;
        default:
            break;
    }

    for (scanSlotIndex = 0u; scanSlotIndex < context->ptrCommonConfig->numSlots; scanSlotIndex++)
    {
        scanSlotIndexValid = scanSlotIndex + (chId + context->ptrCommonConfig->channelOffset) * context->ptrCommonConfig->numSlots;
        Cy_CapSense_GenerateSensorConfig((chId + context->ptrCommonConfig->channelOffset), scanSlotIndexValid, &sensorCfgTmp[0], context);

        /* Does this for Control Mux connection only */
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            slotValue = context->ptrScanSlots[scanSlotIndexValid].wdId;
            if (CY_CAPSENSE_SLOT_EMPTY == slotValue)
            {
                /* INACTIVE SENSORS */
                Cy_CapSense_CalculateMaskRegisters(snsMaskInactive, ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND], &sensorCfgTmp[0u]);
            }
            else
            {
                wdIndex = slotValue;
                if (CY_CAPSENSE_SLOT_SHIELD_ONLY <= slotValue)
                {
                    scanSlotIndexValid = (context->ptrScanSlots[scanSlotIndexValid].snsId * context->ptrCommonConfig->numSlots) +
                                (scanSlotIndexValid % context->ptrCommonConfig->numSlots);
                }

                snsIndex = context->ptrScanSlots[scanSlotIndexValid].snsId;
                wdIndex = context->ptrScanSlots[scanSlotIndexValid].wdId;
                widgetSenseGroup = context->ptrWdConfig[wdIndex].senseGroup;
                snsFuncState = snsFuncStateMutualCap;
                if (CY_CAPSENSE_CSD_GROUP == widgetSenseGroup)
                {
                    snsFuncState = snsFuncStateSelfCap;
                }
                /* INACTIVE SENSORS */
                Cy_CapSense_CalculateMaskRegisters(snsMaskInactive, snsFuncState, &sensorCfgTmp[0u]);

                if (CY_CAPSENSE_SLOT_SHIELD_ONLY > slotValue)
                {
                    /* Initializes an active sensor (including ganged sensors) by SNS, RX or TX sensor state */
                    if (CY_CAPSENSE_CSD_GROUP == widgetSenseGroup)
                    {
                        /* SELF-CAP SENSOR */
                        snsMask = 0u;
                        snsFuncState = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SNS];
                        eltdPinCfg = &context->ptrWdConfig[wdIndex].ptrEltdConfig[snsIndex];
                        /* Proceed only if electrode configuration belongs to selected channel */
                        if ((chId + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
                        {
                            /* Loop through all pads for this electrode (ganged sensor) */
                            for (i = 0u; i < eltdPinCfg->numPins; i++)
                            {
                                snsMask |= 0x01u << eltdPinCfg->ptrPin[i].padNumber;
                            }
                            Cy_CapSense_CalculateMaskRegisters(snsMask, snsFuncState, &sensorCfgTmp[0u]);
                        }
                    }
                }

                if (CY_CAPSENSE_CSX_GROUP == widgetSenseGroup)
                {
                    if (CY_CAPSENSE_SLOT_SHIELD_ONLY > slotValue)
                    {
                        /* RX ELECTRODE */
                        snsMask = 0u;
                        snsFuncState = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_RX];
                        i = snsIndex / context->ptrWdConfig[wdIndex].numRows;
                        eltdPinCfg = &context->ptrWdConfig[wdIndex].ptrEltdConfig[i];
                        if ((chId + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
                        {
                            /* Loop through all pads for this electrode (ganged sensor) */
                            for (i = 0u; i < eltdPinCfg->numPins; i++)
                            {
                                snsMask |= 0x01u << eltdPinCfg->ptrPin[i].padNumber;
                            }
                            Cy_CapSense_CalculateMaskRegisters(snsMask, snsFuncState, &sensorCfgTmp[0u]);
                        }
                    }

                    if (CY_CAPSENSE_SLOT_SHIELD_ONLY != slotValue)
                    {
                        /* Handles multi-phase TX feature */
                        if ((CY_CAPSENSE_ENABLE == context->ptrCommonConfig->mptxEn) &&
                            (context->ptrWdConfig[wdIndex].mptxOrder >= CY_CAPSENSE_MPTX_MIN_ORDER))
                        {
                            /* Multiple TX ELECTRODES */
                            snsMask = 0u;
                            snsMaskNegative = 0u;
                            snsFuncState = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_TX];
                            snsFuncStateNegative = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_TX_NEGATIVE];
                            /* Finds the first sensor number in mptx group */
                            i = snsIndex - (snsIndex % context->ptrWdConfig[wdIndex].mptxOrder);
                            /* Finds TX electrode of the first group sensor */
                            i = context->ptrWdConfig[wdIndex].numCols + (i % context->ptrWdConfig[wdIndex].numRows);
                            eltdPinCfg = &context->ptrWdConfig[wdIndex].ptrEltdConfig[i];
                            /* Finding the right vector / pattern for mptx operation */
                            pattern = context->ptrWdConfig[wdIndex].ptrMptxTable->vector;
                            i = (snsIndex % context->ptrWdConfig[wdIndex].mptxOrder);
                            if (0u != i)
                            {
                                pattern = (pattern >> i) | (pattern << (context->ptrWdConfig[wdIndex].mptxOrder - i));
                            }
                            if (CY_CAPSENSE_MPTX_MAX_ORDER > context->ptrWdConfig[wdIndex].mptxOrder)
                            {
                                pattern &= (1u << context->ptrWdConfig[wdIndex].mptxOrder) - 1u;
                            }
                            /* Loop through all involved mptx TX electrodes, positive and negative */
                            for (j = 0u; j < context->ptrWdConfig[wdIndex].mptxOrder; j++)
                            {
                                if ((chId + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
                                {
                                    if (0u != (pattern & 0x01))
                                    {
                                        /* Loop through all pads for this electrode (ganged sensor) */
                                        for (i = 0u; i < eltdPinCfg->numPins; i++)
                                        {
                                            snsMask |= 0x01u << eltdPinCfg->ptrPin[i].padNumber;
                                        }
                                    }
                                    else
                                    {
                                        /* Loop through all pads for this electrode (ganged sensor) */
                                        for (i = 0u; i < eltdPinCfg->numPins; i++)
                                        {
                                            snsMaskNegative |= 0x01u << eltdPinCfg->ptrPin[i].padNumber;
                                        }
                                    }
                                }
                                pattern >>= 0x01u;
                                eltdPinCfg++;
                            }
                            Cy_CapSense_CalculateMaskRegisters(snsMask, snsFuncState, &sensorCfgTmp[0u]);
                            Cy_CapSense_CalculateMaskRegisters(snsMaskNegative, snsFuncStateNegative, &sensorCfgTmp[0u]);
                        }
                        else
                        {
                            /* TX ELECTRODE */
                            snsMask = 0u;
                            snsFuncState = ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_TX];
                            i = context->ptrWdConfig[wdIndex].numCols +
                                    (snsIndex % context->ptrWdConfig[wdIndex].numRows);
                            eltdPinCfg = &context->ptrWdConfig[wdIndex].ptrEltdConfig[i];
                            if ((chId + context->ptrCommonConfig->channelOffset) == eltdPinCfg->chId)
                            {
                                /* Loop through all pads for this electrode (ganged sensor) */
                                for (i = 0u; i < eltdPinCfg->numPins; i++)
                                {
                                    snsMask |= 0x01u << eltdPinCfg->ptrPin[i].padNumber;
                                }
                                Cy_CapSense_CalculateMaskRegisters(snsMask, snsFuncState, &sensorCfgTmp[0u]);
                            }
                        }
                    }
                }

                /* SHIELD ELECTRODE */
                if (CY_CAPSENSE_SHIELD_DISABLED != context->ptrCommonConfig->csdShieldMode)
                {
                    snsMask = 0u;
                    /* Connect shield to ground in CSX mode */
                    snsFuncState = (CY_CAPSENSE_CSD_GROUP == widgetSenseGroup) ?
                                    ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_SHIELD] : ptrMapping[CY_CAPSENSE_CTRLMUX_STATE_GND];

                    for (i = 0u; i < context->ptrCommonConfig->csdShieldNumPin; i++)
                    {
                        if ((chId + context->ptrCommonConfig->channelOffset) == context->ptrShieldPinConfig[i].chId)
                        {
                            snsMask |= 0x01u << context->ptrShieldPinConfig[i].padNumber;
                        }
                    }
                    Cy_CapSense_CalculateMaskRegisters(snsMask, snsFuncState, &sensorCfgTmp[0u]);
                }
            }
        }

        j = 0u;

        /* Store results */
        if (CY_MSC_5_SNS_REGS == context->ptrInternalContext->snsConfigSize)
        {
            j++;
        }
        for (i = 0u; i < context->ptrInternalContext->snsConfigSize; i++)
        {
            ptrSensorCfg[i] = sensorCfgTmp[j++];
        }
        ptrSensorCfg += context->ptrInternalContext->snsConfigSize;
    }

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_CalculateMaskRegisters
****************************************************************************//**
*
* Calculates the mask for pins that have to be updated for Control MUX
* connection.
*
* \param mask
* Specifies the mask of pins that should be updated.
*
* \param funcState
* Specifies the pin state functionality.
*
* \param ptrCfg
* Specifies the pointer to the mask registers.
*
*******************************************************************************/
static void Cy_CapSense_CalculateMaskRegisters(
                uint32_t mask,
                uint32_t funcState,
                uint32_t * ptrCfg)
{
    uint32_t * ptrCfgMask = ptrCfg;

    ptrCfgMask[0u] &= ~mask;
    ptrCfgMask[1u] &= ~mask;
    ptrCfgMask[2u] &= ~mask;
    if (0u != (funcState & 0x04u))
    {
        ptrCfgMask[0u] |= mask;
    }
    if (0u != (funcState & 0x02u))
    {
        ptrCfgMask[1u] |= mask;
    }
    if (0u != (funcState & 0x01u))
    {
        ptrCfgMask[2u] |= mask;
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateCdacConfig
****************************************************************************//**
*
* Generates the Cap DAC configuration for a selected sensor.
*
* \param scanSlot
* Slot to scan
*
* \param ptrSensorCfg
* Specifies the pointer to the sensor configuration to be filled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
* \return
* Returns the status of the operation:
* - CY_CAPSENSE_STATUS_SUCCESS       - The operation is performed successfully.
*
*******************************************************************************/
cy_capsense_status_t Cy_CapSense_GenerateCdacConfig(
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context)
{
    cy_capsense_status_t capStatus = 0u;
    const cy_stc_capsense_widget_config_t * ptrWdCfg;
    const cy_stc_capsense_sensor_context_t * ptrSnsCxt;
    uint32_t compDiv;
    uint32_t wdIndex;
    uint32_t snsIndex;
    uint32_t cdacLfsrScale;
    uint32_t snsCdacCtlReg = 0u;

    wdIndex = context->ptrScanSlots[scanSlot].wdId;
    snsIndex = context->ptrScanSlots[scanSlot].snsId;
    ptrWdCfg  = &context->ptrWdConfig[wdIndex];
    ptrSnsCxt = &ptrWdCfg->ptrSnsContext[snsIndex];

    /* Compensation CDAC Divider */
    compDiv = context->ptrWdContext[wdIndex].cdacCompDivider;
    compDiv = (compDiv > 0u) ? (compDiv - 1u) : 0u;
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] &= ~MSC_SNS_SCAN_CTL_COMP_DIV_Msk;
    ptrSensorCfg[CY_CAPSENSE_SNS_SCAN_CTL_INDEX] |= compDiv << MSC_SNS_SCAN_CTL_COMP_DIV_Pos;

    /* Cap DAC dithering control */
    snsCdacCtlReg = 0u;

    snsCdacCtlReg |= MSC_SNS_CDAC_CTL_CLOCK_REF_RATE_Msk;

    if (CY_CAPSENSE_ENABLE == ptrWdCfg->ptrWdContext->cdacDitherEn)
    {
        cdacLfsrScale = context->ptrWdContext[wdIndex].cdacDitherValue;
        snsCdacCtlReg |= cdacLfsrScale << MSC_SNS_CDAC_CTL_LFSR_SCALE_FL_Pos;
        snsCdacCtlReg |= MSC_SNS_CDAC_CTL_FL_EN_Msk;
    }

    if ((0u != (CY_CAPSENSE_BUSY_CALIBRATION & context->ptrCommonContext->status)) &&
        (0u == (CY_CAPSENSE_BUSY_VERIFY_CALIBRATION & context->ptrCommonContext->status)))
    {
        /* If calibration, reuse compensation CDAC for single CDAC mode as Ref CDAC */
        snsCdacCtlReg |= ((uint32_t)ptrSnsCxt->cdacComp) << MSC_SNS_CDAC_CTL_SEL_RE_Pos;
    }
    else
    {
        /* Ref CDAC Code setup */
        if ((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) &&
            (ptrWdCfg->numCols <= snsIndex))
        {
            snsCdacCtlReg |= ((uint32_t)ptrWdCfg->ptrWdContext->rowCdacRef[0u]) <<
                                                MSC_SNS_CDAC_CTL_SEL_RE_Pos;
        }
        else
        {
            snsCdacCtlReg |= ((uint32_t)ptrWdCfg->ptrWdContext->cdacRef[0u]) <<
                                                MSC_SNS_CDAC_CTL_SEL_RE_Pos;
        }

        if(((CY_CAPSENSE_CSD_GROUP == ptrWdCfg->senseGroup) && (context->ptrCommonConfig->csdCdacCompEn)) ||
           ((CY_CAPSENSE_CSX_GROUP == ptrWdCfg->senseGroup) && (context->ptrCommonConfig->csxCdacCompEn)))
        {
            snsCdacCtlReg |= ((uint32_t)ptrSnsCxt->cdacComp) << MSC_SNS_CDAC_CTL_SEL_CO_Pos;
        }
    }
    ptrSensorCfg[CY_CAPSENSE_SNS_CDAC_CTL_INDEX] = snsCdacCtlReg;

    return (capStatus);
}


/*******************************************************************************
* Function Name: Cy_CapSense_GenerateCtrlMuxSwControl
****************************************************************************//**
*
* Generates the Control MUX Switch Control register configuration.
*
* \param chIndex
* The desired channel index.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_GenerateCtrlMuxSwControl(
                uint32_t chIndex,
                cy_stc_capsense_context_t * context)
{
    uint32_t cswFuncMode = 0uL;
    uint32_t i;
    const cy_stc_capsense_pin_config_t * ptrPinCfg = context->ptrPinConfig;

    for (i = 0u; i < context->ptrCommonConfig->numPin; i++)
    {
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrPinCfg->chId)
        {
            cswFuncMode |= (0x01u << ptrPinCfg->padNumber);
        }
        ptrPinCfg++;
    }

    ptrPinCfg = context->ptrShieldPinConfig;
    for (i = 0u; i < context->ptrCommonConfig->csdShieldNumPin; i++)
    {
        if ((chIndex + context->ptrCommonConfig->channelOffset) == ptrPinCfg->chId)
        {
            cswFuncMode |= 0x01u << ptrPinCfg->padNumber;
        }
        ptrPinCfg++;
    }
    context->ptrBaseFrameContext[chIndex].cswCtl = cswFuncMode;
}


/*******************************************************************************
* Function Name: Cy_CapSense_ApplyShieldConfig
****************************************************************************//**
*
* Applies shield configuration to the base configuration.
*
* \param ptrBaseCfgMode
* Specifies the pointer to the sense mode registers structure
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_ApplyShieldConfig(
                cy_stc_msc_mode_config_t * ptrBaseCfgMode,
                cy_stc_capsense_context_t * context)
{
    ptrBaseCfgMode->senseDutyCtl &= ~(MSC_MODE_SENSE_DUTY_CTL_PHASE_GAP_FS2_PH0_EN_Msk |
                                      MSC_MODE_SENSE_DUTY_CTL_PHASE_GAP_FS2_PH1_EN_Msk);
    /* Active Shield mode */
    if (CY_CAPSENSE_SHIELD_ACTIVE == context->ptrCommonConfig->csdShieldMode)
    {
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            ptrBaseCfgMode->swSelTop &= ~CY_CAPSENSE_FW_SHIELD_ACTIVE_CTRLMUX_REG_SW_SEL_TOP_MASK;
            ptrBaseCfgMode->swSelSh  &= ~CY_CAPSENSE_FW_SHIELD_ACTIVE_CTRLMUX_REG_SW_SEL_SH_MASK;
            ptrBaseCfgMode->swSelTop |= CY_CAPSENSE_FW_SHIELD_ACTIVE_CTRLMUX_REG_SW_SEL_TOP_VALUE;
            ptrBaseCfgMode->swSelSh  |= CY_CAPSENSE_FW_SHIELD_ACTIVE_CTRLMUX_REG_SW_SEL_SH_VALUE;
        }
        else
        {
            /* Amux sensor_connection_method */
            ptrBaseCfgMode->swSelTop &= ~CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_SEL_TOP_MASK;
            ptrBaseCfgMode->swSelSh  &= ~CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_SEL_SH_MASK;
            ptrBaseCfgMode->swSelTop |= CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_SEL_TOP_VALUE;
            ptrBaseCfgMode->swSelSh  |= CY_CAPSENSE_FW_SHIELD_ACTIVE_AMUX_REG_SW_SEL_SH_VALUE;
        }
    }
    /* Passive Shield mode */
    else
    {
        if (CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD == context->ptrCommonConfig->sensorConnection)
        {
            ptrBaseCfgMode->swSelTop &= ~CY_CAPSENSE_FW_SHIELD_PASSIVE_CTRLMUX_REG_SW_SEL_TOP_MASK;
            ptrBaseCfgMode->swSelTop |= CY_CAPSENSE_FW_SHIELD_PASSIVE_CTRLMUX_REG_SW_SEL_TOP_VALUE;
            ptrBaseCfgMode->senseDutyCtl |= ((1u << MSC_MODE_SENSE_DUTY_CTL_PHASE_GAP_FS2_PH0_EN_Pos) |
                                             (1u << MSC_MODE_SENSE_DUTY_CTL_PHASE_GAP_FS2_PH1_EN_Pos));
        }
    }
}


/*******************************************************************************
* Function Name: Cy_CapSense_ConfigSensorClock
****************************************************************************//**
*
* This function configures the sense clock for different modes.
*
* \param scanSlot
* Slot to scan
*
* \param ptrSensorCfg
* Specifies the pointer to the sensor configuration to be filled.
*
* \param context
* The pointer to the CapSense context structure \ref cy_stc_capsense_context_t.
*
*******************************************************************************/
static void Cy_CapSense_ConfigSensorClock(
                uint32_t scanSlot,
                uint32_t * ptrSensorCfg,
                cy_stc_capsense_context_t * context)
{
    uint32_t snsClkDivider;
    uint32_t wdIndex = context->ptrScanSlots[scanSlot].wdId;
    uint32_t snsIndex = context->ptrScanSlots[scanSlot].snsId;
    /* Get LFSR_MODE */
    uint32_t snsClkMode = (uint32_t)context->ptrWdContext[wdIndex].snsClkSource & ((uint32_t)~(uint32_t)CY_CAPSENSE_CLK_SOURCE_AUTO_MASK);

    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] &= ~(MSC_SNS_CTL_SENSE_DIV_Msk |
                                                 MSC_SNS_CTL_LFSR_MODE_Msk |
                                                 MSC_SNS_CTL_LFSR_BITS_Msk);

    /* Getting row clock divider for matrix buttons or touchpad widgets */
    snsClkDivider = context->ptrWdContext[wdIndex].snsClk;

    if ((CY_CAPSENSE_CSD_GROUP == context->ptrWdConfig[wdIndex].senseGroup) &&
        (context->ptrWdConfig[wdIndex].numCols <= snsIndex))
    {
        snsClkDivider = context->ptrWdContext[wdIndex].rowSnsClk;
    }

    /* In FW sensing method, snsClkDivider needs to be >=8 in case of DIRECT_CLOCK or SSC */
    if ((CY_CAPSENSE_CLK_SOURCE_PRS != snsClkMode) && (8u > snsClkDivider))
    {
        snsClkDivider = 8u;
    }

    /* Check divider value */
    if (0u == snsClkDivider)
    {
        snsClkDivider = 1u;
    }

    ptrSensorCfg[CY_CAPSENSE_SNS_CTL_INDEX] |= ((snsClkDivider - 1u) << MSC_SNS_CTL_SENSE_DIV_Pos) |
                                                         (snsClkMode << MSC_SNS_CTL_LFSR_MODE_Pos) |
                                                         (CY_CAPSENSE_LFSR_BITS_5B << MSC_SNS_CTL_LFSR_BITS_Pos);
}

#endif /* CY_IP_M0S8MSCV3 */


/* [] END OF FILE */
