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

// Configuration macros for the tokenizer module.
#pragma once

#include <assert.h>

// For a tokenized string that has arguments, the types of the arguments are
// encoded in either a 4-byte (uint32_t) or a 8-byte (uint64_t) value. The 4 or
// 6 least-significant bits, respectively, store the number of arguments, while
// the remaining bits encode the argument types. Argument types are encoded
// two-bits per argument, in little-endian order. Up to 14 arguments in 4 bytes
// or 29 arguments in 8 bytes are supported.
#ifndef PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES
#define PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES 4
#endif  // PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES

static_assert(PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 4 ||
                  PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 8,
              "PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES must be 4 or 8");

// Maximum number of characters to hash in C. In C code, strings shorter than
// this length are treated as if they were zero-padded up to the length. Strings
// that are the same length and share a common prefix longer than this value
// hash to the same value. Increasing PW_TOKENIZER_CFG_C_HASH_LENGTH increases
// the compilation time for C due to the complexity of the hashing macros.
//
// PW_TOKENIZER_CFG_C_HASH_LENGTH has no effect on C++ code. In C++, hashing is
// done with a constexpr function instead of a macro. There are no string length
// limitations and compilation times are unaffected by this macro.
//
// Only hash lengths for which there is a corresponding macro header
// (pw_tokenizer/internal/mash_macro_#.h) are supported. Additional macros may
// be generated with the generate_hash_macro.py function. New macro headers must
// then be added to pw_tokenizer/internal/hash.h.
//
// This MUST match the value of DEFAULT_C_HASH_LENGTH in
// pw_tokenizer/py/pw_tokenizer/tokens.py.
#ifndef PW_TOKENIZER_CFG_C_HASH_LENGTH
#define PW_TOKENIZER_CFG_C_HASH_LENGTH 128
#endif  // PW_TOKENIZER_CFG_C_HASH_LENGTH

// The size of the stack-allocated argument encoding buffer to use. This only
// affects tokenization macros that stack-allocate the encoding buffer
// (PW_TOKENIZE_TO_CALLBACK and PW_TOKENIZE_TO_GLOBAL_HANDLER). A buffer of this
// size is allocated and used for the 4-byte token and for encoding all
// arguments. It must be at least large enough for the token (4 bytes).
//
// This buffer does not need to be large to accommodate a good number of
// tokenized string arguments. Integer arguments are usually encoded smaller
// than their native size (e.g. 1 or 2 bytes for smaller numbers). All floating
// point types are encoded as four bytes. Null-terminated strings are encoded
// 1:1 in size.
#ifndef PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES
#define PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES 52
#endif  // PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES
