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

#include "pw_thread/thread.h"

namespace pw::system {

const thread::Options& LogThreadOptions();

const thread::Options& RpcThreadOptions();

const thread::Options& WorkQueueThreadOptions();

// This will run once after pw::system::Init() completes. This callback must
// return or it will block the work queue.
//
// This is the first thing run in a threaded context (specifically on the work
// queue thread).
void UserAppInit();

}  // namespace pw::system
