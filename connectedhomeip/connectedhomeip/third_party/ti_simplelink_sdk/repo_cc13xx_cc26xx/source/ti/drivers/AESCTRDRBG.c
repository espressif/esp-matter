/*
 * Copyright (c) 2019, Texas Instruments Incorporated
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
 *  ======== AESCTRDRBG.c ========
 *
 *  This file contains default values for the AESCTRDRBG_Params struct.
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drivers/AESCTRDRBG.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Extern globals */
extern const AESCTRDRBG_Config AESCTRDRBG_config[];
extern const uint_least8_t AESCTRDRBG_count;

const AESCTRDRBG_Params AESCTRDRBG_defaultParams = {
    .keyLength = AESCTRDRBG_AES_KEY_LENGTH_128,
    .reseedInterval = 10000,
    .seed = NULL,
    .personalizationData = NULL,
    .personalizationDataLength = 0,
    .returnBehavior = AESCTRDRBG_RETURN_BEHAVIOR_POLLING,
    .custom = NULL,
};

/*
 *  ======== AESCTRDRBG_open ========
 */
AESCTRDRBG_Handle AESCTRDRBG_open(uint_least8_t index, const AESCTRDRBG_Params *params) {
    DebugP_assert(index < AESCTRDRBG_count);

    AESCTRDRBG_Config *config = (AESCTRDRBG_Config*)&AESCTRDRBG_config[index];
    return AESCTRDRBG_construct(config, params);
}

/*
 *  ======== AESCTRDRBG_Params_init ========
 */
void AESCTRDRBG_Params_init(AESCTRDRBG_Params *params){
    *params = AESCTRDRBG_defaultParams;
}
