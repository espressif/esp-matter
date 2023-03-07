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
//
// The header provides a set of helper utils for protobuf related operations.
// The APIs may not be finalized yet.

#pragma once

#include <cstddef>
#include <string_view>

#include "pw_assert/check.h"
#include "pw_protobuf/internal/proto_integer_base.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_stream/interval_reader.h"
#include "pw_stream/stream.h"

namespace pw::protobuf {

// The following defines classes that represent various parsed proto integer
// types or an error code to indicate parsing failure.
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.

class Uint32 : public internal::ProtoIntegerBase<uint32_t> {
 public:
  using ProtoIntegerBase<uint32_t>::ProtoIntegerBase;
};

class Int32 : public internal::ProtoIntegerBase<int32_t> {
 public:
  using ProtoIntegerBase<int32_t>::ProtoIntegerBase;
};

class Sint32 : public internal::ProtoIntegerBase<int32_t> {
 public:
  using ProtoIntegerBase<int32_t>::ProtoIntegerBase;
};

class Fixed32 : public internal::ProtoIntegerBase<uint32_t> {
 public:
  using ProtoIntegerBase<uint32_t>::ProtoIntegerBase;
};

class Sfixed32 : public internal::ProtoIntegerBase<int32_t> {
 public:
  using ProtoIntegerBase<int32_t>::ProtoIntegerBase;
};

class Uint64 : public internal::ProtoIntegerBase<uint64_t> {
 public:
  using ProtoIntegerBase<uint64_t>::ProtoIntegerBase;
};

class Int64 : public internal::ProtoIntegerBase<int64_t> {
 public:
  using ProtoIntegerBase<int64_t>::ProtoIntegerBase;
};

class Sint64 : public internal::ProtoIntegerBase<int64_t> {
 public:
  using ProtoIntegerBase<int64_t>::ProtoIntegerBase;
};

class Fixed64 : public internal::ProtoIntegerBase<uint64_t> {
 public:
  using ProtoIntegerBase<uint64_t>::ProtoIntegerBase;
};

class Sfixed64 : public internal::ProtoIntegerBase<int64_t> {
 public:
  using ProtoIntegerBase<int64_t>::ProtoIntegerBase;
};

class Float : public internal::ProtoIntegerBase<float> {
 public:
  using ProtoIntegerBase<float>::ProtoIntegerBase;
};

class Double : public internal::ProtoIntegerBase<double> {
 public:
  using ProtoIntegerBase<double>::ProtoIntegerBase;
};

class Bool : public internal::ProtoIntegerBase<bool> {
 public:
  using ProtoIntegerBase<bool>::ProtoIntegerBase;
};

// An object that represents a parsed `bytes` field or an error code. The
// bytes are available via an stream::IntervalReader by GetBytesReader().
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.
class Bytes {
 public:
  Bytes() = default;
  Bytes(Status status) : reader_(status) {}
  Bytes(stream::IntervalReader reader) : reader_(reader) {}
  stream::IntervalReader GetBytesReader() { return reader_; }

  bool ok() { return reader_.ok(); }
  Status status() { return reader_.status(); }

  // Check whether the bytes value equals the given `bytes`.
  Result<bool> Equal(ConstByteSpan bytes);

 private:
  stream::IntervalReader reader_;
};

// An object that represents a parsed `string` field or an error code. The
// string value is available via an stream::IntervalReader by
// GetBytesReader().
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.
class String : public Bytes {
 public:
  using Bytes::Bytes;

  // Check whether the string value equals the given `str`
  Result<bool> Equal(std::string_view str);
};

// Forward declaration of parser classes.
template <typename FieldType>
class RepeatedFieldParser;
template <typename FieldType>
class StringMapEntryParser;
template <typename FieldType>
class StringMapParser;
class Message;

using RepeatedBytes = RepeatedFieldParser<Bytes>;
using RepeatedStrings = RepeatedFieldParser<String>;
using RepeatedMessages = RepeatedFieldParser<Message>;
using StringToBytesMapEntry = StringMapEntryParser<Bytes>;
using StringToStringMapEntry = StringMapEntryParser<String>;
using StringToMessageMapEntry = StringMapEntryParser<Message>;
using StringToBytesMap = StringMapParser<Bytes>;
using StringToStringMap = StringMapParser<String>;
using StringToMessageMap = StringMapParser<Message>;

// Message - A helper class for parsing a proto message.
//
// Examples:
//
// message Nested {
//   string nested_str = 1;
//   bytes nested_bytes = 2;
// }
//
// message {
//   string str = 1;
//   bytes bytes = 2;
//   uint32 integer = 3
//   repeated string rep_str = 4;
//   map<string, bytes> str_to_bytes = 5;
//   Nested nested = 6;
// }
//
//   // Given a seekable `reader` that reads the top-level proto message, and
//   // a <size> that gives the size of the proto message:
//   Message message(reader, <size>);
//
//   // Prase simple basic value fields
//   String str = message.AsString(1); // string
//   Bytes bytes = message.AsBytes(2); // bytes
//   Uint32 integer = messasge_parser.AsUint32(3); // uint32 integer
//
//   // Parse repeated field `repeated string rep_str = 4;`
//   RepeatedStrings rep_str = message.AsRepeatedString(4);
//   // Iterate through the entries. If proto is malformed when
//   // iterating, the next element (`str` in this case) will be invalid
//   // and loop will end in the iteration after.
//   for (String str : rep_str) {
//     // Check status
//     if (!str.ok()) {
//       // In the case of error, loop will end in the next iteration if
//       // continues. This is the chance for code to catch the error.
//       ...
//     }
//     ...
//   }
//
//   // Parse map field `map<string, bytes> str_to_bytes = 5;`
//   StringToBytesMap str_to_bytes = message.AsStringToBytesMap(5);
//
//   // Access the entry by a given key value
//   Bytes bytes_for_key = str_to_bytes["key"];
//
//   // Or iterate through map entries
//   for (StringToBytesMapEntry entry : str_to_bytes) {
//     if (!entry.ok()) {
//       // In the case of error, loop will end in the next iteration if
//       // continues. This is the chance for code to catch the error.
//       ...
//     }
//     String key = entry.Key();
//     Bytes value = entry.Value();
//     ...
//   }
//
//   // Parse nested message `Nested nested = 6;`
//   Message nested = message.AsMessage(6).
//   String nested_str = nested.AsString(1);
//   Bytes nested_bytes = nested.AsBytes(2);
//
//   // The `AsXXX()` methods above internally traverse all the fields to find
//   // the one with the give field number. This can be expensive if called
//   // multiple times. Therefore, whenever possible, it is recommended to use
//   // the following iteration to iterate and process each field directly.
//   for (Message::Field field : message) {
//     if (!field.ok()) {
//       // In the case of error, loop will end in the next iteration if
//       // continues. This is the chance for code to catch the error.
//       ...
//     }
//     if (field.field_number() == 1) {
//       String str = field.As<String>();
//       ...
//     } else if (field.field_number() == 2) {
//       Bytes bytes = field.As<Bytes>();
//       ...
//     } else if (field.field_number() == 6) {
//       Message nested = field.As<Message>();
//       ...
//     }
//   }
//
// All parser objects created above internally hold the same reference
// to `reader`. Therefore it needs to maintain valid lifespan throughout the
// operations. The parser objects can work independently and without blocking
// each other. All method calls and for-iterations above are re-enterable.
class Message {
 public:
  class Field {
   public:
    uint32_t field_number() { return field_number_; }
    const stream::IntervalReader& field_reader() { return field_reader_; }
    bool ok() { return field_reader_.ok(); }
    Status status() { return field_reader_.status(); }

    // Create a helper parser type of `FieldType` for the field.
    // The default implementation below assumes the field is a length-delimited
    // field. Other cases such as primitive integer uint32 will be handled by
    // template specialization.
    template <typename FieldType>
    FieldType As() {
      if (!field_reader_.ok()) {
        return FieldType(field_reader_.status());
      }

      StreamDecoder decoder(field_reader_.Reset());
      PW_TRY(decoder.Next());
      Result<StreamDecoder::Bounds> payload_bounds =
          decoder.GetLengthDelimitedPayloadBounds();
      PW_TRY(payload_bounds.status());
      // The bounds is relative to the given stream::IntervalReader. Convert
      // it to the interval relative to the source_reader.
      return FieldType(stream::IntervalReader(
          field_reader_.source_reader(),
          payload_bounds.value().low + field_reader_.start(),
          payload_bounds.value().high + field_reader_.start()));
    }

   private:
    Field() = default;
    Field(Status status) : field_reader_(status), field_number_(0) {}
    Field(stream::IntervalReader reader, uint32_t field_number)
        : field_reader_(reader), field_number_(field_number) {}

    stream::IntervalReader field_reader_;
    uint32_t field_number_;

    friend class Message;
  };

  class iterator {
   public:
    iterator& operator++();

    iterator operator++(int) {
      iterator iter = *this;
      this->operator++();
      return iter;
    }

    bool ok() { return status_.ok(); }
    Status status() { return status_; }
    Field operator*() { return current_; }
    Field* operator->() { return &current_; }
    bool operator!=(const iterator& other) const { return !(*this == other); }

    bool operator==(const iterator& other) const {
      return eof_ == other.eof_ && reader_ == other.reader_;
    }

   private:
    stream::IntervalReader reader_;
    bool eof_ = false;
    Field current_;
    Status status_ = OkStatus();

    iterator(stream::IntervalReader reader) : reader_(reader) {
      this->operator++();
    }

    friend class Message;
  };

  Message() = default;
  Message(Status status) : reader_(status) {}
  Message(stream::IntervalReader reader) : reader_(reader) {}
  Message(stream::SeekableReader& proto_source, size_t size)
      : reader_(proto_source, 0, size) {}

  // Parse a sub-field in the message given by `field_number` as bytes.
  Bytes AsBytes(uint32_t field_number) { return As<Bytes>(field_number); }

  // Parse a sub-field in the message given by `field_number` as string.
  String AsString(uint32_t field_number) { return As<String>(field_number); }

  // Parse a sub-field in the message given by `field_number` as one of the
  // proto integer type.
  Int32 AsInt32(uint32_t field_number) { return As<Int32>(field_number); }
  Sint32 AsSint32(uint32_t field_number) { return As<Sint32>(field_number); }
  Uint32 AsUint32(uint32_t field_number) { return As<Uint32>(field_number); }
  Fixed32 AsFixed32(uint32_t field_number) { return As<Fixed32>(field_number); }
  Int64 AsInt64(uint32_t field_number) { return As<Int64>(field_number); }
  Sint64 AsSint64(uint32_t field_number) { return As<Sint64>(field_number); }
  Uint64 AsUint64(uint32_t field_number) { return As<Uint64>(field_number); }
  Fixed64 AsFixed64(uint32_t field_number) { return As<Fixed64>(field_number); }

  Sfixed32 AsSfixed32(uint32_t field_number) {
    return As<Sfixed32>(field_number);
  }

  Sfixed64 AsSfixed64(uint32_t field_number) {
    return As<Sfixed64>(field_number);
  }

  Float AsFloat(uint32_t field_number) { return As<Float>(field_number); }
  Double AsDouble(uint32_t field_number) { return As<Double>(field_number); }

  Bool AsBool(uint32_t field_number) { return As<Bool>(field_number); }

  // Parse a sub-field in the message given by `field_number` as another
  // message.
  Message AsMessage(uint32_t field_number) { return As<Message>(field_number); }

  // Parse a sub-field in the message given by `field_number` as `repeated
  // string`.
  RepeatedBytes AsRepeatedBytes(uint32_t field_number);

  // Parse a sub-field in the message given by `field_number` as `repeated
  // string`.
  RepeatedStrings AsRepeatedStrings(uint32_t field_number);

  // Parse a sub-field in the message given by `field_number` as `repeated
  // message`.
  RepeatedMessages AsRepeatedMessages(uint32_t field_number);

  // Parse a sub-field in the message given by `field_number` as `map<string,
  // message>`.
  StringToMessageMap AsStringToMessageMap(uint32_t field_number);

  // Parse a sub-field in the message given by `field_number` as
  // `map<string, bytes>`.
  StringToBytesMap AsStringToBytesMap(uint32_t field_number);

  // Parse a sub-field in the message given by `field_number` as
  // `map<string, string>`.
  StringToStringMap AsStringToStringMap(uint32_t field_number);

  // Convert the message to a Bytes that represents the raw bytes of this
  // message. This can be used to obatained the serialized wire-format of the
  // message.
  Bytes ToBytes() { return Bytes(reader_.Reset()); }

  bool ok() { return reader_.ok(); }
  Status status() { return reader_.status(); }

  iterator begin();
  iterator end();

  // Parse a field given by `field_number` as the target parser type
  // `FieldType`.
  //
  // Note: This method assumes that the message has only 1 field with the given
  // <field_number>. It returns the first matching it find. It does not perform
  // value overridding or string concatenation for multiple fields with the same
  // <field_number>.
  //
  // Since the method needs to traverse all fields, it can be inefficient if
  // called multiple times exepcially on slow reader.
  template <typename FieldType>
  FieldType As(uint32_t field_number) {
    for (Field field : *this) {
      if (field.field_number() == field_number) {
        return field.As<FieldType>();
      }
    }

    return FieldType(Status::NotFound());
  }

  template <typename FieldType>
  RepeatedFieldParser<FieldType> AsRepeated(uint32_t field_number) {
    return RepeatedFieldParser<FieldType>(*this, field_number);
  }

  template <typename FieldParser>
  StringMapParser<FieldParser> AsStringMap(uint32_t field_number) {
    return StringMapParser<FieldParser>(*this, field_number);
  }

 private:
  stream::IntervalReader reader_;

  // Consume the current field. If the field has already been processed, i.e.
  // by calling one of the Read..() method, nothing is done. After calling this
  // method, the reader will be pointing either to the start of the next
  // field (i.e. the starting offset of the field key), or the end of the
  // stream. The method is for use by Message for computing field interval.
  static Status ConsumeCurrentField(StreamDecoder& decoder) {
    return decoder.field_consumed_ ? OkStatus() : decoder.SkipField();
  }
};

// The following are template specialization for proto integer types.
template <>
Uint32 Message::Field::As<Uint32>();

template <>
Int32 Message::Field::As<Int32>();

template <>
Sint32 Message::Field::As<Sint32>();

template <>
Fixed32 Message::Field::As<Fixed32>();

template <>
Sfixed32 Message::Field::As<Sfixed32>();

template <>
Uint64 Message::Field::As<Uint64>();

template <>
Int64 Message::Field::As<Int64>();

template <>
Sint64 Message::Field::As<Sint64>();

template <>
Fixed64 Message::Field::As<Fixed64>();

template <>
Sfixed64 Message::Field::As<Sfixed64>();

template <>
Float Message::Field::As<Float>();

template <>
Double Message::Field::As<Double>();

template <>
Bool Message::Field::As<Bool>();

// A helper for parsing `repeated` field. It implements an iterator interface
// that only iterates through the fields of a given `field_number`.
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.
template <typename FieldType>
class RepeatedFieldParser {
 public:
  class iterator {
   public:
    // Precondition: iter_ is not pointing to the end.
    iterator& operator++() {
      iter_++;
      MoveToNext();
      return *this;
    }

    iterator operator++(int) {
      iterator iter = *this;
      this->operator++();
      return iter;
    }

    bool ok() { return iter_.ok(); }
    Status status() { return iter_.status(); }
    FieldType operator*() { return current_; }
    FieldType* operator->() { return &current_; }
    bool operator!=(const iterator& other) const { return !(*this == other); }
    bool operator==(const iterator& other) const {
      return &host_ == &other.host_ && iter_ == other.iter_;
    }

   private:
    RepeatedFieldParser& host_;
    Message::iterator iter_;
    FieldType current_ = FieldType(Status::Unavailable());

    iterator(RepeatedFieldParser& host, Message::iterator init_iter)
        : host_(host), iter_(init_iter), current_(Status::Unavailable()) {
      // Move to the first element of the target field number.
      MoveToNext();
    }

    void MoveToNext() {
      // Move the iterator to the next element with the target field number
      for (; iter_ != host_.message_.end(); ++iter_) {
        if (!iter_.ok() || iter_->field_number() == host_.field_number_) {
          current_ = iter_->As<FieldType>();
          break;
        }
      }
    }

    friend class RepeatedFieldParser;
  };

  // `message` -- The containing message.
  // `field_number` -- The field number of the repeated field.
  RepeatedFieldParser(Message& message, uint32_t field_number)
      : message_(message), field_number_(field_number) {}

  RepeatedFieldParser(Status status) : message_(status) {}

  bool ok() { return message_.ok(); }
  Status status() { return message_.status(); }

  iterator begin() { return iterator(*this, message_.begin()); }
  iterator end() { return iterator(*this, message_.end()); }

 private:
  Message message_;
  uint32_t field_number_ = 0;
};

// A helper for pasring the entry type of map<string, <value>>.
// An entry for a proto map is essentially a message of a key(k=1) and
// value(k=2) field, i.e.:
//
// message Entry {
//   string key = 1;
//   bytes value = 2;
// }
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.
template <typename ValueParser>
class StringMapEntryParser {
 public:
  bool ok() { return entry_.ok(); }
  Status status() { return entry_.status(); }
  StringMapEntryParser(Status status) : entry_(status) {}
  StringMapEntryParser(stream::IntervalReader reader) : entry_(reader) {}
  String Key() { return entry_.AsString(kMapKeyFieldNumber); }
  ValueParser Value() { return entry_.As<ValueParser>(kMapValueFieldNumber); }

 private:
  static constexpr uint32_t kMapKeyFieldNumber = 1;
  static constexpr uint32_t kMapValueFieldNumber = 2;
  Message entry_;
};

// A helper class for parsing a string-keyed map field. i.e. map<string,
// <value>>. The template argument `ValueParser` indicates the type the value
// will be parsed as, i.e. String, Bytes, Uint32, Message etc.
//
// For normal uses, the class should be created from `class Message`. See
// comment for `class Message` for usage.
template <typename ValueParser>
class StringMapParser
    : public RepeatedFieldParser<StringMapEntryParser<ValueParser>> {
 public:
  using RepeatedFieldParser<
      StringMapEntryParser<ValueParser>>::RepeatedFieldParser;

  // Operator overload for value access of a given key.
  ValueParser operator[](std::string_view target) {
    // Iterate over all entries and find the one whose key matches `target`
    for (StringMapEntryParser<ValueParser> entry : *this) {
      String key = entry.Key();
      PW_TRY(key.status());

      // Compare key value with the given string
      Result<bool> cmp_res = key.Equal(target);
      PW_TRY(cmp_res.status());
      if (cmp_res.value()) {
        return entry.Value();
      }
    }

    return ValueParser(Status::NotFound());
  }
};

}  // namespace pw::protobuf
