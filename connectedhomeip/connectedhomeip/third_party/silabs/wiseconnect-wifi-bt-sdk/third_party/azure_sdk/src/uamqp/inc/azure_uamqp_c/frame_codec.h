// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef FRAME_CODEC_H
#define FRAME_CODEC_H

#include "azure_uamqp_c/amqpvalue.h"

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

typedef struct PAYLOAD_TAG
{
    const unsigned char* bytes;
    size_t length;
} PAYLOAD;

/* Codes_SRS_FRAME_CODEC_01_016: [The type code indicates the format and purpose of the frame.] */
/* Codes_SRS_FRAME_CODEC_01_017: [The subsequent bytes in the frame header MAY be interpreted differently depending on the type of the frame.] */
/* Codes_SRS_FRAME_CODEC_01_070: [The type code indicates the format and purpose of the frame.] */
/* Codes_SRS_FRAME_CODEC_01_071: [The subsequent bytes in the frame header MAY be interpreted differently depending on the type of the frame.] */
/* Codes_SRS_FRAME_CODEC_01_018: [A type code of 0x00 indicates that the frame is an AMQP frame.] */
/* Codes_SRS_FRAME_CODEC_01_072: [A type code of 0x00 indicates that the frame is an AMQP frame.] */
#define FRAME_TYPE_AMQP    (uint8_t)0x00

/* Codes_SRS_FRAME_CODEC_01_073: [A type code of 0x01 indicates that the frame is a SASL frame] */
/* Codes_SRS_FRAME_CODEC_01_019: [A type code of 0x01 indicates that the frame is a SASL frame] */
#define FRAME_TYPE_SASL    (uint8_t)0x01

    typedef struct FRAME_CODEC_INSTANCE_TAG* FRAME_CODEC_HANDLE;
    typedef void(*ON_FRAME_RECEIVED)(void* context, const unsigned char* type_specific, uint32_t type_specific_size, const unsigned char* frame_body, uint32_t frame_body_size);
    typedef void(*ON_FRAME_CODEC_ERROR)(void* context);
    typedef void(*ON_BYTES_ENCODED)(void* context, const unsigned char* bytes, size_t length, bool encode_complete);

    MOCKABLE_FUNCTION(, FRAME_CODEC_HANDLE, frame_codec_create, ON_FRAME_CODEC_ERROR, on_frame_codec_error, void*, callback_context);
    MOCKABLE_FUNCTION(, void, frame_codec_destroy, FRAME_CODEC_HANDLE, frame_codec);
    MOCKABLE_FUNCTION(, int, frame_codec_set_max_frame_size, FRAME_CODEC_HANDLE, frame_codec, uint32_t, max_frame_size);
    MOCKABLE_FUNCTION(, int, frame_codec_subscribe, FRAME_CODEC_HANDLE, frame_codec, uint8_t, type, ON_FRAME_RECEIVED, on_frame_received, void*, callback_context);
    MOCKABLE_FUNCTION(, int, frame_codec_unsubscribe, FRAME_CODEC_HANDLE, frame_codec, uint8_t, type);
    MOCKABLE_FUNCTION(, int, frame_codec_receive_bytes, FRAME_CODEC_HANDLE, frame_codec, const unsigned char*, buffer, size_t, size);
    MOCKABLE_FUNCTION(, int, frame_codec_encode_frame, FRAME_CODEC_HANDLE, frame_codec, uint8_t, type, const PAYLOAD*, payloads, size_t, payload_count, const unsigned char*, type_specific_bytes, uint32_t, type_specific_size, ON_BYTES_ENCODED, on_bytes_encoded, void*, callback_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FRAME_CODEC_H */
