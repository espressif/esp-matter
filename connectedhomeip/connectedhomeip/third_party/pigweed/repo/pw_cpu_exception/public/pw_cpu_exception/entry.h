// Copyright 2019 The Pigweed Authors
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

// Platform-independent mechanism to catch hardware CPU faults in user code.
// This module encapsulates low level CPU exception handling assembly for the
// platform. By default, this module invokes the following user-defined function
// after early exception handling completes:
//
//   pw_cpu_exception_DefaultHandler(pw_cpu_exception_State* state)
//
// If platform-dependent access to the CPU registers is needed, then
// applications can include the respective backend module directly; for example
// cpu_exception_armv7m.
//
// IMPORTANT: To use this module, you MUST implement
//            pw_cpu_exception_DefaultHandler() in some part of your
//            application.

#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

// Low-level raw exception entry handler.
//
// Captures faulting CPU state into a platform-specific pw_cpu_exception_State
// object, then calls the user-provided fault handler.
//
// This function should be called immediately after a fault; typically by being
// in the interrupt vector table entries for the hard fault exceptions.
//
// Note: applications should almost never invoke this directly; if you do, make
// sure you know what you are doing.
PW_EXTERN_C PW_NO_PROLOGUE void pw_cpu_exception_Entry(void);
