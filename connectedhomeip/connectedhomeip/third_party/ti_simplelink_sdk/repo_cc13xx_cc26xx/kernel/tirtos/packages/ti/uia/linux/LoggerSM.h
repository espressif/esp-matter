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
 *  ======== LoggerSM.h ========
 */

#ifndef _LOGGERSM_
#define _LOGGERSM_

#include <unistd.h>

/* Return code value */
#define LoggerSM_SUCCESS                (0)
#define LoggerSM_ERROR                 (~0)

/* Default shared memory size for each partition */
#define LoggerSM_DEFAULTPARTITIONBUFFERSIZE   0x20000

/*
 *  Maximum number of partitions. To increase this, make sure
 *  LoggerSM_names initialization in the LoggerSM.c is updated
 *  accordingly.
 */
#define LoggerSM_MAXPARTITIONS  8

/*
 *  Number of milliseconds that is slept if there is no strings
 *  read from the shared memory buffers.
 */
#define LoggerSM_MSECS    10

/* Maximum length of a partition owner's name. */
#define LoggerSM_MAXPARTITIONNAMELEN          40

/*
 *  ======== LoggerSM_run =========
 *  Function that empties the shared memory contents and prints them to stdout
 *  if the events have been decoded or to the file if the events have not been
 *  decoded.
 *
 *  Params
 *  ------
 *  physBaseAddr:  Base physical address of the shared memory. This value can be
 *                 obtained by looking in the mapfile of the target and looking
 *                 for the location of ti_uia_runtime_LoggerSM_sharedBuffer__A.
 *
 *  size:          Size of the total shared memory used all partitions. This value must
 *                 be the same as ti.uia.runtime.LoggerSM.sharedMemorySize's
 *                 setting.
 *
 *  numPartitions: Number of partitions using the shared memory. This value must
 *                 be the same as ti.uia.runtime.LoggerSM.numPartitions's
 *                 setting.
 *                 It must also be less or equal to LoggerSM_MAXPARTITIONS.
 *
 *  partitionMask: Specifies which partitions to process
 *
 *  filename:      Filename to be used if any of the partitions are in binary
 *                 format (LoggerSM.decode = false in target's .cfg file).
 *                 If NULL, a default filename is used: "loggerSM.bin"
 */
int LoggerSM_run(unsigned int physBaseAddr, size_t size,
                 unsigned int numPartitions, unsigned int partitionMask,
                 char *filename);

/*
 *  ======== LoggerSM_setName =========
 *  Optional function to specify the names of the partitions that own the partitions.
 *
 *  If a name is specified for a partition, when the strings from that partition
 *  are written to stdout, the name is printed first.
 *
 *  Params
 *  ------
 *  partitionId:  Partition id
 *
 *  name:         Name of the partition that owns the partition. It must be shorter
 *                than LoggerSM_MAXPARTITIONNAMELEN characters. From a readability
 *                standpoint, shorter is better.
 */
int LoggerSM_setName(unsigned int partitionId, char *name);

/*
 *  ======== LoggerSM_dumpSharedMem =========
 *  Debug API used to dump the partition information.
 *
 *  Params
 *  ------
 *  physBaseAddr: Base physical address of the shared memory. This value can be
 *                obtained by looking in the mapfile of the target and looking
 *                for the location of ti_uia_runtime_LoggerSM_sharedBuffer__A.
 *
 *  size:         Size of the total shared memory used all partitions. This value must
 *                be the same as ti.uia.runtime.LoggerSM.sharedMemorySize's
 *                setting.
 *
 *  numPartitions: Number of partitions using the shared memory. This value must
 *                be the same as ti.uia.runtime.LoggerSM.numPartitions's
 *                setting.
 *                It must also be less or equal to LoggerSM_MAXPARTITIONS.
 */
void LoggerSM_dumpSharedMem(unsigned int physBaseAddr, size_t size,
                            unsigned int numPartitions);

#endif /* _LOGGERSM_ */
