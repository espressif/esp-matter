/*
 * Copyright 2021, Cypress Semiconductor Corporation (an Infineon company) or
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

/* Include the actual RTOS definitions for:
 * - wiced_timed_event_t
 * - timed_event_handler_t
 */
#include "wiced_result.h"
#include "wiced_bt_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    WICED_RTOS_DEFER_TO_MPAF_THREAD,
    WICED_RTOS_DEFER_TO_LM_THREAD,
    WICED_RTOS_MAX_DEFER_THREADS_SUPPORTED
}wiced_defer_context_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

/** Defer execution of callback function to a different thread (currently only App and LM threads are supported)
 *
 * @param new_thread_context : thread context to which the execution is being defered to
 * @param func               : Callback function to be invoked from the thread context chosen above
 * @param data               : Data to be given to the above function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_BADARG   : if an invalid arg is received
 */
wiced_result_t wiced_rtos_defer_execution(wiced_defer_context_t new_thread_context, void (*func)(void *), void *data);

/** @} */


#ifdef __cplusplus
} /*extern "C" */
#endif
