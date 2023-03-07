

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_PROPERTIES_H
#define AMQP_DEFINITIONS_PROPERTIES_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct PROPERTIES_INSTANCE_TAG* PROPERTIES_HANDLE;

    MOCKABLE_FUNCTION(, PROPERTIES_HANDLE, properties_create );
    MOCKABLE_FUNCTION(, PROPERTIES_HANDLE, properties_clone, PROPERTIES_HANDLE, value);
    MOCKABLE_FUNCTION(, void, properties_destroy, PROPERTIES_HANDLE, properties);
    MOCKABLE_FUNCTION(, bool, is_properties_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_properties, AMQP_VALUE, value, PROPERTIES_HANDLE*, PROPERTIES_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_properties, PROPERTIES_HANDLE, properties);

    MOCKABLE_FUNCTION(, int, properties_get_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, message_id_value);
    MOCKABLE_FUNCTION(, int, properties_set_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, message_id_value);
    MOCKABLE_FUNCTION(, int, properties_get_user_id, PROPERTIES_HANDLE, properties, amqp_binary*, user_id_value);
    MOCKABLE_FUNCTION(, int, properties_set_user_id, PROPERTIES_HANDLE, properties, amqp_binary, user_id_value);
    MOCKABLE_FUNCTION(, int, properties_get_to, PROPERTIES_HANDLE, properties, AMQP_VALUE*, to_value);
    MOCKABLE_FUNCTION(, int, properties_set_to, PROPERTIES_HANDLE, properties, AMQP_VALUE, to_value);
    MOCKABLE_FUNCTION(, int, properties_get_subject, PROPERTIES_HANDLE, properties, const char**, subject_value);
    MOCKABLE_FUNCTION(, int, properties_set_subject, PROPERTIES_HANDLE, properties, const char*, subject_value);
    MOCKABLE_FUNCTION(, int, properties_get_reply_to, PROPERTIES_HANDLE, properties, AMQP_VALUE*, reply_to_value);
    MOCKABLE_FUNCTION(, int, properties_set_reply_to, PROPERTIES_HANDLE, properties, AMQP_VALUE, reply_to_value);
    MOCKABLE_FUNCTION(, int, properties_get_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, correlation_id_value);
    MOCKABLE_FUNCTION(, int, properties_set_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, correlation_id_value);
    MOCKABLE_FUNCTION(, int, properties_get_content_type, PROPERTIES_HANDLE, properties, const char**, content_type_value);
    MOCKABLE_FUNCTION(, int, properties_set_content_type, PROPERTIES_HANDLE, properties, const char*, content_type_value);
    MOCKABLE_FUNCTION(, int, properties_get_content_encoding, PROPERTIES_HANDLE, properties, const char**, content_encoding_value);
    MOCKABLE_FUNCTION(, int, properties_set_content_encoding, PROPERTIES_HANDLE, properties, const char*, content_encoding_value);
    MOCKABLE_FUNCTION(, int, properties_get_absolute_expiry_time, PROPERTIES_HANDLE, properties, timestamp*, absolute_expiry_time_value);
    MOCKABLE_FUNCTION(, int, properties_set_absolute_expiry_time, PROPERTIES_HANDLE, properties, timestamp, absolute_expiry_time_value);
    MOCKABLE_FUNCTION(, int, properties_get_creation_time, PROPERTIES_HANDLE, properties, timestamp*, creation_time_value);
    MOCKABLE_FUNCTION(, int, properties_set_creation_time, PROPERTIES_HANDLE, properties, timestamp, creation_time_value);
    MOCKABLE_FUNCTION(, int, properties_get_group_id, PROPERTIES_HANDLE, properties, const char**, group_id_value);
    MOCKABLE_FUNCTION(, int, properties_set_group_id, PROPERTIES_HANDLE, properties, const char*, group_id_value);
    MOCKABLE_FUNCTION(, int, properties_get_group_sequence, PROPERTIES_HANDLE, properties, sequence_no*, group_sequence_value);
    MOCKABLE_FUNCTION(, int, properties_set_group_sequence, PROPERTIES_HANDLE, properties, sequence_no, group_sequence_value);
    MOCKABLE_FUNCTION(, int, properties_get_reply_to_group_id, PROPERTIES_HANDLE, properties, const char**, reply_to_group_id_value);
    MOCKABLE_FUNCTION(, int, properties_set_reply_to_group_id, PROPERTIES_HANDLE, properties, const char*, reply_to_group_id_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_PROPERTIES_H */
