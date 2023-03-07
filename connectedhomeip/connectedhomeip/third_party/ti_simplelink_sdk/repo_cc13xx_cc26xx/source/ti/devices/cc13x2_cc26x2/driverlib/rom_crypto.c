/*******************************************************************************
*  Filename:       rom_crypto.c
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    This is the implementation for the API to the ECC functions
*                  built into ROM on the CC13x2/CC26x2.
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
*******************************************************************************/

#include <stdint.h>
#include "rom_crypto.h"


////////////////////////////////////* ECC *////////////////////////////////////

//*****************************************************************************
// ECC_initialize
//*****************************************************************************
void
ECC_initialize(uint32_t *pWorkzone)
{
  // Initialize curve parameters
  //data_p  = ECC_NISTP256_prime;
  *((const uint32_t **)0x20000138) = ECC_NISTP256_prime;

  //data_r  = ECC_NISTP256_order;
  *((const uint32_t **)0x2000013c) = ECC_NISTP256_order;

  //data_a  = ECC_NISTP256_a;
  *((const uint32_t **)0x20000140) = ECC_NISTP256_a;

  //data_b  = ECC_NISTP256_b;
  *((const uint32_t **)0x20000144) = ECC_NISTP256_b;

  //data_Gx = ECC_NISTP256_generatorX;
  *((const uint32_t **)0x2000012c) = ECC_NISTP256_generatorX;

  //data_Gy = ECC_NISTP256_generatorY;
  *((const uint32_t **)0x20000130) = ECC_NISTP256_generatorY;

  // Initialize window size
  //win = (uint8_t) ECC_WINDOW_SIZE;
  *((uint8_t *)0x20000148) = (uint8_t) ECC_WINDOW_SIZE;

  // Initialize work zone
  //workzone = (uint32_t *) pWorkzone;
  *((uint32_t **)0x20000134) = (uint32_t *) pWorkzone;
}

//*****************************************************************************
// ECC_init
//*****************************************************************************
void
ECC_init(uint32_t *workzone,
         uint8_t   windowSize,
         const uint32_t *prime,
         const uint32_t *order,
         const uint32_t *a,
         const uint32_t *b,
         const uint32_t *generatorX,
         const uint32_t *generatorY)
{
  // Initialize curve parameters
  //data_p  = (uint32_t *)PARAM_P;
  *((const uint32_t **)0x20000138) = prime;

  //data_r  = (uint32_t *)PARAM_R;
  *((const uint32_t **)0x2000013c) = order;

  //data_a  = (uint32_t *)PARAM_A;
  *((const uint32_t **)0x20000140) = a;

  //data_b  = (uint32_t *)PARAM_B;
  *((const uint32_t **)0x20000144) = b;

  //data_Gx = (uint32_t *)PARAM_GX;
  *((const uint32_t **)0x2000012c) = generatorX;

  //data_Gy = (uint32_t *)PARAM_GY;
  *((const uint32_t **)0x20000130) = generatorY;

  // Initialize window size
  //win = (uint8_t) windowSize;
  *((uint8_t *)0x20000148) = windowSize;

  // Initialize work zone
  //workzone = (uint32_t *) pWorkzone;
  *((uint32_t **)0x20000134) = workzone;
}

typedef uint8_t(*ecc_keygen_t)(uint32_t *, uint32_t *,uint32_t *, uint32_t *);
ecc_keygen_t ecc_generatekey = (ecc_keygen_t)(0x1001f94d);

typedef uint8_t(*ecdsa_sign_t)(uint32_t *, uint32_t *,uint32_t *, uint32_t *, uint32_t *);
ecdsa_sign_t ecc_ecdsa_sign = (ecdsa_sign_t)(0x10010381);

typedef uint8_t(*ecdsa_verify_t)(uint32_t *, uint32_t *,uint32_t *, uint32_t *, uint32_t *);
ecdsa_verify_t ecc_ecdsa_verify = (ecdsa_verify_t)(0x1000c805);

typedef uint8_t(*ecdh_computeSharedSecret_t)(uint32_t *, uint32_t *,uint32_t *, uint32_t *, uint32_t *);
ecdh_computeSharedSecret_t ecdh_computeSharedSecret = (ecdh_computeSharedSecret_t)(0x10023485);

//*****************************************************************************
// ECC_generateKey
//*****************************************************************************
uint8_t
ECC_generateKey(uint32_t *randString, uint32_t *privateKey,
                uint32_t *publicKey_x, uint32_t *publicKey_y)
{
  return (uint8_t)ecc_generatekey((uint32_t*)randString, (uint32_t*)privateKey,
                                  (uint32_t*)publicKey_x, (uint32_t*)publicKey_y);

}

//*****************************************************************************
// ECC_ECDSA_sign
//*****************************************************************************
uint8_t
ECC_ECDSA_sign(uint32_t *secretKey, uint32_t *text, uint32_t *randString,
               uint32_t *sign1, uint32_t *sign2)
{
  return (uint8_t)ecc_ecdsa_sign((uint32_t*)secretKey, (uint32_t*)text, (uint32_t*)randString,
                             (uint32_t*)sign1, (uint32_t*)sign2);
}

//*****************************************************************************
// ECC_ECDSA_verify
//*****************************************************************************
uint8_t
ECC_ECDSA_verify(uint32_t *publicKey_x, uint32_t *publicKey_y,
                 uint32_t *text, uint32_t *sign1, uint32_t *sign2)
{
  return (uint8_t)ecc_ecdsa_verify((uint32_t*)publicKey_x, (uint32_t*)publicKey_y, (uint32_t*)text,
                              (uint32_t*)sign1, (uint32_t*)sign2);
}

//*****************************************************************************
// ECC_ECDH_computeSharedSecret
//*****************************************************************************
uint8_t
ECC_ECDH_computeSharedSecret(uint32_t *privateKey, uint32_t *publicKey_x,
                             uint32_t *publicKey_y, uint32_t *sharedSecret_x,
                             uint32_t *sharedSecret_y)
{
  return (uint8_t)ecdh_computeSharedSecret((uint32_t*)privateKey, (uint32_t*)publicKey_x,
                                 (uint32_t*)publicKey_y, (uint32_t*)sharedSecret_x,
                                 (uint32_t*)sharedSecret_y);
}
