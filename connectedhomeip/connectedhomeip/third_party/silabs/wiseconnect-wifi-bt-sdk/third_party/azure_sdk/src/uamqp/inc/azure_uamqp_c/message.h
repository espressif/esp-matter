// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MESSAGE_H
#define MESSAGE_H

#include "azure_macro_utils/macro_utils.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/amqp_definitions_annotations.h"
#include "azure_uamqp_c/amqp_definitions_message_annotations.h"
#include "azure_uamqp_c/amqp_definitions_sequence_no.h"
#include "azure_uamqp_c/amqp_definitions_properties.h"
#include "azure_uamqp_c/amqp_definitions_milliseconds.h"
#include "azure_uamqp_c/amqp_definitions_header.h"
#include "azure_uamqp_c/amqp_definitions_delivery_annotations.h"


#ifdef __cplusplus
extern "C" {
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

    #define MESSAGE_BODY_TYPE_VALUES \
        MESSAGE_BODY_TYPE_NONE, \
        MESSAGE_BODY_TYPE_DATA, \
        MESSAGE_BODY_TYPE_SEQUENCE, \
        MESSAGE_BODY_TYPE_VALUE

    MU_DEFINE_ENUM(MESSAGE_BODY_TYPE, MESSAGE_BODY_TYPE_VALUES)

    typedef struct MESSAGE_INSTANCE_TAG* MESSAGE_HANDLE;
    typedef struct BINARY_DATA_TAG
    {
        const unsigned char* bytes;
        size_t length;
    } BINARY_DATA;

    MOCKABLE_FUNCTION(, MESSAGE_HANDLE, message_create);
    MOCKABLE_FUNCTION(, MESSAGE_HANDLE, message_clone, MESSAGE_HANDLE, source_message);
    MOCKABLE_FUNCTION(, void, message_destroy, MESSAGE_HANDLE, message);
    MOCKABLE_FUNCTION(, int, message_set_header, MESSAGE_HANDLE, message, HEADER_HANDLE, message_header);
    MOCKABLE_FUNCTION(, int, message_get_header, MESSAGE_HANDLE, message, HEADER_HANDLE*, message_header);
    MOCKABLE_FUNCTION(, int, message_set_delivery_annotations, MESSAGE_HANDLE, message, delivery_annotations, annotations);
    MOCKABLE_FUNCTION(, int, message_get_delivery_annotations, MESSAGE_HANDLE, message, delivery_annotations*, annotations);
    MOCKABLE_FUNCTION(, int, message_set_message_annotations, MESSAGE_HANDLE, message, message_annotations, annotations);
    MOCKABLE_FUNCTION(, int, message_get_message_annotations, MESSAGE_HANDLE, message, message_annotations*, annotations);
    MOCKABLE_FUNCTION(, int, message_set_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE, properties);
    MOCKABLE_FUNCTION(, int, message_get_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE*, properties);
    MOCKABLE_FUNCTION(, int, message_set_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE, application_properties);
    MOCKABLE_FUNCTION(, int, message_get_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE*, application_properties);
    MOCKABLE_FUNCTION(, int, message_set_footer, MESSAGE_HANDLE, message, annotations, footer);
    MOCKABLE_FUNCTION(, int, message_get_footer, MESSAGE_HANDLE, message, annotations*, footer);
    MOCKABLE_FUNCTION(, int, message_add_body_amqp_data, MESSAGE_HANDLE, message, BINARY_DATA, amqp_data);
    MOCKABLE_FUNCTION(, int, message_get_body_amqp_data_in_place, MESSAGE_HANDLE, message, size_t, index, BINARY_DATA*, amqp_data);
    MOCKABLE_FUNCTION(, int, message_get_body_amqp_data_count, MESSAGE_HANDLE, message, size_t*, count);
    MOCKABLE_FUNCTION(, int, message_set_body_amqp_value, MESSAGE_HANDLE, message, AMQP_VALUE, body_amqp_value);
    MOCKABLE_FUNCTION(, int, message_get_body_amqp_value_in_place, MESSAGE_HANDLE, message, AMQP_VALUE*, body_amqp_value);
    MOCKABLE_FUNCTION(, int, message_add_body_amqp_sequence, MESSAGE_HANDLE, message, AMQP_VALUE, sequence);
    MOCKABLE_FUNCTION(, int, message_get_body_amqp_sequence_in_place, MESSAGE_HANDLE, message, size_t, index, AMQP_VALUE*, sequence);
    MOCKABLE_FUNCTION(, int, message_get_body_amqp_sequence_count, MESSAGE_HANDLE, message, size_t*, count);
    MOCKABLE_FUNCTION(, int, message_get_body_type, MESSAGE_HANDLE, message, MESSAGE_BODY_TYPE*, body_type);
    MOCKABLE_FUNCTION(, int, message_set_message_format, MESSAGE_HANDLE, message, uint32_t, message_format);
    MOCKABLE_FUNCTION(, int, message_get_message_format, MESSAGE_HANDLE, message, uint32_t*, message_format);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MESSAGE_H */
