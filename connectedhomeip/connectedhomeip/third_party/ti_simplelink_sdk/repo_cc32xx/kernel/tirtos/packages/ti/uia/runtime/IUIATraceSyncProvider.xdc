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
 * ======== IUIATraceSyncProvider.xdc ========
 *  Interface implemented by modules that provide the code needed to
 *  inject a sync point into a trace stream (e.g. CPU Trace, System Trace,
 *  etc.).  Modules that implement IUIATraceSyncProvider are responsible
 *  for providing an XDS script that automatically registers its injectIntoTrace
 8  callback unction
 *  with any modules that implement the IUIATraceSyncClient interface.
 */
interface IUIATraceSyncProvider inherits ti.uia.events.IUIAMetaProvider {

    /*!
     * ======== ContextType ========
     * Enumeration of the various types of packet headers.
     * Stored in a 4 bit bitfield (b31-b28) of the first word in the packet.
     */
    enum ContextType {
        ContextType_Reserved0 = 0, /*! reserved for future use */
        ContextType_SyncPoint = 1, /*! Sync Point event serial number */
        ContextType_ContextChange = 2, /*! Context Change event serial number */
        ContextType_Snapshot = 3, /*! Snapshot event Snapshot ID  */
        ContextType_Reserved4 = 4, /*! reserved for future use  */
        ContextType_Reserved5 = 5, /*! reserved for future use */
        ContextType_Reserved6 = 6, /*! reserved for future use */
        ContextType_Reserved7 = 7, /*! reserved for future use */
        ContextType_Reserved8 = 8, /*! reserved for future use */
        ContextType_Reserved9 = 9, /*! reserved for future use */
        ContextType_Reserved10 = 10, /*! reserved for future use */
        ContextType_Reserved11 = 11, /*! reserved for future use */
        ContextType_Reserved12 = 12, /*! reserved for future use  */
        ContextType_Reserved13 = 13, /*! reserved for future use */
        ContextType_Global32bTimestamp = 14, /*! 32b Global Timestamp */
        ContextType_User = 15  /*! User defined data */
    };

    /*!
     * ====== injectIntoTrace ======
     * Inject syncPoint info into GEM Trace
     *
     * This method logs a sync point event and injects
     * correlation info into the trace stream (if available)
     * to enable correlation between software events and hardware trace.
     *
     * @param(serialNum)   the serial number that is to be injected
     * @param(ctxType)     the context type ID to be injected into the trace
     */
    Void injectIntoTrace(UInt32 serialNum, ContextType ctxType);

}
