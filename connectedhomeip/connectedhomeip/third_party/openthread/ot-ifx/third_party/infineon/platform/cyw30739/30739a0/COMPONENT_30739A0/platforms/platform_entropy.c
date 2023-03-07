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
#include <wiced_hal_rand.h>


wiced_result_t wiced_platform_entropy_get(uint8_t *output, uint16_t output_length)
{
    uint16_t remain_length;
    uint8_t *p_index = NULL;
    uint32_t rand_num = 0;
    uint8_t i;
    uint8_t shift;

    if (output == NULL)
    {
        return WICED_BADARG;
    }

    remain_length = output_length;
    p_index = output;
    i = 0;

    while (remain_length)
    {
        if (i % sizeof(uint32_t) == 0)
        {
            rand_num = wiced_hal_rand_gen_num();
        }

        shift = 8 * (i % sizeof(uint32_t));
        *p_index = (uint8_t) (rand_num >> shift);

        p_index++;
        i++;
        remain_length--;
    }

    return WICED_SUCCESS;
}
