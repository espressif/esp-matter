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

#include "proto_bloat.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_protobuf/decoder.h"
#include "pw_protobuf/encoder.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_protobuf_test_protos/size_report.pwpb.h"
#include "pw_result/result.h"
#include "pw_status/status.h"

#ifndef _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN
#define _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN 0
#endif  // _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN

#ifndef _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT
#define _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT 0
#endif  // _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT

#ifndef _PW_PROTOBUF_SIZE_REPORT_MESSAGE
#define _PW_PROTOBUF_SIZE_REPORT_MESSAGE 0
#endif  // _PW_PROTOBUF_SIZE_REPORT_MESSAGE

namespace pw::protobuf_size_report {
namespace {

template <typename T>
PW_NO_INLINE void ConsumeValue(T val) {
  [[maybe_unused]] volatile T no_optimize = val;
}

#if _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> encode_buffer;
pw::protobuf::MemoryEncoder generic_encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  pw::Status status;
  status.Update(generic_encoder.WriteInt64(1, 0x5001DBADFEEDBEE5));
  status.Update(generic_encoder.WriteInt32(2, 128));
  status.Update(generic_encoder.WriteInt32(3, 2));
  ConsumeValue(status);
}

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::protobuf::Decoder generic_decoder(decode_buffer);

PW_NO_INLINE void BasicDecode() {
  while (generic_decoder.Next().ok()) {
    switch (generic_decoder.FieldNumber()) {
      case static_cast<uint32_t>(pwpb::ItemInfo::Fields::OFFSET): {
        uint64_t value;
        if (generic_decoder.ReadUint64(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(pwpb::ItemInfo::Fields::SIZE): {
        uint32_t value;
        if (generic_decoder.ReadUint32(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(pwpb::ItemInfo::Fields::ACCESS_LEVEL): {
        uint32_t value;

        if (generic_decoder.ReadUint32(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
    }
  }
}
#endif  // _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN

#if _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> encode_buffer;
pwpb::ItemInfo::MemoryEncoder encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  pw::Status status;
  status.Update(encoder.WriteOffset(0x5001DBADFEEDBEE5));
  status.Update(encoder.WriteSize(128));
  status.Update(encoder.WriteAccessLevel(pwpb::ItemInfo::Access::WRITE));
  ConsumeValue(status);
}

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::stream::MemoryReader reader(decode_buffer);
pwpb::ItemInfo::StreamDecoder decoder(reader);

PW_NO_INLINE void BasicDecode() {
  while (decoder.Next().ok()) {
    pw::Result<pwpb::ItemInfo::Fields> field = decoder.Field();
    if (!field.ok()) {
      ConsumeValue(field.status());
      return;
    }

    switch (field.value()) {
      case pwpb::ItemInfo::Fields::OFFSET: {
        pw::Result<uint64_t> value = decoder.ReadOffset();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case pwpb::ItemInfo::Fields::SIZE: {
        pw::Result<uint32_t> value = decoder.ReadSize();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case pwpb::ItemInfo::Fields::ACCESS_LEVEL: {
        pw::Result<pwpb::ItemInfo::Access> value = decoder.ReadAccessLevel();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
    }
  }
}
#endif  // _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT

#if _PW_PROTOBUF_SIZE_REPORT_MESSAGE

pwpb::ItemInfo::Message message;

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> encode_buffer;
pwpb::ItemInfo::MemoryEncoder encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  message.offset = 0x5001DBADFEEDBEE5;
  message.size = 128;
  message.access_level = pwpb::ItemInfo::Access::WRITE;
  ConsumeValue(encoder.Write(message));
}

std::array<std::byte, pwpb::ItemInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::stream::MemoryReader reader(decode_buffer);
pwpb::ItemInfo::StreamDecoder decoder(reader);

PW_NO_INLINE void BasicDecode() {
  if (pw::Status status = decoder.Read(message); status.ok()) {
    ConsumeValue(status);
  }
}
#endif  // _PW_PROTOBUF_SIZE_REPORT_MESSAGE

}  // namespace
}  // namespace pw::protobuf_size_report

int main() {
  pw::bloat::BloatThisBinary();
  pw::protobuf_size_report::BloatWithBase();
  pw::protobuf_size_report::BloatWithEncoder();
  pw::protobuf_size_report::BloatWithStreamDecoder();
  pw::protobuf_size_report::BloatWithDecoder();
  pw::protobuf_size_report::BloatWithTableEncoder();
  pw::protobuf_size_report::BloatWithTableDecoder();
  pw::protobuf_size_report::BasicEncode();
  pw::protobuf_size_report::BasicDecode();
  return 0;
}
