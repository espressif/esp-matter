

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_FIELDS_H
#define AMQP_DEFINITIONS_FIELDS_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef AMQP_VALUE fields;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_fields, AMQP_VALUE, value);
    #define fields_clone amqpvalue_clone
    #define fields_destroy amqpvalue_destroy


    #define amqpvalue_get_fields amqpvalue_get_map



#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_FIELDS_H */
