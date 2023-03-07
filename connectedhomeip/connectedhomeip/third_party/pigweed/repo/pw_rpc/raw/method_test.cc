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

#include "pw_rpc/raw/internal/method.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_containers/algorithm.h"
#include "pw_protobuf/decoder.h"
#include "pw_protobuf/encoder.h"
#include "pw_rpc/internal/config.h"
#include "pw_rpc/internal/method_impl_tester.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/raw/internal/method_union.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.pwpb.h"

namespace pw::rpc::internal {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;

// Create a fake service for use with the MethodImplTester.
class TestRawService final : public Service {
 public:
  // Unary signatures

  void Unary(ConstByteSpan, RawUnaryResponder&) {}

  static void StaticUnary(ConstByteSpan, RawUnaryResponder&) {}

  void AsyncUnary(ConstByteSpan, RawUnaryResponder&) {}

  static void StaticAsyncUnary(ConstByteSpan, RawUnaryResponder&) {}

  void UnaryWrongArg(ConstByteSpan, ConstByteSpan) {}

  // Server streaming signatures

  void ServerStreaming(ConstByteSpan, RawServerWriter&) {}

  static void StaticServerStreaming(ConstByteSpan, RawServerWriter&) {}

  static void StaticUnaryVoidReturn(ConstByteSpan, ByteSpan) {}

  Status ServerStreamingBadReturn(ConstByteSpan, RawServerWriter&) {
    return Status();
  }

  static void StaticServerStreamingMissingArg(RawServerWriter&) {}

  // Client streaming signatures

  void ClientStreaming(RawServerReader&) {}

  static void StaticClientStreaming(RawServerReader&) {}

  int ClientStreamingBadReturn(RawServerReader&) { return 0; }

  static void StaticClientStreamingMissingArg() {}

  // Bidirectional streaming signatures

  void BidirectionalStreaming(RawServerReaderWriter&) {}

  static void StaticBidirectionalStreaming(RawServerReaderWriter&) {}

  int BidirectionalStreamingBadReturn(RawServerReaderWriter&) { return 0; }

  static void StaticBidirectionalStreamingMissingArg() {}
};

static_assert(MethodImplTests<RawMethod, TestRawService>().Pass());

template <typename Impl>
class FakeServiceBase : public Service {
 public:
  FakeServiceBase(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<RawMethodUnion, 5> kMethods = {
      RawMethod::AsynchronousUnary<&Impl::DoNothing>(10u),
      RawMethod::AsynchronousUnary<&Impl::AddFive>(11u),
      RawMethod::ServerStreaming<&Impl::StartStream>(12u),
      RawMethod::ClientStreaming<&Impl::ClientStream>(13u),
      RawMethod::BidirectionalStreaming<&Impl::BidirectionalStream>(14u),
  };
};

class FakeService : public FakeServiceBase<FakeService> {
 public:
  FakeService(uint32_t id) : FakeServiceBase(id) {}

  void DoNothing(ConstByteSpan, RawUnaryResponder& responder) {
    ASSERT_EQ(OkStatus(), responder.Finish({}, Status::Unknown()));
  }

  void AddFive(ConstByteSpan request, RawUnaryResponder& responder) {
    DecodeRawTestRequest(request);

    std::array<std::byte, 32> response;
    TestResponse::MemoryEncoder test_response(response);
    EXPECT_EQ(OkStatus(), test_response.WriteValue(last_request.integer + 5));
    ConstByteSpan payload(test_response);

    ASSERT_EQ(OkStatus(),
              responder.Finish(span(response).first(payload.size()),
                               Status::Unauthenticated()));
  }

  void StartStream(ConstByteSpan request, RawServerWriter& writer) {
    DecodeRawTestRequest(request);
    last_writer = std::move(writer);
  }

  void ClientStream(RawServerReader& reader) {
    last_reader = std::move(reader);
  }

  void BidirectionalStream(RawServerReaderWriter& reader_writer) {
    last_reader_writer = std::move(reader_writer);
  }

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

  struct {
    int64_t integer;
    uint32_t status_code;
  } last_request;

  RawServerWriter last_writer;
  RawServerReader last_reader;
  RawServerReaderWriter last_reader_writer;
};

constexpr const RawMethod& kAsyncUnary0 =
    std::get<0>(FakeServiceBase<FakeService>::kMethods).raw_method();
constexpr const RawMethod& kAsyncUnary1 =
    std::get<1>(FakeServiceBase<FakeService>::kMethods).raw_method();
constexpr const RawMethod& kServerStream =
    std::get<2>(FakeServiceBase<FakeService>::kMethods).raw_method();
constexpr const RawMethod& kClientStream =
    std::get<3>(FakeServiceBase<FakeService>::kMethods).raw_method();
constexpr const RawMethod& kBidirectionalStream =
    std::get<4>(FakeServiceBase<FakeService>::kMethods).raw_method();

TEST(RawMethod, AsyncUnaryRpc1_SendsResponse) {
  std::byte buffer[16];
  stream::MemoryWriter writer(buffer);
  TestRequest::StreamEncoder test_request(writer, ByteSpan());
  ASSERT_EQ(OkStatus(), test_request.WriteInteger(456));
  ASSERT_EQ(OkStatus(), test_request.WriteStatusCode(7));

  ServerContextForTest<FakeService> context(kAsyncUnary1);
  rpc_lock().lock();
  kAsyncUnary1.Invoke(context.get(), context.request(writer.WrittenData()));

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

TEST(RawMethod, AsyncUnaryRpc0_SendsResponse) {
  ServerContextForTest<FakeService> context(kAsyncUnary0);

  rpc_lock().lock();
  kAsyncUnary0.Invoke(context.get(), context.request({}));

  const Packet& packet = context.output().last_packet();
  EXPECT_EQ(pwpb::PacketType::RESPONSE, packet.type());
  EXPECT_EQ(Status::Unknown(), packet.status());
  EXPECT_EQ(context.service_id(), packet.service_id());
  EXPECT_EQ(kAsyncUnary0.id(), packet.method_id());
}

TEST(RawMethod, ServerStreamingRpc_SendsNothingWhenInitiallyCalled) {
  std::byte buffer[16];
  stream::MemoryWriter writer(buffer);
  TestRequest::StreamEncoder test_request(writer, ByteSpan());
  ASSERT_EQ(OkStatus(), test_request.WriteInteger(777));
  ASSERT_EQ(OkStatus(), test_request.WriteStatusCode(2));

  ServerContextForTest<FakeService> context(kServerStream);
  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request(writer.WrittenData()));

  EXPECT_EQ(0u, context.output().total_packets());
  EXPECT_EQ(777, context.service().last_request.integer);
  EXPECT_EQ(2u, context.service().last_request.status_code);
  EXPECT_TRUE(context.service().last_writer.active());
  EXPECT_EQ(OkStatus(), context.service().last_writer.Finish());
}

TEST(RawMethod, ServerReader_HandlesRequests) {
  ServerContextForTest<FakeService> context(kClientStream);
  rpc_lock().lock();
  kClientStream.Invoke(context.get(), context.request({}));

  ConstByteSpan request;
  context.service().last_reader.set_on_next(
      [&request](ConstByteSpan req) { request = req; });

  constexpr const char kRequestValue[] = "This is a request payload!!!";
  std::array<std::byte, 128> encoded_request = {};
  auto encoded = context.client_stream(as_bytes(span(kRequestValue)))
                     .Encode(encoded_request);
  ASSERT_EQ(OkStatus(), encoded.status());
  ASSERT_EQ(OkStatus(), context.server().ProcessPacket(*encoded));

  EXPECT_STREQ(reinterpret_cast<const char*>(request.data()), kRequestValue);
}

TEST(RawMethod, ServerReaderWriter_WritesResponses) {
  ServerContextForTest<FakeService> context(kBidirectionalStream);
  rpc_lock().lock();
  kBidirectionalStream.Invoke(context.get(), context.request({}));

  constexpr const char kRequestValue[] = "O_o";
  const auto kRequestBytes = as_bytes(span(kRequestValue));
  EXPECT_EQ(OkStatus(),
            context.service().last_reader_writer.Write(kRequestBytes));

  std::array<std::byte, 128> encoded_response = {};
  auto encoded = context.server_stream(kRequestBytes).Encode(encoded_response);
  ASSERT_EQ(OkStatus(), encoded.status());

  ConstByteSpan sent_payload = context.output().last_packet().payload();
  EXPECT_TRUE(pw::containers::Equal(kRequestBytes, sent_payload));
}

TEST(RawServerWriter, Write_SendsPayload) {
  ServerContextForTest<FakeService> context(kServerStream);
  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  constexpr auto data = bytes::Array<0x0d, 0x06, 0xf0, 0x0d>();
  EXPECT_EQ(context.service().last_writer.Write(data), OkStatus());

  const internal::Packet& packet = context.output().last_packet();
  EXPECT_EQ(packet.type(), pwpb::PacketType::SERVER_STREAM);
  EXPECT_EQ(packet.channel_id(), context.channel_id());
  EXPECT_EQ(packet.service_id(), context.service_id());
  EXPECT_EQ(packet.method_id(), context.get().method().id());
  EXPECT_EQ(std::memcmp(packet.payload().data(), data.data(), data.size()), 0);
  EXPECT_EQ(packet.status(), OkStatus());
}

TEST(RawServerWriter, Write_EmptyBuffer) {
  ServerContextForTest<FakeService> context(kServerStream);
  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  ASSERT_EQ(context.service().last_writer.Write({}), OkStatus());

  const internal::Packet& packet = context.output().last_packet();
  EXPECT_EQ(packet.type(), pwpb::PacketType::SERVER_STREAM);
  EXPECT_EQ(packet.channel_id(), context.channel_id());
  EXPECT_EQ(packet.service_id(), context.service_id());
  EXPECT_EQ(packet.method_id(), context.get().method().id());
  EXPECT_TRUE(packet.payload().empty());
  EXPECT_EQ(packet.status(), OkStatus());
}

TEST(RawServerWriter, Write_Closed_ReturnsFailedPrecondition) {
  ServerContextForTest<FakeService> context(kServerStream);
  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  EXPECT_EQ(OkStatus(), context.service().last_writer.Finish());
  constexpr auto data = bytes::Array<0x0d, 0x06, 0xf0, 0x0d>();
  EXPECT_EQ(context.service().last_writer.Write(data),
            Status::FailedPrecondition());
}

TEST(RawServerWriter, Write_PayloadTooLargeForEncodingBuffer_ReturnsInternal) {
  ServerContextForTest<FakeService> context(kServerStream);
  rpc_lock().lock();
  kServerStream.Invoke(context.get(), context.request({}));

  // A kEncodingBufferSizeBytes payload will never fit in the encoding buffer.
  static constexpr std::array<std::byte, cfg::kEncodingBufferSizeBytes>
      kBigData = {};
  EXPECT_EQ(context.service().last_writer.Write(kBigData), Status::Internal());
}

}  // namespace
}  // namespace pw::rpc::internal
