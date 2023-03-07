// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASL_SERVER_MECHANISM_H
#define SASL_SERVER_MECHANISM_H

#ifdef __cplusplus
extern "C" {
#include "cstdint"
#else
#include "stdint.h"
#include "stdbool.h"
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

    typedef struct SASL_SERVER_MECHANISM_INSTANCE_TAG* SASL_SERVER_MECHANISM_HANDLE;
    typedef void* CONCRETE_SASL_SERVER_MECHANISM_HANDLE;

    typedef struct SASL_SERVER_MECHANISM_BYTES_TAG
    {
        const void* bytes;
        uint32_t length;
    } SASL_SERVER_MECHANISM_BYTES;

    typedef CONCRETE_SASL_SERVER_MECHANISM_HANDLE(*SASL_SERVER_MECHANISM_CREATE)(void* config);
    typedef void(*SASL_SERVER_MECHANISM_DESTROY)(CONCRETE_SASL_SERVER_MECHANISM_HANDLE concrete_sasl_server_mechanism);
    typedef int(*SASL_SERVER_MECHANISM_HANDLE_INITIAL_RESPONSE)(CONCRETE_SASL_SERVER_MECHANISM_HANDLE concrete_sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES* initial_response_bytes, const char* hostname, bool* send_challenge, SASL_SERVER_MECHANISM_BYTES* challenge_bytes);
    typedef int(*SASL_SERVER_MECHANISM_HANDLE_RESPONSE)(CONCRETE_SASL_SERVER_MECHANISM_HANDLE concrete_sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES* response_bytes, bool* send_next_challenge, SASL_SERVER_MECHANISM_BYTES* next_challenge_bytes);
    typedef const char*(*SASL_SERVER_MECHANISM_GET_MECHANISM_NAME)(void);

    typedef struct SASL_SERVER_MECHANISM_INTERFACE_DESCRIPTION_TAG
    {
        SASL_SERVER_MECHANISM_CREATE create;
        SASL_SERVER_MECHANISM_DESTROY destroy;
        SASL_SERVER_MECHANISM_HANDLE_INITIAL_RESPONSE handle_initial_response;
        SASL_SERVER_MECHANISM_HANDLE_RESPONSE handle_response;
        SASL_SERVER_MECHANISM_GET_MECHANISM_NAME get_mechanism_name;
    } SASL_SERVER_MECHANISM_INTERFACE_DESCRIPTION;

    MOCKABLE_FUNCTION(, SASL_SERVER_MECHANISM_HANDLE, sasl_server_mechanism_create, const SASL_SERVER_MECHANISM_INTERFACE_DESCRIPTION*, sasl_server_mechanism_interface_description, void*, sasl_server_mechanism_create_parameters);
    MOCKABLE_FUNCTION(, void, sasl_server_mechanism_destroy, SASL_SERVER_MECHANISM_HANDLE, sasl_server_mechanism);
    MOCKABLE_FUNCTION(, int, sasl_server_mechanism_handle_initial_response, SASL_SERVER_MECHANISM_HANDLE, sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES*, initial_response_bytes, const char*, hostname, bool*, send_challenge, SASL_SERVER_MECHANISM_BYTES*, challenge_bytes);
    MOCKABLE_FUNCTION(, int, sasl_server_mechanism_handle_response, SASL_SERVER_MECHANISM_HANDLE, sasl_server_mechanism, const SASL_SERVER_MECHANISM_BYTES*, response_bytes, bool*, send_next_challenge, SASL_SERVER_MECHANISM_BYTES*, next_challenge_bytes);
    MOCKABLE_FUNCTION(, const char*, sasl_server_mechanism_get_mechanism_name, SASL_SERVER_MECHANISM_HANDLE, sasl_server_mechanism);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SASL_SERVER_MECHANISM_H */
