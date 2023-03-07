// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/sasl_mssbcbs.h"

typedef struct SASL_MSSBCBS_INSTANCE_TAG
{
    unsigned char dummy;
} SASL_MSSBCBS_INSTANCE;

static const SASL_MECHANISM_INTERFACE_DESCRIPTION saslmssbcbs_interface =
{
    saslmssbcbs_create,
    saslmssbcbs_destroy,
    saslmssbcbs_get_init_bytes,
    saslmssbcbs_get_mechanism_name,
    saslmssbcbs_challenge
};

CONCRETE_SASL_MECHANISM_HANDLE saslmssbcbs_create(void* config)
{
    (void)config;
    return malloc(sizeof(SASL_MSSBCBS_INSTANCE));
}

void saslmssbcbs_destroy(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle)
{
    if (sasl_mechanism_concrete_handle != NULL)
    {
        free(sasl_mechanism_concrete_handle);
    }
}

int saslmssbcbs_get_init_bytes(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES* init_bytes)
{
    int result;

    if (sasl_mechanism_concrete_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        init_bytes->bytes = NULL;
        init_bytes->length = 0;

        result = 0;
    }

    return result;
}

const char* saslmssbcbs_get_mechanism_name(CONCRETE_SASL_MECHANISM_HANDLE sasl_mechanism)
{
    const char* result;

    if (sasl_mechanism == NULL)
    {
        result = NULL;
    }
    else
    {
        result = "MSSBCBS";
    }

    return result;
}

int saslmssbcbs_challenge(CONCRETE_SASL_MECHANISM_HANDLE concrete_sasl_mechanism, const SASL_MECHANISM_BYTES* challenge_bytes, SASL_MECHANISM_BYTES* response_bytes)
{
    (void)concrete_sasl_mechanism;
    (void)challenge_bytes;
    (void)response_bytes;
    return 0;
}

const SASL_MECHANISM_INTERFACE_DESCRIPTION* saslmssbcbs_get_interface(void)
{
    return &saslmssbcbs_interface;
}
