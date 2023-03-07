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
//

#include "pw_rpc/pwpb/internal/method_union.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/service.h"
#include "pw_rpc_pwpb_private/internal_test_utils.h"
#include "pw_rpc_test_protos/test.pwpb.h"

namespace pw::rpc::internal {
namespace {

template <typename Implementation>
class FakeGeneratedService : public Service {
 public:
  constexpr FakeGeneratedService(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<PwpbMethodUnion, 4> kMethods = {
      GetPwpbOrRawMethodFor<&Implementation::DoNothing,
                            MethodType::kUnary,
                            pw::rpc::test::pwpb::Empty::Message,
                            pw::rpc::test::pwpb::Empty::Message>(
          10u,
          kPwpbMethodSerde<&pw::rpc::test::pwpb::Empty::kMessageFields,
                           &pw::rpc::test::pwpb::Empty::kMessageFields>),
      GetPwpbOrRawMethodFor<&Implementation::RawStream,
                            MethodType::kServerStreaming,
                            pw::rpc::test::pwpb::TestRequest::Message,
                            pw::rpc::test::pwpb::TestResponse::Message>(
          11u,
          kPwpbMethodSerde<&pw::rpc::test::pwpb::TestRequest::kMessageFields,
                           &pw::rpc::test::pwpb::TestResponse::kMessageFields>),
      GetPwpbOrRawMethodFor<&Implementation::AddFive,
                            MethodType::kUnary,
                            pw::rpc::test::pwpb::TestRequest::Message,
                            pw::rpc::test::pwpb::TestResponse::Message>(
          12u,
          kPwpbMethodSerde<&pw::rpc::test::pwpb::TestRequest::kMessageFields,
                           &pw::rpc::test::pwpb::TestResponse::kMessageFields>),
      GetPwpbOrRawMethodFor<&Implementation::StartStream,
                            MethodType::kServerStreaming,
                            pw::rpc::test::pwpb::TestRequest::Message,
                            pw::rpc::test::pwpb::TestResponse::Message>(
          13u,
          kPwpbMethodSerde<&pw::rpc::test::pwpb::TestRequest::kMessageFields,
                           &pw::rpc::test::pwpb::TestResponse::kMessageFields>),
  };
};

class FakeGeneratedServiceImpl
    : public FakeGeneratedService<FakeGeneratedServiceImpl> {
 public:
  FakeGeneratedServiceImpl(uint32_t id) : FakeGeneratedService(id) {}

  Status AddFive(const pw::rpc::test::pwpb::TestRequest::Message& request,
                 pw::rpc::test::pwpb::TestResponse::Message& response) {
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

  void StartStream(
      const pw::rpc::test::pwpb::TestRequest::Message& request,
      PwpbServerWriter<pw::rpc::test::pwpb::TestResponse::Message>& writer) {
    last_request = request;
    last_writer = std::move(writer);
  }

  pw::rpc::test::pwpb::TestRequest::Message last_request;
  PwpbServerWriter<pw::rpc::test::pwpb::TestResponse::Message> last_writer;
  RawServerWriter last_raw_writer;
};

TEST(PwpbMethodUnion, Raw_CallsUnaryMethod) {
  const Method& method =
      std::get<0>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);
  rpc_lock().lock();
  method.Invoke(context.get(), context.request({}));

  const Packet& response = context.output().last_packet();
  EXPECT_EQ(response.status(), Status::Unknown());
}

TEST(PwpbMethodUnion, Raw_CallsServerStreamingMethod) {
  PW_ENCODE_PB(pw::rpc::test::pwpb::TestRequest,
               request,
               .integer = 555,
               .status_code = 0);

  const Method& method =
      std::get<1>(FakeGeneratedServiceImpl::kMethods).method();
  ServerContextForTest<FakeGeneratedServiceImpl> context(method);

  rpc_lock().lock();
  method.Invoke(context.get(), context.request(request));

  EXPECT_TRUE(context.service().last_raw_writer.active());
  EXPECT_EQ(OkStatus(), context.service().last_raw_writer.Finish());
  EXPECT_EQ(context.output().last_packet().type(), pwpb::PacketType::RESPONSE);
}

TEST(PwpbMethodUnion, Pwpb_CallsUnaryMethod) {
  PW_ENCODE_PB(pw::rpc::test::pwpb::TestRequest,
               request,
               .integer = 123,
               .status_code = 3);

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

TEST(PwpbMethodUnion, Pwpb_CallsServerStreamingMethod) {
  PW_ENCODE_PB(pw::rpc::test::pwpb::TestRequest,
               request,
               .integer = 555,
               .status_code = 0);

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
