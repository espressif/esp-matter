/*
 *  t_cose_make_openssl_test_key.c
 *
 * Copyright 2019-2020, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "t_cose_make_test_pub_key.h" /* The interface implemented here */

#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h" /* for NID for EC curve */
#include "openssl/err.h"


/*
 * Some hard coded keys for the test cases here.
 */
#define PUBLIC_KEY_prime256v1 \
    "0437ab65955fae0466673c3a2934a3" \
    "4f2f0ec2b3eec224198557998fc04b" \
    "f4b2b495d9798f2539c90d7d102b3b" \
    "bbda7fcbdb0e9b58d4e1ad2e61508d" \
    "a75f84a67b"

#define PRIVATE_KEY_prime256v1 \
    "f1b7142343402f3b5de7315ea894f9" \
    "da5cf503ff7938a37ca14eb0328698" \
    "8450"


#define PUBLIC_KEY_secp384r1 \
    "04bdd9c3f818c9cef3e11e2d40e775" \
    "beb37bc376698d71967f93337a4e03" \
    "2dffb11b505067dddb4214b56d9bce" \
    "c59177eccd8ab05f50975933b9a738" \
    "d90c0b07eb9519567ef9075807cf77" \
    "139fc1fe85608851361136806123ed" \
    "c735ce5a03e8e4"

#define PRIVATE_KEY_secp384r1 \
    "03df14f4b8a43fd8ab75a6046bd2b5" \
    "eaa6fd10b2b203fd8a78d7916de20a" \
    "a241eb37ec3d4c693d23ba2b4f6e5b" \
    "66f57f"


#define PUBLIC_KEY_secp521r1 \
    "0400e4d253175a14311fc2dd487687" \
    "70cb49b07bd15d327beb98aa33e60c" \
    "d0181b17fb8f1cbf07dbc8652ff5b7" \
    "b4452c082e0686c0fab8089071cbc5" \
    "37101d344b94c201e6424f3a18da4f" \
    "20ecabfbc84b8467c217cd67055fa5" \
    "dec7fb1ae87082302c1813caa4b7b1" \
    "cf28d94677e486fb4b317097e9307a" \
    "bdb9d50187779a3d1e682c123c"

#define PRIVATE_KEY_secp521r1 \
    "0045d2d1439435fab333b1c6c8b534" \
    "f0969396ad64d5f535d65f68f2a160" \
    "6590bb15fd5322fc97a416c395745e" \
    "72c7c85198c0921ab3b8e92dd901b5" \
    "a42159adac6d"

/*
 * Public function, see t_cose_make_test_pub_key.h
 */
/*
 * The key object returned by this is malloced and has to be freed by
 * by calling free_ecdsa_key_pair(). This heap use is a part of
 * OpenSSL and not t_cose which does not use the heap
 */
enum t_cose_err_t make_ecdsa_key_pair(int32_t           cose_algorithm_id,
                                      struct t_cose_key *key_pair)
{
    EC_GROUP          *ossl_ec_group = NULL;
    enum t_cose_err_t  return_value;
    BIGNUM            *ossl_private_key_bn = NULL;
    EC_KEY            *ossl_ec_key = NULL;
    int                ossl_result;
    EC_POINT          *ossl_pub_key_point = NULL;
    int                nid;
    const char        *public_key;
    const char        *private_key;

    switch (cose_algorithm_id) {
    case T_COSE_ALGORITHM_ES256:
        nid         = NID_X9_62_prime256v1;
        public_key  = PUBLIC_KEY_prime256v1;
        private_key =  PRIVATE_KEY_prime256v1 ;
        break;

    case T_COSE_ALGORITHM_ES384:
        nid         = NID_secp384r1;
        public_key  = PUBLIC_KEY_secp384r1;
        private_key = PRIVATE_KEY_secp384r1;
        break;

    case T_COSE_ALGORITHM_ES512:
        nid         = NID_secp521r1;
        public_key  = PUBLIC_KEY_secp521r1;
        private_key = PRIVATE_KEY_secp521r1;
        break;

    default:
        return -1;
    }

    /* Make a group for the particular EC algorithm */
    ossl_ec_group = EC_GROUP_new_by_curve_name(nid);
    if(ossl_ec_group == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Make an empty EC key object */
    ossl_ec_key = EC_KEY_new();
    if(ossl_ec_key == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Associate group with key object */
    ossl_result = EC_KEY_set_group(ossl_ec_key, ossl_ec_group);
    if (!ossl_result) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Make an instance of a big number to store the private key */
    ossl_private_key_bn = BN_new();
    if(ossl_private_key_bn == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }
    BN_zero(ossl_private_key_bn);

    /* Stuff the specific private key into the big num */
    ossl_result = BN_hex2bn(&ossl_private_key_bn, private_key);
    if(ossl_private_key_bn == 0) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Now associate the big num with the key object so we finally
     * have a key set up and ready for signing */
    ossl_result = EC_KEY_set_private_key(ossl_ec_key, ossl_private_key_bn);
    if (!ossl_result) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }


    /* Make an empty EC point into which the public key gets loaded */
    ossl_pub_key_point = EC_POINT_new(ossl_ec_group);
    if(ossl_pub_key_point == NULL) {
        return_value = T_COSE_ERR_INSUFFICIENT_MEMORY;
        goto Done;
    }

    /* Turn the serialized public key into an EC point */
    ossl_pub_key_point = EC_POINT_hex2point(ossl_ec_group,
                                            public_key,
                                            ossl_pub_key_point,
                                            NULL);
    if(ossl_pub_key_point == NULL) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    /* Associate the EC point with key object */
    /* The key object has both the public and private keys in it */
    ossl_result = EC_KEY_set_public_key(ossl_ec_key, ossl_pub_key_point);
    if(ossl_result == 0) {
        return_value = T_COSE_ERR_SIG_FAIL;
        goto Done;
    }

    key_pair->k.key_ptr  = ossl_ec_key;
    key_pair->crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
    return_value         = T_COSE_SUCCESS;

Done:
    return return_value;
}


/*
 * Public function, see t_cose_make_test_pub_key.h
 */
void free_ecdsa_key_pair(struct t_cose_key key_pair)
{
    EC_KEY_free(key_pair.k.key_ptr);
}


/*
 * Public function, see t_cose_make_test_pub_key.h
 */
int check_for_key_pair_leaks()
{
    /* So far no good way to do this for OpenSSL or malloc() in general
       in a nice portable way. The PSA version does check so there is
       some coverage of the code even though there is no check here.
     */
    return 0;
}



