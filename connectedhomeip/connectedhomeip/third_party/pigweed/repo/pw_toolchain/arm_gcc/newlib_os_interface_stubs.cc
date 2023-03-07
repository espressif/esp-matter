// Copyright 2022 The Pigweed Authors
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

#include <sys/stat.h>   // POSIX header provided by Newlib; needed for stat.
#include <sys/times.h>  // POSIX header provided by Newlib; needed for times.

#include <cstdio>  // fpos_t

#include "pw_assert/check.h"

namespace pw::toolchain {
namespace {

[[noreturn]] void AbortIfUnsupportedNewlibFunctionIsCalled() {
  PW_CRASH(
      "Attempted to invoke an unsupported Newlib function! The stdout and "
      "stderr FILE objects are not supported.");
}

}  // namespace

// Wrap the stdio read, write, seek, and close Newlib functions defined in
// libc/stdio/stdio.c. These should never be called, so abort if they are.
//
// These functions are unconditionally linked, even if they're never called,
// because they are assigned as members of the stdout/stderr FILE struct. The
// Newlib implementations invoke some of the unsupported OS interface functions.
#define PW_WRAP_NEWLIB_FILE_FUNCTION(function, ...) \
  extern "C" int __wrap_##function(__VA_ARGS__) {   \
    AbortIfUnsupportedNewlibFunctionIsCalled();     \
  }

PW_WRAP_NEWLIB_FILE_FUNCTION(__sread, void*, char*, int)
PW_WRAP_NEWLIB_FILE_FUNCTION(__swrite, void*, char*, int)
PW_WRAP_NEWLIB_FILE_FUNCTION(__sseek, void*, fpos_t, int)
PW_WRAP_NEWLIB_FILE_FUNCTION(__sclose, void*)

#undef PW_WRAP_NEWLIB_FILE_FUNCTION

// Newlib defines a set of OS interface functions. Most of these should never be
// called, since they're used by libc functions not supported in Pigweed(e.g.
// fopen or printf). If they're linked into a binary, that indicates that an
// unsupported function was called.
//
// Newlib provides default, nop implementations of these functions. Starting
// with arm-none-eabi-gcc 11.3, a warning is issued when any of these defaults
// are used.
//
// Provide implementations for most of the Newlib OS interface functions, which
// are documented at https://sourceware.org/newlib/libc.html#Stubs. The default
// implementation calls the following function, which is never defined,
// resulting in a linker error.
[[noreturn]] void AttempedToInvokeUnsupportedNewlibOsInterfaceFunction();

#define PW_DISABLE_NEWLIB_FUNCTION(function, ...)           \
  extern "C" int _##function(__VA_ARGS__) {                 \
    AttempedToInvokeUnsupportedNewlibOsInterfaceFunction(); \
  }

PW_DISABLE_NEWLIB_FUNCTION(_exit, void)
PW_DISABLE_NEWLIB_FUNCTION(close, int)
PW_DISABLE_NEWLIB_FUNCTION(execve, char*, char**, char**)
PW_DISABLE_NEWLIB_FUNCTION(fork, void)

// Provide the minimal fstat implementation recommended by the Newlib
// documentation since fstat is called indirectly by snprintf.
extern "C" int _fstat(int, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

PW_DISABLE_NEWLIB_FUNCTION(getpid, void)

// Provide the minimal isatty implementation recommended by the Newlib
// documentation since isatty is called indirectly by snprintf.
extern "C" int _isatty(int) { return 1; }

PW_DISABLE_NEWLIB_FUNCTION(kill, int, int)
PW_DISABLE_NEWLIB_FUNCTION(link, char*, char*)
PW_DISABLE_NEWLIB_FUNCTION(lseek, int, int, int)
PW_DISABLE_NEWLIB_FUNCTION(open, const char*, int, int)
PW_DISABLE_NEWLIB_FUNCTION(read, int, char*, int)
PW_DISABLE_NEWLIB_FUNCTION(sbrk, int)
PW_DISABLE_NEWLIB_FUNCTION(stat, char*, struct stat*)
PW_DISABLE_NEWLIB_FUNCTION(times, struct tms*)
PW_DISABLE_NEWLIB_FUNCTION(unlink, char*)
PW_DISABLE_NEWLIB_FUNCTION(wait, int*)
PW_DISABLE_NEWLIB_FUNCTION(write, int, char*, int)

#undef PW_DISABLE_NEWLIB_FUNCTION

}  // namespace pw::toolchain
