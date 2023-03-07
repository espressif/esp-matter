/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
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
 *  ======== ECJPAKE.c ========
 *
 *  This file contains default values for the ECJPAKE_Params struct
 *
 */

#include <stdlib.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/ECJPAKE.h>

extern const ECJPAKE_Config ECJPAKE_config[];
extern const uint_least8_t ECJPAKE_count;

const ECJPAKE_Params ECJPAKE_defaultParams = {
    .returnBehavior = ECJPAKE_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== ECJPAKE_Params_init ========
 */
void ECJPAKE_Params_init(ECJPAKE_Params *params){
    *params = ECJPAKE_defaultParams;
}

/*
 *  ======== ECJPAKE_open ========
 */
ECJPAKE_Handle ECJPAKE_open(uint_least8_t index, ECJPAKE_Params *params) {
    DebugP_assert(index < ECJPAKE_count);

    ECJPAKE_Config *config = (ECJPAKE_Config*) &ECJPAKE_config[index];
    return ECJPAKE_construct(config, params);
}

/*
 *  ======== ECJPAKE_OperationRoundOneGenerateKeys_init ========
 */
void ECJPAKE_OperationRoundOneGenerateKeys_init(ECJPAKE_OperationRoundOneGenerateKeys *operation){
    memset(operation, 0x00, sizeof(ECJPAKE_OperationRoundOneGenerateKeys));
}

/*
 *  ======== ECJPAKE_OperationGenerateZKP_init ========
 */
void ECJPAKE_OperationGenerateZKP_init(ECJPAKE_OperationGenerateZKP *operation){
    memset(operation, 0x00, sizeof(ECJPAKE_OperationGenerateZKP));
}

/*
 *  ======== ECJPAKE_OperationVerifyZKP_init ========
 */
void ECJPAKE_OperationVerifyZKP_init(ECJPAKE_OperationVerifyZKP *operation){
    memset(operation, 0x00, sizeof(ECJPAKE_OperationVerifyZKP));
}

/*
 *  ======== ECJPAKE_OperationRoundTwoGenerateKeys_init ========
 */
void ECJPAKE_OperationRoundTwoGenerateKeys_init(ECJPAKE_OperationRoundTwoGenerateKeys *operation){
    memset(operation, 0x00, sizeof(ECJPAKE_OperationRoundTwoGenerateKeys));
}

/*
 *  ======== ECJPAKE_OperationComputeSharedSecret_init ========
 */
void ECJPAKE_OperationComputeSharedSecret_init(ECJPAKE_OperationComputeSharedSecret *operation){
    memset(operation, 0x00, sizeof(ECJPAKE_OperationComputeSharedSecret));
}
