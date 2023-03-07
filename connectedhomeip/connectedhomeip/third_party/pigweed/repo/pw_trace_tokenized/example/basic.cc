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
// pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_basic
//
// RUN
// ./out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_basic
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
#include "pw_trace_tokenized/example/trace_to_file.h"

int main(int argc, char** argv) {  // Take filename as arg
  if (argc != 2) {
    PW_LOG_ERROR("Expected output file name as argument.\n");
    return -1;
  }

  // Enable tracing.
  PW_TRACE_SET_ENABLED(true);

  // Dump trace data to the file passed in.
  pw::trace::TraceToFile trace_to_file(argv[1]);

  PW_LOG_INFO("Running basic trace example...\n");
  RunTraceSampleApp();
  return 0;
}
