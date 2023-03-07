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
#pragma once

#include "pw_cpu_exception/state.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

// By default, the exception entry function will terminate by handing execution
// over to pw_cpu_exception_DefaultHandler(). This can be used to override the
// current handler. This allows runtime insertion of an exception handler which
// may also be helpful for loading a bootloader exception handler by default
// that an application overrides.
void pw_cpu_exception_SetHandler(void (*handler)(pw_cpu_exception_State*));

// Set the exception handler to point to pw_cpu_exception_DefaultHandler().
void pw_cpu_exception_RestoreDefaultHandler(void);

// Application-defined recoverable CPU exception handler.
//
// Applications must define this function; it is not defined by the exception
// entry backend. After CPU state is captured by the cpu exception entry
// backend, this function is called. Applications can then choose to either
// gracefully handle the exception and return, or decide the exception cannot be
// handled and abort normal execution (e.g. reset).
//
// Examples of what applications could do in the handler: gracefully recover
// (e.g. enabling a floating point unit after triggering an exception executing
// a floating point instruction), reset the device, or wait for a debugger to
// attach.
//
// See the cpu_exception module documentation for more details.
PW_USED void pw_cpu_exception_DefaultHandler(pw_cpu_exception_State* state);

// This is the underlying function the CPU exception entry backend should call.
// This calls the currently set handler.
void pw_cpu_exception_HandleException(void* cpu_state);

PW_EXTERN_C_END
