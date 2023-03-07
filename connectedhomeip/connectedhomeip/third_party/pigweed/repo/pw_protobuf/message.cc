// Copyright 2021 The Pigweed Authors
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

#include "pw_protobuf/message.h"

#include <cstddef>

#include "pw_protobuf/serialized_size.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_result/result.h"
#include "pw_status/status_with_size.h"
#include "pw_stream/interval_reader.h"
#include "pw_stream/stream.h"

namespace pw::protobuf {

template <>
Uint32 Message::Field::As<Uint32>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadUint32();
}

template <>
Int32 Message::Field::As<Int32>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadInt32();
}

template <>
Sint32 Message::Field::As<Sint32>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadSint32();
}

template <>
Fixed32 Message::Field::As<Fixed32>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadFixed32();
}

template <>
Sfixed32 Message::Field::As<Sfixed32>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadSfixed32();
}

template <>
Uint64 Message::Field::As<Uint64>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadUint64();
}

template <>
Int64 Message::Field::As<Int64>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadInt64();
}

template <>
Sint64 Message::Field::As<Sint64>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadSint64();
}

template <>
Fixed64 Message::Field::As<Fixed64>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadFixed64();
}

template <>
Sfixed64 Message::Field::As<Sfixed64>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadSfixed64();
}

template <>
Float Message::Field::As<Float>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadFloat();
}

template <>
Double Message::Field::As<Double>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadDouble();
}

template <>
Bool Message::Field::As<Bool>() {
  protobuf::StreamDecoder decoder(field_reader_.Reset());
  PW_TRY(decoder.Next());
  return decoder.ReadBool();
}

Result<bool> Bytes::Equal(ConstByteSpan bytes) {
  stream::IntervalReader bytes_reader = GetBytesReader();
  if (bytes_reader.interval_size() != bytes.size()) {
    return false;
  }

  std::byte buf[1];
  for (size_t i = 0; i < bytes.size();) {
    Result<ByteSpan> res = bytes_reader.Read(buf);
    PW_TRY(res.status());
    if (res.value().size() == 1) {
      if (buf[0] != bytes[i++])
        return false;
    }
  }

  return true;
}

Result<bool> String::Equal(std::string_view str) {
  return Bytes::Equal(as_bytes(span<const char>{str}));
}

Message::iterator& Message::iterator::operator++() {
  // If this is not a valid iterator, increment it to the end iterator,
  // so loop will end.
  if (!ok()) {
    reader_.Exhaust();
    eof_ = true;
    return *this;
  }

  // Store the starting offset of the field.
  size_t field_start = reader_.current();
  protobuf::StreamDecoder decoder(reader_);
  Status status = decoder.Next();
  if (status.IsOutOfRange()) {
    eof_ = true;
    return *this;
  } else if (!status.ok()) {
    // In the case of error, invalidate the iterator. We don't immediately
    // move the iterator to end(), so that calling code has a chance to catch
    // the error.
    status_ = status;
    current_ = Field(status_);
    return *this;
  }

  Result<uint32_t> field_number = decoder.FieldNumber();
  // Consume the field so that the reader will be pointing to the start
  // of the next field, which is equivalent to the end offset of the
  // current field.
  status = ConsumeCurrentField(decoder);
  if (!status.ok()) {
    status_ = status;
    current_ = Field(status_);
    return *this;
  }

  // Create a Field object with the field interval.
  current_ = Field(stream::IntervalReader(
                       reader_.source_reader(), field_start, reader_.current()),
                   field_number.value());
  return *this;
}

Message::iterator Message::begin() {
  if (!ok()) {
    return end();
  }

  return iterator(reader_.Reset());
}

Message::iterator Message::end() {
  // The end iterator is created by using an exahusted stream::IntervalReader,
  // i.e. the reader is pointing at the internval end.
  stream::IntervalReader reader_end = reader_;
  return iterator(reader_end.Exhaust());
}

RepeatedBytes Message::AsRepeatedBytes(uint32_t field_number) {
  return AsRepeated<Bytes>(field_number);
}

RepeatedFieldParser<String> Message::AsRepeatedStrings(uint32_t field_number) {
  return AsRepeated<String>(field_number);
}

RepeatedFieldParser<Message> Message::AsRepeatedMessages(
    uint32_t field_number) {
  return AsRepeated<Message>(field_number);
}

StringMapParser<Message> Message::AsStringToMessageMap(uint32_t field_number) {
  return AsStringMap<Message>(field_number);
}

StringMapParser<Bytes> Message::AsStringToBytesMap(uint32_t field_number) {
  return AsStringMap<Bytes>(field_number);
}

StringMapParser<String> Message::AsStringToStringMap(uint32_t field_number) {
  return AsStringMap<String>(field_number);
}

}  // namespace pw::protobuf
