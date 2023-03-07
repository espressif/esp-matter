

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_OUTCOME_H
#define AMQP_DEFINITIONS_SASL_OUTCOME_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SASL_OUTCOME_INSTANCE_TAG* SASL_OUTCOME_HANDLE;

    MOCKABLE_FUNCTION(, SASL_OUTCOME_HANDLE, sasl_outcome_create , sasl_code, code_value);
    MOCKABLE_FUNCTION(, SASL_OUTCOME_HANDLE, sasl_outcome_clone, SASL_OUTCOME_HANDLE, value);
    MOCKABLE_FUNCTION(, void, sasl_outcome_destroy, SASL_OUTCOME_HANDLE, sasl_outcome);
    MOCKABLE_FUNCTION(, bool, is_sasl_outcome_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_sasl_outcome, AMQP_VALUE, value, SASL_OUTCOME_HANDLE*, SASL_OUTCOME_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_outcome, SASL_OUTCOME_HANDLE, sasl_outcome);

    MOCKABLE_FUNCTION(, int, sasl_outcome_get_code, SASL_OUTCOME_HANDLE, sasl_outcome, sasl_code*, code_value);
    MOCKABLE_FUNCTION(, int, sasl_outcome_set_code, SASL_OUTCOME_HANDLE, sasl_outcome, sasl_code, code_value);
    MOCKABLE_FUNCTION(, int, sasl_outcome_get_additional_data, SASL_OUTCOME_HANDLE, sasl_outcome, amqp_binary*, additional_data_value);
    MOCKABLE_FUNCTION(, int, sasl_outcome_set_additional_data, SASL_OUTCOME_HANDLE, sasl_outcome, amqp_binary, additional_data_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_OUTCOME_H */
