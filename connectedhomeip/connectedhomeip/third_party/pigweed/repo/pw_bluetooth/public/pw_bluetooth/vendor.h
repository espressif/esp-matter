// Copyright 2022 The Pigweed Authors
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

#include <cstdint>
#include <type_traits>
#include <variant>

namespace pw::bluetooth {

// The maximum buffer length an encoded command might require.
// Update when adding new commands that might require a larger buffer.
constexpr uint16_t kMaxVendorCommandBufferSize = 16;

enum class AclPriority : uint8_t {
  kNormal = 0,
  kSource = 1,
  kSink = 2,
};

struct SetAclPriorityCommandParameters {
  uint16_t connection_handle;
  AclPriority priority;
};

using VendorCommandParameters = std::variant<SetAclPriorityCommandParameters>;

}  // namespace pw::bluetooth
