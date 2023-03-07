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
*Defines the interfaces for Buffer , Timer and Event Management Services
*/
#ifndef _WICED_GKI_H_
#define _WICED_GKI_H_

#include "wiced.h"

/*****************************************************************************
**                                                 Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_bt_did_stack_overflow
 *
 * Checks if the application thread stack overflowed at some point
 *
 * @return    TRUE : on stack overflow;
 *            FALSE : if no stack overflow
 *
 */
uint8_t wiced_bt_did_stack_overflow(void);

/**
 * Function         wiced_bt_stack_check_init
 *
 * Prepares the stack to allow the app to check for stack overflow.
 *
 */
void wiced_bt_stack_check_init(void);

/**
 * Function         wiced_bt_ble_get_available_tx_buffers
 *
 * Used to get the available number of ble tx buffers
 *
 * Return           the available number of ble tx buffers
 */
uint32_t wiced_bt_ble_get_available_tx_buffers( void );

extern void wiced_hal_wdog_reset_system(void);
#define wdog_generate_hw_reset wiced_hal_wdog_reset_system

uint32_t wiced_get_free_memory(void);//TODO: #define wiced_get_free_memory wiced_memory_get_free_bytes


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif //_WICED_GKI_H_
