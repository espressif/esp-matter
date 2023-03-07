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

/**
 * @file
 * @brief AIROC Voice Prompt Decoder definitions
 */

#pragma once

#include <stdint.h>
#include "wiced.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definitions
 */
#define WICED_VPC_DECODER_HEADER_SIZE       5

#define WICED_VPC_DECODER_INPUT_SIZE_MAX    200

#define WICED_VPC_DECODER_OUTPUT_SAMPLES_NB 320     /* 320 samples every 40ms */

/*
 * wiced_vpc_decoder_init
 */
wiced_result_t wiced_vpc_decoder_init(void);

/*
 * wiced_vpc_decoder_header_parse
 */
wiced_result_t wiced_vpc_decoder_header_parse(uint8_t vpc_header[WICED_VPC_DECODER_HEADER_SIZE],
        uint32_t *p_next_nb_bytes);

/*
 * wiced_vpc_decoder_decode
 */
wiced_result_t wiced_vpc_decoder_decode(uint8_t *p_buffer_in, uint32_t buffer_in_size,
        int16_t *p_pcm_out, uint32_t *p_next_nb_bytes, wiced_bool_t *p_end_stream);


#ifdef __cplusplus
} /* extern "C" */
#endif
