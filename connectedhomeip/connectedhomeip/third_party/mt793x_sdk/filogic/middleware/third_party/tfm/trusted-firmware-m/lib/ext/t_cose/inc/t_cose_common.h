/*
 * t_cose_common.h
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#ifndef __T_COSE_COMMON_H__
#define __T_COSE_COMMON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file t_cose_common.h
 *
 * \brief This file contains definitions common to all public t_cose
 * interfaces.
 *
 * t_cose_common.h contains the definitions common to all public
 * t_cose interfaces, particularly the error codes, algorithm
 * identification constants and the structure containing a key.
 *
 * **Compile Time Configuration Options**
 *
 * \c T_COSE_DISABLE_SHORT_CIRCUIT_SIGN -- This disables short-circuit
 * signing test mode. This saves a small amount of object code
 *
 * \c T_COSE_DISABLE_ES512 -- Disables the COSE algorithm ES512
 * algorithm. This saves a tiny amount of code and a few hundred bytes
 * of stack. It saves more than \c T_COSE_DISABLE_ES384.
 *
 * \c T_COSE_DISABLE_ES384 -- Disables the COSE algorithm ES384
 * algorithm. This saves a tiny amount of code and a few hundred bytes
 * of stack. No stack will be saved if \c T_COSE_DISABLE_ES512 is not
 * also defined.
 *
 * \c T_COSE_DISABLE_CONTENT_TYPE -- Disables the content type
 * parameters for both signing and verifying.
 */




/**
 * \def T_COSE_ALGORITHM_ES256
 *
 * \brief Indicates ECDSA with SHA-256.
 *
 * This value comes from the
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 *
 * The COSE standard recommends a key using the secp256r1 curve with
 * this algorithm. This curve is also known as prime256v1 and P-256.
 */
#define T_COSE_ALGORITHM_ES256 -7

/**
 * \def T_COSE_ALGORITHM_ES384
 *
 * \brief Indicates ECDSA with SHA-384.
 *
 * This value comes from the
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 *
 * The COSE standard recommends a key using the secp384r1 curve with
 * this algorithm. This curve is also known as P-384.
 */
#define T_COSE_ALGORITHM_ES384 -35

/**
 * \def T_COSE_ALGORITHM_ES512
 *
 * \brief Indicates ECDSA with SHA-512.
 *
 * This value comes from the
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 *
 * The COSE standard recommends a key using the secp521r1 curve with
 * this algorithm. This curve is also known as P-521.
 */
#define T_COSE_ALGORITHM_ES512 -36




/**
 * Indicates the cryptographic library the \ref t_cose_key is intended
 * for. Usually only one cryptographic library is integrated so this
 * serves as a cross-check.
 */
enum t_cose_crypto_lib_t {
    /** can be used for integrations
     * that don't have or don't want to have any cross-check.
     */
    T_COSE_CRYPTO_LIB_UNIDENTIFIED = 0,
    /** \c key_ptr points to a malloced OpenSSL EC_KEY. The caller
     * needs to free it after the operation is done. */
    T_COSE_CRYPTO_LIB_OPENSSL = 1,
     /** \c key_handle is a \c psa_key_handle_t in Arm's Platform Security
      * Architecture */
    T_COSE_CRYPTO_LIB_PSA = 2
};


/**
 * This structure is used to indicate or pass a key through the t_cose
 * implementation to the underlying, platform-specific cryptography
 * libraries for signing and verifying signature. You must know the
 * cryptographic library that is integrated with t_cose to know how to
 * fill in this data structure.
 *
 * For example, in the OpenSSL integration, \ref key_ptr should point
 * to an OpenSSL \c EC_KEY type.
 */
struct t_cose_key {
    /** Identifies the crypto library this key was created for.  The
     * crypto library knows if it uses the handle or the pointer so
     * this indirectly selects the union member. */
    enum t_cose_crypto_lib_t crypto_lib;
    union {
        /** For libraries that use a pointer to the key or key
         * handle. \c NULL indicates empty. */
        void *key_ptr;
        /** For libraries that use an integer handle to the key */
        uint64_t key_handle;
    } k;
};

/** An empty or \c NULL \c t_cose_key */
#define T_COSE_NULL_KEY \
    ((struct t_cose_key){T_COSE_CRYPTO_LIB_UNIDENTIFIED, {0}})


/* Private value. Intentionally not documented for Doxygen.  This is
 * the size allocated for the encoded protected header parameters.  It
 * needs to be big enough for encode_protected_parameters() to
 * succeed. It currently sized for one parameter with an algorithm ID
 * up to 32 bits long -- one byte for the wrapping map, one byte for
 * the label, 5 bytes for the ID. If this is made accidentially too
 * small, QCBOR will only return an error, and not overrun any
 * buffers.
 *
 * 17 extra bytes are added, rounding it up to 24 total, in case some
 * other protected header parameter is to be added and so the test
 * using T_COSE_TEST_CRIT_PARAMETER_EXIST can work.
 */
#define T_COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS (1+1+5+17)


/**
 * Error codes return by t_cose.
 */
/*
 * Do not reorder these. It is OK to add new ones at the end.
 *
 * Explicit values are included because some tools like debuggers show
 * only the value, not the symbol, and it is hard to count up through
 * 35 lines to figure out the actual value.
 */
enum t_cose_err_t {
    /** Operation completed successfully. */
    T_COSE_SUCCESS = 0,

    /** The requested signing algorithm is not supported.  */
    T_COSE_ERR_UNSUPPORTED_SIGNING_ALG = 1,

    /** Internal error when encoding protected parameters, usually
     * because they are too big. It is internal because the caller
     * can't really affect the size of the protected parameters. */
    T_COSE_ERR_MAKING_PROTECTED = 2,

    /** The hash algorithm needed is not supported. Note that the
     * signing algorithm identifier identifies the hash algorithm. */
    T_COSE_ERR_UNSUPPORTED_HASH = 3,

    /** Some system failure when running the hash algorithm. */
    T_COSE_ERR_HASH_GENERAL_FAIL = 4,

    /** The buffer to receive a hash result is too small. */
    T_COSE_ERR_HASH_BUFFER_SIZE = 5,

    /** The buffer to receive result of a signing operation is too
     * small. */
    T_COSE_ERR_SIG_BUFFER_SIZE = 6,

    /** When verifying a \c COSE_Sign1, the CBOR is "well-formed", but
     * something is wrong with the format of the CBOR outside of the
     * header parameters. For example, it is missing something like
     * the payload or something is of an unexpected type. */
    T_COSE_ERR_SIGN1_FORMAT = 8,

    /** When decoding some CBOR like a \c COSE_Sign1, the CBOR was not
     * "well-formed". Most likely what was supposed to be CBOR is
     * either not or is corrupted. The CBOR is can't be decoded. */
    T_COSE_ERR_CBOR_NOT_WELL_FORMED = 9,

    /** The CBOR is "well-formed", but something is wrong with format
     * in the header parameters.  For example, a parameter is labeled
     * with other than an integer or string or the value is an integer
     * when a byte string is expected. */
    T_COSE_ERR_PARAMETER_CBOR = 10,

    /** No algorithm ID was found when one is needed. For example,
     * when verifying a \c COSE_Sign1. */
    T_COSE_ERR_NO_ALG_ID = 11,

    /** No kid (key ID) was found when one is needed. For example,
     * when verifying a \c COSE_Sign1. */
    T_COSE_ERR_NO_KID = 12,

    /** Signature verification failed. For example, the cryptographic
     * operations completed successfully but hash wasn't as
     * expected. */
    T_COSE_ERR_SIG_VERIFY = 13,

    /** Verification of a short-circuit signature failed. */
    T_COSE_ERR_BAD_SHORT_CIRCUIT_KID = 14,

    /** Some (unspecified) argument was not valid. */
    T_COSE_ERR_INVALID_ARGUMENT = 15,

    /** Out of heap memory. This originates in crypto library as
     * t_cose does not use malloc. */
    T_COSE_ERR_INSUFFICIENT_MEMORY = 16,

    /** General unspecific failure. */
    T_COSE_ERR_FAIL = 17,

    /** Equivalent to \c PSA_ERROR_TAMPERING_DETECTED. */
    T_COSE_ERR_TAMPERING_DETECTED = 18,

    /** The key identified by a \ref t_cose_key or a key ID was not
     * found. */
    T_COSE_ERR_UNKNOWN_KEY = 19,

    /** The key was found, but it was the wrong type for the
      * operation. */
    T_COSE_ERR_WRONG_TYPE_OF_KEY = 20,

    /** Error constructing the COSE \c Sig_structure when signing or
     *  verify. */
    T_COSE_ERR_SIG_STRUCT = 21,

    /** Signature was short-circuit. The option \ref
     * T_COSE_OPT_ALLOW_SHORT_CIRCUIT to allow verification of
     * short-circuit signatures was not set.  */
    T_COSE_ERR_SHORT_CIRCUIT_SIG = 22,

    /** Something generally went wrong in the crypto adaptor when
      * signing or verifying. */
    T_COSE_ERR_SIG_FAIL = 23,

    /** Something went wrong formatting the CBOR.  Possibly the
     * payload has maps or arrays that are not closed when using
     * t_cose_sign1_encode_parameters() and
     * t_cose_sign1_encode_signature() to sign a \c COSE_Sign1. */
    T_COSE_ERR_CBOR_FORMATTING = 24,

     /** The buffer passed in to receive the output is too small. */
    T_COSE_ERR_TOO_SMALL = 25,

    /** More parameters (more than \ref T_COSE_PARAMETER_LIST_MAX)
     * than this implementation can handle. Note that all parameters
     * need to be checked for criticality so all parameters need to be
     * examined. */
    T_COSE_ERR_TOO_MANY_PARAMETERS = 26,

    /** A parameter was encountered that was unknown and also listed in
      * the crit labels parameter. */
    T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER = 27,

    /** A request was made to signed with a short-circuit sig, \ref
     * T_COSE_OPT_SHORT_CIRCUIT_SIG, but short circuit signature are
     * disabled (compiled out) for this implementation.  */
    T_COSE_ERR_SHORT_CIRCUIT_SIG_DISABLED = 28,

    /** The key type in a \ref t_cose_key is wrong for the
     * cryptographic library used by this integration of t_cose.
     */
    T_COSE_ERR_INCORRECT_KEY_FOR_LIB = 29,
    /** This implementation only handles integer COSE algorithm IDs with
     * values less than \c INT32_MAX. */

    T_COSE_ERR_NON_INTEGER_ALG_ID = 30,
    /** The content type parameter contains a content type that is
     * neither integer or text string or it is an integer not in the
     * range of 0 to \c UINT16_MAX. */
    T_COSE_ERR_BAD_CONTENT_TYPE = 31,

    /** If the option \ref T_COSE_OPT_TAG_REQUIRED is set for
     * t_cose_sign1_verify() and the tag is absent, this error is
     * returned. */
    T_COSE_ERR_INCORRECTLY_TAGGED = 32,

    /** The signing or verification key given is empty. */
    T_COSE_ERR_EMPTY_KEY = 33,

    /** A header parameter occurs twice, perhaps once in protected and
     * once in unprotected. Duplicate header parameters are not
     * allowed in COSE.
     */
    T_COSE_ERR_DUPLICATE_PARAMETER = 34,

    /** A header parameter that should be protected (alg id or crit)
     * is not. This occurs when verifying a \c COSE_Sign1 that is
     * improperly constructed. */
    T_COSE_ERR_PARAMETER_NOT_PROTECTED = 35,

    /** Something is wrong with the crit parameter. */
    T_COSE_ERR_CRIT_PARAMETER = 36,

};




/**
 * The maximum number of header parameters that can be handled during
 * verification of a \c COSE_Sign1 message. \ref
 * T_COSE_ERR_TOO_MANY_PARAMETERS will be returned by
 * t_cose_sign1_verify() if the input message has more.
 *
 * There can be both \ref T_COSE_PARAMETER_LIST_MAX integer-labeled
 * parameters and \ref T_COSE_PARAMETER_LIST_MAX string-labeled
 * parameters.
 *
 * This is a hard maximum so the implementation doesn't need
 * malloc. This constant can be increased if needed. Doing so will
 * increase stack usage.
 */
#define T_COSE_PARAMETER_LIST_MAX 10



/**
 * The value of an unsigned integer content type indicating no content
 * type.  See \ref t_cose_parameters.
 */
#define T_COSE_EMPTY_UINT_CONTENT_TYPE UINT16_MAX+1


#ifdef __cplusplus
}
#endif


#endif /* __T_COSE_COMMON_H__ */
