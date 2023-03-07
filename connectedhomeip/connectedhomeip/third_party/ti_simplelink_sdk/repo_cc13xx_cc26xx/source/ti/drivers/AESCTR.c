/*
 * Copyright (c) 2018-2021, Texas Instruments Incorporated
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
 *  ======== AESCTR.c ========
 *
 *  This file contains default values for the AESCTR_Params struct.
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drivers/AESCTR.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Extern globals (board file) */
extern const AESCTR_Config AESCTR_config[];
extern const uint_least8_t AESCTR_count;

const AESCTR_Params AESCTR_defaultParams = {
    .returnBehavior = AESCTR_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== AESCTR_Params_init ========
 */
void AESCTR_Params_init(AESCTR_Params *params){
    *params = AESCTR_defaultParams;
}

/*
 *  ======== AESCTR_open ========
 */
AESCTR_Handle AESCTR_open(uint_least8_t index, const AESCTR_Params *params) {
    DebugP_assert(index < AESCTR_count);

    AESCTR_Config *config = (AESCTR_Config*)&AESCTR_config[index];
    return AESCTR_construct(config, params);
}

/*
 *  ======== AESCTR_Operation_init ========
 */
void AESCTR_Operation_init(AESCTR_Operation *operation) {
    AESCTR_OneStepOperation_init(operation);
}

/*
 *  ======== AESCTR_Operation_init ========
 */
void AESCTR_OneStepOperation_init(AESCTR_OneStepOperation *operation) {
    memset(operation, 0x00, sizeof(AESCTR_OneStepOperation));
}

/*
 *  ======== AESCTR_SegmentedOperation_init ========
 */
void AESCTR_SegmentedOperation_init(AESCTR_SegmentedOperation *operation) {
    memset(operation, 0x00, sizeof(AESCTR_SegmentedOperation));
}
