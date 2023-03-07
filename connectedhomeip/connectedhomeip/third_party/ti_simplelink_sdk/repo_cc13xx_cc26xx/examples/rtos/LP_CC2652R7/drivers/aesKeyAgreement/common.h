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

#ifndef COMMON_H_
#define COMMON_H_

#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <xdc/std.h>

#define AES_KEY_LENGTH              16
#define PRIVATE_KEY_LENGTH          32
#define PUBLIC_KEY_LENGTH           65

#define ENCRYPTED_PACKET            0
#define PUBLIC_KEY_PACKET           1

#define PACKET_IDENT_BYTE           0
#define PACKET_LENGTH_BYTE          1
#define PACKET_DATA_OFFSET          2

#define HEADER_LENGTH               2
#define NONCE_LENGTH                12
#define MESSAGE_LENGTH              17
#define MAC_LENGTH                  4

#define ENCRYPTED_PACKET_LENGTH     (HEADER_LENGTH + NONCE_LENGTH + MESSAGE_LENGTH + MAC_LENGTH)
#define PUBLIC_KEY_PACKET_LENGTH    (HEADER_LENGTH + PUBLIC_KEY_LENGTH + PRIVATE_KEY_LENGTH + PRIVATE_KEY_LENGTH)

/* Transmits 'count' bytes from buff to the RX task and returns. */
int32_t COM_uartTransmit (void *buff, size_t count);

/* Blocks until 'count' bytes have been received, then returns. Outputs to buff. */
int32_t COM_uartReceive (void *buff, size_t count);

/* Generate random number of the given size (in bytes) using the TRNG and stores the generated number on the buffer */
void COM_generateRandomBytes(CryptoKey *entropyKey);

/* Increments a 128-bit nonce that is not random, only unique.
 * A nonce is permitted to be predictable as long as it is not reused. */
void COM_incrementNonce(uint8_t *nonce, uint8_t size);

/* Computes a shared secret which is then used to derive an AES Key. Output is stored in sharedSecret and symmetricKey */
void COM_generateAesKey(CryptoKey *myPrivateKey, CryptoKey *theirPublicKey, CryptoKey *sharedSecret, CryptoKey *symmetricKey);

void RX_mainTaskFunc(UArg a0, UArg a1);
void TX_mainTaskFunc(UArg a0, UArg a1);

#endif /* COMMON_H_ */
