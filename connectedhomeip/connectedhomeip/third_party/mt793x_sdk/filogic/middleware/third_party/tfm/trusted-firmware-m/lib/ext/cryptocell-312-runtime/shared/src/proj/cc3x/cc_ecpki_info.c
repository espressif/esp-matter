/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* this file contains the definitions of the hashes used in the ecpki */

#include "cc_ecpki_local.h"
#include "cc_hash_defs.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_domains_defs.h"
#include "cc_ecpki_domain_secp192r1.h"
#include "cc_ecpki_domain_secp224r1.h"
#include "cc_ecpki_domain_secp256r1.h"
#include "cc_ecpki_domain_secp521r1.h"
#include "cc_ecpki_domain_secp192k1.h"
#include "cc_ecpki_domain_secp224k1.h"
#include "cc_ecpki_domain_secp256k1.h"
#include "cc_ecpki_domain_secp384r1.h"

const CCEcpkiHash_t ecpki_hash_info[CC_ECPKI_HASH_NumOfModes] = {
        /*CC_ECPKI_HASH_SHA1_mode         */        {CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS, CC_HASH_SHA1_mode},
        /*CC_ECPKI_HASH_SHA224_mode       */          {CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS, CC_HASH_SHA224_mode},
        /*CC_ECPKI_HASH_SHA256_mode       */        {CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS, CC_HASH_SHA256_mode},
        /*CC_ECPKI_HASH_SHA384_mode       */        {CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS, CC_HASH_SHA384_mode},
        /*CC_ECPKI_HASH_SHA512_mode       */        {CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS, CC_HASH_SHA512_mode},
        /*CC_ECPKI_AFTER_HASH_SHA1_mode   */        {CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS, CC_HASH_NumOfModes},
        /*CC_ECPKI_AFTER_HASH_SHA224_mode */        {CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS, CC_HASH_NumOfModes},
        /*CC_ECPKI_AFTER_HASH_SHA256_mode */        {CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS, CC_HASH_NumOfModes},
        /*CC_ECPKI_AFTER_HASH_SHA384_mode */        {CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS, CC_HASH_NumOfModes},
        /*CC_ECPKI_AFTER_HASH_SHA512_mode */        {CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS, CC_HASH_NumOfModes},
};

const uint8_t ecpki_supported_hash_modes[CC_ECPKI_HASH_NumOfModes] = {
    /*CC_ECPKI_HASH_SHA1_mode         */ CC_TRUE,
    /*CC_ECPKI_HASH_SHA224_mode       */ CC_TRUE,
    /*CC_ECPKI_HASH_SHA256_mode       */ CC_TRUE,
    /*CC_ECPKI_HASH_SHA384_mode       */ CC_FALSE,
    /*CC_ECPKI_HASH_SHA512_mode       */ CC_TRUE,
    /*CC_ECPKI_AFTER_HASH_SHA1_mode   */ CC_TRUE,
    /*CC_ECPKI_AFTER_HASH_SHA224_mode */ CC_TRUE,
    /*CC_ECPKI_AFTER_HASH_SHA256_mode */ CC_TRUE,
    /*CC_ECPKI_AFTER_HASH_SHA384_mode */ CC_TRUE,
    /*CC_ECPKI_AFTER_HASH_SHA512_mode */ CC_TRUE
};


const getDomainFuncP ecDomainsFuncP[CC_ECPKI_DomainID_OffMode] = {
            (&CC_EcpkiGetSecp192k1DomainP),
            (&CC_EcpkiGetSecp192r1DomainP),
            (&CC_EcpkiGetSecp224k1DomainP),
            (&CC_EcpkiGetSecp224r1DomainP),
            (&CC_EcpkiGetSecp256k1DomainP),
            (&CC_EcpkiGetSecp256r1DomainP),
            (&CC_EcpkiGetSecp384r1DomainP),
            (&CC_EcpkiGetSecp521r1DomainP)
};

