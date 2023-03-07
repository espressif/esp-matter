// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UWS_FRAME_ENCODER_H
#define UWS_FRAME_ENCODER_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stdbool.h>
#include <stddef.h>
#endif

#include "azure_c_shared_utility/buffer_.h"
#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RESERVED_1  0x04
#define RESERVED_2  0x02
#define RESERVED_3  0x01

#define WS_FRAME_TYPE_VALUES \
    WS_CONTINUATION_FRAME, \
    WS_TEXT_FRAME, \
    WS_BINARY_FRAME, \
    WS_RESERVED_NON_CONTROL_FRAME_3, \
    WS_RESERVED_NON_CONTROL_FRAME_4, \
    WS_RESERVED_NON_CONTROL_FRAME_5, \
    WS_RESERVED_NON_CONTROL_FRAME_6, \
    WS_RESERVED_NON_CONTROL_FRAME_7, \
    WS_CLOSE_FRAME, \
    WS_PING_FRAME, \
    WS_PONG_FRAME, \
    WS_RESERVED_CONTROL_FRAME_B, \
    WS_RESERVED_CONTROL_FRAME_C, \
    WS_RESERVED_CONTROL_FRAME_D, \
    WS_RESERVED_CONTROL_FRAME_E, \
    WS_RESERVED_CONTROL_FRAME_F

MU_DEFINE_ENUM_WITHOUT_INVALID(WS_FRAME_TYPE, WS_FRAME_TYPE_VALUES);

MOCKABLE_FUNCTION(, BUFFER_HANDLE, uws_frame_encoder_encode, WS_FRAME_TYPE, opcode, const unsigned char*, payload, size_t, length, bool, is_masked, bool, is_final, unsigned char, reserved);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UWS_FRAME_ENCODER_H */
