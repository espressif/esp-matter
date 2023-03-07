/*
 * Copyright (c) 2012-2015, Texas Instruments Incorporated
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
 *  ======== LoggerSM.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Timestamp.h>

#include <ti/uia/runtime/EventHdr.h>

#ifdef xdc_target__isaCompatible_64P
#include <c6x.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "package/internal/LoggerSM.xdc.h"

/*
 *  ======== LoggerSM_setPartitionId ========
 */
Void LoggerSM_setPartitionId(Int partitionId)
{
    LoggerSM_module->partitionId = partitionId;
}

/*
 *  ======== LoggerSM_setupBuffer ========
 */
Void LoggerSM_setupBuffer(Ptr sharedMemory, Bits32 sharedMemorySize)
{
    SizeT partitionSize = sharedMemorySize / LoggerSM_numPartitions;
    SizeT bufferSize;

    Assert_isTrue(partitionSize > 256, NULL);

    /* Adjust to be a multiple of 128 */
    partitionSize = partitionSize & (~127);

    /*
     * The actual buffer size
     * Note: 128 pad at the beginning for partition info
     *       128 pad at the end for easier writing of records
     *
     */
    bufferSize = partitionSize - 256;

    /*
     *  TODO: Make sure each partition has a buffer that is a multiple of
     *  cacheline
     */
    LoggerSM_module->sharedObj = (LoggerSM_SharedObj *)((Char *)sharedMemory +
            partitionSize * LoggerSM_module->partitionId);
    LoggerSM_module->sharedObj->buffer =
        (Char *)((Char *)(LoggerSM_module->sharedObj) + 128);
    LoggerSM_module->sharedObj->version = LoggerSM_VERSION;
    LoggerSM_module->sharedObj->numPartitions = LoggerSM_numPartitions;
    LoggerSM_module->sharedObj->readPtr = LoggerSM_module->sharedObj->buffer;
    LoggerSM_module->sharedObj->writePtr = LoggerSM_module->sharedObj->buffer;
    LoggerSM_module->sharedObj->endPtr =
        (Char *)((Char *)(LoggerSM_module->sharedObj->buffer) + bufferSize);
    LoggerSM_module->sharedObj->bufferSizeMAU = bufferSize;
    LoggerSM_module->sharedObj->droppedEvents = 0;
    LoggerSM_module->sharedObj->decode = LoggerSM_decode;
    LoggerSM_module->sharedObj->overwrite = LoggerSM_overwrite;
    LoggerSM_module->sharedObj->moduleId = ti_uia_runtime_LoggerSM_Module_id();
    LoggerSM_module->sharedObj->instanceId = 1;
    LoggerSM_module->sharedObj->headerTag = 0x14011938;
}


/*
 *  ======== LoggerSM_setSharedMemory ========
 */
Bool LoggerSM_setSharedMemory(Ptr sharedMemory, Bits32 sharedMemorySize)
{
    /* Make sure buffer was not already set */
    if (LoggerSM_module->sharedObj != NULL) {
        return (FALSE);
    }

    LoggerSM_setupBuffer(sharedMemory, sharedMemorySize);

    return (TRUE);
}

/*
 *  ======== LoggerSM_Module_startup ========
 */
Int LoggerSM_Module_startup(Int phase)
{
    /*
     *  Make sure there is a buffer to setup. There is none when
     *  LoggerSM.sharedMemorySize is set to 0.
     */
    if (LoggerSM_sharedMemorySize != 0) {
        LoggerSM_setupBuffer(LoggerSM_module->sharedBuffer,
                LoggerSM_sharedMemorySize);
    }

    return (Startup_DONE);
}

/*
 *  ======== LoggerSM_enable ========
 */
Bool LoggerSM_enable(LoggerSM_Object *obj)
{
    Bool prev;
    IArg key;

    key = Gate_enterSystem();

    prev = LoggerSM_module->enabled;
    LoggerSM_module->enabled = TRUE;

    Gate_leaveSystem(key);

    return (prev);
}

/*
 *  ======== LoggerSM_disable ========
 */
Bool LoggerSM_disable(LoggerSM_Object *obj)
{
    Bool prev;
    IArg key;

    key = Gate_enterSystem();

    prev = LoggerSM_module->enabled;
    LoggerSM_module->enabled = FALSE;

    Gate_leaveSystem(key);

    return (prev);
}

/*
 *  ======== LoggerSM_setFilterLevel ========
 *  Sets the filter level for the given diags level.
 *
 *  LoggerSM maintains a separate filter level for every diags category.
 *  This is accomplished by maintaining three masks, one for each of the levels
 *  1 - 3, wich store the diags categories which are currently at that level.
 *  There is no mask for level4; if the diags category is not found in levels
 *  1-3, it is  assumed that the filtering level is level4.
 *
 *  This API is an instance function per the IFilterLogger interface, but
 *  LoggerSM only maintains module-wide filter levels.
 *
 *  TODO - Should this be conditional on the 'filterByLevel' config?
 */
Void LoggerSM_setFilterLevel(LoggerSM_Object *obj,
                                  xdc_runtime_Diags_Mask mask,
                                  xdc_runtime_Diags_EventLevel filterLevel)
{
    /*
     * First, remove the bits in 'mask' from all of the current 'level' masks.
     * Use level = (~(mask & level) & level) to remove 'mask' bits from all
     * 'level's.
     *    1. AND mask and level to get set of bits that appear in both
     *    2. Take the inverse of this set and AND it with 'level' to disable
     *       any bits which appear in 'mask'.
     */
    LoggerSM_module->level1 = ~(LoggerSM_module->level1 & mask) &
                               LoggerSM_module->level1;
    LoggerSM_module->level2 = ~(LoggerSM_module->level2 & mask) &
                               LoggerSM_module->level2;
    LoggerSM_module->level3 = ~(LoggerSM_module->level3 & mask) &
                               LoggerSM_module->level3;

    /* Enable the bits specified in 'mask' in the appropriate level. */
    switch (filterLevel) {
        case Diags_LEVEL1:
            LoggerSM_module->level1 |= mask;
            break;
        case Diags_LEVEL2:
            LoggerSM_module->level2 |= mask;
            break;
        case Diags_LEVEL3:
            LoggerSM_module->level3 |= mask;
            break;
        case Diags_LEVEL4:
            break;
        default: {
            /* Raise an error that a bad filter level was received. */
            Error_Block eb;
            Error_init(&eb);
            Error_raise(&eb, LoggerSM_E_badLevel, filterLevel, 0);
            break;
        }
    }
}

/*
 *  ======== LoggerSM_getFilterLevel ========
 *  Returns the mask of diags categories which are set to the specified filter
 *  level.
 *
 *  This API is an instance function per the IFilterLogger interface, but
 *  LoggerSM only maintains module-wide filter levels.
 */
Diags_Mask LoggerSM_getFilterLevel(LoggerSM_Object *obj,
                                    xdc_runtime_Diags_EventLevel level)
{
    /* Return the mask of diags categories associated with 'level'. */
    switch (level) {
        case Diags_LEVEL1:
            return (LoggerSM_module->level1);
        case Diags_LEVEL2:
            return (LoggerSM_module->level2);
        case Diags_LEVEL3:
            return (LoggerSM_module->level3);
        case Diags_LEVEL4:
            /*
             * Return the inverse of all the bits set in levels 1 - 3,
             * and only return those bits which belong to the set of
             * logging categories (excludes asserts and the two level bits).
             */
            return (~(LoggerSM_module->level1 |
                      LoggerSM_module->level2 |
                      LoggerSM_module->level3) &
                     xdc_runtime_Diags_ALL_LOGGING);
        default: {
            /* Raise an error to report the bad filter level. */
            Error_Block eb;
            Error_init(&eb);
            Error_raise(&eb, LoggerSM_E_badLevel, level, 0);
            return (0);
        }
    }
}

/*
 *  ======== LoggerSM_filterOutEvent ========
 *  Returns TRUE if the event should be filtered out.
 *  TODO - Might be more clearly written as 'passesFilter'?
 */
Bool LoggerSM_filterOutEvent(xdc_runtime_Diags_Mask mask)
{
    /*
     * If filtering for the event's diags category is currently
     * set to level1...
     */
    if (LoggerSM_module->level1 & mask) {
        /* If the event is lower than level1, filter it out. */
        return(Diags_compareLevels(Diags_getLevel(mask), Diags_LEVEL1));
    }
    /*
     * If filtering for the event's diags category is currently
     * set to level2...
     */
    else if (LoggerSM_module->level2 & mask) {
        /* If the event is lower than level2, filter it out. */
        return(Diags_compareLevels(Diags_getLevel(mask), Diags_LEVEL2));
    }
    /*
     * If filtering for the event's diags category is currently
     * set to level3...
     */
    else if (LoggerSM_module->level3 & mask) {
        /* If the event is lower than level3, filter it out. */
        return(xdc_runtime_Diags_compareLevels(xdc_runtime_Diags_getLevel(mask),
                                               xdc_runtime_Diags_LEVEL3));
    }
    /*
     * Otherwise, the filter level must be level4, which means that events of
     * all levels should be logged.
     */
    else {
        return (FALSE);
    }
}

/*
 *  ======== LoggerSM_reset ========
 */
Void LoggerSM_reset(LoggerSM_Object *obj)
{
    /* Do not do any work if there is no shared object yet */
    if (LoggerSM_module->sharedObj == NULL) {
        return;
    }

    // assert that logger is disabled?
    // TODO memset buffer?
    LoggerSM_module->sharedObj->readPtr = LoggerSM_module->sharedObj->buffer;
    LoggerSM_module->sharedObj->writePtr = LoggerSM_module->sharedObj->buffer;
    LoggerSM_module->serial        = 1;

    LoggerSM_module->enabled = TRUE;
}
