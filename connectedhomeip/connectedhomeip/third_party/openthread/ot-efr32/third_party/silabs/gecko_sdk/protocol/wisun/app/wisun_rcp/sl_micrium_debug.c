/***************************************************************************//**
 * Copyright 2021 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available here[1]. This software is distributed to you in
 * Source Code format and is governed by the sections of the MSLA applicable to
 * Source Code.
 *
 * [1] www.silabs.com/about-us/legal/master-software-license-agreement
 *
 ******************************************************************************/

/*
 * uC/OS-III does not provide a fixed layout for OS_TCB, which makes it
 * impossible to determine the appropriate offsets within the structure
 * unaided. A priori knowledge of offsets based on os_dbg.c is tied to a
 * specific release and thusly, brittle. The constants defined below
 * provide the necessary information OpenOCD needs to provide support in
 * the most robust manner possible.
 *
 * This file should be linked along with the project to enable RTOS
 * support for uC/OS-III.
 */

#include <stdio.h>
#include <os.h>

#if OS_CFG_DBG_EN == 0
#error "OS_CFG_DBG_EN is required to enable RTOS support for OpenOCD"
#endif

#define OFFSET_OF(type, member) ((CPU_SIZE_T)&(((type *)0)->member))

#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif

// These symbols are used by OpenOCD debugger to provide information about
// Micrium tasks states. Unfortunatly, they may be dropped by section garbage
// collector during the link. Calling export_debugger_data() allow to ensure it
// won't happen.
const CPU_SIZE_T USED openocd_OS_TCB_StkPtr_offset = OFFSET_OF(OS_TCB, StkPtr);
const CPU_SIZE_T USED openocd_OS_TCB_NamePtr_offset = OFFSET_OF(OS_TCB, NamePtr);
const CPU_SIZE_T USED openocd_OS_TCB_TaskState_offset = OFFSET_OF(OS_TCB, TaskState);
const CPU_SIZE_T USED openocd_OS_TCB_Prio_offset = OFFSET_OF(OS_TCB, Prio);
const CPU_SIZE_T USED openocd_OS_TCB_DbgPrevPtr_offset = OFFSET_OF(OS_TCB, DbgPrevPtr);
const CPU_SIZE_T USED openocd_OS_TCB_DbgNextPtr_offset = OFFSET_OF(OS_TCB, DbgNextPtr);

void export_debugger_data()
{
    printf("Test your debugger: %p %p %p %p %p %p\n",
           &openocd_OS_TCB_StkPtr_offset, &openocd_OS_TCB_NamePtr_offset,
           &openocd_OS_TCB_TaskState_offset, &openocd_OS_TCB_Prio_offset,
           &openocd_OS_TCB_DbgPrevPtr_offset, &openocd_OS_TCB_DbgNextPtr_offset);
}
