/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ACOMP_H_
#define _FSL_ACOMP_H_

#include "fsl_common.h"

/*!
 * @addtogroup acomp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief ACOMP driver version */
#define FSL_ACOMP_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*!
 * @brief The macro to get the address based on start address and acomp id.
 */
#define ACOMP_REG_ADDR(startAddr, id) ((uint32_t *)((volatile uint32_t *)(startAddr) + (uint32_t)(id)))
/*!
 * @brief The macro to get register value based on start address and acomp id.
 */
#define ACOMP_GET_REG_VAL(startAddr, id) (*ACOMP_REG_ADDR((startAddr), (id)))
/*!
 * @brief Sets register's bit field.
 */
#define ACOMP_SET_REG_BIT(startAddr, id, val) (ACOMP_GET_REG_VAL((startAddr), (id)) |= (val))
/*!
 * @brief Clears register's bit field.
 */
#define ACOMP_CLEAR_REG_BIT(startAddr, id, val) (ACOMP_GET_REG_VAL((startAddr), (id)) &= ~(val))

/*!
 * @brief The enumeration of interrupts, including ACOMP0 synchrnized output interrupt,
 * ACOMP0 asynchrnized output interrupt, ACOMP1 synchrnized output interrupt, and
 * ACOMP1 asynchrnized output interrupt.
 */
enum _acomp_interrupt_enable
{
    kACOMP_Out0InterruptEnable  = ACOMP_IMR0_OUT_INT_MASK_MASK,  /*!< ACOMP0 synchrnized output interrupt enable. */
    kACOMP_OutA0InterruptEnable = ACOMP_IMR0_OUTA_INT_MASK_MASK, /*!< ACOMP0 asynchrnized output interrupt enable. */
    kACOMP_Out1InterruptEnable = ACOMP_IMR1_OUT_INT_MASK_MASK << 2U, /*!< ACOMP1 synchrnized output interrupt enable. */
    kACOMP_OutA1InterruptEnable = ACOMP_IMR1_OUTA_INT_MASK_MASK << 2U, /*!< ACOMP1 asynchrnized
                                                                            output interrupt enable. */
};

/*!
 * @brief The enumeration of status flags, including ACOMP0 active staus flag, ACOMP1 active status flag, and so on.
 */
enum _acomp_status_flags
{
    kACOMP_Acomp0ActiveFlag = 1UL << 0UL,           /*!< ACOMP0 active status flag, if this flag is set it means
                                                        the ACOMP0 is active. */
    kACOMP_Acomp0OutInterruptFlag = 1UL << 1UL,     /*!< ACOMP0 Synchronized output interrupt flags, this flag is set
                                                        when ACOMP0 synchronized output changes from 0 to 1 and the
                                                        corresponding interrupt is enabled. */
    kACOMP_Acomp0OutAInterruptFlag = 1UL << 2UL,    /*!< ACOMP0 Asynchronized output interrupt flags, this flag is set
                                                        when ACOMP0 asynchronized output changes from 0 to 1 and the
                                                        corresponding interrupt is enabled. */
    kACOMP_Acomp0RawOutInterruptFlag  = 1UL << 3UL, /*!< ACOMP0 raw synchroized output interrrupt flags. */
    kACOMP_Acomp0RawOutAInterruptFlag = 1UL << 4UL, /*!< ACOMP0 raw asynchroized output interrupt flags. */

    kACOMP_Acomp1ActiveFlag = 1UL << 5UL,           /*!< ACOMP1 active status flag, if this flag is set it means
                                                        the ACOMP0 is active. */
    kACOMP_Acomp1OutInterruptFlag = 1UL << 6UL,     /*!< ACOMP1 Synchronized output interrupt flags, this flag is set
                                                        when ACOMP1 synchronized output changes from 0 to 1 and the
                                                        corresponding interrupt is enabled. */
    kACOMP_Acomp1OutAInterruptFlag = 1UL << 7UL,    /*!< ACOMP1 Asynchronized output interrupt flags, this flag is set
                                                        when ACOMP1 asynchronized output changes from 0 to 1 and the
                                                        corresponding interrupt is enabled. */
    kACOMP_Acomp1RawOutInterruptFlag  = 1UL << 8UL, /*!< ACOMP1 raw synchroized output interrrupt flags. */
    kACOMP_Acomp1RawOutAInterruptFlag = 1UL << 9U,  /*!< ACOMP1 raw asynchroized output interrupt flags. */
};

/*!
 * @brief ACOMP result logical status Type definition
 */
typedef enum _acomp_result_logic_status
{
    kACOMP_ResultLogicLow  = 0U, /*!< The comparsion result is high logic. */
    kACOMP_ResultLogicHigh = 1U, /*!< The comparsion result is low logic. */
} acomp_result_logic_status_t;

/*!
 *  @brief ACOMP comparator id.
 */
typedef enum _acomp_comparator_id
{
    kACOMP_Acomp0, /*!< Index for ACOMP0 */
    kACOMP_Acomp1  /*!< Index for ACOMP1 */
} acomp_comparator_id_t;

/*!
 * @brief The enumeration of wave up time.
 */
typedef enum _acomp_warm_up_time
{
    kACOMP_WarmUpTime1us = 0U, /*!< Set wave-up time as 1us. */
    kACOMP_WarmUpTime2us,      /*!< Set wave-up time as 2us. */
    kACOMP_WarmUpTime4us,      /*!< Set wave-up time as 4us. */
    kACOMP_WarmUpTime8us,      /*!< Set wave-up time as 8us. */
} acomp_warm_up_time_t;

/*!
 * @brief The enumeration of response mode. The response mode will affect the delay from input to output.
 */
typedef enum _acomp_response_mode
{
    kACOMP_SlowResponseMode = 0U, /*!< Slow response mode also called power mode 1. */
    kACOMP_MediumResponseMode,    /*!< Medium response mode also called power mode 2. */
    kACOMP_FastResponseMode,      /*!< Fast response mode also called power mode 3. */
} acomp_response_mode_t;

/*!
 * @brief ACOMP interrupt trigger type definition
 */
typedef enum _acomp_interrupt_trigger_type
{
    kACOMP_LowLevelTrig    = 0U, /*!< Low level trigger interrupt. */
    kACOMP_HighLevelTrig   = 1U, /*!< High level trigger interrupt. */
    kACOMP_FallingEdgeTrig = 2U, /*!< Falling edge trigger interrupt. */
    kACOMP_RisingEdgeTrig  = 3U, /*!< Rising edge trigger interrupt. */
} acomp_interrupt_trigger_type_t;

/*!
 *  @brief ACOMP edge pule trigger source type definition
 */
typedef enum _acomp_edge_pulse_trig_source
{
    kACOMP_EdgePulseDis = 0U, /*!< edge pulse function is disable */
    kACOMP_EdgePulseRising,   /*!< Rising edge can trigger edge pulse */
    kACOMP_EdgePulseFalling,  /*!< Falling edge can trigger edge pulse */
    kACOMP_EdgePulseBothEdge, /*!< Both edge can trigger edge pulse */
} acomp_edge_pulse_trig_source_t;

/*!
 *  @brief ACOMP synchronous/asynchronous output type to pin
 */
typedef enum _acomp_pin_out_type
{
    kACOMP_PinOutSyn          = 0U, /*!< Enable ACOMP synchronous pin output */
    kACOMP_PinOutAsyn         = 1U, /*!< Enable ACOMP asynchronous pin output */
    kACOMP_PinOutSynInverted  = 2U, /*!< Enable ACOMP inverted synchronous pin output */
    kACOMP_PinOutAsynInverted = 3U, /*!< Enable ACOMP inverted asynchronous pin output */
    kACOMP_PinOutDisable      = 4U, /*!< Diable ACOMP pin output */
} acomp_pin_out_type_t;

/*!
 *  @brief ACOMP positive channel enumeration
 */
typedef enum _acomp_positive_channel
{
    kACOMP_PosChGPIO42, /*!< Gpio42 selection */
    kACOMP_PosChGPIO43, /*!< Gpio43 selection */
    kACOMP_PosChGPIO44, /*!< Gpio44 selection */
    kACOMP_PosChGPIO45, /*!< Gpio45 selection */
    kACOMP_PosChGPIO46, /*!< Gpio46 selection */
    kACOMP_PosChGPIO47, /*!< Gpio47 selection */
    kACOMP_PosChGPIO48, /*!< Gpio48 selection */
    kACOMP_PosChGPIO49, /*!< Gpio49 selection */
    kACOMP_PosChDACA,   /*!< DACA selection */
    kACOMP_PosChDACB,   /*!< DACB selection */
} acomp_positive_channel_t;

/*!
 *  @brief ACOMP negative channel enumeration.
 */
typedef enum _acomp_negative_channel
{
    kACOMP_NegChGPIO42,   /*!< Gpio42 selection */
    kACOMP_NegChGPIO43,   /*!< Gpio43 selection */
    kACOMP_NegChGPIO44,   /*!< Gpio44 selection */
    kACOMP_NegChGPIO45,   /*!< Gpio45 selection */
    kACOMP_NegChGPIO46,   /*!< Gpio46 selection */
    kACOMP_NegChGPIO47,   /*!< Gpio47 selection */
    kACOMP_NegChGPIO48,   /*!< Gpio48 selection */
    kACOMP_NegChGPIO49,   /*!< Gpio49 selection */
    kACOMP_NegChDACA,     /*!< DACA selection */
    kACOMP_NegChDACB,     /*!< DACB selection */
    kACOMP_NegChVREF1P2,  /*!< Vref1p2 selection */
    kACOMP_NegChAVSS,     /*!< AVSS selection */
    kACOMP_NegChVIO_0P25, /*!< VIO Scaling factor 0.25 */
    kACOMP_NegChVIO_0P50, /*!< VIO Scaling factor 0.50 */
    kACOMP_NegChVIO_0P75, /*!< VIO Scaling factor 0.75 */
    kACOMP_NegChVIO_1P00  /*!< VIO Scaling factor 1.00 */
} acomp_negative_channel_t;

/*!
 *  @brief ACOMP hysteresis level enumeration
 */
typedef enum _acomp_input_hysteresis
{
    kACOMP_Hyster0MV,  /*!< Hysteresis level = 0mv */
    kACOMP_Hyster10MV, /*!< Hysteresis level = 10mv */
    kACOMP_Hyster20MV, /*!< Hysteresis level = 20mv */
    kACOMP_Hyster30MV, /*!< Hysteresis level = 30mv */
    kACOMP_Hyster40MV, /*!< Hysteresis level = 40mv */
    kACOMP_Hyster50MV, /*!< Hysteresis level = 50mv */
    kACOMP_Hyster60MV, /*!< Hysteresis level = 60mv */
    kACOMP_Hyster70MV, /*!< Hysteresis level = 70mv */
} acomp_input_hysteresis_t;

/*!
 * @brief The configuration of positive input, including channel selection and hysteresis level.
 */
typedef struct _acomp_positive_input_config
{
    acomp_positive_channel_t channel : 4U;     /*!< Positive input channel selection,
                                              please refer to @ref acomp_positive_channel_t. */
    acomp_input_hysteresis_t hysterLevel : 4U; /*!< Positive hysteresis voltage level selection,
                                              please refer to @ref acomp_input_hysteresis_t. */
} acomp_positive_input_config_t;

/*!
 * @brief The configuration of negative input, including channel selection and hysteresis level.
 */
typedef struct _acomp_negative_input_config
{
    acomp_negative_channel_t channel : 4U;     /*!< Negative input channel selection,
                                              please refer to @ref acomp_negative_channel_t. */
    acomp_input_hysteresis_t hysterLevel : 4U; /*!< Negative hystersis voltage level selection,
                                              please refer to @ref acomp_input_hysteresis_t. */
} acomp_negative_input_config_t;

/*!
 * @brief The configure structure of acomp, including warm up time, response mode and so on.
 */
typedef struct _acomp_config
{
    acomp_comparator_id_t id : 1U;           /*!< The id of comparator, please refer to @ref acomp_comparator_id_t. */
    bool enable : 1U;                        /*!< Enable/Disable the selected ACOMP.
                                              - \b true Enable the selected ACOMP.
                                              - \b false Disable the selected ACOMP. */
    acomp_warm_up_time_t warmupTime : 2U;    /*!< Configure warm-up time, please refer to @ref acomp_warm_up_time_t. */
    acomp_response_mode_t responseMode : 2U; /*!< Configure responde mode(power mode),
                                            please refer to @ref acomp_response_mode_t for details. */
    acomp_interrupt_trigger_type_t intTrigType : 2U;       /*!< Select interrupt trigger type,
                                                          please refer to @ref acomp_interrupt_trigger_type_t. */
    acomp_result_logic_status_t inactiveValue : 1U;        /*!< Configure output value for inactive state. */
    acomp_edge_pulse_trig_source_t edgeDetectTrigSrc : 2U; /*!< Config edge detect trigger source,
                                                          please refer to @ref acomp_edge_pulse_trig_source_t. */
    acomp_pin_out_type_t outPinMode : 3U;                  /*!< Config the output pin mode,
                                                          please refer to @ref acomp_pin_out_type_t for details. */
    const acomp_positive_input_config_t *posInput; /*!< The pointer to the configuration structure of positive input,
                                                 please refer to @ref acomp_positive_input_config_t. */
    const acomp_negative_input_config_t *negInput; /*!< The pointer to the configuration structure of negative input,
                                                 please refer to @ref acomp_positive_input_config_t. */
} acomp_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name ACOMP Control Interfaces
 * @{
 */

/*!
 * @brief Initializes the module, including warm up time, response mode, inactive value and so on.
 *
 * @param base ACOMP peripheral base address.
 * @param config The pointer to the structure @ref acomp_config_t.
 */
void ACOMP_Init(ACOMP_Type *base, const acomp_config_t *config);

/*!
 * @brief Gets the default configuration of ACOMP module.
 *
 * @code
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
 * @endcode
 * @param config The pointer to the structure @ref acomp_config_t.
 */
void ACOMP_GetDefaultConfig(acomp_config_t *config);

/*!
 * @brief De-initializes the module.
 *
 * @param base ACOMP peripheral base address.
 */
void ACOMP_Deinit(ACOMP_Type *base);

/*!
 * @brief Configures selected comparator's inputs, inclduing input channel and hysteresis level.
 *
 * @param base ACOMP peripheral base address.
 * @param id The selected acomp comparator's id, please refer to @ref acomp_comparator_id_t.
 * @param posInput The configuration of selected comparator's positive input,
 *                  please refer to @ref acomp_positive_input_config_t.
 * @param negInput The configuration of selected comparator's negative input,
 *                  please refer to @ref acomp_negative_input_config_t.
 */
void ACOMP_SetInputConfig(ACOMP_Type *base,
                          acomp_comparator_id_t id,
                          const acomp_positive_input_config_t *posInput,
                          const acomp_negative_input_config_t *negInput);

/*!
 * @brief Does software reset to the selected ACOMP module.
 *
 * @param base ACOMP peripheral base address.
 * @param id The selected acomp comparator's id, please refer to @ref acomp_comparator_id_t.
 */
static inline void ACOMP_DoSoftwareReset(ACOMP_Type *base, acomp_comparator_id_t id)
{
    ACOMP_SET_REG_BIT(&(base->RST0), id, ACOMP_RST0_SOFT_RST_MASK);
    /* Delay */
    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    ACOMP_CLEAR_REG_BIT(&(base->RST0), id, ACOMP_RST0_SOFT_RST_MASK);
}

/*!
 * @brief Enables/Disables ACOMP module.
 *
 * @param base ACOMP peripheral base address.
 * @param id The selected acomp comparator's id, please refer to @ref acomp_comparator_id_t.
 * @param enable Used to enable/disable module.
 *              - \b true Enable comparator instance.
 *              - \b false Disable comparator instance.
 */
static inline void ACOMP_Enable(ACOMP_Type *base, acomp_comparator_id_t id, bool enable)
{
    if (enable)
    {
        ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id) |= ACOMP_CTRL1_MUXEN_MASK;
        ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id) |= ACOMP_CTRL1_EN_MASK;
    }
    else
    {
        ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id) &= ~ACOMP_CTRL1_MUXEN_MASK;
        ACOMP_GET_REG_VAL(&(base->CTRL0), (uint8_t)id) &= ~ACOMP_CTRL1_EN_MASK;
    }
}

/*!
 * @brief Resets clock divider.
 *
 * @param base ACOMP peripheral base address.
 */
static inline void ACOMP_ResetClockDivider(ACOMP_Type *base)
{
    base->CLK |= ACOMP_CLK_SOFT_CLK_RST_MASK;
    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->CLK &= ~ACOMP_CLK_SOFT_CLK_RST_MASK;
}

/*! @} */

/*!
 * @name ACOMP Result Interface
 * @{
 */

/*!
 * @brief Gets the selected acomp conversion result.
 *
 * @param base ACOMP peripheral base address.
 * @param id The selected acomp comparator's id, please refer to @ref acomp_comparator_id_t.
 * @return The result of the selected acomp instance.
 */
static inline acomp_result_logic_status_t ACOMP_GetResult(ACOMP_Type *base, acomp_comparator_id_t id)
{
    return (acomp_result_logic_status_t)((ACOMP_GET_REG_VAL(&(base->STATUS0), id) & ACOMP_STATUS0_OUT_MASK) >>
                                         ACOMP_STATUS0_OUT_SHIFT);
}

/*! @} */

/*!
 * @brief ACOMP Interrupt Control Interfaces
 * @{
 */

/*!
 * @brief Enables interrupts, including acomp0 asynchronized interrupt, acomp0 synchronized interrupt, acomp1
 * asynchronized interrupt, and acomp1 synchronized interrupt.
 *
 * @param base ACOMP peripheral base address.
 * @param interruptMask The OR'ed value of the interrupts to be enabled, please refer to @ref _acomp_interrupt_enable.
 */
static inline void ACOMP_EnableInterrupts(ACOMP_Type *base, uint32_t interruptMask)
{
    base->IMR0 &= ~(interruptMask & 0x3UL);
    base->IMR1 &= ~((interruptMask >> 2UL) & 0x3UL);
}

/*!
 * @brief Disables interrupts, including acomp0 asynchronized interrupt, acomp0 synchronized interrupt, acomp1
 * asynchronized interrupt, and acomp1 synchronized interrupt.
 *
 * @param base ACOMP peripheral base address.
 * @param interruptMask The OR'ed value of the interrupts to be disabled, please refer to @ref _acomp_interrupt_enable.
 */
static inline void ACOMP_DisableInterrupt(ACOMP_Type *base, uint32_t interruptMask)
{
    base->IMR0 |= (interruptMask & 0x3UL);
    base->IMR1 |= ((interruptMask >> 2UL) & 0x3UL);
}

/*! @} */

/*!
 * @brief ACOMP Status Flag Interfaces
 * @{
 */

/*!
 * @brief Gets status flags, such as ACOMP0 active status flags, ACOMP1 active status flags, and so on.
 *
 * @param base ACOMP peripheral base address.
 * @return The OR'ed value ACOMP status flags, please refer to @ref _acomp_status_flags for details.
 */
uint32_t ACOMP_GetStatusFlags(ACOMP_Type *base);

/*!
 * @brief Clears status flags that can be cleared by software.
 *
 * @note Only @ref kACOMP_Acomp0OutInterruptFlag, @ref kACOMP_Acomp0OutAInterruptFlag, @ref
 * kACOMP_Acomp1OutInterruptFlag, and @ref kACOMP_Acomp1OutAInterruptFlag can be cleared by software.
 *
 * @param base ACOMP peripheral base address.
 * @param statusFlagMask The OR'ed value of the status flags that can be cleared.
 */
static inline void ACOMP_ClearStatusFlags(ACOMP_Type *base, uint32_t statusFlagMask)
{
    assert((statusFlagMask & (kACOMP_Acomp0ActiveFlag | kACOMP_Acomp1ActiveFlag | kACOMP_Acomp0RawOutInterruptFlag |
                              kACOMP_Acomp1RawOutInterruptFlag | kACOMP_Acomp0RawOutAInterruptFlag |
                              kACOMP_Acomp1RawOutAInterruptFlag)) == 0UL);

    base->ICR0 = (statusFlagMask & (kACOMP_Acomp0OutInterruptFlag | kACOMP_Acomp0OutAInterruptFlag)) >> 1UL;
    base->ICR1 = (statusFlagMask & (kACOMP_Acomp1OutInterruptFlag | kACOMP_Acomp1OutAInterruptFlag)) >> 6UL;
}

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_ACOMP_H_ */
