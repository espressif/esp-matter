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

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)

/* Crypto driver semaphore used to synchronize accesses to the keyStore, AES, and SHA2 engine */
SemaphoreP_Struct CryptoResourceCC26XX_accessSemaphore;
SemaphoreP_Struct CryptoResourceCC26XX_operationSemaphore;

volatile bool CryptoResourceCC26XX_pollingFlag = 0;

HwiP_Struct CryptoResourceCC26XX_hwi;

static bool isInitialized = false;

static void errorSpin(uintptr_t arg) {
    while(1);
}

void CryptoResourceCC26XX_constructRTOSObjects(void) {
    HwiP_Params hwiParams;
    uint_fast8_t key;

    key = HwiP_disable();

    if (!isInitialized){
        /* Construct the common Hwi with a dummy ISR function. This should not matter as the function is set
         * whenever we start an operation after pending on CryptoResourceCC26XX_accessSemaphore
         */
        HwiP_Params_init(&hwiParams);
        hwiParams.priority = ~0;
        hwiParams.enableInt = false;
        HwiP_construct(&(CryptoResourceCC26XX_hwi), INT_CRYPTO_RESULT_AVAIL_IRQ, errorSpin, &hwiParams);

        SemaphoreP_constructBinary(&CryptoResourceCC26XX_accessSemaphore, 1);
        SemaphoreP_constructBinary(&CryptoResourceCC26XX_operationSemaphore, 0);

        isInitialized = true;
    }

    HwiP_restore(key);
}

bool CryptoResourceCC26XX_acquireLock(uint32_t timeout) {
    SemaphoreP_Status resourceAcquired;

    /* Try and obtain access to the crypto module */
    resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore,
                                       timeout);

    return resourceAcquired == SemaphoreP_OK;
}

void CryptoResourceCC26XX_releaseLock() {
    SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
}
