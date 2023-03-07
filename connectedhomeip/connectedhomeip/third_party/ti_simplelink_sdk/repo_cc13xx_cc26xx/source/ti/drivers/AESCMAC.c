/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated
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
 *  ======== AESCMAC.c ========
 *
 *  This file contains default values for the AESCMAC_Params struct.
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drivers/AESCMAC.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Extern globals */
extern const AESCMAC_Config AESCMAC_config[];
extern const uint_least8_t AESCMAC_count;

const AESCMAC_Params AESCMAC_defaultParams = {
    .returnBehavior = AESCMAC_RETURN_BEHAVIOR_BLOCKING,
    .operationalMode = AESCMAC_OPMODE_CMAC,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== AESCMAC_Params_init ========
 */
void AESCMAC_Params_init(AESCMAC_Params *params){
    *params = AESCMAC_defaultParams;
}

/*
 *  ======== AESCMAC_open ========
 */
AESCMAC_Handle AESCMAC_open(uint_least8_t index, const AESCMAC_Params *params) {
    DebugP_assert(index < AESCMAC_count);

    AESCMAC_Config *config = (AESCMAC_Config*)&AESCMAC_config[index];
    return AESCMAC_construct(config, params);
}

/*
 *  ======== AESCMAC_Operation_init ========
 */
void AESCMAC_Operation_init(AESCMAC_Operation *operation) {
    memset(operation, 0x00, sizeof(AESCMAC_Operation));
}
