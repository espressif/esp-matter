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
 * ======== UIAMessage.xdc ========
 */

import xdc.runtime.Diags;

/*!
 * UIA Message Events
 *
 * The UIAMessage module defines events that allow
 * tooling to monitor messages between tasks and CPUs.
 *
 *  The generation of UIAMessage events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIAMessage` info events are generated only when the Diags.INFO bit is set
 *  in the module's diagnostics mask.
 *
 *  The following configuration script demonstrates how to enable use of
 *  UIAMessage events within an application. The Diags.INFO bitmust be explicitly set
 *  in order to enable these events.
 *
 *  This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAMessage = xdc.useModule('ti.uia.events.UIAMessage');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerSys = xdc.useModule('xdc.runtime.LoggerSys');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var logger = LoggerSys.create();
 *
 *  Defaults.common$.logger = logger;
 *  Defaults.common$.diags_INFO = Diags.ALWAYS_ON;
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of STATUS events and INFO events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAMessage = xdc.useModule('ti.uia.events.UIAMessage');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Mod = xdc.useModule('my.pkg.Mod');
 *
 *  Mod.common$.diags_STATUS = Diags.RUNTIME_OFF;
 *  Mod.common$.diags_INFO = Diags.RUNTIME_OFF;
 *  @p
 *
 *  This is a part of the C code for the application:
 *
 *  @p(code)
 *  // turn on logging of STATUS events (S) and INFO events (F)
 *  // in the module
 *  Diags_setMask("my.pkg.Mod+SF");
 *
 *  // turn off logging of STATUS events and INFO events in the module
 *  Diags_setMask("my.pkg.Mod-SF");
 *  @p
 */
module UIAMessage inherits IUIAEvent {

    /*! ======= msgSent ======
     * Message Sent event
     *  @a(Example)
     *   The following C code shows how to log a msgSent event
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAMessage.h>
     *  ...
     *  Log_write7(UIAMessage_msgSent, numBytes,flags,msgId,destProcId,replyProcId,srcProcId,heapId);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Msg Sent [NumBytes]0x32 [Flags]0xF [MsgId]0x8357 [Dest(ProcID)]0x2 [Reply(ProcID)]0x3 [Src(ProcID)]0x1 [HeapId]0x56"
     *
     * @param(NumBytes) length of the message in bytes
     * @param(Flags) any flags associated with the message
     * @param(MsgId) the message ID for the message
     * @param(Destination ProcID) the procID for the message destination
     * @param(Reply ProcID) the procID that the reply will be sent to
     * @param(Source ProcID) the procID of the message sender
     * @param(HeapId) the HeapID associated with the message
     */
    config xdc.runtime.Log.Event msgSent = {
        mask: Diags.INFO,
        msg: "Msg Sent [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"
    };

    /*! ======= msgReceived ======
     * Message Received event
     *  @a(Example)
     *   The following C code shows how to log a msgReceived event
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAMessage.h>
     *  ...
     *  Log_write7(UIAMessage_msgReceived, numBytes,flags,msgId,destProcId,replyProcId,srcProcId,heapId);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Msg Received [NumBytes]0x32 [Flags]0xF [MsgId]0x8357 [Dest(ProcID)]0x2 [Reply(ProcID)]0x3 [Src(ProcID)]0x1 [HeapId]0x56"
     *
     * @param(NumBytes) length of the message in bytes
     * @param(Flags) any flags associated with the message
     * @param(MsgId) the message ID for the message
     * @param(Destination ProcID) the procID for the message destination
     * @param(Reply ProcID) the procID that the reply will be sent to
     * @param(Source ProcID) the procID of the message sender
     * @param(HeapId) the HeapID associated with the message
     */
    config xdc.runtime.Log.Event msgReceived = {
        mask: Diags.INFO,
        msg: "Msg Received [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"
    };

    /*! ======= replySent ======
     * Reply Sent event
     *  @a(Example)
     *   The following C code shows how to log a msgSent event
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAMessage.h>
     *  ...
     *  Log_write7(UIAMessage_replySent, numBytes,flags,msgId,destProcId,replyProcId,srcProcId,heapId);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Reply Sent [NumBytes]0x32 [Flags]0xF [MsgId]0x8357 [Dest(ProcID)]0x2 [Reply(ProcID)]0x3 [Src(ProcID)]0x1 [HeapId]0x56"
     *

     * @param(NumBytes) length of the message in bytes
     * @param(Flags) any flags associated with the message
     * @param(MsgId) the message ID for the message
     * @param(Destination ProcID) the procID for the message destination
     * @param(Reply ProcID) the procID that the reply will be sent to
     * @param(Source ProcID) the procID of the message sender
     * @param(HeapId) the HeapID associated with the message
     */
    config xdc.runtime.Log.Event replySent = {
        mask: Diags.INFO,
        msg: "Reply Sent [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"
    };

    /*! ======= replyReceived ======
     * Reply Received event
     *  @a(Example)
     *   The following C code shows how to log a msgSent event
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAMessage.h>
     *  ...
     *  Log_write5(UIAMessage_replyReceived,msgId,replyId,numBytes,senderProcId,destProcId);
     *  ...
     *  @p  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Msg Sent [MsgId]0x8357 [NumBytes]0x32 [Flags]0xF [Dest(ProcID)]0x2 [Reply(ProcID)]0x3 [Src(ProcID)]0x1 [HeapId]0x56"
     *
     * @param(MsgId) the message ID for the message
     * @param(ReplyId) the reply's message ID
     * @param(NumBytes) length of the message in bytes
     * @param(Sender ProcID) the procID that the reply will be sent to
     * @param(Destination ProcID) the procID for the message destination
     */
    config xdc.runtime.Log.Event replyReceived = {
        mask: Diags.INFO,
        msg: "Reply Recieved [MsgId]0x%x [ReplyId]0x%x [NumBytes]0x%x [Sender]0x%x [Dest]0x%x "
    };

}
