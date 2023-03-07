// Copyright 2021 The Pigweed Authors
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

#include <stdint.h>

#include "pw_log_tokenized/config.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

// TODO(hepler): Remove this include.
#ifdef __cplusplus
#include "pw_log_tokenized/metadata.h"
#endif  // __cplusplus

// This macro implements PW_LOG using
// PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD or an equivalent alternate macro
// provided by PW_LOG_TOKENIZED_ENCODE_MESSAGE. The log level, module token, and
// flags are packed into the payload argument.
//
// Two strings are tokenized in this macro:
//
//   - The log format string, tokenized in the default tokenizer domain.
//   - PW_LOG_MODULE_NAME, masked to 16 bits and tokenized in the
//     "pw_log_module_names" tokenizer domain.
//
// To use this macro, implement pw_tokenizer_HandleEncodedMessageWithPayload,
// which is defined in pw_tokenizer/tokenize.h. The log metadata can be accessed
// using pw::log_tokenized::Metadata. For example:
//
//   extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
//       pw_tokenizer_Payload payload, const uint8_t data[], size_t size) {
//     pw::log_tokenized::Metadata metadata(payload);
//
//     if (metadata.level() >= kLogLevel && ModuleEnabled(metadata.module())) {
//       EmitLogMessage(data, size, metadata.flags());
//     }
//   }
//
#define PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(                     \
    level, flags, message, ...)                                              \
  do {                                                                       \
    _PW_TOKENIZER_CONST uintptr_t _pw_log_tokenized_module_token =           \
        PW_TOKENIZE_STRING_MASK("pw_log_module_names",                       \
                                ((1u << PW_LOG_TOKENIZED_MODULE_BITS) - 1u), \
                                PW_LOG_MODULE_NAME);                         \
    const uintptr_t _pw_log_tokenized_level = level;                         \
    PW_LOG_TOKENIZED_ENCODE_MESSAGE(                                         \
        (_PW_LOG_TOKENIZED_LEVEL(_pw_log_tokenized_level) |                  \
         _PW_LOG_TOKENIZED_MODULE(_pw_log_tokenized_module_token) |          \
         _PW_LOG_TOKENIZED_FLAGS(flags) | _PW_LOG_TOKENIZED_LINE(__LINE__)), \
        PW_LOG_TOKENIZED_FORMAT_STRING(message),                             \
        __VA_ARGS__);                                                        \
  } while (0)

// If the level field is present, clamp it to the maximum value.
#if PW_LOG_TOKENIZED_LEVEL_BITS == 0
#define _PW_LOG_TOKENIZED_LEVEL(value) ((uintptr_t)0)
#else
#define _PW_LOG_TOKENIZED_LEVEL(value)                   \
  (value < ((uintptr_t)1 << PW_LOG_TOKENIZED_LEVEL_BITS) \
       ? value                                           \
       : ((uintptr_t)1 << PW_LOG_TOKENIZED_LEVEL_BITS) - 1)
#endif  // PW_LOG_TOKENIZED_LEVEL_BITS

// If the line number field is present, shift it to its position. Set it to zero
// if the line number is too large for PW_LOG_TOKENIZED_LINE_BITS.
#if PW_LOG_TOKENIZED_LINE_BITS == 0
#define _PW_LOG_TOKENIZED_LINE(line) ((uintptr_t)0)
#else
#define _PW_LOG_TOKENIZED_LINE(line)                                \
  ((uintptr_t)(line < (1 << PW_LOG_TOKENIZED_LINE_BITS) ? line : 0) \
   << PW_LOG_TOKENIZED_LEVEL_BITS)
#endif  // PW_LOG_TOKENIZED_LINE_BITS

// If the flags field is present, mask it and shift it to its position.
#if PW_LOG_TOKENIZED_FLAG_BITS == 0
#define _PW_LOG_TOKENIZED_FLAGS(value) ((uintptr_t)0)
#else
#define _PW_LOG_TOKENIZED_FLAGS(value)                                       \
  (((uintptr_t)(value) & (((uintptr_t)1 << PW_LOG_TOKENIZED_FLAG_BITS) - 1)) \
   << (PW_LOG_TOKENIZED_LEVEL_BITS + PW_LOG_TOKENIZED_LINE_BITS))
#endif  // PW_LOG_TOKENIZED_FLAG_BITS

// If the module field is present, shift it to its position.
#if PW_LOG_TOKENIZED_MODULE_BITS == 0
#define _PW_LOG_TOKENIZED_MODULE(value) ((uintptr_t)0)
#else
#define _PW_LOG_TOKENIZED_MODULE(value)                  \
  ((uintptr_t)(value) << ((PW_LOG_TOKENIZED_LEVEL_BITS + \
                           PW_LOG_TOKENIZED_LINE_BITS +  \
                           PW_LOG_TOKENIZED_FLAG_BITS)))
#endif  // PW_LOG_TOKENIZED_MODULE_BITS
