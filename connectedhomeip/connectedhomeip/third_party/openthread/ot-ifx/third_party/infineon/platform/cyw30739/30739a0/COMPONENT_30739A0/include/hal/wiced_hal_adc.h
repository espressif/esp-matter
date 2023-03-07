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
* Analog-to-Digital Converter (ADC) driver.
*
*/

#ifndef __WICED_ADC_H__
#define __WICED_ADC_H__

#include "wiced.h"

/**  \addtogroup AdcDriver Analog-to-Digital Converter (ADC)
* \ingroup HardwareDrivers
* @{
* Defines a driver to facilitate interfacing with the Analog-to-Digital
* Converter (ADC) driver. Use this driver to measure a DC voltage via
* a GPIO pin. Voltage measurement values are reported in millivolts (mV).
*
* Note that the ADC channels do not correspond directly to the GPIO pin
* numbering convention (e.g. ADC channel 15 is tied to GPIO P18, etc). Please
* reference the Kit Guide or HW User Manual for your device for more information
* regarding your specific platform.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/


/// Supported ADC input channel selection.
typedef enum ADC_INPUT_CHANNEL_SEL {
    ADC_INPUT_P17           =   0x0,    //GPIO 16
    ADC_INPUT_P16           =   0x1,    //GPIO 17
    ADC_INPUT_P15           =   0x2,    //GPIO 18
    ADC_INPUT_P14           =   0x3,    //GPIO 19
    ADC_INPUT_P13           =   0x4,    //GPIO 20
    ADC_INPUT_P12           =   0x5,    //GPIO 21
    ADC_INPUT_P11           =   0x6,    //GPIO 22
    ADC_INPUT_P10           =   0x7,    //GPIO 23
    ADC_INPUT_P9            =   0x8,    //GPIO 24
    ADC_INPUT_P8            =   0x9,    //GPIO 25
    ADC_INPUT_P1            =   0xA,    //GPIO 26
    ADC_INPUT_P0            =   0xB,    //GPIO 27
    ADC_INPUT_VDDIO         =   0xC,    //ADC_INPUT_VBAT_VDDIO
    ADC_INPUT_VDD_CORE      =   0xD,    //ADC_INPUT_VDDC
    ADC_INPUT_ADC_BGREF     =   0xE,
    ADC_INPUT_ADC_REFGND    =   0xF,
    ADC_INPUT_P38           =   0x10,   //GPIO 0
    ADC_INPUT_P37           =   0x11,   //GPIO 1
    ADC_INPUT_P36           =   0x12,   //GPIO 2
    ADC_INPUT_P35           =   0x13,   //GPIO 3
    ADC_INPUT_P34           =   0x14,   //GPIO 4
    ADC_INPUT_P33           =   0x15,   //GPIO 5
    ADC_INPUT_P32           =   0x16,   //GPIO 6
    ADC_INPUT_P31           =   0x17,   //GPIO 7
    ADC_INPUT_P30           =   0x18,   //GPIO 8
    ADC_INPUT_P29           =   0x19,   //GPIO 9
    ADC_INPUT_P28           =   0x1A,   //GPIO 10
    ADC_INPUT_P23           =   0x1B,   //GPIO 11
    ADC_INPUT_P22           =   0x1C,   //GPIO 12
    ADC_INPUT_P21           =   0x1D,   //GPIO 13
    ADC_INPUT_P19           =   0x1E,   //GPIO 14
    ADC_INPUT_P18           =   0x1F,   //GPIO 15
    ADC_INPUT_CHANNEL_MASK  =   0x1f,
}ADC_INPUT_CHANNEL_SEL;

/// ADC power type
typedef enum ADC_POWER_TYPE
{
    ADC_POWER_DOWN          = 1,
    ADC_POWER_UP            = 0,
}ADC_POWER_TYPE;

/// input  voltage range selection
typedef enum ADC_INPUT_RANGE_SEL
{
    ADC_RANGE_0_3P6V = 0,
    ADC_RANGE_0_1P8V = 1,
}ADC_INPUT_RANGE_SEL;


/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Initialize the ADC hardware to its default state.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_adc_init(void);


///////////////////////////////////////////////////////////////////////////////
/// Calibrate and set the input voltage range expected for the ADC hardware.
///
/// \param range_idx - 0 to 3.6v(0) or 0 to 1.8v(1).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_adc_set_input_range(ADC_INPUT_RANGE_SEL rangeIdx);


///////////////////////////////////////////////////////////////////////////////
/// Read the raw ADC register value for the given channel. The value returned
/// here is direct from the register.
///
/// \param channel - The input channel that corresponds to a GPIO pin.
/// \param avgCnt  - Read adc raw avgCnt+4 times, and choose the middle avgCnt times
///                  to calculate average values
///
/// \return Raw digital value read from the hardware register.
///////////////////////////////////////////////////////////////////////////////
int16_t wiced_hal_adc_read_raw_sample( ADC_INPUT_CHANNEL_SEL channel, uint8_t avgCnt);


///////////////////////////////////////////////////////////////////////////////
/// Read the ADC voltage value for the given channel. The value returned here
/// is converted to a voltage value from the register.
///
/// \param channel - The input channel that corresponds to a GPIO pin.
///
/// \return Converted digital voltage value.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_adc_read_voltage(ADC_INPUT_CHANNEL_SEL channel);


/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_ADC_H__
