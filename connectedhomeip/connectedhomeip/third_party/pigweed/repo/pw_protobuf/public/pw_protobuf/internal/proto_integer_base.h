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
//
// The header provides a set of helper utils for protobuf related operations.
// The APIs may not be finalized yet.

#pragma once

#include "pw_result/result.h"

namespace pw::protobuf::internal {

// A base class for representing parsed proto integer types or an error code
// to indicate parsing failure.
template <typename Integer>
class ProtoIntegerBase {
 public:
  constexpr ProtoIntegerBase(Result<Integer> value) : value_(value) {}
  constexpr ProtoIntegerBase(Status status) : value_(status) {}

  bool ok() { return value_.ok(); }
  Status status() { return value_.status(); }
  Integer value() { return value_.value(); }

 private:
  Result<Integer> value_ = 0;
};

}  // namespace pw::protobuf::internal
