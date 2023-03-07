/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
char *event_dump_string[] = {
    "error :eventgorup should input EventGroup_t handle pointer\r\n",
    "start bugkiller_eventgroup_dump\r\n",
    "pxList is null\r\n",
    "pxListItem is null\r\n",
    "EventGroup TCB pointer value is %p\r\n",
    "EventGroup:uxEventBits = 0x%lx\r\n",
    "the value of pxListItem->xItemValue:%p is 0x%lx\r\n",
    "EventGroup set eventWAIT_FOR_ALL_BITS **FALSE**\r\n",
    "EventGroup set eventWAIT_FOR_ALL_BITS TURE\r\n",
    "EventGroup set eventCLEAR_EVENTS_ON_EXIT_BIT **FALSE**\r\n",
    "EventGroup set eventCLEAR_EVENTS_ON_EXIT_BIT TRUE\r\n",
    "eventUNBLOCKED_DUE_TO_BIT_SET bit is **FALSE**\r\n",
    "eventUNBLOCKED_DUE_TO_BIT_SET is TRUE\r\n",
    "%lu Tasks waiting this event bits\r\n",
    "task name :%s\r\n",
};

char *task_dump_string[] = {
    "Task         State   Priority  Stack    #          Base            TCB              sp              pc\r\n**********************************************************************************************************\r\n",
    "%s",
    "%s",
    "\t%c\t%u\t%u\t%u\t%p\t%p\t%p\t%p\r\n",
};

char *heap_dump_string[] = {
    "All Block HeapStruct Address\r\n*******************************************************\r\n",
    "Head Address     Point To Next Address     BlockSize\r\n*******************************************************\r\n",
    "%p           %p             0x%x\r\n",
    "%p           %p             0x%x\r\n",
    "\r\n",
    "Free Block HeapStruct Address\r\n*******************************************************\r\n",
    "Head Address     Point To Next Address     BlockSize\r\n*******************************************************\r\n",
    "%p           %p             0x%x\r\n",
    "%p           %p             0x%x\r\n",
    "Current left size is %d bytes, mytest left bytes is %ld\r\n",
};

char *sem_dump_string[] = {
    "pcHead: %p, pcWriteTo: %p, uxMessageswaiting: %lu, uxLength: %lu, uxItemSize: %lu, cRxLock: %d, cTxLock: %d ",
    "type: SET\r\n",
    "type: MUTEX\r\n",
    "type: COUNTING_SEMAPHORE\r\n",
    "type: BINARY_SEMAPHORE\r\n",
    "type: RECURSIVE_MUTEX\r\n",
    "%lu Tasks waiting to receive this semaphore\r\n",
    "task name :%s\r\n",
    "%lu Tasks waiting to send this semaphore\r\n",
    "task name :%s\r\n",
};

char *softtimer_dump_string[] = {
    " Name       PeriodInTicks   ID           Callback    Reload  Static  IsActive\r\n",
    " %s\t%lu\t%p\t%p\t%c\t%c\t%c\r\n",
    " Name       PeriodInTicks   ID           Callback    Reload  Static  IsActive\r\n",
    " %s\t%lu\t%p\t%p\t%c\t%c\t%c\r\n",
};

char *streambuffer_dump_string[] = {
    "address:%p, total_length:%u, used:%u, left:%u, xTail:%u, xHead:%u, triggerlevelbytes:%u, ismessagebuffer:%c, isstatic:%c, isfull:%c, isempty:%c\r\n",
    "messagebuffer next message length is %u\r\n",
    "no task waiting for data\r\n",
    "task << %s >> is waiting for data\r\n",
    "no task waiting to send data\r\n",
    "task << %s >> is waiting to send data\r\n",
};
