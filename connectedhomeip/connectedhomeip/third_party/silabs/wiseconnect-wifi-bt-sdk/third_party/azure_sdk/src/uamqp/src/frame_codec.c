// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_uamqp_c/frame_codec.h"
#include "azure_uamqp_c/amqpvalue.h"

#define FRAME_HEADER_SIZE 8
#define MAX_TYPE_SPECIFIC_SIZE    ((255 * 4) - 6)

typedef enum RECEIVE_FRAME_STATE_TAG
{
    RECEIVE_FRAME_STATE_FRAME_SIZE,
    RECEIVE_FRAME_STATE_DOFF,
    RECEIVE_FRAME_STATE_FRAME_TYPE,
    RECEIVE_FRAME_STATE_TYPE_SPECIFIC,
    RECEIVE_FRAME_STATE_FRAME_BODY,
    RECEIVE_FRAME_STATE_ERROR
} RECEIVE_FRAME_STATE;

typedef struct SUBSCRIPTION_TAG
{
    uint8_t frame_type;
    ON_FRAME_RECEIVED on_frame_received;
    void* callback_context;
} SUBSCRIPTION;

typedef struct FRAME_CODEC_INSTANCE_TAG
{
    /* subscriptions */
    SINGLYLINKEDLIST_HANDLE subscription_list;

    /* decode frame */
    RECEIVE_FRAME_STATE receive_frame_state;
    size_t receive_frame_pos;
    uint32_t receive_frame_size;
    uint32_t receive_frame_malloc_size;
    uint32_t type_specific_size;
    uint8_t receive_frame_doff;
    uint8_t receive_frame_type;
    SUBSCRIPTION* receive_frame_subscription;
    unsigned char* receive_frame_bytes;
    ON_FRAME_CODEC_ERROR on_frame_codec_error;
    void* on_frame_codec_error_callback_context;

    /* configuration */
    uint32_t max_frame_size;
} FRAME_CODEC_INSTANCE;

static bool find_subscription_by_frame_type(LIST_ITEM_HANDLE list_item, const void* match_context)
{
    bool result;
    SUBSCRIPTION* subscription = (SUBSCRIPTION*)singlylinkedlist_item_get_value(list_item);

    if (subscription == NULL)
    {
        LogError("Could not get subscription information from the list item");
        result = false;
    }
    else
    {
        result = subscription->frame_type == *((uint8_t*)match_context) ? true : false;
    }

    return result;
}

FRAME_CODEC_HANDLE frame_codec_create(ON_FRAME_CODEC_ERROR on_frame_codec_error, void* callback_context)
{
    FRAME_CODEC_INSTANCE* result;

    /* Codes_SRS_FRAME_CODEC_01_020: [If the on_frame_codec_error argument is NULL, frame_codec_create shall return NULL.] */
    /* Codes_SRS_FRAME_CODEC_01_104: [The callback_context shall be allowed to be NULL.] */
    if (on_frame_codec_error == NULL)
    {
        LogError("NULL on_frame_codec_error");
        result = NULL;
    }
    else
    {
        result = (FRAME_CODEC_INSTANCE*)calloc(1, sizeof(FRAME_CODEC_INSTANCE));
        /* Codes_SRS_FRAME_CODEC_01_022: [If allocating memory for the frame_codec instance fails, frame_codec_create shall return NULL.] */
        if (result == NULL)
        {
            LogError("Could not allocate frame codec");
        }
        else
        {
            /* Codes_SRS_FRAME_CODEC_01_021: [frame_codec_create shall create a new instance of frame_codec and return a non-NULL handle to it on success.] */
            result->receive_frame_state = RECEIVE_FRAME_STATE_FRAME_SIZE;
            result->on_frame_codec_error = on_frame_codec_error;
            result->on_frame_codec_error_callback_context = callback_context;
            result->receive_frame_pos = 0;
            result->receive_frame_size = 0;
            result->receive_frame_malloc_size = 0;
            result->receive_frame_bytes = NULL;
            result->subscription_list = singlylinkedlist_create();

            /* Codes_SRS_FRAME_CODEC_01_082: [The initial max_frame_size_shall be 512.] */
            result->max_frame_size = 512;
        }
    }

    return result;
}

void frame_codec_destroy(FRAME_CODEC_HANDLE frame_codec)
{
    /* Codes_SRS_FRAME_CODEC_01_024: [If frame_codec is NULL, frame_codec_destroy shall do nothing.] */
    if (frame_codec == NULL)
    {
        LogError("NULL frame_codec");
    }
    else
    {
        FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;

        singlylinkedlist_destroy(frame_codec_data->subscription_list);
        if (frame_codec_data->receive_frame_bytes != NULL)
        {
            free(frame_codec_data->receive_frame_bytes);
        }

        /* Codes_SRS_FRAME_CODEC_01_023: [frame_codec_destroy shall free all resources associated with a frame_codec instance.] */
        free(frame_codec);
    }
}

int frame_codec_set_max_frame_size(FRAME_CODEC_HANDLE frame_codec, uint32_t max_frame_size)
{
    int result;
    FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;

    /* Codes_SRS_FRAME_CODEC_01_077: [If frame_codec is NULL, frame_codec_set_max_frame_size shall return a non-zero value.] */
    if ((frame_codec == NULL) ||
        /* Codes_SRS_FRAME_CODEC_01_078: [If max_frame_size is invalid according to the AMQP standard, frame_codec_set_max_frame_size shall return a non-zero value.] */
        (max_frame_size < FRAME_HEADER_SIZE) ||
        /* Codes_SRS_FRAME_CODEC_01_081: [If a frame being decoded already has a size bigger than the max_frame_size argument then frame_codec_set_max_frame_size shall return a non-zero value and the previous frame size shall be kept.] */
        ((max_frame_size < frame_codec_data->receive_frame_size) && (frame_codec_data->receive_frame_state != RECEIVE_FRAME_STATE_FRAME_SIZE)))
    {
        LogError("Bad arguments: frame_codec = %p, max_frame_size = %" PRIu32,
            frame_codec,
            max_frame_size);
        result = MU_FAILURE;
    }
    /* Codes_SRS_FRAME_CODEC_01_097: [Setting a frame size on a frame_codec that had a decode error shall fail.] */
    else if (frame_codec_data->receive_frame_state == RECEIVE_FRAME_STATE_ERROR)
    {
        LogError("Frame codec in error state");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_FRAME_CODEC_01_075: [frame_codec_set_max_frame_size shall set the maximum frame size for a frame_codec.] */
        /* Codes_SRS_FRAME_CODEC_01_079: [The new frame size shall take effect immediately, even for a frame that is being decoded at the time of the call.] */
        frame_codec_data->max_frame_size = max_frame_size;

        /* Codes_SRS_FRAME_CODEC_01_076: [On success, frame_codec_set_max_frame_size shall return 0.] */
        result = 0;
    }

    return result;
}

/* Codes_SRS_FRAME_CODEC_01_001: [Frames are divided into three distinct areas: a fixed width frame header, a variable width extended header, and a variable width frame body.] */
/* Codes_SRS_FRAME_CODEC_01_002: [frame header The frame header is a fixed size (8 byte) structure that precedes each frame.] */
/* Codes_SRS_FRAME_CODEC_01_003: [The frame header includes mandatory information necessary to parse the rest of the frame including size and type information.] */
/* Codes_SRS_FRAME_CODEC_01_004: [extended header The extended header is a variable width area preceding the frame body.] */
/* Codes_SRS_FRAME_CODEC_01_007: [frame body The frame body is a variable width sequence of bytes the format of which depends on the frame type.] */
/* Codes_SRS_FRAME_CODEC_01_028: [The sequence of bytes shall be decoded according to the AMQP ISO.] */
/* Codes_SRS_FRAME_CODEC_01_029: [The sequence of bytes does not have to be a complete frame, frame_codec shall be responsible for maintaining decoding state between frame_codec_receive_bytes calls.] */
int frame_codec_receive_bytes(FRAME_CODEC_HANDLE frame_codec, const unsigned char* buffer, size_t size)
{
    int result = MU_FAILURE;
    FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;

    /* Codes_SRS_FRAME_CODEC_01_026: [If frame_codec or buffer are NULL, frame_codec_receive_bytes shall return a non-zero value.] */
    if ((frame_codec == NULL) ||
        (buffer == NULL) ||
        /* Codes_SRS_FRAME_CODEC_01_027: [If size is zero, frame_codec_receive_bytes shall return a non-zero value.] */
        (size == 0))
    {
        LogError("Bad arguments: frame_codec = %p, buffer = %p, size = %u",
            frame_codec,
            buffer,
            (unsigned int)size);
        result = MU_FAILURE;
    }
    else
    {
        while (size > 0)
        {
            switch (frame_codec_data->receive_frame_state)
            {
            default:
            case RECEIVE_FRAME_STATE_ERROR:
                /* Codes_SRS_FRAME_CODEC_01_074: [If a decoding error is detected, any subsequent calls on frame_codec_data_receive_bytes shall fail.] */
                LogError("Frame codec is in error state");
                result = MU_FAILURE;
                size = 0;
                break;

                /* Codes_SRS_FRAME_CODEC_01_008: [SIZE Bytes 0-3 of the frame header contain the frame size.] */
            case RECEIVE_FRAME_STATE_FRAME_SIZE:
                /* Codes_SRS_FRAME_CODEC_01_009: [This is an unsigned 32-bit integer that MUST contain the total frame size of the frame header, extended header, and frame body.] */
                frame_codec_data->receive_frame_size += buffer[0] << (24 - frame_codec_data->receive_frame_pos * 8);
                buffer++;
                size--;
                frame_codec_data->receive_frame_pos++;

                if (frame_codec_data->receive_frame_pos == 4)
                {
                    /* Codes_SRS_FRAME_CODEC_01_010: [The frame is malformed if the size is less than the size of the frame header (8 bytes).] */
                    if ((frame_codec_data->receive_frame_size < FRAME_HEADER_SIZE) ||
                        /* Codes_SRS_FRAME_CODEC_01_096: [If a frame bigger than the current max frame size is received, frame_codec_receive_bytes shall fail and return a non-zero value.] */
                        (frame_codec_data->receive_frame_size > frame_codec_data->max_frame_size))
                    {
                        /* Codes_SRS_FRAME_CODEC_01_074: [If a decoding error is detected, any subsequent calls on frame_codec_data_receive_bytes shall fail.] */
                        frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_ERROR;
                        /* Codes_SRS_FRAME_CODEC_01_103: [Upon any decode error, if an error callback has been passed to frame_codec_create, then the error callback shall be called with the context argument being the on_frame_codec_error_callback_context argument passed to frame_codec_create.] */
                        frame_codec_data->on_frame_codec_error(frame_codec_data->on_frame_codec_error_callback_context);
                        LogError("Received frame size is too big");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_DOFF;
                        result = 0;
                    }
                }
                else
                {
                    result = 0;
                }

                break;

            case RECEIVE_FRAME_STATE_DOFF:
                /* Codes_SRS_FRAME_CODEC_01_011: [DOFF Byte 4 of the frame header is the data offset.] */
                /* Codes_SRS_FRAME_CODEC_01_013: [The value of the data offset is an unsigned, 8-bit integer specifying a count of 4-byte words.] */
                /* Codes_SRS_FRAME_CODEC_01_012: [This gives the position of the body within the frame.] */
                frame_codec_data->receive_frame_doff = buffer[0];
                buffer++;
                size--;

                /* Codes_SRS_FRAME_CODEC_01_014: [Due to the mandatory 8-byte frame header, the frame is malformed if the value is less than 2.] */
                if (frame_codec_data->receive_frame_doff < 2)
                {
                    /* Codes_SRS_FRAME_CODEC_01_074: [If a decoding error is detected, any subsequent calls on frame_codec_data_receive_bytes shall fail.] */
                    frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_ERROR;

                    /* Codes_SRS_FRAME_CODEC_01_103: [Upon any decode error, if an error callback has been passed to frame_codec_create, then the error callback shall be called with the context argument being the on_frame_codec_error_callback_context argument passed to frame_codec_create.] */
                    frame_codec_data->on_frame_codec_error(frame_codec_data->on_frame_codec_error_callback_context);

                    LogError("Malformed frame received");
                    result = MU_FAILURE;
                }
                else
                {
                    frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_FRAME_TYPE;
                    result = 0;
                }

                break;

            case RECEIVE_FRAME_STATE_FRAME_TYPE:
            {
                LIST_ITEM_HANDLE item_handle;
                frame_codec_data->type_specific_size = (frame_codec_data->receive_frame_doff * 4) - 6;

                /* Codes_SRS_FRAME_CODEC_01_015: [TYPE Byte 5 of the frame header is a type code.] */
                frame_codec_data->receive_frame_type = buffer[0];
                buffer++;
                size--;

                /* Codes_SRS_FRAME_CODEC_01_035: [After successfully registering a callback for a certain frame type, when subsequently that frame type is received the callbacks shall be invoked, passing to it the received frame and the callback_context value.] */
                item_handle = singlylinkedlist_find(frame_codec_data->subscription_list, find_subscription_by_frame_type, &frame_codec_data->receive_frame_type);
                if (item_handle == NULL)
                {
                    frame_codec_data->receive_frame_subscription = NULL;
                    frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_TYPE_SPECIFIC;
                    result = 0;
                    break;
                }
                else
                {
                    frame_codec_data->receive_frame_subscription = (SUBSCRIPTION*)singlylinkedlist_item_get_value(item_handle);
                    if (frame_codec_data->receive_frame_subscription == NULL)
                    {
                        frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_TYPE_SPECIFIC;
                        result = 0;
                        break;
                    }
                    else
                    {
                        frame_codec_data->receive_frame_pos = 0;

                        /* Codes_SRS_FRAME_CODEC_01_102: [frame_codec_receive_bytes shall allocate memory to hold the frame_body bytes.] */
                        frame_codec_data->receive_frame_malloc_size = frame_codec_data->receive_frame_size - 6;
                        frame_codec_data->receive_frame_bytes = (unsigned char*)malloc(frame_codec_data->receive_frame_malloc_size);
                        if (frame_codec_data->receive_frame_bytes == NULL)
                        {
                            /* Codes_SRS_FRAME_CODEC_01_101: [If the memory for the frame_body bytes cannot be allocated, frame_codec_receive_bytes shall fail and return a non-zero value.] */
                            /* Codes_SRS_FRAME_CODEC_01_030: [If a decoding error occurs, frame_codec_data_receive_bytes shall return a non-zero value.] */
                            /* Codes_SRS_FRAME_CODEC_01_074: [If a decoding error is detected, any subsequent calls on frame_codec_data_receive_bytes shall fail.] */
                            frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_ERROR;

                            /* Codes_SRS_FRAME_CODEC_01_103: [Upon any decode error, if an error callback has been passed to frame_codec_create, then the error callback shall be called with the context argument being the on_frame_codec_error_callback_context argument passed to frame_codec_create.] */
                            frame_codec_data->on_frame_codec_error(frame_codec_data->on_frame_codec_error_callback_context);

                            LogError("Cannot allocate memory for frame bytes");
                            result = MU_FAILURE;
                            break;
                        }
                        else
                        {
                            frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_TYPE_SPECIFIC;
                            result = 0;
                            break;
                        }
                    }
                }
            }

            case RECEIVE_FRAME_STATE_TYPE_SPECIFIC:
            {
                size_t to_copy = frame_codec_data->type_specific_size - frame_codec_data->receive_frame_pos;
                if (to_copy > size)
                {
                    to_copy = size;
                }

                if (frame_codec_data->receive_frame_subscription != NULL)
                {
                    // [bug:8781089] WRITE address points to the zero page - SEGV
                    if (frame_codec_data->receive_frame_bytes == NULL)
                    {
                        LogError("Frame bytes memory was not allocated");
                        result = MU_FAILURE;
                        size = 0;
                        break;
                    }
                    else if (frame_codec_data->receive_frame_pos + to_copy > frame_codec_data->receive_frame_malloc_size)
                    {
                        result = MU_FAILURE;
                        size = 0;
                        break;
                    }                    
                    (void)memcpy(&frame_codec_data->receive_frame_bytes[frame_codec_data->receive_frame_pos], buffer, to_copy);
                    frame_codec_data->receive_frame_pos += to_copy;
                    buffer += to_copy;
                    size -= to_copy;
                }
                else
                {
                    frame_codec_data->receive_frame_pos += to_copy;
                    buffer += to_copy;
                    size -= to_copy;
                }

                if (frame_codec_data->receive_frame_pos == frame_codec_data->type_specific_size)
                {
                    if (frame_codec_data->receive_frame_size == FRAME_HEADER_SIZE)
                    {
                        if (frame_codec_data->receive_frame_subscription != NULL)
                        {
                            /* Codes_SRS_FRAME_CODEC_01_031: [When a complete frame is successfully decoded it shall be indicated to the upper layer by invoking the on_frame_received passed to frame_codec_subscribe.] */
                            /* Codes_SRS_FRAME_CODEC_01_032: [Besides passing the frame information, the callback_context value passed to frame_codec_data_subscribe shall be passed to the on_frame_received function.] */
                            /* Codes_SRS_FRAME_CODEC_01_005: [This is an extension point defined for future expansion.] */
                            /* Codes_SRS_FRAME_CODEC_01_006: [The treatment of this area depends on the frame type.] */
                            /* Codes_SRS_FRAME_CODEC_01_100: [If the frame body size is 0, the frame_body pointer passed to on_frame_received shall be NULL.] */
                            frame_codec_data->receive_frame_subscription->on_frame_received(frame_codec_data->receive_frame_subscription->callback_context, frame_codec_data->receive_frame_bytes, frame_codec_data->type_specific_size, NULL, 0);
                            free(frame_codec_data->receive_frame_bytes);
                            frame_codec_data->receive_frame_bytes = NULL;
                        }

                        frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_FRAME_SIZE;
                        frame_codec_data->receive_frame_size = 0;
                    }
                    else
                    {
                        frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_FRAME_BODY;
                    }

                    frame_codec_data->receive_frame_pos = 0;
                }

                result = 0;
                break;
            }

            case RECEIVE_FRAME_STATE_FRAME_BODY:
            {
                uint32_t frame_body_size = frame_codec_data->receive_frame_size - (frame_codec_data->receive_frame_doff * 4);
                size_t to_copy = frame_body_size - frame_codec_data->receive_frame_pos;

                if (to_copy > size)
                {
                    to_copy = size;
                }

                if (frame_codec_data->receive_frame_bytes == NULL)
                {
                    result = MU_FAILURE;
                    size = 0;
                    break;
                }

                (void)memcpy(frame_codec_data->receive_frame_bytes + frame_codec_data->receive_frame_pos + frame_codec_data->type_specific_size, buffer, to_copy);

                buffer += to_copy;
                size -= to_copy;
                frame_codec_data->receive_frame_pos += to_copy;

                if (frame_codec_data->receive_frame_pos == frame_body_size)
                {
                    if (frame_codec_data->receive_frame_subscription != NULL)
                    {
                        /* Codes_SRS_FRAME_CODEC_01_031: [When a complete frame is successfully decoded it shall be indicated to the upper layer by invoking the on_frame_received passed to frame_codec_subscribe.] */
                        /* Codes_SRS_FRAME_CODEC_01_032: [Besides passing the frame information, the callback_context value passed to frame_codec_data_subscribe shall be passed to the on_frame_received function.] */
                        /* Codes_SRS_FRAME_CODEC_01_005: [This is an extension point defined for future expansion.] */
                        /* Codes_SRS_FRAME_CODEC_01_006: [The treatment of this area depends on the frame type.] */
                        /* Codes_SRS_FRAME_CODEC_01_099: [A pointer to the frame_body bytes shall also be passed to the on_frame_received.] */
                        frame_codec_data->receive_frame_subscription->on_frame_received(frame_codec_data->receive_frame_subscription->callback_context, frame_codec_data->receive_frame_bytes, frame_codec_data->type_specific_size, frame_codec_data->receive_frame_bytes + frame_codec_data->type_specific_size, frame_body_size);
                        free(frame_codec_data->receive_frame_bytes);
                        frame_codec_data->receive_frame_bytes = NULL;
                    }

                    frame_codec_data->receive_frame_state = RECEIVE_FRAME_STATE_FRAME_SIZE;
                    frame_codec_data->receive_frame_pos = 0;
                    frame_codec_data->receive_frame_size = 0;
                }
                result = 0;

                break;
            }
            }
        }
    }

    return result;
}

/* Codes_SRS_FRAME_CODEC_01_033: [frame_codec_subscribe subscribes for a certain type of frame received by the frame_codec instance identified by frame_codec.] */
int frame_codec_subscribe(FRAME_CODEC_HANDLE frame_codec, uint8_t type, ON_FRAME_RECEIVED on_frame_received, void* callback_context)
{
    int result;

    /* Codes_SRS_FRAME_CODEC_01_034: [If any of the frame_codec or on_frame_received arguments is NULL, frame_codec_subscribe shall return a non-zero value.] */
    if ((frame_codec == NULL) ||
        (on_frame_received == NULL))
    {
        LogError("Bad arguments: frame_codec = %p, on_frame_received = %p",
            frame_codec, on_frame_received);
        result = MU_FAILURE;
    }
    else
    {
        FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;
        SUBSCRIPTION* subscription;

        /* Codes_SRS_FRAME_CODEC_01_036: [Only one callback pair shall be allowed to be registered for a given frame type.] */
        /* find the subscription for this frame type */
        LIST_ITEM_HANDLE list_item = singlylinkedlist_find(frame_codec_data->subscription_list, find_subscription_by_frame_type, &type);
        if (list_item != NULL)
        {
            subscription = (SUBSCRIPTION*)singlylinkedlist_item_get_value(list_item);
            if (subscription == NULL)
            {
                /* Codes_SRS_FRAME_CODEC_01_037: [If any failure occurs while performing the subscribe operation, frame_codec_subscribe shall return a non-zero value.] */
                LogError("Cannot retrieve subscription information from the list for type %u", (unsigned int)type);
                result = MU_FAILURE;
            }
            else
            {
                /* a subscription was found */
                subscription->on_frame_received = on_frame_received;
                subscription->callback_context = callback_context;

                /* Codes_SRS_FRAME_CODEC_01_087: [On success, frame_codec_subscribe shall return zero.] */
                result = 0;
            }
        }
        else
        {
            /* add a new subscription */
            subscription = (SUBSCRIPTION*)calloc(1, sizeof(SUBSCRIPTION));
            /* Codes_SRS_FRAME_CODEC_01_037: [If any failure occurs while performing the subscribe operation, frame_codec_subscribe shall return a non-zero value.] */
            if (subscription == NULL)
            {
                LogError("Cannot allocate memory for new subscription");
                result = MU_FAILURE;
            }
            else
            {
                subscription->on_frame_received = on_frame_received;
                subscription->callback_context = callback_context;
                subscription->frame_type = type;

                /* Codes_SRS_FRAME_CODEC_01_037: [If any failure occurs while performing the subscribe operation, frame_codec_subscribe shall return a non-zero value.] */
                if (singlylinkedlist_add(frame_codec_data->subscription_list, subscription) == NULL)
                {
                    free(subscription);
                    LogError("Cannot add subscription to list");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_FRAME_CODEC_01_087: [On success, frame_codec_subscribe shall return zero.] */
                    result = 0;
                }
            }
        }
    }

    return result;
}

int frame_codec_unsubscribe(FRAME_CODEC_HANDLE frame_codec, uint8_t type)
{
    int result;

    /* Codes_SRS_FRAME_CODEC_01_039: [If frame_codec is NULL, frame_codec_unsubscribe shall return a non-zero value.] */
    if (frame_codec == NULL)
    {
        LogError("NULL frame_codec");
        result = MU_FAILURE;
    }
    else
    {
        FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;
        LIST_ITEM_HANDLE list_item = singlylinkedlist_find(frame_codec_data->subscription_list, find_subscription_by_frame_type, &type);

        if (list_item == NULL)
        {
            /* Codes_SRS_FRAME_CODEC_01_040: [If no subscription for the type frame type exists, frame_codec_unsubscribe shall return a non-zero value.] */
            /* Codes_SRS_FRAME_CODEC_01_041: [If any failure occurs while performing the unsubscribe operation, frame_codec_unsubscribe shall return a non-zero value.] */
            LogError("Cannot find subscription for type %u", (unsigned int)type);
            result = MU_FAILURE;
        }
        else
        {
            SUBSCRIPTION* subscription = (SUBSCRIPTION*)singlylinkedlist_item_get_value(list_item);
            if (subscription == NULL)
            {
                /* Codes_SRS_FRAME_CODEC_01_041: [If any failure occurs while performing the unsubscribe operation, frame_codec_unsubscribe shall return a non-zero value.] */
                LogError("singlylinkedlist_item_get_value failed when unsubscribing");
                result = MU_FAILURE;
            }
            else
            {
                free(subscription);
                if (singlylinkedlist_remove(frame_codec_data->subscription_list, list_item) != 0)
                {
                    /* Codes_SRS_FRAME_CODEC_01_041: [If any failure occurs while performing the unsubscribe operation, frame_codec_unsubscribe shall return a non-zero value.] */
                    LogError("Cannot remove subscription from list");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_FRAME_CODEC_01_038: [frame_codec_unsubscribe removes a previous subscription for frames of type type and on success it shall return 0.] */
                    result = 0;
                }
            }
        }
    }

    return result;
}

int frame_codec_encode_frame(FRAME_CODEC_HANDLE frame_codec, uint8_t type, const PAYLOAD* payloads, size_t payload_count, const unsigned char* type_specific_bytes, uint32_t type_specific_size, ON_BYTES_ENCODED on_bytes_encoded, void* callback_context)
{
    int result;

    FRAME_CODEC_INSTANCE* frame_codec_data = (FRAME_CODEC_INSTANCE*)frame_codec;

    /* Codes_SRS_FRAME_CODEC_01_044: [If any of arguments `frame_codec` or `on_bytes_encoded` is NULL, `frame_codec_encode_frame` shall return a non-zero value.] */
    if ((frame_codec == NULL) ||
        (on_bytes_encoded == NULL) ||
        /* Codes_SRS_FRAME_CODEC_01_091: [If the argument type_specific_size is greater than 0 and type_specific_bytes is NULL, frame_codec_encode_frame shall return a non-zero value.] */
        ((type_specific_size > 0) && (type_specific_bytes == NULL)) ||
        /* Codes_SRS_FRAME_CODEC_01_092: [If type_specific_size is too big to allow encoding the frame according to the AMQP ISO then frame_codec_encode_frame shall return a non-zero value.] */
        (type_specific_size > MAX_TYPE_SPECIFIC_SIZE))
    {
        LogError("Bad arguments: frame_codec = %p, on_bytes_encoded = %p, type_specific_size = %u, type_specific_bytes = %p",
            frame_codec, on_bytes_encoded, (unsigned int)type_specific_size, type_specific_bytes);
        result = MU_FAILURE;
    }
    else if ((payloads == NULL) && (payload_count > 0))
    {
        /* Codes_SRS_FRAME_CODEC_01_107: [If the argument `payloads` is NULL and `payload_count` is non-zero, `frame_codec_encode_frame` shall return a non-zero value.]*/
        LogError("NULL payloads argument with non-zero payload count");
        result = MU_FAILURE;
    }
    else
    {
        /* round up to the 4 bytes for doff */
        /* Codes_SRS_FRAME_CODEC_01_067: [The value of the data offset is an unsigned, 8-bit integer specifying a count of 4-byte words.] */
        /* Codes_SRS_FRAME_CODEC_01_068: [Due to the mandatory 8-byte frame header, the frame is malformed if the value is less than 2.] */
        uint8_t padding_byte_count;
        uint32_t frame_body_offset = type_specific_size + 6;
        uint8_t doff = (uint8_t)((frame_body_offset + 3) / 4);
        size_t i;
        size_t frame_size;
        size_t frame_body_size = 0;
        frame_body_offset = doff * 4;
        padding_byte_count = (uint8_t)(frame_body_offset - type_specific_size - 6);

        for (i = 0; i < payload_count; i++)
        {
            /* Codes_SRS_FRAME_CODEC_01_110: [ If the `bytes` member of a payload entry is NULL, `frame_codec_encode_frame` shall fail and return a non-zero value. ] */
            if ((payloads[i].bytes == NULL) ||
                /* Codes_SRS_FRAME_CODEC_01_111: [ If the `length` member of a payload entry is 0, `frame_codec_encode_frame` shall fail and return a non-zero value. ] */
                (payloads[i].length == 0))
            {
                break;
            }

            frame_body_size += payloads[i].length;
        }

        if (i < payload_count)
        {
            LogError("Bad payload entry");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_FRAME_CODEC_01_063: [This is an unsigned 32-bit integer that MUST contain the total frame size of the frame header, extended header, and frame body.] */
            frame_size = frame_body_size + frame_body_offset;

            if (frame_size > frame_codec_data->max_frame_size)
            {
                /* Codes_SRS_FRAME_CODEC_01_095: [If the frame_size needed for the frame is bigger than the maximum frame size, frame_codec_encode_frame shall fail and return a non-zero value.] */
                LogError("Encoded frame size exceeds the maximum allowed frame size");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_FRAME_CODEC_01_108: [ Memory shall be allocated to hold the entire frame. ]*/
                unsigned char* encoded_frame = (unsigned char*)malloc(frame_size);
                if (encoded_frame == NULL)
                {
                    /* Codes_SRS_FRAME_CODEC_01_109: [ If allocating memory fails, `frame_codec_encode_frame` shall fail and return a non-zero value. ]*/
                    LogError("Cannot allocate memory for frame");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_FRAME_CODEC_01_042: [frame_codec_encode_frame encodes the header, type specific bytes and frame payload of a frame that has frame_payload_size bytes.]*/
                    /* Codes_SRS_FRAME_CODEC_01_055: [Frames are divided into three distinct areas: a fixed width frame header, a variable width extended header, and a variable width frame body.] */
                    /* Codes_SRS_FRAME_CODEC_01_056: [frame header The frame header is a fixed size (8 byte) structure that precedes each frame.] */
                    /* Codes_SRS_FRAME_CODEC_01_057: [The frame header includes mandatory information necessary to parse the rest of the frame including size and type information.] */
                    /* Codes_SRS_FRAME_CODEC_01_058: [extended header The extended header is a variable width area preceding the frame body.] */
                    /* Codes_SRS_FRAME_CODEC_01_059: [This is an extension point defined for future expansion.] */
                    /* Codes_SRS_FRAME_CODEC_01_060: [The treatment of this area depends on the frame type.]*/
                    /* Codes_SRS_FRAME_CODEC_01_062: [SIZE Bytes 0-3 of the frame header contain the frame size.] */
                    /* Codes_SRS_FRAME_CODEC_01_063: [This is an unsigned 32-bit integer that MUST contain the total frame size of the frame header, extended header, and frame body.] */
                    /* Codes_SRS_FRAME_CODEC_01_064: [The frame is malformed if the size is less than the size of the frame header (8 bytes).] */
                    unsigned char frame_header[6];
                    size_t current_pos = 0;
                    /* Codes_SRS_FRAME_CODEC_01_090: [If the type_specific_size - 2 does not divide by 4, frame_codec_encode_frame shall pad the type_specific bytes with zeroes so that type specific data is according to the AMQP ISO.] */
                    unsigned char padding_bytes[] = { 0x00, 0x00, 0x00 };

                    frame_header[0] = (frame_size >> 24) & 0xFF;
                    frame_header[1] = (frame_size >> 16) & 0xFF;
                    frame_header[2] = (frame_size >> 8) & 0xFF;
                    frame_header[3] = frame_size & 0xFF;
                    /* Codes_SRS_FRAME_CODEC_01_065: [DOFF Byte 4 of the frame header is the data offset.] */
                    frame_header[4] = doff;
                    /* Codes_SRS_FRAME_CODEC_01_069: [TYPE Byte 5 of the frame header is a type code.] */
                    frame_header[5] = type;

                    (void)memcpy(encoded_frame, frame_header, sizeof(frame_header));
                    current_pos += sizeof(frame_header);

                    if (type_specific_size > 0)
                    {
                        (void)memcpy(encoded_frame + current_pos, type_specific_bytes, type_specific_size);
                        current_pos += type_specific_size;
                    }

                    /* send padding bytes */
                    if (padding_byte_count > 0)
                    {
                        (void)memcpy(encoded_frame + current_pos, padding_bytes, padding_byte_count);
                        current_pos += padding_byte_count;
                    }

                    /* Codes_SRS_FRAME_CODEC_01_106: [All payloads shall be encoded in order as part of the frame.] */
                    for (i = 0; i < payload_count; i++)
                    {
                        (void)memcpy(encoded_frame + current_pos, payloads[i].bytes, payloads[i].length);
                        current_pos += payloads[i].length;
                    }

                    /* Codes_SRS_FRAME_CODEC_01_088: [Encoded bytes shall be passed to the `on_bytes_encoded` callback in a single call, while setting the `encode complete` argument to true.] */
                    on_bytes_encoded(callback_context, encoded_frame, frame_size, true);

                    free(encoded_frame);

                    /* Codes_SRS_FRAME_CODEC_01_043: [On success it shall return 0.] */
                    result = 0;
                }
            }
        }
    }

    return result;
}
