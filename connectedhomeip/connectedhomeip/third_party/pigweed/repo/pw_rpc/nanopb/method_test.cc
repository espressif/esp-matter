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

#include "pw_rpc/nanopb/internal/method.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_containers/algorithm.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method_impl_tester.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/nanopb/internal/method_union.h"
#include "pw_rpc/service.h"
#include "pw_rpc_nanopb_private/internal_test_utils.h"
#include "pw_rpc_test_protos/test.pb.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wmissing-field-initializers");

namespace pw::rpc::internal {
namespace {

using std::byte;

struct FakePb {};

// Create a fake service for use with the MethodImplTester.
class TestNanopbService final : public Service {
 public:
  // Unary signatures

  Status Unary(const FakePb&, FakePb&) { return Status(); }

  static Status StaticUnary(const FakePb&, FakePb&) { return Status(); }

  void AsyncUnary(const FakePb&, NanopbUnaryResponder<FakePb>&) {}

  static void StaticAsyncUnary(const FakePb&, NanopbUnaryResponder<FakePb>&) {}

  Status UnaryWrongArg(FakePb&, FakePb&) { return Status(); }

  static void StaticUnaryVoidReturn(const FakePb&, FakePb&) {}

  // Server streaming signatures

  void ServerStreaming(const FakePb&, NanopbServerWriter<FakePb>&) {}

  static void StaticServerStreaming(const FakePb&,
                                    NanopbServerWriter<FakePb>&) {}

  int ServerStreamingBadReturn(const FakePb&, NanopbServerWriter<FakePb>&) {
    return 5;
  }

  static void StaticServerStreamingMissingArg(NanopbServerWriter<FakePb>&) {}

  // Client streaming signatures

  void ClientStreaming(NanopbServerReader<FakePb, FakePb>&) {}

  static void StaticClientStreaming(NanopbServerReader<FakePb, FakePb>&) {}

  int ClientStreamingBadReturn(NanopbServerReader<FakePb, FakePb>&) {
    return 0;
  }

  static void StaticClientStreamingMissingArg() {}

  // Bidirectional streaming signatures

  void BidirectionalStreaming(NanopbServerReaderWriter<FakePb, FakePb>&) {}

  static void StaticBidirectionalStreaming(
      NanopbServerReaderWriter<FakePb, FakePb>&) {}

  int BidirectionalStreamingBadReturn(
      NanopbServerReaderWriter<FakePb, FakePb>&) {
    return 0;
  }

  static void StaticBidirectionalStreamingMissingArg() {}
};

struct WrongPb;

// Test matches() rejects incorrect request/response types.
// clang-format off
static_assert(!NanopbMethod::template matches<&TestNanopbService::Unary, WrongPb, FakePb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::Unary, FakePb, WrongPb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::Unary, WrongPb, WrongPb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::StaticUnary, FakePb, WrongPb>());

static_assert(!NanopbMethod::template matches<&TestNanopbService::ServerStreaming, WrongPb, FakePb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::StaticServerStreaming, FakePb, WrongPb>());

static_assert(!NanopbMethod::template matches<&TestNanopbService::ClientStreaming, WrongPb, FakePb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::StaticClientStreaming, FakePb, WrongPb>());

static_assert(!NanopbMethod::template matches<&TestNanopbService::BidirectionalStreaming, WrongPb, FakePb>());
static_assert(!NanopbMethod::template matches<&TestNanopbService::StaticBidirectionalStreaming, FakePb, WrongPb>());
// clang-format on

static_assert(MethodImplTests<NanopbMethod, TestNanopbService>().Pass(
    MatchesTypes<FakePb, FakePb>(),
    std::tuple<const NanopbMethodSerde&>(
        kNanopbMethodSerde<nullptr, nullptr>)));

template <typename Impl>
class FakeServiceBase : public Service {
 public:
  FakeServiceBase(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<NanopbMethodUnion, 5> kMethods = {
      NanopbMethod::SynchronousUnary<&Impl::DoNothing>(
          10u,
          kNanopbMethodSerde<pw_rpc_test_Empty_fields,
                             pw_rpc_test_Empty_fields>),
      NanopbMethod::AsynchronousUnary<&Impl::AddFive>(
          11u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
      NanopbMethod::ServerStreaming<&Impl::StartStream>(
          12u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
      NanopbMethod::ClientStreaming<&Impl::ClientStream>(
          13u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>),
      NanopbMethod::BidirectionalStreaming<&Impl::BidirectionalStream>(
          14u,
          kNanopbMethodSerde<pw_rpc_test_TestRequest_fields,
                             pw_rpc_test_TestResponse_fields>)};
};

class FakeService : public FakeServiceBase<FakeService> {
 public:
  FakeService(uint32_t id) : FakeServiceBase(id) {}

  Status DoNothing(const pw_rpc_test_Empty&, pw_rpc_test_Empty&) {
    return Status::Unknown();
  }

  void AddFive(const pw_rpc_test_TestRequest& request,
               NanopbUnaryResponder<pw_rpc_test_TestResponse>& responder) {
    last_request = request;

    if (fail_to_encode_async_unary_response) {
      pw_rpc_test_TestResponse response = pw_rpc_test_TestResponse_init_default;
      response.repeated_field.funcs.encode =
          [](pb_ostream_t*, const pb_field_t*, void* const*) { return false; };
      ASSERT_EQ(OkStatus(), responder.Finish(response, Status::NotFound()));
    } else {
      ASSERT_EQ(
          OkStatus(),
          responder.Finish({.value = static_cast<int32_t>(request.integer + 5)},
                           Status::Unauthenticated()));
    }
  }

  void StartStream(const pw_rpc_test_TestRequest& request,
                   NanopbServerWriter<pw_rpc_test_TestResponse>& writer) {
    last_request = request;
    last_writer = std::move(writer);
  }

  void ClientStream(NanopbServerReader<pw_rpc_test_TestRequest,
                                       pw_rpc_test_TestResponse>& reader) {
    last_reader = std::move(reader);
  }

  void BidirectionalStream(
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestResponse>& reader_writer) {
    last_reader_writer = std::move(reader_writer);
  }

  bool fail_to_encode_async_unary_response = false;

  pw_rpc_test_TestRequest last_request;
  NanopbServerWriter<pw_rpc_test_TestResponse> last_writer;
  NanopbServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestResponse>
      last_reader;
  NanopbServerReaderWriter<pw_rpc_test_TestRequest, pw_rpc_test_TestResponse>
      last_reader_writer;
};

constexpr const NanopbMethod& kSyncUnary =
    std::get<0>(FakeServiceBase<FakeService>::kMethods).nanopb_method();
constexpr const NanopbMethod& kAsyncUnary =
    std::get<1>(FakeServiceBase<FakeService>::kMethods).nanopb_method();
constexpr const NanopbMethod& kServerStream =
    std::get<2>(FakeServiceBase<FakeService>::kMethods).nanopb_method();
constexpr const NanopbMethod& kClientStream =
    std::get<3>(FakeServiceBase<FakeService>::kMethods).nanopb_method();
constexpr const NanopbMethod& kBidirectionalStream =
    std::get<4>(FakeServiceBase<FakeService>::kMethods).nanopb_method();

TEST(NanopbMethod, AsyncUnaryRpc_SendsResponse) {
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 123, .status_code = 0);

  ServerContextForTest<FakeService> context(kAsyncUnary);
  rpc_lock().lock();
  kAsyncUnary.Invoke(context.get(), context.request(request));

  const Packet& response = context.output().last_packet();
  EXPECT_EQ(response.status(), Status::Unauthenticated());

  // Field 1 (encoded as 1 << 3) with 128 as the value.
  constexpr std::byte expected[]{
      std::byte{0x08}, std::byte{0x80}, std::byte{0x01}};

  EXPECT_EQ(sizeof(expected), response.payload().size());
  EXPECT_EQ(0,
            std::memcmp(expected, response.payload().data(), sizeof(expected)));

  EXPECT_EQ(123, context.service().last_request.integer);
}

TEST(NanopbMethod, SyncUnaryRpc_InvalidPayload_SendsError) {
  std::array<byte, 8> bad_payload{byte{0xFF}, byte{0xAA}, byte{0xDD}};

  ServerContextForTest<FakeService> context(kSyncUnary);
  rpc_lock().lock();
  kSyncUnary.Invoke(context.get(), context.request(bad_payload));

  const Packet& packet = context.output().last_packet();
  EXPECT_EQ(pwpb::PacketType::SERVER_ERROR, packet.type());
  EXPECT_EQ(Status::DataLoss(), packet.status());
  EXPECT_EQ(context.service_id(), packet.service_id());
  EXPECT_EQ(kSyncUnary.id(), packet.method_id());
}

TEST(NanopbMethod, AsyncUnaryRpc_ResponseEncodingFails_SendsInternalError) {
  constexpr int64_t value = 0x7FFFFFFF'FFFFFF00ll;
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = value, .status_code = 0);

  ServerContextForTest<FakeService> context(kAsyncUnary);
  context.service().fail_to_encode_async_unary_response = true;

  rpc_lock().lock();
  kAsyncUnary.Invoke(context.get(), context.request(request));

  const Packet& packet = context.output().last_packet();
  EXPECT_EQ(pwpb::PacketType::SERVER_ERROR, packet.type());
  EXPECT_EQ(Status::Internal(), packet.status());
  EXPECT_EQ(context.service_id(), packet.service_id());
  EXPECT_EQ(kAsyncUnary.id(), packet.method_id());

  EXPECT_EQ(value, context.service().last_request.integer);
}

TEST(NanopbMethod, ServerStreamingRpc_SendsNothingWhenInitiallyCalled) {
  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 555, .status_code = 0);

  ServerContextForTest<FakeService> context(kServerStream);

  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request(request));

  EXPECT_EQ(0u, context.output().total_packets());
  EXPECT_EQ(555, context.service().last_request.integer);
}

TEST(NanopbMethod, ServerWriter_SendsResponse) {
  ServerContextForTest<FakeService> context(kServerStream);

  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  EXPECT_EQ(OkStatus(), context.service().last_writer.Write({.value = 100}));

  PW_ENCODE_PB(pw_rpc_test_TestResponse, payload, .value = 100);
  std::array<byte, 128> encoded_response = {};
  auto encoded = context.server_stream(payload).Encode(encoded_response);
  ASSERT_EQ(OkStatus(), encoded.status());

  ConstByteSpan sent_payload = context.output().last_packet().payload();
  EXPECT_TRUE(pw::containers::Equal(payload, sent_payload));
}

TEST(NanopbMethod, ServerWriter_WriteWhenClosed_ReturnsFailedPrecondition) {
  ServerContextForTest<FakeService> context(kServerStream);

  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  EXPECT_EQ(OkStatus(), context.service().last_writer.Finish());
  EXPECT_TRUE(context.service()
                  .last_writer.Write({.value = 100})
                  .IsFailedPrecondition());
}

TEST(NanopbMethod, ServerWriter_WriteAfterMoved_ReturnsFailedPrecondition) {
  ServerContextForTest<FakeService> context(kServerStream);

  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));
  NanopbServerWriter<pw_rpc_test_TestResponse> new_writer =
      std::move(context.service().last_writer);

  EXPECT_EQ(OkStatus(), new_writer.Write({.value = 100}));

  EXPECT_EQ(Status::FailedPrecondition(),
            context.service().last_writer.Write({.value = 100}));
  EXPECT_EQ(Status::FailedPrecondition(),
            context.service().last_writer.Finish());

  EXPECT_EQ(OkStatus(), new_writer.Finish());
}

TEST(NanopbMethod, ServerStreamingRpc_ResponseEncodingFails_InternalError) {
  ServerContextForTest<FakeService> context(kServerStream);

  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  EXPECT_EQ(OkStatus(), context.service().last_writer.Write({}));

  pw_rpc_test_TestResponse response = pw_rpc_test_TestResponse_init_default;
  response.repeated_field.funcs.encode =
      [](pb_ostream_t*, const pb_field_t*, void* const*) { return false; };
  EXPECT_EQ(Status::Internal(), context.service().last_writer.Write(response));
}

TEST(NanopbMethod, ServerReader_HandlesRequests) {
  ServerContextForTest<FakeService> context(kClientStream);

  rpc_lock().lock();
  kClientStream.Invoke(context.get(), context.request({}));

  pw_rpc_test_TestRequest request_struct{};
  context.service().last_reader.set_on_next(
      [&request_struct](const pw_rpc_test_TestRequest& req) {
        request_struct = req;
      });

  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 1 << 30, .status_code = 9);
  std::array<byte, 128> encoded_request = {};
  auto encoded = context.client_stream(request).Encode(encoded_request);
  ASSERT_EQ(OkStatus(), encoded.status());
  ASSERT_EQ(OkStatus(), context.server().ProcessPacket(*encoded));

  EXPECT_EQ(request_struct.integer, 1 << 30);
  EXPECT_EQ(request_struct.status_code, 9u);
}

TEST(NanopbMethod, ServerReaderWriter_WritesResponses) {
  ServerContextForTest<FakeService> context(kBidirectionalStream);

  rpc_lock().lock();
  kBidirectionalStream.Invoke(context.get(), context.request({}));

  EXPECT_EQ(OkStatus(),
            context.service().last_reader_writer.Write({.value = 100}));

  PW_ENCODE_PB(pw_rpc_test_TestResponse, payload, .value = 100);
  std::array<byte, 128> encoded_response = {};
  auto encoded = context.server_stream(payload).Encode(encoded_response);
  ASSERT_EQ(OkStatus(), encoded.status());

  ConstByteSpan sent_payload = context.output().last_packet().payload();
  EXPECT_TRUE(pw::containers::Equal(payload, sent_payload));
}

TEST(NanopbMethod, ServerReaderWriter_HandlesRequests) {
  ServerContextForTest<FakeService> context(kBidirectionalStream);

  rpc_lock().lock();
  kBidirectionalStream.Invoke(context.get(), context.request({}));

  pw_rpc_test_TestRequest request_struct{};
  context.service().last_reader_writer.set_on_next(
      [&request_struct](const pw_rpc_test_TestRequest& req) {
        request_struct = req;
      });

  PW_ENCODE_PB(
      pw_rpc_test_TestRequest, request, .integer = 1 << 29, .status_code = 8);
  std::array<byte, 128> encoded_request = {};
  auto encoded = context.client_stream(request).Encode(encoded_request);
  ASSERT_EQ(OkStatus(), encoded.status());
  ASSERT_EQ(OkStatus(), context.server().ProcessPacket(*encoded));

  EXPECT_EQ(request_struct.integer, 1 << 29);
  EXPECT_EQ(request_struct.status_code, 8u);
}

}  // namespace
}  // namespace pw::rpc::internal

PW_MODIFY_DIAGNOSTICS_POP();
