// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASL_FRAME_CODEC_H
#define SASL_FRAME_CODEC_H

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#endif /* __cplusplus */
#include "azure_uamqp_c/frame_codec.h"

#include "umock_c/umock_c_prod.h"

#define SASL_MECHANISMS         (uint64_t)0x40
#define SASL_INIT               (uint64_t)0x41
#define SASL_CHALLENGE          (uint64_t)0x42
#define SASL_RESPONSE           (uint64_t)0x43
#define SASL_OUTCOME            (uint64_t)0x44

typedef struct SASL_FRAME_CODEC_INSTANCE_TAG* SASL_FRAME_CODEC_HANDLE;
typedef void(*ON_SASL_FRAME_RECEIVED)(void* context, AMQP_VALUE sasl_frame_value);
typedef void(*ON_SASL_FRAME_CODEC_ERROR)(void* context);

MOCKABLE_FUNCTION(, SASL_FRAME_CODEC_HANDLE, sasl_frame_codec_create, FRAME_CODEC_HANDLE, frame_codec, ON_SASL_FRAME_RECEIVED, on_sasl_frame_received, ON_SASL_FRAME_CODEC_ERROR, on_sasl_frame_codec_error, void*, callback_context);
MOCKABLE_FUNCTION(, void, sasl_frame_codec_destroy, SASL_FRAME_CODEC_HANDLE, sasl_frame_codec);
MOCKABLE_FUNCTION(, int, sasl_frame_codec_encode_frame, SASL_FRAME_CODEC_HANDLE, sasl_frame_codec, AMQP_VALUE, sasl_frame_value, ON_BYTES_ENCODED, on_bytes_encoded, void*, callback_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SASL_FRAME_CODEC_H */
