
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
* Auxiliary Clock (aclk) driver.
*
*/

#ifndef __WICED_ACLK_H__
#define __WICED_ACLK_H__

#include "wiced.h"

/**  \addtogroup AuxClkDriver Auxiliary Clock (ACLK)
*   \ingroup HardwareDrivers
*
* @{
*
* Defines a driver to facilitate interfacing with the auxiliary clock.
*
* Use this driver to output a clock to a GPIO pin for external use. Two clock
* sources are avaliable: clk0 or clk1, as well as two base frequencies used
* to calulate the prescaler : 1 MHz or 24 MHz. Note that the desired GPIO pin
* must have already been configured to output for aclk. See the HW muxing
* section in the Kit Guide or HW User Manual for your device for more information.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following enumerations are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

// Available clock sources.
enum WICED_CLK_SRC_SEL
{
    WICED_ACLK0,
    WICED_ACLK1,
};

// Available base clock frequencies.
enum WICED_CLK_SRC_FREQ_SEL
{
    WICED_ACLK_FREQ_24_MHZ = 1
};

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, and starts the aclk to be active on a
/// preconfigured GPIO pin.
///
/// (!) Note that the desired GPIO pin must have already been configured
/// to output for aclk. See the HW muxing section in the HW User Manual or Kit
/// Guide for your device for more information.
///
/// \param frequency   - Desired frequency (in KHz) to output; 0 is the same as disable.
/// \param clkSrc      - ACLK0 or ACLK1
/// \param baseFreqSel - ACLK_FREQ_24_MHZ.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_aclk_enable(uint32_t frequency,
                                         uint32_t clkSrc,
                                         uint32_t baseFreqSel);


///////////////////////////////////////////////////////////////////////////////
/// Disables the desired auxilary clock source.
///
/// \param clkSrc - Clock source to disable (aclk0 or aclk1).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_aclk_disable(uint32_t clkSrc);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_ACLK_H__
