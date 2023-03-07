/*
 * attest_token_decode.h
 *
 * Copyright (c) 2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */
#ifndef __ATTEST_TOKEN_DECODE_H__
#define __ATTEST_TOKEN_DECODE_H__

#include "q_useful_buf.h"
#include <stdbool.h>
#include "attest_token.h"
#include "attest_eat_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file attest_token_decode.h
 *
 * \brief Attestation Token Decoding Interface
 *
 * The context and functions here are used to decode an attestation
 * token as follows:
 *
 * -# Create a \ref attest_token_decode_context, most likely as a
 *    stack variable.
 *
 * -# Initialize it by calling attest_token_decode_init()
 *
 * -# Tell it which public key to use for verification using
 *    attest_token_decode_set_cose_pub_key() or
 *    attest_token_decode_set_pub_key_select().
 *
 * -# Pass the token in and validate it by calling
 *    attest_token_decode_validate_token().
 *
 * -# Call the various \c attest_token_get_xxx() methods in any
 * order. The strings returned by the these functions will point into
 * the token passed to attest_token_decode_validate_token(). A copy is
 * NOT made.
 *
 * The entire token is validated and decoded in place.  No copies are
 * made internally. The data returned by the \c attest_token_get_xxx()
 * methods is not a copy so the lifetime of the \c struct \c
 * q_useful_buf_c containing the token must be maintained.
 *
 * Aside from the cryptographic functions, this allocates no
 * memory. It works entirely off the stack. It makes use of t_cose to
 * validate the signature and QCBOR for CBOR decoding.
 *
 * This decoder only works with labels (keys) that are integers even
 * though labels can be any data type in CBOR. The presumption is that
 * this is for small embedded use cases where space is a premium and
 * only integer labels will be used.
 *
 * All claims are optional in tokens. This decoder will ignore all
 * CBOR encoded data that it doesn't understand without error.
 *
 * This interface is primarily for the claims defined by Arm for
 * TF-M. It includes only some of the claims from the EAT IETF draft,
 * https://tools.ietf.org/html/draft-mandyam-eat-01.
 *
 * The claims are not described in detail here. That is left to the
 * definition documents and eventually an IETF standard.
 *
 * If a method to get the claim you are interested in doesn't exist,
 * there are several methods where you can give the label (the key)
 * for the claim and have it returned. This only works for simple
 * claims (strings and integers).
 *
 * The entire payload can be retrieved unparsed. Then you can use a
 * separate CBOR parser to decode the claims out of it.  Future work may
 * include more general facilities for handling claims with complex
 * structures made up of maps and arrays.
 *
 * This should not yet be considered a real commercial
 * implementation of token decoding. It is
 * close, but not there yet. It's purpose is to test
 * token encoding. The main thing this needs to become
 * a real commercial implementation is code that
 * tests this. It is a parser / decoder, so a
 * proper test involves a lot of hostile input.
 */


/**
 * The context for decoding an attestation token. The caller of must
 * create one of these and pass it to the functions here. It is small
 * enough that it can go on the stack. It is most of the memory needed
 * to create a token except the output buffer and any memory
 * requirements for the cryptographic operations.
 *
 * The structure is opaque for the caller.
 *
 */
struct attest_token_decode_context {
    /* PRIVATE DATA STRUCTURE. USE ACCESSOR FUNCTIONS. */
    struct q_useful_buf_c   payload;
    uint32_t                options;
    enum attest_token_err_t last_error;
    /* FIXME: This will have to expand when the pub key
       handling functions are implemented */
};


/**
 * \brief Initialize token decoder.
 *
 * \param[in] me      The token decoder context to be initialized.
 * \param[in] options Decoding options.
 *
 * Must be called on a \ref attest_token_decode_context before
 * use. An instance of \ref attest_token_decode_context can
 * be used again by calling this on it again.
 **/
void attest_token_decode_init(struct attest_token_decode_context *me,
                              uint32_t options);



/**
 * \brief Set specific public key to use for verification.
 *
 * \param[in] me           The token decoder context to configure.
 * \param[in] cose_pub_key A CBOR-encoded \c COSE_Key containing
 *                         the public key to use for signature
 *                         verification.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * (This has not been implemented yet)
 *
 * The key type must work with the signing algorithm in the token
 * being verified.
 *
 * The \c kid in the \c COSE_Key must match the one in the token.
 *
 * If there is no kid in the \c COSE_Key it will be used no matter
 * what kid is indicated in the token.
 *
 * Once set, a key can be used for multiple verifications.
 *
 * Calling this again will replace the previous key that was
 * configured. It will also replace the key set by
 * attest_token_decode_set_pub_key_select().
 */
enum attest_token_err_t
attest_token_decode_set_cose_pub_key(struct attest_token_decode_context *me,
                                     struct q_useful_buf cose_pub_key);


/**
 * \brief Set specific public key to use for verification.
 *
 * \param[in] me         The token decoder context to configure.
 * \param[in] key_select Selects the key to verify.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * (This has not been implemented yet)
 *
 * The key type must work with the signing algorithm in the token
 * being verified.
 *
 * The meaning of key_select depends on the platform this is running
 * on.
 *
 * Once set, a key can be used for multiple verifications.
 *
 * Calling this again will replace the previous key that was
 * configured. It will also replace the key set by
 * attest_token_decode_set_cose_pub_key().
 *
 */
enum attest_token_err_t
attest_token_decode_set_pub_key_select(struct attest_token_decode_context *me,
                                       int32_t key_select);


/**
 * \brief Set the token to work on and validate its signature.
 *
 * \param[in] me     The token decoder context to validate with.
 * \param[in] token  The CBOR-encoded token to validate and decode.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The signature on the token is validated. If it is successful the
 * token and its payload is remembered in the \ref
 * attest_token_decode_context \c me so the \c
 * attest_token_decode_get_xxx() functions can be called to get the
 * various claims out of it.
 *
 * Generally, a public key has to be configured for this to work. It
 * can however validate short-circuit signatures even if one is not
 * set.
 *
 * The code for any error that occurs during validation is remembered
 * in decode context. The \c attest_token_decode_get_xxx() functions
 * can be called and they will just return this error. The \c
 * attest_token_decode_get_xxx() functions will generally return 0 or
 * \c NULL if the token is in error.
 *
 * It is thus possible to call attest_token_decode_validate_token()
 * and all the \c attest_token_decode_get_xxx() functions to parse the
 * token and ignore the error codes as long as
 * attest_token_decode_get_error() is called before any of the claim
 * data returned is used.
 */
enum attest_token_err_t
attest_token_decode_validate_token(struct attest_token_decode_context *me,
                                   struct q_useful_buf_c token);


/**
 * \brief Get the last decode error.
 *
 * \param[in] me The token decoder context.
 *
 * \return An error from \ref attest_token_err_t.
 */
static enum attest_token_err_t
attest_token_decode_get_error(struct attest_token_decode_context *me);


/**
 * \brief Get undecoded CBOR payload from the token.
 *
 * \param[in]  me      The token decoder context.
 * \param[out] payload The returned, verified token payload.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * This will return an error if the signature over the payload did not
 * validate.
 *
 * This allows the caller to parse the payload with any CBOR decoder
 * they wish to use. It also an "escape hatch" to get to claims in the
 * token not supported by decoding in this implementation, for example
 * claims that have non-integer labels.
 */
enum attest_token_err_t
attest_token_decode_get_payload(struct attest_token_decode_context *me,
                                struct q_useful_buf_c *payload);


/** Label for bits in \c item_flags in \ref
    attest_token_iat_simple_t */
enum attest_token_item_index_t {
    NONCE_FLAG =              0,
    UEID_FLAG  =              1,
    BOOT_SEED_FLAG =          2,
    HW_VERSION_FLAG =         3,
    IMPLEMENTATION_ID_FLAG =  4,
    CLIENT_ID_FLAG =          5,
    SECURITY_LIFECYCLE_FLAG = 6,
    PROFILE_DEFINITION_FLAG = 7,
    ORIGINATION_FLAG =        8,
    NUMBER_OF_ITEMS =         9
};


/**
 * This structure holds the simple-to-get fields from the
 * token that can be bundled into one structure.
 *
 * This is 7 * 8 + 12 = 72 bytes on a 32-bit machine.
 */
struct attest_token_iat_simple_t {
    struct q_useful_buf_c nonce; /* byte string */
    struct q_useful_buf_c ueid; /* byte string */
    struct q_useful_buf_c boot_seed; /* byte string */
    struct q_useful_buf_c hw_version; /* text string */
    struct q_useful_buf_c implementation_id; /* byte string */
    uint32_t              security_lifecycle;
    int32_t               client_id;
    struct q_useful_buf_c profile_definition; /* text string */
    struct q_useful_buf_c origination; /* text string */
    uint32_t              item_flags;
};


/**
 * Macro to determine if data item is present in \ref
 * attest_token_iat_simple_t
 */
 #define IS_ITEM_FLAG_SET(item_index, item_flags) \
     (((0x01U << (item_index))) & (item_flags))


/**
 * \brief Batch fetch of all simple data items in a token.
 *
 * \param[in]  me     The token decoder context.
 * \param[out] items  Structure into which all found items are placed.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_SUCCESS
 *         Indicates that the token was successfully searched. It
 *         could mean that all the data item were found, only
 *         some were found, or even none were found.
 *
 * This searches the token for the simple unstructured data items all
 * at once. It can be a little more efficient than getting them one by
 * one.
 *
 * Use \ref IS_ITEM_FLAG_SET on \c item_flags in \c
 * attest_token_iat_simple_t to determine if the data item was found or
 * not and whether the corresponding member in the structure is valid.
 */
enum attest_token_err_t
attest_token_decode_get_iat_simple(struct attest_token_decode_context *me,
                                   struct attest_token_iat_simple_t *items);


/**
 \brief Get the nonce out of the token.
 *
 * \param[in]  me     The token decoder context.
 * \param[out] nonce  Returned pointer and length of nonce.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The nonce is a byte string. The nonce is also known as the
 * challenge.
 */
static enum attest_token_err_t
attest_token_decode_get_nonce(struct attest_token_decode_context *me,
                              struct q_useful_buf_c *nonce);


/**
 * \brief Get the boot seed out of the token.
 *
 * \param[in]  me         The token decoder context.
 * \param[out] boot_seed  Returned pointer and length of boot_seed.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The boot seed is a byte string.
 */
static enum attest_token_err_t
attest_token_decode_get_boot_seed(struct attest_token_decode_context *me,
                                  struct q_useful_buf_c *boot_seed);


/**
 * \brief Get the UEID out of the token.
 *
 * \param[in]  me    The token decoder context.
 * \param[out] ueid  Returned pointer and length of ueid.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The UEID is a byte string.
 */
static enum attest_token_err_t
attest_token_decode_get_ueid(struct attest_token_decode_context *me,
                             struct q_useful_buf_c *ueid);



/**
 * \brief Get the HW Version out of the token
 *
 * \param[in]  me          The token decoder context.
 * \param[out] hw_version  Returned pointer and length of
 *                         \c hw_version.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * This is also known as the HW ID.
 *
 * The HW Version is a UTF-8 text string. It is returned as a pointer
 * and length. It is NOT \c NULL terminated.
 */
static enum attest_token_err_t
attest_token_decode_get_hw_version(struct attest_token_decode_context *me,
                                   struct q_useful_buf_c *hw_version);


/**
 * \brief Get the implementation ID out of the token.
 *
 * \param[in]  me                 The token decoder context.
 * \param[out] implementation_id  Returned pointer and length of
 *                                implementation_id.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The implementation ID is a byte string.
 */
static enum attest_token_err_t
attest_token_decode_get_implementation_id(struct attest_token_decode_context*me,
                                      struct q_useful_buf_c *implementation_id);


/**
 * \brief Get the origination out of the token.
 *
 * \param[in]  me           The token decoder context.
 * \param[out] origination  Returned pointer and length of origination.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * This is also known as the Verification Service Indicator.
 *
 * The \c origination is a UTF-8 text string. It is returned as a
 * pointer* and length. It is NOT \c NULL terminated.
 */
static enum attest_token_err_t
attest_token_decode_get_origination(struct attest_token_decode_context *me,
                                    struct q_useful_buf_c *origination);


/**
 * \brief Get the profile definition out of the token.
 *
 * \param[in]  me                  The token decoder context.
 * \param[out] profile_definition  Returned pointer and length of
 *                                 profile_definition.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * The profile definition is a UTF-8 text string. It is returned as a
 * pointer and length. It is NOT \c NULL terminated.
 */
static enum attest_token_err_t
attest_token_decode_get_profile_definition(
                                    struct attest_token_decode_context *me,
                                    struct q_useful_buf_c *profile_definition);


/**
 * \brief Get the client ID out of the token.
 *
 * \param[in]  me         The token decoder context.
 * \param[out] client_id  Returned pointer and length of client_id.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_INTEGER_VALUE
 *         If integer is larger or smaller than will fit
 *         in an \c int32_t.
 *
 * Also called the caller ID.
 */
static enum attest_token_err_t
attest_token_decode_get_client_id(struct attest_token_decode_context *me,
                                  int32_t *client_id);


/**
 * \brief Get the security lifecycle out of the token.
 *
 * \param[in]  me         The token decoder context.
 * \param[out] lifecycle  Returned pointer and length of lifecycle.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_INTEGER_VALUE
 *         If integer is larger
 *         or smaller than will fit in a \c uint32_t.
 */
static enum attest_token_err_t
attest_token_decode_get_security_lifecycle(
    struct attest_token_decode_context *me,
    uint32_t *lifecycle);


/**
 * Use \ref IS_ITEM_FLAG_SET macro with these values and \c
 * attest_token_sw_component_t.item_flags to find out if the
 * data item is filled in in the attest_token_sw_component_t structure.
 *
 * Items that are of type \c struct \c q_useful_buf_c will also be \c
 * NULL_Q_USEFUL_BUF_C when they are absent.
 */
enum attest_token_sw_index_t {
    SW_MEASUREMENT_TYPE_FLAG = 0,
    SW_MEASURMENT_VAL_FLAG = 1,
    /* Reserved: 2 */
    SW_VERSION_FLAG = 3,
    SW_SIGNER_ID_FLAG = 5,
    SW_MEASUREMENT_DESC_FLAG = 6,
};

/**
 * Structure to hold one SW component
 *
 * This is about 50 bytes on a 32-bit machine and 100 on a 64-bit
 * machine.
 *
 * There will probably be an expanded version of this when more is
 * added to describe a SW component.
 */
struct attest_token_sw_component_t {
    struct q_useful_buf_c measurement_type; /* text string */
    struct q_useful_buf_c measurement_val; /* binary string */
    struct q_useful_buf_c version; /* text string */
    struct q_useful_buf_c signer_id; /* binary string */
    struct q_useful_buf_c measurement_desc; /* text string */
    uint32_t              item_flags;
};


/**
 * \brief Get the number of SW components in the token
 *
 * \param[in]  me                The token decoder context.
 * \param[out] num_sw_components The number of SW components in the
 *                               token.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * If there are explicitly no SW components, this will return successfully
 * and the \c num_sw_components will be zero.
 *
 * Per Arm's IAT specification the only two ways this will succeed
 * are.
 * - The SW components array is present and has one or more (not zero)
 * SW components and the "no SW Components" claim is absent.
 * - The "no SW Components" integer claim is present, its value
 * is 1, and the SW Components array is absent.
 */
enum attest_token_err_t
attest_token_get_num_sw_components(struct attest_token_decode_context *me,
                                   uint32_t *num_sw_components);


/**
 * \brief Get the nth SW component.
 *
 * \param[in] me              The token decoder context.
 * \param[in] requested_index Index, from 0 to num_sw_components,
 *                            of request component.
 * \param[out] sw_components  Place to return the details of the
 *                            SW component
 *
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         There were not \c requested_index in the token.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *         The claim labeled to contain SW components is not an array.
 */
enum attest_token_err_t
attest_token_get_sw_component(struct attest_token_decode_context *me,
                       uint32_t requested_index,
                       struct attest_token_sw_component_t *sw_components);


/**
 *
 * \brief Get a top-level claim, by integer label that is a byte
 * string.
 *
 * \param[in]  me    The token decoder context.
 * \param[in]  label The integer label identifying the claim.
 * \param[out] claim The byte string or \c NULL_Q_USEFUL_BUF_C.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         General structure of the token is incorrect, for example
 *         the top level is not a map or some map wasn't closed.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR syntax is wrong and it is not decodable.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *         Returned if the claim is not a byte string.
 *
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         Data item for \c label was not found in token.
 *
 * If an error occurs, the claim will be set to \c NULL_Q_USEFUL_BUF_C
 * and the error state inside \c attest_token_decode_context will
 * be set.
 */
enum attest_token_err_t
attest_token_decode_get_bstr(struct attest_token_decode_context *me,
                             int32_t label,
                             struct q_useful_buf_c *claim);


/**
 * \brief Get a top-level claim, by integer label that is a text
 * string.
 *
 * \param[in] me     The token decoder context.
 * \param[in] label  The integer label identifying the claim.
 * \param[out] claim The byte string or \c NULL_Q_USEFUL_BUF_C.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         General structure of the token is incorrect, for example
 *         the top level is not a map or some map wasn't closed.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR syntax is wrong and it is not decodable.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *         Returned if the claim is not a byte string.
 *
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         Data item for \c label was not found in token.
 *
 * Even though this is a text string, it is not NULL-terminated.
 *
 * If an error occurs, the claim will be set to \c NULL_Q_USEFUL_BUF_C
 * and the error state inside \c attest_token_decode_context will
 * be set.
 */
enum attest_token_err_t
attest_token_decode_get_tstr(struct attest_token_decode_context *me,
                             int32_t label,
                             struct q_useful_buf_c *claim);



/**
 * \brief Get a top-level claim by integer label who's value is a
 * signed integer
 *
 * \param[in]  me    The token decoder context.
 * \param[in]  label The integer label identifying the claim.
 * \param[out] claim The signed integer or 0.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         General structure of the token is incorrect, for example
 *         the top level is not a map or some map wasn't closed.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR syntax is wrong and it is not decodable.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *         Returned if the claim is not a byte string.
 *
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         Data item for \c label was not found in token.
 *
 * \retval ATTEST_TOKEN_ERR_INTEGER_VALUE
 *         Returned if the integer value is larger
 *         than \c INT64_MAX.
 *
 * This will succeed if the CBOR type of the claim is either a
 * positive or negative integer as long as the value is between \c
 * INT64_MIN and \c INT64_MAX.
 *
 * See also attest_token_decode_get_uint().
 *
 * If an error occurs the value 0 will be returned and the error
 * inside the \c attest_token_decode_context will be set.
 */
enum attest_token_err_t
attest_token_decode_get_int(struct attest_token_decode_context *me,
                            int32_t label,
                            int64_t *claim);


/**
 * \brief Get a top-level claim by integer label who's value is an
 * unsigned integer
 *
 * \param[in]  me    The token decoder context.
 * \param[in]  label The integer label identifying the claim.
 * \param[out] claim The unsigned integer or 0.
 *
 * \return An error from \ref attest_token_err_t.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_STRUCTURE
 *         General structure of the token is incorrect, for example
 *         the top level is not a map or some map wasn't closed.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED
 *         CBOR syntax is wrong and it is not decodable.
 *
 * \retval ATTEST_TOKEN_ERR_CBOR_TYPE
 *         Returned if the claim is not a byte string.
 *
 * \retval ATTEST_TOKEN_ERR_NOT_FOUND
 *         Data item for \c label was not found in token.
 *
 * \retval ATTEST_TOKEN_ERR_INTEGER_VALUE
 *         Returned if the integer value is negative.
 *
 * This will succeed if the CBOR type of the claim is either a
 * positive or negative integer as long as the value is between 0 and
 * \c MAX_UINT64.
 *
 * See also attest_token_decode_get_int().
 *
 *  If an error occurs the value 0 will be returned and the error
 *  inside the \c attest_token_decode_context will be set.
 */
enum attest_token_err_t
attest_token_decode_get_uint(struct attest_token_decode_context *me,
                             int32_t label,
                             uint64_t *claim);




/* ====================================================================
 *   Inline Implementations
 *   Typically, these are small and called only once.
 * ==================================================================== */

static inline enum attest_token_err_t
attest_token_decode_get_error(struct attest_token_decode_context *me)
{
    return me->last_error;
}


static inline enum attest_token_err_t
attest_token_decode_get_nonce(struct attest_token_decode_context *me,
                              struct q_useful_buf_c *nonce)
{
    return attest_token_decode_get_bstr(me,
                                        EAT_CBOR_ARM_LABEL_CHALLENGE,
                                        nonce);
}


static inline enum attest_token_err_t
attest_token_decode_get_ueid(struct attest_token_decode_context *me,
                             struct q_useful_buf_c *ueid)
{
    return attest_token_decode_get_bstr(me, EAT_CBOR_ARM_LABEL_UEID, ueid);
}


static inline enum attest_token_err_t
attest_token_decode_get_boot_seed(struct attest_token_decode_context *me,
                                  struct q_useful_buf_c *boot_seed)
{
    return attest_token_decode_get_bstr(me,
                                        EAT_CBOR_ARM_LABEL_BOOT_SEED,
                                        boot_seed);
}


static inline enum attest_token_err_t
attest_token_decode_get_hw_version(struct attest_token_decode_context *me,
                                   struct q_useful_buf_c *hw_version)
{
    return attest_token_decode_get_tstr(me,
                                        EAT_CBOR_ARM_LABEL_HW_VERSION,
                                        hw_version);
}


static inline enum attest_token_err_t
attest_token_decode_get_implementation_id(
                                       struct attest_token_decode_context *me,
                                       struct q_useful_buf_c*implementation_id)
{
    return attest_token_decode_get_bstr(me,
                                        EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID,
                                        implementation_id);
}


static inline enum attest_token_err_t
attest_token_decode_get_client_id(struct attest_token_decode_context *me,
                                  int32_t *caller_id)
{
    enum attest_token_err_t return_value;
    int64_t caller_id_64;

    return_value = attest_token_decode_get_int(me,
                                               EAT_CBOR_ARM_LABEL_CLIENT_ID,
                                               &caller_id_64);
    if(return_value != ATTEST_TOKEN_ERR_SUCCESS) {
        goto Done;
    }
    if(caller_id_64 > INT32_MAX || caller_id_64 < INT32_MIN) {
        return_value = ATTEST_TOKEN_ERR_INTEGER_VALUE;
        goto Done;
    }
    *caller_id = (int32_t)caller_id_64;

Done:
    return return_value;
}


static inline enum attest_token_err_t
attest_token_decode_get_security_lifecycle(
                                 struct attest_token_decode_context *me,
                                 uint32_t *security_lifecycle)
{
    enum attest_token_err_t return_value;
    uint64_t security_lifecycle_64;

    return_value = attest_token_decode_get_uint(me,
                                       EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE,
                                       &security_lifecycle_64);
    if(security_lifecycle_64 > UINT32_MAX) {
        return_value = ATTEST_TOKEN_ERR_INTEGER_VALUE;
        goto Done;
    }

    *security_lifecycle = (uint32_t)security_lifecycle_64;

Done:
    return return_value;
}

static inline enum attest_token_err_t
attest_token_decode_get_profile_definition(
                                    struct attest_token_decode_context *me,
                                    struct q_useful_buf_c *profile_definition)
{
    return attest_token_decode_get_tstr(me,
                                        EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION,
                                        profile_definition);
}

static inline enum attest_token_err_t
attest_token_decode_get_origination(struct attest_token_decode_context*me,
                                    struct q_useful_buf_c *origination)
{
    return attest_token_decode_get_tstr(me,
                                        EAT_CBOR_ARM_LABEL_ORIGINATION,
                                        origination);
}


#ifdef __cplusplus
}
#endif


#endif /* __ATTEST_TOKEN_DECODE_H__ */
