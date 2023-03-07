/*
 *  t_cose_util.h
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#ifndef __T_COSE_UTIL_H__
#define __T_COSE_UTIL_H__

#include <stdint.h>
#include "q_useful_buf.h"
#include "t_cose_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file t_cose_util.h
 *
 * \brief Utility functions used internally by the t_cose implementation.
 *
 */


/**
 * The modes in which the payload is passed to create_tbs_hash().
 * This exists so the TBS bytes can be hashed in two separate chunks
 * and avoids needing a second buffer the size of the payload in the
 * t_cose implementation.
 */
enum t_cose_tbs_hash_mode_t {
    /** The bytes passed for the payload include a wrapping bstr so
     * one does not need to be added.
     */
    T_COSE_TBS_PAYLOAD_IS_BSTR_WRAPPED,
    /** The bytes passed for the payload do NOT have a wrapping bstr
     * so one must be added.
     */
    T_COSE_TBS_BARE_PAYLOAD
};



/**
 * This value represents an invalid or in-error algorithm ID.  The
 * value selected is 0 as this is reserved in the IANA COSE algorithm
 * registry and is very unlikely to ever be used.  (It would take am
 * IETF standards-action to put it to use).
 */
#define T_COSE_INVALID_ALGORITHM_ID COSE_ALGORITHM_RESERVED


/**
 * \brief Return hash algorithm ID from a signature algorithm ID
 *
 * \param[in] cose_algorithm_id  A COSE signature algorithm identifier.
 *
 * \return \c T_COSE_INVALID_ALGORITHM_ID when the signature algorithm ID
              is not known.
 *
 * This works off of algorithm identifiers defined in the
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 * Corresponding local integer constants are defined in
 * t_cose_standard_constants.h.
 *
 * COSE signing algorithms are the combination of public key
 * algorithm, hash algorithm and hash size and imply an appropriate
 * key size.  They are simple integers making them convenient for
 * direct use in code.
 *
 * This function returns an identifier for only the hash algorithm
 * from the combined identifier.
 *
 * If the needed algorithm identifiers are not in the IANA registry,
 * they can be added to it. This will take some time and work.  It is
 * also fine to use algorithms in the COSE proprietary space.
 */
int32_t hash_alg_id_from_sig_alg_id(int32_t cose_algorithm_id);


/**
 * \brief Create the hash of the to-be-signed (TBS) bytes for COSE.
 *
 * \param[in] cose_algorithm_id     The COSE signing algorithm ID. Used to
 *                                  determine which hash function to use.
 * \param[in] protected_parameters  Full, CBOR encoded, protected parameters.
 * \param[in] payload_mode          See \ref t_cose_tbs_hash_mode_t.
 * \param[in] payload               The CBOR encoded payload. It may or may
 *                                  not have a wrapping bstr per
 *                                  \c payload_mode.
 * \param[in] buffer_for_hash       Pointer and length of buffer into which
 *                                  the resulting hash is put.
 * \param[out] hash                 Pointer and length of the
 *                                  resulting hash.
 *
 * \return This returns one of the error codes defined by \ref t_cose_err_t.
 *
 * \retval T_COSE_ERR_SIG_STRUCT
 *         Most likely this is because the protected_parameters passed in
 *         is larger than \c T_COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS.
 * \retval T_COSE_ERR_UNSUPPORTED_HASH
 *         If the hash algorithm is not known.
 * \retval T_COSE_ERR_HASH_GENERAL_FAIL
 *         In case of some general hash failure.
 *
 * The input to the public key signature algorithm in COSE is the hash
 * of a CBOR encoded structure containing the protected parameters
 * algorithm ID and a few other things. This formats that structure
 * and computes the hash of it. These are known as the to-be-signed or
 * "TBS" bytes. The exact specification is in
 * [RFC 8152 section 4.4](https://tools.ietf.org/html/rfc8152#section-4.4).
 */
enum t_cose_err_t create_tbs_hash(int32_t                     cose_algorithm_id,
                                  struct q_useful_buf_c       protected_parameters,
                                  enum t_cose_tbs_hash_mode_t payload_mode,
                                  struct q_useful_buf_c       payload,
                                  struct q_useful_buf         buffer_for_hash,
                                  struct q_useful_buf_c      *hash);




#ifndef T_COSE_DISABLE_SHORT_CIRCUIT_SIGN

/**
 * Size of the key returned by get_short_circuit_kid(). It is always
 * this size.
 */
#define T_COSE_SHORT_CIRCUIT_KID_SIZE 32


/**
 * \brief Get the special kid for short-circuit signing.
 *
 * \returns Buffer with the kid.
 *
 * This always returns the same kid. It always indicates short-circuit
 * signing. It is OK to hard code this kid value as the probability of
 * collision with this ID is extremely low and the same as for
 * collision between any two key IDs (kids) of any sort.
 *
 * This always returns a pointer to the same memory as the result
 * returned by this never changes.
 *
 * This is the value of the kid.
 *
 *        0xef, 0x95, 0x4b, 0x4b, 0xd9, 0xbd, 0xf6, 0x70,
 *        0xd0, 0x33, 0x60, 0x82, 0xf5, 0xef, 0x15, 0x2a,
 *        0xf8, 0xf3, 0x5b, 0x6a, 0x6c, 0x00, 0xef, 0xa6,
 *        0xa9, 0xa7, 0x1f, 0x49, 0x51, 0x7e, 0x18, 0xc6
 *
 */
struct q_useful_buf_c get_short_circuit_kid(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __T_COSE_UTIL_H__ */
