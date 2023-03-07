

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SOURCE_H
#define AMQP_DEFINITIONS_SOURCE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SOURCE_INSTANCE_TAG* SOURCE_HANDLE;

    MOCKABLE_FUNCTION(, SOURCE_HANDLE, source_create );
    MOCKABLE_FUNCTION(, SOURCE_HANDLE, source_clone, SOURCE_HANDLE, value);
    MOCKABLE_FUNCTION(, void, source_destroy, SOURCE_HANDLE, source);
    MOCKABLE_FUNCTION(, bool, is_source_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_source, AMQP_VALUE, value, SOURCE_HANDLE*, SOURCE_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_source, SOURCE_HANDLE, source);

    MOCKABLE_FUNCTION(, int, source_get_address, SOURCE_HANDLE, source, AMQP_VALUE*, address_value);
    MOCKABLE_FUNCTION(, int, source_set_address, SOURCE_HANDLE, source, AMQP_VALUE, address_value);
    MOCKABLE_FUNCTION(, int, source_get_durable, SOURCE_HANDLE, source, terminus_durability*, durable_value);
    MOCKABLE_FUNCTION(, int, source_set_durable, SOURCE_HANDLE, source, terminus_durability, durable_value);
    MOCKABLE_FUNCTION(, int, source_get_expiry_policy, SOURCE_HANDLE, source, terminus_expiry_policy*, expiry_policy_value);
    MOCKABLE_FUNCTION(, int, source_set_expiry_policy, SOURCE_HANDLE, source, terminus_expiry_policy, expiry_policy_value);
    MOCKABLE_FUNCTION(, int, source_get_timeout, SOURCE_HANDLE, source, seconds*, timeout_value);
    MOCKABLE_FUNCTION(, int, source_set_timeout, SOURCE_HANDLE, source, seconds, timeout_value);
    MOCKABLE_FUNCTION(, int, source_get_dynamic, SOURCE_HANDLE, source, bool*, dynamic_value);
    MOCKABLE_FUNCTION(, int, source_set_dynamic, SOURCE_HANDLE, source, bool, dynamic_value);
    MOCKABLE_FUNCTION(, int, source_get_dynamic_node_properties, SOURCE_HANDLE, source, node_properties*, dynamic_node_properties_value);
    MOCKABLE_FUNCTION(, int, source_set_dynamic_node_properties, SOURCE_HANDLE, source, node_properties, dynamic_node_properties_value);
    MOCKABLE_FUNCTION(, int, source_get_distribution_mode, SOURCE_HANDLE, source, const char**, distribution_mode_value);
    MOCKABLE_FUNCTION(, int, source_set_distribution_mode, SOURCE_HANDLE, source, const char*, distribution_mode_value);
    MOCKABLE_FUNCTION(, int, source_get_filter, SOURCE_HANDLE, source, filter_set*, filter_value);
    MOCKABLE_FUNCTION(, int, source_set_filter, SOURCE_HANDLE, source, filter_set, filter_value);
    MOCKABLE_FUNCTION(, int, source_get_default_outcome, SOURCE_HANDLE, source, AMQP_VALUE*, default_outcome_value);
    MOCKABLE_FUNCTION(, int, source_set_default_outcome, SOURCE_HANDLE, source, AMQP_VALUE, default_outcome_value);
    MOCKABLE_FUNCTION(, int, source_get_outcomes, SOURCE_HANDLE, source, AMQP_VALUE*, outcomes_value);
    MOCKABLE_FUNCTION(, int, source_set_outcomes, SOURCE_HANDLE, source, AMQP_VALUE, outcomes_value);
    MOCKABLE_FUNCTION(, int, source_get_capabilities, SOURCE_HANDLE, source, AMQP_VALUE*, capabilities_value);
    MOCKABLE_FUNCTION(, int, source_set_capabilities, SOURCE_HANDLE, source, AMQP_VALUE, capabilities_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SOURCE_H */
