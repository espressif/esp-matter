/***************************************************************************//**
 * @file
 * @brief TEMPDRV API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_TEMPDRV_H__
#define __SILICON_LABS_TEMPDRV_H__

#include "em_device.h"
#include "em_emu.h"
#include "ecode.h"
#include "tempdrv_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup tempdrv  TEMPDRV  Temperature Driver
 * @brief TEMPDRV Temperature Driver provides an interface and various
 *        functionalities to the EMU internal temperature sensor.
 *        Subsequent sections provide more insight into TEMPDRV driver.
 *   @details
 *
 *   @li @ref tempdrv_intro
 *   @li @ref tempdrv_usage
 *
 *   @n @section tempdrv_intro Introduction
 *        TEMPDRV provides a user-friendly interface to the EMU internal
 *        temperature sensor, which is present on the EFR32 and some EFM32
 *		  devices. TEMPDRV supports application-specific callbacks at given
 *		  temperature thresholds. EMU internal temperature sensor runs in energy
 *        modes EM0-EM4 and can wake up the core whenever temperature changes.
 *        Also, EMU temperature sensor runs continuously and measurements are
 *        taken every 250 ms.
 *
 *        @note TEMPDRV uses the EMU peripheral and not the ADC peripheral. ADC
 *        contains another internal temperature sensor, which is not touched by
 *        the TEMPDRV.
 *
 *        TEMPDRV provides an important errata fix for the EFR32 first
 *        generation devices when operating at high temperature environments
 *        (above 50&deg;C). The "EMU_E201 - High Temperature Operation" errata is
 *        described in the EFR32 errata. To implement the errata fix in a user
 *        application, include the TEMPDRV and call @ref TEMPDRV_Init() at the
 *        start of the program. This will activate the errata fix code, which
 *        modifies registers based on changes in the EMU temperature.
 *   @n @section tempdrv_usage TEMPDRV Usage
 *
 * @{
 ******************************************************************************/

/**************************************************************************//**
* @addtogroup tempdrv_error_codes Error Codes
* @brief TEMPDRV error codes
* @{
******************************************************************************/
#define ECODE_EMDRV_TEMPDRV_OK                (ECODE_OK)                              ///< Success return value.
#define ECODE_EMDRV_TEMPDRV_NO_INIT           (ECODE_EMDRV_TEMPDRV_BASE | 0x00000001) ///< Function requires prior initialization
#define ECODE_EMDRV_TEMPDRV_PARAM_ERROR       (ECODE_EMDRV_TEMPDRV_BASE | 0x00000002) ///< Illegal input parameter.
#define ECODE_EMDRV_TEMPDRV_BAD_LIMIT         (ECODE_EMDRV_TEMPDRV_BASE | 0x00000003) ///< Temperature mismatch with limit.
#define ECODE_EMDRV_TEMPDRV_NO_CALLBACK       (ECODE_EMDRV_TEMPDRV_BASE | 0x00000004) ///< Can't find callback.
#define ECODE_EMDRV_TEMPDRV_NO_SPACE          (ECODE_EMDRV_TEMPDRV_BASE | 0x00000005) ///< No more space to register
#define ECODE_EMDRV_TEMPDRV_TEMP_UNDER        (ECODE_EMDRV_TEMPDRV_BASE | 0x00000006) ///< Requested temperature below measurable range
#define ECODE_EMDRV_TEMPDRV_TEMP_OVER         (ECODE_EMDRV_TEMPDRV_BASE | 0x00000007) ///< Requested temperature above measurable range
#define ECODE_EMDRV_TEMPDRV_DUP_TEMP          (ECODE_EMDRV_TEMPDRV_BASE | 0x00000008) ///< Requested temperature is a duplicate

/** @} (end addtogroup tempdrv_error_codes) */

/**
 * Temperature limit. This is used in the TEMPDRV to specify the
 * direction of temperature change.
 */
typedef enum TEMPDRV_LimitType{
  TEMPDRV_LIMIT_LOW  = 0,   /**< Temperature low. Used to specify that a temperature is decreasing. */
  TEMPDRV_LIMIT_HIGH = 1    /**< Temperature high. Used to specify that a temperature is increasing. */
} TEMPDRV_LimitType_t;

/***************************************************************************//**
 * @brief
 *   TEMPDRV temperature limit callback function.
 *
 * @details
 *   Called from the interrupt context. The callback
 *   function is called when the current temperature is equal to or exceeds
 *   one of the temperature limits that have been registered with the driver.
 *
 * @param[in] temp
 *   The current temperature at the time when the EMU temperature triggers an
 *   interrupt. Note that this is not necessarily the same temperature as was
 *   specified when registering a callback.
 *
 * @param[in] limit
 *   The upper/lower limit reached
 ******************************************************************************/
typedef void (*TEMPDRV_Callback_t)(int8_t temp, TEMPDRV_LimitType_t limit);

/***************************************************************************//**
 * @brief
 *  TEMPDRV IRQ Handler.
 *
 * @details
 *  This IRQ Handler should be called from within the @ref EMU_IRQHandler
 *  to enable TEMPDRV callbacks. This is included by default with
 *  EMU_CUSTOM_IRQ_HANDLER defined as false.
 ******************************************************************************/
void TEMPDRV_IRQHandler(void);

Ecode_t TEMPDRV_Init(void);

Ecode_t TEMPDRV_DeInit(void);

Ecode_t TEMPDRV_Enable(bool enable);

uint8_t TEMPDRV_GetActiveCallbacks(TEMPDRV_LimitType_t limit);

int8_t TEMPDRV_GetTemp(void);

Ecode_t TEMPDRV_RegisterCallback(int8_t temp,
                                 TEMPDRV_LimitType_t limit,
                                 TEMPDRV_Callback_t callback);

Ecode_t TEMPDRV_UnregisterCallback(TEMPDRV_Callback_t callback);

/** @} (end addtogroup tempdrv) */

#ifdef __cplusplus
}
#endif
#endif // __SILICON_LABS_TEMPDRV_H__
