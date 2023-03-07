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
// ******************************************************************************
//  File Name: wiced_hal_audiocodec_interface.h
//
//  Abstract:  This file defines the Audio Codec Interface.
//
//  History:
//  *******************************************************************************

#ifndef __WICED_AUDIOCODEC_INTERFACE_H__
#define __WICED_AUDIOCODEC_INTERFACE_H__

#include "brcm_fw_types.h"
#include "wiced.h"

/**
* \addtogroup  interfaces   Audio Codec Interface
* \ingroup     wicedbt_av
*/
/*! @{ */
/**
* Defines an audio codec interface.  Application uses this
* interface to control external audio codec activities.
*/

typedef struct
{
    void (*initialize)(uint16_t NumChannles, uint32_t SampleRate, uint16_t BitsPerSample, uint8_t gain, uint8_t boost);
    void (*init)(void);

    uint8_t (*readSR)(void);
    uint8_t (*readBPS)(void);
    uint8_t (*readPCM)(void);
    uint16_t (*readHPF)(void);
    int16_t (*readPGA)(void);

    uint8_t (*writeSR)(uint8_t p);
    uint8_t (*writeBPS)(uint8_t p);
    uint8_t (*writePCM)(uint8_t p);
    uint8_t (*writeHPF)(uint16_t h);
    uint8_t (*writePGA)(int16_t g);

    uint16_t (*regRead)(int8_t index);
    void (*regWrite)(int8_t index, uint16_t data);
} wiced_audio_codec_interface_func_tbl;

#endif
