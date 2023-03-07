/*
 * Copyright (c) 2021-2022, Texas Instruments Incorporated
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
 *  ======== GPIO.c ========
 */
#include <stdint.h>
#include <string.h>

#include <ti/drivers/GPIO.h>

extern GPIO_Config GPIO_config;

/*
 *  ======== GPIO_setCallback ========
 */
void GPIO_setCallback(uint_least8_t index, GPIO_CallbackFxn callback)
{
    /*
     * Only update callbacks entry if different.
     * This allows the callbacks array to be in flash for static systems.
     */
    if (index != GPIO_INVALID_INDEX && GPIO_config.callbacks[index] != callback)
    {
        GPIO_config.callbacks[index] = callback;
    }
}

/*
 *  ======== GPIO_getCallback ========
 */
GPIO_CallbackFxn GPIO_getCallback(uint_least8_t index)
{
    return GPIO_config.callbacks[index];
}

/*
 *  ======== GPIO_setUserArg ========
 */
void GPIO_setUserArg(uint_least8_t index, void* arg)
{
    if (index != GPIO_INVALID_INDEX)
    {
        GPIO_config.userArgs[index] = arg;
    }
}

/*
 *  ======== GPIO_getUserArg ========
 */
void* GPIO_getUserArg(uint_least8_t index)
{
    return GPIO_config.userArgs[index];
}

/*
 *  ======== GPIO_resetConfig ========
 */

#if (DeviceFamily_ID != DeviceFamily_ID_CC3220)
void GPIO_resetConfig(uint_least8_t index)
{
    if (index != GPIO_INVALID_INDEX)
    {
        GPIO_disableInt(index);
        GPIO_setConfig(index, GPIO_config.configs[index]);
        GPIO_setCallback(index, NULL);
        GPIO_setUserArg(index, NULL);
    }
}
#endif
