/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PKA_ECC_EXPORT_H
#define PKA_ECC_EXPORT_H


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_types.h"
#include "cc_rnd_common.h"
#include "cc_ecpki_types.h"


/* Temporary buffers used for the functions called from ECDSA */
typedef struct{
//! Debug : set to 10, may be less?
    CCEcdsaSignIntBuff_t tempBuff;
}EcWrstDsaSignDb_t;

/* Temporary buffers used for the  functions called from ECDSA */
typedef struct{
    CCEcdsaVerifyIntBuff_t tempBuff;
}EcWrstDsaVerifyDb_t;


/* internal ECPKI buffer structure used on LLF and containing Barrett tags for*
*  modulus and gen.order                                                      */
typedef  struct {
       uint32_t modTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
       uint32_t ordTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
}EcWrstDomain_t;

/* affine ec-point in uint32 arrays format) */
typedef struct{
  uint32_t x[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t y[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
} EcWrstAffPoint_t;

/* modified jacobian ec-point: X:x/z^2, Y:y/z^3, t:a*z^4 (uint32 arrays) */
typedef struct{
  uint32_t  x[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t  y[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t  z[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t  t[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
} EcwrstMdfPoint_t;

/* EC curve (domain) structure as uint32 array */
typedef struct{
  uint32_t p[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];          // modulo
  uint32_t a[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t b[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];          // y^2 = x^3 + a.x + b (mod p)
  uint32_t Gx[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
  uint32_t Gy[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];         // generator
  uint32_t n[CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS];          // ord(G) = n
  uint32_t h[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];          // cofactor: #E = n.h
  uint32_t np[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];  // Barrett tag for modulus
  uint32_t nn[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];  // Barrett tag for order
} EcwrstCurve_t;

void EcWrstDsaTruncateMsg(uint32_t *pMsgOut,
            uint8_t *pMsgIn,
            uint32_t outSizeBits);



CCError_t  EcWrstInitPubKey(CCEcpkiPublKey_t *pPublKey,
                  uint8_t pointCtl);


CCError_t  EcWrstFullCheckPublKey(CCEcpkiPublKey_t    *pPublKey,
                    uint32_t            *pTempBuff);

/* EC WRST DSA */
CCError_t EcWrstDsaSign(CCRndContext_t   *pRndContext,
                        CCEcpkiPrivKey_t *pSignPrivKey,
                        uint32_t             *pMsgRepres,
                        uint32_t              isEphemerKeyInternal,
                        uint32_t             *pEphemKey,
                        uint32_t             *pSignC,
                        uint32_t             *pSignD,
                        uint32_t             *pTempBuff);

CCError_t EcWrstDsaVerify(CCEcpkiPublKey_t *signPublKey,
                    uint32_t  *messageRepresent,
                    uint32_t   messRepresSizeWords,
                    uint32_t  *signC,
                    uint32_t  *signD);



/* EC WRST DH */
CCError_t  EcWrstDhDeriveSharedSecret(CCEcpkiPublKey_t    *pPublKey,
                        CCEcpkiPrivKey_t    *pPrivKey,
                        uint8_t                 *pSharedSecretValue,
                        CCEcdhTempData_t    *pTempBuff);




/* EC WRST Key Generate */
CEXPORT_C CCError_t EcWrstGenKeyPair(const CCEcpkiDomain_t  *pDomain,
                        CCEcpkiUserPrivKey_t   *pUserPrivKey,
                        CCEcpkiUserPublKey_t   *pUserPublKey,
                        CCEcpkiKgTempData_t    *pTempBuff);

/* EC WRST Key Generate with configurable base point */
CEXPORT_C CCError_t EcWrstGenKeyPairBase(const CCEcpkiDomain_t  *pDomain,
                        const uint32_t         ecX [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS],
                        const uint32_t         ecY [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS],
                        CCEcpkiUserPrivKey_t   *pUserPrivKey,
                        CCEcpkiUserPublKey_t   *pUserPublKey,
                        CCEcpkiKgTempData_t    *pTempBuff);



#ifdef __cplusplus
}
#endif

#endif


