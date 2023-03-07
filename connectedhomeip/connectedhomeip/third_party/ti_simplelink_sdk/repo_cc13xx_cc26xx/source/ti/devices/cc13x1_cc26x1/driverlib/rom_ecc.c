/******************************************************************************
*  Filename:       ECC_NISTP256.c
*  Revised:        2020-04-03 19:11:17 +0200 (Fri, 03 Apr 2020)
*  Revision:       57290
*
*  Description:    Constant definitions for ECC_NISTP256 curve
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include "rom_ecc.h"

#include <stdbool.h>
#include <string.h>


/******************************************************************************/
/***                           GLOBAL VARIABLES                             ***/
/******************************************************************************/

//*****************************************************************************
//
// NIST P256 constants in little endian format with prepended length word.
// byte[0] is the least significant byte and
// byte[ECC_NISTP256_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const ECC_NISTP256_Param ECC_NISTP256_generatorX    = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0x96, 0xc2, 0x98, 0xd8, 0x45, 0x39, 0xa1, 0xf4,
                                                0xa0, 0x33, 0xeb, 0x2d, 0x81, 0x7d, 0x03, 0x77,
                                                0xf2, 0x40, 0xa4, 0x63, 0xe5, 0xe6, 0xbc, 0xf8,
                                                0x47, 0x42, 0x2c, 0xe1, 0xf2, 0xd1, 0x17, 0x6b}};

const ECC_NISTP256_Param ECC_NISTP256_generatorY    = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0xf5, 0x51, 0xbf, 0x37, 0x68, 0x40, 0xb6, 0xcb,
                                                0xce, 0x5e, 0x31, 0x6b, 0x57, 0x33, 0xce, 0x2b,
                                                0x16, 0x9e, 0x0f, 0x7c, 0x4a, 0xeb, 0xe7, 0x8e,
                                                0x9b, 0x7f, 0x1a, 0xfe, 0xe2, 0x42, 0xe3, 0x4f}};

const ECC_NISTP256_Param ECC_NISTP256_prime         = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

const ECC_NISTP256_Param ECC_NISTP256_a             = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

const ECC_NISTP256_Param ECC_NISTP256_b             = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0x4b, 0x60, 0xd2, 0x27, 0x3e, 0x3c, 0xce, 0x3b,
                                                0xf6, 0xb0, 0x53, 0xcc, 0xb0, 0x06, 0x1d, 0x65,
                                                0xbc, 0x86, 0x98, 0x76, 0x55, 0xbd, 0xeb, 0xb3,
                                                0xe7, 0x93, 0x3a, 0xaa, 0xd8, 0x35, 0xc6, 0x5a}};

const ECC_NISTP256_Param ECC_NISTP256_order         = {.byte = {0x08, 0x00, 0x00, 0x00,
                                                0x51, 0x25, 0x63, 0xfc, 0xc2, 0xca, 0xb9, 0xf3,
                                                0x84, 0x9e, 0x17, 0xa7, 0xad, 0xfa, 0xe6, 0xbc,
                                                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

const ECC_NISTP256_Param ECC_NISTP256_k_mont        = {.byte = {
                                                0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff,
                                                0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                0xfd, 0xff, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00 }};

const ECC_NISTP256_Param ECC_NISTP256_a_mont        = {.byte  = {
                                                0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x04, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff }};

const ECC_NISTP256_Param ECC_NISTP256_b_mont        = {.byte  = {
                                                0xdf, 0xbd, 0xc4, 0x29, 0x62, 0xdf, 0x9c, 0xd8,
                                                0x90, 0x30, 0x84, 0x78, 0xcd, 0x05, 0xf0, 0xac,
                                                0xd6, 0x2e, 0x21, 0xf7, 0xab, 0x20, 0xa2, 0xe5,
                                                0x34, 0x48, 0x87, 0x04, 0x1d, 0x06, 0x30, 0xdc }};



//*****************************************************************************
// ECC_initialize
//*****************************************************************************
void ECC_initialize(ECC_State *state, uint32_t *workzone)
{
    state->data_Gx = ECC_NISTP256_generatorX.word;
    state->data_Gy = ECC_NISTP256_generatorY.word;
    state->data_p = ECC_NISTP256_prime.word;
    state->data_r = ECC_NISTP256_order.word;
    state->data_a = ECC_NISTP256_a.word;
    state->data_b = ECC_NISTP256_b.word;
    state->data_a_mont = ECC_NISTP256_a_mont.word;
    state->data_b_mont = ECC_NISTP256_b_mont.word;
    state->data_k_mont = ECC_NISTP256_k_mont.word;
    state->win = 3;
    state->workzone = workzone;
}

//*****************************************************************************
// ECC_init
//*****************************************************************************
void ECC_init(ECC_State *state,
              uint32_t *workzone,
              uint8_t windowSize,
              const uint32_t *prime,
              const uint32_t *order,
              const uint32_t *a,
              const uint32_t *b,
              const uint32_t *generatorX,
              const uint32_t *generatorY)
{
    state->data_Gx = generatorX;
    state->data_Gy = generatorY;
    state->data_p = prime;
    state->data_r = order;
    state->data_a = a;
    state->data_b = b;
    // TODO: change signature to accommodate in future
    state->data_a_mont = ECC_NISTP256_a_mont.word;
    state->data_b_mont = ECC_NISTP256_b_mont.word;
    state->data_k_mont = ECC_NISTP256_k_mont.word;
    state->win = windowSize;
    state->workzone = workzone;
}


//*****************************************************************************
// ECC_generateKey
//*****************************************************************************
uint8_t ECC_generateKey(ECC_State *state,
                        uint32_t *randEntropy,
                        uint32_t *privateKey,
                        uint32_t *publicKey_x,
                        uint32_t *publicKey_y)
{
    return HapiECCKeyGen(state,
                         randEntropy,
                         privateKey,
                         publicKey_x,
                         publicKey_y);

}

//*****************************************************************************
// ECC_ECDSA_sign
//*****************************************************************************
uint8_t ECC_ECDSA_sign(ECC_State *state,
                       uint32_t *privateKey,
                       uint32_t *hash,
                       uint32_t *pmsn,
                       uint32_t *r,
                       uint32_t *s)
{
    return HapiECDSASign(state, privateKey, hash, pmsn, r, s);
}

//*****************************************************************************
// ECC_ECDSA_verify
//*****************************************************************************
uint8_t ECC_ECDSA_verify(ECC_State *state,
                         uint32_t *publicKey_x,
                         uint32_t *publicKey_y,
                         uint32_t *hash,
                         uint32_t *r,
                         uint32_t *s)
{
    return HapiECDSAVerify(state, publicKey_x, publicKey_y, hash, r, s);
}

//*****************************************************************************
// ECC_ECDH_computeSharedSecret
//*****************************************************************************
uint8_t ECC_ECDH_computeSharedSecret(ECC_State *state,
                                     uint32_t *privateKey,
                                     uint32_t *publicKey_x,
                                     uint32_t *publicKey_y,
                                     uint32_t *sharedSecret_x,
                                     uint32_t *sharedSecret_y)
{
    return HapiECDHCommonKey(state,
                             privateKey,
                             publicKey_x,
                             publicKey_y,
                             sharedSecret_x,
                             sharedSecret_y);
}

//*****************************************************************************
// ECC_validatePublicKey
//*****************************************************************************
uint8_t ECC_validatePublicKey(ECC_State *state,
                              uint32_t *publicKey_x,
                              uint32_t *publicKey_y)
{
    return HapiECCValidatePublicKeyWeierstrass(state,
                                               publicKey_x,
                                               publicKey_y);
}

//*****************************************************************************
// ECC_validatePrivateKey
//*****************************************************************************
uint8_t ECC_validatePrivateKey(ECC_State *state, uint32_t *privateKey)
{
    return HapiECCValidatePrivateKeyWeierstrass(state, privateKey);
}
