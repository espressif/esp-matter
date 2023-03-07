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
//==============================================================================
/*

NOTE
To use this example you need to enable nanopb, one option is to set this in
either your out/args.gn or the root .gn:
default_args = {
  dir_pw_third_party_nanopb = "<path to nanopb repo>"
}

BUILD
ninja -C out
pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_rpc

RUN
./out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_rpc

DECODE
python pw_trace_tokenized/py/pw_trace_tokenized/get_trace.py
 -s localhost:33000
 -o trace.json
 -t
 out/pw_strict_host_clang_debug/obj/pw_trace_tokenized/bin/trace_tokenized_example_rpc
 pw_trace_tokenized/pw_trace_protos/trace_rpc.proto

VIEW
In chrome navigate to chrome://tracing, and load the trace.json file.
*/
#include <thread>

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_rpc/server.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_trace/example/sample_app.h"
#include "pw_trace/trace.h"
#include "pw_trace_tokenized/trace_rpc_service_nanopb.h"

namespace {

pw::trace::TraceService trace_service;

void RpcThread() {
  pw::rpc::system_server::Init();

  // Set up the server and start processing data.
  pw::rpc::system_server::Server().RegisterService(trace_service);
  PW_CHECK_OK(pw::rpc::system_server::Start());
}

}  // namespace

int main() {
  std::thread rpc_thread(RpcThread);

  // Enable tracing.
  PW_TRACE_SET_ENABLED(true);

  PW_LOG_INFO("Running basic trace example...\n");
  RunTraceSampleApp();
  return 0;
}
