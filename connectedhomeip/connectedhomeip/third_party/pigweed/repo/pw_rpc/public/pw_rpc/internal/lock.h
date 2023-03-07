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
#pragma once

#include "pw_rpc/internal/config.h"
#include "pw_sync/lock_annotations.h"

#if PW_RPC_USE_GLOBAL_MUTEX

#include <mutex>

#include "pw_sync/mutex.h"  // nogncheck

#endif  // PW_RPC_USE_GLOBAL_MUTEX

namespace pw::rpc::internal {

#if PW_RPC_USE_GLOBAL_MUTEX

using RpcLock = sync::Mutex;
using LockGuard = std::lock_guard<RpcLock>;

#else

class PW_LOCKABLE("pw::rpc::internal::RpcLock") RpcLock {
 public:
  constexpr void lock() PW_EXCLUSIVE_LOCK_FUNCTION() {}
  constexpr void unlock() PW_UNLOCK_FUNCTION() {}
};

class PW_SCOPED_LOCKABLE LockGuard {
 public:
  // [[maybe_unused]] needs to be after the parameter to workaround a gcc bug
  // context: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81429
  constexpr LockGuard(RpcLock& mutex [[maybe_unused]])
      PW_EXCLUSIVE_LOCK_FUNCTION(mutex) {}

  ~LockGuard() PW_UNLOCK_FUNCTION() = default;
};

#endif  // PW_RPC_USE_GLOBAL_MUTEX

RpcLock& rpc_lock();

}  // namespace pw::rpc::internal
