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
 *  ======== txTask.c ========
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
#include <xdc/runtime/System.h>

/* BIOS Module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Application specific headers */
#include "ti_drivers_config.h"
#include "common.h"

static uint8_t nonce[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t msgBuffer[MESSAGE_LENGTH] = { 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'p', 'l', 'a', 'i', 'n', 't', 'e', 'x', 't' };

/* Semaphores to pend on button presses */
static Semaphore_Handle buttonKeyGenPressed;
static Semaphore_Handle buttonEncryptPacketPressed;

static uint8_t txPacket[PUBLIC_KEY_PACKET_LENGTH + ENCRYPTED_PACKET_LENGTH];

/* Public key pair of the receiving unit device */
static uint8_t rxPublicKeyMaterial[PUBLIC_KEY_LENGTH] = {
    /* Octet string formatting byte */
    0x04,
    /* X */
    0x06,0xf4,0x26,0xdc,0x4f,0x93,0x7d,0x4c,
    0xb7,0x7c,0x11,0xc5,0xd8,0x3b,0x98,0xf7,
    0x32,0xbe,0xec,0x18,0x6d,0x3c,0xd3,0x9d,
    0x8c,0x01,0x31,0x04,0x6c,0x44,0x85,0xbf,

    /* Y */
    0xc2,0x1f,0xed,0x3a,0xa8,0x8b,0xe6,0x29,
    0x90,0xb7,0x6c,0xc0,0xac,0xec,0x22,0x1a,
    0x79,0xdb,0x28,0xb6,0x3c,0x05,0x26,0xd2,
    0xe1,0xca,0xa4,0x99,0x31,0x08,0x1b,0xb8,
};

static uint8_t txPrivateKeyMaterial[PRIVATE_KEY_LENGTH];
static uint8_t txPublicKeyMaterial[PUBLIC_KEY_LENGTH];
static uint8_t sharedSecretKeyMaterial[PUBLIC_KEY_LENGTH];
static uint8_t symmetricKeyMaterial[AES_KEY_LENGTH];

static CryptoKey txPublicKey;
static CryptoKey txPrivateKey;
static CryptoKey rxPublicKey;
static CryptoKey sharedSecretKey;
static CryptoKey symmetricKey;

/* Interrupt callback for button presses; posts the relevant semaphore */
static void buttonCallbackFxn(uint_least8_t index) {
    if (!GPIO_read(index)) {
        if (index == CONFIG_GPIO_BUTTON_0) {
            Semaphore_post(buttonKeyGenPressed);
        } else {
            Semaphore_post(buttonEncryptPacketPressed);
        }
    }
}

/* Generates private-public key pair and fill both inputs with the results */
static int_fast16_t generateKeys(CryptoKey *privateKey, CryptoKey *publicKey) {
    int_fast16_t result;
    ECDH_Handle handle;
    ECDH_Params params;
    ECDH_OperationGeneratePublicKey operation;

    /* Use TRNG to generate a new random private key */
    COM_generateRandomBytes(privateKey);

    ECDH_init();
    ECDH_Params_init(&params);
    params.returnBehavior = ECDH_RETURN_BEHAVIOR_BLOCKING;
    handle = ECDH_open(CONFIG_ECDH_0, &params);

    ECDH_OperationGeneratePublicKey_init(&operation);
    operation.curve = &ECCParams_NISTP256;
    operation.myPrivateKey = privateKey;
    operation.myPublicKey = publicKey;

    /* Generate public key pair for this unit by using elliptic curve cryptography */
    result = ECDH_generatePublicKey(handle, &operation);

    ECDH_close(handle);
    return result;
}

/* Signs public key with private key and assembles a public key packet in txPacket */
static uint8_t createPublicKeyPacket(uint8_t *txPacket, CryptoKey *privateKey, CryptoKey *publicKey, uint8_t keyCoordinateType) {
    int_fast16_t result;

    SHA2_Handle handleHash;

    ECDSA_Handle handleSign;
    ECDSA_OperationSign operationSign;

    /* Buffers for the SHA-2 output of the hashed data to be signed */
    uint8_t hashedDataBuffer[PRIVATE_KEY_LENGTH];

    /* Load public key and packet metadata to the txPacket */
    txPacket[PACKET_IDENT_BYTE] = keyCoordinateType;
    txPacket[PACKET_LENGTH_BYTE] = PUBLIC_KEY_PACKET_LENGTH;
    memcpy(&txPacket[PACKET_DATA_OFFSET], publicKey->u.plaintext.keyMaterial, publicKey->u.plaintext.keyLength);

    /* Perform SHA-2 computation on the data to be signed */
    handleHash = SHA2_open(CONFIG_SHA2_0, NULL);
    result = SHA2_hashData(handleHash, txPacket, PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH, hashedDataBuffer);

    if (result != SHA2_STATUS_SUCCESS) {
        while (1)
            ;
    }

    SHA2_close(handleHash);

    /* Sign some key data to verify to the receiver that this unit has the corresponding private key to the transmit public key pair */
    ECDSA_OperationSign_init(&operationSign);
    operationSign.curve = &ECCParams_NISTP256;
    operationSign.myPrivateKey = privateKey;
    operationSign.hash = hashedDataBuffer;
    operationSign.r = &txPacket[PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH];
    operationSign.s = &txPacket[PACKET_DATA_OFFSET + PUBLIC_KEY_LENGTH + PRIVATE_KEY_LENGTH];

    handleSign = ECDSA_open(CONFIG_ECDSA_0, NULL);
    result = ECDSA_sign(handleSign, &operationSign);
    if (result != ECDSA_STATUS_SUCCESS) {
        while (1)
            ;
    }
    ECDSA_close(handleSign);

    return result;
}

void createEncryptedPacket(uint8_t *txPacket, CryptoKey *symmetricKey) {
    AESCCM_Handle handle;
    AESCCM_Operation operation;
    int_fast16_t encryptionResult;

    /* Header contains packet type and length. In this case length is fixed by packet type
     * but in a real application it may vary message to message. */
    txPacket[PACKET_IDENT_BYTE] = ENCRYPTED_PACKET;
    txPacket[PACKET_LENGTH_BYTE] = ENCRYPTED_PACKET_LENGTH;

    /* Increment a unique nonce each time a message is sent.
     * In a two-way application, the RX would use even and the TX odd numbers (or vice versa). */
    COM_incrementNonce(nonce, NONCE_LENGTH);
    memcpy(&txPacket[HEADER_LENGTH], nonce, NONCE_LENGTH);

    handle = AESCCM_open(CONFIG_AESCCM_0, NULL);
    if (handle == NULL) {
        while (1)
            ;
    }

    /* Initialize encryption transaction */
    AESCCM_Operation_init(&operation);
    operation.key = symmetricKey;

    operation.aad = txPacket;
    operation.aadLength = HEADER_LENGTH;

    operation.nonce = &txPacket[HEADER_LENGTH];
    operation.nonceLength = NONCE_LENGTH;

    operation.input = (uint8_t*) msgBuffer;
    operation.inputLength = MESSAGE_LENGTH;

    operation.output = &txPacket[HEADER_LENGTH + NONCE_LENGTH];

    operation.mac = &txPacket[HEADER_LENGTH + NONCE_LENGTH + MESSAGE_LENGTH];
    operation.macLength = MAC_LENGTH;

    /* Execute the encryption transaction */
    encryptionResult = AESCCM_oneStepEncrypt(handle, &operation);
    if (encryptionResult != AESCCM_STATUS_SUCCESS) {
        while (1)
            ;
    }
    AESCCM_close(handle);
}

void TX_mainTaskFunc(UArg a0, UArg a1) {
    int_fast16_t status;

    /* Initialize semaphores */
    Semaphore_Params semParam;
    Semaphore_Params_init(&semParam);
    buttonKeyGenPressed = Semaphore_create(0, &semParam, NULL);
    buttonEncryptPacketPressed = Semaphore_create(0, &semParam, NULL);

    /* Setup callback for button pins */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, buttonCallbackFxn);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, buttonCallbackFxn);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    /* Initialize all the CryptoKeys */
    CryptoKeyPlaintext_initBlankKey(&txPublicKey, txPublicKeyMaterial, PUBLIC_KEY_LENGTH);
    CryptoKeyPlaintext_initBlankKey(&txPrivateKey, txPrivateKeyMaterial, PRIVATE_KEY_LENGTH);
    CryptoKeyPlaintext_initBlankKey(&sharedSecretKey, sharedSecretKeyMaterial, PUBLIC_KEY_LENGTH);
    CryptoKeyPlaintext_initBlankKey(&symmetricKey, symmetricKeyMaterial, AES_KEY_LENGTH);
    CryptoKeyPlaintext_initKey(&rxPublicKey, rxPublicKeyMaterial, PUBLIC_KEY_LENGTH);

    /* Wait for button0 pressed before proceeding */
    Semaphore_pend(buttonKeyGenPressed, BIOS_WAIT_FOREVER);

    /* Generate public key pair and load it on packet */
    status = generateKeys(&txPrivateKey, &txPublicKey);
    if (status != ECDH_STATUS_SUCCESS) {
        while (1)
            ;
    }

    /* Sign message with private key so the receiver can verify the public keys received*/
    status = createPublicKeyPacket(txPacket, &txPrivateKey, &txPublicKey, PUBLIC_KEY_PACKET);
    if (status != ECDSA_STATUS_SUCCESS) {
        while (1)
            ;
    }

    /* Generate AES key for message encryption */
    COM_generateAesKey(&txPrivateKey, &rxPublicKey, &sharedSecretKey, &symmetricKey);

    /* Send the resulting packet (public key + two signatures) to the RX task */
    COM_uartTransmit(txPacket, PUBLIC_KEY_PACKET_LENGTH);

    while (1) {
        /* Wait for the user to press button1 before proceeding */
        Semaphore_pend(buttonEncryptPacketPressed, BIOS_WAIT_FOREVER);

        /* Encrypt the phrase 'this is plaintext' and transmit it to the RX task */
        createEncryptedPacket(txPacket, &symmetricKey);
        COM_uartTransmit(txPacket, ENCRYPTED_PACKET_LENGTH);
    }
}
