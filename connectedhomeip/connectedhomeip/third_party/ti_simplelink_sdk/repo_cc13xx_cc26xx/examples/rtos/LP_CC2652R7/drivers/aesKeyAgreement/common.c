/*
 * Copyright (c) 2018-2019, Texas Instruments Incorporated
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
 *  ======== common.c ========
 */
#include <string.h>

/* Driver Header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/ECDH.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/ECDSA.h>
#include <ti/drivers/AESCCM.h>

#include <ti/drivers/cryptoutils/ecc/ECCParams.h>

/* XDC Module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Module Headers */
#include <ti/sysbios/knl/Task.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/trng.h)

/* Driver configuration */
#include "ti_drivers_config.h"
#include "common.h"

/* Structs and storage space for the two main tasks */
static Task_Struct txTask;
static Task_Struct rxTask;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */
#define STACKSIZE 1024
static uint8_t txTaskStack[STACKSIZE] __attribute__ ((aligned (8)));
static uint8_t rxTaskStack[STACKSIZE] __attribute__ ((aligned (8)));

UART_Handle uart;
UART_Params uartParams;

/* Wrapper to allow both tasks to share a UART Handle */
int32_t COM_uartTransmit(void *buff, size_t count) {
    return UART_write(uart, buff, count);
}

/* Wrapper to allow both tasks to share a UART Handle */
int32_t COM_uartReceive(void *buff, size_t count) {
    return UART_read(uart, buff, count);
}

/* Generate random bytes in the provided buffer up to size using the TRNG */
void COM_generateRandomBytes(CryptoKey *entropyKey) {
    TRNG_Handle handle;
    int_fast16_t result;

    /* Open a TRNG_Handle with default parameters */
    handle = TRNG_open(CONFIG_TRNG_0, NULL);
    if (!handle) {
        while (1)
            ;
    }

    /* Generate some randomness in the provided buffer */
    result = TRNG_generateEntropy(handle, entropyKey);
    if (result != TRNG_STATUS_SUCCESS) {
        while (1)
            ;
    }

    TRNG_close(handle);
}

/* Adds two to the total value of the array passed in. If a byte overflows
 * the next byte is incremented. The whole nonce wraps back to 0 silently. */
void COM_incrementNonce(uint8_t *nonce, uint8_t size) {
    uint8_t i = 0;

    bool wrapped = nonce[0] == 254 || nonce[0] == 255;
    nonce[0] += 2;

    for (i = 1; wrapped && nonce[i] == 255 && i < size - 1; ++i) {
        ++nonce[i];
        ++nonce[i + 1];
    }
}

/* Derives a 128-bit AES key based on a computed shared secret */
void COM_generateAesKey(CryptoKey *myPrivateKey, CryptoKey *theirPublicKey, CryptoKey *sharedSecret,
        CryptoKey *symmetricKey) {
    int_fast16_t operationResult;

    ECDH_Handle ecdhHandle;
    ECDH_OperationComputeSharedSecret operationComputeSharedSecret;

    SHA2_Handle handleHash;

    uint8_t entropyBuffer[SHA2_DIGEST_LENGTH_BYTES_256];

    /* Since we are using default ECDH_Params, we just pass in NULL for that parameter. */
    ecdhHandle = ECDH_open(CONFIG_ECDH_0, NULL);

    /* The ECC_NISTP256 struct is provided in ti/drivers/types/EccParams.h and the corresponding device-specific implementation. */
    ECDH_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
    operationComputeSharedSecret.curve = &ECCParams_NISTP256;
    operationComputeSharedSecret.myPrivateKey = myPrivateKey;
    operationComputeSharedSecret.theirPublicKey = theirPublicKey;
    operationComputeSharedSecret.sharedSecret = sharedSecret;

    /* Compute the shared secret and copy it to sharedSecretKeyingMaterial */
    operationResult = ECDH_computeSharedSecret(ecdhHandle, &operationComputeSharedSecret);
    if (operationResult != ECDH_STATUS_SUCCESS) {
        while (1)
            ;
    }
    ECDH_close(ecdhHandle);

    /* Hash the sharedSecret to a 256-bit buffer */
    handleHash = SHA2_open(CONFIG_SHA2_0, NULL);

    /* As the Y-coordinate is derived from the X-coordinate, hashing only the X component (i.e. keyLength/2 bytes)
     * is a relatively common way of deriving a symmetric key from a shared secret if you are not using a dedicated key derivation function. */
    operationResult = SHA2_hashData(handleHash, sharedSecret->u.plaintext.keyMaterial, sharedSecret->u.plaintext.keyLength/2, entropyBuffer);

    if (operationResult != SHA2_STATUS_SUCCESS) {
        while (1)
            ;
    }

    SHA2_close(handleHash);

    /* AES keys are 128-bit long, so truncate the generated hash */
    memcpy(symmetricKey->u.plaintext.keyMaterial, entropyBuffer, symmetricKey->u.plaintext.keyLength);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0) {
    /* Call driver init functions */
    Board_init();
    TRNG_init();
    UART_init();
    AESCCM_init();
    ECDSA_init();
    SHA2_init();
    ECDH_init();
    GPIO_init();

    /* Create a UART with data processing off. Ordinarily, the two devices
     * would have their own UART handles, but in this example they are
     * required to share one handle to avoid contention. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

    /* Enable the UART Loopback bit inside the UART0 Control Register
     Allows us to operate both 'ends' of this application inside one device
     See the CC26X2 TRM for details */
    *((volatile uint32_t*) (UART0_BASE + 0x30)) |= 0x80;

    /* Initialize the TX and RX main tasks */
    Task_Params workTaskParams;
    Task_Params_init(&workTaskParams);

    workTaskParams.stackSize = STACKSIZE;
    workTaskParams.priority = 2;
    workTaskParams.stack = &txTaskStack;
    Task_construct(&txTask, TX_mainTaskFunc, &workTaskParams, NULL);

    workTaskParams.priority = 3;
    workTaskParams.stack = &rxTaskStack;
    Task_construct(&rxTask, RX_mainTaskFunc, &workTaskParams, NULL);

    return NULL;
}
