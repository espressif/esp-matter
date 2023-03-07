/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * Pulse width modulation
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "gpBsp.h"
#include "gpLog.h"
#include "gpHal.h"
#include "hal_DMA.h"
#include "hal_timer.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_PWM_NR_OF_PWM_CHANNELS  8

// Used to mark unused PWM in halPWM_ChannelGpioMap
#define HAL_UART_GPIO_INVALID       0xff

// Input clock frequency into timer prescalers (16 MHz)
#define HAL_PWM_PRESCALER_FREQUENCY 16000000UL

// Default PWM frequency.
#define HAL_PWM_FREQUENCY_250_HZ    250
#ifndef HAL_PWM_FREQUENCY
#define HAL_PWM_FREQUENCY HAL_PWM_FREQUENCY_250_HZ
#endif //HAL_PWM_FREQUENCY


// DMA buffer size in bytes for PCM playback.
#define DMA_TX_BUF_SIZE             512

#define HAL_TIMESTAMP_COUNTER       halTimer_timer2
#define HAL_PWM_MAIN_COUNTER        halTimer_timer3
#define HAL_PWM_CARRIER_COUNTER     halTimer_timer4

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/

#ifdef HAL_DIVERSITY_PWM_WITH_DMA
static hal_DmaChannel_t halPWM_DmaChannel = HAL_DMA_CHANNEL_INVALID;
static Bool   halPWM_DmaActive;
static UInt8  halPWM_PcmChannel;
static Bool   halPWM_PcmWordMode;
static UInt8  halPWM_DmaBuffer[DMA_TX_BUF_SIZE];
static hal_DmaPointer_t halPWM_DmaWritePtr;
#endif // HAL_DIVERSITY_PWM_WITH_DMA

// PWM output pin information from BSP.
static const UInt8 halPWM_ChannelGpioMap[HAL_PWM_NR_OF_PWM_CHANNELS] = GP_BSP_PWM_GPIO_MAP;
static const UInt8 halPWM_ChannelAlternateMap[HAL_PWM_NR_OF_PWM_CHANNELS] = GP_BSP_PWM_ALTERNATE_MAP;
static const UInt8 halPWM_ChannelDriveMap[HAL_PWM_NR_OF_PWM_CHANNELS] = GP_BSP_PWM_DRIVE_MAP;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* Set main counter prescaler and wrap value. */
static void halPWM_SetMainCounterPeriod(UInt8 prescalerDiv, UInt16 wrap)
{
    halTimer_setPrescalerDiv(HAL_PWM_MAIN_COUNTER, prescalerDiv);
    halTimer_setThreshold(HAL_PWM_MAIN_COUNTER, wrap);
}

/* Set carrier counter prescaler and wrap value. */
static void halPWM_SetCarrierCounterPeriod(UInt8 prescalerDiv, UInt16 wrap)
{
    halTimer_setPrescalerDiv(HAL_PWM_CARRIER_COUNTER, prescalerDiv);
    halTimer_setThreshold(HAL_PWM_CARRIER_COUNTER, wrap);
}

/* Return PWM base register address for specified channel number. */
static INLINE UInt32 halPWM_BaseAddress(UInt8 channel)
{
    return GP_WB_PWMS_PWM0_UP_DOWN_ENABLE_ADDRESS +
           (UInt32)channel * (GP_WB_PWMS_PWM1_UP_DOWN_ENABLE_ADDRESS - GP_WB_PWMS_PWM0_UP_DOWN_ENABLE_ADDRESS);
}

/* Return true if PWM channel is in up/down mode. */
static Bool halPWM_GetUpDownEnable(UInt8 channel)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    return GP_WB_READ_PWM_UP_DOWN_ENABLE(halPWM_BaseAddress(channel));
}

/* Enable/disable up/down mode of PWM channel. */
static void halPWM_SetUpDownEnable(UInt8 channel, Bool enable)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_WB_WRITE_PWM_UP_DOWN_ENABLE(halPWM_BaseAddress(channel), enable);
}

/* Set the drive type for the PWM output pin. */
static void halPWM_SetOutputDrive(UInt8 channel, UInt8 drive)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_WB_WRITE_PWM_OUTPUT_DRIVE(halPWM_BaseAddress(channel), drive);
}

/* Enable/disable inverted output. */
static void halPWM_SetOutputInvert(UInt8 channel, Bool invert)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_WB_WRITE_PWM_OUTPUT_INVERT(halPWM_BaseAddress(channel), invert);
}

/* Return the threshold of the PWM comparator. */
static UInt16 halPWM_GetThreshold(UInt8 channel)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    return GP_WB_READ_PWM_THRESHOLD(halPWM_BaseAddress(channel));
}

/* Update the threshold of the PWM comparator. */
static void halPWM_UpdateThreshold(UInt8 channel, UInt16 threshold)
{
    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    // Write to next_threshold register, then trigger update for selected channel.
    GP_WB_WRITE_PWMS_NEXT_THRESHOLD(threshold);
    GP_WB_PWM_THRESHOLD_UPDATE(halPWM_BaseAddress(channel));
}

/* Enable or disable PWM alternate function of GPIO pin. */
static void halPWM_SetPwmGpioEnabled(UInt8 channel, Bool enable)
{
    UInt8 gpio;
    UInt8 alternate;
    UInt8 regval;

    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    gpio = halPWM_ChannelGpioMap[channel];
    alternate = halPWM_ChannelAlternateMap[channel];
    if (gpio != HAL_UART_GPIO_INVALID)
    {
        regval = (((UInt8)enable) << GP_WB_IOB_GPIO_0_ALTERNATE_ENABLE_LSB) |
                 (alternate       << GP_WB_IOB_GPIO_0_ALTERNATE_LSB);
        GP_WB_WRITE_U8(GP_WB_IOB_GPIO_0_ALTERNATE_ENABLE_ADDRESS + gpio, regval);
    }
}

/* Returns if a the PWM alternate function is set of GPIO pin.*/
static Bool halPWM_IsPwmGpioEnabled(UInt8 channel)
{
    UInt8 gpio;
    UInt8 alternate;
    UInt8 regval;
    Bool enable = true;

    GP_ASSERT_DEV_INT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    gpio = halPWM_ChannelGpioMap[channel];
    alternate = halPWM_ChannelAlternateMap[channel];
    if (gpio != HAL_UART_GPIO_INVALID)
    {
        regval = (((UInt8)enable) << GP_WB_IOB_GPIO_0_ALTERNATE_ENABLE_LSB) |
                 (alternate       << GP_WB_IOB_GPIO_0_ALTERNATE_LSB);
        return (GP_WB_READ_U8(GP_WB_IOB_GPIO_0_ALTERNATE_ENABLE_ADDRESS + gpio) == regval);
    }

    // No GPIO was mapped to this channel, so it can't be enabled
    return false;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void hal_InitPWM(void)
{
    UIntLoop i;

    // Configure timer inputs.
    // TMR3 (main counter) counts on internal clock.
    // TMR4 (carrier counter) counts on TMR3 wrap.
    halTimer_initTimer(HAL_PWM_MAIN_COUNTER, 0, halTimer_clkSelIntClk, 0, NULL, false);
    halTimer_initTimer(HAL_PWM_CARRIER_COUNTER, 0, halTimer_clkSelTmr3, 0, NULL, false);

    // Configure PWM timer selection.
    // TMR3 = main counter and timestamp counter
    // TMR4 = carrier counter
    GP_WB_WRITE_PWMS_MAIN_TMR(HAL_PWM_MAIN_COUNTER);
    GP_WB_WRITE_PWMS_CARRIER_TMR(HAL_PWM_CARRIER_COUNTER);
    GP_WB_WRITE_PWMS_TIMESTAMP_TMR(HAL_TIMESTAMP_COUNTER);

    // Disable PWM.
    hal_EnablePwm(false);

    // Enable PWM function on associated GPIOs.
    for (i = 0; i < HAL_PWM_NR_OF_PWM_CHANNELS; i++)
    {
        halPWM_UpdateThreshold(i, 0);
        halPWM_SetOutputDrive(i, halPWM_ChannelDriveMap[i]);
        halPWM_SetPwmGpioEnabled(i, true);
    }

    // Set default PWM frequency.
    hal_SetFrequencyPwm(HAL_PWM_FREQUENCY);

#ifdef HAL_DIVERSITY_PWM_WITH_DMA
    // Make sure we claim the DMA channel only once.
    GP_ASSERT_DEV_EXT(halPWM_DmaChannel == HAL_DMA_CHANNEL_INVALID);

    halPWM_DmaChannel = hal_DmaClaim();
    GP_ASSERT_DEV_EXT(halPWM_DmaChannel != HAL_DMA_CHANNEL_INVALID);

    halPWM_DmaActive = false;
#endif //HAL_DIVERSITY_PWM_WITH_DMA
}

void hal_SetFrequencyPwm(UInt32 frequency)
{
    UInt32 period;
    UInt16 wrap;
    UInt8 power;

    GP_ASSERT_DEV_EXT(frequency > 0);

    period = HAL_PWM_PRESCALER_FREQUENCY / frequency;
    power = 0;

    //Frequencies above standard 16MHz prescaler clock
    //Use power field to scale clock down + halve period to fit in 16-bit
    while (period > 0xFFFF)
    {
        period /= 2;
        power++;
    }

    wrap = (period > 0) ? (UInt16)(period - 1) : 0;

    //GP_LOG_PRINTF("%lu %u %x %lu",0,frequency, power, wrap, period);

    // Set prescaler and wrap value of main counter.
    halPWM_SetMainCounterPeriod(power, wrap);

    // Reset main counter value to avoid glitch after decreasing threshold.
    halTimer_resetTimer(HAL_PWM_MAIN_COUNTER);
}

void hal_EnablePwm(Bool enable)
{
    Bool pwmMainTimerEnabled = (halTimer_isEnabledTimer(HAL_PWM_MAIN_COUNTER) != 0);
    Bool pwmCarrierTimerEnabled = (halTimer_isEnabledTimer(HAL_PWM_CARRIER_COUNTER) != 0);

    /* When PWM outputs are enabled, do not go to sleep, as timers will stop functioning */
    if (enable == false)
    {
        if (pwmMainTimerEnabled == true && pwmCarrierTimerEnabled == true)
        {
            if (gpHal_IsRadioAccessible())
            {
                gpHal_GoToSleepWhenIdle(true);
            }

            // reset the counter (start from clean state when block is re-enabled)
            halTimer_resetTimer(HAL_PWM_MAIN_COUNTER);
            halTimer_resetTimer(HAL_PWM_CARRIER_COUNTER);
            halTimer_enableTimer(HAL_PWM_MAIN_COUNTER, 0);
            halTimer_enableTimer(HAL_PWM_CARRIER_COUNTER, 0);
        }
    }
    else
    {
        if (pwmMainTimerEnabled == false || pwmCarrierTimerEnabled == false)
        {
            if (gpHal_IsRadioAccessible() == 0)
            {
                gpHal_GoToSleepWhenIdle(false);
            }

            // Enable/disable main counter and carrier counter.
            halTimer_enableTimer(HAL_PWM_MAIN_COUNTER, enable);
            halTimer_enableTimer(HAL_PWM_CARRIER_COUNTER, enable);
        }
    }
}

void hal_SetChannelEnabled(UInt8 channel, Bool enabled)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_ASSERT_DEV_EXT(halPWM_ChannelGpioMap[channel] != HAL_UART_GPIO_INVALID);

    // Each PWM channel configures:
    // - output drive: Keep fixed after Init()
    // - pin mapping: map/unmap to indicate channel is enabled/disabled
    halPWM_SetPwmGpioEnabled(channel, enabled);
}

Bool hal_PwmIsChannelEnabled(UInt8 channel)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_ASSERT_DEV_EXT(halPWM_ChannelGpioMap[channel] != HAL_UART_GPIO_INVALID);

    // Each PWM channel configures:
    // - output drive: Keep fixed after Init()
    // - pin mapping: map/unmap to indicate channel is enabled/disabled
    return halPWM_IsPwmGpioEnabled(channel);
}

Bool hal_PwmIsAnyChannelEnabled(void)
{
    UInt8 pwmChannel;
    Bool isAnyChannelMapped = false;

    for (pwmChannel = 0; pwmChannel < HAL_PWM_NR_OF_PWM_CHANNELS; ++pwmChannel)
    {
        if (halPWM_ChannelGpioMap[pwmChannel] == HAL_UART_GPIO_INVALID)
        {
            // This channel is not mapped, check others.
            continue;
        }
        if (hal_PwmIsChannelEnabled(pwmChannel))
        {
            isAnyChannelMapped = true;
            break;
        }
    }

    return isAnyChannelMapped;
}

void hal_InvertOutput(UInt8 channel, Bool invert)
{
    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    halPWM_SetOutputInvert(channel, invert);
}

void hal_SetDutyCycle(UInt8 channel, UInt16 dutyCycle)
{
    UInt16 threshold;

    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    // in case of balanced mode, apply correction factor to threshold
    threshold = dutyCycle;
    if (halPWM_GetUpDownEnable(channel))
    {
        threshold = (threshold + 1) / 2;
    }

    halPWM_UpdateThreshold(channel, threshold);
}

void hal_SetDutyCyclePercentage(UInt8 channel, UInt16 dutyCyclePercent)
{
    UInt16 wrapValue;
    UInt32 threshold;

    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);
    GP_ASSERT_DEV_EXT(dutyCyclePercent <= 10000);

    wrapValue = halTimer_getThreshold(HAL_PWM_MAIN_COUNTER);

    threshold = ((UInt32)dutyCyclePercent * ((UInt32)wrapValue + 1)) / 10000UL;

    // in case of balanced mode, apply correction factor to threshold
    if (halPWM_GetUpDownEnable(channel))
    {
        threshold = (threshold + 1) / 2;
    }

    threshold = min(threshold, 0xFFFF);
    halPWM_UpdateThreshold(channel, (UInt16)threshold);
}

void hal_SetSymmetricMode(UInt8 channel, Bool enable)
{
    Bool updownMode;
    UInt16 threshold;

    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    updownMode = halPWM_GetUpDownEnable(channel);
    if (updownMode != enable)
    {
        threshold = halPWM_GetThreshold(channel);
        if (enable)
        {
            // convert linear threshold to corresponding symmetric threshold
            threshold = ((UInt32)threshold + 1) / 2;
        }
        else
        {
            // convert symmetric threshold to corresponding linear threshold
            threshold = min((UInt32)threshold * 2 - 1, 0xFFFF);
        }
        halPWM_SetUpDownEnable(channel, enable);
        halPWM_UpdateThreshold(channel, threshold);
    }
}

/* Change PWM frequency such that main counter wraps at (65535 >> factor). */
void hal_SetFrequencyCorrectionFactor(UInt8 factor)
{
    UInt16 oldWrap;
    UInt16 newWrap;
    UIntLoop i;

    GP_ASSERT_DEV_EXT(factor < 16);

    oldWrap = halTimer_getTimerValue(HAL_PWM_MAIN_COUNTER);
    newWrap = 0xffff >> factor;

    halPWM_SetMainCounterPeriod(0, newWrap);
    halTimer_resetTimer(HAL_PWM_MAIN_COUNTER);

    // Adjust PWM thresholds to maintain current duty cycle.
    for(i = 0; i < HAL_PWM_NR_OF_PWM_CHANNELS; i++)
    {
        UInt32 threshold = halPWM_GetThreshold(i);
        threshold = threshold * ((UInt32)newWrap + 1) / ((UInt32)oldWrap + 1);
        threshold = min(threshold, 0xffff);
        halPWM_UpdateThreshold(i, (UInt16)threshold);
    }
}

void hal_SetPrescalerCounterWrapPower(UInt8 val)
{
    halTimer_setPrescalerDiv(HAL_PWM_MAIN_COUNTER, val);
}

void hal_ConfigPWM(
            UInt8 counterResolution,
            UInt8 carrier,
            Bool wordmode,
            Bool shiftThresholdUpdate,
            UInt8 channel)
{
    // Disable counters during initialisation
    hal_EnablePwm(false);

    // Setup prescaler to run main counter at 16 MHz
    // Set main counter wrap value to get desired resolution.
    halPWM_SetMainCounterPeriod(0, counterResolution);

    // Set carrier wrap value to get the desired update rate.
    // Update rate = 16 MHz / (counterResolution + 1) / (2**carrier)
    // For example: counterResolution=255, carrier=2 --> 15.625 kHz update rate.
    GP_ASSERT_DEV_EXT(carrier < 16);
    halPWM_SetCarrierCounterPeriod(0, (1UL << carrier) - 1);

#ifdef HAL_DIVERSITY_PWM_WITH_DMA

    GP_ASSERT_DEV_EXT(channel < HAL_PWM_NR_OF_PWM_CHANNELS);

    // Note channel and mode for DMA handling.
    halPWM_PcmChannel = channel;
    halPWM_PcmWordMode = wordmode;

    // Configure threshold FIFO byte/word mode.
    GP_WB_WRITE_PWMS_NEXT_THRESHOLD_FIFO_SIZE(wordmode);

    // Set PWM channel to update threshold on carrier counter wrap.
    GP_WB_WRITE_PWM_THRESHOLD_UPDATE_ON_CARRIER_COUNTER_WRAP(halPWM_BaseAddress(channel), 1);

    GP_WB_WRITE_PWMS_AUTO_SHIFT_THRESHOLD_UPDATE_ON_CARRIER_COUNTER_WRAP(shiftThresholdUpdate);
#endif
}

#ifdef HAL_DIVERSITY_PWM_WITH_DMA

/* Called when DMA buffer almost empty. */
static void halPWM_cDmaBufferAlmostComplete(hal_DmaChannel_t channel)
{
    // Reached almost-empty threshold.
    // Note the threshold is exactly half the buffer size.

    // Refill buffer.
    if (!halPWM_cbWriteDmaBufferIndication(DMA_TX_BUF_SIZE/2, halPWM_DmaBuffer, halPWM_DmaWritePtr.offset))
    {
        /* playback finished, stop PWM and DMA*/
        hal_DmaStop(channel);
        halPWM_DmaActive = false;
        return;
    }

    // Update buffer write pointer.
    halPWM_DmaWritePtr.offset += DMA_TX_BUF_SIZE / 2;
    if (halPWM_DmaWritePtr.offset >= DMA_TX_BUF_SIZE)
    {
        halPWM_DmaWritePtr.offset = 0;
        halPWM_DmaWritePtr.wrap = !halPWM_DmaWritePtr.wrap;
    }
    hal_DmaUpdatePointers(channel, halPWM_DmaWritePtr);
}

void halPWM_DmaStart(void)
{


    // Invoke callback to fill buffer to 100%.
    if (!halPWM_cbWriteDmaBufferIndication(DMA_TX_BUF_SIZE, halPWM_DmaBuffer, 0))
    {
        // No data available - don't start playback.
        return;
    }

    // Set initial write pointer to buffer 100% full.
    halPWM_DmaWritePtr.offset = 0;
    halPWM_DmaWritePtr.wrap = 1;

    // Prepare DMA configuration.
    GP_ASSERT_DEV_EXT(halPWM_DmaChannel != HAL_DMA_CHANNEL_INVALID);
    GP_ASSERT_DEV_EXT(!halPWM_DmaActive);

    hal_DmaDescriptor_t dmaDesc;
    dmaDesc.channel = halPWM_DmaChannel;
    dmaDesc.cbAlmostComplete = halPWM_cDmaBufferAlmostComplete;
    dmaDesc.cbComplete = NULL;
    dmaDesc.wordMode = halPWM_PcmWordMode ? GP_WB_ENUM_DMA_WORD_MODE_HALF_WORD : GP_WB_ENUM_DMA_WORD_MODE_BYTE;
    dmaDesc.bufferSize = sizeof(halPWM_DmaBuffer);
    dmaDesc.writePtr.offset = halPWM_DmaWritePtr.offset;
    dmaDesc.writePtr.wrap = halPWM_DmaWritePtr.wrap;
    dmaDesc.circBufSel = GP_WB_ENUM_CIRCULAR_BUFFER_SRC_BUFFER;
    dmaDesc.threshold = sizeof(halPWM_DmaBuffer) / 2;
    dmaDesc.dmaTriggerSelect = GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TX_NOT_FULL;
    dmaDesc.srcAddr = (UInt32) halPWM_DmaBuffer;
    dmaDesc.srcAddrInRam = true;
    dmaDesc.destAddr = GP_WB_PWMS_NEXT_THRESHOLD_ADDRESS;
    dmaDesc.destAddrInRam = false;
    dmaDesc.bufCompleteIntMode = GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_ERROR_MODE;

    // Start DMA.
    hal_DmaResult_t result = hal_DmaStart(&dmaDesc);
    GP_ASSERT_DEV_EXT(result == HAL_DMA_RESULT_SUCCESS);
    halPWM_DmaActive = true;

    // Enable PWM output.
    hal_SetChannelEnabled(halPWM_PcmChannel, true);
    hal_EnablePwm(true);
}

void halPWM_DmaStop(void)
{
    hal_SetChannelEnabled(halPWM_PcmChannel, false);

    if (halPWM_DmaActive)
    {
        hal_DmaStop(halPWM_DmaChannel);
        halPWM_DmaActive = false;
    }


}

#endif //HAL_DIVERSITY_PWM_WITH_DMA
