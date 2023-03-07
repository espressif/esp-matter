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

#include "pw_protobuf/find.h"

#include "gtest/gtest.h"

namespace pw::protobuf {
namespace {

// clang-format off
constexpr uint8_t encoded_proto[] = {
  // type=int32, k=1, v=42
  0x08, 0x2a,
  // type=sint32, k=2, v=-13
  0x10, 0x19,
  // type=bool, k=3, v=false
  0x18, 0x00,
  // type=double, k=4, v=3.14159
  0x21, 0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40,
  // type=fixed32, k=5, v=0xdeadbeef
  0x2d, 0xef, 0xbe, 0xad, 0xde,
  // type=string, k=6, v="Hello world"
  0x32, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',

  // type=message, k=7, len=2
  0x3a, 0x02,
  // (nested) type=uint32, k=1, v=3
  0x08, 0x03
};

TEST(FindDecodeHandler, SingleLevel_FindsExistingField) {
  CallbackDecoder decoder;
  FindDecodeHandler finder(3);

  decoder.set_handler(&finder);
  ASSERT_EQ(Status::Cancelled(), decoder.Decode(as_bytes(span(encoded_proto))));

  EXPECT_TRUE(finder.found());
  EXPECT_TRUE(decoder.cancelled());
}

TEST(FindDecodeHandler, SingleLevel_DoesntFindNonExistingField) {
  CallbackDecoder decoder;
  FindDecodeHandler finder(8);

  decoder.set_handler(&finder);
  ASSERT_EQ(OkStatus(), decoder.Decode(as_bytes(span(encoded_proto))));

  EXPECT_FALSE(finder.found());
  EXPECT_FALSE(decoder.cancelled());
}

TEST(FindDecodeHandler, MultiLevel_FindsExistingNestedField) {
  CallbackDecoder decoder;
  FindDecodeHandler nested_finder(1);
  FindDecodeHandler finder(7, &nested_finder);

  decoder.set_handler(&finder);
  ASSERT_EQ(Status::Cancelled(), decoder.Decode(as_bytes(span(encoded_proto))));

  EXPECT_TRUE(finder.found());
  EXPECT_TRUE(nested_finder.found());
  EXPECT_TRUE(decoder.cancelled());
}

TEST(FindDecodeHandler, MultiLevel_DoesntFindNonExistingNestedField) {
  CallbackDecoder decoder;
  FindDecodeHandler nested_finder(3);
  FindDecodeHandler finder(7, &nested_finder);

  decoder.set_handler(&finder);
  ASSERT_EQ(OkStatus(), decoder.Decode(as_bytes(span(encoded_proto))));

  EXPECT_TRUE(finder.found());
  EXPECT_FALSE(nested_finder.found());
  EXPECT_FALSE(decoder.cancelled());
}

}  // namespace
}  // namespace pw::protobuf
