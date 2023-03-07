// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/sasl_server_mechanism.h"

typedef struct SASL_SERVER_MECHANISM_INSTANCE_TAG
{
    const SASL_SERVER_MECHANISM_INTERFACE_DESCRIPTION* sasl_server_mechanism_interface_description;
    CONCRETE_SASL_SERVER_MECHANISM_HANDLE concrete_sasl_server_mechanism;
} SASL_SERVER_MECHANISM_INSTANCE;

SASL_SERVER_MECHANISM_HANDLE sasl_server_mechanism_create(const SASL_SERVER_MECHANISM_INTERFACE_DESCRIPTION* sasl_server_mechanism_interface_description, void* sasl_server_mechanism_create_parameters)
{
    SASL_SERVER_MECHANISM_HANDLE result;

    if (sasl_server_mechanism_interface_description == NULL)
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_004: [ If the argument `sasl_server_mechanism_interface_description` is NULL, `sasl_server_mechanism_create` shall return NULL.]*/
        LogError("NULL sasl_server_mechanism_interface_description");
        result = NULL;
    }
    else if ((sasl_server_mechanism_interface_description->create == NULL) ||
        (sasl_server_mechanism_interface_description->destroy == NULL) ||
        (sasl_server_mechanism_interface_description->handle_initial_response == NULL) ||
        (sasl_server_mechanism_interface_description->handle_response == NULL) ||
        (sasl_server_mechanism_interface_description->get_mechanism_name == NULL))
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_005: [ If any `sasl_server_mechanism_interface_description` member is NULL, `sasl_server_mechanism_create` shall fail and return NULL.]*/
        LogError("Bad interface, create = %p, destroy = %p, handle_initial_response = %p, handle_response = %p, get_mechanism_name = %p",
            sasl_server_mechanism_interface_description->create,
            sasl_server_mechanism_interface_description->destroy,
            sasl_server_mechanism_interface_description->handle_initial_response,
            sasl_server_mechanism_interface_description->handle_response,
            sasl_server_mechanism_interface_description->get_mechanism_name);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_001: [`sasl_server_mechanism_create` shall return on success a non-NULL handle to a new SASL server mechanism interface.]*/
        result = (SASL_SERVER_MECHANISM_HANDLE)malloc(sizeof(SASL_SERVER_MECHANISM_INSTANCE));
        if (result == NULL)
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_006: [ If allocating the memory needed for the SASL server mechanism interface fails then `sasl_server_mechanism_create` shall fail and return NULL. ]*/
            LogError("Could not allocate memory for SASL mechanism");
        }
        else
        {
            result->sasl_server_mechanism_interface_description = sasl_server_mechanism_interface_description;

            /* Codes_SRS_SASL_SERVER_MECHANISM_01_002: [ In order to instantiate the concrete SASL server mechanism implementation the function `create` from the `sasl_server_mechanism_interface_description` shall be called, passing the `sasl_server_mechanism_create_parameters` to it.]*/
            result->concrete_sasl_server_mechanism = result->sasl_server_mechanism_interface_description->create(sasl_server_mechanism_create_parameters);
            if (result->concrete_sasl_server_mechanism == NULL)
            {
                /* Codes_SRS_SASL_SERVER_MECHANISM_01_003: [ If the underlying `create` call fails, `sasl_server_mechanism_create` shall return NULL. ]*/
                LogError("concrete sasl server mechanism create failed");
                free(result);
                result = NULL;
            }
        }
    }

    return result;
}

void sasl_server_mechanism_destroy(SASL_SERVER_MECHANISM_HANDLE sasl_server_mechanism)
{
    if (sasl_server_mechanism == NULL)
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_009: [ If the argument `sasl_server_mechanism` is NULL, `sasl_server_mechanism_destroy` shall do nothing. ]*/
        LogError("NULL sasl_server_mechanism");
    }
    else
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_008: [ `sasl_server_mechanism_destroy` shall also call the `destroy` function that is member of the `sasl_mechanism_interface_description` argument passed to `sasl_server_mechanism_create`, while passing as argument to `destroy` the result of the underlying concrete SASL mechanism handle. ]*/
        sasl_server_mechanism->sasl_server_mechanism_interface_description->destroy(sasl_server_mechanism->concrete_sasl_server_mechanism);
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_007: [ `sasl_server_mechanism_destroy` shall free all resources associated with the SASL mechanism handle. ]*/
        free(sasl_server_mechanism);
    }
}

int sasl_server_mechanism_handle_initial_response(SASL_SERVER_MECHANISM_HANDLE sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES* initial_response_bytes, const char* hostname, bool* send_challenge, SASL_SERVER_MECHANISM_BYTES* challenge_bytes)
{
    int result;

    if (sasl_server_mechanism == NULL)
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_012: [ If the argument `sasl_server_mechanism` is NULL, `sasl_server_mechanism_handle_initial_response` shall fail and return a non-zero value. ]*/
        LogError("NULL sasl_server_mechanism");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_010: [ `sasl_server_mechanism_handle_initial_response` shall call the specific `handle_initial_response` function specified in `sasl_server_mechanism_create`, passing the `initial_response_bytes`, `hostname`, `send_challenge` and `challenge_bytes` arguments to it. ]*/
        if (sasl_server_mechanism->sasl_server_mechanism_interface_description->handle_initial_response(sasl_server_mechanism->concrete_sasl_server_mechanism, initial_response_bytes, hostname, send_challenge, challenge_bytes) != 0)
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_013: [ If the underlying `handle_initial_response` fails, `sasl_server_mechanism_handle_initial_response` shall fail and return a non-zero value. ]*/
            LogError("handle_initial_response_failed");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_011: [ On success, `sasl_server_mechanism_handle_initial_response` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int sasl_server_mechanism_handle_response(SASL_SERVER_MECHANISM_HANDLE sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES* response_bytes, bool* send_next_challenge, SASL_SERVER_MECHANISM_BYTES* next_challenge_bytes)
{
    int result;

    if (sasl_server_mechanism == NULL)
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_017: [ If the argument `sasl_server_mechanism` is NULL, `sasl_server_mechanism_handle_response` shall fail and return a non-zero value. ]*/
        LogError("NULL sasl_server_mechanism");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_014: [ `sasl_server_mechanism_handle_response` shall call the specific `handle_response` function specified in `sasl_server_mechanism_create`, passing the `response_bytes`, `send_next_challenge` and `next_challenge_bytes` arguments to it. ]*/
        if (sasl_server_mechanism->sasl_server_mechanism_interface_description->handle_response(sasl_server_mechanism->concrete_sasl_server_mechanism, response_bytes, send_next_challenge, next_challenge_bytes) != 0)
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_018: [ If the underlying `handle_response` fails, `sasl_server_mechanism_handle_response` shall fail and return a non-zero value. ]*/
            LogError("handle_response_failed");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_016: [ On success, `sasl_server_mechanism_handle_response` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

const char* sasl_server_mechanism_get_mechanism_name(SASL_SERVER_MECHANISM_HANDLE sasl_server_mechanism)
{
    const char* result;

    if (sasl_server_mechanism == NULL)
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_021: [ If the argument `sasl_server_mechanism` is NULL, `sasl_server_mechanism_get_mechanism_name` shall fail and return a non-zero value. ]*/
        LogError("NULL sasl_server_mechanism");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_019: [ `sasl_server_mechanism_get_mechanism_name` shall call the specific `get_mechanism_name` function specified in `sasl_server_mechanism_create`. ]*/
        /* Codes_SRS_SASL_SERVER_MECHANISM_01_020: [ On success, `sasl_server_mechanism_get_mechanism_name` shall return a pointer to a string with the mechanism name. ]*/
        result = sasl_server_mechanism->sasl_server_mechanism_interface_description->get_mechanism_name();
        if (result == NULL)
        {
            /* Codes_SRS_SASL_SERVER_MECHANISM_01_022: [ If the underlying `get_mechanism_name` fails, `sasl_server_mechanism_get_mechanism_name` shall return NULL. ]*/
            LogError("concrete_sasl_mechanism_get_mechanism_name failed");
        }
    }

    return result;
}
