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
 *  ======== ECDH.c ========
 *
 *  This file contains default values for the ECDH_Params struct
 *
 */

#include <stdlib.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/ECDH.h>

/* Extern globals */
extern const ECDH_Config ECDH_config[];
extern const uint_least8_t ECDH_count;

const ECDH_Params ECDH_defaultParams = {
    .returnBehavior = ECDH_RETURN_BEHAVIOR_BLOCKING,
    .callbackFxn = NULL,
    .timeout = SemaphoreP_WAIT_FOREVER,
    .custom = NULL,
};

/*
 *  ======== ECDH_open ========
 */
ECDH_Handle ECDH_open(uint_least8_t index, const ECDH_Params *params) {
    DebugP_assert(index < ECDH_count);

    ECDH_Config *config = (ECDH_Config*)&ECDH_config[index];
    return ECDH_construct(config, params);
}

/*
 *  ======== ECDH_OperationGeneratePublicKey_init ========
 */
void ECDH_OperationGeneratePublicKey_init(ECDH_OperationGeneratePublicKey *operation){
    operation->curve = NULL;
    operation->myPrivateKey = NULL;
    operation->myPublicKey = NULL;
    /* Default public key format is octet string */
    operation->publicKeyDataFormat = ECDH_PUBLIC_KEY_DATA_FORMAT_OCTET_STRING;
}
/*
 *  ======== ECDH_OperationComputeSharedSecret_init ========
 */
void ECDH_OperationComputeSharedSecret_init(ECDH_OperationComputeSharedSecret *operation){
    operation->curve = NULL;
    operation->myPrivateKey = NULL;
    operation->theirPublicKey = NULL;
    operation->sharedSecret = NULL;
    /* Default public key format is octet string */
    operation->publicKeyDataFormat = ECDH_PUBLIC_KEY_DATA_FORMAT_OCTET_STRING;
    /* Default shared secret format is octet string */
    operation->sharedSecretDataFormat = ECDH_PUBLIC_KEY_DATA_FORMAT_OCTET_STRING;
}
