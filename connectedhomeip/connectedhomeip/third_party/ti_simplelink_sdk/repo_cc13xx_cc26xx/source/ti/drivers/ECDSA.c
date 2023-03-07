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
 *  ======== ECDSA.c ========
 *
 *  This file contains default values for the ECDSA_Params struct
 *
 */

#include <stdlib.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/ECDSA.h>

/* Extern globals */
extern const ECDSA_Config ECDSA_config[];
extern const uint_least8_t ECDSA_count;

const ECDSA_Params ECDSA_defaultParams = {
    .returnBehavior = ECDSA_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== ECDSA_Params_init ========
 */
void ECDSA_Params_init(ECDSA_Params *params){
    *params = ECDSA_defaultParams;
}

/*
 *  ======== ECDSA_open ========
 */
ECDSA_Handle ECDSA_open(uint_least8_t index, const ECDSA_Params *params) {
    DebugP_assert(index < ECDSA_count);

    ECDSA_Config *config = (ECDSA_Config*)&ECDSA_config[index];
    return ECDSA_construct(config, params);
}

/*
 *  ======== ECDSA_OperationSign_init ========
 */
void ECDSA_OperationSign_init(ECDSA_OperationSign *operation){
    memset(operation, 0x00, sizeof(ECDSA_OperationSign));
}

/*
 *  ======== ECDSA_OperationVerify_init ========
 */
void ECDSA_OperationVerify_init(ECDSA_OperationVerify *operation){
    memset(operation, 0x00, sizeof(ECDSA_OperationVerify));
}
