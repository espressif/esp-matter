/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_RTC_H_
#define _FSL_RTC_H_

#include "fsl_common.h"

/*!
 * @addtogroup rtc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_RTC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*! @brief List of RTC interrupts */
typedef enum _rtc_interrupt_enable
{
    kRTC_AlarmInterruptEnable        = (1U << 15U), /*!< Alarm interrupt. */
    kRTC_TimeOverflowInterruptEnable = (1U << 16U), /*!< Time overflow interrupt. */
    kRTC_AllInterruptsEnable         = (3U << 15U), /*!< All interrupts to enable. */
} rtc_interrupt_enable_t;

/*! @brief List of RTC flags */
typedef enum _rtc_status_flags
{
    kRTC_CntRunFlag           = (1U << 0U),  /*!< Counter reset done flag */
    kRTC_CntRstDoneFlag       = (1U << 1U),  /*!< Counter reset done flag */
    kRTC_ResetFlag            = (1U << 2U),  /*!< Module reset flag */
    kRTC_AlarmFlag            = (1U << 15U), /*!< Alarm flag */
    kRTC_TimeOverflowFlag     = (1U << 16U), /*!< Time overflow flag */
    kRTC_AllNonClearableFlags = (7U << 0U),  /*!< All non-clearable flags */
    kRTC_AllClearableFlags    = (3U << 15U), /*!< All clearable flags */
} rtc_status_flags_t;

/*! @brief List of clock divider */
typedef enum _rtc_clock_div
{
    kRTC_ClockDiv1     = 0U,  /*!< Clock divider 1 */
    kRTC_ClockDiv2     = 1U,  /*!< Clock divider 2 */
    kRTC_ClockDiv4     = 2U,  /*!< Clock divider 4 */
    kRTC_ClockDiv8     = 3U,  /*!< Clock divider 8 */
    kRTC_ClockDiv16    = 4U,  /*!< Clock divider 16 */
    kRTC_ClockDiv32    = 5U,  /*!< Clock divider 32 */
    kRTC_ClockDiv64    = 6U,  /*!< Clock divider 64 */
    kRTC_ClockDiv128   = 7U,  /*!< Clock divider 128 */
    kRTC_ClockDiv256   = 8U,  /*!< Clock divider 256 */
    kRTC_ClockDiv512   = 9U,  /*!< Clock divider 512 */
    kRTC_ClockDiv1024  = 10U, /*!< Clock divider 1024 */
    kRTC_ClockDiv2048  = 11U, /*!< Clock divider 2048 */
    kRTC_ClockDiv4096  = 12U, /*!< Clock divider 4096 */
    kRTC_ClockDiv8192  = 13U, /*!< Clock divider 8192 */
    kRTC_ClockDiv16384 = 14U, /*!< Clock divider 16384 */
    kRTC_ClockDiv32768 = 15U, /*!< Clock divider 32768 */
} rtc_clock_div_t;

/*!
 * @brief RTC config structure
 *
 * This structure holds the configuration settings for the RTC peripheral. To initialize this
 * structure to reasonable defaults, call the RTC_GetDefaultConfig() function and pass a
 * pointer to your config structure instance.
 *
 * The config struct can be made const so it resides in flash
 */
typedef struct _rtc_config
{
    bool ignoreInRunning;   /*!< true: If the counter is already running, the configuration will be ignored.
                                 false: Counter will always be reset and the the configurations will always be set. */
    bool autoUpdateCntVal;  /*!< true: CNT_VAL is updated on every counter clock tick.
                                 false: CNT_VAL is not updated. */
    bool stopCntInDebug;    /*!< true: In debug mode, stop the counters.
                                 false: In debug mode, counters are not affected. */
    rtc_clock_div_t clkDiv; /*!< Counter clock divider. */
    uint32_t cntUppVal;     /*!< If counter reaches this value, the counter will overflow to 0. */
} rtc_config_t;

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
 * @brief Ungates the RTC clock and configures the peripheral for basic operation.
 *
 * This function configures the RTC only if the counter is not running.
 *
 * @note This API should be called at the beginning of the application using the RTC driver.
 *
 * @param base   RTC peripheral base address
 * @param config Pointer to the user's RTC configuration structure.
 */
void RTC_Init(RTC_Type *base, const rtc_config_t *config);

/*!
 * @brief Stops the timer and gate the RTC clock.
 *
 * @param base RTC peripheral base address
 */
void RTC_Deinit(RTC_Type *base);

/*!
 * @brief Fills in the RTC config struct with the default settings.
 *
 * The default values are as follows.
 * @code
 *    config->ignoreInRunning = true;
 *    config->autoUpdateCntVal = true;
 *    config->stopCntInDebug = true;
 *    config->clkDiv = kRTC_ClockDiv32;
 *    config->cntUppVal = 0xFFFFFFFFU;
 * @endcode
 * @param config Pointer to the user's RTC configuration structure.
 */
void RTC_GetDefaultConfig(rtc_config_t *config);

/*! @}*/

/*!
 * @name Current Time & Alarm
 * @{
 */

/*!
 * @brief Gets current RTC counter value.
 *
 * @param base     RTC peripheral base address
 *
 * @return counter value in RTC.
 */
uint32_t RTC_GetCounter(RTC_Type *base);

/*!
 * @brief Sets the RTC alarm time.
 *
 * The function checks whether the specified alarm time is greater than the present
 * time. If not, the function does not set the alarm and returns an error.
 *
 * @param base      RTC peripheral base address
 * @param alarmCnt  Counter value to trigger the alarm.
 *
 * @return kStatus_Success: success in setting the RTC alarm
 *         kStatus_Fail: Error because the alarm time has already passed
 */
status_t RTC_SetAlarm(RTC_Type *base, uint32_t alarmCnt);

/*!
 * @brief Returns the RTC alarm counter value.
 *
 * @param base     RTC peripheral base address
 *
 * @return Alarm counter value in RTC.
 */
uint32_t RTC_GetAlarm(RTC_Type *base);

/*! @}*/

/*!
 * @name Interrupt Interface
 * @{
 */

/*!
 * @brief Enables the selected RTC interrupts.
 *
 * @param base RTC peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::rtc_interrupt_enable_t
 */
void RTC_EnableInterrupts(RTC_Type *base, uint32_t mask);

/*!
 * @brief Disables the selected RTC interrupts.
 *
 * @param base RTC peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::rtc_interrupt_enable_t
 */
void RTC_DisableInterrupts(RTC_Type *base, uint32_t mask);

/*!
 * @brief Gets the enabled RTC interrupts.
 *
 * @param base RTC peripheral base address
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::rtc_interrupt_enable_t
 */
uint32_t RTC_GetEnabledInterrupts(RTC_Type *base);

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the RTC status flags.
 *
 * @param base RTC peripheral base address
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::rtc_status_flags_t
 */
uint32_t RTC_GetStatusFlags(RTC_Type *base);

/*!
 * @brief  Clears the RTC status flags.
 *
 * @param base RTC peripheral base address
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::rtc_status_flags_t
 */
void RTC_ClearStatusFlags(RTC_Type *base, uint32_t mask);

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the RTC time counter.
 *
 * @param base RTC peripheral base address
 */
void RTC_StartTimer(RTC_Type *base);

/*!
 * @brief Stops the RTC time counter.
 *
 * @param base RTC peripheral base address
 */
void RTC_StopTimer(RTC_Type *base);

/*!
 * @brief Performs a counter reset on the RTC module.
 *
 * @param base RTC peripheral base address
 */
void RTC_ResetTimer(RTC_Type *base);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_RTC_H_ */
