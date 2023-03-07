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

#include <stddef.h>

#include "wiced_memory.h"
#include "wiced_platform_os.h"

/**
 * \brief       Allocate memory from the dynamic memory pool.
 *
 * @param[in]   size - size of required memory block
 *
 * @retval      address of the allocate memory block
 *
 */
void *wiced_platform_memory_allocate(uint32_t size)
{
    uint32_t *p_addr = NULL;

    wiced_platform_os_preemption_suspend();

    p_addr = (uint32_t *) wiced_memory_allocate(size);

    wiced_platform_os_preemption_resume();

    return p_addr;
}

/**
 * \brief       Free a memory block.
 *
 * @param[in]   p_addr - start address of the memory block to be freed
 */
void wiced_platform_memory_free(void *p_addr)
{
    wiced_platform_os_preemption_suspend();

    wiced_memory_free(p_addr);

    wiced_platform_os_preemption_resume();
}
