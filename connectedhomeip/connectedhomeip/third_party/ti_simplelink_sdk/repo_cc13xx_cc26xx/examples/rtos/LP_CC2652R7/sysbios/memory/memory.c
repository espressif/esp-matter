/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 */

/*
 *  ======== memory.c ========
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include <ti/drivers/Board.h>

#define TASK0BUFSIZE    32       /* size of allocations */
#define TASK0NUMBUFS    2        /* number of buffers */

#define TASK1BUFSIZE0   128      /* size of allocation */
#define TASK1BUFSIZE1   64       /* size of allocation */
#define TASK1BUFSIZE2   32       /* size of allocation */
#define TASK1BUFSIZE3   16       /* size of allocation */
#define TASK1NUMBUFS    4        /* number of buffers */
#define TASKSTACKSIZE   768      /* stack size for constructed tasks */
#define HEAPBUFSIZE     64
#define HEAPMEMSIZE     512

/* used to align heapmem/heapbuf buffers */
typedef union {
    double d;
    long l;
    void *p;
} AlignData;

Void task0Fxn(UArg arg0, UArg arg1);
Void task1Fxn(UArg arg0, UArg arg1);
Void idl0Fxn(Void);

/* Function to print heap statistics */
static Void printHeapStats(IHeap_Handle heap);

Task_Struct task0Struct, task1Struct;
AlignData heapMemBuffer[HEAPMEMSIZE / sizeof(AlignData)],
          heapBufBuffer[HEAPBUFSIZE / sizeof(AlignData)];
Char task0Stack[TASKSTACKSIZE], task1Stack[TASKSTACKSIZE];
HeapBuf_Struct heapBufStruct;
HeapBuf_Handle task0Heap;
HeapMem_Struct heapMemStruct;
HeapMem_Handle task1Heap;

/*
 *  ======== main ========
 */
int main()
{
    /* Call driver init functions */
    Board_init();

    /* Construct BIOS objects */
    Task_Params taskParams;
    HeapBuf_Params heapBufParams;
    HeapMem_Params heapMemParams;

    /* Construct writer/reader Task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)task0Fxn, &taskParams, NULL);

    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1Fxn, &taskParams, NULL);

    /* Construct two heaps to be used by two separate tasks for alloc and free. */
    HeapBuf_Params_init(&heapBufParams);
    heapBufParams.blockSize = HEAPBUFSIZE / 2;
    heapBufParams.numBlocks = 2;
    heapBufParams.align = 8;
    heapBufParams.buf = heapBufBuffer;
    heapBufParams.bufSize = HEAPBUFSIZE;
    HeapBuf_construct(&heapBufStruct, &heapBufParams, NULL);
    task0Heap = HeapBuf_handle(&heapBufStruct);

    HeapMem_Params_init(&heapMemParams);

    heapMemParams.size = HEAPMEMSIZE;
    heapMemParams.minBlockAlign = 8;
    heapMemParams.buf = heapMemBuffer;
    HeapMem_construct(&heapMemStruct, &heapMemParams);
    task1Heap = HeapMem_handle(&heapMemStruct);

    System_printf("Memory example started.\n");

    BIOS_start();    /* Does not return */
    return(0);
}

/*
 *  ======== task0Fxn ========
 */
Void task0Fxn(UArg arg0, UArg arg1)
{
    Int i;
    Ptr bufs[TASK0NUMBUFS];

    IHeap_Handle heap = HeapBuf_Handle_upCast(task0Heap);

    System_printf("Initial task0 heap status\n");

    /* Print initial task0heap status */
    printHeapStats(heap);

    /* Allocate blocks from task0Heap */
    for (i = 0; i < TASK0NUMBUFS; i++) {
        bufs[i] = Memory_alloc(heap, TASK0BUFSIZE, 0, NULL);
    }

    /* Free memory blocks */
    for (i = 0; i < TASK0NUMBUFS; i++) {
        Memory_free(heap, bufs[i], TASK0BUFSIZE);
    }

    System_printf("Final task0 heap status\n");

    /* Print task0Heap status */
    printHeapStats(heap);

    System_printf("Task0 Complete\n");
}

/*
 *  ======== task1Fxn ========
 */
Void task1Fxn(UArg arg0, UArg arg1)
{
    Ptr bufs[TASK1NUMBUFS];
    IHeap_Handle heap = HeapMem_Handle_upCast(task1Heap);

    System_printf("Initial task1 heap status\n");

    /* Print initial task1Heap status */
    printHeapStats(heap);

    bufs[0] = Memory_alloc(heap, TASK1BUFSIZE0, 0, NULL);

    bufs[1] = Memory_alloc(heap, TASK1BUFSIZE1, 0, NULL);

    bufs[2] = Memory_alloc(heap, TASK1BUFSIZE2, 0, NULL);

    Memory_free(heap, bufs[1], TASK1BUFSIZE1);
    Memory_free(heap, bufs[2], TASK1BUFSIZE2);

    bufs[3] = Memory_alloc(heap, TASK1BUFSIZE3, 0, NULL);

    Memory_free(heap, bufs[0], TASK1BUFSIZE0);
    Memory_free(heap, bufs[3], TASK1BUFSIZE3);

    System_printf("Final task1 heap status\n");

    /* Print task1Heap status */
    printHeapStats(heap);

    System_printf("Task1 Complete\n");
}

static Void printHeapStats(IHeap_Handle heap)
{
    Memory_Stats stats;

    Memory_getStats(heap, &stats);
#ifdef xdc_target__isaCompatible_28
    System_printf("largestFreeSize = %ld\n", (ULong)stats.largestFreeSize);
    System_printf("totalFreeSize = %ld\n", (ULong)stats.totalFreeSize);
    System_printf("totalSize = %ld\n", (ULong)stats.totalSize);
#else
    System_printf("largestFreeSize = %d\n", stats.largestFreeSize);
    System_printf("totalFreeSize = %d\n", stats.totalFreeSize);
    System_printf("totalSize = %d\n", stats.totalSize);
#endif
}

/*
 *  ======== idl0Fxn ========
 */
Void idl0Fxn()
{
    BIOS_exit(0);
}
