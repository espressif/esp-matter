

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_TARGET_H
#define AMQP_DEFINITIONS_TARGET_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct TARGET_INSTANCE_TAG* TARGET_HANDLE;

    MOCKABLE_FUNCTION(, TARGET_HANDLE, target_create );
    MOCKABLE_FUNCTION(, TARGET_HANDLE, target_clone, TARGET_HANDLE, value);
    MOCKABLE_FUNCTION(, void, target_destroy, TARGET_HANDLE, target);
    MOCKABLE_FUNCTION(, bool, is_target_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_target, AMQP_VALUE, value, TARGET_HANDLE*, TARGET_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_target, TARGET_HANDLE, target);

    MOCKABLE_FUNCTION(, int, target_get_address, TARGET_HANDLE, target, AMQP_VALUE*, address_value);
    MOCKABLE_FUNCTION(, int, target_set_address, TARGET_HANDLE, target, AMQP_VALUE, address_value);
    MOCKABLE_FUNCTION(, int, target_get_durable, TARGET_HANDLE, target, terminus_durability*, durable_value);
    MOCKABLE_FUNCTION(, int, target_set_durable, TARGET_HANDLE, target, terminus_durability, durable_value);
    MOCKABLE_FUNCTION(, int, target_get_expiry_policy, TARGET_HANDLE, target, terminus_expiry_policy*, expiry_policy_value);
    MOCKABLE_FUNCTION(, int, target_set_expiry_policy, TARGET_HANDLE, target, terminus_expiry_policy, expiry_policy_value);
    MOCKABLE_FUNCTION(, int, target_get_timeout, TARGET_HANDLE, target, seconds*, timeout_value);
    MOCKABLE_FUNCTION(, int, target_set_timeout, TARGET_HANDLE, target, seconds, timeout_value);
    MOCKABLE_FUNCTION(, int, target_get_dynamic, TARGET_HANDLE, target, bool*, dynamic_value);
    MOCKABLE_FUNCTION(, int, target_set_dynamic, TARGET_HANDLE, target, bool, dynamic_value);
    MOCKABLE_FUNCTION(, int, target_get_dynamic_node_properties, TARGET_HANDLE, target, node_properties*, dynamic_node_properties_value);
    MOCKABLE_FUNCTION(, int, target_set_dynamic_node_properties, TARGET_HANDLE, target, node_properties, dynamic_node_properties_value);
    MOCKABLE_FUNCTION(, int, target_get_capabilities, TARGET_HANDLE, target, AMQP_VALUE*, capabilities_value);
    MOCKABLE_FUNCTION(, int, target_set_capabilities, TARGET_HANDLE, target, AMQP_VALUE, capabilities_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_TARGET_H */
