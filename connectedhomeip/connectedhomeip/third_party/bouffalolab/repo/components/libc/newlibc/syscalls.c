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
#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <reent.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <FreeRTOS.h>
#include "task.h"

#ifdef SYS_VFS_ENABLE
#include <vfs.h>
#endif

#ifdef SYS_TRACE_MEM_ENABLE
typedef struct
{
    void *ptr;
    size_t size;
    void *caller;  // actually the return address of malloc/free, used to trace where malloc/free is called
}malloc_entry_t;

typedef struct
{
    int table_full;
    uint32_t entry_num;
    uint32_t entry_num_max;
}malloc_table_info_t;

typedef struct 
{
    void* caller;
    uint32_t totalSize;
    uint32_t mallocTimes;
}mem_stats_t;

malloc_entry_t malloc_entry[SYS_TRACE_MEM_ENTRY_NUM];
malloc_table_info_t malloc_table_info;
#define SYS_TRACE_MEM_STATS_ENTRY_NUM 100
mem_stats_t mem_stats[SYS_TRACE_MEM_STATS_ENTRY_NUM];

bool set_monitor;
uint32_t callerAddr;
uint32_t mallocCnt = 0;
uint32_t freeCnt = 0;

void trace_malloc(void *ptr, size_t size, void *caller)
{
    int i;

    if(ptr == NULL){
        return;
    }

    if(!malloc_table_info.table_full){
        for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
            if(malloc_entry[i].ptr == NULL){
                malloc_entry[i].ptr = ptr;
                malloc_entry[i].size = size;
                malloc_entry[i].caller = caller;
                if(set_monitor && callerAddr == (uint32_t)caller)
                {
                    mallocCnt++;
                    printf("trace_malloc, caller=%08lx, ptr=%08lx, mallocCnt=%lu\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr, mallocCnt);
                }
                break;
            }
        }
        if(i == SYS_TRACE_MEM_ENTRY_NUM){
            malloc_table_info.table_full = 1;
        }
    }

    malloc_table_info.entry_num++;
    if(malloc_table_info.entry_num > malloc_table_info.entry_num_max){
        malloc_table_info.entry_num_max = malloc_table_info.entry_num;
    }
}

void trace_free(void *ptr, void *caller)
{
    int i;

    if(ptr == NULL){
        return;
    }

    if(!malloc_table_info.table_full){
        for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
            if(malloc_entry[i].ptr == ptr){
                if(set_monitor && callerAddr == (uint32_t)malloc_entry[i].caller)
                {
                    freeCnt++;
                    printf("trace_free, caller=%08lx, ptr=%08lx, freeCnt=%lu\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr, freeCnt);
                }
                malloc_entry[i].ptr = NULL;
                break;
            }
        }
    }

    malloc_table_info.entry_num--;
}

void trace_realloc(void *ptr_new, void *ptr_old, size_t size, void *caller)
{
    int i;

    if(ptr_old == NULL){
        trace_malloc(ptr_new, size, caller);
    }else if(ptr_new == NULL){
        trace_free(ptr_old, caller);
    }else{
        if(!malloc_table_info.table_full){
            for(i = 0; i < SYS_TRACE_MEM_ENTRY_NUM; i++){
                if(malloc_entry[i].ptr == ptr_old){
                    malloc_entry[i].ptr = ptr_new;
                    malloc_entry[i].size = size;
                    malloc_entry[i].caller = caller;
                    if(set_monitor && callerAddr == (uint32_t)malloc_entry[i].caller)
                    {
                        printf("trace_realloc, caller=%08lx, ptr=%08lx\r\n", callerAddr, (uint32_t)malloc_entry[i].ptr);
                    }
                    break;
                }
            }
            if(i == SYS_TRACE_MEM_ENTRY_NUM){
                trace_malloc(ptr_new, size, caller);
            }
        }
    }
}

void mem_trace_stats()
{
    for(int i=0;i<SYS_TRACE_MEM_STATS_ENTRY_NUM;i++)
    {
        mem_stats[i].caller = NULL;
        mem_stats[i].totalSize = 0;
        mem_stats[i].mallocTimes = 0;
    }
    
    for(int i=0; i<SYS_TRACE_MEM_ENTRY_NUM; i++)
    {
        if(malloc_entry[i].ptr == NULL)
        {
            continue;
        }

        uint8_t fExist = 0;
        uint16_t firstEmpty = 0xFFFF;
        for(int j=0; j<SYS_TRACE_MEM_STATS_ENTRY_NUM;j++)
        {
            if(malloc_entry[i].caller == mem_stats[j].caller)
            {
                fExist = 1;
                mem_stats[j].mallocTimes++;
                mem_stats[j].totalSize += malloc_entry[i].size;
                break;
            }

            if(firstEmpty==0xFFFF && mem_stats[j].caller == NULL)
            {
                firstEmpty = j;
            }
        }

        if(!fExist && firstEmpty != 0xFFFF)
        {
            mem_stats[firstEmpty].caller = malloc_entry[i].caller;
            mem_stats[firstEmpty].totalSize = malloc_entry[i].size;
            mem_stats[firstEmpty].mallocTimes = 1;
        }
    }
    
    for(int i=0;i<SYS_TRACE_MEM_STATS_ENTRY_NUM;i++)
    {
        if(mem_stats[i].caller)
        {
            printf("%d, caller:0x%08lx, totalSize:%lu, mallocTimes:%lu\r\n", i, (uint32_t)mem_stats[i].caller, mem_stats[i].totalSize, mem_stats[i].mallocTimes);
        }
    }
    printf("Current left size is %d bytes\r\n", xPortGetFreeHeapSize());
}

#endif

/* Reentrant versions of system calls.  */

/* global errno in RT-Thread */
static volatile int _sys_errno = 0;

#ifndef _REENT_ONLY
int *__errno ()
{
    #if( configUSE_POSIX_ERRNO == 1 )
	{
        extern int FreeRTOS_errno;

        return &FreeRTOS_errno;
	}
	#endif
    return &_REENT->_errno;;
}
#endif

struct _reent *__getreent(void)
{
  return _impure_ptr;
}

int _getpid_r(struct _reent *ptr)
{
    return 0;
}

int _execve_r(struct _reent *ptr, const char * name, char *const *argv, char *const *env)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    _off_t rc;

    rc = (_off_t)aos_lseek(fd, pos, whence);
    return rc;
#endif
}

int	mkdir (const char *_path, mode_t __mode )
{
#ifndef SYS_VFS_ENABLE
    return -1;
#else
    int rc;

    rc = aos_mkdir(_path);
    return rc;
#endif
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    int rc;

    rc = aos_open(file, flags);
    return rc;
#endif
}

int _close_r(struct _reent *ptr, int fd)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    return aos_close(fd);
#endif
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    _ssize_t rc;

    rc = (_ssize_t)aos_read(fd, buf, nbytes);
    return rc;
#endif
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    int rc;

    rc = aos_rename(old, new);
    return rc;
#endif
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    int rc;

    rc = aos_stat(file, pstat);
    return rc;
#endif
}

int _unlink_r(struct _reent *ptr, const char *file)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    return aos_unlink(file);
#endif
}

int _wait_r(struct _reent *ptr, int *status)
{
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    ptr->_errno = ENOSYS;
    return -1;
#else
    _ssize_t rc;

    rc = (_ssize_t)aos_write(fd, buf, nbytes);
    return rc;
#endif
}

int fsync(int fd)
{
#ifndef SYS_VFS_ENABLE
    /* return "not supported" */
    errno = ENOSYS;
    return -1;
#else
    int rc;

    rc = aos_sync(fd);
    return rc;
#endif
}

#if defined(CFG_USE_PSRAM)
#define IS_PSARAM(addr) ((addr&0xFF000000) == 0x26000000 || \
                        (addr&0xFF000000) == 0x24000000 )
#endif

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void* result;

    if (size == 0)
    {
        return NULL;
    }

#if defined(CFG_USE_PSRAM)
    if (xPortGetFreeHeapSizePsram() > size) {
        result = (void*)pvPortMallocPsram(size);
    }
    else {
        result = (void*)pvPortMalloc(size);
    }
#else
	result = (void*)pvPortMalloc(size);
#endif

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_malloc(result, size, (void *)__builtin_return_address(0));
#endif

    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void* result;

#if defined(CFG_USE_PSRAM)
    if (IS_PSARAM((uint32_t)old)) {
        result = (void*)pvPortReallocPsram(old, newlen);
    }
    else {
        result = (void*)pvPortRealloc(old, newlen);
    }
#else
	result = (void*)pvPortRealloc(old, newlen);
#endif

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_realloc(result, old, newlen, (void *)__builtin_return_address(0));
#endif

    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void* result;

    if (size == 0)
    {
        return NULL;
    }

#if defined(CFG_USE_PSRAM)
    if (xPortGetFreeHeapSizePsram()) {
        result = (void*)pvPortCallocPsram(size, len);
    }
    else {
        result = (void*)pvPortCalloc(size, len);
    }
#else
	result = (void*)pvPortCalloc(size, len);
#endif

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

#ifdef SYS_TRACE_MEM_ENABLE
    trace_malloc(result, size, (void *)__builtin_return_address(0));
#endif

    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
#if defined(CFG_USE_PSRAM)	
    if (IS_PSARAM((uint32_t)addr)) {
        vPortFreePsram(addr);
    }
    else {
        vPortFree(addr);
    }
#else
	vPortFree(addr);
#endif

#ifdef SYS_TRACE_MEM_ENABLE
    trace_free(addr, (void *)__builtin_return_address(0));
#endif
}

void* _valloc_r(struct _reent *ptr, size_t size) __attribute__((alias("_malloc_r")));

void* _pvalloc_r(struct _reent *ptr, size_t size) __attribute__((alias("_malloc_r")));

void _cfree_r(struct _reent *ptr, void *addr) __attribute__((alias("_free_r")));

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    void *ret;
    ptr->_errno = ENOMEM;
    ret = (void *)-1;
    return ret;
}

/* for exit() and abort() */
void __attribute__ ((noreturn))
_exit (int status)
{
    configASSERT(0);
    while(1);
}

void _system(const char *s)
{
}

void __libc_init_array(void)
{
    /* we not use __libc init_aray to initialize C++ objects */
}

mode_t umask(mode_t mask)
{
    return 022;
}

int flock(int fd, int operation)
{
    return 0;
}

/*
These functions are implemented and replaced by the 'common/time.c' file
int _gettimeofday_r(struct _reent *ptr, struct timeval *__tp, void *__tzp);
_CLOCK_T_  _times_r(struct _reent *ptr, struct tms *ptms);
*/
void newlibc_init(void)
{
    /*dummy functions*/
}
