/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  @file  ti/utils/runtime/Log.h
 *
 *  @addtogroup ti_utils_runtime_LOG Log Interface
 *
 *  @brief      The Log module provides APIs to instrument source code.
 *
 *  To access the LOG APIs, the application should include its header file
 *  as follows:
 *  @code
 *  #include <ti/utils/runtime/Log.h>
 *  @endcode
 *
 *  When the application is configured using syscfg, the generated
 *  ti_utils_build_linker command file will pull in the necessary libraries
 *  based on the options selected in the UI.
 *
 *  If syscfg is not used, the developer must add any relevant Logger
 *  transport libraries to the link line. An example is shown below for
 *  LoggerBuf which is a memory based log transport:
 *  @code
 *  .../source/ti/loggers/utils/lib/{toolchain}/{isa}/loggers_{profile}.a
 *  @endcode
 *
 *  ## Beta Disclaimer ##
 *  The logging ecosystem are to be considered beta quality.
 *  They are not recommended for use in production code by TI.
 *  APIs and behaviour will change in future releases.
 *  Please report issues or feedback to [__E2E__][e2e].
 *
 *  [e2e]: https://e2e.ti.com/
 *
 *  ## Definitions ##
 *
 *  The following terms are used throughout the log documentation.
 *
 *  | Term                            | Definition                                                                                                                                                                                                                                                                                                                     |
 *  |---------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *  | `LogModule`                     | A parameter passed to Log APIs to indicate which software module the log statement originated from.                                                                                                                                                                                                                            |
 *  | `LogLevel`                      | The severity or importance of a given log statement.                                                                                                                                                                                                                                                                           |
 *  | `BackendDelegate`               | Also simply called a logger. This is a transport specific logger implementation. <br> The Logging framework is flexible such that multiple backend delegates may exist in a single firmware image.                                                                                                                             |
 *  | `CallSite`                      | A specific invocation of a Log API in a given file or program.                                                                                                                                                                                                                                                                 |
 *  | `LogSite`                       | The syscfg module that handles routing of `LogModule`s to specific `BackendDelegate`s.                                                                                                                                                                                                                                         |
 *  | Link Time Optimization (LTO)    | Conditional inclusion/exclusion of logs using link time optimization/dead code elimination. <br> This approach will push the decision of log inclusion to the linker. <br> In order to achieve good performance all code and libraries participating in logging must be compiled and linked at the highest optimization level. |
 *  | Preprocessor Optimization (PPO) | Conditional inclusion/exclusion of logs using the C preprocessor. The decision to include/exclude logs is made during preprocessing.                                                                                                                                                                                           |
 *
 *  ## Modules ##
 *
 *  When adding log statements to the target software, it is recommended to
 *  create a logging module for each software component in the image. Modules
 *  enable the reader to understand where the log record originated from.
 *  Some log visualizers may allow the reader to filter or sort log statements
 *  by module. It is also recommended to namespace modules.
 *
 *  For example, a good module name for the `UART` driver that exists in
 *  `source/ti/drivers`, would be `ti_drivers_UART`.
 *
 *  Modules also control the routing of log records to a backend delegate.
 *  Routing is controlled via the LogSite panel in syscfg. The default log
 *  module is called `ti_utils_runtime_LogMain` and is always available. All
 *  other modules must be created via LogSite. Enabling a module
 *  via LogSite will generate externs and definitions in
 *  `ti_utils_runtime_config`. Each extern and definition is namespaced by the
 *  module.
 *
 *  An example of the definitions generated for the default log module is shown
 *  below:
 *
 *  @code
 *  const uint32_t        ti_utils_runtime_LogMain_LogSite_level = 0;
 *  const ILogger_Handle  ti_utils_runtime_LogMain_LogSite_handle = &ti_loggers_utils_LoggerBuf_config[0];
 *  extern void           ti_utils_runtime_LogMain_LogSite_event(ILogger_Handle, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
 *  extern void           ti_utils_runtime_LogMain_LogSite_printf(ILogger_Handle, uint32_t, uint32_t, ...);
 *  extern void           ti_utils_runtime_LogMain_LogSite_buf(ILogger_Handle, uint32_t, const char*, uint8_t *, size_t);
 *  @endcode
 *
 *  In summary, each new module will instantiate a level bitmask and handle.
 *  It will also create externs of each log API that is prefixed with the
 *  module name. The extern definitions allow routing of different modules to
 *  different logger backends.
 *
 *  The routing of module --> backend is achieved at link time by remapping the
 *  externed symbol generated above to the actual backend logger API. Routing
 *  is achieved by using the symbol renaming function of the linker. The LogSite
 *  syscfg will generate a file called `ti_utils_runtime_LogSite.cmd.genlibs`
 *  that does this mapping at link time. A sample of this for the TI ARM
 *  compiler is shown below.
 *  It will route the externs created above to the LoggerBuf backend.
 *
 *  @code
 *  --symbol_map=ti_utils_runtime_LogMain_LogSite_event=ti_loggers_utils_LoggerBuf_event
 *  --symbol_map=ti_utils_runtime_LogMain_LogSite_printf=ti_loggers_utils_LoggerBuf_printf
 *  --symbol_map=ti_utils_runtime_LogMain_LogSite_buf=ti_loggers_utils_LoggerBuf_buf
 *  @endcode
 *
 *  ## Levels ##
 *  Log levels are a way to indicate the severity or importance of the contents
 *  of a particular log call site. Each call site takes an argument that allows
 *  the user to specify the level. As with modules, log visualization tools
 *  allow the user to sort or filter on a given level. This can help the reader
 *  to find important or relevant log statements in visualization.
 *
 *  Log levels are also used to control the emission of logs. Each call site
 *  will check that the level is enabled before calling the underlying log API.
 *
 *  When using LTO log levels are configurable on a per module basis
 *  while when using PPO the levels are controlled via preprocessor definitions
 *  for example: `ti_utils_runtime_Log_ENABLE_INFO1`. For PPO, all log levels
 *  of interest must be enabled in the preprocessor options. For LTO, they are
 *  controlled via syscfg.
 *
 *  For PPO:
 *
 *  @code
 *  if ((level) & module##_LogSite_level) {
 *      // Call Log API
 *  }
 *  @endcode
 *
 *  For LTO:
 *  @code
 *  if ((level) & module##_LogSite_level) {
 *      // Call Log API
 *  }
 *  @endcode
 *
 *  @remark
 *
 *  ## Log Metadata ##
 *
 *  Each time a Log API is invoked, a metadata string is placed in the
 *  .out file. This string contains information about the API type, file, line
 *  module, level, and other information associated with the log call site.
 *  Each call site emits a string to a specific memory section called
 *  `.log_data`. In order to use logging, this section should be added to the
 *  linker command file. By default, this section points to a nonloadable region
 *  of memory. Meaning that the metadata will not be loaded on the target
 *  device. Instead, the various logging visualization tools such as wireshark
 *  and TI ROV2 will read the metadata from this section and properly
 *  decode the log statements. The benefit of this approach is that very little
 *  memory is consumed on target. Additionally, the log transport only needs to
 *  store or send pointers to this meta section when a log API is called.
 *
 *  In summary, this approach minimizes the amount of memory consumed on device
 *  and bytes sent over the transport. This section can be loaded on target if
 *  desired or if you are creating a custom logger. The design does not preclude
 *  this.
 *
 *  In order to use the logging framework, the log section must be added to
 *  the linker command file. Here is a sample for the TI linker. Other
 *  examples can be found in the TI provided linker files for each toolchain.
 *
 *  @code
 *  MEMORY
 *  {
 *      // List other memory regions here
 *      LOG_DATA (R) : origin = 0x90000000, length = 0x40000
 *  }
 *  SECTIONS
 *  {
 *       .log_data       :   > LOG_DATA, type = COPY
 *  }
 *  @endcode
 *
 *  # Usage #
 *  This section provides a basic @ref ti_utils_runtime_LOG_Synopsis
 *  "usage summary" and a set of @ref ti_utils_runtime_LOG_Examples "examples"
 *  in the form of commented code fragments.
 *  Detailed descriptions of the LOG APIs are provided in subsequent sections.
 *
 *  @anchor ti_utils_runtime_LOG_Synopsis
 *  ### Synopsis ###
 *
 *  @code
 *  // Import the Log header
 *  #include <ti/utils/runtime/Log.h>
 *
 *  // Printf is the easiest to use log-api. It offers medium performance
 *  // for most transport mechanisms
 *  Log_printf(ti_utils_runtime_LogMain, Log_INFO1, "Hello World!");
 *  @endcode
 *
 *  @anchor ti_utils_runtime_LOG_Examples
 *  ### Examples ###
 *  * @ref ti_utils_LOG_Example_event "Log Event"
 *  * @ref ti_utils_LOG_Example_printf "Log printf"
 *  * @ref ti_utils_LOG_Example_buf "Logging buffers"
 *
 *  @anchor ti_utils_LOG_Example_event
 *  **Log Event**: The following example demonstrates how to create
 *  a log event object and use it in the code. There are two steps to using a
 *  log event: 1. instantiation and 2. call site(s). Instantiation creates the
 *  event and the necessary metadata, and call site is where the event is
 *  actually recorded by the logger framework. Because most of the metadata and
 *  arguments are statically created, the event is the most efficient log
 *  statement.
 *
 *  @code
 *  // Create a log event data type called LogEvent_count
 *  // The log module is ti_utils_runtime_LogMain
 *  // The format string is "count=%d" -- this should describe what the event
 *  // does
 *  Log_EVENT_CONSTRUCT(ti_utils_runtime_LogMain, LogEvent_count, "count=%d");
 *  @endcode
 *
 *  Later on, in the application, the count event is consumed. Note the log
 *  module must match between event creation and call site. In the code below,
 *  a LogEvent record is created for serialization or stage by the backend
 *  logger.
 *
 *  @code
 *  Log_event(ti_utils_runtime_LogMain, Log_INFO1, LogEvent_count, count++);
 *  @endcode
 *
 *  @anchor ti_utils_LOG_Example_printf
 *  **Log Printf**: The following example demonstrates use of the Log printf API.
 *  in code. Printf will embed the format string in the call site and will take
 *  arguments using varadic arguments
 *
 *  @code
 *  Log_printf(ti_utils_runtime_LogMain, Log_INFO1, "Hello World!");
 *  @endcode
 *
 *  @anchor ti_utils_LOG_Example_buf
 *  **Log Buf**: The following example demonstrates use of the Log buf API.
 *  in code. Printf will embed the format string in the call site and will take
 *  the buffer as a pointer and length. Buffers are treated as arrays of bytes.
 *  The buffer API should only be used when it is necessary to log data
 *  that is only available at runtime. It will actually send or store the entire
 *  contents of the buffer, so this API should be used sparingly as it is costly
 *  in terms of runtime and memory overhead.
 *
 *  @code
 *  uint8_t bufferToLog[] = {0, 1, 2, 3, 4, 5};
 *  Log_buf(ti_utils_runtime_LogMain,
 *          Log_INFO1,
 *          "The contents of bufferToLog are: ",
 *          bufferToLog,
 *          sizeof(bufferToLog));
 *  @endcode
 */

/*  Remove the include guard to allow this file to participate
 *  multiple times in the same compilation unit. This is a special
 *  build flow where multiple source files are included into the
 *  same compilation unit. When each source file is preprocessed,
 *  this header file will reset the preprocessor state such that
 *  the current file may re-define the Log macros as requested.
 *
 *  #ifndef ti_utils_runtime_Log__include
 *  #define ti_utils_runtime_Log__include
 */

/*! @ingroup ti_utils_runtime_LOG */
/*@{*/

/*
 *  ======== Log.h ========
 *  @brief Contains Log library APIs
 */
#include <stdint.h>

#include <ti/utils/runtime/ILogger.h>

#if defined (__cplusplus)
extern "C" {
#endif


/*
 *  ======== undef all working macros ========
 *  Remove working macros from preprocessor state
 *
 *  When including multiple source files into one compilation unit,
 *  this removes the macros left over from a previous include of this
 *  file.
 */
#if ti_utils_runtime_Log_USE_LTO || ti_utils_runtime_Log_USE_PPO

#undef Log_MASK_INFO1
#undef Log_MASK_INFO2
#undef Log_MASK_INFO3
#undef Log_MASK_INFO4
#undef Log_MASK_INFO5
#undef Log_MASK_INFO6
#undef Log_MASK_WARN
#undef Log_MASK_ERROR
#undef Log_MASK
#undef _Log_event_B
#undef _Log_buf_B
#undef _Log_printf_B
#undef _Log_DECL_Write
#undef _Log_DECL_Write_A
#undef _Log_DECL_Write_B
#undef _Log_DECL_Handle
#undef _Log_DECL_Handle_A
#undef _Log_DECL_Handle_B
#undef _Log_DECL_Level
#undef _Log_DECL_Level_A
#undef _Log_DECL_Level_B
#undef _Log_DECL_buf
#undef _Log_DECL_buf_A
#undef _Log_DECL_buf_B
#undef _Log_DECL_Printf
#undef _Log_DECL_Printf_A
#undef _Log_DECL_Printf_B


#undef _Log_ARGS
#undef _Log_ARGS_A
#undef _Log_ARGS_B
#undef _Log_ARGS0
#undef _Log_ARGS1
#undef _Log_ARGS2
#undef _Log_ARGS3
#undef _Log_ARGS4
#undef _Log_NUMARGS
#undef _Log_NUMARGS_A
#undef _Log_NUMARGS_B
#undef _Log_CONCAT2_A
#undef _Log_CONCAT2
#undef _Log__TOKEN2STRING_A
#undef _Log_TOKEN2STRING
#undef _Log_APPEND_META_TO_FORMAT
#undef _Log_FIRST_ARG
#undef _Log_PLACE_FORMAT_IN_SECTOR
#undef _Log_GUARD_MACRO
#endif

/* these are always defined */
#undef ti_utils_runtime_Log_USE_LTO
#undef ti_utils_runtime_Log_USE_PPO
#undef Log_DECLARE_MODULE
#undef _Log_DECL_Module_Id_Name
#undef Log_EVENT_CONSTRUCT
#undef Log_event
#undef Log_buf
#undef Log_printf

/*
 *  ======== ti_utils_runtime_Log_ENABLE_LTO ========
 *  Enable instrumentation using link-time optimization implementation
 *
 *  Define this symbol to add instrumentation at compile time.
 *  It must be defined before including this header file.
 */
#ifdef ti_utils_runtime_Log_ENABLE_LTO
#undef ti_utils_runtime_Log_ENABLE_LTO
#define ti_utils_runtime_Log_USE_LTO 1
#else
#define ti_utils_runtime_Log_USE_LTO 0
#endif

/*
 *  ======== ti_utils_runtime_Log_ENABLE_PPO ========
 *  Enable instrumentation using preprocessor implementation
 *
 *  Define this symbol to add instrumentation at compile time.
 *  It must be defined before including this header file.
 */
#ifdef ti_utils_runtime_Log_ENABLE_PPO
#undef ti_utils_runtime_Log_ENABLE_PPO
#define ti_utils_runtime_Log_USE_PPO 1
#else
#define ti_utils_runtime_Log_USE_PPO 0
#endif

/* cannot define both macros at the same time */
#if ti_utils_runtime_Log_USE_LTO && ti_utils_runtime_Log_USE_PPO
#error Cannot define both LTO and PPO at the same time.
#endif

#if ti_utils_runtime_Log_USE_LTO || ti_utils_runtime_Log_USE_PPO
/*
 *  =============================
 *  ======== Log Enabled ========
 *  =============================
 */

/** @cond NODOC */

#ifdef ti_utils_runtime_Log_ENABLE_ALL
#undef ti_utils_runtime_Log_ENABLE_ALL

#define Log_MASK_INFO1 Log_INFO1
#define Log_MASK_INFO2 Log_INFO2
#define Log_MASK_INFO3 Log_INFO3
#define Log_MASK_INFO4 Log_INFO4
#define Log_MASK_INFO5 Log_INFO5
#define Log_MASK_INFO6 Log_INFO6
#define Log_MASK_WARN  Log_WARN
#define Log_MASK_ERROR Log_ERROR

#else

#ifdef ti_utils_runtime_Log_ENABLE_INFO1
#undef ti_utils_runtime_Log_ENABLE_INFO1
#define Log_MASK_INFO1 Log_INFO1
#else
#define Log_MASK_INFO1 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_INFO2
#undef ti_utils_runtime_Log_ENABLE_INFO2
#define Log_MASK_INFO2 Log_INFO2
#else
#define Log_MASK_INFO2 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_INFO3
#undef ti_utils_runtime_Log_ENABLE_INFO3
#define Log_MASK_INFO3 Log_INFO3
#else
#define Log_MASK_INFO3 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_INFO4
#undef ti_utils_runtime_Log_ENABLE_INFO4
#define Log_MASK_INFO4 Log_INFO4
#else
#define Log_MASK_INFO4 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_INFO5
#undef ti_utils_runtime_Log_ENABLE_INFO5
#define Log_MASK_INFO5 Log_INFO5
#else
#define Log_MASK_INFO5 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_INFO6
#undef ti_utils_runtime_Log_ENABLE_INFO6
#define Log_MASK_INFO6 Log_INFO6
#else
#define Log_MASK_INFO6 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_WARN
#undef ti_utils_runtime_Log_ENABLE_WARN
#define Log_MASK_WARN Log_WARN
#else
#define Log_MASK_WARN 0
#endif

#ifdef ti_utils_runtime_Log_ENABLE_ERROR
#undef ti_utils_runtime_Log_ENABLE_ERROR
#define Log_MASK_ERROR Log_ERROR
#else
#define Log_MASK_ERROR 0
#endif

#endif /* ti_utils_runtime_Log_ENABLE_ALL */

#define Log_MASK (\
        Log_MASK_INFO1 | Log_MASK_INFO2 | Log_MASK_INFO3 | \
        Log_MASK_INFO4 | Log_MASK_INFO5 | Log_MASK_INFO6 | \
        Log_MASK_WARN  | Log_MASK_ERROR)


/* This macro protects against sideffects of the C preprocessor expansion
 * of log statements. Each log API should be guarded by it.
 * An article explaining this behavior can be found here:
 * https://gcc.gnu.org/onlinedocs/cpp/Swallowing-the-Semicolon.html
 */
#define _Log_GUARD_MACRO(x) do{ x }while(0)

/*
 *
 *  ======== Log Private Macros ========
 *
 *  The following macros are intended to be private to the log module and
 *  are not intended for use by the user. Private macros will start with _Log.
 *
 *  In the case of multi level macros (macros that invoke other macros), a
 *  letter is appended at the end of the definition. With each level of nesting,
 *  the appended letter is incremented.
 *
 *  For example: _Log_test --> _Log_test_A --> _Log_test_B
 */
/* Extracts the first argument from __VA_ARGS__ */
#define _Log_FIRST_ARG(N, ...) N

/*  Routes a log call based on the number of arguments provided */
#define _Log_EVENT_ARGS(module, header, ...) \
           _Log_EVENT_ARGS_A(module, header, _Log_NUMARGS(__VA_ARGS__), __VA_ARGS__)
#define _Log_EVENT_ARGS_A(module, header, num, ...)                            \
            _Log_EVENT_ARGS_B(module, header, num, __VA_ARGS__)
#define _Log_EVENT_ARGS_B(module, header, num, ...)                            \
            _Log_EVENT_ARGS##num(module, uintptr_t, header, __VA_ARGS__)

/*  Helper macros to fill out dummy parameters if they're unused */
#define _Log_EVENT_ARGS0(module, type, header, event)                          \
            (uint32_t)header,                                                  \
            (uintptr_t)module##_##event,                                       \
            (type)0,                                                           \
            (type)0,                                                           \
            (type)0,                                                           \
            (type)0

#define _Log_EVENT_ARGS1(module, type, header, event, a1)                      \
            (uint32_t)header,                                                  \
            (uintptr_t)module##_##event,                                       \
            (type)a1,                                                          \
            (type)0,                                                           \
            (type)0,                                                           \
            (type)0

#define _Log_EVENT_ARGS2(module, type, header, event, a1, a2)                  \
            (uint32_t)header,                                                  \
            (uintptr_t)module##_##event,                                       \
            (type)a1,                                                          \
            (type)a2,                                                          \
            (type)0,                                                           \
            (type)0

#define _Log_EVENT_ARGS3(module, type, header, event, a1, a2, a3)              \
            ((uint32_t)header,                                                 \
             (uintptr_t)module##_##event,                                      \
             (type)a1,                                                         \
             (type)a2,                                                         \
             (type)a3,                                                         \
             (type)0)

#define _Log_EVENT_ARGS4(module, type, header, event, a1, a2, a3, a4)          \
            (uint32_t)header,                                                  \
            (uintptr_t)module##_##event,                                       \
            (type)a1,                                                          \
            (type)a2,                                                          \
            (type)a3,                                                          \
            (type)a4

/*  Helper macro to count the number of arguments in __VA_ARGS_ */
#define _Log_NUMARGS(...) _Log_NUMARGS_A(__VA_ARGS__, 4, 3, 2, 1, 0)
#define _Log_NUMARGS_A(...) _Log_NUMARGS_B(__VA_ARGS__)
#define _Log_NUMARGS_B(_first, _4, _3, _2, _1, N, ...) N

/*
 *  ======== Meta string tokenization macros ========
 */
/*  Helper macro to concatenate two symbols */
#define _Log_CONCAT2_A(x,y) x ## _ ## y
#define _Log_CONCAT2(x,y) _Log_CONCAT2_A(x,y)

/*  Helper macro to concatenate two symbols */
#define _Log__TOKEN2STRING_A(x) #x
#define _Log_TOKEN2STRING(x) _Log__TOKEN2STRING_A(x)

/* Macro to place meta string in a memory section separated by record separator */
#define _Log_APPEND_META_TO_FORMAT(opcode,                                     \
                                    file,                                      \
                                    line,                                      \
                                    level,                                     \
                                    module,                                    \
                                    format,                                    \
                                    nargs)                                     \
                                    _Log_TOKEN2STRING(opcode)     "\x1e"       \
                                    _Log_TOKEN2STRING(file)       "\x1e"       \
                                    _Log_TOKEN2STRING(line)       "\x1e"       \
                                    _Log_TOKEN2STRING(level)      "\x1e"       \
                                    _Log_TOKEN2STRING(module)     "\x1e"       \
                                    _Log_TOKEN2STRING(format)     "\x1e"       \
                                    _Log_TOKEN2STRING(nargs)

/* Place a string in trace format section named ".log_data"
 * This section must exist in the linker file
 */
#if defined(__IAR_SYSTEMS_ICC__)
#define _Log_PLACE_FORMAT_IN_SECTOR(name, opcode, level, module, format, nargs)\
            __root static const char name[] @ ".log_data" =                    \
            _Log_APPEND_META_TO_FORMAT(opcode,                                 \
                                       __FILE__,                               \
                                       __LINE__,                               \
                                       level,                                  \
                                       module,                                 \
                                       format,                                 \
                                       nargs);
#elif defined(__TI_COMPILER_VERSION__) || (defined(__clang__) && defined(__ti_version__)) || defined(__GNUC__)
#define _Log_PLACE_FORMAT_IN_SECTOR(name, opcode, level, module, format, nargs)\
            static const char name[]                                           \
            __attribute__((used,section(".log_data"))) =                       \
            _Log_APPEND_META_TO_FORMAT(opcode,                                 \
                                        __FILE__,                              \
                                        __LINE__,                              \
                                        level,                                 \
                                        module,                                \
                                        format,                                \
                                        nargs);
#else
#error Incompatible compiler: Logging is currently supported by the following \
compilers: TI ARM Compiler, TI CLANG Compiler, GCC, IAR. Please migrate to a \
a supported compiler.
#endif

/** @endcond */

/**
 *  Declare a log module other than the default (ti_utils_runtime_LogMain)
 *
 *  This macro will create the necessary externs used to route this module's
 *  traffic to a logger instance. It can be useful for statically linked
 *  libraries to forward declare their modules so that they can be routed
 *  at link time
 *
 *  This must be combined with a symbol remapping to the specific backend
 *  logger that will accept this modules log statements. This is discussed in
 *  the modules section above.  If using syscfg, this macro is not required
 *  as the externs are generated by the LogSite module.
 *
 *  @param[in] module      Log module to declare
 */
#define  Log_DECLARE_MODULE(module)                                            \
            _Log_DECL_Write(module);                                           \
            _Log_DECL_Printf(module);                                          \
            _Log_DECL_buf(module);                                             \
            _Log_DECL_Handle(module);                                          \
            _Log_DECL_Level(module);

/** @cond NODOC */
#define _Log_DECL_Write(module) _Log_DECL_Write_A(module)
#define _Log_DECL_Write_A(module) _Log_DECL_Write_B(module)
#define _Log_DECL_Write_B(module)                                              \
    extern void module##_LogSite_event(ILogger_Handle logger,                  \
                                       uint32_t header,                        \
                                       uintptr_t event,                        \
                                       uintptr_t arg0,                         \
                                       uintptr_t arg1,                         \
                                       uintptr_t arg2,                         \
                                       uintptr_t arg3)

#define _Log_DECL_Printf(module) _Log_DECL_Printf_A(module)
#define _Log_DECL_Printf_A(module) _Log_DECL_Printf_B(module)
#define _Log_DECL_Printf_B(module)                                             \
    extern void module##_LogSite_printf(ILogger_Handle handle,                 \
                                        uint32_t header,                       \
                                        uint32_t numArgs,                      \
                                        ...)

#define _Log_DECL_buf(module) _Log_DECL_buf_A(module)
#define _Log_DECL_buf_A(module) _Log_DECL_buf_B(module)
#define _Log_DECL_buf_B(module)                                                \
    extern void module##_LogSite_buf(ILogger_Handle handle,                    \
                                     uint32_t header,                          \
                                     const char* format,                       \
                                     uint8_t *data,                            \
                                     size_t size)

#define _Log_DECL_Handle(module) _Log_DECL_Handle_A(module)
#define _Log_DECL_Handle_A(module) _Log_DECL_Handle_B(module)
#define _Log_DECL_Handle_B(module)                                             \
    extern const ILogger_Handle module##_LogSite_handle

#define _Log_DECL_Level(module) _Log_DECL_Level_A(module)
#define _Log_DECL_Level_A(module) _Log_DECL_Level_B(module)
#define _Log_DECL_Level_B(module)                                              \
    extern const uint32_t module##_LogSite_level

#if ti_utils_runtime_Log_USE_LTO

#define _Log_event_B(module, level, ...)                                       \
    if ((level) & module##_LogSite_level) {                                    \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_EVENT,                          \
                                    level,                                     \
                                    module,                                    \
                                    _Log_FIRST_ARG(__VA_ARGS__),               \
                                    _Log_NUMARGS(__VA_ARGS__))                 \
        module##_LogSite_event(module##_LogSite_handle,                        \
                               _Log_EVENT_ARGS(module,                         \
                                               &_Log_CONCAT2(LogSymbol,        \
                                                             __LINE__),        \
                                               __VA_ARGS__));                  \
    }

#endif

#if ti_utils_runtime_Log_USE_PPO

#define _Log_event_B(module, level, ...)                                       \
    if ((level) & (Log_MASK)) {                                                \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_EVENT,                          \
                                    level,                                     \
                                    module,                                    \
                                    _Log_FIRST_ARG(__VA_ARGS__),               \
                                    _Log_NUMARGS(__VA_ARGS__))                 \
        module##_LogSite_event(module##_LogSite_handle,                        \
                               _Log_EVENT_ARGS(module,                         \
                                               &_Log_CONCAT2(LogSymbol,        \
                                                             __LINE__),        \
                                               __VA_ARGS__));                  \
    }

#endif

#if ti_utils_runtime_Log_USE_LTO

#define _Log_buf_B(module , level, format, data, size)                         \
    if ((level) & module##_LogSite_level) {                                    \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_BUFFER,                         \
                                    level,                                     \
                                    module,                                    \
                                    format,                                    \
                                    0)                                         \
        module##_LogSite_buf(module##_LogSite_handle,                          \
                             (uint32_t)&_Log_CONCAT2(LogSymbol, __LINE__),     \
                             format,                                           \
                             data,                                             \
                             size);                                            \
    }

#endif

#if ti_utils_runtime_Log_USE_PPO

#define _Log_buf_B(module , level, format, data, size)                         \
    if ((level) & (Log_MASK)) {                                                \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_BUFFER,                         \
                                    level,                                     \
                                    module,                                    \
                                    format,                                    \
                                    0)                                         \
        module##_LogSite_buf(module##_LogSite_handle,                          \
                             (uint32_t)&_Log_CONCAT2(LogSymbol, __LINE__),     \
                             format,                                           \
                             data,                                             \
                             size);                                            \
    }

#endif

#if ti_utils_runtime_Log_USE_LTO

#define _Log_printf_B(module, level, ...)                                      \
    if ((level) & module##_LogSite_level) {                                    \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_FORMATED_TEXT,                  \
                                    level,                                     \
                                    module,                                    \
                                    _Log_FIRST_ARG(__VA_ARGS__),               \
                                    _Log_NUMARGS(__VA_ARGS__))                 \
        module##_LogSite_printf(module##_LogSite_handle,                       \
                                (uint32_t)&_Log_CONCAT2(LogSymbol, __LINE__),  \
                                _Log_NUMARGS(__VA_ARGS__),                     \
                                __VA_ARGS__);                                  \
    }

#endif

#if ti_utils_runtime_Log_USE_PPO

#define _Log_printf_B(module, level, ...)                                      \
    if ((level) & (Log_MASK)) {                                                \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, __LINE__),         \
                                    LOG_OPCODE_FORMATED_TEXT,                  \
                                    level,                                     \
                                    module,                                    \
                                    _Log_FIRST_ARG(__VA_ARGS__),               \
                                    _Log_NUMARGS(__VA_ARGS__))                 \
        module##_LogSite_printf(module##_LogSite_handle,                       \
                                (uint32_t)&_Log_CONCAT2(LogSymbol, __LINE__),  \
                                _Log_NUMARGS(__VA_ARGS__),                     \
                                __VA_ARGS__);                                  \
    }

#endif

/** @endcond */

/**
 *  @brief Raise a log event to the logger
 *
 *  This API will emit a log record with the user provided event arguments.
 *  This event is linked to the one constructed by @ref Log_EVENT_CONSTRUCT
 *  through a pointer to the metadata location. Separate construct and consume
 *  APIs means that an event can be constructed in the code and reused multiple
 *  places in the code with different runtime arguments.
 *
 *  @param[in] module      Log module that the event belongs to
 *  @param[in] level       log level
 *  @param[in]  ...        Variadic arguments consisting of the following
 *                         - `event` structure declared by
 *                         @ref Log_EVENT_CONSTRUCT. **This is required**
 *                         - `a0-a3` arguments consumed by the
 *                         Log_EVENT_CONSTRUCT format string. These are optional
 *
 *  @remark
 *    The maximum number of arguments is four. All 4 arguments will always
 *    be provided to the backend. Unused arguments are passed as 0.
 */
#define Log_event(module, level, ...)                                          \
                     _Log_GUARD_MACRO(_Log_event_B(module , level, __VA_ARGS__))


/**
 *  @brief Construct a log event object
 *
 *  Use this marco to define a log event object. The object is private
 *  to the current file. Use the object handle in a call to @ref Log_event to
 *  raise that event to the logger.
 *
 *  @param[in]  module     Log module that the event belongs to
 *  @param[in]  name       Event variable name, to be passed to Log_event API
 *  @param[in]  fmt        Restricted format string. Note `%s` is not supported.
 *                         Supported format specifiers include: `%c`, `%f`,
 *                         `%d`, `%x`
 *  @remark
 *    The module passed here must match the one from the callsite
 */
#define Log_EVENT_CONSTRUCT(module, name, fmt)                                 \
        _Log_PLACE_FORMAT_IN_SECTOR(_Log_CONCAT2(LogSymbol, name),             \
                                    LOG_EVENT_CONSTRUCT,                       \
                                    name,                                      \
                                    module,                                    \
                                    fmt,                                       \
                                    0)                                         \
        const char *_Log_CONCAT2(module, name) = _Log_CONCAT2(LogSymbol,       \
                                                              name);


/**
 *  @brief Log a continuous block of memory
 *
 *  Use this marco to send out runtime data from the device. This API should be
 *  used when the data is non constant and can only be derived at runtime. It
 *  is the most intrusive in terms of record overhead and instructions used.
 *
 *  @param[in]  module     Log module that the buffer originated from
 *  @param[in]  level      log level of type @ref Log_Level
 *  @param[in]  format     Restricted format string.
 *  @param[in]  data       Pointer to array of bytes (uint8_t *)
 *  @param[in]  size       Size in bytes of array to send
 *
 */
#define Log_buf(module, level, format, data, size)                             \
                _Log_GUARD_MACRO(_Log_buf_B(module , level, format, data, size))

/**
 *  @brief Emit a printf formatted log
 *
 *  Use this marco to enable printf style logging. This API offers the most
 *  flexibility as the construction of the format string is embedded in the call
 *  site of the API. It also supports true variadic arguments.
 *
 *  @param[in]  module     Log module that the buffer originated from
 *  @param[in]  level      log level of type @ref Log_Level
 *  @param[in]  ...        Variadic arguments consisting of the following
 *                         - `format` format string. **This is required**
 *                         - `a0-a3` arguments consumed by the format string.
 *                         These are optional
 *
 *  @remark
 *    The number of arguments is currently limited to 4. Only used arguments are
 *    included in the record.
 */
#define Log_printf(module, level, ...)                                         \
                     _Log_GUARD_MACRO(_Log_printf_B(module, level, __VA_ARGS__))

#else /* ti_utils_runtime_Log_USE_LTO || ti_utils_runtime_Log_USE_PPO */

/*
 *  =================================================
 *  ======== Log Disabled (default behavior) ========
 *  =================================================
 */
#define _Log_DECL_Config()
#define _Log_DECL_Module_Id_Name(module)
#define Log_EVENT_CONSTRUCT(name, type, fmt)
#define Log_event(level, ...)

#endif /* ti_utils_runtime_Log_USE_LTO || ti_utils_runtime_Log_USE_PPO */

/*
 *  ======== Log_FIRST ========
 *  Mark the first passage through this file
 *
 *  This macro is not needed in the typical use-case. However,
 *  when including multiple source files into one compilation
 *  unit, where each source file includes Log.h, this macro is
 *  needed to mark that the first pass has been completed. This
 *  will guard the type definitions on subsequent passes, to
 *  avoid the multiple definition errors raised by the compiler.
 */
#ifndef Log_FIRST
/** @cond NODOC */
#define Log_FIRST
/** @endcond */

/*
 *  ======== Log_Level ========
 */
typedef enum Log_Level {
    Log_INFO1 = 1,
    Log_INFO2 = 2,
    Log_INFO3 = 4,
    Log_INFO4 = 8,
    Log_INFO5 = 16,
    Log_INFO6 = 32,
    Log_WARN  = 64,
    Log_ERROR = 128
} Log_Level;

/*
 *  ======== LogMain ========
 *  Courtesy declarations on behalf of LogMain
 */
/** @cond NODOC */
extern const uint32_t ti_utils_runtime_LogMain_LogSite_level;
extern const ILogger_Handle ti_utils_runtime_LogMain_LogSite_handle;
extern void ti_utils_runtime_LogMain_LogSite_event(ILogger_Handle,
                                                   uint32_t header,
                                                   uintptr_t event,
                                                   uintptr_t,
                                                   uintptr_t,
                                                   uintptr_t,
                                                   uintptr_t);
extern void ti_utils_runtime_LogMain_LogSite_printf(ILogger_Handle handle,
                                                    uint32_t header,
                                                    uint32_t numArgs,
                                                    ...);
extern void ti_utils_runtime_LogMain_LogSite_buf(ILogger_Handle handle,
                                                 uint32_t header,
                                                 const char* format,
                                                 uint8_t *data,
                                                 size_t size);
/** @endcond */
#endif /* Log_FIRST */


/*! @} */
#if defined (__cplusplus)
}
#endif

/*  Remove include guard.
 *
 *  #endif ti_utils_runtime_Log__include
 */
