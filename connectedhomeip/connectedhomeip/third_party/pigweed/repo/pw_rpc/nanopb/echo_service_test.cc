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

#include "gtest/gtest.h"
#include "pw_rpc/echo_service_nanopb.h"
#include "pw_rpc/nanopb/test_method_context.h"

namespace pw::rpc {
namespace {

TEST(EchoService, Echo_EchoesRequestMessage) {
  PW_NANOPB_TEST_METHOD_CONTEXT(EchoService, Echo) context;
  ASSERT_EQ(context.call(_pw_rpc_EchoMessage{"Hello, world"}), OkStatus());
  EXPECT_STREQ(context.response().msg, "Hello, world");
}

TEST(EchoService, Echo_EmptyRequest) {
  PW_NANOPB_TEST_METHOD_CONTEXT(EchoService, Echo) context;
  ASSERT_EQ(context.call({.msg = {}}), OkStatus());
  EXPECT_STREQ(context.response().msg, "");
}

}  // namespace
}  // namespace pw::rpc
