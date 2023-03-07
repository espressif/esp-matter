/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
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
 *  ======== LogSnapshot__epilogue.h ========
 *  Implementation of the LogSnapshot_* macros
 *
 *  The implementation below relies on five symbols defined by every module
 *  header.  Each of these symbols is a reference to a constant defined in a
 *  separate compilation unit.  The values of these constants are as follows:
 *
 *      Module__MID     - the module's ID (see Text.xs)
 *      LogSnapshot__LOGOBJ  - the module's logger object
 *      LogSnapshot__LOGMEMORYFXN - the module's logger's write0 function
 *      LogSnapshot__LOGDEF  - 0 if the module has a logger, non-zero otherwise
 */
#include <ti/uia/events/UIASnapshot.h>


#define LogSnapshot_IDTAG_BIT_SHIFT       16
#define LogSnapshot_IDTAG_MASK            0xFFFF0000
#define LogSnapshot_SNAPSHOTID_BIT_SHIFT  0
#define LogSnapshot_SNAPSHOTID_MASK       0x0000FFFF

/*
 *  ======== ti_uia_runtime_LogSnapshot_getMask ========
 */
#define ti_uia_runtime_LogSnapshot_getMask(evt) ((evt) & 0x0000ffff)

/*
 *  ======== ti_uia_runtime_LogSnapshot_getRope ========
 */
#define ti_uia_runtime_LogSnapshot_getRope(evt) ((xdc_runtime_Types_RopeId)((evt) >> 16))

/*
 *  ======== ti_uia_runtime_LogSnapshot_getEventId ========
 */
#define ti_uia_runtime_LogSnapshot_getEventId(evt) ((xdc_runtime_Log_EventId)((evt) >>16))

/*
 *  ======== ti_uia_stm_LogSnapshot_putMemoryRange ========
 *  Unconditionally put the specified `Types` event.
 *  Supports UIASnapshot_memoryRange, UIASnapshot_string and UIASnapshot_nameOfReference.
 *
 *  This method unconditionally puts the specified memoryRange`{@link Types#Event}`
 *  `evt` into the log.  This type of event is created either implicitly
 *  (and passed to an `{@link ISnapshotLogger}` implementation) or explicitly
 *  via `{@link Types#makeEvent()}`.
 *
 *  @param(evt)     the `Types` event to put into the log
 *  @param(mid)         the module ID of the caller
 *  @param(startAdrs)      the start address of the memory range to log
 *  @param(lengthInMAUs)   the number of minimum addressable units (e.g. bytes) to log
 *  @a(return)          value to use as snapshotId parameter for subsequent events
 */
#define ti_uia_runtime_LogSnapshot_putMemoryRange(evt, mid, snapshotId, fileName, lineNum, fmt, startAdrs, lengthInMAUs) \
     (LogSnapshot__LOGMEMORYFXN(LogSnapshot__LOGOBJ, (evt), (mid), (snapshotId), (fileName), (lineNum), (fmt), (startAdrs), (lengthInMAUs)))
/*   (LogSnapshot__LOGMEMORYFXN(LogSnapshot__LOGOBJ, (evt),(mask), (snapshotId), (fileName), (lineNum), (fmt), (startAdrs), (lengthInMAUs)) */

/*
 *  ======== ti_uia_runtime_LogSnapshot_writeStringWithIdTag ========
 *  Log_Events need to be converted to Type_Events prior to calling Log_put;
 *  the lower 16-bits are a mask that needs to be replaced with the caller's
 *  module ID.
 */
#define ti_uia_runtime_LogSnapshot_writeStringWithIdTag(idTag, snapshotId, fmt, startAdrs, lengthInMAUs) \
    ((ti_uia_runtime_LogSnapshot_isSnapshotLoggingEnabled(ti_uia_events_UIASnapshot_stringOnHeap)) ? \
        (ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_stringOnHeap, Module__MID, \
        (IArg)((((UInt32)idTag & (UInt32)LogSnapshot_IDTAG_MASK) << LogSnapshot_IDTAG_BIT_SHIFT) | \
        ((((UInt32)snapshotId) & (UInt32)LogSnapshot_SNAPSHOTID_MASK)<<LogSnapshot_SNAPSHOTID_BIT_SHIFT)),\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) : (Void)0 \
    )

/*
 *  ======== ti_uia_runtime_LogSnapshot_writeString ========
 *  Log_Events need to be converted to Type_Events prior to calling Log_put;
 *  the lower 16-bits are a mask that needs to be replaced with the caller's
 *  module ID.
 */
#define ti_uia_runtime_LogSnapshot_writeString(snapshotId, fmt, startAdrs, lengthInMAUs) \
    (ti_uia_runtime_LogSnapshot_writeStringWithIdTag(0, snapshotId, fmt, startAdrs, lengthInMAUs) )


/*
 *  ======== ti_uia_runtime_LogSnapshot_writeMemoryBlockWithIdTag ========
 *  Log_Events need to be converted to Type_Events prior to calling Log_put;
 *  the lower 16-bits are a mask that needs to be replaced with the caller's
 *  module ID.
 */
#define ti_uia_runtime_LogSnapshot_writeMemoryBlockWithIdTag(idTag, snapshotId, fmt, startAdrs, lengthInMAUs) \
    ((ti_uia_runtime_LogSnapshot_isSnapshotLoggingEnabled(ti_uia_events_UIASnapshot_memoryRange)) ? \
        (ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_memoryRange, Module__MID, \
        (IArg)((((UInt32)idTag & (UInt32)LogSnapshot_IDTAG_MASK) << LogSnapshot_IDTAG_BIT_SHIFT) | \
        ((((UInt32)snapshotId) & (UInt32)LogSnapshot_SNAPSHOTID_MASK)<<LogSnapshot_SNAPSHOTID_BIT_SHIFT)),\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)fmt, startAdrs, lengthInMAUs)) : (Void)0 \
    )

/*
 *  ======== ti_uia_runtime_LogSnapshot_writeMemoryBlock ========
 *  Log_Events need to be converted to Type_Events prior to calling Log_put;
 *  the lower 16-bits are a mask that needs to be replaced with the caller's
 *  module ID.
 */
#define ti_uia_runtime_LogSnapshot_writeMemoryBlock(snapshotId, fmt, startAdrs, lengthInMAUs) \
    ( ti_uia_runtime_LogSnapshot_writeMemoryBlockWithIdTag(0, snapshotId, fmt, startAdrs, lengthInMAUs))

/*
 *  ======== ti_uia_runtime_LogSnapshot_writeNameOfReference ========
 *  Log_Events need to be converted to Type_Events prior to calling Log_put;
 *  the lower 16-bits are a mask that needs to be replaced with the caller's
 *  module ID.
 */
#define ti_uia_runtime_LogSnapshot_writeNameOfReference(refId, fmt, startAdrs, lengthInMAUs) \
    ((ti_uia_runtime_LogSnapshot_isSnapshotLoggingEnabled(ti_uia_events_UIASnapshot_nameOfReference)) ? \
        (ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_nameOfReference, Module__MID, \
        (IArg)refId,(IArg)__FILE__,(IArg)__LINE__, \
         (IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) : (Void)0 \
    )
