// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/amqp_definitions.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/amqpvalue.h"

typedef struct BODY_AMQP_DATA_TAG
{
    unsigned char* body_data_section_bytes;
    size_t body_data_section_length;
} BODY_AMQP_DATA;

typedef struct MESSAGE_INSTANCE_TAG
{
    BODY_AMQP_DATA* body_amqp_data_items;
    size_t body_amqp_data_count;
    AMQP_VALUE* body_amqp_sequence_items;
    size_t body_amqp_sequence_count;
    AMQP_VALUE body_amqp_value;
    HEADER_HANDLE header;
    delivery_annotations delivery_annotations;
    message_annotations message_annotations;
    PROPERTIES_HANDLE properties;
    application_properties application_properties;
    annotations footer;
    uint32_t message_format;
} MESSAGE_INSTANCE;

MESSAGE_BODY_TYPE internal_get_body_type(MESSAGE_HANDLE message)
{
    MESSAGE_BODY_TYPE result;

    if (message->body_amqp_value != NULL)
    {
        result = MESSAGE_BODY_TYPE_VALUE;
    }
    else if (message->body_amqp_data_count > 0)
    {
        result = MESSAGE_BODY_TYPE_DATA;
    }
    else if (message->body_amqp_sequence_count > 0)
    {
        result = MESSAGE_BODY_TYPE_SEQUENCE;
    }
    else
    {
        result = MESSAGE_BODY_TYPE_NONE;
    }

    return result;
}

static void free_all_body_data_items(MESSAGE_HANDLE message)
{
    size_t i;

    for (i = 0; i < message->body_amqp_data_count; i++)
    {
        if (message->body_amqp_data_items[i].body_data_section_bytes != NULL)
        {
            free(message->body_amqp_data_items[i].body_data_section_bytes);
        }
    }

    if (message->body_amqp_data_items != NULL)
    {
        free(message->body_amqp_data_items);
    }
    message->body_amqp_data_count = 0;
    message->body_amqp_data_items = NULL;
}

static void free_all_body_sequence_items(MESSAGE_HANDLE message)
{
    size_t i;

    for (i = 0; i < message->body_amqp_sequence_count; i++)
    {
        if (message->body_amqp_sequence_items[i] != NULL)
        {
            /* Codes_SRS_MESSAGE_01_137: [ Each sequence shall be freed by calling `amqpvalue_destroy`. ]*/
            amqpvalue_destroy(message->body_amqp_sequence_items[i]);
        }
    }

    if (message->body_amqp_sequence_items != NULL)
    {
        free(message->body_amqp_sequence_items);
    }
    message->body_amqp_sequence_count = 0;
    message->body_amqp_sequence_items = NULL;
}

MESSAGE_HANDLE message_create(void)
{
    MESSAGE_HANDLE result = (MESSAGE_HANDLE)calloc(1, sizeof(MESSAGE_INSTANCE));
    if (result == NULL)
    {
        /* Codes_SRS_MESSAGE_01_002: [If allocating memory for the message fails, `message_create` shall fail and return NULL.] */
        LogError("Cannot allocate memory for message");
    }
    else
    {
        result->header = NULL;
        result->delivery_annotations = NULL;
        result->message_annotations = NULL;
        result->properties = NULL;
        result->application_properties = NULL;
        result->footer = NULL;
        result->body_amqp_data_items = NULL;
        result->body_amqp_data_count = 0;
        result->body_amqp_value = NULL;
        result->body_amqp_sequence_items = NULL;
        result->body_amqp_sequence_count = 0;

        /* Codes_SRS_MESSAGE_01_135: [ By default a message on which `message_set_message_format` was not called shall have message format set to 0. ]*/
        result->message_format = 0;
    }

    /* Codes_SRS_MESSAGE_01_001: [`message_create` shall create a new AMQP message instance and on success it shall return a non-NULL handle for the newly created message instance.] */
    return result;
}

MESSAGE_HANDLE message_clone(MESSAGE_HANDLE source_message)
{
    MESSAGE_HANDLE result;

    if (source_message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_062: [If `source_message` is NULL, `message_clone` shall fail and return NULL.] */
        LogError("NULL source_message");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MESSAGE_01_003: [`message_clone` shall clone a message entirely and on success return a non-NULL handle to the cloned message.] */
        result = (MESSAGE_HANDLE)message_create();
        if (result == NULL)
        {
            /* Codes_SRS_MESSAGE_01_004: [If allocating memory for the new cloned message fails, `message_clone` shall fail and return NULL.] */
            LogError("Cannot clone message");
        }
        else
        {
            result->message_format = source_message->message_format;

            if (source_message->header != NULL)
            {
                /* Codes_SRS_MESSAGE_01_005: [If a header exists on the source message it shall be cloned by using `header_clone`.] */
                result->header = header_clone(source_message->header);
                if (result->header == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone message header");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->delivery_annotations != NULL))
            {
                /* Codes_SRS_MESSAGE_01_006: [If delivery annotations exist on the source message they shall be cloned by using `annotations_clone`.] */
                result->delivery_annotations = annotations_clone(source_message->delivery_annotations);
                if (result->delivery_annotations == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone delivery annotations");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->message_annotations != NULL))
            {
                /* Codes_SRS_MESSAGE_01_007: [If message annotations exist on the source message they shall be cloned by using `annotations_clone`.] */
                result->message_annotations = annotations_clone(source_message->message_annotations);
                if (result->message_annotations == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone message annotations");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->properties != NULL))
            {
                /* Codes_SRS_MESSAGE_01_008: [If message properties exist on the source message they shall be cloned by using `properties_clone`.] */
                result->properties = properties_clone(source_message->properties);
                if (result->properties == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone message properties");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->application_properties != NULL))
            {
                /* Codes_SRS_MESSAGE_01_009: [If application properties exist on the source message they shall be cloned by using `amqpvalue_clone`.] */
                result->application_properties = amqpvalue_clone(source_message->application_properties);
                if (result->application_properties == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone application annotations");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->footer != NULL))
            {
                /* Codes_SRS_MESSAGE_01_010: [If a footer exists on the source message it shall be cloned by using `annotations_clone`.] */
                result->footer = amqpvalue_clone(source_message->footer);
                if (result->footer == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone message footer");
                    message_destroy(result);
                    result = NULL;
                }
            }

            if ((result != NULL) && (source_message->body_amqp_data_count > 0))
            {
                size_t i;

                result->body_amqp_data_items = (BODY_AMQP_DATA*)calloc(1, (source_message->body_amqp_data_count * sizeof(BODY_AMQP_DATA)));
                if (result->body_amqp_data_items == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot allocate memory for body data sections");
                    message_destroy(result);
                    result = NULL;
                }
                else
                {
                    for (i = 0; i < source_message->body_amqp_data_count; i++)
                    {
                        result->body_amqp_data_items[i].body_data_section_length = source_message->body_amqp_data_items[i].body_data_section_length;

                        /* Codes_SRS_MESSAGE_01_011: [If an AMQP data has been set as message body on the source message it shall be cloned by allocating memory for the binary payload.] */
                        result->body_amqp_data_items[i].body_data_section_bytes = (unsigned char*)malloc(source_message->body_amqp_data_items[i].body_data_section_length);
                        if (result->body_amqp_data_items[i].body_data_section_bytes == NULL)
                        {
                            LogError("Cannot allocate memory for body data section %u", (unsigned int)i);
                            break;
                        }
                        else
                        {
                            (void)memcpy(result->body_amqp_data_items[i].body_data_section_bytes, source_message->body_amqp_data_items[i].body_data_section_bytes, result->body_amqp_data_items[i].body_data_section_length);
                        }
                    }

                    result->body_amqp_data_count = i;
                    if (i < source_message->body_amqp_data_count)
                    {
                        /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                        message_destroy(result);
                        result = NULL;
                    }
                }
            }

            if ((result != NULL) && (source_message->body_amqp_sequence_count > 0))
            {
                size_t i;

                result->body_amqp_sequence_items = (AMQP_VALUE*)calloc(1, (source_message->body_amqp_sequence_count * sizeof(AMQP_VALUE)));
                if (result->body_amqp_sequence_items == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot allocate memory for body AMQP sequences");
                    message_destroy(result);
                    result = NULL;
                }
                else
                {
                    for (i = 0; i < source_message->body_amqp_sequence_count; i++)
                    {
                        /* Codes_SRS_MESSAGE_01_160: [ If AMQP sequences are set as AMQP body they shall be cloned by calling `amqpvalue_clone`. ] */
                        result->body_amqp_sequence_items[i] = amqpvalue_clone(source_message->body_amqp_sequence_items[i]);
                        if (result->body_amqp_sequence_items[i] == NULL)
                        {
                            LogError("Cannot clone AMQP sequence %u", (unsigned int)i);
                            break;
                        }
                    }

                    result->body_amqp_sequence_count = i;
                    if (i < source_message->body_amqp_sequence_count)
                    {
                        /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                        message_destroy(result);
                        result = NULL;
                    }
                }
            }

            if ((result != NULL) && (source_message->body_amqp_value != NULL))
            {
                /* Codes_SRS_MESSAGE_01_159: [If an AMQP value has been set as message body on the source message it shall be cloned by calling `amqpvalue_clone`. ]*/
                result->body_amqp_value = amqpvalue_clone(source_message->body_amqp_value);
                if (result->body_amqp_value == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_012: [ If any cloning operation for the members of the source message fails, then `message_clone` shall fail and return NULL. ]*/
                    LogError("Cannot clone body AMQP value");
                    message_destroy(result);
                    result = NULL;
                }
            }
        }
    }

    return result;
}

void message_destroy(MESSAGE_HANDLE message)
{
    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_014: [ If `message` is NULL, `message_destroy` shall do nothing. ]*/
        LogError("NULL message");
    }
    else
    {
        /* Codes_SRS_MESSAGE_01_013: [ `message_destroy` shall free all resources allocated by the message instance identified by the `message` argument. ]*/
        if (message->header != NULL)
        {
            /* Codes_SRS_MESSAGE_01_015: [ The message header shall be freed by calling `header_destroy`. ]*/
            header_destroy(message->header);
        }

        if (message->delivery_annotations != NULL)
        {
            /* Codes_SRS_MESSAGE_01_016: [ The delivery annotations shall be freed by calling `annotations_destroy`. ]*/
            annotations_destroy(message->delivery_annotations);
        }

        if (message->message_annotations != NULL)
        {
            /* Codes_SRS_MESSAGE_01_017: [ The message annotations shall be freed by calling `annotations_destroy`. ]*/
            annotations_destroy(message->message_annotations);
        }

        if (message->properties != NULL)
        {
            /* Codes_SRS_MESSAGE_01_018: [ The message properties shall be freed by calling `properties_destroy`. ]*/
            properties_destroy(message->properties);
        }

        if (message->application_properties != NULL)
        {
            /* Codes_SRS_MESSAGE_01_019: [ The application properties shall be freed by calling `amqpvalue_destroy`. ]*/
            application_properties_destroy(message->application_properties);
        }

        if (message->footer != NULL)
        {
            /* Codes_SRS_MESSAGE_01_020: [ The message footer shall be freed by calling `annotations_destroy`. ]*/
            annotations_destroy(message->footer);
        }

        if (message->body_amqp_value != NULL)
        {
            /* Codes_SRS_MESSAGE_01_021: [ If the message body is made of an AMQP value, the value shall be freed by calling `amqpvalue_destroy`. ]*/
            amqpvalue_destroy(message->body_amqp_value);
        }

        /* Codes_SRS_MESSAGE_01_136: [ If the message body is made of several AMQP data items, they shall all be freed. ]*/
        free_all_body_data_items(message);

        /* Codes_SRS_MESSAGE_01_136: [ If the message body is made of several AMQP sequences, they shall all be freed. ]*/
        free_all_body_sequence_items(message);
        free(message);
    }
}

int message_set_header(MESSAGE_HANDLE message, HEADER_HANDLE header)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_024: [ If `message` is NULL, `message_set_header` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        HEADER_HANDLE new_header;

        if (header == NULL)
        {
            /* Codes_SRS_MESSAGE_01_139: [ If `message_header` is NULL, the previously stored header associated with `message` shall be freed. ]*/
            if (message->header != NULL)
            {
                header_destroy(message->header);
                message->header = NULL;
            }

            /* Codes_SRS_MESSAGE_01_023: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_022: [ `message_set_header` shall copy the contents of `message_header` as the header for the message instance identified by message. ]*/
            /* Codes_SRS_MESSAGE_01_025: [ Cloning the header shall be done by calling `header_clone`. ]*/
            new_header = header_clone(header);
            if (new_header == NULL)
            {
                /* Codes_SRS_MESSAGE_01_026: [ If `header_clone` fails, `message_set_header` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message header");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_138: [ If setting the header fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->header != NULL)
                {
                    header_destroy(message->header);
                }

                message->header = new_header;

                /* Codes_SRS_MESSAGE_01_023: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_header(MESSAGE_HANDLE message, HEADER_HANDLE* header)
{
    int result;

    if ((message == NULL) ||
        (header == NULL))
    {
        /* Codes_SRS_MESSAGE_01_029: [ If `message` or `message_header` is NULL, `message_get_header` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, header = %p",
            message, header);
        result = MU_FAILURE;
    }
    else
    {
        if (message->header == NULL)
        {
            /* Codes_SRS_MESSAGE_01_143: [ If no header has been set, `message_get_header` shall set `message_header` to NULL. ]*/
            *header = NULL;

            /* Codes_SRS_MESSAGE_01_028: [ On success, `message_get_header` shall return 0.]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_027: [ `message_get_header` shall copy the contents of header for the message instance identified by `message` into the argument `message_header`. ]*/
            /* Codes_SRS_MESSAGE_01_030: [ Cloning the header shall be done by calling `header_clone`. ]*/
            *header = header_clone(message->header);
            if (*header == NULL)
            {
                /* Codes_SRS_MESSAGE_01_031: [ If `header_clone` fails, `message_get_header` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message header");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_028: [ On success, `message_get_header` shall return 0.]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_set_delivery_annotations(MESSAGE_HANDLE message, delivery_annotations annotations)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_034: [ If `message` is NULL, `message_set_delivery_annotations` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        delivery_annotations new_delivery_annotations;

        if (annotations == NULL)
        {
            /* Codes_SRS_MESSAGE_01_141: [ If `annotations` is NULL, the previously stored delivery annotations associated with `message` shall be freed. ]*/
            if (message->delivery_annotations != NULL)
            {
                annotations_destroy(message->delivery_annotations);
                message->delivery_annotations = NULL;
            }

            /* Codes_SRS_MESSAGE_01_033: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_032: [ `message_set_delivery_annotations` shall copy the contents of `annotations` as the delivery annotations for the message instance identified by `message`. ]*/
            /* Codes_SRS_MESSAGE_01_035: [ Cloning the delivery annotations shall be done by calling `annotations_clone`. ]*/
            new_delivery_annotations = annotations_clone(annotations);
            if (new_delivery_annotations == NULL)
            {
                /* Codes_SRS_MESSAGE_01_036: [ If `annotations_clone` fails, `message_set_delivery_annotations` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone delivery annotations");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_140: [** If setting the delivery annotations fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->delivery_annotations != NULL)
                {
                    annotations_destroy(message->delivery_annotations);
                }

                message->delivery_annotations = new_delivery_annotations;

                /* Codes_SRS_MESSAGE_01_033: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_delivery_annotations(MESSAGE_HANDLE message, delivery_annotations* annotations)
{
    int result;

    if ((message == NULL) ||
        (annotations == NULL))
    {
        /* Codes_SRS_MESSAGE_01_039: [ If `message` or `annotations` is NULL, `message_get_delivery_annotations` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, annotations = %p",
            message, annotations);
        result = MU_FAILURE;
    }
    else
    {
        if (message->delivery_annotations == NULL)
        {
            /* Codes_SRS_MESSAGE_01_142: [ If no delivery annotations have been set, `message_get_delivery_annotations` shall set `annotations` to NULL. ]*/
            *annotations = NULL;

            /* Codes_SRS_MESSAGE_01_038: [ On success, `message_get_delivery_annotations` shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_037: [ `message_get_delivery_annotations` shall copy the contents of delivery annotations for the message instance identified by `message` into the argument `annotations`. ]*/
            /* Codes_SRS_MESSAGE_01_040: [ Cloning the delivery annotations shall be done by calling `annotations_clone`. ]*/
            *annotations = annotations_clone(message->delivery_annotations);
            if (*annotations == NULL)
            {
                /* Codes_SRS_MESSAGE_01_041: [ If `annotations_clone` fails, `message_get_delivery_annotations` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone delivery annotations");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_038: [ On success, `message_get_delivery_annotations` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_set_message_annotations(MESSAGE_HANDLE message, message_annotations annotations)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_044: [ If `message` is NULL, `message_set_message_annotations` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        if (annotations == NULL)
        {
            /* Codes_SRS_MESSAGE_01_145: [ If `annotations` is NULL, the previously stored message annotations associated with `message` shall be freed. ]*/
            if (message->message_annotations != NULL)
            {
                annotations_destroy(message->message_annotations);
                message->message_annotations = NULL;
            }

            /* Codes_SRS_MESSAGE_01_043: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            message_annotations new_message_annotations;

            /* Codes_SRS_MESSAGE_01_042: [ `message_set_message_annotations` shall copy the contents of `annotations` as the message annotations for the message instance identified by `message`. ]*/
            /* Codes_SRS_MESSAGE_01_045: [ Cloning the message annotations shall be done by calling `annotations_clone`. ]*/
            new_message_annotations = annotations_clone(annotations);
            if (new_message_annotations == NULL)
            {
                /* Codes_SRS_MESSAGE_01_046: [ If `annotations_clone` fails, `message_set_message_annotations` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message annotations");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_144: [ If setting the message annotations fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->message_annotations != NULL)
                {
                    annotations_destroy(message->message_annotations);
                }

                message->message_annotations = new_message_annotations;

                /* Codes_SRS_MESSAGE_01_043: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_message_annotations(MESSAGE_HANDLE message, annotations* message_annotations)
{
    int result;

    if ((message == NULL) ||
        (message_annotations == NULL))
    {
        /* Codes_SRS_MESSAGE_01_049: [ If `message` or `annotations` is NULL, `message_get_message_annotations` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, message_annotations = %p",
            message, message_annotations);
        result = MU_FAILURE;
    }
    else
    {
        if (message->message_annotations == NULL)
        {
            /* Codes_SRS_MESSAGE_01_146: [ If no message annotations have been set, `message_get_message_annotations` shall set `annotations` to NULL. ]*/
            *message_annotations = NULL;

            /* Codes_SRS_MESSAGE_01_048: [ On success, `message_get_message_annotations` shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_047: [ `message_get_message_annotations` shall copy the contents of message annotations for the message instance identified by `message` into the argument `annotations`. ]*/
            /* Codes_SRS_MESSAGE_01_050: [ Cloning the message annotations shall be done by calling `annotations_clone`. ]*/
            *message_annotations = annotations_clone(message->message_annotations);
            if (*message_annotations == NULL)
            {
                /* Codes_SRS_MESSAGE_01_051: [ If `annotations_clone` fails, `message_get_message_annotations` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message annotations");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_048: [ On success, `message_get_message_annotations` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_set_properties(MESSAGE_HANDLE message, PROPERTIES_HANDLE properties)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_054: [ If `message` is NULL, `message_set_properties` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        if (properties == NULL)
        {
            /* Codes_SRS_MESSAGE_01_147: [ If `properties` is NULL, the previously stored message properties associated with `message` shall be freed. ]*/
            if (message->properties != NULL)
            {
                properties_destroy(message->properties);
                message->properties = NULL;
            }

            /* Codes_SRS_MESSAGE_01_053: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            PROPERTIES_HANDLE new_properties;

            /* Codes_SRS_MESSAGE_01_052: [ `message_set_properties` shall copy the contents of `properties` as the message properties for the message instance identified by `message`. ]*/
            /* Codes_SRS_MESSAGE_01_055: [ Cloning the message properties shall be done by calling `properties_clone`. ]*/
            new_properties = properties_clone(properties);
            if (new_properties == NULL)
            {
                /* Codes_SRS_MESSAGE_01_056: [ If `properties_clone` fails, `message_set_properties` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_063: [ If setting the message properties fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->properties != NULL)
                {
                    properties_destroy(message->properties);
                }

                message->properties = new_properties;

                /* Codes_SRS_MESSAGE_01_053: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_properties(MESSAGE_HANDLE message, PROPERTIES_HANDLE* properties)
{
    int result;

    if ((message == NULL) ||
        (properties == NULL))
    {
        /* Codes_SRS_MESSAGE_01_059: [ If `message` or `properties` is NULL, `message_get_properties` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, properties = %p",
            message, properties);
        result = MU_FAILURE;
    }
    else
    {
        if (message->properties == NULL)
        {
            /* Codes_SRS_MESSAGE_01_148: [ If no message properties have been set, `message_get_properties` shall set `properties` to NULL. ]*/
            *properties = NULL;

            /* Codes_SRS_MESSAGE_01_058: [ On success, `message_get_properties` shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_057: [ `message_get_properties` shall copy the contents of message properties for the message instance identified by `message` into the argument `properties`. ]*/
            /* Codes_SRS_MESSAGE_01_060: [ Cloning the message properties shall be done by calling `properties_clone`. ]*/
            *properties = properties_clone(message->properties);
            if (*properties == NULL)
            {
                /* Codes_SRS_MESSAGE_01_061: [ If `properties_clone` fails, `message_get_properties` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_058: [ On success, `message_get_properties` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_set_application_properties(MESSAGE_HANDLE message, AMQP_VALUE application_properties)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_066: [ If `message` is NULL, `message_set_application_properties` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE new_application_properties;

        /* Tests_SRS_MESSAGE_01_149: [ If `application_properties` is NULL, the previously stored application properties associated with `message` shall be freed. ]*/
        if (application_properties == NULL)
        {
            if (message->application_properties != NULL)
            {
                amqpvalue_destroy(message->application_properties);
                message->application_properties = NULL;
            }

            /* Codes_SRS_MESSAGE_01_065: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_064: [ `message_set_application_properties` shall copy the contents of `application_properties` as the application properties for the message instance identified by `message`. ]*/
            /* Codes_SRS_MESSAGE_01_067: [ Cloning the message properties shall be done by calling `application_properties_clone`. ]*/
            new_application_properties = application_properties_clone(application_properties);
            if (new_application_properties == NULL)
            {
                /* Codes_SRS_MESSAGE_01_068: [ If `application_properties_clone` fails, `message_set_application_properties` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone application properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_069: [ If setting the application properties fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->application_properties != NULL)
                {
                    amqpvalue_destroy(message->application_properties);
                }

                message->application_properties = new_application_properties;

                /* Codes_SRS_MESSAGE_01_065: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_application_properties(MESSAGE_HANDLE message, AMQP_VALUE* application_properties)
{
    int result;

    if ((message == NULL) ||
        (application_properties == NULL))
    {
        /* Codes_SRS_MESSAGE_01_072: [ If `message` or `application_properties` is NULL, `message_get_application_properties` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, application_properties = %p",
            message, application_properties);
        result = MU_FAILURE;
    }
    else
    {
        if (message->application_properties == NULL)
        {
            /* Codes_SRS_MESSAGE_01_150: [ If no application properties have been set, `message_get_application_properties` shall set `application_properties` to NULL. ]*/
            *application_properties = NULL;

            /* Codes_SRS_MESSAGE_01_071: [ On success, `message_get_application_properties` shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_070: [ `message_get_application_properties` shall copy the contents of application message properties for the message instance identified by `message` into the argument `application_properties`. ]*/
            /* Codes_SRS_MESSAGE_01_073: [ Cloning the application properties shall be done by calling `application_properties_clone`. ]*/
            *application_properties = application_properties_clone(message->application_properties);
            if (*application_properties == NULL)
            {
                /* Codes_SRS_MESSAGE_01_074: [ If `application_properties_clone` fails, `message_get_application_properties` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone application properties");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_071: [ On success, `message_get_application_properties` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_set_footer(MESSAGE_HANDLE message, annotations footer)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_077: [ If `message` is NULL, `message_set_footer` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        if (footer == NULL)
        {
            /* Codes_SRS_MESSAGE_01_151: [ If `footer` is NULL, the previously stored footer associated with `message` shall be freed. ]*/
            if (message->footer != NULL)
            {
                annotations_destroy(message->footer);
                message->footer = NULL;
            }

            /* Codes_SRS_MESSAGE_01_076: [ On success it shall return 0. ]*/
            result = 0;
        }
        else
        {
            annotations new_footer;

            /* Codes_SRS_MESSAGE_01_075: [ `message_set_footer` shall copy the contents of `footer` as the footer contents for the message instance identified by `message`. ]*/
            /* Codes_SRS_MESSAGE_01_078: [ Cloning the footer shall be done by calling `annotations_clone`. ]*/
            new_footer = annotations_clone(footer);
            if (new_footer == NULL)
            {
                /* Codes_SRS_MESSAGE_01_079: [ If `annotations_clone` fails, `message_set_footer` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message footer");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_080: [ If setting the footer fails, the previous value shall be preserved. ]*/
                /* Only do the free of the previous value if we could clone the new one*/
                if (message->footer != NULL)
                {
                    annotations_destroy(message->footer);
                }

                message->footer = new_footer;

                /* Codes_SRS_MESSAGE_01_076: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_footer(MESSAGE_HANDLE message, annotations* footer)
{
    int result;

    if ((message == NULL) ||
        (footer == NULL))
    {
        /* Codes_SRS_MESSAGE_01_083: [ If `message` or `footer` is NULL, `message_get_footer` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, footer = %p",
            message, footer);
        result = MU_FAILURE;
    }
    else
    {
        if (message->footer == NULL)
        {
            /* Codes_SRS_MESSAGE_01_152: [ If no footer has been set, `message_get_footer` shall set `footer` to NULL. ]*/
            *footer = NULL;

            /* Codes_SRS_MESSAGE_01_082: [ On success, `message_get_footer` shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_081: [ `message_get_footer` shall copy the contents of footer for the message instance identified by `message` into the argument `footer`. ]*/
            /* Codes_SRS_MESSAGE_01_084: [ Cloning the footer shall be done by calling `annotations_clone`. ]*/
            *footer = annotations_clone(message->footer);
            if (*footer == NULL)
            {
                /* Codes_SRS_MESSAGE_01_085: [ If `annotations_clone` fails, `message_get_footer` shall fail and return a non-zero value. ]*/
                LogError("Cannot clone message footer");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_082: [ On success, `message_get_footer` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_add_body_amqp_data(MESSAGE_HANDLE message, BINARY_DATA amqp_data)
{
    int result;

    /* Codes_SRS_MESSAGE_01_088: [ If `message` is NULL, `message_add_body_amqp_data` shall fail and return a non-zero value. ]*/
    if ((message == NULL) ||
        /* Tests_SRS_MESSAGE_01_089: [ If the `bytes` member of `amqp_data` is NULL and the `size` member is non-zero, `message_add_body_amqp_data` shall fail and return a non-zero value. ]*/
        ((amqp_data.bytes == NULL) &&
         (amqp_data.length != 0)))
    {
        LogError("Bad arguments: message = %p, bytes = %p, length = %u",
            message, amqp_data.bytes, (unsigned int)amqp_data.length);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if ((body_type == MESSAGE_BODY_TYPE_SEQUENCE) ||
            (body_type == MESSAGE_BODY_TYPE_VALUE))
        {
            /* Codes_SRS_MESSAGE_01_091: [ If the body was already set to an AMQP value or a list of AMQP sequences, `message_add_body_amqp_data` shall fail and return a non-zero value. ]*/
            LogError("Body type already set");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_086: [ `message_add_body_amqp_data` shall add the contents of `amqp_data` to the list of AMQP data values for the body of the message identified by `message`. ]*/
            BODY_AMQP_DATA* new_body_amqp_data_items = (BODY_AMQP_DATA*)realloc(message->body_amqp_data_items, sizeof(BODY_AMQP_DATA) * (message->body_amqp_data_count + 1));
            if (new_body_amqp_data_items == NULL)
            {
                /* Codes_SRS_MESSAGE_01_153: [ If allocating memory to store the added AMQP data fails, `message_add_body_amqp_data` shall fail and return a non-zero value. ]*/
                LogError("Cannot allocate memory for body AMQP data items");
                result = MU_FAILURE;
            }
            else
            {
                message->body_amqp_data_items = new_body_amqp_data_items;

                if (amqp_data.length == 0)
                {
                    message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_bytes = NULL;
                    message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_length = 0;
                    message->body_amqp_data_count++;

                    /* Codes_SRS_MESSAGE_01_087: [ On success it shall return 0. ]*/
                    result = 0;
                }
                else
                {
                    message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_bytes = (unsigned char*)malloc(amqp_data.length);
                    if (message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_bytes == NULL)
                    {
                        /* Codes_SRS_MESSAGE_01_153: [ If allocating memory to store the added AMQP data fails, `message_add_body_amqp_data` shall fail and return a non-zero value. ]*/
                        LogError("Cannot allocate memory for body AMQP data to be added");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_length = amqp_data.length;
                        (void)memcpy(message->body_amqp_data_items[message->body_amqp_data_count].body_data_section_bytes, amqp_data.bytes, amqp_data.length);
                        message->body_amqp_data_count++;

                        /* Codes_SRS_MESSAGE_01_087: [ On success it shall return 0. ]*/
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int message_get_body_amqp_data_in_place(MESSAGE_HANDLE message, size_t index, BINARY_DATA* amqp_data)
{
    int result;

    if ((message == NULL) ||
        (amqp_data == NULL))
    {
        /* Codes_SRS_MESSAGE_01_094: [ If `message` or `amqp_data` is NULL, `message_get_body_amqp_data_in_place` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, amqp_data = %p",
            message, amqp_data);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if (body_type != MESSAGE_BODY_TYPE_DATA)
        {
            /* Codes_SRS_MESSAGE_01_096: [ If the body for `message` is not of type `MESSAGE_BODY_TYPE_DATA`, `message_get_body_amqp_data_in_place` shall fail and return a non-zero value. ]*/
            LogError("Body type is not AMQP data");
            result = MU_FAILURE;
        }
        else if (index >= message->body_amqp_data_count)
        {
            /* Codes_SRS_MESSAGE_01_095: [ If `index` indicates an AMQP data entry that is out of bounds, `message_get_body_amqp_data_in_place` shall fail and return a non-zero value. ]*/
            LogError("Index too high for AMQP data (%lu), number of AMQP data entries is %lu",
                (unsigned long)index, (unsigned long)message->body_amqp_data_count);
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_092: [ `message_get_body_amqp_data_in_place` shall place the contents of the `index`th AMQP data for the message instance identified by `message` into the argument `amqp_data`, without copying the binary payload memory. ]*/
            amqp_data->bytes = message->body_amqp_data_items[index].body_data_section_bytes;
            amqp_data->length = message->body_amqp_data_items[index].body_data_section_length;

            /* Codes_SRS_MESSAGE_01_093: [ On success, `message_get_body_amqp_data_in_place` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int message_get_body_amqp_data_count(MESSAGE_HANDLE message, size_t* count)
{
    int result;

    if ((message == NULL) ||
        (count == NULL))
    {
        /* Codes_SRS_MESSAGE_01_099: [ If `message` or `count` is NULL, `message_get_body_amqp_data_count` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, count = %p",
            message, count);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if (body_type != MESSAGE_BODY_TYPE_DATA)
        {
            /* Codes_SRS_MESSAGE_01_100: [ If the body for `message` is not of type `MESSAGE_BODY_TYPE_DATA`, `message_get_body_amqp_data_count` shall fail and return a non-zero value. ]*/
            LogError("Body type is not AMQP data");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_097: [ `message_get_body_amqp_data_count` shall fill in `count` the number of AMQP data chunks that are stored by the message identified by `message`. ]*/
            *count = message->body_amqp_data_count;

            /* Codes_SRS_MESSAGE_01_098: [ On success, `message_get_body_amqp_data_count` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int message_set_body_amqp_value(MESSAGE_HANDLE message, AMQP_VALUE body_amqp_value)
{
    int result;

    if ((message == NULL) ||
        (body_amqp_value == NULL))
    {
        /* Codes_SRS_MESSAGE_01_103: [ If `message` or `body_amqp_value` is NULL, `message_set_body_amqp_value` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, body_amqp_value = %p",
            message, body_amqp_value);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if ((body_type == MESSAGE_BODY_TYPE_DATA) ||
            (body_type == MESSAGE_BODY_TYPE_SEQUENCE))
        {
            /* Codes_SRS_MESSAGE_01_105: [ If the body was already set to an AMQP data list or a list of AMQP sequences, `message_set_body_amqp_value` shall fail and return a non-zero value. ]*/
            LogError("Body is already set to another body type");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_154: [ Cloning the amqp value shall be done by calling `amqpvalue_clone`. ]*/
            AMQP_VALUE new_amqp_value = amqpvalue_clone(body_amqp_value);
            if (new_amqp_value == NULL)
            {
                LogError("Cannot clone body AMQP value");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_104: [ If setting the body AMQP value fails, the previous value shall be preserved. ]*/
                /* Only free the previous value when cloning is succesfull */
                if (message->body_amqp_value != NULL)
                {
                    amqpvalue_destroy(body_amqp_value);
                }

                /* Codes_SRS_MESSAGE_01_101: [ `message_set_body_amqp_value` shall set the contents of body as being the AMQP value indicate by `body_amqp_value`. ]*/
                message->body_amqp_value = new_amqp_value;

                /* Codes_SRS_MESSAGE_01_102: [ On success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_body_amqp_value_in_place(MESSAGE_HANDLE message, AMQP_VALUE* body_amqp_value)
{
    int result;

    if ((message == NULL) ||
        (body_amqp_value == NULL))
    {
        /* Codes_SRS_MESSAGE_01_108: [ If `message` or `body_amqp_value` is NULL, `message_get_body_amqp_value_in_place` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, body_amqp_value = %p",
            message, body_amqp_value);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if (body_type != MESSAGE_BODY_TYPE_VALUE)
        {
            /* Codes_SRS_MESSAGE_01_109: [ If the body for `message` is not of type `MESSAGE_BODY_TYPE_VALUE`, `message_get_body_amqp_value_in_place` shall fail and return a non-zero value. ]*/
            LogError("Body is not of type AMQP value");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_106: [ `message_get_body_amqp_value_in_place` shall get the body AMQP value for the message instance identified by `message` in place (not cloning) into the `body_amqp_value` argument. ]*/
            *body_amqp_value = message->body_amqp_value;

            /* Codes_SRS_MESSAGE_01_107: [ On success, `message_get_body_amqp_value_in_place` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int message_add_body_amqp_sequence(MESSAGE_HANDLE message, AMQP_VALUE sequence_list)
{
    int result;

    if ((message == NULL) ||
        (sequence_list == NULL))
    {
        /* Codes_SRS_MESSAGE_01_112: [ If `message` or `sequence` is NULL, `message_add_body_amqp_sequence` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, sequence_list = %p",
            message, sequence_list);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if ((body_type == MESSAGE_BODY_TYPE_DATA) ||
            (body_type == MESSAGE_BODY_TYPE_VALUE))
        {
            /* Codes_SRS_MESSAGE_01_115: [ If the body was already set to an AMQP data list or an AMQP value, `message_add_body_amqp_sequence` shall fail and return a non-zero value. ]*/
            LogError("Body is already set to another body type");
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE* new_body_amqp_sequence_items = (AMQP_VALUE*)realloc(message->body_amqp_sequence_items, sizeof(AMQP_VALUE) * (message->body_amqp_sequence_count + 1));
            if (new_body_amqp_sequence_items == NULL)
            {
                /* Codes_SRS_MESSAGE_01_158: [ If allocating memory in order to store the sequence fails, `message_add_body_amqp_sequence` shall fail and return a non-zero value. ]*/
                LogError("Cannot allocate enough memory for sequence items");
                result = MU_FAILURE;
            }
            else
            {
                message->body_amqp_sequence_items = new_body_amqp_sequence_items;

                /* Codes_SRS_MESSAGE_01_110: [ `message_add_body_amqp_sequence` shall add the contents of `sequence` to the list of AMQP sequences for the body of the message identified by `message`. ]*/
                /* Codes_SRS_MESSAGE_01_156: [ The AMQP sequence shall be cloned by calling `amqpvalue_clone`. ]*/
                message->body_amqp_sequence_items[message->body_amqp_sequence_count] = amqpvalue_clone(sequence_list);
                if (message->body_amqp_sequence_items[message->body_amqp_sequence_count] == NULL)
                {
                    /* Codes_SRS_MESSAGE_01_157: [ If `amqpvalue_clone` fails, `message_add_body_amqp_sequence` shall fail and return a non-zero value. ]*/
                    LogError("Cloning sequence failed");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_MESSAGE_01_114: [ If adding the AMQP sequence fails, the previous value shall be preserved. ]*/
                    message->body_amqp_sequence_count++;

                    /* Codes_SRS_MESSAGE_01_111: [ On success it shall return 0. ]*/
                    result = 0;
                }
            }
        }
    }

    return result;
}

int message_get_body_amqp_sequence_in_place(MESSAGE_HANDLE message, size_t index, AMQP_VALUE* sequence)
{
    int result;

    if ((message == NULL) ||
        (sequence == NULL))
    {
        /* Codes_SRS_MESSAGE_01_118: [ If `message` or `sequence` is NULL, `message_get_body_amqp_sequence_in_place` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, sequence = %p",
            message, sequence);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if (body_type != MESSAGE_BODY_TYPE_SEQUENCE)
        {
            /* Codes_SRS_MESSAGE_01_120: [ If the body for `message` is not of type `MESSAGE_BODY_TYPE_SEQUENCE`, `message_get_body_amqp_sequence_in_place` shall fail and return a non-zero value. ]*/
            LogError("Body is not of type SEQUENCE");
            result = MU_FAILURE;
        }
        else
        {
            if (index >= message->body_amqp_sequence_count)
            {
                /* Codes_SRS_MESSAGE_01_119: [ If `index` indicates an AMQP sequence entry that is out of bounds, `message_get_body_amqp_sequence_in_place` shall fail and return a non-zero value. ]*/
                LogError("Index too high for AMQP sequence (%lu), maximum is %lu",
                    (unsigned long)index, (unsigned long)message->body_amqp_sequence_count);
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_MESSAGE_01_116: [ `message_get_body_amqp_sequence_in_place` shall return in `sequence` the content of the `index`th AMQP seuquence entry for the message instance identified by `message`. ]*/
                *sequence = message->body_amqp_sequence_items[index];

                /* Codes_SRS_MESSAGE_01_117: [ On success, `message_get_body_amqp_sequence_in_place` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int message_get_body_amqp_sequence_count(MESSAGE_HANDLE message, size_t* count)
{
    int result;

    if ((message == NULL) ||
        (count == NULL))
    {
        /* Codes_SRS_MESSAGE_01_123: [ If `message` or `count` is NULL, `message_get_body_amqp_sequence_count` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, count = %p",
            message, count);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_BODY_TYPE body_type = internal_get_body_type(message);
        if (body_type != MESSAGE_BODY_TYPE_SEQUENCE)
        {
            /* Codes_SRS_MESSAGE_01_124: [ If the body for `message` is not of type `MESSAGE_BODY_TYPE_SEQUENCE`, `message_get_body_amqp_sequence_count` shall fail and return a non-zero value. ]*/
            LogError("Body is not of type SEQUENCE");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_121: [ `message_get_body_amqp_sequence_count` shall fill in `count` the number of AMQP sequences that are stored by the message identified by `message`. ]*/
            *count = message->body_amqp_sequence_count;

            /* Codes_SRS_MESSAGE_01_122: [ On success, `message_get_body_amqp_sequence_count` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int message_get_body_type(MESSAGE_HANDLE message, MESSAGE_BODY_TYPE* body_type)
{
    int result;

    if ((message == NULL) ||
        (body_type == NULL))
    {
        /* Codes_SRS_MESSAGE_01_127: [ If `message` or `body_type` is NULL, `message_get_body_type` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, body_type = %p",
            message, body_type);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MESSAGE_01_125: [ `message_get_body_type` shall fill in `body_type` the AMQP message body type. ]*/
        if (message->body_amqp_value != NULL)
        {
            *body_type = MESSAGE_BODY_TYPE_VALUE;
        }
        else if (message->body_amqp_data_count > 0)
        {
            *body_type = MESSAGE_BODY_TYPE_DATA;
        }
        else if (message->body_amqp_sequence_count > 0)
        {
            *body_type = MESSAGE_BODY_TYPE_SEQUENCE;
        }
        else
        {
            /* Codes_SRS_MESSAGE_01_128: [ If no body has been set on the message, `body_type` shall be `MESSAGE_BODY_TYPE_NONE`. ]*/
            *body_type = MESSAGE_BODY_TYPE_NONE;
        }

        /* Codes_SRS_MESSAGE_01_126: [ On success, `message_get_body_type` shall return 0. ]*/
        result = 0;
    }

    return result;
}

int message_set_message_format(MESSAGE_HANDLE message, uint32_t message_format)
{
    int result;

    if (message == NULL)
    {
        /* Codes_SRS_MESSAGE_01_131: [ If `message` is NULL, `message_set_message_format` shall fail and return a non-zero value. ]*/
        LogError("NULL message");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MESSAGE_01_129: [ `message_set_message_format` shall set the message format for the message identified by `message`. ]*/
        message->message_format = message_format;

        /* Codes_SRS_MESSAGE_01_130: [ On success, `message_set_message_format` shall return 0. ]*/
        result = 0;
    }

    return result;
}

int message_get_message_format(MESSAGE_HANDLE message, uint32_t *message_format)
{
    int result;

    if ((message == NULL) ||
        (message_format == NULL))
    {
        /* Codes_SRS_MESSAGE_01_134: [ If `message` or `message_format` is NULL, `message_get_message_format` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: message = %p, message_format = %p",
            message, message_format);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MESSAGE_01_132: [ `message_get_message_format` shall get the message format for the message identified by `message` and return it in the `message_fomrat` argument. ]*/
        *message_format = message->message_format;

        /* Codes_SRS_MESSAGE_01_133: [ On success, `message_get_message_format` shall return 0. ]*/
        result = 0;
    }

    return result;
}
