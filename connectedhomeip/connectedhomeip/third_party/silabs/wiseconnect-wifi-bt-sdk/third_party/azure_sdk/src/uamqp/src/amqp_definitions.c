

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#include <stdlib.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/amqp_definitions.h"
#include <stdlib.h>
#include <stdbool.h>

/* role */

AMQP_VALUE amqpvalue_create_role(role value)
{
    return amqpvalue_create_boolean(value);
}

/* sender-settle-mode */

AMQP_VALUE amqpvalue_create_sender_settle_mode(sender_settle_mode value)
{
    return amqpvalue_create_ubyte(value);
}

/* receiver-settle-mode */

AMQP_VALUE amqpvalue_create_receiver_settle_mode(receiver_settle_mode value)
{
    return amqpvalue_create_ubyte(value);
}

/* handle */

AMQP_VALUE amqpvalue_create_handle(handle value)
{
    return amqpvalue_create_uint(value);
}

/* seconds */

AMQP_VALUE amqpvalue_create_seconds(seconds value)
{
    return amqpvalue_create_uint(value);
}

/* milliseconds */

AMQP_VALUE amqpvalue_create_milliseconds(milliseconds value)
{
    return amqpvalue_create_uint(value);
}

/* delivery-tag */

AMQP_VALUE amqpvalue_create_delivery_tag(delivery_tag value)
{
    return amqpvalue_create_binary(value);
}

/* sequence-no */

AMQP_VALUE amqpvalue_create_sequence_no(sequence_no value)
{
    return amqpvalue_create_uint(value);
}

/* delivery-number */

AMQP_VALUE amqpvalue_create_delivery_number(delivery_number value)
{
    return amqpvalue_create_sequence_no(value);
}

/* transfer-number */

AMQP_VALUE amqpvalue_create_transfer_number(transfer_number value)
{
    return amqpvalue_create_sequence_no(value);
}

/* message-format */

AMQP_VALUE amqpvalue_create_message_format(message_format value)
{
    return amqpvalue_create_uint(value);
}

/* ietf-language-tag */

AMQP_VALUE amqpvalue_create_ietf_language_tag(ietf_language_tag value)
{
    return amqpvalue_create_symbol(value);
}

/* fields */

AMQP_VALUE amqpvalue_create_fields(AMQP_VALUE value)
{
    return amqpvalue_clone(value);
}

/* error */

typedef struct ERROR_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} ERROR_INSTANCE;

static ERROR_HANDLE error_create_internal(void)
{
    ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)malloc(sizeof(ERROR_INSTANCE));
    if (error_instance != NULL)
    {
        error_instance->composite_value = NULL;
    }

    return error_instance;
}

ERROR_HANDLE error_create(const char* condition_value)
{
    ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)malloc(sizeof(ERROR_INSTANCE));
    if (error_instance != NULL)
    {
        error_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(29);
        if (error_instance->composite_value == NULL)
        {
            free(error_instance);
            error_instance = NULL;
        }
        else
        {
            AMQP_VALUE condition_amqp_value;
            int result = 0;

            condition_amqp_value = amqpvalue_create_symbol(condition_value);
            if ((result == 0) && (amqpvalue_set_composite_item(error_instance->composite_value, 0, condition_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(condition_amqp_value);
        }
    }

    return error_instance;
}

ERROR_HANDLE error_clone(ERROR_HANDLE value)
{
    ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)malloc(sizeof(ERROR_INSTANCE));
    if (error_instance != NULL)
    {
        error_instance->composite_value = amqpvalue_clone(((ERROR_INSTANCE*)value)->composite_value);
        if (error_instance->composite_value == NULL)
        {
            free(error_instance);
            error_instance = NULL;
        }
    }

    return error_instance;
}

void error_destroy(ERROR_HANDLE error)
{
    if (error != NULL)
    {
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        amqpvalue_destroy(error_instance->composite_value);
        free(error_instance);
    }
}

AMQP_VALUE amqpvalue_create_error(ERROR_HANDLE error)
{
    AMQP_VALUE result;

    if (error == NULL)
    {
        result = NULL;
    }
    else
    {
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        result = amqpvalue_clone(error_instance->composite_value);
    }

    return result;
}

bool is_error_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 29))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_error(AMQP_VALUE value, ERROR_HANDLE* error_handle)
{
    int result;
    ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error_create_internal();
    *error_handle = error_instance;
    if (*error_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            error_destroy(*error_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* condition */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                error_destroy(*error_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                error_destroy(*error_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                const char* condition;
                                if (amqpvalue_get_symbol(item_value, &condition) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    error_destroy(*error_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* description */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* description;
                                if (amqpvalue_get_string(item_value, &description) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    error_destroy(*error_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* info */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields info;
                                if (amqpvalue_get_fields(item_value, &info) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    error_destroy(*error_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    error_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int error_get_condition(ERROR_HANDLE error, const char** condition_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        if (amqpvalue_get_composite_item_count(error_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(error_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_symbol(item_value, condition_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int error_set_condition(ERROR_HANDLE error, const char* condition_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        AMQP_VALUE condition_amqp_value = amqpvalue_create_symbol(condition_value);
        if (condition_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(error_instance->composite_value, 0, condition_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(condition_amqp_value);
        }
    }

    return result;
}

int error_get_description(ERROR_HANDLE error, const char** description_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        if (amqpvalue_get_composite_item_count(error_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(error_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, description_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int error_set_description(ERROR_HANDLE error, const char* description_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        AMQP_VALUE description_amqp_value = amqpvalue_create_string(description_value);
        if (description_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(error_instance->composite_value, 1, description_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(description_amqp_value);
        }
    }

    return result;
}

int error_get_info(ERROR_HANDLE error, fields* info_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        if (amqpvalue_get_composite_item_count(error_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(error_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, info_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int error_set_info(ERROR_HANDLE error, fields info_value)
{
    int result;

    if (error == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ERROR_INSTANCE* error_instance = (ERROR_INSTANCE*)error;
        AMQP_VALUE info_amqp_value = amqpvalue_create_fields(info_value);
        if (info_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(error_instance->composite_value, 2, info_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(info_amqp_value);
        }
    }

    return result;
}


/* amqp-error */

AMQP_VALUE amqpvalue_create_amqp_error(amqp_error value)
{
    return amqpvalue_create_symbol(value);
}

/* connection-error */

AMQP_VALUE amqpvalue_create_connection_error(connection_error value)
{
    return amqpvalue_create_symbol(value);
}

/* session-error */

AMQP_VALUE amqpvalue_create_session_error(session_error value)
{
    return amqpvalue_create_symbol(value);
}

/* link-error */

AMQP_VALUE amqpvalue_create_link_error(link_error value)
{
    return amqpvalue_create_symbol(value);
}

/* open */

typedef struct OPEN_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} OPEN_INSTANCE;

static OPEN_HANDLE open_create_internal(void)
{
    OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)malloc(sizeof(OPEN_INSTANCE));
    if (open_instance != NULL)
    {
        open_instance->composite_value = NULL;
    }

    return open_instance;
}

OPEN_HANDLE open_create(const char* container_id_value)
{
    OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)malloc(sizeof(OPEN_INSTANCE));
    if (open_instance != NULL)
    {
        open_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(16);
        if (open_instance->composite_value == NULL)
        {
            free(open_instance);
            open_instance = NULL;
        }
        else
        {
            AMQP_VALUE container_id_amqp_value;
            int result = 0;

            container_id_amqp_value = amqpvalue_create_string(container_id_value);
            if ((result == 0) && (amqpvalue_set_composite_item(open_instance->composite_value, 0, container_id_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(container_id_amqp_value);
        }
    }

    return open_instance;
}

OPEN_HANDLE open_clone(OPEN_HANDLE value)
{
    OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)malloc(sizeof(OPEN_INSTANCE));
    if (open_instance != NULL)
    {
        open_instance->composite_value = amqpvalue_clone(((OPEN_INSTANCE*)value)->composite_value);
        if (open_instance->composite_value == NULL)
        {
            free(open_instance);
            open_instance = NULL;
        }
    }

    return open_instance;
}

void open_destroy(OPEN_HANDLE open)
{
    if (open != NULL)
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        amqpvalue_destroy(open_instance->composite_value);
        free(open_instance);
    }
}

AMQP_VALUE amqpvalue_create_open(OPEN_HANDLE open)
{
    AMQP_VALUE result;

    if (open == NULL)
    {
        result = NULL;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        result = amqpvalue_clone(open_instance->composite_value);
    }

    return result;
}

bool is_open_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 16))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_open(AMQP_VALUE value, OPEN_HANDLE* open_handle)
{
    int result;
    OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open_create_internal();
    *open_handle = open_instance;
    if (*open_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            open_destroy(*open_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* container-id */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                open_destroy(*open_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                open_destroy(*open_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                const char* container_id;
                                if (amqpvalue_get_string(item_value, &container_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* hostname */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* hostname;
                                if (amqpvalue_get_string(item_value, &hostname) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* max-frame-size */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint32_t max_frame_size;
                                if (amqpvalue_get_uint(item_value, &max_frame_size) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* channel-max */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint16_t channel_max;
                                if (amqpvalue_get_ushort(item_value, &channel_max) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* idle-time-out */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                milliseconds idle_time_out;
                                if (amqpvalue_get_milliseconds(item_value, &idle_time_out) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* outgoing-locales */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ietf_language_tag outgoing_locales = NULL;
                                AMQP_VALUE outgoing_locales_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &outgoing_locales_array) != 0)) &&
                                    (amqpvalue_get_ietf_language_tag(item_value, &outgoing_locales) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* incoming-locales */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ietf_language_tag incoming_locales = NULL;
                                AMQP_VALUE incoming_locales_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &incoming_locales_array) != 0)) &&
                                    (amqpvalue_get_ietf_language_tag(item_value, &incoming_locales) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* offered-capabilities */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* offered_capabilities = NULL;
                                AMQP_VALUE offered_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &offered_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &offered_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* desired-capabilities */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* desired_capabilities = NULL;
                                AMQP_VALUE desired_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &desired_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &desired_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* properties */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields properties;
                                if (amqpvalue_get_fields(item_value, &properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    open_destroy(*open_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    open_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int open_get_container_id(OPEN_HANDLE open, const char** container_id_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, container_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_container_id(OPEN_HANDLE open, const char* container_id_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE container_id_amqp_value = amqpvalue_create_string(container_id_value);
        if (container_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 0, container_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(container_id_amqp_value);
        }
    }

    return result;
}

int open_get_hostname(OPEN_HANDLE open, const char** hostname_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, hostname_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_hostname(OPEN_HANDLE open, const char* hostname_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE hostname_amqp_value = amqpvalue_create_string(hostname_value);
        if (hostname_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 1, hostname_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(hostname_amqp_value);
        }
    }

    return result;
}

int open_get_max_frame_size(OPEN_HANDLE open, uint32_t* max_frame_size_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                *max_frame_size_value = 4294967295u;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *max_frame_size_value = 4294967295u;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, max_frame_size_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *max_frame_size_value = 4294967295u;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_max_frame_size(OPEN_HANDLE open, uint32_t max_frame_size_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE max_frame_size_amqp_value = amqpvalue_create_uint(max_frame_size_value);
        if (max_frame_size_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 2, max_frame_size_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(max_frame_size_amqp_value);
        }
    }

    return result;
}

int open_get_channel_max(OPEN_HANDLE open, uint16_t* channel_max_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *channel_max_value = 65535;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *channel_max_value = 65535;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_ushort(item_value, channel_max_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *channel_max_value = 65535;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_channel_max(OPEN_HANDLE open, uint16_t channel_max_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE channel_max_amqp_value = amqpvalue_create_ushort(channel_max_value);
        if (channel_max_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 3, channel_max_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(channel_max_amqp_value);
        }
    }

    return result;
}

int open_get_idle_time_out(OPEN_HANDLE open, milliseconds* idle_time_out_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_milliseconds(item_value, idle_time_out_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_idle_time_out(OPEN_HANDLE open, milliseconds idle_time_out_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE idle_time_out_amqp_value = amqpvalue_create_milliseconds(idle_time_out_value);
        if (idle_time_out_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 4, idle_time_out_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(idle_time_out_amqp_value);
        }
    }

    return result;
}

int open_get_outgoing_locales(OPEN_HANDLE open, AMQP_VALUE* outgoing_locales_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    ietf_language_tag outgoing_locales_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_ietf_language_tag(item_value, &outgoing_locales_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&outgoing_locales_single_value, 0, sizeof(outgoing_locales_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, outgoing_locales_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *outgoing_locales_value = amqpvalue_create_array();
                            if (*outgoing_locales_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_ietf_language_tag(outgoing_locales_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*outgoing_locales_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*outgoing_locales_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*outgoing_locales_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(open_instance->composite_value, 5, *outgoing_locales_value) != 0)
                                        {
                                            amqpvalue_destroy(*outgoing_locales_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*outgoing_locales_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int open_set_outgoing_locales(OPEN_HANDLE open, AMQP_VALUE outgoing_locales_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE outgoing_locales_amqp_value;
        if (outgoing_locales_value == NULL)
        {
            outgoing_locales_amqp_value = NULL;
        }
        else
        {
            outgoing_locales_amqp_value = amqpvalue_clone(outgoing_locales_value);
        }
        if (outgoing_locales_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 5, outgoing_locales_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(outgoing_locales_amqp_value);
        }
    }

    return result;
}

int open_get_incoming_locales(OPEN_HANDLE open, AMQP_VALUE* incoming_locales_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    ietf_language_tag incoming_locales_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_ietf_language_tag(item_value, &incoming_locales_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&incoming_locales_single_value, 0, sizeof(incoming_locales_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, incoming_locales_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *incoming_locales_value = amqpvalue_create_array();
                            if (*incoming_locales_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_ietf_language_tag(incoming_locales_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*incoming_locales_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*incoming_locales_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*incoming_locales_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(open_instance->composite_value, 6, *incoming_locales_value) != 0)
                                        {
                                            amqpvalue_destroy(*incoming_locales_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*incoming_locales_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int open_set_incoming_locales(OPEN_HANDLE open, AMQP_VALUE incoming_locales_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE incoming_locales_amqp_value;
        if (incoming_locales_value == NULL)
        {
            incoming_locales_amqp_value = NULL;
        }
        else
        {
            incoming_locales_amqp_value = amqpvalue_clone(incoming_locales_value);
        }
        if (incoming_locales_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 6, incoming_locales_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(incoming_locales_amqp_value);
        }
    }

    return result;
}

int open_get_offered_capabilities(OPEN_HANDLE open, AMQP_VALUE* offered_capabilities_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* offered_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &offered_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&offered_capabilities_single_value, 0, sizeof(offered_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, offered_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *offered_capabilities_value = amqpvalue_create_array();
                            if (*offered_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(offered_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*offered_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*offered_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*offered_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(open_instance->composite_value, 7, *offered_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*offered_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*offered_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int open_set_offered_capabilities(OPEN_HANDLE open, AMQP_VALUE offered_capabilities_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE offered_capabilities_amqp_value;
        if (offered_capabilities_value == NULL)
        {
            offered_capabilities_amqp_value = NULL;
        }
        else
        {
            offered_capabilities_amqp_value = amqpvalue_clone(offered_capabilities_value);
        }
        if (offered_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 7, offered_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(offered_capabilities_amqp_value);
        }
    }

    return result;
}

int open_get_desired_capabilities(OPEN_HANDLE open, AMQP_VALUE* desired_capabilities_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* desired_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &desired_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&desired_capabilities_single_value, 0, sizeof(desired_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, desired_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *desired_capabilities_value = amqpvalue_create_array();
                            if (*desired_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(desired_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*desired_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*desired_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*desired_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(open_instance->composite_value, 8, *desired_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*desired_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*desired_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int open_set_desired_capabilities(OPEN_HANDLE open, AMQP_VALUE desired_capabilities_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE desired_capabilities_amqp_value;
        if (desired_capabilities_value == NULL)
        {
            desired_capabilities_amqp_value = NULL;
        }
        else
        {
            desired_capabilities_amqp_value = amqpvalue_clone(desired_capabilities_value);
        }
        if (desired_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 8, desired_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(desired_capabilities_amqp_value);
        }
    }

    return result;
}

int open_get_properties(OPEN_HANDLE open, fields* properties_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        if (amqpvalue_get_composite_item_count(open_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(open_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int open_set_properties(OPEN_HANDLE open, fields properties_value)
{
    int result;

    if (open == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        OPEN_INSTANCE* open_instance = (OPEN_INSTANCE*)open;
        AMQP_VALUE properties_amqp_value = amqpvalue_create_fields(properties_value);
        if (properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(open_instance->composite_value, 9, properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(properties_amqp_value);
        }
    }

    return result;
}


/* begin */

typedef struct BEGIN_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} BEGIN_INSTANCE;

static BEGIN_HANDLE begin_create_internal(void)
{
    BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)malloc(sizeof(BEGIN_INSTANCE));
    if (begin_instance != NULL)
    {
        begin_instance->composite_value = NULL;
    }

    return begin_instance;
}

BEGIN_HANDLE begin_create(transfer_number next_outgoing_id_value, uint32_t incoming_window_value, uint32_t outgoing_window_value)
{
    BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)malloc(sizeof(BEGIN_INSTANCE));
    if (begin_instance != NULL)
    {
        begin_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(17);
        if (begin_instance->composite_value == NULL)
        {
            free(begin_instance);
            begin_instance = NULL;
        }
        else
        {
            AMQP_VALUE next_outgoing_id_amqp_value;
            AMQP_VALUE incoming_window_amqp_value;
            AMQP_VALUE outgoing_window_amqp_value;
            int result = 0;

            next_outgoing_id_amqp_value = amqpvalue_create_transfer_number(next_outgoing_id_value);
            if ((result == 0) && (amqpvalue_set_composite_item(begin_instance->composite_value, 1, next_outgoing_id_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            incoming_window_amqp_value = amqpvalue_create_uint(incoming_window_value);
            if ((result == 0) && (amqpvalue_set_composite_item(begin_instance->composite_value, 2, incoming_window_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            outgoing_window_amqp_value = amqpvalue_create_uint(outgoing_window_value);
            if ((result == 0) && (amqpvalue_set_composite_item(begin_instance->composite_value, 3, outgoing_window_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(next_outgoing_id_amqp_value);
            amqpvalue_destroy(incoming_window_amqp_value);
            amqpvalue_destroy(outgoing_window_amqp_value);
        }
    }

    return begin_instance;
}

BEGIN_HANDLE begin_clone(BEGIN_HANDLE value)
{
    BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)malloc(sizeof(BEGIN_INSTANCE));
    if (begin_instance != NULL)
    {
        begin_instance->composite_value = amqpvalue_clone(((BEGIN_INSTANCE*)value)->composite_value);
        if (begin_instance->composite_value == NULL)
        {
            free(begin_instance);
            begin_instance = NULL;
        }
    }

    return begin_instance;
}

void begin_destroy(BEGIN_HANDLE begin)
{
    if (begin != NULL)
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        amqpvalue_destroy(begin_instance->composite_value);
        free(begin_instance);
    }
}

AMQP_VALUE amqpvalue_create_begin(BEGIN_HANDLE begin)
{
    AMQP_VALUE result;

    if (begin == NULL)
    {
        result = NULL;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        result = amqpvalue_clone(begin_instance->composite_value);
    }

    return result;
}

bool is_begin_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 17))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_begin(AMQP_VALUE value, BEGIN_HANDLE* begin_handle)
{
    int result;
    BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin_create_internal();
    *begin_handle = begin_instance;
    if (*begin_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            begin_destroy(*begin_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* remote-channel */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint16_t remote_channel;
                                if (amqpvalue_get_ushort(item_value, &remote_channel) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* next-outgoing-id */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            {
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                transfer_number next_outgoing_id;
                                if (amqpvalue_get_transfer_number(item_value, &next_outgoing_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* incoming-window */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            {
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint32_t incoming_window;
                                if (amqpvalue_get_uint(item_value, &incoming_window) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* outgoing-window */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            {
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                begin_destroy(*begin_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint32_t outgoing_window;
                                if (amqpvalue_get_uint(item_value, &outgoing_window) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* handle-max */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                handle handle_max;
                                if (amqpvalue_get_handle(item_value, &handle_max) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* offered-capabilities */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* offered_capabilities = NULL;
                                AMQP_VALUE offered_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &offered_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &offered_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* desired-capabilities */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* desired_capabilities = NULL;
                                AMQP_VALUE desired_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &desired_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &desired_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* properties */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields properties;
                                if (amqpvalue_get_fields(item_value, &properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    begin_destroy(*begin_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    begin_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int begin_get_remote_channel(BEGIN_HANDLE begin, uint16_t* remote_channel_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_ushort(item_value, remote_channel_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_remote_channel(BEGIN_HANDLE begin, uint16_t remote_channel_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE remote_channel_amqp_value = amqpvalue_create_ushort(remote_channel_value);
        if (remote_channel_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 0, remote_channel_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(remote_channel_amqp_value);
        }
    }

    return result;
}

int begin_get_next_outgoing_id(BEGIN_HANDLE begin, transfer_number* next_outgoing_id_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_transfer_number(item_value, next_outgoing_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_next_outgoing_id(BEGIN_HANDLE begin, transfer_number next_outgoing_id_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE next_outgoing_id_amqp_value = amqpvalue_create_transfer_number(next_outgoing_id_value);
        if (next_outgoing_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 1, next_outgoing_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(next_outgoing_id_amqp_value);
        }
    }

    return result;
}

int begin_get_incoming_window(BEGIN_HANDLE begin, uint32_t* incoming_window_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, incoming_window_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_incoming_window(BEGIN_HANDLE begin, uint32_t incoming_window_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE incoming_window_amqp_value = amqpvalue_create_uint(incoming_window_value);
        if (incoming_window_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 2, incoming_window_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(incoming_window_amqp_value);
        }
    }

    return result;
}

int begin_get_outgoing_window(BEGIN_HANDLE begin, uint32_t* outgoing_window_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, outgoing_window_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_outgoing_window(BEGIN_HANDLE begin, uint32_t outgoing_window_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE outgoing_window_amqp_value = amqpvalue_create_uint(outgoing_window_value);
        if (outgoing_window_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 3, outgoing_window_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(outgoing_window_amqp_value);
        }
    }

    return result;
}

int begin_get_handle_max(BEGIN_HANDLE begin, handle* handle_max_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                *handle_max_value = 4294967295u;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *handle_max_value = 4294967295u;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_handle(item_value, handle_max_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *handle_max_value = 4294967295u;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_handle_max(BEGIN_HANDLE begin, handle handle_max_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE handle_max_amqp_value = amqpvalue_create_handle(handle_max_value);
        if (handle_max_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 4, handle_max_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(handle_max_amqp_value);
        }
    }

    return result;
}

int begin_get_offered_capabilities(BEGIN_HANDLE begin, AMQP_VALUE* offered_capabilities_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* offered_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &offered_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&offered_capabilities_single_value, 0, sizeof(offered_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, offered_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *offered_capabilities_value = amqpvalue_create_array();
                            if (*offered_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(offered_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*offered_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*offered_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*offered_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(begin_instance->composite_value, 5, *offered_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*offered_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*offered_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_offered_capabilities(BEGIN_HANDLE begin, AMQP_VALUE offered_capabilities_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE offered_capabilities_amqp_value;
        if (offered_capabilities_value == NULL)
        {
            offered_capabilities_amqp_value = NULL;
        }
        else
        {
            offered_capabilities_amqp_value = amqpvalue_clone(offered_capabilities_value);
        }
        if (offered_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 5, offered_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(offered_capabilities_amqp_value);
        }
    }

    return result;
}

int begin_get_desired_capabilities(BEGIN_HANDLE begin, AMQP_VALUE* desired_capabilities_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* desired_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &desired_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&desired_capabilities_single_value, 0, sizeof(desired_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, desired_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *desired_capabilities_value = amqpvalue_create_array();
                            if (*desired_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(desired_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*desired_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*desired_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*desired_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(begin_instance->composite_value, 6, *desired_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*desired_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*desired_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_desired_capabilities(BEGIN_HANDLE begin, AMQP_VALUE desired_capabilities_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE desired_capabilities_amqp_value;
        if (desired_capabilities_value == NULL)
        {
            desired_capabilities_amqp_value = NULL;
        }
        else
        {
            desired_capabilities_amqp_value = amqpvalue_clone(desired_capabilities_value);
        }
        if (desired_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 6, desired_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(desired_capabilities_amqp_value);
        }
    }

    return result;
}

int begin_get_properties(BEGIN_HANDLE begin, fields* properties_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        if (amqpvalue_get_composite_item_count(begin_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(begin_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int begin_set_properties(BEGIN_HANDLE begin, fields properties_value)
{
    int result;

    if (begin == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        BEGIN_INSTANCE* begin_instance = (BEGIN_INSTANCE*)begin;
        AMQP_VALUE properties_amqp_value = amqpvalue_create_fields(properties_value);
        if (properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(begin_instance->composite_value, 7, properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(properties_amqp_value);
        }
    }

    return result;
}


/* attach */

typedef struct ATTACH_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} ATTACH_INSTANCE;

static ATTACH_HANDLE attach_create_internal(void)
{
    ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)malloc(sizeof(ATTACH_INSTANCE));
    if (attach_instance != NULL)
    {
        attach_instance->composite_value = NULL;
    }

    return attach_instance;
}

ATTACH_HANDLE attach_create(const char* name_value, handle handle_value, role role_value)
{
    ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)malloc(sizeof(ATTACH_INSTANCE));
    if (attach_instance != NULL)
    {
        attach_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(18);
        if (attach_instance->composite_value == NULL)
        {
            free(attach_instance);
            attach_instance = NULL;
        }
        else
        {
            AMQP_VALUE name_amqp_value;
            AMQP_VALUE handle_amqp_value;
            AMQP_VALUE role_amqp_value;
            int result = 0;

            name_amqp_value = amqpvalue_create_string(name_value);
            if ((result == 0) && (amqpvalue_set_composite_item(attach_instance->composite_value, 0, name_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            handle_amqp_value = amqpvalue_create_handle(handle_value);
            if ((result == 0) && (amqpvalue_set_composite_item(attach_instance->composite_value, 1, handle_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            role_amqp_value = amqpvalue_create_role(role_value);
            if ((result == 0) && (amqpvalue_set_composite_item(attach_instance->composite_value, 2, role_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(name_amqp_value);
            amqpvalue_destroy(handle_amqp_value);
            amqpvalue_destroy(role_amqp_value);
        }
    }

    return attach_instance;
}

ATTACH_HANDLE attach_clone(ATTACH_HANDLE value)
{
    ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)malloc(sizeof(ATTACH_INSTANCE));
    if (attach_instance != NULL)
    {
        attach_instance->composite_value = amqpvalue_clone(((ATTACH_INSTANCE*)value)->composite_value);
        if (attach_instance->composite_value == NULL)
        {
            free(attach_instance);
            attach_instance = NULL;
        }
    }

    return attach_instance;
}

void attach_destroy(ATTACH_HANDLE attach)
{
    if (attach != NULL)
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        amqpvalue_destroy(attach_instance->composite_value);
        free(attach_instance);
    }
}

AMQP_VALUE amqpvalue_create_attach(ATTACH_HANDLE attach)
{
    AMQP_VALUE result;

    if (attach == NULL)
    {
        result = NULL;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        result = amqpvalue_clone(attach_instance->composite_value);
    }

    return result;
}

bool is_attach_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 18))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_attach(AMQP_VALUE value, ATTACH_HANDLE* attach_handle)
{
    int result;
    ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach_create_internal();
    *attach_handle = attach_instance;
    if (*attach_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            attach_destroy(*attach_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* name */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                const char* name;
                                if (amqpvalue_get_string(item_value, &name) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* handle */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            {
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                handle handle;
                                if (amqpvalue_get_handle(item_value, &handle) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* role */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            {
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                attach_destroy(*attach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                role role;
                                if (amqpvalue_get_role(item_value, &role) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* snd-settle-mode */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                sender_settle_mode snd_settle_mode;
                                if (amqpvalue_get_sender_settle_mode(item_value, &snd_settle_mode) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* rcv-settle-mode */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                receiver_settle_mode rcv_settle_mode;
                                if (amqpvalue_get_receiver_settle_mode(item_value, &rcv_settle_mode) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* source */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* target */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* unsettled */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                AMQP_VALUE unsettled;
                                if (amqpvalue_get_map(item_value, &unsettled) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* incomplete-unsettled */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool incomplete_unsettled;
                                if (amqpvalue_get_boolean(item_value, &incomplete_unsettled) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* initial-delivery-count */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                sequence_no initial_delivery_count;
                                if (amqpvalue_get_sequence_no(item_value, &initial_delivery_count) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* max-message-size */
                    if (list_item_count > 10)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 10);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint64_t max_message_size;
                                if (amqpvalue_get_ulong(item_value, &max_message_size) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* offered-capabilities */
                    if (list_item_count > 11)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 11);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* offered_capabilities = NULL;
                                AMQP_VALUE offered_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &offered_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &offered_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* desired-capabilities */
                    if (list_item_count > 12)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 12);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* desired_capabilities = NULL;
                                AMQP_VALUE desired_capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &desired_capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &desired_capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* properties */
                    if (list_item_count > 13)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 13);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields properties;
                                if (amqpvalue_get_fields(item_value, &properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    attach_destroy(*attach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    attach_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int attach_get_name(ATTACH_HANDLE attach, const char** name_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, name_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_name(ATTACH_HANDLE attach, const char* name_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE name_amqp_value = amqpvalue_create_string(name_value);
        if (name_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 0, name_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(name_amqp_value);
        }
    }

    return result;
}

int attach_get_handle(ATTACH_HANDLE attach, handle* handle_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_handle(item_value, handle_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_handle(ATTACH_HANDLE attach, handle handle_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE handle_amqp_value = amqpvalue_create_handle(handle_value);
        if (handle_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 1, handle_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return result;
}

int attach_get_role(ATTACH_HANDLE attach, role* role_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_role(item_value, role_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_role(ATTACH_HANDLE attach, role role_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE role_amqp_value = amqpvalue_create_role(role_value);
        if (role_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 2, role_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(role_amqp_value);
        }
    }

    return result;
}

int attach_get_snd_settle_mode(ATTACH_HANDLE attach, sender_settle_mode* snd_settle_mode_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *snd_settle_mode_value = sender_settle_mode_mixed;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *snd_settle_mode_value = sender_settle_mode_mixed;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_sender_settle_mode(item_value, snd_settle_mode_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *snd_settle_mode_value = sender_settle_mode_mixed;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_snd_settle_mode(ATTACH_HANDLE attach, sender_settle_mode snd_settle_mode_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE snd_settle_mode_amqp_value = amqpvalue_create_sender_settle_mode(snd_settle_mode_value);
        if (snd_settle_mode_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 3, snd_settle_mode_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(snd_settle_mode_amqp_value);
        }
    }

    return result;
}

int attach_get_rcv_settle_mode(ATTACH_HANDLE attach, receiver_settle_mode* rcv_settle_mode_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                *rcv_settle_mode_value = receiver_settle_mode_first;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *rcv_settle_mode_value = receiver_settle_mode_first;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_receiver_settle_mode(item_value, rcv_settle_mode_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *rcv_settle_mode_value = receiver_settle_mode_first;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_rcv_settle_mode(ATTACH_HANDLE attach, receiver_settle_mode rcv_settle_mode_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE rcv_settle_mode_amqp_value = amqpvalue_create_receiver_settle_mode(rcv_settle_mode_value);
        if (rcv_settle_mode_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 4, rcv_settle_mode_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(rcv_settle_mode_amqp_value);
        }
    }

    return result;
}

int attach_get_source(ATTACH_HANDLE attach, AMQP_VALUE* source_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *source_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int attach_set_source(ATTACH_HANDLE attach, AMQP_VALUE source_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE source_amqp_value;
        if (source_value == NULL)
        {
            source_amqp_value = NULL;
        }
        else
        {
            source_amqp_value = amqpvalue_clone(source_value);
        }
        if (source_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 5, source_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(source_amqp_value);
        }
    }

    return result;
}

int attach_get_target(ATTACH_HANDLE attach, AMQP_VALUE* target_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *target_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int attach_set_target(ATTACH_HANDLE attach, AMQP_VALUE target_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE target_amqp_value;
        if (target_value == NULL)
        {
            target_amqp_value = NULL;
        }
        else
        {
            target_amqp_value = amqpvalue_clone(target_value);
        }
        if (target_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 6, target_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(target_amqp_value);
        }
    }

    return result;
}

int attach_get_unsettled(ATTACH_HANDLE attach, AMQP_VALUE* unsettled_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_map(item_value, unsettled_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_unsettled(ATTACH_HANDLE attach, AMQP_VALUE unsettled_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE unsettled_amqp_value;
        if (unsettled_value == NULL)
        {
            unsettled_amqp_value = NULL;
        }
        else
        {
            unsettled_amqp_value = amqpvalue_clone(unsettled_value);
        }
        if (unsettled_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 7, unsettled_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(unsettled_amqp_value);
        }
    }

    return result;
}

int attach_get_incomplete_unsettled(ATTACH_HANDLE attach, bool* incomplete_unsettled_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                *incomplete_unsettled_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *incomplete_unsettled_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, incomplete_unsettled_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *incomplete_unsettled_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_incomplete_unsettled(ATTACH_HANDLE attach, bool incomplete_unsettled_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE incomplete_unsettled_amqp_value = amqpvalue_create_boolean(incomplete_unsettled_value);
        if (incomplete_unsettled_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 8, incomplete_unsettled_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(incomplete_unsettled_amqp_value);
        }
    }

    return result;
}

int attach_get_initial_delivery_count(ATTACH_HANDLE attach, sequence_no* initial_delivery_count_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_sequence_no(item_value, initial_delivery_count_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_initial_delivery_count(ATTACH_HANDLE attach, sequence_no initial_delivery_count_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE initial_delivery_count_amqp_value = amqpvalue_create_sequence_no(initial_delivery_count_value);
        if (initial_delivery_count_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 9, initial_delivery_count_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(initial_delivery_count_amqp_value);
        }
    }

    return result;
}

int attach_get_max_message_size(ATTACH_HANDLE attach, uint64_t* max_message_size_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 10)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 10);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_ulong(item_value, max_message_size_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_max_message_size(ATTACH_HANDLE attach, uint64_t max_message_size_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE max_message_size_amqp_value = amqpvalue_create_ulong(max_message_size_value);
        if (max_message_size_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 10, max_message_size_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(max_message_size_amqp_value);
        }
    }

    return result;
}

int attach_get_offered_capabilities(ATTACH_HANDLE attach, AMQP_VALUE* offered_capabilities_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 11)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 11);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* offered_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &offered_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&offered_capabilities_single_value, 0, sizeof(offered_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, offered_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *offered_capabilities_value = amqpvalue_create_array();
                            if (*offered_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(offered_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*offered_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*offered_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*offered_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(attach_instance->composite_value, 11, *offered_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*offered_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*offered_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_offered_capabilities(ATTACH_HANDLE attach, AMQP_VALUE offered_capabilities_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE offered_capabilities_amqp_value;
        if (offered_capabilities_value == NULL)
        {
            offered_capabilities_amqp_value = NULL;
        }
        else
        {
            offered_capabilities_amqp_value = amqpvalue_clone(offered_capabilities_value);
        }
        if (offered_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 11, offered_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(offered_capabilities_amqp_value);
        }
    }

    return result;
}

int attach_get_desired_capabilities(ATTACH_HANDLE attach, AMQP_VALUE* desired_capabilities_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 12)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 12);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* desired_capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &desired_capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&desired_capabilities_single_value, 0, sizeof(desired_capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, desired_capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *desired_capabilities_value = amqpvalue_create_array();
                            if (*desired_capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(desired_capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*desired_capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*desired_capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*desired_capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(attach_instance->composite_value, 12, *desired_capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*desired_capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*desired_capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_desired_capabilities(ATTACH_HANDLE attach, AMQP_VALUE desired_capabilities_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE desired_capabilities_amqp_value;
        if (desired_capabilities_value == NULL)
        {
            desired_capabilities_amqp_value = NULL;
        }
        else
        {
            desired_capabilities_amqp_value = amqpvalue_clone(desired_capabilities_value);
        }
        if (desired_capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 12, desired_capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(desired_capabilities_amqp_value);
        }
    }

    return result;
}

int attach_get_properties(ATTACH_HANDLE attach, fields* properties_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        if (amqpvalue_get_composite_item_count(attach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 13)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(attach_instance->composite_value, 13);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int attach_set_properties(ATTACH_HANDLE attach, fields properties_value)
{
    int result;

    if (attach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        ATTACH_INSTANCE* attach_instance = (ATTACH_INSTANCE*)attach;
        AMQP_VALUE properties_amqp_value = amqpvalue_create_fields(properties_value);
        if (properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(attach_instance->composite_value, 13, properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(properties_amqp_value);
        }
    }

    return result;
}


/* flow */

typedef struct FLOW_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} FLOW_INSTANCE;

static FLOW_HANDLE flow_create_internal(void)
{
    FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)malloc(sizeof(FLOW_INSTANCE));
    if (flow_instance != NULL)
    {
        flow_instance->composite_value = NULL;
    }

    return flow_instance;
}

FLOW_HANDLE flow_create(uint32_t incoming_window_value, transfer_number next_outgoing_id_value, uint32_t outgoing_window_value)
{
    FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)malloc(sizeof(FLOW_INSTANCE));
    if (flow_instance != NULL)
    {
        flow_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(19);
        if (flow_instance->composite_value == NULL)
        {
            free(flow_instance);
            flow_instance = NULL;
        }
        else
        {
            AMQP_VALUE incoming_window_amqp_value;
            AMQP_VALUE next_outgoing_id_amqp_value;
            AMQP_VALUE outgoing_window_amqp_value;
            int result = 0;

            incoming_window_amqp_value = amqpvalue_create_uint(incoming_window_value);
            if ((result == 0) && (amqpvalue_set_composite_item(flow_instance->composite_value, 1, incoming_window_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            next_outgoing_id_amqp_value = amqpvalue_create_transfer_number(next_outgoing_id_value);
            if ((result == 0) && (amqpvalue_set_composite_item(flow_instance->composite_value, 2, next_outgoing_id_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            outgoing_window_amqp_value = amqpvalue_create_uint(outgoing_window_value);
            if ((result == 0) && (amqpvalue_set_composite_item(flow_instance->composite_value, 3, outgoing_window_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(incoming_window_amqp_value);
            amqpvalue_destroy(next_outgoing_id_amqp_value);
            amqpvalue_destroy(outgoing_window_amqp_value);
        }
    }

    return flow_instance;
}

FLOW_HANDLE flow_clone(FLOW_HANDLE value)
{
    FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)malloc(sizeof(FLOW_INSTANCE));
    if (flow_instance != NULL)
    {
        flow_instance->composite_value = amqpvalue_clone(((FLOW_INSTANCE*)value)->composite_value);
        if (flow_instance->composite_value == NULL)
        {
            free(flow_instance);
            flow_instance = NULL;
        }
    }

    return flow_instance;
}

void flow_destroy(FLOW_HANDLE flow)
{
    if (flow != NULL)
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        amqpvalue_destroy(flow_instance->composite_value);
        free(flow_instance);
    }
}

AMQP_VALUE amqpvalue_create_flow(FLOW_HANDLE flow)
{
    AMQP_VALUE result;

    if (flow == NULL)
    {
        result = NULL;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        result = amqpvalue_clone(flow_instance->composite_value);
    }

    return result;
}

bool is_flow_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 19))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_flow(AMQP_VALUE value, FLOW_HANDLE* flow_handle)
{
    int result;
    FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow_create_internal();
    *flow_handle = flow_instance;
    if (*flow_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            flow_destroy(*flow_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* next-incoming-id */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                transfer_number next_incoming_id;
                                if (amqpvalue_get_transfer_number(item_value, &next_incoming_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* incoming-window */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            {
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint32_t incoming_window;
                                if (amqpvalue_get_uint(item_value, &incoming_window) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* next-outgoing-id */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            {
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                transfer_number next_outgoing_id;
                                if (amqpvalue_get_transfer_number(item_value, &next_outgoing_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* outgoing-window */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            {
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                flow_destroy(*flow_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint32_t outgoing_window;
                                if (amqpvalue_get_uint(item_value, &outgoing_window) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* handle */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                handle handle;
                                if (amqpvalue_get_handle(item_value, &handle) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* delivery-count */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                sequence_no delivery_count;
                                if (amqpvalue_get_sequence_no(item_value, &delivery_count) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* link-credit */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint32_t link_credit;
                                if (amqpvalue_get_uint(item_value, &link_credit) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* available */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint32_t available;
                                if (amqpvalue_get_uint(item_value, &available) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* drain */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool drain;
                                if (amqpvalue_get_boolean(item_value, &drain) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* echo */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool echo;
                                if (amqpvalue_get_boolean(item_value, &echo) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* properties */
                    if (list_item_count > 10)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 10);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields properties;
                                if (amqpvalue_get_fields(item_value, &properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    flow_destroy(*flow_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    flow_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int flow_get_next_incoming_id(FLOW_HANDLE flow, transfer_number* next_incoming_id_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_transfer_number(item_value, next_incoming_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_next_incoming_id(FLOW_HANDLE flow, transfer_number next_incoming_id_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE next_incoming_id_amqp_value = amqpvalue_create_transfer_number(next_incoming_id_value);
        if (next_incoming_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 0, next_incoming_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(next_incoming_id_amqp_value);
        }
    }

    return result;
}

int flow_get_incoming_window(FLOW_HANDLE flow, uint32_t* incoming_window_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, incoming_window_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_incoming_window(FLOW_HANDLE flow, uint32_t incoming_window_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE incoming_window_amqp_value = amqpvalue_create_uint(incoming_window_value);
        if (incoming_window_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 1, incoming_window_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(incoming_window_amqp_value);
        }
    }

    return result;
}

int flow_get_next_outgoing_id(FLOW_HANDLE flow, transfer_number* next_outgoing_id_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_transfer_number(item_value, next_outgoing_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_next_outgoing_id(FLOW_HANDLE flow, transfer_number next_outgoing_id_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE next_outgoing_id_amqp_value = amqpvalue_create_transfer_number(next_outgoing_id_value);
        if (next_outgoing_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 2, next_outgoing_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(next_outgoing_id_amqp_value);
        }
    }

    return result;
}

int flow_get_outgoing_window(FLOW_HANDLE flow, uint32_t* outgoing_window_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, outgoing_window_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_outgoing_window(FLOW_HANDLE flow, uint32_t outgoing_window_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE outgoing_window_amqp_value = amqpvalue_create_uint(outgoing_window_value);
        if (outgoing_window_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 3, outgoing_window_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(outgoing_window_amqp_value);
        }
    }

    return result;
}

int flow_get_handle(FLOW_HANDLE flow, handle* handle_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_handle(item_value, handle_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_handle(FLOW_HANDLE flow, handle handle_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE handle_amqp_value = amqpvalue_create_handle(handle_value);
        if (handle_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 4, handle_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return result;
}

int flow_get_delivery_count(FLOW_HANDLE flow, sequence_no* delivery_count_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_sequence_no(item_value, delivery_count_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_delivery_count(FLOW_HANDLE flow, sequence_no delivery_count_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE delivery_count_amqp_value = amqpvalue_create_sequence_no(delivery_count_value);
        if (delivery_count_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 5, delivery_count_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(delivery_count_amqp_value);
        }
    }

    return result;
}

int flow_get_link_credit(FLOW_HANDLE flow, uint32_t* link_credit_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, link_credit_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_link_credit(FLOW_HANDLE flow, uint32_t link_credit_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE link_credit_amqp_value = amqpvalue_create_uint(link_credit_value);
        if (link_credit_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 6, link_credit_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(link_credit_amqp_value);
        }
    }

    return result;
}

int flow_get_available(FLOW_HANDLE flow, uint32_t* available_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, available_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_available(FLOW_HANDLE flow, uint32_t available_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE available_amqp_value = amqpvalue_create_uint(available_value);
        if (available_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 7, available_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(available_amqp_value);
        }
    }

    return result;
}

int flow_get_drain(FLOW_HANDLE flow, bool* drain_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                *drain_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *drain_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, drain_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *drain_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_drain(FLOW_HANDLE flow, bool drain_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE drain_amqp_value = amqpvalue_create_boolean(drain_value);
        if (drain_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 8, drain_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(drain_amqp_value);
        }
    }

    return result;
}

int flow_get_echo(FLOW_HANDLE flow, bool* echo_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                *echo_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *echo_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, echo_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *echo_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_echo(FLOW_HANDLE flow, bool echo_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE echo_amqp_value = amqpvalue_create_boolean(echo_value);
        if (echo_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 9, echo_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(echo_amqp_value);
        }
    }

    return result;
}

int flow_get_properties(FLOW_HANDLE flow, fields* properties_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        if (amqpvalue_get_composite_item_count(flow_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 10)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(flow_instance->composite_value, 10);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int flow_set_properties(FLOW_HANDLE flow, fields properties_value)
{
    int result;

    if (flow == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        FLOW_INSTANCE* flow_instance = (FLOW_INSTANCE*)flow;
        AMQP_VALUE properties_amqp_value = amqpvalue_create_fields(properties_value);
        if (properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(flow_instance->composite_value, 10, properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(properties_amqp_value);
        }
    }

    return result;
}


/* transfer */

typedef struct TRANSFER_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} TRANSFER_INSTANCE;

static TRANSFER_HANDLE transfer_create_internal(void)
{
    TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)malloc(sizeof(TRANSFER_INSTANCE));
    if (transfer_instance != NULL)
    {
        transfer_instance->composite_value = NULL;
    }

    return transfer_instance;
}

TRANSFER_HANDLE transfer_create(handle handle_value)
{
    TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)malloc(sizeof(TRANSFER_INSTANCE));
    if (transfer_instance != NULL)
    {
        transfer_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(20);
        if (transfer_instance->composite_value == NULL)
        {
            free(transfer_instance);
            transfer_instance = NULL;
        }
        else
        {
            AMQP_VALUE handle_amqp_value;
            int result = 0;

            handle_amqp_value = amqpvalue_create_handle(handle_value);
            if ((result == 0) && (amqpvalue_set_composite_item(transfer_instance->composite_value, 0, handle_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return transfer_instance;
}

TRANSFER_HANDLE transfer_clone(TRANSFER_HANDLE value)
{
    TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)malloc(sizeof(TRANSFER_INSTANCE));
    if (transfer_instance != NULL)
    {
        transfer_instance->composite_value = amqpvalue_clone(((TRANSFER_INSTANCE*)value)->composite_value);
        if (transfer_instance->composite_value == NULL)
        {
            free(transfer_instance);
            transfer_instance = NULL;
        }
    }

    return transfer_instance;
}

void transfer_destroy(TRANSFER_HANDLE transfer)
{
    if (transfer != NULL)
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        amqpvalue_destroy(transfer_instance->composite_value);
        free(transfer_instance);
    }
}

AMQP_VALUE amqpvalue_create_transfer(TRANSFER_HANDLE transfer)
{
    AMQP_VALUE result;

    if (transfer == NULL)
    {
        result = NULL;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        result = amqpvalue_clone(transfer_instance->composite_value);
    }

    return result;
}

bool is_transfer_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 20))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_transfer(AMQP_VALUE value, TRANSFER_HANDLE* transfer_handle)
{
    int result;
    TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer_create_internal();
    *transfer_handle = transfer_instance;
    if (*transfer_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            transfer_destroy(*transfer_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* handle */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                transfer_destroy(*transfer_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                transfer_destroy(*transfer_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                handle handle;
                                if (amqpvalue_get_handle(item_value, &handle) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* delivery-id */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                delivery_number delivery_id;
                                if (amqpvalue_get_delivery_number(item_value, &delivery_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* delivery-tag */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                delivery_tag delivery_tag;
                                if (amqpvalue_get_delivery_tag(item_value, &delivery_tag) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* message-format */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                message_format message_format;
                                if (amqpvalue_get_message_format(item_value, &message_format) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* settled */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool settled;
                                if (amqpvalue_get_boolean(item_value, &settled) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* more */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool more;
                                if (amqpvalue_get_boolean(item_value, &more) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* rcv-settle-mode */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                receiver_settle_mode rcv_settle_mode;
                                if (amqpvalue_get_receiver_settle_mode(item_value, &rcv_settle_mode) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* state */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* resume */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool resume;
                                if (amqpvalue_get_boolean(item_value, &resume) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* aborted */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool aborted;
                                if (amqpvalue_get_boolean(item_value, &aborted) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* batchable */
                    if (list_item_count > 10)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 10);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool batchable;
                                if (amqpvalue_get_boolean(item_value, &batchable) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    transfer_destroy(*transfer_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    transfer_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int transfer_get_handle(TRANSFER_HANDLE transfer, handle* handle_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_handle(item_value, handle_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_handle(TRANSFER_HANDLE transfer, handle handle_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE handle_amqp_value = amqpvalue_create_handle(handle_value);
        if (handle_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 0, handle_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return result;
}

int transfer_get_delivery_id(TRANSFER_HANDLE transfer, delivery_number* delivery_id_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_delivery_number(item_value, delivery_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_delivery_id(TRANSFER_HANDLE transfer, delivery_number delivery_id_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE delivery_id_amqp_value = amqpvalue_create_delivery_number(delivery_id_value);
        if (delivery_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 1, delivery_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(delivery_id_amqp_value);
        }
    }

    return result;
}

int transfer_get_delivery_tag(TRANSFER_HANDLE transfer, delivery_tag* delivery_tag_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_delivery_tag(item_value, delivery_tag_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_delivery_tag(TRANSFER_HANDLE transfer, delivery_tag delivery_tag_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE delivery_tag_amqp_value = amqpvalue_create_delivery_tag(delivery_tag_value);
        if (delivery_tag_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 2, delivery_tag_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(delivery_tag_amqp_value);
        }
    }

    return result;
}

int transfer_get_message_format(TRANSFER_HANDLE transfer, message_format* message_format_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_message_format(item_value, message_format_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_message_format(TRANSFER_HANDLE transfer, message_format message_format_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE message_format_amqp_value = amqpvalue_create_message_format(message_format_value);
        if (message_format_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 3, message_format_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(message_format_amqp_value);
        }
    }

    return result;
}

int transfer_get_settled(TRANSFER_HANDLE transfer, bool* settled_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, settled_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_settled(TRANSFER_HANDLE transfer, bool settled_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE settled_amqp_value = amqpvalue_create_boolean(settled_value);
        if (settled_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 4, settled_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(settled_amqp_value);
        }
    }

    return result;
}

int transfer_get_more(TRANSFER_HANDLE transfer, bool* more_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                *more_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *more_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, more_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *more_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_more(TRANSFER_HANDLE transfer, bool more_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE more_amqp_value = amqpvalue_create_boolean(more_value);
        if (more_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 5, more_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(more_amqp_value);
        }
    }

    return result;
}

int transfer_get_rcv_settle_mode(TRANSFER_HANDLE transfer, receiver_settle_mode* rcv_settle_mode_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_receiver_settle_mode(item_value, rcv_settle_mode_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_rcv_settle_mode(TRANSFER_HANDLE transfer, receiver_settle_mode rcv_settle_mode_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE rcv_settle_mode_amqp_value = amqpvalue_create_receiver_settle_mode(rcv_settle_mode_value);
        if (rcv_settle_mode_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 6, rcv_settle_mode_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(rcv_settle_mode_amqp_value);
        }
    }

    return result;
}

int transfer_get_state(TRANSFER_HANDLE transfer, AMQP_VALUE* state_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *state_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int transfer_set_state(TRANSFER_HANDLE transfer, AMQP_VALUE state_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE state_amqp_value;
        if (state_value == NULL)
        {
            state_amqp_value = NULL;
        }
        else
        {
            state_amqp_value = amqpvalue_clone(state_value);
        }
        if (state_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 7, state_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(state_amqp_value);
        }
    }

    return result;
}

int transfer_get_resume(TRANSFER_HANDLE transfer, bool* resume_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                *resume_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *resume_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, resume_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *resume_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_resume(TRANSFER_HANDLE transfer, bool resume_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE resume_amqp_value = amqpvalue_create_boolean(resume_value);
        if (resume_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 8, resume_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(resume_amqp_value);
        }
    }

    return result;
}

int transfer_get_aborted(TRANSFER_HANDLE transfer, bool* aborted_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                *aborted_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *aborted_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, aborted_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *aborted_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_aborted(TRANSFER_HANDLE transfer, bool aborted_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE aborted_amqp_value = amqpvalue_create_boolean(aborted_value);
        if (aborted_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 9, aborted_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(aborted_amqp_value);
        }
    }

    return result;
}

int transfer_get_batchable(TRANSFER_HANDLE transfer, bool* batchable_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        if (amqpvalue_get_composite_item_count(transfer_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 10)
            {
                *batchable_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(transfer_instance->composite_value, 10);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *batchable_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, batchable_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *batchable_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int transfer_set_batchable(TRANSFER_HANDLE transfer, bool batchable_value)
{
    int result;

    if (transfer == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TRANSFER_INSTANCE* transfer_instance = (TRANSFER_INSTANCE*)transfer;
        AMQP_VALUE batchable_amqp_value = amqpvalue_create_boolean(batchable_value);
        if (batchable_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(transfer_instance->composite_value, 10, batchable_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(batchable_amqp_value);
        }
    }

    return result;
}


/* disposition */

typedef struct DISPOSITION_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} DISPOSITION_INSTANCE;

static DISPOSITION_HANDLE disposition_create_internal(void)
{
    DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)malloc(sizeof(DISPOSITION_INSTANCE));
    if (disposition_instance != NULL)
    {
        disposition_instance->composite_value = NULL;
    }

    return disposition_instance;
}

DISPOSITION_HANDLE disposition_create(role role_value, delivery_number first_value)
{
    DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)malloc(sizeof(DISPOSITION_INSTANCE));
    if (disposition_instance != NULL)
    {
        disposition_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(21);
        if (disposition_instance->composite_value == NULL)
        {
            free(disposition_instance);
            disposition_instance = NULL;
        }
        else
        {
            AMQP_VALUE role_amqp_value;
            AMQP_VALUE first_amqp_value;
            int result = 0;

            role_amqp_value = amqpvalue_create_role(role_value);
            if ((result == 0) && (amqpvalue_set_composite_item(disposition_instance->composite_value, 0, role_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            first_amqp_value = amqpvalue_create_delivery_number(first_value);
            if ((result == 0) && (amqpvalue_set_composite_item(disposition_instance->composite_value, 1, first_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(role_amqp_value);
            amqpvalue_destroy(first_amqp_value);
        }
    }

    return disposition_instance;
}

DISPOSITION_HANDLE disposition_clone(DISPOSITION_HANDLE value)
{
    DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)malloc(sizeof(DISPOSITION_INSTANCE));
    if (disposition_instance != NULL)
    {
        disposition_instance->composite_value = amqpvalue_clone(((DISPOSITION_INSTANCE*)value)->composite_value);
        if (disposition_instance->composite_value == NULL)
        {
            free(disposition_instance);
            disposition_instance = NULL;
        }
    }

    return disposition_instance;
}

void disposition_destroy(DISPOSITION_HANDLE disposition)
{
    if (disposition != NULL)
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        amqpvalue_destroy(disposition_instance->composite_value);
        free(disposition_instance);
    }
}

AMQP_VALUE amqpvalue_create_disposition(DISPOSITION_HANDLE disposition)
{
    AMQP_VALUE result;

    if (disposition == NULL)
    {
        result = NULL;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        result = amqpvalue_clone(disposition_instance->composite_value);
    }

    return result;
}

bool is_disposition_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 21))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_disposition(AMQP_VALUE value, DISPOSITION_HANDLE* disposition_handle)
{
    int result;
    DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition_create_internal();
    *disposition_handle = disposition_instance;
    if (*disposition_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            disposition_destroy(*disposition_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* role */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                disposition_destroy(*disposition_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                disposition_destroy(*disposition_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                role role;
                                if (amqpvalue_get_role(item_value, &role) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    disposition_destroy(*disposition_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* first */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            {
                                disposition_destroy(*disposition_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                disposition_destroy(*disposition_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                delivery_number first;
                                if (amqpvalue_get_delivery_number(item_value, &first) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    disposition_destroy(*disposition_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* last */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                delivery_number last;
                                if (amqpvalue_get_delivery_number(item_value, &last) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    disposition_destroy(*disposition_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* settled */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool settled;
                                if (amqpvalue_get_boolean(item_value, &settled) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    disposition_destroy(*disposition_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* state */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* batchable */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool batchable;
                                if (amqpvalue_get_boolean(item_value, &batchable) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    disposition_destroy(*disposition_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    disposition_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int disposition_get_role(DISPOSITION_HANDLE disposition, role* role_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_role(item_value, role_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int disposition_set_role(DISPOSITION_HANDLE disposition, role role_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE role_amqp_value = amqpvalue_create_role(role_value);
        if (role_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 0, role_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(role_amqp_value);
        }
    }

    return result;
}

int disposition_get_first(DISPOSITION_HANDLE disposition, delivery_number* first_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_delivery_number(item_value, first_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int disposition_set_first(DISPOSITION_HANDLE disposition, delivery_number first_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE first_amqp_value = amqpvalue_create_delivery_number(first_value);
        if (first_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 1, first_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(first_amqp_value);
        }
    }

    return result;
}

int disposition_get_last(DISPOSITION_HANDLE disposition, delivery_number* last_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_delivery_number(item_value, last_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int disposition_set_last(DISPOSITION_HANDLE disposition, delivery_number last_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE last_amqp_value = amqpvalue_create_delivery_number(last_value);
        if (last_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 2, last_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(last_amqp_value);
        }
    }

    return result;
}

int disposition_get_settled(DISPOSITION_HANDLE disposition, bool* settled_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *settled_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *settled_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, settled_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *settled_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int disposition_set_settled(DISPOSITION_HANDLE disposition, bool settled_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE settled_amqp_value = amqpvalue_create_boolean(settled_value);
        if (settled_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 3, settled_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(settled_amqp_value);
        }
    }

    return result;
}

int disposition_get_state(DISPOSITION_HANDLE disposition, AMQP_VALUE* state_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *state_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int disposition_set_state(DISPOSITION_HANDLE disposition, AMQP_VALUE state_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE state_amqp_value;
        if (state_value == NULL)
        {
            state_amqp_value = NULL;
        }
        else
        {
            state_amqp_value = amqpvalue_clone(state_value);
        }
        if (state_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 4, state_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(state_amqp_value);
        }
    }

    return result;
}

int disposition_get_batchable(DISPOSITION_HANDLE disposition, bool* batchable_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        if (amqpvalue_get_composite_item_count(disposition_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                *batchable_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(disposition_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *batchable_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, batchable_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *batchable_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int disposition_set_batchable(DISPOSITION_HANDLE disposition, bool batchable_value)
{
    int result;

    if (disposition == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DISPOSITION_INSTANCE* disposition_instance = (DISPOSITION_INSTANCE*)disposition;
        AMQP_VALUE batchable_amqp_value = amqpvalue_create_boolean(batchable_value);
        if (batchable_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(disposition_instance->composite_value, 5, batchable_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(batchable_amqp_value);
        }
    }

    return result;
}


/* detach */

typedef struct DETACH_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} DETACH_INSTANCE;

static DETACH_HANDLE detach_create_internal(void)
{
    DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)malloc(sizeof(DETACH_INSTANCE));
    if (detach_instance != NULL)
    {
        detach_instance->composite_value = NULL;
    }

    return detach_instance;
}

DETACH_HANDLE detach_create(handle handle_value)
{
    DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)malloc(sizeof(DETACH_INSTANCE));
    if (detach_instance != NULL)
    {
        detach_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(22);
        if (detach_instance->composite_value == NULL)
        {
            free(detach_instance);
            detach_instance = NULL;
        }
        else
        {
            AMQP_VALUE handle_amqp_value;
            int result = 0;

            handle_amqp_value = amqpvalue_create_handle(handle_value);
            if ((result == 0) && (amqpvalue_set_composite_item(detach_instance->composite_value, 0, handle_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return detach_instance;
}

DETACH_HANDLE detach_clone(DETACH_HANDLE value)
{
    DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)malloc(sizeof(DETACH_INSTANCE));
    if (detach_instance != NULL)
    {
        detach_instance->composite_value = amqpvalue_clone(((DETACH_INSTANCE*)value)->composite_value);
        if (detach_instance->composite_value == NULL)
        {
            free(detach_instance);
            detach_instance = NULL;
        }
    }

    return detach_instance;
}

void detach_destroy(DETACH_HANDLE detach)
{
    if (detach != NULL)
    {
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        amqpvalue_destroy(detach_instance->composite_value);
        free(detach_instance);
    }
}

AMQP_VALUE amqpvalue_create_detach(DETACH_HANDLE detach)
{
    AMQP_VALUE result;

    if (detach == NULL)
    {
        result = NULL;
    }
    else
    {
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        result = amqpvalue_clone(detach_instance->composite_value);
    }

    return result;
}

bool is_detach_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 22))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_detach(AMQP_VALUE value, DETACH_HANDLE* detach_handle)
{
    int result;
    DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach_create_internal();
    *detach_handle = detach_instance;
    if (*detach_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            detach_destroy(*detach_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* handle */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                detach_destroy(*detach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                detach_destroy(*detach_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                handle handle;
                                if (amqpvalue_get_handle(item_value, &handle) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    detach_destroy(*detach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* closed */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool closed;
                                if (amqpvalue_get_boolean(item_value, &closed) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    detach_destroy(*detach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* error */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ERROR_HANDLE error;
                                if (amqpvalue_get_error(item_value, &error) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    detach_destroy(*detach_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                                else
                                {
                                    error_destroy(error);
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    detach_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int detach_get_handle(DETACH_HANDLE detach, handle* handle_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        if (amqpvalue_get_composite_item_count(detach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(detach_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_handle(item_value, handle_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int detach_set_handle(DETACH_HANDLE detach, handle handle_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        AMQP_VALUE handle_amqp_value = amqpvalue_create_handle(handle_value);
        if (handle_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(detach_instance->composite_value, 0, handle_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(handle_amqp_value);
        }
    }

    return result;
}

int detach_get_closed(DETACH_HANDLE detach, bool* closed_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        if (amqpvalue_get_composite_item_count(detach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                *closed_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(detach_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *closed_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, closed_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *closed_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int detach_set_closed(DETACH_HANDLE detach, bool closed_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        AMQP_VALUE closed_amqp_value = amqpvalue_create_boolean(closed_value);
        if (closed_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(detach_instance->composite_value, 1, closed_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(closed_amqp_value);
        }
    }

    return result;
}

int detach_get_error(DETACH_HANDLE detach, ERROR_HANDLE* error_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        if (amqpvalue_get_composite_item_count(detach_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(detach_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_error(item_value, error_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int detach_set_error(DETACH_HANDLE detach, ERROR_HANDLE error_value)
{
    int result;

    if (detach == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        DETACH_INSTANCE* detach_instance = (DETACH_INSTANCE*)detach;
        AMQP_VALUE error_amqp_value = amqpvalue_create_error(error_value);
        if (error_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(detach_instance->composite_value, 2, error_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(error_amqp_value);
        }
    }

    return result;
}


/* end */

typedef struct END_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} END_INSTANCE;

static END_HANDLE end_create_internal(void)
{
    END_INSTANCE* end_instance = (END_INSTANCE*)malloc(sizeof(END_INSTANCE));
    if (end_instance != NULL)
    {
        end_instance->composite_value = NULL;
    }

    return end_instance;
}

END_HANDLE end_create(void)
{
    END_INSTANCE* end_instance = (END_INSTANCE*)malloc(sizeof(END_INSTANCE));
    if (end_instance != NULL)
    {
        end_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(23);
        if (end_instance->composite_value == NULL)
        {
            free(end_instance);
            end_instance = NULL;
        }
    }

    return end_instance;
}

END_HANDLE end_clone(END_HANDLE value)
{
    END_INSTANCE* end_instance = (END_INSTANCE*)malloc(sizeof(END_INSTANCE));
    if (end_instance != NULL)
    {
        end_instance->composite_value = amqpvalue_clone(((END_INSTANCE*)value)->composite_value);
        if (end_instance->composite_value == NULL)
        {
            free(end_instance);
            end_instance = NULL;
        }
    }

    return end_instance;
}

void end_destroy(END_HANDLE end)
{
    if (end != NULL)
    {
        END_INSTANCE* end_instance = (END_INSTANCE*)end;
        amqpvalue_destroy(end_instance->composite_value);
        free(end_instance);
    }
}

AMQP_VALUE amqpvalue_create_end(END_HANDLE end)
{
    AMQP_VALUE result;

    if (end == NULL)
    {
        result = NULL;
    }
    else
    {
        END_INSTANCE* end_instance = (END_INSTANCE*)end;
        result = amqpvalue_clone(end_instance->composite_value);
    }

    return result;
}

bool is_end_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 23))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_end(AMQP_VALUE value, END_HANDLE* end_handle)
{
    int result;
    END_INSTANCE* end_instance = (END_INSTANCE*)end_create_internal();
    *end_handle = end_instance;
    if (*end_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            end_destroy(*end_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* error */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ERROR_HANDLE error;
                                if (amqpvalue_get_error(item_value, &error) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    end_destroy(*end_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                                else
                                {
                                    error_destroy(error);
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    end_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int end_get_error(END_HANDLE end, ERROR_HANDLE* error_value)
{
    int result;

    if (end == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        END_INSTANCE* end_instance = (END_INSTANCE*)end;
        if (amqpvalue_get_composite_item_count(end_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(end_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_error(item_value, error_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int end_set_error(END_HANDLE end, ERROR_HANDLE error_value)
{
    int result;

    if (end == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        END_INSTANCE* end_instance = (END_INSTANCE*)end;
        AMQP_VALUE error_amqp_value = amqpvalue_create_error(error_value);
        if (error_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(end_instance->composite_value, 0, error_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(error_amqp_value);
        }
    }

    return result;
}


/* close */

typedef struct CLOSE_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} CLOSE_INSTANCE;

static CLOSE_HANDLE close_create_internal(void)
{
    CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)malloc(sizeof(CLOSE_INSTANCE));
    if (close_instance != NULL)
    {
        close_instance->composite_value = NULL;
    }

    return close_instance;
}

CLOSE_HANDLE close_create(void)
{
    CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)malloc(sizeof(CLOSE_INSTANCE));
    if (close_instance != NULL)
    {
        close_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(24);
        if (close_instance->composite_value == NULL)
        {
            free(close_instance);
            close_instance = NULL;
        }
    }

    return close_instance;
}

CLOSE_HANDLE close_clone(CLOSE_HANDLE value)
{
    CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)malloc(sizeof(CLOSE_INSTANCE));
    if (close_instance != NULL)
    {
        close_instance->composite_value = amqpvalue_clone(((CLOSE_INSTANCE*)value)->composite_value);
        if (close_instance->composite_value == NULL)
        {
            free(close_instance);
            close_instance = NULL;
        }
    }

    return close_instance;
}

void close_destroy(CLOSE_HANDLE close)
{
    if (close != NULL)
    {
        CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)close;
        amqpvalue_destroy(close_instance->composite_value);
        free(close_instance);
    }
}

AMQP_VALUE amqpvalue_create_close(CLOSE_HANDLE close)
{
    AMQP_VALUE result;

    if (close == NULL)
    {
        result = NULL;
    }
    else
    {
        CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)close;
        result = amqpvalue_clone(close_instance->composite_value);
    }

    return result;
}

bool is_close_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 24))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_close(AMQP_VALUE value, CLOSE_HANDLE* close_handle)
{
    int result;
    CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)close_create_internal();
    *close_handle = close_instance;
    if (*close_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            close_destroy(*close_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* error */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ERROR_HANDLE error;
                                if (amqpvalue_get_error(item_value, &error) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    close_destroy(*close_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                                else
                                {
                                    error_destroy(error);
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    close_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int close_get_error(CLOSE_HANDLE close, ERROR_HANDLE* error_value)
{
    int result;

    if (close == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)close;
        if (amqpvalue_get_composite_item_count(close_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(close_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_error(item_value, error_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int close_set_error(CLOSE_HANDLE close, ERROR_HANDLE error_value)
{
    int result;

    if (close == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        CLOSE_INSTANCE* close_instance = (CLOSE_INSTANCE*)close;
        AMQP_VALUE error_amqp_value = amqpvalue_create_error(error_value);
        if (error_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(close_instance->composite_value, 0, error_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(error_amqp_value);
        }
    }

    return result;
}


/* sasl-code */

AMQP_VALUE amqpvalue_create_sasl_code(sasl_code value)
{
    return amqpvalue_create_ubyte(value);
}

/* sasl-mechanisms */

typedef struct SASL_MECHANISMS_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SASL_MECHANISMS_INSTANCE;

static SASL_MECHANISMS_HANDLE sasl_mechanisms_create_internal(void)
{
    SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)malloc(sizeof(SASL_MECHANISMS_INSTANCE));
    if (sasl_mechanisms_instance != NULL)
    {
        sasl_mechanisms_instance->composite_value = NULL;
    }

    return sasl_mechanisms_instance;
}

SASL_MECHANISMS_HANDLE sasl_mechanisms_create(AMQP_VALUE sasl_server_mechanisms_value)
{
    SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)malloc(sizeof(SASL_MECHANISMS_INSTANCE));
    if (sasl_mechanisms_instance != NULL)
    {
        sasl_mechanisms_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(64);
        if (sasl_mechanisms_instance->composite_value == NULL)
        {
            free(sasl_mechanisms_instance);
            sasl_mechanisms_instance = NULL;
        }
        else
        {
            AMQP_VALUE sasl_server_mechanisms_amqp_value;
            int result = 0;

            sasl_server_mechanisms_amqp_value = sasl_server_mechanisms_value;
            if ((result == 0) && (amqpvalue_set_composite_item(sasl_mechanisms_instance->composite_value, 0, sasl_server_mechanisms_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(sasl_server_mechanisms_amqp_value);
        }
    }

    return sasl_mechanisms_instance;
}

SASL_MECHANISMS_HANDLE sasl_mechanisms_clone(SASL_MECHANISMS_HANDLE value)
{
    SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)malloc(sizeof(SASL_MECHANISMS_INSTANCE));
    if (sasl_mechanisms_instance != NULL)
    {
        sasl_mechanisms_instance->composite_value = amqpvalue_clone(((SASL_MECHANISMS_INSTANCE*)value)->composite_value);
        if (sasl_mechanisms_instance->composite_value == NULL)
        {
            free(sasl_mechanisms_instance);
            sasl_mechanisms_instance = NULL;
        }
    }

    return sasl_mechanisms_instance;
}

void sasl_mechanisms_destroy(SASL_MECHANISMS_HANDLE sasl_mechanisms)
{
    if (sasl_mechanisms != NULL)
    {
        SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)sasl_mechanisms;
        amqpvalue_destroy(sasl_mechanisms_instance->composite_value);
        free(sasl_mechanisms_instance);
    }
}

AMQP_VALUE amqpvalue_create_sasl_mechanisms(SASL_MECHANISMS_HANDLE sasl_mechanisms)
{
    AMQP_VALUE result;

    if (sasl_mechanisms == NULL)
    {
        result = NULL;
    }
    else
    {
        SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)sasl_mechanisms;
        result = amqpvalue_clone(sasl_mechanisms_instance->composite_value);
    }

    return result;
}

bool is_sasl_mechanisms_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 64))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_sasl_mechanisms(AMQP_VALUE value, SASL_MECHANISMS_HANDLE* sasl_mechanisms_handle)
{
    int result;
    SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)sasl_mechanisms_create_internal();
    *sasl_mechanisms_handle = sasl_mechanisms_instance;
    if (*sasl_mechanisms_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            sasl_mechanisms_destroy(*sasl_mechanisms_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* sasl-server-mechanisms */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                sasl_mechanisms_destroy(*sasl_mechanisms_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                sasl_mechanisms_destroy(*sasl_mechanisms_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                const char* sasl_server_mechanisms = NULL;
                                AMQP_VALUE sasl_server_mechanisms_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &sasl_server_mechanisms_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &sasl_server_mechanisms) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_mechanisms_destroy(*sasl_mechanisms_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }

                    sasl_mechanisms_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int sasl_mechanisms_get_sasl_server_mechanisms(SASL_MECHANISMS_HANDLE sasl_mechanisms, AMQP_VALUE* sasl_server_mechanisms_value)
{
    int result;

    if (sasl_mechanisms == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)sasl_mechanisms;
        if (amqpvalue_get_composite_item_count(sasl_mechanisms_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_mechanisms_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* sasl_server_mechanisms_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &sasl_server_mechanisms_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&sasl_server_mechanisms_single_value, 0, sizeof(sasl_server_mechanisms_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, sasl_server_mechanisms_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *sasl_server_mechanisms_value = amqpvalue_create_array();
                            if (*sasl_server_mechanisms_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(sasl_server_mechanisms_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*sasl_server_mechanisms_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*sasl_server_mechanisms_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*sasl_server_mechanisms_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(sasl_mechanisms_instance->composite_value, 0, *sasl_server_mechanisms_value) != 0)
                                        {
                                            amqpvalue_destroy(*sasl_server_mechanisms_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*sasl_server_mechanisms_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int sasl_mechanisms_set_sasl_server_mechanisms(SASL_MECHANISMS_HANDLE sasl_mechanisms, AMQP_VALUE sasl_server_mechanisms_value)
{
    int result;

    if (sasl_mechanisms == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_MECHANISMS_INSTANCE* sasl_mechanisms_instance = (SASL_MECHANISMS_INSTANCE*)sasl_mechanisms;
        AMQP_VALUE sasl_server_mechanisms_amqp_value;
        if (sasl_server_mechanisms_value == NULL)
        {
            sasl_server_mechanisms_amqp_value = NULL;
        }
        else
        {
            sasl_server_mechanisms_amqp_value = amqpvalue_clone(sasl_server_mechanisms_value);
        }
        if (sasl_server_mechanisms_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_mechanisms_instance->composite_value, 0, sasl_server_mechanisms_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(sasl_server_mechanisms_amqp_value);
        }
    }

    return result;
}


/* sasl-init */

typedef struct SASL_INIT_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SASL_INIT_INSTANCE;

static SASL_INIT_HANDLE sasl_init_create_internal(void)
{
    SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)malloc(sizeof(SASL_INIT_INSTANCE));
    if (sasl_init_instance != NULL)
    {
        sasl_init_instance->composite_value = NULL;
    }

    return sasl_init_instance;
}

SASL_INIT_HANDLE sasl_init_create(const char* mechanism_value)
{
    SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)malloc(sizeof(SASL_INIT_INSTANCE));
    if (sasl_init_instance != NULL)
    {
        sasl_init_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(65);
        if (sasl_init_instance->composite_value == NULL)
        {
            free(sasl_init_instance);
            sasl_init_instance = NULL;
        }
        else
        {
            AMQP_VALUE mechanism_amqp_value;
            int result = 0;

            mechanism_amqp_value = amqpvalue_create_symbol(mechanism_value);
            if ((result == 0) && (amqpvalue_set_composite_item(sasl_init_instance->composite_value, 0, mechanism_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(mechanism_amqp_value);
        }
    }

    return sasl_init_instance;
}

SASL_INIT_HANDLE sasl_init_clone(SASL_INIT_HANDLE value)
{
    SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)malloc(sizeof(SASL_INIT_INSTANCE));
    if (sasl_init_instance != NULL)
    {
        sasl_init_instance->composite_value = amqpvalue_clone(((SASL_INIT_INSTANCE*)value)->composite_value);
        if (sasl_init_instance->composite_value == NULL)
        {
            free(sasl_init_instance);
            sasl_init_instance = NULL;
        }
    }

    return sasl_init_instance;
}

void sasl_init_destroy(SASL_INIT_HANDLE sasl_init)
{
    if (sasl_init != NULL)
    {
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        amqpvalue_destroy(sasl_init_instance->composite_value);
        free(sasl_init_instance);
    }
}

AMQP_VALUE amqpvalue_create_sasl_init(SASL_INIT_HANDLE sasl_init)
{
    AMQP_VALUE result;

    if (sasl_init == NULL)
    {
        result = NULL;
    }
    else
    {
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        result = amqpvalue_clone(sasl_init_instance->composite_value);
    }

    return result;
}

bool is_sasl_init_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 65))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_sasl_init(AMQP_VALUE value, SASL_INIT_HANDLE* sasl_init_handle)
{
    int result;
    SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init_create_internal();
    *sasl_init_handle = sasl_init_instance;
    if (*sasl_init_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            sasl_init_destroy(*sasl_init_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* mechanism */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                sasl_init_destroy(*sasl_init_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                sasl_init_destroy(*sasl_init_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                const char* mechanism;
                                if (amqpvalue_get_symbol(item_value, &mechanism) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_init_destroy(*sasl_init_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* initial-response */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                amqp_binary initial_response;
                                if (amqpvalue_get_binary(item_value, &initial_response) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_init_destroy(*sasl_init_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* hostname */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* hostname;
                                if (amqpvalue_get_string(item_value, &hostname) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_init_destroy(*sasl_init_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    sasl_init_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int sasl_init_get_mechanism(SASL_INIT_HANDLE sasl_init, const char** mechanism_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        if (amqpvalue_get_composite_item_count(sasl_init_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_init_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_symbol(item_value, mechanism_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_init_set_mechanism(SASL_INIT_HANDLE sasl_init, const char* mechanism_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        AMQP_VALUE mechanism_amqp_value = amqpvalue_create_symbol(mechanism_value);
        if (mechanism_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_init_instance->composite_value, 0, mechanism_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(mechanism_amqp_value);
        }
    }

    return result;
}

int sasl_init_get_initial_response(SASL_INIT_HANDLE sasl_init, amqp_binary* initial_response_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        if (amqpvalue_get_composite_item_count(sasl_init_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_init_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_binary(item_value, initial_response_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_init_set_initial_response(SASL_INIT_HANDLE sasl_init, amqp_binary initial_response_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        AMQP_VALUE initial_response_amqp_value = amqpvalue_create_binary(initial_response_value);
        if (initial_response_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_init_instance->composite_value, 1, initial_response_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(initial_response_amqp_value);
        }
    }

    return result;
}

int sasl_init_get_hostname(SASL_INIT_HANDLE sasl_init, const char** hostname_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        if (amqpvalue_get_composite_item_count(sasl_init_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_init_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, hostname_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_init_set_hostname(SASL_INIT_HANDLE sasl_init, const char* hostname_value)
{
    int result;

    if (sasl_init == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_INIT_INSTANCE* sasl_init_instance = (SASL_INIT_INSTANCE*)sasl_init;
        AMQP_VALUE hostname_amqp_value = amqpvalue_create_string(hostname_value);
        if (hostname_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_init_instance->composite_value, 2, hostname_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(hostname_amqp_value);
        }
    }

    return result;
}


/* sasl-challenge */

typedef struct SASL_CHALLENGE_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SASL_CHALLENGE_INSTANCE;

static SASL_CHALLENGE_HANDLE sasl_challenge_create_internal(void)
{
    SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)malloc(sizeof(SASL_CHALLENGE_INSTANCE));
    if (sasl_challenge_instance != NULL)
    {
        sasl_challenge_instance->composite_value = NULL;
    }

    return sasl_challenge_instance;
}

SASL_CHALLENGE_HANDLE sasl_challenge_create(amqp_binary challenge_value)
{
    SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)malloc(sizeof(SASL_CHALLENGE_INSTANCE));
    if (sasl_challenge_instance != NULL)
    {
        sasl_challenge_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(66);
        if (sasl_challenge_instance->composite_value == NULL)
        {
            free(sasl_challenge_instance);
            sasl_challenge_instance = NULL;
        }
        else
        {
            AMQP_VALUE challenge_amqp_value;
            int result = 0;

            challenge_amqp_value = amqpvalue_create_binary(challenge_value);
            if ((result == 0) && (amqpvalue_set_composite_item(sasl_challenge_instance->composite_value, 0, challenge_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(challenge_amqp_value);
        }
    }

    return sasl_challenge_instance;
}

SASL_CHALLENGE_HANDLE sasl_challenge_clone(SASL_CHALLENGE_HANDLE value)
{
    SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)malloc(sizeof(SASL_CHALLENGE_INSTANCE));
    if (sasl_challenge_instance != NULL)
    {
        sasl_challenge_instance->composite_value = amqpvalue_clone(((SASL_CHALLENGE_INSTANCE*)value)->composite_value);
        if (sasl_challenge_instance->composite_value == NULL)
        {
            free(sasl_challenge_instance);
            sasl_challenge_instance = NULL;
        }
    }

    return sasl_challenge_instance;
}

void sasl_challenge_destroy(SASL_CHALLENGE_HANDLE sasl_challenge)
{
    if (sasl_challenge != NULL)
    {
        SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)sasl_challenge;
        amqpvalue_destroy(sasl_challenge_instance->composite_value);
        free(sasl_challenge_instance);
    }
}

AMQP_VALUE amqpvalue_create_sasl_challenge(SASL_CHALLENGE_HANDLE sasl_challenge)
{
    AMQP_VALUE result;

    if (sasl_challenge == NULL)
    {
        result = NULL;
    }
    else
    {
        SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)sasl_challenge;
        result = amqpvalue_clone(sasl_challenge_instance->composite_value);
    }

    return result;
}

bool is_sasl_challenge_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 66))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_sasl_challenge(AMQP_VALUE value, SASL_CHALLENGE_HANDLE* sasl_challenge_handle)
{
    int result;
    SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)sasl_challenge_create_internal();
    *sasl_challenge_handle = sasl_challenge_instance;
    if (*sasl_challenge_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            sasl_challenge_destroy(*sasl_challenge_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* challenge */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                sasl_challenge_destroy(*sasl_challenge_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                sasl_challenge_destroy(*sasl_challenge_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                amqp_binary challenge;
                                if (amqpvalue_get_binary(item_value, &challenge) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_challenge_destroy(*sasl_challenge_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }

                    sasl_challenge_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int sasl_challenge_get_challenge(SASL_CHALLENGE_HANDLE sasl_challenge, amqp_binary* challenge_value)
{
    int result;

    if (sasl_challenge == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)sasl_challenge;
        if (amqpvalue_get_composite_item_count(sasl_challenge_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_challenge_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_binary(item_value, challenge_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_challenge_set_challenge(SASL_CHALLENGE_HANDLE sasl_challenge, amqp_binary challenge_value)
{
    int result;

    if (sasl_challenge == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_CHALLENGE_INSTANCE* sasl_challenge_instance = (SASL_CHALLENGE_INSTANCE*)sasl_challenge;
        AMQP_VALUE challenge_amqp_value = amqpvalue_create_binary(challenge_value);
        if (challenge_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_challenge_instance->composite_value, 0, challenge_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(challenge_amqp_value);
        }
    }

    return result;
}


/* sasl-response */

typedef struct SASL_RESPONSE_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SASL_RESPONSE_INSTANCE;

static SASL_RESPONSE_HANDLE sasl_response_create_internal(void)
{
    SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)malloc(sizeof(SASL_RESPONSE_INSTANCE));
    if (sasl_response_instance != NULL)
    {
        sasl_response_instance->composite_value = NULL;
    }

    return sasl_response_instance;
}

SASL_RESPONSE_HANDLE sasl_response_create(amqp_binary response_value)
{
    SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)malloc(sizeof(SASL_RESPONSE_INSTANCE));
    if (sasl_response_instance != NULL)
    {
        sasl_response_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(67);
        if (sasl_response_instance->composite_value == NULL)
        {
            free(sasl_response_instance);
            sasl_response_instance = NULL;
        }
        else
        {
            AMQP_VALUE response_amqp_value;
            int result = 0;

            response_amqp_value = amqpvalue_create_binary(response_value);
            if ((result == 0) && (amqpvalue_set_composite_item(sasl_response_instance->composite_value, 0, response_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(response_amqp_value);
        }
    }

    return sasl_response_instance;
}

SASL_RESPONSE_HANDLE sasl_response_clone(SASL_RESPONSE_HANDLE value)
{
    SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)malloc(sizeof(SASL_RESPONSE_INSTANCE));
    if (sasl_response_instance != NULL)
    {
        sasl_response_instance->composite_value = amqpvalue_clone(((SASL_RESPONSE_INSTANCE*)value)->composite_value);
        if (sasl_response_instance->composite_value == NULL)
        {
            free(sasl_response_instance);
            sasl_response_instance = NULL;
        }
    }

    return sasl_response_instance;
}

void sasl_response_destroy(SASL_RESPONSE_HANDLE sasl_response)
{
    if (sasl_response != NULL)
    {
        SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)sasl_response;
        amqpvalue_destroy(sasl_response_instance->composite_value);
        free(sasl_response_instance);
    }
}

AMQP_VALUE amqpvalue_create_sasl_response(SASL_RESPONSE_HANDLE sasl_response)
{
    AMQP_VALUE result;

    if (sasl_response == NULL)
    {
        result = NULL;
    }
    else
    {
        SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)sasl_response;
        result = amqpvalue_clone(sasl_response_instance->composite_value);
    }

    return result;
}

bool is_sasl_response_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 67))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_sasl_response(AMQP_VALUE value, SASL_RESPONSE_HANDLE* sasl_response_handle)
{
    int result;
    SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)sasl_response_create_internal();
    *sasl_response_handle = sasl_response_instance;
    if (*sasl_response_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            sasl_response_destroy(*sasl_response_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* response */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                sasl_response_destroy(*sasl_response_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                sasl_response_destroy(*sasl_response_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                amqp_binary response;
                                if (amqpvalue_get_binary(item_value, &response) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_response_destroy(*sasl_response_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }

                    sasl_response_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int sasl_response_get_response(SASL_RESPONSE_HANDLE sasl_response, amqp_binary* response_value)
{
    int result;

    if (sasl_response == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)sasl_response;
        if (amqpvalue_get_composite_item_count(sasl_response_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_response_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_binary(item_value, response_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_response_set_response(SASL_RESPONSE_HANDLE sasl_response, amqp_binary response_value)
{
    int result;

    if (sasl_response == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_RESPONSE_INSTANCE* sasl_response_instance = (SASL_RESPONSE_INSTANCE*)sasl_response;
        AMQP_VALUE response_amqp_value = amqpvalue_create_binary(response_value);
        if (response_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_response_instance->composite_value, 0, response_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(response_amqp_value);
        }
    }

    return result;
}


/* sasl-outcome */

typedef struct SASL_OUTCOME_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SASL_OUTCOME_INSTANCE;

static SASL_OUTCOME_HANDLE sasl_outcome_create_internal(void)
{
    SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)malloc(sizeof(SASL_OUTCOME_INSTANCE));
    if (sasl_outcome_instance != NULL)
    {
        sasl_outcome_instance->composite_value = NULL;
    }

    return sasl_outcome_instance;
}

SASL_OUTCOME_HANDLE sasl_outcome_create(sasl_code code_value)
{
    SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)malloc(sizeof(SASL_OUTCOME_INSTANCE));
    if (sasl_outcome_instance != NULL)
    {
        sasl_outcome_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(68);
        if (sasl_outcome_instance->composite_value == NULL)
        {
            free(sasl_outcome_instance);
            sasl_outcome_instance = NULL;
        }
        else
        {
            AMQP_VALUE code_amqp_value;
            int result = 0;

            code_amqp_value = amqpvalue_create_sasl_code(code_value);
            if ((result == 0) && (amqpvalue_set_composite_item(sasl_outcome_instance->composite_value, 0, code_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(code_amqp_value);
        }
    }

    return sasl_outcome_instance;
}

SASL_OUTCOME_HANDLE sasl_outcome_clone(SASL_OUTCOME_HANDLE value)
{
    SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)malloc(sizeof(SASL_OUTCOME_INSTANCE));
    if (sasl_outcome_instance != NULL)
    {
        sasl_outcome_instance->composite_value = amqpvalue_clone(((SASL_OUTCOME_INSTANCE*)value)->composite_value);
        if (sasl_outcome_instance->composite_value == NULL)
        {
            free(sasl_outcome_instance);
            sasl_outcome_instance = NULL;
        }
    }

    return sasl_outcome_instance;
}

void sasl_outcome_destroy(SASL_OUTCOME_HANDLE sasl_outcome)
{
    if (sasl_outcome != NULL)
    {
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        amqpvalue_destroy(sasl_outcome_instance->composite_value);
        free(sasl_outcome_instance);
    }
}

AMQP_VALUE amqpvalue_create_sasl_outcome(SASL_OUTCOME_HANDLE sasl_outcome)
{
    AMQP_VALUE result;

    if (sasl_outcome == NULL)
    {
        result = NULL;
    }
    else
    {
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        result = amqpvalue_clone(sasl_outcome_instance->composite_value);
    }

    return result;
}

bool is_sasl_outcome_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 68))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_sasl_outcome(AMQP_VALUE value, SASL_OUTCOME_HANDLE* sasl_outcome_handle)
{
    int result;
    SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome_create_internal();
    *sasl_outcome_handle = sasl_outcome_instance;
    if (*sasl_outcome_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            sasl_outcome_destroy(*sasl_outcome_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* code */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                sasl_outcome_destroy(*sasl_outcome_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                sasl_outcome_destroy(*sasl_outcome_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                sasl_code code;
                                if (amqpvalue_get_sasl_code(item_value, &code) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_outcome_destroy(*sasl_outcome_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* additional-data */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                amqp_binary additional_data;
                                if (amqpvalue_get_binary(item_value, &additional_data) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    sasl_outcome_destroy(*sasl_outcome_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    sasl_outcome_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int sasl_outcome_get_code(SASL_OUTCOME_HANDLE sasl_outcome, sasl_code* code_value)
{
    int result;

    if (sasl_outcome == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        if (amqpvalue_get_composite_item_count(sasl_outcome_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_outcome_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_sasl_code(item_value, code_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_outcome_set_code(SASL_OUTCOME_HANDLE sasl_outcome, sasl_code code_value)
{
    int result;

    if (sasl_outcome == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        AMQP_VALUE code_amqp_value = amqpvalue_create_sasl_code(code_value);
        if (code_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_outcome_instance->composite_value, 0, code_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(code_amqp_value);
        }
    }

    return result;
}

int sasl_outcome_get_additional_data(SASL_OUTCOME_HANDLE sasl_outcome, amqp_binary* additional_data_value)
{
    int result;

    if (sasl_outcome == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        if (amqpvalue_get_composite_item_count(sasl_outcome_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(sasl_outcome_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_binary(item_value, additional_data_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int sasl_outcome_set_additional_data(SASL_OUTCOME_HANDLE sasl_outcome, amqp_binary additional_data_value)
{
    int result;

    if (sasl_outcome == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SASL_OUTCOME_INSTANCE* sasl_outcome_instance = (SASL_OUTCOME_INSTANCE*)sasl_outcome;
        AMQP_VALUE additional_data_amqp_value = amqpvalue_create_binary(additional_data_value);
        if (additional_data_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(sasl_outcome_instance->composite_value, 1, additional_data_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(additional_data_amqp_value);
        }
    }

    return result;
}


/* terminus-durability */

AMQP_VALUE amqpvalue_create_terminus_durability(terminus_durability value)
{
    return amqpvalue_create_uint(value);
}

/* terminus-expiry-policy */

AMQP_VALUE amqpvalue_create_terminus_expiry_policy(terminus_expiry_policy value)
{
    return amqpvalue_create_symbol(value);
}

/* node-properties */

AMQP_VALUE amqpvalue_create_node_properties(node_properties value)
{
    return amqpvalue_create_fields(value);
}

/* filter-set */

AMQP_VALUE amqpvalue_create_filter_set(AMQP_VALUE value)
{
    return amqpvalue_clone(value);
}

/* source */

typedef struct SOURCE_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} SOURCE_INSTANCE;

static SOURCE_HANDLE source_create_internal(void)
{
    SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)malloc(sizeof(SOURCE_INSTANCE));
    if (source_instance != NULL)
    {
        source_instance->composite_value = NULL;
    }

    return source_instance;
}

SOURCE_HANDLE source_create(void)
{
    SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)malloc(sizeof(SOURCE_INSTANCE));
    if (source_instance != NULL)
    {
        source_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(40);
        if (source_instance->composite_value == NULL)
        {
            free(source_instance);
            source_instance = NULL;
        }
    }

    return source_instance;
}

SOURCE_HANDLE source_clone(SOURCE_HANDLE value)
{
    SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)malloc(sizeof(SOURCE_INSTANCE));
    if (source_instance != NULL)
    {
        source_instance->composite_value = amqpvalue_clone(((SOURCE_INSTANCE*)value)->composite_value);
        if (source_instance->composite_value == NULL)
        {
            free(source_instance);
            source_instance = NULL;
        }
    }

    return source_instance;
}

void source_destroy(SOURCE_HANDLE source)
{
    if (source != NULL)
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        amqpvalue_destroy(source_instance->composite_value);
        free(source_instance);
    }
}

AMQP_VALUE amqpvalue_create_source(SOURCE_HANDLE source)
{
    AMQP_VALUE result;

    if (source == NULL)
    {
        result = NULL;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        result = amqpvalue_clone(source_instance->composite_value);
    }

    return result;
}

bool is_source_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 40))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_source(AMQP_VALUE value, SOURCE_HANDLE* source_handle)
{
    int result;
    SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source_create_internal();
    *source_handle = source_instance;
    if (*source_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            source_destroy(*source_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* address */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* durable */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                terminus_durability durable;
                                if (amqpvalue_get_terminus_durability(item_value, &durable) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* expiry-policy */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                terminus_expiry_policy expiry_policy;
                                if (amqpvalue_get_terminus_expiry_policy(item_value, &expiry_policy) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* timeout */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                seconds timeout;
                                if (amqpvalue_get_seconds(item_value, &timeout) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* dynamic */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool dynamic;
                                if (amqpvalue_get_boolean(item_value, &dynamic) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* dynamic-node-properties */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                node_properties dynamic_node_properties;
                                if (amqpvalue_get_node_properties(item_value, &dynamic_node_properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* distribution-mode */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* distribution_mode;
                                if (amqpvalue_get_symbol(item_value, &distribution_mode) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* filter */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                filter_set filter;
                                if (amqpvalue_get_filter_set(item_value, &filter) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* default-outcome */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* outcomes */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* outcomes = NULL;
                                AMQP_VALUE outcomes_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &outcomes_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &outcomes) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* capabilities */
                    if (list_item_count > 10)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 10);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* capabilities = NULL;
                                AMQP_VALUE capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    source_destroy(*source_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    source_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int source_get_address(SOURCE_HANDLE source, AMQP_VALUE* address_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *address_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int source_set_address(SOURCE_HANDLE source, AMQP_VALUE address_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE address_amqp_value;
        if (address_value == NULL)
        {
            address_amqp_value = NULL;
        }
        else
        {
            address_amqp_value = amqpvalue_clone(address_value);
        }
        if (address_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 0, address_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(address_amqp_value);
        }
    }

    return result;
}

int source_get_durable(SOURCE_HANDLE source, terminus_durability* durable_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                *durable_value = terminus_durability_none;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *durable_value = terminus_durability_none;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_terminus_durability(item_value, durable_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *durable_value = terminus_durability_none;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_durable(SOURCE_HANDLE source, terminus_durability durable_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE durable_amqp_value = amqpvalue_create_terminus_durability(durable_value);
        if (durable_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 1, durable_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(durable_amqp_value);
        }
    }

    return result;
}

int source_get_expiry_policy(SOURCE_HANDLE source, terminus_expiry_policy* expiry_policy_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                *expiry_policy_value = terminus_expiry_policy_session_end;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *expiry_policy_value = terminus_expiry_policy_session_end;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_terminus_expiry_policy(item_value, expiry_policy_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *expiry_policy_value = terminus_expiry_policy_session_end;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_expiry_policy(SOURCE_HANDLE source, terminus_expiry_policy expiry_policy_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE expiry_policy_amqp_value = amqpvalue_create_terminus_expiry_policy(expiry_policy_value);
        if (expiry_policy_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 2, expiry_policy_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(expiry_policy_amqp_value);
        }
    }

    return result;
}

int source_get_timeout(SOURCE_HANDLE source, seconds* timeout_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *timeout_value = 0;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *timeout_value = 0;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_seconds(item_value, timeout_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *timeout_value = 0;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_timeout(SOURCE_HANDLE source, seconds timeout_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE timeout_amqp_value = amqpvalue_create_seconds(timeout_value);
        if (timeout_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 3, timeout_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(timeout_amqp_value);
        }
    }

    return result;
}

int source_get_dynamic(SOURCE_HANDLE source, bool* dynamic_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                *dynamic_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *dynamic_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, dynamic_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *dynamic_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_dynamic(SOURCE_HANDLE source, bool dynamic_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE dynamic_amqp_value = amqpvalue_create_boolean(dynamic_value);
        if (dynamic_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 4, dynamic_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(dynamic_amqp_value);
        }
    }

    return result;
}

int source_get_dynamic_node_properties(SOURCE_HANDLE source, node_properties* dynamic_node_properties_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_node_properties(item_value, dynamic_node_properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_dynamic_node_properties(SOURCE_HANDLE source, node_properties dynamic_node_properties_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE dynamic_node_properties_amqp_value = amqpvalue_create_node_properties(dynamic_node_properties_value);
        if (dynamic_node_properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 5, dynamic_node_properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(dynamic_node_properties_amqp_value);
        }
    }

    return result;
}

int source_get_distribution_mode(SOURCE_HANDLE source, const char** distribution_mode_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_symbol(item_value, distribution_mode_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_distribution_mode(SOURCE_HANDLE source, const char* distribution_mode_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE distribution_mode_amqp_value = amqpvalue_create_symbol(distribution_mode_value);
        if (distribution_mode_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 6, distribution_mode_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(distribution_mode_amqp_value);
        }
    }

    return result;
}

int source_get_filter(SOURCE_HANDLE source, filter_set* filter_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_filter_set(item_value, filter_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int source_set_filter(SOURCE_HANDLE source, filter_set filter_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE filter_amqp_value = amqpvalue_create_filter_set(filter_value);
        if (filter_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 7, filter_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(filter_amqp_value);
        }
    }

    return result;
}

int source_get_default_outcome(SOURCE_HANDLE source, AMQP_VALUE* default_outcome_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *default_outcome_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int source_set_default_outcome(SOURCE_HANDLE source, AMQP_VALUE default_outcome_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE default_outcome_amqp_value;
        if (default_outcome_value == NULL)
        {
            default_outcome_amqp_value = NULL;
        }
        else
        {
            default_outcome_amqp_value = amqpvalue_clone(default_outcome_value);
        }
        if (default_outcome_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 8, default_outcome_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(default_outcome_amqp_value);
        }
    }

    return result;
}

int source_get_outcomes(SOURCE_HANDLE source, AMQP_VALUE* outcomes_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* outcomes_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &outcomes_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&outcomes_single_value, 0, sizeof(outcomes_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, outcomes_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *outcomes_value = amqpvalue_create_array();
                            if (*outcomes_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(outcomes_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*outcomes_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*outcomes_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*outcomes_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(source_instance->composite_value, 9, *outcomes_value) != 0)
                                        {
                                            amqpvalue_destroy(*outcomes_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*outcomes_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int source_set_outcomes(SOURCE_HANDLE source, AMQP_VALUE outcomes_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE outcomes_amqp_value;
        if (outcomes_value == NULL)
        {
            outcomes_amqp_value = NULL;
        }
        else
        {
            outcomes_amqp_value = amqpvalue_clone(outcomes_value);
        }
        if (outcomes_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 9, outcomes_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(outcomes_amqp_value);
        }
    }

    return result;
}

int source_get_capabilities(SOURCE_HANDLE source, AMQP_VALUE* capabilities_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        if (amqpvalue_get_composite_item_count(source_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 10)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(source_instance->composite_value, 10);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&capabilities_single_value, 0, sizeof(capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *capabilities_value = amqpvalue_create_array();
                            if (*capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(source_instance->composite_value, 10, *capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int source_set_capabilities(SOURCE_HANDLE source, AMQP_VALUE capabilities_value)
{
    int result;

    if (source == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOURCE_INSTANCE* source_instance = (SOURCE_INSTANCE*)source;
        AMQP_VALUE capabilities_amqp_value;
        if (capabilities_value == NULL)
        {
            capabilities_amqp_value = NULL;
        }
        else
        {
            capabilities_amqp_value = amqpvalue_clone(capabilities_value);
        }
        if (capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(source_instance->composite_value, 10, capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(capabilities_amqp_value);
        }
    }

    return result;
}


/* target */

typedef struct TARGET_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} TARGET_INSTANCE;

static TARGET_HANDLE target_create_internal(void)
{
    TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)malloc(sizeof(TARGET_INSTANCE));
    if (target_instance != NULL)
    {
        target_instance->composite_value = NULL;
    }

    return target_instance;
}

TARGET_HANDLE target_create(void)
{
    TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)malloc(sizeof(TARGET_INSTANCE));
    if (target_instance != NULL)
    {
        target_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(41);
        if (target_instance->composite_value == NULL)
        {
            free(target_instance);
            target_instance = NULL;
        }
    }

    return target_instance;
}

TARGET_HANDLE target_clone(TARGET_HANDLE value)
{
    TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)malloc(sizeof(TARGET_INSTANCE));
    if (target_instance != NULL)
    {
        target_instance->composite_value = amqpvalue_clone(((TARGET_INSTANCE*)value)->composite_value);
        if (target_instance->composite_value == NULL)
        {
            free(target_instance);
            target_instance = NULL;
        }
    }

    return target_instance;
}

void target_destroy(TARGET_HANDLE target)
{
    if (target != NULL)
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        amqpvalue_destroy(target_instance->composite_value);
        free(target_instance);
    }
}

AMQP_VALUE amqpvalue_create_target(TARGET_HANDLE target)
{
    AMQP_VALUE result;

    if (target == NULL)
    {
        result = NULL;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        result = amqpvalue_clone(target_instance->composite_value);
    }

    return result;
}

bool is_target_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 41))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_target(AMQP_VALUE value, TARGET_HANDLE* target_handle)
{
    int result;
    TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target_create_internal();
    *target_handle = target_instance;
    if (*target_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            target_destroy(*target_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* address */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* durable */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                terminus_durability durable;
                                if (amqpvalue_get_terminus_durability(item_value, &durable) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* expiry-policy */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                terminus_expiry_policy expiry_policy;
                                if (amqpvalue_get_terminus_expiry_policy(item_value, &expiry_policy) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* timeout */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                seconds timeout;
                                if (amqpvalue_get_seconds(item_value, &timeout) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* dynamic */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool dynamic;
                                if (amqpvalue_get_boolean(item_value, &dynamic) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* dynamic-node-properties */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                node_properties dynamic_node_properties;
                                if (amqpvalue_get_node_properties(item_value, &dynamic_node_properties) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* capabilities */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* capabilities = NULL;
                                AMQP_VALUE capabilities_array;
                                if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, &capabilities_array) != 0)) &&
                                    (amqpvalue_get_symbol(item_value, &capabilities) != 0))
                                {
                                    amqpvalue_destroy(item_value);
                                    target_destroy(*target_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    target_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int target_get_address(TARGET_HANDLE target, AMQP_VALUE* address_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *address_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int target_set_address(TARGET_HANDLE target, AMQP_VALUE address_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE address_amqp_value;
        if (address_value == NULL)
        {
            address_amqp_value = NULL;
        }
        else
        {
            address_amqp_value = amqpvalue_clone(address_value);
        }
        if (address_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 0, address_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(address_amqp_value);
        }
    }

    return result;
}

int target_get_durable(TARGET_HANDLE target, terminus_durability* durable_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                *durable_value = terminus_durability_none;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *durable_value = terminus_durability_none;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_terminus_durability(item_value, durable_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *durable_value = terminus_durability_none;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int target_set_durable(TARGET_HANDLE target, terminus_durability durable_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE durable_amqp_value = amqpvalue_create_terminus_durability(durable_value);
        if (durable_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 1, durable_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(durable_amqp_value);
        }
    }

    return result;
}

int target_get_expiry_policy(TARGET_HANDLE target, terminus_expiry_policy* expiry_policy_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                *expiry_policy_value = terminus_expiry_policy_session_end;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *expiry_policy_value = terminus_expiry_policy_session_end;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_terminus_expiry_policy(item_value, expiry_policy_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *expiry_policy_value = terminus_expiry_policy_session_end;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int target_set_expiry_policy(TARGET_HANDLE target, terminus_expiry_policy expiry_policy_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE expiry_policy_amqp_value = amqpvalue_create_terminus_expiry_policy(expiry_policy_value);
        if (expiry_policy_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 2, expiry_policy_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(expiry_policy_amqp_value);
        }
    }

    return result;
}

int target_get_timeout(TARGET_HANDLE target, seconds* timeout_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *timeout_value = 0;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *timeout_value = 0;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_seconds(item_value, timeout_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *timeout_value = 0;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int target_set_timeout(TARGET_HANDLE target, seconds timeout_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE timeout_amqp_value = amqpvalue_create_seconds(timeout_value);
        if (timeout_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 3, timeout_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(timeout_amqp_value);
        }
    }

    return result;
}

int target_get_dynamic(TARGET_HANDLE target, bool* dynamic_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                *dynamic_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *dynamic_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, dynamic_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *dynamic_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int target_set_dynamic(TARGET_HANDLE target, bool dynamic_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE dynamic_amqp_value = amqpvalue_create_boolean(dynamic_value);
        if (dynamic_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 4, dynamic_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(dynamic_amqp_value);
        }
    }

    return result;
}

int target_get_dynamic_node_properties(TARGET_HANDLE target, node_properties* dynamic_node_properties_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_node_properties(item_value, dynamic_node_properties_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int target_set_dynamic_node_properties(TARGET_HANDLE target, node_properties dynamic_node_properties_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE dynamic_node_properties_amqp_value = amqpvalue_create_node_properties(dynamic_node_properties_value);
        if (dynamic_node_properties_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 5, dynamic_node_properties_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(dynamic_node_properties_amqp_value);
        }
    }

    return result;
}

int target_get_capabilities(TARGET_HANDLE target, AMQP_VALUE* capabilities_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        if (amqpvalue_get_composite_item_count(target_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(target_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    const char* capabilities_single_value;
                    int get_single_value_result;
                    if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                    {
                        get_single_value_result = amqpvalue_get_symbol(item_value, &capabilities_single_value);
                    }
                    else
                    {
                        (void)memset((void*)&capabilities_single_value, 0, sizeof(capabilities_single_value));
                        get_single_value_result = 1;
                    }

                    if (((amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY) || (amqpvalue_get_array(item_value, capabilities_value) != 0)) &&
                        (get_single_value_result != 0))
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_ARRAY)
                        {
                            *capabilities_value = amqpvalue_create_array();
                            if (*capabilities_value == NULL)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                AMQP_VALUE single_amqp_value = amqpvalue_create_symbol(capabilities_single_value);
                                if (single_amqp_value == NULL)
                                {
                                    amqpvalue_destroy(*capabilities_value);
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    if (amqpvalue_add_array_item(*capabilities_value, single_amqp_value) != 0)
                                    {
                                        amqpvalue_destroy(*capabilities_value);
                                        amqpvalue_destroy(single_amqp_value);
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        if (amqpvalue_set_composite_item(target_instance->composite_value, 6, *capabilities_value) != 0)
                                        {
                                            amqpvalue_destroy(*capabilities_value);
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            result = 0;
                                        }
                                    }

                                    amqpvalue_destroy(single_amqp_value);
                                }
                                amqpvalue_destroy(*capabilities_value);
                            }
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int target_set_capabilities(TARGET_HANDLE target, AMQP_VALUE capabilities_value)
{
    int result;

    if (target == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TARGET_INSTANCE* target_instance = (TARGET_INSTANCE*)target;
        AMQP_VALUE capabilities_amqp_value;
        if (capabilities_value == NULL)
        {
            capabilities_amqp_value = NULL;
        }
        else
        {
            capabilities_amqp_value = amqpvalue_clone(capabilities_value);
        }
        if (capabilities_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(target_instance->composite_value, 6, capabilities_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(capabilities_amqp_value);
        }
    }

    return result;
}


/* annotations */

AMQP_VALUE amqpvalue_create_annotations(AMQP_VALUE value)
{
    return amqpvalue_clone(value);
}

/* message-id-ulong */

AMQP_VALUE amqpvalue_create_message_id_ulong(message_id_ulong value)
{
    return amqpvalue_create_ulong(value);
}

/* message-id-uuid */

AMQP_VALUE amqpvalue_create_message_id_uuid(message_id_uuid value)
{
    return amqpvalue_create_uuid(value);
}

/* message-id-binary */

AMQP_VALUE amqpvalue_create_message_id_binary(message_id_binary value)
{
    return amqpvalue_create_binary(value);
}

/* message-id-string */

AMQP_VALUE amqpvalue_create_message_id_string(message_id_string value)
{
    return amqpvalue_create_string(value);
}

/* address-string */

AMQP_VALUE amqpvalue_create_address_string(address_string value)
{
    return amqpvalue_create_string(value);
}

/* header */

typedef struct HEADER_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} HEADER_INSTANCE;

static HEADER_HANDLE header_create_internal(void)
{
    HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)malloc(sizeof(HEADER_INSTANCE));
    if (header_instance != NULL)
    {
        header_instance->composite_value = NULL;
    }

    return header_instance;
}

HEADER_HANDLE header_create(void)
{
    HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)malloc(sizeof(HEADER_INSTANCE));
    if (header_instance != NULL)
    {
        header_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(112);
        if (header_instance->composite_value == NULL)
        {
            free(header_instance);
            header_instance = NULL;
        }
    }

    return header_instance;
}

HEADER_HANDLE header_clone(HEADER_HANDLE value)
{
    HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)malloc(sizeof(HEADER_INSTANCE));
    if (header_instance != NULL)
    {
        header_instance->composite_value = amqpvalue_clone(((HEADER_INSTANCE*)value)->composite_value);
        if (header_instance->composite_value == NULL)
        {
            free(header_instance);
            header_instance = NULL;
        }
    }

    return header_instance;
}

void header_destroy(HEADER_HANDLE header)
{
    if (header != NULL)
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        amqpvalue_destroy(header_instance->composite_value);
        free(header_instance);
    }
}

AMQP_VALUE amqpvalue_create_header(HEADER_HANDLE header)
{
    AMQP_VALUE result;

    if (header == NULL)
    {
        result = NULL;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        result = amqpvalue_clone(header_instance->composite_value);
    }

    return result;
}

bool is_header_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 112))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_header(AMQP_VALUE value, HEADER_HANDLE* header_handle)
{
    int result;
    HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header_create_internal();
    *header_handle = header_instance;
    if (*header_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            header_destroy(*header_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* durable */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool durable;
                                if (amqpvalue_get_boolean(item_value, &durable) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    header_destroy(*header_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* priority */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint8_t priority;
                                if (amqpvalue_get_ubyte(item_value, &priority) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    header_destroy(*header_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* ttl */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                milliseconds ttl;
                                if (amqpvalue_get_milliseconds(item_value, &ttl) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    header_destroy(*header_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* first-acquirer */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool first_acquirer;
                                if (amqpvalue_get_boolean(item_value, &first_acquirer) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    header_destroy(*header_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* delivery-count */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                uint32_t delivery_count;
                                if (amqpvalue_get_uint(item_value, &delivery_count) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    header_destroy(*header_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    header_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int header_get_durable(HEADER_HANDLE header, bool* durable_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        if (amqpvalue_get_composite_item_count(header_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                *durable_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(header_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *durable_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, durable_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *durable_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int header_set_durable(HEADER_HANDLE header, bool durable_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        AMQP_VALUE durable_amqp_value = amqpvalue_create_boolean(durable_value);
        if (durable_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(header_instance->composite_value, 0, durable_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(durable_amqp_value);
        }
    }

    return result;
}

int header_get_priority(HEADER_HANDLE header, uint8_t* priority_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        if (amqpvalue_get_composite_item_count(header_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                *priority_value = 4;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(header_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *priority_value = 4;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_ubyte(item_value, priority_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *priority_value = 4;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int header_set_priority(HEADER_HANDLE header, uint8_t priority_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        AMQP_VALUE priority_amqp_value = amqpvalue_create_ubyte(priority_value);
        if (priority_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(header_instance->composite_value, 1, priority_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(priority_amqp_value);
        }
    }

    return result;
}

int header_get_ttl(HEADER_HANDLE header, milliseconds* ttl_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        if (amqpvalue_get_composite_item_count(header_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(header_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_milliseconds(item_value, ttl_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int header_set_ttl(HEADER_HANDLE header, milliseconds ttl_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        AMQP_VALUE ttl_amqp_value = amqpvalue_create_milliseconds(ttl_value);
        if (ttl_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(header_instance->composite_value, 2, ttl_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(ttl_amqp_value);
        }
    }

    return result;
}

int header_get_first_acquirer(HEADER_HANDLE header, bool* first_acquirer_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        if (amqpvalue_get_composite_item_count(header_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                *first_acquirer_value = false;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(header_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *first_acquirer_value = false;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, first_acquirer_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *first_acquirer_value = false;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int header_set_first_acquirer(HEADER_HANDLE header, bool first_acquirer_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        AMQP_VALUE first_acquirer_amqp_value = amqpvalue_create_boolean(first_acquirer_value);
        if (first_acquirer_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(header_instance->composite_value, 3, first_acquirer_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(first_acquirer_amqp_value);
        }
    }

    return result;
}

int header_get_delivery_count(HEADER_HANDLE header, uint32_t* delivery_count_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        if (amqpvalue_get_composite_item_count(header_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                *delivery_count_value = 0;
                result = 0;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(header_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    *delivery_count_value = 0;
                    result = 0;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, delivery_count_value);
                    if (get_single_value_result != 0)
                    {
                        if (amqpvalue_get_type(item_value) != AMQP_TYPE_NULL)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            *delivery_count_value = 0;
                            result = 0;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int header_set_delivery_count(HEADER_HANDLE header, uint32_t delivery_count_value)
{
    int result;

    if (header == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        HEADER_INSTANCE* header_instance = (HEADER_INSTANCE*)header;
        AMQP_VALUE delivery_count_amqp_value = amqpvalue_create_uint(delivery_count_value);
        if (delivery_count_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(header_instance->composite_value, 4, delivery_count_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(delivery_count_amqp_value);
        }
    }

    return result;
}


/* delivery-annotations */

AMQP_VALUE amqpvalue_create_delivery_annotations(delivery_annotations value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_create_annotations(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(113);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_delivery_annotations_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 113))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* message-annotations */

AMQP_VALUE amqpvalue_create_message_annotations(message_annotations value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_create_annotations(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(114);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_message_annotations_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 114))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* application-properties */

AMQP_VALUE amqpvalue_create_application_properties(AMQP_VALUE value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_clone(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(116);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_application_properties_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 116))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* data */

AMQP_VALUE amqpvalue_create_data(data value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_create_binary(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(117);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_data_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 117))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* amqp-sequence */

AMQP_VALUE amqpvalue_create_amqp_sequence(AMQP_VALUE value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_clone(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(118);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_amqp_sequence_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 118))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* amqp-value */

AMQP_VALUE amqpvalue_create_amqp_value(AMQP_VALUE value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_clone(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(119);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_amqp_value_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 119))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* footer */

AMQP_VALUE amqpvalue_create_footer(footer value)
{
    AMQP_VALUE result;
    AMQP_VALUE described_value = amqpvalue_create_annotations(value);
    if (described_value == NULL)
    {
        result = NULL;
    }
    else
    {
        AMQP_VALUE descriptor = amqpvalue_create_ulong(120);
        if (descriptor == NULL)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_described(amqpvalue_clone(descriptor), amqpvalue_clone(described_value));

            amqpvalue_destroy(descriptor);
        }

        amqpvalue_destroy(described_value);
    }

    return result;
}

bool is_footer_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 120))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/* properties */

typedef struct PROPERTIES_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} PROPERTIES_INSTANCE;

static PROPERTIES_HANDLE properties_create_internal(void)
{
    PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)malloc(sizeof(PROPERTIES_INSTANCE));
    if (properties_instance != NULL)
    {
        properties_instance->composite_value = NULL;
    }

    return properties_instance;
}

PROPERTIES_HANDLE properties_create(void)
{
    PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)malloc(sizeof(PROPERTIES_INSTANCE));
    if (properties_instance != NULL)
    {
        properties_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(115);
        if (properties_instance->composite_value == NULL)
        {
            free(properties_instance);
            properties_instance = NULL;
        }
    }

    return properties_instance;
}

PROPERTIES_HANDLE properties_clone(PROPERTIES_HANDLE value)
{
    PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)malloc(sizeof(PROPERTIES_INSTANCE));
    if (properties_instance != NULL)
    {
        properties_instance->composite_value = amqpvalue_clone(((PROPERTIES_INSTANCE*)value)->composite_value);
        if (properties_instance->composite_value == NULL)
        {
            free(properties_instance);
            properties_instance = NULL;
        }
    }

    return properties_instance;
}

void properties_destroy(PROPERTIES_HANDLE properties)
{
    if (properties != NULL)
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        amqpvalue_destroy(properties_instance->composite_value);
        free(properties_instance);
    }
}

AMQP_VALUE amqpvalue_create_properties(PROPERTIES_HANDLE properties)
{
    AMQP_VALUE result;

    if (properties == NULL)
    {
        result = NULL;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        result = amqpvalue_clone(properties_instance->composite_value);
    }

    return result;
}

bool is_properties_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 115))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_properties(AMQP_VALUE value, PROPERTIES_HANDLE* properties_handle)
{
    int result;
    PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties_create_internal();
    *properties_handle = properties_instance;
    if (*properties_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            properties_destroy(*properties_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* message-id */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* user-id */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                amqp_binary user_id;
                                if (amqpvalue_get_binary(item_value, &user_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* to */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* subject */
                    if (list_item_count > 3)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 3);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* subject;
                                if (amqpvalue_get_string(item_value, &subject) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* reply-to */
                    if (list_item_count > 4)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 4);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* correlation-id */
                    if (list_item_count > 5)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 5);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* content-type */
                    if (list_item_count > 6)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 6);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* content_type;
                                if (amqpvalue_get_symbol(item_value, &content_type) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* content-encoding */
                    if (list_item_count > 7)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 7);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* content_encoding;
                                if (amqpvalue_get_symbol(item_value, &content_encoding) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* absolute-expiry-time */
                    if (list_item_count > 8)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 8);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                timestamp absolute_expiry_time;
                                if (amqpvalue_get_timestamp(item_value, &absolute_expiry_time) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* creation-time */
                    if (list_item_count > 9)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 9);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                timestamp creation_time;
                                if (amqpvalue_get_timestamp(item_value, &creation_time) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* group-id */
                    if (list_item_count > 10)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 10);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* group_id;
                                if (amqpvalue_get_string(item_value, &group_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* group-sequence */
                    if (list_item_count > 11)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 11);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                sequence_no group_sequence;
                                if (amqpvalue_get_sequence_no(item_value, &group_sequence) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* reply-to-group-id */
                    if (list_item_count > 12)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 12);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                const char* reply_to_group_id;
                                if (amqpvalue_get_string(item_value, &reply_to_group_id) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    properties_destroy(*properties_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    properties_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int properties_get_message_id(PROPERTIES_HANDLE properties, AMQP_VALUE* message_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *message_id_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int properties_set_message_id(PROPERTIES_HANDLE properties, AMQP_VALUE message_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE message_id_amqp_value;
        if (message_id_value == NULL)
        {
            message_id_amqp_value = NULL;
        }
        else
        {
            message_id_amqp_value = amqpvalue_clone(message_id_value);
        }
        if (message_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 0, message_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(message_id_amqp_value);
        }
    }

    return result;
}

int properties_get_user_id(PROPERTIES_HANDLE properties, amqp_binary* user_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_binary(item_value, user_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_user_id(PROPERTIES_HANDLE properties, amqp_binary user_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE user_id_amqp_value = amqpvalue_create_binary(user_id_value);
        if (user_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 1, user_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(user_id_amqp_value);
        }
    }

    return result;
}

int properties_get_to(PROPERTIES_HANDLE properties, AMQP_VALUE* to_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *to_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int properties_set_to(PROPERTIES_HANDLE properties, AMQP_VALUE to_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE to_amqp_value;
        if (to_value == NULL)
        {
            to_amqp_value = NULL;
        }
        else
        {
            to_amqp_value = amqpvalue_clone(to_value);
        }
        if (to_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 2, to_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(to_amqp_value);
        }
    }

    return result;
}

int properties_get_subject(PROPERTIES_HANDLE properties, const char** subject_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 3)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 3);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, subject_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_subject(PROPERTIES_HANDLE properties, const char* subject_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE subject_amqp_value = amqpvalue_create_string(subject_value);
        if (subject_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 3, subject_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(subject_amqp_value);
        }
    }

    return result;
}

int properties_get_reply_to(PROPERTIES_HANDLE properties, AMQP_VALUE* reply_to_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 4)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 4);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *reply_to_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int properties_set_reply_to(PROPERTIES_HANDLE properties, AMQP_VALUE reply_to_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE reply_to_amqp_value;
        if (reply_to_value == NULL)
        {
            reply_to_amqp_value = NULL;
        }
        else
        {
            reply_to_amqp_value = amqpvalue_clone(reply_to_value);
        }
        if (reply_to_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 4, reply_to_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(reply_to_amqp_value);
        }
    }

    return result;
}

int properties_get_correlation_id(PROPERTIES_HANDLE properties, AMQP_VALUE* correlation_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 5)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 5);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    *correlation_id_value = item_value;
                    result = 0;
                }
            }
        }
    }

    return result;
}

int properties_set_correlation_id(PROPERTIES_HANDLE properties, AMQP_VALUE correlation_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE correlation_id_amqp_value;
        if (correlation_id_value == NULL)
        {
            correlation_id_amqp_value = NULL;
        }
        else
        {
            correlation_id_amqp_value = amqpvalue_clone(correlation_id_value);
        }
        if (correlation_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 5, correlation_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(correlation_id_amqp_value);
        }
    }

    return result;
}

int properties_get_content_type(PROPERTIES_HANDLE properties, const char** content_type_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 6)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 6);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_symbol(item_value, content_type_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_content_type(PROPERTIES_HANDLE properties, const char* content_type_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE content_type_amqp_value = amqpvalue_create_symbol(content_type_value);
        if (content_type_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 6, content_type_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(content_type_amqp_value);
        }
    }

    return result;
}

int properties_get_content_encoding(PROPERTIES_HANDLE properties, const char** content_encoding_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 7)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 7);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_symbol(item_value, content_encoding_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_content_encoding(PROPERTIES_HANDLE properties, const char* content_encoding_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE content_encoding_amqp_value = amqpvalue_create_symbol(content_encoding_value);
        if (content_encoding_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 7, content_encoding_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(content_encoding_amqp_value);
        }
    }

    return result;
}

int properties_get_absolute_expiry_time(PROPERTIES_HANDLE properties, timestamp* absolute_expiry_time_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 8)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 8);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_timestamp(item_value, absolute_expiry_time_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_absolute_expiry_time(PROPERTIES_HANDLE properties, timestamp absolute_expiry_time_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE absolute_expiry_time_amqp_value = amqpvalue_create_timestamp(absolute_expiry_time_value);
        if (absolute_expiry_time_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 8, absolute_expiry_time_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(absolute_expiry_time_amqp_value);
        }
    }

    return result;
}

int properties_get_creation_time(PROPERTIES_HANDLE properties, timestamp* creation_time_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 9)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 9);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_timestamp(item_value, creation_time_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_creation_time(PROPERTIES_HANDLE properties, timestamp creation_time_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE creation_time_amqp_value = amqpvalue_create_timestamp(creation_time_value);
        if (creation_time_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 9, creation_time_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(creation_time_amqp_value);
        }
    }

    return result;
}

int properties_get_group_id(PROPERTIES_HANDLE properties, const char** group_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 10)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 10);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, group_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_group_id(PROPERTIES_HANDLE properties, const char* group_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE group_id_amqp_value = amqpvalue_create_string(group_id_value);
        if (group_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 10, group_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(group_id_amqp_value);
        }
    }

    return result;
}

int properties_get_group_sequence(PROPERTIES_HANDLE properties, sequence_no* group_sequence_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 11)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 11);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_sequence_no(item_value, group_sequence_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_group_sequence(PROPERTIES_HANDLE properties, sequence_no group_sequence_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE group_sequence_amqp_value = amqpvalue_create_sequence_no(group_sequence_value);
        if (group_sequence_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 11, group_sequence_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(group_sequence_amqp_value);
        }
    }

    return result;
}

int properties_get_reply_to_group_id(PROPERTIES_HANDLE properties, const char** reply_to_group_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        if (amqpvalue_get_composite_item_count(properties_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 12)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(properties_instance->composite_value, 12);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_string(item_value, reply_to_group_id_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int properties_set_reply_to_group_id(PROPERTIES_HANDLE properties, const char* reply_to_group_id_value)
{
    int result;

    if (properties == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        PROPERTIES_INSTANCE* properties_instance = (PROPERTIES_INSTANCE*)properties;
        AMQP_VALUE reply_to_group_id_amqp_value = amqpvalue_create_string(reply_to_group_id_value);
        if (reply_to_group_id_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(properties_instance->composite_value, 12, reply_to_group_id_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(reply_to_group_id_amqp_value);
        }
    }

    return result;
}


/* received */

typedef struct RECEIVED_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} RECEIVED_INSTANCE;

static RECEIVED_HANDLE received_create_internal(void)
{
    RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)malloc(sizeof(RECEIVED_INSTANCE));
    if (received_instance != NULL)
    {
        received_instance->composite_value = NULL;
    }

    return received_instance;
}

RECEIVED_HANDLE received_create(uint32_t section_number_value, uint64_t section_offset_value)
{
    RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)malloc(sizeof(RECEIVED_INSTANCE));
    if (received_instance != NULL)
    {
        received_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(35);
        if (received_instance->composite_value == NULL)
        {
            free(received_instance);
            received_instance = NULL;
        }
        else
        {
            AMQP_VALUE section_number_amqp_value;
            AMQP_VALUE section_offset_amqp_value;
            int result = 0;

            section_number_amqp_value = amqpvalue_create_uint(section_number_value);
            if ((result == 0) && (amqpvalue_set_composite_item(received_instance->composite_value, 0, section_number_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }
            section_offset_amqp_value = amqpvalue_create_ulong(section_offset_value);
            if ((result == 0) && (amqpvalue_set_composite_item(received_instance->composite_value, 1, section_offset_amqp_value) != 0))
            {
                result = MU_FAILURE;
            }

            amqpvalue_destroy(section_number_amqp_value);
            amqpvalue_destroy(section_offset_amqp_value);
        }
    }

    return received_instance;
}

RECEIVED_HANDLE received_clone(RECEIVED_HANDLE value)
{
    RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)malloc(sizeof(RECEIVED_INSTANCE));
    if (received_instance != NULL)
    {
        received_instance->composite_value = amqpvalue_clone(((RECEIVED_INSTANCE*)value)->composite_value);
        if (received_instance->composite_value == NULL)
        {
            free(received_instance);
            received_instance = NULL;
        }
    }

    return received_instance;
}

void received_destroy(RECEIVED_HANDLE received)
{
    if (received != NULL)
    {
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        amqpvalue_destroy(received_instance->composite_value);
        free(received_instance);
    }
}

AMQP_VALUE amqpvalue_create_received(RECEIVED_HANDLE received)
{
    AMQP_VALUE result;

    if (received == NULL)
    {
        result = NULL;
    }
    else
    {
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        result = amqpvalue_clone(received_instance->composite_value);
    }

    return result;
}

bool is_received_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 35))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_received(AMQP_VALUE value, RECEIVED_HANDLE* received_handle)
{
    int result;
    RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received_create_internal();
    *received_handle = received_instance;
    if (*received_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            received_destroy(*received_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* section-number */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            {
                                received_destroy(*received_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                received_destroy(*received_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint32_t section_number;
                                if (amqpvalue_get_uint(item_value, &section_number) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    received_destroy(*received_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                    /* section-offset */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            {
                                received_destroy(*received_handle);
                                result = MU_FAILURE;
                                break;
                            }
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                amqpvalue_destroy(item_value);
                                received_destroy(*received_handle);
                                result = MU_FAILURE;
                                break;
                            }
                            else
                            {
                                uint64_t section_offset;
                                if (amqpvalue_get_ulong(item_value, &section_offset) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    received_destroy(*received_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }

                    received_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int received_get_section_number(RECEIVED_HANDLE received, uint32_t* section_number_value)
{
    int result;

    if (received == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        if (amqpvalue_get_composite_item_count(received_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(received_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_uint(item_value, section_number_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int received_set_section_number(RECEIVED_HANDLE received, uint32_t section_number_value)
{
    int result;

    if (received == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        AMQP_VALUE section_number_amqp_value = amqpvalue_create_uint(section_number_value);
        if (section_number_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(received_instance->composite_value, 0, section_number_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(section_number_amqp_value);
        }
    }

    return result;
}

int received_get_section_offset(RECEIVED_HANDLE received, uint64_t* section_offset_value)
{
    int result;

    if (received == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        if (amqpvalue_get_composite_item_count(received_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(received_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_ulong(item_value, section_offset_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int received_set_section_offset(RECEIVED_HANDLE received, uint64_t section_offset_value)
{
    int result;

    if (received == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        RECEIVED_INSTANCE* received_instance = (RECEIVED_INSTANCE*)received;
        AMQP_VALUE section_offset_amqp_value = amqpvalue_create_ulong(section_offset_value);
        if (section_offset_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(received_instance->composite_value, 1, section_offset_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(section_offset_amqp_value);
        }
    }

    return result;
}


/* accepted */

typedef struct ACCEPTED_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} ACCEPTED_INSTANCE;

static ACCEPTED_HANDLE accepted_create_internal(void)
{
    ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)malloc(sizeof(ACCEPTED_INSTANCE));
    if (accepted_instance != NULL)
    {
        accepted_instance->composite_value = NULL;
    }

    return accepted_instance;
}

ACCEPTED_HANDLE accepted_create(void)
{
    ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)malloc(sizeof(ACCEPTED_INSTANCE));
    if (accepted_instance != NULL)
    {
        accepted_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(36);
        if (accepted_instance->composite_value == NULL)
        {
            free(accepted_instance);
            accepted_instance = NULL;
        }
    }

    return accepted_instance;
}

ACCEPTED_HANDLE accepted_clone(ACCEPTED_HANDLE value)
{
    ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)malloc(sizeof(ACCEPTED_INSTANCE));
    if (accepted_instance != NULL)
    {
        accepted_instance->composite_value = amqpvalue_clone(((ACCEPTED_INSTANCE*)value)->composite_value);
        if (accepted_instance->composite_value == NULL)
        {
            free(accepted_instance);
            accepted_instance = NULL;
        }
    }

    return accepted_instance;
}

void accepted_destroy(ACCEPTED_HANDLE accepted)
{
    if (accepted != NULL)
    {
        ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)accepted;
        amqpvalue_destroy(accepted_instance->composite_value);
        free(accepted_instance);
    }
}

AMQP_VALUE amqpvalue_create_accepted(ACCEPTED_HANDLE accepted)
{
    AMQP_VALUE result;

    if (accepted == NULL)
    {
        result = NULL;
    }
    else
    {
        ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)accepted;
        result = amqpvalue_clone(accepted_instance->composite_value);
    }

    return result;
}

bool is_accepted_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 36))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_accepted(AMQP_VALUE value, ACCEPTED_HANDLE* accepted_handle)
{
    int result;
    ACCEPTED_INSTANCE* accepted_instance = (ACCEPTED_INSTANCE*)accepted_create_internal();
    *accepted_handle = accepted_instance;
    if (*accepted_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            accepted_destroy(*accepted_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {

                    accepted_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}


/* rejected */

typedef struct REJECTED_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} REJECTED_INSTANCE;

static REJECTED_HANDLE rejected_create_internal(void)
{
    REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)malloc(sizeof(REJECTED_INSTANCE));
    if (rejected_instance != NULL)
    {
        rejected_instance->composite_value = NULL;
    }

    return rejected_instance;
}

REJECTED_HANDLE rejected_create(void)
{
    REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)malloc(sizeof(REJECTED_INSTANCE));
    if (rejected_instance != NULL)
    {
        rejected_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(37);
        if (rejected_instance->composite_value == NULL)
        {
            free(rejected_instance);
            rejected_instance = NULL;
        }
    }

    return rejected_instance;
}

REJECTED_HANDLE rejected_clone(REJECTED_HANDLE value)
{
    REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)malloc(sizeof(REJECTED_INSTANCE));
    if (rejected_instance != NULL)
    {
        rejected_instance->composite_value = amqpvalue_clone(((REJECTED_INSTANCE*)value)->composite_value);
        if (rejected_instance->composite_value == NULL)
        {
            free(rejected_instance);
            rejected_instance = NULL;
        }
    }

    return rejected_instance;
}

void rejected_destroy(REJECTED_HANDLE rejected)
{
    if (rejected != NULL)
    {
        REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)rejected;
        amqpvalue_destroy(rejected_instance->composite_value);
        free(rejected_instance);
    }
}

AMQP_VALUE amqpvalue_create_rejected(REJECTED_HANDLE rejected)
{
    AMQP_VALUE result;

    if (rejected == NULL)
    {
        result = NULL;
    }
    else
    {
        REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)rejected;
        result = amqpvalue_clone(rejected_instance->composite_value);
    }

    return result;
}

bool is_rejected_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 37))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_rejected(AMQP_VALUE value, REJECTED_HANDLE* rejected_handle)
{
    int result;
    REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)rejected_create_internal();
    *rejected_handle = rejected_instance;
    if (*rejected_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            rejected_destroy(*rejected_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* error */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                ERROR_HANDLE error;
                                if (amqpvalue_get_error(item_value, &error) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    rejected_destroy(*rejected_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                                else
                                {
                                    error_destroy(error);
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    rejected_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int rejected_get_error(REJECTED_HANDLE rejected, ERROR_HANDLE* error_value)
{
    int result;

    if (rejected == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)rejected;
        if (amqpvalue_get_composite_item_count(rejected_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(rejected_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_error(item_value, error_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int rejected_set_error(REJECTED_HANDLE rejected, ERROR_HANDLE error_value)
{
    int result;

    if (rejected == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        REJECTED_INSTANCE* rejected_instance = (REJECTED_INSTANCE*)rejected;
        AMQP_VALUE error_amqp_value = amqpvalue_create_error(error_value);
        if (error_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(rejected_instance->composite_value, 0, error_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(error_amqp_value);
        }
    }

    return result;
}


/* released */

typedef struct RELEASED_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} RELEASED_INSTANCE;

static RELEASED_HANDLE released_create_internal(void)
{
    RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)malloc(sizeof(RELEASED_INSTANCE));
    if (released_instance != NULL)
    {
        released_instance->composite_value = NULL;
    }

    return released_instance;
}

RELEASED_HANDLE released_create(void)
{
    RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)malloc(sizeof(RELEASED_INSTANCE));
    if (released_instance != NULL)
    {
        released_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(38);
        if (released_instance->composite_value == NULL)
        {
            free(released_instance);
            released_instance = NULL;
        }
    }

    return released_instance;
}

RELEASED_HANDLE released_clone(RELEASED_HANDLE value)
{
    RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)malloc(sizeof(RELEASED_INSTANCE));
    if (released_instance != NULL)
    {
        released_instance->composite_value = amqpvalue_clone(((RELEASED_INSTANCE*)value)->composite_value);
        if (released_instance->composite_value == NULL)
        {
            free(released_instance);
            released_instance = NULL;
        }
    }

    return released_instance;
}

void released_destroy(RELEASED_HANDLE released)
{
    if (released != NULL)
    {
        RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)released;
        amqpvalue_destroy(released_instance->composite_value);
        free(released_instance);
    }
}

AMQP_VALUE amqpvalue_create_released(RELEASED_HANDLE released)
{
    AMQP_VALUE result;

    if (released == NULL)
    {
        result = NULL;
    }
    else
    {
        RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)released;
        result = amqpvalue_clone(released_instance->composite_value);
    }

    return result;
}

bool is_released_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 38))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_released(AMQP_VALUE value, RELEASED_HANDLE* released_handle)
{
    int result;
    RELEASED_INSTANCE* released_instance = (RELEASED_INSTANCE*)released_create_internal();
    *released_handle = released_instance;
    if (*released_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            released_destroy(*released_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {

                    released_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}


/* modified */

typedef struct MODIFIED_INSTANCE_TAG
{
    AMQP_VALUE composite_value;
} MODIFIED_INSTANCE;

static MODIFIED_HANDLE modified_create_internal(void)
{
    MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)malloc(sizeof(MODIFIED_INSTANCE));
    if (modified_instance != NULL)
    {
        modified_instance->composite_value = NULL;
    }

    return modified_instance;
}

MODIFIED_HANDLE modified_create(void)
{
    MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)malloc(sizeof(MODIFIED_INSTANCE));
    if (modified_instance != NULL)
    {
        modified_instance->composite_value = amqpvalue_create_composite_with_ulong_descriptor(39);
        if (modified_instance->composite_value == NULL)
        {
            free(modified_instance);
            modified_instance = NULL;
        }
    }

    return modified_instance;
}

MODIFIED_HANDLE modified_clone(MODIFIED_HANDLE value)
{
    MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)malloc(sizeof(MODIFIED_INSTANCE));
    if (modified_instance != NULL)
    {
        modified_instance->composite_value = amqpvalue_clone(((MODIFIED_INSTANCE*)value)->composite_value);
        if (modified_instance->composite_value == NULL)
        {
            free(modified_instance);
            modified_instance = NULL;
        }
    }

    return modified_instance;
}

void modified_destroy(MODIFIED_HANDLE modified)
{
    if (modified != NULL)
    {
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        amqpvalue_destroy(modified_instance->composite_value);
        free(modified_instance);
    }
}

AMQP_VALUE amqpvalue_create_modified(MODIFIED_HANDLE modified)
{
    AMQP_VALUE result;

    if (modified == NULL)
    {
        result = NULL;
    }
    else
    {
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        result = amqpvalue_clone(modified_instance->composite_value);
    }

    return result;
}

bool is_modified_type_by_descriptor(AMQP_VALUE descriptor)
{
    bool result;

    uint64_t descriptor_ulong;
    if ((amqpvalue_get_ulong(descriptor, &descriptor_ulong) == 0) &&
        (descriptor_ulong == 39))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


int amqpvalue_get_modified(AMQP_VALUE value, MODIFIED_HANDLE* modified_handle)
{
    int result;
    MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified_create_internal();
    *modified_handle = modified_instance;
    if (*modified_handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE list_value = amqpvalue_get_inplace_described_value(value);
        if (list_value == NULL)
        {
            modified_destroy(*modified_handle);
            result = MU_FAILURE;
        }
        else
        {
            uint32_t list_item_count;
            if (amqpvalue_get_list_item_count(list_value, &list_item_count) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                do
                {
                    AMQP_VALUE item_value;
                    /* delivery-failed */
                    if (list_item_count > 0)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 0);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool delivery_failed;
                                if (amqpvalue_get_boolean(item_value, &delivery_failed) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    modified_destroy(*modified_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* undeliverable-here */
                    if (list_item_count > 1)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 1);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                bool undeliverable_here;
                                if (amqpvalue_get_boolean(item_value, &undeliverable_here) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    modified_destroy(*modified_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }
                    /* message-annotations */
                    if (list_item_count > 2)
                    {
                        item_value = amqpvalue_get_list_item(list_value, 2);
                        if (item_value == NULL)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            if (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL)
                            {
                                /* no error, field is not mandatory */
                            }
                            else
                            {
                                fields message_annotations;
                                if (amqpvalue_get_fields(item_value, &message_annotations) != 0)
                                {
                                    amqpvalue_destroy(item_value);
                                    modified_destroy(*modified_handle);
                                    result = MU_FAILURE;
                                    break;
                                }
                            }

                            amqpvalue_destroy(item_value);
                        }
                    }

                    modified_instance->composite_value = amqpvalue_clone(value);

                    result = 0;
                } while(0);
            }
        }
    }

    return result;
}

int modified_get_delivery_failed(MODIFIED_HANDLE modified, bool* delivery_failed_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        if (amqpvalue_get_composite_item_count(modified_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(modified_instance->composite_value, 0);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, delivery_failed_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int modified_set_delivery_failed(MODIFIED_HANDLE modified, bool delivery_failed_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        AMQP_VALUE delivery_failed_amqp_value = amqpvalue_create_boolean(delivery_failed_value);
        if (delivery_failed_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(modified_instance->composite_value, 0, delivery_failed_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(delivery_failed_amqp_value);
        }
    }

    return result;
}

int modified_get_undeliverable_here(MODIFIED_HANDLE modified, bool* undeliverable_here_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        if (amqpvalue_get_composite_item_count(modified_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 1)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(modified_instance->composite_value, 1);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_boolean(item_value, undeliverable_here_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int modified_set_undeliverable_here(MODIFIED_HANDLE modified, bool undeliverable_here_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        AMQP_VALUE undeliverable_here_amqp_value = amqpvalue_create_boolean(undeliverable_here_value);
        if (undeliverable_here_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(modified_instance->composite_value, 1, undeliverable_here_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(undeliverable_here_amqp_value);
        }
    }

    return result;
}

int modified_get_message_annotations(MODIFIED_HANDLE modified, fields* message_annotations_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint32_t item_count;
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        if (amqpvalue_get_composite_item_count(modified_instance->composite_value, &item_count) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (item_count <= 2)
            {
                result = MU_FAILURE;
            }
            else
            {
                AMQP_VALUE item_value = amqpvalue_get_composite_item_in_place(modified_instance->composite_value, 2);
                if ((item_value == NULL) ||
                    (amqpvalue_get_type(item_value) == AMQP_TYPE_NULL))
                {
                    result = MU_FAILURE;
                }
                else
                {
                    int get_single_value_result = amqpvalue_get_fields(item_value, message_annotations_value);
                    if (get_single_value_result != 0)
                    {
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int modified_set_message_annotations(MODIFIED_HANDLE modified, fields message_annotations_value)
{
    int result;

    if (modified == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        MODIFIED_INSTANCE* modified_instance = (MODIFIED_INSTANCE*)modified;
        AMQP_VALUE message_annotations_amqp_value = amqpvalue_create_fields(message_annotations_value);
        if (message_annotations_amqp_value == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_composite_item(modified_instance->composite_value, 2, message_annotations_amqp_value) != 0)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(message_annotations_amqp_value);
        }
    }

    return result;
}


