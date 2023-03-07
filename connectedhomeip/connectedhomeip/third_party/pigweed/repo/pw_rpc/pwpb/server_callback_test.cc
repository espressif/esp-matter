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

#include <array>

#include "gtest/gtest.h"
#include "pw_containers/vector.h"
#include "pw_rpc/pwpb/test_method_context.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"

namespace pw::rpc {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

class TestServiceImpl final
    : public test::pw_rpc::pwpb::TestService::Service<TestServiceImpl> {
 public:
  Status TestUnaryRpc(const TestRequest::Message&,
                      TestResponse::Message& response) {
    response.value = 42;
    return OkStatus();
  }

  Status TestAnotherUnaryRpc(const TestRequest::Message&,
                             TestResponse::Message& response) {
    response.value = 42;
    response.repeated_field.SetEncoder(
        [](TestResponse::StreamEncoder& encoder) {
          constexpr std::array<uint32_t, 3> kValues = {7, 8, 9};
          return encoder.WriteRepeatedField(kValues);
        });
    return OkStatus();
  }

  void TestServerStreamRpc(const TestRequest::Message&,
                           PwpbServerWriter<TestStreamResponse::Message>&) {}

  void TestClientStreamRpc(
      PwpbServerReader<TestRequest::Message, TestStreamResponse::Message>&) {}

  void TestBidirectionalStreamRpc(
      PwpbServerReaderWriter<TestRequest::Message,
                             TestStreamResponse::Message>&) {}
};

TEST(PwpbTestMethodContext, ResponseWithoutCallbacks) {
  // Calling response() without an argument returns a Response struct without
  // any callbacks set.
  PW_PWPB_TEST_METHOD_CONTEXT(TestServiceImpl, TestUnaryRpc) ctx;
  ASSERT_EQ(ctx.call({}), OkStatus());

  TestResponse::Message response = ctx.response();
  EXPECT_EQ(42, response.value);
}

TEST(PwpbTestMethodContext, ResponseWithCallbacks) {
  PW_PWPB_TEST_METHOD_CONTEXT(TestServiceImpl, TestAnotherUnaryRpc) ctx;
  ASSERT_EQ(ctx.call({}), OkStatus());

  // To decode a response object that requires to set callbacks, pass it to the
  // response() method as a parameter.
  pw::Vector<uint32_t, 4> values{};

  TestResponse::Message response{};
  response.repeated_field.SetDecoder(
      [&values](TestResponse::StreamDecoder& decoder) {
        return decoder.ReadRepeatedField(values);
      });
  ctx.response(response);

  EXPECT_EQ(42, response.value);

  EXPECT_EQ(3u, values.size());
  EXPECT_EQ(7u, values[0]);
  EXPECT_EQ(8u, values[1]);
  EXPECT_EQ(9u, values[2]);
}

}  // namespace
}  // namespace pw::rpc
