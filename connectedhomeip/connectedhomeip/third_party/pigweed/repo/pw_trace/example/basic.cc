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
// ninja -C out/host trace_sample
// ./out/host/obj/pw_trace_tokenized/trace_sample
// python pw_trace_tokenized/py/trace.py -i out1.bin -o trace.json
//       ./out/host/obj/pw_trace_tokenized/trace_sample

#include "pw_log/log.h"
#include "pw_trace/example/sample_app.h"

int main() {
  PW_LOG_ERROR("Running basic trace example...\n");
  RunTraceSampleApp();
  return 0;
}