/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
*/

/** @file
*
*Defines the interface for device low power managment
*/

#ifndef __WICED_HAL_LPM_H__
#define __WICED_HAL_LPM_H__

/*****************************************************************************
**                               Constants
******************************************************************************/
/// The wake up sources
enum
{
    /// Wake from GPIO
    WICED_BT_LPM_WAKE_SOURCE_GPIO    = (1 << 8),

    /// Wake from LHL is the same as waking from GPIO
    WICED_BT_LPM_WAKE_SOURCE_LHL     = WICED_BT_LPM_WAKE_SOURCE_GPIO,

    /// Wake from keyscan
    WICED_BT_LPM_WAKE_SOURCE_KEYSCAN = (1 << 6),

    /// Wake from wuadrature sensor
    WICED_BT_LPM_WAKE_SOURCE_QUAD    = (1 << 7),

    /// All wake HID sources
    WICED_BT_LPM_WAKE_SOURCE_MASK    = (WICED_BT_LPM_WAKE_SOURCE_GPIO | WICED_BT_LPM_WAKE_SOURCE_KEYSCAN | WICED_BT_LPM_WAKE_SOURCE_QUAD)
};

// The poll type of low power mode
typedef enum wiced_bt_lpm_poll_type_e
{
    /// lpm is probing the registered method if device can sleep
    WICED_BT_LOW_POWER_MODE_POLL_TYPE_SLEEP = 0,

    /// lpm is probing the registered method if the device can enter shutdown sleep (SDS)
    WICED_BT_LOW_POWER_MODE_POLL_TYPE_SDS = 2,
}wiced_bt_lpm_poll_type_t;

/// The sleep type of the power managment modole (PMU)
typedef enum wiced_bt_lpm_pmu_sleep_type_t
{
//! sleep is not allowed
    WICED_BT_PMU_SLEEP_NOT_ALLOWED = 0,

//! shutdown sleep (SDS) is allowed. SDS will disable almost all the HW during sleep
    WICED_BT_PMU_SLEEP_SDS_ALLOWED = 2,

//! shutdown sleep (SDS) is NOT allowed. SDS will disable almost all the HW during sleep
    WICED_BT_PMU_SLEEP_NO_SDS = 5

} wiced_bt_lpm_pmu_sleep_type_t;


#ifdef __cplusplus
extern "C" {
#endif


/// This typedef is what is expected when registering a function as
/// a callback. This callback function is expected to respond with time to
/// sleep or if it is ok to enter shutdown sleep (SDS) or not.
///
/// when poll type is WICED_BT_LOW_POWER_MODE_POLL_TYPE_SLEEP,
///         return value 0 indicates do not sleep;
///         return value non-zero indicates the sleep time allowed.
/// when poll type is WICED_BT_LOW_POWER_MODE_POLL_TYPE_SDS,
///         return value WICED_BT_PMU_SLEEP_SDS_ALLOWED indicates OK to enter SDS.
///         return value WICED_BT_PMU_SLEEP_NO_SDS indicates do not enter SDS;
typedef uint32_t (*wiced_bt_lpm_callback_fp)(wiced_bt_lpm_poll_type_t type, uint32_t context);


/******************************************************************************
*** Function prototypes.
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// Initialize device low power management module
///
/// \param none
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hal_lpm_init(void);

////////////////////////////////////////////////////////////////////////////////
/// Enables wake up from the given sources
///
/// \param sources - The WICED_BT_LPM_WAKE_SOURCE_* sources for wake. See bellowed:
///
///     enum
///     {
///         /// Wake from GPIO
///         WICED_BT_LPM_WAKE_SOURCE_GPIO    = (1 << 8),
///
///         /// Wake from LHL is the same as waking from GPIO
///         WICED_BT_LPM_WAKE_SOURCE_LHL     = WICED_BT_LPM_WAKE_SOURCE_GPIO,
///
///         /// Wake from keyscan
///         WICED_BT_LPM_WAKE_SOURCE_KEYSCAN = (1 << 6),
///
///         /// Wake from wuadrature sensor
///         WICED_BT_LPM_WAKE_SOURCE_QUAD    = (1 << 7),
///
///         /// All wake HID sources
///         WICED_BT_LPM_WAKE_SOURCE_MASK    = (WICED_BT_LPM_WAKE_SOURCE_GPIO | WICED_BT_LPM_WAKE_SOURCE_KEYSCAN | WICED_BT_LPM_WAKE_SOURCE_QUAD)
///     };
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hal_lpm_enable_wake_from(uint32_t sources);

////////////////////////////////////////////////////////////////////////////////
/// Allows an application to register for wake notification that the wake source are enabled by
/// wiced_hal_lpm_enable_wake_from(uint32_t sources).
///
/// \param fn - Pointer to function that will handle the wake notification
/// \param data - An opaque data that is handed to callback.

///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hal_lpm_register_for_wake_notification(uint8_t (*fn)(void*), void* data);

////////////////////////////////////////////////////////////////////////////////
/// Adds the given object to the list of objects that need to be queried for sleep and SDS
///
/// \param callback - The callback that needs to be invoked to check for sleep/SDS OK.
///                            The callback should decide very quickly and return within a few 10s of uS.
/// \param context - Any arbitrary context that the callback function may need later. Will be passed
///                           in as is when the callback is invoked.
///
/// \return TRUE if successfully added, else FALSE.
////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_lpm_register_for_low_power_queries(wiced_bt_lpm_callback_fp callback, uint32_t context);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_HAL_LPM_H__
