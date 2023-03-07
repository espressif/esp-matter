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

#include "pw_assert/assert.h"
#include "pw_containers/intrusive_list.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"
#include "pw_transfer/internal/event.h"

namespace pw::transfer {
namespace internal {

class Context;

}  // namespace internal

// The Handler class is the base class for the transfer handler classes.
// Transfer handlers connect a transfer resource ID to a data stream, wrapped
// with initialization and cleanup procedures.
//
// Handlers use a stream::Reader or stream::Writer to do the reads and writes.
// They also provide optional Prepare and Finalize functions.
class Handler : public IntrusiveList<Handler>::Item {
 public:
  virtual ~Handler() = default;

  constexpr uint32_t id() const { return resource_id_; }

  // Called at the beginning of a read transfer. The stream::Reader must be
  // ready to read after a successful PrepareRead() call. Returning a non-OK
  // status aborts the read.
  //
  // Status::Unimplemented() indicates that reads are not supported.
  virtual Status PrepareRead() = 0;

  // FinalizeRead() is called at the end of a read transfer. The status argument
  // indicates whether the data transfer was successful or not.
  virtual void FinalizeRead(Status) {}

  // Called at the beginning of a write transfer. The stream::Writer must be
  // ready to read after a successful PrepareRead() call. Returning a non-OK
  // status aborts the write.
  //
  // Status::Unimplemented() indicates that writes are not supported.
  virtual Status PrepareWrite() = 0;

  // FinalizeWrite() is called at the end of a write transfer. The status
  // argument indicates whether the data transfer was successful or not.
  //
  // Returning an error signals that the transfer failed, even if it had
  // succeeded up to this point.
  virtual Status FinalizeWrite(Status) { return OkStatus(); }

 protected:
  constexpr Handler(uint32_t resource_id, stream::Reader* reader)
      : resource_id_(resource_id), reader_(reader) {}

  constexpr Handler(uint32_t resource_id, stream::Writer* writer)
      : resource_id_(resource_id), writer_(writer) {}

  void set_reader(stream::Reader& reader) { reader_ = &reader; }
  void set_writer(stream::Writer& writer) { writer_ = &writer; }

 private:
  friend class internal::Context;

  // Prepares for either a read or write transfer.
  Status Prepare(internal::TransferType type) {
    return type == internal::TransferType::kTransmit ? PrepareRead()
                                                     : PrepareWrite();
  }

  // Only valid after a PrepareRead() or PrepareWrite() call that returns OK.
  stream::Stream& stream() const {
    PW_DASSERT(reader_ != nullptr);
    return *reader_;
  }

  uint32_t resource_id_;

  // Use a union to support constexpr construction.
  union {
    stream::Reader* reader_;
    stream::Writer* writer_;
  };
};

class ReadOnlyHandler : public Handler {
 public:
  constexpr ReadOnlyHandler(uint32_t resource_id)
      : Handler(resource_id, static_cast<stream::Reader*>(nullptr)) {}

  constexpr ReadOnlyHandler(uint32_t resource_id, stream::Reader& reader)
      : Handler(resource_id, &reader) {}

  ~ReadOnlyHandler() override = default;

  Status PrepareRead() override { return OkStatus(); }

  // Writes are not supported.
  Status PrepareWrite() final { return Status::PermissionDenied(); }

  using Handler::set_reader;

 private:
  using Handler::set_writer;
};

class WriteOnlyHandler : public Handler {
 public:
  constexpr WriteOnlyHandler(uint32_t resource_id)
      : Handler(resource_id, static_cast<stream::Writer*>(nullptr)) {}

  constexpr WriteOnlyHandler(uint32_t resource_id, stream::Writer& writer)
      : Handler(resource_id, &writer) {}

  ~WriteOnlyHandler() override = default;

  // Reads are not supported.
  Status PrepareRead() final { return Status::PermissionDenied(); }

  Status PrepareWrite() override { return OkStatus(); }

  using Handler::set_writer;

 private:
  using Handler::set_reader;
};

class ReadWriteHandler : public Handler {
 public:
  constexpr ReadWriteHandler(uint32_t resource_id)
      : Handler(resource_id, static_cast<stream::Reader*>(nullptr)) {}
  constexpr ReadWriteHandler(uint32_t resource_id,
                             stream::ReaderWriter& reader_writer)
      : Handler(resource_id, &static_cast<stream::Reader&>(reader_writer)) {}

  ~ReadWriteHandler() override = default;

  // Both reads and writes are supported.
  Status PrepareRead() override { return OkStatus(); }
  Status PrepareWrite() override { return OkStatus(); }

  void set_reader_writer(stream::ReaderWriter& reader_writer) {
    set_reader(reader_writer);
  }
};

}  // namespace pw::transfer
