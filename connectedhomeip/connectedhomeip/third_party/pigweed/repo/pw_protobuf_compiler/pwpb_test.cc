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

#include "gtest/gtest.h"
#include "pw_protobuf_compiler_pwpb_protos/pwpb_test.pwpb.h"
#include "pw_string/string.h"

namespace pw::protobuf_compiler {
namespace {

TEST(Pwpb, CompilesProtobufs) {
  pwpb::Point::Message point = {4, 8, "point"};
  EXPECT_EQ(point.x, 4u);
  EXPECT_EQ(point.y, 8u);
  EXPECT_EQ(point.name.size(), 5u);
  EXPECT_EQ(point.name, "point");
}

TEST(Pwpb, OptionsFilesAreApplied) {
  pwpb::OptionsFileExample::Message string_options_comparison;

  static_assert(
      std::is_same_v<decltype(string_options_comparison.thirty_two_chars),
                     pw::InlineString<32>>,
      "Field `thirty_two_chars` should be a `pw::InlineString<32>`.");

  static_assert(
      std::is_same_v<decltype(string_options_comparison.forty_two_chars),
                     pw::InlineString<42>>,
      "Field `forty_two_chars` should be a `pw::InlineString<42>`.");

  static_assert(
      std::is_same_v<
          decltype(string_options_comparison.unspecified_length),
          pw::protobuf::Callback<pwpb::OptionsFileExample::StreamEncoder,
                                 pwpb::OptionsFileExample::StreamDecoder>>,
      "The field `unspecified_length` should be a `pw::protobuf::Callback`.");
}

}  // namespace
}  // namespace pw::protobuf_compiler
