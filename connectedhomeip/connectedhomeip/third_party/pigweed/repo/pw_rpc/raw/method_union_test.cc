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

#include "pw_rpc/raw/internal/method_union.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_protobuf/decoder.h"
#include "pw_protobuf/encoder.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.pwpb.h"

namespace pw::rpc::internal {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;

template <typename Implementation>
class FakeGeneratedService : public Service {
 public:
  constexpr FakeGeneratedService(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<RawMethodUnion, 3> kMethods = {
      GetRawMethodFor<&Implementation::DoNothing, MethodType::kUnary>(10u),
      GetRawMethodFor<&Implementation::AddFive, MethodType::kUnary>(11u),
      GetRawMethodFor<&Implementation::StartStream,
                      MethodType::kServerStreaming>(12u),
  };
};

class FakeGeneratedServiceImpl
    : public FakeGeneratedService<FakeGeneratedServiceImpl> {
 public:
  FakeGeneratedServiceImpl(uint32_t id) : FakeGeneratedService(id) {}

  void DoNothing(ConstByteSpan, RawUnaryResponder&) {}

  void AddFive(ConstByteSpan request, RawUnaryResponder& responder) {
    DecodeRawTestRequest(request);

    std::byte response[32] = {};
    TestResponse::MemoryEncoder test_response(response);
    ASSERT_EQ(OkStatus(), test_response.WriteValue(last_request.integer + 5));

    ASSERT_EQ(OkStatus(),
              responder.Finish(span(response).first(test_response.size()),
                               Status::Unauthenticated()));
  }

  void StartStream(ConstByteSpan request, RawServerWriter& writer) {
    DecodeRawTestRequest(request);
    last_writer = std::move(writer);
  }

  struct {
    int64_t integer;
    uint32_t status_code;
  } last_request;
  RawServerWriter last_writer;

 private:
  void DecodeRawTestRequest(ConstByteSpan request) {
    protobuf::Decoder decoder(request);

    while (decoder.Next().ok()) {
      TestRequest::Fields field =
          static_cast<TestRequest::Fields>(decoder.FieldNumber());

      switch (field) {
        case TestRequest::Fields::INTEGER:
          ASSERT_EQ(OkStatus(), decoder.ReadInt64(&last_request.integer));
          break;
        case TestRequest::Fields::STATUS_CODE:
          ASSERT_EQ(OkStatus(), decoder.ReadUint32(&last_request.status_code));
          break;
      }
    }
  }
};

TEST(RawMethodUnion, InvokesUnary) {
  std::byte buffer[16];

  TestRequest::MemoryEncoder test_request(buffer);
  ASSERT_EQ(OkStatus(), test_request.WriteInteger(456));
  ASSERT_EQ(OkStatus(), test_request.WriteStatusCode(7));

  const Method& method =
      std::get<1>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);
  rpc_lock().lock();
  method.Invoke(context.get(), context.request(test_request));

  EXPECT_EQ(context.service().last_request.integer, 456);
  EXPECT_EQ(context.service().last_request.status_code, 7u);

  const Packet& response = context.output().last_packet();
  EXPECT_EQ(response.status(), Status::Unauthenticated());

  protobuf::Decoder decoder(response.payload());
  ASSERT_TRUE(decoder.Next().ok());
  int64_t value;
  EXPECT_EQ(decoder.ReadInt64(&value), OkStatus());
  EXPECT_EQ(value, 461);
}

TEST(RawMethodUnion, InvokesServerStreaming) {
  std::byte buffer[16];

  TestRequest::MemoryEncoder test_request(buffer);
  ASSERT_EQ(OkStatus(), test_request.WriteInteger(777));
  ASSERT_EQ(OkStatus(), test_request.WriteStatusCode(2));

  const Method& method =
      std::get<2>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);

  rpc_lock().lock();
  method.Invoke(context.get(), context.request(test_request));

  EXPECT_EQ(0u, context.output().total_packets());
  EXPECT_EQ(777, context.service().last_request.integer);
  EXPECT_EQ(2u, context.service().last_request.status_code);
  EXPECT_TRUE(context.service().last_writer.active());
  EXPECT_EQ(OkStatus(), context.service().last_writer.Finish());
}

}  // namespace
}  // namespace pw::rpc::internal
