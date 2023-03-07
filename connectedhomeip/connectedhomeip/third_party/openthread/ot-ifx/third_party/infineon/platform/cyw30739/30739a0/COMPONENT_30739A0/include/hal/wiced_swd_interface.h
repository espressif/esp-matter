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

/*******************************************************************************
 * All common definitions for configuring SWD interface pins
 *******************************************************************************/

#ifndef _WICED_SWD_INTERFACE__H_
#define _WICED_SWD_INTERFACE__H_

#include "wiced_hal_gpio.h"

 /* Enum of all the possible pins through which SWDCK can be routed. */
typedef enum
{
    SWDCK_DISABLED = -1,
    SWDCK_ON_P01
}SWDCK_pin_t;

/* Enum of all the possible pins through which SWDIO can be routed. */
typedef enum
{
    SWDIO_DISABLED = -1,
    SWDIO_ON_P00
}SWDIO_pin_t;

#ifdef __cplusplus
extern "C" {
#endif


/* Choose the pins for SWDIO and SWDCK */
void wiced_configure_swd_pins(SWDCK_pin_t swdck_pin, SWDIO_pin_t swdio_pin);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* _WICED_SWD_INTERFACE__H_ */
