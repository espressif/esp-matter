/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
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
 *  ======== AESCCM.c ========
 *
 *  This file contains default values for the AESCCM_Params struct.
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drivers/AESCCM.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Extern globals */
extern const AESCCM_Config AESCCM_config[];
extern const uint_least8_t AESCCM_count;

const AESCCM_Params AESCCM_defaultParams = {
    .returnBehavior = AESCCM_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== AESCCM_Params_init ========
 */
void AESCCM_Params_init(AESCCM_Params *params){
    *params = AESCCM_defaultParams;
}

/*
 *  ======== AESCCM_open ========
 */
AESCCM_Handle AESCCM_open(uint_least8_t index, const AESCCM_Params *params) {
    DebugP_assert(index < AESCCM_count);

    AESCCM_Config *config = (AESCCM_Config*)&AESCCM_config[index];
    return AESCCM_construct(config, params);
}

/*
 *  ======== AESCCM_Operation_init ========
 */
void AESCCM_Operation_init(AESCCM_Operation *operationStruct) {
    memset(operationStruct, 0x00, sizeof(AESCCM_Operation));
}

/*
 *  ======== AESCCM_OneStepOperation_init ========
 */
void AESCCM_OneStepOperation_init(AESCCM_OneStepOperation *operationStruct) {
    memset(operationStruct, 0x00, sizeof(AESCCM_OneStepOperation));
}

/*
 *  ======== AESCCM_SegmentedAADOperation_init ========
 */
void AESCCM_SegmentedAADOperation_init(AESCCM_SegmentedAADOperation *operationStruct) {
    memset(operationStruct, 0x00, sizeof(AESCCM_SegmentedAADOperation));
}

/*
 *  ======== AESCCM_SegmentedDataOperation_init ========
 */
void AESCCM_SegmentedDataOperation_init(AESCCM_SegmentedDataOperation *operationStruct) {
    memset(operationStruct, 0x00, sizeof(AESCCM_SegmentedDataOperation));
}

/*
 *  ======== AESCCM_SegmentedFinalizeOperation_init ========
 */
void AESCCM_SegmentedFinalizeOperation_init(AESCCM_SegmentedFinalizeOperation *operationStruct) {
    memset(operationStruct, 0x00, sizeof(AESCCM_SegmentedFinalizeOperation));
}
