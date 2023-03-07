// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/sasl_frame_codec.h"
#include "azure_uamqp_c/frame_codec.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/amqp_definitions.h"

/* Requirements implemented by design or by other modules */
/* Codes_SRS_SASL_FRAME_CODEC_01_011: [A SASL frame has a type code of 0x01.] */
/* Codes_SRS_SASL_FRAME_CODEC_01_016: [The maximum size of a SASL frame is defined by MIN-MAX-FRAME-SIZE.] */

#define MIX_MAX_FRAME_SIZE 512

typedef enum SASL_FRAME_DECODE_STATE_TAG
{
    SASL_FRAME_DECODE_FRAME,
    SASL_FRAME_DECODE_ERROR
} SASL_FRAME_DECODE_STATE;

typedef struct SASL_FRAME_CODEC_INSTANCE_TAG
{
    FRAME_CODEC_HANDLE frame_codec;

    /* decode */
    ON_SASL_FRAME_RECEIVED on_sasl_frame_received;
    ON_SASL_FRAME_CODEC_ERROR on_sasl_frame_codec_error;
    void* callback_context;
    AMQPVALUE_DECODER_HANDLE decoder;
    SASL_FRAME_DECODE_STATE decode_state;
    AMQP_VALUE decoded_sasl_frame_value;
} SASL_FRAME_CODEC_INSTANCE;

static void amqp_value_decoded(void* context, AMQP_VALUE decoded_value)
{
    SASL_FRAME_CODEC_INSTANCE* sasl_frame_codec_instance = (SASL_FRAME_CODEC_INSTANCE*)context;
    AMQP_VALUE descriptor = amqpvalue_get_inplace_descriptor(decoded_value);

    if (descriptor == NULL)
    {
        LogError("Cannot get frame descriptor");
        sasl_frame_codec_instance->decode_state = SASL_FRAME_DECODE_ERROR;

        /* Codes_SRS_SASL_FRAME_CODEC_01_049: [If any error occurs while decoding a frame, the decoder shall call the on_sasl_frame_codec_error and pass to it the callback_context, both of those being the ones given to sasl_frame_codec_create.] */
        sasl_frame_codec_instance->on_sasl_frame_codec_error(sasl_frame_codec_instance->callback_context);
    }
    else
    {
        /* Codes_SRS_SASL_FRAME_CODEC_01_009: [The frame body of a SASL frame MUST contain exactly one AMQP type, whose type encoding MUST have provides="sasl-frame".] */
        if (!is_sasl_mechanisms_type_by_descriptor(descriptor) &&
            !is_sasl_init_type_by_descriptor(descriptor) &&
            !is_sasl_challenge_type_by_descriptor(descriptor) &&
            !is_sasl_response_type_by_descriptor(descriptor) &&
            !is_sasl_outcome_type_by_descriptor(descriptor))
        {
            LogError("Not a SASL frame");
            sasl_frame_codec_instance->decode_state = SASL_FRAME_DECODE_ERROR;

            /* Codes_SRS_SASL_FRAME_CODEC_01_049: [If any error occurs while decoding a frame, the decoder shall call the on_sasl_frame_codec_error and pass to it the callback_context, both of those being the ones given to sasl_frame_codec_create.] */
            sasl_frame_codec_instance->on_sasl_frame_codec_error(sasl_frame_codec_instance->callback_context);
        }
        else
        {
            sasl_frame_codec_instance->decoded_sasl_frame_value = decoded_value;
        }
    }
}

static void frame_received(void* context, const unsigned char* type_specific, uint32_t type_specific_size, const unsigned char* frame_body, uint32_t frame_body_size)
{
    SASL_FRAME_CODEC_INSTANCE* sasl_frame_codec_instance = (SASL_FRAME_CODEC_INSTANCE*)context;

    /* Codes_SRS_SASL_FRAME_CODEC_01_006: [Bytes 6 and 7 of the header are ignored.] */
    (void)type_specific;
    /* Codes_SRS_SASL_FRAME_CODEC_01_007: [The extended header is ignored.] */

    /* Codes_SRS_SASL_FRAME_CODEC_01_008: [The maximum size of a SASL frame is defined by MIN-MAX-FRAME-SIZE.] */
    if ((type_specific_size + frame_body_size + 6 > MIX_MAX_FRAME_SIZE) ||
        /* Codes_SRS_SASL_FRAME_CODEC_01_010: [Receipt of an empty frame is an irrecoverable error.] */
        (frame_body_size == 0))
    {
        LogError("Bad SASL frame size");

        /* Codes_SRS_SASL_FRAME_CODEC_01_049: [If any error occurs while decoding a frame, the decoder shall call the on_sasl_frame_codec_error and pass to it the callback_context, both of those being the ones given to sasl_frame_codec_create.] */
        sasl_frame_codec_instance->on_sasl_frame_codec_error(sasl_frame_codec_instance->callback_context);
    }
    else
    {
        switch (sasl_frame_codec_instance->decode_state)
        {
        default:
        case SASL_FRAME_DECODE_ERROR:
            break;

        case SASL_FRAME_DECODE_FRAME:
            sasl_frame_codec_instance->decoded_sasl_frame_value = NULL;

            /* Codes_SRS_SASL_FRAME_CODEC_01_039: [sasl_frame_codec shall decode the sasl-frame value as a described type.] */
            /* Codes_SRS_SASL_FRAME_CODEC_01_048: [Receipt of an empty frame is an irrecoverable error.] */
            while ((frame_body_size > 0) &&
                (sasl_frame_codec_instance->decoded_sasl_frame_value == NULL) &&
                (sasl_frame_codec_instance->decode_state != SASL_FRAME_DECODE_ERROR))
            {
                /* Codes_SRS_SASL_FRAME_CODEC_01_040: [Decoding the sasl-frame type shall be done by feeding the bytes to the decoder create in sasl_frame_codec_create.] */
                if (amqpvalue_decode_bytes(sasl_frame_codec_instance->decoder, frame_body, 1) != 0)
                {
                    LogError("Could not decode SASL frame AMQP value");
                    sasl_frame_codec_instance->decode_state = SASL_FRAME_DECODE_ERROR;
                }
                else
                {
                    frame_body_size--;
                    frame_body++;
                }
            }

            /* Codes_SRS_SASL_FRAME_CODEC_01_009: [The frame body of a SASL frame MUST contain exactly one AMQP type, whose type encoding MUST have provides="sasl-frame".] */
            if (frame_body_size > 0)
            {
                LogError("More than one AMQP value detected in SASL frame");
                sasl_frame_codec_instance->decode_state = SASL_FRAME_DECODE_ERROR;

                /* Codes_SRS_SASL_FRAME_CODEC_01_049: [If any error occurs while decoding a frame, the decoder shall call the on_sasl_frame_codec_error and pass to it the callback_context, both of those being the ones given to sasl_frame_codec_create.] */
                sasl_frame_codec_instance->on_sasl_frame_codec_error(sasl_frame_codec_instance->callback_context);
            }

            if (sasl_frame_codec_instance->decode_state != SASL_FRAME_DECODE_ERROR)
            {
                /* Codes_SRS_SASL_FRAME_CODEC_01_041: [Once the sasl frame is decoded, the callback on_sasl_frame_received shall be called.] */
                /* Codes_SRS_SASL_FRAME_CODEC_01_042: [The decoded sasl-frame value and the context passed in sasl_frame_codec_create shall be passed to on_sasl_frame_received.] */
                sasl_frame_codec_instance->on_sasl_frame_received(sasl_frame_codec_instance->callback_context, sasl_frame_codec_instance->decoded_sasl_frame_value);
            }
            break;
        }
    }
}

static int encode_bytes(void* context, const unsigned char* bytes, size_t length)
{
    PAYLOAD* payload = (PAYLOAD*)context;
    (void)memcpy((unsigned char*)payload->bytes + payload->length, bytes, length);
    payload->length += length;
    return 0;
}

SASL_FRAME_CODEC_HANDLE sasl_frame_codec_create(FRAME_CODEC_HANDLE frame_codec, ON_SASL_FRAME_RECEIVED on_sasl_frame_received, ON_SASL_FRAME_CODEC_ERROR on_sasl_frame_codec_error, void* callback_context)
{
    SASL_FRAME_CODEC_INSTANCE* result;

    /* Codes_SRS_SASL_FRAME_CODEC_01_019: [If any of the arguments frame_codec, on_sasl_frame_received or on_sasl_frame_codec_error is NULL, sasl_frame_codec_create shall return NULL.] */
    if ((frame_codec == NULL) ||
        (on_sasl_frame_received == NULL) ||
        (on_sasl_frame_codec_error == NULL))
    {
        LogError("Bad arguments: frame_codec = %p, on_sasl_frame_received = %p, on_sasl_frame_codec_error = %p",
            frame_codec, on_sasl_frame_received, on_sasl_frame_codec_error);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SASL_FRAME_CODEC_01_018: [sasl_frame_codec_create shall create an instance of an sasl_frame_codec and return a non-NULL handle to it.] */
        result = (SASL_FRAME_CODEC_INSTANCE*)calloc(1, sizeof(SASL_FRAME_CODEC_INSTANCE));
        if (result == NULL)
        {
            LogError("Cannot allocate memory for SASL frame codec");
        }
        else
        {
            result->frame_codec = frame_codec;
            result->on_sasl_frame_received = on_sasl_frame_received;
            result->on_sasl_frame_codec_error = on_sasl_frame_codec_error;
            result->callback_context = callback_context;
            result->decode_state = SASL_FRAME_DECODE_FRAME;

            /* Codes_SRS_SASL_FRAME_CODEC_01_022: [sasl_frame_codec_create shall create a decoder to be used for decoding SASL values.] */
            result->decoder = amqpvalue_decoder_create(amqp_value_decoded, result);
            if (result->decoder == NULL)
            {
                /* Codes_SRS_SASL_FRAME_CODEC_01_023: [If creating the decoder fails, sasl_frame_codec_create shall fail and return NULL.] */
                LogError("Cannot create AMQP value decoder");
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_SASL_FRAME_CODEC_01_020: [sasl_frame_codec_create shall subscribe for SASL frames with the given frame_codec.] */
                /* Codes_SRS_SASL_FRAME_CODEC_01_001: [A SASL frame has a type code of 0x01.] */
                if (frame_codec_subscribe(frame_codec, FRAME_TYPE_SASL, frame_received, result) != 0)
                {
                    /* Codes_SRS_SASL_FRAME_CODEC_01_021: [If subscribing for SASL frames fails, sasl_frame_codec_create shall fail and return NULL.] */
                    LogError("Cannot subscribe for SASL frames");
                    amqpvalue_decoder_destroy(result->decoder);
                    free(result);
                    result = NULL;
                }
            }
        }
    }

    return result;
}

void sasl_frame_codec_destroy(SASL_FRAME_CODEC_HANDLE sasl_frame_codec)
{
    /* Codes_SRS_SASL_FRAME_CODEC_01_026: [If sasl_frame_codec is NULL, sasl_frame_codec_destroy shall do nothing.] */
    if (sasl_frame_codec == NULL)
    {
        LogError("NULL sasl_frame_codec");
    }
    else
    {
        /* Codes_SRS_SASL_FRAME_CODEC_01_025: [sasl_frame_codec_destroy shall free all resources associated with the sasl_frame_codec instance.] */
        SASL_FRAME_CODEC_INSTANCE* sasl_frame_codec_instance = (SASL_FRAME_CODEC_INSTANCE*)sasl_frame_codec;

        /* Codes_SRS_SASL_FRAME_CODEC_01_027: [sasl_frame_codec_destroy shall unsubscribe from receiving SASL frames from the frame_codec that was passed to sasl_frame_codec_create.] */
        (void)frame_codec_unsubscribe(sasl_frame_codec_instance->frame_codec, FRAME_TYPE_SASL);

        /* Codes_SRS_SASL_FRAME_CODEC_01_028: [The decoder created in sasl_frame_codec_create shall be destroyed by sasl_frame_codec_destroy.] */
        amqpvalue_decoder_destroy(sasl_frame_codec_instance->decoder);
        free(sasl_frame_codec_instance);
    }
}

/* Codes_SRS_SASL_FRAME_CODEC_01_029: [sasl_frame_codec_encode_frame shall encode the frame header and sasl_frame_value AMQP value in a SASL frame and on success it shall return 0.] */
int sasl_frame_codec_encode_frame(SASL_FRAME_CODEC_HANDLE sasl_frame_codec, AMQP_VALUE sasl_frame_value, ON_BYTES_ENCODED on_bytes_encoded, void* callback_context)
{
    int result;
    SASL_FRAME_CODEC_INSTANCE* sasl_frame_codec_instance = (SASL_FRAME_CODEC_INSTANCE*)sasl_frame_codec;

    /* Codes_SRS_SASL_FRAME_CODEC_01_030: [If sasl_frame_codec or sasl_frame_value is NULL, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
    if ((sasl_frame_codec == NULL) ||
        (sasl_frame_value == NULL))
    {
        /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
        LogError("Bad arguments: sasl_frame_codec = %p, sasl_frame_value = %p",
            sasl_frame_codec, sasl_frame_value);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE descriptor;
        uint64_t sasl_frame_descriptor_ulong;
        size_t encoded_size;

        if ((descriptor = amqpvalue_get_inplace_descriptor(sasl_frame_value)) == NULL)
        {
            /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Cannot get SASL frame descriptor AMQP value");
            result = MU_FAILURE;
        }
        else if (amqpvalue_get_ulong(descriptor, &sasl_frame_descriptor_ulong) != 0)
        {
            /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Cannot get SASL frame descriptor ulong");
            result = MU_FAILURE;
        }
        /* Codes_SRS_SASL_FRAME_CODEC_01_047: [The frame body of a SASL frame MUST contain exactly one AMQP type, whose type encoding MUST have provides="sasl-frame".] */
        else if ((sasl_frame_descriptor_ulong < SASL_MECHANISMS) ||
            (sasl_frame_descriptor_ulong > SASL_OUTCOME))
        {
            /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Bad SASL frame descriptor");
            result = MU_FAILURE;
        }
        /* Codes_SRS_SASL_FRAME_CODEC_01_032: [The payload frame size shall be computed based on the encoded size of the sasl_frame_value and its fields.] */
        /* Codes_SRS_SASL_FRAME_CODEC_01_033: [The encoded size of the sasl_frame_value and its fields shall be obtained by calling amqpvalue_get_encoded_size.] */
        else if (amqpvalue_get_encoded_size(sasl_frame_value, &encoded_size) != 0)
        {
            /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Cannot get SASL frame encoded size");
            result = MU_FAILURE;
        }
            /* Codes_SRS_SASL_FRAME_CODEC_01_016: [The maximum size of a SASL frame is defined by MIN-MAX-FRAME-SIZE.] */
        else if (encoded_size > MIX_MAX_FRAME_SIZE - 8)
        {
            /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("SASL frame encoded size too big");
            result = MU_FAILURE;
        }
        else
        {
            unsigned char* sasl_frame_bytes = (unsigned char*)malloc(encoded_size);
            if (sasl_frame_bytes == NULL)
            {
                LogError("Cannot allocate SASL frame bytes");
                result = MU_FAILURE;
            }
            else
            {
                PAYLOAD payload;

                payload.bytes = sasl_frame_bytes;
                payload.length = 0;

                if (amqpvalue_encode(sasl_frame_value, encode_bytes, &payload) != 0)
                {
                    LogError("Cannot encode SASL frame value");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_SASL_FRAME_CODEC_01_031: [sasl_frame_codec_encode_frame shall encode the frame header and its contents by using frame_codec_encode_frame.] */
                    /* Codes_SRS_SASL_FRAME_CODEC_01_012: [Bytes 6 and 7 of the header are ignored.] */
                    /* Codes_SRS_SASL_FRAME_CODEC_01_013: [Implementations SHOULD set these to 0x00.] */
                    /* Codes_SRS_SASL_FRAME_CODEC_01_014: [The extended header is ignored.] */
                    /* Codes_SRS_SASL_FRAME_CODEC_01_015: [Implementations SHOULD therefore set DOFF to 0x02.] */
                    if (frame_codec_encode_frame(sasl_frame_codec_instance->frame_codec, FRAME_TYPE_SASL, &payload, 1, NULL, 0, on_bytes_encoded, callback_context) != 0)
                    {
                        /* Codes_SRS_SASL_FRAME_CODEC_01_034: [If any error occurs during encoding, sasl_frame_codec_encode_frame shall fail and return a non-zero value.] */
                        LogError("Cannot encode SASL frame");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }

                free(sasl_frame_bytes);
            }
        }
    }

    return result;
}
