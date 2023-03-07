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

#include <string_view>

#include "gtest/gtest.h"
#include "pw_rpc/echo_service_pwpb.h"
#include "pw_rpc/pwpb/test_method_context.h"

namespace pw::rpc {
namespace {

TEST(EchoService, Echo_EchoesRequestMessage) {
  PW_PWPB_TEST_METHOD_CONTEXT(EchoService, Echo) context;
  ASSERT_EQ(context.call({"Hello, world"}), OkStatus());
  EXPECT_EQ(std::string_view(context.response().msg.data(),
                             context.response().msg.size()),
            "Hello, world");
}

TEST(EchoService, Echo_EmptyRequest) {
  PW_PWPB_TEST_METHOD_CONTEXT(EchoService, Echo) context;
  ASSERT_EQ(context.call({}), OkStatus());
  EXPECT_EQ(std::string_view(context.response().msg.data(),
                             context.response().msg.size()),
            "");
}

}  // namespace
}  // namespace pw::rpc
