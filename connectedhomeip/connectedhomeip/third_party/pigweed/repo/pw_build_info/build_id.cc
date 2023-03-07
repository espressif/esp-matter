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

#include "pw_build_info/build_id.h"

#include <cstdint>
#include <cstring>

#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"

extern "C" const uint8_t gnu_build_id_begin;

namespace pw::build_info {
namespace {

PW_PACKED(struct) ElfNoteInfo {
  uint32_t name_size;
  uint32_t descriptor_size;
  uint32_t type;
};

}  // namespace

// Reading more than a uint8_t from gnu_build_id_begin triggers compiler
// warnings that must be silenced.
PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Warray-bounds");
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wstringop-overflow");
#if __GNUC__ >= 11
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wstringop-overread");
#endif

span<const std::byte> BuildId() {
  // Read the sizes at the beginning of the note section.
  ElfNoteInfo build_id_note_sizes;
  memcpy(
      &build_id_note_sizes, &gnu_build_id_begin, sizeof(build_id_note_sizes));
  // Skip the "name" entry of the note section, and return a span to the
  // descriptor.
  return as_bytes(span(&gnu_build_id_begin + sizeof(build_id_note_sizes) +
                           build_id_note_sizes.name_size,
                       build_id_note_sizes.descriptor_size));
}

PW_MODIFY_DIAGNOSTICS_POP();

}  // namespace pw::build_info
