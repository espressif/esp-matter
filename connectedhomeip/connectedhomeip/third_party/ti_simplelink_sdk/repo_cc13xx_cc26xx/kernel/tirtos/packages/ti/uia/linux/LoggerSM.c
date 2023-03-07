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
 *  ======== LoggerSM.c ========
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <LoggerSM.h>
#include <UIAPacket.h>

#define LoggerSM_HEADERTAG       0x14011938
#define LoggerSM_VERSION       1

/* Structure in shared memory. Must match ti.uia.runtime.LoggerSM */
typedef struct {
    volatile unsigned int headerTag;
    volatile unsigned int version;
    volatile unsigned int numPartitions;
    volatile char *endPtr;
    volatile char *readPtr;
    volatile char *writePtr;
    volatile char *buffer;
    volatile unsigned int bufferSize;
    volatile unsigned int droppedEvents;
    volatile unsigned short int moduleId;
    volatile unsigned short int instanceId;
    volatile unsigned short int decode;
    volatile unsigned short int overwrite;
} LoggerSM_SharedObj;

/*
 *  ======== LoggerSM_Obj =========
 *  Internal module client object structure
 */
typedef struct LoggerSM_Obj {
    unsigned int physBaseAddr;
    unsigned char *virtBaseAddr;
    size_t size;
    unsigned int numPartitions;
    unsigned int memDevFd;
    LoggerSM_SharedObj *sharedObj[LoggerSM_MAXPARTITIONS];
    char outputStr[1024];
    char tempStr[1024];
    struct timespec delayTime;
    FILE *outFile;
    UIAPacket_Hdr uiaHdr;
} LoggerSM_Obj;

static char *LoggerSM_names[LoggerSM_MAXPARTITIONS] = {
    NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL
};

static unsigned short int LoggerSM_seq[LoggerSM_MAXPARTITIONS];



#define TRUE 1
#define FALSE 0

/*
 *  ======== LoggerSM_obj =========
 *  Internal module client object.
 */
static LoggerSM_Obj LoggerSM_obj;

/*
 *  ======== LoggerSM_mapMem =========
 */
static int LoggerSM_mapMem(size_t size)
{
    LoggerSM_obj.memDevFd = open("/dev/mem",O_RDWR|O_SYNC);
    if (LoggerSM_obj.memDevFd < 0) {
        printf(" ERROR: /dev/mem open failed !!!\n");
        return (-1);
    }

    LoggerSM_obj.virtBaseAddr = mmap(
                (void   *)LoggerSM_obj.physBaseAddr,
                size,
                PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
                LoggerSM_obj.memDevFd,
                LoggerSM_obj.physBaseAddr);

    if (LoggerSM_obj.virtBaseAddr == MAP_FAILED) {
        printf(" ERROR: mmap() failed!!! errno = 0x%x\n", errno);
        close(LoggerSM_obj.memDevFd);
        return (-1);
    }
    return (0);
}

/*
 *  ======== LoggerSM_unmapMem =========
 */
static void LoggerSM_unmapMem()
{
    size_t size = LoggerSM_obj.size;

    /* If set, unmap it */
    if (LoggerSM_obj.virtBaseAddr) {
        munmap((void*)LoggerSM_obj.virtBaseAddr, size);
    }

    /* If opened, close it */
    if (LoggerSM_obj.memDevFd >= 0) {
        close(LoggerSM_obj.memDevFd);
    }
}

/*
 *  ======== LoggerSM_clientInit =========
 */
static int LoggerSM_clientInit(unsigned int physBaseAddr, size_t size,
                               unsigned int numPartitions)
{
    size_t partitionSize = size/numPartitions & (~127);
    int partitionId;
    int status;

    /* Assign the physical address and size */
    LoggerSM_obj.physBaseAddr = physBaseAddr;

    /* Map the memory to make it usable */
    status = LoggerSM_mapMem(size);
    if (status != LoggerSM_SUCCESS) {
      return (LoggerSM_ERROR);
    }

    /* Loop through each partitions and setup the object */
    for (partitionId = 0; partitionId < numPartitions; partitionId++) {

        /* address of object */
        LoggerSM_obj.sharedObj[partitionId] = (LoggerSM_SharedObj *)
            ((unsigned int)LoggerSM_obj.virtBaseAddr + partitionSize * partitionId);

        /* Initial UIA packet sequence */
        LoggerSM_seq[partitionId] = 0;
    }

    LoggerSM_obj.numPartitions  = numPartitions;
    LoggerSM_obj.size = partitionSize - 256; /* pad before and after */

    /* Set the delay used when no strings are found */
    LoggerSM_obj.delayTime.tv_sec  = LoggerSM_MSECS / 1000;
    LoggerSM_obj.delayTime.tv_nsec = (LoggerSM_MSECS % 1000) * 1000000;

    /*
     *  Initialize some of the fields in the UIAPacket that will be
     *  written into the file .
     */
    UIAPacket_initEventRecHdr(&(LoggerSM_obj.uiaHdr),
        UIAPacket_PayloadEndian_LITTLE, 0, 0, 0, 0, 0, UIAPacket_HOST, 0);

    return (0);
}

/*
 *  ======== LoggerSM_clientDeInit =========
 */
static int LoggerSM_clientDeInit()
{
    LoggerSM_unmapMem();

    return (0);
}

/*
 *  ======== LoggerSM_convertLoadEvents =========
 */
void LoggerSM_convertLoadEvents(char *pString)
{
    char *pchHeader;
    char *pchEvent;
    char *pchTask;
    char *pch;
    unsigned int count;
    unsigned int total;
    float percent;

    strcpy(LoggerSM_obj.tempStr, pString);
    pchHeader = strtok(LoggerSM_obj.tempStr, "S");
    pchEvent = strtok(NULL, ":");

    if (pchEvent == NULL) {
        /* Strings may have "\n" embedded within */
        return;
    }

    if ((strcmp(pchEvent, "LS_hwiLoad") == 0) ||
        (strcmp(pchEvent, "LS_swiLoad") == 0)) {

        pch = strtok(NULL, " ,:");
        count = atoi(pch);

        pch = strtok(NULL, " ,:");
        total = atoi(pch);

        percent = (float)count/(float)total * 100;

        sprintf(pString, "%sS:%s: %3.2f%%", pchHeader, pchEvent, percent);
    }
    else if (strcmp(pchEvent, "LS_taskLoad") == 0) {
        pchTask = strtok(NULL, " ,:");
        pch = strtok(NULL, " ,:");
        count = atoi(pch);

        pch = strtok(NULL, " ,:");
        total = atoi(pch);

        percent = (float)count / (float)total  * 100;

        sprintf(pString, "%sS:%s: handle=%s %3.2f%%", pchHeader, pchEvent, pchTask, percent);
    }
    else if (strcmp(pchEvent, "LS_cpuLoad") == 0) {
        pch = strtok(NULL, " ,:");

        sprintf(pString, "%sS:%s: %s", pchHeader, pchEvent, pch);
    }
}

/*
 *  ======== LoggerSM_sanity =========
 */
void LoggerSM_sanity(LoggerSM_SharedObj *sharedObj)
{
   if (sharedObj->version != LoggerSM_VERSION) {
      printf("ERROR: There is a mismatch on the version numbers!\n");
      printf(" Target version = %d, Linux version = %d\n",
               sharedObj->version, LoggerSM_VERSION);
   }

   if (sharedObj->numPartitions != LoggerSM_obj.numPartitions) {
        printf("ERROR: There is a mismatch on the number of partitions!\n");
        printf(" Target numPartition = %d, Linux numPartitions = %d\n",
               sharedObj->numPartitions, LoggerSM_obj.numPartitions);
        exit(-1);
   }

   if (sharedObj->bufferSize != LoggerSM_obj.size) {
        printf("ERROR: There is a mismatch on the bufferSize!\n");
        printf(" Target bufferSize = %d, Linux size = %d\n",
               sharedObj->bufferSize, LoggerSM_obj.size);
        exit(-1);
   }
}

/*
 *  ======== LoggerSM_writeUIAHeader =========
 */
static void LoggerSM_writeUIAHeader(LoggerSM_SharedObj *sharedObj,
    size_t numBytes, unsigned int partitionId)
{
    /* Set length of packet */
    UIAPacket_setEventLength(&(LoggerSM_obj.uiaHdr),
        numBytes + sizeof(UIAPacket_Hdr));

    /* Set sequence number */
    UIAPacket_setSequenceCount(&(LoggerSM_obj.uiaHdr),
        LoggerSM_seq[partitionId]++);

    /* Set moduleId */
    UIAPacket_setLoggerModuleId(&(LoggerSM_obj.uiaHdr), sharedObj->moduleId);

    /* Set priority */
    UIAPacket_setLoggerPriority(&(LoggerSM_obj.uiaHdr),
        UIAPacket_Priority_STANDARD);

    /* Set instanceId */
    UIAPacket_setLoggerInstanceId(&(LoggerSM_obj.uiaHdr),
        sharedObj->instanceId);

    /* Set moduleId */
    UIAPacket_setLoggerModuleId(&(LoggerSM_obj.uiaHdr),
        sharedObj->moduleId);

    /* Set senderAdrs */
    UIAPacket_setSenderAdrs(&(LoggerSM_obj.uiaHdr), partitionId);

    fwrite((char *)&(LoggerSM_obj.uiaHdr), sizeof(UIAPacket_Hdr), 1,
           LoggerSM_obj.outFile);
}

/*
 *  ======== LoggerSM_getBinary =========
 */
static void LoggerSM_getBinary(LoggerSM_SharedObj *sharedObj,
                               unsigned int partitionId)
{
    size_t numBytes;
    volatile char *writePtr;
    char *readPtr;

    writePtr = (char *)sharedObj->writePtr;
    readPtr = (char *)sharedObj->readPtr;

    if (writePtr != readPtr) {

        if (writePtr >= readPtr) {
            numBytes = writePtr - readPtr;

            LoggerSM_writeUIAHeader(sharedObj, numBytes, partitionId);

            fwrite(readPtr, numBytes, 1, LoggerSM_obj.outFile);
        }
        else {

            /* end of buffer */
            numBytes = sharedObj->bufferSize - (unsigned int)readPtr + (unsigned int)writePtr;
            LoggerSM_writeUIAHeader(sharedObj, numBytes, partitionId);

            numBytes = (unsigned int)sharedObj->endPtr - (unsigned int)readPtr;
            fwrite(readPtr, numBytes, 1, LoggerSM_obj.outFile);

            /* start of buffer */
            numBytes = writePtr - sharedObj->buffer;
            fwrite((char *)sharedObj->buffer, numBytes, 1, LoggerSM_obj.outFile);
        }

        sharedObj->readPtr = writePtr;
        fflush(LoggerSM_obj.outFile);
    }
}

/*
 *  ======== LoggerSM_clientGetLine =========
 */
static int LoggerSM_clientGetLine(LoggerSM_SharedObj *sharedObj, char *pString)
{
    unsigned int numBytes, copyBytes=0;
    char *writePtr;
    char *buffer;
    volatile unsigned char curChar;

    writePtr = (char *)sharedObj->writePtr;

    if (writePtr >= sharedObj->readPtr) {
        numBytes = writePtr - sharedObj->readPtr;
    }
    else {
        numBytes = sharedObj->bufferSize - (unsigned int)sharedObj->readPtr +
                   (unsigned int)writePtr;
    }

    if (numBytes > 0) {
        buffer = (char *)((unsigned int)(sharedObj->buffer) + writePtr);

        for(copyBytes = 0; copyBytes < numBytes; copyBytes++)
        {
            if (sharedObj->readPtr >= sharedObj->endPtr) {
                sharedObj->readPtr = (char *)sharedObj->buffer;
            }

            curChar = *(sharedObj->readPtr);

            (sharedObj->readPtr)++;

            if (curChar==0xA0 || curChar=='\r' || curChar=='\n' ||
               curChar==0    || copyBytes >= sharedObj->bufferSize) {
                break;
            }
            else {
                *pString++ = curChar;
            }
        }
    }

    /* Null terminate the string */
    *pString = 0;

    return (copyBytes);
}

/*
 *  ======== LoggerSM_setName =========
 */
int LoggerSM_setName(unsigned int partitionId, char *name)
{
    if (partitionId >= LoggerSM_MAXPARTITIONS) {
        printf("ERROR: partitionId [%d] exceeds max [%d]\n", partitionId,
               LoggerSM_MAXPARTITIONS);
        return (LoggerSM_ERROR);
    }

    if (name == NULL) {
        printf("ERROR: name cannot be null\n");
        return (LoggerSM_ERROR);
    }

    /* Copy name into internal array */
    LoggerSM_names[partitionId] = malloc(LoggerSM_MAXPARTITIONNAMELEN);
    strncpy(LoggerSM_names[partitionId], name, LoggerSM_MAXPARTITIONNAMELEN);

    return (LoggerSM_SUCCESS);
}

/*
 *  ======== LoggerSM_run =========
 */
int LoggerSM_run(unsigned int physBaseAddr, size_t sharedMemorySize,
                 unsigned int numPartitions, unsigned int partitionMask,
                 char *filename)
{
    unsigned int partitionId;
    int numBytes, status;
    unsigned int doWait;
    LoggerSM_SharedObj *sharedObj;
    struct timespec elaspedTime;

    if (numPartitions > LoggerSM_MAXPARTITIONS) {
        printf("ERROR: numPartitions [%d] exceeds max [%d]\n", numPartitions,
                LoggerSM_MAXPARTITIONS);
        return (LoggerSM_ERROR);
    }

    /* Initialize the global variable */
    memset(&LoggerSM_obj, 0, sizeof(LoggerSM_obj));

    /* Setup the default filename */
    if ((filename == NULL) || (strlen(filename)) == 0) {
        filename = "loggerSM.bin";
    }

    /* Open file to store encoded records */
    LoggerSM_obj.outFile = fopen(filename, "w");
    if (LoggerSM_obj.outFile == NULL) {
        printf("ERROR: unable to open file %s\n", filename);
    }
    else {
        printf("Opened %s to store encoded records\n", filename);
    }

    /* Initialize the global structure, passing in the address */
    status = LoggerSM_clientInit(physBaseAddr, sharedMemorySize, numPartitions);
    if (status != LoggerSM_SUCCESS) {
        return (status);
    }

    while(1)
    {
        doWait = 1;

        /* Process each partition */
        for (partitionId = 0; partitionId < numPartitions; partitionId++) {
            /* Only process the partitions the caller requested */
            if ((partitionMask & (1 << partitionId)) == 0) {
                continue;
            }

            /* Make sure the headerTag is initialized */
            sharedObj = LoggerSM_obj.sharedObj[partitionId];
            if (sharedObj->headerTag == LoggerSM_HEADERTAG) {

                LoggerSM_sanity(sharedObj);

                if (sharedObj->decode == TRUE) {
                   /*
                    *  Check partition for strings. If found, print it and move
                    *  to the next partition. Note: there might be more strings
                    *  on a partition, so set doWait to zero, so we loop back.
                    */
                    numBytes = LoggerSM_clientGetLine(sharedObj,
                                                      LoggerSM_obj.outputStr);
                    if (numBytes > 0) {

                        /* convert the load events */
                        LoggerSM_convertLoadEvents(LoggerSM_obj.outputStr);

                        if (LoggerSM_names[partitionId] != NULL) {
                            printf("N:%s P:%d", LoggerSM_names[partitionId],
                                   partitionId);
                        }
                        else {
                            printf("P:%d", partitionId);
                        }
                        printf(" %s\n", LoggerSM_obj.outputStr);
                        doWait = 0;
                    }
                    fflush(stdout);
                }
                else {

                    /*
                     *  Check partition for binary. Do not modify doWait since all
                     *  the data is moved from the partition in this call.
                     */
                    LoggerSM_getBinary(sharedObj, partitionId);
                }
            }
        }

        /* If no strings were printed, wait for a bit */
        if (doWait) {
            nanosleep(&(LoggerSM_obj.delayTime), &elaspedTime);
        }
   }

   /* Reverse the clientInit */
   LoggerSM_clientDeInit();

   return (LoggerSM_SUCCESS);
}

/*
 *  ======== LoggerSM_dumpSharedMem =========
 *  Used for debugging this module...
 */
void LoggerSM_dumpSharedMem(unsigned int physBaseAddr, size_t size,
                            unsigned int numPartitions)
{
    unsigned int partitionId;
    int status;
    unsigned int *tempPtr;

    /* Initialize the global variable */
    memset(&LoggerSM_obj, 0, sizeof(LoggerSM_obj));

    status = LoggerSM_clientInit(physBaseAddr, size, numPartitions);
    if (status != LoggerSM_SUCCESS) {
        printf("LoggerSM_dumpSharedMem: failed\n");
        return;
    }

    printf("LoggerSM_dumpSharedMem: Show status of all %d partitions\n",
           numPartitions);
    for (partitionId = 0; partitionId < numPartitions; partitionId++) {

        printf("partitionId = %d, addr = 0x%08x, headerTag = 0x%08x", partitionId,
               (unsigned int)&(LoggerSM_obj.sharedObj[partitionId]->headerTag),
               LoggerSM_obj.sharedObj[partitionId]->headerTag);

        if (LoggerSM_obj.sharedObj[partitionId]->headerTag ==
            LoggerSM_HEADERTAG) {
            printf(" [initialized]\n");
            tempPtr = (unsigned int *)(LoggerSM_obj.sharedObj[partitionId]);

            printf("  version = %d, decode = %d, overwrite = 0x%x\n",
                   LoggerSM_obj.sharedObj[partitionId]->version,
                   LoggerSM_obj.sharedObj[partitionId]->decode,
                   LoggerSM_obj.sharedObj[partitionId]->overwrite);
            printf("  moduleId = 0x%x, instanceId = 0x%x\n",
                   LoggerSM_obj.sharedObj[partitionId]->moduleId,
                   LoggerSM_obj.sharedObj[partitionId]->instanceId);
            printf("  buffer = 0x%x, endPtr = 0x%x\n",
                   (unsigned int)LoggerSM_obj.sharedObj[partitionId]->buffer,
                   (unsigned int)LoggerSM_obj.sharedObj[partitionId]->endPtr);
            printf("  writePtr = 0x%x, readPtr = 0x%x\n",
                   (unsigned int)LoggerSM_obj.sharedObj[partitionId]->writePtr,
                   (unsigned int)LoggerSM_obj.sharedObj[partitionId]->readPtr);
            printf("  bufferSize = 0x%x, droppedEvents = 0x%x\n",
                   LoggerSM_obj.sharedObj[partitionId]->bufferSize,
                   LoggerSM_obj.sharedObj[partitionId]->droppedEvents);
        }
        else {
            printf(" [not initialized]\n");
        }
    }
}
