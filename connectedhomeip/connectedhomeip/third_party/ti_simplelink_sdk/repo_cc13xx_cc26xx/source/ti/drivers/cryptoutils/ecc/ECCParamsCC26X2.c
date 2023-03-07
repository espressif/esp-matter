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
 *  ======== ECCParamsCC26X2.c ========
 *
 *  This file contains structure definitions for various ECC curves for use
 *  on CC26X2 devices.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/pka.h)

const ECCParams_CurveParams ECCParams_NISTP224 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = NISTP224_PARAM_SIZE_BYTES,
    .prime          = NISTP224_prime.byte,
    .order          = NISTP224_order.byte,
    .a              = NISTP224_a.byte,
    .b              = NISTP224_b.byte,
    .generatorX     = NISTP224_generator.x.byte,
    .generatorY     = NISTP224_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_NISTP256 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = NISTP256_PARAM_SIZE_BYTES,
    .prime          = NISTP256_prime.byte,
    .order          = NISTP256_order.byte,
    .a              = NISTP256_a.byte,
    .b              = NISTP256_b.byte,
    .generatorX     = NISTP256_generator.x.byte,
    .generatorY     = NISTP256_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_NISTP384 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = NISTP384_PARAM_SIZE_BYTES,
    .prime          = NISTP384_prime.byte,
    .order          = NISTP384_order.byte,
    .a              = NISTP384_a.byte,
    .b              = NISTP384_b.byte,
    .generatorX     = NISTP384_generator.x.byte,
    .generatorY     = NISTP384_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_NISTP521 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = NISTP521_PARAM_SIZE_BYTES,
    .prime          = NISTP521_prime.byte,
    .order          = NISTP521_order.byte,
    .a              = NISTP521_a.byte,
    .b              = NISTP521_b.byte,
    .generatorX     = NISTP521_generator.x.byte,
    .generatorY     = NISTP521_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_BrainpoolP256R1 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = BrainpoolP256R1_PARAM_SIZE_BYTES,
    .prime          = BrainpoolP256R1_prime.byte,
    .order          = BrainpoolP256R1_order.byte,
    .a              = BrainpoolP256R1_a.byte,
    .b              = BrainpoolP256R1_b.byte,
    .generatorX     = BrainpoolP256R1_generator.x.byte,
    .generatorY     = BrainpoolP256R1_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_BrainpoolP384R1 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = BrainpoolP384R1_PARAM_SIZE_BYTES,
    .prime          = BrainpoolP384R1_prime.byte,
    .order          = BrainpoolP384R1_order.byte,
    .a              = BrainpoolP384R1_a.byte,
    .b              = BrainpoolP384R1_b.byte,
    .generatorX     = BrainpoolP384R1_generator.x.byte,
    .generatorY     = BrainpoolP384R1_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_BrainpoolP512R1 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = BrainpoolP512R1_PARAM_SIZE_BYTES,
    .prime          = BrainpoolP512R1_prime.byte,
    .order          = BrainpoolP512R1_order.byte,
    .a              = BrainpoolP512R1_a.byte,
    .b              = BrainpoolP512R1_b.byte,
    .generatorX     = BrainpoolP512R1_generator.x.byte,
    .generatorY     = BrainpoolP512R1_generator.y.byte,
    .cofactor       = 1
};

const ECCParams_CurveParams ECCParams_Curve25519 = {
    .curveType      = ECCParams_CURVE_TYPE_MONTGOMERY,
    .length         = 32,
    .prime          = Curve25519_prime.byte,
    .order          = Curve25519_order.byte,
    .a              = Curve25519_a.byte,
    .b              = Curve25519_b.byte,
    .generatorX     = Curve25519_generator.x.byte,
    .generatorY     = Curve25519_generator.y.byte,
    .cofactor       = 1
};

/*
 * Ed25519 constants in little endian format. byte[0] is the least
 * significant byte and byte[Ed25519_PARAM_SIZE_BYTES - 1] is the most
 * significant.
 */
const PKA_EccPoint256 Ed25519_generator = {
    .x = {.byte = {0x1a, 0xd5, 0x25, 0x8f, 0x60, 0x2d, 0x56, 0xc9,
                   0xb2, 0xa7, 0x25, 0x95, 0x60, 0xc7, 0x2c, 0x69,
                   0x5c, 0xdc, 0xd6, 0xfd, 0x31, 0xe2, 0xa4, 0xc0,
                   0xfe, 0x53, 0x6e, 0xcd, 0xd3, 0x36, 0x69, 0x21}},
    .y = {.byte = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                   0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                   0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                   0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66}},
};

const PKA_EccParam256 Ed25519_prime =
    {.byte = {0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f}};

const PKA_EccParam256 Ed25519_order =
    {.byte = {0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
              0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}};

const PKA_EccParam256 Ed25519_a     =
    {.byte = {0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f}};

const PKA_EccParam256 Ed25519_d     =
    {.byte = {0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75,
              0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
              0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c,
              0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52}};

const ECCParams_CurveParams ECCParams_Ed25519 = {
    .curveType      = ECCParams_CURVE_TYPE_EDWARDS,
    .length         = 32,
    .prime          = Ed25519_prime.byte,
    .order          = Ed25519_order.byte,
    .a              = Ed25519_a.byte,
    .b              = Ed25519_d.byte,
    .generatorX     = Ed25519_generator.x.byte,
    .generatorY     = Ed25519_generator.y.byte,
    .cofactor       = 8
};

/*
 * Wei25519 constants in little endian format. byte[0] is the least
 * significant byte and byte[Wei25519_PARAM_SIZE_BYTES - 1] is the most
 * significant.
 */
const PKA_EccPoint256 Wei25519_generator = {
    .x = {.byte = {0x5a, 0x24, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a}},
    .y = {.byte = {0xd9, 0xd3, 0xce, 0x7e, 0xa2, 0xc5, 0xe9, 0x29,
                   0xb2, 0x61, 0x7c, 0x6d, 0x7e, 0x4d, 0x3d, 0x92,
                   0x4c, 0xd1, 0x48, 0x77, 0x2c, 0xdd, 0x1e, 0xe0,
                   0xb4, 0x86, 0xa0, 0xb8, 0xa1, 0x19, 0xae, 0x20}},
};

const PKA_EccParam256 Wei25519_prime =
    {.byte = {0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f}};

const PKA_EccParam256 Wei25519_order =
    {.byte = {0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
              0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}};

const PKA_EccParam256 Wei25519_a     =
    {.byte = {0x44, 0xa1, 0x14, 0x49, 0x98, 0xaa, 0xaa, 0xaa,
              0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
              0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
              0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a}};

const PKA_EccParam256 Wei25519_b     =
    {.byte = {0x64, 0xc8, 0x10, 0x77, 0x9c, 0x5e, 0x0b, 0x26,
              0xb4, 0x97, 0xd0, 0x5e, 0x42, 0x7b, 0x09, 0xed,
              0x25, 0xb4, 0x97, 0xd0, 0x5e, 0x42, 0x7b, 0x09,
              0xed, 0x25, 0xb4, 0x97, 0xd0, 0x5e, 0x42, 0x7b}};

const ECCParams_CurveParams ECCParams_Wei25519 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_GEN,
    .length         = 32,
    .prime          = Wei25519_prime.byte,
    .order          = Wei25519_order.byte,
    .a              = Wei25519_a.byte,
    .b              = Wei25519_b.byte,
    .generatorX     = Wei25519_generator.x.byte,
    .generatorY     = Wei25519_generator.y.byte,
    .cofactor       = 8
};

/*
 *  ======== ECCParams_formatCurve25519PrivateKey ========
 */
int_fast16_t ECCParams_formatCurve25519PrivateKey(CryptoKey *myPrivateKey){
    myPrivateKey->u.plaintext.keyMaterial[31] &= 0xF8;
    myPrivateKey->u.plaintext.keyMaterial[0] &= 0x7F;
    myPrivateKey->u.plaintext.keyMaterial[0] |= 0x40;

    return ECCParams_STATUS_SUCCESS;
}

/*
 *  ======== ECCParams_getUncompressedGeneratorPoint ========
 */
int_fast16_t ECCParams_getUncompressedGeneratorPoint(const ECCParams_CurveParams *curveParams,
                                                     uint8_t *buffer,
                                                     size_t length) {

    size_t paramLength = curveParams->length;
    size_t pointLength = (paramLength * 2) + 1;

    if (length < pointLength) {
        return ECCParams_STATUS_ERROR;
    }

    /* Reverse and concatenate x and y */
    uint32_t i = 0;
    for (i = 0; i < paramLength; i++) {
        buffer[i + 1]               = curveParams->generatorX[paramLength - i - 1];
        buffer[i + 1 + paramLength] = curveParams->generatorY[paramLength - i - 1];
    }

    buffer[0] = 0x04;
    /* Fill the remaining buffer with 0 if needed */
    memset(buffer + pointLength, 0, length - pointLength);

    return ECCParams_STATUS_SUCCESS;
}
