/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
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
 *  ======== mailbox.c ========
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <ti/drivers/Board.h>

#define NUMMSGS         5
#define TASKSTACKSIZE   512

/*
 * This type is accessed by the application. When changing the data members of
 * this structure, considerations should be made for padding and data alignment.
 */
typedef struct MsgObj {
    Int     id;
    Char    val;
} MsgObj;

/*
 * Mailbox messages are stored in a queue that requires a header in front of
 * each message. Mailbox_MbxElem is defined such that the header and its size
 * are factored into the total data size requirement for a mailbox instance.
 * Because Mailbox_MbxElem contains Int data types, padding may be added to
 * this struct depending on the data members defined in MsgObj.
 */
typedef struct MailboxMsgObj {
    Mailbox_MbxElem  elem;      /* Mailbox header        */
    MsgObj           obj;       /* Application's mailbox */
} MailboxMsgObj;

/* This buffer is not directly accessed by the application */
MailboxMsgObj mailboxBuffer[NUMMSGS];

Mailbox_Struct mbxStruct;
Mailbox_Handle mbxHandle;

Task_Struct task0Struct, task1Struct;
Char task0Stack[TASKSTACKSIZE], task1Stack[TASKSTACKSIZE];

Void readerTask(UArg arg0, UArg arg1);
Void writerTask(UArg arg0, UArg arg1);

/*
 *  ======== main ========
 */
int main()
{
    Task_Params taskParams;
    Mailbox_Params mbxParams;

    /* Call Driver init functions */
    Board_init();

    /* Construct read and writer tasks */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)writerTask, &taskParams, NULL);
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)readerTask, &taskParams, NULL);

    /* Construct a Mailbox instance */
    Mailbox_Params_init(&mbxParams);
    mbxParams.buf = (Ptr)mailboxBuffer;
    mbxParams.bufSize = sizeof(mailboxBuffer);
    Mailbox_construct(&mbxStruct, sizeof(MsgObj), NUMMSGS, &mbxParams, NULL);
    mbxHandle = Mailbox_handle(&mbxStruct);

    BIOS_start();

    return(0);
}


/*
 *  ======== readerTask ========
 */
Void readerTask(UArg arg0, UArg arg1)
{
    MsgObj msg;
    Int i;

    for (i = 0; i < NUMMSGS; i++) {

        Mailbox_pend(mbxHandle, &msg, BIOS_WAIT_FOREVER);

        System_printf("Mailbox Read: ID = %d and Value = '%c'.\n",
            msg.id, msg.val);
    }

    System_printf("All messages received. Exiting...\n");

    BIOS_exit(0);
}

/*
 *  ======== writerTask ========
 */
Void writerTask(UArg arg0, UArg arg1)
{
    MsgObj msg;
    Int i;

    for (i = 0; i < NUMMSGS; i++) {

        msg.id = i;
        msg.val = i + 'a';

        if (Mailbox_post(mbxHandle, &msg, BIOS_NO_WAIT)) {

            System_printf("Mailbox Write: ID = %d and Value = '%c'.\n",
                msg.id, msg.val);
        }
        else {

            System_printf("Mailbox Write Failed: ID = %d and Value = '%c'.\n",
                            msg.id, msg.val);
        }

        Task_sleep(500);
    }
}
