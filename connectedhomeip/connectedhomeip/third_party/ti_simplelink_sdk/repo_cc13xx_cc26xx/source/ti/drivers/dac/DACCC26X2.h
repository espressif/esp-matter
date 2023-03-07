/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       DACCC26X2.h
 *  @brief      DAC driver implementation for the DAC peripheral on CC26X2
 *
 *  This DAC driver implementation is designed to operate on a DAC peripheral
 *  for CC26X2.
 *
 *  Refer to @ref DAC.h for a complete description of APIs & example of use.
 *
 ******************************************************************************
 */
#ifndef ti_drivers_dac_DACCC26X2__include
#define ti_drivers_dac_DACCC26X2__include

#include <ti/drivers/DAC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>


#ifdef __cplusplus
extern "C" {
#endif


/*!
 *  @brief  DAC voltage reference setting
 *
 *  The enum defines the DAC voltage reference that the module uses.
 */
typedef enum {
    /*!< Use the Core Voltage Decoupling pin as the DAC's voltage reference source. */
    DAC_VREF_DCOUPL = AUXDAC_VREF_SEL_DCOUPL,
    /*!< Use the ADC reference voltage as the DAC's voltage reference source. */
    DAC_VREF_ADCREF = AUXDAC_VREF_SEL_ADCREF,
    /*!< Use the main supply voltage VDDS as the DAC's voltage reference source. */
    DAC_VREF_VDDS   = AUXDAC_VREF_SEL_VDDS,
} DAC_VrefSource;

/*!
 *  @brief  DACCC26XX Hardware attributes
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions.
 *
 */
typedef struct {
    /*!< Pin used for DAC output */
    uint_least8_t       outputPin;
    /*!< DAC voltage reference source */
    DAC_VrefSource      dacVrefSource;
    /*!< Internal signal routed to COMPA_IN */
    uint8_t             dacCompAInput;
    /*!< Flag to determine precharge state when DCOUPL has been selected as voltage reference */
    bool                dacPrecharge;
} DACCC26XX_HWAttrs;

/*!
 *  @brief  DACCC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /*!< Flag to determine current state of the DAC's output */
    bool                isEnabled;
    /*!< Flag if the instance is in use */
    bool                isOpen;
    /*!< Current DAC code set */
    uint32_t            currCode;
    /*!< Minimum DAC output in uV */
    uint32_t            dacOutputMin;
    /*!< Maximum DAC output in uV */
    uint32_t            dacOutputMax;
} DACCC26XX_Object;



#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_dac_DACCC26X2__include */
