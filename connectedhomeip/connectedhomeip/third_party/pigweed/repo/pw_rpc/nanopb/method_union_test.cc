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
//

#include "pw_rpc/nanopb/internal/method_union.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc_nanopb_private/internal_test_utils.h"
#include "pw_rpc_test_protos/test.pb.h"

namespace pw::rpc::internal {
namespace {

using std::byte;

template <typename Implementation>
class FakeGeneratedService : public Service {
 public:
  constexpr FakeGeneratedService(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<NanopbMethodUnion, 4> kMethods = {
      GetNanopbOrRawMethodFor<&Implementation::DoNothing,
                              MethodType::kUnary,
                              pw_rpc_test_Empty,
                              pw_rpc_test_Empty>(
          10u,
          kNanopbMethodSerde<pw_rpc_test_Empty_fields,
                             pw_rpc_test_Empty_fields>),
      GetNanopbOrRawMethodFor<&Implementation::RawStream,
                              MethodType::kServerStreaming,
                              pw_rpc_test_TestRequest,
                              pw_rpc_test_TestResponse>(
          11u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
      GetNanopbOrRawMethodFor<&Implementation::AddFive,
                              MethodType::kUnary,
                              pw_rpc_test_TestRequest,
                              pw_rpc_test_TestResponse>(
          12u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
      GetNanopbOrRawMethodFor<&Implementation::StartStream,
                              MethodType::kServerStreaming,
                              pw_rpc_test_TestRequest,
                              pw_rpc_test_TestResponse>(
          13u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
  };
};

class FakeGeneratedServiceImpl
    : public FakeGeneratedService<FakeGeneratedServiceImpl> {
 public:
  FakeGeneratedServiceImpl(uint32_t id) : FakeGeneratedService(id) {}

  Status AddFive(const pw_rpc_test_TestRequest& request,
                 pw_rpc_test_TestResponse& response) {
    last_request = request;
    response.value = request.integer + 5;
    return Status::Unauthenticated();
  }

  void DoNothing(ConstByteSpan, RawUnaryResponder& responder) {
    ASSERT_EQ(OkStatus(), responder.Finish({}, Status::Unknown()));
  }

  void RawStream(ConstByteSpan, RawServerWriter& writer) {
    last_raw_writer = std::move(writer);
  }

  void StartStream(const pw_rpc_test_TestRequest& request,
                   NanopbServerWriter<pw_rpc_test_TestResponse>& writer) {
    last_request = request;
    last_writer = std::move(writer);
  }

  pw_rpc_test_TestRequest last_request;
  NanopbServerWriter<pw_rpc_test_TestResponse> last_writer;
  RawServerWriter last_raw_writer;
};

TEST(NanopbMethodUnion, Raw_CallsUnaryMethod) {
  const Method& method =
      std::get<0>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);
  rpc_lock().lock();
  method.Invoke(context.get(), context.request({}));

  const Packet& response = context.output().last_packet();
  EXPECT_EQ(response.status(), Status::Unknown());
}

TEST(NanopbMethodUnion, Raw_CallsServerStreamingMethod) {
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 555, .status_code = 0);

  const Method& method =
      std::get<1>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);

  rpc_lock().lock();
  method.Invoke(context.get(), context.request(request));

  EXPECT_TRUE(context.service().last_raw_writer.active());
  EXPECT_EQ(OkStatus(), context.service().last_raw_writer.Finish());
  EXPECT_EQ(context.output().last_packet().type(), pwpb::PacketType::RESPONSE);
}

TEST(NanopbMethodUnion, Nanopb_CallsUnaryMethod) {
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 123, .status_code = 3);

  const Method& method =
      std::get<2>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);
  rpc_lock().lock();
  method.Invoke(context.get(), context.request(request));

  const Packet& response = context.output().last_packet();
  EXPECT_EQ(response.status(), Status::Unauthenticated());

  // Field 1 (encoded as 1 << 3) with 128 as the value.
  constexpr std::byte expected[]{
      std::byte{0x08}, std::byte{0x80}, std::byte{0x01}};

  EXPECT_EQ(sizeof(expected), response.payload().size());
  EXPECT_EQ(0,
            std::memcmp(expected, response.payload().data(), sizeof(expected)));

  EXPECT_EQ(123, context.service().last_request.integer);
  EXPECT_EQ(3u, context.service().last_request.status_code);
}

TEST(NanopbMethodUnion, Nanopb_CallsServerStreamingMethod) {
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 555, .status_code = 0);

  const Method& method =
      std::get<3>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);

  rpc_lock().lock();
  method.Invoke(context.get(), context.request(request));

  EXPECT_EQ(555, context.service().last_request.integer);
  EXPECT_TRUE(context.service().last_writer.active());

  EXPECT_EQ(OkStatus(), context.service().last_writer.Finish());
  EXPECT_EQ(context.output().last_packet().type(), pwpb::PacketType::RESPONSE);
}

}  // namespace
}  // namespace pw::rpc::internal
