/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __SYS_RTXC_H__
#define __SYS_RTXC_H__

#include  <kernel/include/os.h>

/* Module configuration */

#define LWIP_MAX_TASKS  2          /* Number of LwIP tasks */
#define LWIP_STACK_SIZE 3000       /* Stack size for LwIP tasks */
/* Note:
   Task priorities, LWIP_START_PRIO through (LWIP_START_PRIO+LWIP_MAX_TASKS-1) must be reserved
   for LwIP and must not used by other applications outside. */

#define LWIP_Q_SIZE 20              /* LwIP queue size */

#define SYS_MBOX_NULL   NULL
#define SYS_SEM_NULL    NULL

typedef struct {
  /** The mail queue itself. */
  OS_Q        Q;
  /** The semaphore used to count the number of available slots. */
  OS_SEM      Q_full;
  /** The validity flag. */
  int         is_valid;
} sys_mbox_t;

typedef struct {
  OS_SEM    sem;
  int       is_valid;
} sys_sem_t;

typedef uint8_t     sys_thread_t;

typedef struct {
  OS_MUTEX mutex;
  int      is_valid;
} sys_mutex_t;

#endif /* __SYS_RTXC_H__ */
