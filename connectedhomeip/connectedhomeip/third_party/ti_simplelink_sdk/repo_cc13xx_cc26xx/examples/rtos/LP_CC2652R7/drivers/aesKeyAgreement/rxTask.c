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
 *  ======== rxTask.c ========
 */
#include <string.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/ECDH.h>
#include <ti/drivers/ECDSA.h>
#include <ti/drivers/AESCCM.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

/* XDC Module Headers */
#include <xdc/std.h>

/* Application specific headers */
#include "ti_drivers_config.h"
#include "common.h"

/* Buffer for UART receive operations */
static uint8_t rxPacket[PUBLIC_KEY_PACKET_LENGTH + ENCRYPTED_PACKET_LENGTH];

/* Hard coded private key for this unit.
 * One key must be hard coded in a system with one-way communication. */
static uint8_t rxPrivateKeyMaterial[PRIVATE_KEY_LENGTH] = {
    0x20,0x00,0x0c,0xd8,0x7c,0x58,0xd6,0x94,
    0x6e,0xba,0x42,0xaa,0x87,0x26,0xe2,0xb9,
    0x8e,0xb0,0x2c,0xcf,0xf5,0x80,0x55,0xf2,
    0x10,0x51,0xb8,0x38,0x63,0x9a,0x8f,0xa2,
};

/* Storage for the X and Y components of the transmit unit's public key */
static uint8_t txPublicKeyMaterial[PUBLIC_KEY_LENGTH];

/* Buffer for the X and Y components of the shared secret */
static uint8_t sharedSecretKeyMaterial[PUBLIC_KEY_LENGTH];

/* Storage for the generated symmetric encryption key */
static uint8_t symmetricKeyMaterial[AES_KEY_LENGTH];

static CryptoKey txPublicKey;
static CryptoKey rxPrivateKey;
static CryptoKey sharedSecretKey;
static CryptoKey symmetricKey;

/* Extracts public keys from the incoming rxPacket and places them into publicKey. Also verifies the public key signature. */
static int_fast16_t handlePublicKeyPacket(uint8_t *rxPacket, CryptoKey *txPublicKey) {
    SHA2_Handle handleHash;

    ECDSA_Handle handleVerify;
    ECDSA_OperationVerify operationVerify;

    int_fast16_t result;
    uint8_t hashedDataBuffer[PRIVATE_KEY_LENGTH];

    /* Copy the public keys from the packet into the parameters */
    memcpy(txPublicKeyMaterial, &rxPacket[PACKET_DATA_OFFSET], PUBLIC_KEY_LENGTH);
    CryptoKeyPlaintext_initKey(txPublicKey, txPublicKeyMaterial, PUBLIC_KEY_LENGTH);

    /* Hash the header and public key component of the message. Pass NULL to use the default parameters */
    handleHash = SHA2_open(CONFIG_SHA2_0, NULL);
    result = SHA2_hashData(handleHash, rxPacket, PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH, hashedDataBuffer);

    if (result != SHA2_STATUS_SUCCESS) {
        while (1);
    }

    SHA2_close(handleHash);

    /* Verify signature of public key */
    ECDSA_OperationVerify_init(&operationVerify);
    operationVerify.curve = &ECCParams_NISTP256;
    operationVerify.theirPublicKey = txPublicKey;
    operationVerify.hash = hashedDataBuffer;
    operationVerify.r = &rxPacket[PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH];
    operationVerify.s = &rxPacket[PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH + PRIVATE_KEY_LENGTH];

    /* Pass NULL to use the default parameters */
    handleVerify = ECDSA_open(CONFIG_ECDSA_0, NULL);
    result = ECDSA_verify(handleVerify, &operationVerify);
    if (result != ECDSA_STATUS_SUCCESS) {
        while (1);
    }
    ECDSA_close(handleVerify);

    return result;
}

/* Decrypts received packets with the given AES key */
static int_fast16_t decryptPacket(uint8_t *rxPacket, CryptoKey *symmetricKey) {
    AESCCM_Handle handle;
    AESCCM_Operation operation;
    int_fast16_t decryptionResult;

    uint8_t msgBuffer[MESSAGE_LENGTH];

    AESCCM_init();
    handle = AESCCM_open(CONFIG_AESECB_0, NULL);
    if (handle == NULL) {
        while (1);
    }

    /* Initialize decryption transaction */
    AESCCM_Operation_init(&operation);
    operation.key = symmetricKey;

    operation.aad = rxPacket;
    operation.aadLength = HEADER_LENGTH;

    operation.nonce = &rxPacket[HEADER_LENGTH];
    operation.nonceLength = NONCE_LENGTH;

    operation.input = &rxPacket[HEADER_LENGTH + NONCE_LENGTH];
    operation.inputLength = MESSAGE_LENGTH;

    operation.output = msgBuffer;

    operation.mac = &rxPacket[HEADER_LENGTH + NONCE_LENGTH + MESSAGE_LENGTH];
    operation.macLength = MAC_LENGTH;

    /* Start the decryption transaction */
    decryptionResult = AESCCM_oneStepDecrypt(handle, &operation);
    if (decryptionResult != AESCCM_STATUS_SUCCESS) {
        while (1);
    }
    AESCCM_close(handle);

    return decryptionResult;
}

void RX_mainTaskFunc(UArg a0, UArg a1) {
    CryptoKeyPlaintext_initBlankKey(&sharedSecretKey, sharedSecretKeyMaterial, PUBLIC_KEY_LENGTH);
    CryptoKeyPlaintext_initBlankKey(&symmetricKey, symmetricKeyMaterial, AES_KEY_LENGTH);
    CryptoKeyPlaintext_initKey(&rxPrivateKey, rxPrivateKeyMaterial, PRIVATE_KEY_LENGTH);

    /* Block until we receive a full public key packet from the TX side */
    COM_uartReceive(rxPacket, PUBLIC_KEY_PACKET_LENGTH);

    /* Extract and verify the signature of the public key */
    handlePublicKeyPacket(rxPacket, &txPublicKey);
    /* Generate a shared secret used for symmetric encryption */
    COM_generateAesKey(&rxPrivateKey, &txPublicKey, &sharedSecretKey, &symmetricKey);

    /* Toggle the green LED to indicate successful key exchange */
    GPIO_toggle(CONFIG_GPIO_LED_1);

    while (1) {
        /* Block until we receive an encrypted packet from the TX side */
        COM_uartReceive(rxPacket, ENCRYPTED_PACKET_LENGTH);

        /* Perform decryption and signature verification on the received packet */
        decryptPacket(rxPacket, &symmetricKey);

        /* Toggle the red LED to indicate successful packet decryption */
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }
}
