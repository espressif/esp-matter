/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "cc_pal_log.h"
#include "cc_ecpki_error.h"
#include "mbedtls_cc_ec_mont_edw_error.h"
#include "ecp.h"
#include "ecp_common.h"
#include "cc_bitops.h"

int error_mapping_cc_to_mbedtls_ecc (CCError_t cc_error)
{
    int ret;
    switch (cc_error)
    {

            case CC_ECPKI_ILLEGAL_DOMAIN_ID_ERROR:
            case CC_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_ID_IS_NOT_VALID_ERROR:
            case CC_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR :
            case CC_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR:
            case CC_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR:
            case CC_ECPKI_INVALID_DOMAIN_ID_ERROR:
            case CC_EC_MONT_IS_NOT_SUPPORTED:
            case CC_EC_EDW_IS_NOT_SUPPORTED:

                ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
                break;

            case CC_ECPKI_INTERNAL_ERROR:
            case CC_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR:
            case CC_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR:
            case CC_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR:
            case CC_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR:
            case CC_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR:
            case CC_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR:
            case CC_ECPKI_INVALID_PRIV_KEY_TAG_ERROR:
            case CC_ECPKI_INVALID_PUBL_KEY_TAG_ERROR:
            case CC_ECIES_INVALID_PUBL_KEY_TAG_ERROR:
            case CC_ECIES_INVALID_PRIV_KEY_TAG_ERROR:
            case CC_ECIES_INVALID_PRIV_KEY_VALUE_ERROR:

                ret = MBEDTLS_ERR_ECP_INVALID_KEY;
                break;

            case CC_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR:
            case CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR:
                ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
                break;

            case CC_ECPKI_DOMAIN_PTR_ERROR:
            case CC_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR:
            case CC_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR:
            case CC_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR:
            case CC_ECPKI_RND_CONTEXT_PTR_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR:
            case CC_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR:
            case CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_DOMAIN_PTR_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_EC_PARAMETR_PTR_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_EC_PARAMETR_SIZE_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_COFACTOR_PARAMS_ERROR:
            case CC_ECPKI_BUILD_DOMAIN_SECURITY_STRENGTH_ERROR:
            case CC_ECPKI_BUILD_SCA_RESIST_ILLEGAL_MODE_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR:
            case CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR:
            case CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR:
            case CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR:
            case CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
            case CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR:
            case CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR:
            case CC_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR:
            case CC_ECDSA_SIGN_INVALID_RND_FUNCTION_PTR_ERROR:
            case CC_ECDSA_SIGN_SIGNING_ERROR:
            case CC_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR:
            case CC_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR:
            case CC_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR:
            case CC_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR:
            case CC_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR:
            case CC_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
            case CC_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
            case CC_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR:
            case CC_ECC_ILLEGAL_HASH_MODE_ERROR:
            case CC_ECPKI_INVALID_RND_FUNC_PTR_ERROR:
            case CC_ECPKI_INVALID_RND_CTX_PTR_ERROR:
            case CC_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR:
            case CC_ECPKI_INVALID_BASE_POINT_PTR_ERROR:
            case CC_ECIES_INVALID_PUBL_KEY_PTR_ERROR:
            case CC_ECIES_INVALID_PRIV_KEY_PTR_ERROR:
            case CC_ECIES_INVALID_KDF_DERIV_MODE_ERROR:
            case CC_ECIES_INVALID_KDF_HASH_MODE_ERROR:
            case CC_ECIES_INVALID_SECRET_KEY_PTR_ERROR:
            case CC_ECIES_INVALID_SECRET_KEY_SIZE_ERROR:
            case CC_ECIES_INVALID_CIPHER_DATA_PTR_ERROR:
            case CC_ECIES_INVALID_CIPHER_DATA_SIZE_PTR_ERROR:
            case CC_ECIES_INVALID_CIPHER_DATA_SIZE_ERROR:
            case CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR:
            case CC_ECIES_INVALID_EPHEM_KEY_PAIR_PTR_ERROR:
            case CC_EC_EDW_INVALID_INPUT_POINTER_ERROR:
            case CC_EC_EDW_INVALID_INPUT_SIZE_ERROR:
            case CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR:
            case CC_EC_EDW_INVALID_SCALAR_DATA_ERROR:
            case CC_EC_EDW_RND_CONTEXT_PTR_INVALID_ERROR:
            case CC_EC_EDW_RND_GEN_VECTOR_FUNC_ERROR:
            case CC_EC_MONT_INVALID_INPUT_POINTER_ERROR:
            case CC_EC_MONT_INVALID_INPUT_SIZE_ERROR:
            case CC_EC_MONT_INVALID_DOMAIN_ID_ERROR:
            case CC_ECEDW_INTERNAL_ERROR:
            case CC_ECMONT_INTERNAL_ERROR:

                ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
                break;

            default:
                ret = -1;
                CC_PAL_LOG_ERR("Unknown CC_ERROR %d\n", cc_error);
                break;
    }
    CC_PAL_LOG_INFO("Converted CC_ERROR %d to MBEDTLS_ERR %d\n", cc_error, ret);
    return ret;
}

/* conversion from mbedtls group id (curve) to CC domain*/
int ecp_grp_id_to_domain_id (const mbedtls_ecp_group_id id, CCEcpkiDomainID_t *domain_id)
{
    switch (id)
    {
        case MBEDTLS_ECP_DP_SECP192R1:
            *domain_id = CC_ECPKI_DomainID_secp192r1;
            break;                          /*!< 192-bits NIST curve  */
        case MBEDTLS_ECP_DP_SECP224R1:      /*!< 224-bits NIST curve  */
            *domain_id = CC_ECPKI_DomainID_secp224r1;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:      /*!< 256-bits NIST curve  */
            *domain_id = CC_ECPKI_DomainID_secp256r1;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:      /*!< 384-bits NIST curve  */
            *domain_id = CC_ECPKI_DomainID_secp384r1;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:      /*!< 521-bits NIST curve  */
            *domain_id = CC_ECPKI_DomainID_secp521r1;
            break;
        case MBEDTLS_ECP_DP_SECP192K1:      /*!< 192-bits "Koblitz" curve */
            *domain_id = CC_ECPKI_DomainID_secp192k1;
            break;
        case MBEDTLS_ECP_DP_SECP224K1:      /*!< 224-bits "Koblitz" curve */
            *domain_id = CC_ECPKI_DomainID_secp224k1;
            break;
        case MBEDTLS_ECP_DP_SECP256K1:      /*!< 256-bits "Koblitz" curve */
            *domain_id = CC_ECPKI_DomainID_secp256k1;
            break;

        default:
            CC_PAL_LOG_ERR("Error - ecp_grp_id_to_domain_id, group id %d is not supported\n", id);
            *domain_id = CC_ECPKI_DomainIDLast;
            return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    return (0);
}

