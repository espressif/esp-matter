/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_acomp.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wm_acomp"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#define ACOMP_INTERRUPT_TYPE_MASK  (ACOMP_CTRL0_INT_ACT_HI_MASK | ACOMP_CTRL0_EDGE_LEVL_SEL_MASK)
#define ACOMP_INTERRUPT_TYPE_SHIFT (30U)
#define ACOMP_INTERRUPT_TYPE(x) \
    (((uint32_t)(((uint32_t)(x)) << ACOMP_INTERRUPT_TYPE_SHIFT)) & ACOMP_INTERRUPT_TYPE_MASK)

#define ACOMP_EDGE_DETECT_TRIG_SRC_MASK  (ACOMP_CTRL0_RIE_MASK | ACOMP_CTRL0_FIE_MASK)
#define ACOMP_EDGE_DETECT_TRIG_SRC_SHIFT (28U)
#define ACOMP_EDGE_DETECT_TRIG_SRC(x) \
    (((uint32_t)(((uint32_t)(x)) << ACOMP_EDGE_DETECT_TRIG_SRC_SHIFT)) & ACOMP_EDGE_DETECT_TRIG_SRC_MASK)

#define ACOMP_GET_BIT_FIELD_VAL(val, mask, shift) (((val) & (mask)) >> (shift))

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
static uint32_t ACOMP_GetInstance(ACOMP_Type *base);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL  */

/*******************************************************************************
 * Variables
 ******************************************************************************/
static ACOMP_Type *const s_acompBases[] = ACOMP_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to acomp clocks for each instance. */
static const clock_ip_name_t s_acompClocks[] = ACOMP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*!
 * @brief Get the ACOMP peripheral instance
 *
 * @param base ACOMP peripheral base address.
 * @return The instance of input ACOMP peripheral base address.
 */
static uint32_t ACOMP_GetInstance(ACOMP_Type *base)
{
    uint32_t instance;
    uint32_t acompArrayCount = (sizeof(s_acompBases) / sizeof(s_acompBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < acompArrayCount; instance++)
    {
        if (s_acompBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < acompArrayCount);

    return instance;
}
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*!
 * brief Initializes the module, including warm up time, response mode, inactive value and so on.
 *
 * param base ACOMP peripheral base address.
 * param config The pointer to the structure acomp_config_t.
 */
void ACOMP_Init(ACOMP_Type *base, const acomp_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the ACOMP clock*/
    (void)CLOCK_EnableClock(s_acompClocks[ACOMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    uint32_t tmp32;

    tmp32 = ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)config->id);
    tmp32 &= ~(ACOMP_CTRL0_GPIOINV_MASK | ACOMP_CTRL0_WARMTIME_MASK | ACOMP_CTRL0_BIAS_PROG_MASK |
               ACOMP_CTRL0_INACT_VAL_MASK | ACOMP_CTRL0_RIE_MASK | ACOMP_CTRL0_FIE_MASK | ACOMP_CTRL0_INT_ACT_HI_MASK |
               ACOMP_CTRL0_EDGE_LEVL_SEL_MASK);

    tmp32 |= ACOMP_CTRL0_WARMTIME(config->warmupTime) | ACOMP_CTRL0_BIAS_PROG(config->responseMode) |
             ACOMP_INTERRUPT_TYPE(config->intTrigType) | ACOMP_CTRL0_INACT_VAL(config->inactiveValue) |
             ACOMP_EDGE_DETECT_TRIG_SRC(config->edgeDetectTrigSrc);
    tmp32 |= ACOMP_CTRL0_GPIOINV((((uint8_t)(config->outPinMode)) & 0x2U) >> 1U);

    /* Reset the acomp module. */
    ACOMP_DoSoftwareReset(base, config->id);

    ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)config->id) = tmp32;

    tmp32 = ACOMP_GET_REG_VAL(&(base->ROUTE0), (uint8_t)config->id);
    tmp32 &= ~(ACOMP_ROUTE0_OUTSEL_MASK | ACOMP_ROUTE0_PE_MASK);
    tmp32 |= ACOMP_ROUTE0_OUTSEL((uint8_t)(config->outPinMode) & 0x1U) |
             ACOMP_ROUTE0_PE((!((uint8_t)(config->outPinMode) >> 2U)) & 0x1U);
    ACOMP_GET_REG_VAL(&(base->ROUTE0), (uint8_t)config->id) = tmp32;

    ACOMP_SetInputConfig(base, config->id, config->posInput, config->negInput);

    ACOMP_Enable(base, config->id, config->enable);
}

/*!
 * brief Gets the default configuration of ACOMP module.
 *
 * code
 *          config->id = kACOMP_Acomp0;
 *          config->enable = false;
 *          config->warmupTime = kACOMP_WarmUpTime1us;
 *          config->responseMode = kACOMP_SlowResponseMode;
 *          config->inactiveValue = kACOMP_ResultLogicLow;
 *          config->intTrigType = kACOMP_HighLevelTrig;
 *          config->edgeDetectTrigSrc = kACOMP_EdgePulseDis;
 *          config->outPinMode = kACOMP_PinOutDisable;
 *          config->posInput = NULL;
 *          config->negInput = NULL;
 * endcode
 * param config The pointer to the structure acomp_config_t.
 */
void ACOMP_GetDefaultConfig(acomp_config_t *config)
{
    assert(config != NULL);

    (void)memset(config, 0U, sizeof(acomp_config_t));

    config->id                = kACOMP_Acomp0;
    config->enable            = false;
    config->warmupTime        = kACOMP_WarmUpTime1us;
    config->responseMode      = kACOMP_SlowResponseMode;
    config->inactiveValue     = kACOMP_ResultLogicLow;
    config->intTrigType       = kACOMP_HighLevelTrig;
    config->edgeDetectTrigSrc = kACOMP_EdgePulseDis;
    config->outPinMode        = kACOMP_PinOutDisable;
    config->posInput          = NULL;
    config->negInput          = NULL;
}

/*!
 * @brief De-initializes the module.
 *
 * @param base ACOMP peripheral base address.
 */
void ACOMP_Deinit(ACOMP_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the ACOMP clock*/
    (void)CLOCK_DisableClock(s_acompClocks[ACOMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    ACOMP_Enable(base, kACOMP_Acomp0, false);
    ACOMP_Enable(base, kACOMP_Acomp1, false);
}

/*!
 * brief Configures selected comparator's inputs, inclduing input channel and hysteresis level.
 *
 * param base ACOMP peripheral base address.
 * param id The selected acomp comparator's id, please refer to acomp_comparator_id_t.
 * param posInput The configuration of selected comparator's positive input,
 *                  please refer to acomp_positive_input_config_t.
 * param negInput The configuration of selected comparator's negative input,
 *                  please refer to acomp_negative_input_config_t.
 */
void ACOMP_SetInputConfig(ACOMP_Type *base,
                          acomp_comparator_id_t id,
                          const acomp_positive_input_config_t *posInput,
                          const acomp_negative_input_config_t *negInput)
{
    assert(posInput != NULL);
    assert(negInput != NULL);

    uint32_t tmp32 = 0UL;

    tmp32 = ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id);
    tmp32 &= ~(ACOMP_CTRL0_HYST_SELN_MASK | ACOMP_CTRL0_HYST_SELP_MASK | ACOMP_CTRL0_NEG_SEL_MASK |
               ACOMP_CTRL0_POS_SEL_MASK | ACOMP_CTRL0_LEVEL_SEL_MASK);
    tmp32 |= ACOMP_CTRL0_HYST_SELN(negInput->hysterLevel) | ACOMP_CTRL0_HYST_SELP(posInput->hysterLevel) |
             ACOMP_CTRL0_NEG_SEL(negInput->channel) | ACOMP_CTRL0_POS_SEL(posInput->channel);
    if (negInput->channel >= kACOMP_NegChVIO_0P25)
    {
        tmp32 |= ACOMP_CTRL0_LEVEL_SEL((negInput->channel & 0x3U) << 4U);
    }
    ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id) = tmp32;
}

/*!
 * brief Gets status flags, such as ACOMP0 active status flags, ACOMP1 active status flags, and so on.
 *
 * param base ACOMP peripheral base address.
 * return The OR'ed value ACOMP status flags, please refer to _acomp_status_flags for details.
 */
uint32_t ACOMP_GetStatusFlags(ACOMP_Type *base)
{
    uint32_t tmp32 = 0UL;

    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->STATUS0, ACOMP_STATUS0_ACT_MASK, ACOMP_STATUS0_ACT_SHIFT) << 0UL;
    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->ISR0, ACOMP_ISR0_OUT_INT_MASK | ACOMP_ISR0_OUTA_INT_MASK,
                                     ACOMP_ISR0_OUT_INT_SHIFT)
             << 1UL;
    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->IRSR0, ACOMP_IRSR0_OUT_INT_RAW_MASK | ACOMP_IRSR0_OUTA_INT_RAW_MASK,
                                     ACOMP_IRSR0_OUT_INT_RAW_SHIFT)
             << 3UL;

    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->STATUS1, ACOMP_STATUS1_ACT_MASK, ACOMP_STATUS1_ACT_SHIFT) << 5UL;
    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->ISR1, ACOMP_ISR1_OUT_INT_MASK | ACOMP_ISR1_OUTA_INT_MASK,
                                     ACOMP_ISR1_OUT_INT_SHIFT)
             << 6UL;
    tmp32 |= ACOMP_GET_BIT_FIELD_VAL(base->IRSR1, ACOMP_IRSR1_OUT_INT_RAW_MASK | ACOMP_IRSR1_OUTA_INT_RAW_MASK,
                                     ACOMP_IRSR1_OUT_INT_RAW_SHIFT)
             << 8UL;

    return tmp32;
}
