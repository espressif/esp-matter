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
#include "pw_protobuf/serialized_size.h"

#include <cinttypes>

#include "gtest/gtest.h"

namespace pw::protobuf {
namespace {

#define TEST_VARINT(type)                                           \
  TEST(SerializedSize, type) {                                      \
    static_assert(SizeOfField##type(1, 0) == 1 + 1,                 \
                  #type " minimum encoded size, key 1");            \
    static_assert(SizeOfField##type(1) == 1 + kMaxSizeBytes##type,  \
                  #type " maximum encoded size, key 1");            \
    static_assert(SizeOfField##type(16, 0) == 2 + 1,                \
                  #type " minimum encoded size, key 16");           \
    static_assert(SizeOfField##type(16) == 2 + kMaxSizeBytes##type, \
                  #type " maximum encoded size, key 16");           \
  }                                                                 \
  static_assert(true, "require semicolons")

#define TEST_FIXED(type)                                            \
  TEST(SerializedSize, SizeOf##type##Field) {                       \
    static_assert(SizeOfField##type(1) == 1 + kMaxSizeBytes##type,  \
                  #type " key 1");                                  \
    static_assert(SizeOfField##type(15) == 1 + kMaxSizeBytes##type, \
                  #type " key 15");                                 \
    static_assert(SizeOfField##type(16) == 2 + kMaxSizeBytes##type, \
                  #type " key 16");                                 \
    static_assert(SizeOfField##type(17) == 2 + kMaxSizeBytes##type, \
                  #type " key 17");                                 \
  }                                                                 \
  static_assert(true, "require semicolons")

#define TEST_DELIMITED(function)                                            \
  TEST(SerializedSize, function) {                                          \
    static_assert(function(1, 0) == 1 + 1 + 0, #function " key 1");         \
    static_assert(function(1, 1) == 1 + 1 + 1, #function " key 1");         \
    static_assert(function(1, 128) == 1 + 2 + 128, #function " key 1");     \
    static_assert(function(1, 1000) == 1 + 2 + 1000, #function " key 1");   \
    static_assert(function(16, 0) == 2 + 1 + 0, #function " key 16");       \
    static_assert(function(16, 1) == 2 + 1 + 1, #function " key 16");       \
    static_assert(function(16, 128) == 2 + 2 + 128, #function " key 16");   \
    static_assert(function(16, 1000) == 2 + 2 + 1000, #function " key 16"); \
  }                                                                         \
  static_assert(true, "require semicolons")

TEST(SerializedSize, SizeOfVarintField) {
  static_assert(SizeOfVarintField(1, 0) == 1 + 1);
  static_assert(SizeOfVarintField(1, 127) == 1 + 1);

  static_assert(SizeOfVarintField(1, INT32_C(-1)) == 1 + 10);
  static_assert(SizeOfVarintField(1, INT64_C(-1)) == 1 + 10);

  static_assert(SizeOfVarintField(16, 0) == 2 + 1);
  static_assert(SizeOfVarintField(16, 127) == 2 + 1);

  static_assert(SizeOfVarintField(16, INT32_C(-1)) == 2 + 10);
  static_assert(SizeOfVarintField(16, INT64_C(-1)) == 2 + 10);
}

TEST(SerializedSize, SizeOfDelimitedFieldWithoutValue) {
  static_assert(SizeOfDelimitedFieldWithoutValue(1, 0) == 1 + 1);
  static_assert(SizeOfDelimitedFieldWithoutValue(1, 1) == 1 + 1);
  static_assert(SizeOfDelimitedFieldWithoutValue(1, 128) == 1 + 2);
  static_assert(SizeOfDelimitedFieldWithoutValue(1, 1000) == 1 + 2);
  static_assert(SizeOfDelimitedFieldWithoutValue(1) == 1 + 5);
  static_assert(SizeOfDelimitedFieldWithoutValue(16, 0) == 2 + 1);
  static_assert(SizeOfDelimitedFieldWithoutValue(16, 1) == 2 + 1);
  static_assert(SizeOfDelimitedFieldWithoutValue(16, 128) == 2 + 2);
  static_assert(SizeOfDelimitedFieldWithoutValue(16) == 2 + 5);
}

TEST_DELIMITED(SizeOfDelimitedField);

TEST_FIXED(Float);
TEST_FIXED(Double);

TEST_VARINT(Int32);
TEST_VARINT(Int64);
TEST_VARINT(Sint32);
TEST_VARINT(Sint64);
TEST_VARINT(Uint32);
TEST_VARINT(Uint64);

TEST_FIXED(Fixed32);
TEST_FIXED(Fixed64);
TEST_FIXED(Sfixed32);
TEST_FIXED(Sfixed64);
TEST_FIXED(Bool);

TEST_DELIMITED(SizeOfFieldString);
TEST_DELIMITED(SizeOfFieldBytes);

TEST_VARINT(Enum);

}  // namespace
}  // namespace pw::protobuf
