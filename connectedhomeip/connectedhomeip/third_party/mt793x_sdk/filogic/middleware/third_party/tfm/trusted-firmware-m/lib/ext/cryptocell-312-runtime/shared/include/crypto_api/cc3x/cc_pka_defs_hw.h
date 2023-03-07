/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pka_defs_hw
 @{
 */

/*!
 @file
 @brief This file contains all of the enums and definitions that are used in
 PKA APIs.
 */

#ifndef _CC_PKA_DEFS_HW_H_
#define _CC_PKA_DEFS_HW_H_

#include "cc_pal_types.h"
#include "cc_pka_hw_plat_defs.h"

/* The valid key sizes in bits for RSA primitives (exponentiation) */
/*! The maximal RSA modulus size. */
#define CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS ((CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS + CC_PKA_WORD_SIZE_IN_BITS) / CC_BITS_IN_32BIT_WORD )
/*! The maximal EC modulus size. */
#define CC_ECPKI_MODUL_MAX_LENGTH_IN_BITS   521

/*! The size of the buffers for Barrett modulus tag NP, used in PKI
algorithms. */
#define CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS 5
/*! The size of the buffers for Barrett modulus tag NP, used in ECC. */
#define CC_PKA_ECPKI_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS  CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS
/*! The actual size of Barrett modulus tag NP in words for current
HW platform. */
#define CC_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS  \
    (((CC_PKA_WORD_SIZE_IN_BITS + PKA_EXTRA_BITS - 1) + (CC_BITS_IN_32BIT_WORD - 1)) / CC_BITS_IN_32BIT_WORD )
/*! The maximal size of the PKA modulus. */
#define CC_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS
/*! The maximal size of the PKA public-key in words. */
#define CC_PKA_PUB_KEY_BUFF_SIZE_IN_WORDS (2*CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS)
/*! The maximal size of the PKA private-key in words. */
#define CC_PKA_PRIV_KEY_BUFF_SIZE_IN_WORDS (2*CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS)
/*! The maximal size of the PKA KG buffer in words */
#define CC_PKA_KGDATA_BUFF_SIZE_IN_WORDS   (3*CC_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 3*CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS)

/*! The maximal size of the EC modulus in words. */
#define CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS  18 /*!< \internal [(CC_ECPKI_MODUL_MAX_LENGTH_IN_BITS + 31)/(sizeof(uint32_t)) + 1] */
/*! The maximal size of the EC order in words. */
#define CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS  (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1)
/*! The maximal size of the EC domain in words. */
#define CC_PKA_DOMAIN_BUFF_SIZE_IN_WORDS (2*CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS)

/*! The ECC NAF buffer definitions. */
#define COUNT_NAF_WORDS_PER_KEY_WORD  8  /*!< \internal Change according to NAF representation (? 2)*/
/*! The maximal length of the ECC NAF buffer. */
#define CC_PKA_ECDSA_NAF_BUFF_MAX_LENGTH_IN_WORDS (COUNT_NAF_WORDS_PER_KEY_WORD*CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS + 1)

#ifndef CC_SUPPORT_ECC_SCA_SW_PROTECT
/* on fast SCA non protected mode required additional buffers for NAF key */
/*! The size of the Scalar buffer in words. */
#define CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS (CC_PKA_ECDSA_NAF_BUFF_MAX_LENGTH_IN_WORDS+CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+2)
#else
/*! The size of the Scalar buffer in words. */
#define CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS  1 /*(4*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)*/
#endif
/*! The size of the ECC temporary buffer in words. */
#define CC_PKA_ECPKI_BUILD_TMP_BUFF_MAX_LENGTH_IN_WORDS (3*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS)
/*! The size of the ECC sign temporary buffer in words. */
#define CC_PKA_ECDSA_SIGN_BUFF_MAX_LENGTH_IN_WORDS (6*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS)
/*! The size of the ECC ECDH temporary-buffer in words. */
#define CC_PKA_ECDH_BUFF_MAX_LENGTH_IN_WORDS (2*CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS + CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS)
/*! The size of the PKA KG temporary-buffer in words. */
#define CC_PKA_KG_BUFF_MAX_LENGTH_IN_WORDS (2*CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS + CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS)
/*! The size of the ECC verify temporary-buffer in words. */
#define CC_PKA_ECDSA_VERIFY_BUFF_MAX_LENGTH_IN_WORDS (3*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)

/* *************************************************************************** */
/*! The maximal size of the modulus buffers for CC_EC_MONT and EC_EDW in
bytes.*/
#define CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_BYTES  32U  /*!< \internal for Curve25519 */
/*! The maximal size of the modulus buffers for CC_EC_MONT and EC_EDW in
words. */
#define CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS   8U  /*!< \internal for Curve25519 */
/*! The size of the ECC Montgomery temporary buffer in words. */
#define CC_EC_MONT_TEMP_BUFF_SIZE_IN_32BIT_WORDS  (8 * CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS) /*!< \internal Change according to actual requirements */
/*! The size of the ECC Edwards temporary buffer in words. */
#define CC_EC_EDW_TEMP_BUFF_SIZE_IN_32BIT_WORDS   (8*CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS + (sizeof(CCHashUserContext_t)+CC_32BIT_WORD_SIZE-1)/CC_32BIT_WORD_SIZE)

/*!
 @}
 */
#endif /*_CC_PKA_DEFS_HW_H_*/
