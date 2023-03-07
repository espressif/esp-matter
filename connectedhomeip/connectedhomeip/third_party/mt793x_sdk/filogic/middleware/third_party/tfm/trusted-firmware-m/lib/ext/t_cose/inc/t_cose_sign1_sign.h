/*
 * t_cose_sign1_sign.h
 *
 * Copyright (c) 2018-2019, Laurence Lundblade. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __T_COSE_SIGN1_H__
#define __T_COSE_SIGN1_H__

#include <stdint.h>
#include <stdbool.h>
#include "qcbor.h"
#include "t_cose_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \file t_cose_sign1_sign.h
 *
 * \brief Create a \c COSE_Sign1 message, usually for EAT or CWT Token.
 *
 * This creates a \c COSE_Sign1 message in compliance with
 * [COSE (RFC 8152)](https://tools.ietf.org/html/rfc8152).
 * A \c COSE_Sign1 message is a CBOR encoded binary blob that contains
 * header parameters, a payload and a signature. Usually the signature is made
 * with an EC signing algorithm like ECDSA.
 *
 * This implementation is intended to be small and portable to
 * different OS's and platforms. Its dependencies are:
 * - [QCBOR](https://github.com/laurencelundblade/QCBOR)
 * - <stdint.h>, <string.h>, <stddef.h>
 * - Hash functions like SHA-256
 * - Signing functions like ECDSA
 *
 * There is a cryptographic adaptation layer defined in
 * t_cose_crypto.h.  An implementation can be made of the functions in
 * it for different cryptographic libraries. This means that different
 * integrations with different cryptographic libraries may support
 * only signing with a particular set of algorithms. Integration with
 * [OpenSSL](https://www.openssl.org) is supported.  Key ID look up
 * also varies by different cryptographic library integrations.
 *
 * This implementation has a mode where a CBOR-format payload can be
 * output directly into the output buffer. This saves having two
 * copies of the payload in memory. For this mode use
 * t_cose_sign1_encode_parameters() and
 * t_cose_sign1_encode_signature(). For a simpler API that just takes
 * the payload as an input buffer use t_cose_sign1_sign().
 *
 * See t_cose_common.h for preprocessor defines to reduce object code
 * and stack use by disabling features.
 */


/**
 * This is the context for creating a \c COSE_Sign1 structure. The
 * caller should allocate it and pass it to the functions here.  This
 * is about 100 bytes so it fits easily on the stack.
 */
struct t_cose_sign1_sign_ctx {
    /* Private data structure */
    uint8_t               protected_parameters_buffer[T_COSE_SIGN1_MAX_SIZE_PROTECTED_PARAMETERS];
    struct q_useful_buf_c protected_parameters; /* The encoded protected parameters */
    int32_t               cose_algorithm_id;
    struct t_cose_key     signing_key;
    int32_t               option_flags;
    struct q_useful_buf_c kid;
#ifndef T_COSE_DISABLE_CONTENT_TYPE
    uint32_t              content_type_uint;
    const char *          content_type_tstr;
#endif
};


/**
 * This selects a signing test mode called _short_ _circuit_
 * _signing_. This mode is useful when there is no signing key
 * available, perhaps because it has not been provisioned or
 * configured for the particular device. It may also be because the
 * public key cryptographic functions have not been connected up in
 * the cryptographic adaptation layer.
 *
 * It has no value for security at all. Data signed this way MUST NOT
 * be trusted as anyone can sign like this.
 *
 * In this mode, the signature is the hash of that which would
 * normally be signed by the public key algorithm. To make the
 * signature the correct size for the particular algorithm, instances
 * of the hash are concatenated to pad it out.
 *
 * This mode is very useful for testing because all the code except
 * the actual signing algorithm is run exactly as it would if a proper
 * signing algorithm was run. This can be used for end-end system
 * testing all the way to a server or relying party, not just for
 * testing device code as t_cose_sign1_verify() supports it too.
 */
#define T_COSE_OPT_SHORT_CIRCUIT_SIG 0x00000001


/**
 * An \c option_flag for t_cose_sign1_sign_init() to not add the CBOR
 * type 6 tag for \c COSE_Sign1 whose value is 18. Some uses of COSE
 * may require this tag be absent because it is known that it is a \c
 * COSE_Sign1 from surrounding context.
 *
 * Or said another way, per the COSE RFC, this code produces a \c
 * COSE_Sign1_Tagged by default and a \c COSE_Sign1 when this flag is
 * set.  The only difference between these two is the CBOR tag.
 */
#define T_COSE_OPT_OMIT_CBOR_TAG 0x00000002




/**
 * \brief  Initialize to start creating a \c COSE_Sign1.
 *
 * \param[in] context            The t_cose signing context.
 * \param[in] option_flags       One of \c T_COSE_OPT_XXXX.
 * \param[in] cose_algorithm_id  The algorithm to sign with, for example
 *                               \ref T_COSE_ALGORITHM_ES256.
 *
 * Initialize the \ref t_cose_sign1_ctx context. Typically, no
 * \c option_flags are needed and 0 is passed. A \c cose_algorithm_id
 * must always be given. See \ref T_COSE_OPT_SHORT_CIRCUIT_SIG and
 * related for possible option flags.
 *
 * The algorithm ID space is from
 * [COSE (RFC8152)](https://tools.ietf.org/html/rfc8152) and the
 * [IANA COSE Registry](https://www.iana.org/assignments/cose/cose.xhtml).
 * \ref T_COSE_ALGORITHM_ES256 and a few others are defined here for
 * convenience. The signing algorithms supported depends on the
 * cryptographic library that t_cose is integrated with.
 *
 * Errors such as the passing of an unsupported \c cose_algorithm_id
 * are reported when t_cose_sign1_sign() or
 * t_cose_sign1_encode_parameters() is called.
 */
static void
t_cose_sign1_sign_init(struct t_cose_sign1_sign_ctx *context,
                       int32_t                       option_flags,
                       int32_t                       cose_algorithm_id);


/**
 * \brief  Set the key and kid (key ID) for signing.
 *
 * \param[in] context      The t_cose signing context.
 * \param[in] signing_key  The signing key to use or \ref T_COSE_NULL_KEY.
 * \param[in] kid          COSE kid (key ID) parameter or \c NULL_Q_USEFUL_BUF_C.
 *
 * This needs to be called to set the signing key to use. The \c kid
 * may be omitted by giving \c NULL_Q_USEFUL_BUF_C.
 *
 * If short-circuit signing is used,
 * \ref T_COSE_OPT_SHORT_CIRCUIT_SIG, then this does not need to be
 * called. If it is called the \c kid given will be used, but the \c
 * signing_key is never used. When the \c kid is given with a
 * short-circuit signature, the internally fixed kid for short circuit
 * will not be used and this \c COSE_Sign1 message can not be verified
 * by t_cose_sign1_verify().
 */
static void
t_cose_sign1_set_signing_key(struct t_cose_sign1_sign_ctx *context,
                             struct t_cose_key             signing_key,
                             struct q_useful_buf_c         kid);



#ifndef T_COSE_DISABLE_CONTENT_TYPE
/**
 * \brief Set the payload content type using CoAP content types.
 *
 * \param[in] context      The t_cose signing context.
 * \param[in] content_type The content type of the payload as defined
 *                         in the IANA CoAP Content-Formats registry.
 *
 * It is not allowed to have both a CoAP and MIME content type. This
 * error will show up when t_cose_sign1_sign() or
 * t_cose_sign1_encode_parameters() is called as no error is returned by
 * this function.
 *
 * The IANA CoAP Content-Formats registry is found
 * [here](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats).
 */
static inline void
t_cose_sign1_set_content_type_uint(struct t_cose_sign1_sign_ctx *context,
                                   uint16_t                      content_type);

/**
 * \brief Set the payload content type using MIME content types.
 *
 * \param[in] context      The t_cose signing context.
 * \param[in] content_type The content type of the payload as defined
 *                         in the IANA Media Types registry.

 *
 * It is not allowed to have both a CoAP and MIME content type. This
 * error will show up when t_cose_sign1_sign() or
 * t_cose_sign1_encode_parameters() is called.
 *
 * The IANA Media Types registry can be found
 * [here](https://www.iana.org/assignments/media-types/media-types.xhtml).
 * These have been known as MIME types in the past.
 */
static inline void
t_cose_sign1_set_content_type_tstr(struct t_cose_sign1_sign_ctx *context,
                                   const char                   *content_type);
#endif /* T_COSE_DISABLE_CONTENT_TYPE */



/**
 * \brief  Create and sign a \c COSE_Sign1 message with a payload.
 *
 * \param[in] context  The t_cose signing context.
 * \param[in] payload  Pointer and length of payload to sign.
 * \param[in] out_buf  Pointer and length of buffer to output to.
 * \param[out] result  Pointer and length of the resulting \c COSE_Sign1.
 *
 * The \c context must have been initialized with
 * t_cose_sign1_sign_init() and the key set with
 * t_cose_sign1_set_signing_key() before this is called.
 *
 * This creates the COSE header parameter, hashes and signs the
 * payload and creates the signature. \c out_buf gives the pointer and
 * length memory into which the output is written. The pointer and
 * length of the actual \c COSE_Sign1 is returned in \c result.
 *
 * Typically, the required size of \c out_buf is about 30 bytes plus
 * the size of the signature and the size of the key ID. This is about
 * 150 bytes for ECDSA 256 with a 32-byte key ID.
 *
 * To compute the size of the buffer needed before it is allocated
 * call this with \c out_buf containing a \c NULL pointer and large
 * length like \c UINT32_MAX.  The algorithm and key, kid and such
 * must be set up just as if the real \c COSE_Sign1 were to be created
 * as these values are needed to compute the size correctly.  The
 * contents of \c result will be a \c NULL pointer and the length of
 * the \c COSE_Sign1. When this is run like this, the cryptographic
 * functions will not actually run, but the size of their output will
 * be taken into account to give an exact size.
 *
 * This function requires the payload be complete and formatted in a
 * contiguous buffer. The resulting \c COSE_Sign1 message also
 * contains the payload preceded by the header parameters and followed
 * by the signature, all CBOR formatted. This function thus requires
 * two copies of the payload to be in memory.  Alternatively
 * t_cose_sign1_encode_parameters() and
 * t_cose_sign1_encode_signature() can be used. They are more complex
 * to use, but avoid the two copies of the payload.
 */
enum t_cose_err_t
t_cose_sign1_sign(struct t_cose_sign1_sign_ctx *context,
                  struct q_useful_buf_c         payload,
                  struct q_useful_buf           out_buf,
                  struct q_useful_buf_c        *result);


/**
 * \brief  Output first part and parameters for a \c COSE_Sign1 message.
 *
 * \param[in] context          The t_cose signing context.
 * \param[in] cbor_encode_ctx  Encoding context to output to.
 *
 * This is the more complex and more memory efficient alternative to
 * t_cose_sign1_sign(). Like t_cose_sign1_sign(),
 * t_cose_sign1_sign_init() and t_cose_sign1_set_signing_key() must be
 * called before calling this.
 *
 * When this is called, the opening parts of the \c COSE_Sign1 message
 * are output to the \c cbor_encode_ctx.
 *
 * After this is called, the CBOR-formatted payload must be written to
 * the \c cbor_encode_ctx by calling all the various
 * \c QCBOREncode_AddXxx calls. It can be as simple or complex as needed.
 *
 * To complete the \c COSE_Sign1 call t_cose_sign1_encode_signature().
 *
 * The \c cbor_encode_ctx must have been initialized with an output
 * buffer to hold the \c COSE_Sign1 header parameters, the payload and the
 * signature.
 *
 * This and t_cose_sign1_encode_signature() can be used to calculate
 * the size of the \c COSE_Sign1 in the way \c QCBOREncode is usually
 * used to calculate sizes. In this case the \c t_cose_sign1_ctx must
 * be initialized with the options, algorithm, key and kid just as
 * normal as these are needed to calculate the size. Then set up the
 * QCBOR encoder context with a \c NULL pointer and large length like
 * \c UINT32_MAX.  Call t_cose_sign1_encode_parameters(), then format
 * the payload into the encoder context, then call
 * t_cose_sign1_encode_signature().  Finally call \c
 * QCBOREncode_FinishGetSize() to get the length.
 */
enum t_cose_err_t
t_cose_sign1_encode_parameters(struct t_cose_sign1_sign_ctx *context,
                               QCBOREncodeContext           *cbor_encode_ctx);


/**
 * \brief Finish a \c COSE_Sign1 message by outputting the signature.
 *
 * \param[in] context          The t_cose signing context.
 * \param[in] cbor_encode_ctx  Encoding context to output to.
 *
 * \return This returns one of the error codes defined by \ref t_cose_err_t.
 *
 * Call this to complete creation of a signed \c COSE_Sign1 started
 * with t_cose_sign1_encode_parameters().
 *
 * This is when the cryptographic signature algorithm is run.
 *
 * The completed \c COSE_Sign1 message is retrieved from the
 * \c cbor_encode_ctx by calling \c QCBOREncode_Finish().
 */
enum t_cose_err_t
t_cose_sign1_encode_signature(struct t_cose_sign1_sign_ctx *context,
                              QCBOREncodeContext           *cbor_encode_ctx);






/* ------------------------------------------------------------------------
 * Inline implementations of public functions defined above.
 */
static inline void
t_cose_sign1_sign_init(struct t_cose_sign1_sign_ctx *me,
                       int32_t                       option_flags,
                       int32_t                       cose_algorithm_id)
{
    memset(me, 0, sizeof(*me));
#ifndef T_COSE_DISABLE_CONTENT_TYPE
    /* Only member for which 0 is not the empty state */
    me->content_type_uint = T_COSE_EMPTY_UINT_CONTENT_TYPE;
#endif

    me->cose_algorithm_id = cose_algorithm_id;
    me->option_flags      = option_flags;
}


static inline void
t_cose_sign1_set_signing_key(struct t_cose_sign1_sign_ctx *me,
                             struct t_cose_key             signing_key,
                             struct q_useful_buf_c         kid)
{
    me->kid         = kid;
    me->signing_key = signing_key;
}


#ifndef T_COSE_DISABLE_CONTENT_TYPE
static inline void
t_cose_sign1_set_content_type_uint(struct t_cose_sign1_sign_ctx *me,
                                   uint16_t                     content_type)
{
    me->content_type_uint = content_type;
}


static inline void
t_cose_sign1_set_content_type_tstr(struct t_cose_sign1_sign_ctx *me,
                                   const char                   *content_type)
{
    me->content_type_tstr = content_type;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __T_COSE_SIGN1_H__ */
