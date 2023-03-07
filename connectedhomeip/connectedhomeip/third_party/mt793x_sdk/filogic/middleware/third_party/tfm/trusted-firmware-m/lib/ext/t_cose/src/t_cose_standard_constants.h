/*
 * t_cose_standard_constants.h
 *
 * Copyright (c) 2018-2019, Laurence Lundblade. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __T_COSE_STANDARD_CONSTANTS_H__
#define __T_COSE_STANDARD_CONSTANTS_H__

/**
 * \file t_cose_standard_constants.h
 *
 * \brief Constants from COSE standard and IANA registry.
 *
 * This file contains constants identifiers defined in
 * [COSE (RFC 8152)](https://tools.ietf.org/html/rfc8152) and
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 * They include algorithm IDs and other constants.
 *
 * Many constants in the IANA registry are not included here yet as
 * they are not needed by t_cose. They can be added if they become
 * needed.
 *
 * This file is not part of the t_cose public interface as it contains
 * lots of stuff not needed in the public interface. The parts that
 * are needed in the public interface are also defined as \ref
 * T_COSE_ALGORITHM_ES256 and related (there is a pre processor cross
 * check to make sure they don't get defined differently in
 * t_cose_sign1_sign.c).
 */


/* --------------- COSE Header parameters -----------
 * https://www.iana.org/assignments/cose/cose.xhtml#header-parameters
 */

/**
 * \def COSE_HEADER_PARAM_ALG
 *
 * \brief Label of COSE parameter that indicates an algorithm.
 *
 * The algorithm assignments are found in the IANA registry here
 * https://www.iana.org/assignments/cose/cose.xhtml#algorithms
 * Signing algorithms are identified as combinations of the
 * public key algorithm, padding mode and hash. This must be
 * a protected header. They may be string or integers. This
 * implementation only support integer IDs.
 */
#define COSE_HEADER_PARAM_ALG 1


/**
 * \def COSE_HEADER_PARAM_CRIT
 *
 * \brief Label of COSE parameter listing critical header parameters
 *
 * The contents is an array of header parameter labels, either string or
 * integer. The implementation must know how to process them or it is
 * an error.
 */
#define COSE_HEADER_PARAM_CRIT 2


/**
 * \def COSE_HEADER_PARAM_CONTENT_TYPE
 *
 * \brief Label of COSE parameter with the content type
 *
 * Either an integer CoAP content type or a string MIME type. This is
 * the type of the data in the payload.
 */
#define COSE_HEADER_PARAM_CONTENT_TYPE 3


/**
 * \def COSE_HEADER_PARAM_KID
 *
 * \brief CBOR map label of COSE parameter that contains a kid (key ID).
 *
 * The kid is a byte string identifying the key. It is optional and
 * there is no required format. They are not even required to be
 * unique.
 */
#define COSE_HEADER_PARAM_KID 4


/**
 * \def COSE_HEADER_PARAM_IV
 *
 * \brief CBOR map label of parameter that contains an initialization
 * vector.
 *
 * A binary string initialization vector.
 *
 * This implementation only parses this.
 */
#define COSE_HEADER_PARAM_IV 5


/**
 * \def COSE_HEADER_PARAM_PARTIAL_IV
 *
 * \brief CBOR map label of parameter containing partial
 * initialization vector.
 *
 * A binary string partial initialization vector.
 *
 * This implementation only parses this.
 */
#define COSE_HEADER_PARAM_PARTIAL_IV 6


/**
 * \def COSE_HEADER_PARAM_COUNTER_SIGNATURE
 *
 * \brief CBOR map label of parameter that holds one or more counter signature.
 *
 * Counter signatures can be full \c COSE_Sign1, \c COSE_Signature and
 * such messages.  This implementation doesn't support them.
 */
#define COSE_HEADER_PARAM_COUNTER_SIGNATURE 6





/* ------------ COSE Header Algorithm Parameters --------------
 * https://www.iana.org/assignments/cose/cose.xhtml#header-algorithm-parameters
 *
 * None of these are defined here yet, as they are not needed by t_cose yet.
 */




/* ------------- COSE Algorithms ----------------------------
 * https://www.iana.org/assignments/cose/cose.xhtml#algorithms
 */

/**
 * This is defined as reserved by IANA. This implementation uses it to
 * mean the end of a list of algorithm IDs or an unset algorithm ID.
 */
#define COSE_ALGORITHM_RESERVED 0


/**
 * \def COSE_ALGORITHM_ES256
 *
 * \brief Indicates ECDSA with SHA-256.
 *
 * Value for \ref COSE_HEADER_PARAM_ALG to indicate ECDSA with SHA-256.
 *
 * RFC 8152 section 8.1 suggests, but does not require, that this
 * algorithm identifier only be used with keys based on the P-256
 * curve (also known as prime256v1 or secp256r1).
 *
 * See https://tools.ietf.org/search/rfc4492 and https://tools.ietf.org/html/rfc8152
 */
#define COSE_ALGORITHM_ES256 -7

/**
 * \def COSE_ALGORITHM_ES384
 *
 * \brief Indicates ECDSA with SHA-384.
 *
 * See discussion on \ref COSE_ALGORITHM_ES256.
 *
 * RFC 8152 section 8.1 suggests, but does not require, that this
 * algorithm identifier be used only with keys based on the P-384
 * curve (also known as secp384r1).
 */
#define COSE_ALGORITHM_ES384 -35

/**
 * \def COSE_ALGORITHM_ES512
 *
 * \brief Indicates ECDSA with SHA-512.
 *
 * See discussion on \ref COSE_ALGORITHM_ES256.
 *
 * RFC 8152 section 8.1 suggests, but does not require, that this
 * algorithm identifier be used only with keys based on the P-521
 * curve (also known as secp521r1)
 */
#define COSE_ALGORITHM_ES512 -36


/**
 * \def COSE_ALGORITHM_SHA_256
 *
 * \brief Indicates simple SHA-256 hash.
 *
 * This is not used in the t_cose interface, just used internally.
 */
#define COSE_ALGORITHM_SHA_256 -16

/**
 * \def COSE_ALGORITHM_SHA_384
 *
 * \brief Indicates simple SHA-384 hash.
 *
 * This is not used in the t_cose interface, just used internally.
 */
#define COSE_ALGORITHM_SHA_384 -43

/**
 * \def COSE_ALGORITHM_SHA_512
 *
 * \brief Indicates simple SHA-512 hash.
 *
 * This is not used in the t_cose interface, just used internally.
 */
#define COSE_ALGORITHM_SHA_512 -44




/* ---------- COSE Key Common Parameters --------------
 * https://www.iana.org/assignments/cose/cose.xhtml#key-common-parameters
 */

/**
 * \def COSE_KEY_COMMON_KTY
 *
 * \brief Label for data item containing the key type.
 *
 * In a \c COSE_Key, label that indicates the data item containing the
 * key type.
 */
#define COSE_KEY_COMMON_KTY  1

/**
 * \def COSE_KEY_COMMON_KID
 *
 * \brief Label for data item containing the key's kid.
 *
 * In a \c COSE_Key, label that indicates the data item containing the
 * kid of this key.
 */
#define COSE_KEY_COMMON_KID  2




/* ---------- COSE Key Type Parameters --------------------
 * https://www.iana.org/assignments/cose/cose.xhtml#key-type-parameters
 *
 * These are not used by this implementation.
 */

/**
 * \def COSE_KEY_PARAM_CRV
 *
 * \brief Label for data item indicating EC curve.
 *
 * In a \c COSE_Key that holds an EC key of either type \ref
 * COSE_KEY_TYPE_EC2 or \ref COSE_KEY_TYPE_OKP this labels the data
 * item with the EC curve for the key.
 */
#define COSE_KEY_PARAM_CRV           -1

/**
 * \def COSE_KEY_PARAM_X_COORDINATE
 *
 * \brief Label for data item that is an X coordinate of an EC key.
 *
 * In a \c COSE_Key that holds an EC key, this is label that indicates
 * the data item containing the X coordinate.
 *
 * This is used for both key types \ref COSE_KEY_TYPE_EC2 and \ref
 * COSE_KEY_TYPE_OKP.
 */
#define COSE_KEY_PARAM_X_COORDINATE  -2

/**
 * \def COSE_KEY_PARAM_Y_COORDINATE
 *
 * \brief Label for data item that is a y coordinate of an EC key.
 *
 * In a COSE_Key that holds an EC key, this is label that indicates
 * the data item containing the Y coordinate.
 *
 * This is used only for key type \ref COSE_KEY_TYPE_EC2.
 */
#define COSE_KEY_PARAM_Y_COORDINATE  -3

/**
 * \def COSE_KEY_PARAM_PRIVATE_D
 *
 * \brief Label for data item that is d, the private part of EC key.
 *
 * In a \c COSE_Key that holds an EC key, this is label that indicates
 * the data item containing the Y coordinate.
 *
 * This is used for both key types \ref COSE_KEY_TYPE_EC2 and \ref
 * COSE_KEY_TYPE_OKP.
 */
#define COSE_KEY_PARAM_PRIVATE_D  -4




/* ---------- COSE Key Types --------------------------------
 * https://www.iana.org/assignments/cose/cose.xhtml#key-type
 */

/**
 * \def COSE_KEY_TYPE_OKP
 *
 * \brief Key type is Octet Key Pair
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_COMMON_KTY that indicates the \c COSE_Key is some sort of
 * key pair represented by some octets. It may or may not be an EC
 * key.
 */
#define COSE_KEY_TYPE_OKP       1

/**
 * \def COSE_KEY_TYPE_EC2
 *
 * \brief Key is a 2-parameter EC key.
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_COMMON_KTY that indicates the \c COSE_Key is an EC key
 * specified with two coordinates, X and Y.
 */
#define COSE_KEY_TYPE_EC2       2

/**
 * \def COSE_KEY_TYPE_SYMMETRIC
 *
 * \brief Key is a symmetric key.
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_COMMON_KTY that indicates the \c COSE_Key is a symmetric
 * key.
 */
#define COSE_KEY_TYPE_SYMMETRIC  4




/* ----------- COSE Elliptic Curves ---------------------
 * https://www.iana.org/assignments/cose/cose.xhtml#elliptic-curves
 */

/**
 * \def COSE_ELLIPTIC_CURVE_P_256
 *
 * \brief Key type for NIST P-256 key
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_PARAM_CRV to indicate the NIST P-256 curve, also known as
 * secp256r1.
 *
 * This key type is always \ref COSE_KEY_TYPE_EC2.
 */
#define COSE_ELLIPTIC_CURVE_P_256 1

/**
 * \def COSE_ELLIPTIC_CURVE_P_384
 *
 * \brief Key type for NIST P-384 key
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_PARAM_CRV to indicate the NIST P-384 curve, also known as
 * secp384r1.
 *
 * This key type is always \ref COSE_KEY_TYPE_EC2.
 */
#define COSE_ELLIPTIC_CURVE_P_384 2

/**
 * \def COSE_ELLIPTIC_CURVE_P_521
 *
 * \brief Key type for NIST P-521 key
 *
 * In a \c COSE_Key, this is a value of the data item labeled \ref
 * COSE_KEY_PARAM_CRV to indicate the NIST P-521 curve, also known as
 * secp521r1.
 */
#define COSE_ELLIPTIC_CURVE_P_521 3




/* ------- Constants from RFC 8152 ---------
 */

/**
 * \def COSE_SIG_CONTEXT_STRING_SIGNATURE1
 *
 * \brief This is a string constant used by COSE to label \c
 * COSE_Sign1 structures. See RFC 8152, section 4.4.
 */
#define COSE_SIG_CONTEXT_STRING_SIGNATURE1 "Signature1"


#endif /* __T_COSE_STANDARD_CONSTANTS_H__ */
