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
#ifndef __HSL_RGB_LIB_H
#define __HSL_RGB_LIB_H


#define HSL_COLOR_MIN_HUE                           (0)
#define HSL_COLOR_MAX_HUE                           (65535)
#define HSL_COLOR_MIN_SATURATION                    (0)
#define HSL_COLOR_MAX_SATURATION                    (65535)
#define HSL_COLOR_MIN_LIGHTNESS                     (0)
#define HSL_COLOR_MAX_LIGHTNESS                     (65535)
#define RGB_COLOR_MIN_RED                           (0)
#define RGB_COLOR_MAX_RED                           (255)
#define RGB_COLOR_MIN_GREEN                         (0)
#define RGB_COLOR_MAX_GREEN                         (255)
#define RGB_COLOR_MIN_BLUE                          (0)
#define RGB_COLOR_MAX_BLUE                          (255)

#define NULL_VALUE  0

/******************************************************************************
* Function Name: hsl_2_rgb
***************************************************************************//**
* Convert HSL Color values to RGB Color values.
*
* \param hue            Input, HSL color hue value, value range: 0 ~ 65535, unit: degree.
* \param saturation     Input, HSL color saturation value, value range: 0 ~ 65535, unit: percentage.
* \param lightness      Input, HSL color lightness, value range: 0 ~ 65535, unit: percentage.
* \param red            Output, RBG color red value, value range: 0 ~255.
* \param green          Output, RBG color green value, value range: 0 ~255.
* \param blue           Output, RBG color blue value, value range: 0 ~255.
*
*
* \return None
******************************************************************************/
int hsl_2_rgb(int hue, int saturation, int lightness, int *red, int *green, int *blue);

#endif
