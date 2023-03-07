/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 * */

/*
 * ======== UIAErr.xdc ========
 */
import xdc.runtime.Diags;

/*!
 * UIA Standard Error Events
 *
 * The UIAErr module defines standard error events that allow
 * tooling to identify common errors in a consistent way.  They are all
 * intended to be used with the Log_writeX APIs and, in the future, with
 * crash dump APIs, and provide a way to log errors in a more standardized
 * way than the generic Log_error API enables.
 *
 * The events in this module have one of the following event priority levels:
 *  EMERGENCY: used to indicate a non-recoverable situation (e.g. a crash, with
 *   the event containing information about the cause of the crash)
 *  CRITICAL: used to indicate a sever error that should raise an alarm or
 *   cause a notification message to be sent to a system administrator.
 *  ERROR: used to indicate a recoverable error that does not require an alarm
 *   to be raised.
 *
 * The following special formatting specifiers may be used to define the
 * msg field of the UIAErr events:

 * %$S - a string parameter that can provide additional formatting specifiers
 *       Note that $S use in strings passed in as a paramter is not supported.
 *@p
 * %$F - a specifier for a string parameter containing the file name (__FILE__) and
 *       an integer parameter containing the line number (__LINE__).
 *
 *  The generation of UIAErr events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIAErr` events are generated only when the Diags.STATUS bit is set
 *  in the module's diagnostics mask.  The Diags.STATUS bit is set to ALWAYS_ON
 *  by default.
 *
 *  The following configuration script demonstrates how to enable use of
 *  UIAErr events within an application.  Since the Diags.STATUS bits are set
 *  to ALWAYS_ON by default, no explicit code is required to enable the
 *  Diags Masks for these events.
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAErr = xdc.useModule('ti.uia.events.UIAErr');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerSys = xdc.useModule('xdc.runtime.LoggerSys');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var logger = LoggerSys.create();
 *
 *  Defaults.common$.logger = logger;
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of STATUS events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAErr = xdc.useModule('ti.uia.events.UIAErr');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Mod = xdc.useModule('my.pkg.Mod');
 *
 *  Mod.common$.diags_STATUS = Diags.RUNTIME_OFF;
 *  @p
 *
 *  This is a part of the C code for the application:
 *
 *  @p(code)
 *  // turn on logging of STATUS events in the module
 *  Diags_setMask("my.pkg.Mod+S");
 *
 *  // turn off logging of STATUS events in the module
 *  Diags_setMask("my.pkg.Mod-S");
 *  @p
 */
module UIAErr inherits IUIAEvent {

    /*!
     *  ======== error ========
     *  Event to use to log an existing software Error Code.
     *
     *  UIAErr_error is used primarily to support logging of legacy error codes with minimal
     *  overhead. Metadata is generated for this event that provides a predefined format
     *  specifier string that can be used to display the error code with.  This minimizes the
     *  number of parameters that need to be logged with the event (it only requires the
     *  actual error code to be logged).  Unlike the Log_error API, no __FILE__ or
     *  __LINE__ information about the call-site is logged with the error code.
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy error code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myErrorCode = 0xEC0DE;
     *  Log_write1(UIAErr_error,myErrorCode);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: ErrorCode:0xECODE"
     *
     *  @param(%x)          integer that identifies the type of error
     *  @see #errorWithStr
     */
    config xdc.runtime.Log.Event error = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: ErrorCode:0x%x"
    };

    /*!
     *  ======== errorWithStr ========
     *  Event to use to log an existing software Error Code and fmt string
     *
     *  UIAErr_errorWithStr is used primarily to support logging of legacy error codes along
     *  with user-defined strings that describe the error. Unlike the Log_error API,
     *  no __FILE__ or __LINE__ information about the call-site is logged with the error code.
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy error code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myErrorCode = 0xEC0DE;
     *  String myErrorStr = "Legacy Error String for error 0xECODE";
     *  ...
     *  Log_write2(UIAErr_errorWithStr,myErrorCode,(IArg)myErrorStr);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: ErrorCode:0xECODE. Legacy Error String for error 0xECODE"
     *
     *  @param(%x)          integer that identifies the type of error
     *  @param(%$S)         fmt string used to format the event message.
     *
     *  Up to 6 additional arguments can be logged with this event if
     *  required.  The formatting string should provide format specifiers
     *  for each of the additional arguments
     *
     *  @see #error
     *
     */
    config xdc.runtime.Log.Event errorWithStr = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: ErrorCode:0x%x. %$S"
    };

    /*!
     *  ======== hwError ========
     * hardware error event code
     *
     * Used to log a generic hardware error.
     * Unlike the Log_error API, no __FILE__ or __LINE__ information about
     *  the call-site is logged with the error code.
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy error code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myHWErrorCode = 0xEC0DE;
     *  ...
     *  Log_write1(UIAErr_hwError,myHWErrorCode);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "HW ERROR: ErrorCode:0xECODE."
     *
     * @param(%x)          integer that identifies the type of warning
     * @see #error
     */
    config xdc.runtime.Log.Event hwError = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "HW ERROR: ErrorCode:0x%x"};

    /*!
     *  ======== hwErrorWithStr ========
     * hardware error event code and fmt string
     *
     * Used to log a generic hardware error.
     *  @a(Example)
     *   The following C code shows how to log a legacy error code and an
     *   associated string as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myHWErrorCode = 0xEC0DE;
     *  String myHWErrorStr = "My description of hardware error 0xEC0DE";
     *  ...
     *  Log_write2(UIAErr_hwErrorWithStr,myHWErrorCode,(IArg)myHWErrorStr);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "HW ERROR: ErrorCode:0xECODE. My description of hardware error 0xEC0DE"
     *
     *  @param(%x)          integer that identifies the type of error
     *  @param(%$S)         fmt string used to format the event message.
     *
     *  Up to 6 additional arguments can be logged with this event if
     *  required.  The formatting string should provide format specifiers
     *  for each of the additional arguments
     *
     *  @see #error
     *
     */
    config xdc.runtime.Log.Event hwErrorWithStr = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "HW ERROR: ErrorCode:0x%x. %$S"};

    /*!
     *  ======== fatal ========
     * fatal error code
     *
     * Used to log a fatal, nonrecoverable error (Event level = EMERGENCY)
     *
     *  @a(Example)
     *   The following C code shows how to log a fatal error code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myFatalErrorCode = 0xDEADC0DE;
     *  ...
     *  Log_write1(UIAErr_fatal,myFatalErrorCode);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "FATAL ERROR: ErrorCode:0xDEADC0DE."
     *  @param(%x)          integer that identifies the type of error
     *  @see #error
     */
    config xdc.runtime.Log.Event fatal = {
            mask: Diags.STATUS,
            level: Diags.EMERGENCY,
            msg: "FATAL ERROR: ErrorCode:0x%x"};

    /*!
     *  ======== fatalWithStr ========
     * fatal error event code and fmt string
     *
     * Used to log a fatal, nonrecoverable error.
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy error code and an
     *   associated string as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myFatalErrorCode = 0xDEADC0DE;
     *  String myFatalErrorStr = "My description of fatal error 0xDEADC0DE";
     *  ...
     *  Log_write2(UIAErr_fatalWithStr,myFatalErrorCode,(IArg)myFatalErrorStr);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "FATAL ERROR: ErrorCode:0xDEADC0DE. My description of fatal error 0xDEADC0DE"
     *
     *  @param(%x)          integer that identifies the type of error
     *  @param(%$S)         fmt string used to format the event message.
     *
     *  Up to 6 additional arguments can be logged with this event if
     *  required.  The formatting string should provide format specifiers
     *  for each of the additional arguments
     *  @see #error
     */
    config xdc.runtime.Log.Event fatalWithStr = {
            mask: Diags.STATUS,
            level: Diags.EMERGENCY,
            msg: "FATAL ERROR: ErrorCode:0x%x. %$S"};

    /*!
     *  ======== critical ========
     * critical error event code
     *
     *  Used to log a critical error in which the system
     *  has e.g. had to kill certain operations in order to
     *  keep running.  (Event Level = CRITICAL)
     *
     *  @a(Example)
     *   The following C code shows how to log a fatal error code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myCriticalErrorCode = 0xACODE;
     *  ...
     *  Log_write1(UIAErr_critical,myCriticalErrorCode);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "CRITICAL ERROR: ErrorCode:0xACODE."
     *  @param(%x)          integer that identifies the type of error
     *
     *  @see #error
     */
    config xdc.runtime.Log.Event critical = {
            mask: Diags.STATUS,
            level: Diags.CRITICAL,
            msg: "CRITICAL ERROR: ErrorCode:0x%x"};

    /*!
     *  ======== criticalWithStr ========
     * critical error event code and fmt string
     *
     * Used to log a critical error in which the system
     * has e.g. had to kill certain operations in order to
     * keep running.  (Event Level = CRITICAL)
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy error code and an
     *   associated string as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Int myCriticalErrorCode = 0xAC0DE;
     *  ...
     *  Log_write2(UIAErr_criticalWithStr,myCriticalErrorCode,(IArg)"My description of critical error 0xAC0DE");
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "CRITICAL ERROR: ErrorCode:0xAC0DE. My description of critical error 0xAC0DE"
     *
     *  @param(%x)          integer that identifies the type of error
     *  @param(%$S)         fmt string used to format the event message.
     *
     *  Up to 6 additional arguments can be logged with this event if
     *  required.  The formatting string should provide format specifiers
     *  for each of the additional arguments
     *
     *  @see #error
     */
    config xdc.runtime.Log.Event criticalWithStr = {
            mask: Diags.STATUS,
            level: Diags.CRITICAL,
            msg: "CRITICAL ERROR: ErrorCode:0x%x. %$S"};
    /*!
     *  ======== exception ========
     * exception event code
     *
     * Used to log that an exception occurred.
     * Typically used with LogSnapshot or LogCrashDump APIs to
     * log stack dump data that identifies why the exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_exception,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *
     *  @see #error
     */
    config xdc.runtime.Log.Event exception = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Exception at %$F."};

    /*!
     *  ======== uncaughtException ========
     * uncaughtException event code
     *
     * Used to log that an uncaught exception occurred.
     * Typically used with LogSnapshot or LogCrashDump APIs to
     * log stack dump data that identifies why the exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_uncaughtException,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Uncaught Exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *
     */
    config xdc.runtime.Log.Event uncaughtException = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Uncaught Exception at %$F."};

    /*!
     *  ======== nullPointerException ========
     * nullPointerException event code
     *
     * Used to log that a null pointer exception occurred.
     * Typically used with LogSnapshot or LogCrashDump APIs to
     * log stack dump data that identifies why the exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_nullPointerException,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Null Pointer Exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event nullPointerException = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Null Pointer Exception at %$F."};

    /*!
     *  ======== unexpectedInterrupt ========
     * unexpectedInterrupt event code
     *
     * Used to log that an unexpected interrupt occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_unexpectedInterrupt,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Null Pointer Exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event unexpectedInterrupt = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Unexpected Interrupt at %$F."};

    /*!
     *  ======== memoryAccessFault ========
     * memoryAccessFault event code
     *
     * Used to log that a memory access fault occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write3(UIAErr_memoryAccessFault,(IArg)__FILE__,__LINE__,(IArg)badAdrs);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Memory Access Fault at demo.c line 1234. [ADRS]0xFFFFFFFF"
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *  @param(adrs)       The address that caused the memory access fault
     */
    config xdc.runtime.Log.Event memoryAccessFault = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Memory Access Fault at %$F. [ADRS]0x%x"};

    /*!
     *  ======== securityException ========
     * securityException event code
     *
     * Used to log that a security exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_securityException,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Security Exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event securityException = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Security Exception at %$F."};

    /*!
     *  ======== divisionByZero ========
     * divisionByZero event code
     *
     * Used to log that divide by zero exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_divisionByZero,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Division by zero at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event divisionByZero = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Division by zero at %$F."};

    /*!
     *  ======== overflowException ========
     * overflowException event code
     *
     * Used to log that an overflow exception occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_overflowException,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Overflow exception at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event overflowException = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Overflow exception at %$F."};

    /*!
     * ======== indexOutOfRange ========
     * indexOutOfRange event code
     *
     * Used to log that an index out of range condition occurred.
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write3(UIAErr_indexOutOfRange,(IArg)__FILE__,__LINE__,badIndex);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: OIndex out of range at demo.c line 1234. [INDEX]0xFFFFFFFF"
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *  @param(index)      The index value that was out of range
     */
    config xdc.runtime.Log.Event indexOutOfRange = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Index out of range at %$F. [INDEX]0x%x"};

    /*!
     * ======== notImplemented ========
     * notImplemented event code
     *
     * Used to log that an attempt to access a feature that is not implemented
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_notImplemented,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Attempt to access feature that is not implemented at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event notImplemented = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Attempt to access feature that is not implemented at %$F."};

    /*!
     * ======== stackOverflow ========
     * stackOverflow event code
     *
     * Used to log that a stack overflow was detected.  (Event Level = CRITICAL)
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_stackOverflow,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Stack Overflow detected at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event stackOverflow = {
            mask: Diags.STATUS,
            level: Diags.CRITICAL,
            msg: "ERROR: Stack Overflow detected at %$F."};

    /*!
     * ======== illegalInstruction ========
     * illegalInstruction event code
     *
     * Used to log that an illegal instruction was executed
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_illegalInstruction,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Illegal Instruction executed at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event illegalInstruction = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Illegal Instruction executed at %$F."};

    /*!
     *  ======== entryPointNotFound  ========
     *  entryPointNotFound event code
     *
     *  Used to log that a module or DLL entry point was not found
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_entryPointNotFound,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Entry Point Not Found at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event entryPointNotFound = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Entry Point Not Found at %$F."};

    /*!
     *  ======== moduleNotFound  ========
     *  moduleNotFound event code
     *
     *  Used to log that a module was not found
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write3(UIAErr_moduleNotFound,(IArg)__FILE__,__LINE__,moduleIdThatWasNotFound);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Module not found at demo.c line 1234. [MODULE_ID]0x32903"
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *  @param(moduleId)   The Module ID of the module that was not found
     */
    config xdc.runtime.Log.Event moduleNotFound = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Module not found at %$F. [MODULE_ID]0x%x."};

    /*!
     *  ======== floatingPointError  ========
     *  floatingPointError event code
     *
     *  Used to log that a floating point error occurred
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Log_write2(UIAErr_floatingPointError,(IArg)__FILE__,__LINE__);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Floating Point Error at demo.c line 1234."
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     */
    config xdc.runtime.Log.Event floatingPointError = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Floating Point Error at %$F."};

    /*!
     *  ======== invalidParameter  ========
     *  invalidParameter event code
     *
     *  Used to log that an invalid parameter was detected
     *  @a(Example)
     *   The following C code shows how to log an exception that identifies
     *  the file and line number that the exception occurred at as a UIA event.
     *  Comments in the code can then provide further explication of the error.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAErr.h>
     *  ...
     *  Void myFunc(Int caseNumber){
     *    switch(caseNumber){
     *      ...
     *      break;
     *      default :
     *         Log_write4(UIAErr_floatingPointError,(IArg)__FILE__,__LINE__,1,caseNumber);
     *    }
     *  }
     *
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "ERROR: Invalid Parameter at demo.c line 1234. [ParamNum]1 [ParamValue]0xFFFFFFFF"
     *  @param(__FILE__)   The file that the exception occurred in
     *  @param(__LINE__)   The line that the exception occurred at
     *  @param(paramNum)   The parameter number in the function's signature that was invalid
     *  @param(paramValue) The invalid parameter value
     */
    config xdc.runtime.Log.Event invalidParameter = {
            mask: Diags.STATUS,
            level: Diags.ERROR,
            msg: "ERROR: Invalid Parameter at %$F. [ParamNum]%d [ParamValue]0x%x"};

}
