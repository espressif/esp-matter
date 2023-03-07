/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

int _close(int file)
{
    return -1;
}

int _stat(char *file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _open(const char *name, int flags, int mode)
{
    return -1;
}

int _read(int file, char *ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = __io_getchar();
    }

    return len;
}

extern int log_write(char *buf, int len);

int _write(int file, char *ptr, int len)
{
    return log_write(ptr, len);
}


void* _sbrk_r(struct _reent *r, ptrdiff_t nbytes)
{
    extern char end;       /* Defined by linker */
    static char *heap_end; /* Previous end of heap or 0 if none */
    char        *prev_heap_end;
    char        *stack;

    __asm volatile("MRS %0, msp\n" : "=r"(stack));

    if (0 == heap_end) {
        heap_end = &end; /* Initialize first time round */
    }

    prev_heap_end = heap_end;

    if (stack < (prev_heap_end + nbytes)) {
     /* heap would overlap the current stack depth.
        * Future:  use sbrk() system call to make simulator grow memory beyond
        * the stack and allocate that
        */
        //errno = ENOMEM;
        return (char *) - 1;
    }
    heap_end += nbytes;

    return (void *) prev_heap_end;
}


int _execve(char *name, char **argv, char **env)
{
    errno = ENOTSUP;
    return -1;
}

int _fork(void)
{
    errno = ENOTSUP;
    return -1;
}

int _getpid(void)
{
    errno = ENOTSUP;
    return -1;
}

int _kill(int pid, int sig)
{
    errno = ENOTSUP;
    return -1;
}

int _wait(int *status)
{
    errno = ENOTSUP;
    return -1;
}

void _exit(int __status)
{
	errno = ENOTSUP;
	printf("_exit\n");
	while (1) {;}
}

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int _unlink(char *name)
{
    errno = EMLINK;
    return -1;
}


#include "FreeRTOS.h"
#include "task.h"
#include <sys/time.h>
#include <sys/times.h>

int _gettimeofday(struct timeval *tv, void *ptz)
{
    int ticks = xTaskGetTickCount();
    if (tv != NULL) {
        tv->tv_sec = (ticks / 1000);
        tv->tv_usec = (ticks % 1000) * 1000;
        return 0;
    }

    return -1;
}

int _times(struct tms *buf)
{
    errno = ENOTSUP;
    return -1;
}

