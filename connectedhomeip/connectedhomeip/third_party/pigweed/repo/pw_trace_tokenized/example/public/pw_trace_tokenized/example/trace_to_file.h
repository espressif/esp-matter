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
//==============================================================================
//

#pragma once

#include <stdio.h>

#include <fstream>

#include "pw_trace/example/sample_app.h"
#include "pw_trace_tokenized/trace_callback.h"

namespace pw {
namespace trace {

class TraceToFile {
 public:
  TraceToFile(const char* file_name) {
    Callbacks::Instance()
        .RegisterSink(TraceSinkStartBlock,
                      TraceSinkAddBytes,
                      TraceSinkEndBlock,
                      &out_,
                      &sink_handle_)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    out_.open(file_name, std::ios::out | std::ios::binary);
  }

  ~TraceToFile() {
    Callbacks::Instance()
        .UnregisterSink(sink_handle_)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    out_.close();
  }

  static void TraceSinkStartBlock(void* user_data, size_t size) {
    std::ofstream* out = reinterpret_cast<std::ofstream*>(user_data);
    uint8_t b = static_cast<uint8_t>(size);
    out->write(reinterpret_cast<const char*>(&b), sizeof(b));
  }

  static void TraceSinkAddBytes(void* user_data,
                                const void* bytes,
                                size_t size) {
    std::ofstream* out = reinterpret_cast<std::ofstream*>(user_data);
    out->write(reinterpret_cast<const char*>(bytes), size);
  }

  static void TraceSinkEndBlock(void* user_data) {
    std::ofstream* out = reinterpret_cast<std::ofstream*>(user_data);
    out->flush();
  }

 private:
  std::ofstream out_;
  CallbacksImpl::SinkHandle sink_handle_;
};

}  // namespace trace
}  // namespace pw
