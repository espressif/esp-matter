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
#pragma once

#include "pw_protobuf/encoder.h"
#include "pw_protobuf/internal/codegen.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_span/span.h"

namespace pw::rpc {

using PwpbMessageDescriptor =
    const span<const protobuf::internal::MessageField>*;

// Serializer/deserializer for a pw_protobuf message.
class PwpbSerde {
 public:
  explicit constexpr PwpbSerde(PwpbMessageDescriptor table) : table_(table) {}

  PwpbSerde(const PwpbSerde&) = default;
  PwpbSerde& operator=(const PwpbSerde&) = default;

  // Encodes a pw_protobuf struct to the serialized wire format.
  template <typename Message>
  StatusWithSize Encode(const Message& message, ByteSpan buffer) const {
    return Encoder(buffer).Write(as_bytes(span(&message, 1)), table_);
  }

  // Decodes a serialized protobuf into a pw_protobuf message struct.
  template <typename Message>
  Status Decode(ConstByteSpan buffer, Message& message) const {
    return Decoder(buffer).Read(as_writable_bytes(span(&message, 1)), table_);
  }

 private:
  class Encoder : public protobuf::MemoryEncoder {
   public:
    constexpr Encoder(ByteSpan buffer) : protobuf::MemoryEncoder(buffer) {}

    StatusWithSize Write(ConstByteSpan message, PwpbMessageDescriptor table) {
      const auto status = protobuf::MemoryEncoder::Write(message, *table);
      return StatusWithSize(status, size());
    }
  };

  class Decoder : public protobuf::StreamDecoder {
   public:
    constexpr Decoder(ConstByteSpan buffer)
        : protobuf::StreamDecoder(reader_), reader_(buffer) {}

    Status Read(ByteSpan message, PwpbMessageDescriptor table) {
      return protobuf::StreamDecoder::Read(message, *table);
    }

   private:
    stream::MemoryReader reader_;
  };

  PwpbMessageDescriptor table_;
};

// Serializer/deserializer for pw_protobuf request and response message structs
// within an RPC method.
class PwpbMethodSerde {
 public:
  constexpr PwpbMethodSerde(PwpbMessageDescriptor request_table,
                            PwpbMessageDescriptor response_table)
      : request_serde_(request_table), response_serde_(response_table) {}

  PwpbMethodSerde(const PwpbMethodSerde&) = delete;
  PwpbMethodSerde& operator=(const PwpbMethodSerde&) = delete;

  // Encodes the pw_protobuf request struct to the serialized wire format.
  template <typename Request>
  StatusWithSize EncodeRequest(const Request& request, ByteSpan buffer) const {
    return request_serde_.Encode(request, buffer);
  }

  // Encodes the pw_protobuf response struct to the serialized wire format.
  template <typename Response>
  StatusWithSize EncodeResponse(const Response& response,
                                ByteSpan buffer) const {
    return response_serde_.Encode(response, buffer);
  }
  // Decodes a serialized protobuf into the pw_protobuf request struct.
  template <typename Request>
  Status DecodeRequest(ConstByteSpan buffer, Request& request) const {
    return request_serde_.Decode(buffer, request);
  }

  // Decodes a serialized protobuf into the pw_protobuf response struct.
  template <typename Response>
  Status DecodeResponse(ConstByteSpan buffer, Response& response) const {
    return response_serde_.Decode(buffer, response);
  }

  const PwpbSerde& request() const { return request_serde_; }
  const PwpbSerde& response() const { return response_serde_; }

 private:
  PwpbSerde request_serde_;
  PwpbSerde response_serde_;
};

}  // namespace pw::rpc
