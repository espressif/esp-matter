/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
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
 *  ======== EDDSA.c ========
 *
 *  This file contains default values for the EDDSA_Params struct
 *
 */

#include <stdlib.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/EDDSA.h>

/* Extern globals */
extern const EDDSA_Config EDDSA_config[];
extern const uint_least8_t EDDSA_count;

const EDDSA_Params EDDSA_defaultParams = {
    .returnBehavior = EDDSA_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== EDDSA_Params_init ========
 */
void EDDSA_Params_init(EDDSA_Params *params){
    *params = EDDSA_defaultParams;
}

/*
 *  ======== EDDSA_open ========
 */
EDDSA_Handle EDDSA_open(uint_least8_t index, const EDDSA_Params *params) {
    DebugP_assert(index < EDDSA_count);

    EDDSA_Config *config = (EDDSA_Config*)&EDDSA_config[index];
    return(EDDSA_construct(config, params));
}

/*
 *  ======== EDDSA_OperationGeneratePublicKey_init ========
 */
void EDDSA_OperationGeneratePublicKey_init(EDDSA_OperationGeneratePublicKey
                                           *operation){
    memset(operation, 0x00, sizeof(EDDSA_OperationGeneratePublicKey));
}

/*
 *  ======== EDDSA_OperationSign_init ========
 */
void EDDSA_OperationSign_init(EDDSA_OperationSign *operation){
    memset(operation, 0x00, sizeof(EDDSA_OperationSign));
}

/*
 *  ======== EDDSA_OperationVerify_init ========
 */
void EDDSA_OperationVerify_init(EDDSA_OperationVerify *operation){
    memset(operation, 0x00, sizeof(EDDSA_OperationVerify));
}
