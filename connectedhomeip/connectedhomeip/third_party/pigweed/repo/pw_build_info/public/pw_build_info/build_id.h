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

#include <cstddef>

#include "pw_span/span.h"

namespace pw::build_info {

// Build IDs may be generated with several different algorithms. The largest of
// these (aside from user-provided build IDs) are a fixed size of 20 bytes.
inline constexpr size_t kMaxBuildIdSizeBytes = 20;

// Reads a GNU build ID from the address starting at the address of the
// `gnu_build_id_begin` symbol. This must be manually explicitly provided as
// part of a linker script. See build_id_linker_snippet.ld for an example.
span<const std::byte> BuildId();

}  // namespace pw::build_info
