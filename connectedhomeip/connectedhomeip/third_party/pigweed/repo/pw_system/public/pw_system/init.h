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

namespace pw::system {

// This function should be called after all required platform initialization is
// complete, but before the scheduler is started. This function WILL return so
// the caller may start the scheduler if needed.
//
// Init will start logging, RPC, and work queue threads, and do any
// initialization required for those systems. Note that this initialization is
// largely not synchronous: only the work queue thread is dispatched, and the
// remainder of the initialization is added as a work queue item so it can be
// run in the normal context of a running scheduler/OS. This means RPC and
// logging will not be fully initialized until after that first work queue item
// is complete.
//
// To run something after pw_system's initialization is complete,
// simply add a callback to the work queue after calling pw::system::Init()
// rather than directly calling the function itself.
void Init();

}  // namespace pw::system
