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

#include "pw_preprocessor/arch.h"

#if !_PW_ARCH_ARM_CORTEX_M
#error You can only build this for ARM Cortex-M architectures. If you are \
       trying to do this and are still seeing this error, see \
       pw_preprocessor/arch.h
#endif  // !_PW_ARCH_ARM_CORTEX_M

#if !_PW_ARCH_ARM_V7M && !_PW_ARCH_ARM_V7EM && !_PW_ARCH_ARM_V8M_MAINLINE && \
    !_PW_ARCH_ARM_V8_1M_MAINLINE
#error "Your selected ARM Cortex-M arch is not yet supported by this module."
#endif

// Which log level to use for this module.
#ifndef PW_CPU_EXCEPTION_CORTEX_M_LOG_LEVEL
#define PW_CPU_EXCEPTION_CORTEX_M_LOG_LEVEL PW_LOG_LEVEL_DEBUG
#endif  // PW_CPU_EXCEPTION_CORTEX_M_LOG_LEVEL

// Enables extended logging in pw::cpu_exception::LogCpuState() and
// pw::cpu_exception::cortex_m::LogExceptionAnalysis() that dumps the active
// CFSR fields with help strings. This is disabled by default since it
// increases the binary size by >1.5KB when using plain-text logs, or ~460
// Bytes when using tokenized logging. It's useful to enable this for device
// bringup until your application has an end-to-end crash reporting solution.
#ifndef PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP
#define PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP 0
#endif  // PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP
