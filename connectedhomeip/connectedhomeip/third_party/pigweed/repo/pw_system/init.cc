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
#define PW_LOG_MODULE_NAME "pw_system"

#include "pw_system/init.h"

#include "pw_log/log.h"
#include "pw_metric/global.h"
#include "pw_metric/metric_service_pwpb.h"
#include "pw_rpc/echo_service_pwpb.h"
#include "pw_system/config.h"
#include "pw_system/rpc_server.h"
#include "pw_system/target_hooks.h"
#include "pw_system/work_queue.h"
#include "pw_system_private/log.h"
#include "pw_thread/detached_thread.h"

#if PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE
#include "pw_system/thread_snapshot_service.h"
#endif  // PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE

namespace pw::system {
namespace {
metric::MetricService metric_service(metric::global_metrics,
                                     metric::global_groups);

rpc::EchoService echo_service;

void InitImpl() {
  PW_LOG_INFO("System init");

  // Setup logging.
  const Status status = GetLogThread().OpenUnrequestedLogStream(
      kDefaultRpcChannelId, GetRpcServer(), GetLogService());
  if (!status.ok()) {
    PW_LOG_ERROR("Error opening unrequested log streams %d",
                 static_cast<int>(status.code()));
  }

  PW_LOG_INFO("Registering RPC services");
  GetRpcServer().RegisterService(echo_service);
  GetRpcServer().RegisterService(GetLogService());
  GetRpcServer().RegisterService(metric_service);
#if PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE
  RegisterThreadSnapshotService(GetRpcServer());
#endif  // PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE

  PW_LOG_INFO("Starting threads");
  // Start threads.
  thread::DetachedThread(system::LogThreadOptions(), GetLogThread());
  thread::DetachedThread(system::RpcThreadOptions(), GetRpcDispatchThread());

  GetWorkQueue().CheckPushWork(UserAppInit);
}

}  // namespace

void Init() {
  thread::DetachedThread(system::WorkQueueThreadOptions(), GetWorkQueue());
  GetWorkQueue().CheckPushWork(InitImpl);
}

}  // namespace pw::system
