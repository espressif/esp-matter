// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/sasl_plain.h"

typedef struct SASL_PLAIN_INSTANCE_TAG
{
    unsigned char* init_bytes;
    uint32_t init_bytes_length;
} SASL_PLAIN_INSTANCE;

CONCRETE_SASL_MECHANISM_HANDLE saslplain_create(void* config)
{
    SASL_PLAIN_INSTANCE* result;

    if (config == NULL)
    {
        /* Codes_SRS_SASL_PLAIN_01_003: [If the `config` argument is NULL, then `saslplain_create` shall fail and return NULL.] */
        LogError("NULL config");
        result = NULL;
    }
    else
    {
        SASL_PLAIN_CONFIG* sasl_plain_config = (SASL_PLAIN_CONFIG*)config;

        /* Codes_SRS_SASL_PLAIN_01_025: [ `authzid` shall be optional. ]*/
        if ((sasl_plain_config->authcid == NULL) ||
            (sasl_plain_config->passwd == NULL))
        {
            /* Codes_SRS_SASL_PLAIN_01_004: [If either the `authcid` or `passwd` member of the `config` structure is NULL, then `saslplain_create` shall fail and return NULL.] */
            LogError("Bad configuration: authcid = %p, passwd = %p",
                sasl_plain_config->authcid, sasl_plain_config->passwd);
            result = NULL;
        }
        else
        {
            size_t authzid_length = sasl_plain_config->authzid == NULL ? 0 : strlen(sasl_plain_config->authzid);
            size_t authcid_length = strlen(sasl_plain_config->authcid);
            size_t passwd_length = strlen(sasl_plain_config->passwd);

            /* Codes_SRS_SASL_PLAIN_01_020: [   authcid   = 1*SAFE ; MUST accept up to 255 octets] */
            if ((authcid_length > 255) || (authcid_length == 0) ||
                /* Codes_SRS_SASL_PLAIN_01_021: [   authzid   = 1*SAFE ; MUST accept up to 255 octets] */
                (authzid_length > 255) ||
                /* Codes_SRS_SASL_PLAIN_01_022: [   passwd    = 1*SAFE ; MUST accept up to 255 octets] */
                (passwd_length > 255) || (passwd_length == 0))
            {
                LogError("Bad configuration: authcid length = %u, passwd length = %u",
                    (unsigned int)authcid_length, (unsigned int)passwd_length);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_SASL_PLAIN_01_001: [`saslplain_create` shall return on success a non-NULL handle to a new SASL plain mechanism.] */
                result = (SASL_PLAIN_INSTANCE*)calloc(1, sizeof(SASL_PLAIN_INSTANCE));
                if (result == NULL)
                {
                    /* Codes_SRS_SASL_PLAIN_01_002: [If allocating the memory needed for the saslplain instance fails then `saslplain_create` shall return NULL.] */
                    LogError("Cannot allocate memory for SASL plain instance");
                }
                else
                {
                    /* Ignore UTF8 for now */
                    result->init_bytes = (unsigned char*)malloc(authzid_length + authcid_length + passwd_length + 2);
                    if (result->init_bytes == NULL)
                    {
                        /* Codes_SRS_SASL_PLAIN_01_002: [If allocating the memory needed for the saslplain instance fails then `saslplain_create` shall return NULL.] */
                        LogError("Cannot allocate init bytes");
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        /* Codes_SRS_SASL_PLAIN_01_016: [The mechanism consists of a single message, a string of [UTF-8] encoded [Unicode] characters, from the client to the server.] */
                        /* Codes_SRS_SASL_PLAIN_01_017: [The client presents the authorization identity (identity to act as), followed by a NUL (U+0000) character, followed by the authentication identity (identity whose password will be used), followed by a NUL (U+0000) character, followed by the clear-text password.] */
                        /* Codes_SRS_SASL_PLAIN_01_019: [   message   = [authzid] UTF8NUL authcid UTF8NUL passwd] */
                        /* Codes_SRS_SASL_PLAIN_01_023: [The authorization identity (authzid), authentication identity (authcid), password (passwd), and NUL character deliminators SHALL be transferred as [UTF-8] encoded strings of [Unicode] characters.] */
                        /* Codes_SRS_SASL_PLAIN_01_024: [As the NUL (U+0000) character is used as a deliminator, the NUL (U+0000) character MUST NOT appear in authzid, authcid, or passwd productions.] */

                        /* Codes_SRS_SASL_PLAIN_01_018: [As with other SASL mechanisms, the client does not provide an authorization identity when it wishes the server to derive an identity from the credentials and use that as the authorization identity.] */
                        if (authzid_length > 0)
                        {
                            (void)memcpy(result->init_bytes, sasl_plain_config->authzid, authzid_length);
                        }

                        result->init_bytes[authzid_length] = 0;
                        (void)memcpy(result->init_bytes + authzid_length + 1, sasl_plain_config->authcid, authcid_length);
                        result->init_bytes[authzid_length + authcid_length + 1] = 0;
                        (void)memcpy(result->init_bytes + authzid_length + authcid_length + 2, sasl_plain_config->passwd, passwd_length);
                        result->init_bytes_length = (uint32_t)(authzid_length + authcid_length + passwd_length + 2);
                    }
                }
            }
        }
    }

    return result;
}

void saslplain_destroy(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle)
{
    if (sasl_mechanism_concrete_handle == NULL)
    {
        /* Codes_SRS_SASL_PLAIN_01_006: [If the argument `concrete_sasl_mechanism` is NULL, `saslplain_destroy` shall do nothing.] */
        LogError("NULL sasl_mechanism_concrete_handle");
    }
    else
    {
        /* Codes_SRS_SASL_PLAIN_01_005: [`saslplain_destroy` shall free all resources associated with the SASL mechanism.] */
        SASL_PLAIN_INSTANCE* sasl_plain_instance = (SASL_PLAIN_INSTANCE*)sasl_mechanism_concrete_handle;
        if (sasl_plain_instance->init_bytes != NULL)
        {
            free(sasl_plain_instance->init_bytes);
        }

        free(sasl_plain_instance);
    }
}

int saslplain_get_init_bytes(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES* init_bytes)
{
    int result;

    if ((sasl_mechanism_concrete_handle == NULL) ||
        (init_bytes == NULL))
    {
        /* Codes_SRS_SASL_PLAIN_01_009: [If any argument is NULL, `saslplain_get_init_bytes` shall return a non-zero value.] */
        LogError("Bad arguments: sasl_mechanism_concrete_handle = %p, init_bytes = %p",
            sasl_mechanism_concrete_handle, init_bytes);
        result = MU_FAILURE;
    }
    else
    {
        SASL_PLAIN_INSTANCE* sasl_plain_instance = (SASL_PLAIN_INSTANCE*)sasl_mechanism_concrete_handle;

        /* Codes_SRS_SASL_PLAIN_01_007: [`saslplain_get_init_bytes` shall construct the initial bytes per the RFC 4616.] */
        init_bytes->bytes = sasl_plain_instance->init_bytes;
        init_bytes->length = sasl_plain_instance->init_bytes_length;

        /* Codes_SRS_SASL_PLAIN_01_008: [On success `saslplain_get_init_bytes` shall return zero.] */
        result = 0;
    }

    return result;
}

const char* saslplain_get_mechanism_name(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism)
{
    const char* result;

    if (sasl_mechanism == NULL)
    {
        /* Codes_SRS_SASL_PLAIN_01_011: [If the argument `concrete_sasl_mechanism` is NULL, `saslplain_get_mechanism_name` shall return NULL.] */
        LogError("NULL sasl_mechanism");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SASL_PLAIN_01_010: [`saslplain_get_mechanism_name` shall validate the argument `concrete_sasl_mechanism` and on success it shall return a pointer to the string "PLAIN".] */
        result = "PLAIN";
    }

    return result;
}

int saslplain_challenge(CONCRETE_SASL_MECHANISM_HANDLE concrete_sasl_mechanism, const SASL_MECHANISM_BYTES* challenge_bytes, SASL_MECHANISM_BYTES* response_bytes)
{
    int result;

    (void)challenge_bytes;

    /* Codes_SRS_SASL_PLAIN_01_014: [If the `concrete_sasl_mechanism` or `response_bytes` argument is NULL then `saslplain_challenge` shall fail and return a non-zero value.] */
    if ((concrete_sasl_mechanism == NULL) ||
        (response_bytes == NULL))
    {
        LogError("Bad arguments: concrete_sasl_mechanism = %p, response_bytes = %p",
            concrete_sasl_mechanism, response_bytes);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SASL_PLAIN_01_012: [`saslplain_challenge` shall set the `response_bytes` buffer to NULL and 0 size as the PLAIN SASL mechanism does not implement challenge/response.] */
        response_bytes->bytes = NULL;
        response_bytes->length = 0;

        /* Codes_SRS_SASL_PLAIN_01_013: [On success, `saslplain_challenge` shall return 0.] */
        result = 0;
    }

    return result;
}

static const SASL_MECHANISM_INTERFACE_DESCRIPTION saslplain_interface =
{
    /* Codes_SRS_SASL_PLAIN_01_015: [**`saslplain_get_interface` shall return a pointer to a `SASL_MECHANISM_INTERFACE_DESCRIPTION` structure that contains pointers to the functions: `saslplain_create`, `saslplain_destroy`, `saslplain_get_init_bytes`, `saslplain_get_mechanism_name`, `saslplain_challenge`.] */
    saslplain_create,
    saslplain_destroy,
    saslplain_get_init_bytes,
    saslplain_get_mechanism_name,
    saslplain_challenge
};

const SASL_MECHANISM_INTERFACE_DESCRIPTION* saslplain_get_interface(void)
{
    return &saslplain_interface;
}
