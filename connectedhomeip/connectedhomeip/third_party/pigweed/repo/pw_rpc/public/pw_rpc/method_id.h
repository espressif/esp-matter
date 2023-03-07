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
#include <functional>

// NOTE: These wrappers exist in order to provide future compatibility for
// different internal representations of method identifiers.

namespace pw::rpc {

class MethodId;

namespace internal {
constexpr MethodId WrapMethodId(uint32_t id);
constexpr uint32_t UnwrapMethodId(MethodId id);
}  // namespace internal

// An identifier for a method.
class MethodId {
 private:
  constexpr explicit MethodId(uint32_t id) : id_(id) {}
  friend constexpr MethodId internal::WrapMethodId(uint32_t id);
  friend constexpr uint32_t internal::UnwrapMethodId(MethodId id);
  uint32_t id_;
};

constexpr bool operator==(MethodId lhs, MethodId rhs) {
  return internal::UnwrapMethodId(lhs) == internal::UnwrapMethodId(rhs);
}

constexpr bool operator!=(MethodId lhs, MethodId rhs) { return !(lhs == rhs); }

// Comparisons are provided to enable sorting by `MethodId`.

constexpr bool operator<(MethodId lhs, MethodId rhs) {
  return internal::UnwrapMethodId(lhs) < internal::UnwrapMethodId(rhs);
}

constexpr bool operator>(MethodId lhs, MethodId rhs) { return rhs < lhs; }

constexpr bool operator<=(MethodId lhs, MethodId rhs) { return !(lhs > rhs); }

constexpr bool operator>=(MethodId lhs, MethodId rhs) { return !(lhs < rhs); }

namespace internal {

constexpr MethodId WrapMethodId(uint32_t id) { return MethodId(id); }
constexpr uint32_t UnwrapMethodId(MethodId id) { return id.id_; }

}  // namespace internal
}  // namespace pw::rpc

namespace std {

template <>
struct hash<pw::rpc::MethodId> {
  size_t operator()(const pw::rpc::MethodId& id) const {
    return hash<uint32_t>{}(::pw::rpc::internal::UnwrapMethodId(id));
  }
};

}  // namespace std
