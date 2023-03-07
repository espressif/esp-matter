/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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

package ti.uia.runtime

import xdc.runtime.Types;
import xdc.rov.ViewInfo;

/*!
 *  @_nodoc
 *  ======== QueueDescriptor.xdc ========
 *  Module to manage descriptors that are used by an instrumentation host
 *
 *  The QueueDescriptor module maintains a list of descriptors. Each descriptor
 *  contains information that a host tool can use to get debug information
 *  about a debug buffer (e.g. LoggerStopMode instance buffer).
 *
 *  The QueueDescriptor module contains two global variables that the host
 *  can access.
 *  @p(blist)
 *   - ti_uia_runtime_QueueDescriptor_gPtrToFirstDescriptor:
 *     pointer to the first queue descriptor. The rest of the active descriptors
 *     are maintained as list off the first one (via the next pointer in the
 *     structure).
 *     Global variable needed to manage reading / writing the circular buffer
 *     from the DebugServer. Initialized to null.  A non-null value indicates
 *     that the descriptor it points to has been fully initialized.
 *
 *  - ti_uia_runtime_QueueDescriptor_gUpdateCount:
 *     Integer value read by the host to determine whether it needs to walk the
 *     list of descriptors
 *     Incremented whenever a queue descriptor is modified, added or removed
 *     from the list of descriptors. If the value has not changed, then the host
 *     can safely use cached versions of the descriptors.
 *  @p
 */
@CustomHeader
module QueueDescriptor {

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Ptr mPtrToFirstDescriptor;
        UInt mUpdateCount;
        UInt16 is5555ifInitialized;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [['Module', {type: ViewInfo.MODULE,
                                  viewInitFxn: 'viewInitModule',
                                  structName: 'ModuleView'}
                     ]]
        });

    /*!
     *  ======== QueueType ========
     *  Type of Queue.  We use constants instead of enums as a
     *  work-around for bug DVT-1928.
     */
    const UInt QueueType_NONE = 0;
    const UInt QueueType_TOHOST_CMD_CIRCULAR_BUFFER = 1;
    const UInt QueueType_FROMHOST_CMD_CIRCULAR_BUFFER = 2;
    const UInt QueueType_TOHOST_EVENT_CIRCULAR_BUFFER = 3;
    const UInt QueueType_TOHOST_EVENT_OVERFLOW_BUFFER = 4;
    const UInt QueueType_TOHOST_DATA_CIRCULAR_BUFFER = 5;
    const UInt QueueType_FROMHOST_DATA_CIRCULAR_BUFFER = 6;
    const UInt QueueType_TOHOST_EVENT_UIAPACKET_ARRAY = 7;
    const UInt QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE = 8;

    /*!
     *  ======== Header ========
     *  Structure of the descriptor
     *
     *  @field(structSize)       Used for version control to determine if newer
     *                           fields are available
     *  @field(next)             Pointer to the next Header in the list
     *  @field(queueType)        Identifies the type of queue and thus who owns
     *                           the read and write pointers.
     *  @field(readPtr)          Points to the next (word-aligned) byte to be
     *                           read from the buffer
     *  @field(writePtr)         Points to the next (word-aligned) byte to be
     *                           written into
     *  @field(queueStartAdrs)   Start address of the buffer (word-aligned)
     *  @field(queueSizeInMAUs)  Queue Size in min. addressable units
     *                           (buffer size must be word-aligned)
     *  @field(instanceId)       16b unique ID that identifies the instance of the module
     *                           that owns this queue descriptor.
     *                           b15=1 indicates that the logger was dynamically
     *                           created. Corresponds to the logger instance Id
     *                           in the rta.xml and uia.xml metadata and
     *                           UIAPacket event packet header.
     *  @field(ownerModuleId)    The module ID of the module that owns this
     *                           queue descriptor
     *  @field(priority)         The priority of the queue. 0 is normal priority.
     *                           The higher the number, the higher the priority.
     *                           Used to set the priority field of the UIAPacket
     *                           event packet header.
     *  @field(numDroppedCtrAdrs) Points to the counter used to count the number
     *                           of dropped events.  NULL if no counter available.
     *  @field(partialPacketWritePtr) Points to the write pointer used by the logger
     *                           while writing a packet. For use by the host only when the
     *                           target is halted.
     */
    struct Header {
        Int structSize;
        Header *next;
        UInt queueType;
        Bits32 *readPtr;
        Bits32 *writePtr;
        Bits32 *queueStartAdrs;
        SizeT queueSizeInMAUs;
        UInt instanceId;
        UInt ownerModuleId;
        UInt priority;
        Bits32 *numDroppedCtrAdrs;
        Bits32 *partialPacketWritePtr;
    };

    /*!
     *  ======== addToList ========
     *  Function to add a descriptor to the global list.
     *
     *  @param(pHdrToAdd) Descriptor to add
     */
    @DirectCall
    Void addToList(QueueDescriptor.Header *pHdrToAdd);

    /*
     *  ======== generateInstanceId ========
     *  Returns a unique logger instance ID for use by a logger that
     *  implements the IUIATransfer interface.
     *
     *  Note that all Logger Instance Ids must be non-zero.
     *  LoggerInstanceIds with b15=1 are reserved for dynamically created
     *  instances of the logger.
     */
    metaonly UInt16 generateInstanceId();

    /*!
     *  ======== initHeader ========
     *  Function initialize a descriptor
     *
     *  @param(pHdr)  Descriptor to initialize
     *  @param(start) Start address of the buffer
     *  @param(size)  Size of the buffer
     *  @param(loggerModuleId) module ID of the logger that owns the buffer
     *  @param(loggerInstanceId) instance ID of the logger that owns the buffer
     *  @param(loggerPriority) priority of the logger that owns the buffer
     *  @param(type)  Type of descriptor
     */
    @DirectCall
    Void initHeader(QueueDescriptor.Header *pHdr, Ptr start,
                   SizeT size, UInt loggerModuleId, UInt loggerInstanceId,
                   UInt loggerPriority, UInt type, Ptr pNumDroppedCtr);

    /*!
     *  ======== removeFromList ========
     *  Function to remove a descriptor from the global list.
     *
     *  @param(pHdrToRemove) Descriptor to remove
     */
    @DirectCall
    Void removeFromList(QueueDescriptor.Header *pHdrToRemove);

internal:
    metaonly config Int maxId = 0;

    struct Module_State {
        Ptr mPtrToFirstDescriptor;
        UInt mUpdateCount;

        /*
         *  Set to true after pointer to list of queue descriptors has been
         *  set.
         */
        UInt32 is5555ifInitialized;
    };
}
