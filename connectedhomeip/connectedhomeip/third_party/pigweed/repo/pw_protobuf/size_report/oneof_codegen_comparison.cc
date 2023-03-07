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

namespace ItemInfo = pwpb::ItemInfo;
namespace ResponseInfo = pwpb::ResponseInfo;

template <typename T>
PW_NO_INLINE void ConsumeValue(T val) {
  [[maybe_unused]] volatile T no_optimize = val;
}

#if _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN

std::array<std::byte, ItemInfo::kMaxEncodedSizeBytes> encode_buffer;
pw::protobuf::MemoryEncoder encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  pw::Status status;
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::KEY_STRING;
  volatile bool has_timestamp = true;
  volatile bool has_has_value = false;
  if (which_key == KeyType::KEY_STRING) {
    encoder.WriteString(1, "test");
  } else if (which_key == KeyType::KEY_TOKEN) {
    encoder.WriteFixed32(2, 99999);
  }

  if (has_timestamp) {
    encoder.WriteInt64(3, 1663003467);
  }

  if (has_has_value) {
    encoder.WriteBool(4, true);
  }

  {
    pw::protobuf::StreamEncoder submessage_encoder =
        encoder.GetNestedEncoder(5);
    status.Update(submessage_encoder.WriteInt64(1, 0x5001DBADFEEDBEE5));
    status.Update(submessage_encoder.WriteInt32(2, 128));
    status.Update(submessage_encoder.WriteInt32(3, 2));
  }
  ConsumeValue(status);
}

std::array<std::byte, ItemInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::protobuf::Decoder decoder(decode_buffer);

PW_NO_INLINE void DecodeItemInfo(pw::ConstByteSpan data) {
  pw::protobuf::Decoder submessage_decoder(data);
  while (submessage_decoder.Next().ok()) {
    switch (submessage_decoder.FieldNumber()) {
      case static_cast<uint32_t>(ItemInfo::Fields::OFFSET): {
        uint64_t value;
        if (submessage_decoder.ReadUint64(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ItemInfo::Fields::SIZE): {
        uint32_t value;
        if (submessage_decoder.ReadUint32(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ItemInfo::Fields::ACCESS_LEVEL): {
        uint32_t value;

        if (submessage_decoder.ReadUint32(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
    }
  }
}

PW_NO_INLINE void BasicDecode() {
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::NONE;
  volatile bool has_timestamp = false;
  volatile bool has_has_value = false;

  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(ResponseInfo::Fields::KEY_STRING): {
        which_key = KeyType::KEY_STRING;
        std::string_view value;
        if (decoder.ReadString(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ResponseInfo::Fields::KEY_TOKEN): {
        which_key = KeyType::KEY_TOKEN;
        uint32_t value;
        if (decoder.ReadUint32(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ResponseInfo::Fields::TIMESTAMP): {
        uint64_t value;
        has_timestamp = true;
        if (decoder.ReadUint64(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ResponseInfo::Fields::HAS_VALUE): {
        bool value;
        has_has_value = true;
        if (decoder.ReadBool(&value).ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case static_cast<uint32_t>(ResponseInfo::Fields::ITEM_INFO): {
        pw::ConstByteSpan value;
        if (decoder.ReadBytes(&value).ok()) {
          DecodeItemInfo(value);
        }
        break;
      }
    }
  }
  ConsumeValue(which_key);
  ConsumeValue(has_timestamp);
  ConsumeValue(has_has_value);
}

#endif  // _PW_PROTOBUF_SIZE_REPORT_NO_CODEGEN

#if _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT

std::array<std::byte, ResponseInfo::kMaxEncodedSizeBytes> encode_buffer;
ResponseInfo::MemoryEncoder encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  pw::Status status;
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::KEY_STRING;
  volatile bool has_timestamp = true;
  volatile bool has_has_value = false;
  if (which_key == KeyType::KEY_STRING) {
    encoder.WriteKeyString("test");
  } else if (which_key == KeyType::KEY_TOKEN) {
    encoder.WriteKeyToken(99999);
  }

  if (has_timestamp) {
    encoder.WriteTimestamp(1663003467);
  }

  if (has_has_value) {
    encoder.WriteHasValue(true);
  }

  {
    ItemInfo::StreamEncoder submessage_encoder = encoder.GetItemInfoEncoder();
    status.Update(submessage_encoder.WriteOffset(0x5001DBADFEEDBEE5));
    status.Update(submessage_encoder.WriteSize(128));
    status.Update(submessage_encoder.WriteAccessLevel(ItemInfo::Access::WRITE));
  }
  ConsumeValue(status);
}

std::array<std::byte, ResponseInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::stream::MemoryReader reader(decode_buffer);
ResponseInfo::StreamDecoder decoder(reader);

PW_NO_INLINE void DecodeItemInfo(ItemInfo::StreamDecoder& submessage_decoder) {
  while (submessage_decoder.Next().ok()) {
    pw::Result<ItemInfo::Fields> field = submessage_decoder.Field();
    if (!field.ok()) {
      ConsumeValue(field.status());
      return;
    }

    switch (field.value()) {
      case ItemInfo::Fields::OFFSET: {
        pw::Result<uint64_t> value = submessage_decoder.ReadOffset();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case ItemInfo::Fields::SIZE: {
        pw::Result<uint32_t> value = submessage_decoder.ReadSize();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
      case ItemInfo::Fields::ACCESS_LEVEL: {
        pw::Result<ItemInfo::Access> value =
            submessage_decoder.ReadAccessLevel();
        if (value.ok()) {
          ConsumeValue(value);
        }
        break;
      }
    }
  }
}

PW_NO_INLINE void BasicDecode() {
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::NONE;
  volatile bool has_timestamp = false;
  volatile bool has_has_value = false;

  while (decoder.Next().ok()) {
    while (decoder.Next().ok()) {
      pw::Result<ResponseInfo::Fields> field = decoder.Field();
      if (!field.ok()) {
        ConsumeValue(field.status());
        return;
      }

      switch (field.value()) {
        case ResponseInfo::Fields::KEY_STRING: {
          which_key = KeyType::KEY_STRING;
          std::array<char, 8> value;
          pw::StatusWithSize status = decoder.ReadKeyString(value);
          if (status.ok()) {
            ConsumeValue(pw::span(value));
          }
          break;
        }
        case ResponseInfo::Fields::KEY_TOKEN: {
          which_key = KeyType::KEY_TOKEN;
          pw::Result<uint32_t> value = decoder.ReadKeyToken();
          if (value.ok()) {
            ConsumeValue(value);
          }
          break;
        }
        case ResponseInfo::Fields::TIMESTAMP: {
          has_timestamp = true;
          pw::Result<int64_t> value = decoder.ReadTimestamp();
          if (value.ok()) {
            ConsumeValue(value);
          }
          break;
        }
        case ResponseInfo::Fields::HAS_VALUE: {
          has_has_value = true;
          pw::Result<bool> value = decoder.ReadHasValue();
          if (value.ok()) {
            ConsumeValue(value);
          }
          break;
        }
        case ResponseInfo::Fields::ITEM_INFO: {
          ItemInfo::StreamDecoder submessage_decoder =
              decoder.GetItemInfoDecoder();
          DecodeItemInfo(submessage_decoder);
          break;
        }
      }
    }
  }
  ConsumeValue(which_key);
  ConsumeValue(has_timestamp);
  ConsumeValue(has_has_value);
}
#endif  // _PW_PROTOBUF_SIZE_REPORT_WIRE_FORMAT

#if _PW_PROTOBUF_SIZE_REPORT_MESSAGE

ResponseInfo::Message message;

std::array<std::byte, ResponseInfo::kMaxEncodedSizeBytes> encode_buffer;
ResponseInfo::MemoryEncoder encoder(encode_buffer);

PW_NO_INLINE void BasicEncode() {
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::KEY_STRING;
  volatile bool has_timestamp = true;
  volatile bool has_has_value = false;
  if (which_key == KeyType::KEY_STRING) {
    message.key_string.SetEncoder(
        [](ResponseInfo::StreamEncoder& key_string_encoder) -> pw::Status {
          key_string_encoder.WriteKeyString("test");
          return pw::OkStatus();
        });
  } else if (which_key == KeyType::KEY_TOKEN) {
    message.key_token = 99999;
  }
  message.timestamp =
      has_timestamp ? std::optional<uint32_t>(1663003467) : std::nullopt;
  message.has_value = has_has_value ? std::optional<bool>(false) : std::nullopt;

  message.item_info.offset = 0x5001DBADFEEDBEE5;
  message.item_info.size = 128;
  message.item_info.access_level = ItemInfo::Access::WRITE;
  ConsumeValue(encoder.Write(message));
}

std::array<std::byte, ResponseInfo::kMaxEncodedSizeBytes> decode_buffer;
pw::stream::MemoryReader reader(decode_buffer);
ResponseInfo::StreamDecoder decoder(reader);

PW_NO_INLINE void BasicDecode() {
  volatile enum KeyType : uint32_t {
    NONE = 0,
    KEY_STRING = 1,
    KEY_TOKEN = 2,
  } which_key = KeyType::NONE;
  volatile bool has_timestamp = false;
  volatile bool has_has_value = false;
  if (pw::Status status = decoder.Read(message); status.ok()) {
    ConsumeValue(status);
    has_timestamp = message.timestamp.has_value();
    has_has_value = message.has_value.has_value();
  }
  ConsumeValue(which_key);
  ConsumeValue(has_timestamp);
  ConsumeValue(has_has_value);
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
