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
// different internal representations of service and method identifiers.

namespace pw::rpc {

class ServiceId;

namespace internal {
constexpr ServiceId WrapServiceId(uint32_t id);
constexpr uint32_t UnwrapServiceId(ServiceId id);
}  // namespace internal

// An identifier for a service.
//
// Note: this does not identify instances of a service (Servers), only the
// service itself.
class ServiceId {
 private:
  constexpr explicit ServiceId(uint32_t id) : id_(id) {}
  friend constexpr ServiceId internal::WrapServiceId(uint32_t id);
  friend constexpr uint32_t internal::UnwrapServiceId(ServiceId id);
  uint32_t id_;
};

constexpr bool operator==(ServiceId lhs, ServiceId rhs) {
  return internal::UnwrapServiceId(lhs) == internal::UnwrapServiceId(rhs);
}

constexpr bool operator!=(ServiceId lhs, ServiceId rhs) {
  return !(lhs == rhs);
}

// Comparisons are provided to enable sorting by `ServiceId`.

constexpr bool operator<(ServiceId lhs, ServiceId rhs) {
  return internal::UnwrapServiceId(lhs) < internal::UnwrapServiceId(rhs);
}

constexpr bool operator>(ServiceId lhs, ServiceId rhs) { return rhs < lhs; }

constexpr bool operator<=(ServiceId lhs, ServiceId rhs) { return !(lhs > rhs); }

constexpr bool operator>=(ServiceId lhs, ServiceId rhs) { return !(lhs < rhs); }

namespace internal {

constexpr ServiceId WrapServiceId(uint32_t id) { return ServiceId(id); }
constexpr uint32_t UnwrapServiceId(ServiceId id) { return id.id_; }

}  // namespace internal
}  // namespace pw::rpc

namespace std {

template <>
struct hash<pw::rpc::ServiceId> {
  size_t operator()(const pw::rpc::ServiceId& id) const {
    return hash<uint32_t>{}(::pw::rpc::internal::UnwrapServiceId(id));
  }
};

}  // namespace std
