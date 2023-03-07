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

/** ============================================================================
 *  @file       drv5055.h
 *
 *  @brief      DRV5055 Automotive Ratiometric Linear Hall Effect Sensor
 *
 *  The DRV5055-Q1 is a linear Hall effect sensor that responds proportionally
 *  to magnetic flux density. It can be used for accurate position sensing in
 *  a wide range of applications.
 *
 *  The DRV5055 header file should be included in an application as
 *  follows:
 *  @code
 *  #include <ti/common/sail/drv5055/drv5055.h>
 *  @endcode
 *
 *  # Operation
 *  The DRV5055 driver simplifies using a DRV5055 sensor to perform magnetic
 *  flux readings. The DRV5055 sensor's output is analog with voltage ranging
 *  from 0 to Vcc. With zero magnetic flux, the sensor's output is Vcc/2. A
 *  value less than Vcc/2 indicates north pole shift and value greater than Vcc/2
 *  indicate south pole shift. The DRV5055 driver routes the analog output to
 *  ADC and converts its value to magnetic flux with in the range of the sensor.
 *
 *  The API in this driver serve as an interface to a DPL(Driver Porting Layer)
 *  The specific implementations are responsible for creating all the RTOS
 *  specific primitives to allow for thread-safe operation.
 *
 *  This driver has no dynamic memory allocation.
 *
 *  ============================================================================
 */

#ifndef DRV5055_H_
#define DRV5055_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Driver Header files */
#include <ti/drivers/ADC.h>

 /** \defgroup grp_sensitivity of the DRV5055 sensor with 3.3V supply
  */
 /**@{*/
#define DRV5055A1_3_3V 60.0f  /*!< Sensitivity = 60 mV/mT  */
#define DRV5055A2_3_3V 30.0f  /*!< Sensitivity = 30 mV/mT  */
#define DRV5055A3_3_3V 15.0f  /*!< Sensitivity = 15 mV/mT  */
#define DRV5055A4_3_3V 7.5f /*!< Sensitivity = 7.5 mV/mT */
 /**@}*/

 /** \defgroup grp_sensitivity of the DRV5055 sensor with 5V supply
  */
 /**@{*/
#define DRV5055A1_5V 100.0f  /*!< Sensitivity = 100 mV/mT  */
#define DRV5055A2_5V 50.0f  /*!< Sensitivity = 50 mV/mT  */
#define DRV5055A3_5V 25.0f  /*!< Sensitivity = 25 mV/mT  */
#define DRV5055A4_5V 12.5f /*!< Sensitivity = 12.5 mV/mT */
 /**@}*/

typedef enum DRV5055_opvoltage{
	DRV5055_3_3V   = 0U,
	DRV5055_5_0V   = 1U
}DRV5055_opvoltage;
/*!
 *  @brief  Function to get the sensor's magnetic flux reading
 *
 *  @param  ADC_handle   Handle to the ADC connected to the DRV5055 output
 *
 *  @param  sensitivity  Sensitivity of the DRV5055.
 *                       Flexible magnetic sensitivity options:
 *                           DRV5055A1: 7.5 mV/mT sensitivity, +/-22 mT linear range
 *                           DRV5055A2: 15 mV/mT sensitivity, +/-44 mT linear range
 *                           DRV5055A3: 30 mV/mT sensitivity, +/-88 mT linear range
 *                           DRV5055A4: 60 mV/mT sensitivity, +/-176 mT linear range
 * @param  offset        Set this value in mV at which the magnetic field given by 
 *                       drv5055 is 0mT .
 *
 * @param  volt          Set this paramete to determine if the operation is at 3.3V
 *                       or 5V.
 *
 *  @return value of the Magnetic Flux.
 */
extern float DRV5055_getMagneticFlux(ADC_Handle adc, float sensitivity,float offset, DRV5055_opvoltage volt);

#ifdef __cplusplus
}
#endif

#endif /* DRV5055_H_ */
