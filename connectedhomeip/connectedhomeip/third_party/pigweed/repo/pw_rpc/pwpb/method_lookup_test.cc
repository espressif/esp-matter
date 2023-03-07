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
#include "pw_rpc/pwpb/test_method_context.h"
#include "pw_rpc/raw/test_method_context.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"

namespace pw::rpc {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

class MixedService1
    : public test::pw_rpc::pwpb::TestService::Service<MixedService1> {
 public:
  void TestUnaryRpc(ConstByteSpan, RawUnaryResponder& responder) {
    std::byte response[5] = {};
    ASSERT_EQ(OkStatus(), responder.Finish(response, OkStatus()));
  }

  void TestAnotherUnaryRpc(const TestRequest::Message&,
                           PwpbUnaryResponder<TestResponse::Message>&) {
    called_async_unary_method = true;
  }

  void TestServerStreamRpc(const TestRequest::Message&,
                           ServerWriter<TestStreamResponse::Message>&) {
    called_server_streaming_method = true;
  }

  void TestClientStreamRpc(RawServerReader&) {
    called_client_streaming_method = true;
  }

  void TestBidirectionalStreamRpc(
      ServerReaderWriter<TestRequest::Message, TestStreamResponse::Message>&) {
    called_bidirectional_streaming_method = true;
  }

  bool called_async_unary_method = false;
  bool called_server_streaming_method = false;
  bool called_client_streaming_method = false;
  bool called_bidirectional_streaming_method = false;
};

class MixedService2
    : public test::pw_rpc::pwpb::TestService::Service<MixedService2> {
 public:
  Status TestUnaryRpc(const TestRequest::Message&, TestResponse::Message&) {
    return Status::Unauthenticated();
  }

  void TestAnotherUnaryRpc(ConstByteSpan, RawUnaryResponder&) {
    called_async_unary_method = true;
  }

  void TestServerStreamRpc(ConstByteSpan, RawServerWriter&) {
    called_server_streaming_method = true;
  }

  void TestClientStreamRpc(
      ServerReader<TestRequest::Message, TestStreamResponse::Message>&) {
    called_client_streaming_method = true;
  }

  void TestBidirectionalStreamRpc(RawServerReaderWriter&) {
    called_bidirectional_streaming_method = true;
  }

  bool called_async_unary_method = false;
  bool called_server_streaming_method = false;
  bool called_client_streaming_method = false;
  bool called_bidirectional_streaming_method = false;
};

TEST(MixedService1, CallRawMethod_SyncUnary) {
  PW_RAW_TEST_METHOD_CONTEXT(MixedService1, TestUnaryRpc) context;
  context.call({});
  EXPECT_EQ(OkStatus(), context.status());
  EXPECT_EQ(5u, context.response().size());
}

TEST(MixedService1, CallPwpbMethod_AsyncUnary) {
  PW_PWPB_TEST_METHOD_CONTEXT(MixedService1, TestAnotherUnaryRpc) context;
  ASSERT_FALSE(context.service().called_async_unary_method);
  context.call({});
  EXPECT_TRUE(context.service().called_async_unary_method);
}

TEST(MixedService1, CallPwpbMethod_ServerStreaming) {
  PW_PWPB_TEST_METHOD_CONTEXT(MixedService1, TestServerStreamRpc) context;
  ASSERT_FALSE(context.service().called_server_streaming_method);
  context.call({});
  EXPECT_TRUE(context.service().called_server_streaming_method);
}

TEST(MixedService1, CallRawMethod_ClientStreaming) {
  PW_RAW_TEST_METHOD_CONTEXT(MixedService1, TestClientStreamRpc) context;
  ASSERT_FALSE(context.service().called_client_streaming_method);
  context.call();
  EXPECT_TRUE(context.service().called_client_streaming_method);
}

TEST(MixedService1, CallPwpbMethod_BidirectionalStreaming) {
  PW_PWPB_TEST_METHOD_CONTEXT(MixedService1, TestBidirectionalStreamRpc)
  context;
  ASSERT_FALSE(context.service().called_bidirectional_streaming_method);
  context.call();
  EXPECT_TRUE(context.service().called_bidirectional_streaming_method);
}

TEST(MixedService2, CallPwpbMethod_SyncUnary) {
  PW_PWPB_TEST_METHOD_CONTEXT(MixedService2, TestUnaryRpc) context;
  Status status = context.call({});
  EXPECT_EQ(Status::Unauthenticated(), status);
}

TEST(MixedService2, CallRawMethod_AsyncUnary) {
  PW_RAW_TEST_METHOD_CONTEXT(MixedService2, TestAnotherUnaryRpc) context;
  ASSERT_FALSE(context.service().called_async_unary_method);
  context.call({});
  EXPECT_TRUE(context.service().called_async_unary_method);
}

TEST(MixedService2, CallRawMethod_ServerStreaming) {
  PW_RAW_TEST_METHOD_CONTEXT(MixedService2, TestServerStreamRpc) context;
  ASSERT_FALSE(context.service().called_server_streaming_method);
  context.call({});
  EXPECT_TRUE(context.service().called_server_streaming_method);
}

TEST(MixedService2, CallPwpbMethod_ClientStreaming) {
  PW_PWPB_TEST_METHOD_CONTEXT(MixedService2, TestClientStreamRpc) context;
  ASSERT_FALSE(context.service().called_client_streaming_method);
  context.call();
  EXPECT_TRUE(context.service().called_client_streaming_method);
}

TEST(MixedService2, CallRawMethod_BidirectionalStreaming) {
  PW_RAW_TEST_METHOD_CONTEXT(MixedService2, TestBidirectionalStreamRpc) context;
  ASSERT_FALSE(context.service().called_bidirectional_streaming_method);
  context.call();
  EXPECT_TRUE(context.service().called_bidirectional_streaming_method);
}

}  // namespace
}  // namespace pw::rpc
