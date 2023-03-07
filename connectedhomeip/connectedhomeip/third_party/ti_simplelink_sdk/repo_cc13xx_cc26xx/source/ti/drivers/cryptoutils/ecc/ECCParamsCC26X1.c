/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  on CC26XX devices.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rom_ecc.h)

/* The parameters are formatted as little-endian integers with a prepended
 * length word in words. That makes each param 36 bytes long with
 * the actual value starting after the first word.
 */
const ECCParams_CurveParams ECCParams_NISTP256 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = ECC_NISTP256_PARAM_LENGTH_BYTES,
    .prime          = ECC_NISTP256_prime.byte,
    .order          = ECC_NISTP256_order.byte,
    .a              = ECC_NISTP256_a.byte,
    .b              = ECC_NISTP256_b.byte,
    .generatorX     = ECC_NISTP256_generatorX.byte,
    .generatorY     = ECC_NISTP256_generatorY.byte,
    .cofactor       = 1
};

// TODO: fill out when adding Curve25519 support
const ECCParams_CurveParams ECCParams_Curve25519 = {
    .curveType      = ECCParams_CURVE_TYPE_MONTGOMERY,
    .length         = 32,
    .prime          = NULL,
    .order          = NULL,
    .a              = NULL,
    .b              = NULL,
    .generatorX     = NULL,
    .generatorY     = NULL,
    .cofactor       = 1
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
 *
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
        buffer[i + 1]               = curveParams->generatorX[paramLength
                                                              + ECC_LENGTH_OFFSET_BYTES
                                                              - i
                                                              - 1];
        buffer[i + 1 + paramLength] = curveParams->generatorY[paramLength
                                                              + ECC_LENGTH_OFFSET_BYTES
                                                              - i
                                                              - 1];
    }

    buffer[0] = 0x04;
    /* Fill the remaining buffer with 0 if needed */
    memset(buffer + pointLength, 0, length - pointLength);

    return ECCParams_STATUS_SUCCESS;
}
