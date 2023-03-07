

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SENDER_SETTLE_MODE_H
#define AMQP_DEFINITIONS_SENDER_SETTLE_MODE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef uint8_t sender_settle_mode;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sender_settle_mode, sender_settle_mode, value);


    #define amqpvalue_get_sender_settle_mode amqpvalue_get_ubyte

    #define sender_settle_mode_unsettled 0
    #define sender_settle_mode_settled 1
    #define sender_settle_mode_mixed 2


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SENDER_SETTLE_MODE_H */
