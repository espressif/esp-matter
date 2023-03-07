

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_MODIFIED_H
#define AMQP_DEFINITIONS_MODIFIED_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct MODIFIED_INSTANCE_TAG* MODIFIED_HANDLE;

    MOCKABLE_FUNCTION(, MODIFIED_HANDLE, modified_create );
    MOCKABLE_FUNCTION(, MODIFIED_HANDLE, modified_clone, MODIFIED_HANDLE, value);
    MOCKABLE_FUNCTION(, void, modified_destroy, MODIFIED_HANDLE, modified);
    MOCKABLE_FUNCTION(, bool, is_modified_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_modified, AMQP_VALUE, value, MODIFIED_HANDLE*, MODIFIED_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_modified, MODIFIED_HANDLE, modified);

    MOCKABLE_FUNCTION(, int, modified_get_delivery_failed, MODIFIED_HANDLE, modified, bool*, delivery_failed_value);
    MOCKABLE_FUNCTION(, int, modified_set_delivery_failed, MODIFIED_HANDLE, modified, bool, delivery_failed_value);
    MOCKABLE_FUNCTION(, int, modified_get_undeliverable_here, MODIFIED_HANDLE, modified, bool*, undeliverable_here_value);
    MOCKABLE_FUNCTION(, int, modified_set_undeliverable_here, MODIFIED_HANDLE, modified, bool, undeliverable_here_value);
    MOCKABLE_FUNCTION(, int, modified_get_message_annotations, MODIFIED_HANDLE, modified, fields*, message_annotations_value);
    MOCKABLE_FUNCTION(, int, modified_set_message_annotations, MODIFIED_HANDLE, modified, fields, message_annotations_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_MODIFIED_H */
