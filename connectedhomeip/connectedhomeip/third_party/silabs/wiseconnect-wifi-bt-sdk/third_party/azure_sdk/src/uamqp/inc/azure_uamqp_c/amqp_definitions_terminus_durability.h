

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_TERMINUS_DURABILITY_H
#define AMQP_DEFINITIONS_TERMINUS_DURABILITY_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef uint32_t terminus_durability;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_terminus_durability, terminus_durability, value);


    #define amqpvalue_get_terminus_durability amqpvalue_get_uint

    #define terminus_durability_none 0
    #define terminus_durability_configuration 1
    #define terminus_durability_unsettled_state 2


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_TERMINUS_DURABILITY_H */
