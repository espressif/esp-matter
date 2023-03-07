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
// BUILD
// ninja -C out
// pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_filter
//
// RUN
// ./out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_filter
// trace.bin
//
// DECODE
// python pw_trace_tokenized/py/trace_tokenized.py -i trace.bin -o trace.json
// ./out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_basic#trace
//
// VIEW
// In chrome navigate to chrome://tracing, and load the trace.json file.

#include "pw_log/log.h"
#include "pw_trace/example/sample_app.h"
#include "pw_trace/trace.h"
#include "pw_trace_tokenized/example/trace_to_file.h"
#include "pw_trace_tokenized/trace_callback.h"
#include "pw_trace_tokenized/trace_tokenized.h"

pw_trace_TraceEventReturnFlags TraceEventCallback(
    void* /* user_data */,
    uint32_t /* trace_ref */,
    pw_trace_EventType /* event_type */,
    const char* module,
    uint32_t trace_id,
    uint8_t /* flags */) {
  // Filter out all traces from processing task, which aren't traceId 3
  static constexpr uint32_t kFilterId = 3;
  return (strcmp("Processing", module) == 0 && trace_id != kFilterId)
             ? PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT
             : 0;
}

int main(int argc, char** argv) {  // Take filename as arg
  if (argc != 2) {
    PW_LOG_ERROR("Expected output file name as argument.\n");
    return -1;
  }

  // Register filter callback
  pw::trace::Callbacks::Instance()
      .RegisterEventCallback(TraceEventCallback)
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  PW_TRACE_SET_ENABLED(true);  // Start with tracing enabled

  // Dump trace data to the file passed in.
  pw::trace::TraceToFile trace_to_file(argv[1]);

  PW_LOG_INFO("Running filter example...");
  RunTraceSampleApp();
  return 0;
}
