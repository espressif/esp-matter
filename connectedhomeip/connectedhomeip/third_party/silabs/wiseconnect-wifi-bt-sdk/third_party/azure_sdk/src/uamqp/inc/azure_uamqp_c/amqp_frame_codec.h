// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AMQP_FRAME_CODEC_H
#define AMQP_FRAME_CODEC_H

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

#define AMQP_OPEN               (uint64_t)0x10
#define AMQP_BEGIN              (uint64_t)0x11
#define AMQP_ATTACH             (uint64_t)0x12
#define AMQP_FLOW               (uint64_t)0x13
#define AMQP_TRANSFER           (uint64_t)0x14
#define AMQP_DISPOSITION        (uint64_t)0x15
#define AMQP_DETACH             (uint64_t)0x16
#define AMQP_END                (uint64_t)0x17
#define AMQP_CLOSE              (uint64_t)0x18

typedef struct AMQP_FRAME_CODEC_TAG* AMQP_FRAME_CODEC_HANDLE;
typedef void(*AMQP_EMPTY_FRAME_RECEIVED_CALLBACK)(void* context, uint16_t channel);
typedef void(*AMQP_FRAME_RECEIVED_CALLBACK)(void* context, uint16_t channel, AMQP_VALUE performative, const unsigned char* payload_bytes, uint32_t frame_payload_size);
typedef void(*AMQP_FRAME_CODEC_ERROR_CALLBACK)(void* context);

MOCKABLE_FUNCTION(, AMQP_FRAME_CODEC_HANDLE, amqp_frame_codec_create, FRAME_CODEC_HANDLE, frame_codec, AMQP_FRAME_RECEIVED_CALLBACK, frame_received_callback, AMQP_EMPTY_FRAME_RECEIVED_CALLBACK, empty_frame_received_callback, AMQP_FRAME_CODEC_ERROR_CALLBACK, amqp_frame_codec_error_callback, void*, callback_context);
MOCKABLE_FUNCTION(, void, amqp_frame_codec_destroy, AMQP_FRAME_CODEC_HANDLE, amqp_frame_codec);
MOCKABLE_FUNCTION(, int, amqp_frame_codec_encode_frame, AMQP_FRAME_CODEC_HANDLE, amqp_frame_codec, uint16_t, channel, AMQP_VALUE, performative, const PAYLOAD*, payloads, size_t, payload_count, ON_BYTES_ENCODED, on_bytes_encoded, void*, callback_context);
MOCKABLE_FUNCTION(, int, amqp_frame_codec_encode_empty_frame, AMQP_FRAME_CODEC_HANDLE, amqp_frame_codec, uint16_t, channel, ON_BYTES_ENCODED, on_bytes_encoded, void*, callback_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AMQP_FRAME_CODEC_H */
