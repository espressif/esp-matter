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
#pragma once

#include <string_view>

#include "pw_protobuf/wire_format.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_varint/varint.h"

// This file defines a low-level event-based protobuf wire format decoder.
// The decoder processes an encoded message by iterating over its fields. The
// caller can extract the values of any fields it cares about.
//
// The decoder does not provide any in-memory data structures to represent a
// protobuf message's data. More sophisticated APIs can be built on top of the
// low-level decoder to provide additional functionality, if desired.
//
// Example usage:
//
//   Decoder decoder(proto);
//   while (decoder.Next().ok()) {
//     switch (decoder.FieldNumber()) {
//       case 1:
//         decoder.ReadUint32(&my_uint32);
//         break;
//       // ... and other fields.
//     }
//   }
//
namespace pw::protobuf {

// TODO(frolv): Rename this to MemoryDecoder to match the encoder naming.
class Decoder {
 public:
  constexpr Decoder(span<const std::byte> proto)
      : proto_(proto), previous_field_consumed_(true) {}

  Decoder(const Decoder& other) = delete;
  Decoder& operator=(const Decoder& other) = delete;

  // Advances to the next field in the proto.
  //
  // If Next() returns OK, there is guaranteed to be a valid protobuf field at
  // the current cursor position.
  //
  // Return values:
  //
  //             OK: Advanced to a valid proto field.
  //   OUT_OF_RANGE: Reached the end of the proto message.
  //      DATA_LOSS: Invalid protobuf data.
  //
  Status Next();

  // Returns the field number of the field at the current cursor position.
  //
  // A return value of 0 indicates that the field number is invalid. An invalid
  // field number terminates the decode operation; any subsequent calls to
  // Next() or Read*() will return DATA_LOSS.
  //
  // TODO(frolv): This should be refactored to return a Result<uint32_t>.
  uint32_t FieldNumber() const;

  // Reads a proto int32 value from the current cursor.
  Status ReadInt32(int32_t* out) {
    return ReadUint32(reinterpret_cast<uint32_t*>(out));
  }

  // Reads a proto uint32 value from the current cursor.
  Status ReadUint32(uint32_t* out);

  // Reads a proto int64 value from the current cursor.
  Status ReadInt64(int64_t* out) {
    return ReadVarint(reinterpret_cast<uint64_t*>(out));
  }

  // Reads a proto uint64 value from the current cursor.
  Status ReadUint64(uint64_t* out) { return ReadVarint(out); }

  // Reads a proto sint32 value from the current cursor.
  Status ReadSint32(int32_t* out);

  // Reads a proto sint64 value from the current cursor.
  Status ReadSint64(int64_t* out);

  // Reads a proto bool value from the current cursor.
  Status ReadBool(bool* out);

  // Reads a proto fixed32 value from the current cursor.
  Status ReadFixed32(uint32_t* out) { return ReadFixed(out); }

  // Reads a proto fixed64 value from the current cursor.
  Status ReadFixed64(uint64_t* out) { return ReadFixed(out); }

  // Reads a proto sfixed32 value from the current cursor.
  Status ReadSfixed32(int32_t* out) {
    return ReadFixed32(reinterpret_cast<uint32_t*>(out));
  }

  // Reads a proto sfixed64 value from the current cursor.
  Status ReadSfixed64(int64_t* out) {
    return ReadFixed64(reinterpret_cast<uint64_t*>(out));
  }

  // Reads a proto float value from the current cursor.
  Status ReadFloat(float* out) {
    static_assert(sizeof(float) == sizeof(uint32_t),
                  "Float and uint32_t must be the same size for protobufs");
    return ReadFixed(out);
  }

  // Reads a proto double value from the current cursor.
  Status ReadDouble(double* out) {
    static_assert(sizeof(double) == sizeof(uint64_t),
                  "Double and uint64_t must be the same size for protobufs");
    return ReadFixed(out);
  }

  // Reads a proto string value from the current cursor and returns a view of it
  // in `out`. The raw protobuf data must outlive `out`. If the string field is
  // invalid, `out` is not modified.
  Status ReadString(std::string_view* out);

  // Reads a proto bytes value from the current cursor and returns a view of it
  // in `out`. The raw protobuf data must outlive the `out` span. If the
  // bytes field is invalid, `out` is not modified.
  Status ReadBytes(span<const std::byte>* out) { return ReadDelimited(out); }

  // Resets the decoder to start reading a new proto message.
  void Reset(span<const std::byte> proto) {
    proto_ = proto;
    previous_field_consumed_ = true;
  }

 private:
  // Advances the cursor to the next field in the proto.
  Status SkipField();

  // Returns the size of the current field, or 0 if the field is invalid.
  size_t FieldSize() const;

  Status ConsumeKey(WireType expected_type);

  // Reads a varint key-value pair from the current cursor position.
  Status ReadVarint(uint64_t* out);

  // Reads a fixed-size key-value pair from the current cursor position.
  Status ReadFixed(std::byte* out, size_t size);

  template <typename T>
  Status ReadFixed(T* out) {
    static_assert(
        sizeof(T) == sizeof(uint32_t) || sizeof(T) == sizeof(uint64_t),
        "Protobuf fixed-size fields must be 32- or 64-bit");
    return ReadFixed(reinterpret_cast<std::byte*>(out), sizeof(T));
  }

  Status ReadDelimited(span<const std::byte>* out);

  span<const std::byte> proto_;
  bool previous_field_consumed_;
};

class DecodeHandler;

// A protobuf decoder that iterates over an encoded protobuf, calling a handler
// for each field it encounters.
//
// Example usage:
//
//   class FooProtoHandler : public DecodeHandler {
//    public:
//     Status ProcessField(CallbackDecoder& decoder,
//                         uint32_t field_number) override {
//       switch (field_number) {
//         case FooFields::kBar:
//           if (!decoder.ReadSint32(&bar).ok()) {
//             bar = 0;
//           }
//           break;
//         case FooFields::kBaz:
//           if (!decoder.ReadUint32(&baz).ok()) {
//             baz = 0;
//           }
//           break;
//       }
//
//       return OkStatus();
//     }
//
//     int bar;
//     unsigned int baz;
//   };
//
//   void DecodeFooProto(span<std::byte> raw_proto) {
//     Decoder decoder;
//     FooProtoHandler handler;
//
//     decoder.set_handler(&handler);
//     if (!decoder.Decode(raw_proto).ok()) {
//       LOG_FATAL("Invalid foo message!");
//     }
//
//     LOG_INFO("Read Foo proto message; bar: %d baz: %u",
//              handler.bar, handler.baz);
//   }
//
class CallbackDecoder {
 public:
  constexpr CallbackDecoder()
      : decoder_({}), handler_(nullptr), state_(kReady) {}

  CallbackDecoder(const CallbackDecoder& other) = delete;
  CallbackDecoder& operator=(const CallbackDecoder& other) = delete;

  void set_handler(DecodeHandler* handler) { handler_ = handler; }

  // Decodes the specified protobuf data. The registered handler's ProcessField
  // function is called on each field found in the data.
  Status Decode(span<const std::byte> proto);

  // Reads a proto int32 value from the current cursor.
  Status ReadInt32(int32_t* out) { return decoder_.ReadInt32(out); }

  // Reads a proto uint32 value from the current cursor.
  Status ReadUint32(uint32_t* out) { return decoder_.ReadUint32(out); }

  // Reads a proto int64 value from the current cursor.
  Status ReadInt64(int64_t* out) { return decoder_.ReadInt64(out); }

  // Reads a proto uint64 value from the current cursor.
  Status ReadUint64(uint64_t* out) { return decoder_.ReadUint64(out); }

  // Reads a proto sint64 value from the current cursor.
  Status ReadSint32(int32_t* out) { return decoder_.ReadSint32(out); }

  // Reads a proto sint64 value from the current cursor.
  Status ReadSint64(int64_t* out) { return decoder_.ReadSint64(out); }

  // Reads a proto bool value from the current cursor.
  Status ReadBool(bool* out) { return decoder_.ReadBool(out); }

  // Reads a proto fixed32 value from the current cursor.
  Status ReadFixed32(uint32_t* out) { return decoder_.ReadFixed32(out); }

  // Reads a proto fixed64 value from the current cursor.
  Status ReadFixed64(uint64_t* out) { return decoder_.ReadFixed64(out); }

  // Reads a proto sfixed32 value from the current cursor.
  Status ReadSfixed32(int32_t* out) { return decoder_.ReadSfixed32(out); }

  // Reads a proto sfixed64 value from the current cursor.
  Status ReadSfixed64(int64_t* out) { return decoder_.ReadSfixed64(out); }

  // Reads a proto float value from the current cursor.
  Status ReadFloat(float* out) { return decoder_.ReadFloat(out); }

  // Reads a proto double value from the current cursor.
  Status ReadDouble(double* out) { return decoder_.ReadDouble(out); }

  // Reads a proto string value from the current cursor and returns a view of it
  // in `out`. The raw protobuf data must outlive `out`. If the string field is
  // invalid, `out` is not modified.
  Status ReadString(std::string_view* out) { return decoder_.ReadString(out); }

  // Reads a proto bytes value from the current cursor and returns a view of it
  // in `out`. The raw protobuf data must outlive the `out` span. If the
  // bytes field is invalid, `out` is not modified.
  Status ReadBytes(span<const std::byte>* out) {
    return decoder_.ReadBytes(out);
  }

  bool cancelled() const { return state_ == kDecodeCancelled; }

 private:
  enum State {
    kReady,
    kDecodeInProgress,
    kDecodeCancelled,
    kDecodeFailed,
  };

  Decoder decoder_;
  DecodeHandler* handler_;

  State state_;
};

// The event-handling interface implemented for a proto callback decoding
// operation.
class DecodeHandler {
 public:
  virtual ~DecodeHandler() = default;

  // Callback called for each field encountered in the decoded proto message.
  // Receives a pointer to the decoder object, allowing the handler to call
  // the appropriate method to extract the field's data.
  //
  // If the status returned is not OkStatus(), the decode operation is exited
  // with the provided status. Returning Status::Cancelled() allows a convenient
  // way of stopping a decode early (for example, if a desired field is found).
  virtual Status ProcessField(CallbackDecoder& decoder,
                              uint32_t field_number) = 0;
};

}  // namespace pw::protobuf
