/*
 * Copyright (c) 2013-2017, Texas Instruments Incorporated
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
 *  ======== QueueDescriptor.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Startup.h>
#include "package/internal/QueueDescriptor.xdc.h"

#define TI_UIA_QUEUEDESCRIPTOR_INIT_COMPLETE (0x5555)
/*
 *  ======== ti_uia_runtime_QueueDescriptor_gPtrToFirstDescriptor ========
 *  pointer ot the first message buffer descriptor
 *
 *  Global variable needed to manage reading / writing the circular buffer
 *  from the DebugServer. Initialized to null.  A non-null value indicates
 *  that the descriptor it points to has been fully initialized.
 */
volatile Ptr ti_uia_runtime_QueueDescriptor_gPtrToFirstDescriptor = NULL;

/*
 *  ======== ti_uia_runtime_QueueDescriptor_gUpdateCount ========
 *  Integer value read by the host to determine whether it needs to walk the
 *  list of descriptors
 *
 *  Incremented whenever a queue descriptor is modified, added or removed from
 *  the list of descriptors. If the value has not changed, then the host can
 *  safely use cached versions of the descriptors.
 */
volatile UInt ti_uia_runtime_QueueDescriptor_gUpdateCount = 0;

/*
 *  ======== QueueDescriptor_addToList ========
 */
Void QueueDescriptor_addToList(QueueDescriptor_Header *pHdrToAdd)
{
    IArg key;
    QueueDescriptor_Header* pList;

    /* Enter the gate and find the end. */
    key = Gate_enterSystem();

    if (QueueDescriptor_module->is5555ifInitialized != TI_UIA_QUEUEDESCRIPTOR_INIT_COMPLETE) {
        /*
         *  This should have been initialized by the QueueDescriptor.xs file...
         *  Add lazy init code here for situations where this is not supported.
         */
        QueueDescriptor_module->is5555ifInitialized = TI_UIA_QUEUEDESCRIPTOR_INIT_COMPLETE;
        QueueDescriptor_module->mPtrToFirstDescriptor = NULL;
        QueueDescriptor_module->mUpdateCount = 0;
    }

    /*
     *  If the module's mPtrToFirstDescriptor is NULL, set it to point to this
     *  header.
     *  If it is not NULL, traverse the list and add to the end. This
     *  should not happen often and the list should not be long, so the
     *  using a tail is not needed.
     */
    if (pHdrToAdd->structSize == sizeof(ti_uia_runtime_QueueDescriptor_Header)) {
        /*  If mPtrToFirstDescriptor is NULL, set it to point to this header.
         *  If it is not NULL, traverse the list and add to the end. This
         *  should not happen often and the list should not be long, so
         *  using a tail is not needed.
         */
        if (QueueDescriptor_module->mPtrToFirstDescriptor == NULL) {
            QueueDescriptor_module->mPtrToFirstDescriptor = pHdrToAdd;
            /*
             *  If the global gPtrToFirstDescriptor is NULL, set it to point to
             *  this header. If it is not NULL then the memory location it
             *  points to does not contain initialized data.
             *  This can occur in an openMP single image application when
             *  multiple cores are using the same shared memory location for
             *  this global variable.
             */
            if (ti_uia_runtime_QueueDescriptor_gPtrToFirstDescriptor == NULL){
                ti_uia_runtime_QueueDescriptor_gPtrToFirstDescriptor = pHdrToAdd;
            }
        }
        else {
            pList = QueueDescriptor_module->mPtrToFirstDescriptor;
            /* Find the end of the list */
            while (pList->next != NULL) {
                pList = pList->next;
            }

            if (pList != pHdrToAdd) {
                /* Add onto the end */
                pList->next = pHdrToAdd;
                /* Ensure next pointer in the newly added descriptor is NULL */
                pHdrToAdd->next = NULL;
            }
        }

        QueueDescriptor_module->mUpdateCount++;
        /*
         *  Update the global update counter to maintain backwards
         *  compatibility with System Analyzer
         */
        ti_uia_runtime_QueueDescriptor_gUpdateCount++;

        /* Ensure that dynamically created logger instances have a
         * unique instance ID to allow them to be handled properly
         * on the host.  Set b15 to 1 to indicate that the ID was auto-generated
         * and that the logger was dynamically created.
         */
        if (pHdrToAdd->instanceId == 0){
            pHdrToAdd->instanceId = 0x8000 | QueueDescriptor_module->mUpdateCount;
        }
    }
    Gate_leaveSystem(key);
}

/*
 *  ======== QueueDescriptor_initHeader ========
 *  Initial the header
 */
Void QueueDescriptor_initHeader(QueueDescriptor_Header *pHdr, Ptr start,
        SizeT size, UInt loggerModuleId, UInt loggerInstanceId,
        UInt loggerPriority, UInt type, Ptr pNumDroppedCtr)
{
    pHdr->structSize = sizeof(ti_uia_runtime_QueueDescriptor_Header);
    pHdr->next = NULL;
    pHdr->queueType = type;
    pHdr->readPtr = start;
    pHdr->writePtr = start;
    pHdr->queueStartAdrs = start;
    pHdr->queueSizeInMAUs = size;
    pHdr->instanceId = loggerInstanceId;
    pHdr->ownerModuleId = loggerModuleId;
    pHdr->priority = loggerPriority;
    pHdr->numDroppedCtrAdrs = pNumDroppedCtr;
}

/*
 *  ======== QueueDescriptor_removeFromList ========
 */
Void QueueDescriptor_removeFromList(QueueDescriptor_Header *pHdrToRemove)
{
    IArg key;
    QueueDescriptor_Header *pList;

    /* Enter the gate and traverse the list, looking for a match */
    key = Gate_enterSystem();
    if (QueueDescriptor_module->is5555ifInitialized == TI_UIA_QUEUEDESCRIPTOR_INIT_COMPLETE){
        /*
         *  If the list is empty, do nothing.
         *  If the header to remove is the first one, update the global
         *  accordingly else traverse the list looking for a match. Once the
         *  match is found, remove the header.
         *  If the header is never found, do nothing.
         */
        if (QueueDescriptor_module->mPtrToFirstDescriptor != NULL) {
            if (QueueDescriptor_module->mPtrToFirstDescriptor == pHdrToRemove) {
                QueueDescriptor_module->mPtrToFirstDescriptor =
                    pHdrToRemove->next;
                QueueDescriptor_module->mUpdateCount++;

                /*
                 *  Update the global update counter to maintain backwards
                 *  compatibility with System Analyzer.
                 */
                ti_uia_runtime_QueueDescriptor_gUpdateCount++;
            }
            else {
                pList = QueueDescriptor_module->mPtrToFirstDescriptor;
                while (pList->next != NULL)  {

                    if (pList->next == pHdrToRemove) {
                        /* Found a match, remove the header */
                        pList->next = pHdrToRemove->next;
                        QueueDescriptor_module->mUpdateCount++;
                        ti_uia_runtime_QueueDescriptor_gUpdateCount++;
                        break;
                    }

                    pList = pList->next;
                }
            }
        }
    }
    Gate_leaveSystem(key);

    /* Make sure the next pointer is NULL if it is re-used */
    pHdrToRemove->next = NULL;
}
