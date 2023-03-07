/***************************************************************************//**
 * @file
 * @brief LwIP system specific functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include <stdlib.h>
/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"
#include  <kernel/include/os.h>
#if !NO_SYS

CPU_STK sys_stack[LWIP_MAX_TASKS][LWIP_STACK_SIZE];
OS_TCB  sys_TCB[LWIP_MAX_TASKS];
static uint8_t sys_thread_no;

#if defined(LWIP_SOCKET_SET_ERRNO) && defined(LWIP_PROVIDE_ERRNO)
int errno;
#endif

/**************************************************************************//**
 * Creates an empty mailbox
 *****************************************************************************/
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  RTOS_ERR err;
  mbox->is_valid = 0;
  OSQCreate(&(mbox->Q), "lwipQ", size, &err);
  LWIP_ASSERT("OSQCreate", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  OSSemCreate(&(mbox->Q_full), "lwipQsem", size, &err);
  LWIP_ASSERT("OSSemCreate", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  mbox->is_valid = 1;
#if SYS_STATS
  ++lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
  return ERR_OK;
}

/**************************************************************************//**
 * Deallocates a mailbox.
 *****************************************************************************/
void sys_mbox_free(sys_mbox_t *mbox)
{
  RTOS_ERR err;
  OSSemDel(&(mbox->Q_full), OS_OPT_DEL_ALWAYS, &err);
  LWIP_ASSERT("OSSemDel", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  OSQDel(&(mbox->Q), OS_OPT_DEL_ALWAYS, &err);
  LWIP_ASSERT("OSQDel", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  mbox->is_valid = 0;
#if SYS_STATS
  --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/**************************************************************************//**
 * Posts the "msg" to the mailbox.
 *****************************************************************************/
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
  RTOS_ERR err;
  /* Wait for an available slot in the queue. */
  OSSemPend(&(mbox->Q_full), 0, OS_OPT_PEND_BLOCKING, 0, &err);
  LWIP_ASSERT("OSSemPend", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  /* Posts the message to the queue. */
  OSQPost(&(mbox->Q), data, 0, OS_OPT_POST_FIFO, &err);
  LWIP_ASSERT("OSQPost", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
}

/**************************************************************************//**
 * Try to post the "msg" to the mailbox. Non-blocking.
 *****************************************************************************/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  err_t result = ERR_OK;
  RTOS_ERR err;

  OSSemPend(&(mbox->Q_full), 0, OS_OPT_PEND_NON_BLOCKING, 0, &err);
  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    /* Posts the message to the queue. */
    OSQPost(&(mbox->Q), msg, 0, OS_OPT_POST_FIFO, &err);
    LWIP_ASSERT("OSQPost", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  } else {
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    return ERR_MEM;
  }

  return result;
}

/**************************************************************************//**
 * Try to post the "msg" to the mailbox. Non-blocking.
 *****************************************************************************/
err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  err_t result = ERR_OK;
  RTOS_ERR err;

  OSSemPend(&(mbox->Q_full), 0, OS_OPT_PEND_NON_BLOCKING, 0, &err);
  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    // Posts the message to the queue.
    OSQPost(&(mbox->Q), msg, 0, OS_OPT_POST_FIFO, &err);
    LWIP_ASSERT("OSQPost", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
  } else {
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif // SYS_STATS
    return ERR_MEM;
  }

  return result;
}

/**************************************************************************//**
 * Fetch a msg" from the mailbox.
 *
 * @param mbox Mailbox to fetch from
 * @param msg Message result
 * @param timeout Timeout in milliseconds
 * @returns Time spent waiting or timeout
 *
 * Blocks the thread until a message arrives in the mailbox, but does
 * not block the thread longer than "timeout" milliseconds
 *****************************************************************************/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  int32_t os_timeout;
  u32_t starttime;
  RTOS_ERR err;
  OS_MSG_SIZE size;
  void *temp;
  starttime = sys_now();
  /* Convert lwIP timeout (in milliseconds) to uOS timeout (in OS_TICKS) */

  os_timeout = timeout;

  temp = OSQPend(&(mbox->Q), os_timeout, OS_OPT_PEND_BLOCKING, &size, 0, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return SYS_ARCH_TIMEOUT;
  }
  /* Tells tasks waiting because of a full buffer that the buffer is not full
   * anymore. */
  OSSemPost(&(mbox->Q_full), OS_OPT_POST_1, &err);

  if (msg) {
    *msg = temp;
  }

  return (sys_now() - starttime);
}

/**************************************************************************//**
 * Fetch a msg" from the mailbox. Non-blocking.
 *
 * @param mbox Mailbox to fetch from
 * @param msg Message result
 * @returns Success or SYS_MBOX_EMPTY
 *
 *****************************************************************************/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  RTOS_ERR err;
  OS_MSG_SIZE size;
  void *temp;
  temp = OSQPend(&(mbox->Q), 0, OS_OPT_PEND_NON_BLOCKING, &size, 0, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return SYS_MBOX_EMPTY;
  }
  /* Tells tasks waiting because of a full buffer that the buffer is not full
   * anymore. */
  OSSemPost(&(mbox->Q_full), OS_OPT_POST_1, &err);

  if (msg) {
    *msg = temp;
  }

  return 0;
}

/**************************************************************************//**
 * Check if mailbox is valid
 *****************************************************************************/
int sys_mbox_valid(sys_mbox_t *mbox)
{
  return mbox->is_valid;
}

/**************************************************************************//**
 * Set mailbox to be invalid
 *****************************************************************************/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  mbox->is_valid = 0;
}

/**************************************************************************//**
 * Create a semaphore with initial value of count
 *
 * @param sem Semaphore to create
 * @param count Initialization value
 *****************************************************************************/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  RTOS_ERR err;
  OSSemCreate(&(sem->sem), "lwip sem", count, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    sem->is_valid = 0;
#if SYS_STATS
    ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
    return ERR_MEM;
  }
  sem->is_valid = 1;
#if SYS_STATS
  ++lwip_stats.sys.sem.used;
  if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used) {
    lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
  }
#endif /* SYS_STATS */

  return ERR_OK;
}

/**************************************************************************//**
 * Wait for semaphore. Blocking.
 *
 * @param sem Semaphore to wait on
 * @param timeout Timeout to wait
 * @returns Time waited or SYS_ARCH_TIMEOUT
 *****************************************************************************/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  int32_t os_timeout;

  u32_t starttime;
  RTOS_ERR err;
  starttime = sys_now();

  os_timeout = timeout;

  OSSemPend(&(sem->sem), os_timeout, OS_OPT_PEND_BLOCKING, 0, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return SYS_ARCH_TIMEOUT;
  }

  return (sys_now() - starttime);
}

/**************************************************************************//**
 * Signals a semaphore
 *****************************************************************************/
void sys_sem_signal(sys_sem_t *sem)
{
  RTOS_ERR err;
  OSSemPost(&(sem->sem), OS_OPT_POST_1, &err);
}

/**************************************************************************//**
 * Deallocate a semaphore
 *****************************************************************************/
void sys_sem_free(sys_sem_t *sem)
{
  RTOS_ERR err;
#if SYS_STATS
  --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */

  OSSemDel(&(sem->sem), OS_OPT_DEL_ALWAYS, &err);
}

/**************************************************************************//**
 * Check if semaphore is valid
 *****************************************************************************/
int sys_sem_valid(sys_sem_t *sem)
{
  return sem->is_valid;
}

/**************************************************************************//**
 * Set semaphore to invalid
 *****************************************************************************/
void sys_sem_set_invalid(sys_sem_t *sem)
{
  sem->is_valid = 0;
}

OS_MUTEX   lwip_sys_mutex;

/**************************************************************************//**
 * Initialize LwIP OS interface
 *****************************************************************************/
void sys_init(void)
{
  RTOS_ERR err;
  sys_thread_no = 0;
  OSMutexCreate(&lwip_sys_mutex, "lwip_sys_mutex", &err);
  LWIP_ASSERT("OSMutexCreate", (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) );
}

#if LWIP_COMPAT_MUTEX == 0
/**************************************************************************//**
 * Create a mutex
 *****************************************************************************/
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  RTOS_ERR err;

  OSMutexCreate(&(mutex->mutex), "lwip mutex", &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    mutex->is_valid = 0;
#if SYS_STATS
    ++lwip_stats.sys.mutex.err;
#endif /* SYS_STATS */
    return ERR_MEM;
  }
  mutex->is_valid = 1;
#if SYS_STATS
  ++lwip_stats.sys.mutex.used;
  if (lwip_stats.sys.mutex.max < lwip_stats.sys.mutex.used) {
    lwip_stats.sys.mutex.max = lwip_stats.sys.mutex.used;
  }
#endif /* SYS_STATS */
  return ERR_OK;
}

/**************************************************************************//**
 * Deallocate a mutex
 *****************************************************************************/
void sys_mutex_free(sys_mutex_t *mutex)
{
  RTOS_ERR err;
#if SYS_STATS
  --lwip_stats.sys.mutex.used;
#endif /* SYS_STATS */

  OSMutexDel(&(mutex->mutex), OS_OPT_DEL_ALWAYS, &err);
  mutex->is_valid = 0;
}

/**************************************************************************//**
 * Check if mutex is valid
 *****************************************************************************/
int sys_mutex_valid(sys_mutex_t *mutex)
{
  return mutex->is_valid;
}

/**************************************************************************//**
 * Set mutex to invalid
 *****************************************************************************/
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  mutex->is_valid = 0;
}

/**************************************************************************//**
 * Lock a mutex
 *****************************************************************************/
void sys_mutex_lock(sys_mutex_t *mutex)
{
  RTOS_ERR err;
  OSMutexPend(&(mutex->mutex), 0, OS_OPT_PEND_BLOCKING, NULL, &err);
}

/**************************************************************************//**
 * Unlock a mutex
 *****************************************************************************/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  RTOS_ERR err;
  OSMutexPost(&(mutex->mutex),
              OS_OPT_POST_NONE,
              &err);
}
#endif /*LWIP_COMPAT_MUTEX*/

/**************************************************************************//**
 * Starts a new thread
 *
 * @param name Name of thread
 * @param thread Function to execute
 * @param arg Argument to pass to thread
 * @param stacksize Stack size to allocate for thread
 * @prio Priority of thread
 * @returns Thread ID
 *****************************************************************************/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  RTOS_ERR err;
  (void)stacksize;
  LWIP_ASSERT("sys_thread_new: Max Sys. Tasks reached.", sys_thread_no < LWIP_MAX_TASKS);
  ++sys_thread_no; /* next task created will be one lower to this one */

  OSTaskCreate(&(sys_TCB[sys_thread_no - 1]),
               (CPU_CHAR*)name,
               thread,
               arg,
               prio,
               &sys_stack[sys_thread_no - 1][0],
               (LWIP_STACK_SIZE / 10u),
               LWIP_STACK_SIZE,
               LWIP_Q_SIZE,
               0,
               DEF_NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
               &err);

  LWIP_ASSERT("Failed to create task.", RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  return sys_thread_no;
}

/**************************************************************************//**
 * "Fast" critical region protection
 *****************************************************************************/
sys_prot_t sys_arch_protect(void)
{
  RTOS_ERR err;
  OSMutexPend(&lwip_sys_mutex,
              0,
              OS_OPT_PEND_BLOCKING,
              NULL,
              &err);

  return (sys_prot_t)1;
}

/**************************************************************************//**
 * "Fast" critical region protection removal
 *****************************************************************************/
void sys_arch_unprotect(sys_prot_t pval)
{
  ( void ) pval;
  RTOS_ERR err;
  OSMutexPost(&lwip_sys_mutex,
              OS_OPT_POST_NONE,
              &err);
}

/**************************************************************************//**
 * Return system time in milliseconds
 *****************************************************************************/
u32_t sys_now(void)
{
  RTOS_ERR err;
  return OSTimeGet(&err);
}

#endif /* !NO_SYS */
