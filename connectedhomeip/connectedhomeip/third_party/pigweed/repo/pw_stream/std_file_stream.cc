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

#include "pw_stream/std_file_stream.h"

#include "pw_assert/check.h"

namespace pw::stream {
namespace {

std::ios::seekdir WhenceToSeekDir(Stream::Whence whence) {
  switch (whence) {
    case Stream::Whence::kBeginning:
      return std::ios::beg;
    case Stream::Whence::kCurrent:
      return std::ios::cur;
    case Stream::Whence::kEnd:
      return std::ios::end;
  }
  PW_CRASH("Unknown value for enum Stream::Whence");
}

}  // namespace

StatusWithSize StdFileReader::DoRead(ByteSpan dest) {
  stream_.peek();  // Peek to set EOF if at the end of the file.
  if (stream_.eof()) {
    return StatusWithSize::OutOfRange();
  }

  stream_.read(reinterpret_cast<char*>(dest.data()), dest.size());
  if (stream_.bad()) {
    return StatusWithSize::Unknown();
  }

  return StatusWithSize(stream_.gcount());
}

Status StdFileReader::DoSeek(ptrdiff_t offset, Whence origin) {
  // Explicitly clear EOF bit if needed.
  if (stream_.eof()) {
    stream_.clear();
  }
  if (!stream_.seekg(offset, WhenceToSeekDir(origin))) {
    return Status::Unknown();
  }
  return OkStatus();
}

size_t StdFileReader::DoTell() {
  auto pos = static_cast<int>(stream_.tellg());
  return pos < 0 ? kUnknownPosition : pos;
}

size_t StdFileReader::ConservativeLimit(LimitType limit) const {
  if (limit == LimitType::kWrite) {
    return 0;
  }

  // Attempt to determine the number of bytes left in the file by seeking
  // to the end and checking where we end up.
  if (stream_.eof()) {
    return 0;
  }
  auto stream = const_cast<std::ifstream*>(&this->stream_);
  auto start = stream->tellg();
  if (start == -1) {
    return 0;
  }
  stream->seekg(0, std::ios::end);
  auto end = stream->tellg();
  if (end == -1) {
    return 0;
  }
  stream->seekg(start, std::ios::beg);
  return end - start;
}

Status StdFileWriter::DoWrite(ConstByteSpan data) {
  if (stream_.eof()) {
    return Status::OutOfRange();
  }

  if (stream_.write(reinterpret_cast<const char*>(data.data()), data.size())) {
    return OkStatus();
  }

  return Status::Unknown();
}

Status StdFileWriter::DoSeek(ptrdiff_t offset, Whence origin) {
  if (!stream_.seekp(offset, WhenceToSeekDir(origin))) {
    return Status::Unknown();
  }
  return OkStatus();
}

size_t StdFileWriter::DoTell() {
  auto pos = static_cast<int>(stream_.tellp());
  return pos < 0 ? kUnknownPosition : pos;
}

}  // namespace pw::stream
