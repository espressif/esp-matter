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
* List of parameters and defined functions needed to access the
* Multiple Interface Adapter (MIA) driver.
*
*/

#ifndef __WICED_MIA_H__
#define __WICED_MIA_H__

#include "brcm_fw_types.h"

/**  \addtogroup MIADriver Multiple Interface Adapter (MIA)
* \ingroup HardwareDrivers
* @{
*
* Defines a Multiple Interface Adapter (MIA) driver to facilitate interfacing
* with various components of the hardware
* The MIA driver controls aspects such as GPIO pin muxing, interrupt
* managment, and timing.
*
* Use this driver to enable and disable interrupts, along with managing
* hardware events (such as PUART RX). There are many possibilities--please
* look over the following API functions to see what can be done.
*/

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Initializes the MIA driver and its private values. Also programs all
/// relevant GPIOs to be ready for use. This must be invoked before
/// accessing any MIA driver services, typically at boot.
/// This is independent of other drivers and must be one of the first to
/// be initialized.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_init(void);


///////////////////////////////////////////////////////////////////////////////
/// Determines and returns if the reason for the last reset was from Power
/// On (Power On Reset).
///
/// \param none
///
/// \return reason - 1 if reset was caused by Power On Reset, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_is_reset_reason_por(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable or disable all MIA-based interrupts.
///
/// \param enable - 1 to enable interrupts, 0 to disable interrupts.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_enable_mia_interrupt(BOOL32 enable);


///////////////////////////////////////////////////////////////////////////////
/// Enable or disable all LHL-based interrupts.
///
/// \param enable - 1 to enable interrupts, 0 to disable interrupts.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_enable_lhl_interrupt(BOOL32 enable);

////////////////////////////////////////////////////////////////////////////////
/// If the HW has seen any change in KS or Quad, this will freeze Mia clock,
/// extract the event from HW FIFO and then unfreeze clock.
///
/// \param none
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_pollHardware(void);

///////////////////////////////////////////////////////////////////////////////
/// Get the MIA interrupt state.  This state may not be the
/// actual hardware MIA interrupt setting, since the interrupt
/// context interrupt handler will disable the MIA interrupt
/// without updating the state variable, which is a good thing
/// since it makes it easy to restore the correct hardware
/// setting from this state variable when done handling the
/// serialized interrupt by calling wiced_hal_mia_restoreMiaInterruptState().
///
/// \return 1 if MIA interrupts should be enabled, 0 if MIA interrupts should
/// be disabled.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_get_mia_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Get the LHL interrupt state.  This state may not be the
/// actual hardware LHL interrupt setting, since the interrupt
/// context interrupt handler will disable the LHL interrupt
/// without updating the state variable, which is a good thing
/// since it makes it easy to restore the correct hardware
/// setting from this state variable when done handling the
/// serialized interrupt by calling wiced_hal_restoreLhlInterruptSetting().
///
/// \return 1 if lhl interrupts should be enabled, 0 if lhl interrupts should
/// be disabled.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_get_lhl_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Restore the possibly overridden MIA interrupt setting to
/// that of the MIA interrupt state.  See wiced_hal_getMiaInterruptState()
/// for more details.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_restore_mia_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Restore the possibly overridden LHL interrupt setting to
/// that of the LHL interrupt state.  See wiced_hal_getLhlInterruptState()
/// for more details.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_restore_lhl_interrupt_state(void);


/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_MIA_H__
