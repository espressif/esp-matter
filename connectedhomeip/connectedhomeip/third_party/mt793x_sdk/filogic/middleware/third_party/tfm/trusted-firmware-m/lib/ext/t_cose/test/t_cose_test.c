/*
 *  t_cose_test.c
 *
 * Copyright 2019-2020, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "t_cose_test.h"
#include "t_cose_sign1_sign.h"
#include "t_cose_sign1_verify.h"
#include "t_cose_make_test_messages.h"
#include "q_useful_buf.h"
#include "t_cose_crypto.h" /* For signature size constant */
#include "t_cose_util.h" /* for get_short_circuit_kid */


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_self_test()
{
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           signed_cose;
    struct q_useful_buf_c           payload;


    /* --- Make COSE Sign1 object --- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    /* No key necessary because short-circuit test mode is used */

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &signed_cose);
    if(return_value) {
        return 1000 + return_value;
    }
    /* --- Done making COSE Sign1 object  --- */


    /* --- Start verifying the COSE Sign1 object  --- */
    /* Select short circuit signing */
    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    /* No key necessary with short circuit */

    /* Run the signature verification */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       signed_cose,
                                       /* The returned payload */
                                       &payload,
                                       /* Don't return parameters */
                                       NULL);
    if(return_value) {
        return 2000 + return_value;
    }

    /* compare payload output to the one expected */
    if(q_useful_buf_compare(payload, Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"))) {
        return 3000;
    }
    /* --- Done verifying the COSE Sign1 object  --- */

    return 0;
}


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_verify_fail_test()
{
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           signed_cose;
    struct q_useful_buf_c           payload;
    size_t                          payload_offset;

    /* --- Start making COSE Sign1 object  --- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    /* No key necessary because short-circuit test mode is used */

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &signed_cose);
    if(return_value) {
        return 1000 + return_value;
    }
    /* --- Done making COSE Sign1 object  --- */


    /* --- Start Tamper with payload  --- */
    /* Find the offset of the payload in COSE_Sign1 */
    payload_offset = q_useful_buf_find_bytes(signed_cose, Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"));
    if(payload_offset == SIZE_MAX) {
        return 6000;
    }
    /* Change "payload" to "hayload" */
    ((char *)signed_cose.ptr)[payload_offset] = 'h';
    /* --- Tamper with payload Done --- */


    /* --- Start verifying the COSE Sign1 object  --- */

    /* Select short circuit signing */
    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    /* No key necessary with short circuit */

    /* Run the signature verification */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       signed_cose,
                                       /* The returned payload */
                                       &payload,
                                       /* Don't return parameters */
                                       NULL);
    if(return_value != T_COSE_ERR_SIG_VERIFY) {
        return 4000 + return_value;
    }
    /* --- Done verifying the COSE Sign1 object  --- */

    return 0;
}


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_signing_error_conditions_test()
{
    struct t_cose_sign1_sign_ctx sign_ctx;
    QCBOREncodeContext           cbor_encode;
    enum t_cose_err_t            return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(  signed_cose_buffer, 300);
    Q_USEFUL_BUF_MAKE_STACK_UB(  small_signed_cose_buffer, 15);
    struct q_useful_buf_c        signed_cose;


    /* -- Test bad algorithm ID 0 -- */
    /* Use reserved alg ID 0 to cause error. */
    t_cose_sign1_sign_init(&sign_ctx, T_COSE_OPT_SHORT_CIRCUIT_SIG, 0);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &signed_cose);
    if(return_value != T_COSE_ERR_UNSUPPORTED_SIGNING_ALG) {
        return -1;
    }


    /* -- Test bad algorithm ID -4444444 -- */
    /* Use unassigned alg ID -4444444 to cause error. */
    t_cose_sign1_sign_init(&sign_ctx, T_COSE_OPT_SHORT_CIRCUIT_SIG, -4444444);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &signed_cose);
    if(return_value != T_COSE_ERR_UNSUPPORTED_SIGNING_ALG) {
        return -2;
    }



    /* -- Tests detection of CBOR encoding error in the payload -- */
    QCBOREncode_Init(&cbor_encode, signed_cose_buffer);

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);
    return_value = t_cose_sign1_encode_parameters(&sign_ctx, &cbor_encode);


    QCBOREncode_AddSZString(&cbor_encode, "payload");
    /* Force a CBOR encoding error by closing a map that is not open */
    QCBOREncode_CloseMap(&cbor_encode);

    return_value = t_cose_sign1_encode_signature(&sign_ctx, &cbor_encode);

    if(return_value != T_COSE_ERR_CBOR_FORMATTING) {
        return -3;
    }


    /* -- Tests the output buffer being too small -- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     small_signed_cose_buffer,
                                     &signed_cose);

    if(return_value != T_COSE_ERR_TOO_SMALL) {
        return -4;
    }

    return 0;
}


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_make_cwt_test()
{
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;
    QCBOREncodeContext              cbor_encode;
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           signed_cose;
    struct q_useful_buf_c           payload;
    QCBORError                      cbor_error;

    /* --- Start making COSE Sign1 object  --- */

    /* The CBOR encoder instance that the COSE_Sign1 is output into */
    QCBOREncode_Init(&cbor_encode, signed_cose_buffer);

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    /* Do the first part of the the COSE_Sign1, the parameters */
    return_value = t_cose_sign1_encode_parameters(&sign_ctx, &cbor_encode);
    if(return_value) {
        return 1000 + return_value;
    }

    QCBOREncode_OpenMap(&cbor_encode);
    QCBOREncode_AddSZStringToMapN(&cbor_encode, 1, "coap://as.example.com");
    QCBOREncode_AddSZStringToMapN(&cbor_encode, 2, "erikw");
    QCBOREncode_AddSZStringToMapN(&cbor_encode, 3, "coap://light.example.com");
    QCBOREncode_AddInt64ToMapN(&cbor_encode, 4, 1444064944);
    QCBOREncode_AddInt64ToMapN(&cbor_encode, 5, 1443944944);
    QCBOREncode_AddInt64ToMapN(&cbor_encode, 6, 1443944944);
    const uint8_t xx[] = {0x0b, 0x71};
    QCBOREncode_AddBytesToMapN(&cbor_encode, 7, Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(xx));
    QCBOREncode_CloseMap(&cbor_encode);

    /* Finish up the COSE_Sign1. This is where the signing happens */
    return_value = t_cose_sign1_encode_signature(&sign_ctx, &cbor_encode);
    if(return_value) {
        return 2000 + return_value;
    }

    /* Finally close off the CBOR formatting and get the pointer and length
     * of the resulting COSE_Sign1
     */
    cbor_error = QCBOREncode_Finish(&cbor_encode, &signed_cose);
    if(cbor_error) {
        return 3000 + cbor_error;
    }
    /* --- Done making COSE Sign1 object  --- */


    /* --- Compare to expected from CWT RFC --- */
    /* The first part, the intro and protected pararameters must be the same */
    const uint8_t cwt_first_part_bytes[] = {0xd2, 0x84, 0x43, 0xa1, 0x01, 0x26};
    struct q_useful_buf_c fp = Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(cwt_first_part_bytes);
    struct q_useful_buf_c head = q_useful_buf_head(signed_cose, sizeof(cwt_first_part_bytes));
    if(q_useful_buf_compare(head, fp)) {
        return -1;
    }

    /* Skip the key id, because this has the short-circuit key id */
    const size_t kid_encoded_len =
       1 +
       1 +
       2 +
       32; // length of short-circuit key id

    /* Compare the payload */
    const uint8_t rfc8392_payload_bytes[] = {
        0x58, 0x50, 0xa7, 0x01, 0x75, 0x63, 0x6f, 0x61, 0x70, 0x3a, 0x2f,
        0x2f, 0x61, 0x73, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65,
        0x2e, 0x63, 0x6f, 0x6d, 0x02, 0x65, 0x65, 0x72, 0x69, 0x6b, 0x77,
        0x03, 0x78, 0x18, 0x63, 0x6f, 0x61, 0x70, 0x3a, 0x2f, 0x2f, 0x6c,
        0x69, 0x67, 0x68, 0x74, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c,
        0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x04, 0x1a, 0x56, 0x12, 0xae, 0xb0,
        0x05, 0x1a, 0x56, 0x10, 0xd9, 0xf0, 0x06, 0x1a, 0x56, 0x10, 0xd9,
        0xf0, 0x07, 0x42, 0x0b, 0x71};

    struct q_useful_buf_c fp2 = Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(rfc8392_payload_bytes);

    struct q_useful_buf_c payload2 = q_useful_buf_tail(signed_cose,
                                                       sizeof(cwt_first_part_bytes)+kid_encoded_len);
    struct q_useful_buf_c pl3 = q_useful_buf_head(payload2,
                                                sizeof(rfc8392_payload_bytes));
    if(q_useful_buf_compare(pl3, fp2)) {
        return -2;
    }

    /* Skip the signature because ECDSA signatures usually have a random
     component */


    /* --- Start verifying the COSE Sign1 object  --- */
    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    /* No key necessary with short circuit */

    /* Run the signature verification */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       signed_cose,
                                       /* The returned payload */
                                       &payload,
                                       /* Don't return parameters */
                                       NULL);
    if(return_value) {
        return 4000 + return_value;
    }

    /* Format the expected payload CBOR fragment */

    /* compare payload output to the one expected */
    if(q_useful_buf_compare(payload, q_useful_buf_tail(fp2, 2))) {
        return 5000;
    }
    /* --- Done verifying the COSE Sign1 object  --- */

    return 0;
}


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_decode_only_test()
{
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;
    QCBOREncodeContext              cbor_encode;
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           signed_cose;
    struct q_useful_buf_c           payload;
    Q_USEFUL_BUF_MAKE_STACK_UB(     expected_payload_buffer, 10);
    struct q_useful_buf_c           expected_payload;
    QCBORError                      cbor_error;

    /* --- Start making COSE Sign1 object  --- */

    /* The CBOR encoder instance that the COSE_Sign1 is output into */
    QCBOREncode_Init(&cbor_encode, signed_cose_buffer);

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    /* Do the first part of the the COSE_Sign1, the parameters */
    return_value = t_cose_sign1_encode_parameters(&sign_ctx, &cbor_encode);
    if(return_value) {
        return 1000 + return_value;
    }


    QCBOREncode_AddSZString(&cbor_encode, "payload");

    /* Finish up the COSE_Sign1. This is where the signing happens */
    return_value = t_cose_sign1_encode_signature(&sign_ctx, &cbor_encode);
    if(return_value) {
        return 2000 + return_value;
    }

    /* Finally close of the CBOR formatting and get the pointer and length
     * of the resulting COSE_Sign1
     */
    cbor_error = QCBOREncode_Finish(&cbor_encode, &signed_cose);
    if(cbor_error) {
        return 3000 + cbor_error;
    }
    /* --- Done making COSE Sign1 object  --- */

    /* -- Tweak signature bytes -- */
    /* The signature is the last thing so reach back that many bytes and tweak
       so if signature verification were attempted, it would fail */
    const size_t last_byte_offset = signed_cose.len - T_COSE_EC_P256_SIG_SIZE;
    ((uint8_t *)signed_cose.ptr)[last_byte_offset] += 1;


    /* --- Start verifying the COSE Sign1 object  --- */
    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_DECODE_ONLY);

    /* No key necessary with short circuit */

    /* Run the signature verification */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       signed_cose,
                                       /* The returned payload */
                                       &payload,
                                       /* Don't return parameters */
                                       NULL);


    if(return_value) {
        return 4000 + return_value;
    }

    /* Format the expected payload CBOR fragment */
    QCBOREncode_Init(&cbor_encode, expected_payload_buffer);
    QCBOREncode_AddSZString(&cbor_encode, "payload");
    QCBOREncode_Finish(&cbor_encode, &expected_payload);

    /* compare payload output to the one expected */
    if(q_useful_buf_compare(payload, expected_payload)) {
        return 5000;
    }
    /* --- Done verifying the COSE Sign1 object  --- */

    return 0;
}


/*
 18( [
    / protected / h’a10126’ / {
        \ alg \ 1:-7 \ ECDSA 256 \
    }/ ,
    / unprotected / {
      / kid / 4:’11’
    },
    / payload / ’This is the content.’,

       / signature / h’8eb33e4ca31d1c465ab05aac34cc6b23d58fef5c083106c4
   d25a91aef0b0117e2af9a291aa32e14ab834dc56ed2a223444547e01f11d3b0916e5
   a4c345cacb36’
] )

 */

/* This comes from Appendix_C_2_1.json from COSE_C by Jim Schaad */
static const uint8_t rfc8152_example_2_1[] = {
    0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA1, 0x04,
    0x42, 0x31, 0x31, 0x54, 0x54, 0x68, 0x69, 0x73,
    0x20, 0x69, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2E, /* end of hdrs and payload*/
    0x58, 0x40, 0x8E, 0xB3, 0x3E, 0x4C, 0xA3, 0x1D, /* Sig starts with 0x58 */
    0x1C, 0x46, 0x5A, 0xB0, 0x5A, 0xAC, 0x34, 0xCC,
    0x6B, 0x23, 0xD5, 0x8F, 0xEF, 0x5C, 0x08, 0x31,
    0x06, 0xC4, 0xD2, 0x5A, 0x91, 0xAE, 0xF0, 0xB0,
    0x11, 0x7E, 0x2A, 0xF9, 0xA2, 0x91, 0xAA, 0x32,
    0xE1, 0x4A, 0xB8, 0x34, 0xDC, 0x56, 0xED, 0x2A,
    0x22, 0x34, 0x44, 0x54, 0x7E, 0x01, 0xF1, 0x1D,
    0x3B, 0x09, 0x16, 0xE5, 0xA4, 0xC3, 0x45, 0xCA,
    0xCB, 0x36};


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t cose_example_test()
{
    enum t_cose_err_t             return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(   signed_cose_buffer, 200);
    struct q_useful_buf_c         output;
    struct t_cose_sign1_sign_ctx  sign_ctx;
    struct q_useful_buf_c         head_actual;
    struct q_useful_buf_c         head_exp;

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    t_cose_sign1_set_signing_key(&sign_ctx,
                                 T_COSE_NULL_KEY,
                                 Q_USEFUL_BUF_FROM_SZ_LITERAL("11"));

    /* Make example C.2.1 from RFC 8152 */

    return_value = t_cose_sign1_sign(&sign_ctx,
                                      Q_USEFUL_BUF_FROM_SZ_LITERAL("This is the content."),
                                      signed_cose_buffer,
                                     &output);

    if(return_value != T_COSE_SUCCESS) {
        return return_value;
    }

    /* Compare only the headers and payload as this was not signed
     * with the same key as the example. The first 32 bytes contain
     * the header parameters and payload. */
    head_actual = q_useful_buf_head(output, 32);
    head_exp = q_useful_buf_head(Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(rfc8152_example_2_1), 32);

    if(q_useful_buf_compare(head_actual, head_exp)) {
        return -1000;
    }

    return return_value;
}


static enum t_cose_err_t run_test_sign_and_verify(uint32_t test_mess_options)
{
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;
    QCBOREncodeContext              cbor_encode;
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           signed_cose;
    struct q_useful_buf_c           payload;

    /* --- Start making COSE Sign1 object  --- */

    /* The CBOR encoder instance that the COSE_Sign1 is output into */
    QCBOREncode_Init(&cbor_encode, signed_cose_buffer);

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    return_value =
        t_cose_test_message_sign1_sign(&sign_ctx,
                                       test_mess_options,
                                       Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                       signed_cose_buffer,
                                       &signed_cose);
    if(return_value) {
        return ((enum t_cose_err_t) (2000 + return_value));
    }
    /* --- Done making COSE Sign1 object  --- */


    /* --- Start verifying the COSE Sign1 object  --- */
    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    /* No key necessary with short circuit */


    /* Run the signature verification */
    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       signed_cose,
                                       /* The returned payload */
                                       &payload,
                                       /* Don't return parameters */
                                       NULL);

    return return_value;
}



#ifdef T_COSE_DISABLE_SHORT_CIRCUIT_SIGN
/* copied from t_cose_util.c so these tests that depend on
 * short circuit signatures can run even when it is
 * is disabled.  TODO: is this dependency real?*/

/* This is a random hard coded key ID that is used to indicate
 * short-circuit signing. It is OK to hard code this as the
 * probability of collision with this ID is very low and the same
 * as for collision between any two key IDs of any sort.
 */

static const uint8_t defined_short_circuit_kid[] = {
    0xef, 0x95, 0x4b, 0x4b, 0xd9, 0xbd, 0xf6, 0x70,
    0xd0, 0x33, 0x60, 0x82, 0xf5, 0xef, 0x15, 0x2a,
    0xf8, 0xf3, 0x5b, 0x6a, 0x6c, 0x00, 0xef, 0xa6,
    0xa9, 0xa7, 0x1f, 0x49, 0x51, 0x7e, 0x18, 0xc6};

static struct q_useful_buf_c ss_kid;


/*
 * Public function. See t_cose_util.h
 */
static struct q_useful_buf_c get_short_circuit_kid(void)
{
    ss_kid.len = sizeof(defined_short_circuit_kid);
    ss_kid.ptr = defined_short_circuit_kid;

    return ss_kid;
}
#endif /* T_COSE_DISABLE_SHORT_CIRCUIT_SIGN */

int_fast32_t all_header_parameters_test()
{
    enum t_cose_err_t               return_value;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 300);
    struct q_useful_buf_c           output;
    struct q_useful_buf_c           payload;
    struct t_cose_parameters        parameters;
    struct t_cose_sign1_sign_ctx    sign_ctx;
    struct t_cose_sign1_verify_ctx  verify_ctx;


    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    t_cose_sign1_set_signing_key(&sign_ctx,
                                 T_COSE_NULL_KEY,
                                 Q_USEFUL_BUF_FROM_SZ_LITERAL("11"));

    return_value =
        t_cose_test_message_sign1_sign(&sign_ctx,
                                       T_COSE_TEST_ALL_PARAMETERS,
                                       Q_USEFUL_BUF_FROM_SZ_LITERAL("This is the content."),
                                       signed_cose_buffer,
                                      &output);
    if(return_value) {
        return 1;
    }

    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    /* No key necessary with short circuit */


    return_value = t_cose_sign1_verify(&verify_ctx,
                                       /* COSE to verify */
                                       output,
                                       /* The returned payload */
                                       &payload,
                                       /* Get parameters for checking */
                                       &parameters);

    // Need to compare to short circuit kid
    if(q_useful_buf_compare(parameters.kid, get_short_circuit_kid())) {
        return 2;
    }

    if(parameters.cose_algorithm_id != T_COSE_ALGORITHM_ES256) {
        return 3;
    }

#ifndef T_COSE_DISABLE_CONTENT_TYPE
    if(parameters.content_type_uint != 1) {
        return 4;
    }
#endif /* T_COSE_DISABLE_CONTENT_TYPE */

    if(q_useful_buf_compare(parameters.iv, Q_USEFUL_BUF_FROM_SZ_LITERAL("iv"))) {
        return 5;
    }

    if(q_useful_buf_compare(parameters.partial_iv, Q_USEFUL_BUF_FROM_SZ_LITERAL("partial_iv"))) {
        return 6;
    }

    return 0;
}

struct test_case {
    uint32_t test_option;
    enum t_cose_err_t   result;
};

static struct test_case bad_parameters_tests_table[] = {
    /* Test existance of the critical header. Also makes sure that
     * it works with the max number of labels allowed in it.
     */
    {T_COSE_TEST_EMPTY_PROTECTED_PARAMETERS, T_COSE_ERR_UNSUPPORTED_HASH},

    {T_COSE_TEST_DUP_CONTENT_ID, T_COSE_ERR_DUPLICATE_PARAMETER},

    {T_COSE_TEST_UNCLOSED_PROTECTED, T_COSE_ERR_CBOR_NOT_WELL_FORMED},

    {T_COSE_TEST_TOO_LARGE_CONTENT_TYPE, T_COSE_ERR_BAD_CONTENT_TYPE},

    /* This makes consume_item() error out */
    {T_COSE_TEST_NOT_WELL_FORMED_2, T_COSE_ERR_CBOR_NOT_WELL_FORMED},

    {T_COSE_TEST_KID_IN_PROTECTED, T_COSE_ERR_DUPLICATE_PARAMETER},

    {T_COSE_TEST_TOO_MANY_UNKNOWN, T_COSE_ERR_TOO_MANY_PARAMETERS},

    {T_COSE_TEST_UNPROTECTED_NOT_MAP, T_COSE_ERR_PARAMETER_CBOR},

    {T_COSE_TEST_BAD_CRIT_PARAMETER, T_COSE_ERR_PARAMETER_NOT_PROTECTED},

    {T_COSE_TEST_BAD_CRIT_PARAMETER, T_COSE_ERR_PARAMETER_NOT_PROTECTED},

    {T_COSE_TEST_NOT_WELL_FORMED_1, T_COSE_ERR_CBOR_NOT_WELL_FORMED},

    {T_COSE_TEST_NO_UNPROTECTED_PARAMETERS, T_COSE_ERR_PARAMETER_CBOR},

    {T_COSE_TEST_NO_PROTECTED_PARAMETERS, T_COSE_ERR_SIGN1_FORMAT},

    {T_COSE_TEST_EXTRA_PARAMETER, T_COSE_SUCCESS},

    {T_COSE_TEST_PARAMETER_LABEL, T_COSE_ERR_PARAMETER_CBOR},

    {T_COSE_TEST_BAD_PROTECTED, T_COSE_ERR_PARAMETER_CBOR},

    {0, T_COSE_SUCCESS}
};


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t bad_parameters_test()
{
    struct test_case *test;

    for(test = bad_parameters_tests_table; test->test_option; test++) {
        if(run_test_sign_and_verify(test->test_option) != test->result) {
            return (int_fast32_t)(test - bad_parameters_tests_table);
        }
    }

    return 0;
}




static struct test_case crit_tests_table[] = {
    /* Test existance of the critical header. Also makes sure that
     * it works with the max number of labels allowed in it.
     */
    {T_COSE_TEST_CRIT_PARAMETER_EXIST, T_COSE_SUCCESS},

    /* Exceed the max number of labels by one and get an error */
    {T_COSE_TEST_TOO_MANY_CRIT_PARAMETER_EXIST, T_COSE_ERR_CRIT_PARAMETER},

    /* A critical parameter exists in the protected section, but the
     * format of the internals of this parameter is not the expected CBOR
     */
    {T_COSE_TEST_BAD_CRIT_LABEL, T_COSE_ERR_CRIT_PARAMETER},

    /* A critical label is listed in the protected section, but
     * the label doesn't exist. This works for integer-labeled header params.
     */
    {T_COSE_TEST_UNKNOWN_CRIT_UINT_PARAMETER, T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER},

    /* A critical label is listed in the protected section, but
     * the label doesn't exist. This works for string-labeled header params.
     */
    {T_COSE_TEST_UNKNOWN_CRIT_TSTR_PARAMETER, T_COSE_ERR_UNKNOWN_CRITICAL_PARAMETER},

    /* The critical labels list is not protected */
    {T_COSE_TEST_CRIT_NOT_PROTECTED, T_COSE_ERR_PARAMETER_NOT_PROTECTED},

    {T_COSE_TEST_EMPTY_CRIT_PARAMETER, T_COSE_ERR_CRIT_PARAMETER},

    {T_COSE_TEST_TOO_MANY_TSTR_CRIT_LABLELS, T_COSE_ERR_CRIT_PARAMETER},

    {0, T_COSE_SUCCESS}
};


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t crit_parameters_test()
{
    struct test_case *test;

    for(test = crit_tests_table; test->test_option; test++) {
        if(run_test_sign_and_verify(test->test_option) != test->result) {
            return (int_fast32_t)(test - crit_tests_table);
        }
    }

    return 0;
}


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t content_type_test()
{
#ifndef T_COSE_DISABLE_CONTENT_TYPE

    struct t_cose_parameters        parameters;
    struct t_cose_sign1_sign_ctx    sign_ctx;
    Q_USEFUL_BUF_MAKE_STACK_UB(     signed_cose_buffer, 200);
    struct q_useful_buf_c           output;
    struct q_useful_buf_c           payload;
    enum t_cose_err_t               return_value;
    struct t_cose_sign1_verify_ctx  verify_ctx;


    /* -- integer content type -- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    t_cose_sign1_set_content_type_uint(&sign_ctx, 42);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                      Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                      signed_cose_buffer,
                                     &output);
    if(return_value) {
        return 1;
    }

    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    return_value = t_cose_sign1_verify(&verify_ctx,
                                        output,
                                       &payload,
                                       &parameters);
    if(return_value) {
        return 2;
    }

    if(parameters.content_type_uint != 42) {
        return 5;
    }


    /* -- string content type -- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    t_cose_sign1_set_content_type_tstr(&sign_ctx, "text/plain");

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &output);
    if(return_value) {
        return 1;
    }

    t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_ALLOW_SHORT_CIRCUIT);

    return_value = t_cose_sign1_verify(&verify_ctx,
                                       output,
                                       &payload,
                                       &parameters);
    if(return_value) {
        return 2;
    }

    if(q_useful_buf_compare(parameters.content_type_tstr, Q_USEFUL_BUF_FROM_SZ_LITERAL("text/plain"))) {
        return 6;
    }


    /* -- content type in error -- */
    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    t_cose_sign1_set_content_type_tstr(&sign_ctx, "text/plain");
    t_cose_sign1_set_content_type_uint(&sign_ctx, 42);


    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &output);
    if(return_value != T_COSE_ERR_DUPLICATE_PARAMETER) {
        return 1;
    }
#endif
    return 0;

}


struct sign1_sample {
    struct q_useful_buf_c CBOR;
    enum t_cose_err_t     expected_error;
};

static struct sign1_sample sign1_sample_inputs[] = {
    /* With an indefinite length string payload */
    { {(uint8_t[]){0x84, 0x40, 0xa0, 0x5f, 0x00, 0xff, 0x40}, 7}, T_COSE_ERR_SIGN1_FORMAT},
    /* Too few items in unprotected header parameters bucket */
    { {(uint8_t[]){0x84, 0x40, 0xa3, 0x40, 0x40}, 5}, T_COSE_ERR_PARAMETER_CBOR},
    /* Too few items in definite array */
    { {(uint8_t[]){0x83, 0x40, 0xa0, 0x40}, 4}, T_COSE_ERR_SIGN1_FORMAT},
    /* Too-long signature */
    { {(uint8_t[]){0x84, 0x40, 0xa0, 0x40, 0x4f}, 5}, T_COSE_ERR_SIGN1_FORMAT},
    /* Too-long payload */
    { {(uint8_t[]){0x84, 0x40, 0xa0, 0x4f, 0x40}, 5}, T_COSE_ERR_SIGN1_FORMAT},
    /* Too-long protected parameters bucket */
    { {(uint8_t[]){0x84, 0x4f, 0xa0, 0x40, 0x40}, 5}, T_COSE_ERR_SIGN1_FORMAT},
    /* Unterminated indefinite length */
    { {(uint8_t[]){0x9f, 0x40, 0xbf, 0xff, 0x40, 0x40}, 6}, T_COSE_ERR_CBOR_NOT_WELL_FORMED},
    /* The smallest legal COSE_Sign1 using indefinite lengths */
    { {(uint8_t[]){0x9f, 0x40, 0xbf, 0xff, 0x40, 0x40, 0xff}, 7}, T_COSE_SUCCESS},
    /* The smallest legal COSE_Sign1 using definite lengths */
    { {(uint8_t[]){0x84, 0x40, 0xa0, 0x40, 0x40}, 5}, T_COSE_SUCCESS},
    /* Just one not-well-formed byte -- a reserved value */
    { {(uint8_t[]){0x3c}, 1}, T_COSE_ERR_SIGN1_FORMAT },
    /* terminate the list */
    { {NULL, 0}, T_COSE_SUCCESS },
};


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t sign1_structure_decode_test(void)
{
    const struct sign1_sample      *sample;
    struct q_useful_buf_c           payload;
    enum t_cose_err_t               result;
    struct t_cose_sign1_verify_ctx  verify_ctx;


    for(sample = sign1_sample_inputs; !q_useful_buf_c_is_null(sample->CBOR); sample++) {
        t_cose_sign1_verify_init(&verify_ctx, T_COSE_OPT_DECODE_ONLY);


        result = t_cose_sign1_verify(&verify_ctx,
                                      sample->CBOR,
                                     &payload,
                                      NULL);
        if(result != sample->expected_error) {
            /* Returns 100 * index of the input + the unexpected error code */
            const size_t sample_index = (size_t)(sample - sign1_sample_inputs) / sizeof(struct sign1_sample);
            return (int32_t)((sample_index+1)*100 + result);
        }
    }

    return 0;
}


#ifdef T_COSE_ENABLE_HASH_FAIL_TEST

/* Linkage to global variable in t_cose_test_crypto.c. This is only
 * used for an occasional test in a non-threaded environment so a global
 * variable is safe. This test and the hacks in the crypto code are
 * never enabled for commercial deployments.
 */
extern int hash_test_mode;


/*
 * Public function, see t_cose_test.h
 */
int_fast32_t short_circuit_hash_fail_test()
{
    struct t_cose_sign1_sign_ctx sign_ctx;
    enum t_cose_err_t            return_value;
    struct q_useful_buf_c        wrapped_payload;
    Q_USEFUL_BUF_MAKE_STACK_UB(  signed_cose_buffer, 200);

    /* See test description in t_cose_test.h for a full description of
     * what this does and what it needs to run.
     */


    /* Set the global variable to cause the hash implementation to
     * error out so this test can see what happens
     */
    hash_test_mode = 1;

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &wrapped_payload);

    hash_test_mode = 0;

    if(return_value != T_COSE_ERR_HASH_GENERAL_FAIL) {
        return 2000 + return_value;
    }


    /* Set the global variable to cause the hash implementation to
     * error out so this test can see what happens
     */
    hash_test_mode = 2;

    t_cose_sign1_sign_init(&sign_ctx,
                           T_COSE_OPT_SHORT_CIRCUIT_SIG,
                           T_COSE_ALGORITHM_ES256);

    return_value = t_cose_sign1_sign(&sign_ctx,
                                     Q_USEFUL_BUF_FROM_SZ_LITERAL("payload"),
                                     signed_cose_buffer,
                                     &wrapped_payload);

    hash_test_mode = 0;

    if(return_value != T_COSE_ERR_HASH_GENERAL_FAIL) {
        return 2000 + return_value;
    }

    return 0;
}

#endif /* T_COSE_ENABLE_HASH_FAIL_TEST */
