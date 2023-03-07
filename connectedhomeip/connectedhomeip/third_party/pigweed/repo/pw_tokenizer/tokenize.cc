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

// This file defines the functions that encode tokenized logs at runtime. These
// are the only pw_tokenizer functions present in a binary that tokenizes
// strings. All other tokenizing code is resolved at compile time.

#include "pw_tokenizer/tokenize.h"

#include <cstring>

#include "pw_tokenizer/encode_args.h"

namespace pw {
namespace tokenizer {
namespace {

// Store metadata about this compilation's string tokenization in the ELF.
//
// The tokenizer metadata will not go into the on-device executable binary code.
// This metadata will be present in the ELF file's .pw_tokenizer.info section,
// from which the host-side tooling (Python, Java, etc.) can understand how to
// decode tokenized strings for the given binary. Only attributes that affect
// the decoding process are recorded.
//
// Tokenizer metadata is stored in an array of key-value pairs. Each Metadata
// object is 32 bytes: a 24-byte string and an 8-byte value. Metadata structs
// may be parsed in Python with the struct format '24s<Q'.
PW_PACKED(struct) Metadata {
  char name[24];   // name of the metadata field
  uint64_t value;  // value of the field
};

static_assert(sizeof(Metadata) == 32, "Metadata should be exactly 32 bytes");

// Store tokenization metadata in its own section. Mach-O files are not
// supported by pw_tokenizer, but a short, Mach-O compatible section name is
// used on macOS so that this file can at least compile.
#ifdef __APPLE__
#define PW_TOKENIZER_INFO_SECTION PW_KEEP_IN_SECTION(".pw_tokenizer")
#else
#define PW_TOKENIZER_INFO_SECTION PW_KEEP_IN_SECTION(".pw_tokenizer.info")
#endif  // __APPLE__

constexpr Metadata metadata[] PW_TOKENIZER_INFO_SECTION = {
    {"c_hash_length_bytes", PW_TOKENIZER_CFG_C_HASH_LENGTH},
    {"sizeof_long", sizeof(long)},            // %l conversion specifier
    {"sizeof_intmax_t", sizeof(intmax_t)},    // %j conversion specifier
    {"sizeof_size_t", sizeof(size_t)},        // %z conversion specifier
    {"sizeof_ptrdiff_t", sizeof(ptrdiff_t)},  // %t conversion specifier
};

}  // namespace

extern "C" void _pw_tokenizer_ToBuffer(void* buffer,
                                       size_t* buffer_size_bytes,
                                       Token token,
                                       pw_tokenizer_ArgTypes types,
                                       ...) {
  if (*buffer_size_bytes < sizeof(token)) {
    *buffer_size_bytes = 0;
    return;
  }

  std::memcpy(buffer, &token, sizeof(token));

  va_list args;
  va_start(args, types);
  const size_t encoded_bytes = EncodeArgs(
      types,
      args,
      span<std::byte>(static_cast<std::byte*>(buffer) + sizeof(token),
                      *buffer_size_bytes - sizeof(token)));
  va_end(args);

  *buffer_size_bytes = sizeof(token) + encoded_bytes;
}

extern "C" void _pw_tokenizer_ToCallback(
    void (*callback)(const uint8_t* encoded_message, size_t size_bytes),
    Token token,
    pw_tokenizer_ArgTypes types,
    ...) {
  va_list args;
  va_start(args, types);
  EncodedMessage encoded(token, types, args);
  va_end(args);

  callback(encoded.data_as_uint8(), encoded.size());
}

}  // namespace tokenizer
}  // namespace pw
