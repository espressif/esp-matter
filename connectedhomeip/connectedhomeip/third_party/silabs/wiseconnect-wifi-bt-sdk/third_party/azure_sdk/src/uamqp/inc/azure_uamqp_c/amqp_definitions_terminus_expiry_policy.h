

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_TERMINUS_EXPIRY_POLICY_H
#define AMQP_DEFINITIONS_TERMINUS_EXPIRY_POLICY_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef const char* terminus_expiry_policy;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_terminus_expiry_policy, terminus_expiry_policy, value);


    #define amqpvalue_get_terminus_expiry_policy amqpvalue_get_symbol

    #define terminus_expiry_policy_link_detach "link-detach"
    #define terminus_expiry_policy_session_end "session-end"
    #define terminus_expiry_policy_connection_close "connection-close"
    #define terminus_expiry_policy_never "never"


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_TERMINUS_EXPIRY_POLICY_H */
