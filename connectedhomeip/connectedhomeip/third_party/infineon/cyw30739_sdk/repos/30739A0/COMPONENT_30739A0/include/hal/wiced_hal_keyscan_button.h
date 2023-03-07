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
* Button driver which is based on keyscan driver using keyscan HW
*
*/

#ifndef __WICED_KEYSCAN_BUTTON_H__
#define __WICED_KEYSCAN_BUTTON_H__

#include "brcm_fw_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**  \addtogroup keyscanButton Keyscan Button
 *  \ingroup keyscan
*/
/*! @{ */
/**
* Defines the button driver that based on keyscan driver using keyscan hardware.
* Buttons are implemented as keys. A scan matrix of 1 row, n cols
* is created and scanned by the keyscan hardware. The hardware reports the
* state of the buttons as keys. This information is translated by the button
* interface into a byte representing various buttons.
*
*/

///////////////////////////////////////////////////////////////////////////////
/// configure button driver
///
/// Application use this to configure the button driver, which is based on keyscan driver using the keyscan HW
///
/// \param num_buttons - defines the number of columns in keyscan matrix. MAX is 16.  For button driver, number of rows is always 1.
/// \param flushUserActivityAllowed - indicates whether all pending key events should be processed when a flush request is received.
/// \param clearCurButtonStateOnFlush -indicates whether current button state should be cleared on a flush.
///                                                        Note!!! this is independent of flushUserActivityAllowed item
///                                                        However, it is reccommended that this flag should only be set to WICED_TRUE if
///                                                        flushUserActivityAllowed is set to WICED_TRUE
/// \param report_bits -pointer to button report bit mapping
///
/// Default button configuraton is as followed:
///{
///    // Default to 4 buttons
///    4,
///
///    // Disregard flush request from the mouse application.
///    // Flushing may cause us to lose connect button events
///    FALSE,
///
///    // Don't clear button state on flush. This means that a flush determines the
///    // current state of the buttons, discarding intermediate events
///    FALSE,
///
///    // Button bit to report bit mappings
///    // repor_bits
///    {
///        // 1st button is left button
///        0x0001,
///
///        // 2nd button is middle button
///        0x0004,
///
///        // 3rd button is right button
///        0x0002,
///
///        // 4th button is connect button. Map it as the last button
///        0x8000
///    }
///};
///
/// \return - none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_keyscan_button_configure(uint8_t num_buttons, wiced_bool_t flush_user_activity_allowed, wiced_bool_t clear_cur_button_state_on_flush, uint16_t *report_bits);

/////////////////////////////////////////////////////////////////////////////
///  Initilaiizes the button interface. The state of all buttons is cleared.
/// This must be invoked once at power up before accessing any button services
/// Since this initializes keyscan driver, keyscan driver must not be
/// initialized separately. This will cause undefined behavior. Also,
/// button driver must be initialized after mia driver has been initialized.
/////////////////////////////////////////////////////////////////////////////
void wiced_hal_keyscan_button_init(void);

////////////////////////////////////////////////////////////////////////////////
///  Get the current button state.
///
/// \return
///   Bit mapped value providing the current button state
////////////////////////////////////////////////////////////////////////////////////
uint16_t wiced_hal_keyscan_button_get_current_state(void);

///////////////////////////////////////////////////////////////////////////////
///   Flush any queued button activity in FW or HW. This function actually processes
/// all pending key events from the keyscan driver. Depending on the configuration
/// it may also set state of all buttons to "not pressed"
///
/// \return
///  Bit mapped value providing the current button state
/////////////////////////////////////////////////////////////////////////////////
uint16_t wiced_hal_keyscan_button_flush(void);


/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
