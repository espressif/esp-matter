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

#include "FreeRTOS.h"
#include "pw_system/init.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread/thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

namespace pw::system {

// Low to high priorities.
enum class ThreadPriority : UBaseType_t {
  kWorkQueue = tskIDLE_PRIORITY + 1,
  // TODO(amontanez): These should ideally be at different priority levels, but
  // there's synchronization issues when they are.
  kLog = kWorkQueue,
  kRpc = kWorkQueue,
  kNumPriorities,
};

static_assert(static_cast<UBaseType_t>(ThreadPriority::kNumPriorities) <=
              configMAX_PRIORITIES);

static constexpr size_t kLogThreadStackWords = 1024;
static thread::freertos::StaticContextWithStack<kLogThreadStackWords>
    log_thread_context;
const thread::Options& LogThreadOptions() {
  static constexpr auto options =
      pw::thread::freertos::Options()
          .set_name("LogThread")
          .set_static_context(log_thread_context)
          .set_priority(static_cast<UBaseType_t>(ThreadPriority::kLog));
  return options;
}

static constexpr size_t kRpcThreadStackWords = 512;
static thread::freertos::StaticContextWithStack<kRpcThreadStackWords>
    rpc_thread_context;
const thread::Options& RpcThreadOptions() {
  static constexpr auto options =
      pw::thread::freertos::Options()
          .set_name("RpcThread")
          .set_static_context(rpc_thread_context)
          .set_priority(static_cast<UBaseType_t>(ThreadPriority::kRpc));
  return options;
}

static constexpr size_t kWorkQueueThreadStackWords = 512;
static thread::freertos::StaticContextWithStack<kWorkQueueThreadStackWords>
    work_queue_thread_context;
const thread::Options& WorkQueueThreadOptions() {
  static constexpr auto options =
      pw::thread::freertos::Options()
          .set_name("WorkQueueThread")
          .set_static_context(work_queue_thread_context)
          .set_priority(static_cast<UBaseType_t>(ThreadPriority::kWorkQueue));
  return options;
}

}  // namespace pw::system
