// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/amqp_frame_codec.h"
#include "azure_uamqp_c/frame_codec.h"
#include "azure_uamqp_c/amqpvalue.h"

typedef enum AMQP_FRAME_DECODE_STATE_TAG
{
    AMQP_FRAME_DECODE_FRAME,
    AMQP_FRAME_DECODE_ERROR
} AMQP_FRAME_DECODE_STATE;

typedef struct AMQP_FRAME_CODEC_TAG
{
    FRAME_CODEC_HANDLE frame_codec;

    /* decode */
    AMQP_FRAME_RECEIVED_CALLBACK frame_received_callback;
    AMQP_EMPTY_FRAME_RECEIVED_CALLBACK empty_frame_received_callback;
    AMQP_FRAME_CODEC_ERROR_CALLBACK error_callback;
    void* callback_context;
    AMQPVALUE_DECODER_HANDLE decoder;
    AMQP_FRAME_DECODE_STATE decode_state;
    AMQP_VALUE decoded_performative;
} AMQP_FRAME_CODEC;

static void amqp_value_decoded(void* context, AMQP_VALUE decoded_value)
{
    AMQP_FRAME_CODEC_HANDLE amqp_frame_codec = (AMQP_FRAME_CODEC_HANDLE)context;
    uint64_t performative_descriptor_ulong;
    AMQP_VALUE descriptor = amqpvalue_get_inplace_descriptor(decoded_value);

    /* Codes_SRS_AMQP_FRAME_CODEC_01_060: [If any error occurs while decoding a frame, the decoder shall switch to an error state where decoding shall not be possible anymore.] */
    if ((descriptor == NULL) ||
        (amqpvalue_get_ulong(descriptor, &performative_descriptor_ulong) != 0) ||
        /* Codes_SRS_AMQP_FRAME_CODEC_01_003: [The performative MUST be one of those defined in section 2.7 and is encoded as a described type in the AMQP type system.] */
        (performative_descriptor_ulong < AMQP_OPEN) ||
        (performative_descriptor_ulong > AMQP_CLOSE))
    {
        /* Codes_SRS_AMQP_FRAME_CODEC_01_060: [If any error occurs while decoding a frame, the decoder shall switch to an error state where decoding shall not be possible anymore.] */
        amqp_frame_codec->decode_state = AMQP_FRAME_DECODE_ERROR;
    }
    else
    {
        amqp_frame_codec->decoded_performative = decoded_value;
    }
}

static void frame_received(void* context, const unsigned char* type_specific, uint32_t type_specific_size, const unsigned char* frame_body, uint32_t frame_body_size)
{
    AMQP_FRAME_CODEC_HANDLE amqp_frame_codec = (AMQP_FRAME_CODEC_HANDLE)context;
    uint16_t channel;

    switch (amqp_frame_codec->decode_state)
    {
    default:
    /* Codes_SRS_AMQP_FRAME_CODEC_01_050: [All subsequent decoding shall fail and no AMQP frames shall be indicated from that point on to the consumers of amqp_frame_codec.] */
    case AMQP_FRAME_DECODE_ERROR:
        break;

    case AMQP_FRAME_DECODE_FRAME:
        /* Codes_SRS_AMQP_FRAME_CODEC_01_049: [If not enough type specific bytes are received to decode the channel number, the decoding shall stop with an error.] */
        if (type_specific_size < 2)
        {
            amqp_frame_codec->decode_state = AMQP_FRAME_DECODE_ERROR;

            /* Codes_SRS_AMQP_FRAME_CODEC_01_069: [If any error occurs while decoding a frame, the decoder shall indicate the error by calling the amqp_frame_codec_error_callback  and passing to it the callback context argument that was given in amqp_frame_codec_create.] */
            amqp_frame_codec->error_callback(amqp_frame_codec->callback_context);
        }
        else
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_001: [Bytes 6 and 7 of an AMQP frame contain the channel number ] */
            channel = ((uint16_t)type_specific[0]) << 8;
            channel += type_specific[1];

            if (frame_body_size == 0)
            {
                /* Codes_SRS_AMQP_FRAME_CODEC_01_048: [When a frame header is received from frame_codec and the frame payload size is 0, empty_frame_received_callback shall be invoked, while passing the channel number as argument.] */
                /* Codes_SRS_AMQP_FRAME_CODEC_01_007: [An AMQP frame with no body MAY be used to generate artificial traffic as needed to satisfy any negotiated idle timeout interval ] */
                amqp_frame_codec->empty_frame_received_callback(amqp_frame_codec->callback_context, channel);
            }
            else
            {
                /* Codes_SRS_AMQP_FRAME_CODEC_01_051: [If the frame payload is greater than 0, amqp_frame_codec shall decode the performative as a described AMQP type.] */
                /* Codes_SRS_AMQP_FRAME_CODEC_01_002: [The frame body is defined as a performative followed by an opaque payload.] */
                amqp_frame_codec->decoded_performative = NULL;

                while ((frame_body_size > 0) &&
                       (amqp_frame_codec->decoded_performative == NULL) &&
                       (amqp_frame_codec->decode_state != AMQP_FRAME_DECODE_ERROR))
                {
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_052: [Decoding the performative shall be done by feeding the bytes to the decoder create in amqp_frame_codec_create.] */
                    if (amqpvalue_decode_bytes(amqp_frame_codec->decoder, frame_body, 1) != 0)
                    {
                        /* Codes_SRS_AMQP_FRAME_CODEC_01_060: [If any error occurs while decoding a frame, the decoder shall switch to an error state where decoding shall not be possible anymore.] */
                        amqp_frame_codec->decode_state = AMQP_FRAME_DECODE_ERROR;
                    }
                    else
                    {
                        frame_body_size--;
                        frame_body++;
                    }
                }

                if (amqp_frame_codec->decode_state == AMQP_FRAME_DECODE_ERROR)
                {
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_069: [If any error occurs while decoding a frame, the decoder shall indicate the error by calling the amqp_frame_codec_error_callback  and passing to it the callback context argument that was given in amqp_frame_codec_create.] */
                    amqp_frame_codec->error_callback(amqp_frame_codec->callback_context);
                }
                else
                {
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_004: [The remaining bytes in the frame body form the payload for that frame.] */
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_067: [When the performative is decoded, the rest of the frame_bytes shall not be given to the AMQP decoder, but they shall be buffered so that later they are given to the frame_received callback.] */
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_054: [Once the performative is decoded and all frame payload bytes are received, the callback frame_received_callback shall be called.] */
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_068: [A pointer to all the payload bytes shall also be passed to frame_received_callback.] */
                    amqp_frame_codec->frame_received_callback(amqp_frame_codec->callback_context, channel, amqp_frame_codec->decoded_performative, frame_body, frame_body_size);
                }
            }
        }
        break;
    }
}

static int encode_bytes(void* context, const unsigned char* bytes, size_t length)
{
    PAYLOAD* payload = (PAYLOAD*)context;
    (void)memcpy((unsigned char*)payload->bytes + payload->length, bytes, length);
    payload->length += length;
    return 0;
}

/* Codes_SRS_AMQP_FRAME_CODEC_01_011: [amqp_frame_codec_create shall create an instance of an amqp_frame_codec and return a non-NULL handle to it.] */
AMQP_FRAME_CODEC_HANDLE amqp_frame_codec_create(FRAME_CODEC_HANDLE frame_codec, AMQP_FRAME_RECEIVED_CALLBACK frame_received_callback,
    AMQP_EMPTY_FRAME_RECEIVED_CALLBACK empty_frame_received_callback, AMQP_FRAME_CODEC_ERROR_CALLBACK amqp_frame_codec_error_callback, void* callback_context)
{
    AMQP_FRAME_CODEC_HANDLE result;

    /* Codes_SRS_AMQP_FRAME_CODEC_01_012: [If any of the arguments frame_codec, frame_received_callback, amqp_frame_codec_error_callback or empty_frame_received_callback is NULL, amqp_frame_codec_create shall return NULL.] */
    if ((frame_codec == NULL) ||
        (frame_received_callback == NULL) ||
        (empty_frame_received_callback == NULL) ||
        (amqp_frame_codec_error_callback == NULL))
    {
        LogError("Bad arguments: frame_codec = %p, frame_received_callback = %p, empty_frame_received_callback = %p, amqp_frame_codec_error_callback = %p",
            frame_codec, frame_received_callback, empty_frame_received_callback, amqp_frame_codec_error_callback);
        result = NULL;
    }
    else
    {
        result = (AMQP_FRAME_CODEC_HANDLE)calloc(1, sizeof(AMQP_FRAME_CODEC));
        /* Codes_SRS_AMQP_FRAME_CODEC_01_020: [If allocating memory for the new amqp_frame_codec fails, then amqp_frame_codec_create shall fail and return NULL.] */
        if (result == NULL)
        {
            LogError("Could not allocate memory for AMQP frame codec");
        }
        else
        {
            result->frame_codec = frame_codec;
            result->frame_received_callback = frame_received_callback;
            result->empty_frame_received_callback = empty_frame_received_callback;
            result->error_callback = amqp_frame_codec_error_callback;
            result->callback_context = callback_context;
            result->decode_state = AMQP_FRAME_DECODE_FRAME;

            /* Codes_SRS_AMQP_FRAME_CODEC_01_018: [amqp_frame_codec_create shall create a decoder to be used for decoding AMQP values.] */
            result->decoder = amqpvalue_decoder_create(amqp_value_decoded, result);
            if (result->decoder == NULL)
            {
                /* Codes_SRS_AMQP_FRAME_CODEC_01_019: [If creating the decoder fails, amqp_frame_codec_create shall fail and return NULL.] */
                LogError("Could not create AMQP decoder");
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_AMQP_FRAME_CODEC_01_013: [amqp_frame_codec_create shall subscribe for AMQP frames with the given frame_codec.] */
                if (frame_codec_subscribe(frame_codec, FRAME_TYPE_AMQP, frame_received, result) != 0)
                {
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_014: [If subscribing for AMQP frames fails, amqp_frame_codec_create shall fail and return NULL.] */
                    LogError("Could not subscribe for received AMQP frames");
                    amqpvalue_decoder_destroy(result->decoder);
                    free(result);
                    result = NULL;
                }
            }
        }
    }

    return result;
}

void amqp_frame_codec_destroy(AMQP_FRAME_CODEC_HANDLE amqp_frame_codec)
{
    if (amqp_frame_codec == NULL)
    {
        /* Codes_SRS_AMQP_FRAME_CODEC_01_016: [If amqp_frame_codec is NULL, amqp_frame_codec_destroy shall do nothing.] */
        LogError("NULL amqp_frame_codec");
    }
    else
    {
        /* Codes_SRS_AMQP_FRAME_CODEC_01_017: [amqp_frame_codec_destroy shall unsubscribe from receiving AMQP frames from the frame_codec that was passed to amqp_frame_codec_create.] */
        (void)frame_codec_unsubscribe(amqp_frame_codec->frame_codec, FRAME_TYPE_AMQP);

        /* Codes_SRS_AMQP_FRAME_CODEC_01_021: [The decoder created in amqp_frame_codec_create shall be destroyed by amqp_frame_codec_destroy.] */
        amqpvalue_decoder_destroy(amqp_frame_codec->decoder);

        /* Codes_SRS_AMQP_FRAME_CODEC_01_015: [amqp_frame_codec_destroy shall free all resources associated with the amqp_frame_codec instance.] */
        free(amqp_frame_codec);
    }
}

int amqp_frame_codec_encode_frame(AMQP_FRAME_CODEC_HANDLE amqp_frame_codec, uint16_t channel, AMQP_VALUE performative, const PAYLOAD* payloads, size_t payload_count, ON_BYTES_ENCODED on_bytes_encoded, void* callback_context)
{
    int result;

    /* Codes_SRS_AMQP_FRAME_CODEC_01_024: [If frame_codec, performative or on_bytes_encoded is NULL, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
    if ((amqp_frame_codec == NULL) ||
        (performative == NULL) ||
        (on_bytes_encoded == NULL))
    {
        LogError("Bad arguments: amqp_frame_codec = %p, performative = %p, on_bytes_encoded = %p",
            amqp_frame_codec, performative, on_bytes_encoded);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE descriptor;
        uint64_t performative_ulong;
        size_t encoded_size;

        if ((descriptor = amqpvalue_get_inplace_descriptor(performative)) == NULL)
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_029: [If any error occurs during encoding, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Getting the descriptor failed");
            result = MU_FAILURE;
        }
        else if (amqpvalue_get_ulong(descriptor, &performative_ulong) != 0)
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_029: [If any error occurs during encoding, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Getting the descriptor ulong failed");
            result = MU_FAILURE;
        }
        /* Codes_SRS_AMQP_FRAME_CODEC_01_008: [The performative MUST be one of those defined in section 2.7 and is encoded as a described type in the AMQP type system.] */
        else if ((performative_ulong < AMQP_OPEN) ||
            (performative_ulong > AMQP_CLOSE))
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_029: [If any error occurs during encoding, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Bad arguments: amqp_frame_codec = %p, performative = %p, on_bytes_encoded = %p",
                amqp_frame_codec, performative, on_bytes_encoded);
            result = MU_FAILURE;
        }
        /* Codes_SRS_AMQP_FRAME_CODEC_01_027: [The encoded size of the performative and its fields shall be obtained by calling amqpvalue_get_encoded_size.] */
        else if (amqpvalue_get_encoded_size(performative, &encoded_size) != 0)
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_029: [If any error occurs during encoding, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
            LogError("Getting the encoded size failed");
            result = MU_FAILURE;
        }
        else
        {
            unsigned char* amqp_performative_bytes = (unsigned char*)malloc(encoded_size);
            if (amqp_performative_bytes == NULL)
            {
                LogError("Could not allocate performative bytes");
                result = MU_FAILURE;
            }
            else
            {
                PAYLOAD* new_payloads = (PAYLOAD*)calloc(1, (sizeof(PAYLOAD) * (payload_count + 1)));
                if (new_payloads == NULL)
                {
                    LogError("Could not allocate frame payloads");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_070: [The payloads argument for frame_codec_encode_frame shall be made of the payload for the encoded performative and the payloads passed to amqp_frame_codec_encode_frame.] */
                    /* Codes_SRS_AMQP_FRAME_CODEC_01_028: [The encode result for the performative shall be placed in a PAYLOAD structure.] */
                    new_payloads[0].bytes = amqp_performative_bytes;
                    new_payloads[0].length = 0;

                    if (payload_count > 0)
                    {
                        (void)memcpy(new_payloads + 1, payloads, sizeof(PAYLOAD) * payload_count);
                    }

                    if (amqpvalue_encode(performative, encode_bytes, &new_payloads[0]) != 0)
                    {
                        LogError("amqpvalue_encode failed");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        unsigned char channel_bytes[2];

                        channel_bytes[0] = channel >> 8;
                        channel_bytes[1] = channel & 0xFF;

                        /* Codes_SRS_AMQP_FRAME_CODEC_01_005: [Bytes 6 and 7 of an AMQP frame contain the channel number ] */
                        /* Codes_SRS_AMQP_FRAME_CODEC_01_025: [amqp_frame_codec_encode_frame shall encode the frame header by using frame_codec_encode_frame.] */
                        /* Codes_SRS_AMQP_FRAME_CODEC_01_006: [The frame body is defined as a performative followed by an opaque payload.] */
                        if (frame_codec_encode_frame(amqp_frame_codec->frame_codec, FRAME_TYPE_AMQP, new_payloads, payload_count + 1, channel_bytes, sizeof(channel_bytes), on_bytes_encoded, callback_context) != 0)
                        {
                            /* Codes_SRS_AMQP_FRAME_CODEC_01_029: [If any error occurs during encoding, amqp_frame_codec_encode_frame shall fail and return a non-zero value.] */
                            LogError("frame_codec_encode_frame failed");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            /* Codes_SRS_AMQP_FRAME_CODEC_01_022: [amqp_frame_codec_begin_encode_frame shall encode the frame header and AMQP performative in an AMQP frame and on success it shall return 0.] */
                            result = 0;
                        }
                    }

                    free(new_payloads);
                }

                free(amqp_performative_bytes);
            }
        }
    }

    return result;
}

/* Codes_SRS_AMQP_FRAME_CODEC_01_042: [amqp_frame_codec_encode_empty_frame shall encode a frame with no payload.] */
/* Codes_SRS_AMQP_FRAME_CODEC_01_010: [An AMQP frame with no body MAY be used to generate artificial traffic as needed to satisfy any negotiated idle timeout interval ] */
int amqp_frame_codec_encode_empty_frame(AMQP_FRAME_CODEC_HANDLE amqp_frame_codec, uint16_t channel, ON_BYTES_ENCODED on_bytes_encoded, void* callback_context)
{
    int result;

    /* Codes_SRS_AMQP_FRAME_CODEC_01_045: [If amqp_frame_codec is NULL, amqp_frame_codec_encode_empty_frame shall fail and return a non-zero value.] */
    if (amqp_frame_codec == NULL)
    {
        LogError("NULL amqp_frame_codec");
        result = MU_FAILURE;
    }
    else
    {
        unsigned char channel_bytes[2];

        channel_bytes[0] = channel >> 8;
        channel_bytes[1] = channel & 0xFF;

        /* Codes_SRS_AMQP_FRAME_CODEC_01_044: [amqp_frame_codec_encode_empty_frame shall use frame_codec_encode_frame to encode the frame.] */
        if (frame_codec_encode_frame(amqp_frame_codec->frame_codec, FRAME_TYPE_AMQP, NULL, 0, channel_bytes, sizeof(channel_bytes), on_bytes_encoded, callback_context) != 0)
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_046: [If encoding fails in any way, amqp_frame_codec_encode_empty_frame shall fail and return a non-zero value.]  */
            LogError("frame_codec_encode_frame failed when encoding empty frame");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_AMQP_FRAME_CODEC_01_043: [On success, amqp_frame_codec_encode_empty_frame shall return 0.] */
            result = 0;
        }
    }

    return result;
}
