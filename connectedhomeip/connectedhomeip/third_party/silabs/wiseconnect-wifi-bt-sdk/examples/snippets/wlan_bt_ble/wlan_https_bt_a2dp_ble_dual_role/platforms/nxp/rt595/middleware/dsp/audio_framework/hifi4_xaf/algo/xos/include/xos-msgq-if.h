/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
#ifdef __TOOLS_RF2__
#include <xtensa/xos/xos.h>
#else   // #ifdef __TOOLS_RF2__
#include <xtensa/xos.h>
#endif  // #ifdef __TOOLS_RF2__

/*******************************************************************************
 * Global Definitions
 ******************************************************************************/

#define SEND_MSGQ_ENTRIES              16
#define RECV_MSGQ_ENTRIES              16

#define CMD_MSGQ_READY                 0x1
#define RESP_MSGQ_READY                0x2
#define DIE_MSGQ_ENTRY                 0x4

typedef struct IPC_MSGQ {
    
    XosMsgQueue   *cmd_msgq;
    XosMsgQueue   *resp_msgq;
    XosEvent      *msgq_event;
    int           init_done;

} ipc_msgq_t;

int ipc_msgq_init(void **cmdq, void **respq, void **msgq_event);
int ipc_msgq_delete(void **cmdq, void **respq, void **msgq_event);

