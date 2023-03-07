

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_CODE_H
#define AMQP_DEFINITIONS_SASL_CODE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef uint8_t sasl_code;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_code, sasl_code, value);


    #define amqpvalue_get_sasl_code amqpvalue_get_ubyte

    #define sasl_code_ok 0
    #define sasl_code_auth 1
    #define sasl_code_sys 2
    #define sasl_code_sys_perm 3
    #define sasl_code_sys_temp 4


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_CODE_H */
