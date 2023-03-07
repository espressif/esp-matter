/******************************************************************************

 @file task_config.h

 @brief This file contains the definitions of the RTOS task priorities and
        stack sizes.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/**
 * Priority of the OpenThread Stack task.
 */
#ifndef TASK_CONFIG_OT_TASK_PRIORITY
#define TASK_CONFIG_OT_TASK_PRIORITY    1
#endif

/**
 * Size of the OpenThread Stack task call stack.
 *
 * NOTE: last stack peak tested at 3280 bytes (conformance test 8.2.1 CCS
 *       toolchain 2020-6-9). Adding 20% and rounding to largest 1K for round
 *       numbers.
 */
#ifndef TASK_CONFIG_OT_TASK_STACK_SIZE
#define TASK_CONFIG_OT_TASK_STACK_SIZE  4096
#endif

/**
 * Priority of the Application task.
 */
#ifndef TASK_CONFIG_TEMPSENSOR_TASK_PRIORITY
#define TASK_CONFIG_TEMPSENSOR_TASK_PRIORITY   2
#endif

/**
 * Size of the cli task call stack.
 */
#ifndef TASK_CONFIG_TEMPSENSOR_TASK_STACK_SIZE
#define TASK_CONFIG_TEMPSENSOR_TASK_STACK_SIZE 1096
#endif

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * Creation funciton for the OpenThread Stack task.
 */
extern void OtStack_taskCreate(void);

/**
 * Creation funciton for the temperature sensor application task.
 */
extern void TempSensor_taskCreate(void);

#ifdef __cplusplus
}
#endif

#endif /* _TASK_CONFIG_H_ */

