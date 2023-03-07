/******************************************************************************

 @file  sensor.h

 @brief Interface to sensor driver shared code.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef HAL_SENSOR_H
#define HAL_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdbool.h"
#include "stdint.h"

/*********************************************************************
 * CONSTANTS and MACROS
 */

/* Sensor bit values for self-test */
#define ST_IRTEMP                             0x01
#define ST_HUMIDITY                           0x02
#define ST_LIGHT                              0x04
#define ST_PRESSURE                           0x08
#define ST_MPU                                0x10

#define ST_ALL                                ( ST_IRTEMP | ST_HUMIDITY | ST_LIGHT | ST_PRESSURE | ST_MPU )

/* Self test assertion; return FALSE (failed) if condition is not met */
#define ST_ASSERT(cond) st( if (!(cond)) {bspI2cRelease(); return false;} )

/* Data to when an error occurs */
#define ST_ERROR_DATA                         0xCC

/* Conversion macros */
#define st(x)      do { x } while (__LINE__ == -1)

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


/*********************************************************************
 * FUNCTIONS
 */
uint16_t sensorTest(void);
bool     sensorReadReg(uint8_t addr, uint8_t *pBuf, uint8_t nBytes);
bool     sensorWriteReg(uint8_t addr, uint8_t *pBuf, uint8_t nBytes);
void     sensorSetErrorData(uint8_t *pBuf, uint8_t nBytes);

uint16_t floatToSfloat(float data);
float    sfloatToFloat(uint16_t rawData);
uint16_t intToSfloat(int data);
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HAL_SENSOR_H */
