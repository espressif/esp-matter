 /*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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

/*
 *  ======== drv5055.c ========
 */

/* Module Header */
#include <ti/common/sail/drv5055/drv5055.h>

/*
 *  ======== DRV5055_getMagneticFLux ========
 *  Get current magnetic flux reading
 */

float DRV5055_getMagneticFlux(ADC_Handle adc, float sensitivity, float offset, DRV5055_opvoltage volt)
{
    uint16_t adcValue;
    uint32_t adcValueMicroVolt;
    int_fast16_t res;
    float magneticFlux = 0.0f;

    /* Validate Sensitivity levels if out of range */
    if(volt == DRV5055_3_3V)
    {
        if(sensitivity < DRV5055A1_3_3V)
        {
            sensitivity = DRV5055A1_3_3V;
        }
        else if(sensitivity > DRV5055A4_3_3V)
        {
            sensitivity = DRV5055A4_3_3V;
        }
    }
     if(volt == DRV5055_5_0V)
    {
        if(sensitivity < DRV5055A1_5V)
        {
            sensitivity = DRV5055A1_5V;
        }
        else if(sensitivity > DRV5055A4_5V)
        {
            sensitivity = DRV5055A4_5V;
        }
    }
 

    res = ADC_convert(adc, &adcValue);

    if (ADC_STATUS_SUCCESS == res) {

        /* DRV5055 output in micro volts. Range from 0V to 3.2V */
        adcValueMicroVolt = ADC_convertRawToMicroVolts(adc, adcValue);

        magneticFlux = (float)adcValueMicroVolt;
        magneticFlux = (magneticFlux /1000U);
        magneticFlux = magneticFlux - offset;

        /* Convert Voltage to milliTesla*/
        magneticFlux = magneticFlux / sensitivity;

        return (magneticFlux);
    }
    else {
        return 0;
    }
}
