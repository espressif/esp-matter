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

#ifdef __cplusplus

#include <cstddef>
#include <cstdint>

#else

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#endif  // __cplusplus

#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/concat.h"
#include "pw_preprocessor/util.h"
#include "pw_tokenizer/internal/argument_types.h"
#include "pw_tokenizer/internal/tokenize_string.h"

// The type of the token used in place of a format string. Also available as
// pw::tokenizer::Token.
typedef uint32_t pw_tokenizer_Token;

// Strings may optionally be tokenized to a domain. Strings in different domains
// can be processed separately by the token database tools. Each domain in use
// must have a corresponding section declared in the linker script. See
// pw_tokenizer_linker_sections.ld for more details.
//
// The default domain is an empty string.
#define PW_TOKENIZER_DEFAULT_DOMAIN ""

// Tokenizes a string and converts it to a pw_tokenizer_Token. In C++, the
// string may be a literal or a constexpr char array. In C, the argument must be
// a string literal. In either case, the string must be null terminated, but may
// contain any characters (including '\0').
//
// Two different versions are provided, PW_TOKENIZE_STRING and
// PW_TOKENIZE_STRING_EXPR. PW_TOKENIZE_STRING can be assigned to a local or
// global variable, including constexpr variables. PW_TOKENIZE_STRING can be
// used with special function variables like __func__.
//
// PW_TOKENIZE_STRING_EXPR can be used inside an expression.
// PW_TOKENIZE_STRING_EXPR is implemented using a lambda function, so it will
// not work as expected with special function variables like __func__. It is
// also only usable with C++.
//
//   constexpr uint32_t global = PW_TOKENIZE_STRING("Wow!");  // This works.
//
//   void SomeFunction() {
//     constexpr uint32_t token = PW_TOKENIZE_STRING("Cool!");  // This works.
//
//     DoSomethingElse(PW_TOKENIZE_STRING("Lame!"));  // This does NOT work.
//     DoSomethingElse(PW_TOKENIZE_STRING_EXPR("Yay!"));  // This works.
//
//     constexpr uint32_t token2 = PW_TOKENIZE_STRING(__func__);  // This works.
//     DoSomethingElse(PW_TOKENIZE_STRING_EXPR(__func__));  // Does NOT work.
//   }
//
#define PW_TOKENIZE_STRING(string_literal) \
  PW_TOKENIZE_STRING_DOMAIN(PW_TOKENIZER_DEFAULT_DOMAIN, string_literal)

#define PW_TOKENIZE_STRING_EXPR(string_literal)                               \
  [&] {                                                                       \
    constexpr uint32_t lambda_ret_token = PW_TOKENIZE_STRING(string_literal); \
    return lambda_ret_token;                                                  \
  }()

// Same as PW_TOKENIZE_STRING, but tokenizes to the specified domain.
#define PW_TOKENIZE_STRING_DOMAIN(domain, string_literal) \
  PW_TOKENIZE_STRING_MASK(domain, UINT32_MAX, string_literal)

#define PW_TOKENIZE_STRING_DOMAIN_EXPR(domain, string_literal) \
  [&] {                                                        \
    constexpr uint32_t lambda_ret_token =                      \
        PW_TOKENIZE_STRING_DOMAIN(domain, string_literal);     \
    return lambda_ret_token;                                   \
  }()

// Same as PW_TOKENIZE_STRING_DOMAIN, but applies a mask to the token.
#define PW_TOKENIZE_STRING_MASK(domain, mask, string_literal)                \
  /* assign to a variable */ _PW_TOKENIZER_MASK_TOKEN(mask, string_literal); \
                                                                             \
  static_assert(0 < (mask) && (mask) <= UINT32_MAX,                          \
                "Tokenizer masks must be non-zero uint32_t values.");        \
                                                                             \
  _PW_TOKENIZER_RECORD_ORIGINAL_STRING(                                      \
      _PW_TOKENIZER_MASK_TOKEN(mask, string_literal), domain, string_literal)

#define PW_TOKENIZE_STRING_MASK_EXPR(domain, mask, string_literal) \
  [&] {                                                            \
    constexpr uint32_t lambda_ret_token =                          \
        PW_TOKENIZE_STRING_MASK(domain, mask, string_literal);     \
    return lambda_ret_token;                                       \
  }()

#define _PW_TOKENIZER_MASK_TOKEN(mask, string_literal) \
  ((pw_tokenizer_Token)(mask)&PW_TOKENIZER_STRING_TOKEN(string_literal))

// Encodes a tokenized string and arguments to the provided buffer. The size of
// the buffer is passed via a pointer to a size_t. After encoding is complete,
// the size_t is set to the number of bytes written to the buffer.
//
// The macro's arguments are equivalent to the following function signature:
//
//   TokenizeToBuffer(void* buffer,
//                    size_t* buffer_size_pointer,
//                    const char* format,
//                    ...);  /* printf-style arguments */
//
// For example, the following encodes a tokenized string with a temperature to a
// buffer. The buffer is passed to a function to send the message over a UART.
//
//   uint8_t buffer[32];
//   size_t size_bytes = sizeof(buffer);
//   PW_TOKENIZE_TO_BUFFER(
//       buffer, &size_bytes, "Temperature (C): %0.2f", temperature_c);
//   MyProject_EnqueueMessageForUart(buffer, size);
//
#define PW_TOKENIZE_TO_BUFFER(buffer, buffer_size_pointer, format, ...) \
  PW_TOKENIZE_TO_BUFFER_DOMAIN(PW_TOKENIZER_DEFAULT_DOMAIN,             \
                               buffer,                                  \
                               buffer_size_pointer,                     \
                               format,                                  \
                               __VA_ARGS__)

// Same as PW_TOKENIZE_TO_BUFFER, but tokenizes to the specified domain.
#define PW_TOKENIZE_TO_BUFFER_DOMAIN(                 \
    domain, buffer, buffer_size_pointer, format, ...) \
  PW_TOKENIZE_TO_BUFFER_MASK(                         \
      domain, UINT32_MAX, buffer, buffer_size_pointer, format, __VA_ARGS__)

// Same as PW_TOKENIZE_TO_BUFFER_DOMAIN, but applies a mask to the token.
#define PW_TOKENIZE_TO_BUFFER_MASK(                               \
    domain, mask, buffer, buffer_size_pointer, format, ...)       \
  do {                                                            \
    PW_TOKENIZE_FORMAT_STRING(domain, mask, format, __VA_ARGS__); \
    _pw_tokenizer_ToBuffer(buffer,                                \
                           buffer_size_pointer,                   \
                           _pw_tokenizer_token,                   \
                           PW_TOKENIZER_ARG_TYPES(__VA_ARGS__)    \
                               PW_COMMA_ARGS(__VA_ARGS__));       \
  } while (0)

// Encodes a tokenized string and arguments to a buffer on the stack. The
// provided callback is called with the encoded data. The size of the
// stack-allocated argument encoding buffer is set with the
// PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES option.
//
// The macro's arguments are equivalent to the following function signature:
//
//   TokenizeToCallback(void (*callback)(const uint8_t* data, size_t size),
//                      const char* format,
//                      ...);  /* printf-style arguments */
//
// For example, the following encodes a tokenized string with a sensor name and
// floating point data. The encoded message is passed directly to the
// MyProject_EnqueueMessageForUart function, which the caller provides as a
// callback.
//
//   void MyProject_EnqueueMessageForUart(const uint8_t* buffer,
//                                        size_t size_bytes) {
//     uart_queue_write(uart_instance, buffer, size_bytes);
//   }
//
//   void LogSensorValue(const char* sensor_name, float value) {
//     PW_TOKENIZE_TO_CALLBACK(MyProject_EnqueueMessageForUart,
//                             "%s: %f",
//                             sensor_name,
//                             value);
//   }
//
#define PW_TOKENIZE_TO_CALLBACK(callback, format, ...) \
  PW_TOKENIZE_TO_CALLBACK_DOMAIN(                      \
      PW_TOKENIZER_DEFAULT_DOMAIN, callback, format, __VA_ARGS__)

// Same as PW_TOKENIZE_TO_CALLBACK, but tokenizes to the specified domain.
#define PW_TOKENIZE_TO_CALLBACK_DOMAIN(domain, callback, format, ...) \
  PW_TOKENIZE_TO_CALLBACK_MASK(                                       \
      domain, UINT32_MAX, callback, format, __VA_ARGS__)

// Same as PW_TOKENIZE_TO_CALLBACK_DOMAIN, but applies a mask to the token.
#define PW_TOKENIZE_TO_CALLBACK_MASK(domain, mask, callback, format, ...) \
  do {                                                                    \
    PW_TOKENIZE_FORMAT_STRING(domain, mask, format, __VA_ARGS__);         \
    _pw_tokenizer_ToCallback(callback,                                    \
                             _pw_tokenizer_token,                         \
                             PW_TOKENIZER_ARG_TYPES(__VA_ARGS__)          \
                                 PW_COMMA_ARGS(__VA_ARGS__));             \
  } while (0)

PW_EXTERN_C_START

// These functions encode the tokenized strings. These should not be called
// directly. Instead, use the corresponding PW_TOKENIZE_TO_* macros above.
void _pw_tokenizer_ToBuffer(void* buffer,
                            size_t* buffer_size_bytes,  // input and output arg
                            pw_tokenizer_Token token,
                            pw_tokenizer_ArgTypes types,
                            ...);

void _pw_tokenizer_ToCallback(void (*callback)(const uint8_t* encoded_message,
                                               size_t size_bytes),
                              pw_tokenizer_Token token,
                              pw_tokenizer_ArgTypes types,
                              ...);

// This empty function allows the compiler to check the format string.
static inline void pw_tokenizer_CheckFormatString(const char* format, ...)
    PW_PRINTF_FORMAT(1, 2);

static inline void pw_tokenizer_CheckFormatString(const char* format, ...) {
  (void)format;
}

PW_EXTERN_C_END

// These macros implement string tokenization. They should not be used directly;
// use one of the PW_TOKENIZE_* macros above instead.

// This macro takes a printf-style format string and corresponding arguments. It
// checks that the arguments are correct, stores the format string in a special
// section, and calculates the string's token at compile time. This
// clang-format off
#define PW_TOKENIZE_FORMAT_STRING(domain, mask, format, ...)                  \
  if (0) { /* Do not execute to prevent double evaluation of the arguments. */ \
    pw_tokenizer_CheckFormatString(format PW_COMMA_ARGS(__VA_ARGS__));         \
  }                                                                            \
                                                                               \
  /* Check that the macro is invoked with a supported number of arguments. */  \
  static_assert(                                                               \
      PW_FUNCTION_ARG_COUNT(__VA_ARGS__) <= PW_TOKENIZER_MAX_SUPPORTED_ARGS,   \
      "Tokenized strings cannot have more than "                               \
      PW_STRINGIFY(PW_TOKENIZER_MAX_SUPPORTED_ARGS) " arguments; "             \
      PW_STRINGIFY(PW_FUNCTION_ARG_COUNT(__VA_ARGS__))                         \
      " arguments were used for " #format " (" #__VA_ARGS__ ")");              \
                                                                               \
  /* Tokenize the string to a pw_tokenizer_Token at compile time. */           \
  static _PW_TOKENIZER_CONST pw_tokenizer_Token _pw_tokenizer_token =          \
      _PW_TOKENIZER_MASK_TOKEN(mask, format);                                  \
                                                                               \
  _PW_TOKENIZER_RECORD_ORIGINAL_STRING(_pw_tokenizer_token, domain, format)

// clang-format on

// Creates unique names to use for tokenized string entries and linker sections.
#define _PW_TOKENIZER_UNIQUE(prefix) PW_CONCAT(prefix, __LINE__, _, __COUNTER__)

#ifdef __cplusplus

#define _PW_TOKENIZER_CONST constexpr

#define _PW_TOKENIZER_RECORD_ORIGINAL_STRING(token, domain, string)            \
  alignas(1) static constexpr auto _PW_TOKENIZER_SECTION _PW_TOKENIZER_UNIQUE( \
      _pw_tokenizer_string_entry_) =                                           \
      ::pw::tokenizer::internal::MakeEntry(token, domain, string)

namespace pw {
namespace tokenizer {

using Token = ::pw_tokenizer_Token;

}  // namespace tokenizer
}  // namespace pw

#else

#define _PW_TOKENIZER_CONST const

#define _PW_TOKENIZER_RECORD_ORIGINAL_STRING(token, domain, string) \
  _Alignas(1) static const _PW_TOKENIZER_STRING_ENTRY(token, domain, string)

#endif  // __cplusplus

// _PW_TOKENIZER_SECTION places the tokenized strings in a special .pw_tokenizer
// linker section. Host-side decoding tools read the strings and tokens from
// this section to build a database of tokenized strings.
//
// This section should be declared as type INFO so that it is excluded from the
// final binary. To declare the section, as well as the .pw_tokenizer.info
// metadata section, add the following to the linker script's SECTIONS command:
//
//   .pw_tokenizer.info 0x0 (INFO) :
//   {
//     KEEP(*(.pw_tokenizer.info))
//   }
//
//   .pw_tokenizer.entries 0x0 (INFO) :
//   {
//     KEEP(*(.pw_tokenizer.entries.*))
//   }
//
// A linker script snippet that provides these sections is provided in the file
// pw_tokenizer_linker_sections.ld. This file may be directly included into
// existing linker scripts.
//
// The tokenized string sections can also be managed without linker script
// modifications, though this is not recommended. The section can be extracted
// and removed from the ELF with objcopy:
//
//   objcopy --only-section .pw_tokenizer.* <ORIGINAL_ELF> <OUTPUT_ELF>
//   objcopy --remove-section .pw_tokenizer.* <ORIGINAL_ELF>
//
// OUTPUT_ELF will be an ELF with only the tokenized strings, and the original
// ELF file will have the sections removed.
//
// Without the above linker script modifications, the section garbage collection
// option (--gc-sections) removes the tokenized string sections. To avoid
// editing the target linker script, a separate metadata ELF can be linked
// without --gc-sections to preserve the tokenized data.
//
// pw_tokenizer is intended for use with ELF files only. Mach-O files (macOS
// executables) do not support section names longer than 16 characters, so a
// short, unused section name is used on macOS.
#ifdef __APPLE__
#define _PW_TOKENIZER_SECTION \
  PW_KEEP_IN_SECTION(PW_STRINGIFY(_PW_TOKENIZER_UNIQUE(.pw.)))
#else
#define _PW_TOKENIZER_SECTION \
  PW_KEEP_IN_SECTION(PW_STRINGIFY(_PW_TOKENIZER_UNIQUE(.pw_tokenizer.entries.)))
#endif  // __APPLE__
