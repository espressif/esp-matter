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
#pragma once

#include "wiced.h"
#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
/**
 * determine size (number of elements) in an array
 */
#define ARRAY_SIZE(a)                                ( sizeof(a) / sizeof(a[0]) )

/**
 * Macro to determine the element index in an array from the element address
 */
#define ARRAY_POSITION( array, element_pointer )     ( ((uint32_t)element_pointer - (uint32_t)array) / sizeof(array[0]) )

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
/* Logical Button-ids which map to phyiscal buttons on the board */
typedef enum
{
    PLATFORM_BUTTON_1,
	PLATFORM_BUTTON_2,
	PLATFORM_BUTTON_3,
	PLATFORM_BUTTON_4,
	PLATFORM_BUTTON_5,
	PLATFORM_BUTTON_6,
	PLATFORM_BUTTON_MAX,
} platform_button_t;
/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void (*platform_button_state_change_callback_t)( platform_button_t id, wiced_bool_t new_state );

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
extern wiced_result_t  platform_button_init( platform_button_t button );
extern wiced_result_t  platform_button_deinit( platform_button_t button );
extern wiced_result_t  platform_button_enable( platform_button_t button );
extern wiced_result_t  platform_button_disable( platform_button_t button );
extern wiced_bool_t    platform_button_get_value( platform_button_t button );
extern wiced_result_t  platform_button_register_state_change_callback( platform_button_state_change_callback_t callback );

#ifdef __cplusplus
} /* extern "C" */
#endif
