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

/******************************************************************************
**
**  File Name:   sbc_plc_bec_tgt.h
**
**  Description: This file enables / disable optional feature of the SBC PLC BEC library

**
******************************************************************************/

/* Include MONO A2DP code */
#define SBC_DEC_MONO_A2DP_INCLUDED FALSE

/* Exclude ATTENUATION code */
#define SBC_ATTENUATION_INCLUDED FALSE

/* Exclude MIXING code */
#define SBC_MIXING_INCLUDED FALSE

/* Exclude Stereo 32 kHz downsampling code */
#define SBC_STEREO_32K_DOWNSAMPLED_16K FALSE

/* Exclude FIR Filter Convolution */
#define SBC_CONVOLUTION_FILTER_INCLUDED FALSE


/* Exclude Sample Rate Converter for 2074 A2 only. This is to have the SBC PCM output dowsampled to 16kHz.  This is only for for 2074 A2 where only DAC does support Frequency sampling up to 16 kHz only */
#define SBC_SAMPLE_RATE_CONVERTER_16K FALSE
