/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_WDT_H
#define _FSL_WDT_H

#include "fsl_common.h"

/*!
 * @addtogroup wdt
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief WDT driver version 2.0.0. */
#define FSL_WDT_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @name WDT refresh pattern. */
/*@{*/
#define WDT_REFRESH_PATTERN (0x76U) /*!< Refresh pattern to restart WDT */
/*@}*/

/*! @brief enumeration for wdt timeout response mode select. */
typedef enum _wdt_timeout_response_mode
{
    kWDT_ModeTimeoutReset     = 0, /*!< WDT will generate system reset for timeout */
    kWDT_ModeTimeoutInterrupt = 1, /*!< WDT will generate interrupt for timeout */
} wdt_timeout_response_mode_t;

/*! @brief enumeration for wdt reset assert width select. */
typedef enum _wdt_reset_width
{
    kWDT_ResetPulseLen2   = 0, /*!< WDT reset pulse length: 2 pclk cycles */
    kWDT_ResetPulseLen4   = 1, /*!< WDT reset pulse length: 4 pclk cycles */
    kWDT_ResetPulseLen8   = 2, /*!< WDT reset pulse length: 8 pclk cycles */
    kWDT_ResetPulseLen16  = 3, /*!< WDT reset pulse length: 16 pclk cycles */
    kWDT_ResetPulseLen32  = 4, /*!< WDT reset pulse length: 32 pclk cycles */
    kWDT_ResetPulseLen64  = 5, /*!< WDT reset pulse length: 64 pclk cycles */
    kWDT_ResetPulseLen128 = 6, /*!< WDT reset pulse length: 128 pclk cycles */
    kWDT_ResetPulseLen256 = 7, /*!< WDT reset pulse length: 256 pclk cycles */
} wdt_reset_width_t;

/*! @brief enumeration for wdt timeout value config. */
typedef enum _wdt_timeout_value
{
    kWDT_TimeoutVal2ToThePowerOf16 = 0,  /*!< WDT timeout value : 2^16 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf17 = 1,  /*!< WDT timeout value : 2^17 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf18 = 2,  /*!< WDT timeout value : 2^18 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf19 = 3,  /*!< WDT timeout value : 2^19 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf20 = 4,  /*!< WDT timeout value : 2^20 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf21 = 5,  /*!< WDT timeout value : 2^21 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf22 = 6,  /*!< WDT timeout value : 2^22 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf23 = 7,  /*!< WDT timeout value : 2^23 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf24 = 8,  /*!< WDT timeout value : 2^24 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf25 = 9,  /*!< WDT timeout value : 2^25 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf26 = 10, /*!< WDT timeout value : 2^26 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf27 = 11, /*!< WDT timeout value : 2^27 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf28 = 12, /*!< WDT timeout value : 2^28 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf29 = 13, /*!< WDT timeout value : 2^29 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf30 = 14, /*!< WDT timeout value : 2^30 pclk cycles */
    kWDT_TimeoutVal2ToThePowerOf31 = 15, /*!< WDT timeout value : 2^31 pclk cycles */
} wdt_timeout_value_t;

/*! @brief structure for WDT module initialization configuration. */
typedef struct _wdt_config
{
    wdt_timeout_value_t timeoutValue;
    wdt_timeout_response_mode_t timeoutMode : 1;
    wdt_reset_width_t resetWidth : 4;
    bool enableWDT : 1;
} wdt_config_t;

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @name Initialization Interfaces
 * @{
 */

/*!
 * @brief Initializes the WDT module with input configuration
 * Call this function to do initialization configuration for WDT module.
 * The configurations are:
 * - WDT counter timeout value
 * - Timeout response mode select
 * - Reset assert pulse width configuration
 * - WDT enable/disable
 * @param base   WDT peripheral base address.
 * @param config The pointer to WDT configuration structure, @ref wdt_config_t.
 */
void WDT_Init(WDT_Type *base, const wdt_config_t *config);

/*!
 * @brief Prepares an available pre-defined setting for module's configuration.
 * This function initializes the WDT configuration structure to default values.
 *   config->timeoutValue = kWDT_TimeoutVal2ToThePowerOf16;
 *   config->timeoutMode =kWDT_TimeoutModeReset;
 *   config->resetWidth = kWDT_ResetWidthLen2;
 *   config->enableWDT = false;
 * @param config Pointer to the WDT configuration structure, @ref wdt_config_t.
 */
void WDT_GetDefaultConfig(wdt_config_t *config);
/* @} */

/*!
 * @name WDT Operation Interfaces
 * @{
 */

/*!
 * @brief Enable/Disable the WDT module.
 *
 * This function enable/disables the WDT.
 * To disable the WDT Watchdog, call WDT_Enable(base, false).
 *
 * @param base  WDT peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void WDT_Enable(WDT_Type *base, bool enable)
{
    if (enable)
    {
        base->WDT_CR |= WDT_WDT_CR_WDT_EN_MASK;
    }
    else
    {
        base->WDT_CR &= ~WDT_WDT_CR_WDT_EN_MASK;
    }
}

/*!
 * @brief Sets the WDT timeout value.
 *
 */
static inline void WDT_SetTimeoutValue(WDT_Type *base, wdt_timeout_value_t timeoutValue)
{
    base->WDT_TORR = (base->WDT_TORR & ~WDT_WDT_TORR_TOP_MASK) | WDT_WDT_TORR_TOP((uint32_t)timeoutValue);
}

/*!
 * @brief Gets the WDT timeout value.
 *
 */
static inline uint32_t WDT_GetCounterValue(WDT_Type *base)
{
    return base->WDT_CCVR;
}

/*!
 * @brief Refreshes the WDT timer
 *
 * This function feeds/services the WDT.
 *
 * @param base WDT peripheral base address.
 */
void WDT_Refresh(WDT_Type *base);

/* @} */

/*!
 * @name Interrupt Control Interfaces
 * @{
 */
/*!
 * @brief Enables the WDT interrupt, if config->timeoutMode = kWDT_TimeoutModeInterrupt
 * for calling WDT_Init, the WDT interrupt is by default enabled, this function does not
 * take effect.
 *
 * This function set WDT timeout response mode to generate interrupt.
 *
 * @param base WDT peripheral base address
 */
static inline void WDT_EnableInterrupt(WDT_Type *base)
{
    base->WDT_CR |= WDT_WDT_CR_RMOD_MASK;
}

/*!
 * @brief Enables the WDT interrupt, if config->timeoutMode = kWDT_TimeoutModeReset
 * for calling WDT_Init, the WDT interrupt is by default disabled, this function does not
 * take effect.
 *
 * This function set WDT timeout response mode to generate reset.
 *
 * @param base WDT peripheral base address
 */
static inline void WDT_DisableInterrupt(WDT_Type *base)
{
    base->WDT_CR &= ~WDT_WDT_CR_RMOD_MASK;
}
/* @} */

/*!
 * @name Status get/clear interface.
 * @{
 */

/*!
 * @brief Gets WDT interrupt status.
 *
 * @param base WDT peripheral base address
 *
 * @return true if interrupt flag is asserted, else false
 *
 */

static inline bool WDT_GetInterruptStatus(WDT_Type *base)
{
    return ((base->WDT_STAT & WDT_WDT_STAT_WDT_STAT_MASK) != 0U) ? true : false;
}

/*!
 * @brief Clear WDT interrupt status
 *
 * @param base WDT peripheral base address
 */
static inline void WDT_ClearInterruptStatus(WDT_Type *base)
{
    volatile uint32_t tmpData;
    /* Clear interrupt flag for read */
    tmpData = base->WDT_EOI;
    tmpData = tmpData;
}

/*! @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* _FSL_WDT_H_ */
