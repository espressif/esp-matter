# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)
include($ENV{PW_ROOT}/pw_protobuf_compiler/proto.cmake)
include($ENV{PW_ROOT}/pw_tokenizer/backend.cmake)

pw_add_module_config(pw_tokenizer_CONFIG)

pw_add_library(pw_tokenizer.config INTERFACE
  HEADERS
    public/pw_tokenizer/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_tokenizer_CONFIG}
)

pw_add_library(pw_tokenizer STATIC
  HEADERS
    public/pw_tokenizer/encode_args.h
    public/pw_tokenizer/hash.h
    public/pw_tokenizer/tokenize.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_containers
    pw_span
    pw_preprocessor
    pw_tokenizer.config
  SOURCES
    encode_args.cc
    hash.cc
    public/pw_tokenizer/internal/argument_types.h
    public/pw_tokenizer/internal/argument_types_macro_4_byte.h
    public/pw_tokenizer/internal/argument_types_macro_8_byte.h
    public/pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_128_hash_macro.h
    public/pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_256_hash_macro.h
    public/pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_80_hash_macro.h
    public/pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_96_hash_macro.h
    public/pw_tokenizer/internal/tokenize_string.h
    tokenize.cc
  PRIVATE_DEPS
    pw_varint
)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "")
  target_link_options(pw_tokenizer
    PUBLIC
      "-T${CMAKE_CURRENT_SOURCE_DIR}/pw_tokenizer_linker_sections.ld"
  )
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR Zephyr_FOUND)
  target_link_options(pw_tokenizer
    PUBLIC
      "-T${CMAKE_CURRENT_SOURCE_DIR}/add_tokenizer_sections_to_default_script.ld"
      "-L${CMAKE_CURRENT_SOURCE_DIR}"
  )
endif()

pw_add_library(pw_tokenizer.base64 STATIC
  HEADERS
    public/pw_tokenizer/base64.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_base64
    pw_span
    pw_string.string
    pw_tokenizer
    pw_tokenizer.config
  SOURCES
    base64.cc
)

pw_add_library(pw_tokenizer.decoder STATIC
  HEADERS
    public/pw_tokenizer/detokenize.h
    public/pw_tokenizer/token_database.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_span
    pw_tokenizer
  SOURCES
    decode.cc
    detokenize.cc
    public/pw_tokenizer/internal/decode.h
    token_database.cc
  PRIVATE_DEPS
    pw_bytes
    pw_bytes.bit
    pw_varint
)

pw_add_facade(pw_tokenizer.global_handler STATIC
  BACKEND
    pw_tokenizer.global_handler_BACKEND
  HEADERS
    public/pw_tokenizer/tokenize_to_global_handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
    pw_tokenizer
  SOURCES
    tokenize_to_global_handler.cc
)

pw_add_facade(pw_tokenizer.global_handler_with_payload STATIC
  BACKEND
    pw_tokenizer.global_handler_with_payload_BACKEND
  HEADERS
    public/pw_tokenizer/tokenize_to_global_handler_with_payload.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
    pw_tokenizer
  SOURCES
    tokenize_to_global_handler_with_payload.cc
)

pw_proto_library(pw_tokenizer.proto
  SOURCES
    options.proto
  PREFIX
    pw_tokenizer/proto
)

# Executable for generating test data for the C++ and Python detokenizers. This
# target should only be built for the host.
add_executable(pw_tokenizer.generate_decoding_test_data EXCLUDE_FROM_ALL
    generate_decoding_test_data.cc)
target_link_libraries(pw_tokenizer.generate_decoding_test_data PRIVATE
    pw_varint pw_tokenizer)
target_compile_options(pw_tokenizer.generate_decoding_test_data PRIVATE
    -Wall -Werror)

# Executable for generating a test ELF file for elf_reader_test.py. A host
# version of this binary is checked in for use in elf_reader_test.py.
add_executable(pw_tokenizer.elf_reader_test_binary EXCLUDE_FROM_ALL
    py/elf_reader_test_binary.c)
target_link_libraries(pw_tokenizer.elf_reader_test_binary PRIVATE
    -Wl,--unresolved-symbols=ignore-all)  # main is not defined
set_target_properties(pw_tokenizer.elf_reader_test_binary PROPERTIES
    OUTPUT_NAME elf_reader_test_binary.elf)

pw_add_test(pw_tokenizer.argument_types_test
  SOURCES
    argument_types_test_c.c
    argument_types_test.cc
  PRIVATE_DEPS
    pw_tokenizer
  GROUPS
    modules
    pw_tokenizer
)

pw_add_test(pw_tokenizer.base64_test
  SOURCES
    base64_test.cc
  PRIVATE_DEPS
    pw_tokenizer.base64
  GROUPS
    modules
    pw_tokenizer
)

pw_add_test(pw_tokenizer.decode_test
  SOURCES
    decode_test.cc
  PRIVATE_DEPS
    pw_varint
    pw_tokenizer.decoder
  GROUPS
    modules
    pw_tokenizer
)

pw_add_test(pw_tokenizer.detokenize_test
  SOURCES
    detokenize_test.cc
  PRIVATE_DEPS
    pw_tokenizer.decoder
  GROUPS
    modules
    pw_tokenizer
)

if((NOT "${pw_tokenizer.global_handler_with_payload_BACKEND}" STREQUAL "") AND
   (NOT "${pw_tokenizer.global_handler_BACKEND}" STREQUAL ""))
  pw_add_test(pw_tokenizer.global_handlers_test
    SOURCES
      global_handlers_test_c.c
      global_handlers_test.cc
    PRIVATE_DEPS
      pw_tokenizer.global_handler
      pw_tokenizer.global_handler_with_payload
    GROUPS
      modules
      pw_tokenizer
  )
endif()

pw_add_test(pw_tokenizer.hash_test
  SOURCES
    hash_test.cc
  PRIVATE_DEPS
    pw_tokenizer
  GROUPS
    modules
    pw_tokenizer
)

pw_add_test(pw_tokenizer.token_database_test
  SOURCES
    token_database_test.cc
  PRIVATE_DEPS
    pw_tokenizer.decoder
  GROUPS
    modules
    pw_tokenizer
)

pw_add_test(pw_tokenizer.tokenize_test
  SOURCES
    tokenize_test_c.c
    tokenize_test.cc
  PRIVATE_DEPS
    pw_varint
    pw_tokenizer
  GROUPS
    modules
    pw_tokenizer
)

if(Zephyr_FOUND)
  zephyr_link_libraries_ifdef(CONFIG_PIGWEED_TOKENIZER pw_tokenizer)
  zephyr_link_libraries_ifdef(CONFIG_PIGWEED_TOKENIZER_BASE64 pw_tokenizer.base64)
  zephyr_link_libraries_ifdef(CONFIG_PIGWEED_DETOKENIZER pw_tokenizer.decoder)
endif()
