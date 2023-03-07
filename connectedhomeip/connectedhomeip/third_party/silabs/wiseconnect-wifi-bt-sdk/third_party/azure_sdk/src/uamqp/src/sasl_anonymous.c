// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/sasl_anonymous.h"

typedef struct SASL_ANONYMOUS_INSTANCE_TAG
{
    unsigned char dummy;
} SASL_ANONYMOUS_INSTANCE;

/* Codes_SRS_SASL_ANONYMOUS_01_001: [`saslanonymous_create` shall return on success a non-NULL handle to a new SASL anonymous mechanism.]*/
static CONCRETE_SASL_MECHANISM_HANDLE saslanonymous_create(void* config)
{
    CONCRETE_SASL_MECHANISM_HANDLE result;

    /* Codes_SRS_SASL_ANONYMOUS_01_003: [Since this is the ANONYMOUS SASL mechanism, `config` shall be ignored.]*/
    (void)config;

    result = malloc(sizeof(SASL_ANONYMOUS_INSTANCE));
    if (result == NULL)
    {
        /* Codes_SRS_SASL_ANONYMOUS_01_002: [If allocating the memory needed for the SASL anonymous instance fails then `saslanonymous_create` shall return NULL.] */
        LogError("Cannot allocate memory for SASL anonymous instance");
    }

    return result;
}

static void saslanonymous_destroy(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle)
{
    /* Codes_SRS_SASL_ANONYMOUS_01_005: [If the argument `concrete_sasl_mechanism` is NULL, `saslanonymous_destroy` shall do nothing.]*/
    if (sasl_mechanism_concrete_handle == NULL)
    {
        LogError("NULL sasl_mechanism_concrete_handle");
    }
    else
    {
        /* Codes_SRS_SASL_ANONYMOUS_01_004: [`saslanonymous_destroy` shall free all resources associated with the SASL mechanism.] */
        free(sasl_mechanism_concrete_handle);
    }
}

static int saslanonymous_get_init_bytes(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES* init_bytes)
{
    int result;

    /* Codes_SRS_SASL_ANONYMOUS_01_007: [If any argument is NULL, `saslanonymous_get_init_bytes` shall return a non-zero value.]*/
    if ((sasl_mechanism_concrete_handle == NULL) ||
        (init_bytes == NULL))
    {
        LogError("Bad arguments: sasl_mechanism_concrete_handle = %p, init_bytes = %p",
            sasl_mechanism_concrete_handle, init_bytes);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SASL_ANONYMOUS_01_012: [The bytes field of `init_buffer` shall be set to NULL.] */
        init_bytes->bytes = NULL;
        /* Codes_SRS_SASL_ANONYMOUS_01_006: [`saslanonymous_get_init_bytes` shall validate the `concrete_sasl_mechanism` argument and set the length of the `init_bytes` argument to be zero.] */
        init_bytes->length = 0;

        /* Codes_SRS_SASL_ANONYMOUS_01_011: [On success `saslanonymous_get_init_bytes` shall return zero.] */
        result = 0;
    }

    return result;
}

static const char* saslanonymous_get_mechanism_name(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism)
{
    const char* result;

    /* Codes_SRS_SASL_ANONYMOUS_01_009: [If the argument `concrete_sasl_mechanism` is NULL, `saslanonymous_get_mechanism_name` shall return NULL.] */
    if (sasl_mechanism == NULL)
    {
        LogError("NULL sasl_mechanism");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SASL_ANONYMOUS_01_008: [`saslanonymous_get_mechanism_name` shall validate the argument `concrete_sasl_mechanism` and on success it shall return a pointer to the string `ANONYMOUS`.] */
        result = "ANONYMOUS";
    }

    return result;
}

static int saslanonymous_challenge(CONCRETE_SASL_MECHANISM_HANDLE concrete_sasl_mechanism, const SASL_MECHANISM_BYTES* challenge_bytes, SASL_MECHANISM_BYTES* response_bytes)
{
    int result;

    (void)challenge_bytes;

    /* Codes_SRS_SASL_ANONYMOUS_01_015: [If the `concrete_sasl_mechanism` or `response_bytes` argument is NULL then `saslanonymous_challenge` shall fail and return a non-zero value.] */
    if ((concrete_sasl_mechanism == NULL) ||
        (response_bytes == NULL))
    {
        LogError("Bad arguments: concrete_sasl_mechanism = %p, response_bytes = %p",
            concrete_sasl_mechanism, response_bytes);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SASL_ANONYMOUS_01_013: [`saslanonymous_challenge` shall set the `buffer` field to NULL and `size` to 0 in the `response_bytes` argument as the ANONYMOUS SASL mechanism does not implement challenge/response.] */
        response_bytes->bytes = NULL;
        response_bytes->length = 0;

        /* Codes_SRS_SASL_ANONYMOUS_01_014: [On success, `saslanonymous_challenge` shall return 0.] */
        result = 0;
    }

    return result;
}

/* Codes_SRS_SASL_ANONYMOUS_01_010: [`saslanonymous_get_interface` shall return a pointer to a `SASL_MECHANISM_INTERFACE_DESCRIPTION` structure that contains pointers to the functions: `saslanonymous_create`, `saslanonymous_destroy`, `saslanonymous_get_init_bytes`, `saslanonymous_get_mechanism_name`, `saslanonymous_challenge`.] */
static const SASL_MECHANISM_INTERFACE_DESCRIPTION saslanonymous_interface =
{
    saslanonymous_create,
    saslanonymous_destroy,
    saslanonymous_get_init_bytes,
    saslanonymous_get_mechanism_name,
    saslanonymous_challenge
};

const SASL_MECHANISM_INTERFACE_DESCRIPTION* saslanonymous_get_interface(void)
{
    return &saslanonymous_interface;
}
