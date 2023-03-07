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

#include "pw_rpc/internal/method_info.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/method_info_tester.h"
#include "pw_rpc_test_protos/test.raw_rpc.pb.h"

namespace pw::rpc::internal {
namespace {

class TestService final
    : public pw::rpc::test::pw_rpc::raw::TestService::Service<TestService> {
 public:
  static StatusWithSize TestUnaryRpc(ConstByteSpan, ByteSpan) {
    return StatusWithSize(0);
  }

  void TestAnotherUnaryRpc(ConstByteSpan, RawUnaryResponder&) {}

  void TestServerStreamRpc(ConstByteSpan, RawServerWriter&) {}

  void TestClientStreamRpc(RawServerReader&) {}

  void TestBidirectionalStreamRpc(RawServerReaderWriter&) {}
};

static_assert(
    MethodInfoTests<pw::rpc::test::pw_rpc::raw::TestService, TestService>()
        .Pass());

}  // namespace
}  // namespace pw::rpc::internal
