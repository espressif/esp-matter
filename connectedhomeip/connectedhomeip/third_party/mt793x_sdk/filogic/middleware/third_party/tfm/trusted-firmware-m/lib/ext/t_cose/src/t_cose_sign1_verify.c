/*
 *  t_cose_sign1_verify.c
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#include "t_cose_sign1_verify.h"
#include "qcbor.h"
#include "t_cose_crypto.h"
#include "q_useful_buf.h"
#include "t_cose_util.h"
#include "t_cose_parameters.h"


/**
 * \file t_cose_sign1_verify.c
 *
 * \brief \c COSE_Sign1 verification implementation.
 */



#ifndef T_COSE_DISABLE_SHORT_CIRCUIT_SIGN
/**
 *  \brief Verify a short-circuit signature
 *
 * \param[in] hash_to_verify  Pointer and length of hash to verify.
 * \param[in] signature       Pointer and length of signature.
 *
 * \return This returns one of the error codes defined by \ref
 *         t_cose_err_t.
 *
 * See t_cose_sign1_sign_init() for description of the short-circuit
 * signature.
 */
static inline enum t_cose_err_t
t_cose_crypto_short_circuit_verify(struct q_useful_buf_c hash_to_verify,
                                   struct q_useful_buf_c signature)
{
    struct q_useful_buf_c hash_from_sig;
    enum t_cose_err_t     return_value;

    hash_from_sig = q_useful_buf_head(signature, hash_to_verify.len);
    if(q_useful_buf_c_is_null(hash_from_sig)) {
        return_value = T_COSE_ERR_SIG_VERIFY;
        goto Done;
    }

    if(q_useful_buf_compare(hash_from_sig, hash_to_verify)) {
        return_value = T_COSE_ERR_SIG_VERIFY;
    } else {
        return_value = T_COSE_SUCCESS;
    }

Done:
    return return_value;
}
#endif /* T_COSE_DISABLE_SHORT_CIRCUIT_SIGN */


/*
 * Public function. See t_cose_sign1_verify.h
 */
enum t_cose_err_t
t_cose_sign1_verify(struct t_cose_sign1_verify_ctx *me,
                    struct q_useful_buf_c           cose_sign1,
                    struct q_useful_buf_c          *payload,
                    struct t_cose_parameters       *parameters)
{
    /* Stack use for 32-bit CPUs:
     *   268 for local except hash output
     *   32 to 64 local for hash output
     *   220 to 434 to make TBS hash
     * Total 420 to 768 depending on hash and EC alg.
     * Stack used internally by hash and crypto is extra.
     */
    QCBORDecodeContext            decode_context;
    QCBORItem                     item;
    struct q_useful_buf_c         protected_parameters;
    enum t_cose_err_t             return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(   buffer_for_tbs_hash, T_COSE_CRYPTO_MAX_HASH_SIZE);
    struct q_useful_buf_c         tbs_hash;
    struct q_useful_buf_c         signature;
    struct t_cose_parameters      unprotected_parameters;
    struct t_cose_parameters      parsed_protected_parameters;
    struct t_cose_label_list      critical_labels;
    struct t_cose_label_list      unknown_labels;
#ifndef T_COSE_DISABLE_SHORT_CIRCUIT_SIGN
    struct q_useful_buf_c         short_circuit_kid;
#endif

    *payload = NULL_Q_USEFUL_BUF_C;

    QCBORDecode_Init(&decode_context, cose_sign1, QCBOR_DECODE_MODE_NORMAL);
    /* Calls to QCBORDecode_GetNext() rely on item.uDataType != QCBOR_TYPE_ARRAY
     * to detect decoding errors rather than checking the return code.
     */

    /* --  The array of four -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_ARRAY) {
        return_value = T_COSE_ERR_SIGN1_FORMAT;
        goto Done;
    }

    if((me->option_flags & T_COSE_OPT_TAG_REQUIRED) &&
       !QCBORDecode_IsTagged(&decode_context, &item, CBOR_TAG_COSE_SIGN1)) {
        return_value = T_COSE_ERR_INCORRECTLY_TAGGED;
        goto Done;
    }

    /* -- Clear list where uknown labels are accumulated -- */
    clear_label_list(&unknown_labels);


    /* --  Get the protected header parameters -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_SIGN1_FORMAT;
        goto Done;
    }

    protected_parameters = item.val.string;

    return_value = parse_protected_header_parameters(protected_parameters,
                                                    &parsed_protected_parameters,
                                                    &critical_labels,
                                                    &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }


    /* --  Get the unprotected parameters -- */
    return_value = parse_unprotected_header_parameters(&decode_context,
                                                       &unprotected_parameters,
                                                       &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }
    if((me->option_flags & T_COSE_OPT_REQUIRE_KID) &&
       q_useful_buf_c_is_null(unprotected_parameters.kid)) {
        return_value = T_COSE_ERR_NO_KID;
        goto Done;
    }


    /* -- Check critical parameter labels -- */
    return_value = check_critical_labels(&critical_labels, &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }

    /* -- Check for duplicate parameters and copy to returned parameters -- */
    return_value = check_and_copy_parameters(&parsed_protected_parameters,
                                             &unprotected_parameters,
                                              parameters);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }


    /* -- Get the payload -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_SIGN1_FORMAT;
        goto Done;
    }
    *payload = item.val.string;


    /* -- Get the signature -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_SIGN1_FORMAT;
        goto Done;
    }
    signature = item.val.string;


    /* -- Finish up the CBOR decode -- */
    /* This check make sure the array only had the expected four
     * items. Works for definite and indefinte length arrays. Also
     * make sure there were no extra bytes. */
    if(QCBORDecode_Finish(&decode_context) != QCBOR_SUCCESS) {
        return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
        goto Done;
    }


    /* -- Skip signature verification if such is requested --*/
    if(me->option_flags & T_COSE_OPT_DECODE_ONLY) {
        return_value = T_COSE_SUCCESS;
        goto Done;
    }


    /* -- Compute the TBS bytes -- */
    return_value = create_tbs_hash(parsed_protected_parameters.cose_algorithm_id,
                                   protected_parameters,
                                   T_COSE_TBS_BARE_PAYLOAD,
                                   *payload,
                                   buffer_for_tbs_hash,
                                   &tbs_hash);
    if(return_value) {
        goto Done;
    }


    /* -- Check for short-circuit signature and verify if it exists -- */
#ifndef T_COSE_DISABLE_SHORT_CIRCUIT_SIGN
    short_circuit_kid = get_short_circuit_kid();
    if(!q_useful_buf_compare(unprotected_parameters.kid, short_circuit_kid)) {
        if(!(me->option_flags & T_COSE_OPT_ALLOW_SHORT_CIRCUIT)) {
            return_value = T_COSE_ERR_SHORT_CIRCUIT_SIG;
            goto Done;
        }

        return_value = t_cose_crypto_short_circuit_verify(tbs_hash, signature);
        goto Done;
    }
#endif /* T_COSE_DISABLE_SHORT_CIRCUIT_SIGN */


    /* -- Verify the signature (if it wasn't short-circuit) -- */
    return_value = t_cose_crypto_pub_key_verify(parsed_protected_parameters.cose_algorithm_id,
                                                me->verification_key,
                                                unprotected_parameters.kid,
                                                tbs_hash,
                                                signature);

Done:
    return return_value;
}
