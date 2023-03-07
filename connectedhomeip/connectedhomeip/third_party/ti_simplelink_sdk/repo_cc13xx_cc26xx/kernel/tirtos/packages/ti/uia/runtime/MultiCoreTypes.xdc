/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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
 *  ======== MultiCoreTypes.xdc ========
 *
 */

package ti.uia.runtime;

import xdc.runtime.Assert;

/*!
 *  @_nodoc
 *  ======== Udp ========
 */
module MultiCoreTypes
{
    /*!
     *  ======== ServiceHdr ========
     */
    struct ServiceHdr {
        Char msgHdr[32];
        UIAPacket.Hdr packet;
    };

    /*!
     *  Assert raised when a MessageQ call fails
     */
    config Assert.Id A_ipcFailed  = {
        msg: "A_ipcFailed: Unexpected MessageQ failed"
    };

    /*!
     *  Assert raised an invalid HdrType is specified
     */
    config Assert.Id A_invalidHdrType  = {
        msg: "A_invalidHdrType: Invalid HdrType specified"
    };

    /*!
     *  The different Actions for router MessageQ
     */
    enum Action {
        Action_TOHOST = 0,
        Action_FROMHOST,
        Action_REGISTER,
        Action_STOP,
        Action_STOPACK
    };


    /*!
     *  Prefix for slave's message queue names
     */
    const String SLAVENAME = "uiaSlave";

    /*!
     *  Master's message queue names
     */
    const String MASTERNAME = "uiaMaster";

    /*!
     *  Master's Started message queue names
     */
    const String MASTERSTARTED = "uiaStarted";


    /*!
     *  Internal register function
     */
    struct RegisterMsg {
        Char               msgHdr[32];
        UInt32             remoteMQ;
    }
}
