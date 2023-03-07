// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pw_preprocessor/util.h"
#include "pw_tokenizer/tokenize.h"

// Like PW_TOKENIZE_TO_GLOBAL_HANDLER, encodes a tokenized string and arguments
// to a buffer on the stack. The macro adds a payload argument, which is passed
// through to the global handler function
// pw_tokenizer_HandleEncodedMessageWithPayload, which must be defined by the
// user of pw_tokenizer. The payload is a uintptr_t.
//
// For example, the following tokenizes a log string and passes the log level as
// the payload.
/*
     #define LOG_ERROR(...) \
         PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(kLogLevelError, __VA_ARGS__)

     void pw_tokenizer_HandleEncodedMessageWithPayload(
         pw_tokenizer_Payload log_level,
         const uint8_t encoded_message[],
         size_t size_bytes) {
       if (log_level >= kLogLevelWarning) {
         MyProject_EnqueueMessageForUart(buffer, size_bytes);
       }
     }
 */
#define PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(payload, format, ...) \
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN(                     \
      PW_TOKENIZER_DEFAULT_DOMAIN, payload, format, __VA_ARGS__)

// Same as PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD, but tokenizes to the
// specified domain.
#define PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN( \
    domain, payload, format, ...)                          \
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_MASK(         \
      domain, UINT32_MAX, payload, format, __VA_ARGS__)

// Same as PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN, but applies a mask
// to the token.
#define PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_MASK(                 \
    domain, mask, payload, format, ...)                                  \
  do {                                                                   \
    PW_TOKENIZE_FORMAT_STRING(domain, mask, format, __VA_ARGS__);        \
    _pw_tokenizer_ToGlobalHandlerWithPayload(                            \
        payload,                                                         \
        _pw_tokenizer_token,                                             \
        PW_TOKENIZER_ARG_TYPES(__VA_ARGS__) PW_COMMA_ARGS(__VA_ARGS__)); \
  } while (0)

PW_EXTERN_C_START

typedef uintptr_t pw_tokenizer_Payload;

// This function must be defined pw_tokenizer:global_handler_with_payload
// backend. This function is called with the encoded message by
// pw_tokenizer_ToGlobalHandler and a caller-provided payload argument.
void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload,
    const uint8_t encoded_message[],
    size_t size_bytes);

// This function encodes the tokenized strings. Do not call it directly;
// instead, use the PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD macro.
void _pw_tokenizer_ToGlobalHandlerWithPayload(pw_tokenizer_Payload payload,
                                              pw_tokenizer_Token token,
                                              pw_tokenizer_ArgTypes types,
                                              ...);

PW_EXTERN_C_END
