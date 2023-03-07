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

/**************************************************************************//**
* \file <wiced_thermistor.h>
* List of parameters and defined functions needed to access the
* thermistor sensor driver.
*
******************************************************************************/
#ifndef _WICED_THERMISTOR_H
#define _WICED_THERMISTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Following structure is used to initialize thermistor
 */
typedef struct
{
    uint16_t                high_pin;           /*  A/D input high pin    */
    uint16_t                low_pin;            /*  A/D input low pin (not used for thermistor ncu15wf104)   */
    uint16_t                adc_power_pin;      /*  ADC power pin (not used for thermistor ncu15wf104) */
} thermistor_cfg_t;

/******************************************************************************
* Function Name: therm_init
***************************************************************************//**
* init thermistor.
*
*
* \return None
******************************************************************************/
void thermistor_init(void);

/******************************************************************************
* Function Name: thermistor_read
***************************************************************************//**
* read thermistor temperature.
*
* \param  p_thermistor_cfg: pointer to thermister config structure
*
* \return Temperature in degrees Celsius * 100.
******************************************************************************/
int16_t thermistor_read(thermistor_cfg_t *p_thermistor_cfg);


#ifdef __cplusplus
}
#endif

#endif
