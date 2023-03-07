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

namespace {

// Encodable values. The fuzzer will iteratively choose different field types to
// generate and encode.
enum FieldType : uint8_t {
  kUint32 = 0,
  kPackedUint32,
  kUint64,
  kPackedUint64,
  kInt32,
  kPackedInt32,
  kInt64,
  kPackedInt64,
  kSint32,
  kPackedSint32,
  kSint64,
  kPackedSint64,
  kBool,
  kFixed32,
  kPackedFixed32,
  kFixed64,
  kPackedFixed64,
  kSfixed32,
  kPackedSfixed32,
  kSfixed64,
  kPackedSfixed64,
  kFloat,
  kPackedFloat,
  kDouble,
  kPackedDouble,
  kBytes,
  kString,
  kPush,
  kMaxValue = kPush,
};

}  // namespace
