/***************************************************************************//**
 * @file
 * @brief Driver for the SPV1840LR5H-B MEMS Microphone
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef MIC_H
#define MIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "mic_config.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup Mic
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define MIC_ADC_ACQ_TIME      MIC_CONFIG_ADC_ACQ_TIME    /**< ADC acquisition time  */
#define MIC_ADC_CLOCK_FREQ    MIC_CONFIG_ADC_CLOCK_FREQ  /**< ADC clock frequency   */
/** @endcond */

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define MIC_OK                0                          /**< No errors */
/**@}*/

uint32_t  MIC_init           (uint32_t fs, uint16_t *sampleBuffer, size_t len);
void      MIC_deInit         (void);
void      MIC_start          (uint32_t nsamples);
bool      MIC_isBusy         (void);

float     MIC_getSoundLevel  (float *var);
float     MIC_getMean        (void);
uint16_t *MIC_getSampleBuffer(void);
size_t    MIC_getSampleCount (void);

/** @} */
/** @} */

#endif // MIC_H
