/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_GPT_H_
#define _FSL_GPT_H_

#include "fsl_common.h"

/*!
 * @addtogroup gpt
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief GPT driver version 2.0.0. */
#define FSL_GPT_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!
 * @brief List of GPT channels
 * @note Actual number of available channels is SoC dependent
 */
typedef enum _gpt_chnl
{
    kGPT_Chnl_0 = 0U, /*!< GPT channel number 0*/
    kGPT_Chnl_1,      /*!< GPT channel number 1 */
    kGPT_Chnl_2,      /*!< GPT channel number 2 */
    kGPT_Chnl_3,      /*!< GPT channel number 3 */
    kGPT_Chnl_4,      /*!< GPT channel number 4 */
    kGPT_Chnl_5,      /*!< GPT channel number 5 */
} gpt_chnl_t;

/*! @brief GPT channel polarity */
typedef enum _gpt_chnl_pol
{
    kGPT_ChnlPol_Low = 0U, /*!< Output state reset to 0 */
    kGPT_ChnlPol_High,     /*!< Output state reset to 1 */
} gpt_chnl_pol_t;

/*! @brief GPT PWM operation modes */
typedef enum _gpt_pwm_mode
{
    kGPT_EdgeAlignedPwm = 0U, /*!< Edge-aligned PWM */
    kGPT_CenterAlignedPwm,    /*!< Center-aligned PWM */
} gpt_pwm_mode_t;

/*! @brief GPT input capture edge */
typedef enum _gpt_input_capture_edge
{
    kGPT_RisingEdge = 0U, /*!< Capture on rising edge only */
    kGPT_FallingEdge,     /*!< Capture on falling edge only */
} gpt_input_capture_edge_t;

/*! @brief GPT input capture edge */
typedef enum _gpt_input_capture_filter
{
    kGPT_InputCapFilter_None = 0U, /*!< No filter */
    kGPT_InputCapFilter_2Cycles,   /*!< Filter 2 consecutive cycles */
    kGPT_InputCapFilter_3Cycles,   /*!< Filter 3 consecutive cycles */
    kGPT_InputCapFilter_4Cycles,   /*!< Filter 4 consecutive cycles */
    kGPT_InputCapFilter_5Cycles,   /*!< Filter 5 consecutive cycles */
    kGPT_InputCapFilter_6Cycles,   /*!< Filter 6 consecutive cycles */
    kGPT_InputCapFilter_7Cycles,   /*!< Filter 7 consecutive cycles */
} gpt_input_capture_filter_t;

/*! @brief GPT clock source selection*/
typedef enum _gpt_clock_source
{
    kGPT_SystemClock = 0U, /*!< System clock from PMU selected */
    kGPT_ExternalClock     /*!< External clock from pad */
} gpt_clock_source_t;

/*! @brief GPT clock divider factor selection for the clock source. */
typedef enum _gpt_clock_div
{
    kGPT_ClkDiv_Divide_1 = 0U, /*!< Divide by 1 */
    kGPT_ClkDiv_Divide_2,      /*!< Divide by 2 */
    kGPT_ClkDiv_Divide_4,      /*!< Divide by 4 */
    kGPT_ClkDiv_Divide_8,      /*!< Divide by 8 */
    kGPT_ClkDiv_Divide_16,     /*!< Divide by 16 */
    kGPT_ClkDiv_Divide_32,     /*!< Divide by 32 */
    kGPT_ClkDiv_Divide_64,     /*!< Divide by 64 */
    kGPT_ClkDiv_Divide_128,    /*!< Divide by 128 */
    kGPT_ClkDiv_Divide_256,    /*!< Divide by 256 */
    kGPT_ClkDiv_Divide_512,    /*!< Divide by 512 */
    kGPT_ClkDiv_Divide_1024,   /*!< Divide by 1024 */
    kGPT_ClkDiv_Divide_2048,   /*!< Divide by 2048 */
    kGPT_ClkDiv_Divide_4096,   /*!< Divide by 4096 */
    kGPT_ClkDiv_Divide_8192,   /*!< Divide by 8192 */
    kGPT_ClkDiv_Divide_16384,  /*!< Divide by 16384 */
    kGPT_ClkDiv_Divide_32768,  /*!< Divide by 32768 */
} gpt_clock_div_t;

/*!
 * @brief List of GPT interrupts
 */
typedef enum _gpt_interrupt_enable
{
    kGPT_Chnl0InterruptEnable = (1U << 0), /*!< Channel 0 interrupt */
    kGPT_Chnl1InterruptEnable = (1U << 1), /*!< Channel 1 interrupt */
    kGPT_Chnl2InterruptEnable = (1U << 2), /*!< Channel 2 interrupt */
    kGPT_Chnl3InterruptEnable = (1U << 3), /*!< Channel 3 interrupt */
    kGPT_Chnl4InterruptEnable = (1U << 4), /*!< Channel 4 interrupt */
    kGPT_Chnl5InterruptEnable = (1U << 5), /*!< Channel 5 interrupt */

    kGPT_Chnl0ErrInterruptEnable = (1U << 8),  /*!< Channel 0 error interrupt */
    kGPT_Chnl1ErrInterruptEnable = (1U << 9),  /*!< Channel 1 error interrupt */
    kGPT_Chnl2ErrInterruptEnable = (1U << 10), /*!< Channel 2 error interrupt */
    kGPT_Chnl3ErrInterruptEnable = (1U << 11), /*!< Channel 3 error interrupt */
    kGPT_Chnl4ErrInterruptEnable = (1U << 12), /*!< Channel 4 error interrupt */
    kGPT_Chnl5ErrInterruptEnable = (1U << 13), /*!< Channel 5 error interrupt */

    kGPT_TimeOverflowInterruptEnable = (1U << 16), /*!< Time overflow interrupt */

    kGPT_Dma0OverflowInterruptEnable = (1U << 24), /*!< DMA0 overflow interrupt */
    kGPT_Dma1OverflowInterruptEnable = (1U << 25), /*!< DMA1 overflow interrupt */

    kGPT_AllInterruptsEnable = 0x3013F3FUL, /*!< All interrupts */
} gpt_interrupt_enable_t;

/*!
 * @brief List of GPT flags
 */
typedef enum _gpt_status_flags
{
    kGPT_Chnl0Flag = (1U << 0), /*!< Channel 0 flag */
    kGPT_Chnl1Flag = (1U << 1), /*!< Channel 1 flag */
    kGPT_Chnl2Flag = (1U << 2), /*!< Channel 2 flag */
    kGPT_Chnl3Flag = (1U << 3), /*!< Channel 3 flag */
    kGPT_Chnl4Flag = (1U << 4), /*!< Channel 4 flag */
    kGPT_Chnl5Flag = (1U << 5), /*!< Channel 5 flag */

    kGPT_Chnl0ErrFlag = (1U << 8),  /*!< Channel 0 error flag */
    kGPT_Chnl1ErrFlag = (1U << 9),  /*!< Channel 1 error flag */
    kGPT_Chnl2ErrFlag = (1U << 10), /*!< Channel 2 error flag */
    kGPT_Chnl3ErrFlag = (1U << 11), /*!< Channel 3 error flag */
    kGPT_Chnl4ErrFlag = (1U << 12), /*!< Channel 4 error flag */
    kGPT_Chnl5ErrFlag = (1U << 13), /*!< Channel 5 error flag */

    kGPT_TimeOverflowFlag = (1U << 16), /*!< Time overflow flag */

    kGPT_Dma0OverflowFlag = (1U << 24), /*!< DMA0 overflow flag */
    kGPT_Dma1OverflowFlag = (1U << 25), /*!< DMA1 overflow flag */

    kGPT_AllFlags = 0x3013F3FUL, /*!< All flags */
} gpt_status_flags_t;

/*! @brief GPT counter update mode */
typedef enum _gpt_cnt_update_mode
{
    kGPT_CntUpdateMode_Normal = 0U, /*!< Used in any ratio between counter clock and APB clock. Every 3-4 counter ticks
                                       are updated to CNT_VAL */
    kGPT_CntUpdateMode_Fast = 1U, /*!< Used when counter clock is at least 5 times slower than APB clock. Every counter
                                     tick is updated to CNT_VAL */
    kGPT_CntUpdateMode_Off = 3U,  /*!< Do not update to CNT_VAL */
} gpt_cnt_update_mode_t;

/*! @brief GPT sample clock divider factor selection. */
typedef enum _gpt_input_capture_div
{
    kGPT_SampClkDiv_Divide_1 = 0U, /*!< Divide by 1 */
    kGPT_SampClkDiv_Divide_2,      /*!< Divide by 2 */
    kGPT_SampClkDiv_Divide_4,      /*!< Divide by 4 */
    kGPT_SampClkDiv_Divide_8,      /*!< Divide by 8 */
    kGPT_SampClkDiv_Divide_16,     /*!< Divide by 16 */
    kGPT_SampClkDiv_Divide_32,     /*!< Divide by 32 */
    kGPT_SampClkDiv_Divide_64,     /*!< Divide by 64 */
    kGPT_SampClkDiv_Divide_128,    /*!< Divide by 128 */
} gpt_input_capture_div_t;

/*!
 * @brief GPT configuration structure
 *
 * This structure holds the configuration settings for the GPT peripheral. To initialize this
 * structure to reasonable defaults, call the GPT_GetDefaultConfig() function and pass a
 * pointer to the configuration structure instance.
 *
 * The configuration structure can be made constant so as to reside in flash.
 */
typedef struct _gpt_config
{
    gpt_clock_source_t src;              /*!< GPT clock source selection. */
    gpt_clock_div_t div;                 /*!< GPT clock divider value for counter. */
    uint8_t prescale;                    /*!< GPT clock prescale value for counter, divided by (prescale + 1) */
    gpt_cnt_update_mode_t cntUpdateMode; /*!< Counter value update mode. */
    bool stopCntInDebug;                 /*!< true: In debug mode, stop the counters.
                                              false: In debug mode, counters are not affected. */
    uint32_t cntUppVal;                  /*!< If counter reaches this value, the counter will overflow to 0. */
    gpt_input_capture_div_t icDiv;       /*!< Input capture sample clock divider. */
    gpt_input_capture_filter_t icFilter; /*!< Input capture filter cycles in sampling ticks. */
} gpt_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Ungates the GPT clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application which is using the GPT driver.
 *       If the GPT instance has only TPM features, please use the TPM driver.
 *
 * @param base   GPT peripheral base address
 * @param config Pointer to the user configuration structure.
 *
 * @return kStatus_Success indicates success; Else indicates failure.
 */
status_t GPT_Init(GPT_Type *base, const gpt_config_t *config);

/*!
 * @brief Gates the GPT clock.
 *
 * @param base GPT peripheral base address
 */
void GPT_Deinit(GPT_Type *base);

/*!
 * @brief  Fills in the GPT configuration structure with the default settings.
 *
 * The default values are:
 * @code
 *   config->src = kGPT_SystemClock;
 *   config->div = kGPT_ClkDiv_Divide_1;
 *   config->prescale = 0U;
 *   config->cntUpdateMode = kGPT_CntUpdateMode_Normal;
 *   config->stopCntInDebug = true;
 *   config->cntUppVal = 0xFFFFFFFFU;
 *   config->icDiv = kGPT_SampClkDiv_Divide_1;
 *   config->icFilter = kGPT_InputCapFilter_None;
 * @endcode
 * @param config Pointer to the user configuration structure.
 */
void GPT_GetDefaultConfig(gpt_config_t *config);

/*! @}*/

/*!
 * @name Channel mode operations
 * @{
 */

/*!
 * @brief Enables capturing an input signal on the channel using the function parameters.
 *
 * When the edge specified in the captureMode argument occurs on the channel, the GPT counter is
 * captured into the CMR0 register. The user has to read the CMR0 register separately to get this
 * value.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param captureMode Specifies which edge to capture
 */
void GPT_SetupInputCapture(GPT_Type *base, gpt_chnl_t chnlNumber, gpt_input_capture_edge_t captureMode);

/*!
 * @brief Trigger input capturing by software. Edge and filter is ignored.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 */
void GPT_TriggerInputCapture(GPT_Type *base, gpt_chnl_t chnlNumber);

/*!
 * @brief Get captured value.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @return The captured counter value.
 */
uint32_t GPT_GetCapturedValue(GPT_Type *base, gpt_chnl_t chnlNumber);

/*!
 * @brief Configures one-shot pulse on the channel.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param pol         Channel output polarity reset value.
 * @param delayTicks  Delay width before triggering the pulse, the uint of this value is timer ticks.
 * @param dutyTicks   Pulse width, the uint of this value is timer ticks.
 */
void GPT_SetupOneshotPulse(
    GPT_Type *base, gpt_chnl_t chnlNumber, gpt_chnl_pol_t pol, uint32_t delayTicks, uint32_t dutyTicks);

/*!
 * @brief Configures one-shot edge on the channel.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param pol         Channel output polarity reset value.
 * @param delayTicks  Delay width before changing the edge, the uint of this value is timer ticks.
 */
void GPT_SetupOneshotEdge(GPT_Type *base, gpt_chnl_t chnlNumber, gpt_chnl_pol_t pol, uint32_t delayTicks);

/*!
 * @brief Configures the PWM signal parameters.
 *
 * Call this function to configure the PWM signal period, mode, duty cycle, and edge. Use this
 * function to configure all GPT channels that are used to output a PWM signal.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number.
 * @param pol         PWM output polarity reset value.
 * @param dutyPercent PWM pulse width, value should be between 0 to 100
 * @param mode        PWM operation mode, options available in enumeration ::gpt_pwm_mode_t
 * @param pwmFreq_Hz  PWM signal frequency in Hz
 * @param srcClock_Hz GPT counter clock in Hz
 *
 * @return kStatus_Success if the PWM setup was successful
 *         kStatus_Error on failure
 */
status_t GPT_SetupPwm(GPT_Type *base,
                      gpt_chnl_t chnlNumber,
                      gpt_chnl_pol_t pol,
                      uint32_t dutyPercent,
                      gpt_pwm_mode_t mode,
                      uint32_t pwmFreq_Hz,
                      uint32_t srcClock_Hz);

/*!
 * @brief Configures the PWM mode parameters.
 *
 * Call this function to configure the PWM signal mode, duty cycle in ticks, and edge. Use this
 * function to configure all GPT channels that are used to output a PWM signal.
 * Please note that: This API is similar with GPT_SetupPwm() API, but will not set the timer period,
 *                   and this API will set channel match value in timer ticks, not period percent.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number.
 * @param pol         PWM output polarity reset value.
 * @param periodTicks PWM period width, the uint of this value is timer ticks.
 * @param dutyTicks   PWM pulse width, the uint of this value is timer ticks.
 * @param mode        PWM operation mode, options available in enumeration ::gpt_pwm_mode_t
 *
 * @return kStatus_Success if the PWM setup was successful
 *         kStatus_Error on failure
 */
status_t GPT_SetupPwmMode(GPT_Type *base,
                          gpt_chnl_t chnlNumber,
                          gpt_chnl_pol_t pol,
                          uint32_t periodTicks,
                          uint32_t dutyTicks,
                          gpt_pwm_mode_t mode);

/*!
 * @brief Updates the duty cycle of an active PWM signal.
 *
 * @param base              GPT peripheral base address
 * @param chnlNumber        The channel number.
 * @param dutyPercent       New PWM pulse width; The value should be between 0 to 100
 *                          0=inactive signal(0% duty cycle)...
 *                          100=active signal (100% duty cycle)
 *
 * @return kStatus_Success if the PWM update was successful
 *         kStatus_Error on failure
 */
status_t GPT_UpdatePwmDutycycle(GPT_Type *base, gpt_chnl_t chnlNumber, uint8_t dutyPercent);

/*!
 * @brief Pause the PWM signal.
 *
 * @param base              GPT peripheral base address
 * @param chnlNumber        The channel number.
 *
 * @return kStatus_Success if the PWM update was successful
 *         kStatus_Error on failure
 */
status_t GPT_PausePwm(GPT_Type *base, gpt_chnl_t chnlNumber);

/*! @}*/

/*!
 * @name Interrupt Interface
 * @{
 */

/*!
 * @brief Enables the selected GPT interrupts.
 *
 * @param base GPT peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::gpt_interrupt_enable_t
 */
void GPT_EnableInterrupts(GPT_Type *base, uint32_t mask);

/*!
 * @brief Disables the selected GPT interrupts.
 *
 * @param base GPT peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::gpt_interrupt_enable_t
 */
void GPT_DisableInterrupts(GPT_Type *base, uint32_t mask);

/*!
 * @brief Gets the enabled GPT interrupts.
 *
 * @param base GPT peripheral base address
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::gpt_interrupt_enable_t
 */
uint32_t GPT_GetEnabledInterrupts(GPT_Type *base);

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the GPT status flags.
 *
 * @param base GPT peripheral base address
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::gpt_status_flags_t
 */
uint32_t GPT_GetStatusFlags(GPT_Type *base);

/*!
 * @brief Clears the GPT status flags.
 *
 * @param base GPT peripheral base address
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::gpt_status_flags_t
 */
void GPT_ClearStatusFlags(GPT_Type *base, uint32_t mask);

/*! @}*/

/*!
 * @name Read and write the timer period
 * @{
 */

/*!
 * @brief Sets the timer period in units of ticks for the GPT instance.
 *
 * Timers counts from 0 until it equals the count value set here. The count value is written to
 * the CNT_UPP_VAL register.
 * The period update take effects during the next counter-reach-upper event. To guarantee an
 * immediate update, use GPT_ResetTimer() after setting the period.
 *
 * @note
 * Call the utility macros provided in the fsl_common.h to convert usec or msec to ticks.
 *
 * @param base GPT peripheral base address
 * @param ticks A timer period in units of ticks, which should be equal or greater than 1.
 */
void GPT_SetTimerPeriod(GPT_Type *base, uint32_t ticks);

/*!
 * @brief Reads the current timer counting value.
 *
 * This function returns the real-time timer counting value in a range from 0 to a
 * timer period.
 *
 * @note Call the utility macros provided in the fsl_common.h to convert ticks to usec or msec.
 *
 * @param base GPT peripheral base address
 *
 * @return The current counter value in ticks
 */
uint32_t GPT_GetCurrentTimerCount(GPT_Type *base);

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the GPT counter.
 *
 * @param base        GPT peripheral base address
 */
void GPT_StartTimer(GPT_Type *base);

/*!
 * @brief Stops the GPT counter.
 *
 * @param base GPT peripheral base address
 */
void GPT_StopTimer(GPT_Type *base);

/*!
 * @brief Performs a GPT counter reset.
 *
 * @param base GPT peripheral base address
 */
void GPT_ResetTimer(GPT_Type *base);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_GPT_H_*/
