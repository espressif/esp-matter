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
#include "pb_decode.h"
#include "pb_encode.h"
#include "pw_rpc/nanopb/test_method_context.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

namespace pw::rpc {

class TestServiceImpl final
    : public test::pw_rpc::nanopb::TestService::Service<TestServiceImpl> {
 public:
  Status TestUnaryRpc(const pw_rpc_test_TestRequest&,
                      pw_rpc_test_TestResponse&) {
    return OkStatus();
  }

  Status TestAnotherUnaryRpc(const pw_rpc_test_TestRequest&,
                             pw_rpc_test_TestResponse& response) {
    typedef std::array<uint32_t, 3> ArgType;
    // The values array needs to be kept in memory until after this method call
    // returns since the response is not encoded until after returning from this
    // method.
    static const ArgType values = {7, 8, 9};
    response.repeated_field.funcs.encode = +[](pb_ostream_t* stream,
                                               const pb_field_t* field,
                                               void* const* arg) -> bool {
      // Note: nanopb passes the pointer to the repeated_filed.arg member as
      // arg, not its contents.
      for (auto elem : *static_cast<const ArgType*>(*arg)) {
        if (!pb_encode_tag_for_field(stream, field) ||
            !pb_encode_varint(stream, elem))
          return false;
      }
      return true;
    };
    response.repeated_field.arg = const_cast<ArgType*>(&values);
    return OkStatus();
  }

  void TestServerStreamRpc(
      const pw_rpc_test_TestRequest&,
      NanopbServerWriter<pw_rpc_test_TestStreamResponse>&) {}

  void TestClientStreamRpc(
      NanopbServerReader<pw_rpc_test_TestRequest,
                         pw_rpc_test_TestStreamResponse>&) {}

  void TestBidirectionalStreamRpc(
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestStreamResponse>&) {}
};

TEST(NanopbTestMethodContext, ResponseWithCallbacks) {
  PW_NANOPB_TEST_METHOD_CONTEXT(TestServiceImpl, TestAnotherUnaryRpc) ctx;
  ASSERT_EQ(ctx.call(pw_rpc_test_TestRequest_init_default), OkStatus());

  // Calling response() without an argument returns a Response struct from a
  // newly decoded one, without any callbacks set.
  EXPECT_EQ(ctx.response().repeated_field.arg, nullptr);

  // To decode a response object that requires to set pb_callback_t members,
  // pass it to the response() method as a parameter.
  constexpr size_t kMaxNumValues = 4;
  struct DecoderContext {
    uint32_t num_calls = 0;
    uint32_t values[kMaxNumValues];
    bool failed = false;
  } decoder_context;

  pw_rpc_test_TestResponse response = pw_rpc_test_TestResponse_init_default;
  response.repeated_field.funcs.decode = +[](pb_istream_t* stream,
                                             const pb_field_t* /* field */,
                                             void** arg) -> bool {
    DecoderContext* dec_ctx = static_cast<DecoderContext*>(*arg);
    uint64_t value;
    if (!pb_decode_varint(stream, &value)) {
      dec_ctx->failed = true;
      return false;
    }
    if (dec_ctx->num_calls < kMaxNumValues) {
      dec_ctx->values[dec_ctx->num_calls] = value;
    }
    dec_ctx->num_calls++;
    return true;
  };
  response.repeated_field.arg = &decoder_context;
  ctx.response(response);

  EXPECT_FALSE(decoder_context.failed);
  EXPECT_EQ(3u, decoder_context.num_calls);
  EXPECT_EQ(7u, decoder_context.values[0]);
  EXPECT_EQ(8u, decoder_context.values[1]);
  EXPECT_EQ(9u, decoder_context.values[2]);
}

}  // namespace pw::rpc
